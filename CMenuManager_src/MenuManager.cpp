// MenuManager.cpp: implementation of the CMenuManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MenuManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CMenuManager::CMenuManager()
: m_hInst(0)
, m_hImages(0)
{

}

CMenuManager::CMenuManager(HINSTANCE hInst, int cxIcon, int cyIcon)
{
	Initialize(hInst, cxIcon, cyIcon);
}

CMenuManager::~CMenuManager()
{
	if (m_hImages)
	{
		ImageList_Destroy(m_hImages);
	}
}

void CMenuManager::Initialize(HINSTANCE hInst, int cxIcon, int cyIcon)
{
	m_hInst = hInst;
	m_hImages = ImageList_Create(cxIcon, cyIcon, ILC_COLOR24 | ILC_MASK, 0, 1);
}

int CMenuManager::AddMasked(UINT nIdBitmap, COLORREF crMask)
{
	ASSERT(m_hInst);
	HBITMAP hBitmap = ::LoadBitmap(m_hInst, MAKEINTRESOURCE(nIdBitmap));
	return ImageList_AddMasked(m_hImages, hBitmap, crMask);
}

int CMenuManager::AddMasked(HBITMAP hbmImage, COLORREF crMask)
{
	ASSERT(m_hImages);
	return ImageList_AddMasked(m_hImages, hbmImage, crMask);
}

int CMenuManager::AddIcon(HICON hIcon)
{
	ASSERT(m_hImages);
	return ImageList_AddIcon(m_hImages, hIcon);
}

BOOL CMenuManager::RemoveAll()
{
	ASSERT(m_hImages);
	return ImageList_SetImageCount(m_hImages, 0);
}

BOOL CMenuManager::SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, HICON hIconUnchecked, HICON hIconChecked, BOOL bBigIcon)
{
	ASSERT(hMenu);
	
	HBITMAP hBitmapUnchecked(0);
	HBITMAP hBitmapChecked(0);
	
	int nWidth  = 16;
	int nHeight = 16;
	
	HDC hdc = ::GetDC(0);
	if (hIconUnchecked)
	{
		ICONINFO ii	 = {0};
		::GetIconInfo(hIconUnchecked, &ii);
		BITMAP bitmap = {0};
		::GetObject(ii.hbmColor, sizeof(bitmap), &bitmap);
		if (bBigIcon)
		{
			nWidth  = bitmap.bmWidth;
			nHeight = bitmap.bmHeight;
		}
		HDC hMemDc = CreateCompatibleDC(hdc);
		hBitmapUnchecked = CreateCompatibleBitmap(hdc, nWidth, nWidth);
		::SelectObject(hMemDc, hBitmapUnchecked);
		RECT rect = {0, 0, nWidth, nWidth};
		::FillRect(hMemDc, &rect, GetSysColorBrush(COLOR_MENU));
		::DrawIconEx(hMemDc, 0, 0, hIconUnchecked, nWidth, nHeight, 0, 0, DI_NORMAL);
		::DeleteDC(hMemDc);
	}
	if (hIconChecked)
	{
		ICONINFO ii	 = {0};
		::GetIconInfo(hIconChecked, &ii);
		BITMAP bitmap = {0};
		::GetObject(ii.hbmColor, sizeof(bitmap), &bitmap);
		if (bBigIcon)
		{
			nWidth  = bitmap.bmWidth;
			nHeight = bitmap.bmHeight;
		}
		HDC hMemDc = CreateCompatibleDC(hdc);
		hBitmapChecked = CreateCompatibleBitmap(hdc, nWidth, nWidth);
		::SelectObject(hMemDc, hBitmapChecked);
		RECT rect = {0, 0, nWidth, nWidth};
		::FillRect(hMemDc, &rect, GetSysColorBrush(COLOR_MENU));
		::DrawIconEx(hMemDc, 0, 0, hIconChecked, nWidth, nHeight, 0, 0, DI_NORMAL);
		::DeleteDC(hMemDc);
	}
	::ReleaseDC(0, hdc);
	return ::SetMenuItemBitmaps(hMenu, uPosition, uFlags, hBitmapUnchecked, hBitmapChecked);
}

BOOL CMenuManager::SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, int nImageUnchecked, int nImageChecked)
{
	ASSERT(m_hImages);
	int nImageCount = ImageList_GetImageCount(m_hImages);
	HICON hIconUnchecked(0), hIconChecked(0);
	if (nImageUnchecked < nImageCount)
	{
		hIconUnchecked = ImageList_ExtractIcon(0, m_hImages, nImageUnchecked);
	}
	if (nImageChecked < nImageCount)
	{
		hIconChecked = ImageList_ExtractIcon(0, m_hImages, nImageChecked);
	}
	return SetMenuItemIcons(hMenu, uPosition, uFlags, hIconUnchecked, hIconChecked, TRUE);
}

BOOL CMenuManager::SetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, UINT nIdUnchecked, UINT nIdChecked, BOOL bBigIcon)
{
	ASSERT(m_hInst);
	HICON hIconUnchecked = ::LoadIcon(m_hInst, MAKEINTRESOURCE(nIdUnchecked));
	HICON hIconChecked   = ::LoadIcon(m_hInst, MAKEINTRESOURCE(nIdChecked));

	return SetMenuItemIcons(hMenu, uPosition, uFlags, hIconUnchecked, hIconChecked, bBigIcon);
}
