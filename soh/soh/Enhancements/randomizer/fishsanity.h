#ifndef FISHSANITY_H
#define FISHSANITY_H
#pragma once

#include <z64.h>
#include "randomizerTypes.h"
#include "macros.h"

#define RAND_INF_FISH(num, adult) ((RandomizerInf)MIN((adult ? RAND_INF_ADULT_FISH_1 : RAND_INF_CHILD_FISH_1) + num, (adult ? RAND_INF_ADULT_LOACH : RAND_INF_CHILD_LOACH_2)))

typedef struct FishsanityPondOptions {
    u8 mode;
    u8 numFish;
    bool ageSplit;
} FishsanityPondOptions;

typedef enum FishsanityOptionsSource {
    FSO_SOURCE_RANDO,
    FSO_SOURCE_CVARS
};

/**
 * @brief Fishsanity-related metadata for fishing pond fish
*/
typedef struct FishsanityMeta {
    s16 params;
    bool killAfterCollect;
    FishIdentity fish;
    s16 getItemId;
} FishsanityMeta;

#ifdef __cplusplus
namespace Rando {

/**
 * @brief Class to provide an interface for and direct Fishsanity features
*/
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
     * 
     * @param optionsSource Optionally declare from which source to pull settings 
     * @return A pair of vectors, where the first is all active fishsanity checks, and the second is all inactive fishsanity checks.
    */
    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>> GetFishsanityLocations(FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

    /**
     * @brief Returns the identity for a caught pond fish given its params.
     * Not for use externally from rando, use Randomizer::IdentifyFish or Randomizer_IdentifyFish for that
     * 
     * @param fishParams Actor parameters for the fish to identify
    */
    FishIdentity IdentifyPondFish(u8 fishParams);

    /**
     * @brief Get fishsanity fishing pond options from the requested source
    */
    FishsanityPondOptions GetOptions(FishsanityOptionsSource optionsSource = FSO_SOURCE_RANDO);

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

    /**
     * @brief Returns true if the fishing pond is shuffled
    */
    bool GetPondFishShuffled();

    /**
     * @brief Returns true if grotto fish are shuffled 
    */
    bool GetGrottoFishShuffled();

    /**
     * @brief Returns true if the fishing pond is currently adult (i.e., age split is enabled and Link is adult)
    */
    bool IsAdultPond();

    /**
     * @brief Returns true if all available pond fish checks have been collected for the current age
    */
    bool GetPondCleared();

    /**
     * @brief Advances current fishing pond check; no effect if every fish is shuffled
     * @return The new FishsanityMeta for the current pond, or default metadata if every fish is shuffled
    */
    FishsanityMeta AdvancePond();
  private:
    /**
     * @brief Initialize helper statics if they have not been initialized yet
    */
    void InitializeHelpers();
    
    /**
     * @brief Resolves a FishIdentity directly from params & pond age
     * 
     * @param params Params for Fishing actor
     * @param adultPond Whether to resolve this fish as an adult check
     * @return A FishIdentity for the described fish
    */
    FishIdentity GetPondFish(s16 params, bool adultPond);
    
    /**
     * @brief Current pond fish when all pond fish are not randomized
    */
    std::pair<FishsanityMeta, FishsanityMeta> mCurrPondFish;
    
    /**
     * @brief Metadata for the currently held fish in the fishing pond minigame
    */
    FishsanityMeta mHeldMetadata;
    
    /**
     * @brief True if fishsanity helpers have been initialized
    */
    static bool fishsanityHelpersInit;

    /////////////////////////////////////////////////////////
    //// Helper data structures derived from static data ////
    /////////////////////////////////////////////////////////

    /**
     * @brief Mapping from pond fish check to the age where that check can be collected
    */
    static std::unordered_map<RandomizerCheck, LinkAge> pondFishAgeMap;

    /**
     * @brief List of child pond fish checks
    */
    static std::vector<RandomizerCheck> childPondFish;

    /**
     * @brief List of adult pond fish checks
    */
    static std::vector<RandomizerCheck> adultPondFish;
};
}

extern "C" {
#endif
/// Returns true if pond fish should be shuffled based on fishsanity settings.
bool Randomizer_GetPondFishShuffled();
/// Returns true if grotto fish should be shuffled based on fishsanity settings.
bool Randomizer_GetGrottoFishShuffled();
/// Returns true if the adult fishing pond should be used for fishsanity.
bool Randomizer_IsAdultPond();
#ifdef __cplusplus
}
#endif

#endif // FISHSANITY_H