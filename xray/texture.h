#pragma once
#include "FastDelegate.h"
#include "video_player.h"
#include "base_types.h"
#include <utility> 
#define D3D_DEBUG_INFO
#include <d3d9.h>

#define _RELEASE(x)			{ if(x) { (x)->Release();       (x)=NULL; } }

/*class CTexture;
struct CTextureVtbl
{
  void *(__thiscall *__vecDelDtor)(CTexture *_this, unsigned int);
};
*/
struct WaveForm
{
	enum EFunction
	{
		fCONSTANT = 0x0,
		fSIN = 0x1,
		fTRIANGLE = 0x2,
		fSQUARE = 0x3,
		fSAWTOOTH = 0x4,
		fINVSAWTOOTH = 0x5,
		fFORCE32 = 0xFFFFFFFF,
	};
	WaveForm::EFunction F;
	float arg[4];
};

class /*__declspec(dllimport)*/ CMatrix : public xr_resource_named								{
public:
	enum {
		modeProgrammable = 0,
		modeTCM,
		modeS_refl,
		modeC_refl,
		modeDetail
	};
	enum
	{
		tcmScale		= (1<<0),
		tcmRotate		= (1<<1),
		tcmScroll		= (1<<2),
		tcmFORCE32		= u32(-1)
	};
public:
	Fmatrix			xform;

	u32				dwFrame;
	u32				dwMode;
	union
	{
		u32		tcm;				// mask for tc-modifiers
		Flags32	tcm_flags;
	};
	WaveForm		scaleU, scaleV;
	WaveForm		rotate;
	WaveForm		scrollU,scrollV;

	/*
	CMatrix			()
	{
	Memory.mem_fill	(this,0,sizeof(CMatrix));
	}

	IC void			tc_trans	(Fmatrix& T, float u, float v)
	{
	T.identity	();
	T.m[2][0] = u;
	T.m[2][1] = v;
	}
	void			Calculate	();

	IC	BOOL		Similar		(CMatrix& M)		// comare by modes and params
	{
	if (dwMode!=M.dwMode)				return FALSE;
	if (tcm!=M.tcm)						return FALSE;
	if (!scaleU.Similar(M.scaleU))		return FALSE;
	if (!scaleV.Similar(M.scaleV))		return FALSE;
	if (!rotate.Similar(M.rotate))		return FALSE;
	if (!scrollU.Similar(M.scrollU))	return FALSE;
	if (!scrollV.Similar(M.scrollV))	return FALSE;
	return TRUE;
	}

	void			Load		(IReader* fs);
	void			Save		(IWriter* fs);
	*/
};

typedef	resptr_core<CMatrix,resptr_base<CMatrix> >	ref_matrix;
class CTexture : public xr_resource_named
{
public:
//	CTextureVtbl *vfptr;
	struct 
	{
		u32					bLoaded		: 1;
		u32					bUser		: 1;
		u32					seqCycles	: 1;
		u32					MemoryUsage	: 28;

	} flags;
	fastdelegate::FastDelegate1<unsigned int,void> bind;
	IDirect3DBaseTexture9 *pSurface;
	CAviPlayerCustom *pAVI;
	CTheoraSurface *pTheora;
	float m_material;
	shared_str m_bumpmap;
	union{
		u32								m_play_time;		// sync theora time
		u32								seqMSPF;			// Sequence data milliseconds per frame
	};
	xr_vector<IDirect3DBaseTexture9 *> seqDATA;
	IDirect3DBaseTexture9 *desc_cache;
	_D3DSURFACE_DESC desc;

	BOOL								desc_valid		()		;//{ return pSurface==desc_cache; }
	void								desc_enshure	()		;//{ if (!desc_valid()) desc_update(); }
	void								desc_update		();
public:
	void	__stdcall					apply_load		(u32	stage);
	void	__stdcall					apply_theora	(u32	stage);
	void	__stdcall					apply_avi		(u32	stage);
	void	__stdcall					apply_seq		(u32	stage);
	void	__stdcall					apply_normal	(u32	stage);

	void								Preload			();
	void								Load			();
	void								PostLoad		();
	void								Unload			(void);

	void								surface_set		(IDirect3DBaseTexture9* surf);
	IDirect3DBaseTexture9*				surface_get 	();

	BOOL								isUser			()		;//{ return flags.bUser;					}
	u32									get_Width		()		;//{ desc_enshure(); return desc.Width;	}
	u32									get_Height		()		;//{ desc_enshure(); return desc.Height;	}

	void								video_Sync		(u32 _time) ;//{m_play_time=_time;}
	void								video_Play		(BOOL looped, u32 _time=0xFFFFFFFF);
	void								video_Pause		(BOOL state);
	void								video_Stop		();
	BOOL								video_IsPlaying	();

	CTexture							();
	virtual ~CTexture					();
};

struct resptrcode_texture : public resptr_base<CTexture>
{
	void				create			(LPCSTR	_name);
	void				destroy			()					{ _set(NULL);					}
	shared_str			bump_get		()					{ return _get()->m_bumpmap;		}
	bool				bump_exist		()					{ return 0!=bump_get().size();	}
};

/*struct STextureList
{
	STextureListVtbl *vfptr;
	xr_resource_flagged baseclass_4;
	xr_vector<std::pair<unsigned int,resptr_core<CTexture,resptrcode_texture> > > baseclass_c;
	void push_back(std::pair<unsigned int,class resptr_core<CTexture,resptrcode_texture> > const &);
	STextureList();
	~STextureList()
	{
		Device.Resources->_DeleteTextureList(this);
	}
};
*/
typedef resptr_core<CTexture,resptrcode_texture> text_pt;
typedef resptr_core<CTexture,resptrcode_texture> ref_texture;
template class std::vector<ref_texture,xalloc<ref_texture> >;
struct STextureList : public xr_resource_flagged , public xr_vector<std::pair<unsigned int,text_pt > >
{
//public:
	STextureList();
	~STextureList();
	BOOL		equal (const STextureList& base) const;
	virtual void clear();
	virtual void clear_not_free();
};
typedef	resptr_core<STextureList,resptr_base<STextureList> >								ref_texture_list;

struct SMatrixList : public xr_resource_flagged, public svector<ref_matrix, 4>
{
	~SMatrixList();
};
typedef resptr_core<SMatrixList,resptr_base<SMatrixList>> ref_matrix_list;