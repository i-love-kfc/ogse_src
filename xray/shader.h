#pragma once

#include "texture.h"
#define D3D_DEBUG_INFO
#include <D3D9.h>

#define	SHADER_PASSES_MAX		2

class IBlender;
class IReader;
class IWriter;

struct SDeclaration : xr_resource_flagged
{
  IDirect3DVertexDeclaration9 *dcl;
  xr_vector<_D3DVERTEXELEMENT9> dcl_code;
  ~SDeclaration	();
};

struct SGeometry : xr_resource_flagged
{
  resptr_core<SDeclaration,resptr_base<SDeclaration> > dcl;
  IDirect3DVertexBuffer9 *vb;
  IDirect3DIndexBuffer9 *ib;
  unsigned int vb_stride;
  					~SGeometry		();
};
struct resptrcode_geom : public resptr_base<SGeometry>
{
public:
	void 				create			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				create			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void				destroy			();
	u32					stride			()	const	;
};
typedef resptr_core<SGeometry,resptrcode_geom> ref_geom;

struct CConstant : public xr_resource_named
{
  _color<float> const_float;
  unsigned int const_dword;
  unsigned int dwFrame;
  unsigned int dwMode;
  WaveForm _R;
  WaveForm _G;
  WaveForm _B;
  WaveForm _A;
  CConstant		();
	void			set_float	(float r, float g, float b, float a);
	void			set_float	(Fcolor& c);
	void			set_dword	(u32 c);
	void			Calculate	();
	BOOL			Similar		(CConstant& C);
	void			Load		(IReader* fs);
	void			Save		(IWriter* fs);
};
struct SConstantList : public xr_resource_flagged , public svector<resptr_core<CConstant,resptr_base<CConstant> >,4>
{
	~SConstantList			();
};
typedef resptr_core<SConstantList,resptr_base<SConstantList> > ref_constant_list;
template class std::vector<SConstantList*, xalloc<SConstantList*>>;
class SimulatorStates
{
private:
	struct State
	{
		unsigned int type;
		unsigned int v1;
		unsigned int v2;
		unsigned int v3;
	};
	template class std::vector<State, xalloc<State>>;
	xr_vector<State> States;
};
struct SState : xr_resource_flagged
{
  IDirect3DStateBlock9 *state;
  SimulatorStates state_code;
  ~SState			();
};
typedef	resptr_core<SState,resptr_base<SState> >											ref_state;

struct	R_constant_load
{
	u16						index;		// linear index (pixel)
	u16						cls;		// element class

	R_constant_load() : index(u16(-1)), cls(u16(-1)) {};

	IC BOOL					equal		(R_constant_load& C)
	{
		return (index==C.index) && (cls == C.cls);
	}
};
struct R_constant;
class	R_constant_setup
{
public:
	virtual void			setup		(R_constant* C)	= 0;
};

struct R_constant : public xr_resource
{
  shared_str name;
  unsigned __int16 type;
  unsigned __int16 destination;
  R_constant_load ps;
  R_constant_load vs;
  R_constant_load samp;
  R_constant_setup *handler;

  R_constant() : type(u16(-1)), destination(0), handler(NULL) { };

	IC BOOL					equal		(R_constant& C);
	IC BOOL					equal		(R_constant* C);
};
typedef resptr_core<R_constant,resptr_base<R_constant>> ref_constant;
class	R_constant_table	: public xr_resource_flagged	{
public:
	typedef xr_vector<ref_constant>		c_table;
	c_table					table;
private:
	void					fatal		(LPCSTR s);
public:
	~R_constant_table					();

	void					clear		();
	BOOL					parse		(void* desc, u16 destination);
	void					merge		(R_constant_table* C);
	ref_constant			get			(LPCSTR		name);		// slow search
	ref_constant			get			(shared_str&	name);		// fast search

	BOOL					equal		(R_constant_table& C);
	BOOL					equal		(R_constant_table* C)	{	return equal(*C);		}
	BOOL					empty		()						{	return 0==table.size();	}
};
typedef resptr_core<R_constant_table,resptr_base<R_constant_table> > ref_ctable;
struct SPS : xr_resource_named
{
  IDirect3DPixelShader9 *ps;
  R_constant_table constants;
  ~SPS			();
};
typedef resptr_core<SPS,resptr_base<SPS> > ref_ps;
struct SVS : xr_resource_named
{
  IDirect3DVertexShader9 *vs;
  R_constant_table constants;
  ~SVS			();
};
typedef resptr_core<SVS,resptr_base<SVS> > ref_vs;
struct SPass : public xr_resource_flagged
{
  
	ref_state							state;		// Generic state, like Z-Buffering, samplers, etc
	ref_ps								ps;			// may be NULL = FFP, in that case "state" must contain TSS setup
	ref_vs								vs;			// may be NULL = FFP, in that case "state" must contain RS setup, *and* FVF-compatible declaration must be used
	ref_ctable							constants;	// may be NULL

	ref_texture_list					T;
	ref_constant_list					C;

	~SPass			();
	BOOL				equal			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);

};
typedef	resptr_core<SPass,resptr_base<SPass> >												ref_pass;
struct ShaderElement : public xr_resource_flagged
{
public:
	struct Sflags
	{
		u32	iPriority	:	2;
		u32	bStrictB2F	:	1;
		u32	bEmissive	:	1;
		u32	bDistort	:	1;
		u32	bWmark		:	1;
	};
public:
	Sflags								flags;
	svector<resptr_core<SPass, resptr_base<SPass>>,2>					passes;

						ShaderElement	();
						~ShaderElement	();
	BOOL				equal			(ShaderElement& S);
	BOOL				equal			(ShaderElement* S);
};
typedef resptr_core<ShaderElement,resptr_base<ShaderElement> > ref_selement;
struct Shader : public xr_resource_flagged
{
  resptr_core<ShaderElement,resptr_base<ShaderElement> > E[6];
  					~Shader			();
	BOOL				equal			(Shader& S);
	BOOL				equal			(Shader* S);
};

struct resptrcode_shader : public resptr_base<Shader>
{
	void				create			(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				create			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void				destroy			();
};

typedef resptr_core<Shader, resptrcode_shader> ref_shader;
template class std::vector<ref_shader,xalloc<ref_shader> >;

struct CRT: public xr_resource_named
{
  IDirect3DTexture9 *pSurface;
  IDirect3DSurface9 *pRT;
  ref_texture pTexture;
  unsigned int dwWidth;
  unsigned int dwHeight;
  _D3DFORMAT fmt;
  unsigned __int64 _order;

	CRT					();
	~CRT				();

	void				create			(LPCSTR Name, u32 w, u32 h, D3DFORMAT f);
	void				destroy			();
	void				reset_begin		();
	void				reset_end		();
	BOOL				valid			()	{ return !!pTexture; }
};


struct resptrcode_crt : public resptr_base<CRT>
{
	void				create			(LPCSTR Name, u32 w, u32 h, D3DFORMAT f);
	void				destroy			();
};
typedef resptr_core<CRT,resptrcode_crt> ref_rt;

struct CRTC: public xr_resource_named
{
public:
	IDirect3DCubeTexture9*	pSurface;
	IDirect3DSurface9*		pRT[6];
	ref_texture				pTexture;

	u32						dwSize;
	D3DFORMAT				fmt;

	u64						_order;

	CRTC					();
	~CRTC					();

	void				create			(LPCSTR name, u32 size, D3DFORMAT f);
	void				destroy			();
	void				reset_begin		();
	void				reset_end		();
	BOOL				valid			()	{ return !pTexture; }
};

struct resptrcode_crtc : public resptr_base<CRTC>
{
	void				create			(LPCSTR Name, u32 size, D3DFORMAT f);
	void				destroy			();//	{ _set(NULL);		}
};
typedef resptr_core<CRTC,resptrcode_crtc> ref_rtc;


enum	SE_R1				{
	SE_R1_NORMAL_HQ			= 0,	// high quality/detail
	SE_R1_NORMAL_LQ			= 1,	// normal or low quality
	SE_R1_LPOINT			= 2,	// add: point light
	SE_R1_LSPOT				= 3,	// add:	spot light
	SE_R1_LMODELS			= 4,	// lighting info for models or shadowing from models
};