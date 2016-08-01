// BEncode.h: interface for the CBEncode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BENCODE_H__4D0BB462_2AE0_45B3_8BE8_19D51B2DBB2E__INCLUDED_)
#define AFX_BENCODE_H__4D0BB462_2AE0_45B3_8BE8_19D51B2DBB2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include <list>
#include <map>
#include <string>

// ������
enum ErrorCode
{
	em_noerr = 0,
	em_errString,
	em_errInt,
	em_errList,
	em_errDict,
	em_errEnd,
	em_errUnknown
};

// �ڵ�����
enum NodeType
{
	em_tpNode = 0,
	em_tpString,
	em_tpInt,
	em_tpList,
	em_tpDict
};

// �ڵ�
class CNode
{
public:
	virtual ~CNode() { }
	CNode(NodeType type = em_tpNode)
		: m_type(type)
		, m_error(em_noerr)
		, m_iSize(0)
		, m_szPos(NULL) { }

public:
	void reset() { m_iSize = 0, m_szPos = NULL; m_error = em_noerr; }
	
public:
	int m_iSize;			// �����С
	char* m_szPos;			// ����λ��
	NodeType m_type;		// ��������
	ErrorCode m_error;		// ������
};

// �����ڵ�
class CNodeInt: public CNode
{
public:
	virtual ~CNodeInt() { }
	CNodeInt(): CNode(em_tpInt), m_iValue(-1) { }
	
public:
	int m_iValue;
};

// �ִ��ڵ�
class CNodeString: public CNode
{
public:
	CNodeString()
		: CNode(em_tpString)
		, m_iLen(0)
		, m_szData(NULL) { }
	virtual ~CNodeString() { }
	
public:	
	bool getString(std::string& strValue) const
	{
		if(m_error == em_noerr && m_szData)
		{
			strValue.assign(m_szData, m_iLen);
			return true;
		}
		return false;
	}
	
public:
	int m_iLen;
	char* m_szData;
};

// �б�ڵ�
class CNodeList: public CNode
{
public:
	CNodeList(): CNode(em_tpList) { }
	virtual ~CNodeList() { clear(); }

public:
	void clear()
	{			
		std::list<CNode *>::iterator it;
		for(it = m_listNode.begin(); it != m_listNode.end(); ++it) delete (*it);
		m_listNode.clear();
	}
	
public:	
	std::list<CNode*> m_listNode;
};

// �ֵ�ڵ�
class CNodeDict: public CNode
{
public:
	CNodeDict(): CNode(em_tpDict) { }
	virtual ~CNodeDict() { clear(); }

public:
	void clear()
	{
		// ָ��ӳ��
		m_mapNode.clear();
		std::list<CNode*>::iterator it;
		for(it = m_listNode.begin(); it != m_listNode.end(); ++it) delete (*it);
		m_listNode.clear();
	}
	CNode* getValue(const char* szName)
	{
		std::map<std::string, CNode*>::iterator it = m_mapNode.find(szName);
		if(it != m_mapNode.end()) return it->second;
		return NULL;
	}
	
public:
	std::list<CNode*> m_listNode;
	std::map<std::string, CNode*> m_mapNode;
};

// ����ڵ�
class CBEncode
{
public:
	virtual ~CBEncode() { clear(); }
	CBEncode(): m_lastNode(NULL), m_szText(NULL) { };
	
public:
	bool parse(char* szData);
	
public:
	bool readString(char* szPos, int& iSize, std::list<CNode*>& listNode);
	bool readInt(char* szPos, int& iSize, std::list<CNode*>& listNode);
	bool readList(char* szPos, int& iSize, std::list<CNode*>& listNode);
	bool readDict(char* szPos, int& iSize, std::list<CNode*>& listNode);

public:
	void clear()
	{	
		std::list<CNode*>::iterator it;
		for(it = m_listNode.begin(); it != m_listNode.end(); ++it) delete (*it);
		m_listNode.clear();
	}

public:
	const char* getContent() const { return m_szText; }
	std::list<CNode*>& getNodes() { return m_listNode; }

protected:
	CNode* m_lastNode;
	char* m_szText;
	std::list<CNode*> m_listNode;
};

#endif // !defined(AFX_BENCODE_H__4D0BB462_2AE0_45B3_8BE8_19D51B2DBB2E__INCLUDED_)

