#include "volumetric_fog_lua.h"
#include "xray/fog_volumes.h"

int GetDensity(lua_State *L){
	if (lua_gettop(L) == 1)
	{
		const char *name = luaL_checklstring(L, 1, NULL);
		xr_vector<CFogVolume>& vols = FV->m_volumes;
		CFogVolume* vol = NULL;
		for (xr_vector<CFogVolume>::iterator it = FV->m_volumes.begin(), end = FV->m_volumes.end(); it != end; ++it)
		{
			if (!strcmp(*(it->m_name), name))
			{
				vol = &(*it);
				break;
			}
		}
		if (!vol)
			return luaL_error(L, "GetDensity: fog volume %s not found", name);
		double res = vol->fMaxDensity;
		lua_pushnumber(L, res);
		return 1;
	}
	return luaL_error(L, "GetDensity must have one argument");
}
int SetDensity(lua_State *L){
	if (lua_gettop(L) == 2)
	{
		const char *name = luaL_checklstring(L, 1, NULL);
		double density = luaL_checknumber(L, 2);
		xr_vector<CFogVolume>& vols = FV->m_volumes;
		CFogVolume* vol = NULL;
		for (xr_vector<CFogVolume>::iterator it = FV->m_volumes.begin(), end = FV->m_volumes.end(); it != end; ++it)
		{
			if (!strcmp(*(it->m_name), name))
			{
				it->fMaxDensity = density;
				vol = &(*it);
				break;
			}
		}
		if (!vol)
			return luaL_error(L, "SetDensity: fog volume %s not found", name);
		return 0;
	}
	return luaL_error(L, "SetDensity must have two argument");
}

const struct luaL_Reg vf_funcs[] = {
	{ "GetDensity", GetDensity},
	{ "SetDensity",	SetDensity},
	{ NULL,		NULL		}
};

int open_volumetric_fog(lua_State *L){
	luaL_register(L, "vol_fog", vf_funcs);
	return 0;
}
