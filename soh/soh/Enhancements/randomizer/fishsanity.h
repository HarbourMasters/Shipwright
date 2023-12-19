#ifndef FISHSANITY_H
#define FISHSANITY_H
#pragma once

#include <z64.h>
#include "randomizerTypes.h"
#include "overlays/actors/ovl_Fishing/z_fishing.h"
#include "macros.h"

#define RAND_INF_FISH(num, adult) ((RandomizerInf)MIN((adult ? RAND_INF_ADULT_FISH_1 : RAND_INF_CHILD_FISH_1) + num, (adult ? RAND_INF_ADULT_LOACH : RAND_INF_CHILD_LOACH_2)))

typedef struct FishsanityOptions {
    u8 mode;
    u8 numFish;
    bool ageSplit;
} FishsanityOptions;

typedef enum FishsanityOptionsSource {
    FSO_SOURCE_RANDO,
    FSO_SOURCE_CVARS
};

/// Fishsanity-related metadata for fishing pond fish
typedef struct FishsanityMeta {
    s16 params;
    bool killAfterCollect;
    FishIdentity fish;
    s16 getItemId;
} FishsanityMeta;

#ifdef __cplusplus
namespace Rando {
class Fishsanity {
  public:
    Fishsanity();
    ~Fishsanity();

    static const FishIdentity defaultIdentity;
    static const FishsanityMeta defaultMeta;

    /**
     * @brief Returns true if the given fish location is active
     * 
     * @param loc The Location to check
     * @param optionsSource Optionally declare from which source to pull settings
    */
    bool GetFishLocationIncluded(Rando::Location* loc, FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

    /**
     * @brief Get the active and inactive locations in the fishing pond.
     * 
     * @param optionsSource Optionally declare from which source to pull settings
     * @return A pair of vectors, where the fist is all active pond fish checks, and the second is all inactive pond fish checks.
    */
    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>> GetFishingPondLocations(FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

    /**
     * @brief Get all active fishsanity locations, and all inactive fishing pond locations.
     * @param optionsSource Optionally declare from which source to pull settings 
     * @return A pair of vectors, where the first is all active fishsanity checks, and the second is all inactive fishsanity checks.
    */
    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>> GetFishsanityLocations(FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

    /**
     * @brief Returns the identity for a caught pond fish given its params
     * 
     * @param fishParams Actor parameters for the fish to identify
    */
    FishIdentity IdentifyPondFish(u8 fishParams);

    /**
     * @brief Get fishsanity options from the requested source
    */
    FishsanityOptions GetOptions(FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

    /**
     * @brief Get metadata for a fish given its params
     * 
     * @param params Actor parameters for the pond fish
     * @param current Current metadata associated with this fish, if any. 
    */
    FishsanityMeta GetPondFishMetaFromParams(s16 params, FishsanityMeta* current = NULL);
    
    /**
     * @brief Updates current pond fish according to save data
    */
    void UpdateCurrentPondFish();
    
    /**
     * @brief Initializes internal state from save
    */
    void InitializeFromSave();

    bool GetPondFishShuffled();
    bool GetGrottoFishShuffled();
    bool IsAdultPond();
    bool GetPondCleared();
  private:
    void InitializeHelpers();
    /// Resolves a FishIdentity directly from params & pond age
    FishIdentity GetPondFish(s16 params, bool adultPond);
    /// Current pond fish when all pond fish are not randomized
    std::pair<FishsanityMeta, FishsanityMeta> mCurrPondFish;
    /// Metadata for the currently held fish in the fishing pond minigame
    FishsanityMeta mHeldMetadata;
    /// True if fishsanity helpers have been initialized
    static bool fishsanityHelpersInit;
    // Helper data structures derived from static data
    static std::unordered_map<RandomizerCheck, LinkAge> pondFishAgeMap;
    static std::vector<RandomizerCheck> childPondFish;
    static std::vector<RandomizerCheck> adultPondFish;
};

/// Returns true if the given fish location is active with the provided fishsanity settings
bool Randomizer_FishLocationIncluded(Rando::Location* loc, u8 mode, u8 numFish, bool ageSplit);
/// Constructs a pair of vectors, where the fist is all active pond fish checks, and the second is all inactive pond
/// fish checks.
std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>>
Randomizer_GetFishingPondLocations(u8 mode, u8 numFish, bool ageSplit);
/// Get a pair of vectors, where the first is all active fishsanity checks, and the second is all inactive pond fish
/// checks.
std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>>
Randomizer_GetFishsanityLocations(u8 mode, u8 numFish, bool ageSplit);
}

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