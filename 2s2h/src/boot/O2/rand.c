#include "rand.h"

//! The latest generated random number, used to generate the next number in the sequence.
static u32 sRandInt = 1;

//! Space to store a value to be re-interpreted as a float.
//! This can't be static because it is used in z_kankyo.
u32 gRandFloat;

/**
 * Generates the next pseudo-random integer.
 */
u32 Rand_Next(void) {
    return sRandInt = (sRandInt * RAND_MULTIPLIER) + RAND_INCREMENT;
}

/**
 * Seeds the internal pseudo-random number generator with a provided starting value.
 */
void Rand_Seed(u32 seed) {
    sRandInt = seed;
}

/**
 * Returns a pseudo-random float between 0.0f and 1.0f from the internal PRNG.
 *
 * @note Works by generating the next integer, masking it to an IEEE-754 compliant float between 1.0f and 2.0f, and
 * subtracting 1.0f.
 *
 * @remark This is also recommended by Numerical Recipes, pp. 284-5.
 */
f32 Rand_ZeroOne(void) {
    sRandInt = (sRandInt * RAND_MULTIPLIER) + RAND_INCREMENT;
    gRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&gRandFloat) - 1.0f;
}

/**
 * Returns a pseudo-random float between -0.5f and 0.5f in the same way as Rand_ZeroOne().
 */
f32 Rand_Centered(void) {
    sRandInt = (sRandInt * RAND_MULTIPLIER) + RAND_INCREMENT;
    gRandFloat = ((sRandInt >> 9) | 0x3F800000);
    return *((f32*)&gRandFloat) - 1.5f;
}

//! All functions below are unused variants of the above four, that use a provided random number variable instead of the
//! internal `sRandInt`

/**
 * Seeds a provided pseudo-random number with a provided starting value.
 *
 * @see Rand_Seed
 */
void Rand_Seed_Variable(u32* rndNum, u32 seed) {
    *rndNum = seed;
}

/**
 * Generates the next pseudo-random number from the provided rndNum.
 *
 * @see Rand_Next
 */
u32 Rand_Next_Variable(u32* rndNum) {
    return *rndNum = (*rndNum * RAND_MULTIPLIER) + RAND_INCREMENT;
}

/**
 * Generates the next pseudo-random float between 0.0f and 1.0f from the provided rndNum.
 *
 * @see Rand_ZeroOne
 */
f32 Rand_ZeroOne_Variable(u32* rndNum) {
    u32 next = (*rndNum * RAND_MULTIPLIER) + RAND_INCREMENT;

    gRandFloat = ((*rndNum = next) >> 9) | 0x3F800000;
    return *((f32*)&gRandFloat) - 1.0f;
}

/**
 * Generates the next pseudo-random float between -0.5f and 0.5f from the provided rndNum.
 *
 * @see Rand_ZeroOne, Rand_Centered
 */
f32 Rand_Centered_Variable(u32* rndNum) {
    u32 next = (*rndNum * RAND_MULTIPLIER) + RAND_INCREMENT;

    gRandFloat = ((*rndNum = next) >> 9) | 0x3F800000;
    return *((f32*)&gRandFloat) - 1.5f;
}
