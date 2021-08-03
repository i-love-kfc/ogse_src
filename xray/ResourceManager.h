#pragma once
#include "base_types.h"
#include "texture.h"
#include "shader.h"
#include "Backend.h"

struct STextureList;
class CTexture;
class CMatrix;

struct lua_State;

class CTextureDescrMngr
{
	struct texture_assoc
	{
		shared_str			detail_name;
		R_constant_setup*	        cs;
		u8				usage;
                texture_assoc                   ():cs(NULL),usage(0){}
	};
	struct texture_spec
	{
		shared_str			m_bump_name;
		float				m_material;
		bool				bParallax;
	};
	struct texture_desc{
		texture_assoc*		m_assoc;
		texture_spec*		m_spec;
        texture_desc            ():m_assoc(NULL),m_spec(NULL){}
	};
	template class std::map<shared_str,CTextureDescrMngr::texture_desc,std::less<shared_str>,xalloc<std::pair<shared_str,CTextureDescrMngr::texture_desc> > >;
	DEFINE_MAP(shared_str, texture_desc,	map_TD,	map_TDIt);

	map_TD									m_texture_details;

	void		__cdecl LoadLTX		();
	void		__cdecl LoadTHM		();

public:
	void		Load		();
	void		UnLoad		();
public:
	shared_str	GetBumpName		(const shared_str& tex_name) const;
	float		GetMaterial		(const shared_str& tex_name) const;
	void		GetTextureUsage	(const shared_str& tex_name, BOOL& bDiffuse, BOOL& bBump) const;
	BOOL		GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup* &CS) const;      
};


extern float					r__dtex_range/*	= 50*/;
class cl_dt_scaler		: public R_constant_setup {
public:
	float				scale;

	cl_dt_scaler		(float s) : scale(s)	{};
	virtual void setup	(R_constant* C)
	{
		RCache.set_c	(C,scale,scale,scale,1/r__dtex_range);
	}
};

class __declspec(dllimport) CResourceManager {
private:
	struct str_pred : public std::binary_function<char*, char*, bool>	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return xr_strcmp(x,y)<0;	}
	};
	struct texture_detail	{
		const char*			T;
		R_constant_setup*	cs;
	};
public:
	template class std::map< const char*, IBlender*, str_pred, xalloc<std::pair<const char*, IBlender*>>>;	
	template class std::map< const char*, CTexture*, str_pred, xalloc<std::pair<const char*, CTexture*>> >;	
	template class std::map< const char*, CMatrix*, str_pred, xalloc<std::pair<const char*, CMatrix*>> >;	
	template class std::map< const char*, CConstant*, str_pred, xalloc<std::pair<const char*, CConstant*>> >;	
	template class std::map< const char*, CRT*, str_pred, xalloc<std::pair<const char*, CRT*>> >;	
	template class std::map< const char*, CRTC*, str_pred, xalloc<std::pair<const char*, CRTC*>> >;	
	template class std::map< const char*, SVS*, str_pred, xalloc<std::pair<const char*, SVS*>> >;	
	template class std::map< const char*, SPS*, str_pred, xalloc<std::pair<const char*, SPS*>> >;	
	template class std::map< const char*, texture_detail, str_pred, xalloc<std::pair<const char*, texture_detail>> >;	
	DEFINE_MAP_PRED(const char*,IBlender*,		map_Blender,	map_BlenderIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CTexture*,		map_Texture,	map_TextureIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CMatrix*,		map_Matrix,		map_MatrixIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CConstant*,		map_Constant,	map_ConstantIt,		str_pred);
	DEFINE_MAP_PRED(const char*,CRT*,			map_RT,			map_RTIt,			str_pred);
	DEFINE_MAP_PRED(const char*,CRTC*,			map_RTC,		map_RTCIt,			str_pred);
	DEFINE_MAP_PRED(const char*,SVS*,			map_VS,			map_VSIt,			str_pred);
	DEFINE_MAP_PRED(const char*,SPS*,			map_PS,			map_PSIt,			str_pred);
	DEFINE_MAP_PRED(const char*,texture_detail,	map_TD,			map_TDIt,			str_pred);
private:
	// data
	map_Blender											m_blenders;
public:
	map_Texture											m_textures;
private:
	map_Matrix											m_matrices;
	map_Constant										m_constants;
	map_RT												m_rtargets;
	map_RTC												m_rtargets_c;
	map_VS												m_vs;
	map_PS												m_ps;
	map_TD												m_td;

	xr_vector<SState*>									v_states;
	xr_vector<SDeclaration*>							v_declarations;
	xr_vector<SGeometry*>								v_geoms;
	xr_vector<R_constant_table*>						v_constant_tables;

	// lists
	xr_vector<STextureList*>							lst_textures;
	xr_vector<SMatrixList*>								lst_matrices;
	xr_vector<SConstantList*>							lst_constants;

	// main shader-array
	xr_vector<SPass*>									v_passes;
	xr_vector<ShaderElement*>							v_elements;
	xr_vector<Shader*>									v_shaders;
	
	xr_vector<ref_texture>								m_necessary;
	// misc
public:
	CTextureDescrMngr									m_textures_description;
//.	CInifile*											m_textures_description;
	xr_vector<std::pair<shared_str,R_constant_setup*> >	v_constant_setup;
	lua_State*											LSVM;
	BOOL												bDeferredLoad;
public:
	void _GetMemoryUsage(u32 & m_base, u32 & c_base, u32 & m_lmaps, u32 & c_lmaps);
	void Dump(bool bBrief);
	void DBG_VerifyGeoms();
	//void DBG_VerifyTextures();
	void DestroyNecessaryTextures();
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(const CTexture* T);

/*	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(const CMatrix*  M);*/

/*	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(const CConstant* C);*/

	R_constant_table*				_CreateConstantTable(R_constant_table& C);
	void							_DeleteConstantTable(const R_constant_table* C);

	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f);
	void							_DeleteRT			(const CRT*	RT	);

/*	CRTC*							_CreateRTC			(LPCSTR Name, u32 size,	D3DFORMAT f);
	void							_DeleteRTC			(const CRTC*	RT	);*/

	SPS*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(const SPS*	PS	);

	SVS*							_CreateVS			(LPCSTR Name);
	void							_DeleteVS			(const SVS*	VS	);

	SPass*							_CreatePass			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
	void							_DeletePass			(const SPass* P	);

	// Shader compiling / optimizing
	SState*							_CreateState		(SimulatorStates& Code);
	void							_DeleteState		(const SState* SB);

	SDeclaration*					_CreateDecl			(D3DVERTEXELEMENT9* dcl);
	void							_DeleteDecl			(const SDeclaration* dcl);

	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(const STextureList* L);

	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(const SMatrixList* L);

	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(const SConstantList* L);

	ShaderElement*					_CreateElement		(ShaderElement& L);
	void							_DeleteElement		(const ShaderElement* L);
	
/*	Shader*							_cpp_Create			(LPCSTR		s_shader,	LPCSTR s_textures=0,	LPCSTR s_constants=0,	LPCSTR s_matrices=0);
	Shader*							_cpp_Create			(IBlender*	B,			LPCSTR s_shader=0,		LPCSTR s_textures=0,	LPCSTR s_constants=0, LPCSTR s_matrices=0);
	Shader*							_lua_Create			(LPCSTR		s_shader,	LPCSTR s_textures);
	BOOL							_lua_HasShader		(LPCSTR		s_shader);

	CResourceManager						()	: bDeferredLoad(TRUE)/*, m_description(0)*/	/*{	}
	~CResourceManager						()	;*/

/*	void			OnDeviceCreate			(IReader* F);
	void			OnDeviceCreate			(LPCSTR name);
	void			OnDeviceDestroy			(BOOL   bKeepTextures);

	void			reset_begin				();
	void			reset_end				();*/

	// Creation/Destroying
	Shader*			Create					(LPCSTR s_shader=0, LPCSTR s_textures=0,	LPCSTR s_constants=0,	LPCSTR s_matrices=0);
	Shader*			Create					(IBlender*	B,		LPCSTR s_shader=0,		LPCSTR s_textures=0,	LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			Delete					(const Shader*		S	);
	void			RegisterConstantSetup	(LPCSTR name,		R_constant_setup* s);//	{	v_constant_setup.push_back(mk_pair(shared_str(name),s));	}

	SGeometry*		CreateGeom				(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	SGeometry*		CreateGeom				(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			DeleteGeom				(const SGeometry* VS		);
	void			DeferredLoad			(BOOL E)					{ bDeferredLoad=E;	}
/*	void			DeferredUpload			();
.	void			DeferredUnload			();*/
	void			Evict					();
/*	void			StoreNecessaryTextures	();
	void			DestroyNecessaryTextures();
	void			Dump					(bool bBrief);*/
};
