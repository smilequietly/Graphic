#pragma once
#include <iostream>
typedef unsigned _int8  BYTE8;
typedef unsigned _int16 WORD16;
typedef unsigned _int32 DWORD32;

class bmpFile
{
private:
	//文件头
#pragma pack(1)//表示它后面的代码都按照n个字节对齐
	struct BITMAPFILEHEADER
	{
		WORD16  bfType;		//两字节，文件类型标识，必须为ASCII码“BM”
		DWORD32	bfSize;		//四字节，文件的尺寸，以byte为单位
		WORD16  bfReserved1;//两字节，保留字，必须为0
		WORD16  bfReserved2;//两字节，保留字，必须为0
		DWORD32	bfOffBits;	//四字节，一个以byte为单位的偏移，从fileHeader结构体开始到位图数据
	};
#pragma pack()
	//信息头
	struct BITMAPINFOHEADER {
		DWORD32 biSize;				//这个结构体的尺寸
		DWORD32  biWidth;			//位图的宽度
		DWORD32  biHeight;			//位图的长度
		WORD16  biPlanes;			//The number of planes for the target device. This value must be set to 1.
		WORD16  biBitCount;			//一个像素有几位
		DWORD32 biCompression;		//0：不压缩，1：RLE8，2：RLE4
		DWORD32 biSizeImage;			//4字节对齐的图像数据大小
		DWORD32  biXPelsPerMeter;	//用象素/米表示的水平分辨率
		DWORD32  biYPelsPerMeter;	//用象素/米表示的垂直分辨率
		DWORD32 biClrUsed;			//实际使用的调色板索引数，0：使用所有的调色板索引
		DWORD32 biClrImportant;		//重要的调色板索引数，0：所有的调色板索引都重要
	};
	//RGB
	struct RGBColor
	{
		BYTE8 B;		//蓝
		BYTE8 G;		//绿
		BYTE8 R;		//红
	};
	struct RGBQUAD {
		BYTE8    B;		//该颜色的蓝色分量(值范围为0-255)  
		BYTE8    G;		//该颜色的绿色分量(值范围为0-255) 
		BYTE8    R;		//该颜色的红色分量(值范围为0-255) 
		BYTE8    Reserved; //保留值，设为0
	};

public:
	
	static bool writeBmpFile(const std::string& fileName, int biWidth, int biHeight, int biBitCount = 24, const void* ptrRGBQUAD = nullptr);
	static bool readBMPFile(const std::string& fileName, void** ptrRGBQUAD, void*& pData);
};
