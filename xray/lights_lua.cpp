#include "lights_lua.h"
#include "xray/light.h"

int SetSpeed(lua_State *L){
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

const struct luaL_Reg lfuncs[] = {
/*	{ "set_speed", SetSpeed},
	{ "set_amount",	SetAmount},
	{ "set_smap_jitter",	SetSmapJitter},*/
	{ NULL,		NULL		}
};

int open_lights(lua_State *L){
	luaL_register(L, "lights", lfuncs);
	return 0;
}
