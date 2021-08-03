#include "blenders.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model_EbB::CBlender_Model_EbB	()
{
	description.CLS		= B_MODEL_EbB;
	description.version	= 0x1;
	strcpy_s				(oT2_Name,	"$null");
	strcpy_s				(oT2_xform,	"$null");
	oBlend.value		= FALSE;
}

CBlender_Model_EbB::~CBlender_Model_EbB	()
{
	
}

void	CBlender_Model_EbB::Save(	IWriter& fs )
{
	description.version	= 0x1;
	IBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Environment map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
	xrPWRITE_PROP	(fs,"Alpha-Blend",		xrPID_BOOL,		oBlend);
}

void	CBlender_Model_EbB::Load(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
	if (version>=0x1)	{
		xrPREAD_PROP	(fs,xrPID_BOOL,	oBlend);
	}
}

#include "uber_deffer.h"
void	CBlender_Model_EbB::Compile(CBlender_Compile& C)
{
	IBlender::Compile		(C);

	if (oBlend.value)	{
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ:
			C.SetParams			(3,true);
			uber_forward		(C,true,"model","base",0,true,0,true);
			C.r_Sampler			("s_lmap",	r2_sunmask		);
			C.r_Sampler_clf		("s_smap_near",	r2_RT_smap_depth_near);
			C.r_Sampler_clf		("s_smap_far",	r2_RT_smap_depth_far);
			C.r_Sampler_clf		("env_s0",		r2_T_envs0			);
			C.r_Sampler_clf		("env_s1",		r2_T_envs1			);
			C.r_Sampler_clf		("s_sky",		r2_RT_rain			);
			C.r_Sampler_clw		("s_material",		r2_material		);
			C.r_End				();
			break;
		case SE_R2_NORMAL_LQ:
			C.SetParams			(3,true);
			uber_forward		(C,false,"model","base",0,true,0,true);
			C.r_Sampler			("s_lmap",	r2_sunmask		);
			C.r_Sampler_clf		("s_smap_near",	r2_RT_smap_depth_near);
			C.r_Sampler_clf		("s_smap_far",	r2_RT_smap_depth_far);
			C.r_Sampler_clf		("env_s0",		r2_T_envs0			);
			C.r_Sampler_clf		("env_s1",		r2_T_envs1			);
			C.r_Sampler_clf		("s_sky",		r2_RT_rain			);
			C.r_Sampler_clw		("s_material",		r2_material		);
			C.r_End				();
			break;
		case SE_R2_SHADOW:		// smap
			C.SetParams				(1,false);
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,220);
			else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		default:
			break;
		}
	} else {
		// deferred
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ: 	// deffer
			uber_deffer		(C,true,	"model","base",false);
			break;
		case SE_R2_NORMAL_LQ: 	// deffer
			uber_deffer		(C,false,	"model","base",false);
			break;
		case SE_R2_SHADOW:		// smap
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_model","shadow_direct_base",	FALSE,TRUE,TRUE,FALSE);
			else							C.r_Pass	("shadow_direct_model","shadow_direct_base",	FALSE);
			C.r_Sampler		("s_base",		C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}
