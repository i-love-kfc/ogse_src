#pragma once

#include "CDB.h"

class xrXRC
{
private:
  CDB::COLLIDER CL;
public:
	IC void			ray_options		(u32 f)		
	{ 
		CL.ray_options(f); 
	}
	IC void			ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
	IC void			box_options		(u32 f)	
	{	
		CL.box_options(f);
	}
	IC void			box_query		(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
	IC CDB::RESULT*	r_begin			()	{	return CL.r_begin();		};
	IC int			r_count			()	{	return CL.r_count();		};

	xrXRC();
	xrXRC(xrXRC const &);
	~xrXRC();
	xrXRC* operator=(xrXRC *__that);
};
