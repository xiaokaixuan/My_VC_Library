#include "stdafx.h"
#include "gdijpegutil.h"

#pragma comment(lib,"gdiplus") 

CGDIJpegUtil::CGDIJpegUtil(void)
{
}

CGDIJpegUtil::~CGDIJpegUtil(void)
{
}

WCHAR *CGDIJpegUtil::BStr2WStr(LPCSTR pSource, int &iDstLen)
{
	iDstLen = MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), NULL, 0);
	WCHAR *pWDst = new WCHAR[iDstLen + 1];
	memset(pWDst, 0, sizeof(WCHAR) * (iDstLen + 1));
	MultiByteToWideChar(0, 0, pSource, (int)strlen(pSource), pWDst, iDstLen);
	return pWDst;
}

int GetCodecClsid(LPCWSTR format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
	{
		return -1;
	}

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
	{
		return -1;
	}

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);

	return -1;
}

bool CGDIJpegUtil::BMPToJPG(LPCTSTR pBmpFile, LPCTSTR pJpgFile, long iQuality)
{
	using namespace Gdiplus;
	bool bRet = false;
	try
	{
		CLSID codecClsid;
		EncoderParameters encoderParameters;
		GdiplusStartupInput gdiplusStartupInput;
		ULONG gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		CStringW swBmp(pBmpFile), swJpg(pJpgFile);
		LPCWSTR pWBmpFile = swBmp;
		LPCWSTR pWJpegFile = swJpg;

		GetCodecClsid(L"image/jpeg", &codecClsid);

		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		encoderParameters.Parameter[0].Value = &iQuality;

		Image *pImage = new Image(pWBmpFile);
		Status bStatus = pImage->Save(pWJpegFile, &codecClsid, &encoderParameters);
		delete pImage;

		GdiplusShutdown(gdiplusToken); 
		if (bStatus == Ok)
		{
			bRet = true;
		}
	}
	catch (...)
	{
	}
	return bRet;
}

bool CGDIJpegUtil::JPGToBMP(LPCTSTR pJpgFile, LPCTSTR pBmpFile)
{
	using namespace Gdiplus;
	bool bRet = false;
	try
	{
		CLSID codecClsid;
		EncoderParameters encoderParameters;
		GdiplusStartupInput gdiplusStartupInput;
		ULONG gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		CStringW swBmp(pBmpFile), swJpg(pJpgFile);
		LPCWSTR pWBmpFile = swBmp;
		LPCWSTR pWJpegFile = swJpg;

		GetCodecClsid(L"image/bmp", &codecClsid);

		encoderParameters.Count = 0;

		Image *pImage = new Image(pWJpegFile);
		Status bStatus = pImage->Save(pWBmpFile, &codecClsid, &encoderParameters);
		delete pImage;

		GdiplusShutdown(gdiplusToken); 
		if (bStatus == Ok)
		{
			bRet = true;
		}
	}
	catch (...)
	{
	}
	return bRet;
}

bool CGDIJpegUtil::AddFont(LPCTSTR pSrcName, LPCTSTR pDstName, LPCTSTR pText, int iXPos, int iYPos)
{
	using namespace Gdiplus;
	CLSID codecClsid;
	EncoderParameters encoderParameters;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CStringW swSrc(pSrcName), swDst(pDstName), swText(pText);
	LPCWSTR pWSrcFile = swSrc;
	LPCWSTR pWDstFile = swDst;
	LPCWSTR pWText = swText;

	Image *pImage = new Image(pWSrcFile);
	Graphics *pGraph = new Graphics(pImage);
	SolidBrush *pBrush = new SolidBrush(Color::Black);
	FontFamily fontFamily(L"ËÎÌו");
	Gdiplus::Font font(&fontFamily, 16, FontStyleRegular, UnitPoint);
	PointF fontPoint((REAL)iXPos, (REAL)iYPos);

	pGraph->DrawString(pWText, (INT)wcslen(pWText), &font, fontPoint, pBrush);

	GetCodecClsid(L"image/bmp", &codecClsid);
	encoderParameters.Count = 0;
	Status bStatus = pImage->Save(pWDstFile, &codecClsid, &encoderParameters);
	delete pBrush;
	delete pGraph;
	delete pImage;
	return true;
}