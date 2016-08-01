#if !defined(_DUMPMODULE_H_FBFFBA7D_971E_48e0_A589_6FD14E8965B6)
#define _DUMPMODULE_H_FBFFBA7D_971E_48e0_A589_6FD14E8965B6

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Windows.h>
#include <ImageHlp.h>
#include <Shlwapi.h>

#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Shlwapi.lib")

inline BOOL IsDataSectionNeeded(LPCWSTR pModuleName)
{
	BOOL bReturn(FALSE);
	if(pModuleName)
	{
		LPCWSTR pFileName = PathFindFileNameW(pModuleName);
		if(!lstrcmpiW(pFileName, L"ntdll.dll"))
		{
			bReturn = TRUE;
		}
	}
	return bReturn;
}

inline BOOL CALLBACK MiniDumpCallback(PVOID pParam, PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	BOOL bReturn(FALSE);
	if(pInput && pOutput)
	{
		switch(pInput->CallbackType)
		{
		case ModuleCallback:
			{
				if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
				{
					if(!IsDataSectionNeeded(pInput->Module.FullPath))
					{
						pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg); 
					}
				}
			}
		case IncludeModuleCallback:
		case IncludeThreadCallback:
		case ThreadCallback:
		case ThreadExCallback:
			{
				bReturn = TRUE;
			}
		default:break;
		}
	}
	return bReturn;
}

inline void CreateMiniDump(EXCEPTION_POINTERS* pep, LPCTSTR pszFileName)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId           = GetCurrentThreadId();
		mdei.ExceptionPointers  = pep;
		mdei.ClientPointers     = FALSE;
		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam       = 0;
		MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | 
			MiniDumpWithDataSegs   | 
			MiniDumpWithHandleData | 
			0x00000800 /*MiniDumpWithFullMemoryInfo*/ | 
			0x00001000 /*MiniDumpWithThreadInfo*/     | 
			MiniDumpWithUnloadedModules);
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep) ? &mdei : 0, 0, &mci);
		CloseHandle(hFile);
	}
}

inline LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	SYSTEMTIME st = {0};
	::GetLocalTime(&st);

	TCHAR exeFullPath[MAX_PATH] = {0};
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	PathRemoveExtension(exeFullPath);

	TCHAR dmpFullPath[MAX_PATH] = {0};
	wsprintf(dmpFullPath, _T("%s%04d%02d%02d%02d%02d%02d%03d.dmp"), exeFullPath, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	CreateMiniDump(pExceptionInfo, dmpFullPath);

	return EXCEPTION_CONTINUE_SEARCH;
}

extern "C" __declspec(selectany) LPTOP_LEVEL_EXCEPTION_FILTER g_pExcepFilter = SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);

#pragma comment(linker, "/include:_g_pExcepFilter")

#endif // !defined(_DUMPMODULE_H_FBFFBA7D_971E_48e0_A589_6FD14E8965B6)

