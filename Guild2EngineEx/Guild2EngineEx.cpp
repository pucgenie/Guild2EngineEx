// Guild2EngineEx.cpp : Definiert die exportierten Funktionen für die DLL-Anwendung.
//

#include "stdafx.h"
#include "lua.h"
#include "Guild2EngineEx.h"


#define CURRENT_GEE_VERSION 2

// x86 9A immediate far calls are not supported by MSVC++
const void* regA = reinterpret_cast<void*>(0x0070EE70);
const void* regB = reinterpret_cast<void*>(0x0070EDA0);
const void* regC = reinterpret_cast<void*>(0x0070E8E0);
const void* regD = reinterpret_cast<void*>(0x0070F0A0);

Closure* (*newCclosure) (lua_State *L, int nelems) = reinterpret_cast<Closure* (*) (lua_State *L, int nelems)>(0x00715120);

void(*printErrorMessage) (lua_State *L, LPCSTR msg, int arg1) = reinterpret_cast<void(*) (lua_State *L, LPCSTR str, int arg1)>(0x0053ED80);

// may overflow lua stack, check with stack_last
// -> guildii engine never checks its stack and never reallocates stack. This is possibly a cause to certain vanilla crashes.
// -> fixing this vanilla problem is too critical via binary patches
extern "C" int __cdecl GeeType(lua_State * state)
{
	if (state->top == state->base)
	{
		// no parameter given
		printErrorMessage(state, "missing parameter 1", 0);
		return 0;
	}
	state->top[0].tt = TYPE_NUMBER; // type of return value = number
	state->top[0].value.n = state->base[0].tt; // return value = type of parameter 1
	state->top++; // increase by sizeof(StkId)
	return 1; // we have 1 return value
}

// Return version constant of this extension
extern "C" int __cdecl GeeVersion(lua_State * state)
{
	state->top[0].tt = TYPE_NUMBER;
	state->top[0].value.n = CURRENT_GEE_VERSION;
	state->top++; // increase by sizeof(StkId)
	return 1;
}

// Return the length of given string or number of elements in table
extern "C" int __cdecl GeeLength(lua_State * state)
{
	if (state->top == state->base)
	{
		// no parameter given
		printErrorMessage(state, "missing parameter 1", 0);
		return 0;
	}
	state->top[0].tt = TYPE_NUMBER;
	switch (state->base[0].tt)
	{
	case TYPE_TABLE:
		int size;
		size = state->base[0].value.gc->h.sizearray;
		// LUA tables are like Lists in common managed languages. Their memory expand on demand.
		// But they do not know their real length. So the first 'nil' value will terminate the table
		// at that index. So LUA statement 'table[4] = nil' would cut the table to length '3'(LUA 
		// indexing starts with 1 -.-).
		for (int i = 0; i < size; i++) 
		{
			if (state->base[0].value.gc->h.array[i].tt == TYPE_NIL)
			{
				// found end of table
				size = i;
				break;
			}
		}
		state->top[0].value.n = size;
		break;
	case TYPE_STRING:
		state->top[0].value.n = state->base[0].value.gc->ts.tsv.len;
		break;
	default:
		state->top[0].value.n = 0; // LUA specs for 'length' are undefined for values other than table and string.
	}
	state->top++; // increase by sizeof(StkId)
	return 1;
}

extern "C" int __cdecl  _iterate(lua_State * state)
{
	const int i = static_cast<int>(state->base[1].value.n) + 1;
	if (state->base[0].value.gc->h.array == nullptr)
	{
		// empty table
		return 0;
	}
	if (i >= state->base[0].value.gc->h.sizearray)
	{
		// end iteration due to end of table
		return 0;
	}
	TObject * entry = &state->base[0].value.gc->h.array[i - 1];
	if (entry->tt != TYPE_NIL)
	{
		// copy this element to return value
		state->top[0].tt = TYPE_NUMBER;
		state->top[0].value.n = i;
		state->top[1].tt = entry->tt;
		state->top[1].value = entry->value;
		state->top += 2;
		return 2;
	}
	
	// end iteration - maybe there are more elements but LUA specs say we need to end iteration on first NIL.
	return 0;
}

// We could also export this function to Guild II engine but we don't need it. We can implement
// this with plain Guild II LUA also without hacking into the garbage collector.
extern "C" int __cdecl GeeIPairs(lua_State * state) 
{
	if (state->base[0].tt != TYPE_TABLE)
	{
		printErrorMessage(state, "wrong parameter 1 - 'table' expected", 0);
		return 0;
	}
	state->top[0].tt = TYPE_FUNCTION;
	Closure * c = newCclosure(state, 1); // create garbage collectible container for function delegate
	c->c.f = &_iterate;
	state->top[0].value.gc = reinterpret_cast<GCObject*>(c);
	state->top[1].tt = TYPE_TABLE;
	state->top[1].value.gc = state->base[0].value.gc;
	state->top[2].tt = TYPE_NUMBER;
	state->top[2].value.n = 0;
	state->top += 3;
	return 3; // we have 3 return values
}

const new_lua_function new_functions[] = {
	{ "GeeType", &GeeType },
	{ "GeeVersion", &GeeVersion },
	{ "GeeLength", &GeeLength },
	//{ "GeeIPairs", &GeeIPairs },
	};


extern "C" void __declspec(dllexport) __cdecl GeeRegisterExtension(int lua_register_address)
{
	int num_functions = sizeof(new_functions)/8;
	for (auto i = 0; i < num_functions; i++)
	{
		LPCSTR func_name = new_functions[i].func_name;
		int (*func)(lua_State*) = new_functions[i].func;
		// call the LUA registration methods without modifying the stack.
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

