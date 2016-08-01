// SizeManager.h: interface for the CSizeManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIZEMANAGER_H__D1274EE2_54A6_4759_B1ED_DE4D05349E6F__INCLUDED_)
#define AFX_SIZEMANAGER_H__D1274EE2_54A6_4759_B1ED_DE4D05349E6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable: 4514 4786)
#include <map>

// The scale type definations.

#define RS_CXSIZE 0x1
#define RS_CYSIZE 0x2
#define RS_CXMOVE 0x4
#define RS_CYMOVE 0x8

typedef struct tagSIZEINFO
{
	RECT irc;  // Initialize rectangle.
	RECT crc;  // Current rectangle.
	SIZE nlt;  // Left and top distances to the border.
	SIZE nrb;  // Right and bottom distances to the border.
	BOOL less; // Can or cannot less to initialize rectangle.
	UINT type; // The scale type.
} SIZEINFO, *LPSIZEINFO;

typedef std::map<UINT, SIZEINFO> CMapCtrl;

class CSizeManager  
{
public:
	CSizeManager(HWND hParent = NULL);
	virtual ~CSizeManager();

public:
	void AddCtrl(UINT nID, UINT type, BOOL bLess = FALSE);
	void RemoveCtrl(UINT nID);
	void Resize();
	void SetOwner(HWND hParent);
	void ReinitSize(UINT uID); // Reinitialize specified control.
	void ReinitSizes();		   // Reinitialize all controls.
	BOOL GetSizeInfo(UINT uID, LPSIZEINFO lpSizeInfo);
	BOOL SetSizeInfo(UINT uID, const SIZEINFO& sizeInfo);

protected:
	void _SizeCtrl(UINT uID, SIZEINFO& si);
	void _MoveCtrl(UINT uID, SIZEINFO& si);
	BOOL _InitSize(UINT nID, LPSIZEINFO lpSizeInfo);

protected:
	HWND m_hParent;
	CMapCtrl m_mapCtrls;
};

#endif // !defined(AFX_SIZEMANAGER_H__D1274EE2_54A6_4759_B1ED_DE4D05349E6F__INCLUDED_)
