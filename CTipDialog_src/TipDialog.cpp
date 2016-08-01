// TipDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TipDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTipDialog* CTipDialog::ms_pDlg = NULL;

LRESULT CALLBACK CTipDialog::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (HC_ACTION == nCode && (WM_LBUTTONDOWN == wParam || WM_RBUTTONDOWN == wParam))
	{
		if (NULL != ms_pDlg)
		{
			CPoint pt;
			GetCursorPos(&pt);
			CRect rcDlg;
			ms_pDlg->GetWindowRect(&rcDlg);
			if (!rcDlg.PtInRect(pt))
			{
				CWnd* pPar = ms_pDlg->GetParent();
				if (NULL != pPar)
				{
					CRect rcPar;
					pPar->GetWindowRect(&rcPar);
					if (rcPar.PtInRect(pt))
					{
						ms_pDlg->EndDialog(IDCANCEL);
					}
				}
			}
		}
	}
	return CallNextHookEx(ms_pDlg->m_hhk, nCode, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CTipDialog dialog
CTipDialog::CTipDialog()
{

}

CTipDialog::CTipDialog(LPCTSTR lpszTemplateName, CWnd* pParent /*=NULL*/)
	: CDialog(lpszTemplateName, pParent)
{

}

CTipDialog::CTipDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate, pParent)
{

}

BEGIN_MESSAGE_MAP(CTipDialog, CDialog)
	//{{AFX_MSG_MAP(CTipDialog)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipDialog message handlers

BOOL CTipDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	ms_pDlg = this;
	m_hhk = SetWindowsHookEx(14, LowLevelMouseProc, AfxGetInstanceHandle(), 0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTipDialog::OnDestroy()
{
	ms_pDlg = NULL;
	UnhookWindowsHookEx(m_hhk);
	CDialog::OnDestroy();
}

