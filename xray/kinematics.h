#pragma once
#include "shader.h"
#include "IniFile.h"
#include "intrusive_ptr.h"
#include "base_types.h"
#include "bone.h"

class IReader;
class IRender_Visual;
class CKinematicsAnimated;
struct IParticleCustom;
class CKinematics;
struct IRender_Mesh;

struct IRender_VisualVtbl
{
  void (__thiscall *Render)(IRender_Visual *_this, float);
  void (__thiscall *Load)(IRender_Visual *_this, const char *, IReader *, unsigned int);
  void (__thiscall *Release)(IRender_Visual *_this);
  void (__thiscall *Copy)(IRender_Visual *_this, IRender_Visual *);
  void (__thiscall *Spawn)(IRender_Visual *_this);
  void (__thiscall *Depart)(IRender_Visual *_this);
  CKinematics *(__thiscall *dcast_PKinematics)(IRender_Visual *_this);
  CKinematicsAnimated *(__thiscall *dcast_PKinematicsAnimated)(IRender_Visual *_this);
  IParticleCustom *(__thiscall *dcast_ParticleCustom)(IRender_Visual *_this);
  void *(__thiscall *__vecDelDtor)(IRender_Visual *_this, unsigned int);
};

struct R_feedback;
struct R_feedbackVtbl
{
  void (__thiscall *rfeedback_static)(R_feedback *_this, IRender_Visual *);
};

struct R_feedback
{
  R_feedbackVtbl *vfptr;
};

class	smapvis		: public	R_feedback
{
public:
	enum			{
		state_counting	= 0,
		state_working	= 1,
		state_usingTC	= 3,
	}							state;
	xr_vector<IRender_Visual*>	invisible;

	u32							frame_sleep;
	u32							test_count;
	u32							test_current;
	IRender_Visual*				testQ_V;
	u32							testQ_id;
	u32							testQ_frame;
	void			invalidate	();
	void			resetoccq	();
	void			flushoccq	();
};

struct vis_data
{
  _sphere<float> sphere;
  _box3<float> box;
  unsigned int marker;
  unsigned int accept_frame;
  unsigned int hom_frame;
  unsigned int hom_tested;

	void		clear			()
	{
		sphere.P.set	(0,0,0);
		sphere.R		= 0;
		box.invalidate	();
		marker			= 0;
		accept_frame	= 0;
		hom_frame		= 0;
		hom_tested		= 0;
	}
};

class IRender_Visual
{
public:
  IRender_VisualVtbl *vfptr;
  unsigned int Type;
  vis_data vis;
  resptr_core<Shader,resptrcode_shader> shader;

  void IRender_Visual::Render(float lod);
  void IRender_Visual::Depart();
  void IRender_Visual::Release();
  CKinematics *IRender_Visual::dcast_PKinematics();
  void IRender_Visual::Load(char const *, IReader *, u32);
//  void *__vecDelDtor(unsigned int);
};

struct FHierrarhyVisual : public IRender_Visual
{
  xr_vector<IRender_Visual *> children;
  int bDontDelete;
};

struct FLOD : public FHierrarhyVisual
{
public:
	struct _vertex
	{
		Fvector		v;
		Fvector2	t;
		u32			c_rgb_hemi;	// rgb,hemi
		u8			c_sun;
	};
	struct _face
	{
		_vertex		v[4]	;
		Fvector		N		;
	};
	struct _hw 
	{
		Fvector		p0		;
		Fvector		p1		;
		Fvector		n0		;
		Fvector		n1		;
		u32			sun_af	;
		Fvector2	t0		;
		Fvector2	t1		;
		u32			rgbh0	;
		u32			rgbh1	;
	};

	ref_geom		geom		;
	_face			facets		[8];
	float			lod_factor	;
};

#include "FMesh.h"
struct	IRender_Mesh1;
struct IRender_Mesh1Vtbl
{
	void *(__thiscall *__vecDelDtor)(IRender_Mesh1 *_this, unsigned int);
};
struct	IRender_Mesh1	
{
	IRender_Mesh1Vtbl			*vfptr;
	// format
	ref_geom					rm_geom;

	// verts
	IDirect3DVertexBuffer9*		p_rm_Vertices;
	u32							vBase;
	u32							vCount;

	// indices
	IDirect3DIndexBuffer9*		p_rm_Indices;
	u32							iBase;
	u32							iCount;
	u32							dwPrimitives;
};

struct	Fvisual					: public		IRender_Visual, public IRender_Mesh1
{
public:
	IRender_Mesh*				m_fast			;	
};

struct	FProgressive	: public Fvisual
{
public:
	FSlideWindowItem	nSWI		;
	FSlideWindowItem*	xSWI		;
	u32					last_lod	;

	void 		Render		(float LOD);		// LOD - Level Of Detail  [0.0f - min, 1.0f - max], -1 = Ignored
};

class CSkeletonWallmark;
class IKinematicsAnimated;
struct SEnumVerticesCallback;
//class IBoneData;
//class Fobb;
//class Fbox;


class IKinematics
{
public:
	typedef xr_vector<std::pair<shared_str, u16> >	accel;
	struct	pick_result
	{
		Fvector	normal;
		float	dist;
		Fvector	tri[3];
	};
public:
	virtual		void					Bone_Calculate		(CBoneData* bd, Fmatrix* parent) = 0;
	virtual		void					Bone_GetAnimPos(Fmatrix& pos,u16 id, u8 channel_mask, bool ignore_callbacks) = 0;

	virtual		bool					PickBone			(const Fmatrix &parent_xform, pick_result &r, float dist, const Fvector& start, const Fvector& dir, u16 bone_id) = 0;
	virtual		void					EnumBoneVertices	(SEnumVerticesCallback &C, u16 bone_id) = 0;

	// Low level interface
	virtual u16					__stdcall	LL_BoneID(LPCSTR  B) = 0;
	virtual u16					__stdcall	LL_BoneID(const shared_str& B) = 0;
	virtual LPCSTR				__stdcall	LL_BoneName_dbg(u16 ID) = 0;

	virtual CInifile*			__stdcall	LL_UserData() = 0;
	virtual accel*						LL_Bones() = 0;

	virtual ICF CBoneInstance&	__stdcall	LL_GetBoneInstance(u16 bone_id) = 0;

	CBoneData&				LL_GetData(u16 bone_id);

virtual	const CBoneData&		__stdcall	GetBoneData(u16 bone_id) const = 0;

	virtual u16					__stdcall	LL_BoneCount()const = 0;
	virtual u16							LL_VisibleBoneCount() = 0;

	virtual ICF			Fmatrix& __stdcall	LL_GetTransform(u16 bone_id) = 0;
	virtual ICF const	Fmatrix& __stdcall	LL_GetTransform(u16 bone_id) const = 0;

	virtual ICF Fmatrix&				LL_GetTransform_R(u16 bone_id) = 0;
	virtual Fobb&						LL_GetBox(u16 bone_id) = 0;
	virtual const Fbox&			__stdcall	GetBox()const = 0;
	virtual void						LL_GetBindTransform(xr_vector<Fmatrix>& matrices) = 0;
	virtual int 						LL_GetBoneGroups(xr_vector<xr_vector<u16> >& groups) = 0;

	virtual u16					__stdcall	LL_GetBoneRoot() = 0;
	virtual void						LL_SetBoneRoot(u16 bone_id) = 0;

	virtual BOOL				__stdcall	LL_GetBoneVisible(u16 bone_id) = 0;
	virtual void						LL_SetBoneVisible(u16 bone_id, BOOL val, BOOL bRecursive) = 0;
	virtual unsigned __int64					__stdcall	LL_GetBonesVisible() = 0;
	virtual void						LL_SetBonesVisible(unsigned __int64 mask) = 0;

	// Main functionality
	virtual void						CalculateBones(BOOL bForceExact	= FALSE) = 0; // Recalculate skeleton
	virtual void						CalculateBones_Invalidate() = 0;
	//virtual void						Callback(UpdateCallback C, void* Param) = 0;

	//	Callback: data manipulation
	//virtual void						SetUpdateCallback(UpdateCallback pCallback) = 0;
	//virtual void						SetUpdateCallbackParam(void* pCallbackParam) = 0;

	//virtual UpdateCallback				GetUpdateCallback() = 0;
	//virtual void*						GetUpdateCallbackParam() = 0;
	//UpdateCallback						Update_Callback;
	//void*								Update_Callback_Param;
	virtual IRender_Visual*		__stdcall 	dcast_RenderVisual() = 0;
	virtual IKinematicsAnimated*	__stdcall	dcast_PKinematicsAnimated() = 0;

};


#pragma pack(push, 4)
class CKinematics : public FHierrarhyVisual// , public IKinematics
{
public:
  IRender_Visual *m_lod;
  xr_vector<intrusive_ptr<CSkeletonWallmark> > wallmarks;
  unsigned int wm_frame;
  xr_vector<IRender_Visual *> children_invisible;
  CInifile *pUserData;
  CBoneInstance *bone_instances;
  xr_vector<CBoneData *> *bones;
  unsigned __int16 iRoot;
  xr_vector<std::pair<shared_str,unsigned short> > *bone_map_N;
  xr_vector<std::pair<shared_str,unsigned short> > *bone_map_P;
  int Update_Visibility;
  unsigned int UCalc_Time;
  int UCalc_Visibox;
  _flags<unsigned __int64> visimask;
  void (__cdecl *Update_Callback)(CKinematics *);
  void *Update_Callback_Param;
  void AddWallmark			(const Fmatrix* parent, const Fvector& start, const Fvector& dir, ref_shader shader, float size);

	CBoneData&				LL_GetData(u16 bone_id);
/*	u32					mem_usage			(bool bInstance)
	{
		u32 sz					= sizeof(*this);
		sz						+= bone_instances?bone_instances->mem_usage():0;
		if (!bInstance){
			for (vecBonesIt b_it=bones->begin(); b_it!=bones->end(); b_it++)
				sz				+= sizeof(vecBones::value_type)+(*b_it)->mem_usage();
		}
		return sz;
	}*/
};
#pragma pack(pop)


