#include "z64game_over.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include <libultraship/bridge/consolevariablebridge.h>

void GameOver_Init(PlayState* play) {
    play->gameOverCtx.state = GAMEOVER_INACTIVE;
}

void GameOver_FadeLights(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;

    if ((gameOverCtx->state >= GAMEOVER_DEATH_WAIT_GROUND && gameOverCtx->state < GAMEOVER_REVIVE_START) ||
        (gameOverCtx->state >= GAMEOVER_REVIVE_RUMBLE && gameOverCtx->state < GAMEOVER_REVIVE_FADE_OUT)) {
        Environment_FadeInGameOverLights(play);
    }
}

static s16 sGameOverTimer = 0;

void GameOver_Update(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    s16 timerId;

    switch (gameOverCtx->state) {
        case GAMEOVER_DEATH_START:
            Message_CloseTextbox(play);

            for (timerId = 0; timerId < TIMER_ID_MAX; timerId++) {
                gSaveContext.timerStates[timerId] = TIMER_STATE_OFF;
            }

            CLEAR_EVENTINF_ALT(EVENTINF_10);

            if (CUR_FORM == 0) {
                if (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_KOKIRI &&
                    CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_RAZOR &&
                    CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_GILDED &&
                    CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_DEITY) {

                    if (gSaveContext.buttonStatus[EQUIP_SLOT_B] != BTN_ENABLED) {
                        CUR_FORM_EQUIP(EQUIP_SLOT_B) = gSaveContext.buttonStatus[EQUIP_SLOT_B];
                    } else {
                        CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_NONE;
                    }
                }
            }

            gSaveContext.nayrusLoveTimer = 2000;
            gSaveContext.save.saveInfo.playerData.tatlTimer = 0;
            gSaveContext.seqId = NA_BGM_DISABLED;
            gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
            gSaveContext.eventInf[0] = 0;
            gSaveContext.eventInf[1] = 0;
            gSaveContext.eventInf[2] = 0;
            gSaveContext.eventInf[3] = 0;
            gSaveContext.buttonStatus[EQUIP_SLOT_B] = BTN_ENABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] = BTN_ENABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_DOWN] = BTN_ENABLED;
            gSaveContext.buttonStatus[EQUIP_SLOT_C_RIGHT] = BTN_ENABLED;
            // #region 2S2H
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_RIGHT] = BTN_ENABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_LEFT] = BTN_ENABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_DOWN] = BTN_ENABLED;
            gSaveContext.shipSaveContext.dpad.status[EQUIP_SLOT_D_UP] = BTN_ENABLED;
            // #endregion
            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
            gSaveContext.hudVisibilityForceButtonAlphasByStatus = false;
            gSaveContext.nextHudVisibility = HUD_VISIBILITY_IDLE;
            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
            gSaveContext.hudVisibilityTimer = 0;
            Environment_InitGameOverLights(play);
            sGameOverTimer = 20;
            Rumble_Request(0.0f, 126, 124, 63);
            gameOverCtx->state = GAMEOVER_DEATH_WAIT_GROUND;
            break;

        case GAMEOVER_DEATH_FADE_OUT:
            if (CVarGetInteger("gEnhancements.Kaleido.GameOver", 0)) {
                sGameOverTimer--;

                if (sGameOverTimer == 0) {
                    play->pauseCtx.state = PAUSE_STATE_GAMEOVER_0;
                    gameOverCtx->state++;
                    Rumble_StateReset();
                }
                break;
            }
            if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE) != NA_BGM_GAME_OVER) {
                func_80169F78(play);
                if (gSaveContext.respawnFlag != -7) {
                    gSaveContext.respawnFlag = -6;
                }
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                gSaveContext.save.saveInfo.playerData.health = 0x30;
                gameOverCtx->state++;
                if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU) {
                    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
                    gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
                }
                Rumble_StateReset();
            }
            break;

        case GAMEOVER_REVIVE_START:
            gameOverCtx->state++; // GAMEOVER_REVIVE_RUMBLE
            sGameOverTimer = 0;
            Environment_InitGameOverLights(play);
            ShrinkWindow_Letterbox_SetSizeTarget(32);
            break;

        case GAMEOVER_REVIVE_RUMBLE:
            sGameOverTimer = 50;
            gameOverCtx->state++; // GAMEOVER_REVIVE_WAIT_GROUND
            Rumble_Request(0.0f, 126, 124, 63);
            break;

        case GAMEOVER_REVIVE_WAIT_GROUND:
            sGameOverTimer--;
            if (sGameOverTimer == 0) {
                sGameOverTimer = 64;
                gameOverCtx->state++; // GAMEOVER_REVIVE_WAIT_FAIRY
            }
            break;

        case GAMEOVER_REVIVE_WAIT_FAIRY:
            sGameOverTimer--;
            if (sGameOverTimer == 0) {
                sGameOverTimer = 50;
                gameOverCtx->state++; // GAMEOVER_REVIVE_FADE_OUT
            }
            break;

        case GAMEOVER_REVIVE_FADE_OUT:
            Environment_FadeOutGameOverLights(play);
            sGameOverTimer--;
            if (sGameOverTimer == 0) {
                gameOverCtx->state = GAMEOVER_INACTIVE;
            }
            break;
    }
}
