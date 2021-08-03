#pragma once
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "windows.h"
#include "xrCommon.h"

struct CEffectorCamVtbl
{
};

enum ECamEffectorType
{
	cefDemo = 0x0,
	cefNext = 0x1,
};

struct CEffectorCam
{
	CEffectorCamVtbl *vfptr;
	ECamEffectorType eType;
	float fLifeTime;
};

struct CEffectorZoomInertion : public CEffectorCam
{
	float m_fFloatSpeed;
	float m_fDispRadius;
	float m_fEpsilon;
	_vector3<float> m_vCurrentPoint;
	_vector3<float> m_vLastPoint;
	_vector3<float> m_vTargetPoint;
	_vector3<float> m_vTargetVel;
	_vector3<float> m_vOldCameraDir;
	unsigned int m_dwTimePassed;
	float m_fCameraMoveEpsilon;
	float m_fDispMin;
	float m_fSpeedMin;
	float m_fZoomAimingDispK;
	float m_fZoomAimingSpeedK;
	unsigned int m_dwDeltaTime;
	CRandom m_Random;
	//
	void CalcNextPoint();
	/*__declspec(dllexport)*/ BOOL Process(_vector3<float> &p, _vector3<float> &d, _vector3<float> &n, 
										 float& fFov, float& fFar, float& fAspect);
};
