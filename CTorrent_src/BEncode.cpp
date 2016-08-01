// BEncode.cpp: implementation of the CBEncode class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BEncode.h"


// 循环解析
bool CBEncode::parse(char* szData)
{
	if (!szData || !(*szData)) return false;
	
	clear();
	char* szPos = m_szText = szData;
	
	int iSize = 0;
	while (*szPos)
	{	
		if (*szPos == 'i')
		{
			if(!readInt(szPos, iSize, m_listNode)) break;
			szPos += iSize;
		}
		else if (*szPos == 'l')
		{
			if (!readList(szPos, iSize, m_listNode)) break;
			szPos += iSize;
		}
		else if (*szPos == 'd')
		{
			if (!readDict(szPos, iSize, m_listNode)) break;
			szPos += iSize;
		}
		else			
		{
			if (!readString(szPos, iSize, m_listNode)) break;
			szPos += iSize;
		}
	}
	if (!(*szPos) && m_lastNode->m_error == em_noerr) return true;
	return false;
}

// 读取一个字符串
bool CBEncode::readString(char* szPos, int& iSize, std::list<CNode*>& listNode)
{	
	char* szTmp = szPos;
	CNodeString* pNodeString = new CNodeString;
	pNodeString->m_szPos = szPos;
	
	char szLen[20] = { 0 };
	int i = 0;
	while (*szTmp >='0' && *szTmp <= '9') szLen[i++] = *(szTmp++);
	if (*szTmp == ':')
	{
		int ilen = atoi(szLen);
		if (ilen >= 0)
		{	
			pNodeString->m_szData = ++szTmp;
			pNodeString->m_iLen = ilen;
			szTmp += ilen;
		}
		else pNodeString->m_error = em_errString;
	}
	else pNodeString->m_error = em_errString;
	
	pNodeString->m_iSize = iSize = szTmp - szPos;
	m_lastNode = pNodeString;
	listNode.push_back(pNodeString);
	
	return pNodeString->m_error == em_noerr;
}

// 读取一个整数
bool CBEncode::readInt(char* szPos, int& iSize, std::list<CNode*>& listNode)
{
	const char* szTmp = szPos;
	CNodeInt* pNodeInt = new CNodeInt;
	pNodeInt->m_szPos = szPos;
	
	if(*szTmp == 'i')
	{	
		++szTmp;
		char szLen[20] = { 0 };
		int i = 0;
		if (*szTmp == '-') szLen[i++] = *szTmp++;
		while (*szTmp >= '0' && *szTmp <= '9') szLen[i++] = *(szTmp++);
		if (*szTmp == 'e')
		{
			pNodeInt->m_iValue = atoi(szLen);
			++szTmp;
		}
		else pNodeInt->m_error = em_errInt;
	}
	else pNodeInt->m_error = em_errInt;
	
	pNodeInt->m_iSize = iSize = szTmp - szPos;
	m_lastNode = pNodeInt;
	listNode.push_back(pNodeInt);
	
	return pNodeInt->m_error == em_noerr;
}

// 读取一个列表
bool CBEncode::readList(char* szPos, int& iSize, std::list<CNode*>& listNode)
{
	char* szTmp = szPos;
	CNodeList* pNodeList = new CNodeList;
	pNodeList->m_szPos = szPos;
	
	if (*szTmp == 'l')
	{
		++szTmp;
		int iiSize = 0;
		while (*szTmp != 'e')
		{
			if (*szTmp == 'i')
			{
				if (!readInt(szTmp, iiSize, pNodeList->m_listNode)) break;
				szTmp += iiSize;
			}
			else if (*szTmp == 'l')
			{
				if (!readList(szTmp, iiSize, pNodeList->m_listNode)) break;
				szTmp += iiSize;
			}
			else if (*szTmp == 'd')
			{
				if(!readDict(szTmp, iiSize, pNodeList->m_listNode)) break;
				szTmp += iiSize;
			}
			else			
			{
				if (!readString(szTmp, iiSize, pNodeList->m_listNode)) break;
				szTmp += iiSize;
			}
		}	
		if (*szTmp != 'e' || m_lastNode->m_error != em_noerr)
			pNodeList->m_error = em_errList;
		else ++szTmp;
	}
	else pNodeList->m_error = em_errList;
	
	pNodeList->m_iSize = iSize = szTmp - szPos;
	m_lastNode = pNodeList;
	listNode.push_back(pNodeList);
	
	return pNodeList->m_error == em_noerr;
}

// 读取一个字典
bool CBEncode::readDict(char* szPos, int& iSize, std::list<CNode*>& listNode)
{
	char* szTmp = szPos;
	CNodeDict* pNodeDict= new CNodeDict;
	pNodeDict->m_szPos = szPos;
	
	if(*szTmp == 'd')
	{		
		++szTmp;
		int iiSize = 0;
		while (*szTmp != 'e')
		{
			if(!readString(szTmp, iiSize, pNodeDict->m_listNode)) break;
			szTmp += iiSize;
			if(m_lastNode->m_type != em_tpString) break;
			
			std::string strName(((CNodeString*)m_lastNode)->m_szData,((CNodeString*)m_lastNode)->m_iLen);
			
			if (*szTmp== 'i')
			{
				if (!readInt(szTmp, iiSize, pNodeDict->m_listNode)) break;
				szTmp += iiSize;
			}
			else if(*szTmp== 'l')
			{
				if (!readList(szTmp, iiSize, pNodeDict->m_listNode)) break;
				szTmp += iiSize;
			}
			else if(*szTmp== 'd')
			{
				if (!readDict(szTmp, iiSize, pNodeDict->m_listNode)) break;
				szTmp += iiSize;
			}
			else
			{
				if (!readString(szTmp, iiSize, pNodeDict->m_listNode)) break;
				szTmp += iiSize;
			}
			pNodeDict->m_mapNode.insert(std::pair<std::string, CNode*>(strName, m_lastNode));
		}
		if (*szTmp != 'e' || m_lastNode->m_error != em_noerr)
			pNodeDict->m_error = em_errDict;
		else ++szTmp;
	}
	else pNodeDict->m_error = em_errDict;
	
	pNodeDict->m_iSize = iSize = szTmp - szPos;
	m_lastNode = pNodeDict;
	listNode.push_back(pNodeDict);
	
	return pNodeDict->m_error == em_noerr;
}

