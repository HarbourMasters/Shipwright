#include "random.hpp"

#include <random>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

static bool init = false;
static boost::random::mt19937 generator;

// Initialize with seed specified
void Random_Init(uint32_t seed) {
    init = true;
    generator = boost::random::mt19937{ seed };
}

void Random_InitSeed() {
    if (!init) {
        // No seed given, get a random number from device to seed
#if !defined(__SWITCH__) && !defined(__WIIU__)
        const auto seed = static_cast<uint32_t>(std::random_device{}());
#else
        uint32_t seed = static_cast<uint32_t>(std::hash<std::string>{}(std::to_string(rand())));
#endif
        Random_Init(seed);
    }
}

// Returns a random unsigned integer in range [min, max-1]
uint32_t Random(uint32_t min, uint32_t max) {
    Random_InitSeed();
    boost::random::uniform_int_distribution<uint32_t> distribution(min, max - 1);
    return distribution(generator);
}

// Returns a random floating point number in [0.0, 1.0)
double RandomDouble() {
    boost::random::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}
