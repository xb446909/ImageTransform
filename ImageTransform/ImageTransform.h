// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 IMAGETRANSFORM_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// IMAGETRANSFORM_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef IMAGETRANSFORM_EXPORTS
#define IMAGETRANSFORM_API extern "C" __declspec(dllexport)
#else
#define IMAGETRANSFORM_API extern "C" __declspec(dllimport)
#endif

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

IMAGETRANSFORM_API int DrawToHDC(ImageInfo imgInfo, HDC hDC, BOOL bLockWidthHeightRatio);
IMAGETRANSFORM_API int LoadImageFromFile(const char* szImagePathName, int nType, pImageInfo pImg);
