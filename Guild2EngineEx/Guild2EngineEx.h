#pragma once

extern "C" void __declspec(dllexport) __cdecl GeeRegisterExtension(int luaRegisterAddress);


typedef struct _new_lua_function {
	LPCSTR func_name;
	int ( *func)(lua_State*);
	} new_lua_function;