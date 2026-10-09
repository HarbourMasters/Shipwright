#ifndef RAND_H
#define RAND_H

#include "PR/ultratypes.h"

//! These values are recommended by the algorithms book *Numerical Recipes in C. The Art of Scientific Computing*, 2nd
//! Edition, 1992, ISBN 0-521-43108-5. (p. 284):
//! > This is about as good as any 32-bit linear congruential generator, entirely adequate for many uses.
#define RAND_MULTIPLIER 1664525
#define RAND_INCREMENT 1013904223

u32 Rand_Next(void);
void Rand_Seed(u32 seed);
f32 Rand_ZeroOne(void);
f32 Rand_Centered(void);
void Rand_Seed_Variable(u32* rndNum, u32 seed);
u32 Rand_Next_Variable(u32* rndNum);
f32 Rand_ZeroOne_Variable(u32* rndNum);
f32 Rand_Centered_Variable(u32* rndNum);

extern u32 gRandFloat;

#endif
