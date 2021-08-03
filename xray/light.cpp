#include "light.h"
#include "xr_3da.h"
#include "Game_Persistent.h"
#include "RenderDevice.h"
#include "game_level.h"
#include "_d3d_extensions.h"
#include "xr_render_console.h"
#include "r2_rendertarget.h"
#include "r2.h"

light*			CLight_DB::Create	()
{
	light*	L			= xr_new<light>	();
	L->flags.bStatic	= false;
	L->flags.bActive	= false;
	L->flags.bShadow	= true;
	return				L;
}

void CLight_DB::Load			(IReader *fs) 
{
	IReader* F	= 0;

	// Lights itself
	sun_original		= NULL;
	sun_adapted			= NULL;
	{
		F				= fs->open_chunk		(fsL_LIGHT_DYNAMIC);

		u32 size		= F->length();
		u32 element		= sizeof(Flight)+4;
		u32 count		= size/element;
		VERIFY			(count*element == size);
		v_static.reserve(count);
		for (u32 i=0; i<count; i++) 
		{
			Flight		Ldata;
			light*		L				= Create	();
			L->flags.bStatic			= true;
			L->set_type					(IRender_Light::POINT);

			L->set_shadow				(true);
			u32 controller				= 0;
			F->r						(&controller,4);
			F->r						(&Ldata,sizeof(Flight));
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)	{
				Fvector tmp_R;		tmp_R.set(1,0,0);

				// directional (base)
				sun_original		= L;
				L->set_type			(IRender_Light::DIRECT);
				L->set_shadow		(true);
				L->set_rotation		(Ldata.direction,tmp_R);
				
				// copy to env-sun
				sun_adapted			=	L		= Create();
				L->flags.bStatic	=	true;
				L->set_type			(IRender_Light::DIRECT);
				L->set_shadow		(true);
				L->set_rotation		(Ldata.direction,tmp_R);
			}
			else
			{
				Fvector tmp_D,tmp_R;
				tmp_D.set			(0,0,-1);	// forward
				tmp_R.set			(1,0,0);	// right

				// point
				v_static.push_back	(L);
				L->set_position		(Ldata.position		);
				L->set_rotation		(tmp_D, tmp_R		);
				L->set_range		(Ldata.range		);
				L->set_color		(Ldata.diffuse		);
				L->set_active		(true				);
//				R_ASSERT			(L->spatial.sector	);
			}
		}

		F->close			();
	}
	R_ASSERT2(sun_original && sun_adapted,"Where is sun?");

	// fake spot
	/*
	if (0)
	{
		Fvector	P;			P.set(-5.58f,	-0.00f + 2, -3.63f);
		Fvector	D;			D.set(0,-1,0);
		light*	fake		= Create();
		fake->set_type		(IRender_Light::SPOT);
		fake->set_color		(1,1,1);
		fake->set_cone		(deg2rad(60.f));
		fake->set_direction	(D);
		fake->set_position	(P);
		fake->set_range		(3.f);
		fake->set_active	(true);
	}
	*/
}
void			CLight_DB::Update			()
{
	// set sun params
	if (sun_original && sun_adapted)
	{
		light*	_sun_original		= (light*) sun_original._get();
		light*	_sun_adapted		= (light*) sun_adapted._get();
		CEnvDescriptor&	E			= g_pGamePersistent->Environment().CurrentEnv;
		VERIFY						(_valid(E.sun_dir));
#ifdef DEBUG
		if(E.sun_dir.y>=0)
		{
			Log("sect_name", E.sect_name.c_str());
			Log("E.sun_dir", E.sun_dir);
			Log("E.wind_direction",E.wind_direction);
			Log("E.wind_velocity",E.wind_velocity);
			Log("E.sun_color",E.sun_color);
			Log("E.rain_color",E.rain_color);
			Log("E.rain_density",E.rain_density);
			Log("E.fog_distance",E.fog_distance);
			Log("E.fog_density",E.fog_density);
			Log("E.fog_color",E.fog_color);
			Log("E.far_plane",E.far_plane);
			Log("E.sky_rotation",E.sky_rotation);
			Log("E.sky_color",E.sky_color);
		}
#endif
		VERIFY2						(E.sun_dir.y<0,"Invalid sun direction settings in evironment-config");
		Fvector						OD,OP,AD,AP;
		OD.set						(E.sun_dir).normalize			();
		OP.mad						(Device.vCameraPosition,OD,-500.f);
		AD.set(0,-.75f,0).add		(E.sun_dir);

		// for some reason E.sun_dir can point-up
		int		counter = 0;
		while	(AD.magnitude()<0.001 && counter<10)	{
			AD.add(E.sun_dir); counter++;
		}
		AD.normalize				();
		AP.mad						(Device.vCameraPosition,AD,-500.f);
		sun_original->set_rotation	(OD,_sun_original->right	);
		sun_original->set_position	(OP);
		sun_original->set_color		(E.sun_color.x,E.sun_color.y,E.sun_color.z);
		sun_original->set_range		(600.f);
		sun_adapted->set_rotation	(AD, _sun_adapted->right	);
		sun_adapted->set_position	(AP		);
		sun_adapted->set_color		(E.sun_color.x*ps_r2_sun_lumscale,E.sun_color.y*ps_r2_sun_lumscale,E.sun_color.z*ps_r2_sun_lumscale);
		sun_adapted->set_range		(600.f	);

		//test: take from CoP for real shadows from sun
		if (ps_r2_ls_flags.test(R2FLAG_TRUE_SHADOWS))
		{
			sun_adapted->set_rotation (OD,_sun_original->right	);
			sun_adapted->set_position (OP);
		}
	}

	// Clear selection
	package.clear	();
}
void	light_Package::clear	()
{
	v_point.clear		();
	v_spot.clear		();
	v_shadowed.clear	();
}
void			CLight_DB::Unload	()
{
	v_static.clear			();
	sun_original.destroy	();
	sun_adapted.destroy		();
}
light::light		(void)	: ISpatial(g_SpatialSpace)
{
	spatial.type	= STYPE_LIGHTSOURCE;
	flags.type		= POINT;
	flags.bStatic	= false;
	flags.bActive	= false;
	flags.bShadow	= false;
	position.set	(0,-1000,0);
	direction.set	(0,-1,0);
	right.set		(0,0,0);
	range			= 8.f;
	cone			= deg2rad(60.f);
	color.set		(1,1,1,1);

	frame_render	= 0;

	ZeroMemory		(omnipart,sizeof(omnipart));
	s_spot			= NULL;
	s_point			= NULL;
	vis.frame2test	= 0;	// xffffffff;
	vis.query_id	= 0;
	vis.query_order	= 0;
	vis.visible		= true;
	vis.pending		= false;

	smap_near = 0.1;
	smap_near_neg = -0.1;

	lsf_speed		= 0;
	lsf_amount		= 0;
	lsf_smap_jitter	= 0;

	is_require_flare = false;
	is_volumetric = false;
	is_actor_torch = false;
	fBlend = 0;
}

light::~light	()
{
	for (int f=0; f<6; f++)	xr_delete(omnipart[f]);
	set_active		(false);

	// remove from Lights_LastFrame
	for (u32 it=0; it<RImplementation.Lights_LastFrame.size(); it++)
		if (this==RImplementation.Lights_LastFrame[it])	RImplementation.Lights_LastFrame[it]=0;
}

void light::set_texture		(LPCSTR name)
{
	if ((0==name) || (0==name[0]))
	{
		// default shaders
		s_spot.destroy		();
		s_point.destroy		();
		return;
	}

#pragma todo				("Only shadowed spot implements projective texture")
	string256				temp;
	s_spot.create			(RImplementation.Target->b_accum_spot,strconcat(sizeof(temp),temp,"r2\\accum_spot_",name),name);
	s_spot.create			(RImplementation.Target->b_accum_spot,strconcat(sizeof(temp),temp,"r2\\accum_spot_",name),name);
}

void light::set_active		(bool a)
{
	if (a)
	{
		if (flags.bActive)					return;
		flags.bActive						= true;
		spatial_register					();
		spatial_move						();
		//Msg								("!!! L-register: %X",u32(this));

#ifdef DEBUG
		Fvector	zero = {0,-1000,0}			;
		if (position.similar(zero))			{
			Msg	("- Uninitialized light position.");
		}
#endif // DEBUG
	}
	else
	{
		if (!flags.bActive)					return;
		flags.bActive						= false;
		spatial_move						();
		spatial_unregister					();
		//Msg								("!!! L-unregister: %X",u32(this));
	}
}

void	light::set_position		(const Fvector& P)
{
	float	eps					=	EPS_L;	//_max	(range*0.001f,EPS_L);
	if (position.similar(P,eps))return	;
	position.set				(P);
	spatial_move				();
}

void	light::set_range		(float R)			{
	float	eps					=	_max	(range*0.1f,EPS_L);
	if (fsimilar(range,R,eps))	return	;
	range						= R		;
	spatial_move				();
};

void	light::set_cone			(float angle)		{
	if (fsimilar(cone,angle))	return	;
	VERIFY						(cone < deg2rad(121.f));	// 120 is hard limit for lights
	cone						= angle;
	spatial_move				();
}
void	light::set_rotation		(const Fvector& D, const Fvector& R)	{ 
	Fvector	old_D		= direction;
	direction.normalize	(D);
	right.normalize(R);
	if (!fsimilar(1.f, old_D.dotproduct(D)))	spatial_move	();
}

void	light::spatial_move			()
{
	switch(flags.type)	{
	case IRender_Light::REFLECTED	:	
	case IRender_Light::POINT		:	
		{
			spatial.sphere.set		(position, range);
		} 
		break;
	case IRender_Light::SPOT		:	
		{
			// minimal enclosing sphere around cone
			VERIFY2						(cone < deg2rad(121.f), "Too large light-cone angle. Maybe you have passed it in 'degrees'?");
			if (cone>=PI_DIV_2)			{
				// obtused-angled
				spatial.sphere.P.mad	(position,direction,range);
				spatial.sphere.R		= range * tanf(cone/2.f);
			} else {
				// acute-angled
				spatial.sphere.R		= range / (2.f * _sqr(_cos(cone/2.f)));
				spatial.sphere.P.mad	(position,direction,spatial.sphere.R);
			}
		}
		break;
	case IRender_Light::OMNIPART	:
		{
			// is it optimal? seems to be...
			spatial.sphere.P.mad		(position,direction,range);
			spatial.sphere.R			= range;
		}
		break;
	}

	// update spatial DB
	ISpatial::spatial_move			();

	if (flags.bActive) gi_generate	();
	svis.invalidate					();
}
/*
vis_data&	light::get_homdata		()
{
	// commit vis-data
	hom.sphere.set	(spatial.sphere.P,spatial.sphere.R);
	hom.box.set		(spatial.sphere.P,spatial.sphere.P);
	hom.box.grow	(spatial.sphere.R);
	return			hom;
};
*/
Fvector	light::spatial_sector_point	()	
{ 
	return position; 
}
extern __declspec(dllimport) float		r_ssaGLOD_start,	r_ssaGLOD_end;
float	light::get_LOD					()
{
	if	(!flags.bShadow)	return 1;
	float	distSQ			= Device.vCameraPosition.distance_to_sqr(spatial.sphere.P)+EPS;
	float	ssa				= ps_r2_slight_fade * spatial.sphere.R/distSQ;
	float	lod				= _sqrt(clampr((ssa - r_ssaGLOD_end)/(r_ssaGLOD_start-r_ssaGLOD_end),0.f,1.f));
	return	lod	;
}
//////////////////////////////////////////////////////////////////////////
// Xforms

void	light::xform_calc			()
{
	if	(Device.dwFrame == m_xform_frame)	return;
	m_xform_frame	= Device.dwFrame;

	// build final rotation / translation
	Fvector					L_dir,L_up,L_right;

	// dir
	L_dir.set				(direction);
	float l_dir_m			= L_dir.magnitude();
	if (_valid(l_dir_m) && l_dir_m>EPS_S)	L_dir.div(l_dir_m);
	else									L_dir.set(0,0,1);

	// R&N
	if (right.square_magnitude()>EPS)				{
		// use specified 'up' and 'right', just enshure ortho-normalization
		L_right.set					(right);				L_right.normalize	();
		L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
		L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
	} else {
		// auto find 'up' and 'right' vectors
		L_up.set					(0,1,0);				if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct		(L_up,L_dir);			L_right.normalize	();
		L_up.crossproduct			(L_dir,L_right);		L_up.normalize		();
	}

	// matrix
	Fmatrix					mR;
	mR.i					= L_right;	mR._14	= 0;
	mR.j					= L_up;		mR._24	= 0;
	mR.k					= L_dir;	mR._34	= 0;
	mR.c					= position;	mR._44	= 1;

	// switch
	switch(flags.type)	{
	case IRender_Light::REFLECTED	:
	case IRender_Light::POINT		:
		{
			// scale of identity sphere
			float		L_R			= range;
			Fmatrix		mScale;		mScale.scale	(L_R,L_R,L_R);
			m_xform.mul_43			(mR,mScale);
		}
		break;
	case IRender_Light::SPOT		:
		{
			// scale to account range and angle
			float		s			= 2.f*range*tanf(cone/2.f);	
			Fmatrix		mScale;		mScale.scale(s,s,range);	// make range and radius
			m_xform.mul_43			(mR,mScale);
		}
		break;
	case IRender_Light::OMNIPART	:
		{
			float		L_R			= 2*range;		// volume is half-radius
			Fmatrix		mScale;		mScale.scale	(L_R,L_R,L_R);
			m_xform.mul_43			(mR,mScale);
		}
		break;
	default:
		m_xform.identity	();
		break;
	}
}

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
static	Fvector cmNorm[6]	= {{0.f,1.f,0.f}, {0.f,1.f,0.f}, {0.f,0.f,-1.f},{0.f,0.f,1.f}, {0.f,1.f,0.f}, {0.f,1.f,0.f}};
static	Fvector cmDir[6]	= {{1.f,0.f,0.f}, {-1.f,0.f,0.f},{0.f,1.f,0.f}, {0.f,-1.f,0.f},{0.f,0.f,1.f}, {0.f,0.f,-1.f}};
/*
void	light::export		(light_Package& package)
{
	if (flags.bShadow)			{
		switch (flags.type)	{
			case IRender_Light::POINT:
				{
					// tough: create/update 6 shadowed lights
					if (0==omnipart[0])	for (int f=0; f<6; f++)
					{
						omnipart[f] = xr_new<light> ();
						omnipart[f]->smap_near = smap_near;
						omnipart[f]->smap_near_neg = smap_near_neg;
					}
					for (int f=0; f<6; f++)	{
						light*	L			= omnipart[f];
						Fvector				R;
						R.crossproduct		(cmNorm[f],cmDir[f]);
						L->set_type			(IRender_Light::OMNIPART);
						L->set_shadow		(true);
						L->set_position		(position);
						L->set_rotation		(cmDir[f],	R);
						L->set_cone			(PI_DIV_2);
						L->set_range		(range);
						L->set_color		(color);
						L->spatial.sector	= spatial.sector;	//. dangerous?
						L->s_spot			= s_spot	;
						L->s_point			= s_point	;
						L->lsf_speed		= lsf_speed;
						L->lsf_amount		= lsf_amount;
						L->lsf_smap_jitter	= lsf_smap_jitter;
						package.v_shadowed.push_back	(L);
					}
				}
				break;
			case IRender_Light::SPOT:
				package.v_shadowed.push_back			(this);
				break;
		}
	}	else	{
		switch (flags.type)	{
			case IRender_Light::POINT:		package.v_point.push_back	(this);	break;
			case IRender_Light::SPOT:		package.v_spot.push_back	(this);	break;
		}
	}
}*/