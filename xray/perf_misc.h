/*#ifdef _OGSE_DEBUG
	#define BEGIN_PERF_EVENT(a,b)	D3DPERF_BeginEvent(a,b)
	#define END_PERF_EVENT			D3DPERF_EndEvent()
#else*/
	#define BEGIN_PERF_EVENT(a,b)	do {} while (0)
	#define END_PERF_EVENT			do {} while (0)
//#endif