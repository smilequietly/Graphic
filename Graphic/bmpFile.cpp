#include "bmpFile.h"
#include <gl/glut.h>

bool bmpFile::writeBmpFile(const std::string& fileName,int biWidth, int biHeight, int biBitCount,const void* ptrRGBQUAD)
{
	FILE* fp;
	BYTE8* pData;
	fopen_s(&fp,fileName.c_str(), "wb");
	if (fp == NULL) return false;
	size_t colorTableSize = 0;//颜色表大小
	if (biBitCount == 8)
		colorTableSize = 1024;
	//计算每行byte数，RowSize=4*[BPP*Width/32]，向上取整
	int RowByteSize = (biWidth * biBitCount / 8 + 3) / 4 * 4;
	//开始读取像素
	pData = new BYTE8[RowByteSize * biHeight];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, biWidth, biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData);
	//填写文件头
	BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;	//'BM',表示是bmp类型 
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + RowByteSize * biHeight;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
	//填写信息头
	BITMAPINFOHEADER bitmapHeader;
	bitmapHeader.biBitCount = biBitCount; //比特数 / 像素数，值有1、2、4、8、16、24、32；
	bitmapHeader.biBitCount = biBitCount;
	bitmapHeader.biClrImportant = 0;
	bitmapHeader.biClrUsed = 0;
	bitmapHeader.biCompression = 0;//BI_RGB即不压缩
	bitmapHeader.biHeight = biHeight;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biSize = 40;
	bitmapHeader.biSizeImage = RowByteSize * biHeight;
	bitmapHeader.biWidth = biWidth;
	bitmapHeader.biXPelsPerMeter = 0;
	bitmapHeader.biYPelsPerMeter = 0;
	//写入文件头和信息头
    fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	if (biBitCount == 8)
		fwrite(ptrRGBQUAD, sizeof(RGBQUAD), 256, fp);
	//写入
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
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);//获取文件头
	if (fileHeader.bfType != 0x4D42)					//如果该文件不是BMP类型文件
		return false;

	BITMAPINFOHEADER infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);//获取信息头
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
