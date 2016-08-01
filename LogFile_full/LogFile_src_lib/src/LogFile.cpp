#include "StdAfx.h"
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <clocale>
#include <cstdio>
#include <shlwapi.h>
#include "LogFile.h"

#pragma comment(lib, "shlwapi.lib")

#pragma warning(disable: 4996)


WORD			  CLogFile::sm_wType = 0;
CLogFile*		  CLogFile::sm_pMe   = 0;
CRITICAL_SECTION  CLogFile::sm_hCrtObject;
CLogFile::CInitor CLogFile::sm_Initor(sm_hCrtObject);


CLogFile::CInitor::CInitor(CRITICAL_SECTION& hCrtSec)
: m_hCrtObject(hCrtSec)
{
	_wsetlocale(LC_ALL, L"chs");

	GetModuleFileNameW(NULL, m_szFileName, MAX_PATH);
	PathRenameExtensionW(m_szFileName, L".log");

	InitializeCriticalSection(&m_hCrtObject);
}

CLogFile::CInitor::~CInitor()
{
	DeleteCriticalSection(&m_hCrtObject);
}

void CLogFile::__CreateFile()
{
	LPCWSTR pszFileName = sm_Initor.m_szFileName;
	
	WCHAR szExtension[48]	   = {0};
	WCHAR szFileName[MAX_PATH] = {0};
	lstrcpynW(szFileName, pszFileName, MAX_PATH);
	wsprintfW(szExtension, L".%04d-%02d-%02d.log", m_stLocal.wYear, m_stLocal.wMonth, m_stLocal.wDay);
	PathRenameExtensionW(szFileName, szExtension);

	m_pFile = _wfsopen(szFileName, L"a+", 0x20);
	
	int nNumber = 1;
	while (!m_pFile && nNumber < 100)
	{
		WCHAR szExtension[48] = {0};
		wsprintfW(szExtension, L".%d.log", nNumber++);
		WCHAR szCurrentFile[MAX_PATH] = {0};
		lstrcpynW(szCurrentFile, szFileName, MAX_PATH);
		PathRenameExtensionW(szCurrentFile, szExtension);
		m_pFile = _wfsopen(szCurrentFile, L"a+", 0x20);
	}
}

void CLogFile::__CheckFile(const SYSTEMTIME& sysTime)
{
	if (sysTime.wDay != m_stLocal.wDay || sysTime.wMonth != m_stLocal.wMonth
		|| sysTime.wYear != m_stLocal.wYear)
	{
		m_stLocal = sysTime;
		if (m_pFile)
		{
			fclose(m_pFile);
		}
		__CreateFile();
	}
}

CLogFile::CLogFile()
{
	if (sm_wType & LT_FILE)
	{
		GetLocalTime(&m_stLocal);
		__CreateFile();
	}
	if (sm_wType & LT_CONSOLE)
	{
		AllocConsole();
		_wfreopen(L"CONOUT$", L"w+", stdout);
		
		SetConsoleTitleW(L"Log Recorder");
		
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
		GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
		
		SMALL_RECT rect = csbiInfo.srWindow;
		COORD coord = {200, 600};
		rect.Right = 599;
		
		SetConsoleScreenBufferSize(hConsole, coord);
		SetConsoleWindowInfo(hConsole, FALSE, &rect);
		CloseHandle(hConsole);
	}

	InitializeCriticalSection(&m_hWrCrtSec);
}

CLogFile::~CLogFile()
{
	DeleteCriticalSection(&m_hWrCrtSec);

	if (sm_wType & LT_FILE)
	{
		if (m_pFile)
		{
			fclose(m_pFile);
		}
	}
	if (sm_wType & LT_CONSOLE)
	{
		FreeConsole();
	}
}

CLogFile* CLogFile::__InitLogger()
{
	if (!sm_pMe)
	{
		EnterCriticalSection(&sm_hCrtObject);
		if (!sm_pMe)
		{
			static CLogFile Me;
			sm_pMe = &Me;
		}
		LeaveCriticalSection(&sm_hCrtObject);
	}
	return sm_pMe;
}

void CLogFile::__OutputMessage(LPCSTR lpFile, DWORD dwLine, LPCSTR lpMessage)
{
	WCHAR wszMessage[3030] = {0};
	mbstowcs(wszMessage, lpMessage, 3030);
	__OutputMessage(lpFile, dwLine, wszMessage);
}

void CLogFile::__OutputMessage(LPCSTR lpFile, DWORD dwLine, LPCWSTR lpMessage)
{
	LPCSTR lpFileName = PathFindFileNameA(lpFile);
	
	WCHAR szFileName[MAX_PATH] = {0};
	mbstowcs(szFileName, lpFileName, MAX_PATH);
	
	WCHAR szSysTime[256]  = {0};
	WCHAR szMsgLine[4096] = {0};
	
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	
	wsprintfW(szSysTime, L"%04d%02d%02d %02d:%02d:%02d:%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	wsprintfW(szMsgLine, L"[%s][%d][%d][%s:%d]%s\n", szSysTime, GetCurrentProcessId(), GetCurrentThreadId(), szFileName, dwLine, lpMessage);
	
	EnterCriticalSection(&m_hWrCrtSec);
	
	if (sm_wType & LT_FILE)
	{
		__CheckFile(st);
		if (m_pFile)
		{
			fputws(szMsgLine, m_pFile);
			fflush(m_pFile);
		}
	}
#ifdef _DEBUG
	OutputDebugStringW(szMsgLine);
#endif
	if (sm_wType & LT_CONSOLE)
	{
		wprintf(szMsgLine);
	}
	
	LeaveCriticalSection(&m_hWrCrtSec);
}

void CLogFile::SetOption(WORD wType)
{
	sm_wType = wType;
}

void CLogFile::SetOption(WORD wType, LPCSTR pszFileName)
{
	sm_wType = wType;
	if (pszFileName)
	{
		WCHAR wszFileName[MAX_PATH] = {0};
		mbstowcs(wszFileName, pszFileName, MAX_PATH);
		SetOption(wType, wszFileName);
	}
}

void CLogFile::SetOption(WORD wType, LPCWSTR pszFileName)
{
	sm_wType = wType;
	if (pszFileName)
	{
		lstrcpynW(sm_Initor.m_szFileName, pszFileName, MAX_PATH);
	}
}

void CLogFile::Log(LPCSTR lpFile, DWORD dwLine, LPCSTR lpFormat, ...)
{
	CHAR szMessage[3030] = {0};
		
	va_list args;
	va_start(args, lpFormat);
	wvsprintfA(szMessage, lpFormat, args);
	va_end(args);
		
	__InitLogger()->__OutputMessage(lpFile, dwLine, szMessage);
}

void CLogFile::Log(LPCSTR lpFile, DWORD dwLine, LPCWSTR lpFormat, ...)
{
	WCHAR szMessage[3030] = {0};
	
	va_list args;
	va_start(args, lpFormat);
	wvsprintfW(szMessage, lpFormat, args);
	va_end(args);
	
	__InitLogger()->__OutputMessage(lpFile, dwLine, szMessage);
}

void CLogFile::FOTracer::operator()(LPCSTR lpFormat, ...)
{
	CHAR szMessage[3030] = {0};
		
	va_list args;
	va_start(args, lpFormat);
	wvsprintfA(szMessage, lpFormat, args);
	va_end(args);
		
	CLogFile::__InitLogger()->__OutputMessage(m_lpFile, m_dwLine, szMessage);
}

void CLogFile::FOTracer::operator()(LPCWSTR lpFormat, ...)
{
	WCHAR szMessage[3030] = {0};
	
	va_list args;
	va_start(args, lpFormat);
	wvsprintfW(szMessage, lpFormat, args);
	va_end(args);
	
	CLogFile::__InitLogger()->__OutputMessage(m_lpFile, m_dwLine, szMessage);
}

