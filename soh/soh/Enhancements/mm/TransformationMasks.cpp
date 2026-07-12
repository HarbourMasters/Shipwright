#include "PlayerFormSystem.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

extern PlayState* gPlayState;
}

// Transformation Masks: equipping the Goron or Zora mask transforms Link, Majora's Mask style.
// Phase 1 scope: full model/skeleton swap with MM idle animation, per-form heights/collision/mass,
// pause menu preview, and revert on unequip. Abilities (Goron roll, Zora swim, ...) are later phases.
// The form swap itself lives in PlayerFormSystem.cpp, draw substitutions in PlayerFormDraw.cpp.

#define TRANSFORM_FLASH_FRAMES 12

static s32 sFlashTimer = 0;

static void StartTransformFlash(PlayState* play) {
    sFlashTimer = TRANSFORM_FLASH_FRAMES;
    Sfx_PlaySfxCentered(NA_SE_SY_WHITE_OUT_S);
}

static void UpdateTransformFlash(PlayState* play) {
    if (sFlashTimer <= 0) {
        return;
    }
    sFlashTimer--;

    play->envCtx.fillScreen = sFlashTimer > 0;
    play->envCtx.screenFillColor[0] = 255;
    play->envCtx.screenFillColor[1] = 255;
    play->envCtx.screenFillColor[2] = 255;
    play->envCtx.screenFillColor[3] = (u8)(220 * sFlashTimer / TRANSFORM_FLASH_FRAMES);
}

static void RegisterTransformationMasks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        PlayState* play = gPlayState;
        Player* player = GET_PLAYER(play);

        // No transforming on horseback or while holding the fishing pole; force the mask off
        if ((player->currentMask == PLAYER_MASK_GORON || player->currentMask == PLAYER_MASK_ZORA) &&
            ((player->stateFlags1 & PLAYER_STATE1_ON_HORSE) || player->heldItemAction == PLAYER_IA_FISHING_POLE)) {
            player->currentMask = PLAYER_MASK_NONE;
            gSaveContext.ship.maskMemory = PLAYER_MASK_NONE;
        }

        PlayerShipForm target = PlayerForm_GetTarget();
        if (target != PlayerForm_GetApplied()) {
            PlayerForm_Apply(play, player, target);
            StartTransformFlash(play);
        }

        PlayerForm_EnforceInvariants(player);
        UpdateTransformFlash(play);
    });
}

static RegisterShipInitFunc initFunc(RegisterTransformationMasks);
