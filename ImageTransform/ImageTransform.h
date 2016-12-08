
#define IMAGE_TYPE_GRAY		0
#define IMAGE_TYPE_RGB		1
#define IMAGE_TYPE_BGR		2

typedef struct tagImageInfo
{
	int nWidth;
	int nHeight;
	int nChannel;
	int nType;
	void* pBuffer;
}ImageInfo, *pImageInfo;

int __stdcall DrawToHDC(ImageInfo imgInfo, HDC hDC, BOOL bLockWidthHeightRatio);
int __stdcall LoadImageFromFile(const char* szImagePathName, int nType, pImageInfo pImg);
