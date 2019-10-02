#ifndef LINEHPP
#define LINEHPP

#include <cmath>
#include <vector>

#include "geometry.hpp"

class lineRast {
public:
	lineRast(intRect offset);

	void setOffset(intRect offset);
	void rasterise(const vector3f &p0, const vector3f &p1, std::vector<vector2i> &out);

	float xToFloat(int x);
	float yToFloat(int y);
	int xToInt(float x);
	int yToInt(float y);

private:
	floatRect _offset;
};

lineRast::lineRast(intRect offset)
	: _offset(offset.left + 0.5f, offset.top + 0.5f, offset.width * 0.5f, offset.height * 0.5f)
{
}

void lineRast::setOffset(intRect offset)
{
	_offset.left = offset.left + 0.5f;
	_offset.top = offset.top + 0.5f;
	_offset.width = offset.width * 0.5f;
	_offset.height = offset.height * 0.5f;
}

float lineRast::xToFloat(int x)
{
	return (x - _offset.left) / _offset.width - 1.f;
}

float lineRast::yToFloat(int y)
{
	return (y - _offset.top) / _offset.height - 1.f;
}

int lineRast::xToInt(float x)
{
	return std::lround((x + 1.f) * _offset.width + _offset.left - 1.f);
}

int lineRast::yToInt(float y)
{
	return std::lround((y + 1.f) * _offset.height + _offset.top - 1.f);
}

void lineRast::rasterise(const vector3f &p0, const vector3f &p1, std::vector<vector2i> &out)
{
	const vector3f delta = p1 - p0;

	auto clamp = [](float x) {
		float eps = 1e-6;
		return x >= 1.f ? 1.f - eps : (x <= -1.f ? -1.f + eps : x);
	};

	int xmin = xToInt(clamp(std::min(p0.x, p1.x)));
	int xmax = xToInt(clamp(std::max(p0.x, p1.x)));
	int ymin = yToInt(clamp(std::min(p0.y, p1.y)));
	int ymax = yToInt(clamp(std::max(p0.y, p1.y)));

	if (std::abs(delta.x) > std::abs(delta.y))
		for (int x = xmin; x <= xmax; x++) {
			float t = (xToFloat(x) - p0.x) / delta.x;
			int y = yToInt(p0.y + delta.y * t);

			if (y >= ymin && y <= ymax)
				out.push_back(vector2i(x, y));
		}
	else
		for (int y = ymin; y <= ymax; y++) {
			float t = (yToFloat(y) - p0.y) / delta.y;
			int x = xToInt(p0.x + delta.x * t);

			if (x >= xmin && x <= xmax)
				out.push_back(vector2i(x, y));
		}
}

#endif //LINEHPP
