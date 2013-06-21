#pragma once

#include <d3dx9math.h>

// MATH
#define PI 3.1415926535897932384626433832795f
#define DEGREES_PER_RADIAN 57.2957795f
#define FERROR 0.0001f

typedef unsigned int u32;
typedef D3DXVECTOR2 Vector2;
typedef D3DXVECTOR3 Vector3;
typedef D3DCOLOR Color;

template<typename Iterator, typename T> inline
T* binarySearch(Iterator begin, Iterator end, const T& value)
{
	std::vector<T>::iterator iter = std::lower_bound(begin, end, value);
	if (iter != end && (*iter == value))
	{
		return &(*iter);
	}
	else
	{
		return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////////
static void createRect(u32 ux, u32 uy, u32 lx, u32 ly, RECT * prect)
{
	prect->left = ux;
	prect->top = uy;
	prect->right = lx;
	prect->bottom = ly;
}