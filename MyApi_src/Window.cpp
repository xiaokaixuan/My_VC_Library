#include "StdAfx.h"
#include <windows.h>
#include "kxExapi.h"

void kxGetSystrayIconRect(LPCTSTR lpTipText, LPRECT lpRect)
{
	HWND hwnd = FindWindow(_T("Shell_TrayWnd"), NULL);
	if (hwnd != NULL)
	{
		hwnd = FindWindowEx(hwnd, NULL, _T("TrayNotifyWnd"), NULL);
		if (hwnd != NULL)
		{
			hwnd = FindWindowEx(hwnd, NULL, _T("SysPager"), NULL);
			if (hwnd != NULL)
			{
				hwnd = FindWindowEx(hwnd, NULL, _T("ToolbarWindow32"), NULL);
			}
		}
	}
	if (hwnd != NULL)
	{
		DWORD proID(0);
		GetWindowThreadProcessId(hwnd, &proID);
		if (proID != 0)
		{
			HANDLE hPro = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, proID);
			
			typedef struct tagSYSTRAYINFO
			{
				TBBUTTON tbbtn;
				RECT     rect;
				TCHAR    szTxt[MAX_PATH];
			} SYSTRAYINFO, *LPSYSTRAYINFO;
			
			LPVOID pBuf(NULL);
			if (hPro != NULL)
			{
				pBuf = VirtualAllocEx(hPro, NULL, sizeof(SYSTRAYINFO), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			}
			if (pBuf != NULL)
			{
				SYSTRAYINFO info;
				ZeroMemory(&info, sizeof(info));
				LPSYSTRAYINFO pInfo = (LPSYSTRAYINFO)pBuf;
				
				if (WriteProcessMemory(hPro, pBuf, &info, sizeof(info), NULL))
				{
					int iCnt = SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0);
					int i(0);
					for (; i < iCnt; ++i)
					{
						ZeroMemory(&info, sizeof(info));
						
						SendMessage(hwnd, TB_GETBUTTON, i, (LPARAM)&pInfo->tbbtn);
						ReadProcessMemory(hPro, pInfo, &info.tbbtn, sizeof(info.tbbtn), NULL);
						
						SendMessage(hwnd, TB_GETBUTTONTEXT, info.tbbtn.idCommand, (LPARAM)pInfo->szTxt);
						ReadProcessMemory(hPro, pInfo->szTxt, info.szTxt, sizeof(info.szTxt), NULL);
						
						if (lstrcmpi(info.szTxt, lpTipText) == 0)
						{
							SendMessage(hwnd, TB_GETITEMRECT, i, (LPARAM)&pInfo->rect);
							ReadProcessMemory(hPro, &pInfo->rect, &info.rect, sizeof(info.rect), NULL);
							break;
						}
					}
					
					CPoint lt(info.rect.left, info.rect.top);
					CPoint rb(info.rect.right, info.rect.bottom);
					ClientToScreen(hwnd, &lt);
					ClientToScreen(hwnd, &rb);
					
					lpRect->left = lt.x;
					lpRect->top = lt.y;
					lpRect->right = rb.x;
					lpRect->bottom = rb.y;
				}
				VirtualFreeEx(hPro, pBuf, 0, MEM_RELEASE);
			}
			CloseHandle(hPro);
		}
	}
}

void kxAnimateWindowToSystray(HWND hwnd, LPCTSTR lpTipText)
{
	CRect destRc, curRc;
	kxGetSystrayIconRect(lpTipText, &destRc);
	GetWindowRect(hwnd, &curRc);
	DrawAnimatedRects(hwnd, IDANI_CAPTION, &curRc, &destRc);
	ShowWindow(hwnd, SW_HIDE);
}

void kxRestoreWindowFromSystray(HWND hwnd, LPCRECT lpRect, LPCTSTR lpTipText)
{
	CRect destRc(*lpRect), curRc;
	kxGetSystrayIconRect(lpTipText, &curRc);
	DrawAnimatedRects(hwnd, IDANI_CAPTION, &curRc, &destRc);
	ShowWindow(hwnd, SW_SHOW);
}

BOOL kxSetWindowRgnRGB(HWND hwnd, LPCTSTR lpRes, COLORREF transClr)
{
	CRgn wndRgn;
	wndRgn.CreateRectRgn(0, 0, 0, 0);
	
	CBitmap bitmap;
	bitmap.LoadBitmap(lpRes);
	HDC hdc = GetDC(hwnd);
	HDC hMem = CreateCompatibleDC(hdc);
	SelectObject(hMem, bitmap.GetSafeHandle());
	
	BITMAP bmp;
	bitmap.GetBitmap(&bmp);
	int height = bmp.bmHeight;
	int width = bmp.bmWidth;
	
	for (int y(0); y < height; ++y)
	{
		int leftx(0);
		for (int x(0); x < width;)
		{
			while (x < width && GetPixel(hMem, x, y) == transClr)
			{
				++x;
			}
			leftx = x;
			while (x < width && GetPixel(hMem, x, y) != transClr)
			{
				++x;
			}
			if (leftx != x)
			{
				CRgn rgn;
				rgn.CreateRectRgn(leftx, y, x, y + 1);
				wndRgn.CombineRgn(&wndRgn, &rgn, RGN_OR);
				rgn.DeleteObject();
			}
		}
	}
	bitmap.DeleteObject();
	DeleteObject(hMem);
	return SetWindowRgn(hwnd, (HRGN)wndRgn.GetSafeHandle(), TRUE);
}

BOOL kxSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	BOOL bRet(FALSE);

	DWORD const WS_EX_LAYERED = 0x00080000; 
	DWORD const LWA_COLORKEY  = 0x00000001; // specific color transparent.
	DWORD const LWA_ALPHA     = 0x00000002; // window transparent. 
	typedef BOOL (WINAPI *TypeSetLayeredWindowAttributes)( 
		HWND hwnd,           // handle to the layered window 
		COLORREF crKey,      // specifies the color key 
		BYTE bAlpha,         // value for the blend function 
		DWORD dwFlags        // action 
		); 
	
	// modify window extended style. 
	SetWindowLong(hwnd, GWL_EXSTYLE
		, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	
	// Load User32.dll. 
	HMODULE hInst = LoadLibrary(_T("User32.dll")); 
	if (NULL != hInst) 
	{ 
		TypeSetLayeredWindowAttributes pSetTransparent 
			= (TypeSetLayeredWindowAttributes)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		bRet = pSetTransparent(hwnd, crKey, bAlpha, dwFlags);
		FreeLibrary(hInst); 
	}
	return bRet;
}

BOOL kxSetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, HICON hIconUnchecked, HICON hIconChecked, BOOL bBigIcon)
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
