#pragma once
#include "base_types.h"

enum EJointType
{
	jtRigid = 0x0,
	jtCloth = 0x1,
	jtJoint = 0x2,
	jtWheel = 0x3,
	jtNone = 0x4,
	jtSlider = 0x5,
	jtForceU32 = 0xFFFFFFFF,
};

struct SJointLimit
{
	_vector2<float> limit;
	float spring_factor;
	float damping_factor;
};

struct SJointIKData
{
	EJointType type;
	SJointLimit limits[3];
	float spring_factor;
	float damping_factor;
	_flags<unsigned int> ik_flags;
	float break_force;
	float break_torque;
	float friction;
};

//struct SBoneShape;
//struct Fobb;

class CBoneData;
typedef xr_vector<CBoneData*>		vecBones;
typedef vecBones::iterator			vecBonesIt;
class CBoneInstance
{
public:
//	u32 mem_usage();
};
struct CBoneDataVtbl
{
	void *(__thiscall *__vecDelDtor)(CBoneData *_this, unsigned int);
	unsigned int (__thiscall *mem_usage)(CBoneData *_this);
};

struct SBoneShape
{
	enum EShapeType
	{
		stNone = 0x0,
		stBox = 0x1,
		stSphere = 0x2,
		stCylinder = 0x3,
		stForceU32 = 0xFFFF,
	};
	unsigned __int16 type;
	_flags<unsigned short> flags;
	_obb<float> box;
	_sphere<float> sphere;
	_cylinder<float> cylinder;
};

#pragma pack(push, 1)
class CBoneData
{
public:
	CBoneDataVtbl *vfptr;
	unsigned __int16 SelfID;
	unsigned __int16 ParentID;
	shared_str name;
	xr_vector<CBoneData*> children;
	_obb<float> obb;
	_matrix<float> bind_transform;
	_matrix<float> m2b_transform;
	SBoneShape shape;
	shared_str game_mtl_name;
	unsigned __int16 game_mtl_idx;
	SJointIKData IK_data;
	__declspec(align(4)) float mass;
	_vector3<float> center_of_mass;
	xr_vector<xr_vector<unsigned short> > child_faces;
public:
	void CalculateM2B(const Fmatrix& Parent);
	CBoneData& GetChild(u16 id);
	const CBoneData& GetChild(u16 id) const;
	u16 GetSelfID(void) const;
	u16 GetNumChildren(void) const;

	const SJointIKData& get_IK_data(void) const {return	IK_data;}
	const Fmatrix& get_bind_transform(void) const {return	bind_transform;}
	const SBoneShape& get_shape(void) const {return shape;}
	const Fobb& get_obb(void) const {return obb;}
	const Fvector& get_center_of_mass(void) const {return center_of_mass;}
	float get_mass(void) const {return mass;}
	u16 get_game_mtl_idx(void) const {return game_mtl_idx;}
	u16 GetParentID(void) const {return ParentID;}
	float lo_limit(u8 k) const {return IK_data.limits[k].limit.x;}
	float hi_limit(u8 k) const {return IK_data.limits[k].limit.y;}
//	u32 mem_usage();
};
#pragma pack(pop)

