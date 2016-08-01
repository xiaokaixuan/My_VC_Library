#if !defined(AFX_CMDOUTPUT_H__4FB57DC3_29A3_11D5_BB60_006097553C52__INCLUDED_)
#define AFX_CMDOUTPUT_H__4FB57DC3_29A3_11D5_BB60_006097553C52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef VOID (CALLBACK *LPCALLBACK)(LPVOID lpParam, INT nType, LPCSTR pszOutput);

class CCmdOutput
{
public:
    CCmdOutput();
    virtual ~CCmdOutput();
	
private:
    HANDLE m_hThread;
    HANDLE m_hEvtStop;
    DWORD m_dwThreadId;
    DWORD m_dwWaitTime;
	
protected:
    HANDLE m_hStdinWrite;
    HANDLE m_hStdoutRead;
    HANDLE m_hChildProcess;
    LPCALLBACK m_pcbFunction;
    LPVOID m_pcbParameter;
	
    BOOL LaunchChild(LPCTSTR pszCmdLine, HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr);
    INT RedirectStdout();
    VOID DestroyHandle(HANDLE& rhObject);
	
    static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);
	
protected:
    virtual VOID WriteStdOut(LPCSTR pszOutput);
    virtual VOID WriteStdError(LPCSTR pszError);
	
public:
    BOOL Open(LPCTSTR pszCmdLine);
    VOID Close();
	VOID SetCallBack(LPCALLBACK pcbFunction, LPVOID pcbParameter);
    BOOL Printf(LPCTSTR pszFormat, ...);
	
    VOID SetWaitTime(DWORD dwWaitTime) { m_dwWaitTime = dwWaitTime; }
};

#endif // !defined(AFX_CMDOUTPUT_H__4FB57DC3_29A3_11D5_BB60_006097553C52__INCLUDED_)

