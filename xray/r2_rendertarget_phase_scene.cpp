#include "r2_rendertarget.h"
#include "RenderDevice.h"

void	CRenderTarget::phase_scene_prepare	()
{
	// Clear depth & stencil
	u_setrt	( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB );
	CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0L) );

/*	u_setrt	( rt_Position, rt_Normal, NULL, 0 );
	CHK_DX	( HW.pDevice->Clear	( 0L, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0L) );*/

	u32 Offset;
	Fvector2 p0,p1;

	// common 
	RCache.set_CullMode		( CULL_NONE )	;
	RCache.set_Stencil		( FALSE		)	;

	struct v_simple	{
		Fvector4	p;
		Fvector2	uv0;
	};

	float	_w					= float(Device.dwWidth);
	float	_h					= float(Device.dwHeight);
	float	ddw					= 1.f/_w;
	float	ddh					= 1.f/_h;
	p0.set						(.5f/_w, .5f/_h);
	p1.set						((_w+.5f)/_w, (_h+.5f)/_h );

	// Set RT's
	u_setrt(rt_Position, rt_Normal,NULL,HW.pBaseZB);

	// Fill vertex buffer
	v_simple* pv					= (v_simple*) RCache.Vertex.Lock	(4,g_KD->vb_stride,Offset);
		pv->p.set(EPS,			float(_h+EPS),	EPS,1.f); pv->uv0.set(p0.x, p1.y);pv++;
		pv->p.set(EPS,			EPS,			EPS,1.f); pv->uv0.set(p0.x, p0.y);pv++;
		pv->p.set(float(_w+EPS),float(_h+EPS),	EPS,1.f); pv->uv0.set(p1.x, p1.y);pv++;
		pv->p.set(float(_w+EPS),EPS,			EPS,1.f); pv->uv0.set(p1.x, p0.y);pv++;
	RCache.Vertex.Unlock		(4,g_KD->vb_stride);

	// Draw COLOR
	RCache.set_Element			(s_combine->E[5]);
	RCache.set_Geometry			(g_KD);
	RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
}