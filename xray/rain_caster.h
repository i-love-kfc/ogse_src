#pragma once

#include "xrCommon.h"

class	r_caster
{
public:
	Fvector			position	;
	Fvector			direction	;

	struct		_D		{
		Fmatrix						combine	;
		s32							minX,maxX	;
		s32							minY,maxY	;
		BOOL						transluent;
	}	D;

public:
	IC void	set_position			(const Fvector& P)
	{
		position = P;
	}
	IC void	set_direction			(const Fvector& D)
	{
		direction = D;
	}
};
