#ifndef RECTHPP
#define RECTHPP

#include "vector2.hpp"

template<class T>
struct rect {
	rect(T _left, T _top, T _width, T _height)
		: left(_left),
		  top(_top),
		  width(_width),
		  height(_height)
	{
	}

	rect(vector2<T> v1, vector2<T> v2)
		: left(v1.x),
		  top(v1.y),
		  width(v2.x),
		  height(v2.y)
	{
	}

	T left;
	T top;
	T width;
	T height;
};

#endif //RECTHPP
