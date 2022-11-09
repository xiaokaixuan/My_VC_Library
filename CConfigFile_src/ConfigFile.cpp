#include "StdAfx.h"
#include <tchar.h>
#include <Shlwapi.h>
#include "ConfigFile.h"

#pragma comment(lib, "Shlwapi.lib")


#define _KEY_SIZE	4096
#define _SEC_SIZE	(4*1024*1024)

inline LPCTSTR c_str(const _tstring &str)
{
#if !defined(_AFX)
	return str.c_str();
#else
	return LPCTSTR(str);
#endif
}

inline DWORD length(const _tstring &str)
{
#if !defined(_AFX)
	return DWORD(str.length());
#else
	return DWORD(str.GetLength());
#endif
}

CConfigFile::CConfigFile(LPCTSTR lpFileName/* = NULL*/)
{
	TCHAR szFileName[MAX_PATH] = { 0 };
	if (lpFileName)
	{
		::GetFullPathName(lpFileName, MAX_PATH, szFileName, NULL);
	}
	else
	{
		::GetModuleFileName(NULL, szFileName, MAX_PATH);
		::PathRenameExtension(szFileName, _T(".cfg"));
	}
	m_strIni = szFileName;
}

CConfigFile::~CConfigFile()
{

}

BOOL CConfigFile::GetSection(LPCTSTR lpAppName, MapStringToString& mapStrToStr)
{
	size_t nMapSize = mapStrToStr.size();
	DWORD dwLen = _SEC_SIZE;
	LPTSTR pString = new TCHAR[dwLen];
	ZeroMemory(pString, dwLen * sizeof(TCHAR));
	DWORD dwRet = ::GetPrivateProfileSection(lpAppName, pString, dwLen, c_str(m_strIni));
	for (LPTSTR pStr(pString); pStr < pString + dwRet; ++pStr)
	{
		DWORD dwLength = ::lstrlen(pStr);

		LPTSTR pChar = _tcschr(pStr, _T('='));
		if (pChar)
		{
			int nMaxLen = int(pChar - pStr) + 1;
			TCHAR szKey[_KEY_SIZE] = { 0 };
			TCHAR szVal[_KEY_SIZE] = { 0 };
			::lstrcpyn(szKey, pStr, nMaxLen > _KEY_SIZE ? _KEY_SIZE : nMaxLen);
			pChar += 1;
			::lstrcpyn(szVal, pChar, _KEY_SIZE);
			::StrTrim(szKey, _T(" \t\n"));
			::StrTrim(szVal, _T(" \t\n"));
			mapStrToStr[szKey] = szVal;
		}
		pStr += dwLength;
	}
	delete[] pString;
	return mapStrToStr.size() > nMapSize;
}

int CConfigFile::GetKeyInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nDefault)
{
	return (int)::GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, c_str(m_strIni));
}

float CConfigFile::GetKeyFloat(LPCTSTR lpAppName, LPCTSTR lpKeyName, float fDefault)
{
	float fRet(fDefault);
	TCHAR szStr[_KEY_SIZE] = { 0 };
	::GetPrivateProfileString(lpAppName, lpKeyName, _T(""), szStr, _KEY_SIZE, c_str(m_strIni));
	if (_stscanf(szStr, _T("%f"), &fRet) < 1)
	{
		fRet = fDefault;
	}
	return fRet;
}

DWORD CConfigFile::GetKeyString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, _tstring& strVal)
{
	DWORD dwRet(FALSE);
	TCHAR szString[_KEY_SIZE] = { 0 };
	dwRet = ::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, szString, _KEY_SIZE, c_str(m_strIni));
	if (dwRet > 0)
	{
		strVal = szString;
	}
	return dwRet;
}

DWORD CConfigFile::GetKeyString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR szRetString, DWORD dwSize)
{
	return ::GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, szRetString, dwSize, c_str(m_strIni));
}

BOOL CConfigFile::WriteSection(LPCTSTR lpAppName, MapStringToString& mapStrToStr)
{
	BOOL bRet(FALSE);
	DWORD dwSize(0);
	for (MapStringToString::iterator i = mapStrToStr.begin(); i != mapStrToStr.end(); ++i)
	{
		dwSize += length(i->first) + length(i->second) + 2; // '=' and '\0'
	}
	if (dwSize > 0)
	{
		dwSize += 1; // "\0\0" end

		LPTSTR szData = new TCHAR[dwSize];
		ZeroMemory(szData, dwSize * sizeof(TCHAR));

		LPTSTR szStr(szData);
		for (MapStringToString::iterator iter = mapStrToStr.begin(); iter != mapStrToStr.end(); ++iter)
		{
			DWORD dwLen = length(iter->first) + length(iter->second) + 2;
			_tstring str(iter->first);
			str += _T("=");
			str += iter->second;
			::lstrcpyn(szStr, c_str(str), dwLen);
			szStr += dwLen;
		}
		bRet = ::WritePrivateProfileSection(lpAppName, szData, c_str(m_strIni));
		delete[] szData;
	}
	return bRet;
}

BOOL CConfigFile::WriteKeyInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nNumber)
{
	TCHAR szNumber[32] = { 0 };
	_sntprintf(szNumber, 32, _T("%d"), nNumber);
	return ::WritePrivateProfileString(lpAppName, lpKeyName, szNumber, c_str(m_strIni));
}

BOOL CConfigFile::WriteKeyFloat(LPCTSTR lpAppName, LPCTSTR lpKeyName, float fNumber)
{
	TCHAR szNumber[256] = { 0 };
	_sntprintf(szNumber, 256, _T("%f"), fNumber);
	return ::WritePrivateProfileString(lpAppName, lpKeyName, szNumber, c_str(m_strIni));
}

BOOL CConfigFile::WriteKeyString(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString)
{
	return ::WritePrivateProfileString(lpAppName, lpKeyName, lpString, c_str(m_strIni));
}

BOOL CConfigFile::GetKeyStruct(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSize)
{
	return ::GetPrivateProfileStruct(lpAppName, lpKeyName, lpStruct, uSize, c_str(m_strIni));
}

BOOL CConfigFile::WriteKeyStruct(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPVOID lpStruct, UINT uSize)
{
	return ::WritePrivateProfileStruct(lpAppName, lpKeyName, lpStruct, uSize, c_str(m_strIni));
}

BOOL CConfigFile::GetStringArray(LPCTSTR lpAppName, StringArray& arrayString)
{
	size_t nLen = arrayString.size();
	DWORD dwLen = _SEC_SIZE;
	LPTSTR pString = new TCHAR[dwLen];
	ZeroMemory(pString, dwLen * sizeof(TCHAR));
	DWORD dwRet = ::GetPrivateProfileSection(lpAppName, pString, dwLen, c_str(m_strIni));
	for (LPTSTR pStr(pString); pStr < pString + dwRet; ++pStr)
	{
		DWORD dwLength = ::lstrlen(pStr);
		TCHAR szLine[_KEY_SIZE] = { 0 };
		::lstrcpyn(szLine, pStr, _KEY_SIZE);
		pStr += dwLength;
		arrayString.push_back(szLine);
	}
	delete[] pString;
	return arrayString.size() > nLen;
}

BOOL CConfigFile::WriteStringArray(LPCTSTR lpAppName, StringArray& arrayString)
{
	BOOL bRet(FALSE);
	DWORD dwSize(0);
	for (StringArray::iterator i(arrayString.begin()); i != arrayString.end(); ++i)
	{
		dwSize += length(*i) + 1;
	}
	if (dwSize > 0)
	{
		dwSize += 1;
		
		LPTSTR szData = new TCHAR[dwSize];
		ZeroMemory(szData, dwSize * sizeof(TCHAR));
		
		LPTSTR szStr(szData);
		for (StringArray::iterator iter(arrayString.begin()); iter != arrayString.end(); ++iter)
		{
			DWORD dwLen = length(*iter) + 1;
			::lstrcpyn(szStr, c_str(*iter), dwLen);
			szStr += dwLen;
		}
		bRet = ::WritePrivateProfileSection(lpAppName, szData, c_str(m_strIni));
		delete[] szData;
	}
	return bRet;
}

