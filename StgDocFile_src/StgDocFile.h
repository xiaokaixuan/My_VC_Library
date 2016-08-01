#if !defined(_8D4FF8CF_B43F_4e98_A282_8E12B63F7837_STGDOCFILE_H__)
#define _8D4FF8CF_B43F_4e98_A282_8E12B63F7837_STGDOCFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)
#pragma warning(disable: 4996)

#include <map>
#include <stack>

class CStgDocFile
{
public:
	CStgDocFile(LPCTSTR lpszDocFile, DWORD grfMode);
	virtual ~CStgDocFile();

public:
	BOOL IsValid() const { return m_bValid; }

public:
	BOOL Open(LPCTSTR lpszDocFile, DWORD grfMode);
	void Close();

public:
	IStorage* OpenStorage(LPCTSTR pszStgPathName, DWORD grfMode);
	void      CloseStorage(IStorage* pStorage);

	IStream*  OpenStream(LPCTSTR  pszStmPathName, DWORD grfMode);
	void      CloseStream(IStream* pStream);

protected:
	BOOL      m_bValid;
	IStorage* m_pRootStg;
	TCHAR     m_szDocFile[MAX_PATH];
private:
	std::map< IStorage*, std::stack<IStorage*> > m_mapStgFather;
	std::map< IStream* , std::stack<IStorage*> > m_mapStmFather;
};

#endif // !defined(_8D4FF8CF_B43F_4e98_A282_8E12B63F7837_STGDOCFILE_H__)


