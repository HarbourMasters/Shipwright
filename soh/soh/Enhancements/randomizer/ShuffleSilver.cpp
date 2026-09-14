#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "dungeon.h"
#include "SeedContext.h"
#include "draw.h"
#include "static_data.h"
#include "randomizer.h" // IWYU pragma: keep
#include "ShuffleSilver.h"

extern "C" {
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

bool IsSilverInPool(RandomizerGet rg) {
    auto ctx = Rando::Context::GetInstance();
    switch (rg) {
        case RG_SHADOW_SILVER_BLADES:
        case RG_SHADOW_SILVER_PIT:
        case RG_SHADOW_SILVER_SPIKES:
        case RG_GTG_SILVER_SLOPE:
        case RG_GTG_SILVER_LAVA:
        case RG_GTG_SILVER_WATER:
        case RG_GANONS_CASTLE_SILVER_FIRE:
            return true;
        case RG_SPIRIT_SILVER_CHILD:
        case RG_SPIRIT_SILVER_SUN:
        case RG_SPIRIT_SILVER_BOULDERS:
            return ctx->GetDungeon(Rando::SPIRIT_TEMPLE).IsVanilla();
        case RG_BOTW_SILVER:
            return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL).IsVanilla();
        case RG_ICE_CAVERN_SILVER_BLADES:
        case RG_ICE_CAVERN_SILVER_BLOCK:
            return ctx->GetDungeon(Rando::ICE_CAVERN).IsVanilla();
        case RG_GANONS_CASTLE_SILVER_LIGHT:
        case RG_GANONS_CASTLE_SILVER_FOREST:
        case RG_GANONS_CASTLE_SILVER_SPIRIT:
            return ctx->GetDungeon(Rando::GANONS_CASTLE).IsVanilla();
        case RG_DODONGOS_CAVERN_MQ_SILVER:
            return ctx->GetDungeon(Rando::DODONGOS_CAVERN).IsMQ();
        case RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES:
            return ctx->GetDungeon(Rando::SHADOW_TEMPLE).IsMQ();
        case RG_SPIRIT_MQ_SILVER_LOBBY:
        case RG_SPIRIT_MQ_SILVER_BIG_WALL:
            return ctx->GetDungeon(Rando::SPIRIT_TEMPLE).IsMQ();
        case RG_GANONS_CASTLE_MQ_SILVER_WATER:
        case RG_GANONS_CASTLE_MQ_SILVER_SHADOW:
            return ctx->GetDungeon(Rando::GANONS_CASTLE).IsMQ();
        default:
            assert(false);
            return false;
    }
}

std::set<RandomizerGet> Rando::StaticData::constantSilvers = { RG_SHADOW_SILVER_BLADES,     RG_SHADOW_SILVER_PIT,
                                                               RG_SHADOW_SILVER_SPIKES,     RG_GTG_SILVER_SLOPE,
                                                               RG_GTG_SILVER_LAVA,          RG_GTG_SILVER_WATER,
                                                               RG_GANONS_CASTLE_SILVER_FIRE };

std::unordered_map<RandomizerGet, RandomizerCheckArea> Rando::StaticData::silverToArea = {
    { RG_DODONGOS_CAVERN_MQ_SILVER, RCAREA_DODONGOS_CAVERN },
    { RG_SPIRIT_SILVER_CHILD, RCAREA_SPIRIT_TEMPLE },
    { RG_SPIRIT_SILVER_SUN, RCAREA_SPIRIT_TEMPLE },
    { RG_SPIRIT_SILVER_BOULDERS, RCAREA_SPIRIT_TEMPLE },
    { RG_SPIRIT_MQ_SILVER_LOBBY, RCAREA_SPIRIT_TEMPLE },
    { RG_SPIRIT_MQ_SILVER_BIG_WALL, RCAREA_SPIRIT_TEMPLE },
    { RG_SHADOW_SILVER_BLADES, RCAREA_SHADOW_TEMPLE },
    { RG_SHADOW_SILVER_PIT, RCAREA_SHADOW_TEMPLE },
    { RG_SHADOW_SILVER_SPIKES, RCAREA_SHADOW_TEMPLE },
    { RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES, RCAREA_SHADOW_TEMPLE },
    { RG_GANONS_CASTLE_SILVER_LIGHT, RCAREA_GANONS_CASTLE },
    { RG_GANONS_CASTLE_SILVER_FOREST, RCAREA_GANONS_CASTLE },
    { RG_GANONS_CASTLE_SILVER_FIRE, RCAREA_GANONS_CASTLE },
    { RG_GANONS_CASTLE_SILVER_SPIRIT, RCAREA_GANONS_CASTLE },
    { RG_GANONS_CASTLE_MQ_SILVER_WATER, RCAREA_GANONS_CASTLE },
    { RG_GANONS_CASTLE_MQ_SILVER_SHADOW, RCAREA_GANONS_CASTLE },
    { RG_ICE_CAVERN_SILVER_BLADES, RCAREA_ICE_CAVERN },
    { RG_ICE_CAVERN_SILVER_BLOCK, RCAREA_ICE_CAVERN },
    { RG_BOTW_SILVER, RCAREA_BOTTOM_OF_THE_WELL },
    { RG_GTG_SILVER_SLOPE, RCAREA_GERUDO_TRAINING_GROUND },
    { RG_GTG_SILVER_LAVA, RCAREA_GERUDO_TRAINING_GROUND },
    { RG_GTG_SILVER_WATER, RCAREA_GERUDO_TRAINING_GROUND }
};

RandomizerGet SilverFromSwitchFlag(s16 switchFlag) {
    auto dungeon = Rando::Context::GetInstance()->GetDungeonFromScene((SceneID)gPlayState->sceneNum);

    assert(dungeon.has_value());

    bool isMQ = dungeon.value()->IsMQ();
    switch (gPlayState->sceneNum) {
        case SCENE_DODONGOS_CAVERN:
            return RG_DODONGOS_CAVERN_MQ_SILVER;
        case SCENE_SHADOW_TEMPLE:
            switch (switchFlag) {
                case 1:
                    return RG_SHADOW_SILVER_BLADES;
                case 3:
                    return RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES;
                case 8:
                    return RG_SHADOW_SILVER_SPIKES;
                case 9:
                case 17:
                    return RG_SHADOW_SILVER_PIT;
            }
            break;
        case SCENE_SPIRIT_TEMPLE:
            switch (switchFlag) {
                case 0:
                    return RG_SPIRIT_MQ_SILVER_BIG_WALL;
                case 2:
                    return RG_SPIRIT_SILVER_BOULDERS;
                case 5:
                    return RG_SPIRIT_SILVER_CHILD;
                case 10:
                    return RG_SPIRIT_SILVER_SUN;
                case 55: // Likely error
                    return RG_SPIRIT_MQ_SILVER_LOBBY;
            }
            break;
        case SCENE_BOTTOM_OF_THE_WELL:
            return RG_BOTW_SILVER;
        case SCENE_ICE_CAVERN:
            switch (switchFlag) {
                case 8:
                case 9:
                    return RG_ICE_CAVERN_SILVER_BLOCK;
                case 31:
                    return RG_ICE_CAVERN_SILVER_BLADES;
            }
            break;
        case SCENE_GERUDO_TRAINING_GROUND:
            switch (switchFlag) {
                case 12:
                    return RG_GTG_SILVER_LAVA;
                case 27:
                    return RG_GTG_SILVER_WATER;
                case 28:
                    return RG_GTG_SILVER_SLOPE;
            }
            break;
        case SCENE_INSIDE_GANONS_CASTLE:
            switch (switchFlag) {
                case 1:
                    return RG_GANONS_CASTLE_SILVER_FIRE;
                case 2:
                    return RG_GANONS_CASTLE_MQ_SILVER_WATER;
                case 9:
                    return RG_GANONS_CASTLE_SILVER_FIRE;
                case 11:
                    return isMQ ? RG_GANONS_CASTLE_MQ_SILVER_SHADOW : RG_GANONS_CASTLE_SILVER_SPIRIT;
                case 14:
                    return RG_GANONS_CASTLE_SILVER_FOREST;
                case 18:
                    return RG_GANONS_CASTLE_SILVER_LIGHT;
            }
            break;
        default:
            break;
    }
    assert(false); // no matching silver found
    return RG_NONE;
}

s8* Randomizer::SilverFieldFromSaveContext(SaveContext* saveContext, RandomizerGet rg) {
    switch (rg) {
        case RG_SHADOW_SILVER_BLADES:
            return &saveContext->ship.quest.data.randomizer.silverShadowBlades;
        case RG_SHADOW_SILVER_PIT:
            return &saveContext->ship.quest.data.randomizer.silverShadowPit;
        case RG_SHADOW_SILVER_SPIKES:
            return &saveContext->ship.quest.data.randomizer.silverShadowSpikes;
        case RG_SPIRIT_SILVER_CHILD:
            return &saveContext->ship.quest.data.randomizer.silverSpiritChild;
        case RG_SPIRIT_SILVER_SUN:
            return &saveContext->ship.quest.data.randomizer.silverSpiritSun;
        case RG_SPIRIT_SILVER_BOULDERS:
            return &saveContext->ship.quest.data.randomizer.silverSpiritBoulders;
        case RG_BOTW_SILVER:
            return &saveContext->ship.quest.data.randomizer.silverBotw;
        case RG_ICE_CAVERN_SILVER_BLADES:
            return &saveContext->ship.quest.data.randomizer.silverIceCavernBlades;
        case RG_ICE_CAVERN_SILVER_BLOCK:
            return &saveContext->ship.quest.data.randomizer.silverIceCavernBlock;
        case RG_GTG_SILVER_SLOPE:
            return &saveContext->ship.quest.data.randomizer.silverGtgSlope;
        case RG_GTG_SILVER_LAVA:
            return &saveContext->ship.quest.data.randomizer.silverGtgLava;
        case RG_GTG_SILVER_WATER:
            return &saveContext->ship.quest.data.randomizer.silverGtgWater;
        case RG_GANONS_CASTLE_SILVER_LIGHT:
            return &saveContext->ship.quest.data.randomizer.silverGanonLight;
        case RG_GANONS_CASTLE_SILVER_FOREST:
            return &saveContext->ship.quest.data.randomizer.silverGanonForest;
        case RG_GANONS_CASTLE_SILVER_FIRE:
            return &saveContext->ship.quest.data.randomizer.silverGanonFire;
        case RG_GANONS_CASTLE_SILVER_SPIRIT:
            return &saveContext->ship.quest.data.randomizer.silverGanonSpirit;
        case RG_DODONGOS_CAVERN_MQ_SILVER:
            return &saveContext->ship.quest.data.randomizer.silverMqDodongosCavern;
        case RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES:
            return &saveContext->ship.quest.data.randomizer.silverMqShadowInvisibleBlades;
        case RG_SPIRIT_MQ_SILVER_LOBBY:
            return &saveContext->ship.quest.data.randomizer.silverMqSpiritLobby;
        case RG_SPIRIT_MQ_SILVER_BIG_WALL:
            return &saveContext->ship.quest.data.randomizer.silverMqSpiritBigWall;
        case RG_GANONS_CASTLE_MQ_SILVER_WATER:
            return &saveContext->ship.quest.data.randomizer.silverMqGanonWater;
        case RG_GANONS_CASTLE_MQ_SILVER_SHADOW:
            return &saveContext->ship.quest.data.randomizer.silverMqGanonShadow;
        default:
            return nullptr;
    }
}

s8 Randomizer::SilverTotal(RandomizerGet rg) {
    auto ctx = Rando::Context::GetInstance();
    return rg == RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES ||
                   (rg == RG_SHADOW_SILVER_SPIKES && ctx->GetDungeon(Rando::SHADOW_TEMPLE).IsMQ())
               ? 10
           : (rg == RG_GTG_SILVER_LAVA && ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND).IsMQ())  ? 6
           : (rg == RG_GTG_SILVER_WATER && ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND).IsMQ()) ? 3
                                                                                                  : 5;
}

bool IsSilverCleared(RandomizerGet rg) {
    return *Randomizer::SilverFieldFromSaveContext(&gSaveContext, rg) >= Randomizer::SilverTotal(rg);
}

bool IsSilver(RandomizerGet rg) {
    return rg >= RG_SHADOW_SILVER_BLADES && rg <= RG_GANONS_CASTLE_MQ_SILVER_SHADOW;
}

extern "C" void EnGSwitch_RandomizerDraw(Actor* thisx, PlayState* play) {
    EnGSwitch* silver = reinterpret_cast<EnGSwitch*>(thisx);
    Matrix_Push();
    Matrix_Scale(17.5f, 17.5f, 17.5f, MTXMODE_APPLY);
    if (silver->type == ENGSWITCH_SILVER_RUPEE) {
        if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
            GetItemEntry_Draw(play, GET_ITEM_MYSTERY);
        } else {
            auto silverIdentity =
                OTRGlobals::Instance->gRandomizer->IdentifySilver(gPlayState->sceneNum, silver->actor.world.pos);
            auto itemEntry =
                Rando::Context::GetInstance()->GetFinalGIEntry(silverIdentity.randomizerCheck, true, GI_NONE);
            GetItemEntry_Draw(play, itemEntry);
        }
        Matrix_Pop();
    }
}

static void SetSilverCleared(EnGSwitch* silver) {
    if (gPlayState->sceneNum == SCENE_GERUDO_TRAINING_GROUND && silver->actor.room == 2) {
        Flags_SetTempClear(gPlayState, silver->actor.room);
        Flags_SetClear(gPlayState, silver->actor.room);
    }
    Flags_SetSwitch(gPlayState, silver->switchFlag);
}

void RegisterShuffleSilver() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_SILVER);

    COND_VB_SHOULD(VB_SILVER_COLLECT, shouldRegister, {
        if (*should) {
            EnGSwitch* silver = va_arg(args, EnGSwitch*);
            auto silverIdentity =
                OTRGlobals::Instance->gRandomizer->IdentifySilver(gPlayState->sceneNum, silver->actor.world.pos);
            Flags_SetRandomizerInf(silverIdentity.randomizerInf);
            Actor_Kill(&silver->actor);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_SILVER_COUNT_CHECK, shouldRegister, {
        EnGSwitch* silver = va_arg(args, EnGSwitch*);
        *should = false;
        if (IsSilverCleared(SilverFromSwitchFlag(silver->switchFlag))) {
            SetSilverCleared(silver);
            Actor_Kill(&silver->actor);
        }
    });

    COND_VB_SHOULD(VB_SILVER_DESPAWN, shouldRegister, {
        EnGSwitch* silver = va_arg(args, EnGSwitch*);
        if (silver->type == ENGSWITCH_SILVER_RUPEE) {
            auto silverIdentity =
                OTRGlobals::Instance->gRandomizer->IdentifySilver(gPlayState->sceneNum, silver->actor.world.pos);
            *should = silverIdentity.randomizerCheck == RC_UNKNOWN_CHECK ||
                      Flags_GetRandomizerInf(silverIdentity.randomizerInf);
            if (!*should) {
                *should = false;
                silver->actor.draw = EnGSwitch_RandomizerDraw;
            }
        } else if (silver->type == ENGSWITCH_SILVER_TRACKER &&
                   IsSilverCleared(SilverFromSwitchFlag(silver->switchFlag))) {
            SetSilverCleared(silver);
            *should = true;
        }
    });
}

void Rando::StaticData::RegisterSilverLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    locationTable[RC_SHADOW_N_UNDER_BLADE_SILVER] =          Location::Collectable(RC_SHADOW_N_UNDER_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3243, -1061),     "North Under Blade Silver",          RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_RC_SHADOW_N_UNDER_BLADE_SILVER));
    locationTable[RC_SHADOW_TALL_BLOCK_BLADE_SILVER] =            Location::Collectable(RC_SHADOW_TALL_BLOCK_BLADE_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3007, -1222),     "Tall Block Blade Silver",           RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_RC_SHADOW_TALL_BLOCK_BLADE_SILVER));
    locationTable[RC_SHADOW_N_NOOK_BLADE_SILVER] =               Location::Collectable(RC_SHADOW_N_NOOK_BLADE_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3554, -1432),     "North Nook Blade Silver",           RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_RC_SHADOW_N_NOOK_BLADE_SILVER));
    locationTable[RC_SHADOW_W_NOOK_BLADE_SILVER] =               Location::Collectable(RC_SHADOW_W_NOOK_BLADE_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2869, -948),      "West Nook Blade Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_W_NOOK_BLADE_SILVER));
    locationTable[RC_SHADOW_S_UNDER_BLADE_SILVER] =               Location::Collectable(RC_SHADOW_S_UNDER_BLADE_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3399, -838),      "South Under Blade Silver",          RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_S_UNDER_BLADE_SILVER));
    locationTable[RC_SHADOW_N_PIT_SILVER] =                  Location::Collectable(RC_SHADOW_N_PIT_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2131, 3030),      "North Pit Silver",                  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_N_PIT_SILVER));
    locationTable[RC_SHADOW_S_PIT_SILVER] =                  Location::Collectable(RC_SHADOW_S_PIT_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2115, 3738),      "South Pit Silver",                  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_S_PIT_SILVER));
    locationTable[RC_SHADOW_BEAMOS_PIT_SILVER] =                  Location::Collectable(RC_SHADOW_BEAMOS_PIT_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2110, 3368),      "Beamos Pit Silver",                 RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_BEAMOS_PIT_SILVER));
    locationTable[RC_SHADOW_E_PIT_SILVER] =                  Location::Collectable(RC_SHADOW_E_PIT_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2243, 3361),      "East Pit Silver",                  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_E_PIT_SILVER));
    locationTable[RC_SHADOW_W_PIT_SILVER] =                  Location::Collectable(RC_SHADOW_W_PIT_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(1998, 3358),      "West Pit Silver",                   RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_W_PIT_SILVER));
    locationTable[RC_SHADOW_PLATFORM_SPIKES_SILVER] =               Location::Collectable(RC_SHADOW_PLATFORM_SPIKES_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2042, 849),       "Platform Spikes Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_PLATFORM_SPIKES_SILVER));
    locationTable[RC_SHADOW_E_TARGET_SPIKES_SILVER] =               Location::Collectable(RC_SHADOW_E_TARGET_SPIKES_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2940, 1069),      "East Target Spikes Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_E_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_FLOOR_SPIKES_SILVER] =               Location::Collectable(RC_SHADOW_FLOOR_SPIKES_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2478, 1208),      "Floor Spikes Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_FLOOR_SPIKES_SILVER));
    locationTable[RC_SHADOW_W_TARGET_SPIKES_SILVER] =               Location::Collectable(RC_SHADOW_W_TARGET_SPIKES_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2135, 1297),      "West Target Spikes Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_W_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_AIRBORNE_SPIKES_SILVER] =               Location::Collectable(RC_SHADOW_AIRBORNE_SPIKES_SILVER,               RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2254, 988),       "Airborne Spikes Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_AIRBORNE_SPIKES_SILVER));
    locationTable[RC_SPIRIT_W_HIGH_CHILD_SILVER] =                Location::Collectable(RC_SPIRIT_W_HIGH_CHILD_SILVER,                RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-766, -1075),     "West High Child Silver",                RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_W_HIGH_CHILD_SILVER));
    locationTable[RC_SPIRIT_W_LOW_CHILD_SILVER] =                Location::Collectable(RC_SPIRIT_W_LOW_CHILD_SILVER,                RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-672, -1075),     "West Low Child Silver",                RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_W_LOW_CHILD_SILVER));
    locationTable[RC_SPIRIT_E_LOW_CHILD_SILVER] =                Location::Collectable(RC_SPIRIT_E_LOW_CHILD_SILVER,                RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-511, -1075),     "East Low Child Silver",                RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_E_LOW_CHILD_SILVER));
    locationTable[RC_SPIRIT_E_HIGH_CHILD_SILVER] =                Location::Collectable(RC_SPIRIT_E_HIGH_CHILD_SILVER,                RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-347, -1075),     "East High Child Silver",                RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_E_HIGH_CHILD_SILVER));
    locationTable[RC_SPIRIT_TORCH_CHILD_SILVER] =                Location::Collectable(RC_SPIRIT_TORCH_CHILD_SILVER,                RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-335, -1409),     "Torch Child Silver",                RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TORCH_CHILD_SILVER));
    locationTable[RC_SPIRIT_NEAR_LIGHT_SUN_SILVER] =                  Location::Collectable(RC_SPIRIT_NEAR_LIGHT_SUN_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-1836, -446),     "Near Light Sun Silver",                  RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_NEAR_LIGHT_SUN_SILVER));
    locationTable[RC_SPIRIT_AIRBORNE_SUN_SILVER] =                  Location::Collectable(RC_SPIRIT_AIRBORNE_SUN_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-1433, -283),     "Airborne Sun Silver",                  RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_AIRBORNE_SUN_SILVER));
    locationTable[RC_SPIRIT_MID_PLATFORM_SUN_SILVER] =                  Location::Collectable(RC_SPIRIT_MID_PLATFORM_SUN_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-1275, -247),     "Middle Platform Sun Silver",                  RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MID_PLATFORM_SUN_SILVER));
    locationTable[RC_SPIRIT_NEAR_CHEST_SUN_SILVER] =                  Location::Collectable(RC_SPIRIT_NEAR_CHEST_SUN_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-1123, 428),      "Near Chest Sun Silver",                  RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_NEAR_CHEST_SUN_SILVER));
    locationTable[RC_SPIRIT_NEAR_DOOR_SUN_SILVER] =                  Location::Collectable(RC_SPIRIT_NEAR_DOOR_SUN_SILVER,                  RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-984, -450),      "Near Door Sun Silver",                  RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_NEAR_DOOR_SUN_SILVER));
    locationTable[RC_SPIRIT_NW_BOULDER_SILVER] =             Location::Collectable(RC_SPIRIT_NW_BOULDER_SILVER,             RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1284, -1355),     "Northwest Boulder Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_NW_BOULDER_SILVER));
    locationTable[RC_SPIRIT_SW_BOULDER_SILVER] =             Location::Collectable(RC_SPIRIT_SW_BOULDER_SILVER,             RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1284, -813),      "Southwest Boulder Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_SW_BOULDER_SILVER));
    locationTable[RC_SPIRIT_SE_BOULDER_SILVER] =             Location::Collectable(RC_SPIRIT_SE_BOULDER_SILVER,             RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1856, -944),      "Southeast Boulder Super",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_SE_BOULDER_SILVER));
    locationTable[RC_SPIRIT_NE_BOULDER_SILVER] =             Location::Collectable(RC_SPIRIT_NE_BOULDER_SILVER,             RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1856, -1219),     "Notheast Boulder Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_NE_BOULDER_SILVER));
    locationTable[RC_SPIRIT_AIRBORNE_BOULDER_SILVER] =             Location::Collectable(RC_SPIRIT_AIRBORNE_BOULDER_SILVER,             RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1573, -920),      "Airborne Boulder Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_AIRBORNE_BOULDER_SILVER));
    locationTable[RC_BOTW_B1_LADDER_SILVER] =                        Location::Collectable(RC_BOTW_B1_LADDER_SILVER,                        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_BOTTOM_OF_THE_WELL,           TWO_ACTOR_PARAMS(-796, -150),      "B1 Ladder Silver",                        RHT_BOTW_SILVER,                                         RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_B1_LADDER_SILVER));
    locationTable[RC_BOTW_B2_LADDER_SILVER] =                        Location::Collectable(RC_BOTW_B2_LADDER_SILVER,                        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_BOTTOM_OF_THE_WELL,           TWO_ACTOR_PARAMS(-614, -297),      "B2 Ladder Silver",                        RHT_BOTW_SILVER,                                         RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_B2_LADDER_SILVER));
    locationTable[RC_BOTW_B3_LADDER_SILVER] =                        Location::Collectable(RC_BOTW_B3_LADDER_SILVER,                        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_BOTTOM_OF_THE_WELL,           TWO_ACTOR_PARAMS(-560, -291),      "B3 Ladder Silver",                        RHT_BOTW_SILVER,                                         RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_B3_LADDER_SILVER));
    locationTable[RC_BOTW_NW_OOZE_SILVER] =                        Location::Collectable(RC_BOTW_NW_OOZE_SILVER,                        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_BOTTOM_OF_THE_WELL,           TWO_ACTOR_PARAMS(-402, -401),      "Northwest Ooze Silver",                        RHT_BOTW_SILVER,                                         RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_NW_OOZE_SILVER));
    locationTable[RC_BOTW_SE_OOZE_SILVER] =                        Location::Collectable(RC_BOTW_SE_OOZE_SILVER,                        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_BOTTOM_OF_THE_WELL,           TWO_ACTOR_PARAMS(-259, -234),      "Southeast Ooze Silver",                        RHT_BOTW_SILVER,                                         RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_SE_OOZE_SILVER));
    locationTable[RC_ICE_CAVERN_ICICLES_BLADE_SILVER] =           Location::Collectable(RC_ICE_CAVERN_ICICLES_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(1, -143),         "Icicles Blade Silver",           RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_ICICLES_BLADE_SILVER));
    locationTable[RC_ICE_CAVERN_W_INNER_BLADE_SILVER] =           Location::Collectable(RC_ICE_CAVERN_W_INNER_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(198, -388),       "West Inner Blade Silver",           RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_W_INNER_BLADE_SILVER));
    locationTable[RC_ICE_CAVERN_OUTER_BLADE_SILVER] =           Location::Collectable(RC_ICE_CAVERN_OUTER_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(278, -637),       "Outer Blade Silver",           RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_OUTER_BLADE_SILVER));
    locationTable[RC_ICE_CAVERN_E_INNER_BLADE_SILVER] =           Location::Collectable(RC_ICE_CAVERN_E_INNER_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(389, -382),       "East Inner Blade Silver",           RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_E_INNER_BLADE_SILVER));
    locationTable[RC_ICE_CAVERN_AIRBORNE_BLADE_SILVER] =           Location::Collectable(RC_ICE_CAVERN_AIRBORNE_BLADE_SILVER,           RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(414, -579),       "Airborne Blade Silver",           RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_AIRBORNE_BLADE_SILVER));
    locationTable[RC_ICE_CAVERN_NEAR_FIRE_BLOCK_SILVER] =            Location::Collectable(RC_ICE_CAVERN_NEAR_FIRE_BLOCK_SILVER,            RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(-1676, -552),     "Near Fire Block Silver",            RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_FIRE_BLOCK_SILVER));
    locationTable[RC_ICE_CAVERN_LOWER_BLOCK_SILVER] =            Location::Collectable(RC_ICE_CAVERN_LOWER_BLOCK_SILVER,            RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(-1558, -951),     "Lower Block Silver",            RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_LOWER_BLOCK_SILVER));
    locationTable[RC_ICE_CAVERN_NEAR_BLOCK_SILVER] =            Location::Collectable(RC_ICE_CAVERN_NEAR_BLOCK_SILVER,            RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(-1294, -899),     "Near Block Silver",            RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_BLOCK_SILVER));
    locationTable[RC_ICE_CAVERN_FROZEN_BLOCK_SILVER] =            Location::Collectable(RC_ICE_CAVERN_FROZEN_BLOCK_SILVER,            RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(-1120, -1577),    "Frozen Block Silver",            RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_FROZEN_BLOCK_SILVER));
    locationTable[RC_ICE_CAVERN_NEAR_GS_BLOCK_SILVER] =            Location::Collectable(RC_ICE_CAVERN_NEAR_GS_BLOCK_SILVER,            RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_ICE_CAVERN,                   TWO_ACTOR_PARAMS(-1040, -485),     "Near GS Block Silver",            RHT_ICE_CAVERN_SILVER,                                   RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_GS_BLOCK_SILVER));
    locationTable[RC_GTG_FIRE_WALL_SLOPE_SILVER] =                   Location::Collectable(RC_GTG_FIRE_WALL_SLOPE_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1332, -992),     "Fire Wall Slope Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_FIRE_WALL_SLOPE_SILVER));
    locationTable[RC_GTG_LOWER_BOULDER_SLOPE_SILVER] =                   Location::Collectable(RC_GTG_LOWER_BOULDER_SLOPE_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1886, -956),     "Lower Boulder Slope Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_LOWER_BOULDER_SLOPE_SILVER));
    locationTable[RC_GTG_TARGET_SLOPE_SILVER] =                   Location::Collectable(RC_GTG_TARGET_SLOPE_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1579, -999),     "Target Slope Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_TARGET_SLOPE_SILVER));
    locationTable[RC_GTG_JUMPDOWN_SLOPE_SILVER] =                   Location::Collectable(RC_GTG_JUMPDOWN_SLOPE_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1627, -1462),    "Jumpdown Slope Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_JUMPDOWN_SLOPE_SILVER));
    locationTable[RC_GTG_UPPER_BOULDER_SLOPE_SILVER] =                   Location::Collectable(RC_GTG_UPPER_BOULDER_SLOPE_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1887, -2134),    "Upper Boulder Slope Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_UPPER_BOULDER_SLOPE_SILVER));
    locationTable[RC_GTG_NEAR_SWITCH_LAVA_SILVER] =                    Location::Collectable(RC_GTG_NEAR_SWITCH_LAVA_SILVER,                    RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1320, -1248),     "Near Switch Lava Silver",                    RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_NEAR_SWITCH_LAVA_SILVER));
    locationTable[RC_GTG_NEAR_BARRED_DOOR_LAVA_SILVER] =                    Location::Collectable(RC_GTG_NEAR_BARRED_DOOR_LAVA_SILVER,                    RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1558, -1370),     "Near Barred Door Lava Silver",                    RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_NEAR_BARRED_DOOR_LAVA_SILVER));
    locationTable[RC_GTG_FIRE_RING_LAVA_SILVER] =                    Location::Collectable(RC_GTG_FIRE_RING_LAVA_SILVER,                    RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1134, -1841),     "Fire Ring Lava Silver",                    RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_FIRE_RING_LAVA_SILVER));
    locationTable[RC_GTG_UPPER_LAVA_SILVER] =                    Location::Collectable(RC_GTG_UPPER_LAVA_SILVER,                    RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1437, -2193),     "Upper Lava Silver",                    RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_UPPER_LAVA_SILVER));
    locationTable[RC_GTG_UNDER_LEDGE_LAVA_SILVER] =                    Location::Collectable(RC_GTG_UNDER_LEDGE_LAVA_SILVER,                    RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1560, -1861),     "Under Ledge Lava Silver",                    RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_UNDER_LEDGE_LAVA_SILVER));
    locationTable[RC_GTG_MIDDLE_WATER_SILVER] =                   Location::Collectable(RC_GTG_MIDDLE_WATER_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2308, -1464),     "Middle Water Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MIDDLE_WATER_SILVER));
    locationTable[RC_GTG_ABOVE_TARGET_WATER_SILVER] =                   Location::Collectable(RC_GTG_ABOVE_TARGET_WATER_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2497, -1465),     "Above Target Water Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_ABOVE_TARGET_WATER_SILVER));
    locationTable[RC_GTG_LEFT_WATER_SILVER] =                   Location::Collectable(RC_GTG_LEFT_WATER_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2453, -1612),     "Left Water Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_LEFT_WATER_SILVER));
    locationTable[RC_GTG_UNDER_TARGET_WATER_SILVER] =                   Location::Collectable(RC_GTG_UNDER_TARGET_WATER_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2078, -1458),     "Under Target Water Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_UNDER_TARGET_WATER_SILVER));
    locationTable[RC_GTG_RIGHT_WATER_SILVER] =                   Location::Collectable(RC_GTG_RIGHT_WATER_SILVER,                   RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2160, -1315),     "Right Water Silver",                   RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_RIGHT_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_OUTER_RIGHT_LIGHT_SILVER] =         Location::Collectable(RC_GANONS_CASTLE_OUTER_RIGHT_LIGHT_SILVER,         RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2509, -1091),    "Outer Right Light Trial Silver",         RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_OUTER_RIGHT_LIGHT_SILVER));
    locationTable[RC_GANONS_CASTLE_OUTER_LEFT_LIGHT_SILVER] =         Location::Collectable(RC_GANONS_CASTLE_OUTER_LEFT_LIGHT_SILVER,         RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2649, -545),     "Outer Left Light Trial Silver",         RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_OUTER_LEFT_LIGHT_SILVER));
    locationTable[RC_GANONS_CASTLE_UPPER_LIGHT_SILVER] =         Location::Collectable(RC_GANONS_CASTLE_UPPER_LIGHT_SILVER,         RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2646, -839),     "Upper Light Trial Silver",         RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_UPPER_LIGHT_SILVER));
    locationTable[RC_GANONS_CASTLE_INNER_RIGHT_LIGHT_SILVER] =         Location::Collectable(RC_GANONS_CASTLE_INNER_RIGHT_LIGHT_SILVER,         RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2680, -893),     "Inner Right Light Trial Silver",         RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_INNER_RIGHT_LIGHT_SILVER));
    locationTable[RC_GANONS_CASTLE_INNER_LEFT_LIGHT_SILVER] =         Location::Collectable(RC_GANONS_CASTLE_INNER_LEFT_LIGHT_SILVER,         RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2674, -777),     "Inner Left Light Trial Silver",         RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_INNER_LEFT_LIGHT_SILVER));
    locationTable[RC_GANONS_CASTLE_LARGE_PLATFORM_FOREST_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_LARGE_PLATFORM_FOREST_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1247, 1787),      "Large Platform Forest Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_LARGE_PLATFORM_FOREST_SILVER));
    locationTable[RC_GANONS_CASTLE_SOT_BLOCK_FOREST_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_SOT_BLOCK_FOREST_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1361, 1222),      "SoT Block Forest Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_SOT_BLOCK_FOREST_SILVER));
    locationTable[RC_GANONS_CASTLE_SMALL_PLATFORM_FOREST_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_SMALL_PLATFORM_FOREST_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1538, 2225),      "Small Platform Forest Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_SMALL_PLATFORM_FOREST_SILVER));
    locationTable[RC_GANONS_CASTLE_UNDER_LEDGE_FOREST_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_UNDER_LEDGE_FOREST_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1651, 2021),      "Under Ledge Forest Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_UNDER_LEDGE_FOREST_SILVER));
    locationTable[RC_GANONS_CASTLE_ON_TARGET_FOREST_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_ON_TARGET_FOREST_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1634, 1550),      "On Target Forest Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_ON_TARGET_FOREST_SILVER));
    locationTable[RC_GANONS_CASTLE_TORCH_SLUG_FIRE_SILVER] =          Location::Collectable(RC_GANONS_CASTLE_TORCH_SLUG_FIRE_SILVER,          RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-854, -3679),     "Torch Slug Fire Trial Silver",          RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_TORCH_SLUG_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_FLAME_JETS_FIRE_SILVER] =          Location::Collectable(RC_GANONS_CASTLE_FLAME_JETS_FIRE_SILVER,          RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-1555, -2317),    "Flame Jets Fire Temple Silver",          RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_FLAME_JETS_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_DISTANT_PLATFORM_FIRE_SILVER] =          Location::Collectable(RC_GANONS_CASTLE_DISTANT_PLATFORM_FIRE_SILVER,          RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-515, -3253),     "Distant Platform Fire Trial Silver",          RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_DISTANT_PLATFORM_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_CLOSE_PLATFORM_FIRE_SILVER] =          Location::Collectable(RC_GANONS_CASTLE_CLOSE_PLATFORM_FIRE_SILVER,          RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-604, -2592),     "Close Platform Fire Trial Silver",          RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_CLOSE_PLATFORM_FIRE_SILVER));
    locationTable[RC_GANONS_GANONS_CASTLE_UNDER_PILLAR_FIRE_SILVER] =          Location::Collectable(RC_GANONS_GANONS_CASTLE_UNDER_PILLAR_FIRE_SILVER,          RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-1867, -2754),    "Under Pillar Fire Trial Silver",          RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_GANONS_CASTLE_UNDER_PILLAR_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_AIRBORNE_SPIRIT_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_AIRBORNE_SPIRIT_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-829, 591),       "Airborne Spirit Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_AIRBORNE_SPIRIT_SILVER));
    locationTable[RC_GANONS_CASTLE_NEAR_DOORMAT_SPIRIT_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_NEAR_DOORMAT_SPIRIT_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-940, 270),       "Near Doormat Spirit Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_NEAR_DOORMAT_SPIRIT_SILVER));
    locationTable[RC_GANONS_CASTLE_LOWER_MIDDLE_SPIRIT_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_LOWER_MIDDLE_SPIRIT_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-865, 656),       "Lower Middle Spirit Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_LOWER_MIDDLE_SPIRIT_SILVER));
    locationTable[RC_GANONS_CASTLE_CORNER_SPIRIT_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_CORNER_SPIRIT_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-725, 942),       "Corner Spirit Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_CORNER_SPIRIT_SILVER));
    locationTable[RC_GANONS_CASTLE_NEAR_BARRED_DOOR_SPIRIT_SILVER] =        Location::Collectable(RC_GANONS_CASTLE_NEAR_BARRED_DOOR_SPIRIT_SILVER,        RCQUEST_VANILLA,    RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-1142, 639),      "Near Barred Door Spirit Trial Silver",        RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_NEAR_BARRED_DOOR_SPIRIT_SILVER));
    locationTable[RC_DODONGOS_CAVERN_MQ_BEAMOS_SILVER] =          Location::Collectable(RC_DODONGOS_CAVERN_MQ_BEAMOS_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_DODONGOS_CAVERN,              TWO_ACTOR_PARAMS(-2277, -1362),    "MQ Beamos Silver",          RHT_DODONGOS_CAVERN_SILVER,                              RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_BEAMOS_SILVER));
    locationTable[RC_DODONGOS_CAVERN_MQ_1F_CRATE_SILVER] =          Location::Collectable(RC_DODONGOS_CAVERN_MQ_1F_CRATE_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_DODONGOS_CAVERN,              TWO_ACTOR_PARAMS(-2189, -1834),    "MQ 1F Crate Silver",          RHT_DODONGOS_CAVERN_SILVER,                              RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_1F_CRATE_SILVER));
    locationTable[RC_DODONGOS_CAVERN_MQ_LOWER_2F_CRATE_SILVER] =          Location::Collectable(RC_DODONGOS_CAVERN_MQ_LOWER_2F_CRATE_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_DODONGOS_CAVERN,              TWO_ACTOR_PARAMS(-2411, -1836),    "MQ Lower 2F Crate Silver",          RHT_DODONGOS_CAVERN_SILVER,                              RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LOWER_2F_CRATE_SILVER));
    locationTable[RC_DODONGOS_CAVERN_MQ_VINES_SILVER] =          Location::Collectable(RC_DODONGOS_CAVERN_MQ_VINES_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_DODONGOS_CAVERN,              TWO_ACTOR_PARAMS(-1907, -1243),    "MQ Vines Silver",          RHT_DODONGOS_CAVERN_SILVER,                              RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_VINES_SILVER));
    locationTable[RC_DODONGOS_CAVERN_MQ_UPPER_2F_CRATE_SILVER] =          Location::Collectable(RC_DODONGOS_CAVERN_MQ_UPPER_2F_CRATE_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_DODONGOS_CAVERN,              TWO_ACTOR_PARAMS(-1512, -1083),    "MQ Upper 2F Crate Silver",          RHT_DODONGOS_CAVERN_SILVER,                              RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_UPPER_2F_CRATE_SILVER));
    locationTable[RC_SHADOW_MQ_W_NOOK_SILVER] =            Location::Collectable(RC_SHADOW_MQ_W_NOOK_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2810, -961 ),     "MQ West Nook Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_W_NOOK_SILVER));
    locationTable[RC_SHADOW_MQ_TALL_BLOCK_BLADE_SILVER] =            Location::Collectable(RC_SHADOW_MQ_TALL_BLOCK_BLADE_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3007, -1222),     "MQ Tall Block Blade Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_TALL_BLOCK_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_NE_UNDER_BLADE_SILVER] =            Location::Collectable(RC_SHADOW_MQ_NE_UNDER_BLADE_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3243, -1061),     "MQ Northwest Under Blade Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_NE_UNDER_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_S_UNDER_BLADE_SILVER] =            Location::Collectable(RC_SHADOW_MQ_S_UNDER_BLADE_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3399, -838 ),     "MQ South Under Blade Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_S_UNDER_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_N_NOOK_BLADE_SILVER] =            Location::Collectable(RC_SHADOW_MQ_N_NOOK_BLADE_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(3558, -1490),     "MQ North Nook Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_N_NOOK_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_W_PIT_SILVER] =               Location::Collectable(RC_SHADOW_MQ_W_PIT_SILVER,               RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(1970, 3372),      "MQ West Pit Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_W_PIT_SILVER));
    locationTable[RC_SHADOW_MQ_HIGH_PIT_SILVER] =               Location::Collectable(RC_SHADOW_MQ_HIGH_PIT_SILVER,               RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2110, -970),      "MQ High Pit Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_HIGH_PIT_SILVER));
    locationTable[RC_SHADOW_MQ_HIGHEST_PIT_SILVER] =               Location::Collectable(RC_SHADOW_MQ_HIGHEST_PIT_SILVER,               RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2110, -1092),     "MQ Highest Pit Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_HIGHEST_PIT_SILVER));
    locationTable[RC_SHADOW_MQ_N_PIT_SILVER] =               Location::Collectable(RC_SHADOW_MQ_N_PIT_SILVER,               RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2131, 3030),      "MQ North Pit Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_N_PIT_SILVER));
    locationTable[RC_SHADOW_MQ_E_PIT_SILVER] =               Location::Collectable(RC_SHADOW_MQ_E_PIT_SILVER,               RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2250, 3372),      "MQ East Pit Silver",               RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_E_PIT_SILVER));
    locationTable[RC_SHADOW_MQ_W_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_W_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5089, 2049),      "MQ West Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_W_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_SW_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_SW_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5158, 2315),      "MQ Southwest Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_SW_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_NW_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_NW_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5217, 1852),      "MQ Northwest Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_NW_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_S_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_S_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5270, 2453),      "MQ South Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_S_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_INNER_NE_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_INNER_NE_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5404, 1977),      "MQ Inner Northeast Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_INNER_NE_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_SE_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_SE_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5466, 2243),      "MQ Southeast Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_SE_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_OUTSIDE_CIRCLE_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_OUTSIDE_CIRCLE_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5489, 2476),      "MQ Outside Circle Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_OUTSIDE_CIRCLE_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_OUTER_NE_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_OUTER_NE_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5601, 1898),      "MQ Outer Northeast Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_OUTER_NE_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_E_INVISIBLE_BLADE_SILVER] =  Location::Collectable(RC_SHADOW_MQ_E_INVISIBLE_BLADE_SILVER,  RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5637, 2134),      "MQ East Invisible Blade Silver",  RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_E_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_SOT_BLOCK_INVISIBLE_BLADE_SILVER] = Location::Collectable(RC_SHADOW_MQ_SOT_BLOCK_INVISIBLE_BLADE_SILVER, RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(5667, 2686),      "MQ SoT Block Invisible Blade Silver", RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_SOT_BLOCK_INVISIBLE_BLADE_SILVER));
    locationTable[RC_SHADOW_MQ_PLATFORM_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_PLATFORM_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2042, 849),       "MQ Platform Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_PLATFORM_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_W_TARGET_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_W_TARGET_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2135, 1297),      "MQ West Target Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_W_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_W_AIRBORNE_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_W_AIRBORNE_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2254, 988),       "MQ West Airborne Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_W_AIRBORNE_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_N_TARGET_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_N_TARGET_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2478, 893),       "MQ North Target Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_N_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_CENTRAL_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_CENTRAL_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2478, 1208),      "MQ Central Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_CENTRAL_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_CENTRAL_TARGET_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_CENTRAL_TARGET_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2478, 1200),      "MQ Central Target Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_CENTRAL_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_S_TARGET_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_S_TARGET_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2478, 1404),      "MQ South Target Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_S_TARGET_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_INSIDE_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_INSIDE_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2705, 1089),      "MQ Inside Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_INSIDE_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_E_AIRBORNE_SPIKES_SILVER] =            Location::Collectable(RC_SHADOW_MQ_E_AIRBORNE_SPIKES_SILVER,            RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2730, 876),       "MQ East Airborne Spikes Silver",            RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_E_AIRBORNE_SPIKES_SILVER));
    locationTable[RC_SHADOW_MQ_E_TARGET_SPIKES_SILVER] =           Location::Collectable(RC_SHADOW_MQ_E_TARGET_SPIKES_SILVER,           RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SHADOW_TEMPLE,                TWO_ACTOR_PARAMS(2940, 1069),      "MQ East Target Spikes Silver",           RHT_SHADOW_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_MQ_E_TARGET_SPIKES_SILVER));
    locationTable[RC_SPIRIT_MQ_STAIRS_LOBBY_SILVER] =             Location::Collectable(RC_SPIRIT_MQ_STAIRS_LOBBY_SILVER,             RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(1016, -73),       "MQ Stairs Lobby Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_STAIRS_LOBBY_SILVER));
    locationTable[RC_SPIRIT_MQ_E_BOULDER_LOBBY_SILVER] =             Location::Collectable(RC_SPIRIT_MQ_E_BOULDER_LOBBY_SILVER,             RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(160, 268),        "MQ East Boulder Lobby Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_E_BOULDER_LOBBY_SILVER));
    locationTable[RC_SPIRIT_MQ_W_BOULDER_LOBBY_SILVER] =             Location::Collectable(RC_SPIRIT_MQ_W_BOULDER_LOBBY_SILVER,             RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(-160, 270),       "MQ West Boulder Lobby Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_W_BOULDER_LOBBY_SILVER));
    locationTable[RC_SPIRIT_MQ_CORNER_LOBBY_SILVER] =             Location::Collectable(RC_SPIRIT_MQ_CORNER_LOBBY_SILVER,             RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(967, -547),       "MQ Corner Lobby Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_CORNER_LOBBY_SILVER));
    locationTable[RC_SPIRIT_MQ_JET_LOBBY_SILVER] =             Location::Collectable(RC_SPIRIT_MQ_JET_LOBBY_SILVER,             RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(744, 4),          "MQ Jet Lobby Silver",             RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_JET_LOBBY_SILVER));
    locationTable[RC_SPIRIT_MQ_LOWEST_WALL_SILVER] =          Location::Collectable(RC_SPIRIT_MQ_LOWEST_WALL_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(723, -75),        "MQ Lowest Big Wall Silver",          RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_LOWEST_WALL_SILVER));
    locationTable[RC_SPIRIT_MQ_MID_LOW_WALL_SILVER] =          Location::Collectable(RC_SPIRIT_MQ_MID_LOW_WALL_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(582, -75),        "MQ Mid-Low Wall Silver",          RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_MID_LOW_WALL_SILVER));
    locationTable[RC_SPIRIT_MQ_MID_HIGH_WALL_SILVER] =          Location::Collectable(RC_SPIRIT_MQ_MID_HIGH_WALL_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(754, -75),        "MQ Mid-High Wall Silver",          RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_MID_HIGH_WALL_SILVER));
    locationTable[RC_SPIRIT_MQ_HIGHEST_WALL_SILVER] =          Location::Collectable(RC_SPIRIT_MQ_HIGHEST_WALL_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(644, -75),        "MQ Highest Wall Silver",          RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_HIGHEST_WALL_SILVER));
    locationTable[RC_SPIRIT_MQ_MIDDLE_WALL_SILVER] =          Location::Collectable(RC_SPIRIT_MQ_MIDDLE_WALL_SILVER,          RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_SPIRIT_TEMPLE,                TWO_ACTOR_PARAMS(681, -75),        "MQ Middle Wall Silver",          RHT_SPIRIT_TEMPLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_MQ_MIDDLE_WALL_SILVER));
    locationTable[RC_GTG_MQ_ICICLES_SLOPE_SILVER] =                Location::Collectable(RC_GTG_MQ_ICICLES_SLOPE_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1245, -2112),    "MQ Icicles Slope Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_ICICLES_SLOPE_SILVER));
    locationTable[RC_GTG_MQ_JUMPDOWN_SLOPE_SILVER] =                Location::Collectable(RC_GTG_MQ_JUMPDOWN_SLOPE_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1542, -1467),    "MQ Jumpdown Slope Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_JUMPDOWN_SLOPE_SILVER));
    locationTable[RC_GTG_MQ_HIGH_SLOPE_SILVER] =                Location::Collectable(RC_GTG_MQ_HIGH_SLOPE_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1480, -1000),    "MQ High Slope Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_HIGH_SLOPE_SILVER));
    locationTable[RC_GTG_MQ_FREEZARD_SLOPE_SILVER] =                Location::Collectable(RC_GTG_MQ_FREEZARD_SLOPE_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1886, -956),     "MQ Freezard Slope Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_FREEZARD_SLOPE_SILVER));
    locationTable[RC_GTG_MQ_PIT_SLOPE_SILVER] =                Location::Collectable(RC_GTG_MQ_PIT_SLOPE_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(-1261, -923),     "MQ Pit Slope Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_PIT_SLOPE_SILVER));
    locationTable[RC_GTG_MQ_NEAR_SWITCH_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_NEAR_SWITCH_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1560, -1861),     "MQ Near Switch Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_NEAR_SWITCH_LAVA_SILVER));
    locationTable[RC_GTG_MQ_FAR_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_FAR_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1134, -1841),     "MQ Far Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_FAR_LAVA_SILVER));
    locationTable[RC_GTG_MQ_MIDDLE_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_MIDDLE_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1404, -1653),     "MQ Middle Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_MIDDLE_LAVA_SILVER));
    locationTable[RC_GTG_MQ_NEAR_BARRED_DOOR_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_NEAR_BARRED_DOOR_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1560, -1380),     "MQ Near Barred Door Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_SILVER_LAVA_4));
    locationTable[RC_GTG_MQ_NEAR_MAZE_LEDGE_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_NEAR_MAZE_LEDGE_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1083, -1374),     "MQ Near Maze Ledge Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_NEAR_MAZE_LEDGE_LAVA_SILVER));
    locationTable[RC_GTG_MQ_NEAR_LIT_TORCH_LAVA_SILVER] =                 Location::Collectable(RC_GTG_MQ_NEAR_LIT_TORCH_LAVA_SILVER,                 RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(1317, -1243),     "MQ Near Lit Torch Lava Silver",                 RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_NEAR_LIT_TORCH_LAVA_SILVER));
    locationTable[RC_GTG_MQ_BACK_WATER_SILVER] =                Location::Collectable(RC_GTG_MQ_BACK_WATER_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2453, -1612),     "MQ Back Water Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_BACK_WATER_SILVER));
    locationTable[RC_GTG_MQ_MIDDLE_WATER_SILVER] =                Location::Collectable(RC_GTG_MQ_MIDDLE_WATER_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2302, -1464),     "MQ Middle Water Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_MIDDLE_WATER_SILVER));
    locationTable[RC_GTG_MQ_FRONT_WATER_SILVER] =                Location::Collectable(RC_GTG_MQ_FRONT_WATER_SILVER,                RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_GERUDO_TRAINING_GROUND,       TWO_ACTOR_PARAMS(2160, -1315),     "MQ Front Water Silver",                RHT_GTG_SILVER,                                          RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GTG_MQ_FRONT_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_UNDER_PILLAR_FIRE_SILVER] =       Location::Collectable(RC_GANONS_CASTLE_MQ_UNDER_PILLAR_FIRE_SILVER,       RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-970, -3747),     "MQ Under Pillar Fire Trial Silver",       RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_UNDER_PILLAR_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_NEAR_TARGET_FIRE_SILVER] =       Location::Collectable(RC_GANONS_CASTLE_MQ_NEAR_TARGET_FIRE_SILVER,       RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-1891, -2753),    "MQ Near Target Fire Trial Silver",       RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_NEAR_TARGET_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_ON_PILLAR_FIRE_SILVER] =       Location::Collectable(RC_GANONS_CASTLE_MQ_ON_PILLAR_FIRE_SILVER,       RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-2044, -3354),    "MQ On Pillar Fire Trial Silver",       RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_ON_PILLAR_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_LAUNCH_PLATFORM_FIRE_SILVER] =       Location::Collectable(RC_GANONS_CASTLE_MQ_LAUNCH_PLATFORM_FIRE_SILVER,       RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-686, -2945),     "MQ Launch Platform Fire Trial Silver",       RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_LAUNCH_PLATFORM_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_JET_PLATFORM_FIRE_SILVER] =       Location::Collectable(RC_GANONS_CASTLE_MQ_JET_PLATFORM_FIRE_SILVER,       RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(-1655, -2133),    "MQ Jet Platform Fire Trial Silver",       RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_JET_PLATFORM_FIRE_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_IN_HOLE_WATER_SILVER] =      Location::Collectable(RC_GANONS_CASTLE_MQ_IN_HOLE_WATER_SILVER,      RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(2757, -765),      "MQ In Hole Water Trial Silver",      RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_IN_HOLE_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_AIRBORNE_WATER_SILVER] =      Location::Collectable(RC_GANONS_CASTLE_MQ_AIRBORNE_WATER_SILVER,      RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(2912, -971),      "MQ Airborne Water Trial Silver",      RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_AIRBORNE_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_FROZEN_WATER_SILVER] =      Location::Collectable(RC_GANONS_CASTLE_MQ_FROZEN_WATER_SILVER,      RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(2905, -1478),     "MQ Frozen Water Trial Silver",      RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_FROZEN_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_OVER_PIT_WATER_SILVER] =      Location::Collectable(RC_GANONS_CASTLE_MQ_OVER_PIT_WATER_SILVER,      RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(3253, -649),      "MQ Over Pit Water Trial Silver",      RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_OVER_PIT_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_NOOK_WATER_SILVER] =      Location::Collectable(RC_GANONS_CASTLE_MQ_NOOK_WATER_SILVER,      RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(2905, -1263),     "MQ Nook Water Trial Silver",      RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_NOOK_WATER_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_BOMB_FLOWER_SHADOW_SILVER] =     Location::Collectable(RC_GANONS_CASTLE_MQ_BOMB_FLOWER_SHADOW_SILVER,     RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1322, -2262),     "MQ Bomb Flower Shadow Trial Silver",     RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_BOMB_FLOWER_SHADOW_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_NEAR_TORCH_SHADOW_SILVER] =     Location::Collectable(RC_GANONS_CASTLE_MQ_NEAR_TORCH_SHADOW_SILVER,     RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1279, -3111),     "MQ Near Torch Shadow Trial Silver",     RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_NEAR_TORCH_SHADOW_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_BEAMOS_PLATFORM_SHADOW_SILVER] =     Location::Collectable(RC_GANONS_CASTLE_MQ_BEAMOS_PLATFORM_SHADOW_SILVER,     RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1529, -4117),     "MQ Beamos Platform Shadow Trial Silver",     RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_BEAMOS_PLATFORM_SHADOW_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_GUILLOTINE_SHADOW_SILVER] =     Location::Collectable(RC_GANONS_CASTLE_MQ_GUILLOTINE_SHADOW_SILVER,     RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1829, -4071),     "MQ Guillotine Shadow Trial Silver",     RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_GUILLOTINE_SHADOW_SILVER));
    locationTable[RC_GANONS_CASTLE_MQ_MOVING_PLATFORM_SHADOW_SILVER] =     Location::Collectable(RC_GANONS_CASTLE_MQ_MOVING_PLATFORM_SHADOW_SILVER,     RCQUEST_MQ,         RCTYPE_SILVER, ACTOR_EN_G_SWITCH, SCENE_INSIDE_GANONS_CASTLE,         TWO_ACTOR_PARAMS(1100, -2554),     "MQ Moving Platform Shadow Trial Silver",     RHT_GANONS_CASTLE_SILVER,                                RG_BLUE_RUPEE,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_MOVING_PLATFORM_SHADOW_SILVER));
    // clang-format on
}

static RegisterShipInitFunc registerShuffleSilver(RegisterShuffleSilver, { "IS_RANDO" });
static RegisterShipInitFunc registerShuffleSilverLocations(Rando::StaticData::RegisterSilverLocations);
