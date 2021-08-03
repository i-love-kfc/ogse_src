#include "occRasterizer.h"
#include "r2.h"
/*
IC	BOOL			test_Level	(occD* depth, int dim, float _x0, float _y0, float _x1, float _y1, occD z)
{
	int x0		= iFloor	(_x0*dim+.5f);	clamp(x0,0,		dim-1);
	int x1		= iFloor	(_x1*dim+.5f);	clamp(x1,x0,	dim-1);
	int y0		= iFloor	(_y0*dim+.5f);	clamp(y0,0,		dim-1);
	int y1		= iFloor	(_y1*dim+.5f);	clamp(y1,y0,	dim-1);
	
	// MT-Sync (delayed as possible)
	RImplementation.HOM.MT_SYNC	();

	for (int y=y0; y<=y1; y++)
	{
		occD* base	= depth+y*dim;
		occD* it	= base + x0;
		occD* end	= base + x1;
		for (; it<=end; it++)
			if (z<*it)	return TRUE;
	}
	return FALSE;
}

BOOL occRasterizer::test		(float _x0, float _y0, float _x1, float _y1, float _z)
{ 
	occD	z	= df_2_s32up	(_z)+1;
	return		test_Level		(get_depth_level(0),occ_dim_0,_x0,_y0,_x1,_y1,z);
	/*
	if	(test_Level(get_depth_level(2),occ_dim_2,_x0,_y0,_x1,_y1,z))
	{
		// Visbible on level 2 - test level 0
		return test_Level(get_depth_level(0),occ_dim_0,_x0,_y0,_x1,_y1,z);
	}
	return FALSE;
	*/
//}