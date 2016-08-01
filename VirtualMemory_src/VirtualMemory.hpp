#if !defined(_VIRTUALMEMORY_HPP__F5ED6E87_F849_4d52_B6FE_7A798D931AF7__INCLUDED_)
#define _VIRTUALMEMORY_HPP__F5ED6E87_F849_4d52_B6FE_7A798D931AF7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <exception>

class CVirtualMemory
{
public:
	explicit CVirtualMemory(DWORD dwSize = 0)
		: m_pMem(0)
		, m_dwSize(dwSize)
	{
		if (m_dwSize)
		{
			Alloc(m_dwSize);
		}
	}

	CVirtualMemory(const CVirtualMemory &other)
		: m_pMem(0)
		, m_dwSize(0)
	{
		*this = other;
	}

	CVirtualMemory& operator=(const CVirtualMemory &other)
	{
		if (m_dwSize)
		{
			VirtualFree(m_pMem, 0, MEM_RELEASE);
		}
		m_dwSize = other.m_dwSize;
		if (m_dwSize)
		{
			Alloc(m_dwSize);
			CopyMemory(m_pMem, other.m_pMem, other.m_dwSize);
		}
		return *this;
	}

	~CVirtualMemory()
	{
		Alloc(0);
	}

	LPVOID Alloc(DWORD dwSize)
	{
		if (m_pMem)
		{
			VirtualFree(m_pMem, 0, MEM_RELEASE);
			m_pMem = 0;
			m_dwSize = 0;
		}
		if (dwSize)
		{
			m_pMem = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!m_pMem)
			{
				throw std::exception("VirtualAlloc failed!");
			}
			m_dwSize = dwSize;
		}
		return m_pMem;
	}

	LPVOID Get(DWORD dwOffset) const
	{
		return ((BYTE*)m_pMem) + dwOffset;
	}

	operator LPVOID()
	{
		return m_pMem;
	}

	DWORD Size() const
	{
		return m_dwSize;
	}

protected:
	LPVOID m_pMem;
	DWORD  m_dwSize;
};

#endif // !defined(_VIRTUALMEMORY_HPP__F5ED6E87_F849_4d52_B6FE_7A798D931AF7__INCLUDED_)

