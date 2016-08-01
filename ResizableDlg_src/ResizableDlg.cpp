#include "StdAfx.h"
#include "ResizableDlg.h"

tControl::tControl(HWND hWnd, CRect rect, int anchor, double dxMove, double dyMove, double dxResize, double dyResize, BOOL isTransparent)
{
	m_hWnd = hWnd;
	m_rect = rect;
	m_anchor = anchor;
	m_dxMove = dxMove;
	m_dyMove = dyMove;
	m_dxResize = dxResize;
	m_dyResize = dyResize;
	m_isTransparent = isTransparent;
}

CResizableDlg::CResizableDlg(UINT nIDTemplate, int dlgWidthMin, int dlgHeightMin, CWnd* pParent)
: CDialog(nIDTemplate, pParent)
, m_hGripper(NULL)
, m_sizeMinimum(dlgWidthMin, dlgHeightMin)
{
}

CResizableDlg::~CResizableDlg()
{
	for (int i(0); i < m_controls.GetSize(); ++i)
	{
		delete m_controls.GetAt(i);
	}
	m_controls.RemoveAll();
}

// 0 <= dxMoveRelative <= 1, 0 <= dxResizeRelative <= 1, dxMoveRelative + dxResizeRelative <= 1
// 0 <= dyMoveRelative <= 1, 0 <= dyResizeRelative <= 1, dyMoveRelative + dyResizeRelative <= 1
// isTransparent = TRUE: group box
void CResizableDlg::MoveResizeControl(int nID, int anchor, double dxMoveRelative, double dyMoveRelative, double dxResizeRelative, double dyResizeRelative, BOOL isTransparent)
{
	CRect rect; HWND hWnd;

	GetDlgItem(nID, &hWnd); GetDlgItem(nID)->GetWindowRect(&rect); ScreenToClient(&rect);
	m_controls.Add(new tControl(hWnd, rect, anchor, dxMoveRelative, dyMoveRelative, dxResizeRelative, dyResizeRelative, isTransparent));
}

void CResizableDlg::LoadDlgPlacement()
{
	UINT nBytes = 0; BYTE* pBytes = NULL;
	
	if (AfxGetApp()->GetProfileBinary(_T("Workspace"), _T("WindowPlacement"), &pBytes, &nBytes))
	{
		if (nBytes == sizeof(WINDOWPLACEMENT))
		{
			SetWindowPlacement((WINDOWPLACEMENT*)pBytes);
		}
		delete[] pBytes;
	}
}

void CResizableDlg::SaveDlgPlacement()
{
	WINDOWPLACEMENT wp = { 0 };

	if (GetWindowPlacement(&wp))
	{
		wp.ptMinPosition.x = m_sizeMinimum.cx; wp.ptMinPosition.y = m_sizeMinimum.cy;
		AfxGetApp()->WriteProfileBinary(_T("Workspace"), _T("WindowPlacement"), (LPBYTE)&wp, wp.length);
	}
}

BEGIN_MESSAGE_MAP(CResizableDlg, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CResizableDlg::OnInitDialog()
{
	CRect rect, rectGripper;

	CDialog::OnInitDialog();

	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

	// If minimum dialog width/height size not provided (value 0), use initial dialog width/height size as minimal.
	if (m_sizeMinimum.cx == 0 || m_sizeMinimum.cy == 0) 
	{ 
		GetWindowRect(rect);
		if (m_sizeMinimum.cx == 0) m_sizeMinimum.cx = rect.Width();
		if (m_sizeMinimum.cy == 0) m_sizeMinimum.cx = rect.Height();	
	}

	// Calculate initial size of the client area as a baseline for moving/resizing controls.
	GetClientRect(rect); m_sizeInitial = rect.Size();

	// Create gripper at the bottom-right corner.
	if (GetStyle() & WS_THICKFRAME)
	{
		rectGripper.SetRect(-GetSystemMetrics(SM_CXVSCROLL), -GetSystemMetrics(SM_CYHSCROLL), 0, 0);
		rectGripper.OffsetRect(rect.BottomRight());
		if (m_hGripper = CreateWindow(_T("Scrollbar"), _T("size"), WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP, rectGripper.left, rectGripper.top, rectGripper.Width(), rectGripper.Height(), m_hWnd, NULL, AfxGetInstanceHandle(), NULL))
		{
			::SetWindowPos(m_hGripper, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
			m_controls.Add(new tControl(m_hGripper, rectGripper, tControl::anchor::TOP_LEFT, 1, 1, 0, 0, FALSE));
		}
	}

	LoadDlgPlacement();

	return TRUE;
}

void CResizableDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);

	if (lpMMI->ptMinTrackSize.x < m_sizeMinimum.cx) lpMMI->ptMinTrackSize.x = m_sizeMinimum.cx;
	if (lpMMI->ptMinTrackSize.y < m_sizeMinimum.cy) lpMMI->ptMinTrackSize.y = m_sizeMinimum.cy;
}

void CResizableDlg::OnSize(UINT nType, int cx, int cy)
{
	int dx, dy;
	UINT uFlags;
	CRect rectNew;
	HDWP hDefer = NULL;

	CDialog::OnSize(nType, cx, cy);

	if (hDefer = BeginDeferWindowPos((int)m_controls.GetSize()))
	{
		dx = cx - m_sizeInitial.cx;
		dy = cy - m_sizeInitial.cy;
		for (int i(0); i < m_controls.GetSize() && hDefer; ++i)
		{
			rectNew = m_controls.GetAt(i)->m_rect;
			if (m_controls.GetAt(i)->m_anchor == tControl::anchor::TOP_LEFT)
			{
				rectNew.OffsetRect((int)(dx * m_controls.GetAt(i)->m_dxMove), (int)(dy * m_controls.GetAt(i)->m_dyMove));
				rectNew.right  += (int)(dx * m_controls.GetAt(i)->m_dxResize);
				rectNew.bottom += (int)(dy * m_controls.GetAt(i)->m_dyResize);
			}
			else if (m_controls.GetAt(i)->m_anchor == tControl::anchor::BOTTOM_LEFT)
			{
				rectNew.OffsetRect((int)(dx * m_controls.GetAt(i)->m_dxMove), (int)(dy * (1 - m_controls.GetAt(i)->m_dyMove)));
				rectNew.right += (int)(dx * m_controls.GetAt(i)->m_dxResize);
				rectNew.top   -= (int)(dy * m_controls.GetAt(i)->m_dyResize);
			}
			else if (m_controls.GetAt(i)->m_anchor == tControl::anchor::TOP_RIGHT)
			{
				rectNew.OffsetRect((int)(dx * (1 - m_controls.GetAt(i)->m_dxMove)), (int)(dy * m_controls.GetAt(i)->m_dyMove));
				rectNew.left   -= (int)(dx * m_controls.GetAt(i)->m_dxResize);
				rectNew.bottom += (int)(dy * m_controls.GetAt(i)->m_dyResize);
			}
			else
			{
				rectNew.OffsetRect((int)(dx * (1 - m_controls.GetAt(i)->m_dxMove)), (int)(dy * (1 - m_controls.GetAt(i)->m_dyMove)));
				rectNew.left -= (int)(dx * m_controls.GetAt(i)->m_dxResize);
				rectNew.top  -= (int)(dy * m_controls.GetAt(i)->m_dyResize);
			}
			uFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | (m_controls.GetAt(i)->m_isTransparent ? SWP_NOCOPYBITS : SWP_NOZORDER);
			hDefer = DeferWindowPos(hDefer, m_controls.GetAt(i)->m_hWnd, HWND_TOP, rectNew.left, rectNew.top, rectNew.Width(), rectNew.Height(), uFlags);
		}
		if(hDefer)
		{
			EndDeferWindowPos(hDefer);
		}
	}
	if (m_hGripper)
	{
		::ShowWindow(m_hGripper, (nType == SIZE_MAXIMIZED) ? SW_HIDE : SW_SHOW);
	}
}

void CResizableDlg::OnDestroy()
{
	SaveDlgPlacement();
	if (m_hGripper)
	{
		::DestroyWindow(m_hGripper);
		m_hGripper = NULL;
	}
	for (int i(0); i < m_controls.GetSize(); ++i)
	{
		delete m_controls.GetAt(i);
	}
	m_controls.RemoveAll();
	CDialog::OnDestroy();
}

