#include "context.hpp"
#include "vector2.hpp"
#include <iostream>

typedef vector2<float> vector2f;

int main()
{
	context c;

	c.clear();

	while (true) {

		for (int y = 100; y < 400; y++)
			for (int x = 300; x < 700; x++)
				c[x][y] = {255, 255, 255, 255};

		c.update();
	}

	return 0;
}
