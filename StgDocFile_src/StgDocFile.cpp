#include "StdAfx.h"
#include <vector>
#include <Shlwapi.h>
#include "StgDocFile.h"

#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif // ASSERT

#pragma comment(lib, "Shlwapi.lib")

CStgDocFile::CStgDocFile(LPCTSTR lpszDocFile, DWORD grfMode)
: m_bValid(FALSE)
{
	CoInitialize(NULL);
	Open(lpszDocFile, grfMode);
}

CStgDocFile::~CStgDocFile()
{
	Close();
	CoUninitialize();
}

IStorage* CStgDocFile::OpenStorage(LPCTSTR pszStgPathName, DWORD grfMode)
{
	ASSERT(m_bValid);
	IStorage* pStgReturn = NULL;

	WCHAR wszStgPath[MAX_PATH] = {0};
#if defined(UNICODE) || defined(_UNICODE)
	lstrcpyn(wszStgPath, pszStgPathName, MAX_PATH);
#else
	mbstowcs(wszStgPath, pszStgPathName, MAX_PATH);
#endif

	grfMode |= STGM_SHARE_EXCLUSIVE;

	std::vector<LPWSTR> vecStgName;
	LPWSTR pToken = wcstok(wszStgPath, L"\\");
	while (pToken)
	{
		vecStgName.push_back(pToken);
		pToken = wcstok(NULL, L"\\");
	}

	size_t nLen = vecStgName.size();
	if (nLen > 0)
	{
		std::stack<IStorage*> stkFather;
		IStorage* pStgCurrent = m_pRootStg;
		for (size_t i(0); i < nLen; ++i)
		{
			IStorage *pResult = NULL;
			HRESULT hr = pStgCurrent->OpenStorage(vecStgName[i], NULL, grfMode, 0, 0, &pResult);
			if (STG_E_FILENOTFOUND == hr)
			{
				hr = pStgCurrent->CreateStorage(vecStgName[i], grfMode, 0, 0, &pResult);
			}
			if (S_OK == hr)
			{
				if (i < nLen - 1)
				{
					stkFather.push(pResult);
					pStgCurrent = pResult;
				}
				else
				{
					pStgReturn = pResult;
				}
			}
		}
		if (pStgReturn && !stkFather.empty())
		{
			m_mapStgFather[pStgReturn] = stkFather;
		}
	}

	return pStgReturn;
}

void CStgDocFile::CloseStorage(IStorage* pStorage)
{
	ASSERT(pStorage);
	pStorage->Release();

	std::map< IStorage*, std::stack<IStorage*> >::iterator iFind = m_mapStgFather.find(pStorage);
	if (iFind != m_mapStgFather.end())
	{
		std::stack<IStorage*>& stkStg = m_mapStgFather[pStorage];
		while (!stkStg.empty())
		{
			stkStg.top()->Release();
			stkStg.pop();
		}
		m_mapStgFather.erase(pStorage);
	}
}

IStream* CStgDocFile::OpenStream(LPCTSTR  pszStmPathName, DWORD grfMode)
{
	ASSERT(m_bValid);
	IStream* pStmReturn = NULL;

	WCHAR wszStmPath[MAX_PATH] = {0};
#if defined(UNICODE) || defined(_UNICODE)
	lstrcpyn(wszStmPath, pszStmPathName, MAX_PATH);
#else
	mbstowcs(wszStmPath, pszStmPathName, MAX_PATH);
#endif

	grfMode |= STGM_SHARE_EXCLUSIVE;

	std::vector<LPWSTR> vecStgName;
	LPWSTR pToken = wcstok(wszStmPath, L"\\");
	while (pToken)
	{
		vecStgName.push_back(pToken);
		pToken = wcstok(NULL, L"\\");
	}

	size_t nLen = vecStgName.size();
	if (nLen > 0)
	{
		std::stack<IStorage*> stkFather;
		IStorage* pStgCurrent = m_pRootStg;
		for (size_t i(0); i < nLen; ++i)
		{
			if (i < nLen - 1)
			{
				IStorage *pResult = NULL;
				HRESULT hr = pStgCurrent->OpenStorage(vecStgName[i], NULL, grfMode, 0, 0, &pResult);
				if (STG_E_FILENOTFOUND == hr)
				{
					hr = pStgCurrent->CreateStorage(vecStgName[i], grfMode, 0, 0, &pResult);
				}
				if (S_OK == hr)
				{
					stkFather.push(pResult);
					pStgCurrent = pResult;
				}
			}
			else
			{
				IStream *pResult = NULL;
				HRESULT hr = pStgCurrent->OpenStream(vecStgName[i], NULL, grfMode, 0, &pResult);
				if (STG_E_FILENOTFOUND == hr)
				{
					hr = pStgCurrent->CreateStream(vecStgName[i], grfMode, 0, 0, &pResult);
				}
				if (S_OK == hr)
				{
					pStmReturn = pResult;
				}
			}
		}
		if (pStmReturn && !stkFather.empty())
		{
			m_mapStmFather[pStmReturn] = stkFather;
		}
	}

	return pStmReturn;
}

void CStgDocFile::CloseStream(IStream* pStream)
{
	ASSERT(pStream);
	pStream->Release();

	std::map< IStream*, std::stack<IStorage*> >::iterator iFind = m_mapStmFather.find(pStream);
	if (iFind != m_mapStmFather.end())
	{
		std::stack<IStorage*>& stkStg = m_mapStmFather[pStream];
		while (!stkStg.empty())
		{
			stkStg.top()->Release();
			stkStg.pop();
		}
		m_mapStmFather.erase(pStream);
	}
}

BOOL CStgDocFile::Open(LPCTSTR lpszDocFile, DWORD grfMode)
{
	lstrcpyn(m_szDocFile, lpszDocFile, MAX_PATH);

	WCHAR wszDocFile[MAX_PATH] = {0};
#if defined(UNICODE) || defined(_UNICODE)
	lstrcpyn(wszDocFile, lpszDocFile, MAX_PATH);
#else
	mbstowcs(wszDocFile, lpszDocFile, MAX_PATH);
#endif
	
	grfMode |= STGM_SHARE_EXCLUSIVE;

	HRESULT hResult = S_FALSE;
	if (grfMode & STGM_CREATE)
	{
		hResult = StgCreateDocfile(wszDocFile, grfMode, 0, &m_pRootStg);
	}
	else
	{
		hResult = StgOpenStorage(wszDocFile, NULL, grfMode, 0, 0, &m_pRootStg);
	}
	m_bValid = (S_OK == hResult);
	return m_bValid;
}

void CStgDocFile::Close()
{
	if (m_bValid)
	{
		std::map< IStorage*, std::stack<IStorage*> >::iterator iStg(m_mapStgFather.begin());
		for (; iStg != m_mapStgFather.end(); ++iStg)
		{
			while (!iStg->second.empty())
			{
				iStg->second.top()->Release();
				iStg->second.pop();
			}
		}
		std::map< IStream*, std::stack<IStorage*> >::iterator iStm(m_mapStmFather.begin());
		for (; iStm != m_mapStmFather.end(); ++iStm)
		{
			while (!iStm->second.empty())
			{
				iStm->second.top()->Release();
				iStm->second.pop();
			}
		}

		m_pRootStg->Release();
		m_pRootStg = NULL;
		m_bValid = FALSE;
		ZeroMemory(m_szDocFile, sizeof(m_szDocFile));
	}
}
