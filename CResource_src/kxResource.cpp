#include "StdAfx.h"
#include "kxResource.h"


// Constructor/destructor
CResource::CResource(HMODULE hModule)
: m_hModule(hModule)
, m_hResource(NULL)
, m_hGlobal(NULL)
{

}

CResource::~CResource()
{
	Release();
}

// Load methods
BOOL CResource::Load(LPCTSTR lpType, LPCTSTR lpName)
{
	ASSERT(m_hModule != NULL);
	ASSERT(m_hResource == NULL);
	ASSERT(m_hGlobal == NULL);
	
	m_hResource = ::FindResource(m_hModule, lpName, lpType);
	if(m_hResource == NULL)
		return FALSE;
	
	m_hGlobal = ::LoadResource(m_hModule, m_hResource);
	if(m_hGlobal == NULL)
	{
		m_hResource = NULL;
		return FALSE;
	}
	
	return TRUE;
}

BOOL CResource::LoadEx(LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage)
{
	ASSERT(m_hModule != NULL);
	ASSERT(m_hResource == NULL);
	ASSERT(m_hGlobal == NULL);
	
	m_hResource = ::FindResourceEx(m_hModule, lpType, lpName, wLanguage);
	if(m_hResource == NULL)
		return FALSE;
	
	m_hGlobal = ::LoadResource(m_hModule, m_hResource);
	if(m_hGlobal == NULL)
	{
		m_hResource = NULL;
		return FALSE;
	}
	
	return TRUE;
}

// Misc operations
HMODULE CResource::GetModuleHandle() const
{
	return m_hModule;
}

DWORD CResource::GetSize() const
{
	ASSERT(m_hResource != NULL);
	return ::SizeofResource(m_hModule, m_hResource);
}

LPVOID CResource::Lock()
{
	ASSERT(m_hModule != NULL);
	ASSERT(m_hResource != NULL);
	ASSERT(m_hGlobal != NULL);

	LPVOID pVoid = ::LockResource(m_hGlobal);
	ASSERT(pVoid != NULL);
	return pVoid;
}

void CResource::Release()
{
	if(m_hGlobal != NULL)
	{
		FreeResource(m_hGlobal);
		m_hGlobal = NULL;
		m_hResource = NULL;
	}
}

BOOL CResource::Save(LPCTSTR lpFileName)
{
	ASSERT(m_hModule != NULL);
	ASSERT(m_hResource != NULL);
	ASSERT(m_hGlobal != NULL);

	BOOL bRet(FALSE);
	LPVOID lpBuff = Lock();
	if (NULL != lpBuff)
	{
		HANDLE hFile = ::CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE != hFile)
		{
			DWORD dwSucceed(0);
			if (::WriteFile(hFile, lpBuff, GetSize(), &dwSucceed, NULL))
			{
				bRet = TRUE;
			}
			CloseHandle(hFile);
		}
	}
	return bRet;
}

BOOL CResource::ExtractResource(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpFileName)
{
	BOOL bRet(FALSE);
	CResource resource(hModule);
	if (resource.Load(lpType, lpName))
	{
		if (resource.Save(lpFileName))
		{
			bRet = TRUE;
			resource.Release();
		}
	}
	return bRet;
}

BOOL CResource::ExtractResourceEx(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage, LPCTSTR lpFileName)
{
	BOOL bRet(FALSE);
	CResource resource(hModule);
	if (resource.LoadEx(lpType, lpName, wLanguage))
	{
		if (resource.Save(lpFileName))
		{
			bRet = TRUE;
			resource.Release();
		}
	}
	return bRet;
}
