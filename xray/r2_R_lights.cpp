#include "r2_rendertarget.h"
#include "r2.h"
#include "xr_render_console.h"

void	CRender::render_indirect			(light* L)
{
	if (!ps_r2_ls_flags.test(R2FLAG_GI))	return;

	light									LIGEN;
	LIGEN.set_type							(IRender_Light::REFLECTED);
	LIGEN.set_shadow						(false);
	LIGEN.set_cone							(PI_DIV_2*2.f);

	xr_vector<light_indirect>&	Lvec		= L->indirect;
	if (Lvec.empty())						return;
	float	LE								= L->color.intensity	();
	for (u32 it=0; it<Lvec.size(); it++)	{
		light_indirect&	LI				= Lvec[it];

		// energy and color
		float	LIE						= LE*LI.E;
		if (LIE < ps_r2_GI_clip)		continue;
		Fvector T; T.set(L->color.r,L->color.g,L->color.b).mul(LI.E);
		LIGEN.set_color					(T.x,T.y,T.z);

		// geometric
		Fvector L_up,L_right;			L_up.set	(0,1,0);	if (_abs(L_up.dotproduct(LI.D))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct			(L_up,LI.D).normalize	();
		LIGEN.spatial.sector			= LI.S;
		LIGEN.set_position				(LI.P);
		LIGEN.set_rotation				(LI.D,L_right);

		// range
		// dist^2 / range^2 = A - has infinity number of solutions
		// approximate energy by linear fallof Emax / (1 + x) = Emin
		float	Emax					= LIE;
		float	Emin					= 1.f / 255.f;
		float	x						= (Emax - Emin)/Emin;
		if		(x < 0.1f)				continue;
		LIGEN.set_range					(x);

		Target->accum_reflected			(&LIGEN);
	}
}
