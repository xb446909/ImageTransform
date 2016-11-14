// ImageTransform.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ImageTransform.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"


#pragma comment(lib, "ippicvmt.lib")

#ifdef _DEBUG
#pragma comment(lib, "IlmImfd.lib")
#pragma comment(lib, "libjasperd.lib")
#pragma comment(lib, "libjpegd.lib")
#pragma comment(lib, "libpngd.lib")
#pragma comment(lib, "libtiffd.lib")
#pragma comment(lib, "libwebpd.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "opencv_core310d.lib")
#pragma comment(lib, "opencv_imgcodecs310d.lib")
#pragma comment(lib, "opencv_imgproc310d.lib")
#else
#pragma comment(lib, "IlmImf.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libtiff.lib")
#pragma comment(lib, "libwebp.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
#pragma comment(lib, "opencv_imgproc310.lib")
#endif


void DrawImage(IplImage* pImg, HDC hDC);
IplImage* FillBorder(IplImage* pSrc, HDC hDC);


int LoadImageFromFile(const char* szImagePathName, int nType, pImageInfo pImg)
{
	IplImage* pSrc = NULL;
	if (nType == IMAGE_TYPE_GRAY)
		pSrc = cvLoadImage(szImagePathName, CV_LOAD_IMAGE_GRAYSCALE);
	else
		pSrc = cvLoadImage(szImagePathName, CV_LOAD_IMAGE_COLOR);
	if (pSrc == NULL) return -1;
	pImg->nChannel = pSrc->nChannels;
	pImg->nHeight = pSrc->height;
	pImg->nWidth = pSrc->width;
	pImg->nType = nType;
	long nSize = 0;
	int nLineSize = pSrc->width * pSrc->nChannels;
	char* pBuffer = new char[pSrc->width * pSrc->height * pSrc->nChannels];
	for (int i = 0; i < pSrc->height; i++)
	{
		memcpy(&pBuffer[nSize], pSrc->imageData + i * pSrc->widthStep, nLineSize);
		nSize += nLineSize;
	}
	pImg->pBuffer = pBuffer;
	return 0;
}


IMAGETRANSFORM_API int DrawToHDC(ImageInfo imgInfo, HDC hDC, BOOL bLockWidthHeightRatio)
{
	IplImage* pImgHeader = cvCreateImageHeader(cvSize(imgInfo.nWidth, imgInfo.nHeight), IPL_DEPTH_8U, imgInfo.nChannel);
	cvSetData(pImgHeader, imgInfo.pBuffer, imgInfo.nWidth * imgInfo.nChannel);

	IplImage* pShowImage = cvCreateImage(cvGetSize(pImgHeader), pImgHeader->depth, 3);

	switch (imgInfo.nType)
	{
	case IMAGE_TYPE_GRAY:
		cvCvtColor(pImgHeader, pShowImage, CV_GRAY2BGR);
		break;
	case IMAGE_TYPE_BGR:
		cvCopy(pImgHeader, pShowImage);
		break;
	case IMAGE_TYPE_RGB:
		cvCvtColor(pImgHeader, pShowImage, CV_RGB2BGR);
		break;
	default:
		break;
	}

	if (bLockWidthHeightRatio)
	{
		IplImage* pDst = FillBorder(pShowImage, hDC);
		DrawImage(pDst, hDC);
		cvReleaseImage(&pDst);
	}
	else
	{
		DrawImage(pShowImage, hDC);
	}

	cvReleaseImage(&pShowImage);
	cvReleaseImageHeader(&pImgHeader);

    return 0;
}


void DrawImage(IplImage* pImg, HDC hDC)
{
	BITMAPINFO bmi;
	BITMAPINFOHEADER* bmih = &(bmi.bmiHeader);
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = pImg->width;
	bmih->biHeight = -abs(pImg->height);
	bmih->biPlanes = 1;
	bmih->biBitCount = pImg->depth * pImg->nChannels;
	bmih->biCompression = BI_RGB;

	RECT rect;
	HWND hWnd = WindowFromDC(hDC);
	GetWindowRect(hWnd, &rect);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	SetStretchBltMode(hDC, HALFTONE);
	StretchDIBits(hDC, 0, 0, nWidth, nHeight, 0, 0, pImg->width, pImg->height, pImg->imageData, &bmi, DIB_RGB_COLORS, SRCCOPY);
}


IplImage* FillBorder(IplImage* pSrc, HDC hDC)
{
	RECT rect;
	HWND hWnd = WindowFromDC(hDC);
	GetWindowRect(hWnd, &rect);
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	IplImage* pDst = NULL;

	int nDir = 0;
	double ratio;
	double dbWidthDivHeight1, dbWidthDivHeight2;
	dbWidthDivHeight1 = (double)pSrc->width / (double)pSrc->height;
	dbWidthDivHeight2 = (double)nWidth / (double)nHeight;
	if (dbWidthDivHeight1 > dbWidthDivHeight2)
	{
		ratio = (double)nWidth / (double)pSrc->width;
		nDir = 0;
	}
	else
	{
		ratio = (double)nHeight / (double)pSrc->height;
		nDir = 1;
	}

	IplImage* pResize = cvCreateImage(cvSize(pSrc->width * ratio, pSrc->height * ratio), pSrc->depth, pSrc->nChannels);
	cvResize(pSrc, pResize, CV_INTER_AREA);

	if (nDir == 0)
	{
		int nBheight = (nHeight - pResize->height) / 2;
		pDst = cvCreateImage(cvSize(pResize->width, nHeight), pResize->depth, pResize->nChannels);
		cvZero(pDst);

		cvSetImageROI(pDst, cvRect(0, nBheight, pResize->width, pResize->height));
		cvCopy(pResize, pDst);
		cvResetImageROI(pDst);
	}
	else
	{
		pDst = cvCreateImage(cvSize(nWidth, pResize->height), pResize->depth, pResize->nChannels);
		cvZero(pDst);
		int nWidth = (nWidth - pResize->width) / 2;

		cvSetImageROI(pDst, cvRect(nWidth, 0, pResize->width, pResize->height));
		cvCopy(pResize, pDst);
		cvResetImageROI(pDst);
	}

	cvReleaseImage(&pResize);

	return pDst;
}