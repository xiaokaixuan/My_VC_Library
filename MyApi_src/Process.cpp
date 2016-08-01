#include "StdAfx.h"
#include <windows.h>
#include "kxExapi.h"

BOOL kxShellProcess(LPHANDLE lpHandle, LPCTSTR lpExeFile, LPCTSTR lpParameters, int nShow, BOOL bWait)
{
	BOOL bRet(FALSE);
	if (NULL != lpHandle)
	{
		*lpHandle = NULL;
	}
	SHELLEXECUTEINFO sif = {0};
	sif.cbSize = sizeof(sif);
	sif.fMask = SEE_MASK_NOCLOSEPROCESS;
	sif.lpFile = lpExeFile;
	sif.nShow = nShow;
	sif.lpVerb = _T("Open");
	sif.lpParameters = lpParameters;
	if (ShellExecuteEx(&sif))
	{
		bRet = TRUE;
		if (bWait)
		{
			::WaitForSingleObject(sif.hProcess, INFINITE);
			::CloseHandle(sif.hProcess);
		}
		else
		{
			*lpHandle = sif.hProcess;
		}
	}
	return bRet;
}

BOOL kxCreateProcess(LPHANDLE lpHandle, LPCTSTR lpFileName, LPCTSTR lpParameters, int nShow, BOOL bWait)
{
	BOOL bRet(FALSE);
	if (NULL != lpHandle)
	{
		*lpHandle = NULL;
	}
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = nShow;
	PROCESS_INFORMATION pi = {0};
	TCHAR CmdLine[2048] = {0};
	lstrcat(CmdLine, lpFileName);
	lstrcat(CmdLine, _T(" "));
	lstrcat(CmdLine, lpParameters);
	if (CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		bRet = TRUE;
		if (bWait)
		{
			::WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
		}
		else
		{
			*lpHandle = pi.hProcess;
		}
	}
	return bRet;
}
