#ifndef VECTOR2HPP
#define VECTOR2HPP

#include <ostream>

template<class T>
class vector2 {
public:
	vector2(T x, T y)
		: _x(x),
		  _y(y)
	{
	}

	T _x, _y;
};

template<class T>
vector2<T> operator+(const vector2<T> &v1, const vector2<T> &v2)
{
	return vector2<T>(v1._x + v2._x, v1._y + v2._y);
}

template<class T>
vector2<T> operator-(const vector2<T> &v1, const vector2<T> &v2)
{
	return vector2<T>(v1._x - v2._x, v1._y - v2._y);
}

template<class T>
vector2<T> operator*(const vector2<T> &v, const float &num)
{
	return vector2<T>(v._x * num, v._y * num);
}

template<class T>
vector2<T> operator*(const float &num, const vector2<T> &v)
{
	return v * num;
}

template<class T>
vector2<T> operator/(const vector2<T> &v, const float &num)
{
	return vector2<T>(v._x / num, v._y / num);
}

template<class T>
vector2<T> &operator+=(vector2<T> &v1, const vector2<T> &v2)
{
	v1._x += v2._x;
	v1._y += v2._y;
	return v1;
}

template<class T>
vector2<T> &operator-=(vector2<T> &v1, const vector2<T> &v2)
{
	v1._x -= v2._x;
	v1._y -= v2._y;
	return v1;
}

template<class T>
std::ostream &operator<<(std::ostream &out, const vector2<T> &v)
{
	out << "(" << v._x << ", " << v._y << ")";

	return out;
}

#endif
