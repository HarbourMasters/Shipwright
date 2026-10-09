#include "ActorBehavior.h"
#include "2s2h/Enhancements/Cutscenes/StoryCutscenes/SkipGiantsChamber.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "overlays/actors/ovl_Dm_Hina/z_dm_hina.h"
void func_808B9524(DoorWarp1* doorWarp1, PlayState* play);
}

void Rando::ActorBehavior::InitDoorWarp1VBehavior() {
    /*
     * This actor normally checks for the boss remains flags for multiple things (spawning the item, warping animation,
     * whether to go to the Giants' Chamber). In rando, use rando checks instead
     */
    COND_VB_SHOULD(VB_SPAWN_BOSS_REMAINS, IS_RANDO, {
        s32* ret = va_arg(args, s32*);
        if ((gPlayState->sceneId == SCENE_MITURIN_BS) &&
            !RANDO_SAVE_CHECKS[RC_WOODFALL_TEMPLE_BOSS_WARP].cycleObtained) {
            // Odolwa's Lair
            *ret = 1;
        } else if ((gPlayState->sceneId == SCENE_HAKUGIN_BS) &&
                   !RANDO_SAVE_CHECKS[RC_SNOWHEAD_TEMPLE_BOSS_WARP].cycleObtained) {
            // Goht's Lair
            *ret = 2;
        } else if ((gPlayState->sceneId == SCENE_SEA_BS) &&
                   !RANDO_SAVE_CHECKS[RC_GREAT_BAY_TEMPLE_BOSS_WARP].cycleObtained) {
            // Gyorg's Lair
            *ret = 3;
        } else if ((gPlayState->sceneId == SCENE_INISIE_BS) &&
                   !RANDO_SAVE_CHECKS[RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP].cycleObtained) {
            // Twinmold's Lair
            *ret = 4;
        }
        *should = false;
    });

    /*
     * This should only be reached if the check has not already been activated. On repeats, there is no item, and the
     * crystal warp animation plays out instead.
     */
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        DoorWarp1* doorWarp1 = (DoorWarp1*)actor;
        Player* player = GET_PLAYER(gPlayState);
        if (actor->id == ACTOR_DOOR_WARP1) {
            RandoCheckId checkId;
            switch (gPlayState->sceneId) {
                case SCENE_MITURIN_BS: // Odolwa's Lair
                    checkId = RC_WOODFALL_TEMPLE_BOSS_WARP;
                    break;
                case SCENE_HAKUGIN_BS: // Goht's Lair
                    checkId = RC_SNOWHEAD_TEMPLE_BOSS_WARP;
                    break;
                case SCENE_SEA_BS: // Gyorg's Lair
                    checkId = RC_GREAT_BAY_TEMPLE_BOSS_WARP;
                    break;
                case SCENE_INISIE_BS: // Twinmold's Lair
                    checkId = RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP;
                    break;
            }
            RANDO_SAVE_CHECKS[checkId].eligible = !RANDO_SAVE_CHECKS[checkId].cycleObtained;
            // Transform back to human Link and warp away without waiting for a textbox to close as normal
            Player_SetCsActionWithHaltedActors(gPlayState, &doorWarp1->dyna.actor, PLAYER_CSACTION_9);
            player->unk_3A0.x = doorWarp1->dyna.actor.world.pos.x;
            player->unk_3A0.z = doorWarp1->dyna.actor.world.pos.z;
            doorWarp1->unk_1CA = 1;
            doorWarp1->actionFunc = func_808B9524;
            // Now that we have reached this boss remains check, stop drawing it.
            if (doorWarp1->unk_1A0 != nullptr) {
                ((DmHina*)doorWarp1->unk_1A0)->isDrawn = false;
            }
            HandleGiantsCutsceneSkip();
            *should = false;
        }
    });

    /*
     * Changes the requirements to activate the boss room warp pad shortcut to check for boss room blue warp used
     */
    COND_VB_SHOULD(VB_ACTIVATE_BOSS_WARP_PAD, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId checkId = RC_UNKNOWN;

        switch (DOORWARP1_GET_FF(actor)) {
            case ENDOORWARP1_FF_2:
                checkId = RC_WOODFALL_TEMPLE_BOSS_WARP;
                break;
            case ENDOORWARP1_FF_3:
                checkId = RC_SNOWHEAD_TEMPLE_BOSS_WARP;
                break;
            case ENDOORWARP1_FF_4:
                checkId = RC_GREAT_BAY_TEMPLE_BOSS_WARP;
                break;
            case ENDOORWARP1_FF_5:
                checkId = RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_WARP;
                break;
        }

        *should = RANDO_SAVE_CHECKS[checkId].obtained;
    });
}
