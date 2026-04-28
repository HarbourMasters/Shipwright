#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"
#include <soh_assets.h>

extern "C" {
#include <z64.h>
#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayState* gPlayState;
extern s32 Player_GetItemOnButton(PlayState*, s32);
}

#define MAX_ROCS_USES 1

static uint8_t rocsUseCount = 0;
static uint8_t groundTimer = 0;
static uint8_t waterTimer = 0;
static f32 effectsScale = 1.0f;

static const u16 sItemButtons[8] = {
    BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT,
};

static void DoRocsFeatherJump(Player* player) {
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

void RegisterRocsFeather() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_ROCS_FEATHER);

    // Reset rocsUseCount after Link spends 3+ frames grounded or floating on the water surface.
    COND_HOOK(OnPlayerUpdate, shouldRegister, []() {
        Player* player = GET_PLAYER(gPlayState);

        if (player->actor.bgCheckFlags & 1) {
            if (groundTimer <= 3) {
                groundTimer++;
            }
        } else {
            groundTimer = 0;
        }

        bool onSurface = (player->stateFlags1 & PLAYER_STATE1_IN_WATER) &&
                         !(player->stateFlags2 & (PLAYER_STATE2_UNDERWATER | PLAYER_STATE2_DIVING));
        if (onSurface) {
            if (waterTimer <= 3) {
                waterTimer++;
            }
        } else {
            waterTimer = 0;
        }

        if (groundTimer >= 3 || waterTimer >= 3) {
            rocsUseCount = 0;
        }
    });

    // Surface-water jump
    // Separate because VB_CHANGE_HELD_ITEM_AND_USE_ITEM never fires while swimming
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_ACTION_FUNC, shouldRegister, {
        if (!*should)
            return;

        Player* player = va_arg(args, Player*);
        if (player->actor.id != ACTOR_PLAYER)
            return;
        if (!(player->stateFlags1 & PLAYER_STATE1_IN_WATER))
            return;
        if (player->stateFlags2 & (PLAYER_STATE2_UNDERWATER | PLAYER_STATE2_DIVING))
            return;

        Input* input = va_arg(args, Input*);

        *should = false;
        player->actionFunc(player, gPlayState);

        if (rocsUseCount >= MAX_ROCS_USES)
            return;

        for (size_t i = 1; i < ARRAY_COUNT(sItemButtons); i++) {
            if (Player_GetItemOnButton(gPlayState, i) == ITEM_ROCS_FEATHER &&
                CHECK_BTN_ALL(input->press.button, sItemButtons[i])) {
                rocsUseCount++;
                DoRocsFeatherJump(player);
                break;
            }
        }
    });

    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, shouldRegister, {
        int32_t usedItem = va_arg(args, int32_t);

        // Roc's Feather behaviour
        if (usedItem == ITEM_ROCS_FEATHER) {
            *should = false;

            if (rocsUseCount < MAX_ROCS_USES) {
                rocsUseCount++;
                DoRocsFeatherJump(GET_PLAYER(gPlayState));
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
