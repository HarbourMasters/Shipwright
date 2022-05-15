#include "random_experiment.h"
#include <random>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long (*randomUint32)(unsigned long) = &standardRandomUint32;
bool gCsprngInitialized = false;
std::random_device cryptProvider;

void cryptoRandomInit() {
    randomUint32 = &cryptoRandomUint32;
    gCsprngInitialized = true;
}

unsigned long cryptoRandomUint32(unsigned long seed) {
    (void)seed;
    unsigned long randomBits = 0;
    randomBits = cryptProvider();
    return randomBits;
}

unsigned long standardRandomUint32(unsigned long seed) {
    return (seed * 1664525) + 1013904223;
}

#ifdef __cplusplus
}
#endif