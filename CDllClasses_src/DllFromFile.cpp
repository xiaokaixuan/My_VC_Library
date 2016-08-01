// DllFromFile.cpp: implementation of the CDllFromFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DllFromFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDllFromFile::CDllFromFile()
: m_hDll(NULL)
{
}

CDllFromFile::CDllFromFile(LPCTSTR lpFileName)
{
	m_hDll = ::LoadLibrary(lpFileName);
}

CDllFromFile::~CDllFromFile()
{
	if (NULL != m_hDll)
	{
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

BOOL CDllFromFile::LoadLibrary(LPCTSTR lpFileName)
{
	BOOL bRet(FALSE);

	HMODULE hDll = m_hDll;
	m_hDll = ::LoadLibrary(lpFileName);
	if (NULL != m_hDll)
	{
		::FreeLibrary(hDll);
		bRet = TRUE;
	}
	else
	{
		m_hDll = hDll;
	}
	return bRet;
}

BOOL CDllFromFile::FreeLibrary()
{
	BOOL bRet(FALSE);
	if (NULL != m_hDll)
	{
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
		bRet = TRUE;
	}
	return bRet;
}

FARPROC CDllFromFile::GetProcAddress(LPCSTR lpProcName)
{
	FARPROC rtProc = NULL;
	if (NULL != m_hDll)
	{
		rtProc = ::GetProcAddress(m_hDll, lpProcName);
	}
	return rtProc;
}

HMODULE CDllFromFile::GetModuleHandle()
{
	return m_hDll;
}
