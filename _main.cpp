#include <lua.hpp>
#include "_additional_libraries.h"
#include "_head.h"

//#include "xray\test_tamplates.h"

#include "xray\xr_3da.h"
#include "xray\Environment.h"
#include "xray\Game_Persistent.h"
#include "xray\IniFile.h"

#include <Windows.h>
#include <tlhelp32.h> 
#include "global_ns.h"
#include "math.h"
#include "volumetric_fog_lua.h"
#include "xray\RenderDevice.h"
#include "xray\object.h"
//#include "lights_lua.h"

//#include "internal.h"
//#include "xray\xr_render_console.h"

unsigned int BASE_ADDRESS_XRRENDER_R2;
unsigned int BASE_ADDRESS_XRGAME;

int set_weather(lua_State *L);
int get_weather_time(lua_State *L);
int get_sun_direction(lua_State *L);
int set_artefact_slot(lua_State *L);
int set_anomaly_slot(lua_State *L);
int set_detector_mode(lua_State *L);

BOOL ListProcessModules( DWORD dwPID ); 
BOOL InitGlobalVariables();

//Library entry point
extern "C" int luaopen_ogse(lua_State *L){
	open_bit(L);
	open_global_ns(L);
	open_math(L);
	open_volumetric_fog(L);
//	open_lights(L);
	//lua_register(L, "MySetWeather",		set_weather);
	//lua_register(L, "GetWeatherTime",		get_weather_time);
	lua_register(L, "get_sun_direction",	get_sun_direction);	
	lua_register(L, "set_artefact_slot",	set_artefact_slot);
	lua_register(L, "set_anomaly_slot",		set_anomaly_slot);
	lua_register(L, "set_detector_mode",	set_detector_mode);
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) 
	{
		/* init variables from render dll */
/*		InitGlobalVariables();*/
		Msg("using OGSE.DLL rev.%d build %s", REV, BUILD_DATE);
	}
	return TRUE;
}

Fvector2 artefacts_position[8];
int set_artefact_slot(lua_State *L)
{
	if (lua_gettop(L) == 4)
	{
		Fvector pos;
		int i  = luaL_checkinteger(L, 1);
		pos.x  = luaL_checkinteger(L, 2);
		pos.y  = luaL_checkinteger(L, 3);
		pos.z  = luaL_checkinteger(L, 4);

		Fvector2 art_dir, cam_dir;
		art_dir.set(pos.x - Device.vCameraPosition.x, pos.z - Device.vCameraPosition.z);
		art_dir.normalize();
		cam_dir.set(Device.vCameraDirection.x, Device.vCameraDirection.z);

		if (fis_zero(pos.x) && fis_zero(pos.y) && fis_zero(pos.z))
			artefacts_position[i].set(0.f, 0.f);
		else
		{
			Device.mView.transform_tiny(pos);
			artefacts_position[i].set(pos.x, pos.z);
		}

		return 0;
	}
	return luaL_error(L, "set_artefact_slot must have four arguments");
}
Ivector3 detector_params;
int set_detector_mode(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		int _mode  = luaL_checkinteger(L, 1);
		int _block  = luaL_checkinteger(L, 2);

		detector_params.set(_mode, _block, 0);

		return 0;
	}
	return luaL_error(L, "set_detector_mode must have two arguments");
}
Fvector2 anomalys_position[8];
int set_anomaly_slot(lua_State *L)
{
	if (lua_gettop(L) == 4)
	{
		Fvector pos;
		int i  = luaL_checkinteger(L, 1);
		pos.x  = luaL_checkinteger(L, 2);
		pos.y  = luaL_checkinteger(L, 3);
		pos.z  = luaL_checkinteger(L, 4);

		Fvector2 art_dir, cam_dir;
		art_dir.set(pos.x - Device.vCameraPosition.x, pos.z - Device.vCameraPosition.z);
		art_dir.normalize();
		cam_dir.set(Device.vCameraDirection.x, Device.vCameraDirection.z);

		if (fis_zero(pos.x) && fis_zero(pos.y) && fis_zero(pos.z))
			anomalys_position[i].set(0.f, 0.f);
		else
		{
			Device.mView.transform_tiny(pos);
			anomalys_position[i].set(pos.x, pos.z);
		}

		return 0;
	}
	return luaL_error(L, "set_anomaly_slot must have four arguments");
}
/*
int set_visible(lua_State *L)
{
	if (lua_gettop(L) == 2)
	{
		CObject* obj = (CObject *)luaL_checkinteger(L, 1);
		int is_visible = luaL_checkinteger(L, 2);
		obj->setVisible(is_visible);

		return 0;
	}
	return luaL_error(L, "set_visible must have two arguments");
}
*/
int set_weather(lua_State *L){
	if (lua_gettop(L) == 1)
	{
		const char *str  = luaL_checkstring(L, 1);
		CInifile* ini = CInifile::Create(str, 1);
		CInifile* old_settings = pSettings;
		//pSettings = ini;
		//Log(str);
		CEnvironment & environment = g_pGamePersistent->Environment();
		environment.unload();
		environment.load();
		CInifile::Destroy(ini);
		//pSettings = old_settings;
		return 0;
	}
	return luaL_error(L, "set_weather must have one argument");
}
int get_sun_direction(lua_State *L) {
	if (lua_gettop(L) == 0)
	{
		CEnvironment & environment = g_pGamePersistent->Environment();
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.x);
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.y);
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.z);
		return 3;
	}
	return luaL_error(L, "get_sun_direction must have no arguments");
}
int get_weather_time(lua_State *L) {
	if (lua_gettop(L) == 1)
	{
		int pos  = luaL_checkinteger(L, 1);
		CEnvironment & environment = g_pGamePersistent->Environment();
		unsigned char*addr = ((unsigned char*)&environment + pos);
		unsigned char c = *addr;
		unsigned int i = *(unsigned int*)addr;
		float f = *(float*)addr;
		int ss = sizeof(environment.CloudsIndices);
		int ss2 = sizeof(environment.m_b_skybox);
		
		//float t = environment.fGameTime;
		
		//float t = environment.fTimeFactor;
		//float t = environment.CurrentEnv.rain_density;
		//int ss= (int) ((char*)&environment.CurrentWeatherName - (char*)&environment);
		//char * cc = environment.m_b_skybox.description.cName;
		//char * cc2 = environment.m_b_skybox.description.cComputer;
		//int ss1 = environment.CloudsVerts.size();
		//int ss2 = environment.CloudsIndices.size();
		lua_pushnumber(L, environment.fGameTime);
		lua_pushnumber(L, environment.fTimeFactor);
		lua_pushnumber(L, environment.wind_gust_factor);
		lua_pushnumber(L, environment.wind_strength_factor);
		lua_pushnumber(L, environment.CurrentEnv.weight);
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.x);
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.y);
		lua_pushnumber(L, environment.CurrentEnv.sun_dir.z);
		//lua_pushnumber(L, t);
		//lua_pushstring(L, cc);
		//lua_pushinteger(L, ss);
		//lua_pushinteger(L, c);
		//lua_pushinteger(L, i);
		//lua_pushnumber(L, f);
		//lua_pushinteger(L, ss);
		//lua_pushinteger(L, ss2);
		//lua_pushstring(L, cc2);
		return 7;
	}
	return luaL_error(L, "get_weather_time must have no arguments");
}
/*
BOOL ListProcessModules( DWORD dwPID ) 
{ 
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
  MODULEENTRY32 me32; 
 
//  Take a snapshot of all modules in the specified process. 
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID ); 
  if( hModuleSnap == INVALID_HANDLE_VALUE ) 
  { 
    return( FALSE ); 
  } 
 
//  Set the size of the structure before using it. 
  me32.dwSize = sizeof( MODULEENTRY32 ); 
 
//  Retrieve information about the first module, 
//  and exit if unsuccessful 
  if( !Module32First( hModuleSnap, &me32 ) ) 
  { 
    CloseHandle( hModuleSnap );     // Must clean up the snapshot object! 
    return( FALSE ); 
  } 
 
//  Now walk the module list of the process
  do 
  { 
	if (!strcmp(me32.szModule, "xrRender_R2.dll")) {
		BASE_ADDRESS_XRRENDER_R2 = (DWORD)me32.modBaseAddr;
	}
	if (!strcmp(me32.szModule, "xrGame.dll")) {
		BASE_ADDRESS_XRGAME = (DWORD)me32.modBaseAddr;
	}
	if ((BASE_ADDRESS_XRRENDER_R2 != NULL) && (BASE_ADDRESS_XRGAME != NULL))
		break;
 
  } while( Module32Next( hModuleSnap, &me32 ) ); 

  if ((BASE_ADDRESS_XRRENDER_R2 == NULL) || (BASE_ADDRESS_XRGAME == NULL))
  {
	  return( FALSE ); 
  }

//  Clean up the snapshot object. 
  CloseHandle( hModuleSnap ); 
  return( TRUE ); 
} */
/*
BOOL InitGlobalVariables()
{
	__asm
	{
		// CRender *RImplementation
		push	edx;
		mov		edx, ADDR_CRENDER_RIMPLEMENTATION;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		RImplementation, edx;

		// CPortalTraverser *PortalTraverser
		mov		edx, ADDR_CPORTALTRAVERSER_PORTALTRAVERSER;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		PortalTraverser, edx;

		// occRasterizer *Raster
		mov		edx, ADDR_OCCRASTERIZER_RASTER;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		Raster, edx;

		// r_dtex_range
		mov		edx, ADDR_r_dtex_range;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		r_dtex_range, edx;

		// ps_r__Detail_density
		mov		edx, ADDR_ps_r__Detail_density;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r__Detail_density, edx;

		// ps_r__LOD
		mov		edx, ADDR_ps_r__LOD;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r__LOD, edx;

		// ps_r__tf_Anisotropic
		mov		edx, ADDR_ps_r__tf_Anisotropic;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r__tf_Anisotropic, edx;

		// ps_r1_ssaLOD_A
		mov		edx, ADDR_ps_r1_ssaLOD_A;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_ssaLOD_A, edx;

		// ps_r1_ssaLOD_B
		mov		edx, ADDR_ps_r1_ssaLOD_B;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_ssaLOD_B, edx;

		// ps_r1_lmodel_lerp
		mov		edx, ADDR_ps_r1_lmodel_lerp;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_lmodel_lerp, edx;

		// ps_r1_tf_Mipbias
		mov		edx, ADDR_ps_r1_tf_Mipbias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_tf_Mipbias, edx;

		// ps_r1_flags
		mov		edx, ADDR_ps_r1_flags;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_flags, edx;

		// ps_r1_dlights_clip
		mov		edx, ADDR_ps_r1_dlights_clip;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_dlights_clip, edx;

		// ps_r1_pps_u
		mov		edx, ADDR_ps_r1_pps_u;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_pps_u, edx;

		// ps_r1_pps_v
		mov		edx, ADDR_ps_r1_pps_v;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_pps_v, edx;

		// ps_r1_GlowsPerFrame
		mov		edx, ADDR_ps_r1_GlowsPerFrame;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r1_GlowsPerFrame, edx;

		// ps_r2_ssaLOD_A
		mov		edx, ADDR_ps_r2_ssaLOD_A;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ssaLOD_A, edx;

		// ps_r2_ssaLOD_B
		mov		edx, ADDR_ps_r2_ssaLOD_B;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ssaLOD_B, edx;

		// ps_r2_tf_Mipbias
		mov		edx, ADDR_ps_r2_tf_Mipbias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_tf_Mipbias, edx;

		// ps_r2_gmaterial
		mov		edx, ADDR_ps_r2_gmaterial;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_gmaterial, edx;

		// ps_r2_ls_flags
		mov		edx, ADDR_ps_r2_ls_flags;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_flags, edx;

		// ps_r2_tonemap_middlegray
		mov		edx, ADDR_ps_r2_tonemap_middlegray;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_tonemap_middlegray, edx;

		// ps_r2_tonemap_adaptation
		mov		edx, ADDR_ps_r2_tonemap_adaptation;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_tonemap_adaptation, edx;

		// ps_r2_tonemap_low_lum
		mov		edx, ADDR_ps_r2_tonemap_low_lum;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_tonemap_low_lum, edx;

		// ps_r2_tonemap_amount
		mov		edx, ADDR_ps_r2_tonemap_amount;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_tonemap_amount, edx;

		// ps_r2_ls_bloom_kernel_scale
		mov		edx, ADDR_ps_r2_ls_bloom_kernel_scale;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_bloom_kernel_scale, edx;

		// ps_r2_ls_bloom_kernel_g
		mov		edx, ADDR_ps_r2_ls_bloom_kernel_g;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_bloom_kernel_g, edx;

		// ps_r2_ls_bloom_kernel_b
		mov		edx, ADDR_ps_r2_ls_bloom_kernel_b;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_bloom_kernel_b, edx;

		// ps_r2_ls_bloom_threshold
		mov		edx, ADDR_ps_r2_ls_bloom_threshold;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_bloom_threshold, edx;

		// ps_r2_ls_bloom_speed
		mov		edx, ADDR_ps_r2_ls_bloom_speed;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_bloom_speed, edx;

		// ps_r2_ls_dsm_kernel
		mov		edx, ADDR_ps_r2_ls_dsm_kernel;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_dsm_kernel, edx;

		// ps_r2_ls_psm_kernel
		mov		edx, ADDR_ps_r2_ls_psm_kernel;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_psm_kernel, edx;

		// ps_r2_ls_ssm_kernel
		mov		edx, ADDR_ps_r2_ls_ssm_kernel;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_ssm_kernel, edx;

		// ps_r2_ls_squality
		mov		edx, ADDR_ps_r2_ls_squality;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_squality, edx;

		// ps_r2_zfill
		mov		edx, ADDR_ps_r2_zfill;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_zfill, edx;

		// ps_r2_gloss_factor
		mov		edx, ADDR_ps_r2_gloss_factor;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_gloss_factor, edx;

		// ps_r2_sun_tsm_projection
		mov		edx, ADDR_ps_r2_sun_tsm_projection;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_tsm_projection, edx;

		// ps_r2_sun_tsm_bias
		mov		edx, ADDR_ps_r2_sun_tsm_bias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_tsm_bias, edx;

		// ps_r2_sun_near
		mov		edx, ADDR_ps_r2_sun_near;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_near, edx;

		// ps_r2_sun_near_border
		mov		edx, ADDR_ps_r2_sun_near_border;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_near_border, edx;

		// ps_r2_sun_depth_far_scale
		mov		edx, ADDR_ps_r2_sun_depth_far_scale;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_depth_far_scale, edx;

		// ps_r2_sun_depth_far_bias
		mov		edx, ADDR_ps_r2_sun_depth_far_bias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_depth_far_bias, edx;

		// ps_r2_sun_depth_near_scale
		mov		edx, ADDR_ps_r2_sun_depth_near_scale;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_depth_near_scale, edx;

		// ps_r2_sun_depth_near_bias
		mov		edx, ADDR_ps_r2_sun_depth_near_bias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_depth_near_bias, edx;

		// ps_r2_sun_lumscale
		mov		edx, ADDR_ps_r2_sun_lumscale;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_lumscale, edx;

		// ps_r2_sun_lumscale_hemi
		mov		edx, ADDR_ps_r2_sun_lumscale_hemi;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_lumscale_hemi, edx;

		// ps_r2_sun_lumscale_amb
		mov		edx, ADDR_ps_r2_sun_lumscale_amb;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_sun_lumscale_amb, edx;

		// ps_r2_aa_kernel
		mov		edx, ADDR_ps_r2_aa_kernel;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_aa_kernel, edx;

		// ps_r2_mblur
		mov		edx, ADDR_ps_r2_mblur;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_mblur, edx;

		// ps_r2_GI_clip
		mov		edx, ADDR_ps_r2_GI_clip;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_GI_clip, edx;

		// ps_r2_GI_depth
		mov		edx, ADDR_ps_r2_GI_depth;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_GI_depth, edx;

		// ps_r2_GI_photons
		mov		edx, ADDR_ps_r2_GI_photons;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_GI_photons, edx;

		// ps_r2_GI_refl
		mov		edx, ADDR_ps_r2_GI_refl;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_GI_refl, edx;

		// ps_r2_wait_sleep
		mov		edx, ADDR_ps_r2_wait_sleep;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_wait_sleep, edx;

		// ps_r2_ls_depth_scale
		mov		edx, ADDR_ps_r2_ls_depth_scale;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_depth_scale, edx;

		// ps_r2_ls_depth_bias
		mov		edx, ADDR_ps_r2_ls_depth_bias;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_ls_depth_bias, edx;

		// ps_r2_df_parallax_h
		mov		edx, ADDR_ps_r2_df_parallax_h;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_df_parallax_h, edx;

		// ps_r2_slight_fade
		mov		edx, ADDR_ps_r2_slight_fade;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_slight_fade, edx;

		// ps_r2_aa_barier
		mov		edx, ADDR_ps_r2_aa_barier;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_aa_barier, edx;

		// ps_r2_aa_weight
		mov		edx, ADDR_ps_r2_aa_weight;
		add		edx, BASE_ADDRESS_XRRENDER_R2;
		mov		ps_r2_aa_weight, edx;
	
		pop		edx;
	}
	return TRUE;
};
*/