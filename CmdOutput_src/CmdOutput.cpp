#include "StdAfx.h"
#include "CmdOutput.h"


CCmdOutput::CCmdOutput()
: m_hStdinWrite(NULL)
, m_hStdoutRead(NULL)
, m_hChildProcess(NULL)
, m_hThread(NULL)
, m_hEvtStop(NULL)
, m_dwThreadId(0)
, m_dwWaitTime(200)
, m_pcbFunction(NULL)
, m_pcbParameter(NULL)
{
}

CCmdOutput::~CCmdOutput()
{
    Close();
}

BOOL CCmdOutput::Open(LPCTSTR pszCmdLine)
{
    HANDLE hStdoutReadTmp(NULL);
    HANDLE hStdoutWrite(NULL), hStderrWrite(NULL);
    HANDLE hStdinWriteTmp(NULL);
    HANDLE hStdinRead(NULL);
    SECURITY_ATTRIBUTES sa = { 0 };
	
    Close();
	
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;
	
    BOOL bOK = FALSE;
    do
    {
        if (!::CreatePipe(&hStdoutReadTmp, &hStdoutWrite, &sa, 0)) break;
        if (!::DuplicateHandle(
            ::GetCurrentProcess(),
            hStdoutWrite,
            ::GetCurrentProcess(),
            &hStderrWrite,
            0, TRUE,
            DUPLICATE_SAME_ACCESS)) break;
		
        if (!::CreatePipe(&hStdinRead, &hStdinWriteTmp, &sa, 0)) break;
        if (!::DuplicateHandle(
            ::GetCurrentProcess(),
            hStdoutReadTmp,
            ::GetCurrentProcess(),
            &m_hStdoutRead,
            0, FALSE,
            DUPLICATE_SAME_ACCESS)) break;
		
        if (!::DuplicateHandle(
            ::GetCurrentProcess(),
            hStdinWriteTmp,
            ::GetCurrentProcess(),
            &m_hStdinWrite,
            0, FALSE,
            DUPLICATE_SAME_ACCESS)) break;
		
        DestroyHandle(hStdoutReadTmp);
        DestroyHandle(hStdinWriteTmp);
		
        if (!LaunchChild(pszCmdLine, hStdoutWrite, hStdinRead, hStderrWrite)) break;
        DestroyHandle(hStdoutWrite);
        DestroyHandle(hStdinRead);
        DestroyHandle(hStderrWrite);
		
        m_hEvtStop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        m_hThread = ::CreateThread(
            NULL, 0,
            OutputThread,
            this,
            0,
            &m_dwThreadId);
        if (!m_hThread) break;
		
        bOK = TRUE;
    }
    while (0);
    if (!bOK)
    {
        DWORD dwOsErr = ::GetLastError();
        char szMsg[40] = { 0 };
        ::sprintf(szMsg, "Redirect console error: %x\r\n", dwOsErr);
        WriteStdError(szMsg);
        DestroyHandle(hStdoutReadTmp);
        DestroyHandle(hStdoutWrite);
        DestroyHandle(hStderrWrite);
        DestroyHandle(hStdinWriteTmp);
        DestroyHandle(hStdinRead);
        Close();
        ::SetLastError(dwOsErr);
    }
    return bOK;
}

VOID CCmdOutput::Close()
{
    do
    {
		if (!m_hThread) break;
		if (::GetCurrentThreadId() == m_dwThreadId) break;
		::SetEvent(m_hEvtStop);
		if (::WaitForSingleObject(m_hThread, 5000) != WAIT_TIMEOUT) break;
		WriteStdError(_T("The redir thread is dead\r\n"));
		::TerminateThread(m_hThread, -2);
    }
    while (0);
	if (m_hChildProcess)
    {
		::TerminateProcess(m_hChildProcess, -2);
		DestroyHandle(m_hChildProcess);
	}
    if (m_dwThreadId) m_dwThreadId = 0;
    if (m_hThread) DestroyHandle(m_hThread);
    if (m_hEvtStop) DestroyHandle(m_hEvtStop);
    if (m_hStdinWrite) DestroyHandle(m_hStdinWrite);
    if (m_hStdoutRead) DestroyHandle(m_hStdoutRead);
}

VOID CCmdOutput::SetCallBack(LPCALLBACK pcbFunction, LPVOID pcbParameter)
{
	m_pcbFunction = pcbFunction;
	m_pcbParameter = pcbParameter;
}

BOOL CCmdOutput::Printf(LPCTSTR pszFormat, ...)
{
    if (!m_hStdinWrite) return FALSE;
	
    CString strInput;
    va_list argList;
	
    va_start(argList, pszFormat);
    strInput.FormatV(pszFormat, argList);
    va_end(argList);
	
    DWORD dwWritten = 0;
    return ::WriteFile(m_hStdinWrite, (LPCTSTR)strInput, strInput.GetLength(), &dwWritten, NULL);
}

BOOL CCmdOutput::LaunchChild(LPCTSTR pszCmdLine, HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr)
{
    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFO si = { 0 };
    
    si.cb = sizeof(STARTUPINFO);
    si.hStdOutput = hStdOut;
    si.hStdInput = hStdIn;
    si.hStdError = hStdErr;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	
    if (!::CreateProcess(
        NULL,
        (LPTSTR)pszCmdLine,
        NULL, NULL,
        TRUE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si,
        &pi)) return FALSE;
	
    m_hChildProcess = pi.hProcess;
    ::CloseHandle(pi.hThread);
    return TRUE;
}

// redirect the child process's stdout:
// return: 1: no more data, 0: child terminated, -1: os error
INT CCmdOutput::RedirectStdout()
{
    for (;;)
    {
        DWORD dwAvail = 0;
        if (!::PeekNamedPipe(m_hStdoutRead, NULL, 0, NULL,
            &dwAvail, NULL))            // error
            break;
		
        if (!dwAvail)                   // not data available
            return 1;
		
        char szOutput[256] = { 0 };
        DWORD dwRead = 0;
        if (!::ReadFile(m_hStdoutRead, szOutput, min(255, dwAvail),
            &dwRead, NULL) || !dwRead)  // error, the child might ended
            break;
		
        szOutput[dwRead] = 0;
        WriteStdOut(szOutput);
    }
	
    DWORD dwError = ::GetLastError();
    if (dwError == ERROR_BROKEN_PIPE || // pipe has been ended
        dwError == ERROR_NO_DATA)       // pipe closing in progress
    {
#ifdef _TEST_REDIR
        WriteStdOut("\r\n<TEST INFO>: Child process ended\r\n");
#endif
        return 0;   // child process ended
    }
	
    WriteStdError("Read stdout pipe error\r\n");
    return -1;      // os error
}

VOID CCmdOutput::DestroyHandle(HANDLE& rhObject)
{
    if (rhObject)
    {
        ::CloseHandle(rhObject);
        rhObject = NULL;
    }
}

VOID CCmdOutput::WriteStdOut(LPCSTR pszOutput)
{
    TRACE("%s", pszOutput);
    if (m_pcbFunction) m_pcbFunction(m_pcbParameter, 1, pszOutput);
}

VOID CCmdOutput::WriteStdError(LPCSTR pszError)
{
    TRACE("%s", pszError);
    if (m_pcbFunction) m_pcbFunction(m_pcbParameter, 2, pszError);
}

// thread to receive output of the child process
DWORD WINAPI CCmdOutput::OutputThread(LPVOID lpvThreadParam)
{
    HANDLE aHandles[2];
    INT nRet = 0;
    CCmdOutput* pRedir = (CCmdOutput*) lpvThreadParam;
	
    ASSERT(pRedir != NULL);
    aHandles[0] = pRedir->m_hChildProcess;
    aHandles[1] = pRedir->m_hEvtStop;
	
    for (;;)
    {
        // redirect stdout till there's no more data.
        nRet = pRedir->RedirectStdout();
        if (nRet <= 0) break;
		
        // check if the child process has terminated.
        DWORD dwRc = ::WaitForMultipleObjects(2, aHandles, FALSE, pRedir->m_dwWaitTime);
        if (WAIT_OBJECT_0 == dwRc)      // the child process ended
        {
            nRet = pRedir->RedirectStdout();
            if (nRet > 0) nRet = 0;
            break;
        }
        if (WAIT_OBJECT_0 + 1 == dwRc)    // m_hEvtStop was signalled
        {
            nRet = 1;   // cancelled
            break;
        }
    }
    pRedir->Close();
    return nRet;
}

