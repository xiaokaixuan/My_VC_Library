#include "StdAfx.h"
#include <atlbase.h>
#include "AdobeFlashWnd.h"

#pragma comment(lib, "atl.lib")
#pragma comment(lib, "AdobeFlash15.lib")

extern "C" HRESULT WINAPI Flash_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv);

CAdobeFlashWnd::CAdobeFlashWnd()
: m_ptrIFlash(NULL)
{
	IClassFactoryPtr pClassFactory = NULL;
	HRESULT hr = Flash_DllGetClassObject(__uuidof(ShockwaveFlashObjects::ShockwaveFlash), IID_IClassFactory, (LPVOID*)&pClassFactory);
	if (S_OK == hr)
	{
		hr = pClassFactory->CreateInstance(NULL, __uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID*)&m_ptrIFlash);
		if (S_OK != hr)
		{
			m_ptrIFlash = NULL;
		}
	}
}

CAdobeFlashWnd::~CAdobeFlashWnd()
{

}

BEGIN_MESSAGE_MAP(CAdobeFlashWnd, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CAdobeFlashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_ptrIFlash)
	{
		Init(m_ptrIFlash);
		AtlAxAttachControl(m_ptrIFlash, GetSafeHwnd(), NULL);
	}

	return 0;
}

HRESULT CAdobeFlashWnd::LoadMovie(LPCTSTR pszUrl)
{
	HRESULT hr = S_FALSE;
	if (m_ptrIFlash)
	{
		hr = m_ptrIFlash->LoadMovie(0, pszUrl);
	}
	return hr;
}

HRESULT CAdobeFlashWnd::Play()
{
	HRESULT hr = S_FALSE;
	if (m_ptrIFlash)
	{
		hr = m_ptrIFlash->Play();
	}
	return hr;
}

HRESULT CAdobeFlashWnd::Stop()
{
	HRESULT hr = S_FALSE;
	if (m_ptrIFlash)
	{
		hr = m_ptrIFlash->Stop();
	}
	return hr;
}

HRESULT CAdobeFlashWnd::FSCommand(_bstr_t command, _bstr_t args)
{
	CWnd* pParent = GetParent();
	if (pParent)
	{
		NMFSCMD nmfscmd = {0};
		nmfscmd.hdr.code = UNM_FSCOMMAND;
		nmfscmd.hdr.idFrom = GetDlgCtrlID();
		nmfscmd.hdr.hwndFrom = GetSafeHwnd();
		nmfscmd.command = command;
		nmfscmd.args = args;
		pParent->SendMessage(WM_NOTIFY, WPARAM(nmfscmd.hdr.idFrom), LPARAM(&nmfscmd));
	}
	return S_OK;
}

CString CAdobeFlashWnd::CallFunction(LPCTSTR lpFuncName)
{
	ASSERT(lpFuncName);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"></invoke>"), lpFuncName);
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction(LPCTSTR lpFuncName, bool bParam)
{
	CString strBool(_T("<false/>"));
	if (bParam)
	{
		strBool = _T("<true/>");
	}
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments>%s</arguments></invoke>"), 
		lpFuncName, strBool);
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction(LPCTSTR lpFuncName, int nNum)
{
	ASSERT(lpFuncName);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><number>%d</number></arguments></invoke>"), 
		lpFuncName, nNum);
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction(LPCTSTR lpFuncName, double dNum)
{
	ASSERT(lpFuncName);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><number>%f</number></arguments></invoke>"), 
		lpFuncName, dNum);
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction(LPCTSTR lpFuncName, LPCTSTR lpParam)
{
	ASSERT(lpFuncName && lpParam);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><string>%s</string></arguments></invoke>"), 
		lpFuncName, lpParam);
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction_Array(LPCTSTR lpFuncName, const CFlashArray& Array)
{
	ASSERT(lpFuncName);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><array>"), lpFuncName);
	for (CFlashArray::const_iterator iter = Array.begin(); iter != Array.end(); ++iter)
	{
		const tagPropertyItem& Item = *iter;
		CString strProperty;
		strProperty.Format(_T("<property id=\"%s\">"), Item.strKey);
		if (Item.value.emType == VT_NUMBER)
		{
			CString strVal;
			strVal.Format(_T("<number>%s</number></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else if (Item.value.emType == VT_STRING)
		{
			CString strVal;
			strVal.Format(_T("<string>%s</string></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else if (Item.value.emType == VT_BOOLEAN)
		{
			CString strVal;
			strVal.Format(_T("<%s/></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else
		{
			strProperty.Empty();
		}
		strRequest += strProperty;
	}
	strRequest += _T("</array></arguments></invoke>");
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction_Object(LPCTSTR lpFuncName, const CFlashObject& Object)
{
	ASSERT(lpFuncName);
	CString strRequest, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><object>"), lpFuncName);
	for (CFlashObject::const_iterator iter = Object.begin(); iter != Object.end(); ++iter)
	{
		const tagPropertyItem& Item = *iter;
		CString strProperty;
		strProperty.Format(_T("<property id=\"%s\">"), Item.strKey);
		if (Item.value.emType == VT_NUMBER)
		{
			CString strVal;
			strVal.Format(_T("<number>%s</number></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else if (Item.value.emType == VT_STRING)
		{
			CString strVal;
			strVal.Format(_T("<string>%s</string></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else if (Item.value.emType == VT_BOOLEAN)
		{
			CString strVal;
			strVal.Format(_T("<%s/></property>"), Item.value.strValue);
			strProperty += strVal;
		}
		else
		{
			strProperty.Empty();
		}
		strRequest += strProperty;
	}
	strRequest += _T("</object></arguments></invoke>");
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}

CString CAdobeFlashWnd::CallFunction_Object_Array(LPCTSTR lpFuncName, const CFlashObjectArray& Array)
{
	ASSERT(lpFuncName);
	CString strRequest, strObject, strReturn;
	strRequest.Format(_T("<invoke name=\"%s\" returntype=\"xml\"><arguments><array>"), lpFuncName);
	for(CFlashObjectArray::const_iterator iterOA = Array.begin(); iterOA != Array.end(); ++iterOA)
	{
		strObject.Format(_T("<property id=\"%d\"><object>"), iterOA - Array.begin());
		for (std::vector<tagPropertyItem>::const_iterator iter = iterOA->begin(); iter != iterOA->end(); ++iter)
		{
			const tagPropertyItem& Item = *iter;
			CString strProperty;
			strProperty.Format(_T("<property id=\"%s\">"), Item.strKey);
			if (Item.value.emType == VT_NUMBER)
			{
				CString strVal;
				strVal.Format(_T("<number>%s</number></property>"), Item.value.strValue);
				strProperty += strVal;
			}
			else if (Item.value.emType == VT_STRING)
			{
				CString strVal;
				strVal.Format(_T("<string>%s</string></property>"), Item.value.strValue);
				strProperty += strVal;
			}
			else if (Item.value.emType == VT_BOOLEAN)
			{
				CString strVal;
				strVal.Format(_T("<%s/></property>"), Item.value.strValue);
				strProperty += strVal;
			}
			else
			{
				strProperty.Empty();
			}
			strObject += strProperty;
		}
		strObject  += _T("</object></property>");
		strRequest += strObject;
	}
	strRequest += _T("</array></arguments></invoke>");
	try
	{
		strReturn = (LPCTSTR)m_ptrIFlash->CallFunction(LPCTSTR(strRequest));
	}
	catch (...)
	{
	}
	return strReturn;
}
