#pragma once
#include "windows.h"
#include "ResourceManager.h"
#include "Stats.h"
#include "GammaControl.h"
#include "base_types.h"
#include "shader.h"
#include "pureRender.h"

extern	__declspec(dllimport) Flags32		psDeviceFlags		;

class CStats;

// psDeviceFlags
enum {
	rsFullscreen					= (1ul<<0ul),
	rsClearBB						= (1ul<<1ul),
	rsVSync							= (1ul<<2ul),
	rsWireframe						= (1ul<<3ul),
	rsOcclusion						= (1ul<<4ul),
	rsStatistic						= (1ul<<5ul),
	rsDetails						= (1ul<<6ul),
	rsRefresh60hz					= (1ul<<7ul),
	rsConstantFPS					= (1ul<<8ul),
	rsDrawStatic					= (1ul<<9ul),
	rsDrawDynamic					= (1ul<<10ul),
	rsDisableObjectsAsCrows			= (1ul<<11ul),

	rsOcclusionDraw					= (1ul<<12ul),
	rsOcclusionStats				= (1ul<<13ul),

	mtSound							= (1ul<<14ul),
	mtPhysics						= (1ul<<15ul),
	mtNetwork						= (1ul<<16ul),
	mtParticles						= (1ul<<17ul),

	rsCameraPos						= (1ul<<18ul),
	rsR2							= (1ul<<19ul),
	//rsDebugMsg					= (1ul<<20ul), 
	//wtf??? results to fatal error C1001 of the compiler in xrgame

	// 20-32 bit - reserved to Editor
};

struct pureRender;
class pureAppActivate;
class pureAppDeactivate;
class pureAppStart;
class pureAppEnd;
class pureFrame;
class pureDeviceReset;
class fastdelegate_dummy;

class CResourceManager;

class /*__declspec(dllimport)*/ CRenderDevice
{
public:
	unsigned int m_dwWindowStyle; //  4 * 1  4
	RECT m_rcWindowBounds; // 16   4 * 4 16
	RECT m_rcWindowClient; // 16   4 * 4 16
	unsigned int Timer_MM_Delta; //   4 * 1  4  40
	CTimer_paused Timer; // 20        4 * 5 20
	CTimer_paused TimerGlobal; // 20  4 * 5 20  40
	CTimer TimerMM; // 16             4 * 4 16
	HWND__ *m_hWnd;  //               4 * 1  4  20
	unsigned int dwFrame;         //      1
	unsigned int dwPrecacheFrame; //      1
	unsigned int dwPrecacheTotal; //      1
	unsigned int dwWidth;         //      1
	unsigned int dwHeight;        //      1 10
	float fWidth_2;               //      1
	float fHeight_2;              //      1
	int b_is_Ready;               //      1
	int b_is_Active;              //      1 4 34

	ref_shader								m_WireShader;
	ref_shader								m_SelectionShader;

	BOOL									m_bNearer;
public:
	// Registrators
	CRegistrator	<pureRender			>			seqRender;
	CRegistrator	<pureAppActivate	>			seqAppActivate;
	CRegistrator	<pureAppDeactivate	>			seqAppDeactivate;
	CRegistrator	<pureAppStart		>			seqAppStart;
	CRegistrator	<pureAppEnd			>			seqAppEnd;
	CRegistrator	<pureFrame			>			seqFrame;
	CRegistrator	<pureFrame			>			seqFrameMT;
	CRegistrator	<pureDeviceReset	>			seqDeviceReset;
	xr_vector		<fastdelegate_dummy>	seqParallel;
public:
	CResourceManager * Resources;     //468
	CStats *Statistic;                //472
	CGammaControl Gamma;              //476
	float fTimeDelta;                 //504
	float fTimeGlobal;                //508
	unsigned int dwTimeDelta;         //512
	unsigned int dwTimeGlobal;
	unsigned int dwTimeContinual;
	_vector3<float> vCameraPosition;
	_vector3<float> vCameraDirection;
	_vector3<float> vCameraTop;
	_vector3<float> vCameraRight;
	_matrix<float> mView;
	_matrix<float> mProject;
	_matrix<float> mFullTransform;
	_matrix<float> mInvFullTransform;
	float fFOV;
	float fASPECT;
	xrCriticalSection mt_csEnter;
	xrCriticalSection mt_csLeave;
	volatile int mt_bMustExit;
};
extern __declspec(dllimport) CRenderDevice Device;
static u32 frame = 0;