#include "../../OTRGlobals.h"
#include "fishsanity.h"
#include "variables.h"

// Reference soh/src/overlays/actors/ovl_Fishing/z_fishing.c
std::array<std::pair<RandomizerCheck, RandomizerCheck>, 17> Rando::StaticData::randomizerFishingPondFish = {{
    /*   Child Check           Adult Check    */
    { RC_LH_CHILD_FISH_1,   RC_LH_ADULT_FISH_1 },
    { RC_LH_CHILD_FISH_2,   RC_LH_ADULT_FISH_2 },
    { RC_LH_CHILD_FISH_3,   RC_LH_ADULT_FISH_3 },
    { RC_LH_CHILD_FISH_4,   RC_LH_ADULT_FISH_4 },
    { RC_LH_CHILD_FISH_5,   RC_LH_ADULT_FISH_5 },
    { RC_LH_CHILD_FISH_6,   RC_LH_ADULT_FISH_6 },
    { RC_LH_CHILD_FISH_7,   RC_LH_ADULT_FISH_7 },
    { RC_LH_CHILD_FISH_8,   RC_LH_ADULT_FISH_8 },
    { RC_LH_CHILD_FISH_9,   RC_LH_ADULT_FISH_9 },
    { RC_LH_CHILD_FISH_10,  RC_LH_ADULT_FISH_10 },
    { RC_LH_CHILD_FISH_11,  RC_LH_ADULT_FISH_11 },
    { RC_LH_CHILD_FISH_12,  RC_LH_ADULT_FISH_12 },
    { RC_LH_CHILD_FISH_13,  RC_LH_ADULT_FISH_13 },
    { RC_LH_CHILD_FISH_14,  RC_LH_ADULT_FISH_14 },
    { RC_LH_CHILD_FISH_15,  RC_LH_ADULT_FISH_15 },
    { RC_LH_CHILD_LOACH_1,  RC_LH_ADULT_LOACH },
    { RC_LH_CHILD_LOACH_2,  RC_UNKNOWN_CHECK }
}};

std::unordered_map<int8_t, RandomizerCheck> Rando::StaticData::randomizerGrottoFishMap = {
    { 0x2C, RC_KF_STORMS_GROTTO_FISH },
    { 0x14, RC_LW_NEAR_SHORTCUTS_GROTTO_FISH },
    { 0x22, RC_HF_SOUTHEAST_GROTTO_FISH },
    { 0x03, RC_HF_OPEN_GROTTO_FISH },
    { 0x00, RC_HF_NEAR_MARKET_GROTTO_FISH },
    { 0x28, RC_KAK_OPEN_GROTTO_FISH },
    { 0x57, RC_DMT_STORMS_GROTTO_FISH },
    { 0x7A, RC_DMC_UPPER_GROTTO_FISH },
    { 0x29, RC_ZR_OPEN_GROTTO_FISH }
};

bool Randomizer_FishLocationIncluded(Rando::Location* loc, u8 mode, u8 numFish, bool ageSplit) {
    if (loc->GetRCType() != RCTYPE_FISH || mode == RO_FISHSANITY_OFF)
        return false;
    RandomizerCheck rc = loc->GetRandomizerCheck();
    // Are pond fish enabled, and is this a pond fish location?
    if (mode != RO_FISHSANITY_GROTTOS && numFish > 0 && loc->GetScene() == SCENE_FISHING_POND && loc->GetActorID() == ACTOR_FISHING) {
        // Is this a child fish location? If so, is it within the defined number of pond fish checks?
        if (rc >= RC_LH_CHILD_FISH_1 && rc <= RC_LH_CHILD_LOACH_2 && numFish > (loc->GetActorParams() - 100))
            return true;
        // Are adult fish available, and is this an adult fish location? If so, is it within the defined number of pond fish checks?
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

bool Randomizer_GetPondFishShuffled() {
    u8 fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
    return fsMode == RO_FISHSANITY_POND || fsMode == RO_FISHSANITY_BOTH;
}

bool Randomizer_GetGrottoFishShuffled() {
    u8 fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
    return fsMode == RO_FISHSANITY_GROTTOS || fsMode == RO_FISHSANITY_BOTH;
}

bool Randomizer_IsAdultPond() {
    return LINK_IS_ADULT && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_AGE_SPLIT);
}

RandomizerInf Randomizer_GetPondFishInf(u8 fishNum) {
    bool adult = Randomizer_IsAdultPond();
    return RAND_INF_FISH(fishNum, adult);
}