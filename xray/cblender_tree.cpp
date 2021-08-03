// Blender_Vertex_aref.cpp: implementation of the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////

#include "blenders.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Tree::CBlender_Tree()
{
	description.CLS		= B_TREE;
	description.version	= 1;
	oBlend.value		= FALSE;
	oNotAnTree.value	= FALSE;
}

CBlender_Tree::~CBlender_Tree()
{

}

void	CBlender_Tree::Save		(IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
	xrPWRITE_PROP		(fs,"Object LOD",	xrPID_BOOL,		oNotAnTree);
}

void	CBlender_Tree::Load		(IReader& fs, u16 version )
{
	IBlender::Load		(fs,version);
	xrPREAD_PROP		(fs,xrPID_BOOL,		oBlend);
	if (version>=1)		{
		xrPREAD_PROP		(fs,xrPID_BOOL,		oNotAnTree);
	}
}

#include "uber_deffer.h"
void	CBlender_Tree::Compile	(CBlender_Compile& C)
{
	IBlender::Compile	(C);

	//*************** codepath is the same, only shaders differ
	LPCSTR	tvs				= "tree";
	LPCSTR	tvs_s			= "shadow_direct_tree";
	if (oNotAnTree.value)	{ tvs="tree_s"; tvs_s="shadow_direct_tree_s"; }
	LPCSTR	tps				= "base";
	if (oBlend.value)		{tps = "tree";};
	switch (C.iElement)
	{
	case SE_R2_NORMAL_HQ:	// deffer
		uber_deffer			(C,true,tvs,tps,oBlend.value);
		break;
	case SE_R2_NORMAL_LQ:	// deffer
		uber_deffer			(C,false,tvs,tps,oBlend.value);
		break;
	case SE_R2_SHADOW:		// smap-spot
		if (oBlend.value)	C.r_Pass	(tvs_s,"shadow_direct_base_aref",	FALSE,TRUE,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_ONE,TRUE,200);
		else				C.r_Pass	(tvs_s,"shadow_direct_base",		FALSE);
		C.r_Sampler			("s_base",	C.L_textures[0]);
		C.r_End				();
		break;
	}
}
