#include "soh/OTRGlobals.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "item_category_adj.h"
#include "particle_cmc.h"
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include "functions.h"
#include "overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"
#include "objects/object_ice_objects/object_ice_objects.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

uint8_t BgIceShelter_RandomizerHoldsItem(Actor* actor) {
    const auto redIceIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(actor);
    if (redIceIdentity == nullptr) {
        return false;
    }

    RandomizerCheck rc = redIceIdentity->randomizerCheck;

    // Don't pull randomized item if icicle isn't randomized or is already checked
    if (!IS_RANDO || Flags_GetRandomizerInf(redIceIdentity->randomizerInf) ||
        redIceIdentity->randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    } else {
        return true;
    }
}

static void BgIceShelter_RandomizerDraw(Actor* actor, Color_RGBA8* primColor, Color_RGBA8* secColor,
                                        Color_RGBA8* envColor) {
    Vec3f pos;
    s32 type = (actor->params >> 8) & 7;
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };

    velocity.y = -0.05f;
    accel.y = -0.025f;

    // align for King Zora's much bigger red ice
    f32 xzScale = (type == RED_ICE_KING_ZORA) ? 30.0f : 15.0f;
    f32 yOffset = (type == RED_ICE_KING_ZORA) ? 200.0f : 0.0f;
    f32 zOffset = (type == RED_ICE_KING_ZORA) ? 50.0f : 0.0f;

    pos.x = Rand_CenteredFloat(xzScale) + actor->world.pos.x;
    pos.y = (Rand_ZeroOne() * 15.0f) + actor->world.pos.y + yOffset;
    pos.z = Rand_CenteredFloat(xzScale) + actor->world.pos.z + zOffset;
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, secColor, envColor, 2000, 100);
    EffectSsKiraKira_SpawnFocused(gPlayState, &pos, &velocity, &accel, primColor, envColor, 2000, 100);
}

void BgIceShelter_RandomizerDrawSetup(void* actor) {
    GetItemCategory getItemCategory;
    Actor* redIceActor = (Actor*)actor;

    // If not a randomized item or too far, don't draw
    if (!BgIceShelter_RandomizerHoldsItem(redIceActor) || redIceActor->xzDistToPlayer > 1000.0f) {
        return;
    }

    bool cmc = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), 0);
    int requiresStoneAgony = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);

    int isNotCMC = !cmc || (requiresStoneAgony && !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY));

    Color_RGBA8 primColor;
    Color_RGBA8 secColor;
    Color_RGBA8 envColor;

    const auto redIceIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(redIceActor);
    if (redIceIdentity == nullptr) {
        return;
    }

    GetItemEntry redIceItem =
        Rando::Context::GetInstance()->GetFinalGIEntry(redIceIdentity->randomizerCheck, true, GI_NONE);
    getItemCategory = Randomizer_AdjustItemCategory(redIceItem);

    if (isNotCMC) {
        getItemCategory = ITEM_CATEGORY_MAJOR;
    }
    primColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_PRIMARY);
    secColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_SECONDARY);
    envColor = Randomizer_GetParticleCMCColor(getItemCategory, COLOR_FLARE);
    BgIceShelter_RandomizerDraw(redIceActor, &primColor, &secColor, &envColor);
}

void BgIceShelter_RandomizerSpawnCollectible(Actor* actor) {
    const auto redIceIdentity = ObjectExtension::GetInstance().Get<CheckIdentity>(actor);
    Player* player = GET_PLAYER(gPlayState);

    // If King Zora, autocollect to avoid spawning issues
    if (redIceIdentity->randomizerCheck == RC_ZD_KING_ZORA_RED_ICE) {
        Flags_SetRandomizerInf(redIceIdentity->randomizerInf);
    } else {
        EnItem00* item00 = (EnItem00*)Item_DropCollectible2(gPlayState, &actor->world.pos, ITEM00_SOH_DUMMY);
        item00->randoInf = redIceIdentity->randomizerInf;
        item00->itemEntry =
            Rando::Context::GetInstance()->GetFinalGIEntry(redIceIdentity->randomizerCheck, true, GI_NONE);
        item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
        item00->actor.velocity.y = 8.0f;
        // In general, spawn in place, but for checks with objects blocking, spawn out toward player
        if ((redIceIdentity->randomizerCheck >= RC_ICE_CAVERN_HEART_PIECE_ROOM_FREESTANDING_RED_ICE &&
             redIceIdentity->randomizerCheck <= RC_ICE_CAVERN_NEAR_END_RIGHT_RED_ICE) ||
            redIceIdentity->randomizerCheck == RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_LEFT_RED_ICE ||
            redIceIdentity->randomizerCheck == RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_RIGHT_RED_ICE ||
            redIceIdentity->randomizerCheck == RC_GANONS_CASTLE_MQ_WATER_TRIAL_SILVER_RUPEE_RED_ICE) {
            item00->actor.speedXZ = 2.0f;
            item00->actor.world.rot.y =
                Math_Vec3f_Yaw(&item00->actor.world.pos, &player->actor.world.pos) + (s16)Rand_CenteredFloat(16384.0f);
        } else {
            item00->actor.speedXZ = 0.0f;
            item00->actor.world.rot.y = static_cast<int16_t>(Rand_CenteredFloat(65536.0f));
        }
    }
}

void RegisterShuffleRedIce() {
    bool shouldRegister = IS_RANDO && Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_RED_ICE).Get();

    COND_VB_SHOULD(VB_RED_ICE_MELTED_FLAG, shouldRegister, {
        BgIceShelter* redIceActor = va_arg(args, BgIceShelter*);
        Actor* actor = (Actor*)redIceActor;

        auto redIceIdentity = OTRGlobals::Instance->gRandomizer->IdentifyRedIce(
            gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
        ObjectExtension::GetInstance().Set<CheckIdentity>(actor, std::move(redIceIdentity));

        if (*should) {
            if (BgIceShelter_RandomizerHoldsItem(&redIceActor->dyna.actor)) {
                BgIceShelter_RandomizerSpawnCollectible(actor);
            }
        }
    });

    // Draw particle effect to indicate a randomized item
    COND_ID_HOOK(OnActorUpdate, ACTOR_BG_ICE_SHELTER, shouldRegister, BgIceShelter_RandomizerDrawSetup);

    // Collect item for melting red ice
    COND_VB_SHOULD(VB_RED_ICE_DROP_ITEM, shouldRegister, {
        BgIceShelter* redIceActor = va_arg(args, BgIceShelter*);

        if (*should) {
            if (BgIceShelter_RandomizerHoldsItem(&redIceActor->dyna.actor)) {
                BgIceShelter_RandomizerSpawnCollectible(&redIceActor->dyna.actor);
            }
        }
    });
}

void Rando::StaticData::RegisterRedIceLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    //            Randomizer Check                                                  Randomizer Check                                                                     Quest                Area                              Scene ID                        Params                              Short Name                                      Hint Text Key                           Spoiler Collection Check
    locationTable[RC_ZD_KING_ZORA_RED_ICE]                                          = Location::RedIce(RC_ZD_KING_ZORA_RED_ICE,                                          RCQUEST_BOTH,        RCAREA_ZORAS_DOMAIN,              SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(628, -1818),       "King Zora Red Ice",                            RHT_RED_ICE_ZORAS_DOMAIN,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_KING_ZORA_RED_ICE));
    locationTable[RC_ZD_ZORA_SHOP_RED_ICE]                                          = Location::RedIce(RC_ZD_ZORA_SHOP_RED_ICE,                                          RCQUEST_BOTH,        RCAREA_ZORAS_DOMAIN,              SCENE_ZORAS_DOMAIN,             TWO_ACTOR_PARAMS(483, 214),         "Zora Shop Red Ice",                            RHT_RED_ICE_ZORAS_DOMAIN,               SpoilerCollectionCheck::RandomizerInf(RAND_INF_ZD_ZORA_SHOP_RED_ICE));
    locationTable[RC_ICE_CAVERN_ENTRANCE_RED_ICE]                                   = Location::RedIce(RC_ICE_CAVERN_ENTRANCE_RED_ICE,                                   RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(411, 2332),        "Entrance Red Ice",                             RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_ENTRANCE_RED_ICE));
    locationTable[RC_ICE_CAVERN_LOBBY_LEFT_RED_ICE]                                 = Location::RedIce(RC_ICE_CAVERN_LOBBY_LEFT_RED_ICE,                                 RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-105, 854),        "Lobby Left Red Ice",                           RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_LOBBY_LEFT_RED_ICE));
    locationTable[RC_ICE_CAVERN_LOBBY_RIGHT_RED_ICE]                                = Location::RedIce(RC_ICE_CAVERN_LOBBY_RIGHT_RED_ICE,                                RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(119, 856),         "Lobby Right Red Ice",                          RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_LOBBY_RIGHT_RED_ICE));
    locationTable[RC_ICE_CAVERN_SPINNING_BLADE_EAST_RED_ICE]                        = Location::RedIce(RC_ICE_CAVERN_SPINNING_BLADE_EAST_RED_ICE,                        RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(651, -232),        "Spinning Blade East Red Ice",                  RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_SPINNING_BLADE_EAST_RED_ICE));
    locationTable[RC_ICE_CAVERN_SPINNING_BLADE_WEST_RED_ICE]                        = Location::RedIce(RC_ICE_CAVERN_SPINNING_BLADE_WEST_RED_ICE,                        RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-134, -415),       "Spinning Blade West Red Ice",                  RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_SPINNING_BLADE_WEST_RED_ICE));
    locationTable[RC_ICE_CAVERN_HEART_PIECE_ROOM_FREESTANDING_RED_ICE]              = Location::RedIce(RC_ICE_CAVERN_HEART_PIECE_ROOM_FREESTANDING_RED_ICE,              RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(1261, 68),         "Heart Piece Room Freestanding Red Ice",        RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_HEART_PIECE_ROOM_FREESTANDING_RED_ICE));
    locationTable[RC_ICE_CAVERN_HEART_PIECE_ROOM_CHEST_RED_ICE]                     = Location::RedIce(RC_ICE_CAVERN_HEART_PIECE_ROOM_CHEST_RED_ICE,                     RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(1201, 643),        "Heart Piece Room Chest Red Ice",               RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_HEART_PIECE_ROOM_CHEST_RED_ICE));
    locationTable[RC_ICE_CAVERN_MAP_ROOM_POT_RED_ICE]                               = Location::RedIce(RC_ICE_CAVERN_MAP_ROOM_POT_RED_ICE,                               RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(366, -2036),       "Map Room Pot Red Ice",                         RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MAP_ROOM_POT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MAP_ROOM_CHEST_RED_ICE]                             = Location::RedIce(RC_ICE_CAVERN_MAP_ROOM_CHEST_RED_ICE,                             RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(277, -2600),       "Map Room Chest Red Ice",                       RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MAP_ROOM_CHEST_RED_ICE));
    locationTable[RC_ICE_CAVERN_SILVER_RUPEE_RED_ICE]                               = Location::RedIce(RC_ICE_CAVERN_SILVER_RUPEE_RED_ICE,                               RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1126, -1577),     "Silver Rupee Red Ice",                         RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_SILVER_RUPEE_RED_ICE));
    locationTable[RC_ICE_CAVERN_NEAR_END_LEFT_RED_ICE]                              = Location::RedIce(RC_ICE_CAVERN_NEAR_END_LEFT_RED_ICE,                              RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1422, 586),       "Near End Left Red Ice",                        RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_END_LEFT_RED_ICE));
    locationTable[RC_ICE_CAVERN_NEAR_END_MIDDLE_RED_ICE]                            = Location::RedIce(RC_ICE_CAVERN_NEAR_END_MIDDLE_RED_ICE,                            RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1459, 625),       "Near End Middle Red Ice",                      RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_END_MIDDLE_RED_ICE));
    locationTable[RC_ICE_CAVERN_NEAR_END_RIGHT_RED_ICE]                             = Location::RedIce(RC_ICE_CAVERN_NEAR_END_RIGHT_RED_ICE,                             RCQUEST_VANILLA,     RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1488, 676),       "Near End Right Red Ice",                       RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_NEAR_END_RIGHT_RED_ICE));
    locationTable[RC_GANONS_CASTLE_WATER_TRIAL_DOOR_RED_ICE]                        = Location::RedIce(RC_GANONS_CASTLE_WATER_TRIAL_DOOR_RED_ICE,                        RCQUEST_VANILLA,     RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2212, -840),       "Water Trial Door Red Ice",                     RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_WATER_TRIAL_DOOR_RED_ICE));
    locationTable[RC_GANONS_CASTLE_WATER_TRIAL_RUSTED_SWITCH_RED_ICE]               = Location::RedIce(RC_GANONS_CASTLE_WATER_TRIAL_RUSTED_SWITCH_RED_ICE,               RCQUEST_VANILLA,     RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2912, -1420),      "Water Trial Rusted Switch Red Ice",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_WATER_TRIAL_RUSTED_SWITCH_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_WEST_LEFT_RED_ICE]                           = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_WEST_LEFT_RED_ICE,                           RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-142, -377),       "Hub West Left Red Ice",                        RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_WEST_LEFT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_WEST_MIDDLE_RED_ICE]                         = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_WEST_MIDDLE_RED_ICE,                         RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-121, -418),       "Hub West Middle Red Ice",                      RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_WEST_MIDDLE_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_WEST_RIGHT_RED_ICE]                          = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_WEST_RIGHT_RED_ICE,                          RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-134, -462),       "Hub West Right Red Ice",                       RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_WEST_RIGHT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_LEDGE_LEFT_RED_ICE]                          = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_LEDGE_LEFT_RED_ICE,                          RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(577, -818),        "Hub Ledge Left Red Ice",                       RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_LEDGE_LEFT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_LEDGE_MIDDLE_RED_ICE]                        = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_LEDGE_MIDDLE_RED_ICE,                        RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(614, -770),        "Hub Ledge Middle Red Ice",                     RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_LEDGE_MIDDLE_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_HUB_LEDGE_RIGHT_RED_ICE]                         = Location::RedIce(RC_ICE_CAVERN_MQ_HUB_LEDGE_RIGHT_RED_ICE,                         RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(656, -722),        "Hub Ledge Right Red Ice",                      RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_HUB_LEDGE_RIGHT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_COMPASS_RED_ICE]                                 = Location::RedIce(RC_ICE_CAVERN_MQ_COMPASS_RED_ICE,                                 RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(376, -2048),       "Compass Red Ice",                              RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_COMPASS_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_MAP_RED_ICE]                                     = Location::RedIce(RC_ICE_CAVERN_MQ_MAP_RED_ICE,                                     RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(1201, 648),        "Map Red Ice",                                  RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_MAP_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_SCARECROW_LEFT_RED_ICE]                          = Location::RedIce(RC_ICE_CAVERN_MQ_SCARECROW_LEFT_RED_ICE,                          RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1335, -159),      "Scarecrow Left Red Ice",                       RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_SCARECROW_LEFT_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_SCARECROW_MIDDLE_RED_ICE]                        = Location::RedIce(RC_ICE_CAVERN_MQ_SCARECROW_MIDDLE_RED_ICE,                        RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1386, -159),      "Scarecrow Middle Red Ice",                     RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_SCARECROW_MIDDLE_RED_ICE));
    locationTable[RC_ICE_CAVERN_MQ_SCARECROW_RIGHT_RED_ICE]                         = Location::RedIce(RC_ICE_CAVERN_MQ_SCARECROW_RIGHT_RED_ICE,                         RCQUEST_MQ,          RCAREA_ICE_CAVERN,                SCENE_ICE_CAVERN,               TWO_ACTOR_PARAMS(-1438, -155),      "Scarecrow Right Red Ice",                      RHT_ICE_CAVERN_RED_ICE,                 SpoilerCollectionCheck::RandomizerInf(RAND_INF_ICE_CAVERN_MQ_SCARECROW_RIGHT_RED_ICE));
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM_RED_ICE]                = Location::RedIce(RC_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM_RED_ICE,                RCQUEST_MQ,          RCAREA_GERUDO_TRAINING_GROUND,    SCENE_GERUDO_TRAINING_GROUND,   TWO_ACTOR_PARAMS(-864, -2745),      "Stalfos Room Red Ice",                         RHT_GERUDO_TRAINING_GROUND_RED_ICE,     SpoilerCollectionCheck::RandomizerInf(RAND_INF_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM_RED_ICE));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_LEFT_RED_ICE]          = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_LEFT_RED_ICE,          RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(1754, -1152),      "Water Trial First Room Left Red Ice",          RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_LEFT_RED_ICE));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_RIGHT_RED_ICE]         = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_RIGHT_RED_ICE,         RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(1743, -529),       "Water Trial First Room Right Red Ice",         RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_RIGHT_RED_ICE));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_BACK_LEFT_RED_ICE]     = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_BACK_LEFT_RED_ICE,     RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2096, -1011),      "Water Trial First Room Back Left Red Ice",     RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_BACK_LEFT_RED_ICE));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_1]        = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_1,        RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2209, -889),       "Water Trial First Room Door Red Ice 1",        RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_1));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_2]        = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_2,        RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2198, -845),       "Water Trial First Room Door Red Ice 2",        RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_2));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_3]        = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_3,        RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2194, -797),       "Water Trial First Room Door Red Ice 3",        RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_3));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_4]        = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_4,        RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2200, -755),       "Water Trial First Room Door Red Ice 4",        RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_FIRST_ROOM_DOOR_RED_ICE_4));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SILVER_RUPEE_RED_ICE]             = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SILVER_RUPEE_RED_ICE,             RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(3370, -729),       "Water Trial Silver Rupee Red Ice",             RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SILVER_RUPEE_RED_ICE));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_1]            = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_1,            RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(3370, -776),       "Water Trial Second Door Red Ice 1",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_1));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_2]            = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_2,            RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(3370, -834),       "Water Trial Second Door Red Ice 2",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_2));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_3]            = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_3,            RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(3370, -889),       "Water Trial Second Door Red Ice 3",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_3));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_4]            = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_4,            RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(3370, -950),       "Water Trial Second Door Red Ice 4",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_4));
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_5]            = Location::RedIce(RC_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_5,            RCQUEST_MQ,          RCAREA_GANONS_CASTLE,             SCENE_INSIDE_GANONS_CASTLE,     TWO_ACTOR_PARAMS(2906, -1482),      "Water Trial Second Door Red Ice 5",            RHT_GANONS_CASTLE_RED_ICE,              SpoilerCollectionCheck::RandomizerInf(RAND_INF_GANONS_CASTLE_MQ_WATER_TRIAL_SECOND_DOOR_RED_ICE_5));
    // clang-format on
}

static RegisterShipInitFunc initFunc_ShuffleRedIce(RegisterShuffleRedIce, { "IS_RANDO" });
static RegisterShipInitFunc registerRedIceLocations(Rando::StaticData::RegisterRedIceLocations);