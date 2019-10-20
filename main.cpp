#include "colors.hpp"
#include "context.hpp"
#include "line.hpp"
#include "matrix.hpp"
#include "mouse.hpp"
#include "obj.hpp"
#include "perspectiveShader.hpp"
#include "phongShader.hpp"
#include "trirast.hpp"
#include <iostream>

typedef vector2<float> vector2f;

Mesh::vertex mix(Mesh::vertex const v[3], float const b, float const c) noexcept
{
	float const a = 1.f - b - c;

	Mesh::vertex out;
	float *f = reinterpret_cast<float *>(&out);
	float const *const f0 = reinterpret_cast<float const *>(v);
	float const *const f1 = reinterpret_cast<float const *>(v + 1);
	float const *const f2 = reinterpret_cast<float const *>(v + 2);
	for (unsigned int i = 0u; i < sizeof(Mesh::vertex) / sizeof(float); ++i)
		f[i] = a * f0[i] + b * f1[i] + c * f2[i];
	return out;
}

int main(int argv, char **argc)
{
	context c;
	c.clear();

	TriangleRasterizer trRast;

	trRast.set_viewport(0, 0, static_cast<int>(c._width), static_cast<int>(c._height));

	std::vector<TriangleRasterizer::output> out;

	float screenRatio = static_cast<float>(c._width) / c._height;
	float near = 0.2f;
	float far = 5.f;

	float c1 = (far + near) / (far - near);
	float c2 = 2.f * near * far / (far - near);

	vector3f localPos = {0.f, -1.f, 0.f};
	vector3f light = normalize(vector3f(0.f, 1.f, 0.f));

	float *depth = new float[1920 * 1080];

	int x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

	Mesh mesh = import_obj(argv > 1 ? argc[1] : "cat.obj");

	float intenscale = 1.f;

	while (true) {
		while (ms.poll(event)) {
			x += event.dx;
			y += event.dy;

			if (event.left())
				intenscale -= 0.1;
			else if (event.right())
				intenscale += 0.1;
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

		vector3f cameraPos = direction * 1.8f;

		light = mat * vector3f(0.f, 1.f, 0.f);

		rotateShader vShader(mat, localPos, cameraPos, screenRatio, c1, c2);
		phongShader fShader(light, cameraPos, {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f});

		c.clear();

		for (int y = 0; y < 1080; y++)
			for (int x = 0; x < 1920; x++)
				depth[y * 1920 + x] = 1.f;

		for (auto i = 0u; i < mesh.inds.size(); i += 3) {
			Mesh::vertex const v[3] = {
			mesh.verts[mesh.inds[i]],
			mesh.verts[mesh.inds[i + 1]],
			mesh.verts[mesh.inds[i + 2]]};

			vector4f p[3] = {};

			for (int i = 0; i < 3; ++i)
				p[i] = vShader.vertex(v[i]);

			trRast.rasterize(p, out);

			for (TriangleRasterizer::output el : out) {

				if (/*el.depth < -1.f ||*/ el.depth > depth[el.y * 1920 + el.x])
					continue;

				depth[el.y * 1920 + el.x] = el.depth;
				Mesh::vertex v0 = mix(v, el.b, el.c);

				c[el.y][el.x] = fShader.fragment(v0);
			}

			out.clear();
		}
		c.update();
	}
	return 0;
}
