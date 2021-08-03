#pragma once
#include "xr_3da.h"
#include "r2.h"
#include "blenders_clsids.h"

// Writers
void		xrPWRITE		(IWriter& fs, u32 ID, LPCSTR name, LPCVOID data, u32 size );
void		xrPWRITE_MARKER	(IWriter& fs, LPCSTR name);
#define xrPWRITE_PROP(FS,name,ID,data)\
{\
	xrPWRITE	(fs,ID,name,&data,sizeof(data));\
}

// Readers
u32			xrPREAD			(IReader& fs);
void		xrPREAD_MARKER	(IReader& fs);
#define xrPREAD_PROP(fs,ID,data) \
{ \
	R_ASSERT(ID==xrPREAD(fs)); fs.r(&data,sizeof(data)); \
	switch (ID) \
	{ \
	case xrPID_TOKEN:	fs.advance(((xrP_TOKEN*)&data)->Count * sizeof(xrP_TOKEN::Item));	break; \
	case xrPID_CLSID:	fs.advance(((xrP_CLSID*)&data)->Count * sizeof(CLASS_ID));			break; \
	}; \
}

#pragma pack(push, 4)
// fullscreen quad blenders
class CBlender_light_occq	: public IBlender  
{
public:
	LPCSTR		getComment()	{ return "INTERNAL: occlusion testing";	}
	BOOL		canBeDetailed()	{ return FALSE;	}
	BOOL		canBeLMAPped()	{ return FALSE;	}
	void		Compile			(CBlender_Compile& C);
	CBlender_light_occq();
};
class CBlender_accum_direct_mask : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: mask direct light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_accum_direct_mask();
	virtual ~CBlender_accum_direct_mask();
};
class CBlender_accum_direct : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: accumulate direct light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_accum_direct();
	virtual ~CBlender_accum_direct();
};
class CBlender_accum_point : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: accumulate point light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_accum_point();
	virtual ~CBlender_accum_point();
};
class CBlender_accum_spot : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: accumulate spot light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_accum_spot();
	virtual ~CBlender_accum_spot();
};
class CBlender_accum_reflected : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: accumulate reflected light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_accum_reflected();
	virtual ~CBlender_accum_reflected();
};
class CBlender_bloom_build : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: combine to bloom target";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_bloom_build();
	virtual ~CBlender_bloom_build();
};
class CBlender_luminance	: public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: luminance estimate";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_luminance();
	virtual ~CBlender_luminance();
};
class CBlender_combine : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: combiner";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_combine();
	virtual ~CBlender_combine();
};
// standard shading templates
class CBlender_BmmD : public IBlender  
{
public:
	string64	oT2_Name;	// name of secondary texture
	string64	oT2_xform;	// xform for secondary texture
	string64	oR_Name	;	//. задел на будущее
	string64	oG_Name	;	//. задел на будущее
	string64	oB_Name	;	//. задел на будущее
	string64	oA_Name	;	//. задел на будущее
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: Implicit**detail";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE; }
	virtual		BOOL		canBeLMAPped()	{ return TRUE; }

	virtual		void		Save			( IWriter&	fs);
	virtual		void		Load			( IReader&	fs, u16 version);

	virtual		void		Compile			( CBlender_Compile& C);

	CBlender_BmmD();
	virtual ~CBlender_BmmD();
};
class CBlender_Detail_Still : public IBlender  
{
public:
	xrP_BOOL	oBlend;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: detail objects";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Detail_Still();
	virtual ~CBlender_Detail_Still();
};
class CBlender_Editor_Selection : public IBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "EDITOR: selection"; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	
	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Editor_Selection();
	virtual ~CBlender_Editor_Selection();

};
class CBlender_Editor_Wire : public IBlender  
{
	string64	oT_Factor;
public:
	virtual		LPCSTR		getComment()	{ return "EDITOR: wire";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Editor_Wire();
	virtual ~CBlender_Editor_Wire();
};
class CBlender_LmEbB : public IBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
	xrP_BOOL	oBlend;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: lmap*(env^base)";	}
	virtual		BOOL		canBeLMAPped()	{ return TRUE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_LmEbB();
	virtual ~CBlender_LmEbB();
};
class CBlender_Model_EbB : public IBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
	xrP_BOOL	oBlend;
public:
	virtual		LPCSTR		getComment()	{ return "MODEL: env^base";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, u16 version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Model_EbB();
	virtual ~CBlender_Model_EbB();
};
class CBlender_Particle		: public IBlender  
{
	xrP_TOKEN	oBlend;
	xrP_Integer	oAREF;
	xrP_BOOL	oClamp;
public:
	virtual		LPCSTR		getComment()	{ return "particles";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;			}
	
	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Particle();
	virtual ~CBlender_Particle();
};
class CBlender_Screen_SET : public IBlender  
{
	xrP_TOKEN	oBlend;
	xrP_Integer	oAREF;
	xrP_BOOL	oZTest;
	xrP_BOOL	oZWrite;
	xrP_BOOL	oLighting;
	xrP_BOOL	oFog;
	xrP_BOOL	oClamp;
public:
	virtual		LPCSTR		getComment()	{ return "basic (simple)"; }
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	
	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	
	virtual		void		Compile			(CBlender_Compile& C);
	
	CBlender_Screen_SET();
	virtual ~CBlender_Screen_SET();
};
class CBlender_Tree : public IBlender  
{
public:
	xrP_BOOL	oBlend;
	xrP_BOOL	oNotAnTree;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: trees/bushes";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }
	virtual		BOOL		canBeDetailed()	{ return TRUE; }

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, u16 version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Tree();
	virtual ~CBlender_Tree();
};

class CBlender_deffer_aref : public IBlender  
{
public:
	xrP_Integer	oAREF;
	xrP_BOOL	oBlend;
	bool		lmapped;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: defer-base-aref";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE;		}
	virtual		BOOL		canBeLMAPped()	{ return lmapped;	}

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_aref					(bool _lmapped=false);
	virtual ~CBlender_deffer_aref			();
};
class CBlender_deffer_flat : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: defer-base-normal";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_flat();
	virtual ~CBlender_deffer_flat();
};
class CBlender_deffer_model : public IBlender  
{
public:
	xrP_Integer	oAREF;
	xrP_BOOL	oBlend;
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: deffer-model-flat";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Save			(IWriter&	fs);
	virtual		void		Load			(IReader&	fs, u16 version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_deffer_model();
	virtual ~CBlender_deffer_model();
};



// KD
class CBlender_ikvision : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: ikvision";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ikvision();
	virtual ~CBlender_ikvision();
};
class CBlender_sunshafts : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: sunshafts";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_sunshafts();
	virtual ~CBlender_sunshafts();
};
class CBlender_mblur : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: motion blur";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_mblur();
	virtual ~CBlender_mblur();
};
class CBlender_dof : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: depth of field";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_dof();
	virtual ~CBlender_dof();
};
class CBlender_rain_drops : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: rain drops";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_rain_drops();
	virtual ~CBlender_rain_drops();
};
class CBlender_ssao : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "OGSE: screen space ambient occlusion";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_ssao();
	virtual ~CBlender_ssao();
};

class CBlender_aa : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: antialiasing";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_aa();
	virtual ~CBlender_aa();
};
class CBlender_fog_volume : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: fog volumes";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_fog_volume();
	virtual ~CBlender_fog_volume();
};
class CBlender_flare : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: flare";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_flare();
	virtual ~CBlender_flare();
};
class CBlender_rmap : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: rain map";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_rmap();
	virtual ~CBlender_rmap();
};
class CBlender_blur : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: blur";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_blur();
	virtual ~CBlender_blur();
};
class CBlender_reflections : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: reflections";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_reflections();
	virtual ~CBlender_reflections();
};

class CBlender_volumetric : public IBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: accumulate volumetric light";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_volumetric();
	virtual ~CBlender_volumetric();
};

#pragma pack(pop)
inline void	jitter(CBlender_Compile& C)
{
	C.r_Sampler	("jitter0",	JITTER(0), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter1",	JITTER(1), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter2",	JITTER(2), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("jitter3",	JITTER(3), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
	C.r_Sampler	("s_noise",	JITTER(5), true, D3DTADDRESS_WRAP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
}

