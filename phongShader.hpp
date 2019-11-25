#pragma once

#include "shader.hpp"
#include "texture.hpp"

class phongShader : public fragmentShader {
public:
    phongShader(vector3f _light, vector3f _camPos, vector3f _objColor, vector3f _lightColor, texture * _tex)
        : fragmentShader(),
		  light(_light),
		  camPos(_camPos),
		  objColor(_objColor),
          lightColor(_lightColor),
          tex (_tex)
	{
	}

	virtual color fragment(const Mesh::vertex &v0) override;

    vector3f light;
    vector3f camPos;

    vector3f objColor;
    vector3f lightColor;

    texture * tex;
};

color phongShader::fragment(const Mesh::vertex &v0)
{
    vector3f n = v0.norm;

    float NL = std::max(0.f, dot(n, light));
    vector3f cam = normalize(camPos - v0.pos);
    vector3f hw = normalize(cam + light);

    float spec = std::max(0.f, dot(v0.norm, hw));

	for (int i = 0; i < 4; i++)
        spec *= spec;

    vector3f cc = tex->getPixel (v0.tex.x, v0.tex.y) * (0.2f + 0.6f * NL) + lightColor * (0.4f * spec);

	color col = {
    static_cast<uint8_t>(255 * cc.x),
    static_cast<uint8_t>(255 * cc.z),
    static_cast<uint8_t>(255 * cc.y),
	static_cast<uint8_t>(255)};

	return col;
}
