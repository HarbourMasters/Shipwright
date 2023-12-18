#include "global.h"

void GameOver_Init(PlayState* play) {
    play->gameOverCtx.state = GAMEOVER_INACTIVE;
}

void GameOver_FadeInLights(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;

    if ((gameOverCtx->state >= GAMEOVER_DEATH_WAIT_GROUND && gameOverCtx->state < GAMEOVER_REVIVE_START) ||
        (gameOverCtx->state >= GAMEOVER_REVIVE_RUMBLE && gameOverCtx->state < GAMEOVER_REVIVE_FADE_OUT)) {
        Environment_FadeInGameOverLights(play);
    }
}

// This variable cannot be moved into this file as all of z_message_PAL rodata is in the way
extern s16 gGameOverTimer;

void GameOver_Update(PlayState* play) {
    GameOverContext* gameOverCtx = &play->gameOverCtx;
    s16 i;
    s16 j;
    s32 v90;
    s32 v91;
    s32 v92;

    switch (gameOverCtx->state) {
        case GAMEOVER_DEATH_START:
            Message_CloseTextbox(play);

            gSaveContext.timer1State = 0;
            gSaveContext.timer2State = 0;
            gSaveContext.eventInf[1] &= ~1;

            // search inventory for spoiling items and revert if necessary
            if (!(IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE))) {
                for (i = 0; i < ARRAY_COUNT(gSpoilingItems); i++) {
                    if (INV_CONTENT(ITEM_POCKET_EGG) == gSpoilingItems[i]) {
                        INV_CONTENT(gSpoilingItemReverts[i]) = gSpoilingItemReverts[i];

                        // search c buttons for the found spoiling item and revert if necessary
                        for (j = 1; j < ARRAY_COUNT(gSaveContext.equips.buttonItems); j++) {
                            if (gSaveContext.equips.buttonItems[j] == gSpoilingItems[i]) {
                                gSaveContext.equips.buttonItems[j] = gSpoilingItemReverts[i];
                                Interface_LoadItemIcon1(play, j);
                            }
                        }
                    }
                }
			}

            // restore "temporary B" to the B Button if not a sword item
            if (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI &&
                gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER &&
                gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS &&
                gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE) {

                if (gSaveContext.buttonStatus[0] != BTN_ENABLED) {
                    gSaveContext.equips.buttonItems[0] = gSaveContext.buttonStatus[0];
                    Interface_RandoRestoreSwordless();
                } else {
                    gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                }
            }

            gSaveContext.nayrusLoveTimer = 2000;
            gSaveContext.naviTimer = 0;
            gSaveContext.seqId = (u8)NA_BGM_DISABLED;
            gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
            gSaveContext.eventInf[0] = 0;
            gSaveContext.eventInf[1] = 0;
            gSaveContext.eventInf[2] = 0;
            gSaveContext.eventInf[3] = 0;
            for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
                gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
            }
            gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;

            Environment_InitGameOverLights(play);
            gGameOverTimer = 20;
            v90 = VREG(90);
            v91 = VREG(91);
            v92 = VREG(92);

            func_800AA000(0.0f, ((v90 > 0x64) ? 0xFF : (v90 * 0xFF) / 0x64), (CLAMP_MAX(v91 * 3, 0xFF)),
                          ((v92 > 0x64) ? 0xFF : (v92 * 0xFF) / 0x64));

            gameOverCtx->state = GAMEOVER_DEATH_WAIT_GROUND;
            break;

        case GAMEOVER_DEATH_WAIT_GROUND:
            break;

        case GAMEOVER_DEATH_DELAY_MENU:
            gGameOverTimer--;

            if (gGameOverTimer == 0) {
                play->pauseCtx.state = 8;
                gameOverCtx->state++;
                func_800AA15C();
            }
            break;

        case GAMEOVER_REVIVE_START:
            gameOverCtx->state++;
            gGameOverTimer = 0;
            Environment_InitGameOverLights(play);
            ShrinkWindow_SetVal(0x20);
            return;

        case GAMEOVER_REVIVE_RUMBLE:
            gGameOverTimer = 50;
            gameOverCtx->state++;

            v90 = VREG(90);
            v91 = VREG(91);
            v92 = VREG(92);

            func_800AA000(0.0f, ((v90 > 0x64) ? 0xFF : (v90 * 0xFF) / 0x64), (CLAMP_MAX(v91 * 3, 0xFF)),
                          ((v92 > 0x64) ? 0xFF : (v92 * 0xFF) / 0x64));
            break;

        case GAMEOVER_REVIVE_WAIT_GROUND:
            gGameOverTimer--;

            if (gGameOverTimer == 0) {
                gGameOverTimer = 64;
                gameOverCtx->state++;
            }
            break;

        case GAMEOVER_REVIVE_WAIT_FAIRY:
            gGameOverTimer--;

            if (gGameOverTimer == 0) {
                gGameOverTimer = 50;
                gameOverCtx->state++;
            }
            break;

        case GAMEOVER_REVIVE_FADE_OUT:
            Environment_FadeOutGameOverLights(play);
            gGameOverTimer--;

            if (gGameOverTimer == 0) {
                gameOverCtx->state = GAMEOVER_INACTIVE;
            }
            break;
    }
}
