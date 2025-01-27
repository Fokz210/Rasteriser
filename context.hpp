#ifndef CONTEXTHPP
#define CONTEXTHPP

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cwchar>

#ifdef __unix
extern "C" {
#include <fcntl.h>
#include <unistd.h>
}
#endif

#include "texture.hpp"

struct color {
    std::uint8_t b, g, r, a;
};

class context {
public:
                           context();
                           context(unsigned width, unsigned height);
    virtual               ~context();
                           context(const context & )               = delete;
                           context(      context &&)               = delete;

                   color * operator[](unsigned y)                  noexcept;
             const color * operator[](unsigned y)            const noexcept;

                   unsigned  width()                           const noexcept;
                   unsigned  height()                          const noexcept;
    virtual        void    clear()                                 noexcept;
    virtual        void    update()                                noexcept;

                   float *  zbuffer;
protected:

                   unsigned _width;
                   unsigned _height;
                   color *  _fbc;
};

context::context()
    : zbuffer (new float [1920 * 1080]),
      _width(1920),
      _height(1080),
      _fbc(static_cast<color *>(std::calloc(static_cast<size_t>(_width * _height), sizeof(color))))
{
}

context::context (unsigned width, unsigned height)
    : zbuffer (new float [width * height]),
      _width (width),
      _height (height),
      _fbc(static_cast<color*>(std::calloc (static_cast<size_t>(_width * _height), sizeof(color))))
{

}

context::~context()
{
	std::free(_fbc);
    delete[] zbuffer;
}

color *context::operator[](unsigned y) noexcept
{
	return _fbc + _width * (_height - y - 1);
}

inline const color *context::operator[](unsigned y) const noexcept
{
	return _fbc + _width * (_height - y - 1);
}

inline void context::clear() noexcept
{
    std::wmemset(reinterpret_cast<wchar_t *>(_fbc), static_cast<wchar_t>(0xff000000), static_cast<size_t>(_width * _height) * 2);

    for (auto y = 0u; y < _height; y++)
        for (auto x = 0u; x < _width; x++)
            zbuffer [_width * y + x] = 1.f;
}

inline void context::update() noexcept
{
#ifdef __unix
	int framebuffer = open("/dev/fb0", O_WRONLY);

	assert(framebuffer != -1);

    write(framebuffer, _fbc, static_cast<size_t>(_width * _height) * sizeof(color));
	close(framebuffer);
#endif
}

inline unsigned context::width() const noexcept
{
    return _width;
}

inline unsigned context::height() const noexcept
{
    return _height;
}


#endif //CONTEXTHPP
