// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� IMAGETRANSFORM_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// IMAGETRANSFORM_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
