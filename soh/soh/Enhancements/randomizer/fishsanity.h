#ifndef FISHSANITY_H
#define FISHSANITY_H
#pragma once

#include "randomizerTypes.h"
#include <z64.h>

#define RAND_INF_FISH(num, adult) ((adult ? RAND_INF_ADULT_FISH_1 : RAND_INF_CHILD_FISH_1) + num)

#ifdef __cplusplus
extern "C" {
#endif

RandomizerInf Randomizer_GetPondFishInf(u8 fishNum);

#ifdef __cplusplus
}
#endif

#endif