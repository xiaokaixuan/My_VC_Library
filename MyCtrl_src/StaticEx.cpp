// CStaticEx.cpp : implementation file
//

#include "stdafx.h"
#include "StaticEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CStaticEx::CStaticEx()
: m_hbmpUp(0)
, m_hbmpOver(0)
, m_hbmpDown(0)
, m_hicoUp(0)
, m_hicoOver(0)
, m_hicoDown(0)
, m_bTrackLeave(FALSE)
{
}

CStaticEx::~CStaticEx()
{
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


void CStaticEx::SetBitmaps(UINT nIdUp, UINT nIdDown, UINT nIdOver)
{
	m_hbmpOver = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIdOver));
	m_hbmpUp   = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIdUp));
	m_hbmpDown = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIdDown));
	ModifyStyle(0, SS_BITMAP);
	CStatic::SetBitmap(m_hbmpUp);
}
void CStaticEx::SetIcons(UINT nIdUp, UINT nIdDown, UINT nIdOver)
{
	m_hicoOver = AfxGetApp()->LoadIcon(nIdOver);
	m_hicoUp   = AfxGetApp()->LoadIcon(nIdUp);
	m_hicoDown = AfxGetApp()->LoadIcon(nIdDown);
	ModifyStyle(0, SS_ICON);
	CStatic::SetIcon(m_hicoUp);
}

void CStaticEx::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackLeave)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize  = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		_TrackMouseEvent(&tme);
		m_bTrackLeave = TRUE;

		if ((GetStyle() & SS_BITMAP) == SS_BITMAP)
		{
			if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
			{
				CStatic::SetBitmap(m_hbmpDown);
			}
			else
			{
				CStatic::SetBitmap(m_hbmpOver);
			}
		}
		else if ((GetStyle() & SS_ICON) == SS_ICON)
		{
			if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
			{
				CStatic::SetIcon(m_hicoDown);
			}
			else
			{
				CStatic::SetIcon(m_hicoOver);
			}
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CStaticEx::PreSubclassWindow() 
{
	ModifyStyle(0, SS_NOTIFY);
	CStatic::PreSubclassWindow();
}

LRESULT CStaticEx::OnMouseLeave(WPARAM, LPARAM)
{
	m_bTrackLeave = FALSE;
	if ((GetStyle() & SS_BITMAP) == SS_BITMAP)
	{
		CStatic::SetBitmap(m_hbmpUp);
	}
	else if ((GetStyle() & SS_ICON) == SS_ICON)
	{
		CStatic::SetIcon(m_hicoUp);
	}
	return 0;
}

void CStaticEx::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ((GetStyle() & SS_BITMAP) == SS_BITMAP)
	{
		CStatic::SetBitmap(m_hbmpDown);
	}
	else if ((GetStyle() & SS_ICON) == SS_ICON)
	{
		CStatic::SetIcon(m_hicoDown);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CStaticEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ((GetStyle() & SS_BITMAP) == SS_BITMAP)
	{
		CStatic::SetBitmap(m_hbmpOver);
	}
	else if ((GetStyle() & SS_ICON) == SS_ICON)
	{
		CStatic::SetIcon(m_hicoOver);
	}
	CStatic::OnLButtonUp(nFlags, point);
}
