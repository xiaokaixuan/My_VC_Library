// MCIWnd.h: interface for the CMCIWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MCIWND_H__855D34A7_FA1D_4CDC_8897_56E4B554AE13__INCLUDED_)
#define AFX_MCIWND_H__855D34A7_FA1D_4CDC_8897_56E4B554AE13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMCIWnd  
{
public:
	CMCIWnd();
	virtual ~CMCIWnd();

public:
	BOOL Attach(HWND hwnd);
	HWND Detach();
	operator HWND() const;
	HWND GetSafeHwnd() const;
	HWND Create(HWND hwndParent, HINSTANCE hInstance, DWORD dwStyle, LPCTSTR szFile);

public:
	LONG Open(LPCTSTR szFile, DWORD dwFlags);
	LONG Record();
	LONG Play();
	LONG Pause();
	LONG Stop();
	LONG Save(LPCTSTR szFile);
	LONG Close();
	void Destroy();

public:
	LONG GetPos();
	LONG GetVolume();
	LONG SetVolume(UINT iVol);

public:
	LONG New(LPCSTR pszDevice);
	LONG Seek(LONG iPos);
	LONG GetLength();
	BOOL CanPlay();
	BOOL CanRecord();
	BOOL CanSave();
	BOOL CanConfig();

private:
	HWND m_hWnd;
};

#endif // !defined(AFX_MCIWND_H__855D34A7_FA1D_4CDC_8897_56E4B554AE13__INCLUDED_)
