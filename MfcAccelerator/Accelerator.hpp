#ifndef _ACCELERATOR_H_
#define _ACCELERATOR_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

///////////////////////////////////////////////////////////////////////////////
// CAccelerator

class CAccelerator
{
	HACCEL m_hAccel;
public:
	CAccelerator():m_hAccel(NULL)
	{
	}
	
	CAccelerator(LPCTSTR lpAccel)
	{
		m_hAccel = ::LoadAccelerators(
			AfxGetResourceHandle(), lpAccel);
	}

	CAccelerator(DWORD dwAccel)
	{
		m_hAccel = ::LoadAccelerators(
			AfxGetResourceHandle(), MAKEINTRESOURCE(dwAccel));
	}
	
	~CAccelerator()
	{
		if(m_hAccel != NULL)
		{
			::DestroyAcceleratorTable(m_hAccel);
			m_hAccel = NULL;
		}
	}

public:
	CAccelerator& operator =(HACCEL hAccel)
	{
		Attach(hAccel);
		return *this;
	}
	
	void Attach(HACCEL hAccel)
	{
		if(m_hAccel != NULL)
		{
			::DestroyAcceleratorTable(m_hAccel);
		}
		m_hAccel = hAccel;
	}
	
	HACCEL Detach()
	{
		HACCEL hAccel = m_hAccel;
		m_hAccel = NULL;
		return hAccel;
	}
	
	operator HACCEL() const
	{
		return m_hAccel;
	}
	
	BOOL IsNull() const
	{
		return m_hAccel == NULL;
	}

public:
	HACCEL LoadAccelerators(LPCTSTR lpAccel)
	{
		ASSERT(m_hAccel == NULL);
		m_hAccel = ::LoadAccelerators(
			AfxGetResourceHandle(), lpAccel);
		return m_hAccel;
	}

	HACCEL LoadAccelerators(DWORD dwAccel)
	{
		ASSERT(m_hAccel == NULL);
		m_hAccel = ::LoadAccelerators(
			AfxGetResourceHandle(), MAKEINTRESOURCE(dwAccel));
		return m_hAccel;
	}

	HACCEL CreateAcceleratorTable(LPACCEL pAccel, int cEntries)
	{
		ASSERT(m_hAccel == NULL);
		ASSERT(pAccel != NULL);
		m_hAccel = ::CreateAcceleratorTable(pAccel, cEntries);
		return m_hAccel;
	}

	void DestroyObject()
	{
		if(m_hAccel != NULL)
		{
			::DestroyAcceleratorTable(m_hAccel);
			m_hAccel = NULL;
		}
	}

	int CopyAcceleratorTable(LPACCEL lpAccelDst, int cEntries)
	{
		ASSERT(m_hAccel != NULL);
		ASSERT(lpAccelDst != NULL);
		return ::CopyAcceleratorTable(m_hAccel
			, lpAccelDst, cEntries);
	}

	int GetEntriesCount() const
	{
		ASSERT(m_hAccel != NULL);
		return ::CopyAcceleratorTable(m_hAccel, NULL, 0);
	}

	BOOL TranslateAccelerator(HWND hWnd, LPMSG pMsg)
	{
		ASSERT(m_hAccel != NULL);
		ASSERT(::IsWindow(hWnd));
		ASSERT(pMsg != NULL);
		return ::TranslateAccelerator(hWnd, m_hAccel, pMsg);
	}
};

///////////////////////////////////////////////////////////////////////////////

#endif //_ACCELERATOR_H_

