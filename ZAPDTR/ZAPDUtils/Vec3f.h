#pragma once

#include <cstdint>

struct Vec3f
{
	float x, y, z;

	Vec3f()
	{
		x = 0;
		y = 0;
		z = 0;
	};
	Vec3f(float nX, float nY, float nZ)
	{
		x = nX;
		y = nY;
		z = nZ;
	};
};