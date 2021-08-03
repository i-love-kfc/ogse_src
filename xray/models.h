#pragma once

#include "xrCommon.h"
#include "kinematics.h"

#define	MAX_PARTS			u32(4)

//*** Key frame definition ************************************************************************
enum{
    flTKeyPresent 	= (1<<0),
    flRKeyAbsent 	= (1<<1),
    flTKey16IsBit 	= (1<<2),
};
#pragma pack(push,2)/*
struct CKey
{
	Fquaternion	Q;			// rotation
	Fvector		T;			// translation
};*/
struct CKeyQR
{
	s16			x,y,z,w;	// rotation
};
struct CKeyQT8
{
	s8			x1,y1,z1;
};
struct CKeyQT16
{
	s16			x1,y1,z1;
};
#pragma pack(pop)
//*** Motion Data *********************************************************************************
class CMotion
{
	struct{
    	u32				_flags	: 8;
		u32				_count 	: 24;
    };
public:
    ref_smem<CKeyQR>	_keysR;
    ref_smem<CKeyQT8>	_keysT8;
    ref_smem<CKeyQT16>	_keysT16;
	Fvector				_initT;
    Fvector				_sizeT;
};
class motion_marks
{
public:
	typedef					std::pair<  float, float > 				interval;
private:
	typedef xr_vector< interval >									STORAGE;
	typedef STORAGE::iterator										ITERATOR;
	typedef STORAGE::const_iterator									C_ITERATOR;

	STORAGE			intervals;	
public:
	shared_str		name;
};

const float	fQuantizerRangeExt	= 1.5f;
class CMotionDef
{
public:
    u16						bone_or_part;
	u16						motion;
	u16						speed;				// quantized: 0..10
	u16						power;				// quantized: 0..10
	u16						accrue;				// quantized: 0..10
	u16						falloff;			// quantized: 0..10
    u16						flags;
	template class std::vector<motion_marks, xalloc<motion_marks>>;
	xr_vector<motion_marks>	marks;
};

class CPartDef
{
public:
	shared_str			Name;
	xr_vector<u32>		bones;
};

class CPartition
{
	CPartDef			P[MAX_PARTS];
};
struct accel_str_pred : public std::binary_function<shared_str, shared_str, bool>	{	
	IC bool operator()(const shared_str& x, const shared_str& y) const	{	return xr_strcmp(x,y)<0;	}
};
template class std::map<shared_str,unsigned short,accel_str_pred,xalloc<std::pair<shared_str,unsigned short> > >;
typedef xr_map<shared_str,u16,accel_str_pred> 	accel_map;
template class std::vector<CMotion, xalloc<CMotion>>;
DEFINE_VECTOR			(CMotionDef,MotionDefVec,MotionDefVecIt);
DEFINE_VECTOR			(CMotion,MotionVec,MotionVecIt);
DEFINE_VECTOR			(MotionVec*,BoneMotionsVec,BoneMotionsVecIt);
DEFINE_MAP				(shared_str,MotionVec,BoneMotionMap,BoneMotionMapIt);
struct motions_value
{
	accel_map			m_motion_map;		// motion associations
	accel_map			m_cycle;			// motion data itself	(shared)
	accel_map			m_fx;				// motion data itself	(shared)
	CPartition			m_partition;		// partition
	u32					m_dwReference;
	BoneMotionMap		m_motions;
    MotionDefVec		m_mdefs;

	shared_str			m_id;
};

class motions_container
{
	DEFINE_MAP			(shared_str,motions_value*,SharedMotionsMap,SharedMotionsMapIt);
	SharedMotionsMap	container;
public:
						motions_container	();
						~motions_container	();
/*	bool				has					(shared_str key);
	motions_value*		dock				(shared_str key, IReader *data, vecBones* bones);
	void				dump				();*/
	void				clean				(bool force_destroy);
};

 extern	__declspec(dllimport)	motions_container*	g_pMotionsContainer;

class CModelPool
{
private:
	friend class CRender;

	struct str_pred : public std::binary_function<const shared_str&, const shared_str&, bool> 
	{	
		IC bool operator()(const shared_str& x, const shared_str& y) const
		{	return xr_strcmp(x,y)<0;}
	};
	struct ModelDef
	{
		shared_str			name;
		IRender_Visual*		model;
        u32					refs;
        ModelDef()			{ refs=0;model=0; }
	};
public:
	typedef xr_multimap<shared_str,IRender_Visual*,str_pred>	POOL;
	typedef POOL::iterator										POOL_IT;
	typedef xr_map<IRender_Visual*,shared_str>					REGISTRY;
	typedef REGISTRY::iterator									REGISTRY_IT;
	xr_vector<ModelDef>			Models;				// Reference / Base
	xr_vector<IRender_Visual*>	ModelsToDelete;		// 
	REGISTRY					Registry;			// Just pairing of pointer / Name
	POOL						Pool;				// Unused / Inactive
	BOOL						bLogging;
private:
    BOOL						bForceDiscard;
    BOOL						bAllowChildrenDuplicate;
public:
                            CModelPool			();
	virtual 				~CModelPool			();

	IRender_Visual*			Instance_Create		(u32 Type);
	void					Discard				(IRender_Visual* &V, BOOL b_complete);
	void					DeleteInternal		(IRender_Visual* &V, BOOL bDiscard);
private:
	void					Destroy				();

	// wrappers
	void	__thiscall		w_Destroy				();

//	void					dump 				();
};
inline void CModelPool::w_Destroy				()
{
	__asm
	{
		mov		eax, ecx;
		call	CModelPool::Destroy;
	};
}
