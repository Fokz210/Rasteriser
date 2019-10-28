#pragma once
#include <stdexcept>

template<class T>
struct vector4 {
	T x, y, z, w;

	T &operator[](int i)
	{
		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw std::runtime_error("vector subscript out of range");
		}
	}

	const T &operator[](int i) const
	{

		switch (i) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw std::runtime_error("vector subscript out of range");
		}
	}

	operator vector3<T>() const
	{
        return vector3<T>{x / w, y / w, z / w};
	}
};

template<class T>
vector4<T> operator+(const vector4<T> &v1, const vector4<T> &v2)
{
	return vector4<T>{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

template<class T>
vector4<T> operator-(const vector4<T> &v1, const vector4<T> &v2)
{
	return vector4<T>{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

