#pragma once

/*
Copyright © 1994–2019 Lua.org, PUC - Rio.
Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or 
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.
*/

#define TYPE_NIL 0
#define TYPE_BOOLEAN 1
#define TYPE_NUMBER 3
#define TYPE_STRING 4
#define TYPE_TABLE 5
#define TYPE_FUNCTION 6

typedef union GCObject GCObject;

#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked

#define ClosureHeader \
	CommonHeader; lu_byte isC; lu_byte nupvalues; GCObject *gclist

typedef double lua_Number;

typedef struct _lua_State lua_State;

typedef int(*lua_CFunction) (lua_State *L);

typedef unsigned char lu_byte;

typedef union {
	GCObject *gc;
	void *p;
	lua_Number n;
	int b;
} Value;

typedef struct lua_TObject {
	int tt;
	Value value;
} TObject;

typedef TObject *StkId;  /* index to stack elements */

typedef struct GCheader {
	CommonHeader;
} GCheader;

typedef struct CClosure {
	ClosureHeader;
	lua_CFunction f;
	TObject upvalue[1];
} CClosure;


typedef union Closure {
	CClosure c;
} Closure;

typedef struct Node {
	TObject i_key;
	TObject i_val;
	struct Node *next;  /* for chaining */
} Node;

typedef struct Table {
	CommonHeader;
	lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
	lu_byte lsizenode;  /* log2 of size of `node' array */
	struct Table *metatable;
	TObject *array;  /* array part */
	Node *node;
	Node *firstfree;  /* this position is free; all positions after it are full */
	GCObject *gclist;
	int sizearray;  /* size of `array' array */
} Table;

typedef union { double u; void *s; long l; } L_Umaxalign;

typedef union TString {
	L_Umaxalign dummy;  /* ensures maximum alignment for strings */
	struct {
		CommonHeader;
		lu_byte reserved;
		unsigned int hash;
		size_t len;
	} tsv;
} TString;

union GCObject {
	GCheader gch;
	union TString ts;
	union Closure cl;
	struct Table h;
};

typedef struct CallInfo {
	StkId base;  /* base for called function */
	StkId	top;  /* top for this function */
	int state;  /* bit fields; see below */
	union {
		struct {  /* for Lua functions */
			const unsigned long *savedpc;
			const unsigned long **pc;  /* points to `pc' variable in `luaV_execute' */
			int tailcalls;  /* number of tail calls lost under this entry */
		} l;
		struct {  /* for C functions */
			int dummy;  /* just to avoid an empty struct */
		} c;
	} u;
} CallInfo;

typedef struct _lua_State {
	CommonHeader;
	StkId top;  /* first free slot in the stack */
	StkId base;  /* base of current function */
	int l_G; // global_state ptr
	CallInfo *ci;  /* call info for current function */
	StkId stack_last;  /* last free slot in the stack */
	StkId stack;  /* stack base */
	int stacksize;
	CallInfo *end_ci;  /* points after end of ci array*/
	CallInfo *base_ci;  /* array of CallInfo's */
	unsigned short size_ci;  /* size of array `base_ci' */
	unsigned short nCcalls;  /* number of nested C calls */
	lu_byte hookmask;
	lu_byte allowhook;
	lu_byte hookinit;
	int basehookcount;
	int hookcount;
	int hook; // lua_Hook ptr
	TObject _gt;  /* table of globals */
	GCObject *openupval;  /* list of open upvalues in this stack */
	GCObject *gclist;
	struct lua_longjmp *errorJmp;  /* current error recover point */
	ptrdiff_t errfunc;  /* current error handling function (stack index) */
} lua_State;
