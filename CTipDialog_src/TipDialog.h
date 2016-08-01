#if !defined(AFX_TIPDIALOG_H__7ABB0B09_B9DD_4157_9E1C_25E0AFC8E4CD__INCLUDED_)
#define AFX_TIPDIALOG_H__7ABB0B09_B9DD_4157_9E1C_25E0AFC8E4CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTipDialog dialog

class CTipDialog : public CDialog
{
// Construction
public:
	CTipDialog();   // standard constructor
	CTipDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CTipDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CTipDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipDialog)
	//}}AFX_VIRTUAL

// Implementation
protected:
	HHOOK m_hhk;
	static CTipDialog* ms_pDlg;
	static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CTipDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPDIALOG_H__7ABB0B09_B9DD_4157_9E1C_25E0AFC8E4CD__INCLUDED_)
