// SizeManager.cpp: implementation of the CSizeManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SizeManager.h"
#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif

CSizeManager::CSizeManager(HWND hParent/*= NULL*/)
: m_hParent(hParent)
{

}

CSizeManager::~CSizeManager()
{

}

void CSizeManager::SetOwner(HWND hParent)
{
	m_hParent = hParent;
}

void CSizeManager::AddCtrl(UINT nID, UINT type, BOOL bLess/*= FALSE*/)
{
	ASSERT(NULL != m_hParent);
	ASSERT(!((type & RS_CXSIZE) && (type & RS_CXMOVE)));
	ASSERT(!((type & RS_CYSIZE) && (type & RS_CYMOVE)));

	SIZEINFO si = {0};
	if (_InitSize(nID, &si))
	{
		si.less = bLess;
		si.type = type;
		m_mapCtrls[nID] = si;
	}
}

void CSizeManager::RemoveCtrl(UINT nID)
{
	m_mapCtrls.erase(nID);
}

void CSizeManager::ReinitSizes()
{
	ASSERT(NULL != m_hParent);
	for (CMapCtrl::iterator i = m_mapCtrls.begin()
		; i != m_mapCtrls.end(); ++i)
	{
		SIZEINFO si = {0};
		if (_InitSize(i->first, &si))
		{
			si.less = i->second.less;
			si.type = i->second.type;
			m_mapCtrls[i->first] = si;
		}
	}
}

void CSizeManager::ReinitSize(UINT uID)
{
	ASSERT(NULL != m_hParent);
	CMapCtrl::iterator i = m_mapCtrls.find(uID);
	if (i != m_mapCtrls.end())
	{
		SIZEINFO si = {0};
		if (_InitSize(i->first, &si))
		{
			si.less = i->second.less;
			si.type = i->second.type;
			m_mapCtrls[i->first] = si;
		}
	}
}

BOOL CSizeManager::_InitSize(UINT nID, LPSIZEINFO lpSizeInfo)
{
	ASSERT(NULL != m_hParent);
	BOOL bRet(FALSE);
	HWND hCtrl = ::GetDlgItem(m_hParent, nID);
	if (NULL != hCtrl)
	{
		RECT crc = {0};
		::GetWindowRect(hCtrl, &crc);
		POINT ptlt = {crc.left, crc.top};
		POINT ptrb = {crc.right, crc.bottom};
		::ScreenToClient(m_hParent, &ptlt);
		::ScreenToClient(m_hParent, &ptrb);
		crc.left = ptlt.x;
		crc.top = ptlt.y;
		crc.right = ptrb.x;
		crc.bottom = ptrb.y;
		lpSizeInfo->irc = crc;
		lpSizeInfo->crc = crc;
		RECT mrc = {0};
		::GetClientRect(m_hParent, &mrc);
		lpSizeInfo->nlt.cx = crc.left - mrc.left;
		lpSizeInfo->nlt.cy = crc.top - mrc.top;
		lpSizeInfo->nrb.cx = mrc.right - crc.right;
		lpSizeInfo->nrb.cy = mrc.bottom - crc.bottom;
		bRet = TRUE;
	}
	return bRet;
}

void CSizeManager::_SizeCtrl(UINT uID, SIZEINFO& si)
{
	ASSERT(NULL != m_hParent);
	HWND hCtrl = ::GetDlgItem(m_hParent, uID);
	if (NULL != hCtrl)
	{
		RECT& crc = si.crc;
		RECT mrc = {0};
		::GetClientRect(m_hParent, &mrc);
		if (si.type & RS_CXSIZE)
		{
			crc.right = mrc.right - si.nrb.cx;
		}
		if (si.type & RS_CYSIZE)
		{
			crc.bottom = mrc.bottom - si.nrb.cy;
		}
		if (!si.less)
		{
			if (crc.right < si.irc.right)
			{
				crc.right = si.irc.right;
			}
			if (crc.bottom < si.irc.bottom)
			{
				crc.bottom = si.irc.bottom;
			}
		}
		::MoveWindow(hCtrl, crc.left, crc.top, crc.right - crc.left, crc.bottom - crc.top, TRUE);
	}
}

void CSizeManager::_MoveCtrl(UINT uID, SIZEINFO& si)
{
	ASSERT(NULL != m_hParent);
	HWND hCtrl = ::GetDlgItem(m_hParent, uID);
	if (NULL != hCtrl)
	{
		RECT& crc = si.crc;
		RECT mrc = {0};
		::GetClientRect(m_hParent, &mrc);
		if (si.type & RS_CXMOVE)
		{
			int nMove = mrc.right - crc.right - si.nrb.cx;
			crc.left  += nMove;
			crc.right += nMove;
			si.nlt.cx += nMove;
		}
		if (si.type & RS_CYMOVE)
		{
			int nMove = mrc.bottom - crc.bottom - si.nrb.cy;
			crc.top	   += nMove;
			crc.bottom += nMove;
			si.nlt.cy  += nMove;
		}
		if (!si.less)
		{
			if (crc.left < si.irc.left)
			{
				crc.left = si.irc.left;
				crc.right = si.irc.right;
				si.nlt.cx = crc.left - mrc.left;
			}
			if (crc.top < si.irc.top)
			{
				crc.top = si.irc.top;
				crc.bottom = si.irc.bottom;
				si.nlt.cy = crc.top - mrc.top;
			}
		}
		::MoveWindow(hCtrl, crc.left, crc.top, crc.right - crc.left, crc.bottom - crc.top, TRUE);
	}
}

void CSizeManager::Resize()
{
	if (m_hParent != NULL)
	{
		RECT mrc;
		::GetClientRect(m_hParent, &mrc);
		for (CMapCtrl::iterator i = m_mapCtrls.begin()
			; i != m_mapCtrls.end(); ++i)
		{
			_SizeCtrl(i->first, i->second);
			_MoveCtrl(i->first, i->second);
		}
		InvalidateRect(m_hParent, NULL, TRUE);
	}
}

BOOL CSizeManager::GetSizeInfo(UINT uID, LPSIZEINFO lpSizeInfo)
{
	ASSERT(NULL != lpSizeInfo);
	BOOL bRet(FALSE);
	CMapCtrl::iterator i = m_mapCtrls.find(uID);
	if (i != m_mapCtrls.end())
	{
		*lpSizeInfo = i->second;
		bRet = TRUE;
	}
	return bRet;
}

BOOL CSizeManager::SetSizeInfo(UINT uID, const SIZEINFO& sizeInfo)
{
	BOOL bRet(FALSE);
	CMapCtrl::iterator i = m_mapCtrls.find(uID);
	if (i != m_mapCtrls.end())
	{
		i->second = sizeInfo;
		bRet = TRUE;
	}
	return bRet;
}
