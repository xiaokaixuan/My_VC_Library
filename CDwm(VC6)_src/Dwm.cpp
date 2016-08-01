// Dwm.cpp: implementation of the CDwm class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Dwm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDwm::CInitor CDwm::ms_Initor;
COLORREF CDwm::ms_clrAero = RGB(0, 0, 0);


CDwm::CInitor::CInitor()
{
	m_hDwmApi  = ::LoadLibrary(_T("DwmApi.dll"));
	m_hUxTheme = ::LoadLibrary(_T("UxTheme.dll"));
}

CDwm::CInitor::~CInitor()
{
	if (m_hDwmApi)
	{
		::FreeLibrary(m_hDwmApi);
	}
	if (m_hUxTheme)
	{
		::FreeLibrary(m_hUxTheme);
	}
}

CDwm::CDwm()
{

}

CDwm::~CDwm()
{

}

BOOL CDwm::DwmIsCompositionEnabled()
{
	typedef HRESULT (WINAPI *fnDwmIsCompositionEnabled)(BOOL* pfEnabled);
	BOOL bRet(FALSE);
	if (ms_Initor.m_hDwmApi)
	{
		fnDwmIsCompositionEnabled pDwmIsCompositionEnabled
			= (fnDwmIsCompositionEnabled)::GetProcAddress(ms_Initor.m_hDwmApi, "DwmIsCompositionEnabled");
		if (NULL != pDwmIsCompositionEnabled)
		{
			pDwmIsCompositionEnabled(&bRet);
		}
	}
	return bRet;
}

HRESULT CDwm::DwmExtendFrameIntoClientArea(HWND hWnd, const CT_MARGINS* pMarInset)
{
	typedef HRESULT (WINAPI *fnDwmExtendFrameIntoClientArea)(HWND hWnd, const CT_MARGINS* pMarInset);
	HRESULT hRet(-1);
	if (DwmIsCompositionEnabled())
	{
		fnDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea
			= (fnDwmExtendFrameIntoClientArea)::GetProcAddress(ms_Initor.m_hDwmApi, "DwmExtendFrameIntoClientArea");
		if (NULL != pDwmExtendFrameIntoClientArea)
		{
			hRet = pDwmExtendFrameIntoClientArea(hWnd, pMarInset);
		}
	}
	return hRet;
}

BOOL CDwm::DwmSetAeroColor(HWND hwnd, COLORREF clr)
{
	BOOL bRet(FALSE);
	typedef BOOL (WINAPI *fnSetLayeredWindowAttributes)(
		HWND hwnd,           // handle to the layered window 
		COLORREF crKey,      // specifies the color key 
		BYTE bAlpha,         // value for the blend function 
		DWORD dwFlags        // action 
		);
	if (DwmIsCompositionEnabled())
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | 0x80000);
		HMODULE hInst = LoadLibrary(_T("User32.dll"));
		if (hInst) 
		{ 
			fnSetLayeredWindowAttributes pSetTransparent
				= (fnSetLayeredWindowAttributes)GetProcAddress(hInst, "SetLayeredWindowAttributes");
			bRet = pSetTransparent(hwnd, clr, 0, 0x00000001);
			ms_clrAero = clr;
			FreeLibrary(hInst);
		}
	}
	return bRet;
}

typedef int (WINAPI *CT_DTT_CALLBACK_PROC)(HDC hdc, LPWSTR pszText, int cchText, LPRECT prc, UINT dwFlags, LPARAM lParam);
typedef struct tagCT_DTTOPTS 
{
	DWORD             dwSize;
	DWORD             dwFlags;
	COLORREF          crText;
	COLORREF          crBorder;
	COLORREF          crShadow;
	int               iTextShadowType;
	POINT             ptShadowOffset;
	int               iBorderSize;
	int               iFontPropId;
	int               iColorPropId;
	int               iStateId;
	BOOL              fApplyOverlay;
	int               iGlowSize;
	CT_DTT_CALLBACK_PROC pfnDrawTextCallback;
	LPARAM            lParam;
} CT_DTTOPTS, *PCT_DTTOPTS;

VOID CDwm::DrawGlowingText(HDC hDC, LPCTSTR szText, RECT &rcArea, DWORD dwTextFlags/* = DT_LEFT | DT_VCENTER | DT_SINGLELINE*/, int iGlowSize/* = 10*/)
{
	typedef HANDLE  (WINAPI *fnOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
	typedef HRESULT (WINAPI *fnCloseThemeData)(HANDLE hTheme);
	typedef HRESULT (WINAPI *fnDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, 
		int iCharCount, DWORD dwFlags, LPRECT pRect, const CT_DTTOPTS *pOptions);
	
	if (ms_Initor.m_hUxTheme)
	{
		fnOpenThemeData   pFunOpenThemeData   = (fnOpenThemeData)::GetProcAddress(ms_Initor.m_hUxTheme, "OpenThemeData");
		fnCloseThemeData  pFunCloseThemeData  = (fnCloseThemeData)::GetProcAddress(ms_Initor.m_hUxTheme, "CloseThemeData");
		fnDrawThemeTextEx pFunDrawThemeTextEx = (fnDrawThemeTextEx)::GetProcAddress(ms_Initor.m_hUxTheme, "DrawThemeTextEx");
		if (pFunOpenThemeData && pFunDrawThemeTextEx)
		{ 
			HANDLE hTheme = pFunOpenThemeData(GetDesktopWindow(), L"TextStyle");
			
			HDC hMemDC = CreateCompatibleDC(hDC);
			
			BITMAPINFO bmpinfo = {0};
			bmpinfo.bmiHeader.biSize = sizeof(bmpinfo.bmiHeader);
			bmpinfo.bmiHeader.biBitCount = 32;
			bmpinfo.bmiHeader.biCompression = BI_RGB;
			bmpinfo.bmiHeader.biPlanes = 1;
			bmpinfo.bmiHeader.biWidth = rcArea.right - rcArea.left;
			bmpinfo.bmiHeader.biHeight = -(rcArea.bottom - rcArea.top);
			HBITMAP hBitmap = CreateDIBSection(hMemDC, &bmpinfo, DIB_RGB_COLORS, 0, NULL, 0);

			SelectObject(hMemDC, hBitmap);

			if (!hBitmap) return;

			CT_DTTOPTS dttopts = {0};
			dttopts.dwSize = sizeof(dttopts);
			dttopts.dwFlags = (1UL << 11) | (1UL << 13);
			dttopts.iGlowSize = iGlowSize;

			WCHAR wszText[2048] = {0};
#if defined(_UNICODE) || defined(UNICODE)
			lstrcpyn(wszText, szText, 2048);
#else
			mbstowcs(wszText, szText, 2048);
#endif // defined(_UNICODE) || defined(UNICODE)

			RECT rc = {0, 0, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top};

			HBRUSH hBrush = CreateSolidBrush(ms_clrAero);
			FillRect(hMemDC, &rc, hBrush);
			DeleteObject(hBrush);

			HRESULT hr = pFunDrawThemeTextEx(hTheme, hMemDC, 8, 0, wszText, -1, dwTextFlags , &rc, &dttopts);
			if(FAILED(hr)) return;

			BitBlt(hDC, rcArea.left, rcArea.top, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top, hMemDC, 0, 0, SRCCOPY | 0x40000000);
			
			DeleteObject(hBitmap);
			DeleteDC(hMemDC);
			if (pFunCloseThemeData)
			{
				pFunCloseThemeData(hTheme);
			}
		}
	}
}
