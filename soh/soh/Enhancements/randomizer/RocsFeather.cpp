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

#define MAX_ROCS_USES 1

static uint8_t rocsUseCount = 0;
static uint8_t groundTimer = 0;
static f32 effectsScale = 1.0f;

void RegisterRocsFeather() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_ROCS_FEATHER);

    COND_HOOK(OnPlayerUpdate, shouldRegister, []() {
        Player* player = GET_PLAYER(gPlayState);

        // Reset Rocs count when touching the ground for 3+ frames
        if (player->actor.bgCheckFlags & 1) {
            if (groundTimer <= 3) {
                groundTimer++;
            }
        } else {
            groundTimer = 0;
        }

        if (groundTimer >= 3) {
            rocsUseCount = 0;
        }
    });

    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, shouldRegister, {
        int32_t usedItem = va_arg(args, int32_t);

        // Roc's Feather behaviour
        if (usedItem == ITEM_ROCS_FEATHER) {
            *should = false;

            if (rocsUseCount < MAX_ROCS_USES) {
                rocsUseCount++;

                Player* player = GET_PLAYER(gPlayState);

                func_80838940(player, (LinkAnimationHeader*)&gPlayerAnim_link_rocs_feather_jump, 5.8f, gPlayState, 0);

                // Actionvar needed to prevent weird animation morph
                player->av2.actionVar2 = 1;

                // Move player forward on Roc's use
                player->linearVelocity = 5.0f;
                player->actor.world.rot.y = player->yaw = player->actor.shape.rot.y;

                if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                    player->actor.velocity.y = 7.0f;
                    effectsScale = 1.0f;
                } else {
                    player->actor.velocity.y = 7.5f;
                    effectsScale = 1.5f;
                }

                Vec3f effectsPos = player->actor.home.pos;
                effectsPos.y += 3;

                EffectSsGRipple_Spawn(gPlayState, &effectsPos, 200 * effectsScale, 300 * effectsScale, 1);
                EffectSsGSplash_Spawn(gPlayState, &effectsPos, NULL, NULL, 0, 150 * effectsScale);

                // Remove hopping state when using Roc's after sidehop/backflip to allow grabbing ledges again
                player->stateFlags2 &= ~(PLAYER_STATE2_HOPPING);

                Player_PlaySfx(&player->actor, NA_SE_PL_SKIP);
            }
        }
    });

    COND_VB_SHOULD(VB_DRAW_CUSTOM_ITEM_NAME, shouldRegister, {
        u32 namedItem = va_arg(args, u32);
        if (namedItem == ITEM_ROCS_FEATHER) {
            *should = true;
            const char* textureName = gRocsFeatherItemNameENGTex;

            if (gSaveContext.language == LANGUAGE_GER) {
                textureName = gRocsFeatherItemNameGERTex;
            } else if (gSaveContext.language == LANGUAGE_FRA) {
                textureName = gRocsFeatherItemNameFRATex;
            }

            memcpy(gPlayState->pauseCtx.nameSegment, textureName, strlen(textureName) + 1);
        }
    });
}

static RegisterShipInitFunc registerRocsFeather(RegisterRocsFeather, { "IS_RANDO" });
