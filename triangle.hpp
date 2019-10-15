#ifndef TRIANGLEHPP
#define TRIANGLEHPP

#include <iostream>
#include <vector>

#include "context.hpp"
#include "geometry.hpp"
#include "line.hpp"
#include "matrix.hpp"

class triangleRast {
public:
	triangleRast(unsigned width, unsigned height);

	void rasterise(vector3f coords[3], color col, context &c);

protected:
	vector3f bar(vector2i *points, vector2i P);

	unsigned _width, _height;
	lineRast rast;
};

triangleRast::triangleRast(unsigned width, unsigned height)
	: _width(width),
	  _height(height),
	  rast({0, 0, static_cast<int>(width), static_cast<int>(height)})
{
}

vector3f triangleRast::bar(vector2i *points, vector2i P)
{
	vector3f u = cross(vector3f(points[2][0] - points[0][0], points[1][0] - points[0][0], points[0][0] - P[0]),
			   vector3f(points[2][1] - points[0][1], points[1][1] - points[0][1], points[0][1] - P[1]));

	if (std::abs(u.z) < 1)
		return vector3f(-1.f, -1.f, -1.f);

	return vector3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangleRast::rasterise(vector3f coords[3], color col, context &c)
{
	vector2i rectmin(_width - 1, _height - 1);
	vector2i rectmax(0, 0);

	vector2i pts[3] = {};

	for (int i = 0; i < 3; i++) {
		pts[i][0] = rast.xToInt(coords[i][0]);
		pts[i][1] = rast.yToInt(coords[i][1]);
	}

	vector2i clamp(_width - 1, _height - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			rectmin[j] = std::max(0, std::min(rectmin[j], pts[i][j]));
			rectmax[j] = std::min(clamp[j], std::max(rectmax[j], pts[i][j]));
		}
	}

	vector2i P;
	for (P.x = rectmin.x; P.x <= rectmax.x; P.x++) {
		for (P.y = rectmin.y; P.y <= rectmax.y; P.y++) {
			vector3f bc = bar(pts, P);

			if (bc.x < 0 || bc.y < 0 || bc.z < 0)
				continue;
			c[P.y][P.x] = col;
		}
	}
}

#endif //TRIANGLEHPP
