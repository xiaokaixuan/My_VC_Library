// IconListBox.cpp : implementation file
//

#include "StdAfx.h"
#include "IconBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconBox

CIconBox::CIconBox()
{

}

CIconBox::~CIconBox()
{

}

BEGIN_MESSAGE_MAP(CIconBox, CListBox)
	//{{AFX_MSG_MAP(CIconBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CIconBox::AddIcon(HICON hIcon)
{
	int nIndex = AddString((LPCTSTR)hIcon);
	m_hIcons.Add(hIcon);
	return nIndex;
}

void CIconBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your code to draw the specified item
	int itemID = lpDrawItemStruct->itemID;
	CRect rcItem = lpDrawItemStruct->rcItem;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if(lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		pDC->DrawIcon(rcItem.left + 2, rcItem.top + 6, m_hIcons.GetAt(itemID));
	}
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		pDC->FillSolidRect(&rcItem, GetSysColor(COLOR_HIGHLIGHT));
		pDC->DrawIcon(rcItem.left + 2, rcItem.top + 6, m_hIcons.GetAt(itemID));
		pDC->DrawFocusRect(&rcItem);
	}
	if ((lpDrawItemStruct->itemAction & ODA_SELECT) &&
		!(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		pDC->FillSolidRect(&rcItem, GetSysColor(COLOR_WINDOW));
		pDC->DrawIcon(rcItem.left + 2, rcItem.top + 6, m_hIcons.GetAt(itemID));
	}
}
