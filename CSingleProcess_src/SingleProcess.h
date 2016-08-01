#if !defined(AFX_SINGLEPROCESS_H__C1B7814D_A6C9_4FFC_854F_DFF7DDE181DF__INCLUDED_)
#define AFX_SINGLEPROCESS_H__C1B7814D_A6C9_4FFC_854F_DFF7DDE181DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSingleProcess  
{
public:
	explicit CSingleProcess(LPCTSTR lpFlag);
	virtual ~CSingleProcess();

public:
	BOOL IsExist() const;
	HWND GetMainWnd() const;
	void SetMainWnd(HWND hWnd);
	BOOL ActivateWindow(int nCmdShow) const;
	void DeleteObject();

private:
	void __Term();
	void __Init(LPCTSTR lpFlag);

protected:
	BOOL   m_bIsExist;
	HANDLE m_hMap;
	HWND*  m_phMainWnd;
};

#endif // !defined(AFX_SINGLEPROCESS_H__C1B7814D_A6C9_4FFC_854F_DFF7DDE181DF__INCLUDED_)
