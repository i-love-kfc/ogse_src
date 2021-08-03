#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
//#include <vector>
#include "xrCommon.h"

class CCameraBase;
struct CCameraBaseVtbl
{
  void *(__thiscall *__vecDelDtor)(CCameraBase *this_, unsigned int);
  void (__thiscall *Load)(CCameraBase *this_, const char *);
  void (__thiscall *OnActivate)(CCameraBase *this_, CCameraBase *);
  void (__thiscall *OnDeactivate)(CCameraBase *this_);
  void (__thiscall *Move)(CCameraBase *this_, int, float, float);
  void (__thiscall *Update)(CCameraBase *this_, Fvector &, Fvector &);
  void (__thiscall *Get)(CCameraBase *this_, Fvector &, Fvector &, Fvector &);
  void (__thiscall *Set)(CCameraBase *this_, float, float, float);
  void (__thiscall *Set1)(CCameraBase *this_, Fvector &, Fvector &, Fvector &);
  float (__thiscall *GetWorldYaw)(CCameraBase *this_);
  float (__thiscall *GetWorldPitch)(CCameraBase *this_);
  float (__thiscall *CheckLimYaw)(CCameraBase *this_);
  float (__thiscall *CheckLimPitch)(CCameraBase *this_);
  float (__thiscall *CheckLimRoll)(CCameraBase *this_);
};
enum ECameraStyle {
	csCamDebug,
	csFirstEye,
	csLookAt,
	csMax,
	csFixed,
	cs_forcedword,
};
class CObject;
class /*__declspec(dllimport)*/ CCameraBase
{
public:
	CCameraBaseVtbl *vfptr;
	CObject *parent;
	int bClampYaw;
	int bClampPitch;
	int bClampRoll;
	float yaw;
	float pitch;
	float roll;
	DWORD m_Flags;
	ECameraStyle style;
	_vector2<float> lim_yaw;
	_vector2<float> lim_pitch;
	_vector2<float> lim_roll;
	Fvector rot_speed;
	Fvector vPosition;
	Fvector vDirection;
	Fvector vNormal;
	float f_fov;
	float f_aspect;
	int tag;
};
class CEffectorCam;
class CEffectorPP;
typedef xr_vector<CEffectorCam*> EffectorCamVec;
typedef xr_vector<CEffectorPP*> EffectorPPVec;

class /*__declspec(dllimport)*/ CCameraManager {
public:
	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;
	Fvector					vRight;

	EffectorCamVec			m_EffectorsCam;
	EffectorCamVec			m_EffectorsCam_added_deffered;
	EffectorCamVec			m_EffectorsCam_removed_deffered;
	EffectorPPVec			m_EffectorsPP;

	float					fFov;
	float					fFar;
	float					fAspect;
	bool					m_bAutoApply;
	//SPPInfo					pp_affected;
	/*__declspec(dllexport)*/ void Update3(const CCameraBase* C);
	void /*CCameraManager::*/Update(
		Fvector const & P,
		Fvector const & D,
		Fvector const & H,
		float fFOV_Dest,
		float fASPECT_Dest,
		float fFAR_Dest,
		unsigned int flags);
	void Update (const CCameraBase* );
};

void print_pp_effect(CCameraManager* cm);

