#include "random.hpp"

#include <random>

static bool init = false;
static std::mt19937_64 generator;

//Initialize with seed specified
void Random_Init(uint32_t seed) {
    init = true;
    generator = std::mt19937_64{seed};
}

//Returns a random integer in range [min, max-1]
uint32_t Random(int min, int max) {
    if (!init) {
        //No seed given, get a random number from device to seed
#if !defined(__SWITCH__) && !defined(__WIIU__)
        const auto seed = static_cast<uint32_t>(std::random_device{}());
#else
        uint32_t seed = static_cast<uint32_t>(std::hash<std::string>{}(std::to_string(rand())));
#endif
        Random_Init(seed);
    }
    std::uniform_int_distribution<uint32_t> distribution(min, max-1);
    return distribution(generator);
}

//Returns a random floating point number in [0.0, 1.0]
double RandomDouble() {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}
