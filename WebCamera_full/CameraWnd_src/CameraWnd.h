#pragma once

#include <USBCammera.h>

// CCameraWnd

class CCameraWnd : public CWnd
{
	DECLARE_DYNAMIC(CCameraWnd)

public:
	CCameraWnd();
	virtual ~CCameraWnd();

protected:
	int			m_nCamIndex;

	HBITMAP		m_hBitmap;
	CUSBCammera m_camMgr;

public:
	BOOL Create(int nCamIndex, DWORD dwStyle, const RECT& rect, CWnd* pParent, UINT uID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


