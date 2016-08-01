// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__94E9F890_D647_4E0D_80B0_E6F8D35175FC__INCLUDED_)
#define AFX_ENVIRONMENT_H__94E9F890_D647_4E0D_80B0_E6F8D35175FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEnvironment 
{
public:
	// lpOwner is "SYSTEM" or "CURRENTUSER".
	CEnvironment(LPCTSTR lpOwner = _T("SYSTEM"));
	virtual ~CEnvironment();

	// Update environment 
public:
	void ForceUpdate(UINT uTimeout);

	// Public methods.
public:
	LONG DeleteValue(LPCTSTR lpName);
	LONG SetValue(LPCTSTR lpName, LPCTSTR lpValue);
	LONG GetValue(LPCTSTR lpName, LPTSTR lpValue, DWORD *lpdwCount);

	// Public static methods. 
public:
	static LONG DeleteValue(LPCTSTR lpOwner, LPCTSTR lpName);
	static LONG SetValue(LPCTSTR lpOwner, LPCTSTR lpName, LPCTSTR lpValue);
	static LONG GetValue(LPCTSTR lpOwner, LPCTSTR lpName, LPTSTR lpValue, DWORD *lpdwCount);

protected:
	HKEY    m_hOwner;
};

#endif // !defined(AFX_ENVIRONMENT_H__94E9F890_D647_4E0D_80B0_E6F8D35175FC__INCLUDED_)
