#include "LensFlare.h"
#include "Game_Persistent.h"
#include "FVF.h"
#include "D3D9Types.h"
#include "Backend.h"
#include "texture.h"
#include "game_level.h"
#include "kinematics.h"
#include "object.h"
#include "bone.h"
#include "RenderDevice.h"
#include "xr_render_console.h"

void CLensFlareDescriptor::load(CInifile* pIni, LPCSTR sect)
{
	//g_pStringContainer->dump();
	section		= sect;
	//g_pStringContainer->dump();
	const char*ccc = section.c_str();
	Msg("CLensFlareDescriptor::load: sect = %s", ccc);
	m_Flags.set	(flSource, pIni->r_bool(sect,"source" ));
	if (m_Flags.is(flSource)){
		LPCSTR S= pIni->r_string 	( sect,"source_shader" );
		LPCSTR T= pIni->r_string 	( sect,"source_texture" );
		float r = pIni->r_float		( sect,"source_radius" );
		BOOL i 	= pIni->r_bool		( sect,"source_ignore_color" );
		SetSource(r,i,T,S);
	}
	m_Flags.set	(flFlare,pIni->r_bool ( sect,"flares" ));
	if (m_Flags.is(flFlare)){
	    LPCSTR S= pIni->r_string 	( sect,"flare_shader" );
		LPCSTR T= pIni->r_string 	( sect,"flare_textures" );
		LPCSTR R= pIni->r_string 	( sect,"flare_radius" );
		LPCSTR O= pIni->r_string 	( sect,"flare_opacity");
		LPCSTR P= pIni->r_string 	( sect,"flare_position");
		u32 tcnt= _GetItemCount(T);
        char name[256];
		for (u32 i=0; i<tcnt; i++){
			_GetItem(R,i,name); float r=(float)atof(name);
			_GetItem(O,i,name); float o=(float)atof(name);
			_GetItem(P,i,name); float p=(float)atof(name);
			_GetItem(T,i,name);
			AddFlare(r,o,p,name,S);
		}
	}
	m_Flags.set	(flGradient,CInifile::IsBOOL(pIni->r_string( sect, "gradient")));
	if (m_Flags.is(flGradient)){
		LPCSTR S= pIni->r_string 	( sect,"gradient_shader" );
		LPCSTR T= pIni->r_string	( sect,"gradient_texture" );
		float r	= pIni->r_float		( sect,"gradient_radius"  );
		float o = pIni->r_float		( sect,"gradient_opacity" );
		SetGradient(r,o,T,S);
	}
    m_StateBlendUpSpeed	= 1.f/(_max(pIni->r_float( sect,"blend_rise_time" ),0.f)+EPS_S);
    m_StateBlendDnSpeed	= 1.f/(_max(pIni->r_float( sect,"blend_down_time" ),0.f)+EPS_S);

	OnDeviceCreate();
}

const int MAX_Flares = 24;
#define BLEND_INC_SPEED 8.0f
#define BLEND_DEC_SPEED 4.0f

#define FAR_DIST (g_pGamePersistent->Environment().CurrentEnv.far_plane)

typedef svector<ref_shader,MAX_Flares> shaders_vector;

void __fastcall CLensFlare::Render(BOOL bSun, BOOL bFlares, BOOL bGradient)
{
//	Msg("CLensFlare::Render - bSun = %d, bFlares = %d, bGradient = %d, bRender = %d, m_Current = %s", bSun, bFlares, bGradient, bRender, m_Current?m_Current->section.c_str():"none");
	if (!bRender)		return;
	if(!m_Current)		return;
	//VERIFY				(m_Current);
	//Log("2");

	Fcolor				dwLight;
	Fcolor				color;
	Fvector				vec, vecSx, vecSy;
	Fvector				vecDx, vecDy;

	dwLight.set							( LightColor );
	shaders_vector		_2render;

	u32									VS_Offset;
	FVF::LIT *pv						= (FVF::LIT*) RCache.Vertex.Lock(MAX_Flares*4, hGeom.stride(), VS_Offset);
	//Log("3");
	float 	fDistance		= (g_pGamePersistent->Environment().CurrentEnv.far_plane)*0.75f;
	//Msg("fDistance: %f", fDistance);
	if (bSun)
	{
		//Log("4");
		if (m_Current->m_Flags.is(CLensFlareDescriptor::flSource))
		{
			//Log("4_1");
			vecSx.mul			(vecX, m_Current->m_Source.fRadius*fDistance);
			vecSy.mul			(vecY, m_Current->m_Source.fRadius*fDistance);
			if (m_Current->m_Source.ignore_color) 	color.set(1.f,1.f,1.f,1.f);
			else									color.set(dwLight);
			color.a				*= m_StateBlend;
			u32 c				= color.get();
			pv->set				(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
			pv->set				(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
			pv->set				(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
			pv->set				(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
			_2render.push_back	(m_Current->m_Source.hShader);
		}
		//Log("4 1");
	}
	//Msg("fBlend = %f", fBlend);
	//fGradientValue = 1;
	//m_StateBlend = 1;

	if (fBlend>=EPS_L)
	{
		//Msg("bFlares = %d", bFlares);
		if (bFlares)
		{
			//Msg("m_Current->m_Flags: %x", m_Current->m_Flags.flags);
			vecDx.normalize		(vecAxis);
			vecDy.crossproduct	(vecDx, vecDir);
			if (m_Current->m_Flags.is(CLensFlareDescriptor::flFlare))
			{
				//for (CLensFlareDescriptor::FlareIt it=m_Current->m_Flares.begin(); it!=m_Current->m_Flares.end(); it++)
				//Msg("m_Current->m_Flares.size() = %d", m_Current->m_Flares.size());
				for (unsigned int it = 0; it < m_Current->m_Flares.size(); it++)
				{
					//CLensFlareDescriptor::SFlare&	F = *it;
					CLensFlareDescriptor::SFlare&	F = m_Current->m_Flares[it];
					vec.mul				(vecAxis, F.fPosition);
					vec.add				(vecCenter);
					vecSx.mul			(vecDx, F.fRadius*fDistance);
					vecSy.mul			(vecDy, F.fRadius*fDistance);
					float    cl			= F.fOpacity * fBlend * m_StateBlend;
					color.set			( dwLight );
					//
					//color.set			(1,1,1,0.5);
					//
					color.mul_rgba		( cl );
					u32 c				= color.get();
					pv->set				(vec.x+vecSx.x-vecSy.x, vec.y+vecSx.y-vecSy.y, vec.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
					pv->set				(vec.x+vecSx.x+vecSy.x, vec.y+vecSx.y+vecSy.y, vec.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
					pv->set				(vec.x-vecSx.x-vecSy.x, vec.y-vecSx.y-vecSy.y, vec.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
					pv->set				(vec.x-vecSx.x+vecSy.x, vec.y-vecSx.y+vecSy.y, vec.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
					//_2render.push_back	(it->hShader);
					_2render.push_back(F.hShader);
				}
			}
		}
		// gradient
		if (bGradient&&(fGradientValue>=EPS_L))
		{
			//Log("6");
			if (m_Current->m_Flags.is(CLensFlareDescriptor::flGradient)){
				vecSx.mul			(vecX, m_Current->m_Gradient.fRadius*fGradientValue*fDistance);
				vecSy.mul			(vecY, m_Current->m_Gradient.fRadius*fGradientValue*fDistance);

				color.set			( dwLight );
				color.mul_rgba		( fGradientValue*m_StateBlend );
				//
				//color.set			(1,1,1,0.5);
				//
				u32 c				= color.get	();
				pv->set				(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
				pv->set				(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
				pv->set				(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
				pv->set				(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 1, 1); pv++;
				_2render.push_back	(m_Current->m_Gradient.hShader);
			}
			//Log("6 1");
		}
	}
	//Log("7");
	RCache.Vertex.Unlock	(_2render.size()*4,hGeom.stride());
	//Log("8");
	RCache.set_xform_world	(Fidentity);
	//Log("9");
	RCache.set_Geometry		(hGeom);
	//Log("10");
	//Msg("_2render.size() = %d", _2render.size());
	for (u32 i=0; i<_2render.size(); i++)
	{
		//Msg("i=%d", i);
		if (_2render[i])
		{
			u32						vBase	= i*4+VS_Offset;
			//Log("1_");
			RCache.set_Shader		(_2render[i], 0);
			if (bSun && m_Current->m_Flags.is(CLensFlareDescriptor::flSource) && (0==i))
				RCache.set_c			("flare_params", 0.0, 0.0, 0.0, ps_r2_pp_flags.test(R2PP_FLAG_LENS_DIRT_CONTROL)?1.f:0.f);
			//Log("2_");
			RCache.Render			(D3DPT_TRIANGLELIST,vBase, 0,4,0,2);
			//Log("3_");
		}
	}
	/*
	Log("--");
	for (int i = 0; i < _2render.size(); i++)
	{
	Msg("i = %d, ref = %d", i, _2render[i]->dwReference);
	}
	*/
}

class CObject;

BOOL material_callback(collide::rq_result& result, LPVOID params)
{
//	Msg("material_callback");
	STranspParam* fp= (STranspParam*)params;
	float vis		= 1.f;
	if (result.O)
	{
		vis			= 0.f;
		//CKinematics*K=PKinematics(result.O->renderable.visual);
		CKinematics *K = result.O->renderable.visual->vfptr->dcast_PKinematics(result.O->renderable.visual);
		if (K && (result.element>0))
		{
			CBoneData &bone_data = K->LL_GetData(u16(result.element));
			u16 mtl_idx = bone_data.get_game_mtl_idx();
			vis = g_pGamePersistent->MtlTransparent(mtl_idx);
		}
//		Msg("result.O != 0: vis = %7.4f", vis);
	}
	else
	{
		CDB::TRI* T	= g_pGameLevel->ObjectSpace.GetStaticTris() + result.element;
		vis			= g_pGamePersistent->MtlTransparent(T->material);
//		Msg("result.O == 0: vis=%7.4f", vis);
		if (fis_zero(vis)){
			Fvector* V	= g_pGameLevel->ObjectSpace.GetStaticVerts();
			collide::ray_cache &rc = fp->parent->m_ray_cache;
			rc.set(fp->P, fp->D, fp->f, TRUE);
			rc.verts[0].set(V[T->verts[0]]);
			rc.verts[1].set(V[T->verts[1]]);
			rc.verts[2].set(V[T->verts[2]]);
		}
	}
	fp->vis *= vis;
	return (fp->vis > fp->vis_threshold);
}

inline void	blend_lerp(float& cur, float tgt, float speed, float dt)
{
	float diff		= tgt - cur;
	float diff_a	= abs(diff);
	if (diff_a < EPS_S)	return;
	float mot		= speed*dt;
	if (mot>diff_a) mot=diff_a;
	cur				+= (diff/diff_a)*mot;
}

void CLensFlare::OnFrame(int id)
{
//	Msg("RECT: %d,  CTimer_paused: %d CTimer: %d", sizeof(RECT), sizeof(CTimer_paused), sizeof(CTimer));

//	Msg("CLensFlare::OnFrame: id = %d", id);
	if (dwFrame==Device.dwFrame)return;
	if (!g_pGameLevel)			return;

	dwFrame			= Device.dwFrame;

	//R_ASSERT		( _valid(g_pGamePersistent->Environment().CurrentEnv.sun_dir) );
	vSunDir.mul		(g_pGamePersistent->Environment().CurrentEnv.sun_dir,-1);
	//R_ASSERT		( _valid(vSunDir) );

	// color
	float tf		= g_pGamePersistent->Environment().fTimeFactor;
	Fvector& c		= g_pGamePersistent->Environment().CurrentEnv.sun_color;
	LightColor.set	(c.x,c.y,c.z,1.f); 

	CLensFlareDescriptor* desc = (id==-1)?0:&m_Palette[id];
//	Msg("m_State: %d", m_State);
	switch(m_State)
	{
	case lfsNone: 
		m_State=lfsShow; 
		m_Current=desc; 
		break;
	case lfsIdle: 
		if (desc!=m_Current) 
			m_State=lfsHide; 	
		break;
	case lfsShow: 
		m_StateBlend 	= m_Current?(m_StateBlend + m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf):1.f+EPS;
		if (m_StateBlend>=1.f) m_State=lfsIdle;
		break;
	case lfsHide: 
		m_StateBlend 	= m_Current?(m_StateBlend - m_Current->m_StateBlendDnSpeed * Device.fTimeDelta * tf):0.f-EPS;
		if (m_StateBlend<=0.f){ 	
			m_State		= lfsShow;
			m_Current	= desc;
			m_StateBlend= m_Current?m_Current->m_StateBlendUpSpeed * Device.fTimeDelta * tf:0;
		}
		break;
	}
	//	Msg				("%6d : [%s] -> [%s]", Device.dwFrame, state_to_string(previous_state), state_to_string(m_State));
	clamp(m_StateBlend,0.f,1.f);

	if ((m_Current==0)||(LightColor.magnitude_rgb()==0.f)){bRender=false; return;}

	//
	// Compute center and axis of flares
	//
	float fDot;

	Fvector vecPos;

	Fmatrix	matEffCamPos;
	matEffCamPos.identity();
	// Calculate our position and direction

	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);
	vecPos.set(Device.vCameraPosition);

	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize();

	// Figure out of light (or flare) might be visible
	vecLight.set(vSunDir);
	vecLight.normalize();

	fDot = vecLight.dotproduct(vecDir);

	if(fDot <= 0.01f){	bRender = false; return;} else bRender = true;

	// Calculate the point directly in front of us, on the far clip plane
	float 	fDistance	= FAR_DIST*0.75f;
	vecCenter.mul(vecDir, fDistance);
	vecCenter.add(vecPos);
	// Calculate position of light on the far clip plane
	vecLight.mul(fDistance / fDot);
	vecLight.add(vecPos);
	// Compute axis which goes from light through the center of the screen
	vecAxis.sub(vecLight, vecCenter);

	//
	// Figure out if light is behind something else
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize();
	//R_ASSERT( _valid(vecX) );

	vecY.crossproduct(vecX, vecDir);
	//R_ASSERT( _valid(vecY) );

	//	Side vectors to bend normal.
	Fvector vecSx;
	Fvector vecSy;

	//float fScale = m_Current->m_Source.fRadius * vSunDir.magnitude();
	//float fScale = m_Current->m_Source.fRadius;
	//	HACK: it must be read from the weather!
	float fScale = 0.02f;

	vecSx.mul(vecX, fScale);
	vecSy.mul(vecY, fScale);

	//--------------------------------------------------------------------------------------------------------
	// что было здесь: используется pray_cache, в оригинале такого нет
	/*
	CObject*	o_main		= g_pGameLevel->CurrentViewEntity();
	//R_ASSERT				( _valid(vSunDir) );
	STranspParam TP			(&m_ray_cache[0],Device.vCameraPosition,vSunDir,1000.f,EPS_L);

	//R_ASSERT				( _valid(TP.P) );
	//R_ASSERT				( _valid(TP.D) );
	collide::ray_defs RD	(TP.P,TP.D,TP.f,CDB::OPT_CULL,collide::rqtBoth);
	float	fVisResult = 0.0f;

	for ( int i=0; i<MAX_RAYS; ++i)
	{
	TP.D = vSunDir;
	TP.D.add(Fvector().mul( vecSx, RayDeltas[i].x));
	TP.D.add(Fvector().mul( vecSy, RayDeltas[i].y));
	//R_ASSERT		( _valid(TP.D) );
	TP.pray_cache	= &(m_ray_cache[i]);
	TP.vis			= 1.0f;
	RD.dir			= TP.D;

	if (m_ray_cache[i].result&&m_ray_cache[i].similar(TP.P,TP.D,TP.f)){
	// similar with previous query == 0
	TP.vis				= 0.f;
	}else{
	float _u,_v,_range;
	if (CDB::TestRayTri(TP.P,TP.D,m_ray_cache[i].verts,_u,_v,_range,false)&&(_range>0 && _range<TP.f)){
	TP.vis			= 0.f;
	}else{
	// cache outdated. real query.
	r_dest.r_clear	();
	if (g_pGameLevel->ObjectSpace.RayQuery	(r_dest,RD,material_callback,&TP,NULL,o_main))
	m_ray_cache[i].result = FALSE			;
	}
	}

	fVisResult += TP.vis;
	}

	fVisResult *= (1.0f/MAX_RAYS);

	//blend_lerp(fBlend,TP.vis,BLEND_DEC_SPEED,Device.fTimeDelta);
	blend_lerp(fBlend,fVisResult,BLEND_DEC_SPEED,Device.fTimeDelta);
	*/
	
	/* не годится, так как использует m_ray_cache
	CObject*	o_main		= g_pGameLevel->CurrentViewEntity();
	STranspParam TP			(&m_ray_cache,Device.vCameraPosition,vSunDir,1000.f,EPS_L);
	collide::ray_defs RD	(TP.P,TP.D,TP.f,CDB::OPT_CULL,collide::rqtBoth);
	if (m_ray_cache.result&&m_ray_cache.similar(TP.P,TP.D,TP.f)){
	// similar with previous query == 0
	TP.vis				= 0.f;
	}else{
	float _u,_v,_range;
	if (CDB::TestRayTri(TP.P,TP.D,m_ray_cache.verts,_u,_v,_range,false)&&(_range>0 && _range<TP.f)){
	TP.vis			= 0.f;
	}else{
	// cache outdated. real query.
	r_dest.r_clear	();
	if (g_pGameLevel->ObjectSpace.RayQuery	(r_dest,RD,material_callback,&TP,NULL,o_main))
	m_ray_cache.result = FALSE			;
	}
	}

	blend_lerp(fBlend,TP.vis,BLEND_DEC_SPEED,Device.fTimeDelta);
	*/
	
	CObject*	o_main		= g_pGameLevel->CurrentViewEntity();
	STranspParam TP			(this,Device.vCameraPosition,vSunDir,1000.f,EPS_L);
	collide::ray_defs RD	(TP.P,TP.D,TP.f,CDB::OPT_CULL,collide::rqtBoth);
	if (m_ray_cache.result && m_ray_cache.similar(TP.P,TP.D,TP.f)){
		// similar with previous query == 0
		TP.vis				= 0.f;
	}else{
		float _u,_v,_range;
		if (CDB::TestRayTri(TP.P,TP.D,m_ray_cache.verts,_u,_v,_range,false)&&(_range>0 && _range<TP.f)){
			TP.vis			= 0.f;
		}else{
			// cache outdated. real query.
			r_dest.r_clear	();
			if (g_pGameLevel->ObjectSpace.RayQuery(r_dest,RD,material_callback,&TP,NULL,o_main))
				m_ray_cache.result = FALSE;
		}
	}
	blend_lerp(fBlend,TP.vis,BLEND_DEC_SPEED,Device.fTimeDelta);
	

	clamp( fBlend, 0.0f, 1.0f );

	// gradient
	if (m_Current->m_Flags.is(CLensFlareDescriptor::flGradient))
	{
		Fvector				scr_pos;
		Device.mFullTransform.transform	( scr_pos, vecLight );
		float kx = 1, ky = 1;
		float sun_blend		= 0.5f;
		float sun_max		= 2.5f;
		scr_pos.y			*= -1;

		if (abs(scr_pos.x) > sun_blend)	kx = ((sun_max - (float)abs(scr_pos.x))) / (sun_max - sun_blend);
		if (abs(scr_pos.y) > sun_blend)	ky = ((sun_max - (float)abs(scr_pos.y))) / (sun_max - sun_blend);

		if (!((abs(scr_pos.x) > sun_max) || (abs(scr_pos.y) > sun_max))){
			float op		= m_StateBlend*m_Current->m_Gradient.fOpacity;
			fGradientValue	= kx * ky *  op * fBlend;
		}else
			fGradientValue	= 0;
	}
//	Msg("fGradientValue: %7.4f, fBlend: %7.4f, m_StateBlend: %7.4f", fGradientValue, fBlend, m_StateBlend);
}
