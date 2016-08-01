#include "StdAfx.h"
#include "DumpModule.h"

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Shlwapi.lib")

extern "C" LPTOP_LEVEL_EXCEPTION_FILTER g_pExcepFilter = SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);

#pragma comment(linker, "/include:_g_pExcepFilter")