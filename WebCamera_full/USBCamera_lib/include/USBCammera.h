#if !defined(_USBCAMERA_H_8D22F2FD_A66F_4fc3_9B7C_2EA478048A8D)
#define _USBCAMERA_H_8D22F2FD_A66F_4fc3_9B7C_2EA478048A8D

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Windows.h>

class _declspec(dllimport) CUSBCammera
{
public:
	CUSBCammera();
	virtual ~CUSBCammera();

protected:
	class CCameraDS* m_pCamMgr;

public:
	static int GetCamCount();
	static int GetCamName(int nIndex, LPTSTR pszName, int nMaxCount);

public:
	//打开摄像头
	bool OpenCamera(int nIndex, BOOL bSetProperties = FALSE, int nWidth = 320, int nHeight = 240);

	//关闭摄像头，析构时会自动调用
	void CloseCamera();

	//图像宽度
	int GetWidth();
	//图像高度
	int GetHeight();

	//获取帧图像
	HBITMAP QueryFrame();
};

#ifdef _DEBUG
	#ifdef _UNICODE
		#pragma comment(lib, "USBCamera_ud.lib")
	#else
		#pragma comment(lib, "USBCamera_d.lib")
	#endif
#else
	#ifdef _UNICODE
		#pragma comment(lib, "USBCamera_u.lib")
	#else
		#pragma comment(lib, "USBCamera.lib")
	#endif
#endif

#endif // !defined(_USBCAMERA_H_8D22F2FD_A66F_4fc3_9B7C_2EA478048A8D)

