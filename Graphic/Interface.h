#pragma once
#include <iostream>
#include <vector>
#include <list>
#include "GraphicDraw.h"
#include <GL/glut.h>

struct Graph;
using GDraw = Qs::GraphicDraw;
typedef GDraw::Pos<double> GPos;
typedef std::list<Graph>::iterator GraphIter;
typedef std::vector<std::pair<GDraw::Pos<int>, GDraw::Pos<int>>> AETLines;

typedef unsigned _int8  BYTE8;
typedef unsigned _int16 WORD16;
typedef unsigned _int32 DWORD32;

enum Menu {
	SELECT,//ѡ��
	DDALINE, MIDBRESENHAM_LINE, IMPROVED_MIDBRESENHAM_LINE,//ֱ��
	MIDBRESENHAM_CIRCLE,//Բ
	TRIANGLE_PLOYGON, RECTANGLE_PLOYGON, SQUARE_PLOYGON,//�����
	BEZIER_CURVE, NURBS_CURVE3,//����
	COHENSURTHERLAND, LBLINECLIP,//�ü�
	TRANSLATE, ROTATE, SCALE,//���α任
	ADD_POINT,//�ɶ������Σ����ߣ�ֱ�����ӵ�
	DELETE_GRAPH,//ɾ��ͼ��
	FILL,NOT_FILL,//���
	READ_BMP,WRITE_BMP,
	RED,ORANGE,YELLOW,GREEN,BLUE,INDIGO,PURPLE,BLACK,WHITE,//��ɫ����Ȼ��������Ϻڰ�
	CLEAR//���
};
enum Type {
	NUL,//������
	LINE,//ֱ��
	CIRCLE,//Բ
	TRIANGLE,//������
	RECTANGLE,//����
	CURVE,//����
	CUT,//�ü�
	TRANSFORAMATION//���α任
};
enum SysState {
	NOTHING,
	DRAW,
	TRANSFORAM
};

struct Graph {
	std::vector<GPos>posArray;//�ؼ��������
	enum Menu menu = SELECT;//ʹ�õĻ��Ʒ�ʽ
	enum Type type = NUL;
	bool isFilled = false;//�Ƿ����
	enum Menu color = RED;
	Graph& operator=(const Graph& g) {
		posArray.assign(g.posArray.begin(), g.posArray.end());
		menu = g.menu;
		type = g.type;
		color = g.color;
		return *this;
	}
};

void start();//��ʼ��ͼ

bool getKeyPos(int x, int y, std::list<Graph>& Gh);//��ȡGh��(x,y)�㸽���Ĺؼ���
void getWindowToWorld(GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble& posX, GLdouble& posY, GLdouble& posZ);//��������ת��Ϊ��������
double getDistance(int x0, int y0, int x1, int y1);//��ȡ�������
void tracingPoints(int precision, double R);//���
void drawPoint(GLdouble x, GLdouble y, int precision, double R);//���ڻ�Բ
enum Type getType(enum Menu menu);//�Ӳ˵���Ŀ�л�ȡ������
GLdouble getAngle(const GPos& start, const GPos& end);//����ȷ���Ƕ�

void processMenuEvents(int state);//�˵�������
void creatInterfaceMenu();//��������˵�

void processMouseEvent(int button, int state, int x, int y);//��괦����
void processMotionMouseEvent(int x, int y);//Active��괦����
void initDrawGraphic(GLdouble startX, GLdouble startY, enum Type type, enum Menu menu, Graph& g);//����������µĵ��ʼ��ͼ��

void processSpecialKeyboardFunc(int key, int x, int y);//����

void initInterface(const std::string& writeFileName);//��ʼ���˵�����꣬����,�ͱ���

void drawLines(const Graph& g);//��ֱ��
void drawPolygon(const Graph& g);//�������
void drawCircle(const Graph& g);//��Բ
void drawCurve(const Graph& g);//������
void cutRectangle(const Graph& g);//�ü�
void addPoint(Graph& g, const GPos& pos);//��ӵ�
void deleteGraph(GraphIter& g);//ɾ��ͼ��
void drawFillPolygonNoStart(Graph& g);//���

void transGraph(Graph& g, const GPos& start, const GPos& end);//��ͼ�ν���ƽ�Ʊ任
void translate(GPos& pos, GLdouble x, GLdouble y);//ƽ��
void rotate(GPos& pos, GLfloat angle, GLdouble x, GLdouble y);//��ת
void scale(GPos& pos, GLdouble x, GLdouble y);//����

int FileDialog(char* path);
bool writeBmpFile(const std::string& fileName);
bool readBMPFile(const std::string& fileName);

void processColor(enum Menu menu);

void processClear();//���
