#pragma once

#include <cstdint>

struct Vec3s
{
	int16_t x, y, z;

	Vec3s()
	{
		x = 0;
		y = 0;
		z = 0;
	};
	Vec3s(int16_t nX, int16_t nY, int16_t nZ)
	{
		x = nX;
		y = nY;
		z = nZ;
	};
};