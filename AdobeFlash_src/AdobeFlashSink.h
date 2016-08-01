// FlashSink.h: interface for the CFlashSink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLASHSINK_H__BB878CA5_6744_4BFB_927A_9AFAF6032C44__INCLUDED_)
#define AFX_FLASHSINK_H__BB878CA5_6744_4BFB_927A_9AFAF6032C44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#import "AdobeFlash15.dll"

class CAdobeFlashSink: public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:
	CAdobeFlashSink();
	virtual ~CAdobeFlashSink();

protected:
	LPCONNECTIONPOINT	m_pConnectionPoint;     
	DWORD				m_dwCookie;   
	int					m_nRefCount;

public:
	HRESULT Shutdown();
	HRESULT Init(ShockwaveFlashObjects::IShockwaveFlashPtr ptrFlash);

public:
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv);

public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

public:
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo);
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	
	virtual HRESULT OnReadyStateChange (long newState);
	virtual HRESULT OnProgress(long percentDone);
	virtual HRESULT FSCommand(_bstr_t command, _bstr_t args);
};

#endif // !defined(AFX_FLASHSINK_H__BB878CA5_6744_4BFB_927A_9AFAF6032C44__INCLUDED_)
