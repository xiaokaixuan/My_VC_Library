// Environment.cpp: implementation of the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Environment.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CEnvironment::CEnvironment(LPCTSTR lpOwner /*= _T("SYSTEM")*/)
: m_hOwner(NULL)
{
	ASSERT(lstrcmpi(lpOwner, _T("SYSTEM")) == 0 || 0 == lstrcmpi(lpOwner, _T("CURRENTUSER")));
	if (lstrcmpi(lpOwner, _T("SYSTEM")) == 0)
	{
		RegOpenKeyEx(HKEY_LOCAL_MACHINE
			, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")
			, 0, KEY_ALL_ACCESS, &m_hOwner);
	}
	else if (0 == lstrcmpi(lpOwner, _T("CURRENTUSER")))
	{
		RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_ALL_ACCESS, &m_hOwner);
	}
	ASSERT(m_hOwner != NULL);
}

CEnvironment::~CEnvironment()
{
	if (NULL != m_hOwner)
	{
		RegCloseKey(m_hOwner);
		m_hOwner = NULL;
	}
}

// Public methods.

LONG CEnvironment::DeleteValue(LPCTSTR lpName)
{
	ASSERT(m_hOwner != NULL);
	LONG lRet = RegDeleteValue(m_hOwner, lpName);
	return lRet;
}

LONG CEnvironment::SetValue(LPCTSTR lpName, LPCTSTR lpValue)
{
	ASSERT(m_hOwner != NULL);
	LONG lRet = RegSetValueEx(m_hOwner, lpName, 0, REG_EXPAND_SZ
		, (CONST BYTE *)lpValue, (lstrlen(lpValue)+1)*sizeof(TCHAR));
	return lRet;
}

LONG CEnvironment::GetValue(LPCTSTR lpName, LPTSTR lpValue, DWORD *lpdwCount)
{
	ASSERT(m_hOwner != NULL);
	DWORD dwType(0);
	LONG lRet = RegQueryValueEx(m_hOwner, lpName, NULL, &dwType, LPBYTE(lpValue), lpdwCount);
	ASSERT(dwType == REG_EXPAND_SZ || dwType == REG_SZ);
	ASSERT(lRet == ERROR_SUCCESS);
	return lRet;
}

void CEnvironment::ForceUpdate(UINT uTimeout)
{
	::SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, LPARAM(_T("Environment")), SMTO_BLOCK, uTimeout, NULL);
}

// Public static methods.

LONG CEnvironment::DeleteValue(LPCTSTR lpOwner, LPCTSTR lpName)
{
	CEnvironment env(lpOwner);
	LONG lRet = env.DeleteValue(lpName);
	env.ForceUpdate(5000);
	return lRet;
}

LONG CEnvironment::SetValue(LPCTSTR lpOwner, LPCTSTR lpName, LPCTSTR lpValue)
{
	CEnvironment env(lpOwner);
	LONG lRet = env.SetValue(lpName, lpValue);
	env.ForceUpdate(5000);
	return lRet;
}

LONG CEnvironment::GetValue(LPCTSTR lpOwner, LPCTSTR lpName, LPTSTR lpValue, DWORD *lpdwCount)
{
	CEnvironment env(lpOwner);
	return env.GetValue(lpName, lpValue, lpdwCount);
}

