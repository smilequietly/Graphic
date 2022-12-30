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

		//画直线
		static void DDALine(int x0, int y0, int x1, int y1);
		static void MidBresenhamLine(int x0, int y0, int x1, int y1);
		static void ImprovedBresenhamLine(int x0, int y0, int x1, int y1);
		//画圆
		static void MidBresenhamCircle(int x0, int y0, int r);
		//多边形填充
		static void ActiveEdgeTable(std::vector<Pos<int>> p, std::vector<std::pair<Pos<int>, Pos<int>>>& Lines);
		static void AET_Start(std::vector<std::pair<Pos<int>, Pos<int>>>& Lines);
		//编码裁剪算法
		static void CohenSurtherland(const Pos<double>& lb, const Pos<double>& rt, const Pos<double>& src, const Pos<double>& dst);
		//梁友栋算法
		static void LBLineClip(const Pos<double>& lb, const Pos<double>& rt, const Pos<double>& src, const Pos<double>& dst);
		//Bezir曲线生成
		static void BezierCurve(const GLint num, const std::vector<Pos<GLdouble>>& ctrlP);
		//3次B样条曲线
		static void NURBS_Curve3(const GLint num, const std::vector<Pos<GLdouble>>& ctrlP);
	
	private:
		static void CirclePoint(int x0, int y0, int x, int y);
		static void GetBezierP(Pos<GLdouble>& curvePos, const GLdouble t, const std::vector<GLint>& Cnk, const std::vector<Pos<double>>& ctrlP);
	};
}