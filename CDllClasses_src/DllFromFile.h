// DllFromFile.h: interface for the CDllFromFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLFROMFILE_H__AF2AED0E_A76F_4E70_96B5_11EB3B98A8A8__INCLUDED_)
#define AFX_DLLFROMFILE_H__AF2AED0E_A76F_4E70_96B5_11EB3B98A8A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDllFromFile  
{
public:
	CDllFromFile();
	CDllFromFile(LPCTSTR lpFileName);
	virtual ~CDllFromFile();

public:
	FARPROC GetProcAddress(LPCSTR lpProcName);
	HMODULE GetModuleHandle();
	BOOL LoadLibrary(LPCTSTR lpFileName);
	BOOL FreeLibrary();

protected:
	HMODULE m_hDll;
};

#endif // !defined(AFX_DLLFROMFILE_H__AF2AED0E_A76F_4E70_96B5_11EB3B98A8A8__INCLUDED_)
