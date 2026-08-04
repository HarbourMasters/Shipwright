#include "random.hpp"

uint64_t rando_state = 0;
const uint64_t multiplier = 6364136223846793005ULL;
const uint64_t increment = 11634580027462260723ULL;

// Initialize with seed specified
void Random_Init(uint64_t seed) {
    ShipUtils::RandInit(seed, &rando_state);
}

uint32_t next32() {
    return ShipUtils::next32(&rando_state);
}

// Returns a random integer in range [min, max-1]
uint32_t Random(uint32_t min, uint32_t max) {
    return ShipUtils::Random(min, max, &rando_state);
}

// Returns a random floating point number in [0.0, 1.0)
double RandomDouble() {
    return ShipUtils::RandomDouble(&rando_state);
}
