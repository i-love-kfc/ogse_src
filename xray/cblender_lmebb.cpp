#include "blenders.h"
#include "uber_deffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_LmEbB::CBlender_LmEbB	()
{
	description.CLS		= B_LmEbB;
	description.version	= 0x1;
	strcpy_s				(oT2_Name,	"$null");
	strcpy_s				(oT2_xform,	"$null");
	oBlend.value		= FALSE;
}

CBlender_LmEbB::~CBlender_LmEbB	()
{
}

void	CBlender_LmEbB::Save(	IWriter& fs )
{
	description.version	= 0x1;
	IBlender::Save		(fs);
	xrPWRITE_MARKER		(fs,"Environment map");
	xrPWRITE_PROP		(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP		(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
	xrPWRITE_PROP		(fs,"Alpha-Blend",		xrPID_BOOL,		oBlend);
}

void	CBlender_LmEbB::Load(	IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
	if (version>=0x1)	{
		xrPREAD_PROP	(fs,xrPID_BOOL,	oBlend);
	}
}

void	CBlender_LmEbB::Compile(CBlender_Compile& C)
{
/*	if (oBlend.value)	C.r_Pass	("lmapE","lmapE",TRUE,TRUE,TRUE*//*FALSE*//*,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,0);
	else				C.r_Pass	("lmapE","lmapE",TRUE);
	C.r_Sampler			("s_base",	C.L_textures[0]	);
//	C.r_Sampler			("s_lmap",	C.L_textures[1]	);
	C.r_Sampler			("s_lmap",	r2_sunmask		);
//	C.r_Sampler_clf		("s_hemi",	*C.L_textures[2]);
	C.r_Sampler_clf		("s_smap",	r2_RT_smap_depth);
	C.r_Sampler			("s_env",	oT2_Name,		false,D3DTADDRESS_CLAMP);
	C.r_End				();*/

	if (oBlend.value)	
	{
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ:
			C.SetParams			(3,true);
			uber_forward		(C,true,"base","base",0,true,0,true);
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
			uber_forward		(C,false,"base","base",0,true,0,true);
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
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,FALSE,220);
			else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		default:
			break;
		}
	} else {
		C.SetParams				(1,false);	//.

		// codepath is the same, only the shaders differ
		// ***only pixel shaders differ***
		switch(C.iElement) 
		{
		case SE_R2_NORMAL_HQ: 	// deffer
			uber_deffer		(C,true,"base","base",false);
			break;
		case SE_R2_NORMAL_LQ: 	// deffer
			uber_deffer		(C,false,"base","base",false);
			break;
		case SE_R2_SHADOW:		// smap
			if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE,TRUE,TRUE,FALSE,D3DBLEND_ZERO,D3DBLEND_ONE,FALSE,220);
			else							C.r_Pass	("shadow_direct_base_aref","shadow_direct_base_aref",FALSE);
			C.r_Sampler		("s_base",C.L_textures[0]);
			C.r_End			();
			break;
		}
	}
}