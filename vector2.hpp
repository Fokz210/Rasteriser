#ifndef VECTOR2HPP
#define VECTOR2HPP

#include <ostream>

template<class T>
struct vector2 {
	vector2(T _x, T _y)
		: x(_x),
		  y(_y)
	{
	}

	T x, y;
};

template<class T>
vector2<T> operator+(const vector2<T> &v1, const vector2<T> &v2)
{
	return vector2<T>(v1.x + v2.x, v1.y + v2.y);
}

template<class T>
vector2<T> operator-(const vector2<T> &v1, const vector2<T> &v2)
{
	return vector2<T>(v1.x - v2.x, v1.y - v2.y);
}

template<class T>
vector2<T> operator*(const vector2<T> &v, const float &num)
{
	return vector2<T>(v.x * num, v.y * num);
}

template<class T>
vector2<T> operator*(const float &num, const vector2<T> &v)
{
	return v * num;
}

template<class T>
vector2<T> operator/(const vector2<T> &v, const float &num)
{
	return vector2<T>(v.x / num, v.y / num);
}

template<class T>
vector2<T> &operator+=(vector2<T> &v1, const vector2<T> &v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

template<class T>
vector2<T> &operator-=(vector2<T> &v1, const vector2<T> &v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}

template<class T>
std::ostream &operator<<(std::ostream &out, const vector2<T> &v)
{
	out << "(" << v.x << ", " << v.y << ")";

	return out;
}

#endif //VECTOR2HPP
