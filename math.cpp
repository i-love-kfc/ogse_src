#include "math.h"

int Clamp(lua_State *L){
	if (lua_gettop(L) == 3)
	{
		double lim1 = luaL_checknumber(L, 1);
		double lim2 = luaL_checknumber(L, 2);
		if (lim1 > lim2) return luaL_error(L, "Clamp: lim1 must be less than lim2");
		double num  = luaL_checknumber(L, 3);
		double res;
		if (num < lim1)
		{
			res = lim1;
		}
		else if (num > lim2)
		{
			res = lim2;
		}
		else
		{
			res = num;
		}
		lua_pushnumber(L, res);
		return 1;
	}
	return luaL_error(L, "Clamp must have three arguments");
}
int Clamp01(lua_State *L){
	if (lua_gettop(L) == 1)
	{
		double num  = luaL_checknumber(L, 1);
		double res;
		if (num < 0.0)
		{
			res = 0.0;
		}
		else if (num > 1.0)
		{
			res = 1.0;
		}
		else
		{
			res = num;
		}
		lua_pushnumber(L, res);
		return 1;
	}
	return luaL_error(L, "Clamp01 must have one argument");
}


const struct luaL_Reg math_funcs[] = {
	{ "clamp",   Clamp},
	{ "clamp01", Clamp01},
	{ NULL,		NULL		}
};

int open_math(lua_State *L){
	luaL_register(L, "math", math_funcs);
	return 0;
}
