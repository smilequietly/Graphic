#pragma once
#include <vector>
#include <GL/glut.h>

namespace Qs {
	static class GraphicDraw {

	public:
		template<class T>
		struct Pos {
			T x, y;
			Pos(T x_, T y_) : x(x_), y(y_) {}
			Pos(const Pos& p) : x(p.x), y(p.y) {}
		};

		//��ֱ��
		static void DDALine(int x0, int y0, int x1, int y1);
		static void MidBresenhamLine(int x0, int y0, int x1, int y1);
		static void ImprovedBresenhamLine(int x0, int y0, int x1, int y1);
		//��Բ
		static void MidBresenhamCircle(int x0, int y0, int r);
		//��������
		static void ActiveEdgeTable(std::vector<Pos<int>> p, std::vector<std::pair<Pos<int>, Pos<int>>>& Lines);
		static void AET_Start(std::vector<std::pair<Pos<int>, Pos<int>>>& Lines);
		//����ü��㷨
		static void CohenSurtherland(const Pos<double>& lb, const Pos<double>& rt, const Pos<double>& src, const Pos<double>& dst);
		//���Ѷ��㷨
		static void LBLineClip(const Pos<double>& lb, const Pos<double>& rt, const Pos<double>& src, const Pos<double>& dst);
		//Bezir��������
		static void BezierCurve(const GLint num, const std::vector<Pos<GLdouble>>& ctrlP);
		//3��B��������
		static void NURBS_Curve3(const GLint num, const std::vector<Pos<GLdouble>>& ctrlP);
	
	private:
		static void CirclePoint(int x0, int y0, int x, int y);
		static void GetBezierP(Pos<GLdouble>& curvePos, const GLdouble t, const std::vector<GLint>& Cnk, const std::vector<Pos<double>>& ctrlP);
	};
}