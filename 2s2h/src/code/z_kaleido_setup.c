#include "z64pause_menu.h"

#include "z64.h"
#include "z64rumble.h"
#include "z64shrink_window.h"

#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include <string.h>

s16 sKaleidoSetupRightPageIndex[] = {
    PAUSE_MAP,   // PAUSE_ITEM
    PAUSE_QUEST, // PAUSE_MAP
    PAUSE_MASK,  // PAUSE_QUEST
    PAUSE_ITEM,  // PAUSE_MASK
};

f32 sKaleidoSetupRightPageEyeX[] = {
    PAUSE_EYE_DIST * -PAUSE_MAP_X,   // PAUSE_ITEM
    PAUSE_EYE_DIST * -PAUSE_QUEST_X, // PAUSE_MAP
    PAUSE_EYE_DIST * -PAUSE_MASK_X,  // PAUSE_QUEST
    PAUSE_EYE_DIST * -PAUSE_ITEM_X,  // PAUSE_MASK
};

f32 sKaleidoSetupRightPageEyeZ[] = {
    PAUSE_EYE_DIST * -PAUSE_MAP_Z,   // PAUSE_ITEM
    PAUSE_EYE_DIST * -PAUSE_QUEST_Z, // PAUSE_MAP
    PAUSE_EYE_DIST * -PAUSE_MASK_Z,  // PAUSE_QUEST
    PAUSE_EYE_DIST * -PAUSE_ITEM_Z,  // PAUSE_MASK
};

void func_800F4A10(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;

    Rumble_StateReset();

    pauseCtx->switchPageTimer = 0;
    pauseCtx->mainState = PAUSE_MAIN_STATE_SWITCHING_PAGE;

    // Set eye position and pageIndex such that scrolling left brings to the desired page
    pauseCtx->eye.x = sKaleidoSetupRightPageEyeX[pauseCtx->pageIndex];
    pauseCtx->eye.z = sKaleidoSetupRightPageEyeZ[pauseCtx->pageIndex];
    pauseCtx->pageIndex = sKaleidoSetupRightPageIndex[pauseCtx->pageIndex];
    pauseCtx->infoPanelOffsetY = -40;

    for (i = 0; i < ARRAY_COUNT(pauseCtx->worldMapPoints); i++) {
        pauseCtx->worldMapPoints[i] = false;
    }

    if (pauseCtx->state == PAUSE_STATE_OPENING_0) {
        for (i = 0; i < REGION_MAX; i++) {
            if ((gSaveContext.save.saveInfo.regionsVisited >> i) & 1) {
                pauseCtx->worldMapPoints[i] = true;
            }
        }
    } else {
        for (i = OWL_WARP_STONE_TOWER; i >= OWL_WARP_GREAT_BAY_COAST; i--) {
            if ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> i) & 1) {
                pauseCtx->worldMapPoints[i] = true;
                pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = i;
            }
        }

        if ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> 4) & 1) {
            pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 4;
        }
    }

    YREG(11) = -0x00C8;
    YREG(12) = -0x3840;
    YREG(13) = 0x2710;
    YREG(14) = 0x2710;
    YREG(15) = 0x2710;
    YREG(16) = -0x00BE;
    YREG(17) = -0x06D6;
    YREG(18) = -0x0B90;
    YREG(19) = 0x06E0;
    YREG(20) = 0;
    YREG(21) = -0x0622;
    YREG(22) = -0x0C44;
    YREG(23) = 0x0622;
    R_PAUSE_WORLD_MAP_YAW = -0x622;
    R_PAUSE_WORLD_MAP_Y_OFFSET = -90;
    R_PAUSE_WORLD_MAP_DEPTH = -14400;
}

void KaleidoSetup_Update(PlayState* play) {
    Input* input = CONTROLLER1(&play->state);
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;

    if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
        if (msgCtx && msgCtx) {}
    }

    if (IS_PAUSED(pauseCtx) || (play->gameOverCtx.state != GAMEOVER_INACTIVE)) {
        return;
    }

    if ((play->transitionTrigger != TRANS_TRIGGER_OFF) || (play->transitionMode != TRANS_MODE_OFF)) {
        return;
    }

    if ((gSaveContext.save.cutsceneIndex >= 0xFFF0) || (gSaveContext.nextCutsceneIndex >= 0xFFF0)) {
        return;
    }

    if (!Play_InCsMode(play) || ((msgCtx->msgMode != MSGMODE_NONE) && (msgCtx->currentTextId == 0xFF))) {
        if (play->bButtonAmmoPlusOne >= 2) {
            return;
        }

        if ((gSaveContext.magicState == MAGIC_STATE_STEP_CAPACITY) || (gSaveContext.magicState == MAGIC_STATE_FILL)) {
            return;
        }

        if (CHECK_EVENTINF(EVENTINF_17) || (player->stateFlags1 & PLAYER_STATE1_20)) {
            return;
        }

        if ((play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) ||
            (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON)) {
            return;
        }

        if (!play->actorCtx.isOverrideInputOn && CHECK_BTN_ALL(input->press.button, BTN_START)) {
            gSaveContext.prevHudVisibility = gSaveContext.hudVisibility;
            pauseCtx->itemDescriptionOn = false;
            pauseCtx->state = PAUSE_STATE_OPENING_0;
            func_800F4A10(play);
            // Set next page mode to scroll left
            pauseCtx->nextPageMode = pauseCtx->pageIndex * 2 + 1;
            Audio_SetPauseState(true);
        }

        if (pauseCtx->state == PAUSE_STATE_OPENING_0) {
            GameState_SetFramerateDivisor(&play->state, 2);
            if (ShrinkWindow_Letterbox_GetSizeTarget() != 0) {
                ShrinkWindow_Letterbox_SetSizeTarget(0);
            }
            Audio_PlaySfx_PauseMenuOpenOrClose(SFX_PAUSE_MENU_OPEN);
        }
    }
}

void KaleidoSetup_Init(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    s32 pad[2];

    memset(pauseCtx, 0, sizeof(PauseContext));

    pauseCtx->pageIndex = PAUSE_ITEM;

    pauseCtx->maskPageRoll = 160.0f;
    pauseCtx->questPageRoll = 160.0f;
    pauseCtx->mapPageRoll = 160.0f;
    pauseCtx->itemPageRoll = 160.0f;

    pauseCtx->eye.x = -64.0f;
    pauseCtx->unk_20C = 936.0f;
    pauseCtx->roll = -314.0f;

    pauseCtx->cursorPoint[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);

    pauseCtx->cursorSpecialPos = PAUSE_CURSOR_PAGE_RIGHT;
    pauseCtx->pageSwitchInputTimer = 0;

    pauseCtx->cursorItem[PAUSE_ITEM] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);
    pauseCtx->cursorItem[PAUSE_QUEST] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_MASK] = PAUSE_ITEM_NONE;

    pauseCtx->cursorSlot[PAUSE_ITEM] = 0;
    pauseCtx->cursorSlot[PAUSE_MAP] = R_PLAYER_FLOOR_REVERSE_INDEX + (DUNGEON_FLOOR_INDEX_4 - 1);

    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;
    pauseCtx->ocarinaSongIndex = -1;
    pauseCtx->equipAnimScale = 320;
    pauseCtx->equipAnimShrinkRate = 40;
    pauseCtx->promptAlpha = 100;

    View_Init(&pauseCtx->view, play->state.gfxCtx);
}

void KaleidoSetup_Destroy(PlayState* play) {
}
