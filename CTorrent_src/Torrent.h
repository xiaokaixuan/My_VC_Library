// Torrent.h: interface for the CTorrent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TORRENT_H__7E67DA03_B65C_427F_A241_24230BCD6D54__INCLUDED_)
#define AFX_TORRENT_H__7E67DA03_B65C_427F_A241_24230BCD6D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BEncode.h"


class CTorrent
{
public:
	CTorrent();
	virtual ~CTorrent();

public:
	void clear();

public:
	bool parse(const char* szContent);
	bool parseFile(const char* szFilename);
	bool saveFile(const char* szFileName);

public:
	bool isValid();
	std::string getInfoHash();
	CBEncode* getBEncoder();
	CNode* getValue(const char* szName);

protected:
	long	 m_lDataSize;
	char*	 m_pcbData;
	CBEncode m_BEncode;
	CNodeDict* m_pRootDict;
};

#endif // !defined(AFX_TORRENT_H__7E67DA03_B65C_427F_A241_24230BCD6D54__INCLUDED_)

