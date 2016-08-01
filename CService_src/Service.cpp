// Service.cpp: implementation of the CService class.
//

#include "StdAfx.h"
#include "Service.h"

CService* CService::sm_pServices = 0;

CService::CService()
{
	lstrcpyn(m_szName, _T("NONAME"), 256);
	m_svcEntries[0].lpServiceName = m_szName;
	m_svcEntries[0].lpServiceProc = &CService::__ServiceMain;
	m_svcEntries[1].lpServiceName = NULL;
	m_svcEntries[1].lpServiceProc = NULL;

	m_svcStatus.dwServiceType        = SERVICE_WIN32;
	m_svcStatus.dwCurrentState       = SERVICE_START_PENDING;
	m_svcStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	m_svcStatus.dwWin32ExitCode      = NO_ERROR;
	m_svcStatus.dwServiceSpecificExitCode = 0;
	m_svcStatus.dwCheckPoint         = 0;
	m_svcStatus.dwWaitHint           = INFINITE;
	m_hStatus = (SERVICE_STATUS_HANDLE)0;
}

CService::~CService()
{
	CloseServiceHandle((SC_HANDLE)m_hStatus);
}

LPCTSTR CService::GetName() const
{
	return m_szName;
}

VOID CService::SetName(LPCTSTR pszName)
{
	lstrcpyn(m_szName, pszName, 256);
}

VOID WINAPI CService::__ServiceMain(DWORD dwNumServicesArgs, LPTSTR *lpServiceArgVectors)
{
	if (sm_pServices)
	{
		CService* pService = sm_pServices;
		pService->m_hStatus = RegisterServiceCtrlHandler(pService->m_szName, &CService::__ControlHandler);
		if (pService->m_hStatus != (SERVICE_STATUS_HANDLE)0)
		{
			pService->m_svcStatus.dwCurrentState = SERVICE_RUNNING;
			SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus);
			pService->OnRunning(dwNumServicesArgs, lpServiceArgVectors);
			pService->m_svcStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus);
		}
	}
}

VOID WINAPI CService::__ControlHandler(DWORD dwControl)
{
	CService* pService = sm_pServices;

	switch(dwControl)
	{
	case SERVICE_CONTROL_PAUSE:
		if(pService->m_svcStatus.dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE)
		{
			pService->m_svcStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
			if (SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus))
			{
				pService->OnPause();
				pService->m_svcStatus.dwCurrentState = SERVICE_PAUSED;
				SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus);
			}
		}
		break;
	case SERVICE_CONTROL_CONTINUE:
		if(pService->m_svcStatus.dwControlsAccepted & SERVICE_ACCEPT_PAUSE_CONTINUE)
		{
			pService->m_svcStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
			if (SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus))
			{
				pService->OnContinue();
				pService->m_svcStatus.dwCurrentState = SERVICE_RUNNING;
				SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus);
			}
		}
		break;
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		if(pService->m_svcStatus.dwControlsAccepted & SERVICE_ACCEPT_STOP)
		{
			pService->m_svcStatus.dwCurrentState  = SERVICE_STOP_PENDING;
			if (SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus))
			{
				if (SERVICE_CONTROL_STOP == dwControl)
				{
					pService->OnStop();
				}
				else pService->OnShutdown();
				pService->m_svcStatus.dwCurrentState  = SERVICE_STOP;
				SetServiceStatus(pService->m_hStatus, &pService->m_svcStatus);
			}
		}
		break;
	default:
		pService->OnCustomCommand(dwControl);
		break;
	}
}

BOOL CService::Execute()
{
	BOOL bReturn(FALSE);
	if (!sm_pServices)
	{
		sm_pServices = this;
		bReturn = StartServiceCtrlDispatcher(m_svcEntries);
	}
	return bReturn;
}

VOID CService::OnStop()
{

}

VOID CService::OnPause()
{

}

VOID CService::OnShutdown()
{

}

VOID CService::OnContinue()
{
	
}

VOID CService::OnCustomCommand(DWORD dwControl)
{

}

VOID CService::OnRunning(DWORD dwNumServicesArgs, LPTSTR *lpServiceArgVectors)
{

}

DWORD CService::GetWaitHInt() const
{
	return m_svcStatus.dwWaitHint;
}

VOID CService::SetWaitHint(DWORD dwWaitHint)
{
	m_svcStatus.dwWaitHint = dwWaitHint;
}

DWORD CService::GetControlAccepted() const
{
	return m_svcStatus.dwControlsAccepted;
}

VOID CService::SetControlAccepted(DWORD dwControlAccepted)
{
	m_svcStatus.dwControlsAccepted = dwControlAccepted;
}

