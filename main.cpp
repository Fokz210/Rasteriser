#include "colors.hpp"
#include "context.hpp"
#include "line.hpp"
#include "matrix.hpp"
#include "mouse.hpp"
#include "obj.hpp"
#include <iostream>

typedef vector2<float> vector2f;

int main()
{
	context c;
	c.clear();

	lineRast rast({0, 0, static_cast<int>(c._width), static_cast<int>(c._height)});
	std::vector<vector2i> out;

	float screenRatio = static_cast<float>(c._width) / c._height;

	vector3f cameraPos = {0.f, 0.f, 2.f},
		 localPos = {0.f, 1.f, 0.f};

	int x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

	Mesh mesh = import_obj("cat.obj");

	while (true) {
		while (ms.poll(event)) {
			x += event.dx;
			y += event.dy;
		}

		float s = 0.005f;
		float phi = s * x;
		float theta = s * y;

		vector3f direction = {
		std::cos(theta) * std::sin(phi),
		std::sin(theta),
		std::cos(theta) * std::cos(phi),
		};

		mat3f mat = rotate(direction, {0.f, 1.f, 0.f});
		c.clear();

		for (auto i = 0u; i < mesh.inds.size(); i += 3) {
			Mesh::vertex const v[3] =
			{
			mesh.verts[mesh.inds[i]],
			mesh.verts[mesh.inds[i + 1]],
			mesh.verts[mesh.inds[i + 2]]};

			vector3f p[3];
			for (int i = 0; i < 3; ++i) {
				p[i] = (mat * (v[i].pos - localPos)) - cameraPos;
				p[i].x /= -screenRatio * p[i].z;
				p[i].y /= -p[i].z;
			}

			rast.rasterise(p[0], p[1], out);
			rast.rasterise(p[1], p[2], out);
			rast.rasterise(p[2], p[0], out);
			for (auto const &p : out)
				c[p.y][p.x] = C_GREEN;
			out.clear();
		}
		c.update();
	}

	return 0;
}
