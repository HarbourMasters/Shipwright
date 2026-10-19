#include <spdlog/spdlog.h>

#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/RCToRandInf.h"
#include "soh/ShipInit.hpp"
#include "static_data.h"
#include "item_category_adj.h"
#include "particle_cmc.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "macros.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Go2/z_en_go2.h"
extern PlayState* gPlayState;
}

static s32 FireGoron_GetSwitchFlag(Actor* actor) {
    return (actor->params & 0xFC00) >> 0xA;
}

static bool FireGoron_IsFireGoron(Actor* actor) {
    return (actor->params & 0x1F) == GORON_FIRE_GENERIC;
}

static CheckIdentity IdentifyFireGoron(Actor* actor) {
    CheckIdentity goronIdentity;

    goronIdentity.randomizerInf = RAND_INF_MAX;
    goronIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    Rando::Location* location = OTRGlobals::Instance->gRandomizer->GetCheckObjectFromActor(
        ACTOR_EN_GO2, gPlayState->sceneNum, FireGoron_GetSwitchFlag(actor));

    IdentifyCheck(&goronIdentity, location);

    return goronIdentity;
}

static bool FireGoron_IdentityHoldsItem(const CheckIdentity& goronIdentity) {
    return goronIdentity.randomizerCheck != RC_UNKNOWN_CHECK && !Flags_GetRandomizerInf(goronIdentity.randomizerInf);
}

static bool FireGoron_HoldsItem(Actor* actor) {
    const auto goronIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(actor);
    return goronIdentity != nullptr && FireGoron_IdentityHoldsItem(*goronIdentity);
}

static void FireGoron_SpawnSparkles(Actor* actor, const CheckIdentity& goronIdentity) {
    if (actor->xzDistToPlayer > 1000.0f) {
        return;
    }

    bool cmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), 0);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);
    int isNotCMC = !cmc || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    GetItemEntry goronItem =
        Rando::Context::GetInstance()->GetFinalGIEntry(goronIdentity.randomizerCheck, true, GI_NONE);
    GetItemCategory getItemCategory = isNotCMC ? ITEM_CATEGORY_MAJOR : Randomizer_AdjustItemCategory(goronItem);

    Color_RGBA8 primColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_PRIMARY);
    Color_RGBA8 secColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_SECONDARY);
    Color_RGBA8 envColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_FLARE);

    Vec3f pos;
    Vec3f velocity = { 0.0f, -0.05f, 0.0f };
    Vec3f accel = { 0.0f, -0.025f, 0.0f };

    pos.x = Rand_CenteredFloat(10.0f) + actor->world.pos.x;
    pos.y = (Rand_ZeroOne() * 10.0f) + actor->world.pos.y + 60.0f;
    pos.z = Rand_CenteredFloat(10.0f) + actor->world.pos.z;
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, &secColor, &envColor, 2000, 100);
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, &primColor, &envColor, 2000, 100);
}

static void FireGoron_RandomizerUpdate(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (FireGoron_HoldsItem(actor)) {
        const auto& goronIdentity = *ObjectExtension::GetInstance().Get<CheckIdentity>(actor);
        if (!Rando::Context::GetInstance()->GetItemLocation(goronIdentity.randomizerCheck)->IsExcluded()) {
            FireGoron_SpawnSparkles(actor, goronIdentity);
        }
    }
}

static void FireGoron_LogUnrecognized(Actor* actor) {
    SPDLOG_WARN("Unrecognized Fire Temple goron: scene=0x{:02X} room={} quest={} params=0x{:04X} type={} "
                "switchFlag={} pos=({},{},{}) rotY=0x{:04X}",
                gPlayState->sceneNum, actor->room,
                ResourceMgr_IsSceneMasterQuest(gPlayState->sceneNum) ? "MQ" : "Vanilla",
                static_cast<u16>(actor->params), actor->params & 0x1F, FireGoron_GetSwitchFlag(actor),
                static_cast<s32>(actor->world.pos.x), static_cast<s32>(actor->world.pos.y),
                static_cast<s32>(actor->world.pos.z), static_cast<u16>(actor->shape.rot.y));
}

static void FireGoron_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (!FireGoron_IsFireGoron(actor)) {
        return;
    }

    auto goronIdentity = IdentifyFireGoron(actor);
    if (goronIdentity.randomizerCheck == RC_UNKNOWN_CHECK) {
        return;
    }

    ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(goronIdentity));

    if (FireGoron_HoldsItem(actor) && Flags_GetSwitch(gPlayState, FireGoron_GetSwitchFlag(actor))) {
        EnGo2_GetItemAnimation(reinterpret_cast<EnGo2*>(actor), gPlayState);
    }
}

void RegisterShuffleFireGorons() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_FIRE_GORONS);

    COND_ID_HOOK(OnActorInit, ACTOR_EN_GO2, shouldRegister, FireGoron_RandomizerInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_GO2, shouldRegister, FireGoron_RandomizerUpdate);

    COND_VB_SHOULD(VB_DESPAWN_FREED_FIRE_GORON, shouldRegister, {
        EnGo2* goron = va_arg(args, EnGo2*);
        auto goronIdentity = IdentifyFireGoron(&goron->actor);

        if (*should && FireGoron_IdentityHoldsItem(goronIdentity) &&
            !Rando::Context::GetInstance()->GetItemLocation(goronIdentity.randomizerCheck)->IsExcluded()) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_FIRE_GORON, shouldRegister, {
        EnGo2* goron = va_arg(args, EnGo2*);

        if (FireGoron_HoldsItem(&goron->actor)) {
            Flags_SetRandomizerInf(ObjectExtension::GetInstance().Get<CheckIdentity>(&goron->actor)->randomizerInf);
        } else if (ObjectExtension::GetInstance().Get<CheckIdentity>(&goron->actor) == nullptr) {
            FireGoron_LogUnrecognized(&goron->actor);
        }
    });
}

static RegisterShipInitFunc initFunc_ShuffleFireGorons(RegisterShuffleFireGorons, { "IS_RANDO" });

void Rando::StaticData::RegisterFireGoronLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    //            Randomizer Check                                                          Randomizer Check                                    Quest            Area                Scene ID           Params  Short Name                             Hint Text Key                                        Spoiler Collection Check
    locationTable[RC_FIRE_TEMPLE_NEAR_BOSS_GORON]                     = Location::FireGoron(RC_FIRE_TEMPLE_NEAR_BOSS_GORON,                     RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 8,      "Near Boss Goron",                     RHT_FIRE_TEMPLE_NEAR_BOSS_GORON,                     SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_NEAR_BOSS_GORON));
    locationTable[RC_FIRE_TEMPLE_BOSS_KEY_GORON]                      = Location::FireGoron(RC_FIRE_TEMPLE_BOSS_KEY_GORON,                      RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 9,      "Boss Key Goron",                      RHT_FIRE_TEMPLE_BOSS_KEY_GORON,                      SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BOSS_KEY_GORON));
    locationTable[RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON] = Location::FireGoron(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON, RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 1,      "Big Lava Room Lower Open Door Goron", RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON, SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON));
    locationTable[RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON]    = Location::FireGoron(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON,    RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 2,      "Big Lava Room Blocked Door Goron",    RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON));
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON]            = Location::FireGoron(RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON,            RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 3,      "Boulder Maze Lower Goron",            RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON));
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON]         = Location::FireGoron(RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON,         RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 4,      "Boulder Maze Shortcut Goron",         RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON));
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON]        = Location::FireGoron(RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON,        RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 5,      "Boulder Maze Side Room Goron",        RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON));
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON]            = Location::FireGoron(RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON,            RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 10,     "Boulder Maze Upper Goron",            RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON));
    locationTable[RC_FIRE_TEMPLE_HIGHEST_GORON]                       = Location::FireGoron(RC_FIRE_TEMPLE_HIGHEST_GORON,                       RCQUEST_VANILLA, RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 11,     "Highest Goron",                       RHT_FIRE_TEMPLE_HIGHEST_GORON,                       SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_HIGHEST_GORON));
    locationTable[RC_FIRE_TEMPLE_MQ_MAP_GORON]                        = Location::FireGoron(RC_FIRE_TEMPLE_MQ_MAP_GORON,                        RCQUEST_MQ,      RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 9,      "MQ Map Goron",                        RHT_FIRE_TEMPLE_MQ_MAP_GORON,                        SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_MQ_MAP_GORON));
    locationTable[RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON]    = Location::FireGoron(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON,    RCQUEST_MQ,      RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 1,      "MQ Big Lava Room Open Door Goron",    RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON));
    locationTable[RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON] = Location::FireGoron(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON, RCQUEST_MQ,      RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 2,      "MQ Big Lava Room Blocked Door Goron", RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON, SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON));
    locationTable[RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON]    = Location::FireGoron(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON,    RCQUEST_MQ,      RCAREA_FIRE_TEMPLE, SCENE_FIRE_TEMPLE, 5,      "MQ Lizalfos Maze Side Room Goron",    RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON));
    // clang-format on
}

static RegisterShipInitFunc registerFireGoronLocations(Rando::StaticData::RegisterFireGoronLocations);
