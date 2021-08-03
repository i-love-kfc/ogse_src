#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

struct CTimerBase
{
	unsigned __int64 qwStartTime;
	unsigned __int64 qwPausedTime;
	unsigned __int64 qwPauseAccum;
	int bPause;
};


struct CTimer : public CTimerBase
{
	float m_time_factor;
	unsigned __int64 m_real_ticks;
	unsigned __int64 m_ticks;
};


struct CTimer_paused_ex;
struct CTimer_paused_exVtbl
{
	void *(__thiscall *__vecDelDtor)(CTimer_paused_ex *_this, unsigned int);
};
struct CTimer_paused_ex 
{
	CTimer_paused_exVtbl *vfptr;
	CTimer baseclass_8;
	unsigned __int64 save_clock;
};

struct CTimer_paused : public CTimer_paused_ex
{
};