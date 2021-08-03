#pragma once
#define D3D_DEBUG_INFO
#include <D3D9.h>
#include "shader.h"
#include "HW.h"

const	u32		CULL_CCW			= D3DCULL_CCW;
const	u32		CULL_CW				= D3DCULL_CW;
const	u32		CULL_NONE			= D3DCULL_NONE;

enum
{
	RC_float	= 0,
	RC_int		= 1,
	RC_bool		= 2,
	RC_sampler	= 99
};
enum
{
	RC_1x1		= 0,					// vector1, or scalar
	RC_1x4,								// vector4
	RC_2x4,								// 4x2 matrix, transpose
	RC_3x4,								// 4x3 matrix, transpose
	RC_4x4,								// 4x4 matrix, transpose
	RC_1x4a,							// array: vector4
	RC_3x4a,							// array: 4x3 matrix, transpose
	RC_4x4a								// array: 4x4 matrix, transpose
};
enum
{
	RC_dest_pixel	= (1<<0),
	RC_dest_vertex	= (1<<1),
	RC_dest_sampler	= (1<<2)
};

enum
{
	LOCKFLAGS_FLUSH		= D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND	= D3DLOCK_NOOVERWRITE
};

struct R_statistics_element
{
	unsigned int verts;
	unsigned int dips;

	ICF		void	add			(u32 _verts)	{ verts+=_verts; dips++; }
};
struct R_statistics
{
	R_statistics_element s_static;
	R_statistics_element s_flora;
	R_statistics_element s_flora_lods;
	R_statistics_element s_details;
	R_statistics_element s_ui;
	R_statistics_element s_dynamic;
	R_statistics_element s_dynamic_sw;
	R_statistics_element s_dynamic_inst;
	R_statistics_element s_dynamic_1B;
	R_statistics_element s_dynamic_2B;
};
struct _VertexStream
{
	IDirect3DVertexBuffer9 *pVB;
	unsigned int mSize;
	unsigned int mPosition;
	unsigned int mDiscardID;
	IDirect3DVertexBuffer9 *old_pVB;
private:
	void						_clear			();

public:
	void						Create			();
	void						Destroy			();
	void						reset_begin		();
	void						reset_end		();

	IDirect3DVertexBuffer9*	Buffer()		{ return pVB;			}
	u32						DiscardID()		{ return mDiscardID;	}
	void						Flush()			{ mPosition=mSize;		}
	u32							GetSize()		{ return mSize;}

	void*						Lock			( u32 vl_Count, u32 Stride, u32& vOffset );
	void						Unlock			( u32 Count, u32 Stride);

	_VertexStream()				{ _clear();		};
	~_VertexStream()			{ Destroy();	};
};
struct _IndexStream
{
	IDirect3DIndexBuffer9 *pIB;
	unsigned int mSize;
	unsigned int mPosition;
	unsigned int mDiscardID;
	IDirect3DIndexBuffer9 *old_pIB;

	IC IDirect3DIndexBuffer9*	Buffer()		{ return pIB;			}
	IC u32						DiscardID()		{ return mDiscardID;	}
	void						Flush()			{ mPosition=mSize;		}

	u16*						Lock			( u32 Count, u32& vOffset );
	void						Unlock			(u32 RealCount);
};
struct R_xforms
{
	_matrix<float> m_w;
	_matrix<float> m_v;
	_matrix<float> m_p;
	_matrix<float> m_wv;
	_matrix<float> m_vp;
	_matrix<float> m_wvp;
	R_constant *c_w;
	R_constant *c_v;
	R_constant *c_p;
	R_constant *c_wv;
	R_constant *c_vp;
	R_constant *c_wvp;
public:
	R_xforms		();
	void			unmap		();
	void			set_W		(const Fmatrix& m);
	void			set_V		(const Fmatrix& m);
	void			set_P		(const Fmatrix& m);
	const Fmatrix&	get_W	()					{ return m_w;	}
	const Fmatrix&	get_V	()					{ return m_v;	}
	const Fmatrix&	get_P	()					{ return m_p;	}
	void			set_c_w		(R_constant* C);
	void			set_c_v		(R_constant* C);
	void			set_c_p		(R_constant* C);
	void			set_c_wv	(R_constant* C);
	void			set_c_vp	(R_constant* C);
	void			set_c_wvp	(R_constant* C);
};
template <class T, u32 limit>
class	R_constant_cache
{
private:
	ALIGN(16)	svector<T,limit>		array;
	u32									lo,hi;
public:
	R_constant_cache()
	{
		array.resize(limit);
		flush		();
	}
	ICF T*					access	(u32 id)				{ return &array[id];						}
	ICF void				flush	()						{ lo=hi=0;									}
	ICF void				dirty	(u32 _lo, u32 _hi)		{ if (_lo<lo) lo=_lo; if (_hi>hi) hi=_hi;	}
	ICF u32					r_lo	()						{ return lo;								}
	ICF u32					r_hi	()						{ return hi;								}
};

class	R_constant_array
{
public:
	typedef		R_constant_cache<Fvector4,256>	t_f;
	typedef		R_constant_cache<Ivector4,16>	t_i;
	typedef		R_constant_cache<BOOL,16>		t_b;
public:
	ALIGN(16)	t_f					c_f;
//	ALIGN(16)	t_i					c_i;
//	ALIGN(16)	t_b					c_b;
	BOOL							b_dirty;
public:
	t_f&					get_array_f		()	{ return c_f;	}
//	t_i&					get_array_i		()	{ return c_i;	}
//	t_b&					get_array_b		()	{ return c_b;	}

	void					set		(R_constant* C, R_constant_load& L, const Fmatrix& A)
	{
		VERIFY		(RC_float == C->type);
		Fvector4*	it	= c_f.access	(L.index);
		switch		(L.cls)
		{
		case RC_2x4:
			c_f.dirty			(L.index,L.index+2);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			break;
		case RC_3x4:
			c_f.dirty			(L.index,L.index+3);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			break;
		case RC_4x4:
			c_f.dirty			(L.index,L.index+4);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			it[3].set			(A._14, A._24, A._34, A._44);
			break;
		default:
#ifdef DEBUG
			Debug.fatal		(DEBUG_INFO,"Invalid constant run-time-type for '%s'",*C->name);
#else
			NODEFAULT;
#endif
		}
	}

	void					set		(R_constant* C, R_constant_load& L, const Fvector4& A)
	{
		VERIFY		(RC_float	== C->type);
		VERIFY		(RC_1x4		== L.cls);
		c_f.access	(L.index)->set	(A);
		c_f.dirty	(L.index,L.index+1);
	}

	void					seta	(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A)
	{
		VERIFY		(RC_float == C->type);
		u32			base;
		Fvector4*	it;
		switch		(L.cls)
		{
		case RC_2x4:
			base				= L.index + 2*e;
			it					= c_f.access	(base);
			c_f.dirty			(base,base+2);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			break;
		case RC_3x4:
			base				= L.index + 3*e;
			it					= c_f.access	(base);
			c_f.dirty			(base,base+3);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			break;
		case RC_4x4:
			base				= L.index + 4*e;
			it					= c_f.access	(base);
			c_f.dirty			(base,base+4);
			it[0].set			(A._11, A._21, A._31, A._41);
			it[1].set			(A._12, A._22, A._32, A._42);
			it[2].set			(A._13, A._23, A._33, A._43);
			it[3].set			(A._14, A._24, A._34, A._44);
			break;
		default:
#ifdef DEBUG
			Debug.fatal		(DEBUG_INFO,"Invalid constant run-time-type for '%s'",*C->name);
#else
			NODEFAULT;
#endif
		}
	}

	void					seta	(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A)
	{
		VERIFY		(RC_float	== C->type);
		VERIFY		(RC_1x4		== L.cls);
		u32			base	= L.index + e;
		c_f.access	(base)->set	(A);
		c_f.dirty	(base,base+1);
	}
};

/* 2291 */
class	R_constants
{
public:
	ALIGN(16)	R_constant_array	a_pixel;
	ALIGN(16)	R_constant_array	a_vertex;

	void					flush_cache	();
public:
	// fp, non-array versions
	ICF void				set		(R_constant* C, const Fmatrix& A)		{
		if (C->destination&1)		{ a_pixel.set	(C,C->ps,A); a_pixel.b_dirty=TRUE;		}
		if (C->destination&2)		{ a_vertex.set	(C,C->vs,A); a_vertex.b_dirty=TRUE;		}
	}
	ICF void				set		(R_constant* C, const Fvector4& A)		{
		if (C->destination&1)		{ a_pixel.set	(C,C->ps,A); a_pixel.b_dirty=TRUE;		}
		if (C->destination&2)		{ a_vertex.set	(C,C->vs,A); a_vertex.b_dirty=TRUE;		}
	}
	ICF void				set		(R_constant* C, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		set					(C,data);
	}

	// fp, array versions
	ICF void				seta	(R_constant* C, u32 e, const Fmatrix& A)		{
		if (C->destination&1)		{ a_pixel.seta	(C,C->ps,e,A); a_pixel.b_dirty=TRUE;	}
		if (C->destination&2)		{ a_vertex.seta	(C,C->vs,e,A); a_vertex.b_dirty=TRUE;	}
	}
	ICF void				seta	(R_constant* C, u32 e, const Fvector4& A)		{
		if (C->destination&1)		{ a_pixel.seta	(C,C->ps,e,A); a_pixel.b_dirty=TRUE;	}
		if (C->destination&2)		{ a_vertex.seta	(C,C->vs,e,A); a_vertex.b_dirty=TRUE;	}
	}
	ICF void				seta	(R_constant* C, u32 e, float x, float y, float z, float w)	{
		Fvector4 data;		data.set(x,y,z,w);
		seta				(C,e,data);
	}

	//
	ICF void					flush	()
	{
		if (a_pixel.b_dirty || a_vertex.b_dirty)	flush_cache();
	}
};

class __declspec(align(16)) CBackend
{
public:
	struct _stats
	{
		unsigned int polys;
		unsigned int verts;
		unsigned int calls;
		unsigned int vs;
		unsigned int ps;
		unsigned int xforms;
		unsigned int target_rt;
		unsigned int target_zb;
		R_statistics r;
	};
	_VertexStream Vertex;
	_IndexStream Index;
	IDirect3DIndexBuffer9 *QuadIB;
	IDirect3DIndexBuffer9 *old_QuadIB;
	R_xforms xforms;
public:
	IDirect3DSurface9 *pRT[4];
	IDirect3DSurface9 *pZB;
	IDirect3DVertexDeclaration9 *decl;
	IDirect3DVertexBuffer9 *vb;
	IDirect3DIndexBuffer9 *ib;
	unsigned int vb_stride;
	__declspec(align(8)) R_constants constants;
	R_constant_table *ctable;
	IDirect3DStateBlock9 *state;
	IDirect3DPixelShader9 *ps;
	IDirect3DVertexShader9 *vs;
	unsigned int stencil_enable;
	unsigned int stencil_func;
	unsigned int stencil_ref;
	unsigned int stencil_mask;
	unsigned int stencil_writemask;
	unsigned int stencil_fail;
	unsigned int stencil_pass;
	unsigned int stencil_zfail;
	unsigned int colorwrite_mask;
	unsigned int cull_mode;
	STextureList *T;
	SMatrixList *M;
	SConstantList *C;
	CTexture *textures_ps[16];
	CTexture *textures_vs[5];
public:
	CBackend::_stats stat;
	IC	CTexture*					get_ActiveTexture			(u32 stage)
	{
		if (stage>=256)				return textures_vs[stage-256];
		else 						return textures_ps[stage];
	}
	void set_RT					(IDirect3DSurface9* RT, u32 ID=0);
	void set_ZB					(IDirect3DSurface9* ZB);

	IC	void	CBackend::set_xform_world	(const Fmatrix& M)
	{ 
		xforms.set_W(M);	
	}
	IC	void	CBackend::set_xform_view	(const Fmatrix& M)					
	{ 
		xforms.set_V(M);	
	}
	IC	void	CBackend::set_xform_project	(const Fmatrix& M)
	{ 
		xforms.set_P(M);	
	}

	IC	void						set_Shader			(Shader* S, u32 pass=0);
	IC	void						set_Shader			(ref_shader& S, u32 pass=0)			{ set_Shader(&*S,pass);			}
	ICF void						set_Geometry		(SGeometry* _geom);
	ICF void						set_Geometry		(ref_geom& _geom)					{	set_Geometry(&*_geom);		}
	IC	void						set_Element			(ShaderElement* S, u32	pass=0);
	IC	void						set_Element			(ref_selement& S, u32	pass=0)		{ set_Element(&*S,pass);		}

	ICF	void						set_States			(IDirect3DStateBlock9* _state);
	ICF	void						set_States			(ref_state& _state)					{ set_States(_state->state);	}

	ICF void						set_PS				(IDirect3DPixelShader9* _ps, LPCSTR _n=0);
	ICF void						set_PS				(ref_ps& _ps)						{ set_PS(_ps->ps,_ps->cName.c_str());				}

	ICF void						set_VS				(IDirect3DVertexShader9* _vs, LPCSTR _n=0);
	ICF void						set_VS				(ref_vs& _vs)						{ set_VS(_vs->vs,_vs->cName.c_str());				}

	IC	void						set_Constants		(R_constant_table* C);
	IC	void						set_Constants		(ref_ctable& C)						{ set_Constants(&*C);			}

		void						set_Textures		(STextureList* T);
	IC	void						set_Textures		(ref_texture_list& T)				{ set_Textures(&*T);			}

	IC	R_constant_array&			get_ConstantCache_Vertex	()			{ return constants.a_vertex;	}
	IC	R_constant_array&			get_ConstantCache_Pixel		()			{ return constants.a_pixel;		}
	// constants
	ICF	ref_constant				get_c				(LPCSTR			n)													{ if (ctable)	return ctable->get(n);else return 0;}
	ICF	ref_constant				get_c				(shared_str&	n)													{ if (ctable)	return ctable->get(n);else return 0;}

	void							set_ClipPlanes		(u32 _enable, Fmatrix*	_xform =NULL, u32 fmask=0xff);

	// constants - direct (fast)
	ICF	void						set_c				(R_constant* C, const Fmatrix& A)									{ if (C)		constants.set(C,A);					}
	ICF	void						set_c				(R_constant* C, const Fvector4& A)									{ if (C)		constants.set(C,A);					}
	ICF	void						set_c				(R_constant* C, float x, float y, float z, float w)					{ if (C)		constants.set(C,x,y,z,w);			}
	ICF	void						set_ca				(R_constant* C, u32 e, const Fmatrix& A)							{ if (C)		constants.seta(C,e,A);				}
	ICF	void						set_ca				(R_constant* C, u32 e, const Fvector4& A)							{ if (C)		constants.seta(C,e,A);				}
	ICF	void						set_ca				(R_constant* C, u32 e, float x, float y, float z, float w)			{ if (C)		constants.seta(C,e,x,y,z,w);		}

	// constants - LPCSTR (slow)
	ICF	void						set_c				(LPCSTR n, const Fmatrix& A)										{ if(ctable)	set_c	(&*ctable->get(n),A);		}
	ICF	void						set_c				(LPCSTR n, const Fvector4& A)										{ if(ctable)	set_c	(&*ctable->get(n),A);		}
	ICF	void						set_c				(LPCSTR n, float x, float y, float z, float w)						{ if(ctable)	set_c	(&*ctable->get(n),x,y,z,w);	}
	ICF	void						set_ca				(LPCSTR n, u32 e, const Fmatrix& A)									{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
	ICF	void						set_ca				(LPCSTR n, u32 e, const Fvector4& A)								{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
	ICF	void						set_ca				(LPCSTR n, u32 e, float x, float y, float z, float w)				{ if(ctable)	set_ca	(&*ctable->get(n),e,x,y,z,w);}

	// constants - shared_str (average)
	ICF	void						set_c				(shared_str& n, const Fmatrix& A)									{ if(ctable)	set_c	(&*ctable->get(n),A);			}
	ICF	void						set_c				(shared_str& n, const Fvector4& A)									{ if(ctable)	set_c	(&*ctable->get(n),A);			}
	ICF	void						set_c				(shared_str& n, float x, float y, float z, float w)					{ if(ctable)	set_c	(&*ctable->get(n),x,y,z,w);	}
	ICF	void						set_ca				(shared_str& n, u32 e, const Fmatrix& A)							{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
	ICF	void						set_ca				(shared_str& n, u32 e, const Fvector4& A)							{ if(ctable)	set_ca	(&*ctable->get(n),e,A);		}
	ICF	void						set_ca				(shared_str& n, u32 e, float x, float y, float z, float w)			{ if(ctable)	set_ca	(&*ctable->get(n),e,x,y,z,w);}

	ICF void	CBackend::set_CullMode		(u32 _mode)
	{
		if (cull_mode		!= _mode)		{ cull_mode = _mode;			CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			_mode				)); }
	}
	IC  u32							get_CullMode		(){return cull_mode;}

	void set_Stencil(u32 _enable, u32 _func=D3DCMP_ALWAYS, u32 _ref=0x00, u32 _mask=0x00, u32 _writemask=0x00, u32 _fail=D3DSTENCILOP_KEEP, u32 _pass=D3DSTENCILOP_KEEP, u32 _zfail=D3DSTENCILOP_KEEP);
	void set_ColorWriteEnable(u32 _mask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	void Render(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC);
	void Render(D3DPRIMITIVETYPE T, u32 startV, u32 PC);
};
extern __declspec(dllimport) CBackend RCache;