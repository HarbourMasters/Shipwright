#pragma once

#include <cstdint>

struct Vec2f
{
	float x, y;

	Vec2f()
	{
		x = 0;
		y = 0;
	};
	Vec2f(float nX, float nY)
	{
		x = nX;
		y = nY;
	};
};