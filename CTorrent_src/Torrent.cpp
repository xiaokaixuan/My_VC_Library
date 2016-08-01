// Torrent.cpp: implementation of the  class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <fstream>
#include "Torrent.h"
#include <WinCrypt.h>

CTorrent::CTorrent()
{
	m_lDataSize = 0;
	m_pcbData = 0;
	m_pRootDict = NULL;
}

CTorrent::~CTorrent()
{

}

void CTorrent::clear()
{
	m_BEncode.clear();
	delete []m_pcbData;
	m_lDataSize = 0;
	m_pcbData = 0;
	m_pRootDict = NULL;
}

bool CTorrent::isValid()
{		
	if(getValue("info")) return true;
	return false;
}

CNode* CTorrent::getValue(const char* szName)
{
	if (m_pRootDict)
	{
		return m_pRootDict->getValue(szName);
	}
	return NULL;
}

CBEncode* CTorrent::getBEncoder()
{
	return &m_BEncode;
}

bool CTorrent::parse(const char* szContent)
{
	if (!szContent) return false;

	long lLen = strlen(szContent);
	m_pcbData = new char[lLen + 1];
	memcpy(m_pcbData, szContent, lLen);
	m_pcbData[lLen] = 0;

	if (m_BEncode.parse(m_pcbData))
	{
		m_lDataSize = lLen;
		
		std::list<CNode*> listNode = m_BEncode.getNodes();
		std::list<CNode*>::iterator it = listNode.begin();
		for(; it != listNode.end(); ++it)
		{
			if((*it)->m_type == em_tpDict)
			{
				m_pRootDict	= (CNodeDict*)(*it);
				break;
			}
		}
		if (m_pRootDict && m_pRootDict->getValue("info"))
		{
			return true;
		}
	}

	clear();
	return false;
}

bool CTorrent::parseFile(const char* szFilename)
{
	if (!szFilename) return false;
	clear();

	std::ifstream input(szFilename, std::ios::binary);
	if (!input.is_open()) return false;

	input.seekg(0, std::ios::end);
	long lLen = input.tellg();
	input.seekg(0, std::ios::beg);

	m_pcbData = new char[lLen + 1];
	memset(m_pcbData, 0, lLen + 1);
	input.read(m_pcbData, lLen);
	input.close();

	if (m_BEncode.parse(m_pcbData))
	{
		m_lDataSize = lLen;

		std::list<CNode*> listNode = m_BEncode.getNodes();
		std::list<CNode*>::iterator it = listNode.begin();
		for(; it != listNode.end(); ++it)
		{
			if((*it)->m_type == em_tpDict)
			{
				m_pRootDict	= (CNodeDict*)(*it);
				break;
			}
		}
		if (m_pRootDict && m_pRootDict->getValue("info"))
		{
			return true;
		}
	}

	clear();
	return false;
}

// #define _WIN32_WINNT 0x0501
static BOOL GetHash(BYTE *pbData, DWORD dwDataLen, ALG_ID algId, BYTE* pszHash)
{
	BOOL bReturn(TRUE);
	HCRYPTPROV hProv;
	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return (bReturn = FALSE);
	
	HCRYPTHASH hHash;
	if(!CryptCreateHash(hProv, algId, 0, 0, &hHash))
	{
		CryptReleaseContext(hProv, 0);
		return (bReturn = FALSE);
	}
	
	if(!CryptHashData(hHash, pbData, dwDataLen, 0))
	{
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);
		return (bReturn = FALSE);
	}
	
	DWORD dwSize;
	DWORD dwLen = sizeof(dwSize);
	CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);
	
	BYTE* pHash = new BYTE[dwSize];
	dwLen = dwSize;
	CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);
	
	memcpy(pszHash, pHash, dwLen);
	delete [] pHash;
	
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return bReturn;
}

std::string CTorrent::getInfoHash()
{
	std::string strHash;
	CNode* pInfo = getValue("info");
	if (pInfo) {
		BYTE szSha1[21] = { 0 };
		if(GetHash((BYTE*)pInfo->m_szPos, pInfo->m_iSize, CALG_SHA1, szSha1))
		{
			char szHash[100] = { 0 };
			char* szTmp = szHash;
			for(int i = 0; i < 20; i++)
			{				
				sprintf(szTmp, "%02X", szSha1[i]);
				szTmp += 2;
			}
			strHash = szHash;
		}
	}
	return strHash;
}

bool CTorrent::saveFile(const char* szFileName)
{
	if (!szFileName) return false;
	if (m_pcbData && m_lDataSize > 0 && isValid())
	{
		std::ofstream output(szFileName, std::ios::binary);
		if (output.is_open())
		{
			output.write(m_pcbData, m_lDataSize);
			output.close();
			return true;
		}
	}
	return false;
}