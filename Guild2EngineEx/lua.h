#pragma once

typedef struct _luaParameter {
	int parType;
	int badfood;
	union
	{
		double number;
		char *nullTerminatedStr;
		// TODO: boolean?
		// TODO: table?
	};
	} LuaParameter;

typedef struct _luaState {
	int somePointer;
	short someInt;
	short badfood;
	LuaParameter * top;  /* first free slot in the stack */
	LuaParameter * base; /* base of current function */
	} LuaState;