#ifndef FISHSANITY_H
#define FISHSANITY_H
#pragma once

#include <z64.h>
#include "randomizerTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

typedef struct {
    u8 mode;
    u8 numFish;
    bool ageSplit;
} FishsanityPondOptions;

typedef enum {
    FSO_SOURCE_RANDO,
    FSO_SOURCE_CVARS
} FishsanityOptionsSource;

typedef enum {
    FSC_NONE,
    FSC_POND,
    FSC_GROTTO,
    FSC_ZD,
} FishsanityCheckType;

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

    /**
     * @brief Gets the type of a fishsanity check
     * @param rc The RandomizerCheck to categorize
     * @return The check's fishsanity type, or FSC_NONE
     */
    static FishsanityCheckType GetCheckType(RandomizerCheck rc);

    /**
     * @brief Returns true if the given FishIdentity represents an actual fish
     * @param fish The fish to check
    */
    static bool IsFish(FishIdentity* fish);

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
     * @brief Returns true if overworld fish are shuffled 
    */
    bool GetOverworldFishShuffled();

    /**
     * @brief Returns true if the fishing pond is currently adult (i.e., age split is enabled and Link is adult)
    */
    bool IsAdultPond();

    /**
     * @brief Returns true if all available pond fish checks have been collected for the current age
    */
    bool GetPondCleared();

    /**
     * @brief Returns true if all available Zora's Domain fish checks have been collected
    */
    bool GetDomainCleared();

    /**
     * @brief Advances current fishing pond check; no effect if every fish is shuffled
     * @return The new FishIdentity for the current pond, or default identity if every fish is shuffled
    */
    FishIdentity AdvancePond();

    /**
     * @brief ActorInit hook handler for fishsanity
    */
    static void OnActorInitHandler(void* refActor);

    /**
     * @brief FlagSet hook handler for fishsanity
    */
    static void OnFlagSetHandler(int16_t flagType, int16_t flag);

    /**
     * @brief PlayerUpdate hook handler for fishsanity
    */
    static void OnPlayerUpdateHandler();

    /**
     * @brief ActorUpdate hook handler for fishsanity
    */
    static void OnActorUpdateHandler(void* refActor);

    /**
     * @brief SceneInit hook handler for fishsanity
    */
    static void OnSceneInitHandler(int16_t sceneNum);

    /**
     * @brief VB hook handler for fishsanity
    */
    static void OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs);

  private:
    /**
     * @brief Initialize helper statics if they have not been initialized yet
    */
    void InitializeHelpers();
    
    /**
     * @brief Resolves a pond fish's FishIdentity directly from params & pond age
     * 
     * @param params Params for Fishing actor
     * @param adultPond Whether to resolve this fish as an adult check
     * @return The FishIdentity for the described fish
    */
    static FishIdentity GetPondFish(s16 params, bool adultPond);

    /**
     * @brief Current pond fish when all pond fish are not randomized
    */
    std::pair<FishIdentity, FishIdentity> mCurrPondFish;
    
    /**
     * @brief True if fishsanity helpers have been initialized
    */
    static bool fishsanityHelpersInit;

    static s16 fishGroupCounter;

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
/// Returns true if overworld fish should be shuffled based on fishsanity settings.
bool Randomizer_GetOverworldFishShuffled();
/// Returns true if the adult fishing pond should be used for fishsanity.
bool Randomizer_IsAdultPond();
/// Custom shadow draw function to add effect to uncollected fish
void Fishsanity_DrawEffShadow(Actor* actor, Lights* lights, PlayState* play);
/// Overriden actor draw function for bottleable fish
void Fishsanity_DrawEnFish(struct Actor* actor, struct PlayState* play);
/// Overriden actor draw function for the fishing pond
void Fishsanity_DrawFishing(struct Actor* actor, struct PlayState* play);
void Fishsanity_OpenGreyscaleColor(PlayState* play, Color_RGBA16* color, int16_t frameOffset);
void Fishsanity_CloseGreyscaleColor(PlayState* play);
#ifdef __cplusplus
}
#endif

#endif // FISHSANITY_H
