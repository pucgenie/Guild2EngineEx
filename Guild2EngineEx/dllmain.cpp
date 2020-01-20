// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "stdafx.h"

extern "C" int __stdcall DllMain( int hModule,
                       int  ul_reason_for_call,
                       int lpReserved
                     )
{
    return 1;
}
