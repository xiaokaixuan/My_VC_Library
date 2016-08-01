#include "StdAfx.h"
#include <cstdio>
#include <stdarg.h>
#include <clocale>
#include <shlwapi.h>
#include "LogFile.h"

#pragma warning(disable: 4996)
#pragma comment(lib, "shlwapi.lib")


CLogFile*			CLogFile::sm_pMe(NULL);
CRITICAL_SECTION	CLogFile::sm_hCrtGen;
CLogFile::tagInitor CLogFile::sm_Initor(sm_hCrtGen);


CLogFile::tagInitor::tagInitor(CRITICAL_SECTION& hCrtSec)
: m_hCrtSec(hCrtSec)
{
	_tsetlocale(LC_ALL, _T("chs"));

	InitializeCriticalSection(&m_hCrtSec);

	GetModuleFileName(NULL, m_szFileName, MAX_PATH);
	PathRenameExtension(m_szFileName, _T(".log"));
}

CLogFile::tagInitor::~tagInitor()
{
	DeleteCriticalSection(&m_hCrtSec);
}

LPCTSTR CLogFile::tagInitor::GetFileName() const
{
	return m_szFileName;
}

void CLogFile::tagInitor::SetFileName(LPCTSTR pszFileName)
{
#if defined(LOG_FILE)
	lstrcpyn(sm_Initor.m_szFileName, pszFileName, MAX_PATH);
#endif // defined(LOG_FILE)
}

CLogFile::CLogFile()
{
#if defined(LOG_CONSOLE)
#if !defined(_CONSOLE)
		AllocConsole();
		_tfreopen(_T("CONOUT$"), _T("w+"), stdout);

		SetConsoleTitle(_T("Log Recorder"));

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {0};
		GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

		SMALL_RECT rect = csbiInfo.srWindow;
		COORD coord = {200, 600};
		rect.Right = 599;

		SetConsoleScreenBufferSize(hConsole, coord);
		SetConsoleWindowInfo(hConsole, FALSE, &rect);
		CloseHandle(hConsole);
#endif // !defined(_CONSOLE)
#endif // defined(LOG_CONSOLE)

#if defined(LOG_FILE)
		GetLocalTime(&m_stLocal);
		__CreateFile();
#endif // defined(LOG_FILE)

	InitializeCriticalSection(&m_hWrCrtSec);
}

CLogFile::~CLogFile()
{
	DeleteCriticalSection(&m_hWrCrtSec);

#if defined(LOG_CONSOLE)
#if !defined(_CONSOLE)
	FreeConsole();
#endif // !defined(_CONSOLE)
#endif // defined(LOG_CONSOLE)

#if defined(LOG_FILE)
	if (m_pFile)
	{
		fclose(m_pFile);
	}
#endif // defined(LOG_FILE)
}

CLogFile* CLogFile::__InitLogger()
{
	if (!sm_pMe)
	{
		EnterCriticalSection(&sm_hCrtGen);
		if (!sm_pMe)
		{
			static CLogFile Me;
			sm_pMe = &Me;
		}
		LeaveCriticalSection(&sm_hCrtGen);
	}
	return sm_pMe;
}

void CLogFile::__CreateFile()
{
	LPCTSTR pszFileName = sm_Initor.GetFileName();
	TCHAR szExtName[48]		   = {0};
	TCHAR szFileName[MAX_PATH] = {0};
	lstrcpyn(szFileName, pszFileName, MAX_PATH);
	_stprintf(szExtName, _T(".%04hd-%02hd-%02hd.log"), m_stLocal.wYear, m_stLocal.wMonth, m_stLocal.wDay);
	PathRenameExtension(szFileName, szExtName);

	m_pFile = _tfsopen(szFileName, _T("a+"), 0x20);

	int nNumber = 1;
	while (!m_pFile)
	{
		TCHAR szExtName[48] = {0};
		_stprintf(szExtName, _T(".%d.log"), nNumber++);
		TCHAR szLogfile[MAX_PATH] = {0};
		lstrcpyn(szLogfile, szFileName, MAX_PATH);
		PathRenameExtension(szLogfile, szExtName);
		m_pFile = _tfsopen(szLogfile, _T("a+"), 0x20);
	}
}

void CLogFile::__CheckFile(const SYSTEMTIME& systime)
{
	if (systime.wDay != m_stLocal.wDay || systime.wMonth != m_stLocal.wMonth 
		|| systime.wYear != m_stLocal.wYear)
	{
		m_stLocal = systime;
		if (m_pFile)
		{
			fclose(m_pFile);
		}
		__CreateFile();
	}
}

void CLogFile::__WriteMessage(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpMessage)
{
	LPCSTR lpFileName = PathFindFileNameA(lpFile);

	TCHAR szFileName[MAX_PATH] = {0};
#if defined(_UNICODE) || defined(UNICODE)
	mbstowcs(szFileName, lpFileName, MAX_PATH);
#else
	lstrcpyn(szFileName, lpFileName, MAX_PATH);
#endif // defined(_UNICODE) || defined(UNICODE)

	TCHAR szDateTime[256]	 = {0};
	TCHAR szLogMessage[4096] = {0};

	SYSTEMTIME st = {0};
	GetLocalTime(&st);

	_stprintf(szDateTime, _T("%04d%02d%02d %02d:%02d:%02d:%03d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	_stprintf(szLogMessage, _T("[%s][%d][%d][%s:%d]%s\n"), szDateTime, GetCurrentProcessId(), GetCurrentThreadId(), szFileName, dwLine, lpMessage);

	EnterCriticalSection(&m_hWrCrtSec);

#if defined(LOG_CONSOLE)
	_tprintf(szLogMessage);
#endif // defined(LOG_CONSOLE)

#ifdef _DEBUG
	OutputDebugString(szLogMessage);
#endif // _DEBUG

#if defined(LOG_FILE)
	__CheckFile(st);
	_fputts(szLogMessage, m_pFile);
	fflush(m_pFile);
#endif // defined(LOG_FILE)

	LeaveCriticalSection(&m_hWrCrtSec);
}

void CLogFile::SetFileName(LPCTSTR pszFileName)
{
	sm_Initor.SetFileName(pszFileName);
}

void CLogFile::Log(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpFormat, ...)
{
	TCHAR szMessage[3030] = {0};

	va_list args;
	va_start(args, lpFormat);
	_vstprintf(szMessage, lpFormat, args);	
	va_end(args);

	__InitLogger()->__WriteMessage(lpFile, dwLine, szMessage);
}

void CLogFile::FOTracer::operator()(LPCTSTR lpFormat, ...)
{
	TCHAR szMessage[3030] = {0};

	va_list args;
	va_start(args, lpFormat);
	_vstprintf(szMessage, lpFormat, args);
	va_end(args);

	CLogFile::__InitLogger()->__WriteMessage(m_lpFile, m_dwLine, szMessage);
}

