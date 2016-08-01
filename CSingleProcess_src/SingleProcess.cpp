#include "StdAfx.h"
#include "SingleProcess.h"

CSingleProcess::CSingleProcess(LPCTSTR lpFlag)
: m_phMainWnd(0)
, m_hMap(0)
, m_bIsExist(FALSE)
{
	if (lpFlag) __Init(lpFlag);
}

CSingleProcess::~CSingleProcess()
{
	__Term();
}

void CSingleProcess::__Init(LPCTSTR lpFlag)
{
	m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HWND), lpFlag);
	m_bIsExist = ::GetLastError() == ERROR_ALREADY_EXISTS;
	if (m_hMap)
	{
		m_phMainWnd = (HWND*)::MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
	}
}

void CSingleProcess::__Term()
{
	if (m_phMainWnd)
	{
		::UnmapViewOfFile(m_phMainWnd);
		m_phMainWnd = 0;
	}
	if (m_hMap)
	{
		::CloseHandle(m_hMap);
		m_hMap = 0;
	}
}

BOOL CSingleProcess::IsExist() const
{
	return m_bIsExist;
}

HWND CSingleProcess::GetMainWnd() const
{
	return m_phMainWnd ? *m_phMainWnd : 0;
}

void CSingleProcess::SetMainWnd(HWND hWnd)
{
	if (m_phMainWnd)
	{
		*m_phMainWnd = hWnd;
	}
}

BOOL CSingleProcess::ActivateWindow(int nCmdShow) const
{
	BOOL bRet(FALSE);
	if (IsExist())
	{
		HWND hWnd = GetMainWnd();
		if (hWnd)
		{
			::ShowWindow(hWnd, nCmdShow);
			::SetForegroundWindow(hWnd);
			bRet = TRUE;
		}
	}
	return bRet;
}

void CSingleProcess::DeleteObject()
{
	__Term();
}

