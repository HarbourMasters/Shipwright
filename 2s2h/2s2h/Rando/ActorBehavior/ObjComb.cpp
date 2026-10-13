#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Obj_Comb/z_obj_comb.h"
}

std::map<std::tuple<s16, s16, s16>, RandoCheckId> combMap = {
    // Termina Field
    { { SCENE_KAKUSIANA, 0, 0 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_BEEHIVE },
    { { SCENE_KAKUSIANA, 10, 2 }, RC_GREAT_BAY_COAST_COW_GROTTO_BEEHIVE },
    { { SCENE_KAKUSIANA, 11, 10 }, RC_TERMINA_FIELD_BIO_BABA_GROTTO_BEEHIVE_02 },

    // Mountain Village
    { { SCENE_10YUKIYAMANOMURA2, 0, 36 }, RC_MOUNTAIN_VILLAGE_SPRING_BEEHIVE },
    { { SCENE_10YUKIYAMANOMURA2, 0, 42 }, RC_MOUNTAIN_VILLAGE_SPRING_BEEHIVE },

    // Woodfall Temple
    { { SCENE_MITURIN, 3, 17 }, RC_WOODFALL_TEMPLE_MAZE_ROOM_BEEHIVE },

    // Pirates' Fortress
    { { SCENE_PIRATE, 3, 10 }, RC_PIRATE_FORTRESS_CAPTAIN_ROOM_UPPER_BEEHIVE },

    // Swamp Spider House
    { { SCENE_KINSTA1, 2, 9 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_BEEHIVE_01 },
    { { SCENE_KINSTA1, 2, 10 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_BEEHIVE_02 },
    { { SCENE_KINSTA1, 2, 11 }, RC_SWAMP_SPIDER_HOUSE_GOLD_ROOM_BEEHIVE_03 },
    { { SCENE_KINSTA1, 4, 7 }, RC_SWAMP_SPIDER_HOUSE_POT_ROOM_BEEHIVE_01 },
    { { SCENE_KINSTA1, 4, 8 }, RC_SWAMP_SPIDER_HOUSE_POT_ROOM_BEEHIVE_02 },
    { { SCENE_KINSTA1, 4, 11 }, RC_SWAMP_SPIDER_HOUSE_POT_ROOM_BEEHIVE_03 },
    { { SCENE_KINSTA1, 5, 6 }, RC_SWAMP_SPIDER_HOUSE_TREE_ROOM_BEEHIVE_01 },
    { { SCENE_KINSTA1, 5, 7 }, RC_SWAMP_SPIDER_HOUSE_TREE_ROOM_BEEHIVE_02 },
    { { SCENE_KINSTA1, 5, 9 }, RC_SWAMP_SPIDER_HOUSE_TREE_ROOM_BEEHIVE_03 },
};

static void SpawnBeehiveItem(Actor* actor, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
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
}

void Rando::ActorBehavior::InitObjCombBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_COMB, IS_RANDO, [](Actor* actor) {
        RandoCheckId randoCheckId = RC_UNKNOWN;

        s16 actorListIndex = GetActorListIndex(actor);
        auto it = combMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
        if (it != combMap.end()) {
            randoCheckId = it->second;

            // Room 10 is reused for both the Great Bay Coast and Termina Field cow grottos (same room, same
            // actor list); disambiguate with respawn data the same way EnCow.cpp/ObjGrass.cpp do.
            if (randoCheckId == RC_GREAT_BAY_COAST_COW_GROTTO_BEEHIVE &&
                gSaveContext.respawn[RESPAWN_MODE_UNK_3].data == 31) {
                randoCheckId = RC_TERMINA_FIELD_COW_GROTTO_BEEHIVE;
            }
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
    });

    COND_VB_SHOULD(VB_BEEHIVE_SPAWN_ACTOR, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(actor);

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        SpawnBeehiveItem(actor, randoCheckId);
    });

    // The Pirates' Fortress hive permanently Actor_Kill themselves on init once their
    // one-time vanilla event has already fired, and never spawn again afterward. If that happens before the
    // shuffled item was actually collected, it would be lost for the rest of the file. Spawn it here instead,
    // right at the hive's last position
    COND_VB_SHOULD(VB_BEEHIVE_ALREADY_BROKEN, IS_RANDO, {
        if (!(*should)) {
            return;
        }

        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = RC_UNKNOWN;

        s16 actorListIndex = GetActorListIndex(actor);
        auto it = combMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
        if (it != combMap.end()) {
            randoCheckId = it->second;

            if (randoCheckId == RC_GREAT_BAY_COAST_COW_GROTTO_BEEHIVE &&
                gSaveContext.respawn[RESPAWN_MODE_UNK_3].data == 31) {
                randoCheckId = RC_TERMINA_FIELD_COW_GROTTO_BEEHIVE;
            }
        }

        if (randoCheckId == RC_UNKNOWN || !RANDO_SAVE_CHECKS[randoCheckId].shuffled ||
            RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        SpawnBeehiveItem(actor, randoCheckId);
    });
}
