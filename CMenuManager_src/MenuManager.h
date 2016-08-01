// MenuManager.h: interface for the CMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUMANAGER_H__60B6BA53_C883_4DAD_9C59_79088EB53A8B__INCLUDED_)
#define AFX_MENUMANAGER_H__60B6BA53_C883_4DAD_9C59_79088EB53A8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMenuManager  
{
public:
	CMenuManager();
	CMenuManager(HINSTANCE hInst, int cxIcon, int cyIcon);
	virtual ~CMenuManager();

public:
	BOOL RemoveAll();
	void Initialize(HINSTANCE hInst, int cxIcon, int cyIcon);
	int  AddIcon(HICON hIcon);
	int  AddMasked(HBITMAP hbmImage, COLORREF crMask);
	int  AddMasked(UINT nIdBitmap, COLORREF crMask);
	
	BOOL SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, int nImageUnchecked, int nImageChecked);
	BOOL SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, UINT nIdUnchecked, UINT nIdChecked, BOOL bBigIcon);
	BOOL SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, HICON hIconUnchecked, HICON hIconChecked, BOOL bBigIcon);

protected:
	HINSTANCE  m_hInst;
	HIMAGELIST m_hImages;
};

#endif // !defined(AFX_MENUMANAGER_H__60B6BA53_C883_4DAD_9C59_79088EB53A8B__INCLUDED_)
