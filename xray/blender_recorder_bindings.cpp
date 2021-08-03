#pragma once
#include "r2.h"
#include "r2_rendertarget.h"
#include "shader.h"
#include "xr_3da.h"
#include "RenderDevice.h"
#include "Game_Persistent.h"
#include "Backend.h"
#include "xr_render_console.h"
#include "effect_rain.h"
//#include "cameramanager.h"
//#include "game_level.h"

// matrices
#define	BIND_DECLARE(xf)	\
class cl_xform_##xf	: public R_constant_setup {	virtual void setup (R_constant* C) { RCache.xforms.set_c_##xf (C); } }; \
	static cl_xform_##xf	binder_##xf
BIND_DECLARE(w);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

// fog
class cl_fog_plane	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C)
	{
		if (marker!=Device.dwFrame)
		{
			// Plane
			Fvector4		plane;
			Fmatrix&	M	= Device.mFullTransform;
			plane.x			= -(M._14 + M._13);
			plane.y			= -(M._24 + M._23);
			plane.z			= -(M._34 + M._33);
			plane.w			= -(M._44 + M._43);
			float denom		= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
			plane.mul		(denom);

			// Near/Far
			float A			= g_pGamePersistent->Environment().CurrentEnv.fog_near;
			float B			= 1/(g_pGamePersistent->Environment().CurrentEnv.fog_far - A);

			result.set		(-plane.x*B, -plane.y*B, -plane.z*B, 1 - (plane.w-A)*B	);								// view-plane
		}
		RCache.set_c	(C,result);
	}
};	static cl_fog_plane		binder_fog_plane;

// fog-params
class cl_fog_params	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C)
	{
		if (marker!=Device.dwFrame)
		{
			// Near/Far
			float	n		= g_pGamePersistent->Environment().CurrentEnv.fog_near	;
			float	f		= g_pGamePersistent->Environment().CurrentEnv.fog_far		;
			float	r		= 1/(f-n);
			result.set		(-n*r, r, r, r);
//			Msg("fog_params: %f,%f",n,	f);
		}
		RCache.set_c	(C,result);
	}
};	static cl_fog_params	binder_fog_params;

// fog-color
class cl_fog_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptor&	desc	= g_pGamePersistent->Environment().CurrentEnv;
			result.set				(desc.fog_color.x,	desc.fog_color.y, desc.fog_color.z,	(float)Device.dwFrame);
//			Msg("fog_color: %f,%f,%f",desc.fog_color.x,	desc.fog_color.y, desc.fog_color.z);
		}
		RCache.set_c	(C,result);
	}
};	static cl_fog_color		binder_fog_color;

// times
class cl_times		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		float 		t	= Device.fTimeGlobal;

		RCache.set_c	(C,t,t*10,t/10,_sin(t))	;
	}
};
static cl_times		binder_times;

// eye-params
class cl_eye_P		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraPosition;
		RCache.set_c	(C,V.x,V.y,V.z,1);
	}
};
static cl_eye_P		binder_eye_P;

// eye-params
class cl_eye_D		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraDirection;
		RCache.set_c	(C,V.x,V.y,V.z,0);
	}
};
static cl_eye_D		binder_eye_D;

// eye-params
class cl_eye_N		: public R_constant_setup {
	virtual void setup(R_constant* C)
	{
		Fvector&		V	= Device.vCameraTop;
		RCache.set_c	(C,V.x,V.y,V.z,0);
	}
};
static cl_eye_N		binder_eye_N;

// D-Light0
class cl_sun0_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptor&	desc	= g_pGamePersistent->Environment().CurrentEnv;
			result.set				(desc.sun_color.x,	desc.sun_color.y, desc.sun_color.z,	0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_color		binder_sun0_color;
class cl_sun0_dir_w	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptor&	desc	= g_pGamePersistent->Environment().CurrentEnv;
			result.set				(desc.sun_dir.x,	desc.sun_dir.y, desc.sun_dir.z,	0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_dir_w		binder_sun0_dir_w;
class cl_sun0_dir_e	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			Fvector D;
			CEnvDescriptor&	desc		= g_pGamePersistent->Environment().CurrentEnv;
			Device.mView.transform_dir	(D,desc.sun_dir);
			D.normalize					();
			result.set					(D.x,D.y,D.z,0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sun0_dir_e		binder_sun0_dir_e;

//
class cl_amb_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptorMixer&	desc	= g_pGamePersistent->Environment().CurrentEnv;
			result.set				(desc.ambient.x, desc.ambient.y, desc.ambient.z, desc.weight);
		}
		RCache.set_c	(C,result);
	}
};	static cl_amb_color		binder_amb_color;
class cl_env_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptorMixer& desc	= g_pGamePersistent->Environment().CurrentEnv;
			Fvector4	envclr			= { desc.sky_color.x*2+EPS,	desc.sky_color.y*2+EPS,	desc.sky_color.z*2+EPS,	desc.weight					};
			envclr.x		*= 2*(ps_r2_sun_lumscale_hemi); 
			envclr.y		*= 2*(ps_r2_sun_lumscale_hemi); 
			envclr.z		*= 2*(ps_r2_sun_lumscale_hemi);
			result.set		(envclr);
		}
		RCache.set_c	(C,result);
	}
};	static cl_env_color		binder_env_color;
class cl_hemi_color	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			CEnvDescriptor&	desc	= g_pGamePersistent->Environment().CurrentEnv;
			result.set				(desc.hemi_color.x, desc.hemi_color.y, desc.hemi_color.z, desc.hemi_color.w);
		}
		RCache.set_c	(C,result);
	}
};	static cl_hemi_color		binder_hemi_color;

// KD binders
class cl_resolution	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			float _w = float(Device.dwWidth);
			float _h = float(Device.dwHeight);
			result.set (_w, _h, (float)1.0/_w, (float)1.0/_h);
		}
		RCache.set_c	(C,result);
	}
};	static cl_resolution		binder_resolution;
class cl_screen_params	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			float _ffov = float(Device.fFOV);
			float _faspect = float(Device.fASPECT);
			result.set (_ffov, _faspect, /*Device.mProject._43**/tan(deg2rad(_ffov)/2), g_pGamePersistent->Environment().CurrentEnv.far_plane*0.75f/*g_pGameLevel->m_pCameras->fFar*/);
		}
		RCache.set_c	(C,result);
	}
};	static cl_screen_params		binder_screen_params;
class cl_dec_params	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			float _ffov = float(Device.fFOV);
			float _faspect = float(Device.fASPECT);
			float _tan = tan(deg2rad(_ffov)/2.f);
			result.set (_tan/_faspect, -_tan, _tan/_faspect*2.f/float(Device.dwWidth), -_tan*2.f/float(Device.dwHeight));
		}
		RCache.set_c	(C,result);
	}
};	static cl_dec_params		binder_dec_params;
/*
class cl_jitter_params	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			float _w = float(Device.dwWidth);
			float _h = float(Device.dwHeight);
			Fvector2 jitter[2];
			jitter[0].set(0.25f/_w, -0.25f/_h);
			jitter[1].set(-0.25f/_w, 0.25f/_h);
			if (ps_aa_mode == R2AA_SMAA_T2X)
				result.set (jitter[frame].x*2.0, jitter[frame].y*2.0, 0.0, 0.0);
			else
				result.set (0.0, 0.0, 0.0, 0.0);
		}
		RCache.set_c	(C,result);
	}
};	static cl_jitter_params		binder_jitter_params;
*/
class cl_rain_params	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			result.set(rain_params);
		}
		RCache.set_c	(C,result);
	}
};	static cl_rain_params		binder_rain_params;

class cl_inv_v	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
//		if (marker!=Device.dwFrame)	{
			result.invert(Device.mView);
//		}
		RCache.set_c	(C,result);
	}
};	static cl_inv_v		binder_inv_v;
class cl_sns_matrix	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			result.set	(sun_near_shadow_xform);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sns_matrix		binder_sns_matrix;

class cl_sfs_matrix	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			result.set	(sun_far_shadow_xform);
		}
		RCache.set_c	(C,result);
	}
};	static cl_sfs_matrix		binder_sfs_matrix;
class cl_scs_matrix	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
//		if (marker!=Device.dwFrame)	{
			result.set	(sun_clouds_shadow_xform);
//		}
		RCache.set_c	(C,result);
	}
};	static cl_scs_matrix		binder_scs_matrix;

class cl_artifacts	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
/*			for (int i = 0; i < 8; ++i)
				Msg("AP[%d]: %f, %f", i, artefacts_position[i].x, artefacts_position[i].y);*/
			result._11 = artefacts_position[0].x;
			result._12 = artefacts_position[0].y;
			result._13 = artefacts_position[1].x;
			result._14 = artefacts_position[1].y;
			result._21 = artefacts_position[2].x;
			result._22 = artefacts_position[2].y;
			result._23 = artefacts_position[3].x;
			result._24 = artefacts_position[3].y;
			result._31 = artefacts_position[4].x;
			result._32 = artefacts_position[4].y;
			result._33 = artefacts_position[5].x;
			result._34 = artefacts_position[5].y;
			result._41 = artefacts_position[6].x;
			result._42 = artefacts_position[6].y;
			result._43 = artefacts_position[7].x;
			result._44 = artefacts_position[7].y;
		}
		RCache.set_c	(C,result);
	}
};	static cl_artifacts		binder_artifacts;
class cl_anomalys	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
/*			for (int i = 0; i < 8; ++i)
				Msg("AP[%d]: %f, %f", i, artefacts_position[i].x, artefacts_position[i].y);*/
			result._11 = anomalys_position[0].x;
			result._12 = anomalys_position[0].y;
			result._13 = anomalys_position[1].x;
			result._14 = anomalys_position[1].y;
			result._21 = anomalys_position[2].x;
			result._22 = anomalys_position[2].y;
			result._23 = anomalys_position[3].x;
			result._24 = anomalys_position[3].y;
			result._31 = anomalys_position[4].x;
			result._32 = anomalys_position[4].y;
			result._33 = anomalys_position[5].x;
			result._34 = anomalys_position[5].y;
			result._41 = anomalys_position[6].x;
			result._42 = anomalys_position[6].y;
			result._43 = anomalys_position[7].x;
			result._44 = anomalys_position[7].y;
		}
		RCache.set_c	(C,result);
	}
};	static cl_anomalys		binder_anomalys;
class cl_various	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
//		if (marker!=Device.dwFrame)	{
			BOOL sun = RImplementation.is_sun();
			result.set(g_pGamePersistent->Environment().CurrentEnv.params_ex->moon_road_intensity, sun, ps_r2_sun_near, actor_torch_enabled);
//		}
		RCache.set_c	(C,result);
	}
};	static cl_various		binder_various;
class cl_detector	: public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			result.set((float)detector_params.x, (float)detector_params.y, 0.f, 0.f);
		}
		RCache.set_c	(C,result);
	}
};	static cl_detector		binder_detector;
/*
class cl_inv_p	: public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup	(R_constant* C)	{
		if (marker!=Device.dwFrame)	{
			result.invert(Device.mProject);
		}
		RCache.set_c	(C,result);
	}
};	static cl_inv_p		binder_inv_p;*/
// Standart constant-binding
void	CBlender_Compile::SetMapping	()
{
	// matrices
	r_Constant				("m_W",				&binder_w);
	r_Constant				("m_V",				&binder_v);
	r_Constant				("m_P",				&binder_p);
	r_Constant				("m_WV",			&binder_wv);
	r_Constant				("m_VP",			&binder_vp);
	r_Constant				("m_WVP",			&binder_wvp);

	// fog-params
	r_Constant				("fog_plane",		&binder_fog_plane);
	r_Constant				("fog_params",		&binder_fog_params);
	r_Constant				("fog_color",		&binder_fog_color);

	// time
	r_Constant				("timers",			&binder_times);

	// eye-params
	r_Constant				("eye_position",	&binder_eye_P);
	r_Constant				("eye_direction",	&binder_eye_D);
	r_Constant				("eye_normal",		&binder_eye_N);

	// global-lighting (env params)
	r_Constant				("L_sun_color",		&binder_sun0_color);
	r_Constant				("L_sun_dir_w",		&binder_sun0_dir_w);
	r_Constant				("L_sun_dir_e",		&binder_sun0_dir_e);
	r_Constant				("L_hemi_color",	&binder_hemi_color);
	r_Constant				("L_ambient",		&binder_amb_color);
	r_Constant				("env_color",		&binder_env_color);

	// KD binders
	r_Constant				("ogse_c_resolution",	&binder_resolution);
	r_Constant				("ogse_c_screen",		&binder_screen_params);
//	r_Constant				("ogse_pos_decompression",		&binder_dec_params);
//	r_Constant				("ogse_c_jitter",		&binder_jitter_params);
	r_Constant				("ogse_c_rain",			&binder_rain_params);
	r_Constant				("ogse_c_artefacts",	&binder_artifacts);
	r_Constant				("ogse_c_anomalys",		&binder_anomalys);
	r_Constant				("ogse_c_various",		&binder_various);
	r_Constant				("ogse_c_detector",		&binder_detector);
	r_Constant				("m_v2w",				&binder_inv_v);
	r_Constant				("m_shadow_near_ogse",		&binder_sns_matrix);
	r_Constant				("m_shadow_far_ogse",		&binder_sfs_matrix);
	r_Constant				("m_shadow_clouds_ogse",	&binder_scs_matrix);
//	r_Constant				("m_InvP",				&binder_inv_p);

	// detail
	if (bDetail	&& detail_scaler)
		r_Constant			("dt_params",		detail_scaler);

	// other common
	for (u32 it=0; it<Device.Resources->v_constant_setup.size(); it++)
	{
		std::pair<shared_str,R_constant_setup*>	cs	= Device.Resources->v_constant_setup[it];
		r_Constant			(*cs.first,cs.second);
	}
}
