#pragma once
#include "xr_collide.h"
#include "base_types.h"

struct ICollisionForm;
struct ICollisionFormVtbl
{
	void *(__thiscall *__vecDelDtor)(ICollisionForm *_this, unsigned int);
	int (__thiscall *_RayQuery)(ICollisionForm *_this, collide::ray_defs *, collide::rq_results *);
};
class CObject;
enum ECollisionFormType
{
	cftObject = 0x0,
	cftShape = 0x1,
};

struct ICollisionForm
{
	ICollisionFormVtbl *vfptr;
	CObject *owner;
	unsigned int dwQueryID;
	_box3<float> bv_box;
	_sphere<float> bv_sphere;
	ECollisionFormType m_type;
};

struct ICollidable;
struct ICollidableVtbl
{
	void *(__thiscall *__vecDelDtor)(ICollidable *_this, unsigned int);
};

struct ICollidable
{
	ICollidableVtbl *vfptr;
	struct 
	{
		ICollisionForm*			model;
	}							collidable;
};
