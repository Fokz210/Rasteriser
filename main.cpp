#include <cmath>
#include <iostream>

#include "mouse.hpp"
#include "perspectiveShader.hpp"
#include "phongShader.hpp"
#include "pipeline.hpp"

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

	rotateShader vShader(mat3f(), localPos, light, screenRatio, c1, c2);
	phongShader fShader(light, {}, {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f});

	int x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

	Mesh mesh = import_obj(argv > 1 ? argc[1] : "cat.obj");

	thread_pool pool;
	pool.start_threads(8);

	pipeline pipe(&c, &fShader, &vShader, &trRast, &mesh, &pool);

	bool closeWindow = false;

	while (!closeWindow) {
		while (ms.poll(event)) {
			x += event.dx;
			y += event.dy;

			if (event.right())
				closeWindow = true;
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
		light = vector3f(0.f, 1.f, 0.f);

		fShader.light = light;
		fShader.camPos = cameraPos;

		vShader.cameraPos = cameraPos;
		vShader.matrix = mat;

		c.clear();

		pipe.run();

		c.update();
	}

	pool.finish();
}
