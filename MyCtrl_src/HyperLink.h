#if !defined(AFX_HyperLink_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)
#define AFX_HyperLink_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperLink.h : header file
//


class CHyperLink : public CStatic
{
public:
	CHyperLink();
	
protected:
	BOOL m_bAllow;
	BOOL m_bClicked;

public:
	
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	
public:
	virtual ~CHyperLink();
	
public:
	BOOL EnableNavigate(BOOL lpAllow);
	BOOL SetTipText(LPCTSTR lpTip);
	BOOL SetLinkAddress(LPCTSTR lpAddress);
protected:
	CString m_strAddress;
	CString m_strTip;
	CToolTipCtrl m_tipCtrl;

protected:
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_HyperLink_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)

