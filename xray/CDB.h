#pragma once

#include "xrCommon.h"

namespace Opcode {
	struct OPCODE_Model;
};

namespace CDB
{
	enum 
	{
		OPT_CULL				= (1<<0),
		OPT_ONLYFIRST			= (1<<1),
		OPT_ONLYNEAREST			= (1<<2),
		OPT_FULL_TEST			= (1<<3),
	};


	class TRI
	{
	public:
		unsigned int verts[3];
		union 
		{
			struct 
			{
				DWORD material:14; 	// bit offset: 0C, len=14
				DWORD suppress_shadows:1; 	// bit offset: 0C, len=1
				DWORD suppress_wm:1; 	// bit offset: 0C, len=1
				DWORD sector:16; 	// bit offset: 0C, len=16
			};
			int dummy;
		};
		CDB::TRI&  operator=(CDB::TRI const &);
	};

	class MODEL
	{
	public:
		xrCriticalSection cs;
		struct Opcode::OPCODE_Model *tree;
		unsigned int status;
		CDB::TRI *tris;
		int tris_count;
		_vector3<float> *verts;
		int verts_count;

		IC	CDB::TRI*	get_tris() {return tris;};
		IC	Fvector*	get_verts() {return verts;};
		void		build(Fvector* V, int Vcnt, class CDB::TRI *T, int Tcnt, void (__stdcall *bc)(struct _vector3<float> *, int, class CDB::TRI *, int, void *) = NULL, void *bcp = NULL);
		MODEL();
		~MODEL();

		CDB::MODEL&  operator=(CDB::MODEL const &);

	};

	struct RESULT
	{
	public:
		_vector3<float> verts[3];
		unsigned int dummy;
		int id;
		float range;
		float u;
		float v;

		CDB::RESULT&  operator=(CDB::RESULT const &);
	};

	class COLLIDER
	{
	public:
		u32 ray_mode, box_mode, frustum_mode;
		xr_vector<CDB::RESULT> rd;

		IC	void		ray_options		(u32 f)	{	ray_mode = f;	};
		void			ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
		IC	void		box_options		(u32 f)	{	box_mode = f;	};
		void			box_query		(CDB::MODEL const *m_def, Fvector const &b_center, Fvector const &b_dim);
		IC CDB::RESULT* r_begin			()	{	return &(*(rd.begin()));		}
		IC int			r_count			()	{	return rd.size();		};
		COLLIDER();
		COLLIDER(COLLIDER const &);
		~COLLIDER();
		CDB::COLLIDER* operator=(CDB::COLLIDER *__that);
	};

	class Collector
	{
	public:
	  xr_vector<_vector3<float>> verts;
	  xr_vector<CDB::TRI> faces;

	  u32 getVS();
	  Fvector* getV();
	  u32 getTS();
	  CDB::TRI * getT();
	  void add_face_packed_D(Fvector const & v0, Fvector const & v1, Fvector const & v2, u32 limit, float eps = 0.0001);
		Collector();
		Collector(Collector const &);
		~Collector();
		CDB::Collector* operator=(CDB::Collector *__that);
	};

	//-- Ray-Triangle : 2nd level of indirection --------------------------------
	IC bool TestRayTri(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(*p[1], *p[0]);
		edge2.sub(*p[2], *p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0 || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0 || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, *p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}
	//-- Ray-Triangle : 1st level of indirection --------------------------------
	IC bool TestRayTri(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det;
		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);
		if (bCull){						// define TEST_CULL if culling is desired
			if (det < EPS)  return false;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec);			// calculate U parameter and test bounds
			if (u < 0.0f || u > det) return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec);			// calculate V parameter and test bounds
			if (v < 0.0f || u + v > det) return false;
			range = edge2.dotproduct(qvec);		// calculate t, scale parameters, ray intersects triangle
			inv_det = 1.0f / det;
			range *= inv_det;
			u *= inv_det;
			v *= inv_det;
		}else{											// the non-culling branch
			if (det > -EPS && det < EPS) return false;
			inv_det = 1.0f / det;
			tvec.sub(C, p[0]);							// calculate distance from vert0 to ray origin
			u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
			if (u < 0.0f || u > 1.0f)    return false;
			qvec.crossproduct(tvec, edge1);				// prepare to test V parameter
			v = D.dotproduct(qvec)*inv_det;	// calculate V parameter and test bounds
			if (v < 0.0f || u + v > 1.0f) return false;
			range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects triangle
		}
		return true;
	}

	//-- Ray-Triangle(always return range) : 1st level of indirection --------------------------------
	IC bool TestRayTri2(const Fvector& C, const Fvector& D, Fvector* p, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det,u,v;

		// find vectors for two edges sharing vert0
		edge1.sub(p[1], p[0]);
		edge2.sub(p[2], p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);

		if (_abs(det) < EPS_S)		{ range=-1; return false; }
		inv_det = 1.0f / det;
		tvec.sub(C, p[0]);					// calculate distance from vert0 to ray origin
		u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
		qvec.crossproduct(tvec, edge1);		// prepare to test V parameter
		range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects plane
		if (u < 0.0f || u > 1.0f)		return false;
		v = D.dotproduct(qvec)*inv_det;		// calculate V parameter and test bounds
		if (v < 0.0f || u + v > 1.0f) return false;
		return true;
	}
	IC bool TestRayTri2(const Fvector& C, const Fvector& D, Fvector** p, float& range)
	{
		Fvector edge1, edge2, tvec, pvec, qvec;
		float det,inv_det,u,v;

		// find vectors for two edges sharing vert0
		edge1.sub(*p[1], *p[0]);
		edge2.sub(*p[2], *p[0]);
		// begin calculating determinant - also used to calculate U parameter
		pvec.crossproduct(D, edge2);
		// if determinant is near zero, ray lies in plane of triangle
		det = edge1.dotproduct(pvec);

		if (_abs(det) < EPS_S)		{ range=-1; return false; }
		inv_det = 1.0f / det;
		tvec.sub(C, *p[0]);					// calculate distance from vert0 to ray origin
		u = tvec.dotproduct(pvec)*inv_det;	// calculate U parameter and test bounds
		qvec.crossproduct(tvec, edge1);		// prepare to test V parameter
		range = edge2.dotproduct(qvec)*inv_det;// calculate t, ray intersects plane
		if (u < 0.0f || u > 1.0f)		return false;
		v = D.dotproduct(qvec)*inv_det;		// calculate V parameter and test bounds
		if (v < 0.0f || u + v > 1.0f) return false;
		return true;
	}
};