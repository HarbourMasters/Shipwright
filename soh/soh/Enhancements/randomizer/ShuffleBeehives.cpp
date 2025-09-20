#include <soh/OTRGlobals.h>
#include "static_data.h"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "src/overlays/actors/ovl_Obj_Comb/z_obj_comb.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

void ObjComb_RandomizerChooseItemDrop(ObjComb* objComb, PlayState* play) {
    s16 params = objComb->actor.params & 0x1F;
    const auto beehiveIdentity = ObjectExtension::GetInstance().Get<BeehiveIdentity>(&objComb->actor);

    if (RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) && beehiveIdentity != nullptr &&
        !Flags_GetRandomizerInf(beehiveIdentity->randomizerInf)) {
        EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &objComb->actor.world.pos, ITEM00_SOH_DUMMY);
        item00->randoInf = beehiveIdentity->randomizerInf;
        item00->itemEntry =
            OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(beehiveIdentity->randomizerCheck, GI_NONE);
        item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
        return;
    }

    if ((params > 0) || (params < 0x1A)) {
        if (params == 6) {
            if (Flags_GetCollectible(play, (objComb->actor.params >> 8) & 0x3F)) {
                params = -1;
            } else {
                params = (params | (((objComb->actor.params >> 8) & 0x3F) << 8));
            }
        } else if (Rand_ZeroOne() < 0.5f) {
            params = -1;
        }
        if (params >= 0 && !CVarGetInteger(CVAR_ENHANCEMENT("NoRandomDrops"), 0)) {
            Item_DropCollectible(play, &objComb->actor.world.pos, params);
        }
    }
}

void ObjComb_RandomizerWait(ObjComb* objComb, PlayState* play) {
    objComb->unk_1B0 -= 50;

    const auto beehiveIdentity = ObjectExtension::GetInstance().Get<BeehiveIdentity>(&objComb->actor);
    if (RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) && beehiveIdentity == nullptr &&
        !Flags_GetRandomizerInf(beehiveIdentity->randomizerInf)) {
        if (objComb->unk_1B0 <= -5000) {
            objComb->unk_1B0 = 1500;
        }
    } else if (objComb->unk_1B0 < 0) {
        objComb->unk_1B0 = 0;
    }

    if ((objComb->collider.base.acFlags & AC_HIT) != 0) {
        objComb->collider.base.acFlags &= ~AC_HIT;
        s32 dmgFlags = objComb->collider.elements[0].info.acHitInfo->toucher.dmgFlags;
        if (dmgFlags & 0x4001F866) {
            objComb->unk_1B0 = 1500;
        } else {
            ObjComb_Break(objComb, play);
            ObjComb_RandomizerChooseItemDrop(objComb, play);
            Actor_Kill(&objComb->actor);
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &objComb->collider.base);
    }

    if (objComb->actor.update != NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &objComb->collider.base);
    }
}

void ObjComb_RandomizerInit(void* actor) {
    ObjComb* objComb = static_cast<ObjComb*>(actor);
    s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
    auto beehiveIdentity = OTRGlobals::Instance->gRandomizer->IdentifyBeehive(
        gPlayState->sceneNum, (s16)objComb->actor.world.pos.x, respawnData);
    ObjectExtension::GetInstance().Set<BeehiveIdentity>(actor, std::move(beehiveIdentity));
    objComb->actionFunc = (ObjCombActionFunc)ObjComb_RandomizerWait;
}

void ObjComb_RandomizerUpdate(void* actor) {
    ObjComb* combActor = reinterpret_cast<ObjComb*>(actor);
    combActor->actor.shape.rot.x =
        static_cast<int16_t>(Math_SinS(combActor->unk_1B2)) * CLAMP_MIN(combActor->unk_1B0, 0) +
        combActor->actor.home.rot.x;
}

void RegisterShuffleBeehives() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES);

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_COMB, shouldRegister, ObjComb_RandomizerInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_COMB, shouldRegister, ObjComb_RandomizerUpdate);
}

static RegisterShipInitFunc initFunc(RegisterShuffleBeehives, { "IS_RANDO" });

void Rando::StaticData::RegisterBeehiveLocations() {
    static bool registered = false;
    if (registered)
        return;
    registered = true;
    // clang-format off
    locationTable[RC_KF_STORMS_GROTTO_BEEHIVE_LEFT] =          Location::Base(RC_KF_STORMS_GROTTO_BEEHIVE_LEFT,                                RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_KOKIRI_FOREST,                ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x2C),       "Storms Grotto Beehive Left",                  RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_KF_STORMS_GROTTO_LEFT));
    locationTable[RC_KF_STORMS_GROTTO_BEEHIVE_RIGHT] =         Location::Base(RC_KF_STORMS_GROTTO_BEEHIVE_RIGHT,                               RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_KOKIRI_FOREST,                ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x2C),       "Storms Grotto Beehive Right",                 RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_KF_STORMS_GROTTO_RIGHT));
    locationTable[RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_LEFT] =  Location::Base(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_LEFT,                        RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_LOST_WOODS,                   ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x14),       "Tunnel Grotto Beehive Left",                  RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_LW_NEAR_SHORTCUTS_GROTTO_LEFT));
    locationTable[RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_RIGHT] = Location::Base(RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_RIGHT,                       RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_LOST_WOODS,                   ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x14),       "Tunnel Grotto Beehive Right",                 RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_LW_NEAR_SHORTCUTS_GROTTO_RIGHT));
    locationTable[RC_LW_DEKU_SCRUB_GROTTO_BEEHIVE] =           Location::Base(RC_LW_DEKU_SCRUB_GROTTO_BEEHIVE,                                 RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_LOST_WOODS,                   ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(747,  0xF5),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_PAIR_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_LW_DEKU_SCRUB_GROTTO));
    locationTable[RC_SFM_STORMS_GROTTO_BEEHIVE] =              Location::Base(RC_SFM_STORMS_GROTTO_BEEHIVE,                                    RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_SACRED_FOREST_MEADOW,         ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(2262, 0xEE),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_PAIR_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_SFM_STORMS_GROTTO));
    locationTable[RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_LEFT] =     Location::Base(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_LEFT,                           RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x00),       "Near Market Grotto Beehive Left",             RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_NEAR_MARKET_GROTTO_LEFT));
    locationTable[RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_RIGHT] =    Location::Base(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_RIGHT,                          RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x00),       "Near Market Grotto Beehive Right",            RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_NEAR_MARKET_GROTTO_RIGHT));
    locationTable[RC_HF_OPEN_GROTTO_BEEHIVE_LEFT] =            Location::Base(RC_HF_OPEN_GROTTO_BEEHIVE_LEFT,                                  RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x03),       "Open Grotto Beehive Left",                    RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_OPEN_GROTTO_LEFT));
    locationTable[RC_HF_OPEN_GROTTO_BEEHIVE_RIGHT] =           Location::Base(RC_HF_OPEN_GROTTO_BEEHIVE_RIGHT,                                 RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x03),       "Open Grotto Beehive Right",                   RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_OPEN_GROTTO_RIGHT));
    locationTable[RC_HF_SOUTHEAST_GROTTO_BEEHIVE_LEFT] =       Location::Base(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_LEFT,                             RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x22),       "Southeast Grotto Beehive Left",               RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_SOUTHEAST_GROTTO_LEFT));
    locationTable[RC_HF_SOUTHEAST_GROTTO_BEEHIVE_RIGHT] =      Location::Base(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_RIGHT,                            RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x22),       "Southeast Grotto Beehive Right",              RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_SOUTHEAST_GROTTO_RIGHT));
    locationTable[RC_HF_INSIDE_FENCE_GROTTO_BEEHIVE] =         Location::Base(RC_HF_INSIDE_FENCE_GROTTO_BEEHIVE,                               RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_HYRULE_FIELD,                 ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(1410, 0xE6),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_LONELY_SCRUB_GROTTO,                                 RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_HF_INSIDE_FENCE_GROTTO));
    locationTable[RC_LLR_GROTTO_BEEHIVE] =                     Location::Base(RC_LLR_GROTTO_BEEHIVE,                                           RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_LON_LON_RANCH,                ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(5144, 0xFC),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_TRIO_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_LLR_GROTTO));
    locationTable[RC_KAK_OPEN_GROTTO_BEEHIVE_LEFT] =           Location::Base(RC_KAK_OPEN_GROTTO_BEEHIVE_LEFT,                                 RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_KAKARIKO_VILLAGE,             ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x28),       "Open Grotto Beehive Left",                    RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_KAK_OPEN_GROTTO_LEFT));
    locationTable[RC_KAK_OPEN_GROTTO_BEEHIVE_RIGHT] =          Location::Base(RC_KAK_OPEN_GROTTO_BEEHIVE_RIGHT,                                RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_KAKARIKO_VILLAGE,             ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x28),       "Open Grotto Beehive Right",                   RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_KAK_OPEN_GROTTO_RIGHT));
    locationTable[RC_DMT_COW_GROTTO_BEEHIVE] =                 Location::Base(RC_DMT_COW_GROTTO_BEEHIVE,                                       RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_TRAIL,         ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(2617, 0xF8),       "Cow Grotto Beehive",                          RHT_BEEHIVE_COW_GROTTO,                                          RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMT_COW_GROTTO));
    locationTable[RC_DMT_STORMS_GROTTO_BEEHIVE_LEFT] =         Location::Base(RC_DMT_STORMS_GROTTO_BEEHIVE_LEFT,                               RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_TRAIL,         ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x57),       "Storms Grotto Beehive Left",                  RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMT_STORMS_GROTTO_LEFT));
    locationTable[RC_DMT_STORMS_GROTTO_BEEHIVE_RIGHT] =        Location::Base(RC_DMT_STORMS_GROTTO_BEEHIVE_RIGHT,                              RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_TRAIL,         ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x57),       "Storms Grotto Beehive Right",                 RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMT_STORMS_GROTTO_RIGHT));
    locationTable[RC_GC_GROTTO_BEEHIVE] =                      Location::Base(RC_GC_GROTTO_BEEHIVE,                                            RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_GORON_CITY,                   ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(5144, 0xFB),       "Grotto Beehive",                              RHT_BEEHIVE_SCRUB_TRIO_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_GC_GROTTO));
    locationTable[RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT] =          Location::Base(RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT,                                RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_CRATER,        ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x7A),       "Upper Grotto Beehive Left",                   RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMC_UPPER_GROTTO_LEFT));
    locationTable[RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT] =         Location::Base(RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT,                               RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_CRATER,        ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x7A),       "Upper Grotto Beehive Right",                  RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMC_UPPER_GROTTO_RIGHT));
    locationTable[RC_DMC_HAMMER_GROTTO_BEEHIVE] =              Location::Base(RC_DMC_HAMMER_GROTTO_BEEHIVE,                                    RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DEATH_MOUNTAIN_CRATER,        ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(5144, 0xF9),       "Hammer Grotto Beehive",                       RHT_BEEHIVE_SCRUB_TRIO_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_DMC_HAMMER_GROTTO));
    locationTable[RC_ZR_OPEN_GROTTO_BEEHIVE_LEFT] =            Location::Base(RC_ZR_OPEN_GROTTO_BEEHIVE_LEFT,                                  RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_ZORAS_RIVER,                  ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(-144, 0x29),       "Open Grotto Beehive Left",                    RHT_BEEHIVE_CHEST_GROTTO,                                        RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZR_OPEN_GROTTO_LEFT));
    locationTable[RC_ZR_OPEN_GROTTO_BEEHIVE_RIGHT] =           Location::Base(RC_ZR_OPEN_GROTTO_BEEHIVE_RIGHT,                                 RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_ZORAS_RIVER,                  ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(121,  0x29),       "Open Grotto Beehive Right",                   RHT_BEEHIVE_CHEST_GROTTO,                                        RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZR_OPEN_GROTTO_RIGHT));
    locationTable[RC_ZR_STORMS_GROTTO_BEEHIVE] =               Location::Base(RC_ZR_STORMS_GROTTO_BEEHIVE,                                     RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_ZORAS_RIVER,                  ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(2262, 0xEB),       "Storms Grotto Beehive",                       RHT_BEEHIVE_SCRUB_PAIR_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZR_STORMS_GROTTO));
    locationTable[RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_LEFT] =  Location::Base(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_LEFT,                        RCQUEST_BOTH,    RCTYPE_BEEHIVE,                                                          ACTOR_OBJ_COMB,       SCENE_ZORAS_DOMAIN,                 TWO_ACTOR_PARAMS(382,  0x00),       "In Front of King Zora Beehive Left",          RHT_BEEHIVE_IN_FRONT_OF_KING_ZORA,                               RG_BLUE_RUPEE,                                          SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZD_IN_FRONT_OF_KING_ZORA_LEFT));
    locationTable[RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_RIGHT] = Location::Base(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_RIGHT,                       RCQUEST_BOTH,    RCTYPE_BEEHIVE,                                                          ACTOR_OBJ_COMB,       SCENE_ZORAS_DOMAIN,                 TWO_ACTOR_PARAMS(948,  0x00),       "In Front of King Zora Beehive Right",         RHT_BEEHIVE_IN_FRONT_OF_KING_ZORA,                               RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZD_IN_FRONT_OF_KING_ZORA_RIGHT));
    locationTable[RC_ZD_BEHIND_KING_ZORA_BEEHIVE] =            Location::Base(RC_ZD_BEHIND_KING_ZORA_BEEHIVE,                                  RCQUEST_BOTH,    RCTYPE_BEEHIVE,                                                          ACTOR_OBJ_COMB,       SCENE_ZORAS_DOMAIN,                 TWO_ACTOR_PARAMS(701,  0x00),       "Behind King Zora Beehive",                    RHT_BEEHIVE_BEHIND_KING_ZORA,                                    RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_ZD_BEHIND_KING_ZORA));
    locationTable[RC_LH_GROTTO_BEEHIVE] =                      Location::Base(RC_LH_GROTTO_BEEHIVE,                                            RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_LAKE_HYLIA,                   ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(5144, 0xEF),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_TRIO_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_LH_GROTTO));
    locationTable[RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE] =           Location::Base(RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE,                                 RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_GERUDO_VALLEY,                ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(2262, 0xF0),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_PAIR_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_GV_DEKU_SCRUB_GROTTO));
    locationTable[RC_COLOSSUS_GROTTO_BEEHIVE] =                Location::Base(RC_COLOSSUS_GROTTO_BEEHIVE,                                      RCQUEST_BOTH,    RCTYPE_BEEHIVE,                     RCAREA_DESERT_COLOSSUS,              ACTOR_OBJ_COMB,       SCENE_GROTTOS,                      TWO_ACTOR_PARAMS(2262, 0xFD),       "Deku Scrub Grotto Beehive",                   RHT_BEEHIVE_SCRUB_PAIR_GROTTO,                                   RG_RED_RUPEE,                                           SpoilerCollectionCheck::RandomizerInf(RAND_INF_BEEHIVE_COLOSSUS_GROTTO));
    // clang-format-on
}

static ObjectExtension::Register<BeehiveIdentity> RegisterBeehiveIdentity;
static RegisterShipInitFunc registerFunc(Rando::StaticData::RegisterBeehiveLocations);
