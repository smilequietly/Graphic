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
	SELECT,//选择
	DDALINE, MIDBRESENHAM_LINE, IMPROVED_MIDBRESENHAM_LINE,//直线
	MIDBRESENHAM_CIRCLE,//圆
	TRIANGLE_PLOYGON, RECTANGLE_PLOYGON, SQUARE_PLOYGON,//多边形
	BEZIER_CURVE, NURBS_CURVE3,//曲线
	COHENSURTHERLAND, LBLINECLIP,//裁剪
	TRANSLATE, ROTATE, SCALE,//几何变换
	ADD_POINT,//可对三角形，曲线，直线增加点
	DELETE_GRAPH,//删除图形
	FILL,NOT_FILL,//填充
	READ_BMP,WRITE_BMP,
	RED,ORANGE,YELLOW,GREEN,BLUE,INDIGO,PURPLE,BLACK,WHITE,//颜色：红橙黄绿蓝靛紫黑白
	CLEAR//清空
};
enum Type {
	NUL,//无类型
	LINE,//直线
	CIRCLE,//圆
	TRIANGLE,//三角形
	RECTANGLE,//矩形
	CURVE,//曲线
	CUT,//裁剪
	TRANSFORAMATION//几何变换
};
enum SysState {
	NOTHING,
	DRAW,
	TRANSFORAM
};

struct Graph {
	std::vector<GPos>posArray;//关键点的数组
	enum Menu menu = SELECT;//使用的绘制方式
	enum Type type = NUL;
	bool isFilled = false;//是否被填充
	enum Menu color = RED;
	Graph& operator=(const Graph& g) {
		posArray.assign(g.posArray.begin(), g.posArray.end());
		menu = g.menu;
		type = g.type;
		color = g.color;
		return *this;
	}
};

void start();//开始画图

bool getKeyPos(int x, int y, std::list<Graph>& Gh);//获取Gh中(x,y)点附近的关键点
void getWindowToWorld(GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble& posX, GLdouble& posY, GLdouble& posZ);//窗口坐标转化为世界坐标
double getDistance(int x0, int y0, int x1, int y1);//获取两点距离
void tracingPoints(int precision, double R);//描点
void drawPoint(GLdouble x, GLdouble y, int precision, double R);//用于画圆
enum Type getType(enum Menu menu);//从菜单项目中获取其类型
GLdouble getAngle(const GPos& start, const GPos& end);//两点确定角度

void processMenuEvents(int state);//菜单处理函数
void creatInterfaceMenu();//创建界面菜单

void processMouseEvent(int button, int state, int x, int y);//鼠标处理函数
void processMotionMouseEvent(int x, int y);//Active鼠标处理函数
void initDrawGraphic(GLdouble startX, GLdouble startY, enum Type type, enum Menu menu, Graph& g);//根据左键按下的点初始化图形

void processSpecialKeyboardFunc(int key, int x, int y);//按键

void initInterface(const std::string& writeFileName);//初始化菜单和鼠标，按键,和变量

void drawLines(const Graph& g);//画直线
void drawPolygon(const Graph& g);//画多边形
void drawCircle(const Graph& g);//画圆
void drawCurve(const Graph& g);//画曲线
void cutRectangle(const Graph& g);//裁剪
void addPoint(Graph& g, const GPos& pos);//添加点
void deleteGraph(GraphIter& g);//删除图形
void drawFillPolygonNoStart(Graph& g);//填充

void transGraph(Graph& g, const GPos& start, const GPos& end);//对图形进行平移变换
void translate(GPos& pos, GLdouble x, GLdouble y);//平移
void rotate(GPos& pos, GLfloat angle, GLdouble x, GLdouble y);//旋转
void scale(GPos& pos, GLdouble x, GLdouble y);//缩放

int FileDialog(char* path);
bool writeBmpFile(const std::string& fileName);
bool readBMPFile(const std::string& fileName);

void processColor(enum Menu menu);

void processClear();//清除
