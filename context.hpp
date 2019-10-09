#ifndef CONTEXTHPP
#define CONTEXTHPP

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
extern "C" {
#include <fcntl.h>
#include <unistd.h>
}

#ifndef CONTEXT_WIDTH
#define CONTEXT_WIDTH 1920u
#endif //CONTEXT_WIDTH

#ifndef CONTEXT_HEIGHT
#define CONTEXT_HEIGHT 1080u
#endif //CONTEXT_HEIGHT

struct color {
	std::uint8_t b, g, r, a;
};

class context {
public:
	context();
	~context();

	color *operator[](const std::uint32_t y);
	const color *operator[](const std::uint32_t y) const;

	void clear();
	void update();

	const std::uint32_t _width;
	const std::uint32_t _height;
	color *_fbc;
};

context::context()
	: _width(CONTEXT_WIDTH),
	  _height(CONTEXT_HEIGHT),
	  _fbc(static_cast<color *>(std::calloc(_width * _height, sizeof(color))))
{
}

context::~context()
{
	std::free(_fbc);
}

color *context::operator[](std::uint32_t y)
{
	return _fbc + _width * (_height - y - 1);
}

const color *context::operator[](std::uint32_t y) const
{
	return _fbc + _width * (_height - y - 1);
}

void context::clear()
{
	std::wmemset(reinterpret_cast<wchar_t *>(_fbc), 0xff000000, _width * _height);
}

void context::update()
{
	int framebuffer = open("/dev/fb0", O_WRONLY);

	assert(framebuffer != -1);

	write(framebuffer, _fbc, _width * _height * sizeof(color));
	close(framebuffer);
}

#endif //CONTEXTHPP
