#include "StdAfx.h"
#include <windows.h>
#include "kxExapi.h"

BOOL kxDeleteIcon(LPCTSTR lpExeName, LPCTSTR lpResName)
{
	BOOL bRet(FALSE);
	HMODULE hInst = LoadLibrary(lpExeName);
	if (NULL != hInst)
	{
		HRSRC hRes = FindResource(hInst, lpResName, RT_GROUP_ICON);
		if (NULL != hRes)
		{
			HGLOBAL hGlobal = LoadResource(hInst, hRes);
			if (NULL != hGlobal)
			{
				LPVOID lpData = LockResource(hGlobal);
				if (NULL != lpData)
				{
					GRPICONDIR grpDir = {0};
					CopyMemory(&grpDir, lpData, sizeof(grpDir));
					DWORD dwGrpSize = sizeof(GRPICONDIR) + (grpDir.idCount - 1)*sizeof(GRPICONDIRENTRY);
					LPGRPICONDIR lpGrp = (LPGRPICONDIR)new BYTE[dwGrpSize]();
					CopyMemory(lpGrp, lpData, dwGrpSize);

					UnlockResource(hGlobal);
					FreeLibrary(hInst);
					hInst = NULL;
					
					HANDLE hUpdate = BeginUpdateResource(lpExeName, FALSE);
					if (NULL != hUpdate)
					{
						bRet = UpdateResource(hUpdate, RT_GROUP_ICON, lpResName
							, MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT)
							, NULL, 0);
						for (int i(0); i < grpDir.idCount; ++i)
						{
							bRet = UpdateResource(hUpdate, RT_ICON
								, MAKEINTRESOURCE(lpGrp->idEntries[i].nID)
								, MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT)
								, NULL, 0);
						}
						EndUpdateResource(hUpdate, FALSE);
					}
					delete []lpGrp;
					lpGrp = NULL;
				}
			}
		}
	}
	if (NULL != hInst)
	{
		FreeLibrary(hInst);
		hInst = NULL;
	}
	return bRet;
}

BOOL kxInsertIcon(LPCTSTR lpExeName, LPCTSTR lpIconFile, LPCTSTR lpResName)
{   
	BOOL bRet(FALSE);
	DWORD dwReaded(0);

	HANDLE hFile = CreateFile(lpIconFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		kxDeleteIcon(lpExeName, lpResName);

		ICONHEADER header = {0};
		ReadFile(hFile, &header, sizeof(header), &dwReaded, NULL);
		if (dwReaded >= sizeof(header))
		{
			LPICONDIRENTRY pIcons = new ICONDIRENTRY[header.idCount]();
			ReadFile(hFile, pIcons, header.idCount*sizeof(ICONDIRENTRY), &dwReaded, NULL);
			if (dwReaded >= header.idCount*sizeof(ICONDIRENTRY))
			{
				DWORD dwGrpSize = sizeof(GRPICONDIR) + (header.idCount - 1)*sizeof(GRPICONDIRENTRY);
				LPGRPICONDIR lpGrp = (LPGRPICONDIR)new BYTE[dwGrpSize]();
				lpGrp->idType = header.idType;
				lpGrp->idCount = header.idCount;
				lpGrp->idReserved = header.idReserved;

				HMODULE hInst = ::LoadLibrary(lpExeName);
				if (hInst != NULL)
				{
					int nIndex(0);
					WORD wID(1);
					for (; wID <= MAXWORD; ++wID)
					{
						if (NULL == FindResource(hInst, MAKEINTRESOURCE(wID), RT_ICON))
						{
							CopyMemory(lpGrp->idEntries + nIndex, pIcons + nIndex, 12);
							lpGrp->idEntries[nIndex].nID = wID;
							nIndex += 1;
							if (nIndex >= header.idCount)
							{
								break;
							}
						}
					}
					FreeLibrary(hInst);

					if (wID <= MAXWORD)
					{
						HANDLE hUpdate = BeginUpdateResource(lpExeName, FALSE);
						if (NULL != hUpdate)
						{
							bRet = UpdateResource(hUpdate, RT_GROUP_ICON, lpResName
								, MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT), lpGrp, dwGrpSize);
							
							for (int i(0); i < header.idCount; ++i)
							{
								LPBYTE lpIcoDat = new BYTE[pIcons[i].dwBytesInRes]();
								SetFilePointer(hFile, pIcons[i].dwImageOffset, NULL, FILE_BEGIN);
								ReadFile(hFile, lpIcoDat, pIcons[i].dwBytesInRes, &dwReaded, NULL);
								if (dwReaded >= pIcons[i].dwBytesInRes)
								{
									bRet = UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(lpGrp->idEntries[i].nID)
										, MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT), lpIcoDat, pIcons[i].dwBytesInRes);
								}
								delete []lpIcoDat;
								lpIcoDat = NULL;
							}
							EndUpdateResource(hUpdate, FALSE);
						}
					}
				}
				delete []lpGrp;
				lpGrp = NULL;
			}
			delete []pIcons;
			pIcons = NULL;
		}
		CloseHandle(hFile);
	}
	return bRet;
}

BOOL kxSaveIcon(LPCTSTR lpExeName, LPCTSTR lpResName, LPCTSTR lpIcoName)
{
	BOOL bRet(FALSE);

	DWORD dwWritten(0);

	HMODULE hInst = LoadLibrary(lpExeName);
	if (NULL != hInst)
	{
		HRSRC hRes = FindResource(hInst, lpResName, RT_GROUP_ICON);
		if (NULL != hRes)
		{
			HGLOBAL hGlobal = LoadResource(hInst, hRes);
			if (NULL != hGlobal)
			{
				LPVOID lpBuf = LockResource(hGlobal);
				if (NULL != lpBuf)
				{
					GRPICONDIR grpDir = {0};
					CopyMemory(&grpDir, lpBuf, sizeof(GRPICONDIR));
					
					DWORD dwGrpSize = sizeof(GRPICONDIR) + (grpDir.idCount - 1)*sizeof(GRPICONDIRENTRY);
					LPGRPICONDIR lpGrp = (LPGRPICONDIR)new BYTE[dwGrpSize]();
					CopyMemory(lpGrp, lpBuf, dwGrpSize);
					
					UnlockResource(hGlobal);
					
					ICONHEADER header = {grpDir.idReserved, grpDir.idType, grpDir.idCount};
					LPICONDIRENTRY lpIcons = new ICONDIRENTRY[header.idCount]();
					CopyMemory(lpIcons, lpGrp->idEntries, 12);
					lpIcons[0].dwImageOffset = sizeof(header) + header.idCount*sizeof(ICONDIRENTRY); 
					for (int i(1); i < header.idCount; ++i)
					{
						CopyMemory(lpIcons + i, lpGrp->idEntries + i, 12);
						lpIcons[i].dwImageOffset = lpIcons[i-1].dwImageOffset + lpIcons[i-1].dwBytesInRes;
					}
					
					HANDLE hFile = CreateFile(lpIcoName, GENERIC_WRITE, 0, NULL
						, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (INVALID_HANDLE_VALUE != hFile)
					{
						bRet = WriteFile(hFile, &header, sizeof(header), &dwWritten, NULL);
						bRet = WriteFile(hFile, lpIcons, sizeof(ICONDIRENTRY)*header.idCount, &dwWritten, NULL);
						
						for (int nIndex(0); nIndex < header.idCount; ++nIndex)
						{
							WORD wId = lpGrp->idEntries[nIndex].nID;
							HRSRC hRes(NULL);
							HGLOBAL hGlobal(NULL);
							LPVOID  lpBuf(NULL);
							if ((hRes = FindResource(hInst, MAKEINTRESOURCE(wId), RT_ICON)) != NULL &&
								(hGlobal = LoadResource(hInst, hRes)) != NULL &&
								(lpBuf = LockResource(hGlobal)) != NULL)
							{
								int nResSize = SizeofResource(hInst, hRes);
								bRet = WriteFile(hFile, lpBuf, nResSize, &dwWritten, NULL);
								UnlockResource(hGlobal);
							}
						}
						CloseHandle(hFile);
					}
					delete []lpIcons;
					lpIcons = NULL;
					delete []lpGrp;
					lpGrp = NULL;						
				}
			}
		}
		FreeLibrary(hInst);
	}
	return bRet;
}

BOOL kxExtractResource(HMODULE hModule, LPCTSTR lpResName, LPCTSTR lpResType, LPCTSTR lpOutFile)
{
	BOOL bRet(FALSE);

	HRSRC hRes = FindResource(hModule, lpResName, lpResType);
	if (NULL != hRes)
	{
		DWORD dwResSize = SizeofResource(hModule, hRes);
		HGLOBAL hGlobal = LoadResource(hModule, hRes);
		if (NULL != hGlobal)
		{
			LPVOID lpBuff = LockResource(hGlobal);
			HANDLE hFile = CreateFile(lpOutFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE != hFile)
			{
				DWORD dwSizeWri(0);
				bRet = WriteFile(hFile, lpBuff, dwResSize, &dwSizeWri, NULL);
				CloseHandle(hFile);
			}
			UnlockResource(hGlobal);
		}
	}
	return bRet;
}
