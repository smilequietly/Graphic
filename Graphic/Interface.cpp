#include "Interface.h"
#include <cfloat>
#include <windows.h>
#include <Commdlg.h>
#include <cstdio>
#include <tchar.h> 

#define PI acos(-1)
const int LITTLE_DIS = 10;
const double MAX_DISTANCE = 5.0;

std::list<Graph>graph;//存储所有图形的所有关键点的索引
std::list<Graph>cutGraph;//裁剪图形
AETLines Lines;

Graph* selectedGraph = nullptr;//被选中的图形指针
GraphIter selectedGraphIter;
int selectedPosIndex = -1;//被选中图形某个点的索引
enum Menu selectedMenu = SELECT;//被选中的菜单项
enum Menu curColor = RED;

enum SysState sysState = SysState::NOTHING;//系统当前状态
GPos startPos(0.0, 0.0);//刚开始鼠标左击的点
Graph bakGraph;//备份,用于平移变换

std::string writeFilePath = "bmpFile.bmp";//写入bmp文件时的路径
void* pData = nullptr;
bool writeBMP = false;
int IMAGE_WIDTH = 640;
int IMAGE_HEIGHT = 480;

void start() {
	if (pData != nullptr) {
		glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData);
	}

	glBegin(GL_POINTS);//保证无图形画时，能够清空
	glEnd();
	if (cutGraph.empty()) {//如果不需要裁剪
		for (Graph& gh : graph) {//图形
			Lines.clear();
			processColor(gh.color);
			switch (gh.type) {
			case Type::LINE:drawLines(gh); break;
			case Type::CIRCLE:drawCircle(gh); break;
			case Type::CURVE:drawCurve(gh); break;
			case Type::TRIANGLE:
			case Type::RECTANGLE:
				if (!gh.isFilled)
					drawPolygon(gh);
				else
					drawFillPolygonNoStart(gh);
				break;
			}
			GDraw::AET_Start(Lines);
		}

	}
	else {//开始裁剪，只显示裁剪区域内的直线
		for (Graph& gh : cutGraph) {
			processColor(gh.color);
			glEnable(GL_LINE_STIPPLE);//打开画线模式
			glLineStipple(2, 0X00FF);//建立画线的模式
			drawPolygon(gh);
			glDisable(GL_LINE_STIPPLE);//关闭画线模式
			cutRectangle(gh);
		}
	}
	if (writeBMP)
		writeBmpFile(writeFilePath);
}

bool getKeyPos(int x, int y, std::list<Graph>& Gh) {
	for (GraphIter gh = Gh.begin(); gh != Gh.end(); gh++) {
		for (int i = 0; i < gh->posArray.size(); i++) {
			GPos& p = gh->posArray[i];
			if (getDistance(p.x, p.y, x, y) <= MAX_DISTANCE) {
				selectedPosIndex = i;
				selectedGraphIter = gh;
				selectedGraph = &(*gh);
				return true;
			}
		}
	}
	return false;
}
void getWindowToWorld(GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble& posX, GLdouble& posY, GLdouble& posZ)
{
	GLint    viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetIntegerv(GL_VIEWPORT, viewport); // 得到的是最后一个设置视口的参数
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	winY = viewport[3] - winY - 1;
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	return;
}
double getDistance(int x0, int y0, int x1, int y1) {
	return sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
}
void tracingPoints(int precision, double R) {
	if (selectedGraph == nullptr) return;
	for (const GPos& p : selectedGraph->posArray) {
		drawPoint(p.x, p.y, precision, R);
	}
	if (sysState == SysState::TRANSFORAM)
		drawPoint(startPos.x, startPos.y, precision, R);
}
void drawPoint(GLdouble x, GLdouble y, int precision, double R) {
	glBegin(GL_POLYGON);
	for (int i = 0; i < precision; i++)
	{
		glVertex2f(x + R * cos(2 * PI * i / precision), y + R * sin(2 * PI * i / precision));
	}
	glEnd();
}
GLdouble getAngle(const GPos& start, const GPos& end) {
	GLdouble dx = end.x - start.x, dy = end.y - start.y;
	GLdouble angle = atan2(dy, dx);
	return angle;
}
Type getType(Menu menu)
{
	Type type;
	switch (menu) {
	case Menu::DDALINE:
	case Menu::MIDBRESENHAM_LINE:
	case Menu::IMPROVED_MIDBRESENHAM_LINE:
		type = Type::LINE; break;
	case Menu::MIDBRESENHAM_CIRCLE:
		type = Type::CIRCLE; break;
	case Menu::TRIANGLE_PLOYGON:
		type = Type::TRIANGLE; break;
	case Menu::RECTANGLE_PLOYGON:
	case Menu::SQUARE_PLOYGON:
		type = Type::RECTANGLE; break;
	case Menu::BEZIER_CURVE:
	case Menu::NURBS_CURVE3:
		type = Type::CURVE; break;
	case Menu::COHENSURTHERLAND:
	case Menu::LBLINECLIP:
		type = Type::CUT; break;
	case Menu::TRANSLATE:
	case Menu::ROTATE:
	case Menu::SCALE:
		type = Type::TRANSFORAMATION; break;
	default:
		type = NUL;
	}
	return type;
}

void processMenuEvents(int state)
{
	if (state == DELETE_GRAPH) {
		if (selectedGraph != nullptr)
			deleteGraph(selectedGraphIter);
		glutPostRedisplay();
	}
	else if (state == FILL) {
		if (selectedGraph != nullptr)
			selectedGraph->isFilled = true;
		glutPostRedisplay();
	}
	else if (state == NOT_FILL) {
		if (selectedGraph != nullptr)
			selectedGraph->isFilled = false;
		glutPostRedisplay();
	}
	else if (state == READ_BMP)
	{
		char filePath[MAX_PATH]{ 0 };
		if (FileDialog(filePath))
		{
			if (pData != nullptr) {//首先将释放内存
				free(pData);
				pData = nullptr;
			}
			if (readBMPFile(std::string(filePath)))//读入数据到pData 
				glutPostRedisplay();
		}
	}
	else if (state == WRITE_BMP) {
		writeBMP = true;
		glutPostRedisplay();
	}
	else if (state >= RED && state <= WHITE)
	{
		if (selectedGraph != nullptr) {
			selectedGraph->color =(enum Menu)state;
			glutPostRedisplay();
		}
		processColor((enum Menu)state);
		curColor = (enum Menu)state;
	}
	else if (state == CLEAR) {
		processClear();
		glutPostRedisplay();
	}
	else
		selectedMenu = (enum Menu)state;
}
void creatInterfaceMenu()
{
	//绘画子菜单
	int line = glutCreateMenu(processMenuEvents);//直线
	glutAddMenuEntry("DDA", DDALINE);
	glutAddMenuEntry("中点Bresenham", MIDBRESENHAM_LINE);
	glutAddMenuEntry("改进的中点Bresenham", IMPROVED_MIDBRESENHAM_LINE);
	int polygon = glutCreateMenu(processMenuEvents);//多边形
	glutAddMenuEntry("三角形", TRIANGLE_PLOYGON);
	glutAddMenuEntry("正方形", SQUARE_PLOYGON);
	glutAddMenuEntry("矩形", RECTANGLE_PLOYGON);
	int roundness = glutCreateMenu(processMenuEvents);//圆形
	glutAddMenuEntry("中点Bresenham", MIDBRESENHAM_CIRCLE);
	int curve = glutCreateMenu(processMenuEvents);//曲线
	glutAddMenuEntry("Bezier曲线", BEZIER_CURVE);
	glutAddMenuEntry("3次B样条曲线", NURBS_CURVE3);
	int draw = glutCreateMenu(processMenuEvents);//绘画  
	glutAddSubMenu("直线", line);
	glutAddSubMenu("多边形", polygon);
	glutAddSubMenu("圆形", roundness);
	glutAddSubMenu("曲线", curve);
	//裁剪子菜单
	int cut = glutCreateMenu(processMenuEvents);//裁剪
	glutAddMenuEntry("编码裁剪算法", COHENSURTHERLAND);
	glutAddMenuEntry("梁友栋裁剪算法", LBLINECLIP);
	//几何变换子菜单
	int transformation = glutCreateMenu(processMenuEvents);//几何变换
	glutAddMenuEntry("平移", TRANSLATE);
	glutAddMenuEntry("旋转", ROTATE);
	glutAddMenuEntry("缩放", SCALE);
	//文件存取
	int fileProcess = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("导入24位BMP文件", READ_BMP);
	glutAddMenuEntry("保存为24位BMP文件", WRITE_BMP);
	//填充
	int fill = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("AET填充", FILL);
	glutAddMenuEntry("取消填充", NOT_FILL);
	//颜色
	int color = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("红", RED);
	glutAddMenuEntry("橙", ORANGE);
	glutAddMenuEntry("黄", YELLOW);
	glutAddMenuEntry("绿", GREEN);
	glutAddMenuEntry("蓝", BLUE);
	glutAddMenuEntry("靛", INDIGO);
	glutAddMenuEntry("紫", PURPLE);
	glutAddMenuEntry("黑", BLACK);
	glutAddMenuEntry("白", WHITE);
	//其他
	int other = glutCreateMenu(processMenuEvents);//几何变换
	glutAddMenuEntry("删除图形", DELETE_GRAPH);
	glutAddMenuEntry("添加点", ADD_POINT);
	glutAddMenuEntry("清空", CLEAR);
	//主菜单
	int mainMenu = glutCreateMenu(processMenuEvents);//主菜单
	glutAddSubMenu("绘画", draw);
	glutAddSubMenu("裁剪", cut);
	glutAddSubMenu("几何变换", transformation);
	glutAddSubMenu("文件存取", fileProcess);
	glutAddSubMenu("填充", fill);
	glutAddSubMenu("颜色", color);
	glutAddSubMenu("其他", other);
	//绑定
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processSpecialKeyboardFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F3:processMenuEvents(DELETE_GRAPH); break;
	default:break;
	}
	glutPostRedisplay();
}
void processMouseEvent(int button, int state, int x, int y)
{
	GLdouble px, py, pz;
	getWindowToWorld(x, y, 0, px, py, pz);
	if (button == GLUT_LEFT_BUTTON) {
		startPos = { (GLdouble)px,(GLdouble)py };
		if (selectedMenu == Menu::SELECT) {//选择图形
			if (state == GLUT_DOWN) {
				if (getKeyPos(px, py, graph) || getKeyPos(px, py, cutGraph)) {
					sysState = SysState::DRAW;
				}
				else {
					selectedGraph = nullptr;
					sysState = SysState::NOTHING;
				}
			}
			else {//左键松开
				sysState = SysState::NOTHING;
			}
		}
		else if (selectedMenu == Menu::ADD_POINT) {//添加点
			if (state == GLUT_DOWN) {
				if (selectedGraph != nullptr) {
					addPoint(*selectedGraph, { px,py });
					selectedPosIndex = selectedGraph->posArray.size() - 1;
					sysState = SysState::DRAW;
				}
			}
			else {//左键松开
				selectedMenu = SELECT;
				sysState = SysState::NOTHING;
			}
		}
		else
		{//需要画图
			Type type = getType(selectedMenu);
			if (type == Type::TRANSFORAMATION) {//开始进行平移变换
				if (state == GLUT_DOWN) {
					if (selectedGraph == nullptr) {
						selectedMenu = Menu::SELECT;
					}
					else {
						bakGraph = *selectedGraph;
						sysState = SysState::TRANSFORAM;
					}
				}
				else {//左键松开
					selectedMenu = Menu::SELECT;
					sysState = SysState::NOTHING;
				}
			}
			else {//开始画图
				if (state == GLUT_DOWN) {
					Graph gh;
					initDrawGraphic(px, py, type, selectedMenu, gh);
					if (type == CUT) {
						cutGraph.push_back(std::move(gh));
						selectedGraph = &cutGraph.back();
						selectedGraphIter = cutGraph.end();
						selectedGraphIter--;
					}
					else {
						graph.push_back(std::move(gh));
						selectedGraph = &graph.back();
						selectedGraphIter = graph.end();
						selectedGraphIter--;
					}
					selectedPosIndex = 0;
					sysState = SysState::DRAW;
				}
				else {
					selectedMenu = SELECT;
					sysState = SysState::NOTHING;
				}
			}
		}
	}
	glutPostRedisplay();
}
void processMotionMouseEvent(int x, int y) {
	GLdouble px, py, pz;
	getWindowToWorld(x, y, 0, px, py, pz);
	if (sysState == SysState::DRAW) {
		GPos* selectedPos = &selectedGraph->posArray[selectedPosIndex];
		selectedPos->x = px;
		selectedPos->y = py;
		if (selectedGraph->type == Type::CUT || selectedGraph->type == Type::RECTANGLE) {
			std::vector<GPos>& array = selectedGraph->posArray;
			const int Len = selectedGraph->posArray.size();
			GPos& p1 = array[(selectedPosIndex + 1) % Len];
			GPos& p2 = array[(selectedPosIndex + 2) % Len];
			GPos& p3 = array[(selectedPosIndex + 3) % Len];
			int width = selectedPos->x - p2.x;
			int height = selectedPos->y - p2.y;
			int sign = (height > 0) ? 1 : -1;
			bool ok = ((width >= 0 && height >= 0) || (width <= 0 && height <= 0));
			if (selectedGraph->menu == SQUARE_PLOYGON) {//正方形
				if (height == 0) {
					selectedPos->y = abs(width) + p2.y;
				}
				else if (width != 0)
					selectedPos->y = sign * abs(width) + p2.y;
			}
			p1.x = ok ? p2.x : selectedPos->x;
			p1.y = ok ? selectedPos->y : p2.y;
			p3.x = ok ? selectedPos->x : p2.x;
			p3.y = ok ? p2.y : selectedPos->y;
		}
	}
	else if (sysState == SysState::TRANSFORAM) {
		transGraph(*selectedGraph, startPos, { (GLdouble)px,(GLdouble)py });
	}
	glutPostRedisplay();
}
void initInterface(const std::string& fileName) {
	//初始化值
	writeFilePath = fileName;
	//初始化菜单
	creatInterfaceMenu();
	//鼠标事件绑定
	glutMouseFunc(processMouseEvent);
	glutMotionFunc(processMotionMouseEvent);
	//键盘绑定
	glutSpecialFunc(processSpecialKeyboardFunc);
}

void initDrawGraphic(GLdouble startX, GLdouble startY, enum Type type, enum Menu menu, Graph& g) {
	g.menu = menu;
	g.type = type;
	g.color = curColor;
	switch (type) {
		//画直线
	case Type::LINE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY + LITTLE_DIS });
		break;
		//画圆
	case Type::CIRCLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX,startY });
		break;
		//三角形
	case Type::TRIANGLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX - LITTLE_DIS,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY - LITTLE_DIS });
		break;
		//裁剪,矩形
	case Type::CUT:
	case Type::RECTANGLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY });
		break;
		//曲线
	case Type::CURVE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY + LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + 2 * LITTLE_DIS,startY });
		g.posArray.push_back(GPos{ startX + 3 * LITTLE_DIS,startY - LITTLE_DIS });
		break;
		//几何变换
	case Type::TRANSFORAMATION:
		break;
	}
}

void processColor(Menu menu)
{
	switch (menu)
	{
	case RED:		glColor3ub(255, 0, 0);		break;
	case ORANGE:	glColor3ub(255, 125, 0);	break;
	case YELLOW:	glColor3ub(255, 255, 0);	break;
	case GREEN:		glColor3ub(0, 255, 0);		break;
	case BLUE:		glColor3ub(0, 0, 255);		break;
	case INDIGO:	glColor3ub(0, 255, 255);	break;
	case PURPLE:	glColor3ub(255, 0, 255);	break;
	case BLACK:		glColor3ub(0, 0, 0);	break;
	case WHITE:		glColor3ub(255, 255, 255);	break;
	}
}

#define LINE_PARAMETER g.posArray[i].x,g.posArray[i].y,g.posArray[i+1].x,g.posArray[i+1].y
void drawLines(const Graph& g)
{
	if (g.posArray.size() < 2) return;
	for (int i = 0; i < g.posArray.size() - 1; i++) {
		switch (g.menu) {
		case DDALINE:GDraw::DDALine(LINE_PARAMETER); break;
		case MIDBRESENHAM_LINE:GDraw::MidBresenhamLine(LINE_PARAMETER); break;
		case IMPROVED_MIDBRESENHAM_LINE:GDraw::ImprovedBresenhamLine(LINE_PARAMETER); break;
		}
	}
}
void drawPolygon(const Graph& g)
{
	if (g.posArray.size() < 3) return;
	glBegin(GL_LINE_LOOP);
	for (const GPos& p : g.posArray) {
		glVertex2f(p.x, p.y);
	}
	glEnd();
}
void drawCircle(const Graph& g)
{
	if (g.posArray.size() != 2) return;
	double r = getDistance(g.posArray[0].x, g.posArray[0].y, g.posArray[1].x, g.posArray[1].y);
	switch (g.menu) {
	case MIDBRESENHAM_CIRCLE:GDraw::MidBresenhamCircle(g.posArray[1].x, g.posArray[1].y, r);
	}
}
void drawCurve(const Graph& g)
{
	if (g.posArray.size() < 4) return;
	switch (g.menu) {
	case BEZIER_CURVE:GDraw::BezierCurve(500, g.posArray); break;
	case NURBS_CURVE3:GDraw::NURBS_Curve3(500, g.posArray); break;
	}
}
void drawFillPolygonNoStart(Graph& g) {
	if (g.posArray.size() < 3) return;
	std::vector<Qs::GraphicDraw::Pos<int>>q;
	for (GPos& p : g.posArray) {
		q.push_back(Qs::GraphicDraw::Pos<int>{(GLint)p.x, (GLint)p.y});
	}
	GDraw::ActiveEdgeTable(q, Lines);
}

void cutRectangle(const Graph& g)
{
	if (g.posArray.size() != 4) return;
	GLdouble x = DBL_MAX, y = DBL_MAX, px = std::numeric_limits<double>::lowest(), py = std::numeric_limits<double>::lowest();
	for (const GPos& p : g.posArray) {
		x = min(x, p.x);
		y = min(y, p.y);
		px = max(px, p.x);
		py = max(py, p.y);
	}
	//裁剪所有直线
	for (const Graph& gp : graph) {
		for (int i = 0; i < gp.posArray.size() - 1; i++) {
			if (gp.type == LINE) {
				switch (g.menu) {
				case COHENSURTHERLAND:GDraw::CohenSurtherland({ x,y }, { px,py }, gp.posArray[i], gp.posArray[i + 1]); break;
				case LBLINECLIP:GDraw::LBLineClip({ x,y }, { px,py }, gp.posArray[i], gp.posArray[i + 1]); break;
				}
			}
		}
	}
}

void addPoint(Graph& g, const GPos& pos)
{
	switch (g.type) {
	case LINE:
	case TRIANGLE:
	case CURVE:
		g.posArray.push_back(pos);
	}
}
void deleteGraph(GraphIter& g)
{
	if (g->type == CUT) {
		cutGraph.erase(g);
	}
	else {
		graph.erase(g);
	}
}

void transGraph(Graph& g, const GPos& start, const GPos& end)
{
	static GPos lastPos = start;
	static GLfloat scaleValue = 1;
	GLfloat angle = 0.0f;
	GLfloat dx = 0.0, dy = 0.0;

	switch (selectedMenu) {
	case TRANSLATE:
		dx = end.x - start.x, dy = end.y - start.y;
		break;
	case ROTATE:
		angle = getAngle(start, end);
		break;
	case SCALE:
		scaleValue = (end.x - lastPos.x > 0) ? scaleValue * 1.02041 : scaleValue * 0.98;
		lastPos = end;
		break;
	}
	for (int i = 0; i < selectedGraph->posArray.size(); i++) {
		GPos tmp = bakGraph.posArray[i];
		switch (selectedMenu) {
		case TRANSLATE:
			translate(tmp, dx, dy);
			break;
		case ROTATE:

			rotate(tmp, angle, tmp.x, tmp.y);
			break;
		case SCALE:
			scale(tmp, scaleValue, scaleValue);
			break;
		}
		selectedGraph->posArray[i] = tmp;
	}
}
void translate(GPos& pos, GLdouble x, GLdouble y) {
	pos.x += x;
	pos.y += y;
}//平移
void rotate(GPos& pos, GLfloat angle, GLdouble x, GLdouble y) {
	pos.x = x * cos(angle) - y * sin(angle);
	pos.y = x * sin(angle) + y * cos(angle);
}//旋转
void scale(GPos& pos, GLdouble x, GLdouble y) {
	pos.x *= x;
	pos.y *= y;
}

int FileDialog(char* path)// 返回值: 成功 1, 失败 0,通过 path 返回获取的路径
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);	// 结构大小
	ofn.lpstrFile = path;			// 路径
	ofn.nMaxFile = MAX_PATH;		// 路径大小
	ofn.lpstrFilter = "Bitmap Files(*.bmp)/0*.bmp/0"; // 文件类型
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	return GetOpenFileName(&ofn);
}
bool writeBmpFile(const std::string& fileName)
{
	FILE* fp;
	BYTE8* pixelData;
	int biWidth = glutGet(GLUT_WINDOW_WIDTH);
	int biHeight = glutGet(GLUT_WINDOW_HEIGHT);
	fopen_s(&fp, fileName.c_str(), "wb");
	if (fp == NULL)
		return false;
	//计算每行byte数，RowSize=4*[BPP*Width/32]，向上取整
	int RowByteSize = (((biWidth * 24 / 8) + 3) >> 2) << 2;
	int imageSize = RowByteSize * biHeight;
	//开始读取像素
	pixelData = new GLubyte[imageSize];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //设置4位对齐方式
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, biWidth, biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelData);
	//填写文件头
	BITMAPFILEHEADER fileHeader;
	memset(&fileHeader, sizeof(BITMAPFILEHEADER), 0);
	fileHeader.bfType = 0x4D42;	//'BM',表示是bmp类型 
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//填写信息头
	BITMAPINFOHEADER bitmapHeader;
	memset(&bitmapHeader, sizeof(BITMAPINFOHEADER), 0);
	bitmapHeader.biBitCount = 24; //比特数 / 像素数，值有1、2、4、8、16、24、32；
	bitmapHeader.biClrImportant = 0;
	bitmapHeader.biClrUsed = 0;
	bitmapHeader.biCompression = BI_RGB;
	bitmapHeader.biHeight = biHeight;
	bitmapHeader.biPlanes = 1;
	bitmapHeader.biSize = 40;
	bitmapHeader.biSizeImage = 0;
	bitmapHeader.biWidth = biWidth;
	bitmapHeader.biXPelsPerMeter = 0;
	bitmapHeader.biYPelsPerMeter = 0;
	//写入文件头和信息头
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//写入数据
	fwrite(pixelData, imageSize, 1, fp);
	fclose(fp);
	delete[] pixelData;
	return true;
}
bool readBMPFile(const std::string& fileName)
{
	FILE* fp;
	fopen_s(&fp, fileName.c_str(), "rb");
	if (fp == NULL)
		return false;
	BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);//获取文件头
	if (fileHeader.bfType != 0x4D42)					//如果该文件不是BMP类型文件
		return false;
	BITMAPINFOHEADER infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);//获取信息头
	int imageSize = infoHeader.biWidth * 3;
	while (imageSize % 4 != 0) imageSize++;
	imageSize *= infoHeader.biHeight;
	IMAGE_WIDTH = infoHeader.biWidth;
	IMAGE_HEIGHT = infoHeader.biHeight;
	pData = (GLubyte*)malloc(imageSize);
	if (pData != NULL)
		fread(pData, sizeof(GLubyte), imageSize, fp);
	fclose(fp);
	return true;
}

void processClear()
{
	graph.clear();//存储所有图形的所有关键点的索引
	cutGraph.clear();//裁剪图形
	Lines.clear();

	selectedGraph = nullptr;//被选中的图形指针
	selectedPosIndex = -1;//被选中图形某个点的索引
	selectedMenu = SELECT;//被选中的菜单项
	curColor = RED;

	sysState = SysState::NOTHING;//系统当前状态

	pData = nullptr;
	writeBMP = false;
}




