#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
}

void ObjKibako_RandoDraw(Actor* actor, PlayState* play);
void ObjKibako2_RandoDraw(Actor* actor, PlayState* play);

std::map<std::tuple<s16, s16, s16>, RandoCheckId> crateMap = {
    // Clock Town //
    { { SCENE_ALLEY, 0, 7 }, RC_CLOCK_TOWN_LAUNDRY_SMALL_CRATE },
    { { SCENE_TOWN, 0, 11 }, RC_CLOCK_TOWN_EAST_SMALL_CRATE_01 },
    { { SCENE_TOWN, 0, 10 }, RC_CLOCK_TOWN_EAST_SMALL_CRATE_02 },

    // Great Bay Temple //
    { { SCENE_SEA, 5, 15 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_01 },
    { { SCENE_SEA, 5, 7 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_02 },
    { { SCENE_SEA, 5, 9 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_03 },
    { { SCENE_SEA, 5, 17 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_04 },
    { { SCENE_SEA, 5, 13 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_05 },
    { { SCENE_SEA, 5, 3 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_06 },
    { { SCENE_SEA, 5, 5 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_07 },
    { { SCENE_SEA, 5, 11 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_08 },
    { { SCENE_SEA, 5, 12 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_09 },
    { { SCENE_SEA, 5, 4 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_10 },
    { { SCENE_SEA, 5, 2 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_11 },
    { { SCENE_SEA, 5, 10 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_12 },
    { { SCENE_SEA, 5, 14 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_13 },
    { { SCENE_SEA, 5, 8 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_14 },
    { { SCENE_SEA, 5, 6 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_15 },
    { { SCENE_SEA, 5, 16 }, RC_GREAT_BAY_TEMPLE_GEKKO_SMALL_CRATE_16 },
    { { SCENE_SEA, 10, 28 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_01 },
    { { SCENE_SEA, 10, 27 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_02 },
    { { SCENE_SEA, 10, 26 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_03 },
    { { SCENE_SEA, 10, 22 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_04 },
    { { SCENE_SEA, 10, 24 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_05 },
    { { SCENE_SEA, 10, 25 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_06 },
    { { SCENE_SEA, 10, 23 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_LARGE_CRATE_07 },
    { { SCENE_SEA, 2, 12 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_01 },
    { { SCENE_SEA, 2, 14 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_02 },
    { { SCENE_SEA, 2, 13 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_03 },
    { { SCENE_SEA, 2, 11 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_04 },
    { { SCENE_SEA, 2, 10 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_LARGE_CRATE_05 },

    // Pirates Fortress //
    { { SCENE_KAIZOKU, 0, 27 }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_01 },
    { { SCENE_KAIZOKU, 0, 28 }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_02 },
    { { SCENE_KAIZOKU, 0, 29 }, RC_PIRATE_FORTRESS_PLAZA_LARGE_CRATE_03 },

    // Snowhead Temple //
    { { SCENE_HAKUGIN, 2, 17 }, RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM_LARGE_CRATE },
    { { SCENE_HAKUGIN, 8, 16 }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM_LARGE_CRATE_01 },
    { { SCENE_HAKUGIN, 8, 15 }, RC_SNOWHEAD_TEMPLE_DUAL_SWITCHES_ROOM_LARGE_CRATE_02 },
    { { SCENE_HAKUGIN, 9, 6 }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_01 },
    { { SCENE_HAKUGIN, 9, 9 }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_02 },
    { { SCENE_HAKUGIN, 9, 10 }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_03 },
    { { SCENE_HAKUGIN, 9, 8 }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_04 },
    { { SCENE_HAKUGIN, 9, 7 }, RC_SNOWHEAD_TEMPLE_MAP_ROOM_LARGE_CRATE_05 },

    // Stone Tower Temple //
    { { SCENE_INISIE_N, 0, 17 }, RC_STONE_TOWER_TEMPLE_ENTRANCE_SMALL_CRATE_01 },
    { { SCENE_INISIE_N, 0, 18 }, RC_STONE_TOWER_TEMPLE_ENTRANCE_SMALL_CRATE_02 },
    { { SCENE_INISIE_N, 7, 30 }, RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_LARGE_CRATE_01 },
    { { SCENE_INISIE_N, 7, 29 }, RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_LARGE_CRATE_02 },
    { { SCENE_INISIE_N, 1, 22 }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_01 },
    { { SCENE_INISIE_N, 1, 23 }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_02 },
    { { SCENE_INISIE_N, 1, 24 }, RC_STONE_TOWER_TEMPLE_CENTER_SMALL_CRATE_03 },
    { { SCENE_INISIE_N, 2, 28 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_01 },
    { { SCENE_INISIE_N, 2, 27 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_02 },
    { { SCENE_INISIE_N, 2, 26 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_03 },
    { { SCENE_INISIE_N, 2, 25 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_04 },
    { { SCENE_INISIE_N, 2, 24 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_LARGE_CRATE_05 },
    { { SCENE_INISIE_N, 2, 31 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_SMALL_CRATE_01 },
    { { SCENE_INISIE_N, 2, 30 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_SMALL_CRATE_02 },
    { { SCENE_INISIE_R, 0, 12 }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_01 },
    { { SCENE_INISIE_R, 0, 11 }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_02 },
    { { SCENE_INISIE_R, 0, 15 }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_03 },
    { { SCENE_INISIE_R, 0, 14 }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_04 },
    { { SCENE_INISIE_R, 0, 13 }, RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_SMALL_CRATE_05 },
    { { SCENE_INISIE_R, 3, 43 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_01 },
    { { SCENE_INISIE_R, 3, 44 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_02 },
    { { SCENE_INISIE_R, 3, 42 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_03 },
    { { SCENE_INISIE_R, 3, 39 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_04 },
    { { SCENE_INISIE_R, 3, 40 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_05 },
    { { SCENE_INISIE_R, 3, 41 }, RC_STONE_TOWER_TEMPLE_INVERTED_PATH_TO_GOMESS_SMALL_CRATE_06 },

    // Termina Field //
    { { SCENE_KAKUSIANA, 9, 0 }, RC_TERMINA_FIELD_SCRUB_LARGE_CRATE },

    // Gorman Track //
    { { SCENE_KOEPONARACE, 0, 73 }, RC_GORMAN_TRACK_LARGE_CRATE },

    // Romani Ranch //
    { { SCENE_F01, 0, 35 }, RC_ROMANI_RANCH_FIELD_LARGE_CRATE },

    // Cucco Shack //
    { { SCENE_F01C, 0, 22 }, RC_CUCCO_SHACK_LARGE_CRATE_01 },
    { { SCENE_F01C, 0, 15 }, RC_CUCCO_SHACK_LARGE_CRATE_02 },
    { { SCENE_F01C, 0, 14 }, RC_CUCCO_SHACK_LARGE_CRATE_03 },

    // Goron Village (winter and spring) //
    { { SCENE_11GORONNOSATO, 1, 3 }, RC_GORON_VILLAGE_LARGE_CRATE },
    { { SCENE_11GORONNOSATO2, 1, 3 }, RC_GORON_VILLAGE_LARGE_CRATE },

    // Swamp Spider House //
    { { SCENE_KINSTA1, 3, 8 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_01 },
    { { SCENE_KINSTA1, 3, 5 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_02 },
    { { SCENE_KINSTA1, 3, 6 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_03 },
    { { SCENE_KINSTA1, 3, 7 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_04 },
    { { SCENE_KINSTA1, 3, 9 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_05 },
    { { SCENE_KINSTA1, 3, 10 }, RC_SWAMP_SPIDER_HOUSE_MONUMENT_ROOM_LARGE_CRATE_06 },
    { { SCENE_KINSTA1, 2, 7 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_LARGE_CRATE_01 },
    { { SCENE_KINSTA1, 2, 8 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_UPPER_LARGE_CRATE_02 },
};

// Identify the crate RC by scene ID, room, and actor list index
void IdentifyCrate(Actor* actor) {
    RandoCheckId randoCheckId = RC_UNKNOWN;

    s16 actorListIndex = GetActorListIndex(actor);
    auto it = crateMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
    if (it != crateMap.end()) {
        randoCheckId = it->second;
    }

    if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        return;
    }

    Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
    if (!RANDO_SAVE_CHECKS[randoCheckId].obtained) {
        if (actor->id == ACTOR_OBJ_KIBAKO) {
            actor->draw = ObjKibako_RandoDraw;
        } else if (actor->id == ACTOR_OBJ_KIBAKO2) {
            actor->draw = ObjKibako2_RandoDraw;
        }
    }
}

void ObjKibako_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gSmallJunkCrateDL);
        return;
    }

    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallBossKeyCrateDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallHeartCrateDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMinorCrateDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMajorCrateDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallMaskCrateDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallTokenCrateDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallSmallKeyCrateDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallFairyCrateDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gSmallJunkCrateDL);
            break;
    }
}

void ObjKibako2_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
        return;
    }

    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeBossKeyCrateDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeHeartCrateDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMinorCrateDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMajorCrateDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeMaskCrateDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeTokenCrateDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeSmallKeyCrateDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeFairyCrateDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gLargeJunkCrateDL);
            break;
    }
}

void Rando::ActorBehavior::InitObjKibakoBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO, IS_RANDO, IdentifyCrate);

    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_KIBAKO2, IS_RANDO, IdentifyCrate);

    COND_VB_SHOULD(VB_CRATE_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        // Identify has already been called at this point, just check if we have a valid RC.
        if (Rando::ActorBehavior::GetObjectRandoCheckId(actor) != RC_UNKNOWN) {
            *should = false;
            actor->draw = ObjKibako_RandoDraw;
        }
    });

    COND_VB_SHOULD(VB_BARREL_OR_CRATE_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);

        if (actor->id != ACTOR_OBJ_KIBAKO && actor->id != ACTOR_OBJ_KIBAKO2 || randoCheckId == RC_UNKNOWN) {
            return;
        }

        *should = false;

        EnItem00* spawn = CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, randoCheckId,
            [](Actor* actor, PlayState* play) {
                auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
                switch (randoStaticCheck.flagType) {
                    case FLAG_NONE:
                        if (RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].shuffled) {
                            RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].eligible = true;
                        }
                        break;
                    case FLAG_CYCL_SCENE_COLLECTIBLE:
                        Flags_SetCollectible(play, randoStaticCheck.flag);
                        break;
                    default:
                        break;
                }
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                                (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
            });
    });
}
