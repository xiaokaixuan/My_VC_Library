#pragma once

#define STATUS_COUNT 4

enum emSTATUS
{
	BTN_STATUS_NORMAL,
	BTN_STATUS_OVER  ,
	BTN_STATUS_DOWN  ,
	BTN_STATUS_DISABLE
};

class CPngButton : public CButton
{
public:
	CPngButton();
	virtual ~CPngButton();

public:
	void SizeToContent(BOOL bAutoSize = TRUE);
	BOOL SetButtonImages(LPCTSTR lpResType, UINT    nIDPngResId);
	BOOL SetButtonImages(LPCTSTR lpResType, LPCTSTR lpPngResName);

protected:
	BOOL	 m_bIsAutoSize;
	emSTATUS m_emStatus;

	BOOL	 m_bInButton;
	BOOL	 m_bNeedFlush;

	class CImage* m_pImages[4];

protected:
	void _UpdateUI();
	void _DrawImage(HDC hDC, class CImage* pImage);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	afx_msg void OnMouseLeave();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

