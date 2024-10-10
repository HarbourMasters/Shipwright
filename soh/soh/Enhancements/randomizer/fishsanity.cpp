#include "3drando/pool_functions.hpp"
#include "../../OTRGlobals.h"
#include "fishsanity.h"
#include "draw.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include <consolevariablebridge.h>

extern "C" {
#include "src/overlays/actors/ovl_Fishing/z_fishing.h"
#include "src/overlays/actors/ovl_En_Fish/z_en_fish.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define FSi OTRGlobals::Instance->gRandoContext->GetFishsanity()

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

/**
 * @brief Parallel list of pond fish checks for both ages
*/
std::array<std::pair<RandomizerCheck, RandomizerCheck>, 17> Rando::StaticData::randomizerFishingPondFish = {
    { /*   Child Check           Adult Check    */
      { RC_LH_CHILD_FISH_1,  RC_LH_ADULT_FISH_1 },
      { RC_LH_CHILD_FISH_2,  RC_LH_ADULT_FISH_2 },
      { RC_LH_CHILD_FISH_3,  RC_LH_ADULT_FISH_3 },
      { RC_LH_CHILD_FISH_4,  RC_LH_ADULT_FISH_4 },
      { RC_LH_CHILD_FISH_5,  RC_LH_ADULT_FISH_5 },
      { RC_LH_CHILD_FISH_6,  RC_LH_ADULT_FISH_6 },
      { RC_LH_CHILD_FISH_7,  RC_LH_ADULT_FISH_7 },
      { RC_LH_CHILD_FISH_8,  RC_LH_ADULT_FISH_8 },
      { RC_LH_CHILD_FISH_9,  RC_LH_ADULT_FISH_9 },
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

ActorFunc drawFishing = NULL;
ActorFunc drawEnFish = NULL;
Color_RGBA16 fsPulseColor = { 30, 240, 200 };

namespace Rando {
    const FishIdentity Fishsanity::defaultIdentity = { RAND_INF_MAX, RC_UNKNOWN_CHECK };
    bool Fishsanity::fishsanityHelpersInit = false;
    s16 Fishsanity::fishGroupCounter = 0;
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
        auto [mode, numFish, ageSplit] = GetOptions(optionsSource);

        if (loc->GetRCType() != RCTYPE_FISH || mode == RO_FISHSANITY_OFF) {
            return false;
        }
        RandomizerCheck rc = loc->GetRandomizerCheck();
        // Are pond fish enabled, and is this a pond fish location?
        if (mode != RO_FISHSANITY_OVERWORLD && numFish > 0 && loc->GetScene() == SCENE_FISHING_POND &&
            loc->GetActorID() == ACTOR_FISHING) {
            // Is this a child fish location? If so, is it within the defined number of pond fish checks?
            if (rc >= RC_LH_CHILD_FISH_1 && rc <= RC_LH_CHILD_LOACH_2 && numFish > (loc->GetActorParams() - 100)) {
                return true;
            }
            // Are adult fish available, and is this an adult fish location? If so, is it within the defined number of pond
            // fish checks?
            if (ageSplit && rc >= RC_LH_ADULT_FISH_1 && rc <= RC_LH_ADULT_LOACH && numFish > (loc->GetActorParams() - 100)) {
                return true;
            }
        }
        // Are overworld fish enabled, and is this an overworld fish location?
        if (mode != RO_FISHSANITY_POND && (loc->GetScene() == SCENE_GROTTOS || loc->GetScene() == SCENE_ZORAS_DOMAIN)
            && loc->GetActorID() == ACTOR_EN_FISH && (loc->GetActorParams() >> 8)) {
            return true;
        }
        // Must not be an included fish location!
        return false;
    }

    std::pair<std::vector<RandomizerCheck>, std::vector<RandomizerCheck>>
    Fishsanity::GetFishingPondLocations(FishsanityOptionsSource optionsSource) {
        auto [mode, numFish, ageSplit] = GetOptions(optionsSource);
        std::vector<RandomizerCheck> activeFish;
        std::vector<RandomizerCheck> remainingFish;
        std::vector<RandomizerCheck> pondFish = Rando::StaticData::GetPondFishLocations();

        // Fishsanity_InitializeHelpers();
        remainingFish.insert(remainingFish.end(), pondFish.begin(), pondFish.end());

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
        auto [mode, numFish, ageSplit] = GetOptions(optionsSource);
        std::vector<RandomizerCheck> activeFish;
        std::vector<RandomizerCheck> remainingFish;

        // Add pond fish
        if (mode == RO_FISHSANITY_POND || mode == RO_FISHSANITY_BOTH) {
            auto pondLocations = GetFishingPondLocations(optionsSource);
            activeFish.insert(activeFish.end(), pondLocations.first.begin(), pondLocations.first.end());
            remainingFish.insert(remainingFish.end(), pondLocations.second.begin(), pondLocations.second.end());
        }

        // Add overworld fish
        if (mode == RO_FISHSANITY_OVERWORLD || mode == RO_FISHSANITY_BOTH) {
            std::vector<RandomizerCheck> overworldFish = Rando::StaticData::GetOverworldFishLocations();
            activeFish.insert(activeFish.end(), overworldFish.begin(), overworldFish.end());
        }

        return std::make_pair(activeFish, remainingFish);
    }

    FishIdentity Fishsanity::IdentifyPondFish(u8 fishParams) {
        auto [mode, pondCount, ageSplit] = GetOptions();
        FishIdentity identity = defaultIdentity;

        if (!GetPondFishShuffled()) {
            return identity;
        }

        if (pondCount > 16) {
            identity = GetPondFish(fishParams, IsAdultPond());
        } else {
            identity = LINK_IS_ADULT ? mCurrPondFish.second : mCurrPondFish.first;
        }

        return identity;
    }

    FishsanityPondOptions Fishsanity::GetOptions(FishsanityOptionsSource optionsSource) {
        FishsanityPondOptions options{};
        switch (optionsSource) {
            // Used in check tracker
            case FSO_SOURCE_CVARS:
                options.mode = CVarGetInteger(CVAR_RANDOMIZER_SETTING("Fishsanity"), RO_FISHSANITY_OFF);
                options.numFish = CVarGetInteger(CVAR_RANDOMIZER_SETTING("FishsanityPondCount"), 0);
                options.ageSplit = CVarGetInteger(CVAR_RANDOMIZER_SETTING("FishsanityAgeSplit"), 0) == 1;
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

    void Fishsanity::UpdateCurrentPondFish() {
        auto [mode, pondCount, ageSplit] = GetOptions();
        mCurrPondFish = std::pair<FishIdentity, FishIdentity>();
        mCurrPondFish.first = defaultIdentity;
        mCurrPondFish.second = defaultIdentity;

        // Initialize mCurrPondFish if we're shuffling pond fish, but if all fish are shuffled, we don't need to use this.
        if ((mode == RO_FISHSANITY_BOTH || mode == RO_FISHSANITY_POND) && pondCount < 17) {
            // find the first inf that isn't set yet for each age
            // but don't go past the max number
            std::pair<RandomizerCheck, RandomizerCheck> tableEntry;
            for (s16 i = 0, params = 100; i < pondCount; i++, params++) {
                tableEntry = Rando::StaticData::randomizerFishingPondFish[i];
                if (!Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(tableEntry.first)) || i == pondCount - 1) {
                    // Found first child check
                    if (!IsFish(&mCurrPondFish.first)) {
                        mCurrPondFish.first = GetPondFish(params, false);
                    }

                    if (!ageSplit && !IsFish(&mCurrPondFish.second)) {
                        mCurrPondFish.second = GetPondFish(params, false);
                        // both ages are resolved! we can quit here
                        break;
                    }
                }

                if (ageSplit && !IsFish(&mCurrPondFish.second) && tableEntry.second != RC_UNKNOWN_CHECK &&
                    (!Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(tableEntry.second)) || i == pondCount - 1)) {
                    mCurrPondFish.second = mCurrPondFish.second = GetPondFish(params, true);
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

    bool Fishsanity::GetOverworldFishShuffled() {
        u8 fsMode = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY);
        return fsMode == RO_FISHSANITY_OVERWORLD || fsMode == RO_FISHSANITY_BOTH;
    }

    bool Fishsanity::IsAdultPond() {
        return LINK_IS_ADULT && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY_AGE_SPLIT);
    }

    bool Fishsanity::GetPondCleared() {
        auto [mode, pondCount, ageSplit] = GetOptions();
        // no fish shuffled, so pond is always cleared :thumbsup:
        if (pondCount == 0) {
            return true;
        }

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
            if (rc != RC_UNKNOWN_CHECK && !Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(rc))) {
                return false;
            }
        }
        return true;
    }

    bool Fishsanity::GetDomainCleared() {
        for (RandomizerInf i = RAND_INF_ZD_FISH_1; i <= RAND_INF_ZD_FISH_5; i = (RandomizerInf)(i + 1)) {
            if (!Flags_GetRandomizerInf(i)) {
                return false;
            }
        }
        return true;
    }

    void Fishsanity::InitializeHelpers() {
        if (fishsanityHelpersInit) {
            return;
        }

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
        return { OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(rc), rc };
    }

    FishIdentity Fishsanity::AdvancePond() {
        auto [mode, pondCount, ageSplit] = GetOptions();

        // No need to update state with full pond shuffle
        if (pondCount > 16) {
            return defaultIdentity;
        }

        UpdateCurrentPondFish();

        return IsAdultPond() ? mCurrPondFish.second : mCurrPondFish.first;
    }

    FishsanityCheckType Fishsanity::GetCheckType(RandomizerCheck rc) {
        // if it's not RCTYPE_FISH, obviously it's not a fish
        if (Rando::StaticData::GetLocation(rc)->GetRCType() != RCTYPE_FISH) {
            return FSC_NONE;
        }

        auto loc = Rando::StaticData::GetLocation(rc);
        switch (loc->GetScene()) {
        case SCENE_FISHING_POND:
            return FSC_POND;
        case SCENE_ZORAS_DOMAIN:
            return FSC_ZD;
        case SCENE_GROTTOS:
            return FSC_GROTTO;
        default:
            return FSC_NONE;
        }
    }

    bool Fishsanity::IsFish(FishIdentity* fish) {
        if (fish->randomizerCheck == RC_UNKNOWN_CHECK || fish->randomizerInf == RAND_INF_MAX) {
            return false;
        }

        return GetCheckType(fish->randomizerCheck) != FSC_NONE;
    }

    void Fishsanity::OnActorInitHandler(void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);

        auto fs = OTRGlobals::Instance->gRandoContext->GetFishsanity();
        FishIdentity fish;

        if (actor->id == ACTOR_EN_FISH && fs->GetOverworldFishShuffled()) {
            // Set fish ID for ZD fish
            if (gPlayState->sceneNum == SCENE_ZORAS_DOMAIN && actor->params == -1) {
                actor->params ^= fishGroupCounter++;
            } else if (gPlayState->sceneNum == SCENE_GROTTOS && actor->params == 1) {
                actor->params = 0x100 | gSaveContext.respawn[RESPAWN_MODE_RETURN].data;
            }

            fish = OTRGlobals::Instance->gRandomizer->IdentifyFish(gPlayState->sceneNum, actor->params);
            // Render fish as randomized item
            if (Rando::Fishsanity::IsFish(&fish) && !Flags_GetRandomizerInf(fish.randomizerInf)) {
                if (!drawEnFish) {
                    drawEnFish = actor->draw;
                }
                actor->draw = Fishsanity_DrawEnFish;
            }
            return;
        }

        if (actor->id == ACTOR_FISHING && gPlayState->sceneNum == SCENE_FISHING_POND && actor->params >= 100 &&
            actor->params <= 117 && fs->GetPondFishShuffled()) {
            // Initialize pond fish for fishsanity
            // Initialize fishsanity metadata on this actor
            Fishing* fishActor = static_cast<Fishing*>(refActor);
            //fishActor->fishsanityParams = actor->params;
            fish = OTRGlobals::Instance->gRandomizer->IdentifyFish(gPlayState->sceneNum, actor->params);

            // With every pond fish shuffled, caught fish will not spawn unless all fish have been caught.
            if (RAND_GET_OPTION(RSK_FISHSANITY_POND_COUNT) > 16 &&
                !fs->GetPondCleared()) {
                // Create effect for uncaught fish
                if (!Flags_GetRandomizerInf(fish.randomizerInf)) {
                    actor->shape.shadowDraw = Fishsanity_DrawEffShadow;
                    if (!drawFishing) {
                        drawFishing = actor->draw;
                    }
                    actor->draw = Fishsanity_DrawFishing;
                }
            }
        }
    }

    void Fishsanity::OnFlagSetHandler(int16_t flagType, int16_t flag) {
        if (flagType != FLAG_RANDOMIZER_INF) {
            return;
        }
        RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf((RandomizerInf)flag);
        FishsanityCheckType fsType = Rando::Fishsanity::GetCheckType(rc);
        if (fsType == FSC_NONE) {
            return;
        }

        // When a pond fish is caught, advance the pond.
        if (fsType == FSC_POND) {
            OTRGlobals::Instance->gRandoContext->GetFishsanity()->AdvancePond();
        }
    }

    void Fishsanity::OnActorUpdateHandler(void* refActor) {
        if (gPlayState->sceneNum != SCENE_GROTTOS && gPlayState->sceneNum != SCENE_ZORAS_DOMAIN && gPlayState->sceneNum != SCENE_FISHING_POND) {
            return;
        }

        Actor* actor = static_cast<Actor*>(refActor);
        auto fs = OTRGlobals::Instance->gRandoContext->GetFishsanity();

        // Detect fish catch
        if (actor->id == ACTOR_FISHING && fs->GetPondFishShuffled()) {
            Fishing* fish = static_cast<Fishing*>(refActor);

            // State 6 -> Fish caught and hoisted
            if (fish->fishState == 6) {
                FishIdentity identity = OTRGlobals::Instance->gRandomizer->IdentifyFish(gPlayState->sceneNum, actor->params);
                if (identity.randomizerCheck != RC_UNKNOWN_CHECK) {
                    Flags_SetRandomizerInf(identity.randomizerInf);
                    // Remove uncaught effect
                    if (actor->shape.shadowDraw != NULL) {
                        actor->shape.shadowDraw = NULL;
                        actor->draw = drawFishing;
                    }
                }
            }
        }

        if (actor->id == ACTOR_EN_FISH && fs->GetOverworldFishShuffled()) {
            FishIdentity fish = OTRGlobals::Instance->gRandomizer->IdentifyFish(gPlayState->sceneNum, actor->params);
            EnFish* fishActor = static_cast<EnFish*>(refActor);
            if (Rando::Fishsanity::IsFish(&fish) && Flags_GetRandomizerInf(fish.randomizerInf)) {
                // Reset draw method
                if (actor->draw == Fishsanity_DrawEnFish) {
                    actor->draw = drawEnFish;
                }
            }

            if (((actor->params >> 8) > 0) && fishActor->respawnTimer > 0) {
                Actor_Kill(actor);
            }
        }

        // Reset fish group counter when the group gets culled
        if (actor->id == ACTOR_OBJ_MURE && gPlayState->sceneNum == SCENE_ZORAS_DOMAIN && fishGroupCounter > 0 &&
            !(actor->flags & ACTOR_FLAG_UPDATE_WHILE_CULLED) && fs->GetOverworldFishShuffled()) {
            fishGroupCounter = 0;
        }
    }

    void Fishsanity::OnSceneInitHandler(int16_t sceneNum) {
        if (sceneNum == SCENE_ZORAS_DOMAIN) {
            fishGroupCounter = 0;
        }
    }

    void Fishsanity::OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
        va_list args;
        va_copy(args, originalArgs);

        Actor* actor = va_arg(args, Actor*);
        auto fs = OTRGlobals::Instance->gRandoContext->GetFishsanity();

        va_end(args);

        if (id == VB_BOTTLE_ACTOR && actor->id == ACTOR_EN_FISH && fs->GetOverworldFishShuffled()) {
            FishIdentity fish = OTRGlobals::Instance->gRandomizer->IdentifyFish(gPlayState->sceneNum, actor->params);
            if (fish.randomizerCheck != RC_UNKNOWN_CHECK && !Flags_GetRandomizerInf(fish.randomizerInf)) {
                Flags_SetRandomizerInf(fish.randomizerInf);
                actor->parent = &GET_PLAYER(gPlayState)->actor;
                *should = false;
            }
        }
    }
} // namespace Rando

// C interface
extern "C" {
    bool Randomizer_GetPondFishShuffled() {
        return FSi->GetPondFishShuffled();
    }

    bool Randomizer_GetOverworldFishShuffled() {
        return FSi->GetOverworldFishShuffled();
    }

    bool Randomizer_IsAdultPond() {
        return FSi->IsAdultPond();
    }

    void Fishsanity_DrawEffShadow(Actor* actor, Lights* lights, PlayState* play) {
        Vec3f pos, ripplePos;
        static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        static Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 primColor;
        Color_RGBA8 envColor;

        // Color of the circle for the particles
        static Color_RGBA8 mainColors[5][4] = {
            { 240, 154, 137, 200 },
            { 240, 190, 137, 200 },
            { 240, 171, 137, 200 },
            { 240, 141, 146, 200 },
            { 240, 204, 137, 200 }
        };

        // Color of the faded flares stretching off the particles
        static Color_RGBA8 flareColors[5][3] = {
            { 128, 85, 82, 200 },
            { 128, 101, 82, 200 },
            { 128, 93, 82, 200 },
            { 128, 82, 98, 200 },
            { 128, 108, 82, 200 }
        };

        Color_RGBA8_Copy(&primColor, mainColors[ABS(actor->params) % 5]);
        Color_RGBA8_Copy(&envColor, flareColors[ABS(actor->params) % 5]);

        // Spawn sparkles
        pos.x = Rand_CenteredFloat(23.0f) + actor->world.pos.x;
        pos.y = (Rand_Centered() * 12.0f) + actor->world.pos.y;
        pos.z = Rand_CenteredFloat(23.0f) + actor->world.pos.z;
        velocity.y = 0.05f;
        accel.y = 0.025f;
        Math_Vec3f_Copy(&ripplePos, &pos);
        ripplePos.y += actor->yDistToWater;

        if (Rand_ZeroOne() < 0.3f) {
            EffectSsKiraKira_SpawnDispersed(play, &pos, &velocity, &accel, &primColor, &envColor, 1800, 10);
        }

        if (actor->bgCheckFlags & 0x20 && Rand_ZeroOne() < 0.15f) {
            EffectSsGRipple_Spawn(play, &ripplePos, 100, 200, 2);
        }
    }

    void Fishsanity_DrawEnFish(struct Actor* actor, struct PlayState* play) {
        FishIdentity fish = OTRGlobals::Instance->gRandomizer->IdentifyFish(play->sceneNum, actor->params);
        GetItemEntry randoItem = Rando::Context::GetInstance()->GetFinalGIEntry(fish.randomizerCheck, true, GI_FISH);
        if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
            randoItem = GET_ITEM_MYSTERY;
        }

        Matrix_Push();
        Matrix_Scale(30.0, 30.0, 30.0, MTXMODE_APPLY);

        EnItem00_CustomItemsParticles(actor, play, randoItem);
        GetItemEntry_Draw(play, randoItem);

        Matrix_Pop();
    }

    void Fishsanity_DrawFishing(struct Actor* actor, struct PlayState* play) {
        Fishsanity_OpenGreyscaleColor(play, &fsPulseColor, (actor->params - 100) * 20);
        drawFishing(actor, play);
        Fishsanity_CloseGreyscaleColor(play);
    }

    void Fishsanity_OpenGreyscaleColor(PlayState* play, Color_RGBA16* color, int16_t frameOffset) {
        OPEN_DISPS(play->state.gfxCtx);
        gDPSetGrayscaleColor(
            POLY_OPA_DISP++, color->r, color->g, color->b,
            // Make color pulse, offset a bit by the actor params
            ABS(255.0f * Math_CosS((play->gameplayFrames + frameOffset) * 1000)));
        gSPGrayscale(POLY_OPA_DISP++, true);
        CLOSE_DISPS(play->state.gfxCtx);
    }

    void Fishsanity_CloseGreyscaleColor(PlayState* play) {
        OPEN_DISPS(play->state.gfxCtx);
        gSPGrayscale(POLY_OPA_DISP++, false);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
