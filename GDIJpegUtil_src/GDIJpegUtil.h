#pragma once

class CGDIJpegUtil
{
public:
	CGDIJpegUtil(void);
	virtual ~CGDIJpegUtil(void);
	bool AddFont(LPCTSTR pSrcName, LPCTSTR pDstName, LPCTSTR pText, int iXPos, int iYPos);
	bool BMPToJPG(LPCTSTR pBmpFile, LPCTSTR pJpgFile, long iQuality);
	bool JPGToBMP(LPCTSTR pJpgFile, LPCTSTR pBmpFile);
private:
	WCHAR *BStr2WStr(LPCSTR pSource, int &iDstLen);
};
