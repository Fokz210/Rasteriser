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

                   float   width()                           const noexcept;
                   float   height()                          const noexcept;
    virtual        void    clear()                                 noexcept;
    virtual        void    update()                                noexcept;

protected:

                   unsigned _width;
                   unsigned _height;
                   color *  _fbc;
};

context::context()
    : _width(1920),
      _height(1080),
      _fbc(static_cast<color *>(std::calloc(static_cast<size_t>(_width * _height), sizeof(color))))
{
}

context::context (unsigned width, unsigned height)
    : _width (width),
      _height (height),
      _fbc(static_cast<color*>(std::calloc (static_cast<size_t>(_width * _height), sizeof(color))))
{

}

context::~context()
{
	std::free(_fbc);
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
    std::wmemset(reinterpret_cast<wchar_t *>(_fbc), static_cast<wchar_t>(0xff000000), static_cast<size_t>(_width * _height));
}

inline void context::update() noexcept
{
	int framebuffer = open("/dev/fb0", O_WRONLY);

	assert(framebuffer != -1);

    write(framebuffer, _fbc, static_cast<size_t>(_width * _height) * sizeof(color));
	close(framebuffer);
}

inline float context::width() const noexcept
{
    return _width;
}

inline float context::height() const noexcept
{
    return _height;
}


#endif //CONTEXTHPP
