#pragma once

#include "xray\Environment.h"

#include <Lua.hpp>
//#include <map>
//#include <string>

//using std::string;
//using std::map;

bool InitEnvDescriptor(CEnvDescriptor*env_desc, lua_State *L, int stack_idx);
