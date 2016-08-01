#if !defined(AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_)
#define AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconListBox.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// ListBox ctrl must include styles: LBS_OWNERDRAWFIXED | LBS_MULTICOLUMN
// CIconBox window
#include <afxtempl.h>

class CIconBox : public CListBox
{
// Construction
public:
	CIconBox();
	virtual ~CIconBox();

// Attributes
protected:
	CArray<HICON, HICON> m_hIcons;

// Operations
public:
	int AddIcon(HICON hIcon);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	// Generated message map functions
protected:
	//{{AFX_MSG(CIconBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_)
