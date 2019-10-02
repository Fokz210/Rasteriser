#ifndef VECTOR3HPP
#define VECTOR3HPP

template<class T>
struct vector3 {
	vector3(T _x, T _y, T _z)
		: x(_x),
		  y(_y),
		  z(_z)
	{
	}

	T x;
	T y;
	T z;
};

template<class T>
vector3<T> operator+(const vector3<T> &v1, const vector3<T> &v2)
{
	return vector3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template<class T>
vector3<T> operator-(const vector3<T> &v1, const vector3<T> &v2)
{
	return vector3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template<class T>
vector3<T> operator*(const vector3<T> &v, const float &num)
{
	return vector3<T>(v.x * num, v.y * num, v.z * num);
}

template<class T>
vector3<T> operator*(const float &num, const vector3<T> &v)
{
	return v * num;
}

template<class T>
vector3<T> operator/(const vector3<T> &v, const float &num)
{
	return vector3<T>(v.x / num, v.y / num, v.z / num);
}

template<class T>
vector3<T> &operator+=(vector3<T> &v1, const vector3<T> &v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return v1;
}

template<class T>
vector3<T> &operator-=(vector3<T> &v1, const vector3<T> &v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return v1;
}

#endif //VECTOR3HPP
