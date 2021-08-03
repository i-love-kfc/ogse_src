#pragma once
//#include <vector>
//using std::vector;
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "xrCommon.h"
#include "Environment.h"
#include "Backend.h"

struct CBlender_DESC
{
  unsigned __int64 CLS;
  char cName[128];
  char cComputer[32];
  unsigned int cTime;
  unsigned __int16 version;
};
/*
struct IReaderBaseVbl
{
	void *(__thiscall *__vecDelDtor)(IReader *_this, unsigned int);
};
template<class T>
class IReaderBase
{
public:
	IReaderBaseVbl*	vfptr;
	float		r_float		();
	u16			r_u16		();
	u32			r_u32		();//			{	u32 tmp;	r(&tmp,sizeof(tmp)); return tmp;	};
	BOOL		r_chunk_safe(u32 ID, void *dest, u32 dest_size);
	u32 		find_chunk	(u32 ID, BOOL* bCompressed = 0);
};
class IReader :public IReaderBase<IReader>
{
  //char *data;
  //int Pos;
  //int Size;
  //int iterpos;
public:
	void		r			(void *p,int cnt);
	IReader*	open_chunk	(u32 ID);
	void		close		();
	void		advance		(int cnt);
	void*		pointer		()	const;
	void		skip_stringZ();
	void		r_stringZ	(char *dest, u32 tgt_sz);
	int			length		()	const;
};
class CStreamReader: public IReaderBase<CStreamReader>
{
/*  void *m_file_mapping_handle;
  unsigned int m_start_offset;
  unsigned int m_file_size;
  unsigned int m_archive_size;
  unsigned int m_window_size;
  unsigned int m_current_offset_from_start;
  unsigned int m_current_window_size;
  char *m_current_map_view_of_file;
  char *m_start_pointer;
  char *m_current_pointer;*//*
public:
	void			advance				(const int &offset);
	void			r					(void *buffer, u32 buffer_size);
	CStreamReader	*open_chunk			(const u32 &chunk_id);
	void			close				();
};

class IWriter;
struct IWriterVtbl
{
  void *(__thiscall *__vecDelDtor)(IWriter *_this, unsigned int);
  void (__thiscall *seek)(IWriter *_this, unsigned int);
  unsigned int (__thiscall *tell)(IWriter *_this);
  void (__thiscall *w)(IWriter *_this, const void *, unsigned int);
  bool (__thiscall *valid)(IWriter *_this);
};
class IWriter
{
public:
	IWriterVtbl *vfptr;
  //xr_stack<unsigned int,xr_vector<unsigned int,xalloc<unsigned int> > > chunk_pos;
  //shared_str fName;
	void	w		(const void* ptr, u32 count)
	{
		return vfptr->w(this, ptr, count);
	};
	void	w_u32	(u32 d);
	void	w_stringZ(const char *p);
	void	__cdecl	w_printf	(const char* format, ...);
};

class CMemoryWriter : public IWriter
{
	u8*				data;
	u32				position;
	u32				mem_size;
	u32				file_size;
};
*/
struct CSimulatorRS
{
  char dummy[1];
};
struct CSimulatorTSS
{
  char dummy[1];
};
struct CSimulator
{
  CSimulatorTSS TSS;
  CSimulatorRS RS;
  SimulatorStates container;
};

struct CPropertyBaseVtbl;
class CBlender_Compile;
class CPropertyBase
{
//  CPropertyBaseVtbl *vfptr;
public:
	virtual 	LPCSTR		getName			()								= 0;
	virtual		LPCSTR		getComment		()								= 0;

	virtual		void		Save			(IWriter&	fs)					= 0;
	virtual		void		Load			(IReader&	fs, u16 version)	= 0;
};
struct CPropertyBaseVtbl
{
/*  const char *(__thiscall *getName)(CPropertyBase *this_);
  const char *(__thiscall *getComment)(CPropertyBase *this_);
  BOOL (__thiscall *canBeDetailed)(CPropertyBase *this_);
  BOOL (__thiscall *canBeLMAPped)(CPropertyBase *this_);
  void (__thiscall *Save)(CPropertyBase *this_, IWriter *);
  void (__thiscall *Load)(CPropertyBase *this_, IReader *, unsigned __int16);
  void (__thiscall *Compile)(CPropertyBase *this_, CBlender_Compile *);*/
};

enum
{
	bDetailBump = (1 << 0),
	bParallax = (1 << 1),
};

class CBlender_Compile
{
public:
  xr_vector<shared_str> L_textures;
  xr_vector<shared_str> L_constants;
  xr_vector<shared_str> L_matrices;
  const char *detail_texture;
  R_constant_setup *detail_scaler;
  int bEditor;
  int bDetail;
  int bDetail_Diffuse;
  int bFlags;
  int iElement;
  CSimulator RS;
  IBlender *BT;
  ShaderElement *SH;
  SPass dest;
  R_constant_table ctable;
  STextureList passTextures;
  SMatrixList passMatrices;
  SConstantList passConstants;
  unsigned int dwStage;
  char pass_vs[128];
  char pass_ps[128];

  void				PassBegin			();
  void				PassSET_Blend		(BOOL bABlend,	u32 abSRC, u32 abDST, BOOL aTest, u32 aRef);
  void				PassSET_LightFog	(BOOL bLight, BOOL bFog);
  void				PassSET_ZB			(BOOL bZTest,	BOOL bZWrite, BOOL bInvertZTest=FALSE);
  void				PassEnd				();

  void				StageBegin			();
  void				StageSET_Color		(u32 a1, u32 op, u32 a2);
  void				StageSET_Alpha		(u32 a1, u32 op, u32 a2);
  void				StageSET_Address	(u32 adr);
  void				Stage_Texture		(LPCSTR name, u32 address=D3DTADDRESS_WRAP,	u32	 fmin=D3DTEXF_LINEAR, u32 fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR);
  void				Stage_Matrix		(LPCSTR name, int UVW_channel);
  void				Stage_Constant		(LPCSTR name);
  void				StageEnd			();

  void				SetParams			(int iPriority, bool bStrictB2F);
  void				SetMapping			();

  void				r_Pass				(LPCSTR vs,		LPCSTR ps,		bool bFog,	BOOL	bZtest=TRUE,				BOOL	bZwrite=TRUE,			BOOL	bABlend=FALSE,			D3DBLEND	abSRC=D3DBLEND_ONE,		D3DBLEND abDST=D3DBLEND_ZERO,	BOOL aTest=FALSE,	u32 aRef=0);
  void				r_Constant			(LPCSTR name,	R_constant_setup* s);
  u32				r_Sampler			(LPCSTR name,	LPCSTR texture,		bool b_ps1x_ProjectiveDivide=false, u32	address=D3DTADDRESS_WRAP,	u32		fmin=D3DTEXF_LINEAR,	u32		fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR);
  u32				r_Sampler			(LPCSTR name,	shared_str texture, bool b_ps1x_ProjectiveDivide=false, u32	address=D3DTADDRESS_WRAP,	u32		fmin=D3DTEXF_LINEAR,	u32		fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR)	{
	return r_Sampler	(name,texture.c_str(),b_ps1x_ProjectiveDivide,address,fmin,fmip,fmag);
  }
  void				r_Sampler_rtf		(LPCSTR name,	LPCSTR texture,		bool b_ps1x_ProjectiveDivide=false);
  void				r_Sampler_clf		(LPCSTR name,	LPCSTR texture,		bool b_ps1x_ProjectiveDivide=false);
  void				r_Sampler_clw		(LPCSTR name,	LPCSTR texture,		bool b_ps1x_ProjectiveDivide=false);
  void				r_End				();
};
struct xrP_Integer
{
  int value;
  int min;
  int max;
};
struct xrP_Float
{
  float value;
  float min;
  float max;
};
struct xrP_TOKEN
{
	struct Item {
		u32			ID;
		string64	str;
	};

	u32				IDselected;
	u32				Count;

	//--- elements:		(ID,string64)

//	xrP_TOKEN()	: IDselected(0), Count(0)		{}
};
struct xrP_CLSID
{
	CLASS_ID			Selected;
	u32				Count;
	//--- elements:		(...)

//	xrP_CLSID()	: Selected(0), Count(0)			{}
};
struct xrP_BOOL
{
  int value;
};
enum	xrProperties
{
	xrPID_MARKER	= 0,
	xrPID_MATRIX,		// really only name(stringZ) is written into stream
	xrPID_CONSTANT,		// really only name(stringZ) is written into stream
	xrPID_TEXTURE,		// really only name(stringZ) is written into stream
	xrPID_INTEGER,
	xrPID_FLOAT,
	xrPID_BOOL,
	xrPID_TOKEN,
	xrPID_CLSID,
	xrPID_OBJECT,		// really only name(stringZ) is written into stream
	xrPID_STRING,		// really only name(stringZ) is written into stream
	xrPID_MARKER_TEMPLATE,
	xrPID_FORCEDWORD=u32(-1)
};

#pragma pack(push, 4)
class /*__declspec(dllimport)*/ IBlender : public CPropertyBase
{
public:
  CBlender_DESC description;
  xrP_Integer oPriority;
  xrP_BOOL oStrictSorting;
  char oT_Name[64];
  char oT_xform[64];

	virtual 		LPCSTR		getName			()	{return description.cName;}
	virtual			LPCSTR		getComment		()	= 0;

	virtual			BOOL		canBeDetailed	()	{ return FALSE; }
	virtual			BOOL		canBeLMAPped	()	= 0;
	
	virtual			void		Save			(IWriter& fs);
	virtual			void		Load			(IReader& fs, u16 version);
	
	virtual			void		Compile			(CBlender_Compile& C);

	IBlender();
	virtual ~IBlender();
};
#pragma pack(pop)
class /*__declspec(dllimport)*/ CBlender_skybox : public IBlender
{
};
class CPerlinNoiseCustom
{
public:
  int mSeed;
  bool mReady;
  int p[514];
  int mOctaves;
  float mFrequency;
  float mAmplitude;
  xr_vector<float> mTimes;
};

/*  618 */
class CPerlinNoise1D : CPerlinNoiseCustom
{
public:
  float g1[514];
  float mPrevContiniousTime;
};

extern Fvector2 artefacts_position[8];
extern Fvector2 anomalys_position[8];
extern Ivector3 detector_params;