#pragma once
#include "xrXRC.h"
#include "Render.h"
#include <Windows.h>

class CGameFont;
class IBlender;
class CFrustum;
struct IRender_interfaceVtbl;

class IRender_interface
{
public:
	enum GenerationLevel
	{
	  GENERATION_R1 = 0x51,
	  GENERATION_DX81 = 0x51,
	  GENERATION_R2 = 0x5A,
	  GENERATION_DX90 = 0x5A,
	  GENERATION_forcedword = 0xFFFFFFFF,
	};
	enum ScreenshotMode
	{
	  SM_NORMAL = 0x0,
	  SM_FOR_CUBEMAP = 0x1,
	  SM_FOR_GAMESAVE = 0x2,
	  SM_FOR_LEVELMAP = 0x3,
	  SM_forcedword = 0xFFFFFFFF,
	};

  IRender_interfaceVtbl *vfptr;
  int m_skinning;
  CFrustum ViewBase;
  CFrustum *View;

  IC	void				set_Frustum					(CFrustum*	O) { VERIFY(O);	View = O;			};
  void						add_Geometry				(IRender_Visual*	V	);
  IRender_ObjectSpecific*	ros_create					(IRenderable* parent);
  void						Screenshot					(ScreenshotMode mode=SM_NORMAL, LPCSTR name = 0);
   void						light_destroy				(IRender_Light* light);
   void						Statistics					(CGameFont *f);
   IRender_interface::GenerationLevel get_generation	();
};
struct IRender_interfaceVtbl
{
  IRender_interface::GenerationLevel (__thiscall *get_generation)(IRender_interface *_this);
  char dummy[0x20];
/*  void (__thiscall *create)(IRender_interface *_this);
  void (__thiscall *destroy)(IRender_interface *_this);
  void (__thiscall *reset_begin)(IRender_interface *_this);
  void (__thiscall *reset_end)(IRender_interface *_this);
  void (__thiscall *level_Load)(IRender_interface *_this, IReader *);
  void (__thiscall *level_Unload)(IRender_interface *_this);
  IDirect3DBaseTexture9 *(__thiscall *texture_load)(IRender_interface *_this, const char *, unsigned int *);
  HRESULT (__thiscall *shader_compile)(IRender_interface *_this, const char *, const char *, unsigned int, void *, void *, const char *, const char *, unsigned int, void *, void *, void *);*/
  void (__thiscall *Statistics)(IRender_interface *_this, CGameFont *);
  char dummy3[0x38];
/*  const char *(__thiscall *getShaderPath)(IRender_interface *_this);
  ref_shader *(__thiscall *getShader)(IRender_interface *_this, ref_shader *result, int);
  IRender_Sector *(__thiscall *getSector)(IRender_interface *_this, int);
  IRender_Visual *(__thiscall *getVisual)(IRender_interface *_this, int);
  IRender_Sector *(__thiscall *detectSector)(IRender_interface *_this, _vector3<float> *);
  IRender_Target *(__thiscall *getTarget)(IRender_interface *_this);
  void (__thiscall *set_Transform)(IRender_interface *_this, _matrix<float> *);
  void (__thiscall *set_HUD)(IRender_interface *_this, int);
  int (__thiscall *get_HUD)(IRender_interface *_this);
  void (__thiscall *set_Invisible)(IRender_interface *_this, int);
  void (__thiscall *flush)(IRender_interface *_this);
  void (__thiscall *set_Object)(IRender_interface *_this, IRenderable *);
  void (__thiscall *add_Occluder)(IRender_interface *_this, _box2<float> *);
  void (__thiscall *add_Visual)(IRender_interface *_this, IRender_Visual *);*/
  void (__thiscall *add_Geometry)(IRender_interface *_this, IRender_Visual *);
  char dummy1[0x24];/*
  void (__thiscall *add_StaticWallmark)(IRender_interface *_this, ref_shader *, _vector3<float> *, float, CDB::TRI *, _vector3<float> *);
  void (__thiscall *clear_static_wallmarks)(IRender_interface *_this);
  void (__thiscall *add_SkeletonWallmark)(IRender_interface *_this, _matrix<float> *, CKinematics *, ref_shader *, _vector3<float> *, _vector3<float> *, float);
  void (__thiscall *_add_SkeletonWallmark)(IRender_interface *_this, intrusive_ptr<CSkeletonWallmark,intrusive_base>);
  IBlender *(__thiscall *blender_create)(IRender_interface *_this, unsigned __int64);
  void (__thiscall *blender_destroy)(IRender_interface *_this, IBlender **);
  IRender_ObjectSpecific *(__thiscall *ros_create)(IRender_interface *_this, IRenderable *);
  void (__thiscall *ros_destroy)(IRender_interface *_this, IRender_ObjectSpecific **);
  IRender_Light *(__thiscall *light_create)(IRender_interface *_this);*/
  void (__thiscall *light_destroy)(IRender_interface *_this, IRender_Light *);
  	char dummy2[0x44];
/*  IRender_Glow *(__thiscall *glow_create)(IRender_interface *_this);
  void (__thiscall *glow_destroy)(IRender_interface *_this, IRender_Glow *);
  IRender_Visual *(__thiscall *model_CreateParticles)(IRender_interface *_this, const char *);
  IRender_DetailModel *(__thiscall *model_CreateDM)(IRender_interface *_this, IReader *);
  IRender_Visual *(__thiscall *model_Create)(IRender_interface *_this, const char *, IReader *);
  IRender_Visual *(__thiscall *model_CreateChild)(IRender_interface *_this, const char *, IReader *);
  IRender_Visual *(__thiscall *model_Duplicate)(IRender_interface *_this, IRender_Visual *);
  void (__thiscall *model_Delete)(IRender_interface *_this, IRender_DetailModel **);
  void (__thiscall *_model_Delete)(IRender_interface *_this, IRender_Visual **, int);
  void (__thiscall *model_Logging)(IRender_interface *_this, int);
  void (__thiscall *models_Prefetch)(IRender_interface *_this);
  void (__thiscall *models_Clear)(IRender_interface *_this, int);
  int (__thiscall *occ_visible)(IRender_interface *_this, svector<_vector3<float>,48> *);
  int (__thiscall *_occ_visible)(IRender_interface *_this, _box3<float> *);
  int (__thiscall *__occ_visible)(IRender_interface *_this, vis_data *);
  void (__thiscall *Calculate)(IRender_interface *_this);
  void (__thiscall *Render)(IRender_interface *_this);*/
  void (__thiscall *Screenshot)(IRender_interface *_this, IRender_interface::ScreenshotMode, const char *);
/*  void (__thiscall *rmNear)(IRender_interface *_this);
  void (__thiscall *rmFar)(IRender_interface *_this);
  void (__thiscall *rmNormal)(IRender_interface *_this);
  unsigned int (__thiscall *memory_usage)(IRender_interface *_this);
  void *(__thiscall *__vecDelDtor)(IRender_interface *_this, unsigned int);*/
};
extern _declspec(dllimport) IRender_interface *Render;