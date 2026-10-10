#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Cow/z_en_cow.h"
}

std::map<std::tuple<s16, s16, s16>, RandoCheckId> cowMap = {
    { { SCENE_REDEAD, 9, 0 }, RC_BENEATH_THE_WELL_COW },
    { { SCENE_F01, 0, 7 }, RC_ROMANI_RANCH_FIELD_COW_ENTRANCE },
    { { SCENE_F01, 0, 8 }, RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_BACK },
    { { SCENE_F01, 0, 9 }, RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_FRONT },
    { { SCENE_OMOYA, 0, 5 }, RC_ROMANI_RANCH_BARN_COW_MIDDLE },
    { { SCENE_OMOYA, 0, 6 }, RC_ROMANI_RANCH_BARN_COW_LEFT },
    { { SCENE_OMOYA, 0, 7 }, RC_ROMANI_RANCH_BARN_COW_RIGHT },
    { { SCENE_KAKUSIANA, 10, 3 }, RC_GREAT_BAY_COAST_COW_BACK },
    { { SCENE_KAKUSIANA, 10, 7 }, RC_GREAT_BAY_COAST_COW_FRONT },
};

void Rando::ActorBehavior::InitEnCowBehavior() {
    // Identify cow based on scene ID, room, and actor list index
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_COW, IS_RANDO, [](Actor* actor, bool* should) {
        RandoCheckId randoCheckId = RC_UNKNOWN;

        s16 actorListIndex = GetActorListIndex(actor);
        auto it = cowMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
        if (it != cowMap.end()) {
            randoCheckId = it->second;
            // Adjust for Termina Field cow grotto
            if (gPlayState->sceneId == SCENE_KAKUSIANA && gSaveContext.respawn[RESPAWN_MODE_UNK_3].data == 31) {
                randoCheckId = (RandoCheckId)(randoCheckId + (RC_TERMINA_FIELD_COW_BACK - RC_GREAT_BAY_COAST_COW_BACK));
            }

            if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
                return;
            }

            SetObjectRandoCheckId(actor, randoCheckId);

            switch (randoCheckId) {
                case RC_ROMANI_RANCH_BARN_COW_LEFT:
                    actor->world.pos.x = -470.0f;
                    actor->world.pos.y = 1.0f;
                    actor->world.pos.z = 53.0f;
                    actor->shape.rot.y = 7116.0f;
                    break;
                case RC_ROMANI_RANCH_BARN_COW_RIGHT:
                    actor->world.pos.x = -208.0f;
                    actor->world.pos.y = 0.0f;
                    actor->world.pos.z = 87.0f;
                    actor->shape.rot.y = -32768.0f;
                    break;
                case RC_GREAT_BAY_COAST_COW_FRONT:
                case RC_TERMINA_FIELD_COW_FRONT:
                    actor->world.pos.x = 2503.0f;
                    actor->world.pos.y = 0.0f;
                    actor->world.pos.z = 907.0f;
                    actor->shape.rot.y = -5085.0f;
                    break;
                case RC_GREAT_BAY_COAST_COW_BACK:
                case RC_TERMINA_FIELD_COW_BACK:
                    actor->world.pos.x = 2269.0f;
                    actor->world.pos.y = 0.0f;
                    actor->world.pos.z = 907.0f;
                    actor->shape.rot.y = 2950.0f;
                    break;
                default:
                    break;
            }
        }
    });

    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_COWS];

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_COW, shouldRegister, {
        // Original Should is the Range check, if it fails just Return.
        Actor* actor = va_arg(args, Actor*);
        if (!((actor->xzDistToPlayer < 90.0f) &&
              ABS_ALT(BINANG_SUB(actor->yawTowardsPlayer, actor->shape.rot.y)) < 25000)) {
            *should = false;
            return;
        }

        ((EnCow*)actor)->flags |= EN_COW_FLAG_WONT_GIVE_MILK;

        RandoCheckId randoCheckId = GetObjectRandoCheckId(actor);

        if (randoCheckId == RC_UNKNOWN) {
            *should = true;
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
            *should = true;
            return;
        }
        randoSaveCheck.eligible = true;
        *should = false;
    });

    COND_VB_SHOULD(VB_COW_CONSIDER_EPONAS_SONG_PLAYED, shouldRegister, {
        EnCow* actor = va_arg(args, EnCow*);
        if (actor->flags & EN_COW_FLAG_WONT_GIVE_MILK) {
            gPlayState->msgCtx.songPlayed = 0;
        }
        *should = gPlayState->msgCtx.songPlayed == OCARINA_SONG_EPONAS;
    });
}
