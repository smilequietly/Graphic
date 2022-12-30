#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "Interface.h"
#define EXPERIENCE5
#define EXPERIENCE_FUNCTION Experience5
#define PI acos(-1)
using GDraw=Qs::GraphicDraw;
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;

//实验一
#ifdef EXPERIENCE1

void Experience1(void)
{
    glClear(GL_COLOR_BUFFER_BIT);//将窗口背景设置为当前清空颜色白色
    glLineWidth(1.5);
    glColor3f(1.0, 0.0, 0.0);
    //画直线
    GDraw::DDALine(75, -100, -100, 10);
    GDraw::MidBresenhamLine(156, -100, 45, 10);
    GDraw::ImprovedBresenhamLine(48, -100, -150, -50);
    //画圆
    GDraw::MidBresenhamCircle(80, -89, 50);
    //有效边表画多边形
    AETLines Lines;
    GDraw::ActiveEdgeTable({ {30, 10}, {60, 50}, {80, 10}, {120, 90}, {70, 80}, {30, 120}, {10, 70} },Lines);
    GDraw::AET_Start(Lines);
    glutSwapBuffers();
    glFlush();//保证前面的opengl命令立即执行，而不是在缓冲区等待
}
#endif 

//实验三
#ifdef EXPERIENCE3
void drawLine(double x0, double y0, double x1, double y1) {
    glLineWidth(1.5);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glEnd();
}
using Line = std::pair<GDraw::Pos<double>, GDraw::Pos<double>>;
bool cut = true;//是否进行裁剪
GDraw::Pos<double> wLeftBottonPos(50,50);//裁剪窗口左下角点
double wWidth = 200.0,wLength=200.0;//裁剪窗口的长和宽
std::vector <Line> LinePos = {
    {{20,200},{15,26}},
    {{115,89},{200,390}},
    {{88,56},{350,150}},
    {{77,65},{99,250}},
    {{330,210},{35,75}},
    {{66,54},{336,75}},
    {{350,160},{66,350}},
    {{110,220},{56,330}}
};//用于裁剪的线
//实验三绘图函数
void Experience3() {
    glClear(GL_COLOR_BUFFER_BIT);//将窗口背景设置为当前清空颜色白色
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(1.5);
    //画边框
    glBegin(GL_POLYGON);
    glVertex2f(wLeftBottonPos.x, wLeftBottonPos.y);
    glVertex2f(wLeftBottonPos.x + wLength, wLeftBottonPos.y);
    glVertex2f(wLeftBottonPos.x + wLength, wLeftBottonPos.y + wWidth);
    glVertex2f(wLeftBottonPos.x, wLeftBottonPos.y + wWidth);
    glEnd();
    //裁剪画直线
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const Line&line : LinePos) {
        if (cut) {
            GDraw::LBLineClip(wLeftBottonPos,
                GDraw::Pos<double>(wLeftBottonPos.x + wLength, wLeftBottonPos.y + wWidth),
                line.first,
                line.second
                );
        }
        else
            drawLine(line.first.x, line.first.y, line.second.x, line.second.y);
    }
    glutSwapBuffers();
    glFlush();
}
//特殊按键处理
double unitLen = 1;
void SpecialKeyboardFunc_Exp3(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:wLeftBottonPos.y += unitLen; break;
    case GLUT_KEY_DOWN:wLeftBottonPos.y-= unitLen; break;
    case GLUT_KEY_LEFT:wLeftBottonPos.x -= unitLen; break;
    case GLUT_KEY_RIGHT:wLeftBottonPos.x += unitLen; break;
    case GLUT_KEY_F1:if (unitLen < 20)unitLen += 1; break;
    case GLUT_KEY_F2:if (unitLen > 1) unitLen -= 1; break;
    default:break;
    }
    glutPostRedisplay();
}
//普通按键处理
void KeyboardFunc_Exp3(unsigned char Key, int x, int y)
{
    if (Key == 'r') {
        cut = false;
    }
    else if (Key == 'c') {
        cut = true;
    }
    glutPostRedisplay();//重新绘制界面
}
#endif

//实验四
#ifdef EXPERIENCE4 
void drawBezierPoint(double x, double y, double z) {
    glLineWidth(1.5);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(x, y);
    glEnd();
}

void Experience4() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(1.5);
    glColor3f(1.0f, 0.0f, 0.0f);
    //控制点
    std::vector<GPos>ctrlP = {
        {25,37},{25,87}, {125,137},{183,65},{283,65},{263,265}
    };
    GLint num = 500;
    //绘制Bezier曲线
    GDraw::BezierCurve(500, ctrlP);
    GDraw::NURBS_Curve3(500, ctrlP);
    //绘制控制多边形
    glLineWidth(1.5);
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < ctrlP.size(); i++)
        glVertex2f(ctrlP[i].x, ctrlP[i].y);
    glEnd();
    glutSwapBuffers();
    glFlush();
}
#endif 
//实验五
#ifdef EXPERIENCE5

void Experience5() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(1.5);
    start();
    glColor3f(0.0, 1.0, 0.0);
    tracingPoints(100,5.0);
    glutSwapBuffers();
    glFlush();
};
#endif 
//初始化
void Initial(int w, int h)
{
    glViewport(0, 0, w, h);
    //将三维图形投影到窗口中
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);//指定设置投影参数
    glLoadIdentity();//初始化一个单位矩阵
    gluOrtho2D(-(GLdouble)w/2.0, (GLdouble)w/2.0, -(GLdouble)h/2.0, (GLdouble)h/2.0);//用来指定屏幕区域对应的模型坐标范围,设置投影参数
}
int main(int argc,char * argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA);//使用RGB颜色，并使用单缓冲
    glutInitWindowPosition(300,100);//设置窗口在屏幕中位置,x,y为窗口左上角 
    glutInitWindowSize(600, 600);//设置窗口大小
    glutCreateWindow("窗口");//窗口标题
    glutDisplayFunc(&EXPERIENCE_FUNCTION);//设置绘画函数
#ifdef EXPERIENCE3
    glutKeyboardFunc(KeyboardFunc_Exp3);//字母键的按键检测的回调函数
    glutSpecialFunc(SpecialKeyboardFunc_Exp3);
#endif
    glutReshapeFunc(&Initial);
#ifdef EXPERIENCE5
    initInterface("bmpFile.bmp");
#endif 
    glutMainLoop();//进入消息循环
    return 0;
}


