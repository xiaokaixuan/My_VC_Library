#if !defined(AFX_FLASHWND_H__B5B52B6B_E0CA_4BE3_9237_CE8E30F84C90__INCLUDED_)
#define AFX_FLASHWND_H__B5B52B6B_E0CA_4BE3_9237_CE8E30F84C90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AdobeFlashSink.h"

#include <vector>

enum enumVT { VT_ERR = -1, VT_NUMBER, VT_STRING, VT_BOOLEAN };

struct tagValue
{
	enumVT	emType;
	CString strValue;
	
	const tagValue& operator=(int nValue)
	{
		emType = VT_NUMBER;
		strValue.Format(_T("%d"), nValue);
		return *this;
	}
	const tagValue& operator=(LPCTSTR pszValue)
	{
		emType = VT_STRING;
		strValue = pszValue;
		return *this;
	}
	const tagValue& operator=(bool bValue)
	{
		emType = VT_BOOLEAN;
		strValue = bValue? _T("true"):_T("false");
		return *this;
	}
};

struct tagPropertyItem
{
	tagValue value;
	CString	 strKey;
	
	tagPropertyItem(LPCTSTR pszKey)
	{
		strKey = pszKey;
	}
	tagPropertyItem(int nKey)
	{
		strKey.Format(_T("%d"), nKey);
	}
};

class CFlashArray: public std::vector<tagPropertyItem>
{
public:
	tagValue& operator[](int nIndex)
	{
		CString strKey;
		strKey.Format(_T("%d"), nIndex);
		for (iterator i = begin(); i != end(); ++i)
		{
			if (0 == i->strKey.CompareNoCase(strKey))
			{
				return i->value;
			}
		}
		tagPropertyItem Item(nIndex);
		Item.value.emType = VT_ERR;
		push_back(Item);
		return back().value;
	}
};

class CFlashObject: public std::vector<tagPropertyItem>
{
public:
	tagValue& operator[](LPCTSTR pszKey)
	{
		for (iterator i = begin(); i != end(); ++i)
		{
			if (0 == i->strKey.CompareNoCase(pszKey))
			{
				return i->value;
			}
		}
		tagPropertyItem Item(pszKey);
		Item.value.emType = VT_ERR;
		push_back(Item);
		return back().value;
	}
};

typedef std::vector< std::vector<tagPropertyItem> > CFlashObjectArray;

#define UNM_FSCOMMAND	(WM_APP + 101)	// FSCOMMSND notify Message

typedef struct tagNMFSCMD
{
	NMHDR	hdr;
	LPCTSTR command;
	LPCTSTR args;
} NMFSCMD, *LPNMFSCMD;

class CAdobeFlashWnd : public CWnd, public CAdobeFlashSink
{
public:
	CAdobeFlashWnd();
	virtual ~CAdobeFlashWnd();

public:
	HRESULT Play();
    HRESULT Stop();
	HRESULT LoadMovie(LPCTSTR pszUrl);

public:
	virtual HRESULT FSCommand(_bstr_t command, _bstr_t args);

public:
	CString CallFunction(LPCTSTR lpFuncName);
	CString CallFunction(LPCTSTR lpFuncName, int nNum);
	CString CallFunction(LPCTSTR lpFuncName, bool bParam);
	CString CallFunction(LPCTSTR lpFuncName, double dNum);
	CString CallFunction(LPCTSTR lpFuncName, LPCTSTR lpParam);
	CString CallFunction_Array(LPCTSTR lpFuncName, const CFlashArray& Array);
	CString CallFunction_Object(LPCTSTR lpFuncName, const CFlashObject& Object);
	CString CallFunction_Object_Array(LPCTSTR lpFuncName, const CFlashObjectArray& Array);

protected:
	ShockwaveFlashObjects::IShockwaveFlashPtr m_ptrIFlash;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FLASHWND_H__B5B52B6B_E0CA_4BE3_9237_CE8E30F84C90__INCLUDED_)

