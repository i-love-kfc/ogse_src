#pragma once
#include "xrXRC.h"
#include "base_types.h"
#include <d3dx9.h>
#include "kinematics.h"
#include "Render.h"
#include "occRasterizer.h"
#include "Game_Persistent.h"

const		u32					occq_size			= 2*768; //256	;	// queue for occlusion queries

// must conform to following order of allocation/free
// a(A), a(B), a(C), a(D), ....
// f(A), f(B), f(C), f(D), ....
// a(A), a(B), a(C), a(D), ....
//	this mean:
//		use as litle of queries as possible
//		first try to use queries allocated first
//	assumption:
//		used queries number is much smaller than total count

class R_occlusion
{
private:
	struct	_Q	{
		u32					order;
		IDirect3DQuery9*	Q;
	};

	BOOL					enabled;	// 
	xr_vector<_Q>			pool;		// sorted (max ... min), insertions are usually at the end
	xr_vector<_Q>			used;		// id's are generated from this and it is cleared from back only
	xr_vector<u32>			fids;		// free id's
public:
	u32				occq_get		(u32&	ID		);
	void			occq_create		(u32	limit	);
	void			occq_destroy	();
};
class CHOM
{
private:
	xrXRC					xrc;
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	BOOL					bEnabled;
	Fmatrix					m_xform;
	Fmatrix					m_xform_01;

	xrCriticalSection		MT;
	volatile u32			MT_frame_rendered;

public:
	BOOL					visible		(vis_data&	vis);
/*	BOOL					visible		(Fbox&		B);
	BOOL					visible		(sPoly&		P);
	BOOL					visible		(Fbox2&		B, float depth);	// viewport-space (0..1)*/

	void					Disable		();
	void					Enable		();
	void					Render		(CFrustum&	base);
	void					Render_DB	(CFrustum&	base);

	void					Load		();
	void					Unload		();

	// wrappers for matching calling conventions of current code and imported methods
	BOOL 	__thiscall		w_visible		(vis_data&	vis);
	void	__thiscall		w_Load			();
	void	__thiscall		w_Unload		();

	void	__stdcall		MT_RENDER	();
	ICF	void				MT_SYNC		();/*			{ 
		if (g_pGamePersistent->m_pMainMenu && g_pGamePersistent->m_pMainMenu->IsActive())
			return;

		MT_RENDER			(); 
	}*/
};
inline BOOL /*__declspec(naked)*/ CHOM::w_visible		(vis_data&	vis)
{
	__asm
	{
		mov		esi, [esp+8h];
		mov		eax, ecx;
		call	CHOM::visible;
	};	
};
inline void __thiscall CHOM::w_Load		()
{
	__asm
	{
		push	ecx;
		call	CHOM::Load;
	};
};
inline void __thiscall CHOM::w_Unload		()
{
	__asm
	{
		push	esi;
		mov		esi, ecx;
		call	CHOM::Unload;
		pop		esi;
	};
};