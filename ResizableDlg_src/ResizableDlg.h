// Dialog window with x-direction and y-direction from each anchor:
// 
// TOP_LEFT           TOP_RIGHT 
// +------------------+
// | TL ->      <- TR |
// | |              | |
// | v              v |
// | ^              ^ |
// | |              | |
// | BL ->      <- BR |
// +------------------+
// BOTTOM_LEFT        BOTTOM_RIGHT
//
// ----------------------------------------------------------------
// anchor = TOP_LEFT:     resize right and bottom edges of control
// anchor = TOP_RIGHT:    resize left and bottom edges of control
// anchor = BOTTOM_LEFT:  resize right and top edges of control
// anchor = BOTTOM_RIGHT: resize left and top edges of control
// ----------------------------------------------------------------
//
// m_dxMove =  0: x-distance from anchor does not change at OnSize event
// m_dyMove =  0: y-distance from anchor does not change at OnSize event
//
// m_dxResize = 0: no resize in x-direction from anchor at OnSize event
// m_dyResize = 0: no resize in y-direction from anchor at OnSize event

#if !defined(__RESIZABLEDLG_H__)
#define __RESIZABLEDLG_H__

#include <afxtempl.h>

struct tControl
{
	tControl(HWND hWnd, CRect rect, int anchor, double dxMove, double dyMove, double dxResize, double dyResize, BOOL isTransparent);
	enum anchor { TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT };
	
	HWND m_hWnd;			// handle of control                               
	CRect m_rect;			// rectangle of control at initial dialog window                 
	int m_anchor;			// reference point (dialog window point) 
	double m_dxMove;		// move in x-direction from anchor     
	double m_dyMove;		// move in y-direction from anchor       
	double m_dxResize;		// resize in x-direction from anchor     
	double m_dyResize;		// resize in y-direction from anchor     
	BOOL m_isTransparent;	// true for group box
};

class CResizableDlg : public CDialog
{
public:
	CResizableDlg(UINT nIDTemplate, int dlgWidthMin = 0, int dlgHeightMin = 0, CWnd* pParent = NULL);
	~CResizableDlg();

	void MoveResizeControl(int nID, int anchor, double dxMoveRelative, double dyMoveRelative, double dxResizeRelative, double dyResizeRelative, BOOL isTransparent = FALSE);

private:
	HWND m_hGripper;
	CSize m_sizeInitial;
	CSize m_sizeMinimum;
	CArray<tControl*, tControl*> m_controls;  // Array of controls to be repositioned/resized.

	void LoadDlgPlacement();
	void SaveDlgPlacement();
	
protected:
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(__RESIZABLEDLG_H__)

