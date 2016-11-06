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

// 这是导出函数的一个示例。
IMAGETRANSFORM_API int DrawToHDC(ImageInfo imgInfo, HDC hDC)
{
	IplImage* pImgHeader = cvCreateImageHeader(cvSize(imgInfo.nWidth, imgInfo.nHeight), IPL_DEPTH_8U, imgInfo.nChannel);
	cvSetData(pImgHeader, imgInfo.pBuffer, imgInfo.nWidth * imgInfo.nChannel);

	IplImage* pShowImage = cvCreateImage(cvGetSize(pImgHeader), pImgHeader->depth, 3);

	switch (imgInfo.nType)
	{
	case IMAGE_TYPE_GRAY:
		cvCvtColor(pImgHeader, pShowImage, CV_GRAY2RGB);
		break;
	case IMAGE_TYPE_BGR:
		cvCvtColor(pImgHeader, pShowImage, CV_BGR2RGB);
		break;
	case IMAGE_TYPE_RGB:
		cvCopy(pImgHeader, pShowImage);
		break;
	default:
		break;
	}

	cvReleaseImage(&pShowImage);
	cvReleaseImageHeader(&pImgHeader);

    return 0;
}
