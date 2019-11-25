#ifndef TEXSHADER_HPP
#define TEXSHADER_HPP

#pragma once

#include "shader.hpp"
#include "texture.hpp"

class texShader : public fragmentShader {
public:
    texShader(texture * _tex)
        : fragmentShader(),

          tex (_tex)
    {
    }

    virtual color fragment(const Mesh::vertex &v0) override;

    texture * tex;
};

color texShader::fragment(const Mesh::vertex &v0)
{
    vector3f cc;

    cc = tex->getPixel (v0.tex.x, v0.tex.y);

    color col = {
    static_cast<uint8_t>(255 * cc.x),
    static_cast<uint8_t>(255 * cc.z),
    static_cast<uint8_t>(255 * cc.y),
    static_cast<uint8_t>(255)};

	col = { 255, 255, 255, 255 };

    return col;
}


#endif // TEXSHADER_HPP
