#include <cmath>
#include <iostream>

#include "mouse.hpp"
#include "perspectiveShader.hpp"
#include "phongShader.hpp"
#include "pipeline.hpp"
#include "oscontext.hpp"

#include "texShader.hpp"

int main(int argv, char **argc)
{
    WindowContext * c = new WindowContext;

    c->clear();

	TriangleRasterizer trRast;

    trRast.set_viewport(0, 0, static_cast<int>(c->width()), static_cast<int>(c->height()));

	std::vector<TriangleRasterizer::output> out;

    float screenRatio = static_cast<float>(c->width()) / c->height();
    float near = 0.2f;
    float far = 100000.f;

	float c1 = (far + near) / (far - near);
	float c2 = 2.f * near * far / (far - near);

    vector3f localPos = {0.f, -1.f, 0.f};
    vector3f light = normalize(vector3f(-0.2f, 0.f, 1.f));

    texture sky;
    sky.readPPM ("sky/sky.ppm");

    texture A6M;
    A6M.readPPM ("A6M/A6M.ppm");

    rotateShader vShader(mat3f(), localPos, light, screenRatio, c1, c2);

    phongShader fShader(light, {}, {0.1f, 0.8f, 1.f}, {0.7f, 1.f, 1.f}, &A6M);
    texShader tfShader(&sky);

    float x = 0,
	    y = 0;

	mouse ms;
	mouse::event event;

    Mesh sky_mesh = import_obj("sky/sky.obj");
    Mesh A6M_mesh = import_obj("A6M/A6M.obj");


    for (auto && el : sky_mesh.verts)
    {
        el.pos = el.pos * 100;
    }

	thread_pool pool;

    pipeline skypipe(c, &tfShader, &vShader, &trRast);
    pipeline a6mpipe(c, &fShader, &vShader, &trRast);

    skypipe.localPos = localPos;
    a6mpipe.localPos = localPos;

    skypipe.faces_out = false;

	while (c->is_open()) {
		x += 2.f;
		/*while (ms.poll(event)) {
			x += event.dx () - 2.f;
            y += event.dy ();

			if (event.right ())
				c->close ();
		}*/

		float s = 0.005f;
		float phi = s * x;
		float theta = s * y;

        vector3f direction = {
		std::cos(theta) * std::sin(phi),
		std::sin(theta),
		std::cos(theta) * std::cos(phi),
        };

        mat3f mat = rotate(direction, {0.f, 1.f, 0.f});

        vector3f cameraPos = direction * 5.f;
        light = normalize(vector3f(0.f, 5.f, 5.f));

        fShader.light = light;
		fShader.camPos = cameraPos;

		vShader.cameraPos = cameraPos;
		vShader.matrix = mat;

        skypipe.camPos = cameraPos;
        a6mpipe.camPos = cameraPos;

        c->clear();

        //skypipe.run(sky_mesh);
        a6mpipe.run(A6M_mesh);

        c->update();
	}

	pool.finish();

	return 0;
}
