#include <soh/OTRGlobals.h>
#include "soh/ObjectExtension/ObjectExtension.h"
#include "item_category_adj.h"
#include "particle_cmc.h"
extern "C" {
extern PlayState* gPlayState;
#include "overlays/actors/ovl_En_Kanban/z_en_kanban.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Wonder_Talk/z_en_wonder_talk.h"
#include "overlays/actors/ovl_En_Wonder_Talk2/z_en_wonder_talk2.h"
}

uint8_t Sign_RandomizerHoldsItem(Actor* actor, PlayState* play) {
    const auto signIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(actor);
    if (signIdentity == nullptr) {
        return false;
    }

    RandomizerCheck rc = signIdentity->randomizerCheck;
    uint8_t isDungeon = Rando::StaticData::GetLocation(rc)->IsDungeon();
    auto signSetting = RAND_GET_OPTION(RSK_SHUFFLE_SIGNS);

    // Don't pull randomized item if sign isn't randomized or is already checked
    if (!IS_RANDO || (signSetting.Is(RO_SHUFFLE_SIGNS_OVERWORLD) && isDungeon) ||
        (signSetting.Is(RO_SHUFFLE_SIGNS_DUNGEONS) && !isDungeon) ||
        Flags_GetRandomizerInf(signIdentity->randomizerInf) || signIdentity->randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

static void Sign_RandomizerDraw(Actor* actor, Color_RGBA8* primColor, Color_RGBA8* secColor, Color_RGBA8* envColor) {
    Vec3f pos;
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    float yKanbanOffset = LINK_IS_CHILD && actor->id == ACTOR_EN_KANBAN ? 15.0f : 0.0f;

    velocity.y = -0.05f;
    accel.y = -0.025f;

    pos.x = Rand_CenteredFloat(10.0f) + actor->world.pos.x;
    pos.y = (Rand_ZeroOne() * 10.0f) + actor->world.pos.y + yKanbanOffset;
    pos.z = Rand_CenteredFloat(10.0f) + actor->world.pos.z;
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, secColor, envColor, 2000, 100);
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, primColor, envColor, 2000, 100);
}

void Sign_RandomizerDrawSetup(void* actor) {
    GetItemCategory getItemCategory;
    Actor* signActor = (Actor*)actor;

    // If not a randomized item or too far, don't draw
    if (!Sign_RandomizerHoldsItem(signActor, gPlayState) || signActor->xzDistToPlayer > 1000.0f) {
        return;
    }

    bool cmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), 0);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    int isNotCMC = !cmc || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    Color_RGBA8 primColor;
    Color_RGBA8 secColor;
    Color_RGBA8 envColor;

    const auto signIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(signActor);
    if (signIdentity == nullptr) {
        return;
    }

    GetItemEntry signItem =
        Rando::Context::GetInstance()->GetFinalGIEntry(signIdentity->randomizerCheck, true, GI_NONE);
    getItemCategory = Randomizer_AdjustItemCategory(signItem);

    if (isNotCMC) {
        getItemCategory = ITEM_CATEGORY_MAJOR;
    }
    primColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_PRIMARY);
    secColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_SECONDARY);
    envColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_FLARE);
    Sign_RandomizerDraw(signActor, &primColor, &secColor, &envColor);
}

void Sign_RandomizerSpawnCollectible(Actor* actor) {
    const auto signIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(actor);

    if (signIdentity == nullptr) {
        return;
    }
    Flags_SetRandomizerInf(signIdentity->randomizerInf);
}

void Sign_RoyalTombSpawnCollectible(int16_t flagType, int16_t flag) {
    if (!Flags_GetRandomizerInf(RAND_INF_GY_ROYAL_TOMB_GRAVE) &&
        Flags_GetEventChkInf(EVENTCHKINF_DESTROYED_ROYAL_FAMILY_TOMB)) {
        Flags_SetRandomizerInf(RAND_INF_GY_ROYAL_TOMB_GRAVE);
    }
}

void RegisterShuffleSigns() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_SIGNS).Get();

    COND_ID_HOOK(OnActorInit, ACTOR_EN_KANBAN, shouldRegister, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        EnKanban* signActor = static_cast<EnKanban*>(actorRef);

        auto signIdentity = OTRGlobals::Instance->gRandomizer->IdentifySign(
            gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z, actor->id);
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(signIdentity));
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_A_OBJ, shouldRegister, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        EnAObj* signActor = static_cast<EnAObj*>(actorRef);

        auto signIdentity = OTRGlobals::Instance->gRandomizer->IdentifySign(
            gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z, actor->id);
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(signIdentity));
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_WONDER_TALK, shouldRegister, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        EnWonderTalk* signActor = static_cast<EnWonderTalk*>(actorRef);

        auto signIdentity = OTRGlobals::Instance->gRandomizer->IdentifySign(
            gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z, actor->id);
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(signIdentity));
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_WONDER_TALK2, shouldRegister, [](void* actorRef) {
        Actor* actor = static_cast<Actor*>(actorRef);
        EnWonderTalk2* signActor = static_cast<EnWonderTalk2*>(actorRef);

        auto signIdentity = OTRGlobals::Instance->gRandomizer->IdentifySign(
            gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z, actor->id);
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(signIdentity));
    });

    // Draw particle effect to indicate a randomized item
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KANBAN, shouldRegister, Sign_RandomizerDrawSetup);

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_A_OBJ, shouldRegister, Sign_RandomizerDrawSetup);

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_WONDER_TALK, shouldRegister, Sign_RandomizerDrawSetup);

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_WONDER_TALK2, shouldRegister, Sign_RandomizerDrawSetup);

    COND_VB_SHOULD(VB_SKIP_TALKING, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor != NULL) {
            switch (talkActor->id) {
                case ACTOR_EN_KANBAN:
                case ACTOR_EN_A_OBJ:
                case ACTOR_EN_WONDER_TALK:
                case ACTOR_EN_WONDER_TALK2:
                    if (Sign_RandomizerHoldsItem(talkActor, gPlayState)) {
                        Sign_RandomizerSpawnCollectible(talkActor);
                        *should = false;
                    }
                    break;
                default:
                    break;
            }
        }
    });

    // Give Royal Tomb item if destroyed
    COND_HOOK(OnFlagSet, shouldRegister, Sign_RoyalTombSpawnCollectible);
}

void Rando::StaticData::RegisterSignLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    // Overworld Signs
    //            Randomizer Check                                      Randomizer Check                                                            Quest               Area                            Scene ID                        Params                              Short Name                                          Hint Text Key                   Actor Id                    Spoiler Collection Check
locationTable[RC_KF_DEKU_TREE_RECTANGLE_SIGN]                           = Location::Sign(RC_KF_DEKU_TREE_RECTANGLE_SIGN,                            RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(871, 311),         "Deku Tree Rectangle Sign",                         RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_DEKU_TREE_RECTANGLE_SIGN));
locationTable[RC_KF_STEPPING_STONES_RECTANGLE_SIGN]                     = Location::Sign(RC_KF_STEPPING_STONES_RECTANGLE_SIGN,                      RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(607, -80),         "Stepping Stones Rectangle Sign",                   RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_STEPPING_STONES_RECTANGLE_SIGN));
locationTable[RC_KF_LINKS_HOUSE_RECTANGLE_SIGN]                         = Location::Sign(RC_KF_LINKS_HOUSE_RECTANGLE_SIGN,                          RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(49, 967),          "Link's House Rectangle Sign",                      RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_LINKS_HOUSE_RECTANGLE_SIGN));
locationTable[RC_KF_FIRST_TRAINING_CENTER_RECTANGLE_SIGN]               = Location::Sign(RC_KF_FIRST_TRAINING_CENTER_RECTANGLE_SIGN,                RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-494, 598),        "First Training Center Rectangle Sign",             RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_FIRST_TRAINING_CENTER_RECTANGLE_SIGN));
locationTable[RC_KF_SECOND_TRAINING_CENTER_RECTANGLE_SIGN]              = Location::Sign(RC_KF_SECOND_TRAINING_CENTER_RECTANGLE_SIGN,               RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-538, 718),        "Second Training Center Rectangle Sign",            RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_SECOND_TRAINING_CENTER_RECTANGLE_SIGN));
locationTable[RC_KF_AFTER_CRAWLSPACE_RECTANGLE_SIGN]                    = Location::Sign(RC_KF_AFTER_CRAWLSPACE_RECTANGLE_SIGN,                     RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-784, 1675),       "After Crawlspace Rectangle Sign",                  RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_AFTER_CRAWLSPACE_RECTANGLE_SIGN));
locationTable[RC_KF_CRAWL_RECTANGLE_RECTANGLE_SIGN]                     = Location::Sign(RC_KF_CRAWL_RECTANGLE_RECTANGLE_SIGN,                      RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-845, 1018),       "Crawl Rectangle Rectangle Sign",                   RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_CRAWL_RECTANGLE_RECTANGLE_SIGN));
locationTable[RC_KF_LOST_WOODS_RECTANGLE_SIGN]                          = Location::Sign(RC_KF_LOST_WOODS_RECTANGLE_SIGN,                           RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-1432, -426),      "Lost Woods Rectangle Sign",                        RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_LOST_WOODS_RECTANGLE_SIGN));
locationTable[RC_KF_HOUSE_OF_TWINS_ARROW_SIGN]                          = Location::Sign(RC_KF_HOUSE_OF_TWINS_ARROW_SIGN,                           RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(1089, 473),        "House of Twins Arrow Sign",                        RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_HOUSE_OF_TWINS_ARROW_SIGN));
locationTable[RC_KF_SHOP_ARROW_SIGN]                                    = Location::Sign(RC_KF_SHOP_ARROW_SIGN,                                     RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(728, -195),        "Shop Arrow Sign",                                  RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_SHOP_ARROW_SIGN));
locationTable[RC_KF_SARIAS_HOUSE_ARROW_SIGN]                            = Location::Sign(RC_KF_SARIAS_HOUSE_ARROW_SIGN,                             RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(436, 601),         "Saria's House Arrow Sign",                         RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_SARIAS_HOUSE_ARROW_SIGN));
locationTable[RC_KF_LOST_WOODS_ARROW_SIGN]                              = Location::Sign(RC_KF_LOST_WOODS_ARROW_SIGN,                               RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-170, -1335),      "Lost Woods Arrow Sign",                            RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_LOST_WOODS_ARROW_SIGN));
locationTable[RC_KF_MIDOS_HOUSE_ARROW_SIGN]                             = Location::Sign(RC_KF_MIDOS_HOUSE_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-512, -459),       "Mido's House Arrow Sign",                          RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_MIDOS_HOUSE_ARROW_SIGN));
locationTable[RC_KF_TRAINING_CENTER_ENTRANCE_ARROW_SIGN]                = Location::Sign(RC_KF_TRAINING_CENTER_ENTRANCE_ARROW_SIGN,                 RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-779, 424),        "Training Center Entrance Arrow Sign",              RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_TRAINING_CENTER_ENTRANCE_ARROW_SIGN));
locationTable[RC_KF_INNER_TRAINING_CENTER_ARROW_SIGN]                   = Location::Sign(RC_KF_INNER_TRAINING_CENTER_ARROW_SIGN,                    RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-924, 928),        "Inner Training Center Arrow Sign",                 RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_INNER_TRAINING_CENTER_ARROW_SIGN));
locationTable[RC_KF_KNOW_IT_ALL_BROTHERS_HOUSE_ARROW_SIGN]              = Location::Sign(RC_KF_KNOW_IT_ALL_BROTHERS_HOUSE_ARROW_SIGN,               RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_KOKIRI_FOREST,            TWO_ACTOR_PARAMS(-1008, 479),       "Know-It-All Brothers House Arrow Sign",            RHT_SIGN_KOKIRI_FOREST,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_KNOW_IT_ALL_BROTHERS_HOUSE_ARROW_SIGN));
locationTable[RC_KF_LINKS_HOUSE_SIGN]                                   = Location::Sign(RC_KF_LINKS_HOUSE_SIGN,                                    RCQUEST_BOTH,       RCAREA_KOKIRI_FOREST,           SCENE_LINKS_HOUSE,              TWO_ACTOR_PARAMS(78, 116),          "Link's House Sign",                                RHT_SIGN_LINKS_HOUSE,           ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_KF_LINKS_HOUSE_SIGN));
locationTable[RC_LW_THEATER_RECTANGLE_SIGN]                             = Location::Sign(RC_LW_THEATER_RECTANGLE_SIGN,                              RCQUEST_BOTH,       RCAREA_LOST_WOODS,              SCENE_GROTTOS,                  TWO_ACTOR_PARAMS(3898, 1228),       "Theater Rectangle Sign",                           RHT_SIGN_DEKU_THEATER,          ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_LW_THEATER_RECTANGLE_SIGN));
locationTable[RC_HF_CASTLE_EXIT_ARROW_SIGN]                             = Location::Sign(RC_HF_CASTLE_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(-130, 2105),       "Castle Exit Arrow Sign",                           RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CASTLE_EXIT_ARROW_SIGN));
locationTable[RC_HF_WOODED_EXIT_ARROW_SIGN]                             = Location::Sign(RC_HF_WOODED_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(3952, 7232),       "Wooded Exit Arrow Sign",                           RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_WOODED_EXIT_ARROW_SIGN));
locationTable[RC_HF_ROCKY_PATH_EXIT_ARROW_SIGN]                         = Location::Sign(RC_HF_ROCKY_PATH_EXIT_ARROW_SIGN,                          RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(-8780, 7680),      "Rocky Path Exit Arrow Sign",                       RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_ROCKY_PATH_EXIT_ARROW_SIGN));
locationTable[RC_HF_FENCED_ARROW_SIGN]                                  = Location::Sign(RC_HF_FENCED_ARROW_SIGN,                                   RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(-4935, 14045),     "Fenced Arrow Sign",                                RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_FENCED_ARROW_SIGN));
locationTable[RC_HF_CENTER_EXIT_ARROW_SIGN]                             = Location::Sign(RC_HF_CENTER_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(-1564, 4318),      "Center Exit Arrow Sign",                           RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_CENTER_EXIT_ARROW_SIGN));
locationTable[RC_HF_RIVER_EXIT_ARROW_SIGN]                              = Location::Sign(RC_HF_RIVER_EXIT_ARROW_SIGN,                               RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(5925, 3805),       "River Exit Arrow Sign",                            RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_RIVER_EXIT_ARROW_SIGN));
locationTable[RC_HF_STAIRS_EXIT_ARROW_SIGN]                             = Location::Sign(RC_HF_STAIRS_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_HYRULE_FIELD,            SCENE_HYRULE_FIELD,             TWO_ACTOR_PARAMS(3230, 570),        "Stairs Exit Arrow Sign",                           RHT_SIGN_HYRULE_FIELD,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HF_STAIRS_EXIT_ARROW_SIGN));
locationTable[RC_MK_SHOOTING_GALLERY_RECTANGLE_SIGN]                    = Location::Sign(RC_MK_SHOOTING_GALLERY_RECTANGLE_SIGN,                     RCQUEST_BOTH,       RCAREA_MARKET,                  SCENE_SHOOTING_GALLERY,         TWO_ACTOR_PARAMS(59, 365),          "Shooting Gallery Rectangle Sign",                  RHT_SIGN_MK_SHOOTING_GALLERY,   ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_MK_SHOOTING_GALLERY_RECTANGLE_SIGN));
locationTable[RC_MK_MASK_SHOP_SIGN]                                     = Location::Sign(RC_MK_MASK_SHOP_SIGN,                                      RCQUEST_BOTH,       RCAREA_MARKET,                  SCENE_HAPPY_MASK_SHOP,          TWO_ACTOR_PARAMS(-112, 31),         "Mask Shop Sign",                                   RHT_SIGN_HAPPY_MASK_SHOP,       ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_MK_MASK_SHOP_SIGN));
locationTable[RC_TOT_ALTAR]                                             = Location::Sign(RC_TOT_ALTAR,                                              RCQUEST_BOTH,       RCAREA_MARKET,                  SCENE_TEMPLE_OF_TIME,           TWO_ACTOR_PARAMS(0, 1330),          "Altar",                                            RHT_SIGN_TEMPLE_OF_TIME,        ACTOR_EN_WONDER_TALK,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_TOT_ALTAR));
locationTable[RC_HC_DEAD_END_RECTANGLE_SIGN]                            = Location::Sign(RC_HC_DEAD_END_RECTANGLE_SIGN,                             RCQUEST_BOTH,       RCAREA_HYRULE_CASTLE,           SCENE_HYRULE_CASTLE,            TWO_ACTOR_PARAMS(2351, 2634),       "Dead End Rectangle Sign",                          RHT_SIGN_HYRULE_CASTLE,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_HC_DEAD_END_RECTANGLE_SIGN));
locationTable[RC_KAK_GUARD_GATE_RECTANGLE_SIGN]                         = Location::Sign(RC_KAK_GUARD_GATE_RECTANGLE_SIGN,                          RCQUEST_BOTH,       RCAREA_KAKARIKO_VILLAGE,        SCENE_KAKARIKO_VILLAGE,         TWO_ACTOR_PARAMS(210, -1275),       "Guard Gate Rectangle Sign",                        RHT_SIGN_KAKARIKO_VILLAGE,      ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_GUARD_GATE_RECTANGLE_SIGN));
locationTable[RC_KAK_WELL_RECTANGLE_SIGN]                               = Location::Sign(RC_KAK_WELL_RECTANGLE_SIGN,                                RCQUEST_BOTH,       RCAREA_KAKARIKO_VILLAGE,        SCENE_KAKARIKO_VILLAGE,         TWO_ACTOR_PARAMS(655, 525),         "Well Rectangle Sign",                              RHT_SIGN_KAKARIKO_VILLAGE,      ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_WELL_RECTANGLE_SIGN));
locationTable[RC_KAK_SOUTHEAST_EXIT_ARROW_SIGN]                         = Location::Sign(RC_KAK_SOUTHEAST_EXIT_ARROW_SIGN,                          RCQUEST_BOTH,       RCAREA_KAKARIKO_VILLAGE,        SCENE_KAKARIKO_VILLAGE,         TWO_ACTOR_PARAMS(1165, 1550),       "Southeast Exit Arrow Sign",                        RHT_SIGN_KAKARIKO_VILLAGE,      ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_SOUTHEAST_EXIT_ARROW_SIGN));
locationTable[RC_KAK_FRONT_GATE_ARROW_SIGN]                             = Location::Sign(RC_KAK_FRONT_GATE_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_KAKARIKO_VILLAGE,        SCENE_KAKARIKO_VILLAGE,         TWO_ACTOR_PARAMS(-2590, 965),       "Front Gate Arrow Sign",                            RHT_SIGN_KAKARIKO_VILLAGE,      ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_FRONT_GATE_ARROW_SIGN));
locationTable[RC_KAK_SHOOTING_GALLERY_RECTANGLE_SIGN]                   = Location::Sign(RC_KAK_SHOOTING_GALLERY_RECTANGLE_SIGN,                    RCQUEST_BOTH,       RCAREA_KAKARIKO_VILLAGE,        SCENE_SHOOTING_GALLERY,         TWO_ACTOR_PARAMS(59, 365),          "Shooting Gallery Rectangle Sign",                  RHT_SIGN_KAK_SHOOTING_GALLERY,  ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_KAK_SHOOTING_GALLERY_RECTANGLE_SIGN));
locationTable[RC_GY_ENTRANCE_RECTANGLE_SIGN]                            = Location::Sign(RC_GY_ENTRANCE_RECTANGLE_SIGN,                             RCQUEST_BOTH,       RCAREA_GRAVEYARD,               SCENE_GRAVEYARD,                TWO_ACTOR_PARAMS(-1110, 430),       "Entrance Rectangle Sign",                          RHT_SIGN_GRAVEYARD,             ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_ENTRANCE_RECTANGLE_SIGN));
locationTable[RC_GY_ENTRANCE_PLINTH]                                    = Location::Sign(RC_GY_ENTRANCE_PLINTH,                                     RCQUEST_BOTH,       RCAREA_GRAVEYARD,               SCENE_GRAVEYARD,                TWO_ACTOR_PARAMS(-805, 266),        "Entrance Plinth",                                  RHT_SIGN_GRAVEYARD,             ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_ENTRANCE_PLINTH));
locationTable[RC_GY_RIGHT_OF_ROYAL_TOMB_GRAVE]                          = Location::Sign(RC_GY_RIGHT_OF_ROYAL_TOMB_GRAVE,                           RCQUEST_BOTH,       RCAREA_GRAVEYARD,               SCENE_GRAVEYARD,                TWO_ACTOR_PARAMS(654, 258),         "Right of Royal Tomb Grave",                        RHT_SIGN_GRAVEYARD,             ACTOR_EN_WONDER_TALK,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_RIGHT_OF_ROYAL_TOMB_GRAVE));
locationTable[RC_GY_LEFT_OF_ROYAL_TOMB_GRAVE]                           = Location::Sign(RC_GY_LEFT_OF_ROYAL_TOMB_GRAVE,                            RCQUEST_BOTH,       RCAREA_GRAVEYARD,               SCENE_GRAVEYARD,                TWO_ACTOR_PARAMS(654, -102),        "Left of Royal Tomb Grave",                         RHT_SIGN_GRAVEYARD,             ACTOR_EN_WONDER_TALK,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_LEFT_OF_ROYAL_TOMB_GRAVE));
locationTable[RC_GY_ROYAL_TOMB_GRAVE]                                   = Location::Sign(RC_GY_ROYAL_TOMB_GRAVE,                                    RCQUEST_BOTH,       RCAREA_GRAVEYARD,               SCENE_GRAVEYARD,                TWO_ACTOR_PARAMS(752, 85),          "Royal Tomb Grave",                                 RHT_SIGN_GRAVEYARD,             ACTOR_EN_WONDER_TALK,       SpoilerCollectionCheck::RandomizerInf(RAND_INF_GY_ROYAL_TOMB_GRAVE));
locationTable[RC_DMT_ABOVE_DODONGO_RECTANGLE_SIGN]                      = Location::Sign(RC_DMT_ABOVE_DODONGO_RECTANGLE_SIGN,                       RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-1300, -496),      "Above Dodongo Rectangle Sign",                     RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ABOVE_DODONGO_RECTANGLE_SIGN));
locationTable[RC_DMT_ADULT_CENTER_EXIT_ARROW_SIGN]                      = Location::Sign(RC_DMT_ADULT_CENTER_EXIT_ARROW_SIGN,                       RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-299, -1787),      "Adult Center Exit Arrow Sign",                     RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_ADULT_CENTER_EXIT_ARROW_SIGN));
locationTable[RC_DMT_CHILD_CENTER_EXIT_RECTANGLE_SIGN]                  = Location::Sign(RC_DMT_CHILD_CENTER_EXIT_RECTANGLE_SIGN,                   RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-299, -1787),      "Child Center Exit Rectangle Sign",                 RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CHILD_CENTER_EXIT_RECTANGLE_SIGN));
locationTable[RC_DMT_DODONGOS_CAVERN_RECTANGLE_SIGN]                    = Location::Sign(RC_DMT_DODONGOS_CAVERN_RECTANGLE_SIGN,                     RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-1834, -571),      "Dodongo's Cavern Rectangle Sign",                  RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_DODONGOS_CAVERN_RECTANGLE_SIGN));
locationTable[RC_DMT_CENTER_TRAIL_RECTANGLE_SIGN]                       = Location::Sign(RC_DMT_CENTER_TRAIL_RECTANGLE_SIGN,                        RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-1260, 540),       "Center Trail Rectangle Sign",                      RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_CENTER_TRAIL_RECTANGLE_SIGN));
locationTable[RC_DMT_TO_UPPER_TRAIL_ARROW_SIGN]                         = Location::Sign(RC_DMT_TO_UPPER_TRAIL_ARROW_SIGN,                          RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-1127, 44),        "To Upper Trail Arrow Sign",                        RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_TO_UPPER_TRAIL_ARROW_SIGN));
locationTable[RC_DMT_UPPER_EXIT_ARROW_SIGN]                             = Location::Sign(RC_DMT_UPPER_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-40, -4410),       "Upper Exit Arrow Sign",                            RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_UPPER_EXIT_ARROW_SIGN));
locationTable[RC_DMT_TO_CENTER_EXIT_ARROW_SIGN]                         = Location::Sign(RC_DMT_TO_CENTER_EXIT_ARROW_SIGN,                          RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-735, 595),        "To Center Exit Arrow Sign",                        RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_TO_CENTER_EXIT_ARROW_SIGN));
locationTable[RC_DMT_LOWER_EXIT_ARROW_SIGN]                             = Location::Sign(RC_DMT_LOWER_EXIT_ARROW_SIGN,                              RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_TRAIL,    SCENE_DEATH_MOUNTAIN_TRAIL,     TWO_ACTOR_PARAMS(-1300, 2155),      "Lower Exit Arrow Sign",                            RHT_SIGN_DEATH_MOUNTAIN_TRAIL,  ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMT_LOWER_EXIT_ARROW_SIGN));
locationTable[RC_GC_CHILD_ROLLING_GORON_RECTANGLE_SIGN]                 = Location::Sign(RC_GC_CHILD_ROLLING_GORON_RECTANGLE_SIGN,                  RCQUEST_BOTH,       RCAREA_GORON_CITY,              SCENE_GORON_CITY,               TWO_ACTOR_PARAMS(333, -684),        "Child Rolling Goron Rectangle Sign",               RHT_SIGN_GORON_CITY,            ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GC_CHILD_ROLLING_GORON_RECTANGLE_SIGN));
locationTable[RC_DMC_BRIDGE_EXIT_ARROW_SIGN]                            = Location::Sign(RC_DMC_BRIDGE_EXIT_ARROW_SIGN,                             RCQUEST_BOTH,       RCAREA_DEATH_MOUNTAIN_CRATER,   SCENE_DEATH_MOUNTAIN_CRATER,    TWO_ACTOR_PARAMS(-1610, 95),        "Bridge Exit Arrow Sign",                           RHT_SIGN_DEATH_MOUNTAIN_CRATER, ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_DMC_BRIDGE_EXIT_ARROW_SIGN));
locationTable[RC_ZR_SLEEPLESS_WATERFALL_PLAQUE]                         = Location::Sign(RC_ZR_SLEEPLESS_WATERFALL_PLAQUE,                          RCQUEST_BOTH,       RCAREA_ZORAS_RIVER,             SCENE_ZORAS_RIVER,              TWO_ACTOR_PARAMS(4096, -1401),      "Sleepless Waterfall Plaque",                       RHT_SIGN_ZORAS_RIVER,           ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZR_SLEEPLESS_WATERFALL_PLAQUE));
locationTable[RC_ZD_SHOP_RECTANGLE_SIGN]                                = Location::Sign(RC_ZD_SHOP_RECTANGLE_SIGN,                                 RCQUEST_BOTH,       RCAREA_ZORAS_DOMAIN,            SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(477, 318),         "Shop Rectangle Sign",                              RHT_SIGN_ZORAS_DOMAIN,          ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_SHOP_RECTANGLE_SIGN));
locationTable[RC_ZD_ENTRANCE_RECTANGLE_SIGN]                            = Location::Sign(RC_ZD_ENTRANCE_RECTANGLE_SIGN,                             RCQUEST_BOTH,       RCAREA_ZORAS_DOMAIN,            SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(-980, -210),       "Entrance Rectangle Sign",                          RHT_SIGN_ZORAS_DOMAIN,          ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_ENTRANCE_RECTANGLE_SIGN));
locationTable[RC_ZD_KING_ZORA_PATH_ARROW_SIGN]                          = Location::Sign(RC_ZD_KING_ZORA_PATH_ARROW_SIGN,                           RCQUEST_BOTH,       RCAREA_ZORAS_DOMAIN,            SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(217, 150),         "King Zora Path Arrow Sign",                        RHT_SIGN_ZORAS_DOMAIN,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_KING_ZORA_PATH_ARROW_SIGN));
locationTable[RC_ZD_NEAR_KING_ZORA_RECTANGLE_SIGN]                      = Location::Sign(RC_ZD_NEAR_KING_ZORA_RECTANGLE_SIGN,                       RCQUEST_BOTH,       RCAREA_ZORAS_DOMAIN,            SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(720, -1345),       "Near King Zora Rectangle Sign",                    RHT_SIGN_ZORAS_DOMAIN,          ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_NEAR_KING_ZORA_RECTANGLE_SIGN));
locationTable[RC_ZD_NEAR_KING_ZORA_ARROW_SIGN]                          = Location::Sign(RC_ZD_NEAR_KING_ZORA_ARROW_SIGN,                           RCQUEST_BOTH,       RCAREA_ZORAS_DOMAIN,            SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(345, -1572),       "Near King Zora Arrow Sign",                        RHT_SIGN_ZORAS_DOMAIN,          ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_NEAR_KING_ZORA_ARROW_SIGN));
locationTable[RC_ZF_JABU_JABU_PLATFORM_RECTANGLE_SIGN]                  = Location::Sign(RC_ZF_JABU_JABU_PLATFORM_RECTANGLE_SIGN,                   RCQUEST_BOTH,       RCAREA_ZORAS_FOUNTAIN,          SCENE_ZORAS_FOUNTAIN,           TWO_ACTOR_PARAMS(-1900, 208),       "Jabu-Jabu Platform Rectangle Sign",                RHT_SIGN_ZORAS_FOUNTAIN,        ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_JABU_JABU_PLATFORM_RECTANGLE_SIGN));
locationTable[RC_ZF_ENTRANCE_ARROW_SIGN]                                = Location::Sign(RC_ZF_ENTRANCE_ARROW_SIGN,                                 RCQUEST_BOTH,       RCAREA_ZORAS_FOUNTAIN,          SCENE_ZORAS_FOUNTAIN,           TWO_ACTOR_PARAMS(-2557, 486),       "Entrance Arrow Sign",                              RHT_SIGN_ZORAS_FOUNTAIN,        ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZF_ENTRANCE_ARROW_SIGN));
locationTable[RC_LH_LAB_RECTANGLE_SIGN]                                 = Location::Sign(RC_LH_LAB_RECTANGLE_SIGN,                                  RCQUEST_BOTH,       RCAREA_LAKE_HYLIA,              SCENE_LAKE_HYLIA,               TWO_ACTOR_PARAMS(-2300, 3670),      "Lab Rectangle Sign",                               RHT_SIGN_LAKE_HYLIA,            ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_LAB_RECTANGLE_SIGN));
locationTable[RC_LH_NORTH_EXIT_ARROW_SIGN]                              = Location::Sign(RC_LH_NORTH_EXIT_ARROW_SIGN,                               RCQUEST_BOTH,       RCAREA_LAKE_HYLIA,              SCENE_LAKE_HYLIA,               TWO_ACTOR_PARAMS(-1835, 995),       "North Exit Arrow Sign",                            RHT_SIGN_LAKE_HYLIA,            ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_NORTH_EXIT_ARROW_SIGN));
locationTable[RC_LH_FISHING_SIGN]                                       = Location::Sign(RC_LH_FISHING_SIGN,                                        RCQUEST_BOTH,       RCAREA_LAKE_HYLIA,              SCENE_LAKE_HYLIA,               TWO_ACTOR_PARAMS(1341, 3779),       "Fishing Sign",                                     RHT_SIGN_LAKE_HYLIA,            ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_FISHING_SIGN));
locationTable[RC_LH_ISLAND_PEDESTAL]                                    = Location::Sign(RC_LH_ISLAND_PEDESTAL,                                     RCQUEST_BOTH,       RCAREA_LAKE_HYLIA,              SCENE_LAKE_HYLIA,               TWO_ACTOR_PARAMS(-491, 7259),       "Island Pedestal",                                  RHT_SIGN_LAKE_HYLIA,            ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_ISLAND_PEDESTAL));
locationTable[RC_LH_FISHING_POND_RECTANGLE_SIGN]                        = Location::Sign(RC_LH_FISHING_POND_RECTANGLE_SIGN,                         RCQUEST_BOTH,       RCAREA_LAKE_HYLIA,              SCENE_FISHING_POND,             TWO_ACTOR_PARAMS(53, 982),          "Fishing Pond Rectangle Sign",                      RHT_SIGN_FISHING_POND,          ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_LH_FISHING_POND_RECTANGLE_SIGN));
locationTable[RC_GV_BRIDGE_RECTANGLE_SIGN]                              = Location::Sign(RC_GV_BRIDGE_RECTANGLE_SIGN,                               RCQUEST_BOTH,       RCAREA_GERUDO_VALLEY,           SCENE_GERUDO_VALLEY,            TWO_ACTOR_PARAMS(359, 254),         "Bridge Rectangle Sign",                            RHT_SIGN_GERUDO_VALLEY,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_BRIDGE_RECTANGLE_SIGN));
locationTable[RC_GV_EAST_EXIT_ARROW_SIGN]                               = Location::Sign(RC_GV_EAST_EXIT_ARROW_SIGN,                                RCQUEST_BOTH,       RCAREA_GERUDO_VALLEY,           SCENE_GERUDO_VALLEY,            TWO_ACTOR_PARAMS(2778, 593),        "East Exit Arrow Sign",                             RHT_SIGN_GERUDO_VALLEY,         ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_GV_EAST_EXIT_ARROW_SIGN));
locationTable[RC_GF_EAST_EXIT_ARROW_SIGN]                               = Location::Sign(RC_GF_EAST_EXIT_ARROW_SIGN,                                RCQUEST_BOTH,       RCAREA_GERUDO_FORTRESS,         SCENE_GERUDOS_FORTRESS,         TWO_ACTOR_PARAMS(-730, -70),        "East Exit Arrow Sign",                             RHT_SIGN_GERUDO_FORTRESS,       ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_GF_EAST_EXIT_ARROW_SIGN));
locationTable[RC_GF_HBA_RECTANGLE_SIGN]                                 = Location::Sign(RC_GF_HBA_RECTANGLE_SIGN,                                  RCQUEST_BOTH,       RCAREA_GERUDO_FORTRESS,         SCENE_GERUDOS_FORTRESS,         TWO_ACTOR_PARAMS(3635, -360),       "HBA Rectangle Sign",                               RHT_SIGN_GERUDO_FORTRESS,       ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GF_HBA_RECTANGLE_SIGN));
locationTable[RC_GF_GATE_EXIT_RECTANGLE_SIGN]                           = Location::Sign(RC_GF_GATE_EXIT_RECTANGLE_SIGN,                            RCQUEST_BOTH,       RCAREA_GERUDO_FORTRESS,         SCENE_GERUDOS_FORTRESS,         TWO_ACTOR_PARAMS(-1850, -3250),     "Gate Exit Rectangle Sign",                         RHT_SIGN_GERUDO_FORTRESS,       ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GF_GATE_EXIT_RECTANGLE_SIGN));
locationTable[RC_GF_GTG_ENTRANCE_RECTANGLE_SIGN]                        = Location::Sign(RC_GF_GTG_ENTRANCE_RECTANGLE_SIGN,                         RCQUEST_BOTH,       RCAREA_GERUDO_FORTRESS,         SCENE_GERUDOS_FORTRESS,         TWO_ACTOR_PARAMS(14, -1159),        "GTG Entrance Rectangle Sign",                      RHT_SIGN_GERUDO_FORTRESS,       ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_GF_GTG_ENTRANCE_RECTANGLE_SIGN));
locationTable[RC_HW_CARPET_SALESMAN_ARROW_SIGN]                         = Location::Sign(RC_HW_CARPET_SALESMAN_ARROW_SIGN,                          RCQUEST_BOTH,       RCAREA_WASTELAND,               SCENE_HAUNTED_WASTELAND,        TWO_ACTOR_PARAMS(2104, 2389),       "Carpet Salesman Arrow Sign",                       RHT_SIGN_HAUNTED_WASTELAND,     ACTOR_EN_A_OBJ,             SpoilerCollectionCheck::RandomizerInf(RAND_INF_HW_CARPET_SALESMAN_ARROW_SIGN));
locationTable[RC_HW_POE_ALTAR]                                          = Location::Sign(RC_HW_POE_ALTAR,                                           RCQUEST_BOTH,       RCAREA_WASTELAND,               SCENE_HAUNTED_WASTELAND,        TWO_ACTOR_PARAMS(645, -2234),       "Poe Altar",                                        RHT_SIGN_HAUNTED_WASTELAND,     ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_HW_POE_ALTAR));
// Dungeon Signs
locationTable[RC_DODONGOS_CAVERN_TOP_FLOOR_PEDESTAL]                    = Location::Sign(RC_DODONGOS_CAVERN_TOP_FLOOR_PEDESTAL,                     RCQUEST_BOTH,       RCAREA_DODONGOS_CAVERN,         SCENE_DODONGOS_CAVERN,          TWO_ACTOR_PARAMS(578, -929),        "Top Floor Pedestal",                               RHT_SIGN_DODONGOS_CAVERN,       ACTOR_EN_WONDER_TALK2,      SpoilerCollectionCheck::RandomizerInf(RAND_INF_DODONGOS_CAVERN_TOP_FLOOR_PEDESTAL));
locationTable[RC_SHADOW_TEMPLE_TRUTHSPINNER_RECTANGLE_SIGN]             = Location::Sign(RC_SHADOW_TEMPLE_TRUTHSPINNER_RECTANGLE_SIGN,              RCQUEST_VANILLA,    RCAREA_SHADOW_TEMPLE,           SCENE_SHADOW_TEMPLE,            TWO_ACTOR_PARAMS(910, -192),        "Truthspinner Rectangle Sign",                      RHT_SIGN_SHADOW_TEMPLE,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_TEMPLE_TRUTHSPINNER_RECTANGLE_SIGN));
// MQ Dungeon Signs
locationTable[RC_SHADOW_TEMPLE_MQ_LOWER_PIT_RECTANGLE_SIGN]             = Location::Sign(RC_SHADOW_TEMPLE_MQ_LOWER_PIT_RECTANGLE_SIGN,              RCQUEST_MQ,         RCAREA_SHADOW_TEMPLE,           SCENE_SHADOW_TEMPLE,            TWO_ACTOR_PARAMS(2893, 2705),       "Lower Pit Rectangle Sign",                         RHT_SIGN_SHADOW_TEMPLE,         ACTOR_EN_KANBAN,            SpoilerCollectionCheck::RandomizerInf(RAND_INF_SHADOW_TEMPLE_MQ_LOWER_PIT_RECTANGLE_SIGN));

    // clang-format on
}

static RegisterShipInitFunc initFunc(RegisterShuffleSigns, { "IS_RANDO" });
static RegisterShipInitFunc registerSignLocations(Rando::StaticData::RegisterSignLocations);