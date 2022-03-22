#pragma once

#include <cstdint>

struct Color3b
{
	uint8_t r, g, b;

	Color3b()
	{
		r = 0;
		g = 0;
		b = 0;
	};
	Color3b(uint8_t nR, uint8_t nG, uint8_t nB)
	{
		r = nR;
		g = nG;
		b = nB;
	};
};