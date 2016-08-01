// CameraWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "CameraWnd.h"

#define IDI_TIME_DRAW	101		//绘图

// CCameraWnd

IMPLEMENT_DYNAMIC(CCameraWnd, CWnd)

CCameraWnd::CCameraWnd()
: m_nCamIndex(0)
, m_hBitmap(NULL)
{

}

CCameraWnd::~CCameraWnd()
{
}

BOOL CCameraWnd::Create(int nCamIndex, DWORD dwStyle, const RECT& rect, CWnd* pParent, UINT uID)
{
	m_nCamIndex = nCamIndex;
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParent, uID);
}


BEGIN_MESSAGE_MAP(CCameraWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CCameraWnd 消息处理程序


int CCameraWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (CUSBCammera::GetCamCount() > m_nCamIndex)
	{
		try
		{
			if (m_camMgr.OpenCamera(m_nCamIndex, FALSE, 640, 480))
			{
				SetTimer(IDI_TIME_DRAW, 50, NULL);
			}
		}
		catch (...)
		{
		}
	}

	return 0;
}

void CCameraWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == IDI_TIME_DRAW)
	{
		try
		{
			m_hBitmap = m_camMgr.QueryFrame();
		}
		catch (...)
		{
			m_hBitmap = NULL;
		}
		Invalidate();
	}

	CWnd::OnTimer(nIDEvent);
}

void CCameraWnd::OnDestroy()
{
	KillTimer(IDI_TIME_DRAW);

	CWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

BOOL CCameraWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_hBitmap)
	{
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		dc.SelectObject(m_hBitmap);

		CRect rc;
		GetClientRect(&rc);

		pDC->SetStretchBltMode(COLORONCOLOR);
		pDC->StretchBlt(0, 0, rc.Width(), rc.Height(), &dc, 639, 479, -640, -480, SRCCOPY);

		dc.DeleteDC();
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	return CWnd::OnEraseBkgnd(pDC);
}
