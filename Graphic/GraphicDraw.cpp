#include <iostream>
#include <vector>
#include <list>
#include <cmath>

#include "GraphicDraw.h"
using namespace Qs;
//DDA�㷨
void GraphicDraw::DDALine(int x0, int y0, int x1, int y1)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	// epsilon,����С����
	int epsl = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
	// x,y������
	float xIncre = (float)dx / (float)epsl;
	float yIncre = (float)dy / (float)epsl;
	float x = x0, y = y0;
	glBegin(GL_POINTS);
	for (int i = 0; i <= epsl; i++)
	{
		glVertex2i(int(x + 0.5), int(y + 0.5));
		x += xIncre;
		y += yIncre;
	}
	glEnd();
}
//Bresenham�㷨
void GraphicDraw::MidBresenhamLine(int x0, int y0, int x1, int y1) {
	bool vis = false;
	//���k>1��k<-1,����x.y
	if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		vis = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;

	int d = dx - 2 * dy;
	int DownIncre = (dy < 0) ? -2 * dx : 0;
	int UpIncre = (dy < 0) ? 0 : 2 * dx;
	UpIncre -= 2 * dy;
	DownIncre -= 2 * dy;
	int x = x0, y = y0;
	glBegin(GL_POINTS);
	while (x <= x1)
	{
		vis ? glVertex2i(y, x) : glVertex2i(x, y);
		x++;
		if (d < 0) {
			d += UpIncre;
			if (dy > 0)
				y++;
		}
		else {
			d += DownIncre;
			if (dy < 0)
				y--;
		}
	}
	glEnd();
}
//�Ľ���Bresenham�㷨
void GraphicDraw::ImprovedBresenhamLine(int x0, int y0, int x1, int y1) {
	bool vis = false;
	//���k>1��k<-1
	if (std::abs(x1 - x0) < std::abs(y1 - y0)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		vis = true;
	}
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	//΢��
	int dx = x1 - x0;
	int dy = y1 - y0;
	bool judge = (dy > 0);
	int e = -dx;
	int x = x0, y = y0;
	glBegin(GL_POINTS);
	while (x <= x1) {
		vis ? glVertex2i(y, x) : glVertex2i(x, y);
		x++;
		e += 2 * abs(dy);
		if (e > 0) { // y������һ��
			y += judge ? 1 : -1;
			e -= 2 * dx;
		}
	}
	glEnd();
}
//�е�Bresenham��Բ�㷨
void GraphicDraw::CirclePoint(int x0, int y0, int x, int y) {
	glBegin(GL_POINTS);
	glVertex2i(x0 + x, y0 + y);
	glVertex2i(x0 + y, y0 + x);
	glVertex2i(x0 + x, y0 - y);
	glVertex2i(x0 - y, y0 + x);
	glVertex2i(x0 - x, y0 + y);
	glVertex2i(x0 + y, y0 - x);
	glVertex2i(x0 - x, y0 - y);
	glVertex2i(x0 - y, y0 - x);
	glEnd();
}
void GraphicDraw::MidBresenhamCircle(int x0, int y0, int r) {
	int x, y, d;
	x = 0, y = r, d = 1 - r;
	while (x <= y) {
		CirclePoint(x0, y0, x, y);
		if (d < 0)
			d += 2 * x + 3;
		else {
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
	}
}
//---------------------------------------ʵ��AET��Ч�߱�����㷨------------------------------
//�߱�Ľ��
struct Node
{
	double x;           //��ǰɨ��������Ч�߽����x����
	int yMax;           //��Ч�߱����ڵ����ɨ����ֵ
	double kReciprocal; // 1/k
	bool operator<(const Node& other) const
	{
		if (this->x == other.x)
			return this->kReciprocal < other.kReciprocal;
		return this->x < other.x;
	}
};
void addEdge(int i, int j, const std::vector<GraphicDraw::Pos<int>>& p, std::vector<std::list<Node>>& table)
{
	const int len = p.size();
	Node node;
	//����һЩ���㣬�����ظ�����
	bool down = (p[i].y > p[j].y && p[(i - 1 + len) % len].y > p[i].y);
	bool up = (p[i].y < p[j].y&& p[j].y < p[(j + 1) % len].y);                                                                                 //Ҫ��������Ч�߽��
	node.kReciprocal = (double)(p[j].x - p[i].x) / (double)(p[j].y - p[i].y); // 1/k
	int y0 = p[i].y - down, y1 = p[j].y - up;
	int yMin;
	if (y0 <= y1) {
		yMin = y0; node.x = p[i].x; node.yMax = y1;
	}
	else {
		yMin = y1; node.x = p[j].x; node.yMax = y0;
	}
	table[yMin].push_back(node); //��������߱�
}
void GraphicDraw::ActiveEdgeTable(std::vector<Pos<int>> p,std::vector<std::pair<Pos<int>,Pos<int>>>&Lines)
{
	if (p.size() <= 2) return;

	int yMin = p[0].y, yMax = p[0].y;

	for (size_t i = 1; i < p.size(); i++) {
		yMin = (yMin > p[i].y) ? p[i].y : yMin;
		yMax = (yMax < p[i].y) ? p[i].y : yMax;
	}
	//��������Y��ȥyMin
	for (size_t i = 0; i < p.size(); i++) {
		p[i].y -= yMin;
	}
	int bakYMin = yMin;
	yMax -= bakYMin;
	yMin = 0;
	std::vector<std::list<Node>> table(yMax + 1); //��Ч�߱�ͱ߱�
	//��ӱߵ��߱���
	const int len = p.size();
	for (int i = 0; i < len; i++) {
		int LIndex = (len + i - 1) % len; //�ϸ�������
		int RIndex = (len + i + 1) % len; //�¸�������
		//�жϱ�y��Ľ������
		if (p[i].y < p[LIndex].y)
			addEdge(LIndex, i, p, table);
		if (p[i].y < p[RIndex].y)
			addEdge(i, RIndex, p, table);
	}
	//��ʼɨ��ת��
	for (int y = yMin; y <= yMax; y++) {
		//���±߼���
		if (y >= 1 && !table[y - 1].empty()) {
			for (auto it = table[y - 1].begin(); it != table[y - 1].end(); it++) {
				it->x += it->kReciprocal;
				table[y].push_back(*it);
			}
		}
		if (table[y].empty()) continue;
		//����
		table[y].sort();
		//���,ͬʱ���оɱ�ȥ��
		bool vis = false;
		auto it = table[y].begin();//��ǰ���
		auto nextNode = (++table[y].begin());//��һ�����
		glBegin(GL_LINES);
		while (nextNode != table[y].end()) {
			vis = !vis;
			if (vis) {
				int x0 = ceil(it->x);//����ȡ��
				double val = floor(nextNode->x);//����ȡ��
				int x1 = (val == nextNode->x) ? val - 1 : val;//����ҽڵ�պ��ڱ���
				//���ص�ȡ�ڶ����������
				Lines.push_back({ {x0,y+bakYMin},{x1,y+bakYMin} });
			}
			if (it->yMax == y)
				table[y].erase(it);
			it = nextNode;
			nextNode++;
		}
		if (it->yMax == y) table[y].erase(it);
	}
}

void GraphicDraw::AET_Start(std::vector<std::pair<Pos<int>, Pos<int>>>& Lines){
	glBegin(GL_LINES);
	for (int i = 0; i < Lines.size(); i++) {
		glVertex2i(Lines[i].first.x, Lines[i].first.y);
		glVertex2i(Lines[i].second.x, Lines[i].second.y);
	}
	glEnd();
}

//----------------------------------------�ü��㷨-------------------------------------------
//����ü��㷨
void Qs::GraphicDraw::CohenSurtherland(const Pos<double>&lb,const Pos<double>&rt, const Pos<double>&src, const Pos<double>& dst)
{
	double xwl=lb.x, xwr=rt.x, ywb=lb.y, ywt=rt.y;
	double x0 = src.x, y0 = src.y, x1 = dst.x, y1 = dst.y;
	glBegin(GL_LINES);
	while (1) {
		int code0, code1;//��1���ң�2���£�4,�ϣ�8
		code0 = code1 = 0;
		if (x0 < xwl)
			code0 += 1;
		else if (x0 > xwr)
			code0 += 2;

		if (y0 < ywb)
			code0 += 4;
		else if (y0 > ywt)
			code0 += 8;

		if (x1 < xwl)
			code1 += 1;
		else if (x1 > xwr)
			code1 += 2;

		if (y1 < ywb)
			code1 += 4;
		else if (y1 > ywt)
			code1 += 8;
		//������ڴ����ڲ�
		if (!(code0 | code1)) {
			//��ʼ��ֱ��
			glVertex2f(x0, y0);
			glVertex2f(x1, y1);
			break;
		}
		else if (code0 & code1) {//������㶼���ڴ�����
			break;
		}
		else {//�����ڴ����ڣ���ʼ��
			if (!code0) {//���src���ڴ����ڣ�������ȷ��code1�ڴ�����
				std::swap(x0, x1);
				std::swap(y0, y1);
				std::swap(code0, code1);
			}

			if (code0 & 1) {//����������
				y0 = y0 + ((xwl - x0) * (y0 - y1) / (x0 - x1));
				x0 = xwl;
			}
			if (code0 & 2) {
				y0 = y0 + ((xwr - x0) * (y0 - y1) / (x0 - x1));
				x0 = xwr;
			}
			if (code0 & 4) {
				x0 = x0 + ((ywb - y0) * (x0 - x1) / (y0 - y1));
				y0 = ywb;
			}
			if (code0 & 8) {
				x0 = x0 + ((ywt - y0) * (x0 - x1) / (y0 - y1));
				y0 = ywt;
			}
		}
	}
	glEnd();
}
//���Ѷ��㷨
int LBLineClipTest(double p, double q, double& umax, double& umin) {
	double u = 0.0;
	if (p < 0.0) {
		u = q / p;
		if (u > umin)		return 0;
		else if (u > umax)	umax = u;//������ֱ����
	}
	else if (p > 0.0) {
		u = q / p;
		if (u < umax)		return 0;
		else if (u < umin)	umin = u;//������ֱ����
	}
	else if (q < 0.0)		return 0;
	return 1;
}

void Qs::GraphicDraw::LBLineClip(const Pos<double>& lb, const Pos<double>& rt, const Pos<double>& src, const Pos<double>& dst)
{
	double delayX, delayY, umin, umax;
	delayX = dst.x - src.x;
	delayY = dst.y - src.y;
	umin = 1.0;umax = 0.0;
	double x0=src.x, y0=src.y, x1=dst.x, y1=dst.y;
	if (LBLineClipTest(-delayX, src.x - lb.x, umax, umin)) {
		if (LBLineClipTest(delayX, rt.x - src.x, umax, umin)) {
			if (LBLineClipTest(-delayY, src.y - lb.y, umax, umin)) {
				if (LBLineClipTest(delayY, rt.y - src.y, umax, umin)) {
					x0 = src.x + umax * delayX;
					y0 = src.y + umax * delayY;
					x1 = src.x + umin * delayX;
					y1 = src.y + umin * delayY;
					glBegin(GL_LINES);
					glVertex2i(x0, y0);
					glVertex2i(x1, y1);
					glEnd();
				}
			}
		}
	}

}

//--------------------------------------��������--------------------------------------
//����Cnk
void GetCnk(GLint n, std::vector<GLint>&Cnk) {
	//Cnk= n!/k!/(n-k)!
	for (int k = 0; k <= n; k++) {
		Cnk[k] = 1;
		for (int i = n; i >= k + 1; i--)
			Cnk[k] = Cnk[k] * i;
		for (int i = n - k; i >= 2; i--) 
			Cnk[k] = Cnk[k] / i;
	}
}
//����Bezier�����ϵ������
void Qs::GraphicDraw::GetBezierP(Pos<GLdouble>& curvePos, const GLdouble t, const std::vector<GLint>& Cnk, const std::vector<Pos<GLdouble>>& ctrlP)
{	//BEN(t)=Cnk * t^k * (1-t)^(n-k),k=0->k=n����n+1������
	curvePos.x = curvePos.y = 0.0;
	GLdouble Bernstein;
	int n = ctrlP.size() - 1;
	for (int k = 0; k <= n; k++) {
		Bernstein = Cnk[k] * pow(t, k) * pow(1 - t, n - k);
		curvePos.x += ctrlP[k].x * Bernstein;
		curvePos.y += ctrlP[k].y * Bernstein;
	}
}
//���ݿ��Ƶ㣬�������ϵ�num����
void Qs::GraphicDraw::BezierCurve(const GLint num, const std::vector<Pos<GLdouble>>& ctrlP)
{
	static std::vector<GLint>Cnk;
	Cnk.resize(ctrlP.size());
	GetCnk(ctrlP.size() - 1, Cnk);
	Pos<GLdouble> pos(0.0,0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= num; i++) {
		GetBezierP(pos, (GLdouble)i / (GLdouble)num, Cnk, ctrlP);
		glVertex2f(pos.x, pos.y);
	}
	glEnd();
	Cnk.clear();
}
//3��B��������
void Qs::GraphicDraw::NURBS_Curve3(const GLint num,const std::vector<Pos<GLdouble>>& ctrlP)
{//G(t)=1/n! * ��͹�ʽ{(-1)^j * C(n+1)j * (t+n-i-j)^n }
	int m = ctrlP.size() - 3 - 1;
	Pos<GLdouble> pos(0.0, 0.0);
	GLdouble G03, G13, G23, G33;
	glBegin(GL_POINTS);
	for (int k = 0; k <= m; k++) {//��k��
		for (int i = 0; i <= num; i++) {
			double t = (GLdouble)i /(GLdouble) num;
			G03 = (-1 * pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1) / 6;
			G13 = (3 * pow(t, 3) - 6 * pow(t, 2) + 4) / 6;
			G23 = (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1) / 6;
			G33 = pow(t, 3) / 6;
			pos.x = G03 * ctrlP[k].x + G13 * ctrlP[k + 1].x + G23 * ctrlP[k + 2].x + G33 * ctrlP[k + 3].x;
			pos.y = G03 * ctrlP[k].y + G13 * ctrlP[k + 1].y + G23 * ctrlP[k + 2].y + G33 * ctrlP[k + 3].y;
			glVertex2f(pos.x, pos.y);
		}
	}
	glEnd();
}

