#include "context.hpp"
#include "line.hpp"
#include <iostream>

typedef vector2<float> vector2f;

int main()
{
	context c;
	c.clear();

	lineRast rast({0, 0, 1920, 1080});

	std::vector<vector2i> vecArray;

	rast.rasterise({-0.5f, -0.5f, 0.f}, {0.5f, -0.5f, 0.f}, vecArray);
	rast.rasterise({0.5f, -0.5f, 0.f}, {0.5f, 0.5f, 0.f}, vecArray);
	rast.rasterise({0.5f, 0.5f, 0.f}, {-0.5f, 0.5f, 0.f}, vecArray);
	rast.rasterise({-0.5f, 0.5f, 0.f}, {-0.5f, -0.5f, 0.f}, vecArray);

	c.clear();

	while (true) {
		for (auto &&el : vecArray) {
			assert(el.x <= 1920);
			assert(el.y <= 1080);
			assert(el.x >= 0);
			assert(el.y >= 0);

			c[el.y][el.x] = {255, 255, 255, 255};
		}
		c.update();
		c.clear();
	}

	return 0;
}
