#if !defined(AFX_StaticEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)
#define AFX_StaticEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StaticEx.h : header file
//


class CStaticEx : public CStatic
{
public:
	CStaticEx();
	
protected:
	HBITMAP m_hbmpUp;
	HBITMAP m_hbmpDown;
	HBITMAP m_hbmpOver;
	
	HICON   m_hicoUp;
	HICON   m_hicoDown;
	HICON   m_hicoOver;
	BOOL    m_bTrackLeave;

public:
	
protected:
	virtual void PreSubclassWindow();

public:
	virtual ~CStaticEx();
public:
	void SetBitmaps(UINT nIdUp, UINT nIdDown, UINT nIdOver);
	void SetIcons  (UINT nIdUp, UINT nIdDown, UINT nIdOver);
	
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_StaticEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)

