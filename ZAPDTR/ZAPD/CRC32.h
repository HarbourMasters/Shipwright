#pragma once

static uint32_t CRC32B(const unsigned char* message, int32_t size)
{
	int32_t byte, crc;
	int32_t mask;

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