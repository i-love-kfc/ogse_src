#include <Windows.h>
#include "xray\base_types.h"
#include "xray\xrGame.h"
//#include "xray\xrCore.h"
#include "xray\xr_3da.h"
#include "global_ns.h"
#include "xray\Game_Persistent.h"
#include "xray\IniFile.h"
#include "xray\RenderDevice.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "EnvManager.h"
#include "astro\sunpos.h"
#include "xray\xr_render_console.h"
/*
void __cdecl Log(const char*);
void __cdecl Msg(const char*, ...);
void __cdecl SetLogCB(void (__cdecl *)(const char *));
*/
//#include <luabind\luabind.hpp>
//using namespace luabind;
//namespace luabind {};

#define f(n) int n(lua_State *L)

f(pos);
f(GetShift);
f(GetLAlt);
f(GetRAlt);
f(GetAlt);
f(GetCaps);
f(ReadSHitData);
f(ReadMemInt);
f(ReadMemInt16);
f(ReadMemFlt);
f(WriteMemInt);
f(WriteMemInt16);
f(WriteMemFlt);
f(LuaGetMemoryUsage);
f(DestroyNecessaryTextures);
f(SwitchLanguage);
f(GetEnvDescData);
f(SetEnvDescData);
f(InvalidateEnvirinment);
f(GetSunPosition);
f(SendMsg2RemoteTracer);
f(DumpWeather);
f(SetDetailsMinRadius);

int open_global_ns(lua_State *L)
{
	Log("open_global_ns - start");
	lua_register(L, "GetMousePos",	pos			);
	lua_register(L, "GetShift",		GetShift	);
	lua_register(L, "GetLAlt",		GetLAlt   );
	lua_register(L, "GetRAlt",		GetRAlt);
	lua_register(L, "GetAlt",		GetAlt);
	lua_register(L, "GetCaps",		GetCaps		);
	lua_register(L, "read_hit_data",  ReadSHitData);
	//lua_register(L, "write_hit_data",  WriteSHitData);
	lua_register(L, "read_memory_int",  ReadMemInt);
	lua_register(L, "read_memory_int16",  ReadMemInt16);
	lua_register(L, "read_memory_float",  ReadMemFlt);
	lua_register(L, "write_memory_int",  WriteMemInt);
	lua_register(L, "write_memory_int16",  WriteMemInt16);
	lua_register(L, "write_memory_float",  WriteMemFlt);
	lua_register(L, "get_memory_usage",  LuaGetMemoryUsage);
	lua_register(L, "DestroyNecessaryTextures",  DestroyNecessaryTextures);
	lua_register(L, "SwitchLanguage", SwitchLanguage);
	lua_register(L, "GetEnvDescData", GetEnvDescData);
	lua_register(L, "SetEnvDescData", SetEnvDescData);
	lua_register(L, "InvalidateEnvirinment", InvalidateEnvirinment);

	lua_register(L, "get_memory_usage", LuaGetMemoryUsage);
	lua_register(L, "GetSunPosition", GetSunPosition);
	lua_register(L, "SendMsg2RemoteTracer", SendMsg2RemoteTracer);
	lua_register(L, "DumpCurrentWeatherState", DumpWeather);
	lua_register(L, "SetDetailsMinRadius", SetDetailsMinRadius);
	
	//luabind::function	(L,	"Test",	LuaGetMemoryUsage);
	Log("open_global_ns - end");
	return 0;
}

f(pos){
	POINT p;
	GetCursorPos((LPPOINT)&p);
	lua_pushinteger(L, p.x);
	lua_pushinteger(L, p.y);
	return 2;
}

f(GetShift){
	//lua_pushboolean(L, GetKeyState(VK_SHIFT) & 0x8000);
	bool pressed = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0);
	lua_pushboolean(L, pressed);
	return 1;
}

f(GetCaps){
	//lua_pushboolean(L, GetKeyState(VK_CAPITAL) & 0x0001);
	bool pressed = ((GetAsyncKeyState(VK_CAPITAL) & 0x8000) != 0);
	lua_pushboolean(L, pressed);
	return 1;
}

f(GetLAlt){
	bool pressed = ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0);
	//Log(pressed?"VK_LMENU pressed":"VK_LMENU not pressed");
	lua_pushboolean(L, pressed);
	return 1;
}
f(GetRAlt){
	bool pressed = ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0);
	//Log(pressed?"VK_RMENU pressed":"VK_RMENU not pressed");
	lua_pushboolean(L, pressed);
	return 1;
}
f(GetAlt){
	bool pressed = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0);
	lua_pushboolean(L, pressed);
	return 1;
}
f(ReadMemInt) {
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 1:
		{
		pos = luaL_checkinteger(L, 1);
		break;
		}
	case 2:
		{
		pos = luaL_checkinteger(L, 1);
		shift = luaL_checkinteger(L, 2);
		break;
		}
	default:
		return luaL_error(L, "ReadMemInt: wrong number of arguments! Must be 1 or 2.");
	}
	lua_pushnumber(L, *(int*)(pos + shift));
	return 1;
}
f(ReadMemInt16) {
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 1:
		{
		pos = luaL_checkinteger(L, 1);
		break;
		}
	case 2:
		{
		pos = luaL_checkinteger(L, 1);
		shift = luaL_checkinteger(L, 2);
		break;
		}
	default:
		return luaL_error(L, "ReadMemInt: wrong number of arguments! Must be 1 or 2.");
	}
	lua_pushnumber(L, *(short*)(pos + shift));
	return 1;
}
f(ReadMemFlt) {
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 1:
		{
		pos = luaL_checkinteger(L, 1);
		break;
		}
	case 2:
		{
		pos = luaL_checkinteger(L, 1);
		shift = luaL_checkinteger(L, 2);
		break;
		}
	default:
		return luaL_error(L, "ReadMemFlt: wrong number of arguments! Must be 1 or 2.");
	}
	lua_pushnumber(L, *(float*)(pos + shift));
	return 1;
}
f(WriteMemInt) {
	LUA_INTEGER value;
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 2:
		{
		value = luaL_checkinteger(L, 1);
		pos = luaL_checkinteger(L, 2);
		break;
		}
	case 3:
		{
		value = luaL_checkinteger(L, 1);
		pos = luaL_checkinteger(L, 2);
		shift = luaL_checkinteger(L, 3);
		break;
		}
	default:
		return luaL_error(L, "WriteMemInt: wrong number of arguments! Must be 2 or 3.");
	}
	int* n = (int*) (pos + shift);
	//Msg("WriteMemInt: %d, %d, %d", value, pos, shift);
	*n = value;
	//lua_pushinteger(L, *n);
	return 0;
}
f(WriteMemInt16) {
	LUA_INTEGER value;
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 2:
		{
		value = luaL_checkinteger(L, 1);
		pos = luaL_checkinteger(L, 2);
		break;
		}
	case 3:
		{
		value = luaL_checkinteger(L, 1);
		pos = luaL_checkinteger(L, 2);
		shift = luaL_checkinteger(L, 3);
		break;
		}
	default:
		return luaL_error(L, "WriteMemInt: wrong number of arguments! Must be 2 or 3.");
	}
	short* n = (short*) (pos + shift);
	//Msg("WriteMemInt: %d, %d, %d", value, pos, shift);
	*n = value;
	//lua_pushinteger(L, *n);
	return 0;
}
f(WriteMemFlt) {
	float value; 
	LUA_INTEGER pos;
	LUA_INTEGER shift = 0;
	switch (lua_gettop(L))
	{
	case 2:
		{
		value = luaL_checknumber(L, 1);
		pos = luaL_checkinteger(L, 2);
		break;
		}
	case 3:
		{
		value = luaL_checknumber(L, 1);
		pos = luaL_checkinteger(L, 2);
		shift = luaL_checkinteger(L, 3);
		break;
		}
	default:
		return luaL_error(L, "WriteMemFlt: wrong number of arguments! Must be 2 or 3.");
	}
	float* n = (float*) (pos + shift);
	//Msg("WriteMemFlt: %f, %d, %d", value, pos, shift);
	*n = value;
	//lua_pushnumber(L, *n);
	return 0;
}
#define PUSH_BOOL(field_name, value) {\
	lua_pushboolean(L, value);\
	/*Msg(field_name ": %d", value);*/\
	lua_setfield(L, top, field_name);\
}
#define PUSH_INTEGER(field_name, value) {\
	lua_pushinteger(L, value);\
	/*Msg(field_name ": %d", value);*/\
	lua_setfield(L, top, field_name);\
}
#define PUSH_FLOAT(field_name, value) {\
	lua_pushnumber(L, value);\
	/*Msg(field_name ": %7.2f", value);*/\
	lua_setfield(L, top, field_name);\
}
f(ReadSHitData){
	LUA_INTEGER n = luaL_checkinteger(L, 1);
	//Msg("GetSHitData: %x", n);
	SHit* pHit = (SHit*)n;

	//int shift1 = (char*)&pHit->power - (char*)pHit;
	//Msg("power_sh: %d", shift1);
	//int shift2 = (char*)&pHit->impulse - (char*)pHit;
	//Msg("impulse_sh: %d", shift2);
	lua_createtable(L, 0, 8);
	int top = lua_gettop(L);
	PUSH_BOOL("aim_bullet", pHit->aim_bullet);

	PUSH_INTEGER("weapon_id", pHit->weaponID);
	PUSH_INTEGER("who_id", pHit->whoID);
	PUSH_INTEGER("who", (unsigned int)pHit->who);
	PUSH_INTEGER("bone_id", pHit->boneID);
	PUSH_INTEGER("hit_type", pHit->hit_type);
	PUSH_INTEGER("time", pHit->Time);

	PUSH_FLOAT("ap", pHit->ap);
	PUSH_FLOAT("impulse", pHit->impulse);
	PUSH_FLOAT("power", pHit->power);

	return 1;
}
/*f(WriteSHitData){
	if (lua_gettop(L) != 2) luaL_error(L, "WriteSHitData: number of atguments is not 2!");
	LUA_INTEGER arg1 = luaL_checkinteger(L, 1);
	SHit* pHit = (SHit*)arg1;
	if (lua_istable(L, 2) != 1)	luaL_error(L, "WriteSHitData: second argument is not a table!");
	//luaL_checktype(L, 1, LUA_TTABLE);
	//int lua_next (lua_State *L, int index);

	//for(unsigned int i = 1; i <= len; i++){
	
	//}

	lua_getfield(L, 2, "");
	LUA_INTEGER n = luaL_checkinteger(L, 1);
	SHit* pHit = (SHit*)n;
	return 0;
}*/
void GetMemoryUsage(u32 &m_base, u32&c_base, u32&m_lmaps, u32&c_lmaps)
{
	CRenderDevice* pDevice = (CRenderDevice*)(&Device);
	pDevice->Resources->_GetMemoryUsage	(m_base,c_base,m_lmaps,c_lmaps);
	pDevice->Resources->Dump(false);
	pDevice->Resources->DBG_VerifyGeoms();
	//pDevice->Resources->DBG_VerifyTextures();
}
f(LuaGetMemoryUsage){
	u32	m_base,c_base,m_lmaps,c_lmaps;
	GetMemoryUsage(m_base,c_base,m_lmaps,c_lmaps);
	lua_pushinteger(L, m_base);
	lua_pushinteger(L, c_base);
	lua_pushinteger(L, m_lmaps);
	lua_pushinteger(L, c_lmaps);
	return 4;
}
f(DestroyNecessaryTextures){
	CRenderDevice* pDevice = (CRenderDevice*)(&Device);
	pDevice->Resources->DestroyNecessaryTextures();
	return 0;
}

HKL lpList[100];
f(SwitchLanguage){
	/*int num = GetKeyboardLayoutList(100, lpList)
	if (num)
	{
		HKL cur = GetKeyboardLayout(0);
		for (int i=0, num-1, i++)
		{
			if (cut == lpList[i])
			{
				int next = i + 1;
				if (next > num) next = 0;

				return 0;
			}
		}
	}*/
	ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
	return 0;
}



/*
	module(L)[
		class_<profile_timer_script>("profile_timer")
			.def(constructor<>())
			.def(constructor<profile_timer_script&>())
			.def(const_self + profile_timer_script())
			.def(const_self < profile_timer_script())
//			.def(tostring(self))
			.def("start",&profile_timer_script::start)
			.def("stop",&profile_timer_script::stop)
			.def("time",&profile_timer_script::time)
	];

	luabind::function	(L,	"log",							LuaLog);

*/
f(DumpWeather){
	CEnvironment &env = g_pGamePersistent->Environment();
	_vector3<float> & amb = env.CurrentEnv.ambient;
	Msg("ambient: %7.3f, %7.3f, %7.3f", amb.x, amb.y, amb.z);
	Msg("bolt_duration: %7.3f", env.CurrentEnv.bolt_duration);
	Msg("bolt_period: %7.3f", env.CurrentEnv.bolt_period);
	_vector4<float> &cl_col = env.CurrentEnv.clouds_color;
	Msg("clouds_color: %7.3f, %7.3f, %7.3f, %7.3f", cl_col.x, cl_col.y, cl_col.z, cl_col.w);
	Msg("clouds_texture_name: %s", env.CurrentEnv.clouds_texture_name.c_str());
	Msg("exec_time: %7.3f", env.CurrentEnv.exec_time);
	Msg("far_plane: %7.3f", env.CurrentEnv.far_plane);
	_vector3<float> & fog_color = env.CurrentEnv.fog_color;
	Msg("fog_color: %7.3f, %7.3f, %7.3f", fog_color.x, fog_color.y, fog_color.z);
	Msg("fog_density: %7.3f", env.CurrentEnv.fog_density);
	Msg("fog_distance: %7.3f", env.CurrentEnv.fog_distance);
	Msg("fog_far: %7.3f", env.CurrentEnv.fog_far);
	Msg("fog_near: %7.3f", env.CurrentEnv.fog_near);
	_vector4<float> &hemi_color = env.CurrentEnv.hemi_color;
	Msg("hemi_color: %7.3f, %7.3f, %7.3f, %7.3f", hemi_color.x, hemi_color.y, hemi_color.z, hemi_color.w);
	Msg("lens_flare_id: %d", env.CurrentEnv.lens_flare_id);
	Msg("rain_density: %7.3f", env.CurrentEnv.rain_density);
	_vector3<float> & sky_color = env.CurrentEnv.sky_color;
	Msg("sky_color: %7.3f, %7.3f, %7.3f", sky_color.x, sky_color.y, sky_color.z);
	Msg("sky_rotation: %7.3f", env.CurrentEnv.sky_rotation);
	Msg("sky_texture_env_name: %s", env.CurrentEnv.sky_texture_env_name.c_str());
	Msg("sky_texture_name: %s", env.CurrentEnv.sky_texture_name.c_str());
	_vector3<float> & sun_color = env.CurrentEnv.sun_color;
	Msg("sun_color: %7.3f, %7.3f, %7.3f", sun_color.x, sun_color.y, sun_color.z);
	_vector3<float> & sun_dir = env.CurrentEnv.sun_dir;
	Msg("sun_dir: %7.3f, %7.3f, %7.3f", sun_dir.x, sun_dir.y, sun_dir.z);
	Msg("tb_id: %d", env.CurrentEnv.tb_id);
	Msg("weight: %7.3f", env.CurrentEnv.weight);
	Msg("wind_direction: %7.3f", env.CurrentEnv.wind_direction);
	Msg("wind_velocity: %7.3f", env.CurrentEnv.wind_velocity);
	return 0;
}

#define GET_OFFSET(s, f) ((char*)&s.f - (char*)&s)
f(GetEnvDescData){
	LUA_INTEGER n = luaL_checkinteger(L, 1); // номер погодной секции

	CEnvironment &env = g_pGamePersistent->Environment();
	int num = env.CurrentWeather->size();
	Msg("cycles number: %d", num);
	Msg("fTime: %f", env.fGameTime);

	if (env.CurrentWeatherName.size() > 0)
	{
		Msg("CurrentWeatherName: %s", env.CurrentWeatherName.c_str());
	}
	else
	{
		Msg("no CurrentWeatherName");
	}
	//Msg("time: %7.2f", env.fGameTime);
	//Msg("time factor: %7.2f", env.fTimeFactor);
	//Msg("CurrentEnv: %x", GET_OFFSET(env, CurrentEnv));
	//Msg("Current: %x", GET_OFFSET(env, Current));
	Msg("CurrentCycleName: %s", env.CurrentCycleName);
	//Msg("WeatherCycles: %x", GET_OFFSET(env, WeatherCycles));	
	//Msg("Modifiers: %x", GET_OFFSET(env, Modifiers));
	//Msg("sh_2sky: %x", GET_OFFSET(env, sh_2sky));
	//Msg("fTimeFactor: %x", GET_OFFSET(env, fTimeFactor));

	CEnvDescriptorMixer &cur_env = env.CurrentEnv;
	Msg("current environment (mixer)");
	Msg("exec time: %7.2f", env.Current[n]->exec_time);
	Msg("exec time loaded: %7.2f", env.Current[n]->exec_time_loaded);
	//Msg("clouds_texture_name: %x", GET_OFFSET(cur_env, clouds_texture_name));
	//Msg("rain_color: %x", GET_OFFSET(cur_env, rain_color));
	//Msg("fog_far: %x", GET_OFFSET(cur_env, fog_far));
	//Msg("clouds_texture_name: %s", cur_env.clouds_texture_name.p_->value);
	Msg("clouds_texture_name: %s", env.Current[n]->clouds_texture_name.c_str());
	Msg("sky_texture_env_name: %s", env.Current[n]->sky_texture_env_name.c_str());
	Msg("sky_texture_name: %s", env.Current[n]->sky_texture_name.c_str());

	return 0;
	SHit* pHit = (SHit*)n;

	//int shift1 = (char*)&pHit->power - (char*)pHit;
	//Msg("power_sh: %d", shift1);
	//int shift2 = (char*)&pHit->impulse - (char*)pHit;
	//Msg("impulse_sh: %d", shift2);
	lua_createtable(L, 0, 8);
	int top = lua_gettop(L);
	lua_pushboolean(L, pHit->aim_bullet); //Msg("aim_bullet: %d", pHit->aim_bullet);
	lua_setfield(L, top, "aim_bullet");

	lua_pushinteger(L, pHit->weaponID); //Msg("weapon_id: %d", pHit->weaponID);
	lua_setfield(L, top, "weapon_id");

	lua_pushinteger(L, pHit->whoID); //Msg("weapon_id: %d", pHit->whoID);
	lua_setfield(L, top, "who_id");
	
	lua_pushinteger(L, (unsigned int)pHit->who); //Msg("weapon_id: %d", pHit->);
	lua_setfield(L, top, "who");
	
	lua_pushinteger(L, pHit->boneID); //Msg("boneID: %d", pHit->boneID);
	lua_setfield(L, top, "bone_id");

	lua_pushinteger(L, pHit->hit_type); //Msg("hit_type: %d", pHit->hit_type);
	lua_setfield(L, top, "hit_type");

	lua_pushinteger(L, pHit->Time);// Msg("Time: %d", pHit->Time);
	lua_setfield(L, top, "time");

	lua_pushnumber(L, pHit->ap); //Msg("ap: %7.2f", pHit->ap);
	lua_setfield(L, top, "ap");

	lua_pushnumber(L, pHit->impulse); //Msg("impulse: %7.2f", pHit->impulse);
	lua_setfield(L, top, "impulse");

	lua_pushnumber(L, pHit->power); //Msg("power: %7.2f", pHit->power);
	lua_setfield(L, top, "power");

	return 1;
}
f(SetEnvDescData)
{
	Log("1");
	if (lua_gettop(L) != 2) luaL_error(L, "SetEnvDescData: must be only two arguments!");
	//LUA_INTEGER env_ind = luaL_checkinteger(L, 1); // номер погодного дескриптора: 0 или 1
	//if (env_ind != 0 && env_ind != 1) luaL_error(L, "SetEnvDescData: first argument must be 0 or 1!");

	CEnvironment &env = g_pGamePersistent->Environment(); // получаем объект погоды
	Log("1_");
	env.SetWeather("ogse_script_weather", true);
	Log("2");
	//const char* wname = env.CurrentWeatherName.c_str();
	//Msg("new weather name: %s", wname ? wname : "none");
	//Msg("items=%d", env.CurrentWeather->size());
	//return 0;
	for (int i=0; i<2; i++)
	{
		Msg("i=%d", i);
		CEnvDescriptor *env_desc = (*env.CurrentWeather)[i];//env.Current[env_ind];

		if (!InitEnvDescriptor(env_desc, L, i + 1))
		{
			luaL_error(L, "SetEnvDescData: cannot initialize evironment descriptor!");
		}
	}
	CEnvDescriptor* *env_desc0 = &(*env.CurrentWeather)[0];
	CEnvDescriptor* *env_desc1 = &(*env.CurrentWeather)[1];
	Msg("1 - env0: %f, env1: %f", (*env_desc0)->exec_time, (*env_desc1)->exec_time);
	Msg("1 - envd0: %f, envd1: %f", env.Current[0]->exec_time, env.Current[1]->exec_time);
	if ((*env_desc0)->exec_time > (*env_desc1)->exec_time)
	{
		CEnvDescriptor *tmp_desc = *env_desc0;
		*env_desc0 = *env_desc1;
		*env_desc1 = tmp_desc;
	}
	Msg("2 - env0: %f, env1: %f", (*env_desc0)->exec_time, (*env_desc1)->exec_time);
	Msg("2 - envd0: %f, envd1: %f", env.Current[0]->exec_time, env.Current[1]->exec_time);
	env.ReselectEnvs();
	Msg("3 - env0: %f, env1: %f", (*env_desc0)->exec_time, (*env_desc1)->exec_time);
	Msg("3 - envd0: %f, envd1: %f", env.Current[0]->exec_time, env.Current[1]->exec_time);
	return 0;
}

f(InvalidateEnvirinment)
{
	CEnvironment &env = g_pGamePersistent->Environment(); // получаем объект погоды
	env.Invalidate();
	return 0;
}

f(GetSunPosition)
{
	if (lua_gettop(L) == 3)
	{
		int hours = luaL_checkinteger(L, 1);
		hours -= 2.0; // UTC+2
		if (hours < 0) hours += 24;
		//double hours_fract = modf(fmod(hours, 24.0), &hours);
		//double fMinutes;
		//double fMinutes_frac = modf(hours_fract * 60.0, &fMinutes);
		
		cTime udtTime;
		udtTime.iYear = 2012;
		udtTime.iMonth = 10; // October
		udtTime.iDay = 1;
		udtTime.dHours = hours;
		udtTime.dMinutes = luaL_checkinteger(L, 2);;
		udtTime.dSeconds = luaL_checkinteger(L, 3);;

		cLocation udtLocation;
		
		// Prypiat
		udtLocation.dLatitude  = 51.0 + 24 / 60.0; //51 24'00" N (positive)
		udtLocation.dLongitude = 30.0 +  3 / 60.0; //30 03'00" E (positive)
		cSunCoordinates sun_coordinates;
		sunpos(udtTime, udtLocation, sun_coordinates);

		lua_pushnumber(L, sun_coordinates.dZenithAngle);
		lua_pushnumber(L, sun_coordinates.dAzimuth);
		return 2;
	}
	return luaL_error(L, "GetSunPosition must have three arguments");
}

#define BUFSIZE 2048
HANDLE hPipe = 0;
char* pipename = "\\\\.\\pipe\\ogse_debug_channel"; 

f(SendMsg2RemoteTracer)
{
	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		lua_pushboolean(L, false);
		lua_pushstring(L, "invalid pipe");
		return 2;
	}
	if (lua_gettop(L) != 1) return luaL_error(L, "SendMsg2RemoteTracer: must have one string argument!");
	size_t cbToWrite = 0;
	const char* str = luaL_checklstring(L, 1, &cbToWrite);
	if (!str) return luaL_error(L, "SendMsg2RemoteTracer: argument must be a string!");
	//Msg("%s, %d", str, cbToWrite);

	if (!hPipe)
	{
		hPipe = CreateFile( 
			pipename,       // pipe name 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
		if (hPipe == INVALID_HANDLE_VALUE)
		{
			lua_pushboolean(L, false);
			lua_pushstring(L, "failed to create a pipe");
			return 2;
		}
		DWORD  dwMode = PIPE_READMODE_MESSAGE;
		BOOL fSuccess = SetNamedPipeHandleState( 
			hPipe,    // pipe handle 
			&dwMode,  // new pipe mode 
			NULL,     // don't set maximum bytes 
			NULL);    // don't set maximum time 
		if (!fSuccess) 
		{
			hPipe = INVALID_HANDLE_VALUE;
			lua_pushboolean(L, false);
			lua_pushstring(L, "failed to change pipe mode");
			return 2;
		}
	}
	DWORD cbWritten = 0;
	BOOL fSuccess = WriteFile(
		hPipe,       // pipe handle 
		str,         // message 
		cbToWrite,   // message length 
		&cbWritten,  // bytes written 
		NULL);       // not overlapped 

	if (!fSuccess)
	{
		lua_pushboolean(L, false);
		lua_pushstring(L, "failed to write to the pipe");
		lua_pushinteger(L, GetLastError());
		return 3;
	}
	lua_pushboolean(L, true);
	return 1;
}
f(SetDetailsMinRadius)
{
	if (lua_gettop(L) == 1)
	{
		float value = luaL_checknumber(L, 1);
		if (value >= 0.0 && value <= 5.0)
		{
			ps_r2_ndr = value;
			return 0;
		}
	}
	return luaL_error(L, "SetDetailsMinRadius: Must be single float argument in the range [0 - 5]!");
}


