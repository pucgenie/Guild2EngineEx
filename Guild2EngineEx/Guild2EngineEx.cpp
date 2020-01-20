// Guild2EngineEx.cpp : Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"
#include "lua.h"
#include "Guild2EngineEx.h"

void* regA = reinterpret_cast<void*>(0x0070EE70);
void* regB = reinterpret_cast<void*>(0x0070EDA0);
void* regC = reinterpret_cast<void*>(0x0070E8E0);
void* regD = reinterpret_cast<void*>(0x0070F0A0);


extern "C" int __cdecl GeeType(LuaState * state)
{
	state->top[0].parType = 3;
	state->top[0].number = state->base[0].parType;
	state->top++; // increase by sizeof(LuaState)
	return 1;
}

extern "C" int __cdecl GeeVersion(LuaState * state)
{
	state->top[0].parType = 3;
	state->top[0].number = 1;
	state->top++; // increase by sizeof(LuaState)
	return 1;
}

const new_lua_function new_functions[] = {
	{ "GeeType", &GeeType },
	{ "GeeVersion", &GeeVersion }
	};


extern "C" void __declspec(dllexport)   __cdecl GeeRegisterExtension(int lua_register_address)
{
	int num_functions = sizeof(new_functions)/8;
	for (auto i = 0; i < num_functions; i++)
	{
		const char * func_name = new_functions[i].func_name;
		int(*func)(LuaState*) = new_functions[i].func;
		__asm {
			push 0
			push func
			push lua_register_address
			call regA
			push func_name
			push lua_register_address
			call regB
			push 0xFFFFFFFE
			push lua_register_address
			call regC
			push 0xFFFFD8EF
			push lua_register_address
			call regD
			add esp, 0x24
		}
	}
}

