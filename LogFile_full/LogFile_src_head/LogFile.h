#if !defined(__LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)
#define __LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <clocale>
#include <cstdio>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma warning(disable: 4996)

class CLogFile
{
public:
	static CLogFile* InitLogger(LPCTSTR lpLogName = NULL);
	static void Log(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpFormat, ...);

public:
	void Write(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpMessage);

public:
	virtual ~CLogFile();

public:
	class FOTracer
	{
	private:
		LPCSTR m_lpFile;
		DWORD  m_dwLine;
		BYTE   m_cbType;
	public:
		FOTracer(LPCSTR lpFile, DWORD dwLine, BYTE cbType = -1): m_lpFile(lpFile), m_dwLine(dwLine), m_cbType(cbType) { }
		
		void operator()(LPCTSTR lpFormat, ...);
	};
	friend class FOTracer;

private:
	CLogFile(LPCTSTR lpLogName);
	CLogFile(const CLogFile &other);
	CLogFile& operator = (const CLogFile &);

private:
	static CLogFile* m_pMe;

private:
	FILE* m_pFile;
	CRITICAL_SECTION m_hWrCrtSec;
};

__declspec(selectany) CLogFile* CLogFile::m_pMe = NULL;

// Usage: LOG_INIT() or LOG_INIT(_T(C:\\Sample.log));
#define LOG_INIT				CLogFile::InitLogger

// Usage: LOG[_][V|D|I|W|E](_T("%s %d"), szMsg, nNumber);
#define LOG						CLogFile::FOTracer(__FILE__, __LINE__)
#define LOG_V					CLogFile::FOTracer(__FILE__, __LINE__, 0)
#define LOG_D					CLogFile::FOTracer(__FILE__, __LINE__, 1)
#define LOG_I					CLogFile::FOTracer(__FILE__, __LINE__, 2)
#define LOG_W					CLogFile::FOTracer(__FILE__, __LINE__, 3)
#define LOG_E					CLogFile::FOTracer(__FILE__, __LINE__, 4)


inline CLogFile::CLogFile(LPCTSTR lpLogName)
{
#if defined(LOG_CONSOLE)
#if !defined(_CONSOLE)
	AllocConsole();
	_tfreopen(_T("CONOUT$"), _T("w+"), stdout);
	
	TCHAR szTitle[512] = _T("Log Recorder: ");
	lstrcat(szTitle, PathFindFileName(lpLogName));
	SetConsoleTitle(szTitle);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo = { 0 };
	GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	SMALL_RECT rect = csbiInfo.srWindow;
	COORD coord = { 200, 600 };
	rect.Right = 599;

	SetConsoleScreenBufferSize(hConsole, coord);
	SetConsoleWindowInfo(hConsole, FALSE, &rect);
	CloseHandle(hConsole);
#endif // !defined(_CONSOLE)
#else
#if !defined(_DEBUG)
	m_pFile = _tfsopen(lpLogName, _T("a+"), 0x20);

	int nNum(1);
	while (!m_pFile)
	{
		TCHAR szExtName[44] = { 0 };
		_stprintf(szExtName, _T(".%d.log"), nNum++);
		TCHAR szLogName[MAX_PATH] = {0};
		lstrcpyn(szLogName, lpLogName, MAX_PATH);
		PathRenameExtension(szLogName, szExtName);
		m_pFile = _tfsopen(szLogName, _T("a+"), 0x20);
	}
#endif // !defined(_DEBUG)
#endif // defined(LOG_CONSOLE)

	InitializeCriticalSection(&m_hWrCrtSec);
}

inline CLogFile::~CLogFile()
{
	DeleteCriticalSection(&m_hWrCrtSec);

#if defined(LOG_CONSOLE)
#if !defined(_CONSOLE)
	FreeConsole();
#endif // !defined(_CONSOLE)
#else
#if !defined(_DEBUG)
	if (m_pFile)
	{
		fclose(m_pFile);
	}
#endif // !defined(_DEBUG)
#endif // defined(LOG_CONSOLE)
}

inline CLogFile* CLogFile::InitLogger(LPCTSTR lpLogName/* = NULL*/)
{
	if (!m_pMe)
	{
		_tsetlocale(LC_ALL, _T("chs"));
		TCHAR szLogName[MAX_PATH] = { 0 };
		if (lpLogName)
		{
			lstrcpyn(szLogName, lpLogName, MAX_PATH);
		}
		else
		{
			HMODULE hModule(NULL);
#if defined(GetModuleHandleEx)
			LPCTSTR pszModuleName = (LPCTSTR)InitLogger;
			GetModuleHandleEx(0x00000004, pszModuleName, &hModule);
#endif // defined(GetModuleHandleEx)
			GetModuleFileName(hModule, szLogName, MAX_PATH);
			PathRenameExtension(szLogName, _T(".log"));
		}
		static CLogFile Me(szLogName);
		m_pMe = &Me;
	}
	return m_pMe;
}

inline void CLogFile::Write(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpMessage)
{
	LPCSTR lpFileName = PathFindFileNameA(lpFile);

	TCHAR szFileName[MAX_PATH] = { 0 };
#if defined(_UNICODE) || defined(UNICODE)
	mbstowcs(szFileName, lpFileName, MAX_PATH);
#else
	lstrcpyn(szFileName, lpFileName, MAX_PATH);
#endif // defined(_UNICODE) || defined(UNICODE)

	TCHAR szDateTime[256] = { 0 };
	TCHAR szLogMessage[4096] = { 0 };

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	_stprintf(szDateTime, _T("%04d%02d%02d %02d:%02d:%02d:%03d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	_stprintf(szLogMessage, _T("[%s][%d][%d][%s:%d]%s\n"), szDateTime, GetCurrentProcessId(), GetCurrentThreadId(), szFileName, dwLine, lpMessage);

	EnterCriticalSection(&m_hWrCrtSec);

#if defined(LOG_CONSOLE)
	_tprintf(szLogMessage);
#else
#ifdef _DEBUG
	OutputDebugString(szLogMessage);
#else
	_fputts(szLogMessage, m_pFile);
	fflush(m_pFile);
#endif // _DEBUG
#endif // !defined(LOG_CONSOLE)

	LeaveCriticalSection(&m_hWrCrtSec);
}

inline void CLogFile::Log(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpFormat, ...)
{
	if (m_pMe)
	{
		TCHAR szMessage[3030] = { 0 };
		va_list args;
		va_start(args, lpFormat);
		_vstprintf(szMessage, lpFormat, args);
		va_end(args);
		
		m_pMe->Write(lpFile, dwLine, szMessage);
	}
}

inline void CLogFile::FOTracer::operator()(LPCTSTR lpFormat, ...)
{
	if (CLogFile::m_pMe)
	{
		TCHAR szPrefix[32] = { 0 };
		switch (m_cbType)
		{
		case 0:
			lstrcpyn(szPrefix, _T("[VERBOSE]"), 32);
			break;
		case 1:
			lstrcpyn(szPrefix, _T("[DEBUG]"), 32);
			break;
		case 2:
			lstrcpyn(szPrefix, _T("[INFO]"), 32);
			break;
		case 3:
			lstrcpyn(szPrefix, _T("[WARNING]"), 32);
			break;
		case 4:
			lstrcpyn(szPrefix, _T("[ERROR]"), 32);
			break;
		}

		TCHAR szMessage[3030] = { 0 };
		LPTSTR pszMessage = szMessage;
		lstrcpyn(pszMessage, szPrefix, 3030);
		pszMessage += lstrlen(szPrefix);

		va_list args;
		va_start(args, lpFormat);
		_vstprintf(pszMessage, lpFormat, args);
		va_end(args);
		
		CLogFile::m_pMe->Write(m_lpFile, m_dwLine, szMessage);
	}
}

#endif // !defined(__LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)


