#pragma once
#include <iostream>
typedef unsigned _int8  BYTE8;
typedef unsigned _int16 WORD16;
typedef unsigned _int32 DWORD32;

class bmpFile
{
private:
	//�ļ�ͷ
#pragma pack(1)//��ʾ������Ĵ��붼����n���ֽڶ���
	struct BITMAPFILEHEADER
	{
		WORD16  bfType;		//���ֽڣ��ļ����ͱ�ʶ������ΪASCII�롰BM��
		DWORD32	bfSize;		//���ֽڣ��ļ��ĳߴ磬��byteΪ��λ
		WORD16  bfReserved1;//���ֽڣ������֣�����Ϊ0
		WORD16  bfReserved2;//���ֽڣ������֣�����Ϊ0
		DWORD32	bfOffBits;	//���ֽڣ�һ����byteΪ��λ��ƫ�ƣ���fileHeader�ṹ�忪ʼ��λͼ����
	};
#pragma pack()
	//��Ϣͷ
	struct BITMAPINFOHEADER {
		DWORD32 biSize;				//����ṹ��ĳߴ�
		DWORD32  biWidth;			//λͼ�Ŀ��
		DWORD32  biHeight;			//λͼ�ĳ���
		WORD16  biPlanes;			//The number of planes for the target device. This value must be set to 1.
		WORD16  biBitCount;			//һ�������м�λ
		DWORD32 biCompression;		//0����ѹ����1��RLE8��2��RLE4
		DWORD32 biSizeImage;			//4�ֽڶ����ͼ�����ݴ�С
		DWORD32  biXPelsPerMeter;	//������/�ױ�ʾ��ˮƽ�ֱ���
		DWORD32  biYPelsPerMeter;	//������/�ױ�ʾ�Ĵ�ֱ�ֱ���
		DWORD32 biClrUsed;			//ʵ��ʹ�õĵ�ɫ����������0��ʹ�����еĵ�ɫ������
		DWORD32 biClrImportant;		//��Ҫ�ĵ�ɫ����������0�����еĵ�ɫ����������Ҫ
	};
	//RGB
	struct RGBColor
	{
		BYTE8 B;		//��
		BYTE8 G;		//��
		BYTE8 R;		//��
	};
	struct RGBQUAD {
		BYTE8    B;		//����ɫ����ɫ����(ֵ��ΧΪ0-255)  
		BYTE8    G;		//����ɫ����ɫ����(ֵ��ΧΪ0-255) 
		BYTE8    R;		//����ɫ�ĺ�ɫ����(ֵ��ΧΪ0-255) 
		BYTE8    Reserved; //����ֵ����Ϊ0
	};

public:
	
	static bool writeBmpFile(const std::string& fileName, int biWidth, int biHeight, int biBitCount = 24, const void* ptrRGBQUAD = nullptr);
	static bool readBMPFile(const std::string& fileName, void** ptrRGBQUAD, void*& pData);
};
