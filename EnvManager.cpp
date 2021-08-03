#include "EnvManager.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "xray\Game_Persistent.h"

bool str2vect2(const char* s, _vector2<float> &v)
{
	return 2 == sscanf(s,"%f,%f",&v.x,&v.y);
}
bool str2vect3(const char* s, _vector3<float> &v)
{
	return 3 == sscanf(s,"%f,%f,%f",&v.x,&v.y,&v.z);
}
bool str2vect4(const char* s, _vector4<float> &v)
{
	v.w = 0;
	return sscanf(s,"%f,%f,%f,%f",&v.x,&v.y,&v.z,&v.w) >= 3;
}
#define TRY_READ_FLOAT(param_name, var, conv_fun) else if (strcmp(key, param_name) == 0)\
{\
	var = conv_fun(lua_tonumber(L, -1));\
}

#define TRY_READ_VEC3(param_name, var) else if (strcmp(key, param_name) == 0)\
{\
	if (!str2vect3(lua_tostring(L, -1), var))\
	{\
		/*luaL_error(L, "InitEnvDescriptor: invalid 'fog_color' parameter!");*/\
		Msg("SetEnvDescData: invalid '" param_name "' parameter!");\
		return false;\
	}\
}

bool InitEnvDescriptor(CEnvDescriptor *env_desc, lua_State *L, int stack_idx)
{
	Log("InitEnvDescriptor");
	if (!env_desc)
	{
		Msg("InitEnvDescriptor: 'env_desc' is a null pointer!");
		return false;
	}
	if (lua_istable(L, stack_idx) != 1)
	{
		Msg("InitEnvDescriptor: argument given is not a table!");
		return false;
	}
	CEnvironment &env = g_pGamePersistent->Environment(); // получаем объект погоды

	env_desc->sky_rotation = 0;
	env_desc->bolt_period = 0;
	env_desc->bolt_duration = 0;
	env_desc->env_ambient = 0;
	// таблица в стеке по индексу stack_idx
	lua_pushnil(L);  // начинаем с нулевого ключа, т.е. с начала
	while (lua_next(L, stack_idx) != 0)
	{
		//const char* key = lua_tostring(L, -2);
		//const char* v = lua_tostring(L, -1);
		//lua_pop(L, 1);
		//continue;
		// в стеке ключ имеет индекс -2, значение имеет индекс -1
		if (lua_isstring(L, -2)) // если ключ - строка
		{
			const char* key = lua_tostring(L, -2);
			Msg("key: %s", key);
			//const char* value = lua_tostring(L, -1);
			// удаляем из стека значение, но оставляем ключ для следующей итерации
			//lua_pop(L, 1);
			if (strcmp(key, "exec_time") == 0)
			{
				env_desc->exec_time = fmod(lua_tonumber(L, -1), (lua_Number)DAY_LENGTH);
				env_desc->exec_time_loaded = env_desc->exec_time;
				//sscanf(value,"%f,%f,%f,%f,%f",&clouds_color.x,&clouds_color.y,&clouds_color.z,&clouds_color.w,&multiplier);
				//if (1 != sscanf(value,"%f",&env_desc->exec_time))luaL_error(L, "SetEnvDescData: cannot read 'exec_time' value!");
			}
			else if (strcmp(key, "sky_texture") == 0)
			{
				const char* v = lua_tostring(L, -1);
				//Msg("sky_texture: %s", v);
				static char buf[1024];
				strcpy_s(buf, v);
				env_desc->sky_texture_name._set(buf);
				strncat_s(buf, sizeof(buf), "#small", 6);
				env_desc->sky_texture_env_name._set(buf);

			}
			else if (strcmp(key, "clouds_texture") == 0)
			{
				env_desc->clouds_texture_name._set(lua_tostring(L, -1));
			}
			else if (strcmp(key, "clouds_color") == 0)
			{
				float multiplier = 0;
				int res = sscanf(lua_tostring(L, -1), "%f,%f,%f,%f,%f",
					&env_desc->clouds_color.x,
					&env_desc->clouds_color.y,
					&env_desc->clouds_color.z,
					&env_desc->clouds_color.w,
					&multiplier);
				if (res < 3)
				{
					//luaL_error(L, "SetEnvDescData: invalid 'clouds_color' parameter!");
					Msg("SetEnvDescData: invalid 'clouds_color' parameter!");
					return false;
				}
				float save = env_desc->clouds_color.w;
				env_desc->clouds_color.mul(.5f*multiplier);
				env_desc->clouds_color.w = save;

				//if (str2vect(lua_tostring(L, -1), env_desc->clouds_color)
				//	luaL_error(L, "SetEnvDescData: cannot read 'clouds_color' argument!");
			}
			//TRY_READ_VEC3("sky_color", env_desc->sky_color)
			else if (strcmp(key, "sky_color") == 0)
			{
				if (!str2vect3(lua_tostring(L, -1), env_desc->sky_color))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'sky_color' parameter!");
					Msg("SetEnvDescData: invalid 'sky_color' parameter!");
					return false;
				}
				env_desc->sky_color.mul(.5f);
			}
			TRY_READ_FLOAT("sky_rotation", env_desc->sky_rotation, DEG2RAD)
			/*else if (strcmp(key, "sky_rotation") == 0)
			{
				env_desc->sky_rotation = DEG2RAD(lua_tonumber(L, -1));
			}*/
			TRY_READ_FLOAT("far_plane", env_desc->far_plane,)
			/*else if (strcmp(key, "far_plane") == 0)
			{
				env_desc->far_plane = lua_tonumber(L, -1);
			}*/
			TRY_READ_VEC3("fog_color", env_desc->fog_color)
			/*else if (strcmp(key, "fog_color") == 0)
			{
				if (!str2vect3(lua_tostring(L, -1), env_desc->fog_color))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'fog_color' parameter!");
					Msg("SetEnvDescData: invalid 'fog_color' parameter!");
					return false;
				}
			}*/
			TRY_READ_FLOAT("fog_density", env_desc->fog_density,)
			TRY_READ_FLOAT("fog_distance", env_desc->fog_distance,)
			TRY_READ_FLOAT("rain_density", env_desc->rain_density,)
			TRY_READ_VEC3("rain_color", env_desc->params_ex->rain_color)
			/*else if (strcmp(key, "rain_color") == 0)
			{
				if (!str2vect3(lua_tostring(L, -1), env_desc->params_ex->rain_color))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'rain_color' parameter!");
					Msg("SetEnvDescData: invalid 'rain_color' parameter!");
					return false;
				}
			}*/
			TRY_READ_FLOAT("rain_max_drop_angle", env_desc->params_ex->rain_max_drop_angle, DEG2RAD)
			TRY_READ_FLOAT("sun_shafts", env_desc->params_ex->sun_shafts,)
			TRY_READ_FLOAT("sun_shafts_length", env_desc->params_ex->sun_shafts_length,)
			TRY_READ_FLOAT("rain_increase_speed", env_desc->params_ex->rain_increase_speed,)
			TRY_READ_FLOAT("moon_road_intensity", env_desc->params_ex->moon_road_intensity,)
			TRY_READ_FLOAT("wind_velocity", env_desc->wind_velocity,)
			TRY_READ_FLOAT("wind_direction", env_desc->wind_direction, DEG2RAD)
			TRY_READ_VEC3("ambient", env_desc->ambient)
			/*else if (strcmp(key, "ambient") == 0)
			{
				if (!str2vect3(lua_tostring(L, -1), env_desc->ambient))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'ambient' parameter!");
					Msg("SetEnvDescData: invalid 'ambient' parameter!");
					return false;
				}
			}*/
			else if (strcmp(key, "hemi_color") == 0)
			{
				if (!str2vect4(lua_tostring(L, -1), env_desc->hemi_color))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'hemi_color' parameter!");
					Msg("SetEnvDescData: invalid 'hemi_color' parameter!");
					return false;
				}
			}
			TRY_READ_VEC3("sun_color", env_desc->sun_color)
			/*else if (strcmp(key, "sun_color") == 0)
			{
				if (!str2vect3(lua_tostring(L, -1), env_desc->sun_color))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'sun_color' parameter!");
					Msg("SetEnvDescData: invalid 'sun_color' parameter!");
					return false;
				}
			}*/
			else if (strcmp(key, "sun_dir") == 0)
			{
				_vector2<float> v;
				if (!str2vect2(lua_tostring(L, -1), v))
				{
					//luaL_error(L, "SetEnvDescData: invalid 'sun_dir' parameter!");
					Msg("SetEnvDescData: invalid 'sun_dir' parameter!");
					return false;
				}
				env_desc->sun_dir.setHP(DEG2RAD(v.y), DEG2RAD(v.x));
			}
			else if (strcmp(key, "flares") == 0)
			{
				const char* flares_name = lua_tostring(L, -1);
				Msg("flares_name: %s", flares_name);
				env_desc->lens_flare_id = env.eff_LensFlare->AppendDef(pSettings, flares_name);
				Msg("lens_flare_id: %d", env_desc->lens_flare_id);
			}
			else if (strcmp(key, "thunderbolt") == 0)
			{
				const char* thunderbolt_name = lua_tostring(L, -1);
				Msg("thunderbolt_name: %s", thunderbolt_name);
				env_desc->tb_id = env.eff_Thunderbolt->AppendDef(pSettings, thunderbolt_name);
				Msg("thunderbolt_id: %d", env_desc->tb_id);
			}
			TRY_READ_FLOAT("bolt_period", env_desc->bolt_period,)
			/*else if (strcmp(key, "bolt_period") == 0)
			{
				env_desc->bolt_period = lua_tonumber(L, -1);
			}*/
			TRY_READ_FLOAT("bolt_duration", env_desc->bolt_duration,)
			/*else if (strcmp(key, "bolt_duration") == 0)
			{
				env_desc->bolt_duration = lua_tonumber(L, -1);
			}*/
			else if (strcmp(key, "env_ambient") == 0)
			{
				const char* env_sect = lua_tostring(L, -1);
				if (env_sect && strcmp(env_sect, ""))
				{
					//Msg("env_ambient: %s", env_sect);
					//shared_str str;
					//str.set(env_sect);
					env_desc->env_ambient = env.AppendEnvAmb(env_sect);
				}
			}
			else
			{
				Msg("SetEnvDescData: unknown parameter '%s'!", key);
			}
		}
		else
		{
			Msg("SetEnvDescData: table key must be a string!");
			return false;
		}
		lua_pop(L, 1);
	}
	env_desc->on_device_destroy();
	env_desc->on_device_create();
	Log("init done");
	return true;
}