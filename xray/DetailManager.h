// DetailManager.h: interface for the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////
#ifndef DETAILMANAGER_H
#define DETAILMANAGER_H

#pragma once

#include "Render.h"
#include "xrXRC.h"
#include "xrPool.h"
#include "HW.h"
#include "RenderDevice.h"
#include "xr_render_console.h"

//#define HWI
#define DYNAMIC_DETAILS

class CDetail		: public IRender_DetailModel
{
public:
	void			Load		(IReader* S);
	void			Optimize	();
	virtual void	Unload		();

	virtual void	transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset);
	virtual void	transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, float du, float dv);
	virtual			~CDetail	();

	// wrapper
	void __thiscall	w_Optimize	();
};

inline void CDetail::w_Optimize	()
{
	__asm
	{
		mov		esi, ecx;
		call	CDetail::Optimize;
	}
};

#pragma pack(push,1)

#define DETAIL_VERSION		3
#define DETAIL_SLOT_SIZE	2.f
#define DETAIL_SLOT_SIZE_2	DETAIL_SLOT_SIZE*0.5f
#define DO_NO_WAVING	0x0001

#ifndef DYNAMIC_DETAILS
	const u32		dm_size				= 124;//iFloor(ps_r__detail_radius/4)*2;				//!
	const u32 		dm_cache1_line		= dm_size*2/dm_cache1_count;		//! dm_size*2 must be div dm_cache1_count
	const u32		dm_cache_line		= dm_size+1+dm_size;
	const u32		dm_cache_size		= dm_cache_line*dm_cache_line;
	const float	dm_fade				= float(2*dm_size)-.5f;
#else
	extern u32		dm_size;
	extern u32 		dm_cache1_line;
	extern u32		dm_cache_line;
	extern u32		dm_cache_size;
	extern float	dm_fade;
	extern float	ps_current_detail_density;
#endif

const u32	dm_max_decompress	= 7;
const u32 		dm_cache1_count		= 4;								// 
const u32		dm_max_objects		= 64;
const u32		dm_obj_in_slot		= 4;
const float	dm_slot_size		= DETAIL_SLOT_SIZE;
const u32		dm_max_cache_size	= 152321; // assuming max dm_size = 124
extern u32		dm_current_size;//				= iFloor((float)ps_r__detail_radius/4)*2;				//!
extern u32 		dm_current_cache1_line;//		= dm_current_size*2/dm_cache1_count;		//! dm_current_size*2 must be div dm_cache1_count
extern u32		dm_current_cache_line;//		= dm_current_size+1+dm_current_size;
extern u32		dm_current_cache_size;//		= dm_current_cache_line*dm_current_cache_line;
extern float	dm_current_fade;//				= float(2*dm_current_size)-.5f;

struct DetailHeader
{
	u32		version;
	u32		object_count;
	int		offs_x,	offs_z;
	u32		size_x,	size_z;
};
struct DetailPalette
{
	u16		a0:4;
	u16		a1:4;
	u16		a2:4;
	u16		a3:4;
};
struct DetailSlot					// was(4+4+3*4+2 = 22b), now(8+2*4=16b)
{
	u32				y_base	:	12;	// 11	// 1 unit = 20 cm, low = -200m, high = 4096*20cm - 200 = 619.2m
	u32				y_height:	8;	// 20	// 1 unit = 10 cm, low = 0,     high = 256*10 ~= 25.6m
	u32				id0		:   6;	// 26	// 0x3F(63) = empty
	u32				id1		:	6;	// 32	// 0x3F(63) = empty
	u32				id2		:	6;	// 38	// 0x3F(63) = empty
	u32				id3     :	6;	// 42	// 0x3F(63) = empty
	u32				c_dir	:	4;	// 48	// 0..1 q
	u32				c_hemi	:	4;	// 52	// 0..1 q
	u32				c_r		:	4;	// 56	// rgb = 4.4.4
	u32				c_g		:	4;	// 60	// rgb = 4.4.4
	u32				c_b		:	4;	// 64	// rgb = 4.4.4
	DetailPalette	palette [4];
public:
	enum			{	ID_Empty	= 0x3f	};
public:
	void			w_y		(float base, float height)				
	{	
		s32	_base	= iFloor((base + 200)/.2f);			clamp(_base,	0,4095);	y_base		= _base;
		f32 _error	= base - r_ybase();
		s32	_height = iCeil ((height+_error) / .1f);	clamp(_height,	0,255);		y_height	= _height;
	}

	float			r_ybase		()						{	return float(y_base)*.2f - 200.f;								}
	float			r_yheight	()						{	return float(y_height)*.1f;									}
	u32				w_qclr		(float v, u32 range)	{	s32 _v = iFloor(v * float(range)); clamp(_v,0,s32(range)); return _v; };
	float			r_qclr		(u32 v,   u32 range)	{	return float(v)/float(range); }

    void			color_editor(){c_dir=w_qclr(0.5f,15);c_hemi=w_qclr(0.5f,15);c_r=w_qclr(0.f,15);c_g=w_qclr(0.f,15);c_b=w_qclr(0.f,15);}
    u8				r_id		(u32 idx) {	
        switch(idx)	{
        case 0: return (u8)id0;
        case 1: return (u8)id1;
        case 2: return (u8)id2;
        case 3: return (u8)id3;
        default: NODEFAULT;	
        }
    }
    void			w_id		(u32 idx, u8 val) {	
        switch(idx){
        case 0: id0=val; break;
        case 1: id1=val; break;
        case 2: id2=val; break;
        case 3: id3=val; break;
        default: NODEFAULT;
        }
    }
};

#pragma pack(pop)

class CDetailManager
{
public:
	struct	SlotItem	{								// один кустик
		float						scale;
		float						scale_calculated;
		Fmatrix						mRotY;
		u32							vis_ID;				// индекс в visibility списке он же тип [не качается, качается1, качается2]
		float						c_hemi;
		float						c_sun;
		bool						need_to_render_anyway[3];
	};
	DEFINE_VECTOR(SlotItem*,SlotItemVec,SlotItemVecIt);
	struct	SlotPart	{                              	// 
		u32							id;					// ID модельки
		SlotItemVec					items;              // список кустиков
		SlotItemVec					r_items[3];         // список кустиков for render
	};
	enum	SlotType	{
		stReady						= 0,				// Ready to use
		stPending,										// Pending for decompression

		stFORCEDWORD				= 0xffffffff
	};
	struct	Slot		{								// распакованый слот размером DETAIL_SLOT_SIZE
		struct{
			u32						empty	:1;
			u32						type	:1;
			u32						frame	:30;
		};
		int							sx,sz;				// координаты слота X x Y
		vis_data					vis;				// 
		SlotPart					G[dm_obj_in_slot];	// 

									Slot()				{ frame=0;empty=1; type=stReady; sx=sz=0; vis.clear(); }
	};
    struct 	CacheSlot1	{
		u32							empty;
    	vis_data 					vis;
        Slot** 						slots[dm_cache1_count*dm_cache1_count];
		CacheSlot1()				{empty=1; vis.clear();}
    };

	typedef	xr_vector<xr_vector <SlotItemVec* > >	vis_list;
	typedef	svector<CDetail*,dm_max_objects>	DetailVec;
	typedef	DetailVec::iterator					DetailIt;
	typedef	poolSS<SlotItem,/*4096*/65536>				PSS;				// KD: try to avoid blinking
public:
	int								dither			[16][16];
public:
	// swing values
	struct SSwingValue{
		float						rot1;
		float						rot2;
		float						amp1;
		float						amp2;
		float						speed;
		void						lerp	(const SSwingValue& v1, const SSwingValue& v2, float factor);
	};
	SSwingValue						swing_desc[2];
	SSwingValue						swing_current; 
public:
	IReader*						dtFS;
	DetailHeader					dtH;
	DetailSlot*						dtSlots;		// note: pointer into VFS
	DetailSlot						DS_empty;

public:
	DetailVec						objects;
	vis_list						m_visibles	[3];	// 0=still, 1=Wave1, 2=Wave2
  
	xrXRC							xrc;   
#ifndef DYNAMIC_DETAILS
	CacheSlot1 						cache_level1[dm_cache1_line][dm_cache1_line];
	Slot*							cache		[dm_cache_line][dm_cache_line];	// grid-cache itself
	svector<Slot*,dm_cache_size>	cache_task;									// non-unpacked slots
	Slot							cache_pool	[dm_cache_size];				// just memory for slots
#else
	CacheSlot1** 					cache_level1;
	Slot***							cache;	// grid-cache itself
	svector<Slot*,dm_max_cache_size>	cache_task;									// non-unpacked slots
	Slot*							cache_pool;				// just memory for slots
#endif

	int								cache_cx;
	int								cache_cz;

	PSS								poolSI;										// pool из которого выделяются SlotItem

	void							UpdateVisibleM	();
	void							UpdateVisibleS	();
public:
	IC bool							UseVS			()		{ return HW.Caps.geometry_major >= 1; }

	// Software processor
    ref_geom						soft_Geom;
	void							soft_Load		();
	void							soft_Unload		();
	void							soft_Render		();

	// Hardware processor
	ref_geom						hw_Geom;
	u32								hw_BatchSize;
	IDirect3DVertexBuffer9*			hw_VB;
	IDirect3DIndexBuffer9*			hw_IB;
	ref_constant					hwc_consts;
	ref_constant					hwc_wave;
	ref_constant					hwc_wind;
	ref_constant					hwc_array;
	ref_constant					hwc_s_consts;
	ref_constant					hwc_s_xform;
	ref_constant					hwc_s_array;
	void							hw_Load			();
	void							hw_Unload		();
	void							hw_Render		();
	void							hw_Render_dump	(ref_constant array, u32 var_id, u32 lod_id, u32 c_base);
	void							hw_Render_dump_lod	(ref_constant array, u32 c_base);

public:
	// get unpacked slot
	DetailSlot&						QueryDB			(int sx, int sz);
    
	void							cache_Initialize();
	void							cache_Update	(int sx, int sz, Fvector& view, int limit);
	void							cache_Task		(int gx, int gz, Slot* D);
	Slot*							cache_Query		(int sx, int sz);
	void							cache_Decompress(Slot* D);
	BOOL							cache_Validate	();
    // cache grid to world
	int								cg2w_X			(int x)			{ return cache_cx-dm_current_size+x;					}
	int								cg2w_Z			(int z)			{ return cache_cz-dm_current_size+(dm_current_cache_line-1-z);	}
    // world to cache grid 
	int								w2cg_X			(int x)			{ return x-cache_cx+dm_current_size;					}
	int								w2cg_Z			(int z)			{ return cache_cz-dm_current_size+(dm_current_cache_line-1-z);	}

	void							Load			();			// CRender::level_Load();
	void							Unload			();			// CRender::level_Unload();
	void							Render			();			// CRender::Render(), CRender::render_sun_near()

	// MT stuff
	xrCriticalSection				MT;
	volatile u32					m_frame_calc;
	volatile u32					m_frame_rendered;

	void	__stdcall				MT_CALC			() ;		// CRender::OnFrame();
	ICF	void						MT_SYNC			() {
		if (m_frame_calc == Device.dwFrame)
			return;

		MT_CALC						();	
	}

	CDetailManager					();				// CRender::level_Load();
	virtual ~CDetailManager			();				// CRender::level_Unload();

#ifdef HWI
	IDirect3DVertexBuffer9*			HI_instance_data;
#endif

	ref_geom						hw_lod_Geom;
	IDirect3DVertexBuffer9*			hw_lod_VB;
	IDirect3DIndexBuffer9*			hw_lod_IB;
};

#endif