#include "bmpFile.h"
#include <gl/glut.h>

bool bmpFile::writeBmpFile(const std::string& fileName,int biWidth, int biHeight, int biBitCount,const void* ptrRGBQUAD)
{
	FILE* fp;
	BYTE8* pData;
	fopen_s(&fp,fileName.c_str(), "wb");
	if (fp == NULL) return false;
	size_t colorTableSize = 0;//��ɫ���С
	if (biBitCount == 8)
		colorTableSize = 1024;
	//����ÿ��byte����RowSize=4*[BPP*Width/32]������ȡ��
	int RowByteSize = (biWidth * biBitCount / 8 + 3) / 4 * 4;
	//��ʼ��ȡ����
	pData = new BYTE8[RowByteSize * biHeight];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //����4λ���뷽ʽ
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, biWidth, biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData);
	//��д�ļ�ͷ
	BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;	//'BM',��ʾ��bmp���� 
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + RowByteSize * biHeight;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
	//��д��Ϣͷ
	BITMAPINFOHEADER bitmapHeader;
	bitmapHeader.biBitCount = biBitCount; //������ / ��������ֵ��1��2��4��8��16��24��32��
	bitmapHeader.biBitCount = biBitCount;
	bitmapHeader.biClrImportant = 0;
	bitmapHeader.biClrUsed = 0;
	bitmapHeader.biCompression = 0;//BI_RGB����ѹ��
	bitmapHeader.biHeight = biHeight;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biSize = 40;
	bitmapHeader.biSizeImage = RowByteSize * biHeight;
	bitmapHeader.biWidth = biWidth;
	bitmapHeader.biXPelsPerMeter = 0;
	bitmapHeader.biYPelsPerMeter = 0;
	//д���ļ�ͷ����Ϣͷ
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	if (biBitCount == 8)
		fwrite(ptrRGBQUAD, sizeof(RGBQUAD), 256, fp);
	//д��
	fwrite(pData, sizeof(WORD16), biHeight * RowByteSize, fp);
	delete pData;
	return false;
}

bool bmpFile::readBMPFile(const std::string& fileName, void ** ptrRGBQUAD, void*& pData)
{
	FILE* fp;

	//fopen_s(&fp, fileName.c_str(), "rb");
	fopen_s(&fp,"C:\\Users\\23231\\Desktop\\example.bmp", "rb");
	if (fp == NULL) return false;
	BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);//��ȡ�ļ�ͷ
	if (fileHeader.bfType != 0x4D42)					//������ļ�����BMP�����ļ�
		return false;

	BITMAPINFOHEADER infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);//��ȡ��Ϣͷ
	if (infoHeader.biBitCount == 8 && ptrRGBQUAD !=nullptr)
	{
		*ptrRGBQUAD = new RGBQUAD[256];
		fread(ptrRGBQUAD, sizeof(RGBQUAD), 256, fp);
	}
	int imageSize = infoHeader.biWidth * 3;
	while (imageSize % 4 != 0) imageSize++;
	imageSize *= infoHeader.biHeight;
	pData = (GLubyte*)malloc(imageSize);
	fseek(fp, 54, SEEK_SET);
	fread(pData, sizeof(BYTE8), imageSize, fp);
	GLubyte* test = (GLubyte*)pData;
	fclose(fp);
	return true;
} 
