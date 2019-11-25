#include <cmath>
#include <iostream>

#include <X11/XKBlib.h>

#include "mouse.hpp"
#include "perspectiveShader.hpp"
#include "phongShader.hpp"
#include "pipeline.hpp"
#include "xwindow.hpp"

#include "texShader.hpp"

int main(int argv, char **argc)
{
    context * c = nullptr;

    if (argv > 1 && !strcmp (argc[1], "--tty"))
        c = new context;
    else
        c = new XWindow;

    c->clear();

    TriangleRasterizer skyRast;
    TriangleRasterizer a6mRast;

    skyRast.mode = TriangleRasterizer::faces_in;
    a6mRast.mode = TriangleRasterizer::faces_out;

    a6mRast.set_viewport(0, 0, static_cast<int>(c->width()), static_cast<int>(c->height()));
    skyRast.set_viewport(0, 0, static_cast<int>(c->width()), static_cast<int>(c->height()));

	std::vector<TriangleRasterizer::output> out;

    float screenRatio = static_cast<float>(c->width()) / c->height();
    float near = 0.2f;
    float far = 100000.f;

	float c1 = (far + near) / (far - near);
	float c2 = 2.f * near * far / (far - near);

    vector3f localPos = {0.f, 0.f, 0.f};
    vector3f light = normalize(vector3f(-0.2f, 0.f, 1.f));

    texture sky;
    sky.loadPPM ("sky/sky.ppm");

    texture A6M;
    A6M.loadPPM ("A6M/A6M.ppm");

    rotateShader vShader(mat3f(), localPos, light, screenRatio, c1, c2);

    phongShader fShader(light, {}, {0.1f, 0.8f, 1.f}, {0.7f, 1.f, 1.f}, &A6M);
    texShader tfShader(&sky);

    int x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

    Mesh sky_mesh = import_obj("sky/sky.obj");
    Mesh A6M_mesh = import_obj("A6M/A6M.obj");

    for (auto && el : sky_mesh.verts)
    {
        el.pos = el.pos * 1000;
    }

    pipeline skypipe(c, &tfShader, &vShader, &skyRast);
    pipeline a6mpipe(c, &fShader, &vShader, &a6mRast);

    skyRast.localPos = localPos;
    a6mRast.localPos = localPos;

	bool closeWindow = false;

	while (!closeWindow) {
		x += 2.f;
		while (ms.poll(event)) {
			x += event.dx - 2.f;
            y += event.dy;

            //if (event.middle ())
            //	closeWindow = true;
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

        vector3f cameraPos = direction * 20.f;
        light = normalize(vector3f(0.f, 5.f, 5.f));

        fShader.light = light;
		fShader.camPos = cameraPos;

		vShader.cameraPos = cameraPos;
		vShader.matrix = mat;

        a6mRast.camPos = cameraPos;
        skyRast.camPos = cameraPos;

        c->clear();

        skypipe.run(sky_mesh);
        a6mpipe.run(A6M_mesh);

        c->update();
	}

    return 0;
}
