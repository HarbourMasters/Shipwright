#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

typedef uint32_t strhash;

[[maybe_unused]] static strhash CRC32B(unsigned char* message, int32_t size)
{
	int32_t byte = 0, crc = 0;
	int32_t mask = 0;

	crc = 0xFFFFFFFF;

	for (int32_t i = 0; i < size; i++)
	{
		byte = message[i];
		crc = crc ^ byte;

		for (int32_t j = 7; j >= 0; j--)
		{
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~(uint32_t)(crc);
}

[[maybe_unused]] constexpr static strhash CRC32BCT(const char* message, int32_t size)
{
	int32_t byte = 0, crc = 0;
	int32_t mask = 0;

	crc = 0xFFFFFFFF;

	for (int32_t i = 0; i < size; i++)
	{
		byte = message[i];
		crc = crc ^ byte;

		for (int32_t j = 7; j >= 0; j--)
		{
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
	}

	return ~(uint32_t)(crc);
}