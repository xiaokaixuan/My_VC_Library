#include "StdAfx.h"
#include "AdobeFlashSink.h"


CAdobeFlashSink::CAdobeFlashSink()
: m_pConnectionPoint(0)
, m_dwCookie(0)
, m_nRefCount(0)
{

}

CAdobeFlashSink::~CAdobeFlashSink()
{

}

HRESULT CAdobeFlashSink::Init(ShockwaveFlashObjects::IShockwaveFlashPtr ptrFlash)
{
	HRESULT aResult = NOERROR;
	LPCONNECTIONPOINTCONTAINER aConnectionPoint = NULL;   
	if ((ptrFlash->QueryInterface(IID_IConnectionPointContainer, (void**)&aConnectionPoint) == S_OK) && (aConnectionPoint->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &m_pConnectionPoint) == S_OK))               
	{   
		IDispatch* aDispatch = NULL;   
		QueryInterface(__uuidof(IDispatch), (void**)&aDispatch);   
		if (aDispatch != NULL)   
		{   
			aResult = m_pConnectionPoint->Advise((LPUNKNOWN)aDispatch, &m_dwCookie);
			aDispatch->Release();   
		}   
	}   
	
	if (aConnectionPoint != NULL)
	{
		aConnectionPoint->Release();   
	}
	
	return aResult;   
}

HRESULT CAdobeFlashSink::Shutdown()
{   
    HRESULT aResult = S_OK;

    if (m_pConnectionPoint)
    {
		if (m_dwCookie)   
		{
			aResult = m_pConnectionPoint->Unadvise(m_dwCookie);
			m_dwCookie = 0;
		}

		m_pConnectionPoint->Release();
		m_pConnectionPoint = NULL;
    }
    return aResult;
}

HRESULT STDMETHODCALLTYPE CAdobeFlashSink::QueryInterface(REFIID riid, LPVOID* ppv)   
{   
    *ppv = NULL;   
	
    if (riid == IID_IUnknown)   
    {   
		*ppv = (LPUNKNOWN)this;   
		AddRef();   
		return S_OK;   
    }   
    else if (riid == IID_IDispatch)   
    {   
		*ppv = (IDispatch*)this;   
		AddRef();   
		return S_OK;   
    }   
    else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))   
    {   
		*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*)this;   
		AddRef();   
		return S_OK;   
    }   
    else   
    {      
		return E_NOTIMPL;   
    }   
}   

ULONG STDMETHODCALLTYPE CAdobeFlashSink::AddRef()   
{     
    return ++m_nRefCount;   
}   

ULONG STDMETHODCALLTYPE CAdobeFlashSink::Release()
{     
    int aRefCount = --m_nRefCount;   
    if (aRefCount == 0)
	{
		delete this;           
	}
	
    return aRefCount;   
}   
 
HRESULT STDMETHODCALLTYPE CAdobeFlashSink::GetTypeInfoCount(UINT* pctinfo)   
{   
    return E_NOTIMPL;   
}   

HRESULT STDMETHODCALLTYPE CAdobeFlashSink::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)   
{   
    return E_NOTIMPL;    
}   

HRESULT STDMETHODCALLTYPE CAdobeFlashSink::GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid,DISPID* rgDispId)   
{   
    return E_NOTIMPL;    
}   

HRESULT STDMETHODCALLTYPE CAdobeFlashSink::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{   
    switch(dispIdMember)   
    {   
    case 0x7a6:            
		break;   
    case 0x96:             
		if ((pDispParams->cArgs == 2) && (pDispParams->rgvarg[0].vt == VT_BSTR) && (pDispParams->rgvarg[1].vt == VT_BSTR))   
		{   
			FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
		}   
		break;   
    case DISPID_READYSTATECHANGE:                      
		break;   
    default:               
		return DISP_E_MEMBERNOTFOUND;
    }    
	
    return NOERROR;   
}   

HRESULT CAdobeFlashSink::OnReadyStateChange (long newState)   
{   
    return S_OK;   
}   

HRESULT CAdobeFlashSink::OnProgress(long percentDone)   
{   
    return S_OK;   
}   

HRESULT CAdobeFlashSink::FSCommand (_bstr_t command, _bstr_t args)
{
    return S_OK;   
}