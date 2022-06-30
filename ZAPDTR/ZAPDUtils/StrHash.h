#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

typedef uint32_t strhash;

[[maybe_unused]] extern strhash CRC32B(unsigned char* message, int32_t size);
[[maybe_unused]] extern constexpr strhash CRC32BCT(const char* message, int32_t size);
