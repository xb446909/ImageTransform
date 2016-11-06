// ImageTransform.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ImageTransform.h"
#include "opencv\cv.h"

#pragma comment(lib, "IlmImf.lib")
#pragma comment(lib, "ippicvmt.lib")
#pragma comment(lib, "libjasper.lib")
#pragma comment(lib, "libjpeg.lib")
#pragma comment(lib, "libpng.lib")
#pragma comment(lib, "libtiff.lib")
#pragma comment(lib, "libwebp.lib")
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgproc310.lib")

void DrawImage(IplImage* pImg, HDC hDC);
IplImage* FillBorder(IplImage* pSrc, HDC hDC);

// 这是导出函数的一个示例。
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
		IplImage* pBorder = cvCreateImage(cvSize(pResize->width, (nHeight - pResize->height) / 2), pResize->depth, pResize->nChannels);
		cvZero(pBorder);
		pDst = cvCreateImage(cvSize(pResize->width, pBorder->height * 2 + pResize->height), pResize->depth, pResize->nChannels);

		cvSetImageROI(pDst, cvRect(0, 0, pBorder->width, pBorder->height));
		cvCopy(pBorder, pDst);
		cvResetImageROI(pDst);

		cvSetImageROI(pDst, cvRect(0, pBorder->height, pResize->width, pResize->height));
		cvCopy(pResize, pDst);
		cvResetImageROI(pDst);

		cvSetImageROI(pDst, cvRect(0, pBorder->height + pResize->height, pBorder->width, pBorder->height));
		cvCopy(pBorder, pDst);
		cvResetImageROI(pDst);

		cvReleaseImage(&pBorder);
	}
	else
	{
		IplImage* pBorder = cvCreateImage(cvSize((nWidth - pResize->width) / 2, pResize->height), pResize->depth, pResize->nChannels);
		cvZero(pBorder);
		pDst = cvCreateImage(cvSize(pBorder->width * 2 + pResize->width, pResize->height), pResize->depth, pResize->nChannels);

		cvSetImageROI(pDst, cvRect(0, 0, pBorder->width, pBorder->height));
		cvCopy(pBorder, pDst);
		cvResetImageROI(pDst);

		cvSetImageROI(pDst, cvRect(pBorder->width, 0, pResize->width, pResize->height));
		cvCopy(pResize, pDst);
		cvResetImageROI(pDst);

		cvSetImageROI(pDst, cvRect(pBorder->width + pResize->width, 0, pBorder->width, pBorder->height));
		cvCopy(pBorder, pDst);
		cvResetImageROI(pDst);

		cvReleaseImage(&pBorder);
	}


	cvReleaseImage(&pResize);

	return pDst;
}