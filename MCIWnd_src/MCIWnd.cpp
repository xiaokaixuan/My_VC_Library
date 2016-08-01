// MCIWnd.cpp: implementation of the CMCIWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MCIWnd.h"
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMCIWnd::CMCIWnd()
: m_hWnd(NULL)
{

}

CMCIWnd::~CMCIWnd()
{
	if (NULL != m_hWnd)
	{
		MCIWndClose(m_hWnd);
		MCIWndDestroy(m_hWnd);
		m_hWnd = NULL;
	}
}

BOOL CMCIWnd::Attach(HWND hwnd)
{
	ASSERT(NULL == m_hWnd);
	m_hWnd = hwnd;
	return NULL != m_hWnd;
}

HWND CMCIWnd::Detach()
{
	HWND hRet = m_hWnd;
	m_hWnd = NULL;
	return hRet;
}

CMCIWnd::operator HWND() const
{
	return m_hWnd;
}

HWND CMCIWnd::GetSafeHwnd() const
{
	return m_hWnd;
}

HWND CMCIWnd::Create(HWND hwndParent, HINSTANCE hInstance, DWORD dwStyle, LPCTSTR szFile)
{
	ASSERT(m_hWnd == NULL);
	return m_hWnd = MCIWndCreate(hwndParent, hInstance, dwStyle, szFile);
}

LONG CMCIWnd::New(LPCSTR pszDevice)
{
	ASSERT(NULL != m_hWnd);
	MCIWndClose(m_hWnd);
	return MCIWndNew(m_hWnd, pszDevice);
}

LONG CMCIWnd::Seek(LONG iPos)
{
	ASSERT(m_hWnd != NULL);
	return MCIWndSeek(m_hWnd, iPos);
}

LONG CMCIWnd::GetLength()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndGetLength(m_hWnd);
}

BOOL CMCIWnd::CanPlay()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndCanPlay(m_hWnd);
}

BOOL CMCIWnd::CanRecord()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndCanRecord(m_hWnd);
}

BOOL CMCIWnd::CanSave()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndCanSave(m_hWnd);
}

BOOL CMCIWnd::CanConfig()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndCanConfig(m_hWnd);
}

LONG CMCIWnd::Close()
{
	return MCIWndClose(m_hWnd);
}

void CMCIWnd::Destroy()
{
	MCIWndDestroy(m_hWnd);
}

LONG CMCIWnd::Open(LPCTSTR szFile, DWORD dwFlags)
{
	ASSERT(NULL != m_hWnd);
	MCIWndClose(m_hWnd);
	return MCIWndOpen(m_hWnd, szFile, dwFlags);
}

LONG CMCIWnd::Record()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndRecord(m_hWnd);
}

LONG CMCIWnd::Play()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndPlay(m_hWnd);
}

LONG CMCIWnd::Pause()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndPause(m_hWnd);
}

LONG CMCIWnd::Stop()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndStop(m_hWnd);
}

LONG CMCIWnd::Save(LPCTSTR szFile)
{
	ASSERT(NULL != m_hWnd);
	return MCIWndSave(m_hWnd, szFile);
}

LONG CMCIWnd::GetPos()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndGetPosition(m_hWnd);
}

LONG CMCIWnd::GetVolume()
{
	ASSERT(NULL != m_hWnd);
	return MCIWndGetVolume(m_hWnd);
}

LONG CMCIWnd::SetVolume(UINT iVol)
{
	ASSERT(NULL != m_hWnd);
	return MCIWndSetVolume(m_hWnd, iVol);
}
