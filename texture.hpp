#pragma once

#include <stdint.h>
#include <stddef.h>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <cmath>
#include <mutex>
#include "geometry.hpp"

#define PTRCHECK(ptr)  if (!ptr) throw std::runtime_error("error: pointer is not valid")

struct color_gbr
{
    uint8_t g, b, r;
};

class texture
{
public:
    using color = color_gbr;

                    texture ();
                    texture (const texture & other)    = delete;
          texture & operator = (const texture & other) = delete;

                    ~texture ();

          void      create (size_t sX, size_t sY);
          void      deallocate ();
          void      copy (const texture & other);
          void      readPPM (const char * filename);

    const color *   getPixels()                          const;
          size_t    getWidth()                           const noexcept;
          size_t    getHeight()                          const noexcept;

          color *   operator [] (unsigned y)                   noexcept;
    const color *   operator [] (unsigned y)             const noexcept;

          vector3f  getPixel (const float & x, const float & y)                noexcept;

          std::mutex * mutexes;

          color *   pixels;
          size_t    sizeX, sizeY;
};


texture::texture () :
    pixels (nullptr),
    sizeX (),
    sizeY ()
{

}

texture::~texture ()
{
    if (pixels)
        delete[] pixels;
}

void texture::create(size_t sX, size_t sY)
{
    if (pixels)
        deallocate ();

    sizeX = sX;
    sizeY = sY;

    pixels = new color[sizeX * sizeY];
    mutexes = new std::mutex[sizeY];
}

void texture::deallocate ()
{
    PTRCHECK (pixels);

    sizeX = 0;
    sizeY = 0;

    delete[] pixels;
    delete[] mutexes;
}

void texture::copy (const texture & other)
{
    if (other.sizeX != sizeX || other.sizeY != sizeY)
        throw std::runtime_error("texture copy error: different texture sizes are not supported");

    memcpy (reinterpret_cast<void*>(pixels), reinterpret_cast<void*>(other.pixels), sizeof(color) * sizeX * sizeY);
}

void texture::readPPM(const char * filename)
{
    FILE * file = fopen (filename, "r");

    PTRCHECK (file);

    char format[16] = "";

    fscanf (file, "%9s", format);

    if (strcmp (format, "P6"))
        throw std::runtime_error("PPM reading error: file format not supported");

    int sizex = 0, sizey = 0;
    int max = 0;

    fscanf (file, "%d %d %d ", &sizex, &sizey, &max);

    create (static_cast<size_t>(sizex), static_cast<size_t>(sizey));
    fread (reinterpret_cast<void*>(pixels), sizeof(pixels), static_cast<size_t>(sizex * sizey), file);
}

const texture::color * texture::getPixels() const
{
    return pixels;
}

size_t texture::getWidth() const noexcept
{
    return sizeX;
}

size_t texture::getHeight() const noexcept
{
    return sizeY;
}

texture::color * texture::operator[](unsigned int y) noexcept
{
    return pixels + sizeX * (sizeY - y - 1);
}

const texture::color * texture::operator[](unsigned int y) const noexcept
{
    return pixels + sizeX * (sizeY - y - 1);
}

vector3f texture::getPixel(const float & x, const float & y) noexcept
{
    color col =  (*this)[static_cast<unsigned>(std::lround(y * sizeY)) % sizeY][static_cast<unsigned>(std::lround(x * sizeX)) % sizeX];
    return vector3f(static_cast<float>(col.r) / 255, static_cast<float>(col.g) / 255, static_cast<float>(col.b) / 255);
}
