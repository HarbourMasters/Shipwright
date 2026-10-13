#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_Obj_Taru/z_obj_taru.h"
}

std::map<std::tuple<s16, s16, s16>, RandoCheckId> barrelMap = {
    // Great Bay Temple //
    { { SCENE_SEA, 13, 0 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_01 },
    { { SCENE_SEA, 13, 1 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_02 },
    { { SCENE_SEA, 13, 2 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_03 },
    { { SCENE_SEA, 13, 3 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_04 },
    { { SCENE_SEA, 13, 4 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_05 },
    { { SCENE_SEA, 13, 5 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_06 },
    { { SCENE_SEA, 13, 6 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_07 },
    { { SCENE_SEA, 13, 7 }, RC_GREAT_BAY_TEMPLE_ENTRANCE_BARREL_08 },
    { { SCENE_SEA, 12, 11 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_01 },
    { { SCENE_SEA, 12, 12 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_02 },
    { { SCENE_SEA, 12, 10 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_BARREL_03 },
    { { SCENE_SEA, 9, 7 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_BARREL_01 },
    { { SCENE_SEA, 9, 6 }, RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_BARREL_02 },
    { { SCENE_SEA, 2, 2 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_01 },
    { { SCENE_SEA, 2, 5 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_02 },
    { { SCENE_SEA, 2, 6 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_03 },
    { { SCENE_SEA, 2, 4 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_04 },
    { { SCENE_SEA, 2, 3 }, RC_GREAT_BAY_TEMPLE_RED_PIPE_SWITCH_ROOM_BARREL_05 },

    // Pirates Fortress //
    { { SCENE_PIRATE, 3, 7 }, RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_01 },
    { { SCENE_PIRATE, 3, 6 }, RC_PIRATE_FORTRESS_CAPTAIN_ROOM_BARREL_02 },
    { { SCENE_TORIDE, 0, 7 }, RC_PIRATE_FORTRESS_ENTRANCE_BARREL },
    { { SCENE_PIRATE, 13, 1 }, RC_PIRATE_FORTRESS_INTERIOR_GUARDED_BARREL },
    { { SCENE_KAIZOKU, 0, 8 }, RC_PIRATE_FORTRESS_PLAZA_BARREL },
    { { SCENE_PIRATE, 9, 8 }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_01 },
    { { SCENE_PIRATE, 9, 9 }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_02 },
    { { SCENE_PIRATE, 9, 7 }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_03 },
    { { SCENE_PIRATE, 9, 6 }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_04 },
    { { SCENE_PIRATE, 9, 5 }, RC_PIRATE_FORTRESS_SEWERS_END_BARREL_05 },
    { { SCENE_PIRATE, 11, 21 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_01 },
    { { SCENE_PIRATE, 11, 20 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_02 },
    { { SCENE_PIRATE, 11, 19 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_03 },
    { { SCENE_PIRATE, 11, 18 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_04 },
    { { SCENE_PIRATE, 11, 17 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_05 },
    { { SCENE_PIRATE, 11, 16 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_06 },
    { { SCENE_PIRATE, 11, 12 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_07 },
    { { SCENE_PIRATE, 11, 10 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_08 },
    { { SCENE_PIRATE, 11, 9 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_09 },
    { { SCENE_PIRATE, 11, 15 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_10 },
    { { SCENE_PIRATE, 11, 7 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_11 },
    { { SCENE_PIRATE, 11, 6 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_12 },
    { { SCENE_PIRATE, 11, 11 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_13 },
    { { SCENE_PIRATE, 11, 14 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_14 },
    { { SCENE_PIRATE, 11, 8 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_15 },
    { { SCENE_PIRATE, 11, 13 }, RC_PIRATE_FORTRESS_SEWERS_HEART_PIECE_ROOM_BARREL_16 },
};

void ObjTaru_RandoDraw(Actor* actor, PlayState* play) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        Gfx_DrawDListOpa(play, (Gfx*)gBarrelJunkDL);
        return;
    }

    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelBossKeyDL);
            break;
        case RITYPE_HEALTH:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelHeartDL);
            break;
        case RITYPE_LESSER:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelMinorDL);
            break;
        case RITYPE_MAJOR:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelMajorDL);
            break;
        case RITYPE_MASK:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelMaskDL);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelTokenDL);
            break;
        case RITYPE_SMALL_KEY:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelSmallKeyDL);
            break;
        case RITYPE_STRAY_FAIRY:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelFairyDL);
            break;
        default:
            Gfx_DrawDListOpa(play, (Gfx*)gBarrelJunkDL);
            break;
    }
}

void Rando::ActorBehavior::InitObjTaruBehavior() {
    // Identify the barrel based on scene ID, room, and actor list index
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TARU, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;

        s16 actorListIndex = GetActorListIndex(actor);
        auto it = barrelMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
        if (it != barrelMap.end()) {
            randoCheckId = it->second;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
        if (!RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            actor->draw = ObjTaru_RandoDraw;
        }
    });

    COND_VB_SHOULD(VB_BARREL_OR_CRATE_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(actor);

        if (actor->id != ACTOR_OBJ_TARU || randoCheckId == RC_UNKNOWN) {
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
