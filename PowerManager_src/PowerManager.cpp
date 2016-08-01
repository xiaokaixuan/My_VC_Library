// PowerManager.cpp: implementation of the CPowerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PowerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPowerManager::CPowerManager()
: m_hDll(NULL)
, m_pAdjustPrivilege(NULL)
, m_pShutdownSystem(NULL)
{
	m_hDll = ::LoadLibrary(_T("NTDLL.dll"));
	ASSERT(m_hDll != NULL);
	if (NULL != m_hDll)
	{
		m_pAdjustPrivilege = (fnAdjustPrivilege)GetProcAddress(m_hDll, "RtlAdjustPrivilege");
		m_pShutdownSystem = (fnShutdownSystem)GetProcAddress(m_hDll, "NtShutdownSystem");
	}
	ASSERT(NULL != m_pAdjustPrivilege);
	if (NULL != m_pAdjustPrivilege)
	{
		m_pAdjustPrivilege(SE_SHUTDOWN_PRIVILEGE, TRUE, FALSE, &m_bOldPrivilege);
	}
}

CPowerManager::~CPowerManager()
{
	if (NULL != m_hDll)
	{
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

BOOL CPowerManager::Shutdown(BOOL bQuick/* = FALSE*/)
{
	BOOL bResult(FALSE);
	if (bQuick && m_pShutdownSystem)
	{
		if (m_pShutdownSystem(ShutdownPowerOff) >= 0)
		{
			bResult = TRUE;
		}
	}
	else
	{
		if (ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0))
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CPowerManager::Reboot(BOOL bQuick/* = FALSE*/)
{
	BOOL bResult(FALSE);
	if (bQuick && m_pShutdownSystem)
	{
		if (m_pShutdownSystem(ShutdownReboot) >= 0)
		{
			bResult = TRUE;
		}
	}
	else
	{
		if (ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
		{
			bResult = TRUE;
		}
	}
	return TRUE;
}

BOOL CPowerManager::LogOff()
{
	return ExitWindowsEx(EWX_LOGOFF | EWX_FORCE, 0);
}
