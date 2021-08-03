#pragma once

#include "pureFrame.h"
#include "r_dsgraph_types.h"
#include "xrXRC.h"
#include "Render_interface.h"
#include "object.h"

class	R_dsgraph_structure										: public IRender_interface, public pureFrame
{
public:
	IRenderable*												val_pObject;
	Fmatrix*													val_pTransform;
	BOOL														val_bHUD;
	BOOL														val_bInvisible;
	BOOL														val_bRecordMP;		// record nearest for multi-pass
	R_feedback*													val_feedback;		// feedback for geometry being rendered
	u32															val_feedback_breakp;// breakpoint
	xr_vector<Fbox3,render_alloc<Fbox3> >*						val_recorder;		// coarse structure recorder
	u32															phase;
	u32															marker;
	bool														pmask		[2]		;
	bool														pmask_wmark			;
public:
	// Dynamic scene graph
	R_dsgraph::mapNormal_T										mapNormal	[2]		;	// 2==(priority/2)
	R_dsgraph::mapMatrix_T										mapMatrix	[2]		;
	R_dsgraph::mapSorted_T										mapSorted;
	R_dsgraph::mapHUD_T											mapHUD;
	R_dsgraph::mapLOD_T											mapLOD;
	R_dsgraph::mapSorted_T										mapDistort;

	R_dsgraph::mapSorted_T										mapWmark;			// sorted
	R_dsgraph::mapSorted_T										mapEmissive;

	xr_vector<R_dsgraph::mapNormalVS::TNode*,render_alloc<R_dsgraph::mapNormalVS::TNode*> >				nrmVS;
	xr_vector<R_dsgraph::mapNormalPS::TNode*,render_alloc<R_dsgraph::mapNormalPS::TNode*> >				nrmPS;
	xr_vector<R_dsgraph::mapNormalCS::TNode*,render_alloc<R_dsgraph::mapNormalCS::TNode*> >				nrmCS;
	xr_vector<R_dsgraph::mapNormalStates::TNode*,render_alloc<R_dsgraph::mapNormalStates::TNode*> >		nrmStates;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*,render_alloc<R_dsgraph::mapNormalTextures::TNode*> >	nrmTextures;
	xr_vector<R_dsgraph::mapNormalTextures::TNode*,render_alloc<R_dsgraph::mapNormalTextures::TNode*> >	nrmTexturesTemp;

	template class std::vector<R_dsgraph::mapMatrixVS::TNode*,render_alloc<R_dsgraph::mapMatrixVS::TNode*> >;
	template class std::vector<R_dsgraph::mapMatrixCS::TNode*,render_alloc<R_dsgraph::mapMatrixCS::TNode*> >;

	xr_vector<R_dsgraph::mapMatrixVS::TNode*,render_alloc<R_dsgraph::mapMatrixVS::TNode*> >				matVS;
	xr_vector<R_dsgraph::mapMatrixPS::TNode*,render_alloc<R_dsgraph::mapMatrixPS::TNode*> >				matPS;
	xr_vector<R_dsgraph::mapMatrixCS::TNode*,render_alloc<R_dsgraph::mapMatrixCS::TNode*> >				matCS;
	xr_vector<R_dsgraph::mapMatrixStates::TNode*,render_alloc<R_dsgraph::mapMatrixStates::TNode*> >		matStates;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*,render_alloc<R_dsgraph::mapMatrixTextures::TNode*> >	matTextures;
	xr_vector<R_dsgraph::mapMatrixTextures::TNode*,render_alloc<R_dsgraph::mapMatrixTextures::TNode*> >	matTexturesTemp;

	xr_vector<R_dsgraph::_LodItem,render_alloc<R_dsgraph::_LodItem> >	lstLODs		;
	xr_vector<int,render_alloc<int> >									lstLODgroups;
	xr_vector<ISpatial* /**,render_alloc<ISpatial*>/**/>				lstRenderables;
	xr_vector<ISpatial* /**,render_alloc<ISpatial*>/**/>				lstSpatial	;
	xr_vector<IRender_Visual*,render_alloc<IRender_Visual*> >			lstVisuals	;

	xr_vector<IRender_Visual*,render_alloc<IRender_Visual*> >			lstRecorded	;

	u32															counter_S	;
	u32															counter_D	;
	BOOL														b_loaded	;
public:

	void		set_Recorder			(xr_vector<Fbox3,render_alloc<Fbox3> >* dest)		{ val_recorder	= dest;	if (dest) dest->clear();	}

	void		r_pmask											(bool _1, bool _2, bool _wm=false)				{ pmask[0]=_1; pmask[1]=_2;	pmask_wmark = _wm; }

	void		r_dsgraph_insert_dynamic						(IRender_Visual	*pVisual, Fvector& Center);
	void		r_dsgraph_insert_static							(IRender_Visual	*pVisual);

	void		r_dsgraph_render_graph							(u32	_priority,	bool _clear=true);
	void		r_dsgraph_render_hud							();
	void		r_dsgraph_render_lods							(bool	_setup_zb,	bool _clear);
	void		r_dsgraph_render_sorted							();
	void		r_dsgraph_render_emissive						();
	void		r_dsgraph_render_wmarks							();
	void		r_dsgraph_render_distort						();
	void		r_dsgraph_render_subspace						(IRender_Sector* _sector, CFrustum* _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic, BOOL _precise_portals=FALSE	);
//	void		r_dsgraph_render_subspace						(IRender_Sector* _sector, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic, BOOL _precise_portals=FALSE	);
	void		r_dsgraph_render_R1_box							(IRender_Sector* _sector, Fbox& _bb, int _element);

	// wrappers
	void	__thiscall	w_r_dsgraph_render_graph						(u32	_priority,	bool _clear=true);
	void	__thiscall	w_r_dsgraph_render_subspace						(IRender_Sector* _sector, CFrustum* _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic, BOOL _precise_portals=FALSE	);
};

extern __declspec(dllimport) IRender_interface* Render;

inline void __declspec(naked) R_dsgraph_structure::w_r_dsgraph_render_graph (u32	_priority,	bool _clear)
{
	_asm
	{
		push	ebp
		mov		ebp, esp
		mov		eax, _priority;
		push	ecx;
		call	R_dsgraph_structure::r_dsgraph_render_graph;
		pop		ebp
		retn	8h;
	};
};

inline void __declspec(naked) R_dsgraph_structure::w_r_dsgraph_render_subspace (IRender_Sector* _sector, CFrustum* _frustum, Fmatrix& mCombined, Fvector& _cop, BOOL _dynamic, BOOL _precise_portals)
{
	_asm
	{
		push	ebp
		mov		ebp, esp
		push	_cop
		push	mCombined
		push	_frustum
		mov		edx, _sector;
		push	ecx;
		call	R_dsgraph_structure::r_dsgraph_render_subspace;
		pop		ebp
		retn	18h;
	};
};