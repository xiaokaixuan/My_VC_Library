#if !defined(AFX_WMIENGINE_H__F7D96C03_2C6B_4060_8CD8_486A99502852__INCLUDED_)
#define AFX_WMIENGINE_H__F7D96C03_2C6B_4060_8CD8_486A99502852__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4996)

struct IWbemLocator;
struct IWbemServices;
struct IWbemClassObject;
struct IEnumWbemClassObject;

class CWMIEngine  
{
public:
	static CWMIEngine* GetInstance();

private:
	CWMIEngine();

public:
	virtual ~CWMIEngine();

public:
	BOOL Init();

public:
	BOOL ExeQuery(LPCSTR pszWQL);
	BOOL ExeQuery(LPCWSTR pwszWQL);
	BOOL MoveToNext();
	BOOL SkipCount(UINT uCount);
	BOOL GetValue(LPCSTR pszName, LPSTR pszValue, UINT uCount);
	BOOL GetValue(LPCWSTR pwszName, LPWSTR pwszValue, UINT uCount);

public:
	BOOL InvokeMethod(LPCSTR pszClassName, LPCSTR pszMethodName, LPCSTR pszParamters);
	BOOL InvokeMethod(LPCWSTR pwszClassName, LPCWSTR pwszMethodName, LPCWSTR pszParamters);

public:
	LPCSTR  GetErrorStringA() const;
	LPCWSTR GetErrorStringW() const;

private:
	void __SetErrorString(LPCSTR  pszErrString);
	void __SetErrorString(LPCWSTR pwszErrString);

private:
	IWbemLocator*  m_pLocator;
	IWbemServices* m_pServices;
	IWbemClassObject* m_pClassObject;
	IEnumWbemClassObject* m_pEnumerator;

private:
	BOOL  m_bValid;
	CHAR  m_szErrString[256];
	WCHAR m_wszErrString[256];
};

#endif // !defined(AFX_WMIENGINE_H__F7D96C03_2C6B_4060_8CD8_486A99502852__INCLUDED_)

