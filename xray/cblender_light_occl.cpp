#pragma once

#include "blenders.h"
//////////////////////////////////////////////
//////////* CBlender_light_occq */////////////
//////////////////////////////////////////////
CBlender_light_occq::CBlender_light_occq	()	{	description.CLS		= 0;	}
void CBlender_light_occq::Compile (CBlender_Compile& C)
{
	IBlender::Compile	(C);

	switch (C.iElement) 
	{
	case 0:			// occlusion testing
		C.r_Pass	("dumb", "dumb",false,TRUE,FALSE,FALSE);
		C.r_End		();
		break;
	case 1:			// NV40 optimization :)
		C.r_Pass	("null", "dumb",false,FALSE,FALSE,FALSE);
		C.r_End		();
		break;
	}
}