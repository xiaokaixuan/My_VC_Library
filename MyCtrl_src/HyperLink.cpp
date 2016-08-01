// HyperLink.cpp : implementation file
//

#include "stdafx.h"
#include "HyperLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHyperLink::CHyperLink()
: m_bAllow(TRUE)
, m_bClicked(FALSE)
{
}

CHyperLink::~CHyperLink()
{
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CFont* pFont = GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
 	if (!lf.lfUnderline)
 	{
 		lf.lfUnderline = TRUE;
 		HFONT hFont = CreateFontIndirect(&lf);
 		SelectObject(pDC->GetSafeHdc(), hFont);
 	}
	if (!m_bClicked)
	{
		pDC->SetTextColor(RGB(0,0,255));
	}
	else
	{
		pDC->SetTextColor(RGB(140,50,140));
	}
	pDC->SetBkMode(TRANSPARENT);
	HBRUSH hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
	return hbr;
}

BOOL CHyperLink::Create(LPCTSTR lpszText, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

int CHyperLink::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CHyperLink::PreSubclassWindow()
{
	if (::IsWindow(GetSafeHwnd()))
	{
		ModifyStyle(0, SS_NOTIFY);

		GetWindowText(m_strAddress);
		SetTipText(m_strTip);
	}
	CStatic::PreSubclassWindow();
}

BOOL CHyperLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
	return TRUE;
}

void CHyperLink::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bAllow)
	{
		m_bClicked = TRUE;
		Invalidate();
		ShellExecute(GetSafeHwnd(), _T("Open"), m_strAddress, NULL, NULL, SW_SHOW);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

BOOL CHyperLink::SetLinkAddress(LPCTSTR lpAddress)
{
	BOOL bRet(FALSE);
	if (m_strAddress.CompareNoCase(lpAddress) != 0)
	{
		m_strAddress = lpAddress;
		bRet = TRUE;
	}
	return bRet;
}

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
	if (::IsWindow(m_tipCtrl.GetSafeHwnd()))
	{
		m_tipCtrl.RelayEvent(pMsg);
	}
	return CStatic::PreTranslateMessage(pMsg);
}

BOOL CHyperLink::SetTipText(LPCTSTR lpTip)
{
	BOOL bRet(FALSE);
	if (m_strTip.CompareNoCase(lpTip) != 0)
	{
		m_strTip = lpTip;
		if (::IsWindow(m_tipCtrl.GetSafeHwnd()))
		{
			m_tipCtrl.UpdateTipText(m_strTip, this);
		}
		else
		{
			m_tipCtrl.Create(this, TTS_ALWAYSTIP);
			m_tipCtrl.AddTool(this, m_strTip);
			m_tipCtrl.Activate(TRUE);
		}
		bRet = TRUE;
	}
	return bRet;
}

BOOL CHyperLink::EnableNavigate(BOOL lpAllow)
{
	BOOL bPre = m_bAllow;
	m_bAllow = lpAllow;
	return bPre;
}
