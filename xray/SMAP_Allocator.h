#pragma once

#include "xrCommon.h"

struct	SMAP_Rect
{
	Ivector2				min,max;
};

struct	SMAP_Allocator
{
	u32						psize;		// pool size
	xr_vector<SMAP_Rect>	stack;		// 
	xr_vector<Ivector2>		cpoint;		// critical points
};
