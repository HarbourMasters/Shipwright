#include "ShuffleRocks.h"
#include "static_data.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "item_category_adj.h"
#include "particle_cmc.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/RCToRandInf.h"

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Ishi/z_en_ishi.h"
#include "overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.h"
#include "overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_bombiwa/object_bombiwa.h"
#include "objects/object_tk/object_tk.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

static void Sparkles(PlayState* play, Actor* actor, bool boulder, CheckIdentity rockIdentity) {
    GraphicsContext* __gfxCtx = play->state.gfxCtx;
    int csmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), 0);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    GetItemCategory getItemCategory;
    if (csmc && (!requiresStoneAgony || (requiresStoneAgony && CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)))) {
        auto itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rockIdentity.randomizerCheck, true, GI_NONE);
        getItemCategory = Randomizer_AdjustItemCategory(itemEntry);
    } else {
        getItemCategory = ITEM_CATEGORY_MAJOR;
    }

    Color_RGBA8 primColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_PRIMARY);

    f32 yOffset = !boulder ? 40.0f : actor->id == ACTOR_OBJ_BOMBIWA ? 160.0f : 180.0f;
    f32 xOffset = !boulder ? -24.0f : actor->id == ACTOR_OBJ_BOMBIWA ? -90.0f : -90.0f;
    f32 zOffset = !boulder ? 4.0f : actor->id == ACTOR_OBJ_BOMBIWA ? 14.5f : 14.5f;

    // Rotate and draw halo with CMC colors
    if (rockIdentity.randomizerCheck == RC_SPIRIT_TEMPLE_MQ_ENTRANCE_CEILING_BOULDER ||
        rockIdentity.randomizerCheck == RC_ZF_UNDERGROUND_BOULDER) {
        yOffset = -114.0f;
        xOffset = -165.0f;
        zOffset = 19.0f;
        Matrix_Translate(actor->world.pos.x + xOffset, actor->world.pos.y + yOffset, actor->world.pos.z + zOffset,
                         MTXMODE_NEW);
        Matrix_Scale(0.055f, 0.055f, 0.055f, MTXMODE_APPLY);
    } else {
        Matrix_Translate(actor->world.pos.x + xOffset, actor->world.pos.y + yOffset, actor->world.pos.z + zOffset,
                         MTXMODE_NEW);
        Matrix_RotateZ(-M_PI / 2, MTXMODE_APPLY);
        if (boulder) {
            Matrix_Scale(0.04f, 0.04f, 0.04f, MTXMODE_APPLY);
        } else {
            Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        }
    }

    OPEN_DISPS(gPlayState->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gPlayState->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetGrayscaleColor(POLY_OPA_DISP++, primColor.r, primColor.g, primColor.b, 175);
    gSPGrayscale(POLY_OPA_DISP++, true);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gDampeHaloDL);
    gSPGrayscale(POLY_OPA_DISP++, false);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

extern "C" void EnIshi_RandomizerDraw(Actor* thisx, PlayState* play) {
    auto rockActor = ((EnIshi*)thisx);
    const auto rockIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(thisx);

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    if (rockActor->actor.params & 1) {
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSilverRockDL);
    } else {
        Gfx_DrawDListOpa(play, (Gfx*)gFieldKakeraDL);
    }
    CLOSE_DISPS(play->state.gfxCtx);

    if (rockIdentity != nullptr && rockIdentity->randomizerCheck != RC_MAX &&
        Flags_GetRandomizerInf(rockIdentity->randomizerInf) == 0) {
        Sparkles(play, &rockActor->actor, !!(rockActor->actor.params & 1), *rockIdentity);
    }
}

extern "C" void ObjBombiwa_RandomizerDraw(Actor* thisx, PlayState* play) {
    auto rockActor = ((ObjBombiwa*)thisx);
    const auto rockIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(thisx);

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Gfx_DrawDListOpa(play, (Gfx*)object_bombiwa_DL_0009E0);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)object_bombiwa_DL_0009E0);
    CLOSE_DISPS(play->state.gfxCtx);

    if (rockIdentity != nullptr && rockIdentity->randomizerCheck != RC_MAX &&
        Flags_GetRandomizerInf(rockIdentity->randomizerInf) == 0) {
        Sparkles(play, &rockActor->actor, true, *rockIdentity);
    }
}

extern "C" void ObjHamishi_RandomizerDraw(Actor* thisx, PlayState* play) {
    auto rockActor = ((ObjHamishi*)thisx);
    const auto rockIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(thisx);

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 170, 130, 255);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSilverRockDL);
    CLOSE_DISPS(play->state.gfxCtx);

    if (rockIdentity != nullptr && rockIdentity->randomizerCheck != RC_MAX &&
        Flags_GetRandomizerInf(rockIdentity->randomizerInf) == 0) {
        Sparkles(play, &rockActor->actor, true, *rockIdentity);
    }
}

uint8_t Rock_RandomizerHoldsItem(CheckIdentity rockIdentity, PlayState* play, bool isBoulder) {
    RandomizerCheck rc = rockIdentity.randomizerCheck;
    if (rc == RC_MAX || rc == RC_UNKNOWN_CHECK)
        return false;

    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    uint8_t setting =
        Rando::Context::GetInstance()->GetOption(isBoulder ? RSK_SHUFFLE_BOULDERS : RSK_SHUFFLE_ROCKS).Get();

    // Don't pull randomized item if rock isn't randomized or is already checked
    return IS_RANDO &&
           ((!isBoulder && setting) || (isBoulder && (setting == RO_SHUFFLE_BOULDERS_ALL ||
                                                      (isDungeon && setting == RO_SHUFFLE_BOULDERS_DUNGEONS) ||
                                                      (!isDungeon && setting == RO_SHUFFLE_BOULDERS_OVERWORLD)))) &&
           !Flags_GetRandomizerInf(rockIdentity.randomizerInf);
}

void Rock_RandomizerSpawnCollectible(Actor* actor, CheckIdentity rockIdentity, PlayState* play) {
    LUSLOG_INFO("ROCKdrop %d\t:\t%d, %d", rockIdentity.randomizerCheck, (s16)actor->world.pos.x,
                (s16)actor->world.pos.z);
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &actor->world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = rockIdentity.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rockIdentity.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 9.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
    switch (rockIdentity.randomizerCheck) {
        case RC_SPIRIT_TEMPLE_MQ_ENTRANCE_EYE_BOULDER:
            item00->actor.world.rot.y = 0x8000;
            break;
        case RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_LOW:
            item00->actor.velocity.y = 15.0f;
            [[fallthrough]];
        case RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_HIGH:
            item00->actor.world.rot.y = 0x0;
            item00->actor.speedXZ = 8.0f;
            break;
        case RC_GC_MAZE_BOULDER_1:
        case RC_GC_MAZE_BOULDER_2:
        case RC_GC_MAZE_BOULDER_3:
        case RC_GC_MAZE_BOULDER_4:
        case RC_GC_MAZE_BOULDER_5:
        case RC_GC_MAZE_BOULDER_6:
        case RC_GC_MAZE_BOULDER_7:
        case RC_GC_MAZE_BOULDER_8:
        case RC_GC_MAZE_BOULDER_9:
        case RC_GC_MAZE_BOULDER_10:
        case RC_GC_MAZE_BRONZE_BOULDER_1:
        case RC_GC_MAZE_BRONZE_BOULDER_2:
        case RC_GC_MAZE_BRONZE_BOULDER_3:
        case RC_GC_MAZE_BRONZE_BOULDER_4:
        case RC_GC_MAZE_BRONZE_BOULDER_5:
        case RC_DMC_BRONZE_BOULDER_SHORTCUT:
        case RC_ZF_UNDERGROUND_BOULDER:
        case RC_DEKU_TREE_MQ_BOULDER_1:
        case RC_DEKU_TREE_MQ_BOULDER_2:
        case RC_DEKU_TREE_MQ_BOULDER_3:
        case RC_ZR_BOULDER_4:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_1:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_2:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_3:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_4:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_5:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_6:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_7:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_8:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_9:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_10:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_11:
        case RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_12:
        case RC_SPIRIT_TEMPLE_MQ_ENTRANCE_CEILING_BOULDER:
        case RC_SPIRIT_TEMPLE_MQ_CRAWLSPACE_BOULDER:
            item00->actor.speedXZ = 0.0f;
            break;
        default:;
    }
}

static CheckIdentity IdentifyRock(s32 sceneNum, s32 posX, s32 posZ) {
    CheckIdentity rockIdentity;

    rockIdentity.randomizerInf = RAND_INF_MAX;
    rockIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    Rando::Location* location = OTRGlobals::Instance->gRandomizer->GetCheckObjectFromActor(
        ACTOR_EN_ISHI, sceneNum, TWO_ACTOR_PARAMS(posX, posZ));

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        rockIdentity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        rockIdentity.randomizerCheck = location->GetRandomizerCheck();
    } else {
        LUSLOG_WARN("IdentifyRock did not receive a valid RC value %d,%d.", posX, posZ);
    }

    return rockIdentity;
}

void EnIshi_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    EnIshi* rockActor = static_cast<EnIshi*>(actorRef);
    auto rockIdentity = IdentifyRock(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
    if (rockIdentity.randomizerCheck == RC_MAX) {
        LUSLOG_WARN("ROCK ishi  %d\t:\t%d, %d", rockIdentity.randomizerCheck, actor->params & 1,
                    (s16)actor->world.pos.x, (s16)actor->world.pos.z);
    } else {
        LUSLOG_INFO("ROCK ishi%d %d\t:\t%d, %d", rockIdentity.randomizerCheck, actor->params & 1,
                    (s16)actor->world.pos.x, (s16)actor->world.pos.z);
    }

    if (Rock_RandomizerHoldsItem(rockIdentity, gPlayState, actor->params & 1) && rockActor->actor.draw != nullptr) {
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(rockIdentity));
        rockActor->actor.draw = EnIshi_RandomizerDraw;
    }
}

void ObjBombiwa_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    ObjBombiwa* rockActor = static_cast<ObjBombiwa*>(actorRef);
    auto rockIdentity = IdentifyRock(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
    if (rockIdentity.randomizerCheck == RC_MAX) {
        LUSLOG_INFO("ROCK bombiwa\t:\t%d, %d", rockIdentity.randomizerCheck, (s16)actor->world.pos.x,
                    (s16)actor->world.pos.z);
    } else {
        LUSLOG_INFO("ROCK bombiwa%d\t:\t%d, %d", rockIdentity.randomizerCheck, (s16)actor->world.pos.x,
                    (s16)actor->world.pos.z);
    }
    if (Rock_RandomizerHoldsItem(rockIdentity, gPlayState, true) && rockActor->actor.draw != nullptr) {
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(rockIdentity));
        rockActor->actor.draw = ObjBombiwa_RandomizerDraw;
    }
}

void ObjHamishi_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    ObjHamishi* rockActor = static_cast<ObjHamishi*>(actorRef);
    auto rockIdentity = IdentifyRock(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
    if (rockIdentity.randomizerCheck == RC_MAX) {
        LUSLOG_WARN("ROCK hamishi\t:\t%d, %d", rockIdentity.randomizerCheck, (s16)actor->world.pos.x,
                    (s16)actor->world.pos.z);
    } else {
        LUSLOG_INFO("ROCK hamishi%d\t:\t%d, %d", rockIdentity.randomizerCheck, (s16)actor->world.pos.x,
                    (s16)actor->world.pos.z);
    }
    if (Rock_RandomizerHoldsItem(rockIdentity, gPlayState, true) && rockActor->actor.draw != nullptr) {
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(rockIdentity));
        rockActor->actor.draw = ObjHamishi_RandomizerDraw;
    }
}

void RegisterShuffleRock() {
    bool shouldRegister = IS_RANDO && (RAND_GET_OPTION(RSK_SHUFFLE_ROCKS) || RAND_GET_OPTION(RSK_SHUFFLE_BOULDERS));
    bool shouldRegisterBoulder = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_BOULDERS);

    COND_ID_HOOK(OnActorInit, ACTOR_EN_ISHI, shouldRegister, EnIshi_RandomizerInit);
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_BOMBIWA, shouldRegisterBoulder, ObjBombiwa_RandomizerInit);
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_HAMISHI, shouldRegisterBoulder, ObjHamishi_RandomizerInit);

    COND_VB_SHOULD(VB_ROCK_DROP_ITEM, shouldRegister, {
        Actor* rockActor = va_arg(args, Actor*);
        const auto rockIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(rockActor);
        if (rockIdentity != nullptr &&
            Rock_RandomizerHoldsItem(*rockIdentity, gPlayState,
                                     rockActor->id == ACTOR_OBJ_BOMBIWA || rockActor->id == ACTOR_OBJ_HAMISHI ||
                                         rockActor->params & 1)) {
            Rock_RandomizerSpawnCollectible(rockActor, *rockIdentity, gPlayState);
            rockIdentity->randomizerCheck = RC_MAX;
            rockIdentity->randomizerInf = RAND_INF_MAX;
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_BOULDER_BREAK_FLAG, shouldRegisterBoulder, {
        if (*should) {
            Actor* rockActor = va_arg(args, Actor*);
            // hook called before OnActorInit sets up object extension
            auto rockIdentity =
                IdentifyRock(gPlayState->sceneNum, (s16)rockActor->world.pos.x, (s16)rockActor->world.pos.z);
            if (Rock_RandomizerHoldsItem(rockIdentity, gPlayState, true)) {
                Rock_RandomizerSpawnCollectible(rockActor, rockIdentity, gPlayState);
            }
        }
    });
}

void Rando::StaticData::RegisterRockLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    locationTable[RC_KF_CIRCLE_ROCK_1]              = Location::Rock(RC_KF_CIRCLE_ROCK_1,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-292, -350),   "KF Circle Rock 1",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_1));
    locationTable[RC_KF_CIRCLE_ROCK_2]              = Location::Rock(RC_KF_CIRCLE_ROCK_2,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-235, -373),   "KF Circle Rock 2",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_2));
    locationTable[RC_KF_CIRCLE_ROCK_3]              = Location::Rock(RC_KF_CIRCLE_ROCK_3,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-212, -430),   "KF Circle Rock 3",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_3));
    locationTable[RC_KF_CIRCLE_ROCK_4]              = Location::Rock(RC_KF_CIRCLE_ROCK_4,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-235, -486),   "KF Circle Rock 4",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_4));
    locationTable[RC_KF_CIRCLE_ROCK_5]              = Location::Rock(RC_KF_CIRCLE_ROCK_5,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-292, -510),   "KF Circle Rock 5",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_5));
    locationTable[RC_KF_CIRCLE_ROCK_6]              = Location::Rock(RC_KF_CIRCLE_ROCK_6,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-348, -486),   "KF Circle Rock 6",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_6));
    locationTable[RC_KF_CIRCLE_ROCK_7]              = Location::Rock(RC_KF_CIRCLE_ROCK_7,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-372, -430),   "KF Circle Rock 7",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_7));
    locationTable[RC_KF_CIRCLE_ROCK_8]              = Location::Rock(RC_KF_CIRCLE_ROCK_8,              RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-348, -373),   "KF Circle Rock 8",                         RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CIRCLE_ROCK_8));
    locationTable[RC_KF_ROCK_BY_SARIAS_HOUSE]       = Location::Rock(RC_KF_ROCK_BY_SARIAS_HOUSE,       RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(248, 601),     "Sarias House Rock KF",                     RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_ROCK_BY_SARIAS_HOUSE));
    locationTable[RC_KF_ROCK_BEHIND_SARIAS_HOUSE]   = Location::Rock(RC_KF_ROCK_BEHIND_SARIAS_HOUSE,   RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(726, 961),     "Behind Sarias House Rock KF",              RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_ROCK_BEHIND_SARIAS_HOUSE));
    locationTable[RC_KF_ROCK_BY_MIDOS_HOUSE]        = Location::Rock(RC_KF_ROCK_BY_MIDOS_HOUSE,        RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-672, -623),   "Midos House Rock KF",                      RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_ROCK_BY_MIDOS_HOUSE));
    locationTable[RC_KF_ROCK_BY_KNOW_IT_ALLS_HOUSE] = Location::Rock(RC_KF_ROCK_BY_KNOW_IT_ALLS_HOUSE, RCQUEST_BOTH, RCAREA_KOKIRI_FOREST,         SCENE_KOKIRI_FOREST,               TWO_ACTOR_PARAMS(-1361, 145),   "Know It Alls House Rock KF",               RHT_KF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_ROCK_BY_KNOW_IT_ALLS_HOUSE));

    locationTable[RC_LW_BOULDER_BY_GORON_CITY]           = Location::Boulder(RC_LW_BOULDER_BY_GORON_CITY,           RCQUEST_BOTH, RCAREA_LOST_WOODS, SCENE_LOST_WOODS,                TWO_ACTOR_PARAMS(915, -925),    "Goron City Boulder LW",                    RHT_LW_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_LW_BOULDER_BY_GORON_CITY));
    locationTable[RC_LW_BOULDER_BY_SACRED_FOREST_MEADOW] = Location::Boulder(RC_LW_BOULDER_BY_SACRED_FOREST_MEADOW, RCQUEST_BOTH, RCAREA_LOST_WOODS, SCENE_LOST_WOODS,                TWO_ACTOR_PARAMS(670, -2520),   "Sacred Forest Meadow Boulder LW",          RHT_LW_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_LW_BOULDER_BY_SACRED_FOREST_MEADOW));
    locationTable[RC_LW_RUPEE_BOULDER]                   = Location::Boulder(RC_LW_RUPEE_BOULDER,                   RCQUEST_BOTH, RCAREA_LOST_WOODS, SCENE_LOST_WOODS,                TWO_ACTOR_PARAMS(1720, -2510),  "Rupee Boulder LW",                         RHT_LW_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_LW_RUPEE_BOULDER));

    locationTable[RC_HC_ROCK_1]                        = Location::Rock(RC_HC_ROCK_1,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-216, 2977),   "HC Rock 1",                                RHT_HC_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_ROCK_1));
    locationTable[RC_HC_ROCK_2]                        = Location::Rock(RC_HC_ROCK_2,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-110, 3006),   "HC Rock 2",                                RHT_HC_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_ROCK_2));
    locationTable[RC_HC_ROCK_3]                        = Location::Rock(RC_HC_ROCK_3,                    RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(-129, 2897),   "HC Rock 3",                                RHT_HC_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_ROCK_3));
    locationTable[RC_HC_BOULDER]                       = Location::Boulder(RC_HC_BOULDER,                RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_HYRULE_CASTLE,               TWO_ACTOR_PARAMS(2730, 2540),   "HC Boulder",                               RHT_HC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_BOULDER));
    locationTable[RC_OGC_BRONZE_BOULDER_1]             = Location::Boulder(RC_OGC_BRONZE_BOULDER_1,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(2324, 533),    "OGC Bronze Boulder 1",                     RHT_OGC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_BRONZE_BOULDER_1));
    locationTable[RC_OGC_BRONZE_BOULDER_2]             = Location::Boulder(RC_OGC_BRONZE_BOULDER_2,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(1590, 787),    "OGC Bronze Boulder 2",                     RHT_OGC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_BRONZE_BOULDER_2));
    locationTable[RC_OGC_BRONZE_BOULDER_3]             = Location::Boulder(RC_OGC_BRONZE_BOULDER_3,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(1661, 748),    "OGC Bronze Boulder 3",                     RHT_OGC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_BRONZE_BOULDER_3));
    locationTable[RC_OGC_SILVER_BOULDER_1]             = Location::Boulder(RC_OGC_SILVER_BOULDER_1,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(1606, 685),    "OGC Silver Boulder 1",                     RHT_OGC_SILVER_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_SILVER_BOULDER_1));
    locationTable[RC_OGC_SILVER_BOULDER_2]             = Location::Boulder(RC_OGC_SILVER_BOULDER_2,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(1766, 726),    "OGC Silver Boulder 2",                     RHT_OGC_SILVER_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_SILVER_BOULDER_2));
    locationTable[RC_OGC_SILVER_BOULDER_3]             = Location::Boulder(RC_OGC_SILVER_BOULDER_3,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(1701, 661),    "OGC Silver Boulder 3",                     RHT_OGC_SILVER_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_SILVER_BOULDER_3));
    locationTable[RC_OGC_SILVER_BOULDER_4]             = Location::Boulder(RC_OGC_SILVER_BOULDER_4,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_OUTSIDE_GANONS_CASTLE,       TWO_ACTOR_PARAMS(2260, 560),    "OGC Silver Boulder 4",                     RHT_OGC_SILVER_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_OGC_SILVER_BOULDER_4));

    locationTable[RC_DMC_ROCK_BY_FIRE_TEMPLE_1]        = Location::Rock(RC_DMC_ROCK_BY_FIRE_TEMPLE_1,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-50, -714),    "DMC Fire Temple Rock 1",           RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_ROCK_BY_FIRE_TEMPLE_1));
    locationTable[RC_DMC_ROCK_BY_FIRE_TEMPLE_2]        = Location::Rock(RC_DMC_ROCK_BY_FIRE_TEMPLE_2,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-26, -807),    "DMC Fire Temple Rock 2",           RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_ROCK_BY_FIRE_TEMPLE_2));
    locationTable[RC_DMC_ROCK_BY_FIRE_TEMPLE_3]        = Location::Rock(RC_DMC_ROCK_BY_FIRE_TEMPLE_3,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(61, -763),     "DMC Fire Temple Rock 3",           RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_ROCK_BY_FIRE_TEMPLE_3));
    locationTable[RC_DMC_ROCK_BY_FIRE_TEMPLE_4]        = Location::Rock(RC_DMC_ROCK_BY_FIRE_TEMPLE_4,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(71, -610),     "DMC Fire Temple Rock 4",           RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_ROCK_BY_FIRE_TEMPLE_4));
    locationTable[RC_DMC_ROCK_BY_FIRE_TEMPLE_5]        = Location::Rock(RC_DMC_ROCK_BY_FIRE_TEMPLE_5,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(79, -700),     "DMC Fire Temple Rock 5",           RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_ROCK_BY_FIRE_TEMPLE_5));
    locationTable[RC_DMC_CIRCLE_ROCK_1]                = Location::Rock(RC_DMC_CIRCLE_ROCK_1,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(40, 1850),     "DMC Circle Rock 1",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_1));
    locationTable[RC_DMC_CIRCLE_ROCK_2]                = Location::Rock(RC_DMC_CIRCLE_ROCK_2,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(96, 1826),     "DMC Circle Rock 2",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_2));
    locationTable[RC_DMC_CIRCLE_ROCK_3]                = Location::Rock(RC_DMC_CIRCLE_ROCK_3,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(120, 1770),    "DMC Circle Rock 3",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_3));
    locationTable[RC_DMC_CIRCLE_ROCK_4]                = Location::Rock(RC_DMC_CIRCLE_ROCK_4,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(96, 1713),     "DMC Circle Rock 4",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_4));
    locationTable[RC_DMC_CIRCLE_ROCK_5]                = Location::Rock(RC_DMC_CIRCLE_ROCK_5,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(40, 1690),     "DMC Circle Rock 5",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_5));
    locationTable[RC_DMC_CIRCLE_ROCK_6]                = Location::Rock(RC_DMC_CIRCLE_ROCK_6,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-16, 1713),    "DMC Circle Rock 6",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_6));
    locationTable[RC_DMC_CIRCLE_ROCK_7]                = Location::Rock(RC_DMC_CIRCLE_ROCK_7,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-40, 1770),    "DMC Circle Rock 7",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_7));
    locationTable[RC_DMC_CIRCLE_ROCK_8]                = Location::Rock(RC_DMC_CIRCLE_ROCK_8,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-16, 1826),    "DMC Circle Rock 8",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_CIRCLE_ROCK_8));
    locationTable[RC_DMC_GOSSIP_ROCK_1]                = Location::Rock(RC_DMC_GOSSIP_ROCK_1,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(1261, 1533),   "DMC Gossip Rock 1",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_GOSSIP_ROCK_1));
    locationTable[RC_DMC_GOSSIP_ROCK_2]                = Location::Rock(RC_DMC_GOSSIP_ROCK_2,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(1356, 1541),   "DMC Gossip Rock 2",                RHT_DMC_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_GOSSIP_ROCK_2));
    locationTable[RC_DMC_BOULDER_1]                    = Location::Boulder(RC_DMC_BOULDER_1,               RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-504, 1070),   "DMC Boulder 1",                    RHT_DMC_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BOULDER_1));
    locationTable[RC_DMC_BOULDER_2]                    = Location::Boulder(RC_DMC_BOULDER_2,               RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(236, 1199),    "DMC Boulder 2",                    RHT_DMC_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BOULDER_2));
    locationTable[RC_DMC_BOULDER_3]                    = Location::Boulder(RC_DMC_BOULDER_3,               RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(40, 1770),     "DMC Boulder 3",                    RHT_DMC_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BOULDER_3));
    locationTable[RC_DMC_BRONZE_BOULDER_1]             = Location::Boulder(RC_DMC_BRONZE_BOULDER_1,        RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-1699, -472),  "DMC Bronze Boulder 1",             RHT_DMC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BRONZE_BOULDER_1));
    locationTable[RC_DMC_BRONZE_BOULDER_2]             = Location::Boulder(RC_DMC_BRONZE_BOULDER_2,        RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-1332, 921),   "DMC Bronze Boulder 2",             RHT_DMC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BRONZE_BOULDER_2));
    locationTable[RC_DMC_BRONZE_BOULDER_3]             = Location::Boulder(RC_DMC_BRONZE_BOULDER_3,        RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-1303, 975),   "DMC Bronze Boulder 3",             RHT_DMC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BRONZE_BOULDER_3));
    locationTable[RC_DMC_BRONZE_BOULDER_SHORTCUT]      = Location::Boulder(RC_DMC_BRONZE_BOULDER_SHORTCUT, RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_CRATER, SCENE_DEATH_MOUNTAIN_CRATER,       TWO_ACTOR_PARAMS(-1060, 944),   "DMC Bronze Shortcut Boulder",      RHT_DMC_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BRONZE_BOULDER_SHORTCUT));

    locationTable[RC_GV_SILVER_BOULDER]                 = Location::Boulder(RC_GV_SILVER_BOULDER,                 RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(280, 1470),    "GV Silver Boulder",                        RHT_GV_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_SILVER_BOULDER));
    locationTable[RC_GV_ROCK_1]                         = Location::Rock(RC_GV_ROCK_1,                            RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(2738, 297),    "GV Rock 1",                                RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_1));
    locationTable[RC_GV_ROCK_2]                         = Location::Rock(RC_GV_ROCK_2,                            RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(2715, 316),    "GV Rock 2",                                RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_2));
    locationTable[RC_GV_ROCK_3]                         = Location::Rock(RC_GV_ROCK_3,                            RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(2699, 275),    "GV Rock 3",                                RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_3));
    locationTable[RC_GV_UNDERWATER_ROCK_1]              = Location::Rock(RC_GV_UNDERWATER_ROCK_1,                 RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(1559, -63),    "GV Underwater Rock 1",                     RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_UNDERWATER_ROCK_1));
    locationTable[RC_GV_UNDERWATER_ROCK_2]              = Location::Rock(RC_GV_UNDERWATER_ROCK_2,                 RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(1605, 26),     "GV Underwater Rock 2",                     RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_UNDERWATER_ROCK_2));
    locationTable[RC_GV_UNDERWATER_ROCK_3]              = Location::Rock(RC_GV_UNDERWATER_ROCK_3,                 RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(1686, -33),    "GV Underwater Rock 3",                     RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_UNDERWATER_ROCK_3));
    locationTable[RC_GV_ROCK_ACROSS_BRIDGE_1]           = Location::Rock(RC_GV_ROCK_ACROSS_BRIDGE_1,              RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-666, -899),   "GV Rock Across Bridge 1",                  RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_ACROSS_BRIDGE_1));
    locationTable[RC_GV_ROCK_ACROSS_BRIDGE_2]           = Location::Rock(RC_GV_ROCK_ACROSS_BRIDGE_2,              RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-526, -890),   "GV Rock Across Bridge 2",                  RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_ACROSS_BRIDGE_2));
    locationTable[RC_GV_ROCK_ACROSS_BRIDGE_3]           = Location::Rock(RC_GV_ROCK_ACROSS_BRIDGE_3,              RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-607, -791),   "GV Rock Across Bridge 3",                  RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_ACROSS_BRIDGE_3));
    locationTable[RC_GV_ROCK_ACROSS_BRIDGE_4]           = Location::Rock(RC_GV_ROCK_ACROSS_BRIDGE_4,              RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-458, -782),   "GV Rock Across Bridge 4",                  RHT_GV_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_ROCK_ACROSS_BRIDGE_4));
    locationTable[RC_GV_BOULDER_1]                      = Location::Boulder(RC_GV_BOULDER_1,                      RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(751, 569),     "GV Boulder 1",                             RHT_GV_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BOULDER_1));
    locationTable[RC_GV_BOULDER_2]                      = Location::Boulder(RC_GV_BOULDER_2,                      RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(545, -510),    "GV Boulder 2",                             RHT_GV_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BOULDER_2));
    locationTable[RC_GV_BOULDER_ACROSS_BRIDGE]          = Location::Boulder(RC_GV_BOULDER_ACROSS_BRIDGE,          RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-954, 577),    "GV Boulder Across Bridge",                 RHT_GV_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BOULDER_ACROSS_BRIDGE));
    locationTable[RC_GV_BRONZE_BOULDER_1]               = Location::Boulder(RC_GV_BRONZE_BOULDER_1,               RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(861, -778),    "GV Bronze Boulder 1",                      RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_1));
    locationTable[RC_GV_BRONZE_BOULDER_2]               = Location::Boulder(RC_GV_BRONZE_BOULDER_2,               RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(735, 375),     "GV Bronze Boulder 2",                      RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_2));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_1] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_1, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1352, 767),   "GV Bronze Boulder Across Bridge 1",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_1));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_2] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_2, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1695, -350),  "GV Bronze Boulder Across Bridge 2",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_2));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_3] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_3, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1001, 637),   "GV Bronze Boulder Across Bridge 3",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_3));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_4] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_4, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1291, 787),   "GV Bronze Boulder Across Bridge 4",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_4));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_5] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_5, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1416, 778),   "GV Bronze Boulder Across Bridge 5",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_5));
    locationTable[RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_6] = Location::Boulder(RC_GV_BRONZE_BOULDER_ACROSS_BRIDGE_6, RCQUEST_BOTH, RCAREA_GERUDO_VALLEY, SCENE_GERUDO_VALLEY,               TWO_ACTOR_PARAMS(-1256, 856),   "GV Bronze Boulder Across Bridge 6",        RHT_GV_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRONZE_BOULDER_ACROSS_BRIDGE_6));

    locationTable[RC_HF_SILVER_BOULDER]                = Location::Boulder(RC_HF_SILVER_BOULDER,         RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(674, 8256),    "HF Silver Boulder",                        RHT_HF_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_SILVER_BOULDER));
    locationTable[RC_HF_ROCK_1]                        = Location::Rock(RC_HF_ROCK_1,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7875, 6995),  "HF Circle Rock 1",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_1));
    locationTable[RC_HF_ROCK_2]                        = Location::Rock(RC_HF_ROCK_2,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7818, 6971),  "HF Circle Rock 2",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_2));
    locationTable[RC_HF_ROCK_3]                        = Location::Rock(RC_HF_ROCK_3,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7795, 6915),  "HF Circle Rock 3",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_3));
    locationTable[RC_HF_ROCK_4]                        = Location::Rock(RC_HF_ROCK_4,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7818, 6858),  "HF Circle Rock 4",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_4));
    locationTable[RC_HF_ROCK_5]                        = Location::Rock(RC_HF_ROCK_5,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7875, 6835),  "HF Circle Rock 5",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_5));
    locationTable[RC_HF_ROCK_6]                        = Location::Rock(RC_HF_ROCK_6,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7931, 6858),  "HF Circle Rock 6",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_6));
    locationTable[RC_HF_ROCK_7]                        = Location::Rock(RC_HF_ROCK_7,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7955, 6915),  "HF Circle Rock 7",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_7));
    locationTable[RC_HF_ROCK_8]                        = Location::Rock(RC_HF_ROCK_8,                    RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7931, 6971),  "HF Circle Rock 8",                         RHT_HF_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCK_8));
    locationTable[RC_HF_BOULDER_NORTH]                 = Location::Boulder(RC_HF_BOULDER_NORTH,          RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-4450, -425),  "HF Boulder North",                         RHT_HF_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BOULDER_NORTH));
    locationTable[RC_HF_BOULDER_BY_MARKET]             = Location::Boulder(RC_HF_BOULDER_BY_MARKET,      RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-1425, 810),   "Market Boulder HF",                        RHT_HF_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BOULDER_BY_MARKET));
    locationTable[RC_HF_BOULDER_SOUTH]                 = Location::Boulder(RC_HF_BOULDER_SOUTH,          RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-270, 12350),  "HF Boulder South",                         RHT_HF_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BOULDER_SOUTH));
    locationTable[RC_HF_BRONZE_BOULDER_1]              = Location::Boulder(RC_HF_BRONZE_BOULDER_1,       RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7870, 6920),  "HF Bronze Boulder 1",                      RHT_HF_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BRONZE_BOULDER_1));
    locationTable[RC_HF_BRONZE_BOULDER_2]              = Location::Boulder(RC_HF_BRONZE_BOULDER_2,       RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-7804, 7983),  "HF Bronze Boulder 2",                      RHT_HF_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BRONZE_BOULDER_2));
    locationTable[RC_HF_BRONZE_BOULDER_3]              = Location::Boulder(RC_HF_BRONZE_BOULDER_3,       RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-8397, 7947),  "HF Bronze Boulder 3",                      RHT_HF_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BRONZE_BOULDER_3));
    locationTable[RC_HF_BRONZE_BOULDER_4]              = Location::Boulder(RC_HF_BRONZE_BOULDER_4,       RCQUEST_BOTH, RCAREA_HYRULE_FIELD,          SCENE_HYRULE_FIELD,                TWO_ACTOR_PARAMS(-6461, 8220),  "HF Bronze Boulder 4",                      RHT_HF_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_BRONZE_BOULDER_4));

    locationTable[RC_KAK_SILVER_BOULDER]               = Location::Boulder(RC_KAK_SILVER_BOULDER,        RCQUEST_BOTH, RCAREA_KAKARIKO_VILLAGE,      SCENE_KAKARIKO_VILLAGE,            TWO_ACTOR_PARAMS(1436, 1361),   "Kak Silver Boulder",                       RHT_KAK_SILVER_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_SILVER_BOULDER));
    locationTable[RC_KAK_ROCK_1]                       = Location::Rock(RC_KAK_ROCK_1,                   RCQUEST_BOTH, RCAREA_KAKARIKO_VILLAGE,      SCENE_KAKARIKO_VILLAGE,            TWO_ACTOR_PARAMS(220, -1236),   "Kak Rock 1",                               RHT_KAK_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_ROCK_1));
    locationTable[RC_KAK_ROCK_2]                       = Location::Rock(RC_KAK_ROCK_2,                   RCQUEST_BOTH, RCAREA_KAKARIKO_VILLAGE,      SCENE_KAKARIKO_VILLAGE,            TWO_ACTOR_PARAMS(-664, 1288),   "Kak Rock 2",                               RHT_KAK_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_ROCK_2));
    locationTable[RC_GY_ROCK]                          = Location::Rock(RC_GY_ROCK,                      RCQUEST_BOTH, RCAREA_GRAVEYARD,             SCENE_GRAVEYARD,                   TWO_ACTOR_PARAMS(-1193, 693),   "GY Rock",                                  RHT_GY_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_ROCK));

    locationTable[RC_LH_ROCK]                          = Location::Rock(RC_LH_ROCK,                      RCQUEST_BOTH, RCAREA_LAKE_HYLIA,            SCENE_LAKE_HYLIA,                  TWO_ACTOR_PARAMS(1222, 3953),   "LH Rock",                                  RHT_LH_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_ROCK));

    locationTable[RC_ZD_CIRCLE_ROCK_1]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_1,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(462, -696),    "ZD Circle Rock 1",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_1));
    locationTable[RC_ZD_CIRCLE_ROCK_2]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_2,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(518, -719),    "ZD Circle Rock 2",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_2));
    locationTable[RC_ZD_CIRCLE_ROCK_3]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_3,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(542, -776),    "ZD Circle Rock 3",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_3));
    locationTable[RC_ZD_CIRCLE_ROCK_4]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_4,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(518, -832),    "ZD Circle Rock 4",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_4));
    locationTable[RC_ZD_CIRCLE_ROCK_5]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_5,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(462, -856),    "ZD Circle Rock 5",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_5));
    locationTable[RC_ZD_CIRCLE_ROCK_6]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_6,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(405, -832),    "ZD Circle Rock 6",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_6));
    locationTable[RC_ZD_CIRCLE_ROCK_7]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_7,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(382, -776),    "ZD Circle Rock 7",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_7));
    locationTable[RC_ZD_CIRCLE_ROCK_8]                 = Location::Rock(RC_ZD_CIRCLE_ROCK_8,             RCQUEST_BOTH, RCAREA_ZORAS_DOMAIN,          SCENE_ZORAS_DOMAIN,                TWO_ACTOR_PARAMS(405, -719),    "ZD Circle Rock 8",                         RHT_ZD_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_CIRCLE_ROCK_8));
    locationTable[RC_ZF_BOULDER]                       = Location::Boulder(RC_ZF_BOULDER,                RCQUEST_BOTH, RCAREA_ZORAS_FOUNTAIN,        SCENE_ZORAS_FOUNTAIN,              TWO_ACTOR_PARAMS(189, 2586),    "ZF Boulder",                               RHT_ZF_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_BOULDER));
    locationTable[RC_ZF_SILVER_BOULDER]                = Location::Boulder(RC_ZF_SILVER_BOULDER,         RCQUEST_BOTH, RCAREA_ZORAS_FOUNTAIN,        SCENE_ZORAS_FOUNTAIN,              TWO_ACTOR_PARAMS(316, 2634),    "ZF Silver Boulder",                        RHT_ZF_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_SILVER_BOULDER));
    locationTable[RC_ZF_UNDERGROUND_BOULDER]           = Location::Boulder(RC_ZF_UNDERGROUND_BOULDER,    RCQUEST_BOTH, RCAREA_ZORAS_FOUNTAIN,        SCENE_ZORAS_FOUNTAIN,              TWO_ACTOR_PARAMS(317, 2631),    "ZF Underground Boulder",                   RHT_ZF_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_UNDERGROUND_BOULDER));
    locationTable[RC_ZR_BOULDER_1]                     = Location::Boulder(RC_ZR_BOULDER_1,              RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1456, 434),   "ZR Boulder 1",                             RHT_ZR_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_BOULDER_1));
    locationTable[RC_ZR_BOULDER_2]                     = Location::Boulder(RC_ZR_BOULDER_2,              RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1518, 435),   "ZR Boulder 2",                             RHT_ZR_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_BOULDER_2));
    locationTable[RC_ZR_BOULDER_3]                     = Location::Boulder(RC_ZR_BOULDER_3,              RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1576, 430),   "ZR Boulder 3",                             RHT_ZR_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_BOULDER_3));
    locationTable[RC_ZR_BOULDER_4]                     = Location::Boulder(RC_ZR_BOULDER_4,              RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1400, 482),   "ZR Boulder 4",                             RHT_ZR_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_BOULDER_4));
    locationTable[RC_ZR_CIRCLE_ROCK_1]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_1,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1635, -53),   "ZR Circle Rock 1",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_1));
    locationTable[RC_ZR_CIRCLE_ROCK_2]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_2,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1578, -76),   "ZR Circle Rock 2",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_2));
    locationTable[RC_ZR_CIRCLE_ROCK_3]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_3,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1555, -133),  "ZR Circle Rock 3",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_3));
    locationTable[RC_ZR_CIRCLE_ROCK_4]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_4,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1578, -189),  "ZR Circle Rock 4",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_4));
    locationTable[RC_ZR_CIRCLE_ROCK_5]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_5,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1635, -213),  "ZR Circle Rock 5",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_5));
    locationTable[RC_ZR_CIRCLE_ROCK_6]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_6,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1691, -189),  "ZR Circle Rock 6",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_6));
    locationTable[RC_ZR_CIRCLE_ROCK_7]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_7,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1715, -133),  "ZR Circle Rock 7",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_7));
    locationTable[RC_ZR_CIRCLE_ROCK_8]                 = Location::Rock(RC_ZR_CIRCLE_ROCK_8,             RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(-1691, -76),   "ZR Circle Rock 8",                         RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_CIRCLE_ROCK_8));
    locationTable[RC_ZR_UPPER_CIRCLE_BOULDER]          = Location::Boulder(RC_ZR_UPPER_CIRCLE_BOULDER,   RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(672, -366),    "ZR Upper Circle Boulder",                  RHT_ZR_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_BOULDER));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_1]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_1,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(668, -290),    "ZR Upper Circle Rock 1",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_1));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_2]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_2,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(724, -313),    "ZR Upper Circle Rock 2",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_2));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_3]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_3,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(748, -370),    "ZR Upper Circle Rock 3",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_3));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_4]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_4,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(724, -426),    "ZR Upper Circle Rock 4",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_4));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_5]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_5,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(668, -450),    "ZR Upper Circle Rock 5",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_5));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_6]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_6,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(611, -426),    "ZR Upper Circle Rock 6",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_6));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_7]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_7,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(588, -370),    "ZR Upper Circle Rock 7",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_7));
    locationTable[RC_ZR_UPPER_CIRCLE_ROCK_8]           = Location::Rock(RC_ZR_UPPER_CIRCLE_ROCK_8,       RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(611, -313),    "ZR Upper Circle Rock 8",                   RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UPPER_CIRCLE_ROCK_8));
    locationTable[RC_ZR_ROCK]                          = Location::Rock(RC_ZR_ROCK,                      RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(2044, -786),   "ZR Rock",                                  RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_ROCK));
    locationTable[RC_ZR_UNDERWATER_ROCK_1]             = Location::Rock(RC_ZR_UNDERWATER_ROCK_1,         RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(2425, -446),   "ZR Underwater Rock 1",                     RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UNDERWATER_ROCK_1));
    locationTable[RC_ZR_UNDERWATER_ROCK_2]             = Location::Rock(RC_ZR_UNDERWATER_ROCK_2,         RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(2425, -524),   "ZR Underwater Rock 2",                     RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UNDERWATER_ROCK_2));
    locationTable[RC_ZR_UNDERWATER_ROCK_3]             = Location::Rock(RC_ZR_UNDERWATER_ROCK_3,         RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(2503, -571),   "ZR Underwater Rock 3",                     RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UNDERWATER_ROCK_3));
    locationTable[RC_ZR_UNDERWATER_ROCK_4]             = Location::Rock(RC_ZR_UNDERWATER_ROCK_4,         RCQUEST_BOTH, RCAREA_ZORAS_RIVER,           SCENE_ZORAS_RIVER,                 TWO_ACTOR_PARAMS(2550, -415),   "ZR Underwater Rock 4",                     RHT_ZR_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_UNDERWATER_ROCK_4));

    // 5 rocks by dc
    locationTable[RC_DMT_ROCK_1]                       = Location::Rock(RC_DMT_ROCK_1,                   RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1816, -513),  "DMT Rock 1",                         RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ROCK_1));
    locationTable[RC_DMT_ROCK_2]                       = Location::Rock(RC_DMT_ROCK_2,                   RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1831, -614),  "DMT Rock 2",                         RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ROCK_2));
    locationTable[RC_DMT_ROCK_3]                       = Location::Rock(RC_DMT_ROCK_3,                   RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1857, -536),  "DMT Rock 3",                         RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ROCK_3));
    locationTable[RC_DMT_ROCK_4]                       = Location::Rock(RC_DMT_ROCK_4,                   RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1878, -465),  "DMT Rock 4",                         RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ROCK_4));
    locationTable[RC_DMT_ROCK_5]                       = Location::Rock(RC_DMT_ROCK_5,                   RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1787, -550),  "DMT Rock 5",                         RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ROCK_5));
    locationTable[RC_DMT_SUMMIT_ROCK]                  = Location::Rock(RC_DMT_SUMMIT_ROCK,              RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-327, -4286),  "DMT Summit Rock",                    RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_SUMMIT_ROCK));
    locationTable[RC_DMT_CIRCLE_ROCK_1]                = Location::Rock(RC_DMT_CIRCLE_ROCK_1,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-383, -1126),  "DMT Circle Rock 1",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_1));
    locationTable[RC_DMT_CIRCLE_ROCK_2]                = Location::Rock(RC_DMT_CIRCLE_ROCK_2,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-326, -1149),  "DMT Circle Rock 2",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_2));
    locationTable[RC_DMT_CIRCLE_ROCK_3]                = Location::Rock(RC_DMT_CIRCLE_ROCK_3,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-303, -1206),  "DMT Circle Rock 3",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_3));
    locationTable[RC_DMT_CIRCLE_ROCK_4]                = Location::Rock(RC_DMT_CIRCLE_ROCK_4,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-326, -1262),  "DMT Circle Rock 4",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_4));
    locationTable[RC_DMT_CIRCLE_ROCK_5]                = Location::Rock(RC_DMT_CIRCLE_ROCK_5,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-383, -1286),  "DMT Circle Rock 5",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_5));
    locationTable[RC_DMT_CIRCLE_ROCK_6]                = Location::Rock(RC_DMT_CIRCLE_ROCK_6,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-439, -1262),  "DMT Circle Rock 6",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_6));
    locationTable[RC_DMT_CIRCLE_ROCK_7]                = Location::Rock(RC_DMT_CIRCLE_ROCK_7,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-463, -1206),  "DMT Circle Rock 7",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_7));
    locationTable[RC_DMT_CIRCLE_ROCK_8]                = Location::Rock(RC_DMT_CIRCLE_ROCK_8,            RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-439, -1149),  "DMT Circle Rock 8",                  RHT_DMT_ROCK,                 RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CIRCLE_ROCK_8));
    locationTable[RC_DMT_CHILD_BOULDER]                = Location::Boulder(RC_DMT_CHILD_BOULDER,         RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1060, -51),   "DMT Child Boulder",                  RHT_DMT_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CHILD_BOULDER));
    locationTable[RC_DMT_BOULDER_1]                    = Location::Boulder(RC_DMT_BOULDER_1,             RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-625, -55),    "DMT Boulder 1",                      RHT_DMT_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BOULDER_1));
    locationTable[RC_DMT_BOULDER_2]                    = Location::Boulder(RC_DMT_BOULDER_2,             RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-808, -59),    "DMT Boulder 2",                      RHT_DMT_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BOULDER_2));
    locationTable[RC_DMT_COW_BOULDER]                  = Location::Boulder(RC_DMT_COW_BOULDER,           RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-688, -285),   "Cow Boulder",                        RHT_DMT_BOULDER,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_COW_BOULDER));
    locationTable[RC_DMT_BRONZE_BOULDER_1]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_1,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1175, -803),  "DMT Bronze Boulder 1",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_1));
    locationTable[RC_DMT_BRONZE_BOULDER_2]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_2,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1948, 1706),  "DMT Bronze Boulder 2",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_2));
    locationTable[RC_DMT_BRONZE_BOULDER_3]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_3,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-2019, 1101),  "DMT Bronze Boulder 3",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_3));
    locationTable[RC_DMT_BRONZE_BOULDER_4]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_4,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1658, -88),   "DMT Bronze Boulder 4",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_4));
    locationTable[RC_DMT_BRONZE_BOULDER_5]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_5,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1753, 445),   "DMT Bronze Boulder 5",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_5));
    locationTable[RC_DMT_BRONZE_BOULDER_6]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_6,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1018, 1283),  "DMT Bronze Boulder 6",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_6));
    locationTable[RC_DMT_BRONZE_BOULDER_7]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_7,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1986, 727),   "DMT Bronze Boulder 7",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_7));
    locationTable[RC_DMT_BRONZE_BOULDER_8]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_8,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-23, -3196),   "DMT Bronze Boulder 8",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_8));
    locationTable[RC_DMT_BRONZE_BOULDER_9]             = Location::Boulder(RC_DMT_BRONZE_BOULDER_9,      RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-343, -2794),  "DMT Bronze Boulder 9",               RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_9));
    locationTable[RC_DMT_BRONZE_BOULDER_10]            = Location::Boulder(RC_DMT_BRONZE_BOULDER_10,     RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-154, -2484),  "DMT Bronze Boulder 10",              RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_10));
    locationTable[RC_DMT_BRONZE_BOULDER_11]            = Location::Boulder(RC_DMT_BRONZE_BOULDER_11,     RCQUEST_BOTH, RCAREA_DEATH_MOUNTAIN_TRAIL,  SCENE_DEATH_MOUNTAIN_TRAIL,        TWO_ACTOR_PARAMS(-1590, -402),  "DMT Bronze Boulder 11",              RHT_DMT_BRONZE_BOULDER,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_BRONZE_BOULDER_11));

    locationTable[RC_GC_LW_BOULDER_1]                  = Location::Boulder(RC_GC_LW_BOULDER_1,           RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(416, 1049),    "GC Goron City Boulder 1",                  RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_LW_BOULDER_1));
    locationTable[RC_GC_LW_BOULDER_2]                  = Location::Boulder(RC_GC_LW_BOULDER_2,           RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(470, 1031),    "GC Goron City Boulder 2",                  RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_LW_BOULDER_2));
    locationTable[RC_GC_LW_BOULDER_3]                  = Location::Boulder(RC_GC_LW_BOULDER_3,           RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(367, 1078),    "GC Goron City Boulder 3",                  RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_LW_BOULDER_3));
    locationTable[RC_GC_ENTRANCE_BOULDER_1]            = Location::Boulder(RC_GC_ENTRANCE_BOULDER_1,     RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-670, 470),    "GC Entrance Boulder 1",                    RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_ENTRANCE_BOULDER_1));
    locationTable[RC_GC_ENTRANCE_BOULDER_2]            = Location::Boulder(RC_GC_ENTRANCE_BOULDER_2,     RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-607, 419),    "GC Entrance Boulder 2",                    RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_ENTRANCE_BOULDER_2));
    locationTable[RC_GC_ENTRANCE_BOULDER_3]            = Location::Boulder(RC_GC_ENTRANCE_BOULDER_3,     RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-756, 474),    "GC Entrance Boulder 3",                    RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_ENTRANCE_BOULDER_3));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_1]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_1,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1479, -794),  "GC Maze Silver Boulder 1",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_1));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_2]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_2,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1478, -855),  "GC Maze Silver Boulder 2",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_2));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_3]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_3,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1474, -624),  "GC Maze Silver Boulder 3",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_3));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_4]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_4,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1471, -993),  "GC Maze Silver Boulder 4",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_4));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_5]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_5,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1467, -1064), "GC Maze Silver Boulder 5",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_5));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_6]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_6,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1460, -1121), "GC Maze Silver Boulder 6",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_6));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_7]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_7,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1451, -567),  "GC Maze Silver Boulder 7",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_7));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_8]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_8,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1448, -672),  "GC Maze Silver Boulder 8",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_8));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_9]         = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_9,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1440, -1174), "GC Maze Silver Boulder 9",                 RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_9));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_10]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_10, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1437, -1342), "GC Maze Silver Boulder 10",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_10));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_11]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_11, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1424, -1245), "GC Maze Silver Boulder 11",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_11));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_12]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_12, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1424, -609),  "GC Maze Silver Boulder 12",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_12));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_13]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_13, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1399, -1300), "GC Maze Silver Boulder 13",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_13));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_14]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_14, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1394, -654),  "GC Maze Silver Boulder 14",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_14));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_15]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_15, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1343, -698),  "GC Maze Silver Boulder 15",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_15));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_16]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_16, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1319, -1086), "GC Maze Silver Boulder 16",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_16));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_17]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_17, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1312, -1039), "GC Maze Silver Boulder 17",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_17));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_18]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_18, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1306, -837),  "GC Maze Silver Boulder 18",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_18));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_19]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_19, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1279, -656),  "GC Maze Silver Boulder 19",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_19));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_20]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_20, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1255, -840),  "GC Maze Silver Boulder 20",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_20));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_21]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_21, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1246, -1075), "GC Maze Silver Boulder 21",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_21));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_22]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_22, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1244, -589),  "GC Maze Silver Boulder 22",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_22));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_23]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_23, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1210, -852),  "GC Maze Silver Boulder 23",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_23));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_24]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_24, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1206, -627),  "GC Maze Silver Boulder 24",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_24));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_25]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_25, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1187, -896),  "GC Maze Silver Boulder 25",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_25));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_26]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_26, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1157, -954),  "GC Maze Silver Boulder 26",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_26));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_27]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_27, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1130, -1137), "GC Maze Silver Boulder 27",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_27));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_28]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_28, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1120, -1001), "GC Maze Silver Boulder 28",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_28));
    locationTable[RC_GC_MAZE_SILVER_BOULDER_29]        = Location::Boulder(RC_GC_MAZE_SILVER_BOULDER_29, RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1179, -1098), "GC Maze Silver Boulder 29",                RHT_GC_SILVER_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_SILVER_BOULDER_29));
    locationTable[RC_GC_MAZE_BOULDER_1]                = Location::Boulder(RC_GC_MAZE_BOULDER_1,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1342, -628),  "GC Maze Boulder 1",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_1));
    locationTable[RC_GC_MAZE_BOULDER_2]                = Location::Boulder(RC_GC_MAZE_BOULDER_2,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1456, -501),  "GC Maze Boulder 2",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_2));
    locationTable[RC_GC_MAZE_BOULDER_3]                = Location::Boulder(RC_GC_MAZE_BOULDER_3,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1233, -511),  "GC Maze Boulder 3",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_3));
    locationTable[RC_GC_MAZE_BOULDER_4]                = Location::Boulder(RC_GC_MAZE_BOULDER_4,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1137, -657),  "GC Maze Boulder 4",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_4));
    locationTable[RC_GC_MAZE_BOULDER_5]                = Location::Boulder(RC_GC_MAZE_BOULDER_5,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1124, -913),  "GC Maze Boulder 5",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_5));
    locationTable[RC_GC_MAZE_BOULDER_6]                = Location::Boulder(RC_GC_MAZE_BOULDER_6,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1326, -771),  "GC Maze Boulder 6",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_6));
    locationTable[RC_GC_MAZE_BOULDER_7]                = Location::Boulder(RC_GC_MAZE_BOULDER_7,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1469, -737),  "GC Maze Boulder 7",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_7));
    locationTable[RC_GC_MAZE_BOULDER_8]                = Location::Boulder(RC_GC_MAZE_BOULDER_8,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1476, -921),  "GC Maze Boulder 8",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_8));
    locationTable[RC_GC_MAZE_BOULDER_9]                = Location::Boulder(RC_GC_MAZE_BOULDER_9,         RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1391, -1087), "GC Maze Boulder 9",                        RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_9));
    locationTable[RC_GC_MAZE_BOULDER_10]               = Location::Boulder(RC_GC_MAZE_BOULDER_10,        RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1222, -997),  "GC Maze Boulder 10",                       RHT_GC_BOULDER,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BOULDER_10));
    locationTable[RC_GC_MAZE_BRONZE_BOULDER_1]         = Location::Boulder(RC_GC_MAZE_BRONZE_BOULDER_1,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1541, -631),  "GC Maze Bronze Boulder 1",                 RHT_GC_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BRONZE_BOULDER_1));
    locationTable[RC_GC_MAZE_BRONZE_BOULDER_2]         = Location::Boulder(RC_GC_MAZE_BRONZE_BOULDER_2,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1536, -861),  "GC Maze Bronze Boulder 2",                 RHT_GC_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BRONZE_BOULDER_2));
    locationTable[RC_GC_MAZE_BRONZE_BOULDER_3]         = Location::Boulder(RC_GC_MAZE_BRONZE_BOULDER_3,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1536, -1102), "GC Maze Bronze Boulder 3",                 RHT_GC_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BRONZE_BOULDER_3));
    locationTable[RC_GC_MAZE_BRONZE_BOULDER_4]         = Location::Boulder(RC_GC_MAZE_BRONZE_BOULDER_4,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1534, -752),  "GC Maze Bronze Boulder 4",                 RHT_GC_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BRONZE_BOULDER_4));
    locationTable[RC_GC_MAZE_BRONZE_BOULDER_5]         = Location::Boulder(RC_GC_MAZE_BRONZE_BOULDER_5,  RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1536, -991),  "GC Maze Bronze Boulder 5",                 RHT_GC_BRONZE_BOULDER,        SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_BRONZE_BOULDER_5));
    locationTable[RC_GC_MAZE_ROCK]                     = Location::Rock(RC_GC_MAZE_ROCK,                 RCQUEST_BOTH, RCAREA_GORON_CITY,            SCENE_GORON_CITY,                  TWO_ACTOR_PARAMS(-1197, -1329), "GC Maze Rock",                             RHT_GC_ROCK,                  RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_MAZE_ROCK));

    locationTable[RC_COLOSSUS_SILVER_BOULDER]          = Location::Boulder(RC_COLOSSUS_SILVER_BOULDER,   RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(61, -1301),    "Colossus Silver Boulder",                  RHT_COLOSSUS_SILVER_BOULDER,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_SILVER_BOULDER));
    locationTable[RC_COLOSSUS_ROCK]                    = Location::Rock(RC_COLOSSUS_ROCK,                RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(1537, 667),    "Colossus Rock",                            RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_ROCK));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_1]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_1,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-250, -1272),  "Colossus Circle 1 Rock 1",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_1));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_2]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_2,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-193, -1295),  "Colossus Circle 1 Rock 2",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_2));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_3]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_3,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-170, -1352),  "Colossus Circle 1 Rock 3",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_3));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_4]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_4,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-193, -1408),  "Colossus Circle 1 Rock 4",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_4));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_5]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_5,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-250, -1432),  "Colossus Circle 1 Rock 5",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_5));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_6]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_6,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-306, -1408),  "Colossus Circle 1 Rock 6",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_6));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_7]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_7,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-330, -1352),  "Colossus Circle 1 Rock 7",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_7));
    locationTable[RC_COLOSSUS_CIRCLE_1_ROCK_8]         = Location::Rock(RC_COLOSSUS_CIRCLE_1_ROCK_8,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-306, -1295),  "Colossus Circle 1 Rock 8",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_1_ROCK_8));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_1]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_1,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-834, -766),   "Colossus Circle 2 Rock 1",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_1));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_2]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_2,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-777, -789),   "Colossus Circle 2 Rock 2",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_2));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_3]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_3,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-754, -846),   "Colossus Circle 2 Rock 3",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_3));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_4]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_4,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-777, -902),   "Colossus Circle 2 Rock 4",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_4));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_5]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_5,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-834, -926),   "Colossus Circle 2 Rock 5",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_5));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_6]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_6,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-890, -902),   "Colossus Circle 2 Rock 6",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_6));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_7]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_7,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-914, -846),   "Colossus Circle 2 Rock 7",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_7));
    locationTable[RC_COLOSSUS_CIRCLE_2_ROCK_8]         = Location::Rock(RC_COLOSSUS_CIRCLE_2_ROCK_8,     RCQUEST_BOTH, RCAREA_DESERT_COLOSSUS,       SCENE_DESERT_COLOSSUS,             TWO_ACTOR_PARAMS(-890, -789),   "Colossus Circle 2 Rock 8",                 RHT_COLOSSUS_ROCK,            RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_COLOSSUS_CIRCLE_2_ROCK_8));

    locationTable[RC_HC_STORMS_GROTTO_ROCK_1]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_1,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1811, 813),    "HC Storms Grotto Rock 1",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_1));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_2]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_2,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1867, 789),    "HC Storms Grotto Rock 2",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_2));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_3]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_3,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1891, 733),    "HC Storms Grotto Rock 3",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_3));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_4]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_4,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1867, 676),    "HC Storms Grotto Rock 4",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_4));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_5]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_5,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1811, 653),    "HC Storms Grotto Rock 5",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_5));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_6]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_6,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1754, 676),    "HC Storms Grotto Rock 6",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_6));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_7]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_7,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1731, 733),    "HC Storms Grotto Rock 7",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_7));
    locationTable[RC_HC_STORMS_GROTTO_ROCK_8]          = Location::Rock(RC_HC_STORMS_GROTTO_ROCK_8,      RCQUEST_BOTH, RCAREA_HYRULE_CASTLE,         SCENE_GROTTOS,                     TWO_ACTOR_PARAMS(1754, 789),    "HC Storms Grotto Rock 8",                  RHT_HC_STORMS_GROTTO_ROCK,    RG_RECOVERY_HEART,  SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_STORMS_GROTTO_ROCK_8));

    locationTable[RC_BOTW_BOULDER_1]                   = Location::Boulder(RC_BOTW_BOULDER_1,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(-684, -734),   "BOTW Boulder 1",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_1));
    locationTable[RC_BOTW_BOULDER_2]                   = Location::Boulder(RC_BOTW_BOULDER_2,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(-632, -805),   "BOTW Boulder 2",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_2));
    locationTable[RC_BOTW_BOULDER_3]                   = Location::Boulder(RC_BOTW_BOULDER_3,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(333, -681),    "BOTW Boulder 3",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_3));
    locationTable[RC_BOTW_BOULDER_4]                   = Location::Boulder(RC_BOTW_BOULDER_4,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(409, -637),    "BOTW Boulder 4",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_4));
    locationTable[RC_BOTW_BOULDER_5]                   = Location::Boulder(RC_BOTW_BOULDER_5,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(334, -8),      "BOTW Boulder 5",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_5));
    locationTable[RC_BOTW_BOULDER_6]                   = Location::Boulder(RC_BOTW_BOULDER_6,            RCQUEST_VANILLA, RCAREA_BOTTOM_OF_THE_WELL, SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(312, 64),      "BOTW Boulder 6",                           RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_BOULDER_6));

    locationTable[RC_DEKU_TREE_MQ_BOULDER_1]           = Location::Boulder(RC_DEKU_TREE_MQ_BOULDER_1,    RCQUEST_MQ,   RCAREA_DEKU_TREE,             SCENE_DEKU_TREE,                   TWO_ACTOR_PARAMS(-1237, 1558),  "Deku Tree MQ Boulder 1",                   RHT_DEKU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DEKU_TREE_MQ_BOULDER_1));
    locationTable[RC_DEKU_TREE_MQ_BOULDER_2]           = Location::Boulder(RC_DEKU_TREE_MQ_BOULDER_2,    RCQUEST_MQ,   RCAREA_DEKU_TREE,             SCENE_DEKU_TREE,                   TWO_ACTOR_PARAMS(-1183, 1522),  "Deku Tree MQ Boulder 2",                   RHT_DEKU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DEKU_TREE_MQ_BOULDER_2));
    locationTable[RC_DEKU_TREE_MQ_BOULDER_3]           = Location::Boulder(RC_DEKU_TREE_MQ_BOULDER_3,    RCQUEST_MQ,   RCAREA_DEKU_TREE,             SCENE_DEKU_TREE,                   TWO_ACTOR_PARAMS(-1129, 1469),  "Deku Tree MQ Boulder 3",                   RHT_DEKU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DEKU_TREE_MQ_BOULDER_3));

    locationTable[RC_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_1]             = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_1,             RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(-435, -1720),  "Dodongo's MQ Lobby Boulder 1",             RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_1));
    locationTable[RC_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_2]             = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_2,             RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(432, -1719),   "Dodongo's MQ Lobby Boulder 2",             RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LOBBY_BOULDER_2));
    locationTable[RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_1] = Location::Boulder(RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_1, RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(807, -874),    "Dodongo's MQ Mouth Bridge Boulder 1",      RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_1));
    locationTable[RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_2] = Location::Boulder(RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_2, RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(802, -972),    "Dodongo's MQ Mouth Bridge Boulder 2",      RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_2));
    locationTable[RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_3] = Location::Boulder(RC_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_3, RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(783, -923),    "Dodongo's MQ Mouth Bridge Boulder 3",      RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE_BOULDER_3));
    locationTable[RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_1]        = Location::Boulder(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_1,        RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(2464, -402),   "Dodongo's MQ Right Side Boulder 1",        RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_1));
    locationTable[RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_2]        = Location::Boulder(RC_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_2,        RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(2942, -495),   "Dodongo's MQ Right Side Boulder 2",        RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_RIGHT_SIDE_BOULDER_2));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_1]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_1,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4219, -1651),  "Dodongo's MQ Lizalfos Room Boulder 1",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_1));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_2]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_2,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4178, -1602),  "Dodongo's MQ Lizalfos Room Boulder 2",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_2));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_3]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_3,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4162, -1581),  "Dodongo's MQ Lizalfos Room Boulder 3",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_3));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_4]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_4,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4133, -1561),  "Dodongo's MQ Lizalfos Room Boulder 4",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_4));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_5]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_5,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4091, -1510),  "Dodongo's MQ Lizalfos Room Boulder 5",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_5));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_6]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_6,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4067, -1487),  "Dodongo's MQ Lizalfos Room Boulder 6",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_6));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_7]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_7,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(4028, -1472),  "Dodongo's MQ Lizalfos Room Boulder 7",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_7));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_8]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_8,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(3965, -1473),  "Dodongo's MQ Lizalfos Room Boulder 8",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_8));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_9]     = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_9,     RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(3898, -1467),  "Dodongo's MQ Lizalfos Room Boulder 9",     RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_9));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_10]    = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_10,    RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(3832, -1437),  "Dodongo's MQ Lizalfos Room Boulder 10",    RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_10));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_11]    = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_11,    RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(3799, -1383),  "Dodongo's MQ Lizalfos Room Boulder 11",    RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_11));
    locationTable[RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_12]    = Location::Boulder(RC_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_12,    RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(3760, -1318),  "Dodongo's MQ Lizalfos Room Boulder 12",    RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_LIZALFOS_ROOM_BOULDER_12));
    locationTable[RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_BOULDER]          = Location::Boulder(RC_DODONGOS_CAVERN_MQ_TWO_FLAMES_BOULDER,          RCQUEST_MQ,   RCAREA_DODONGOS_CAVERN,       SCENE_DODONGOS_CAVERN,             TWO_ACTOR_PARAMS(2737, -1058),  "Dodongo's MQ Two Flames Boulder",          RHT_DODONGOS_BOULDER,         SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_MQ_TWO_FLAMES_BOULDER));

    locationTable[RC_JABU_JABUS_BELLY_MQ_ENTRANCE_BOULDER] =          Location::Boulder(RC_JABU_JABUS_BELLY_MQ_ENTRANCE_BOULDER,          RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(-1, -296),     "Jabu MQ Entrance Boulder",                 RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_ENTRANCE_BOULDER));
    locationTable[RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_1] =      Location::Boulder(RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_1,      RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(350, -3533),   "Jabu MQ Holes Room Boulder 1",             RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_1));
    locationTable[RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_2] =      Location::Boulder(RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_2,      RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(-192, -3211),  "Jabu MQ Holes Room Boulder 2",             RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_HOLES_ROOM_BOULDER_2));
    locationTable[RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_1] = Location::Boulder(RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_1, RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(245, -2792),   "Jabu MQ Holes Wall Boulder 1",             RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_1));
    locationTable[RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_2] = Location::Boulder(RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_2, RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(220, -2790),   "Jabu MQ Holes Wall Boulder 2",             RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_2));
    locationTable[RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_3] = Location::Boulder(RC_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_3, RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(274, -2790),   "Jabu MQ Holes Wall Boulder 3",             RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_HOLES_ROOM_WALL_BOULDER_3));
    locationTable[RC_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_1] = Location::Boulder(RC_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_1, RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(31, -5177),    "Jabu MQ Forked Corridor Boulder 1",        RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_1));
    locationTable[RC_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_2] = Location::Boulder(RC_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_2, RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(-37, -5173),   "Jabu MQ Forked Corridor Boulder 2",        RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR_BOULDER_2));
    locationTable[RC_JABU_JABUS_BELLY_MQ_TAILPASARAN_BOULDER] =       Location::Boulder(RC_JABU_JABUS_BELLY_MQ_TAILPASARAN_BOULDER,       RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(-885, -5907),  "Jabu MQ Tailpasaran Boulder",              RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_TAILPASARAN_BOULDER));
    locationTable[RC_JABU_JABUS_BELLY_MQ_TAILPASARAN_WALL_BOULDER] =  Location::Boulder(RC_JABU_JABUS_BELLY_MQ_TAILPASARAN_WALL_BOULDER,  RCQUEST_MQ,   RCAREA_JABU_JABUS_BELLY,      SCENE_JABU_JABU,                   TWO_ACTOR_PARAMS(-411, -5682),  "Jabu MQ Tailpasaran Wall Boulder",         RHT_JABU_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_JABU_JABUS_BELLY_MQ_TAILPASARAN_WALL_BOULDER));

    // skip spirit temple boulder, so adult can clear without collecting check for child to pass
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_1]       = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_1,       RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(-160, 270),    "Spirit MQ Entrance Boulder 1",             RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_1));
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_2]       = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_2,       RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(160, 270),     "Spirit MQ Entrance Boulder 2",             RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_ENTRANCE_BOULDER_2));
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_EYE_BOULDER]     = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_EYE_BOULDER,     RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(350, 220),     "Spirit MQ Entrance Eye Boulder",           RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_ENTRANCE_EYE_BOULDER));
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_CEILING_BOULDER] = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_CEILING_BOULDER, RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(0, -60),       "Spirit MQ Entrance Ceiling Boulder",       RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_ENTRANCE_CEILING_BOULDER));
    locationTable[RC_SPIRIT_TEMPLE_MQ_CRAWLSPACE_BOULDER]       = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_CRAWLSPACE_BOULDER,       RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(-1060, -680),  "Spirit MQ Crawlspace Boulder",             RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_CRAWLSPACE_BOULDER));
    locationTable[RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER]            = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER,            RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(-593, -1340),  "Spirit MQ Gibdo Boulder",                  RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER));
    locationTable[RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_LOW]        = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_LOW,        RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(-421, -1036),  "Spirit MQ Gibdo Low Boulder",              RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_LOW));
    locationTable[RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_HIGH]       = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_HIGH,       RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(-786, -930),   "Spirit MQ Gibdo High Boulder",             RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_GIBDO_BOULDER_HIGH));
    locationTable[RC_SPIRIT_TEMPLE_MQ_EARLY_ADULT_BOULDER]      = Location::Boulder(RC_SPIRIT_TEMPLE_MQ_EARLY_ADULT_BOULDER,      RCQUEST_MQ,   RCAREA_SPIRIT_TEMPLE,         SCENE_SPIRIT_TEMPLE,               TWO_ACTOR_PARAMS(1070, -290),   "Spirit MQ Early Adult Boulder",            RHT_SPIRIT_TEMPLE_BOULDER,    SpoilerCollectionCheck::RandomizerInf(RAND_INF_SPIRIT_TEMPLE_MQ_EARLY_ADULT_BOULDER));

    locationTable[RC_BOTW_MQ_BOULDER_1]                = Location::Boulder(RC_BOTW_MQ_BOULDER_1,         RCQUEST_MQ,   RCAREA_BOTTOM_OF_THE_WELL,    SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(-370, -160),   "BotW MQ Boulder 1",                        RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_MQ_BOULDER_1));
    locationTable[RC_BOTW_MQ_BOULDER_2]                = Location::Boulder(RC_BOTW_MQ_BOULDER_2,         RCQUEST_MQ,   RCAREA_BOTTOM_OF_THE_WELL,    SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(-521, -353),   "BotW MQ Boulder 2",                        RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_MQ_BOULDER_2));
    locationTable[RC_BOTW_MQ_BOULDER_3]                = Location::Boulder(RC_BOTW_MQ_BOULDER_3,         RCQUEST_MQ,   RCAREA_BOTTOM_OF_THE_WELL,    SCENE_BOTTOM_OF_THE_WELL,          TWO_ACTOR_PARAMS(-541, -404),   "BotW MQ Boulder 3",                        RHT_BOTW_BOULDER,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_BOTW_MQ_BOULDER_3));
    // clang-format-on
}

static RegisterShipInitFunc initFunc(RegisterShuffleRock, { "IS_RANDO" });
static RegisterShipInitFunc initFunc2(Rando::StaticData::RegisterRockLocations);