#include <cmath>
#include <iostream>

#include <X11/XKBlib.h>

#include "mouse.hpp"
#include "perspectiveShader.hpp"
#include "phongShader.hpp"
#include "pipeline.hpp"
#include "xwindow.hpp"

#include "texture.hpp"

int main(int argv, char **argc)
{
    context * c = nullptr;

    if (argv > 1 && !strcmp (argc[1], "--tty"))
        c = new context;
    else
        c = new XWindow;

    c->clear();

	TriangleRasterizer trRast;

    trRast.set_viewport(0, 0, static_cast<int>(c->width()), static_cast<int>(c->height()));

	std::vector<TriangleRasterizer::output> out;

    float screenRatio = static_cast<float>(c->width()) / c->height();
	float near = 0.2f;
	float far = 5.f;

	float c1 = (far + near) / (far - near);
	float c2 = 2.f * near * far / (far - near);

    vector3f localPos = {0.f, -1.f, 0.f};
    vector3f light = normalize(vector3f(0.f, -1.f, 0.f));

    texture tex;
    tex.readPPM ("texture.ppm");

	rotateShader vShader(mat3f(), localPos, light, screenRatio, c1, c2);
    phongShader fShader(light, {}, {0.1f, 0.8f, 1.f}, {0.7f, 1.f, 1.f}, &tex);

    int x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

    Mesh mesh = import_obj("cat.obj");

	thread_pool pool;

    pipeline pipe(c, &fShader, &vShader, &trRast);

	bool closeWindow = false;

	while (!closeWindow) {
		x += 2.f;
		while (ms.poll(event)) {
			x += event.dx - 2.f;
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
        light = normalize(vector3f(0.f, 5.f, 5.f));

		fShader.light = mat * light;
		fShader.camPos = cameraPos;

		vShader.cameraPos = cameraPos;
		vShader.matrix = mat;

        c->clear();

        pipe.run(mesh);

        c->update();
	}

	pool.finish();
}
