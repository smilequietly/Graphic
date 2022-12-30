#include "Interface.h"
#include <cfloat>
#include <windows.h>
#include <Commdlg.h>
#include <cstdio>
#include <tchar.h> 

#define PI acos(-1)
const int LITTLE_DIS = 10;
const double MAX_DISTANCE = 5.0;

std::list<Graph>graph;//�洢����ͼ�ε����йؼ��������
std::list<Graph>cutGraph;//�ü�ͼ��
AETLines Lines;

Graph* selectedGraph = nullptr;//��ѡ�е�ͼ��ָ��
GraphIter selectedGraphIter;
int selectedPosIndex = -1;//��ѡ��ͼ��ĳ���������
enum Menu selectedMenu = SELECT;//��ѡ�еĲ˵���
enum Menu curColor = RED;

enum SysState sysState = SysState::NOTHING;//ϵͳ��ǰ״̬
GPos startPos(0.0, 0.0);//�տ�ʼ�������ĵ�
Graph bakGraph;//����,����ƽ�Ʊ任

std::string writeFilePath = "bmpFile.bmp";//д��bmp�ļ�ʱ��·��
void* pData = nullptr;
bool writeBMP = false;
int IMAGE_WIDTH = 640;
int IMAGE_HEIGHT = 480;

void start() {
	if (pData != nullptr) {
		glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGR_EXT, GL_UNSIGNED_BYTE, pData);
	}

	glBegin(GL_POINTS);//��֤��ͼ�λ�ʱ���ܹ����
	glEnd();
	if (cutGraph.empty()) {//�������Ҫ�ü�
		for (Graph& gh : graph) {//ͼ��
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
	else {//��ʼ�ü���ֻ��ʾ�ü������ڵ�ֱ��
		for (Graph& gh : cutGraph) {
			processColor(gh.color);
			glEnable(GL_LINE_STIPPLE);//�򿪻���ģʽ
			glLineStipple(2, 0X00FF);//�������ߵ�ģʽ
			drawPolygon(gh);
			glDisable(GL_LINE_STIPPLE);//�رջ���ģʽ
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
	glGetIntegerv(GL_VIEWPORT, viewport); // �õ��������һ�������ӿڵĲ���
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
			if (pData != nullptr) {//���Ƚ��ͷ��ڴ�
				free(pData);
				pData = nullptr;
			}
			if (readBMPFile(std::string(filePath)))//�������ݵ�pData 
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
	//�滭�Ӳ˵�
	int line = glutCreateMenu(processMenuEvents);//ֱ��
	glutAddMenuEntry("DDA", DDALINE);
	glutAddMenuEntry("�е�Bresenham", MIDBRESENHAM_LINE);
	glutAddMenuEntry("�Ľ����е�Bresenham", IMPROVED_MIDBRESENHAM_LINE);
	int polygon = glutCreateMenu(processMenuEvents);//�����
	glutAddMenuEntry("������", TRIANGLE_PLOYGON);
	glutAddMenuEntry("������", SQUARE_PLOYGON);
	glutAddMenuEntry("����", RECTANGLE_PLOYGON);
	int roundness = glutCreateMenu(processMenuEvents);//Բ��
	glutAddMenuEntry("�е�Bresenham", MIDBRESENHAM_CIRCLE);
	int curve = glutCreateMenu(processMenuEvents);//����
	glutAddMenuEntry("Bezier����", BEZIER_CURVE);
	glutAddMenuEntry("3��B��������", NURBS_CURVE3);
	int draw = glutCreateMenu(processMenuEvents);//�滭  
	glutAddSubMenu("ֱ��", line);
	glutAddSubMenu("�����", polygon);
	glutAddSubMenu("Բ��", roundness);
	glutAddSubMenu("����", curve);
	//�ü��Ӳ˵�
	int cut = glutCreateMenu(processMenuEvents);//�ü�
	glutAddMenuEntry("����ü��㷨", COHENSURTHERLAND);
	glutAddMenuEntry("���Ѷ��ü��㷨", LBLINECLIP);
	//���α任�Ӳ˵�
	int transformation = glutCreateMenu(processMenuEvents);//���α任
	glutAddMenuEntry("ƽ��", TRANSLATE);
	glutAddMenuEntry("��ת", ROTATE);
	glutAddMenuEntry("����", SCALE);
	//�ļ���ȡ
	int fileProcess = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("����24λBMP�ļ�", READ_BMP);
	glutAddMenuEntry("����Ϊ24λBMP�ļ�", WRITE_BMP);
	//���
	int fill = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("AET���", FILL);
	glutAddMenuEntry("ȡ�����", NOT_FILL);
	//��ɫ
	int color = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("��", RED);
	glutAddMenuEntry("��", ORANGE);
	glutAddMenuEntry("��", YELLOW);
	glutAddMenuEntry("��", GREEN);
	glutAddMenuEntry("��", BLUE);
	glutAddMenuEntry("��", INDIGO);
	glutAddMenuEntry("��", PURPLE);
	glutAddMenuEntry("��", BLACK);
	glutAddMenuEntry("��", WHITE);
	//����
	int other = glutCreateMenu(processMenuEvents);//���α任
	glutAddMenuEntry("ɾ��ͼ��", DELETE_GRAPH);
	glutAddMenuEntry("��ӵ�", ADD_POINT);
	glutAddMenuEntry("���", CLEAR);
	//���˵�
	int mainMenu = glutCreateMenu(processMenuEvents);//���˵�
	glutAddSubMenu("�滭", draw);
	glutAddSubMenu("�ü�", cut);
	glutAddSubMenu("���α任", transformation);
	glutAddSubMenu("�ļ���ȡ", fileProcess);
	glutAddSubMenu("���", fill);
	glutAddSubMenu("��ɫ", color);
	glutAddSubMenu("����", other);
	//��
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
		if (selectedMenu == Menu::SELECT) {//ѡ��ͼ��
			if (state == GLUT_DOWN) {
				if (getKeyPos(px, py, graph) || getKeyPos(px, py, cutGraph)) {
					sysState = SysState::DRAW;
				}
				else {
					selectedGraph = nullptr;
					sysState = SysState::NOTHING;
				}
			}
			else {//����ɿ�
				sysState = SysState::NOTHING;
			}
		}
		else if (selectedMenu == Menu::ADD_POINT) {//��ӵ�
			if (state == GLUT_DOWN) {
				if (selectedGraph != nullptr) {
					addPoint(*selectedGraph, { px,py });
					selectedPosIndex = selectedGraph->posArray.size() - 1;
					sysState = SysState::DRAW;
				}
			}
			else {//����ɿ�
				selectedMenu = SELECT;
				sysState = SysState::NOTHING;
			}
		}
		else
		{//��Ҫ��ͼ
			Type type = getType(selectedMenu);
			if (type == Type::TRANSFORAMATION) {//��ʼ����ƽ�Ʊ任
				if (state == GLUT_DOWN) {
					if (selectedGraph == nullptr) {
						selectedMenu = Menu::SELECT;
					}
					else {
						bakGraph = *selectedGraph;
						sysState = SysState::TRANSFORAM;
					}
				}
				else {//����ɿ�
					selectedMenu = Menu::SELECT;
					sysState = SysState::NOTHING;
				}
			}
			else {//��ʼ��ͼ
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
			if (selectedGraph->menu == SQUARE_PLOYGON) {//������
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
	//��ʼ��ֵ
	writeFilePath = fileName;
	//��ʼ���˵�
	creatInterfaceMenu();
	//����¼���
	glutMouseFunc(processMouseEvent);
	glutMotionFunc(processMotionMouseEvent);
	//���̰�
	glutSpecialFunc(processSpecialKeyboardFunc);
}

void initDrawGraphic(GLdouble startX, GLdouble startY, enum Type type, enum Menu menu, Graph& g) {
	g.menu = menu;
	g.type = type;
	g.color = curColor;
	switch (type) {
		//��ֱ��
	case Type::LINE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY + LITTLE_DIS });
		break;
		//��Բ
	case Type::CIRCLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX,startY });
		break;
		//������
	case Type::TRIANGLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX - LITTLE_DIS,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY - LITTLE_DIS });
		break;
		//�ü�,����
	case Type::CUT:
	case Type::RECTANGLE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY - LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY });
		break;
		//����
	case Type::CURVE:
		g.posArray.push_back(GPos{ startX,startY });
		g.posArray.push_back(GPos{ startX + LITTLE_DIS,startY + LITTLE_DIS });
		g.posArray.push_back(GPos{ startX + 2 * LITTLE_DIS,startY });
		g.posArray.push_back(GPos{ startX + 3 * LITTLE_DIS,startY - LITTLE_DIS });
		break;
		//���α任
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
	//�ü�����ֱ��
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
}//ƽ��
void rotate(GPos& pos, GLfloat angle, GLdouble x, GLdouble y) {
	pos.x = x * cos(angle) - y * sin(angle);
	pos.y = x * sin(angle) + y * cos(angle);
}//��ת
void scale(GPos& pos, GLdouble x, GLdouble y) {
	pos.x *= x;
	pos.y *= y;
}

int FileDialog(char* path)// ����ֵ: �ɹ� 1, ʧ�� 0,ͨ�� path ���ػ�ȡ��·��
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);	// �ṹ��С
	ofn.lpstrFile = path;			// ·��
	ofn.nMaxFile = MAX_PATH;		// ·����С
	ofn.lpstrFilter = "Bitmap Files(*.bmp)/0*.bmp/0"; // �ļ�����
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
	//����ÿ��byte����RowSize=4*[BPP*Width/32]������ȡ��
	int RowByteSize = (((biWidth * 24 / 8) + 3) >> 2) << 2;
	int imageSize = RowByteSize * biHeight;
	//��ʼ��ȡ����
	pixelData = new GLubyte[imageSize];

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  //����4λ���뷽ʽ
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, biWidth, biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixelData);
	//��д�ļ�ͷ
	BITMAPFILEHEADER fileHeader;
	memset(&fileHeader, sizeof(BITMAPFILEHEADER), 0);
	fileHeader.bfType = 0x4D42;	//'BM',��ʾ��bmp���� 
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
	fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//��д��Ϣͷ
	BITMAPINFOHEADER bitmapHeader;
	memset(&bitmapHeader, sizeof(BITMAPINFOHEADER), 0);
	bitmapHeader.biBitCount = 24; //������ / ��������ֵ��1��2��4��8��16��24��32��
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
	//д���ļ�ͷ����Ϣͷ
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bitmapHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//д������
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
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fp);//��ȡ�ļ�ͷ
	if (fileHeader.bfType != 0x4D42)					//������ļ�����BMP�����ļ�
		return false;
	BITMAPINFOHEADER infoHeader;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fp);//��ȡ��Ϣͷ
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
	graph.clear();//�洢����ͼ�ε����йؼ��������
	cutGraph.clear();//�ü�ͼ��
	Lines.clear();

	selectedGraph = nullptr;//��ѡ�е�ͼ��ָ��
	selectedPosIndex = -1;//��ѡ��ͼ��ĳ���������
	selectedMenu = SELECT;//��ѡ�еĲ˵���
	curColor = RED;

	sysState = SysState::NOTHING;//ϵͳ��ǰ״̬

	pData = nullptr;
	writeBMP = false;
}




