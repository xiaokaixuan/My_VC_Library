// Dwm.h: interface for the CDwm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DWM_H__B9BA84EE_E76D_4D48_97D9_4C4C8A435479__INCLUDED_)
#define AFX_DWM_H__B9BA84EE_E76D_4D48_97D9_4C4C8A435479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4996)

typedef struct tagCT_MARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
} CT_MARGINS, *PCT_MARGINS;

class CDwm
{
public:
	CDwm();
	virtual ~CDwm();

public:
	static BOOL    DwmIsCompositionEnabled();
	static BOOL	   DwmSetAeroColor(HWND hwnd, COLORREF clr);
	static HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, const CT_MARGINS* pMarInset);
	static VOID	   DrawGlowingText(HDC hDC, LPCTSTR szText, RECT &rcArea, DWORD dwTextFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE, int iGlowSize = 10);

protected:
	class CInitor  
	{
		friend class CDwm;
	public:
		CInitor();
		virtual ~CInitor();
	private:
		HMODULE  m_hDwmApi;
		HMODULE  m_hUxTheme;
	};
	static CInitor  ms_Initor;
	static COLORREF ms_clrAero;
};

#endif // !defined(AFX_DWM_H__B9BA84EE_E76D_4D48_97D9_4C4C8A435479__INCLUDED_)
