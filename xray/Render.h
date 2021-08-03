#pragma once
#include "xrCommon.h"
#include "kinematics.h"
#include "collidable.h"
#include "RenderDevice.h"

class IRender_DetailModel;
struct IRender_Glow;
class IRender_Light;
struct IRender_Mesh;
struct IRender_ObjectSpecific;
class IRender_Portal;
class IRender_Sector;
class IRender_Target;
class light;
struct IRenderable;

class	CPortal;

#define FRUSTUM_MAXPLANES	12
#define FRUSTUM_SAFE		(FRUSTUM_MAXPLANES*4)
typedef svector<Fvector,FRUSTUM_SAFE>		sPoly;

const	int					lt_hemisamples	= 26	;

enum EFC_Visible {
	fcvNone = 0,
	fcvPartial,
	fcvFully,
	fcv_forcedword = u32(-1)
};

//////////////////////////////////////////////////////////////////////////
// definition (Target)
class	IRender_Target				{
public:
	virtual	void					set_blur			(float	f)							= 0;
	virtual	void					set_gray			(float	f)							= 0;
	virtual void					set_duality_h		(float	f)							= 0;
	virtual void					set_duality_v		(float	f)							= 0;
	virtual void					set_noise			(float	f)							= 0;
	virtual void					set_noise_scale		(float	f)							= 0;
	virtual void					set_noise_fps		(float	f)							= 0;
	virtual void					set_color_base		(u32	f)							= 0;
	virtual void					set_color_gray		(u32	f)							= 0;
	virtual void					set_color_add		(u32	f)							= 0;
	virtual u32						get_width			()									= 0;
	virtual u32						get_height			()									= 0;
	virtual ~IRender_Target()		{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Object Specific data)

struct IRender_ObjectSpecificVtbl
{
  void (__thiscall *force_mode)(IRender_ObjectSpecific *_this, unsigned int);
  float (__thiscall *get_luminocity)(IRender_ObjectSpecific *_this);
  float (__thiscall *get_luminocity_hemi)(IRender_ObjectSpecific *_this);
  void *(__thiscall *__vecDelDtor)(IRender_ObjectSpecific *_this, unsigned int);
};

struct IRender_ObjectSpecific
{
  IRender_ObjectSpecificVtbl *vfptr;
  enum mode
	{
	  TRACE_LIGHTS = 0x1,
	  TRACE_SUN = 0x2,
	  TRACE_HEMI = 0x4,
	  TRACE_ALL = 0x7,
	};
};

class	CROS_impl			: public IRender_ObjectSpecific
{
public:
	struct	Item			{
		u32					frame_touched		;	// to track creation & removal
		light*				source				;	// 
		collide::ray_cache	cache				;	//
		float				test				;	// note range: (-1[no]..1[yes])
		float				energy				;	//
	};
	struct	Light			{
		light*				source				;
		float				energy				;
		Fcolor				color				;
	};
public:
	// general
	u32						MODE				;
	u32						dwFrame				;
	u32						dwFrameSmooth		;

	// 
	xr_vector<Item>			track				;	// everything what touches
	xr_vector<Light>		lights				;	// 

	bool					result				[lt_hemisamples];
	collide::ray_cache		cache				[lt_hemisamples];
	collide::ray_cache		cache_sun			;
	s32						result_count		;
	u32						result_iterator		;
	u32						result_frame		;
	s32						result_sun			;
public:
	u32						shadow_gen_frame	;
	u32						shadow_recv_frame	;
	int						shadow_recv_slot	;
private:
	float					hemi_value			;
	float					hemi_smooth			;
	float					sun_value			;
	float					sun_smooth			;

	Fvector					approximate			;

public:
	ICF	float				get_hemi			()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									hemi_smooth;
	}
	ICF	float				get_sun				()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									sun_smooth;
	}
	void					update(IRenderable*	O);
	void					update_smooth		(IRenderable*	O=0);
};

//////////////////////////////////////////////////////////////////////////
// definition (Dynamic Light)
class IRender_Light	: public xr_resource									{
public:
	enum LT
	{
		DIRECT		= 0,
		POINT		= 1,
		SPOT		= 2,
		OMNIPART	= 3,
		REFLECTED	= 4,
	};
public:
	virtual void					set_type			(LT type)							= 0;
	virtual void					set_active			(bool)								= 0;
	virtual bool					get_active			()									= 0;
	virtual void					set_shadow			(bool)								= 0;
	virtual void					set_indirect		(bool)								{};
	virtual void					set_position		(const Fvector& P)					= 0;
	virtual void					set_rotation		(const Fvector& D, const Fvector& R)= 0;
	virtual void					set_cone			(float angle)						= 0;
	virtual void					set_range			(float R)							= 0;
	virtual void					set_virtual_size	(float R)							= 0;
	virtual void					set_texture			(LPCSTR name)						= 0;
	virtual void					set_color			(const Fcolor& C)					= 0;
	virtual void					set_color			(float r, float g, float b)			= 0;
	virtual ~IRender_Light()		;
};
struct resptrcode_light	: public resptr_base<IRender_Light>
{
	void				destroy			()				{ _set(NULL);						}
};
typedef	resptr_core<IRender_Light,resptrcode_light >	ref_light;
template class std::vector<ref_light, xalloc<ref_light>>;
/*
//////////////////////////////////////////////////////////////////////////
// definition (Portal)
class IRender_Portal				{
public:
	virtual ~IRender_Portal()			{};
};*/
struct IRender_PortalVtbl
{
	void *(__thiscall *__vecDelDtor)(IRender_Portal *_this, unsigned int);
};
class IRender_Portal
{
	IRender_PortalVtbl*	vfptr;
};

//////////////////////////////////////////////////////////////////////////
// definition (Sector)
/*class IRender_Sector				{
public:
	virtual ~IRender_Sector()			{};
};*/
struct IRender_SectorVtbl
{
	void *(__thiscall *__vecDelDtor)(IRender_Sector *_this, unsigned int);
};
class IRender_Sector
{
	IRender_SectorVtbl*	vfptr;
};

#define FRUSTUM_P_LEFT		(1<<0)
#define FRUSTUM_P_RIGHT		(1<<1)
#define FRUSTUM_P_TOP		(1<<2)
#define FRUSTUM_P_BOTTOM	(1<<3)
#define FRUSTUM_P_NEAR		(1<<4)
#define FRUSTUM_P_FAR		(1<<5)

#define FRUSTUM_P_LRTB		(FRUSTUM_P_LEFT|FRUSTUM_P_RIGHT|FRUSTUM_P_TOP|FRUSTUM_P_BOTTOM)
#define FRUSTUM_P_ALL		(FRUSTUM_P_LRTB|FRUSTUM_P_NEAR|FRUSTUM_P_FAR)

class CFrustum
{
public:
	struct fplane	: public Fplane
	{
		u32			aabb_overlap_id;	// [0..7]
		void		cache	();	
	};
	fplane			planes	[FRUSTUM_MAXPLANES];
	int				p_count;

	void			_add				(Fplane &P);
	IC void			_clear				()				{ p_count=0; }

	EFC_Visible		testSphere			(Fvector& c, float r, u32& test_mask)					const;
	BOOL			testSphere_dirty	(Fvector& c, float r)									const;
	EFC_Visible		testAABB			(const float* mM, u32& test_mask)						const;
	EFC_Visible		testSAABB			(Fvector& c, float r, const float* mM, u32& test_mask)	const;

	void			CreateFromMatrix	(Fmatrix &M,	u32 mask);

	u32				getMask				() const { return (1<<p_count)-1; }
};

//////////////////////////////////////////////////////////////////////////
// definition (Detail Model)

class IRender_DetailModel
{
public:
	struct fvfVertexIn	{
		Fvector P;
		float	u,v;
	};
	struct fvfVertexOut	{
		Fvector P;
		u32		C;
		float	u,v;
	};
public:
	Fsphere		bv_sphere;
	Fbox		bv_bb;
	Flags32		m_Flags;
	float		m_fMinScale;
	float		m_fMaxScale;

	ref_shader	shader;
	fvfVertexIn	*vertices;
	u32			number_vertices;     
	u16			*indices;
	u32			number_indices;
public:
	virtual void					transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset)	= 0;
	virtual void					transfer	(Fmatrix& mXform, fvfVertexOut* vDest, u32 C, u16* iDest, u32 iOffset, float du, float dv)	= 0;
	virtual ~IRender_DetailModel()	{};
};

//////////////////////////////////////////////////////////////////////////
// definition (Glow)

struct IRender_GlowVtbl
{
  void (__thiscall *set_active)(IRender_Glow *_this, bool);
  bool (__thiscall *get_active)(IRender_Glow *_this);
  void (__thiscall *set_position)(IRender_Glow *_this, _vector3<float> *);
  void (__thiscall *set_direction)(IRender_Glow *_this, _vector3<float> *);
  void (__thiscall *set_radius)(IRender_Glow *_this, float);
  void (__thiscall *set_texture)(IRender_Glow *_this, const char *);
  void (__thiscall *set_color)(IRender_Glow *_this, float, float, float);
  void (__thiscall *_set_color)(IRender_Glow *_this, _color<float> *);
  void *(__thiscall *__vecDelDtor)(IRender_Glow *_this, unsigned int);
};

struct IRender_Glow :public xr_resource
{
  IRender_GlowVtbl *vfptr;
};

class	CPortal;
class	CSector;

struct	_scissor					: public Fbox2
{
	float	depth;
};

// Connector
class	CPortal						: public IRender_Portal
#ifdef DEBUG
	, public pureRender
#endif
{
private:
	svector<Fvector,8>				poly;
	CSector							*pFace,*pBack;
public:
	Fplane							P;
	Fsphere							S;
	u32								marker;
	BOOL							bDualRender;

	void							Setup								(Fvector* V, int vcnt, CSector* face, CSector* back);

	svector<Fvector,8>&				getPoly()							{ return poly;		}
	CSector*						Back()								{ return pBack;		}
	CSector*						Front()								{ return pFace;		}
	CSector*						getSector		(CSector* pFrom)	{ return pFrom==pFace?pBack:pFace; }
//	CSector*						getSectorFacing	(const Fvector& V)	{ if (P.classify(V)>0) return pFace; else return pBack; }
//	CSector*						getSectorBack	(const Fvector& V)	{ if (P.classify(V)>0) return pBack; else return pFace;	}
//	float							distance		(const Fvector &V)	{ return _abs(P.classify(V)); }

/*									CPortal			();
	virtual							~CPortal		();

#ifdef DEBUG
	virtual void					OnRender		();
#endif*/
};

// Main 'Sector' class
class	 CSector					: public IRender_Sector
{
protected:
	IRender_Visual*					m_root;			// whole geometry of that sector
	xr_vector<CPortal*>				m_portals;
public:
	xr_vector<CFrustum>				r_frustums;
	xr_vector<_scissor>				r_scissors;
	_scissor						r_scissor_merged;
	u32								r_marker;
public:
	// Main interface
	IRender_Visual*					root			()				{ return m_root; }
/*	void							traverse		(CFrustum& F,	_scissor& R);
	void							load			(IReader& fs);

	CSector							()				{ m_root = NULL;	}
	virtual							~CSector		( );*/
};

class	CPortalTraverser
{
public:
	enum
	{
		VQ_HOM		= (1<<0),
		VQ_SSA		= (1<<1),
		VQ_SCISSOR	= (1<<2),
		VQ_FADE		= (1<<3),				// requires SSA to work
	};
public:
	u32										i_marker;		// input
	u32										i_options;		// input:	culling options
	Fvector									i_vBase;		// input:	"view" point
	Fmatrix									i_mXFORM;		// input:	4x4 xform
	Fmatrix									i_mXFORM_01;	// 
	CSector*								i_start;		// input:	starting point
	xr_vector<IRender_Sector*>				r_sectors;		// result
	xr_vector<std::pair<CPortal*, float> >	f_portals;		// 
	ref_shader								f_shader;
	ref_geom								f_geom;
public:
	void							initialize			();
	void							traverse			(IRender_Sector* start, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options);
};

extern __declspec(dllimport) CPortalTraverser			PortalTraverser	;
