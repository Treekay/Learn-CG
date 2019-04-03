#pragma once

#include <vector>
#include <cmath>

using namespace std;

class Bresenham {
public:
	static vector<int> drawTriangle(int vertices[]) {
		vector<int> points;
		// ��������
		int x1 = vertices[0], y1 = vertices[1];
		int x2 = vertices[2], y2 = vertices[3];
		int x3 = vertices[4], y3 = vertices[5];
		// �ֱ��ÿ���������� BresenHam �㷨����߶��ϵĵ�
		Bresenham::getLinePoints(x1, y1, x2, y2, points);
		Bresenham::getLinePoints(x1, y1, x3, y3, points);
		Bresenham::getLinePoints(x2, y2, x3, y3, points);
		return points;
	}

	static void getLinePoints(int start_x, int start_y, int end_x, int end_y, vector<int> &points) {
		points.push_back(start_x);
		points.push_back(start_y);
		// ��� dx��dy
		int dx = end_x - start_x;
		int dy = end_y - start_y;
		// ����б��ȷ��x��y����㵽�յ�ÿ���ǵ������ߵݼ�
		int step_x = dx > 0 ? 1 : -1;
		int step_y = dy > 0 ? 1 : -1;
		int x = start_x, y = start_y;
		if (abs(dx) > abs(dy)) {
			int p = 2 * abs(dy) - abs(dx);
			while(x != end_x) {
				points.push_back(x);
				points.push_back(y);
				if (p <= 0) {
					p += 2 * abs(dy);
				}
				else {
					y += step_y;
					p += 2 * abs(dy) - 2 * abs(dx);
				}
				x += step_x;
			}
		}
		else {
			int p = 2 * abs(dx) - abs(dy);
			while (y != end_y) {
				points.push_back(x);
				points.push_back(y);
				if (p <= 0) {
					p += 2 * abs(dx);
				}
				else {
					x += step_x;
					p += 2 * abs(dx) - 2 * abs(dy);
				}
				y += step_y;
			}
		}
		points.push_back(end_x);
		points.push_back(end_y);
	}

	static vector<int> drawCircle(int x0, int y0, int r) {
		vector<int> points;
		// ��һ����ѡȡԲ�����Ϸ� (0, r) ��Ϊ (x0, y0)
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		while (x < y) {
			// ���� Bresenham �㷨�ĵ��Ʒ������� p
			if (p < 0) {
				p = p + 4 * x + 6;
			}
			else {
				p = p + 4 * (x - y) + 10;
				y--;
			}
			x++;
			points.push_back(x0 + x); points.push_back(y0 + y);
			points.push_back(x0 + y); points.push_back(y0 + x);
			points.push_back(x0 + x); points.push_back(y0 - y);
			points.push_back(x0 - y); points.push_back(y0 + x);
			points.push_back(x0 - x); points.push_back(y0 + y);
			points.push_back(x0 + y); points.push_back(y0 - x);
			points.push_back(x0 - x); points.push_back(y0 - y);
			points.push_back(x0 - y); points.push_back(y0 - x);
		}
		return points;
	}

	// ֱ�߷���: Ax + By + C = 0
	static vector<int> lineEquation(int x1, int y1, int x2, int y2) {
		vector<int> res;
		int A = y2 - y1;
		int B = x1 - x2;
		int C = x2 * y1 - x1 * y2;
		res.push_back(A);
		res.push_back(B);
		res.push_back(C);
		return res;
	}

	// �����ι�դת���㷨��䱳����ɫ
	static vector<int> RasterizeTriangle(int vertices[]) {
		int x1 = vertices[0], y1 = vertices[1];
		int x2 = vertices[2], y2 = vertices[3];
		int x3 = vertices[4], y3 = vertices[5];
		vector<vector<int>> lines;
		lines.push_back(lineEquation(x1, y1, x2, y2));
		lines.push_back(lineEquation(x1, y1, x3, y3));
		lines.push_back(lineEquation(x3, y3, x2, y2));
		int x_temp[3] = { x3, x2, x1 };
		int y_temp[3] = { y3, y2, y1 };
		// ���Ļ�����
		for (int i = 0; i < 3; i++) {
			// Ax + By + C
			if (lines[i][0] * x_temp[i] + lines[i][1] * y_temp[i] + lines[i][2] < 0) {
				for (int j = 0; j < lines[i].size(); j++) {
					lines[i][j] *= -1;
				}
			}
		}
		// ���������ΰ�Χ���е����е㣬�ҳ��������������ڵĵ�
		int min_x = min(x1, min(x2, x3));
		int max_x = max(x1, max(x2, x3));
		int min_y = min(y1, min(y2, y3));
		int max_y = max(y1, max(y2, y3));
		vector<int> pixels;
		for (int x = min_x; x <= max_x; x++) {
			for (int y = min_y; y <= max_y; y++) {
				bool isInside = true;
				for (int i = 0; i < 3; i++) {
					if (lines[i][0] * x + lines[i][1] * y + lines[i][2] < 0) {
						isInside = false;
						break;
					}
				}
				if (isInside) {
					// ���
					pixels.push_back(x);
					pixels.push_back(y);
				}
			}
		}
		return pixels;
	}
};