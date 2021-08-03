#include "uber_deffer.h"
#include "ResourceManager.h"
#include "r2_types.h"

void fix_texture_name(LPSTR fn)
{
	LPSTR _ext = strext(fn);
	if(  _ext					&&
	  (0==_stricmp(_ext,".tga")	||
		0==_stricmp(_ext,".dds")	||
		0==_stricmp(_ext,".bmp")	||
		0==_stricmp(_ext,".ogm")	) )
		*_ext = 0;
}
void uber_deffer	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256		fname,fnameA,fnameB;
	strcpy_s			(fname,*C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create			(fname);
	bool			bump	= _t.bump_exist		();

	// detect lmap
	bool			lmap	= true;
	if	(C.L_textures.size()<3)	lmap = false;
	else {
		const char*		tex		= C.L_textures[2].c_str();
		if (tex[0]=='l' && tex[1]=='m' && tex[2]=='a' && tex[3]=='p')	lmap = true	;
		else															lmap = false;
	}

	string256		ps,vs,dt,dtA,dtB;
	strconcat		(sizeof(vs),vs,"deffer_", _vspec, lmap?"_lmh":""	);
	strconcat		(sizeof(ps),ps,"deffer_", _pspec, lmap?"_lmh":""	);
	strcpy_s		(dt,sizeof(dt),_detail_replace?_detail_replace:( C.detail_texture?C.detail_texture:"" ) );

/*	bool			detail_bump	= false;
	if (dt[0] != 0) {
		fix_texture_name(dt);
		ref_texture		_dt;		_dt.create			(dt);
		detail_bump	= _dt.bump_exist		();
	}*/

	if	(_aref)		{ strcat(ps,"_aref");	}

	if	(!bump)		{
		fnameA[0] = fnameB[0] = 0;
		strcat			(vs,"_flat");
		strcat			(ps,"_flat");
		if (hq && (C.bDetail_Diffuse || (C.bFlags & bDetailBump)) )	{
			strcat		(vs,"_d");
			strcat		(ps,"_d");
		}
	} else {
		strcpy_s		(fnameA,_t.bump_get().c_str());
		strconcat		(sizeof(fnameB),fnameB,fnameA,"#");
		if (C.bFlags & bDetailBump)
		{
			strcpy_s		(dtA,dt);
			strconcat		(sizeof(dtA),dtA,dtA,"_bump");
			strconcat		(sizeof(dtB),dtB,dtA,"#");
		} else {
			dtA[0] = dtB[0] = 0;
		}
		strcat			(vs,"_bump");
		strcat			(ps,"_bump");
		if (hq && (C.bDetail_Diffuse || (C.bFlags & bDetailBump)) )	{
			strcat		(vs,"_d"	);
			strcat		(ps,"_d"	);
		}
	}

	// HQ
	if (bump && hq)		{
		if (C.bFlags & bDetailBump)
			strcat			(ps,"_db");
		if (C.bFlags & bParallax)
			strcat			(ps,"_steep");
		strcat			(vs,"-hq");
		strcat			(ps,"-hq");
	}

/*	Msg("vs: %s\nps: %s\ns_base: %s\ns_bumpX: %s\ns_bump: %s\ns_detail: %s", vs, ps, *C.L_textures[0], fnameB, fnameA, dt);
	if (C.detail_scaler)
		Msg("dt_params: %f", (dynamic_cast<cl_dt_scaler *>(C.detail_scaler))->scale);*/

	// Uber-construct
	C.r_Pass		(vs,ps,	FALSE);
	C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_bumpD",		dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	if (bump && hq && (C.bFlags & bDetailBump)) {
//		Msg("s_detail_bumpX: %s\ns_detail_bump: %s\n", dtB, dtA);
		C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	} else {
//		Msg("\n");
	}
	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
	if (!DO_NOT_FINISH)		C.r_End	();
}

void uber_forward	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, u32 _aref, bool env, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256		fname,fnameA,fnameB;
	strcpy_s			(fname,*C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create			(fname);
	bool			bump	= _t.bump_exist		();

	// detect lmap
	bool			lmap	= true;
	if	(C.L_textures.size()<3)	lmap = false;
	else {
		const char*		tex		= C.L_textures[2].c_str();
		if (tex[0]=='l' && tex[1]=='m' && tex[2]=='a' && tex[3]=='p')	lmap = true	;
		else															lmap = false;
	}

	string256		ps,vs,dt,dtA,dtB;
	strconcat		(sizeof(vs),vs,"forward_", _vspec, lmap?"_lmh":""	);
	strconcat		(sizeof(ps),ps,"forward_", _pspec, lmap?"_lmh":""	);
	strcpy_s		(dt,sizeof(dt),_detail_replace?_detail_replace:( C.detail_texture?C.detail_texture:"" ) );

	if	(!bump)		{
		fnameA[0] = fnameB[0] = 0;
		strcat			(vs,"_flat");
		strcat			(ps,"_flat");
		if (hq && (C.bDetail_Diffuse/* || (C.bFlags & bDetailBump)*/) )	{
			strcat		(vs,"_d");
			strcat		(ps,"_d");
		}
	} else {
		strcpy_s		(fnameA,_t.bump_get().c_str());
		strconcat		(sizeof(fnameB),fnameB,fnameA,"#");
		if (C.bFlags & bDetailBump)
		{
			strcpy_s		(dtA,dt);
			strconcat		(sizeof(dtA),dtA,dtA,"_bump");
			strconcat		(sizeof(dtB),dtB,dtA,"#");
		} else {
			dtA[0] = dtB[0] = 0;
		}
		strcat			(vs,"_bump");
		strcat			(ps,"_bump");
		if (hq && (C.bDetail_Diffuse || (C.bFlags & bDetailBump)) )	{
			strcat		(vs,"_d"	);
			strcat		(ps,"_d"	);
		}
	}

	// HQ
	if (bump && hq)		{
/*		if (C.bFlags & bDetailBump)
			strcat			(ps,"_db");*/
		if (C.bFlags & bParallax)
			strcat			(ps,"_steep");
		strcat			(vs,"-hq");
		strcat			(ps,"-hq");
	}

	// Uber-construct
	if (_aref)
		C.r_Pass		(vs,ps,FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE, _aref);
	else
		C.r_Pass		(vs,ps,FALSE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	FALSE);
	C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
//	C.r_Sampler		("s_bumpD",		dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
/*	if (bump && hq && (C.bFlags & bDetailBump)) {

		C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	}*/
	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
	if (env)
	{
		C.r_Sampler("s_env0",	r2_T_sky0,	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
		C.r_Sampler("s_env1",	r2_T_sky1,	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
	}
	if (!DO_NOT_FINISH)		C.r_End	();
}