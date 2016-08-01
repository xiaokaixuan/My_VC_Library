// DllFromMem.h: interface for the CDllFromMem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLFROMMEM_H__233D9B97_BA88_48C7_AC00_03525B40C7F1__INCLUDED_)
#define AFX_DLLFROMMEM_H__233D9B97_BA88_48C7_AC00_03525B40C7F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef BOOL (WINAPI *myDllMain)(
								 HINSTANCE hinstDLL,  // handle to the DLL module
								 DWORD fdwReason,     // reason for calling function
								 LPVOID lpvReserved   // reserved
);

class CDllFromMem  
{
public:
	CDllFromMem();
	virtual ~CDllFromMem();
	HANDLE LoadLibraryFromRs(HINSTANCE hInstance,LPCTSTR lpType,LPCTSTR lpName);
	FARPROC GetProcAddressFromRs(LPCSTR lpProcName);
	
private:
	HANDLE m_hBaseAddress;
	HINSTANCE m_hInstance;
	IMAGE_NT_HEADERS *m_ntHead;
	myDllMain m_dllMain;
};

#endif // !defined(AFX_DLLFROMMEM_H__233D9B97_BA88_48C7_AC00_03525B40C7F1__INCLUDED_)
