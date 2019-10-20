#pragma once

#include "shader.hpp"

class rotateShader : public vertexShader {
public:
	rotateShader(mat3f _matrix, vector3f _localPos, vector3f _cameraPos, float _screenRatio, float _c1, float _c2)
		: vertexShader(),
		  matrix(_matrix),
		  localPos(_localPos),
		  cameraPos(_cameraPos),
		  screenRatio(_screenRatio),
		  c1(_c1),
		  c2(_c2)
	{
	}

	virtual vector4f vertex(const Mesh::vertex &v) override;

	mat3f matrix;

	vector3f localPos;
	vector3f cameraPos;
	float screenRatio;
	float c1, c2;
};

vector4f rotateShader::vertex(const Mesh::vertex &v)
{
	vector3f r = matrix * (v.pos + localPos - cameraPos);

	vector4f p;

	p.x = -r.x / screenRatio;
	p.y = r.y;
	p.z = c1 * r.z + c2;
	p.w = r.z;

	return p;
}
