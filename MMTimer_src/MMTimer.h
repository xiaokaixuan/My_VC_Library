// MMTimer.h: interface for the IMMTimer struct.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MMTIMER_H__9FF30726_425A_4EDB_9570_A1A51F84135F__INCLUDED_)
#define AFX_MMTIMER_H__9FF30726_425A_4EDB_9570_A1A51F84135F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment(lib, "winmm.lib")
#pragma warning(disable: 4786)

#include <windows.h>
#include <map>
#include <mmsystem.h>

typedef struct tagPARAMSTRUCT
{
	UINT   uTimerId;
	UINT   uRepeat;
	WPARAM wParam;
	LPARAM lParam;
} PARAMSTRUCT;

struct IMMTimer  
{
private:
	CRITICAL_SECTION			m_ctsMap;
	std::map<UINT, UINT>		m_mapId2Id;
	std::map<UINT, PARAMSTRUCT> m_mapId2Param;

public:
	virtual void OnMMTimer(UINT uTimerId, WPARAM wParam, LPARAM lParam) = 0;

public:
	inline BOOL SetMMTimer(UINT uTimerId, UINT uDelay, UINT uRepeat, WPARAM wParam, LPARAM lParam)
	{
		BOOL bReturn(FALSE);
		if (uTimerId && uRepeat)
		{
			EnterCriticalSection(&m_ctsMap);
			if (m_mapId2Id.find(uTimerId) == m_mapId2Id.end())
			{
				UINT _uTimerId = (UINT)timeSetEvent(uDelay, 1, __StaticTimeProc, DWORD(this), TIME_PERIODIC);
				if (_uTimerId)
				{
					m_mapId2Id[uTimerId] = _uTimerId;
					m_mapId2Param[_uTimerId].uTimerId = uTimerId;
					m_mapId2Param[_uTimerId].uRepeat  = uRepeat;
					m_mapId2Param[_uTimerId].wParam   = wParam;
					m_mapId2Param[_uTimerId].lParam   = lParam;
					bReturn = TRUE;
				}
			}
			LeaveCriticalSection(&m_ctsMap);
		}
		return bReturn;
	}

	inline BOOL KillMMTimer(UINT uTimerId)
	{
		BOOL bReturn(FALSE);
		if (uTimerId)
		{
			EnterCriticalSection(&m_ctsMap);
			if (m_mapId2Id.find(uTimerId) != m_mapId2Id.end())
			{
				UINT _uTimerId = m_mapId2Id[uTimerId];
				m_mapId2Id.erase(uTimerId);
				if (m_mapId2Param.find(_uTimerId) != m_mapId2Param.end())
				{
					m_mapId2Param.erase(_uTimerId);
				}
				if (TIMERR_NOERROR == timeKillEvent(_uTimerId))
				{
					bReturn = TRUE;
				}
			}
			LeaveCriticalSection(&m_ctsMap);
		}
		return bReturn;
	}

protected:
	IMMTimer()
	{
		InitializeCriticalSection(&m_ctsMap);
	}
	virtual ~IMMTimer()
	{
		std::map<UINT, UINT>::iterator i(m_mapId2Id.begin());
		for (; i != m_mapId2Id.end(); ++i)
		{
			timeKillEvent(i->second);
		}
		m_mapId2Id.clear();
		m_mapId2Param.clear();

		DeleteCriticalSection(&m_ctsMap);
	}

private:
	inline void __TimeProc(UINT _uTimerId)
	{
		UINT   uTimerId(0);
		WPARAM wParam(0);
		LPARAM lParam(0);

		EnterCriticalSection(&m_ctsMap);
		if (m_mapId2Param.find(_uTimerId) != m_mapId2Param.end())
		{
			uTimerId = m_mapId2Param[_uTimerId].uTimerId;
			wParam = m_mapId2Param[_uTimerId].wParam;
			lParam = m_mapId2Param[_uTimerId].lParam;

			if (m_mapId2Param[_uTimerId].uRepeat != INFINITE)
			{
				m_mapId2Param[_uTimerId].uRepeat -= 1;
			}
			if (m_mapId2Param[_uTimerId].uRepeat <= 0)
			{
				m_mapId2Param.erase(_uTimerId);
				if (m_mapId2Id.find(uTimerId) != m_mapId2Id.end())
				{
					m_mapId2Id.erase(uTimerId);
				}
				timeKillEvent(_uTimerId);
			}
		}
		LeaveCriticalSection(&m_ctsMap);

		if (uTimerId)
		{
			OnMMTimer(uTimerId, wParam, lParam);
		}
	}
	inline static void CALLBACK __StaticTimeProc(UINT uTimerId, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		IMMTimer* pThis = (IMMTimer*)dwUser;
		pThis->__TimeProc(uTimerId);
	}
};

#endif // !defined(AFX_MMTIMER_H__9FF30726_425A_4EDB_9570_A1A51F84135F__INCLUDED_)


