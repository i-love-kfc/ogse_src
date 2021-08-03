#pragma once
#include "Render.h"
#include "renderable.h"
#include "DLL_Pure.h"
#include "xrPool.h"
#include "feel.h"

struct CWeapon;
class ISpatial;
class IRender_Sector;

enum
{
	STYPE_RENDERABLE			= (1<<0),
	STYPE_LIGHTSOURCE			= (1<<1),
	STYPE_COLLIDEABLE			= (1<<2),
	STYPE_VISIBLEFORAI			= (1<<3),
	STYPE_REACTTOSOUND			= (1<<4),
	STYPE_PHYSIC				= (1<<5),
	STYPE_OBSTACLE				= (1<<6),
	STYPE_SHAPE					= (1<<7),

	STYPEFLAG_INVALIDSECTOR		= (1<<16)
};

struct ISpatial_NODE
{
	ISpatial_NODE *parent;
	ISpatial_NODE *children[8];
	xr_vector<ISpatial * > items;
};
struct ISpatialVtbl
{
	void (__thiscall *spatial_register)(ISpatial *_this);
  void (__thiscall *spatial_unregister)(ISpatial *_this);
  void (__thiscall *spatial_move)(ISpatial *_this);
  _vector3<float> *(__thiscall *spatial_sector_point)(ISpatial *_this, _vector3<float> *result);
  CObject *(__thiscall *dcast_CObject)(ISpatial *_this);
  class Feel::Sound *(__thiscall *dcast_FeelSound)(ISpatial *_this);
  IRenderable *(__thiscall *dcast_Renderable)(ISpatial *_this);
  IRender_Light *(__thiscall *dcast_Light)(ISpatial *_this);
  void *(__thiscall *__vecDelDtor)(ISpatial *_this, unsigned int);
};

class ISpatial_DB
{
private:
	xrCriticalSection				cs;
	poolSS<ISpatial_NODE,128>		allocator;
	xr_vector<ISpatial_NODE*>		allocator_pool;
	ISpatial*						rt_insert_object;
public:
	ISpatial_NODE*					m_root;
	Fvector							m_center;
	float							m_bounds;
	xr_vector<ISpatial*>*			q_result;
	u32								stat_nodes;
	u32								stat_objects;
	CStatTimer						stat_insert;
	CStatTimer						stat_remove;
public:
	// managing
	void							initialize		(Fbox& BB);
	//void							destroy			();
	void							insert			(ISpatial* S);
	void							remove			(ISpatial* S);
	void							update			(u32 nodes=8);
	BOOL							verify			();

public:
	enum
	{
		O_ONLYFIRST		= (1<<0),
		O_ONLYNEAREST	= (1<<1),
		O_ORDERED		= (1<<2),
		O_force_u32		= u32(-1)
	};

	// query
	void							q_ray			(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_and, const Fvector&		_start,  const Fvector&	_dir, float _range);
	void							q_box			(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or,  const Fvector&		_center, const Fvector& _size);
	void							q_sphere		(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or,  const Fvector&		_center, const float _radius);
	void							q_frustum		(xr_vector<ISpatial*>& R, u32 _o, u32 _mask_or,  const CFrustum&	_frustum);
};

extern __declspec(dllimport) ISpatial_DB*		g_SpatialSpace			;
extern __declspec(dllimport) ISpatial_DB*		g_SpatialSpacePhysic	;
//////////////////////////////////////////////////////////////////////////
class ISpatial
{
public:
	struct	_spatial
	{
		u32						type;
		Fsphere					sphere;
		Fvector					node_center;	// Cached node center for TBV optimization
		float					node_radius;	// Cached node bounds for TBV optimization
		ISpatial_NODE*			node_ptr;		// Cached parent node for "empty-members" optimization
		IRender_Sector*			sector;
		ISpatial_DB*			space;			// allow different spaces

		_spatial() : type(0)	{}				// safe way to enhure type is zero before any contstructors takes place
	}							spatial;
public:
	BOOL						spatial_inside		()			;
				void			spatial_updatesector_internal()	;
public:
	virtual		void			spatial_register	()	;
	virtual		void			spatial_unregister	()	;
	virtual		void			spatial_move		()	;
	virtual		Fvector			spatial_sector_point()	{ return spatial.sphere.P; }
	ICF			void			spatial_updatesector()	{
		if (0== (spatial.type&STYPEFLAG_INVALIDSECTOR))	return;
		spatial_updatesector_internal				()	;
	};

	virtual		CObject*		dcast_CObject		()	{ return 0;	}
	virtual		Feel::Sound*	dcast_FeelSound		()	{ return 0;	}
	virtual		IRenderable*	dcast_Renderable	()	{ return 0;	}
	virtual		IRender_Light*	dcast_Light			()	{ return 0; }

				ISpatial		(ISpatial_DB* space	);
	virtual		~ISpatial		();
};

struct ISheduledVtbl {};
struct ISheduled
{
	ISheduledVtbl *vfptr;
	struct
	{
		int _bf0;
	} shedule;
};

struct CObjectVtbl
{
	char	dummy[8];
	int	(__thiscall *AlwaysTheCrow)(CObject *_this);
	char	dummy2[0x9C];
	CWeapon	*(__thiscall *cast_weapon)(CObject *_this);
};

#pragma pack(push, 1)
class CObject 
/*	: public DLL_Pure
	, public ISpatial
	, public ISheduled
	, public IRenderable
	, public ICollidable*/
{
public:
	CObjectVtbl*	vfptr;
	char	dummy[0x4C];
	struct
	{
		_matrix<float> xform;
		IRender_Visual *visual;
		IRender_ObjectSpecific *pROS;
		int pROS_Allowed;
	}		renderable;
	int dummy2;
	struct 
	{
		ICollisionForm*			model;
	}		collidable;
	union	ObjectProperties
	{
		struct 
		{
			u32	net_ID			:	16;
			u32	bActiveCounter	:	8;
			u32	bEnabled		:	1;
			u32	bVisible		:	1;
			u32	bDestroy		:	1;
			u32	net_Local		:	1;
			u32	net_Ready		:	1;
			u32 net_SV_Update	:	1;
			u32 crow			:	1;
			u32	bPreDestroy		:	1;
		};
		u32	storage;
	};
	struct SavedPosition
	{
		unsigned int dwTime;
		_vector3<float> vPosition;
	};
	CObject::ObjectProperties Props;
	shared_str NameObject;
	shared_str NameSection;
	shared_str NameVisual;
	CObject *Parent;
	svector<CObject::SavedPosition,4> PositionStack;
	unsigned int dwFrame_UpdateCL;
	unsigned int dwFrame_AsCrow;

public:
	IRender_ObjectSpecific*			ROS();//{ return renderable_ROS();			}
	void							MakeMeCrow(void);
	BOOL							AlwaysTheCrow(void);
	void							UpdateCL(void);
	IRender_Visual*					Visual				() const 			{ return renderable.visual;			};
	void							spatial_update(float, float);
	Fvector &						Position(void);
	Fvector const &					Position(void)const;
	void							UpdateWeaponParams();
	CWeapon*						cast_weapon			();
};

extern _vector4<u32> w_states;
extern _vector3<float> w_timers;
#pragma pack(pop)
