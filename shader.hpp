#pragma once

#include "context.hpp"
#include "matrix.hpp"
#include "obj.hpp"
#include "trirast.hpp"

class vertexShader {
public:
	vertexShader()
	{
	}

    virtual vector4f vertex(const Mesh::vertex &v) = 0;
};

class fragmentShader {
public:
	fragmentShader()
	{
	}

	virtual color fragment(const Mesh::vertex &v0) = 0;
};
