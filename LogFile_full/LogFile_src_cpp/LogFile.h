#if !defined(_LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)
#define _LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLogFile
{
public:
	static void SetFileName(LPCTSTR pszFileName);
	static void Log(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpFormat, ...);

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
		void operator()(LPCTSTR lpFormat, ...);
	};
	friend class FOTracer;

private:
	static CLogFile* __InitLogger();

private:
	void __CreateFile();
	void __CheckFile(const SYSTEMTIME& systime);
	void __WriteMessage(LPCSTR lpFile, DWORD dwLine, LPCTSTR lpMessage);

private:
	CLogFile();
	CLogFile(const CLogFile&);
	CLogFile& operator = (const CLogFile&);

private:
	FILE*			 m_pFile;
	SYSTEMTIME		 m_stLocal;
	CRITICAL_SECTION m_hWrCrtSec;

private:
	struct tagInitor
	{
	private:
		CRITICAL_SECTION& m_hCrtSec;
		TCHAR m_szFileName[MAX_PATH];
	public:
		tagInitor(CRITICAL_SECTION& hCrtSec);
		~tagInitor();

	public:
		void SetFileName(LPCTSTR pszFileName);
		LPCTSTR GetFileName() const;
	};
	static CLogFile*		sm_pMe;
	static CRITICAL_SECTION sm_hCrtGen;
	static tagInitor		sm_Initor;
};

// Usage: LOG(_T("%s %d"), szMsg, nNumber);

#define LOG					CLogFile::FOTracer(__FILE__, __LINE__)

#endif // !defined(_LOGFILE_H__261C9DDC_AB17_4781_B87F_4B82DD38DD13__INCLUDED)

