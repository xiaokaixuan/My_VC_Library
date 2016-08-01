#include "StdAfx.h"
#include <vector>
#include <Wbemidl.h>
#include "WMIEngine.h"

#pragma comment(lib, "wbemuuid.lib")

CWMIEngine::CWMIEngine()
: m_bValid(FALSE)
, m_pLocator(NULL)
, m_pServices(NULL)
, m_pEnumerator(NULL)
, m_pClassObject(NULL)
{

}

CWMIEngine::~CWMIEngine()
{
	if (m_pClassObject)
	{
		m_pClassObject->Release();
		m_pClassObject = NULL;
	}
	if (m_pEnumerator)
	{
		m_pEnumerator->Release();
		m_pEnumerator = NULL;
	}
	if (m_pServices)
	{
		m_pServices->Release();
		m_pServices = NULL;
	}
	if (m_pLocator)
	{
		m_pLocator->Release();
		m_pLocator = NULL;
	}
	__SetErrorString(_T(""));
	CoUninitialize();
}

CWMIEngine* CWMIEngine::GetInstance()
{
	static CWMIEngine s_manager;
	return &s_manager;
}

LPCSTR CWMIEngine::GetErrorStringA() const
{
	return m_szErrString;
}

LPCWSTR CWMIEngine::GetErrorStringW() const
{
	return m_wszErrString;
}

void CWMIEngine::__SetErrorString(LPCSTR pszErrString)
{
	lstrcpynA(m_szErrString, pszErrString, 256);
	mbstowcs(m_wszErrString, pszErrString, 256);
}

void CWMIEngine::__SetErrorString(LPCWSTR pwszErrString)
{
	lstrcpynW(m_wszErrString, pwszErrString, 256);
	wcstombs(m_szErrString, pwszErrString, 256);
}

BOOL CWMIEngine::Init()
{
	if (m_bValid)
	{
		return TRUE;
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		__SetErrorString(_T("COM initialization failed."));
		CoUninitialize();
		return FALSE;
	}
	hr = CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IDENTIFY,    // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
	if (FAILED(hr))
	{
		__SetErrorString(_T("Failed to initialize security."));
		return FALSE;
	}

	hr = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *)&m_pLocator);

	if (FAILED(hr))
	{
		__SetErrorString(_T("Failed to create IWbemLocator object."));
		CoUninitialize();
		return FALSE;
	}

	hr = m_pLocator->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&m_pServices             // pointer to IWbemServices proxy
		);

	if (FAILED(hr))
	{
		__SetErrorString(_T("Could not connect ROOT\\CIMV2."));
		m_pLocator->Release();
		m_pLocator = NULL;
		CoUninitialize();
		return FALSE;
	}

	hr = CoSetProxyBlanket(
		m_pServices,                 // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hr))
	{
		__SetErrorString(_T("Could not set proxy blanket."));
		m_pServices->Release();
		m_pServices = NULL;
		m_pLocator->Release();
		m_pLocator = NULL;
		CoUninitialize();
		return FALSE;
	}
	m_bValid = TRUE;
	return TRUE;
}

BOOL CWMIEngine::ExeQuery(LPCSTR pszWQL)
{
	if (m_pEnumerator)
	{
		m_pEnumerator->Release();
		m_pEnumerator = NULL;
	}
	HRESULT hr = m_pServices->ExecQuery(
		bstr_t("WQL"), 
		bstr_t(pszWQL),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&m_pEnumerator);

	if (FAILED(hr))
	{
		__SetErrorString(_T("Query for operating system name failed."));
		return FALSE;
	}
	return TRUE;
}

BOOL CWMIEngine::ExeQuery(LPCWSTR pwszWQL)
{
	if (m_pEnumerator)
	{
		m_pEnumerator->Release();
		m_pEnumerator = NULL;
	}
	HRESULT hr = m_pServices->ExecQuery(
		bstr_t("WQL"), 
		bstr_t(pwszWQL),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&m_pEnumerator);

	if (FAILED(hr))
	{
		__SetErrorString(_T("Query for operating system name failed."));
		return FALSE;
	}
	return TRUE;
}

BOOL CWMIEngine::MoveToNext()
{
	if (m_pClassObject)
	{
		m_pClassObject->Release();
		m_pClassObject = NULL;
	}
	if (!m_pEnumerator)
	{
		__SetErrorString(_T("m_pEnumerator is null."));
		return FALSE;
	}

	ULONG uReturn = 0;
	HRESULT hr = m_pEnumerator->Next(WBEM_INFINITE, 1, &m_pClassObject, &uReturn);
	if (uReturn <= 0)
	{
		__SetErrorString(_T("The ExeQuery result is empty."));
		return FALSE;
	}
	if (FAILED(hr))
	{
		__SetErrorString(_T("m_pEnumerator->Next Faild!"));
		return FALSE;
	}
	return TRUE;
}

BOOL CWMIEngine::SkipCount(UINT uCount)
{
	if (!m_pEnumerator)
	{
		__SetErrorString(_T("m_pEnumerator is null."));
		return FALSE;
	}
	HRESULT hr = m_pEnumerator->Skip(WBEM_INFINITE, uCount);
	if (FAILED(hr))
	{
		__SetErrorString(_T("m_pEnumerator->Skip Faild!"));
		return FALSE;
	}
	return TRUE;
}

BOOL CWMIEngine::GetValue(LPCSTR pszName, LPSTR pszValue, UINT uCount)
{
	BOOL bRet(FALSE);
	if (m_pClassObject)
	{
		WCHAR szName[256] = { 0 };
		mbstowcs(szName, pszName, 256);

		LPSTR szValue = new CHAR[uCount + 1];
		memset(szValue, 0, (uCount+1)*sizeof(CHAR));

		VARIANT vtProp;
		HRESULT hr = m_pClassObject->Get(szName, 0, &vtProp, NULL, 0);
		if (SUCCEEDED(hr))
		{
			if (VT_BSTR == vtProp.vt)
			{
				wcstombs(szValue, vtProp.bstrVal, uCount);
				lstrcpynA(pszValue, szValue, uCount);
				VariantClear(&vtProp);
				bRet = TRUE;
			}
		}
		delete[] szValue;
	}
	return bRet;
}

BOOL CWMIEngine::GetValue(LPCWSTR pwszName, LPWSTR pwszValue, UINT uCount)
{
	BOOL bRet(FALSE);
	if (m_pClassObject)
	{
		VARIANT vtProp;
		HRESULT hr = m_pClassObject->Get(pwszName, 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr))
		{
			if (VT_BSTR == vtProp.vt)
			{
				lstrcpynW(pwszValue, vtProp.bstrVal, uCount);
				VariantClear(&vtProp);
				bRet = TRUE;
			}
		}
	}
	return bRet;
}

BOOL CWMIEngine::InvokeMethod(LPCSTR pszClassName, LPCSTR pszMethodName, LPCSTR pszParamters)
{
	int nClassName  = lstrlenA(pszClassName);
	int nMethodName = lstrlenA(pszMethodName);
	int nParamaters = lstrlenA(pszParamters);
	LPWSTR pwszClassName  = new WCHAR[nClassName  + 1];
	LPWSTR pwszMethodName = new WCHAR[nMethodName + 1];
	LPWSTR pwszParamsters = new WCHAR[nParamaters + 1];
	ZeroMemory(pwszClassName,  (nClassName  + 1) * sizeof(WCHAR));
	ZeroMemory(pwszMethodName, (nMethodName + 1) * sizeof(WCHAR));
	ZeroMemory(pwszParamsters, (nParamaters + 1) * sizeof(WCHAR));
	mbstowcs(pwszClassName,  pszClassName,  nClassName);
	mbstowcs(pwszMethodName, pszMethodName, nMethodName);
	mbstowcs(pwszParamsters, pszParamters,  nParamaters);
	BOOL bReturn = InvokeMethod(pwszClassName, pwszMethodName, pwszParamsters);
	delete[] pwszParamsters;
	delete[] pwszMethodName;
	delete[] pwszClassName;
	return bReturn;
}

BOOL CWMIEngine::InvokeMethod(LPCWSTR pwszClassName, LPCWSTR pwszMethodName, LPCWSTR pszParamters)
{
	int nLen = lstrlenW(pszParamters);
	if (nLen <= 3)
	{
		__SetErrorString(_T("Paramaters is too short."));
		return FALSE;
	}
	LPWSTR wszParamters = new WCHAR[nLen + 1];
	ZeroMemory(wszParamters, (nLen + 1) * sizeof(WCHAR));
	lstrcpynW(wszParamters, pszParamters, nLen + 1);

	std::vector<LPCWSTR> vecParam;
	LPCWSTR pszParam = wcstok(wszParamters, L"|");
	while (pszParam)
	{
		vecParam.push_back(pszParam);
		pszParam = wcstok(NULL, L"|");
	}
	if (vecParam.empty())
	{
		delete[] wszParamters;
		__SetErrorString(_T("Paramaters is too short."));
		return FALSE;
	}
	if (!m_bValid)
	{
		delete[] wszParamters;
		__SetErrorString(_T("Please call Init() on first."));
		return FALSE;
	}

	BSTR MethodName = SysAllocString(pwszMethodName);
	BSTR ClassName = SysAllocString(pwszClassName);

	IWbemClassObject* pClass = NULL;
	HRESULT hr = m_pServices->GetObject(ClassName, 0, NULL, &pClass, NULL);

	IWbemClassObject* pInParamsDefinition = NULL;
	hr = pClass->GetMethod(MethodName, 0, &pInParamsDefinition, NULL);

	IWbemClassObject* pClassInstance = NULL;
	hr = pInParamsDefinition->SpawnInstance(0, &pClassInstance);

	for (std::vector<LPCWSTR>::iterator i(vecParam.begin()); i != vecParam.end(); ++i)
	{
		WCHAR ctype = 0;
		WCHAR wszName[256]  = { 0 };
		WCHAR wszValue[512] = { 0 };
		if (swscanf(*i, L"[%c]%[^=]=%[^\0]", &ctype, wszName, wszValue) >= 3)
		{
			StrTrimW(wszName,  L" ");
			StrTrimW(wszValue, L" ");

			VARIANT varCommand;
			switch (ctype)
			{
			case L'i':
			case L'd':
				varCommand.vt = VT_INT;
				varCommand.intVal = _wtoi(wszValue);
				break;
			case L's':
				varCommand.vt = VT_BSTR;
				varCommand.bstrVal = _bstr_t(wszValue);
				break;
			}
			if (VT_NULL != varCommand.vt)
			{
				pClassInstance->Put(wszName, 0, &varCommand, 0);
			}
			VariantClear(&varCommand);
		}
	}
	hr = m_pServices->ExecMethod(ClassName, MethodName, 0,NULL, pClassInstance, NULL, NULL);

	SysFreeString(ClassName);
	SysFreeString(MethodName);
	pClassInstance->Release();
	pInParamsDefinition->Release();
	pClass->Release();
	delete[] wszParamters;

	return SUCCEEDED(hr);
}

