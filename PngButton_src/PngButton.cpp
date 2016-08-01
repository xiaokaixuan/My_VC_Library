#include "StdAfx.h"
#include <atlimage.h>
#include "PngButton.h"


CPngButton::CPngButton()
: m_emStatus(BTN_STATUS_NORMAL)
, m_bIsAutoSize(FALSE)
, m_bInButton(FALSE)
, m_bNeedFlush(FALSE)
{
	for (int i(0); i < _countof(m_pImages); ++i)
	{
		m_pImages[i] = new CImage;
	}
}

CPngButton::~CPngButton()
{
	for (int i(0); i < _countof(m_pImages); ++i)
	{
		delete m_pImages[i];
	}
}


BEGIN_MESSAGE_MAP(CPngButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSELEAVE()
	ON_WM_ENABLE()
END_MESSAGE_MAP()


BOOL CPngButton::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= BS_OWNERDRAW;

	return CButton::PreCreateWindow(cs);
}

void CPngButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

BOOL CPngButton::SetButtonImages(LPCTSTR lpResType, UINT nIDPngResId)
{
	return SetButtonImages(lpResType, MAKEINTRESOURCE(nIDPngResId));
}

BOOL CPngButton::SetButtonImages(LPCTSTR lpResType, LPCTSTR lpPngResName)
{
	BOOL bReturn(FALSE);
	CImage Image;

	HINSTANCE hInst = AfxGetResourceHandle();
	HRSRC hRsrc = ::FindResource(hInst, lpPngResName, lpResType);
	if (hRsrc)
	{
		DWORD len = SizeofResource(hInst, hRsrc);
		BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);
		if (lpRsrc)
		{
			HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, len);
			BYTE* pmem = (BYTE*)GlobalLock(hMem);
			memcpy(pmem, lpRsrc, len);
			IStream* pstm;
			CreateStreamOnHGlobal(hMem, FALSE, &pstm);

			Image.Load(pstm);

			if (Image.GetBPP() == 32)
			{
				for(int i = 0; i < Image.GetWidth(); i++)  
				{  
					for(int j = 0; j < Image.GetHeight(); j++)  
					{  
						LPBYTE pRGBA = (LPBYTE)(Image.GetPixelAddress(i , j));
						pRGBA[0] = pRGBA[0] * pRGBA[3] / 255;
						pRGBA[1] = pRGBA[1] * pRGBA[3] / 255;
						pRGBA[2] = pRGBA[2] * pRGBA[3] / 255;
					}  
				}
			}

			int cx = Image.GetWidth();
			int cy = Image.GetHeight();
			int cxButton = cx / STATUS_COUNT;

			for (int i(0); i < _countof(m_pImages); ++i)
			{
				m_pImages[i]->CreateEx(cxButton, cy, Image.GetBPP(), BI_RGB, NULL, CImage::createAlphaChannel);
				CImageDC dc(*m_pImages[i]);
				Image.Draw(dc, 0, 0, cxButton, cy, i * cxButton, 0, cxButton, cy);
			}
		}
	}
	return bReturn;
}

void CPngButton::_DrawImage(HDC hDC, CImage* pImage)
{
	ASSERT(pImage);
	int cx  = pImage->GetWidth();
	int cy  = pImage->GetHeight();
	int cdx = cx;
	int cdy = cy;

	CRect rcWnd;
	GetWindowRect(&rcWnd);
	if (m_bIsAutoSize)
	{
		CWnd* pParent = GetParent();
		pParent->ScreenToClient(&rcWnd);
		rcWnd.right  = rcWnd.left + cx;
		rcWnd.bottom = rcWnd.top  + cy;
		MoveWindow(&rcWnd);
	}
	else
	{
		cdx = rcWnd.Width();
		cdy = rcWnd.Height();
	}

	pImage->Draw(hDC, 0, 0, cx, cy, 0, 0, cdx, cdy);
}

void CPngButton::SizeToContent(BOOL bAutoSize/* = TRUE*/)
{
	if (bAutoSize && !m_bIsAutoSize)
	{
		int cx = m_pImages[0]->GetWidth();
		int cy = m_pImages[0]->GetHeight();

		CRect rcWnd;
		GetWindowRect(&rcWnd);
		CWnd* pParent = GetParent();
		pParent->ScreenToClient(&rcWnd);
		rcWnd.right  = rcWnd.left + cx;
		rcWnd.bottom = rcWnd.top  + cy;
		MoveWindow(&rcWnd);
	}
	m_bIsAutoSize = bAutoSize;
}

void CPngButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	HDC hDC = lpDrawItemStruct->hDC;

	if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		m_emStatus = BTN_STATUS_DISABLE;
		_DrawImage(hDC, m_pImages[3]);
	}
	else
	{
		switch (m_emStatus)
		{
		case BTN_STATUS_NORMAL:
			_DrawImage(hDC, m_pImages[0]);
			break;
		case BTN_STATUS_OVER:
			_DrawImage(hDC, m_pImages[1]);
			break;
		case BTN_STATUS_DOWN:
			_DrawImage(hDC, m_pImages[2]);
			break;
		}
	}
}

void CPngButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bInButton)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize  = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();
		_TrackMouseEvent(&tme);

		m_bInButton = TRUE;
		m_bNeedFlush = TRUE;
		m_emStatus   = BTN_STATUS_OVER;
	}

	if (m_bNeedFlush)
	{
		m_bNeedFlush = !m_bNeedFlush;
		_UpdateUI();
	}

	CButton::OnMouseMove(nFlags, point);
}

void CPngButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_emStatus  = BTN_STATUS_DOWN;

	_UpdateUI();
	CButton::OnLButtonDown(nFlags, point);
}

void CPngButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_emStatus  = BTN_STATUS_OVER;

	_UpdateUI();
	CButton::OnLButtonUp(nFlags, point);
}

BOOL CPngButton::OnEraseBkgnd(CDC* pDC)
{
	if(GetButtonStyle() & BS_OWNERDRAW)
	{
		return TRUE;
	}
	return CButton::OnEraseBkgnd(pDC);
}

void CPngButton::_UpdateUI()
{
	RECT rcWnd;
	GetWindowRect(&rcWnd);
	CWnd* pParent = GetParent();
	if (pParent)
	{
		pParent->ScreenToClient(&rcWnd);
		pParent->InvalidateRect(&rcWnd);
	}
}

void CPngButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	m_bInButton = FALSE;
	m_emStatus  = BTN_STATUS_NORMAL;
	_UpdateUI();
}

void CPngButton::OnMouseLeave()
{
	m_bInButton = FALSE;
	m_emStatus = BTN_STATUS_NORMAL;
	_UpdateUI();

	CButton::OnMouseLeave();
}

void CPngButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	CPoint point(0, 0);
	GetCursorPos(&point);
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	if (rcWindow.PtInRect(point))
	{
		m_emStatus   = BTN_STATUS_OVER;
	}
	else
	{
		m_emStatus   = BTN_STATUS_NORMAL;
	}
	_UpdateUI();
}
