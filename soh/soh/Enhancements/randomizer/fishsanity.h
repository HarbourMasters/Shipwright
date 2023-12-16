#ifndef FISHSANITY_H
#define FISHSANITY_H
#pragma once

#include "randomizerTypes.h"
#include "macros.h"
#include <z64.h>

#define RAND_INF_FISH(num, adult) ((RandomizerInf)MIN((adult ? RAND_INF_ADULT_FISH_1 : RAND_INF_CHILD_FISH_1) + num, (adult ? RAND_INF_ADULT_LOACH : RAND_INF_CHILD_LOACH_2)))

#ifdef __cplusplus
/// Returns true if the given fish location is active with the provided fishsanity settings
bool Randomizer_FishLocationIncluded(Rando::Location* loc, u8 mode, u8 numFish, bool ageSplit);
/// Constructs a pair of vectors, where the fist is all active pond fish checks, and the second is all inactive pond fish checks.
std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>> Randomizer_GetFishingPondLocations(u8 mode, u8 numFish, bool ageSplit);
/// Get a pair of vectors, where the first is all active fishsanity checks, and the second is all inactive pond fish checks.
std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>> Randomizer_GetFishsanityLocations(u8 mode, u8 numFish, bool ageSplit);

extern "C" {
#endif

/// Returns true if pond fish should be shuffled based on fishsanity settings.
bool Randomizer_GetPondFishShuffled();
/// Returns true if grotto fish should be shuffled based on fishsanity settings.
bool Randomizer_GetGrottoFishShuffled();
/// Returns true if the adult fishing pond should be used for fishsanity.
bool Randomizer_IsAdultPond();
/// Returns the RandomizerInf associated with the given fish number (Fishing.actor.params - 100)
RandomizerInf Randomizer_GetPondFishInf(u8 fishNum);
/// Gets equivalent params for the next pond fish according to fishsanity settings
s16 Randomizer_GetNextPondFishParams(s16 params);
/// Returns true if all currently available pond fish checks have been collected
bool Randomizer_GetPondCleared();

#ifdef __cplusplus
}
#endif

#endif // FISHSANITY_H