#pragma once

#include "xr_collide.h"
#include "base_types.h"
#include "CDB.h"
#include "xrXRC.h"

class ISpatial;
struct ICollisionForm;

class CObjectSpace
{
	xrCriticalSection Lock;
	CDB::MODEL Static;
	_box3<float> m_BoundingVolume;
	xrXRC xrc;
	collide::rq_results r_temp;
	xr_vector<ISpatial *,xalloc<ISpatial *> > r_spatial;
public:
	CDB::TRI*							GetStaticTris		() { return Static.get_tris();	}
	Fvector*							GetStaticVerts		() { return Static.get_verts(); }
	CDB::MODEL*							GetStaticModel		() { return &Static;			}
	BOOL RayQuery( collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb, CObject* ignore_object);
	BOOL RayQuery( collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq);
	BOOL RayPick ( const Fvector &start, const Fvector &dir, float range, collide::rq_target tgt, collide::rq_result& R, CObject* ignore_object );
};
