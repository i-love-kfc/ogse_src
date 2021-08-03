#include "r2_rendertarget.h"
#include "xr_render_console.h"
#include "perf_misc.h"

void CRenderTarget::phase_ssao()
{
	BEGIN_PERF_EVENT(D3DCOLOR_XRGB(0,255,0), L"SSAO");
	render_simple_quad			(rt_ssao,		s_ssao->E[ps_ssao_mode], 1.0);
	END_PERF_EVENT;
};