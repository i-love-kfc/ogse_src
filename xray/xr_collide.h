#pragma once

#include "xrCommon.h"
//#include "object.h"

class CObject;
namespace collide
{
	enum rq_target
	{
		rqtNone = 0x0,
		rqtObject = 0x1,
		rqtStatic = 0x2,
		rqtShape = 0x4,
		rqtObstacle = 0x8,
		rqtBoth = 0x3,
		rqtDyn = 0xD,
	};
	struct rq_result
	{
		CObject *O;
		float range;
		int element;
	};
	typedef xr_vector<rq_result> results_vector;
	class rq_results
	{
	public:
		results_vector results;
		void r_clear()	
		{ 
			results.clear_not_free();	
		}
	};
	struct ray_cache
	{
		_vector3<float> start;
		_vector3<float> dir;
		float range;
		int result;
		_vector3<float> verts[3];
		void set(const Fvector& _start, const Fvector& _dir, const float _range, const BOOL _result)
		{
			start	= _start;
			dir		= _dir;
			range	= _range;
			result	= _result;
		}
		bool similar(const Fvector& _start, const Fvector& _dir, const float _range)
		{
			if (!_start.similar(start))					return false;
			if (!fsimilar(1.f, dir.dotproduct(_dir)))	return false;
			if (!fsimilar(_range, range))				return false;
			return true;
		}
	};
	/*const */struct ray_defs
	{
		_vector3<float> start;
		_vector3<float> dir;
		float range;
		unsigned int flags;
		rq_target tgt;
		ray_defs(const Fvector& _start, const Fvector& _dir, float _range, u32 _flags, rq_target _tgt)
			: start(_start)
			, dir  (_dir  )
			, range(_range)
			, flags(_flags)
			, tgt  (_tgt  )
		{
		}
	};

	typedef  BOOL rq_callback(rq_result& result, void* user_data);
	typedef  BOOL test_callback(const ray_defs& rd, CObject* object, void* user_data);
};

