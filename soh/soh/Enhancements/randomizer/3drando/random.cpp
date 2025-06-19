#include "random.hpp"

#include <bit>
#include <random>
#include <cassert>

static bool init = false;
static uint64_t state = 0;
const uint64_t multiplier = 6364136223846793005ULL;
const uint64_t increment = 11634580027462260723ULL;

// Initialize with seed specified
void Random_Init(uint64_t seed) {
    init = true;
    state = seed;
}

uint32_t next32() {
    if (!init) {
        // No seed given, get a random number from device to seed
#if !defined(__SWITCH__) && !defined(__WIIU__)
        uint64_t seed = static_cast<uint64_t>(std::random_device{}());
#else
        uint64_t seed = static_cast<uint64_t>(std::hash<std::string>{}(std::to_string(rand())));
#endif
        Random_Init(seed);
    }

    state = state * multiplier + increment;
    uint32_t xorshifted = static_cast<uint32_t>(((state >> 18) ^ state) >> 27);
    uint32_t rot = static_cast<int>(state >> 59);
    return std::rotr(xorshifted, rot);
}

// Returns a random integer in range [min, max-1]
uint32_t Random(uint32_t min, uint32_t max) {
    if (min == max) {
        return min;
    }
    assert(max > min);

    uint32_t n = max - min;
    uint32_t cutoff = UINT32_MAX - UINT32_MAX % static_cast<uint32_t>(n);
    for (;;) {
        uint32_t r = next32();
        if (r <= cutoff) {
            return min + r % n;
        }
    }
}

// Returns a random floating point number in [0.0, 1.0)
double RandomDouble() {
    return ldexp(next32(), -32);
}
