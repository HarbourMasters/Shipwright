#include "global.h"

s16 sKaleidoSetupKscpPos0[] = { PAUSE_QUEST, PAUSE_EQUIP, PAUSE_ITEM, PAUSE_MAP };
f32 sKaleidoSetupEyeX0[] = { 0.0f, 64.0f, 0.0f, -64.0f };
f32 sKaleidoSetupEyeZ0[] = { -64.0f, 0.0f, 64.0f, 0.0f };

s16 sKaleidoSetupKscpPos1[] = { PAUSE_MAP, PAUSE_QUEST, PAUSE_EQUIP, PAUSE_ITEM };
f32 sKaleidoSetupEyeX1[] = { -64.0f, 0.0f, 64.0f, 0.0f };
f32 sKaleidoSetupEyeZ1[] = { 0.0f, -64.0f, 0.0f, 64.0f };

void KaleidoSetup_Update(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];

    if (pauseCtx->state == 0 && pauseCtx->debugState == 0 && play->gameOverCtx.state == GAMEOVER_INACTIVE &&
        play->sceneLoadFlag == 0 && play->transitionMode == 0 && gSaveContext.cutsceneIndex < 0xFFF0 &&
        gSaveContext.nextCutsceneIndex < 0xFFF0 && !Play_InCsMode(play) &&
        play->shootingGalleryStatus <= 1 && gSaveContext.magicState != 8 && gSaveContext.magicState != 9 &&
        (play->sceneNum != SCENE_BOWLING || !Flags_GetSwitch(play, 0x38))) {

        if (CVar_GetS32("gCheatEasyPauseBufferFrameAdvance", 0) == 2 && !CHECK_BTN_ALL(input->press.button, BTN_START)) {
            CVar_SetS32("gCheatEasyPauseBufferFrameAdvance", 0);
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_L) && CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
            if (BREG(0)) {
                pauseCtx->debugState = 3;
            }
        } else if ((CHECK_BTN_ALL(input->press.button, BTN_START) && !CVar_GetS32("gCheatEasyPauseBufferFrameAdvance", 0)) || CVar_GetS32("gCheatEasyPauseBufferFrameAdvance", 0) == 1) {
            gSaveContext.unk_13EE = gSaveContext.unk_13EA;

            if (CHECK_BTN_ALL(input->cur.button, BTN_L))
                CVar_SetS32("gPauseTriforce", 1);
            else
                CVar_SetS32("gPauseTriforce", 0);


            WREG(16) = -175;
            WREG(17) = 155;

            pauseCtx->unk_1EA = 0;
            pauseCtx->unk_1E4 = 1;

            if (ZREG(48) == 0) {
                pauseCtx->eye.x = sKaleidoSetupEyeX0[pauseCtx->pageIndex];
                pauseCtx->eye.z = sKaleidoSetupEyeZ0[pauseCtx->pageIndex];
                pauseCtx->pageIndex = sKaleidoSetupKscpPos0[pauseCtx->pageIndex];
            } else {
                pauseCtx->eye.x = sKaleidoSetupEyeX1[pauseCtx->pageIndex];
                pauseCtx->eye.z = sKaleidoSetupEyeZ1[pauseCtx->pageIndex];
                pauseCtx->pageIndex = sKaleidoSetupKscpPos1[pauseCtx->pageIndex];
            }

            pauseCtx->mode = (u16)(pauseCtx->pageIndex * 2) + 1;
            pauseCtx->state = 1;

            osSyncPrintf("Ｍｏｄｅ=%d  eye.x=%f,  eye.z=%f  kscp_pos=%d\n", pauseCtx->mode, pauseCtx->eye.x,
                         pauseCtx->eye.z, pauseCtx->pageIndex);
        }

        if (pauseCtx->state == 1) {
            WREG(2) = -6240;
            R_UPDATE_RATE = 2;

            if (ShrinkWindow_GetVal()) {
                ShrinkWindow_SetVal(0);
            }

            func_800F64E0(1);
        }
    }
}

void KaleidoSetup_Init(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u64 temp = 0; // Necessary to match

    pauseCtx->state = 0;
    pauseCtx->debugState = 0;
    pauseCtx->alpha = 0;
    pauseCtx->unk_1EA = 0;
    pauseCtx->unk_1E4 = 0;
    pauseCtx->mode = 0;
    pauseCtx->pageIndex = PAUSE_ITEM;

    pauseCtx->unk_1F4 = 160.0f;
    pauseCtx->unk_1F8 = 160.0f;
    pauseCtx->unk_1FC = 160.0f;
    pauseCtx->unk_200 = 160.0f;
    pauseCtx->eye.z = 64.0f;
    pauseCtx->unk_1F0 = 936.0f;
    pauseCtx->eye.x = pauseCtx->eye.y = 0.0f;
    pauseCtx->unk_204 = -314.0f;

    pauseCtx->cursorPoint[PAUSE_ITEM] = 0;
    pauseCtx->cursorPoint[PAUSE_MAP] = VREG(30) + 3;
    pauseCtx->cursorPoint[PAUSE_QUEST] = 0;
    pauseCtx->cursorPoint[PAUSE_EQUIP] = 1;
    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 10;

    pauseCtx->cursorX[PAUSE_ITEM] = 0;
    pauseCtx->cursorY[PAUSE_ITEM] = 0;
    pauseCtx->cursorX[PAUSE_MAP] = 0;
    pauseCtx->cursorY[PAUSE_MAP] = 0;
    pauseCtx->cursorX[PAUSE_QUEST] = temp;
    pauseCtx->cursorY[PAUSE_QUEST] = temp;
    pauseCtx->cursorX[PAUSE_EQUIP] = 1;
    pauseCtx->cursorY[PAUSE_EQUIP] = 0;

    pauseCtx->cursorItem[PAUSE_ITEM] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_MAP] = VREG(30) + 3;
    pauseCtx->cursorItem[PAUSE_QUEST] = PAUSE_ITEM_NONE;
    pauseCtx->cursorItem[PAUSE_EQUIP] = ITEM_SWORD_KOKIRI;

    pauseCtx->cursorSlot[PAUSE_ITEM] = 0;
    pauseCtx->cursorSlot[PAUSE_MAP] = VREG(30) + 3;
    pauseCtx->cursorSlot[PAUSE_QUEST] = 0;
    pauseCtx->cursorSlot[PAUSE_EQUIP] = pauseCtx->cursorPoint[PAUSE_EQUIP];

    pauseCtx->infoPanelOffsetY = -40;
    pauseCtx->nameDisplayTimer = 0;
    pauseCtx->nameColorSet = 0;
    pauseCtx->cursorColorSet = 4;
    pauseCtx->ocarinaSongIdx = -1;
    pauseCtx->cursorSpecialPos = 0;

    View_Init(&pauseCtx->view, play->state.gfxCtx);
}

void KaleidoSetup_Destroy(PlayState* play) {
}
