#pragma once
#include "blenders.h"
#include "xr_3da.h"

// Writers
void		xrPWRITE		(IWriter& fs, u32 ID, LPCSTR name, LPCVOID data, u32 size )
{
	fs.w_u32			(ID);
	fs.w_stringZ		(name);
	if (data && size)	fs.w(data,size);
}
void		xrPWRITE_MARKER	(IWriter& fs, LPCSTR name)
{
	xrPWRITE	(fs,xrPID_MARKER,name,0,0);
}
// Readers
u32			xrPREAD			(IReader& fs)
{
	u32 T		= fs.r_u32();
	fs.skip_stringZ	();
	return		T;
}
void		xrPREAD_MARKER	(IReader& fs)
{
	R_ASSERT(xrPID_MARKER==xrPREAD(fs));
}

IBlender::IBlender()
{
	oPriority.min	= 0;
	oPriority.max	= 3;
	oPriority.value	= 1;
	strcpy_s			(oT_Name,	"$base0");
	strcpy_s			(oT_xform,	"$null");
}

IBlender::~IBlender()
{

}

void	IBlender::Save(IWriter& fs )
{
	fs.w			(&description,sizeof(description));
	xrPWRITE_MARKER (fs,"General");
	xrPWRITE_PROP	(fs,"Priority",			xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	(fs,"Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	(fs,"Base Texture");
	xrPWRITE_PROP	(fs,"Name",				xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	(fs,"Transform",		xrPID_MATRIX,	oT_xform);
}

void	IBlender::Load(	IReader& fs, u16  )
{
	// Read desc and doesn't change version
	u16	V		= description.version;
	fs.r			(&description,sizeof(description));
	description.version	= V;

	// Properties
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_INTEGER,	oPriority);
	xrPREAD_PROP	(fs,xrPID_BOOL,		oStrictSorting);
	xrPREAD_MARKER	(fs);
	xrPREAD_PROP	(fs,xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP	(fs,xrPID_MATRIX,	oT_xform);
}

void	IBlender::Compile(CBlender_Compile& C)
{
	if (C.bEditor)	C.SetParams	(oPriority.value,oStrictSorting.value?true:false);
	else			C.SetParams	(oPriority.value,oStrictSorting.value?true:false);
}