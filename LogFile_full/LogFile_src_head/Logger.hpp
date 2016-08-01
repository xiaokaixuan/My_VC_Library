#if !defined(__LOGGER_H__)
#define __LOGGER_H__

#include <windows.h>
#include <tchar.h>
#include <stdarg.h>
#include <clocale>
#include <cstdio>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma warning(disable: 4996)

namespace NSLOG
{
	class CWriter
	{
	public:
		static CWriter* CreateWriter(LPCTSTR lpLogName);
		
	public:
		void Write(LPCTSTR lpMessage);
		
	public:
		void AddRef() { m_dwCount += 1; }
		void Release() {
			if (m_dwCount > 0) m_dwCount -= 1;
			if (m_dwCount <= 0) delete this;
		}
		
	public:
		virtual ~CWriter();
		
	private:
		CWriter(LPCTSTR lpLogName);
		CWriter(const CWriter &);
		CWriter& operator = (const CWriter &);
		
	private:
		FILE* m_pFile;
		CRITICAL_SECTION m_hWrCrtSec;
		DWORD m_dwCount;
	};
	class CLogger
	{
	public:
		void Debug(LPCTSTR lpFormat, ...);
		void Warn(LPCTSTR lpFormat, ...);
		void Error(LPCTSTR lpFormat, ...);
	public:
		CWriter* GetWriter()  const { return m_pWriter; }
		void     SetWriter(CWriter* pWriter) { m_pWriter = pWriter; }
		BYTE     GetLevel()   const { return m_cbLevel; }
		void     SetLevel(BYTE cbLevel) { m_cbLevel = cbLevel; }
		LPCTSTR  GetLogName() const { return m_szLogName; }
		void     SetLogName(LPCTSTR lpLogName) { lstrcpyn(m_szLogName, lpLogName, MAX_PATH); }
	private:
		CWriter* m_pWriter;
		BYTE     m_cbLevel;
		TCHAR    m_szLogName[MAX_PATH];
	};
	__declspec(selectany) CLogger* ns_pLoggers = NULL;
	__declspec(selectany) DWORD    ns_dwCount = 0;
	__declspec(selectany) void* __ns_pdeleter__ = NULL;
	
	struct __Del__ {
		~__Del__() {
			for (CLogger* pLogger = ns_pLoggers; pLogger < ns_pLoggers + ns_dwCount; ++pLogger)
			{
				pLogger->GetWriter()->Release();
			}
			if (ns_pLoggers) delete []ns_pLoggers;
		}
	};
	
	__inline void CreateDirectory(LPCTSTR lpDirectory);
	__inline CLogger* GetLogger(LPCTSTR lpLogName = _T("default"), BYTE cbLevel = 0);
};

__inline void NSLOG::CreateDirectory(LPCTSTR lpLogName)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	lstrcpyn(szPath, lpLogName, MAX_PATH);
	LPTSTR pch = StrChrI(szPath, _T('\\'));
	while (pch)
	{
		*pch = 0;
		if(!PathFileExists(szPath)) ::CreateDirectory(szPath, NULL);
		*pch = _T('\\');
		pch = StrChrI(pch + 1, _T('\\'));
	}
}

__inline NSLOG::CLogger* NSLOG::GetLogger(LPCTSTR lpLogName/* = _T("default")*/, BYTE cbLevel/* = 0*/)
{
	if (!__ns_pdeleter__)
	{
		static __Del__ deleter;
		__ns_pdeleter__ = (void*)&deleter;
	}
	CWriter* pMatchWriter = NULL;
	CLogger* pMatchLogger = NULL;
	for (CLogger* pLogger = ns_pLoggers; pLogger < ns_pLoggers + ns_dwCount; ++pLogger)
	{
		if (lstrcmpi(pLogger->GetLogName(), lpLogName)) continue;
		if (!pMatchWriter) pMatchWriter = pLogger->GetWriter();
		if (pLogger->GetLevel() != cbLevel) continue;
		if (!pMatchLogger) pMatchLogger = pLogger;
		break;
	}
	if (!pMatchLogger)
	{
		if (!pMatchWriter) pMatchWriter = CWriter::CreateWriter(lpLogName);
		pMatchWriter->AddRef();
		CLogger* pnLoggers = new CLogger[ns_dwCount + 1];
		if (ns_pLoggers)
		{
			CopyMemory(pnLoggers, ns_pLoggers, ns_dwCount * sizeof(*ns_pLoggers));
			delete []ns_pLoggers;
		}
		pnLoggers[ns_dwCount].SetLogName(lpLogName);
		pnLoggers[ns_dwCount].SetLevel(cbLevel);
		pnLoggers[ns_dwCount].SetWriter(pMatchWriter);
		ns_pLoggers = pnLoggers;
		pMatchLogger = ns_pLoggers + ns_dwCount++;
	}
	return pMatchLogger;
}

__inline NSLOG::CWriter* NSLOG::CWriter::CreateWriter(LPCTSTR lpLogName)
{
	_tsetlocale(LC_ALL, _T("chs"));
	TCHAR szLogName[MAX_PATH] = { 0 };
	lstrcpyn(szLogName, lpLogName, MAX_PATH);
	lstrcat(szLogName, _T(".log"));
	return new CWriter(szLogName);
}

__inline NSLOG::CWriter::CWriter(LPCTSTR lpLogName)
: m_dwCount(0)
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
	CreateDirectory(lpLogName);
	m_pFile = _tfsopen(lpLogName, _T("a+"), 0x20);
	for (int nNum(1); !m_pFile;)
	{
		TCHAR szExtName[44] = { 0 };
		_stprintf(szExtName, _T("-%d.log"), nNum++);
		TCHAR szLogName[MAX_PATH] = {0};
		lstrcpyn(szLogName, lpLogName, MAX_PATH);
		PathRenameExtension(szLogName, szExtName);
		m_pFile = _tfsopen(szLogName, _T("a+"), 0x20);
	}
#endif // !defined(_DEBUG)
#endif // defined(LOG_CONSOLE)

	InitializeCriticalSection(&m_hWrCrtSec);
}

__inline NSLOG::CWriter::~CWriter()
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

__inline void NSLOG::CWriter::Write(LPCTSTR lpMessage)
{
	TCHAR szDateTime[256] = { 0 }, szLogMessage[4096] = { 0 };
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	
	_stprintf(szDateTime, _T("%04d%02d%02d %02d:%02d:%02d:%03d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	_stprintf(szLogMessage, _T("[%s][%d][%d]%s\n"), szDateTime, GetCurrentProcessId(), GetCurrentThreadId(), lpMessage);
	
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

__inline void NSLOG::CLogger::Debug(LPCTSTR lpFormat, ...)
{
	if (m_cbLevel > 0) return;
	const TCHAR szPrefix[] = _T("[DEBUG]");
	TCHAR szMessage[3030] = { 0 };
	LPTSTR pszMessage = szMessage;
	lstrcpyn(pszMessage, szPrefix, 3030);
	pszMessage += lstrlen(szPrefix);
	
	va_list args;
	va_start(args, lpFormat);
	_vstprintf(pszMessage, lpFormat, args);
	va_end(args);
	
	m_pWriter->Write(szMessage);
}

__inline void NSLOG::CLogger::Warn(LPCTSTR lpFormat, ...)
{
	if (m_cbLevel > 1) return;
	const TCHAR szPrefix[] = _T("[WARN]");
	TCHAR szMessage[3030] = { 0 };
	LPTSTR pszMessage = szMessage;
	lstrcpyn(pszMessage, szPrefix, 3030);
	pszMessage += lstrlen(szPrefix);
	
	va_list args;
	va_start(args, lpFormat);
	_vstprintf(pszMessage, lpFormat, args);
	va_end(args);
	
	m_pWriter->Write(szMessage);
}

__inline void NSLOG::CLogger::Error(LPCTSTR lpFormat, ...)
{
	if (m_cbLevel > 2) return;
	const TCHAR szPrefix[] = _T("[ERROR]");
	TCHAR szMessage[3030] = { 0 };
	LPTSTR pszMessage = szMessage;
	lstrcpyn(pszMessage, szPrefix, 3030);
	pszMessage += lstrlen(szPrefix);
	
	va_list args;
	va_start(args, lpFormat);
	_vstprintf(pszMessage, lpFormat, args);
	va_end(args);
	
	m_pWriter->Write(szMessage);
}

#endif // !defined(__LOGGER_H__)


