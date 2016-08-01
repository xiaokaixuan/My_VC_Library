// Service.h: interface for the CService class.
//

#if !defined(AFX_SERVICE_H__687086F8_4869_4086_A322_752E2416B2B3__INCLUDED_)
#define AFX_SERVICE_H__687086F8_4869_4086_A322_752E2416B2B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsvc.h>

class CService  
{
public:
	CService();
	virtual ~CService();

public:
	LPCTSTR GetName() const;
	VOID	SetName(LPCTSTR pszName);
	
	DWORD   GetWaitHInt() const;
	VOID    SetWaitHint(DWORD dwWaitHint);

	DWORD   GetControlAccepted() const;
	VOID    SetControlAccepted(DWORD dwControlAccepted);

public:
	BOOL Execute();

public:
	virtual VOID OnStop();
	virtual VOID OnPause();
	virtual VOID OnShutdown();
	virtual VOID OnContinue();
	virtual VOID OnCustomCommand(DWORD dwControl);
	virtual VOID OnRunning(DWORD dwNumServicesArgs, LPTSTR *lpServiceArgVectors);
	
private:
	static VOID WINAPI __ControlHandler(DWORD dwControl);
	static VOID WINAPI __ServiceMain(DWORD dwNumServicesArgs, LPTSTR *lpServiceArgVectors);

protected:
	TCHAR				  m_szName[256];
	SERVICE_STATUS		  m_svcStatus;
	SERVICE_TABLE_ENTRY	  m_svcEntries[2];
	SERVICE_STATUS_HANDLE m_hStatus;

protected:
	static CService* sm_pServices;
};

#endif // !defined(AFX_SERVICE_H__687086F8_4869_4086_A322_752E2416B2B3__INCLUDED_)

