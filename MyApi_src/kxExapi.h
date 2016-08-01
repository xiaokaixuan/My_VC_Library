#ifndef _KX_EXAPI_H_
#define _KX_EXAPI_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/********** Start icon definitons. ***********/

// Icon files definitions.
#pragma pack(push, 1)
typedef struct tagICONHEADER
{
	WORD idReserved;
    WORD idType;
    WORD idCount;
}ICONHEADER, *LPICONHEADER;

typedef struct tagICONDIRENTRY
{   
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
}ICONDIRENTRY, *LPICONDIRENTRY;

// RT_GROUP_ICON definitions.
typedef struct tagGRPICONDIRENTRY
{   
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    WORD nID;
}GRPICONDIRENTRY, *LPGRPICONDIRENTRY;

typedef struct tagGRPICONDIR
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
    GRPICONDIRENTRY idEntries[1];
}GRPICONDIR, *LPGRPICONDIR;
#pragma pack( pop )

/********** End icon definition. ***********/

// APIs list about resources...
BOOL kxDeleteIcon(LPCTSTR lpExeName, LPCTSTR lpResName);
BOOL kxSaveIcon(LPCTSTR lpExeName, LPCTSTR lpResName, LPCTSTR lpIcoName);
BOOL kxInsertIcon(LPCTSTR lpExeName, LPCTSTR lpIconFile, LPCTSTR lpResName);
BOOL kxExtractResource(HMODULE hModule, LPCTSTR lpResName, LPCTSTR lpResType, LPCTSTR lpOutFile);

// APIs list about files...
UINT kxGetFileRows(LPCTSTR lpFileName);
BOOL kxCreateRankDirectory(LPCTSTR lpPath);
CString kxCreateUniqueDirectory(LPCTSTR lpPrefix);

// APIs list about memory...
BOOL kxIsObjectOnHeap(LPVOID pObject);
BOOL kxIsObjectOnStack(LPVOID pObject);

// APIs list about Windows...
void kxGetSystrayIconRect(LPCTSTR lpTipText, LPRECT lpRect);
void kxAnimateWindowToSystray(HWND hwnd, LPCTSTR lpTipText);
void kxRestoreWindowFromSystray(HWND hwnd, LPCRECT lpRect, LPCTSTR lpTipText);
BOOL kxSetWindowRgnRGB(HWND hwnd, LPCTSTR lpRes, COLORREF transClr);
BOOL kxSetLayeredWindowAttributes(HWND hwnd, COLORREF color, BYTE byte, DWORD dwFlags);
BOOL kxSetMenuItemIcons(HMENU hMenu, UINT uPosition, UINT uFlags, HICON hIconUnchecked, HICON hIconChecked, BOOL bBigIcon);

// APIs list about process...
BOOL kxShellProcess(LPHANDLE lpHandle, LPCTSTR lpExeFile, LPCTSTR lpParameters, int nShow, BOOL bWait);
BOOL kxCreateProcess(LPHANDLE lpHandle, LPCTSTR lpFileName, LPCTSTR lpParameters, int nShow, BOOL bWait);

#endif //_KX_EXAPI_H_

