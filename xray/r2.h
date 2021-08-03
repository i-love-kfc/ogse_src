#pragma once

#include "r2_types.h"
#include "r_dsgraph_structure.h"
#include "r_occlusion.h"
#include "light.h"
#include "SMAP_Allocator.h"
#include "FMesh.h"
#include "particles.h"
#include "models.h"
#include "FVF.h"

#define VIEWPORT_NEAR  0.2f
static float OLES_SUN_LIMIT_27_01_07 = 180.f;
class CRender;
extern __declspec(dllimport) CRender RImplementation;	

extern unsigned int BASE_ADDRESS_XRRENDER_R2;
extern unsigned int BASE_ADDRESS_XRGAME;
extern BOOL ListProcessModules( DWORD dwPID );

class CDetailManager;
class CWallmarksEngine;
class IRenderTarget;
class CRenderTarget;

// definition
class CRender													:	public R_dsgraph_structure
{
public:
	enum
	{
		PHASE_NORMAL	= 0,	// E[0]
		PHASE_SMAP		= 1,	// E[1]
	};

public:
	struct		_options	{
		u32		bug					: 1;

		u32		smapsize			: 16;
		u32		depth16				: 1;
		u32		mrt					: 1;
		u32		mrtmixdepth			: 1;
		u32		fp16_filter			: 1;
		u32		fp16_blend			: 1;
		u32		albedo_wo			: 1;						// work-around albedo on less capable HW
		u32		HW_smap				: 1;
		u32		HW_smap_PCF			: 1;
		u32		HW_smap_FETCH4		: 1;

		u32		HW_smap_FORMAT		: 32;

		u32		nvstencil			: 1;
		u32		nvdbt				: 1;

		u32		nullrt				: 1;

		u32		distortion			: 1;
		u32		distortion_enabled	: 1;
		u32		mblur				: 1;

		u32		sunfilter			: 1;
		u32		sunstatic			: 1;
		u32		sjitter				: 1;
		u32		noshadows			: 1;
		u32		Tshadows			: 1;						// transluent shadows
		u32		disasm				: 1;

		u32		forcegloss			: 1;
		u32		forceskinw			: 1;
		float	forcegloss_v		;
	}			o;
	struct		_stats		{
		u32		l_total,	l_visible;
		u32		l_shadowed,	l_unshadowed;
		s32		s_used,		s_merged,	s_finalclip;
		u32		o_queries,	o_culled;
		u32		ic_total,	ic_culled;
	}			stats;
public:
	// Sector detection and visibility
	CSector*													pLastSector;
	Fvector														vLastCameraPos;
	u32															uLastLTRACK;
	xr_vector<IRender_Portal*>									Portals;
	xr_vector<IRender_Sector*>									Sectors;
	xrXRC														Sectors_xrc;
	CDB::MODEL*													rmPortals;
	CHOM														HOM;
	R_occlusion													HWOCC;

	// Global vertex-buffer container
	xr_vector<FSlideWindowItem>									SWIs;
	xr_vector<ref_shader>										Shaders;
	typedef svector<D3DVERTEXELEMENT9,MAXD3DDECLLENGTH+1>		VertexDeclarator;
	xr_vector<VertexDeclarator>									nDC,xDC;
	xr_vector<IDirect3DVertexBuffer9*>							nVB,xVB;
	xr_vector<IDirect3DIndexBuffer9*>							nIB,xIB;
	xr_vector<IRender_Visual*>									Visuals;
	CPSLibrary													PSLibrary;

	CDetailManager*												Details;
	CModelPool*													Models;
	CWallmarksEngine*											Wallmarks;

	CRenderTarget*												Target;			// Render-target

	CLight_DB													Lights;
	CLight_Compute_XFORM_and_VIS								LR;
	xr_vector<light*>											Lights_LastFrame;
	SMAP_Allocator												LP_smap_pool;
	light_Package												LP_normal;
	light_Package												LP_pending;

	xr_vector<Fbox3,render_alloc<Fbox3> >						main_coarse_structure;

	shared_str													c_sbase			;
	shared_str													c_lmaterial		;
	float														o_hemi			;
	float														o_sun			;
	IDirect3DQuery9*											q_sync_point[2]	;
	u32															q_sync_count	;

private:
	// Loading / Unloading
	void							LoadBuffers					(CStreamReader	*fs,	BOOL	_alternative);
	void							LoadVisuals					(IReader	*fs);
	void							LoadLights					(IReader	*fs);
	void							LoadPortals					(IReader	*fs);
	void							LoadSectors					(IReader	*fs);
	void							LoadSWIs					(CStreamReader	*fs);

	BOOL							add_Dynamic					(IRender_Visual	*pVisual, u32 planes);		// normal processing
	void							add_Static					(IRender_Visual	*pVisual, u32 planes);
	void							add_leafs_Dynamic			(IRender_Visual	*pVisual);					// if detected node's full visibility
	void							add_leafs_Static			(IRender_Visual	*pVisual);					// if detected node's full visibility

public:
	IRender_Sector*					rimp_detectSector			(Fvector& P, Fvector& D);
	void							render_main					(Fmatrix& mCombined, bool _fportals);
	void							render_forward				();
	void							render_smap_direct			(Fmatrix& mCombined);
	void							render_indirect				(light*			L	);
	void							render_lights				(light_Package& LP	);
	void							render_sun					();
	void							render_sun_near				();
	void							render_sun_filtered			();
	void							render_menu					();
public:
	ShaderElement*					rimp_select_sh_static		(IRender_Visual	*pVisual, float cdist_sq);
	ShaderElement*					rimp_select_sh_dynamic		(IRender_Visual	*pVisual, float cdist_sq);
	D3DVERTEXELEMENT9*				getVB_Format				(int id, BOOL	_alt=FALSE);
	IDirect3DVertexBuffer9*			getVB						(int id, BOOL	_alt=FALSE);
	IDirect3DIndexBuffer9*			getIB						(int id, BOOL	_alt=FALSE);
	FSlideWindowItem*				getSWI						(int id);
	IRender_Portal*					getPortal					(int id)			{ VERIFY(id<int(Portals.size()));	return Portals[id];	}
	IRender_Sector*					getSectorActive				();
	IRender_Visual*					model_CreatePE				(LPCSTR name);
	IRender_Sector*					detectSector				(const Fvector& P, Fvector& D);

	// HW-occlusion culling
	IC u32							occq_begin					(u32&	ID		);
	IC void							occq_end					(u32&	ID		);
	IC u32							occq_get					(u32&	ID		)	{ return HWOCC.occq_get		(ID);	}

	ICF void						apply_object				(IRenderable*	O);
	IC void							apply_lmaterial				()
	{
		R_constant*		C	= &*RCache.get_c	(RImplementation.c_sbase);		// get sampler
		if (0==C)			return;
		VERIFY				(RC_dest_sampler	== C->destination);
		VERIFY				(RC_sampler			== C->type);
		CTexture*		T	= RCache.get_ActiveTexture	(u32(C->samp.index));
		VERIFY				(T);
		float	mtl			= T->m_material;
	#ifdef	DEBUG
		if (ps_r2_ls_flags->test(R2FLAG_GLOBALMATERIAL))	mtl=ps_r2_gmaterial;
	#endif
		RCache.set_c		(RImplementation.c_lmaterial,RImplementation.o_hemi,RImplementation.o_sun,0,(mtl+.5f)/4.f);
	}

public:
	// feature level
//	GenerationLevel			get_generation			()	{ return IRender_interface::GENERATION_R2; }

	// Loading / Unloading
	void					create						();
	void					destroy						();
	void					reset_begin					();
	void					reset_end					();

	void					level_Load					(IReader*);
	void					level_Unload				();

	IDirect3DBaseTexture9*	texture_load			(LPCSTR	fname, u32& msize);
	HRESULT					shader_compile			(
		LPCSTR							name,
		LPCSTR                          pSrcData,
		UINT                            SrcDataLen,
		void*							pDefines,
		void*							pInclude,
		LPCSTR                          pFunctionName,
		LPCSTR                          pTarget,
		DWORD                           Flags,
		void*							ppShader,
		void*							ppErrorMsgs,
		void*							ppConstantTable);

	// Information
	void					Statistics					(CGameFont* F);
	LPCSTR					getShaderPath				()									{ return "r2\\";	}
	ref_shader				getShader					(int id);
	IRender_Sector*			getSector					(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
	IRender_Visual*			getVisual					(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
	IRender_Sector*			detectSector				(const Fvector& P);
	IRender_Target*			getTarget					();// {return Target;};

	// Main 
	void					flush						();
	void					set_Object					(IRenderable*		O	);
	void					add_Occluder				(Fbox2&	bb_screenspace	);			// mask screen region as oclluded
	void					add_Visual					(IRender_Visual*	V	)	{ add_leafs_Dynamic(V);								}			// add visual leaf	(no culling performed at all)
//	void					add_Geometry				(IRender_Visual*	V	); // add visual(s)	(all culling performed) 

	// wallmarks
	void					add_StaticWallmark			(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
	void					clear_static_wallmarks		();
	void					add_SkeletonWallmark		(intrusive_ptr<CSkeletonWallmark> wm);
	void					add_SkeletonWallmark		(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size);

	//
	IBlender*				blender_create				(CLASS_ID cls);
	void					blender_destroy				(IBlender* &);

	//
	IRender_ObjectSpecific*	ros_create					(IRenderable*		parent);
	void					ros_destroy					(IRender_ObjectSpecific* &);

	// Lighting
	IRender_Light*			light_create				();
	IRender_Glow*			glow_create					();

	// Models
	IRender_Visual*			model_CreateParticles		(LPCSTR name);
	IRender_DetailModel*	model_CreateDM				(IReader* F);
	IRender_Visual*			model_Create				(LPCSTR name, IReader* data=0);
	IRender_Visual*			model_CreateChild			(LPCSTR name, IReader* data);
	IRender_Visual*			model_Duplicate				(IRender_Visual*	V);
	void					model_Delete				(IRender_Visual* &	V, BOOL bDiscard);
	void 					model_Delete				(IRender_DetailModel* & F);
	void					model_Logging				(BOOL bEnable);
	void					models_Prefetch				();
	void					models_Clear				(BOOL b_complete);

	// Occlusion culling
	BOOL					occ_visible					(vis_data&	V);
	BOOL					occ_visible					(Fbox&		B);
	BOOL					occ_visible					(sPoly&		P);

	// Main
	void					Calculate					();
	void					Render						();
	void					Screenshot					(ScreenshotMode mode=SM_NORMAL, LPCSTR name = 0);
	void					OnFrame					();

	// Render mode
	void					rmNear						();
	void					rmFar						();
	void					rmNormal					();

	//wrappers
	void	__thiscall				w_LoadBuffers					(CStreamReader	*fs,	BOOL	_alternative);
	void	__thiscall				w_LoadVisuals					(IReader	*fs);
	void	__thiscall				w_LoadSectors					(IReader	*fs);
	void	__thiscall				w_LoadSWIs						(CStreamReader	*fs);
	void	__thiscall				w_render_forward				();

	void					render_rain_near			();
	void					render_rain					();
	BOOL					is_sun						();
};

extern __declspec(dllimport) float		r_dtex_range;

inline 
#if _MSC_VER == 1500
__declspec(naked)
#endif
void CRender::w_LoadBuffers (CStreamReader	*fs,	BOOL	_alternative)
{
	_asm
	{
		mov		eax, ecx;
		jmp		CRender::LoadBuffers;
		retn;
	};
};
inline 
#if _MSC_VER == 1500
__declspec(naked)
#endif
void CRender::w_LoadVisuals (IReader	*fs)
{
	__asm
	{
		pop		eax;
		push	ecx;
		push	eax;
		jmp		CRender::LoadVisuals;
		retn	4;
	};
};
inline 
#if _MSC_VER == 1500
__declspec(naked)
#endif
void CRender::w_LoadSectors (IReader	*fs)
{
	__asm
	{
		pop		eax;
		push	ecx;
		push	eax;
		jmp		CRender::LoadSectors;
		retn	4;
	};
};
inline 
#if _MSC_VER == 1500
__declspec(naked)
#endif
void CRender::w_LoadSWIs (CStreamReader	*fs)
{
	__asm
	{
		mov		eax, [esp+4];
		push	ecx;
		call	CRender::LoadSWIs;
		retn	4;
	};
};
inline void CRender::w_render_forward ()
{
	__asm
	{
		mov		esi, ecx;
		call	CRender::render_forward;
	};
}