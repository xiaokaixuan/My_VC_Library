#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#pragma once

class CResource
{
public:
	// Constructor/destructor
	CResource(HMODULE hModule);
	virtual ~CResource();
	
public:
	// Load methods
	BOOL Load(LPCTSTR lpType, LPCTSTR lpName);
	BOOL LoadEx(LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage);
	
	// Misc operations
	LPVOID Lock();
	HMODULE GetModuleHandle() const;
	DWORD GetSize() const;
	void Release();

	//Save to file
	BOOL Save(LPCTSTR lpFileName);

	//Extract resource to disk file (static)
	static BOOL ExtractResource(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpFileName);
	static BOOL ExtractResourceEx(HMODULE hModule, LPCTSTR lpType, LPCTSTR lpName, WORD wLanguage, LPCTSTR lpFileName);

protected:
	HMODULE m_hModule;
	HRSRC   m_hResource;
	HGLOBAL m_hGlobal;
};


#endif //_RESOURCE_H_