#include "../../OTRGlobals.h"
#include "fishsanity.h"
#include "variables.h"
#include "3drando/pool_functions.hpp"
#include <functions.h>

#define FSi OTRGlobals::Instance->gRandomizer->GetFishsanity()

// Reference soh/src/overlays/actors/ovl_Fishing/z_fishing.c
std::array<std::pair<RandomizerCheck, RandomizerCheck>, 17> Rando::StaticData::randomizerFishingPondFish = {
    { /*   Child Check           Adult Check    */
      { RC_LH_CHILD_FISH_1, RC_LH_ADULT_FISH_1 },
      { RC_LH_CHILD_FISH_2, RC_LH_ADULT_FISH_2 },
      { RC_LH_CHILD_FISH_3, RC_LH_ADULT_FISH_3 },
      { RC_LH_CHILD_FISH_4, RC_LH_ADULT_FISH_4 },
      { RC_LH_CHILD_FISH_5, RC_LH_ADULT_FISH_5 },
      { RC_LH_CHILD_FISH_6, RC_LH_ADULT_FISH_6 },
      { RC_LH_CHILD_FISH_7, RC_LH_ADULT_FISH_7 },
      { RC_LH_CHILD_FISH_8, RC_LH_ADULT_FISH_8 },
      { RC_LH_CHILD_FISH_9, RC_LH_ADULT_FISH_9 },
      { RC_LH_CHILD_FISH_10, RC_LH_ADULT_FISH_10 },
      { RC_LH_CHILD_FISH_11, RC_LH_ADULT_FISH_11 },
      { RC_LH_CHILD_FISH_12, RC_LH_ADULT_FISH_12 },
      { RC_LH_CHILD_FISH_13, RC_LH_ADULT_FISH_13 },
      { RC_LH_CHILD_FISH_14, RC_LH_ADULT_FISH_14 },
      { RC_LH_CHILD_FISH_15, RC_LH_ADULT_FISH_15 },
      { RC_LH_CHILD_LOACH_1, RC_LH_ADULT_LOACH },
      { RC_LH_CHILD_LOACH_2, RC_UNKNOWN_CHECK } }
};

std::unordered_map<int8_t, RandomizerCheck> Rando::StaticData::randomizerGrottoFishMap = {
    { 0x2C, RC_KF_STORMS_GROTTO_FISH },      { 0x14, RC_LW_NEAR_SHORTCUTS_GROTTO_FISH },
    { 0x22, RC_HF_SOUTHEAST_GROTTO_FISH },   { 0x03, RC_HF_OPEN_GROTTO_FISH },
    { 0x00, RC_HF_NEAR_MARKET_GROTTO_FISH }, { 0x28, RC_KAK_OPEN_GROTTO_FISH },
    { 0x57, RC_DMT_STORMS_GROTTO_FISH },     { 0x7A, RC_DMC_UPPER_GROTTO_FISH },
    { 0x29, RC_ZR_OPEN_GROTTO_FISH }
};

namespace Rando {
    const FishIdentity Fishsanity::defaultIdentity = { RAND_INF_MAX, RC_UNKNOWN_CHECK };
    const FishsanityMeta Fishsanity::defaultMeta = { 0, false, Fishsanity::defaultIdentity, GI_NONE };
    bool Fishsanity::fishsanityHelpersInit = false;
    std::unordered_map<RandomizerCheck, LinkAge> Fishsanity::pondFishAgeMap;
    std::vector<RandomizerCheck> Fishsanity::childPondFish;
    std::vector<RandomizerCheck> Fishsanity::adultPondFish;

    Fishsanity::Fishsanity() {
        InitializeHelpers();
    }

    Fishsanity::~Fishsanity() {
    
    }

    bool Fishsanity::GetFishLocationIncluded(Rando::Location* loc,
                                             FishsanityOptionsSource optionsSource) {
        auto [mode, numFish, ageSplit] = GetOptions();

        if (loc->GetRCType() != RCTYPE_FISH || mode == RO_FISHSANITY_OFF)
            return false;
        RandomizerCheck rc = loc->GetRandomizerCheck();
        // Are pond fish enabled, and is this a pond fish location?
        if (mode != RO_FISHSANITY_GROTTOS && numFish > 0 && loc->GetScene() == SCENE_FISHING_POND &&
            loc->GetActorID() == ACTOR_FISHING) {
            // Is this a child fish location? If so, is it within the defined number of pond fish checks?
            if (rc >= RC_LH_CHILD_FISH_1 && rc <= RC_LH_CHILD_LOACH_2 && numFish > (loc->GetActorParams() - 100))
                return true;
            // Are adult fish available, and is this an adult fish location? If so, is it within the defined number of pond
            // fish checks?
            if (ageSplit && rc >= RC_LH_ADULT_FISH_1 && rc <= RC_LH_ADULT_LOACH && numFish > (loc->GetActorParams() - 100))
                return true;
        }
        // Are grotto fish enabled, and is this a grotto fish location?
        if (mode != RO_FISHSANITY_POND && loc->GetScene() == SCENE_GROTTOS && loc->GetActorID() == ACTOR_EN_FISH &&
            loc->GetActorParams() == 1) {
            return true;
        }
        // Must not be an included fish location!
        return false;
    }

    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>>
    Fishsanity::GetFishingPondLocations(FishsanityOptionsSource optionsSource) {
        auto [mode, numFish, ageSplit] = GetOptions();
        std::vector<RandomizerCheck> activeFish;
        std::vector<RandomizerCheck> remainingFish;

        // Fishsanity_InitializeHelpers();
        remainingFish.insert(remainingFish.end(), Rando::StaticData::pondFishLocations.begin(),
                             Rando::StaticData::pondFishLocations.end());

        // No pond fish shuffled
        if (numFish == 0) {
            return std::make_pair(activeFish, remainingFish);
        }
        // Every pond fish is shuffled, so we can save some time
        if (numFish > 16) {
            // Child and adult pond fish are both shuffled, set activeFish to remainingFish and return an empty vector for
            // inactive fish.
            if (ageSplit) {
                return std::make_pair(remainingFish, activeFish);
            }
            // Activate all child fish only
            activeFish = FilterAndEraseFromPool(
                remainingFish, [](const RandomizerCheck loc) { return pondFishAgeMap[loc] == LINK_AGE_CHILD; });
            return std::make_pair(activeFish, remainingFish);
        }
        // Only some pond fish are shuffled, so we have to only activate the requested number.
        activeFish.insert(activeFish.end(), childPondFish.begin(), childPondFish.begin() + numFish);
        // If pond is split, also add the requested number of adult fish.
        if (ageSplit) {
            activeFish.insert(activeFish.end(), adultPondFish.begin(),
                              adultPondFish.begin() + std::min<uint8_t>(numFish, 16));
        }
        // NOTE: This only works because we can assume activeFish is already sorted; changes that break this assumption will
        // also break this
        FilterAndEraseFromPool(remainingFish,
                               [&](uint32_t loc) { return std::binary_search(activeFish.begin(), activeFish.end(), loc); });

        return std::make_pair(activeFish, remainingFish);
    }

    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>>
    Fishsanity::GetFishsanityLocations(FishsanityOptionsSource optionsSource) {
        auto [mode, numFish, ageSplit] = GetOptions();
        std::vector<RandomizerCheck> activeFish;
        std::vector<RandomizerCheck> remainingFish;

        // Add pond fish
        if (mode == RO_FISHSANITY_POND || mode == RO_FISHSANITY_BOTH) {
            auto pondLocations = GetFishingPondLocations(optionsSource);
            activeFish.insert(activeFish.end(), pondLocations.first.begin(), pondLocations.first.end());
            remainingFish.insert(remainingFish.end(), pondLocations.second.begin(), pondLocations.second.end());
        }

        // Add grotto fish
        if (mode == RO_FISHSANITY_GROTTOS || mode == RO_FISHSANITY_BOTH) {
            activeFish.insert(activeFish.end(), Rando::StaticData::grottoFishLocations.begin(),
                              Rando::StaticData::grottoFishLocations.end());
        }

        return std::make_pair(activeFish, remainingFish);
    }

    FishIdentity Fishsanity::IdentifyPondFish(u8 fishParams) {
        auto [mode, pondCount, ageSplit] = GetOptions();
        FishIdentity identity = defaultIdentity;

        if (mode == RO_FISHSANITY_OFF || mode == RO_FISHSANITY_GROTTOS || pondCount < 1) {
            return identity;
        }

        if (pondCount > 16) {
            return GetPondFish(fishParams, IsAdultPond());
        }

        return (LINK_IS_ADULT ? mCurrPondFish.second : mCurrPondFish.first).fish;
    }

    FishsanityPondOptions Fishsanity::GetOptions(FishsanityOptionsSource optionsSource) {
        FishsanityPondOptions options{};
        switch (optionsSource) {
            case FSO_SOURCE_CVARS:
                options.mode = CVarGetInteger("gRandomizeFishsanity", RO_FISHSANITY_OFF);
                options.numFish = CVarGetInteger("gRandomizeFishsanityPondCount", 0);
                options.ageSplit = CVarGetInteger("gRandomizeFishsanityAgeSplit", 0) == 1;
                break;
            case FSO_SOURCE_RANDO:
            default:
                options.mode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
                options.numFish = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_POND_COUNT);
                options.ageSplit = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_AGE_SPLIT);
                break;
        }
        return options;
    }

    FishsanityMeta Fishsanity::GetPondFishMetaFromParams(s16 params, FishsanityMeta* current) {
        auto [mode, numFish, ageSplit] = GetOptions();
        FishsanityMeta meta = defaultMeta;

        // Every pond fish shuffled, params determine meta
        if (numFish > 16) {
            // This fish's metadata is (semi) stable, trust that whatever we've been given is correct.
            if (current != NULL) {
                return meta;
            }

            meta = { params, false, GetPondFish(params, IsAdultPond()) };
            meta.getItemId = OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(meta.fish.randomizerCheck, GI_NONE, false).getItemId;
        } else {
            meta = LINK_IS_ADULT ? mCurrPondFish.second : mCurrPondFish.first;
        }

        return meta;
    }

    void Fishsanity::UpdateCurrentPondFish() {
        auto [mode, pondCount, ageSplit] = GetOptions();
        mCurrPondFish = std::pair<FishsanityMeta, FishsanityMeta>();
        mCurrPondFish.first = defaultMeta;
        mCurrPondFish.second = defaultMeta;

        // Initialize mCurrPondFish if we're shuffling pond fish, but if all fish are shuffled, we don't need to use this.
        if ((mode == RO_FISHSANITY_BOTH || mode == RO_FISHSANITY_POND) && pondCount < 17) {
            // find the first inf that isn't set yet for each age
            // but don't go past the max number
            pondCount -= 1;
            std::pair<RandomizerCheck, RandomizerCheck> tableEntry;
            for (s16 i = 0, params = 100; i < pondCount; i++, params++) {
                tableEntry = Rando::StaticData::randomizerFishingPondFish[i];
                if (!Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(tableEntry.first))) {
                    // Found first child check
                    if (mCurrPondFish.first.params == 0) {
                        mCurrPondFish.first = { params, false, GetPondFish(params, false),
                            OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(tableEntry.first, GI_NONE, false).getItemId
                        };
                    }

                    if (!ageSplit && mCurrPondFish.second.params == 0) {
                        mCurrPondFish.second = { params, false, GetPondFish(params, false),
                            OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(tableEntry.first, GI_NONE, false).getItemId
                        };
                        // both ages are resolved! we can quit here
                        break;
                    }
                }

                if (ageSplit && mCurrPondFish.second.params == 0 && tableEntry.second != RC_UNKNOWN_CHECK &&
                    !Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(tableEntry.second))) {
                    mCurrPondFish.second = mCurrPondFish.second = { params, false, GetPondFish(params, true),
                        OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(tableEntry.second, GI_NONE, false).getItemId
                    };
                }
            }
        }
    }

    void Fishsanity::InitializeFromSave() {
        UpdateCurrentPondFish();
    }

    bool Fishsanity::GetPondFishShuffled() {
        u8 fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
        return OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_POND_COUNT) > 0 &&
           (fsMode == RO_FISHSANITY_POND || fsMode == RO_FISHSANITY_BOTH);
    }

    bool Fishsanity::GetGrottoFishShuffled() {
        u8 fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
        return fsMode == RO_FISHSANITY_GROTTOS || fsMode == RO_FISHSANITY_BOTH;
    }

    bool Fishsanity::IsAdultPond() {
        return LINK_IS_ADULT && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_AGE_SPLIT);
    }

    bool Fishsanity::GetPondCleared() {
        auto [mode, pondCount, ageSplit] = GetOptions();
        // no fish shuffled, so pond is always cleared :thumbsup:
        if (pondCount == 0)
            return true;

        bool adultPond = LINK_IS_ADULT && ageSplit;
        // if we've collected the final shuffled fish, pond is complete
        if (pondCount <= 16) {
            auto tableEntry = Rando::StaticData::randomizerFishingPondFish[pondCount - 1];
            return Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(adultPond ? tableEntry.second : tableEntry.first));
        }

        // the last two checks actually don't matter because logically they will never be true, but maybe one day they will
        // if every fish is shuffled, check if we've collected every fish
        for (auto tableEntry : Rando::StaticData::randomizerFishingPondFish) {
            RandomizerCheck rc = adultPond ? tableEntry.second : tableEntry.first;
            // if we haven't collected this fish, then we're not done yet! get back in there, soldier
            if (rc != RC_UNKNOWN_CHECK && !Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(rc)))
                return false;
        }
        return true;
    }

    void Fishsanity::InitializeHelpers() {
        if (fishsanityHelpersInit)
            return;

        for (auto pair : Rando::StaticData::randomizerFishingPondFish) {
            pondFishAgeMap[pair.first] = LINK_AGE_CHILD;
            pondFishAgeMap[pair.second] = LINK_AGE_ADULT;
            childPondFish.push_back(pair.first);
            adultPondFish.push_back(pair.second);
        }
    }

    FishIdentity Fishsanity::GetPondFish(s16 params, bool adultPond) {
        auto pair = Rando::StaticData::randomizerFishingPondFish[params - 100];
        RandomizerCheck rc = adultPond ? pair.second : pair.first;

        return {
            OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(rc),
            rc
        };
    }
} // namespace Rando

// C interface
extern "C" {
    bool Randomizer_GetPondFishShuffled() {
        return FSi->GetPondFishShuffled();
    }

    bool Randomizer_GetGrottoFishShuffled() {
        return FSi->GetGrottoFishShuffled();
    }

    bool Randomizer_IsAdultPond() {
        return FSi->IsAdultPond();
    }
}