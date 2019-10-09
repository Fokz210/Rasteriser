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

	vector3()
		: x(),
		  y(),
		  z()
	{
	}

	T x, y, z;

	T &operator[](int i)
	{
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}

	const T &operator[](int i) const
	{
		if (i == 0)
			return x;
		else if (i == 1)
			return y;
		else
			return z;
	}
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

template<class T>
vector3<T> cross(const vector3<T> &v1, const vector3<T> &v2)
{
	return vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template<class T>
float dot(const vector3<T> &v1, const vector3<T> &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template<class T>
float length(const vector3<T> &v)
{
	return std::sqrt(dot(v, v));
}
#endif //VECTOR3HPP
