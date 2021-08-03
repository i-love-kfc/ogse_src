#include "blenders.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_BmmD::CBlender_BmmD	()
{
	description.CLS		= B_BmmD;
	strcpy_s				(oT2_Name,	"$null");
	strcpy_s				(oT2_xform,	"$null");
	description.version	= 3;
	strcpy_s				(oR_Name,	"detail\\detail_grnd_grass");	//"$null");
	strcpy_s				(oG_Name,	"detail\\detail_grnd_asphalt");	//"$null");
	strcpy_s				(oB_Name,	"detail\\detail_grnd_earth");	//"$null");
	strcpy_s				(oA_Name,	"detail\\detail_grnd_yantar");	//"$null");
}

CBlender_BmmD::~CBlender_BmmD	()
{
}

void	CBlender_BmmD::Save		(IWriter& fs )
{
	IBlender::Save	(fs);
	xrPWRITE_MARKER	(fs,"Detail map");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT2_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT2_xform);
	xrPWRITE_PROP	(fs,"R2-R",				xrPID_TEXTURE,	oR_Name);
	xrPWRITE_PROP	(fs,"R2-G",				xrPID_TEXTURE,	oG_Name);
	xrPWRITE_PROP	(fs,"R2-B",				xrPID_TEXTURE,	oB_Name);
	xrPWRITE_PROP	(fs,"R2-A",				xrPID_TEXTURE,	oA_Name);
}

void	CBlender_BmmD::Load		(IReader& fs, u16 version )
{
	IBlender::Load	(fs,version);
	if (version<3)	{
		xrPREAD_MARKER	(fs);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
		xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
	} else {
		xrPREAD_MARKER	(fs);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT2_Name);
		xrPREAD_PROP	(fs,xrPID_MATRIX,	oT2_xform);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oR_Name);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oG_Name);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oB_Name);
		xrPREAD_PROP	(fs,xrPID_TEXTURE,	oA_Name);
	}
}

#include "uber_deffer.h"
void	CBlender_BmmD::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	string256				mask;
	strconcat				(sizeof(mask),mask,C.L_textures[0].c_str(),"_mask");
	switch(C.iElement) 
	{
	case SE_R2_NORMAL_HQ: 		// deffer
		uber_deffer		(C, true,	"impl","impl",false,oT2_Name[0]?oT2_Name:0,true);
		C.r_Sampler		("s_mask",	mask);
		C.r_Sampler		("s_lmap",	C.L_textures[1]);

		C.r_Sampler		("s_dt_r",	oR_Name,	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_dt_g",	oG_Name,	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_dt_b",	oB_Name,	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_dt_a",	oA_Name,	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);

		C.r_Sampler		("s_dn_r",	strconcat(sizeof(mask),mask,oR_Name,"_bump") );
		C.r_Sampler		("s_dn_g",	strconcat(sizeof(mask),mask,oG_Name,"_bump") );
		C.r_Sampler		("s_dn_b",	strconcat(sizeof(mask),mask,oB_Name,"_bump") );
		C.r_Sampler		("s_dn_a",	strconcat(sizeof(mask),mask,oA_Name,"_bump") );

		C.r_End			();
		break;
	case SE_R2_NORMAL_LQ: 		// deffer
		uber_deffer		(C, false,	"base","impl",false,oT2_Name[0]?oT2_Name:0,true);
		C.r_Sampler		("s_mask",	mask);
		C.r_Sampler		("s_lmap",	C.L_textures[1]);
		C.r_End			();
		break;
	case SE_R2_SHADOW:			// smap
		if (RImplementation.o.HW_smap)	C.r_Pass	("shadow_direct_base","dumb",				FALSE,TRUE,TRUE,FALSE);
		else							C.r_Pass	("shadow_direct_base","shadow_direct_base",	FALSE);
		C.r_Sampler		("s_base",	C.L_textures[0]);
		C.r_End			();
		break;
	}
}