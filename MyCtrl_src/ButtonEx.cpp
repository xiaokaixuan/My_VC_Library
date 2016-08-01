// ButtonEx.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CButtonEx::CButtonEx()
: m_nIdUp(0)
, m_nIdOver(0)
, m_bTrackLeave(FALSE)
{
}

CButtonEx::~CButtonEx()
{
}

BEGIN_MESSAGE_MAP(CButtonEx, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

void CButtonEx::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW | BS_BITMAP);
	CBitmapButton::PreSubclassWindow();
}

void CButtonEx::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTrackLeave)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize  = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		_TrackMouseEvent(&tme);
		m_bTrackLeave = TRUE;
		m_bitmap.DeleteObject();
		m_bitmap.LoadBitmap(m_nIdOver);
		Invalidate();
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}

LRESULT CButtonEx::OnMouseLeave(WPARAM, LPARAM)
{
	m_bTrackLeave = FALSE;
	m_bitmap.DeleteObject();
	m_bitmap.LoadBitmap(m_nIdUp);
	Invalidate();
	return 0;
}

BOOL CButtonEx::SetBitmaps(UINT nIdUp, UINT nIdDown, UINT nIdOver, UINT nIdDisable)
{
	m_nIdOver = nIdOver;
	m_nIdUp   = nIdUp;
	return CBitmapButton::LoadBitmaps(nIdUp, nIdDown, 0, nIdDisable);
}
