#if !defined(_LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)
#define _LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

enum
{
	LT_NULL    = 0x0000,
	LT_FILE	   = 0x0001,
	LT_CONSOLE = 0x0002
};

class CLogFile
{
public:
	static void SetOption(WORD wType);
	static void SetOption(WORD wType, LPCSTR  pszFileName);
	static void SetOption(WORD wType, LPCWSTR pszFileName);
	static void Log(LPCSTR lpFile, DWORD dwLine, LPCSTR  lpFormat, ...);
	static void Log(LPCSTR lpFile, DWORD dwLine, LPCWSTR lpFormat, ...);

public:
	virtual ~CLogFile();

public:
	class FOTracer
	{
	private:
		LPCSTR m_lpFile;
		DWORD  m_dwLine;
	public:
		FOTracer(LPCSTR lpFile, DWORD dwLine): m_lpFile(lpFile), m_dwLine(dwLine) { }
		void operator()(LPCSTR  lpFormat, ...);
		void operator()(LPCWSTR lpFormat, ...);
	};
	friend class FOTracer;

private:
	static CLogFile* __InitLogger();

private:
	void __CreateFile();
	void __CheckFile(const SYSTEMTIME& sysTime);
	void __OutputMessage(LPCSTR lpFile, DWORD dwLine, LPCSTR  lpMessage);
	void __OutputMessage(LPCSTR lpFile, DWORD dwLine, LPCWSTR lpMessage);

private:
	CLogFile();
	CLogFile(const CLogFile&);
	CLogFile& operator=(const CLogFile&);

private:
	FILE*					m_pFile;
	SYSTEMTIME				m_stLocal;
	CRITICAL_SECTION		m_hWrCrtSec;

private:
	struct CInitor
	{
	public:
		CRITICAL_SECTION&	m_hCrtObject;
		WCHAR				m_szFileName[MAX_PATH];
	public:
		CInitor(CRITICAL_SECTION& hCrtSec);
		virtual ~CInitor();
	};
	static WORD				sm_wType;
	static CLogFile*		sm_pMe;
	static CRITICAL_SECTION	sm_hCrtObject;
	static CInitor			sm_Initor;
};

// Usage: LOG(_T("%d %f...Format string"), 5, 5.5f);

#define LOG				CLogFile::FOTracer(__FILE__, __LINE__)

#endif // !defined(_LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)

