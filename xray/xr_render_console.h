#pragma once

#include "xrCommon.h"
#include "r__pixel_calculator.h"

#define CMD0(cls)					{ static cls x##cls();				Console->AddCommand(&x##cls);}
#define CMD1(cls,p1)				{ static cls x##cls(p1);			Console->AddCommand(&x##cls);}
#define CMD2(cls,p1,p2)				{ static cls x##cls(p1,p2);			Console->AddCommand(&x##cls);}
#define CMD3(cls,p1,p2,p3)			{ static cls x##cls(p1,p2,p3);		Console->AddCommand(&x##cls);}
#define CMD4(cls,p1,p2,p3,p4)		{ static cls x##cls(p1,p2,p3,p4);	Console->AddCommand(&x##cls);}
#define CMD5(cls,p1,p2,p3,p4,p5)	{ static cls x##cls(p1,p2,p3,p4,p5);	Console->AddCommand(&x##cls);}

//extern __declspec(dllimport) int			ps_r__LightSleepFrames;

extern float		ps_r__Detail_l_ambient	;
extern float		ps_r__Detail_l_aniso;
extern __declspec(dllimport) float		ps_r__Detail_density;
//extern __declspec(dllimport) float		ps_r__Detail_rainbow_hemi;

extern __declspec(dllimport) float		ps_r__Tree_w_rot;
extern __declspec(dllimport) float		ps_r__Tree_w_speed;
extern __declspec(dllimport) float		ps_r__Tree_w_amp;
extern __declspec(dllimport) Fvector	ps_r__Tree_Wave;
extern __declspec(dllimport) float		ps_r__Tree_SBC;

extern __declspec(dllimport) float		ps_r__WallmarkTTL;
/*extern __declspec(dllimport) float		ps_r__WallmarkSHIFT;
extern __declspec(dllimport) float		ps_r__WallmarkSHIFT_V;*/

/*extern __declspec(dllimport) float		ps_r__GLOD_ssa_start;
extern __declspec(dllimport) float		ps_r__GLOD_ssa_end;*/
extern __declspec(dllimport) float		ps_r__LOD;
extern __declspec(dllimport) float		ps_r__ssaDISCARD;
/*extern __declspec(dllimport) float		ps_r__ssaDONTSORT;
extern __declspec(dllimport) float		ps_r__ssaHZBvsTEX;*/

extern __declspec(dllimport) int		ps_r__tf_Anisotropic;
// R1
/*extern __declspec(dllimport) float		ps_r1_ssaLOD_A;
extern __declspec(dllimport) float		ps_r1_ssaLOD_B;
extern __declspec(dllimport) float		ps_r1_tf_Mipbias;
extern __declspec(dllimport) Flags32	ps_r1_flags;
extern __declspec(dllimport) float		ps_r1_lmodel_lerp;
extern __declspec(dllimport) float		ps_r1_dlights_clip;*/
extern __declspec(dllimport) float		ps_r1_pps_u;
extern __declspec(dllimport) float		ps_r1_pps_v;

// R1-specific
extern __declspec(dllimport) int			ps_r1_GlowsPerFrame;

// R2
extern __declspec(dllimport) float		ps_r2_ssaLOD_A;
extern __declspec(dllimport) float		ps_r2_ssaLOD_B;
extern __declspec(dllimport) float		ps_r2_tf_Mipbias;

// R2-specific - gr1ph here for lost alpha presets
extern __declspec(dllimport) Flags32	ps_r2_ls_flags;
extern __declspec(dllimport) float		ps_r2_df_parallax_h;
extern __declspec(dllimport) float		ps_r2_df_parallax_range;
extern __declspec(dllimport) float		ps_r2_tonemap_middlegray;
extern __declspec(dllimport) float		ps_r2_tonemap_adaptation;
extern __declspec(dllimport) float		ps_r2_tonemap_low_lum;
extern __declspec(dllimport) float		ps_r2_tonemap_amount;
extern __declspec(dllimport) float		ps_r2_ls_bloom_kernel_g;
extern __declspec(dllimport) float		ps_r2_ls_bloom_kernel_b;
extern __declspec(dllimport) float		ps_r2_ls_bloom_speed;
extern __declspec(dllimport) float		ps_r2_ls_bloom_kernel_scale;
extern __declspec(dllimport) float		ps_r2_ls_dsm_kernel;
extern __declspec(dllimport) float		ps_r2_ls_psm_kernel;
extern __declspec(dllimport) float		ps_r2_ls_ssm_kernel;
extern __declspec(dllimport) float		ps_r2_ls_bloom_threshold;
extern __declspec(dllimport) Fvector	ps_r2_aa_barier;
extern __declspec(dllimport) Fvector	ps_r2_aa_weight;
extern __declspec(dllimport) float		ps_r2_aa_kernel;
extern __declspec(dllimport) float		ps_r2_mblur;
extern __declspec(dllimport) int		ps_r2_GI_depth;
extern __declspec(dllimport) int		ps_r2_GI_photons;
extern __declspec(dllimport) float		ps_r2_GI_clip;
extern __declspec(dllimport) float		ps_r2_GI_refl;
extern __declspec(dllimport) float		ps_r2_ls_depth_scale;
extern __declspec(dllimport) float		ps_r2_ls_depth_bias;
extern __declspec(dllimport) float		ps_r2_ls_squality;
extern __declspec(dllimport) float		ps_r2_sun_tsm_projection;
extern __declspec(dllimport) float		ps_r2_sun_tsm_bias;
extern __declspec(dllimport) float		ps_r2_sun_near;
extern __declspec(dllimport) float		ps_r2_sun_near_border;
extern __declspec(dllimport) float		ps_r2_sun_depth_far_scale;
extern __declspec(dllimport) float		ps_r2_sun_depth_far_bias;
extern __declspec(dllimport) float		ps_r2_sun_depth_near_scale;
extern __declspec(dllimport) float		ps_r2_sun_depth_near_bias;
extern __declspec(dllimport) float		ps_r2_sun_lumscale;
extern __declspec(dllimport) float		ps_r2_sun_lumscale_hemi;
extern __declspec(dllimport) float		ps_r2_sun_lumscale_amb;
extern __declspec(dllimport) float		ps_r2_gmaterial;
extern __declspec(dllimport) float		ps_r2_zfill;

extern __declspec(dllimport) float		ps_r2_dhemi_scale;
extern __declspec(dllimport) int		ps_r2_dhemi_count;
extern __declspec(dllimport) int		ps_r2_wait_sleep;

extern __declspec(dllimport) float		ps_r2_lt_smooth;
extern __declspec(dllimport) float		ps_r2_slight_fade;

//- Mad Max
extern __declspec(dllimport) float		ps_r2_gloss_factor;

// KD
extern Flags32		ps_r2_test_flags;
extern Flags32		ps_r2_pp_flags;
extern Flags32		ps_r2_dof_flags;
extern u32			ps_ssao_mode;
extern u32			ps_aa_mode;
extern u32			ps_sunshafts_mode;
extern u32			ps_Render_mode;

extern u32			ps_ssao_quality;
extern u32			ps_aa_quality;
extern u32			ps_sunshafts_quality;
extern u32			ps_refl_quality;
extern u32			ps_dof_quality;
extern u32			ps_soft_shadows;
extern u32			ps_steep_parallax;
extern u32			ps_r__detail_radius;
extern Fvector		ps_r2_details_opt;
extern int			rmap_size;

extern Fvector		ps_r2_rain_params;
extern float		center_height;

extern Fvector4		ps_r2_color_grading_params;

extern Fvector		ps_r2_blur_params;
extern float		ps_r2_ndr;

extern u32			ps_light_shadow_quality;

//rain map
extern float		ps_r2_rmap_tsm_projection;
extern float		ps_r2_rmap_tsm_bias;
extern float		ps_r2_rmap_near	;
extern float		ps_r2_rmap_near_border;
extern float		ps_r2_rmap_depth_far_scale;
extern float		ps_r2_rmap_depth_far_bias;
extern float		ps_r2_rmap_depth_near_scale;
extern float		ps_r2_rmap_depth_near_bias;

extern float		ps_r2_sharpening;

extern Flags32		ps_service_flags;
extern u32			ps_refl_mode;

extern float		ps_r2_ao_intensity;
extern float		ssa_discard;


enum
{
	R1FLAG_DLIGHTS				= (1<<0),
};

enum
{
	R2FLAG_DOF_COMMON			= (1<<0),
	R2FLAG_DOF_ZOOM				= (1<<1),
	R2FLAG_DOF_RELOAD			= (1<<2),
};

enum
{
	R2FLAG_SUN					= (1<<0),
	R2FLAG_SUN_FOCUS			= (1<<1),
	R2FLAG_SUN_TSM				= (1<<2),
	R2FLAG_SUN_DETAILS			= (1<<3),
	R2FLAG_TONEMAP				= (1<<4),
	R2FLAG_AA					= (1<<5),
	R2FLAG_GI					= (1<<6),
	R2FLAG_FASTBLOOM			= (1<<7),
	R2FLAG_GLOBALMATERIAL		= (1<<8),		// not used in release build
	R2FLAG_ZFILL				= (1<<9),		// used to enabled z-prepass
	R2FLAG_R1LIGHTS				= (1<<10),
	R2FLAG_SUN_IGNORE_PORTALS	= (1<<11),		// no console command, but used in sun render

	R2FLAG_VOLLIGHT			= (1<<12),	// added by kd instead of R2FLAG_SUN_STATIC which is not used at all
	
	R2FLAG_EXP_SPLIT_SCENE					= (1<<13),	// no console command, but used in main render
	R2FLAG_EXP_DONT_TEST_UNSHADOWED			= (1<<14),	// no console command, but used in lights render

	R2FLAG_USE_NVDBT			= (1<<15),		// used to enable NVidia depth bound test. On modern cards there is no difference in performance.
	R2FLAG_USE_NVSTENCIL		= (1<<16),		// not used, but always set up in render flag.

	R2FLAG_EXP_MT_CALC			= (1<<17),		// no console command, but used in details and hom render

	// added by kd
	R2FLAG_SOFT_WATER			= (1<<18),
	R2FLAG_SOFT_PARTICLES		= (1<<19),
	R2FLAG_TRUE_SHADOWS			= (1<<20),
	R2FLAG_BLOODMARKS			= (1<<21),
	R2FLAG_DETAIL_BUMP_DEBUG	= (1<<22),
	R2FLAG_FOG_VOLUME			= (1<<23),
	R2FLAG_GBUFFER_OPT			= (1<<24),
	R2FLAG_LENS_FLARES			= (1<<25),
	R2FLAG_LENS_DIRT			= (1<<26),
	R2FLAG_WET_SURFACES			= (1<<27),
	R2FLAG_PUDDLES				= (1<<28),
	R2FLAG_AO					= (1<<29),
};

// enum for my test stuff :)
enum
{
	R2FLAG_RAIN_MAP				= (1<<0),
	R2FLAG_PCSS					= (1<<1),
	R2FLAG_HW_INSTANSING		= (1<<2),
	R2FLAG_REPROJECT			= (1<<3),
};

enum
{
	S_EXTENDED_GRAPH_STATS		= (1<<0),
};

enum
{
	R2PP_FLAG_AA				= (1<<0),
	R2PP_FLAG_MBLUR				= (1<<1),
	R2PP_FLAG_DOF				= (1<<2),
	R2PP_FLAG_RAIN_DROPS		= (1<<3),
	R2PP_FLAG_SUNSHAFTS			= (1<<4),
	R2PP_FLAG_SSAO				= (1<<5),
	R2PP_FLAG_SUPERSAMPLING_AA	= (1<<6),
	R2PP_FLAG_REFLECTIONS		= (1<<7),
	R2PP_FLAG_RAIN_DROPS_CONTROL	= (1<<8),
	R2PP_FLAG_LENS_DIRT_CONTROL	= (1<<9),
	R2PP_FLAG_DOF_CONTROL		= (1<<10),
	R2PP_FLAG_LIGHT_SHADOW_FADE	= (1<<11),
};

enum
{
	R2RM_NORMAL,
	R2RM_THERMAL,
};

enum
{
	R2AA_FXAA,
	R2AA_SMAA_1X,
	R2AA_SMAA_T2X,
};

enum
{
	R2SS_VOLUMETRIC,
	R2SS_SCREEN_SPACE,
};

void	xrRender_apply_tf		();

extern Fmatrix sun_near_shadow_xform;
extern Fmatrix sun_far_shadow_xform;
extern Fmatrix sun_clouds_shadow_xform;

extern bool actor_torch_enabled;
extern bool is_torch_processed;
//extern int is_lods_limited;