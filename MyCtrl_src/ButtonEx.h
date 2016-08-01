#if !defined(AFX_ButtonEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)
#define AFX_ButtonEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonEx.h : header file
//

class CButtonEx : public CBitmapButton
{
public:
	CButtonEx();
	virtual ~CButtonEx();

public:

public:

protected:
	virtual void PreSubclassWindow();

public:
	BOOL SetBitmaps(UINT nIdUp, UINT nIdDown, UINT nIdOver, UINT nIdDisable);

protected:
	UINT m_nIdUp;
	UINT m_nIdOver;
	BOOL m_bTrackLeave;

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ButtonEx_H__D4B84C31_74F4_4D83_BA41_DFD077461211__INCLUDED_)

