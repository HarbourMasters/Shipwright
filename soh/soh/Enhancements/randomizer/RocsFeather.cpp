#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <soh_assets.h>

extern "C" {
#include <z64.h>
#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayState* gPlayState;
}

uint8_t rocsUseCount = 0;

void RegisterRocsFeather() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_ROCS_FEATHER);

    COND_HOOK(OnPlayerUpdate, shouldRegister, []() {
        Player* player = GET_PLAYER(gPlayState);
        // Reset Rocs count when touching the ground
        if (player->actor.bgCheckFlags & 1) {
            rocsUseCount = 0;
        }
    });

    COND_VB_SHOULD(VB_USE_ITEM, shouldRegister, {
        int32_t* usedItem = va_arg(args, int32_t*);
        Player* player = GET_PLAYER(gPlayState);

        // Roc's Feather behaviour
        if (*usedItem == ITEM_ROCS_FEATHER) {
            *should = false;

            if (!rocsUseCount) {
                rocsUseCount++;

                player->linearVelocity = 5.0f;

                //func_80838940(player, (LinkAnimationHeader*)&gPlayerAnim_link_fighter_backturn_jump, 5.8f, gPlayState, 0);
                func_80838940(player, (LinkAnimationHeader*)&gPlayerAnim_link_rocs_feather_jump, 5.8f, gPlayState, 0);

                player->actor.velocity.y = 8.0f;
                player->actor.world.rot.y = player->yaw = player->actor.shape.rot.y;

                Vec3f effectsPos = player->actor.home.pos;
                effectsPos.y += 3;
                f32 effectsScale = 1;
                if (!gSaveContext.linkAge) {
                    effectsScale = 1.5f;
                }
                EffectSsGRipple_Spawn(gPlayState, &effectsPos, 200 * effectsScale, 300 * effectsScale, 1);
                EffectSsGSplash_Spawn(gPlayState, &effectsPos, NULL, NULL, 0, 150 * effectsScale);

                player->stateFlags2 &= PLAYER_STATE2_HOPPING;

                Player_PlaySfx(&player->actor, NA_SE_PL_SKIP);
            }
        }
    });
}

static RegisterShipInitFunc registerRocsFeather(RegisterRocsFeather, { "IS_RANDO" });
