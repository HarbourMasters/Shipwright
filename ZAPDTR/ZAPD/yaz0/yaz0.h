#pragma once

#include <vector>

void yaz0_decode(const uint8_t* src, uint8_t* dest, int32_t destsize);
std::vector<uint8_t> yaz0_encode(const uint8_t* src, int src_size);