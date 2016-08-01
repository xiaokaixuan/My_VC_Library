#include "StdAfx.h"
#include <windows.h>
#include <shlwapi.h>
#include "kxExapi.h"
#pragma comment(lib, "shlwapi.lib")

UINT kxGetFileRows(LPCTSTR lpFileName)
{
	UINT uCnt(0);
	BOOL bCanCover(TRUE);
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hMap != INVALID_HANDLE_VALUE)
		{
			LPCTSTR lpStart = (LPCTSTR)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
			LPCTSTR lpEnd = lpStart + GetFileSize(hFile, NULL);
			for (LPCTSTR lpBuff(lpStart);lpBuff < lpEnd; ++lpBuff)
			{
				if (*lpBuff == _T('\n'))
				{
					bCanCover = TRUE;
				}
				else if (*lpBuff != _T('\r') && bCanCover)
				{
					uCnt += 1;
					bCanCover = FALSE;
				}
			}
			UnmapViewOfFile(lpStart);
			CloseHandle(hMap);
		}
		CloseHandle(hFile);
	}
	return uCnt;
}

BOOL kxCreateRankDirectory(LPCTSTR lpPath)
{
	BOOL bRet(FALSE);
	CString strPath(lpPath);
	strPath.Replace(_T('/'), _T('\\'));
	if (!PathFileExists(strPath))
	{
		CString strTemp;
		int pos(-1);
		while (-1 != (pos=strPath.Find('\\')))
		{
			strTemp += strPath.Left(pos + 1);
			strPath = strPath.Right(strPath.GetLength() - pos - 1);
			CreateDirectory(strTemp,NULL);
			
		}
		strTemp += strPath;
		CreateDirectory(strTemp,NULL);
		bRet = TRUE;
	}
	return bRet;
}

CString kxCreateUniqueDirectory(LPCTSTR lpPrefix)
{
	srand((unsigned int)time(NULL));
	int random = rand() % 9000 + 1000;
	CString strFile;
	strFile.Format(_T("%s%d"), lpPrefix, random);
	while (PathFileExists(strFile))
	{
		random = rand() % 9000 + 1000;
		strFile.Format(_T("%s%d"), lpPrefix, random);
	}
	kxCreateRankDirectory(strFile);
	return strFile;
}
