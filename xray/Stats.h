#pragma once

#include "pureRender.h"
#include "xrCommon.h"

class Object;
struct _REG_INFO
{
	void *Object;
	int Prio;
	unsigned int Flags;
};
extern int	__cdecl	_REG_Compare(const void *e1, const void *e2);
template<class T>
class CRegistrator
{
public:
	xr_vector<_REG_INFO> R;
	struct {
		u32		in_process	:1;
		u32		changed		:1;
	};
	
	CRegistrator()			{ in_process=false; changed=false;}
	void Add	(T *obj, int priority=REG_PRIORITY_NORMAL, u32 flags=0)
	{
		_REG_INFO			I;
		I.Object			=obj;
		I.Prio				=priority;
		I.Flags				=flags;
		R.push_back			(I);
	
		if(in_process)		changed=true;
		else Resort			( );
	};
	void Remove	(T *obj)
	{
		for (u32 i=0; i<R.size(); i++) {
			if (R[i].Object==obj) R[i].Prio = REG_PRIORITY_INVALID;
		}
		if(in_process)		changed=true;
		else Resort			( );
	};
	void Resort	(void)
	{
		qsort	(&*R.begin(),R.size(),sizeof(_REG_INFO),_REG_Compare);
		while	((R.size()) && (R[R.size()-1].Prio==REG_PRIORITY_INVALID)) R.pop_back();
		if (R.empty())		R.clear		();
		changed				= false;
	};
};
class pureStats;
class CGameFont;
struct pureStatsVtbl
{ 
	void (__thiscall *OnStats)(pureStats *_this);
};

class pureStats
{ 
	pureStatsVtbl *vfptr;
};

class CStats: public pureRender
{
public:
	CGameFont*	pFont;

	float		fFPS,fRFPS,fTPS		;			// FPS, RenderFPS, TPS
	float		fMem_calls			;
	u32			dwMem_calls			;
	u32			dwSND_Played,dwSND_Allocated;	// Play/Alloc
	float		fShedulerLoad		;

	CStatTimer	EngineTOTAL;			// 
	CStatTimer	Sheduler;				// 
	CStatTimer	UpdateClient;			// 
	u32			UpdateClient_updated;	//
	u32			UpdateClient_crows;		//
	u32			UpdateClient_active;	//
	u32			UpdateClient_total;		//
	u32			Particles_starting;	// starting
	u32			Particles_active;	// active
	u32			Particles_destroy;	// destroying
	CStatTimer	Physics;			// movement+collision
	CStatTimer	ph_collision;		// collision
	CStatTimer	ph_core;			// collision
	CStatTimer	AI_Think;			// thinking
	CStatTimer	AI_Range;			// query: range
	CStatTimer	AI_Path;			// query: path
	CStatTimer	AI_Node;			// query: node
	CStatTimer	AI_Vis;				// visibility detection - total
	CStatTimer	AI_Vis_Query;		// visibility detection - portal traversal and frustum culling
	CStatTimer	AI_Vis_RayTests;	// visibility detection - ray casting

	CStatTimer	RenderTOTAL;		// 
	CStatTimer	RenderTOTAL_Real;	
	CStatTimer	RenderCALC;			// portal traversal, frustum culling, entities "renderable_Render"
	CStatTimer	RenderCALC_HOM;		// HOM rendering
	CStatTimer	Animation;			// skeleton calculation
	CStatTimer	RenderDUMP;			// actual primitive rendering
	CStatTimer	RenderDUMP_Wait;	// ...waiting something back (queries results, etc.)
	CStatTimer	RenderDUMP_Wait_S;	// ...frame-limit sync
	CStatTimer	RenderDUMP_RT;		// ...render-targets
	CStatTimer	RenderDUMP_SKIN;	// ...skinning
	CStatTimer	RenderDUMP_HUD;		// ...hud rendering
	CStatTimer	RenderDUMP_Glows;	// ...glows vis-testing,sorting,render
	CStatTimer	RenderDUMP_Lights;	// ...d-lights building/rendering
	CStatTimer	RenderDUMP_WM;		// ...wallmark sorting, rendering
	u32			RenderDUMP_WMS_Count;// ...number of static wallmark
	u32			RenderDUMP_WMD_Count;// ...number of dynamic wallmark
	u32			RenderDUMP_WMT_Count;// ...number of wallmark tri
	CStatTimer	RenderDUMP_DT_VIS;	// ...details visibility detection
	CStatTimer	RenderDUMP_DT_Render;// ...details rendering
	CStatTimer	RenderDUMP_DT_Cache;// ...details slot cache access
	u32			RenderDUMP_DT_Count;// ...number of DT-elements
	CStatTimer	RenderDUMP_Pcalc;	// ...projectors	building
	CStatTimer	RenderDUMP_Scalc;	// ...shadows		building
	CStatTimer	RenderDUMP_Srender;	// ...shadows		render
	
	CStatTimer	Sound;				// total time taken by sound subsystem (accurate only in single-threaded mode)
	CStatTimer	Input;				// total time taken by input subsystem (accurate only in single-threaded mode)
	CStatTimer	clRAY;				// total: ray-testing
	CStatTimer	clBOX;				// total: box query
	CStatTimer	clFRUSTUM;			// total: frustum query
	
	CStatTimer	netClient1;
	CStatTimer	netClient2;
	CStatTimer	netServer;
	
	CStatTimer	TEST0;				// debug counter
	CStatTimer	TEST1;				// debug counter
	CStatTimer	TEST2;				// debug counter
	CStatTimer	TEST3;				// debug counter

	shared_str	eval_line_1;
	shared_str	eval_line_2;
	shared_str	eval_line_3;

	void			Show			(void);
/*	virtual void 	OnRender		();
	void			OnDeviceCreate	(void);
	void			OnDeviceDestroy	(void);*/
public:
	xr_vector		<shared_str>	errors;
	CRegistrator	<pureStats>		seqStats;
/*public:
					CStats			();
					~CStats			();

	IC CGameFont*	Font			(){return pFont;}*/

	// KD
/*	CStatTimer	RenderDUMP_DOF;
	CStatTimer	RenderDUMP_Sunshafts;
	CStatTimer	RenderDUMP_SSAO;
	CStatTimer	RenderDUMP_AA;*/
};