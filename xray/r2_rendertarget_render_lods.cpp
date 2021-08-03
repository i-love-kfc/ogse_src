#include "r2_rendertarget.h"
#include "r2.h"
#include "RenderDevice.h"
#include "game_level.h"
#include "xr_render_console.h"

extern __declspec(dllimport) float r_ssaLOD_A;
extern __declspec(dllimport) float r_ssaLOD_B;
#pragma comment(lib, "d3d9.lib")

#define uiVertexPerImposter 4

ICF		bool	pred_dot		(const std::pair<float,u32>& _1, const std::pair<float,u32>& _2)	{ return _1.first < _2.first; }
void R_dsgraph_structure::r_dsgraph_render_lods	(bool _setup_zb, bool _clear)
{
/*	if (1==is_lods_limited)
	{
		if (_clear)			mapLOD.clear			();
		return;
	}*/
	if (_setup_zb)	mapLOD.getLR	(lstLODs)	;	// front-to-back
	else			mapLOD.getRL	(lstLODs)	;	// back-to-front
	if (lstLODs.empty())			return		;
 
	// *** Fill VB and generate groups
	u32		shid							= _setup_zb?SE_R1_LMODELS:SE_R1_NORMAL_LQ;
	FLOD*						firstV		= (FLOD*)lstLODs[0].pVisual;
	ref_selement				cur_S		= firstV->shader->E[shid];
	float	ssaRange						= r_ssaLOD_A - r_ssaLOD_B;
	if		(ssaRange<EPS_S)	ssaRange	= EPS_S;

	const u32	uiImpostersFit		= RCache.Vertex.GetSize()
		/ (firstV->geom->vb_stride*uiVertexPerImposter);
//	Msg("fit: %d, range: %f", uiImpostersFit, ssaRange);

	for (u32 i=0; i<lstLODs.size(); i++)
	{
		const u32	iBatchSize	= _min( lstLODs.size()-i, uiImpostersFit);
		int			cur_count	= 0;
		u32			vOffset;
		FLOD::_hw*	V			= (FLOD::_hw*)RCache.Vertex.Lock	(iBatchSize*uiVertexPerImposter,firstV->geom->vb_stride, vOffset);
		for ( u32 j=0; j<iBatchSize; ++j, ++i )
		{
			// sort out redundancy
			R_dsgraph::_LodItem		&P		= lstLODs[i];
			if (P.pVisual->shader->E[shid]==cur_S)	cur_count++	;
			else {
				lstLODgroups.push_back	(cur_count);
				cur_S				= P.pVisual->shader->E[shid];
				cur_count			= 1;
			}

			// calculate alpha
			float	ssaDiff					= P.ssa - r_ssaLOD_B;
			float	scale					= ssaDiff/ssaRange	;
			int		iA						= iFloor	((1-scale)*255.f);	
			u32		uA						= u32		(clampr(iA,0,255));

			// calculate direction and shift
			FLOD*							lodV		=	(FLOD*)P.pVisual;
			Fvector							Ldir,shift	;
			Ldir.sub						(lodV->vis.sphere.P,Device.vCameraPosition).normalize();
			shift.mul						(Ldir,-.5f * lodV->vis.sphere.R);

			// gen geometry
			FLOD::_face*					facets		= lodV->facets;
			svector<std::pair<float,u32>,8>	selector	;
			for (u32 s=0; s<8; s++)			selector.push_back	(mk_pair(Ldir.dotproduct(facets[s].N),s));
			std::sort						(selector.begin(),selector.end(),pred_dot);

			float							dot_best	= selector	[selector.size()-1].first	;
			float							dot_next	= selector	[selector.size()-2].first	;
			float							dot_next_2	= selector	[selector.size()-3].first	;
			u32								id_best		= selector	[selector.size()-1].second	;
			u32								id_next		= selector	[selector.size()-2].second	;

			// Now we have two "best" planes, calculate factor, and approx normal
			float	fA = dot_best, fB = dot_next, fC = dot_next_2;
			float	alpha	=	0.5f + 0.5f*(1-(fB-fC)/(fA-fC))	;
			int		iF		=	iFloor		(alpha*255.5f)		;
			u32		uF 		=	u32			(clampr	(iF,0,255))	;

			// Fill VB
			FLOD::_face&	FA				= facets[id_best]	;
			FLOD::_face&	FB				= facets[id_next]	;
			static int		vid [4]			= {3,0,2,1}			;
			for (u32 vit=0; vit<4; vit++)	{
				int			id	= vid[vit]			;
				V->p0.add	(FB.v[id].v,shift)		;
				V->p1.add	(FA.v[id].v,shift)		;
				V->n0		= FB.N					;
				V->n1		= FA.N					;
				V->sun_af	= color_rgba			(FB.v[id].c_sun,FA.v[id].c_sun,uA,uF);
				V->t0		= FB.v[id].t			;
				V->t1		= FA.v[id].t			;
				V->rgbh0	= FB.v[id].c_rgb_hemi	;
				V->rgbh1	= FA.v[id].c_rgb_hemi	;
				V++			;
			}
		}
		lstLODgroups.push_back		(cur_count);
		RCache.Vertex.Unlock		(iBatchSize*uiVertexPerImposter,firstV->geom->vb_stride);

		// *** Render
		int current=0;
		RCache.set_xform_world		(Fidentity);
		for (u32 g=0; g<lstLODgroups.size(); g++)	{
			int p_count				= lstLODgroups[g];
			Msg("Group %d, count %d", g, p_count);
			RCache.set_Element		(lstLODs[current].pVisual->shader->E[shid]);
			RCache.set_Geometry		(firstV->geom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
			RCache.stat.r.s_flora_lods.add	(4*p_count);
			current	+=	p_count		;
			vOffset	+=	4*p_count	;
		}
		lstLODgroups.clear	();
	}

	lstLODs.clear		();

	if (_clear)			mapLOD.clear			();
}


void FProgressive::Render	(float LOD)
{
	return;
/*	IDirect3DQuery9* pEvent;
	HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT, &pEvent);

	pEvent->Issue(D3DISSUE_END);
	while(S_FALSE == pEvent->GetData( NULL, 0, D3DGETDATA_FLUSH ))
	;*/
//	Msg("LOD %f", LOD);
	if (m_fast && RImplementation.phase==CRender::PHASE_SMAP)
	{
		int lod_id			= iFloor((1.f-clampr(LOD,0.f,1.f))*float(xSWI->count-1)+0.5f);
		VERIFY				(lod_id>=0 && lod_id<int(xSWI->count));
		FSlideWindow& SW	= xSWI->sw[lod_id];
		RCache.set_Geometry	(m_fast->rm_geom);
		RCache.Render		(D3DPT_TRIANGLELIST,m_fast->vBase,0,SW.num_verts,m_fast->iBase+SW.offset,SW.num_tris);
		RCache.stat.r.s_static.add	(SW.num_verts);
	} else {
//		Msg("LOD %f", LOD);
		int lod_id		= last_lod;
		if (LOD>=0.f){
			clamp			(LOD,0.f,1.f);
			lod_id			= iFloor((1.f-LOD)*float(nSWI.count-1)+0.5f);
			last_lod		= lod_id;
		}
		VERIFY				(lod_id>=0 && lod_id<int(nSWI.count));
/*		if (1==is_lods_limited)
		{
			lod_id = 0; last_lod = 0;
		}*/
//		Msg("lod_id %d", lod_id);
		FSlideWindow& SW	= nSWI.sw[lod_id];
		RCache.set_Geometry	(rm_geom);
/*		Fvector3 world1 = vis.box.min;
		Fvector3 world2 = vis.box.max;
		Fvector3 center = world2.sub(world1);
		center.mul(0.5);
		center.add(world1);
		Fvector3 radius = world2.sub(center);
		float max = abs(radius.x);
		if (abs(radius.y) > max)
			max = abs(radius.y);
		if (abs(radius.z) > max)
			max = abs(radius.z);*/
		
//		RCache.Render		(D3DPT_TRIANGLELIST,vBase,0,SW.num_verts,iBase+SW.offset,SW.num_tris);
		
		RCache.stat.calls			++;
		RCache.stat.verts			+= SW.num_verts;
		RCache.stat.polys			+= SW.num_tris;
		RCache.constants.flush		();

	/*	string512 str;
		sprintf(str, "Frame %d: DrawIndexedPrimitive for lod %d on coords %f, %f, %f, radius %f, render STARTED", Device.dwFrame, lod_id, center.x, center.y, center.z, max);
		OutputDebugString(str);*/

		HW.pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,vBase, 0, SW.num_verts,iBase+SW.offset,SW.num_tris);	

/*		string512 str1;
		sprintf(str1, "Frame %d: DrawIndexedPrimitive for lod %d on coords %f, %f, %f, render ENDED", Device.dwFrame, lod_id, world.x, world.y, world.z);
		OutputDebugString(str1);*/

/*		pEvent->Issue(D3DISSUE_END);
		while(S_FALSE == pEvent->GetData( NULL, 0, D3DGETDATA_FLUSH ))
		;*/
	}
}