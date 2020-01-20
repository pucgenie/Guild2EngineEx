#pragma once

extern "C" void __declspec(dllexport) __cdecl GeeRegisterExtension(int luaRegisterAddress);


typedef struct _new_lua_function {
	const char * func_name;
	int ( *func)(LuaState*);
	} new_lua_function;