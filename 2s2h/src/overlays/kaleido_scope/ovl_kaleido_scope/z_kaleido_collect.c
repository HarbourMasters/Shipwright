/*
 * File: z_kaleido_collect.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu - Quest Status Page
 */

#include "z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"
#include "archives/icon_item_static/icon_item_static_yar.h"

s32 KaleidoScope_UpdateQuestStatusPoint(PauseContext* pauseCtx, s16 point);

s16 sQuestRemainsColorTimerInit[] = { 120, 60, 2, 80 };
s16 sQuestHpColorTimerInits[] = { 20, 4, 20, 10 };
s16 sQuestSongPlayedOcarinaButtonsNum = 0;
u8 sQuestSongPlayedOcarinaButtons[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 sQuestSongPlayedOcarinaButtonsAlpha[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
s16 sQuestHpPrimColorTargets[][4] = {
    { 255, 0, 0, 255 },
    { 255, 70, 0, 150 },
    { 255, 70, 0, 150 },
    { 255, 0, 0, 255 },
};
s16 sQuestRemainsEnvTargets[][3] = {
    { 0, 0, 0 },    // QUEST_REMAINS_ODOLWA Target 1
    { 0, 0, 0 },    // QUEST_REMAINS_GOHT Target 1
    { 0, 0, 0 },    // QUEST_REMAINS_GYORG Target 1
    { 0, 0, 0 },    // QUEST_REMAINS_TWINMOLD Target 1
    { 0, 0, 0 },    // Unused
    { 0, 0, 0 },    // Unused
    { 0, 60, 0 },   // QUEST_REMAINS_ODOLWA Target 2
    { 90, 0, 0 },   // QUEST_REMAINS_GOHT Target 2
    { 0, 40, 110 }, // QUEST_REMAINS_GYORG Target 2
    { 80, 40, 0 },  // QUEST_REMAINS_TWINMOLD Target 2
    { 70, 0, 90 },  // Unused
    { 90, 90, 0 },  // Unused
};
s16 sQuestRemainsEnvRed[] = {
    255, // QUEST_REMAINS_ODOLWA
    255, // QUEST_REMAINS_GOHT
    255, // QUEST_REMAINS_GYORG
    255, // QUEST_REMAINS_TWINMOLD
    255, // Unused
    255, // Unused
};
s16 sQuestRemainsEnvGreen[] = {
    255, // QUEST_REMAINS_ODOLWA
    255, // QUEST_REMAINS_GOHT
    255, // QUEST_REMAINS_GYORG
    255, // QUEST_REMAINS_TWINMOLD
    255, // Unused
    255, // Unused
};
s16 sQuestRemainsEnvBlue[] = {
    150, // QUEST_REMAINS_ODOLWA
    150, // QUEST_REMAINS_GOHT
    150, // QUEST_REMAINS_GYORG
    150, // QUEST_REMAINS_TWINMOLD
    150, // Unused
    150, // Unused
};

// 2S2H [Port] (and in the function) don't do pointer math and access the list of digits directly.
extern const char* sCounterTextures[];

void KaleidoScope_DrawQuestStatus(PlayState* play) {
    static s16 sQuestRemainsColorTimer = 20;
    static s16 sQuestRemainsColorTimerIndex = 0;
    static s16 sQuestHpPrimRed = 0;
    static s16 sQuestHpPrimGreen = 0;
    static s16 sQuestHpPrimBlue = 0;
    static s16 sQuestHpPrimAlpha = 0;
    static s16 sQuestHpColorTimer = 20;
    static s16 sQuestHpPrimColorTargetIndex = 0;
    static TexturePtr sOcarinaButtonTextures[] = {
        gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
    };
    static s16 sQuestSongsPrimRed[] = {
        150, // QUEST_SONG_SONATA
        255, // QUEST_SONG_LULLABY
        100, // QUEST_SONG_BOSSA_NOVA
        255, // QUEST_SONG_ELEGY
        255, // QUEST_SONG_OATH
        255, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static s16 sQuestSongsPrimGreen[] = {
        255, // QUEST_SONG_SONATA
        80,  // QUEST_SONG_LULLABY
        150, // QUEST_SONG_BOSSA_NOVA
        160, // QUEST_SONG_ELEGY
        100, // QUEST_SONG_OATH
        240, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static s16 sQuestSongsPrimBlue[] = {
        100, // QUEST_SONG_SONATA
        40,  // QUEST_SONG_LULLABY
        255, // QUEST_SONG_BOSSA_NOVA
        0,   // QUEST_SONG_ELEGY
        255, // QUEST_SONG_OATH
        100, // QUEST_SONG_SARIA
        255, // QUEST_SONG_TIME
        255, // QUEST_SONG_HEALING
        255, // QUEST_SONG_EPONA
        255, // QUEST_SONG_SOARING
        255, // QUEST_SONG_STORMS
        255, // QUEST_SONG_SUN
    };
    static TexturePtr sQuestUpgradeTextures[][3] = {
        { gItemIconQuiver30Tex, gItemIconQuiver40Tex, gItemIconQuiver50Tex },    // UPG_QUIVER
        { gItemIconBombBag20Tex, gItemIconBombBag30Tex, gItemIconBombBag40Tex }, // UPG_BOMB_BAG
    };
    static u8 sQuestUpgrades[] = { UPG_QUIVER, UPG_BOMB_BAG };
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 sp1CA; // colorSetR and numOcarinaButtons
    s16 sp1C8; // colorSetG and ocarinaButtonIndex
    s16 sp1C6;
    s16 sp1C4;
    s16 var_v1;
    s16 i;
    s16 j;
    s16 k;
    s16 skullTokenDigits[3];
    u16 isDigitDrawn;
    u32* questItemsPtr;

    OPEN_DISPS(play->state.gfxCtx);

    KaleidoScope_SetCursorVtxPos(pauseCtx, pauseCtx->cursorSlot[PAUSE_QUEST] * 4, pauseCtx->questVtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    // Draw the Boss Remains icons
    sQuestRemainsColorTimer--;
    for (i = 0, j = 0; i < 4; i++, j += 4) {
        if ((sQuestRemainsColorTimerIndex != 1) && (sQuestRemainsColorTimerIndex != 3)) {
            var_v1 = (sQuestRemainsColorTimerIndex != 0) ? (i + 6) : i;

            if (sQuestRemainsColorTimer != 0) {
                sp1CA = ABS_ALT(sQuestRemainsEnvRed[i] - sQuestRemainsEnvTargets[var_v1][0]) / sQuestRemainsColorTimer;
                sp1C8 =
                    ABS_ALT(sQuestRemainsEnvGreen[i] - sQuestRemainsEnvTargets[var_v1][1]) / sQuestRemainsColorTimer;
                sp1C6 = ABS_ALT(sQuestRemainsEnvBlue[i] - sQuestRemainsEnvTargets[var_v1][2]) / sQuestRemainsColorTimer;

                if (sQuestRemainsEnvRed[i] >= sQuestRemainsEnvTargets[var_v1][0]) {
                    sQuestRemainsEnvRed[i] -= sp1CA;
                } else {
                    sQuestRemainsEnvRed[i] += sp1CA;
                }
                if (sQuestRemainsEnvGreen[i] >= sQuestRemainsEnvTargets[var_v1][1]) {
                    sQuestRemainsEnvGreen[i] -= sp1C8;
                } else {
                    sQuestRemainsEnvGreen[i] += sp1C8;
                }
                if (sQuestRemainsEnvBlue[i] >= sQuestRemainsEnvTargets[var_v1][2]) {
                    sQuestRemainsEnvBlue[i] -= sp1C6;
                } else {
                    sQuestRemainsEnvBlue[i] += sp1C6;
                }
            } else {
                sQuestRemainsEnvRed[i] = sQuestRemainsEnvTargets[var_v1][0];
                sQuestRemainsEnvGreen[i] = sQuestRemainsEnvTargets[var_v1][1];
                sQuestRemainsEnvBlue[i] = sQuestRemainsEnvTargets[var_v1][2];
            }
        }

        if (CHECK_QUEST_ITEM(i)) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
            gDPSetEnvColor(POLY_OPA_DISP++, sQuestRemainsEnvRed[i], sQuestRemainsEnvGreen[i], sQuestRemainsEnvBlue[i],
                           0);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
            KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[ITEM_REMAINS_ODOLWA + i], 32, 32, 0);
        }
    }

    if (sQuestRemainsColorTimer == 0) {
        sQuestRemainsColorTimer = sQuestRemainsColorTimerInit[sQuestRemainsColorTimerIndex];
        if (++sQuestRemainsColorTimerIndex > 3) {
            sQuestRemainsColorTimerIndex = 0;
        }
    }

    // Draw shield
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[(ITEM_SHIELD_HERO - 1) + GET_CUR_EQUIP_VALUE(1)],
                                       32, 32, 0);
    }

    j += 4;

    // Draw Sword
    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) != EQUIP_VALUE_SWORD_NONE) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[(ITEM_SWORD_KOKIRI - 1) + GET_CUR_EQUIP_VALUE(0)],
                                       32, 32, 0);
    }

    j += 4;

    // Draw Songs
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gItemIconSongNoteTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    for (i = 0; i < 12; i++, j += 4) {
        if (CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) ||
            ((i == (QUEST_SONG_LULLABY - QUEST_SONG_SONATA)) && !CHECK_QUEST_ITEM(i + QUEST_SONG_SONATA) &&
             CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) {
            if ((i + QUEST_SONG_SONATA) == pauseCtx->cursorSlot[PAUSE_QUEST]) {
                pauseCtx->questVtx[j + 0].v.ob[0] = pauseCtx->questVtx[j + 2].v.ob[0] =
                    pauseCtx->questVtx[j + 0].v.ob[0] - 2;

                pauseCtx->questVtx[j + 1].v.ob[0] = pauseCtx->questVtx[j + 3].v.ob[0] =
                    pauseCtx->questVtx[j + 1].v.ob[0] + 4;

                pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                    pauseCtx->questVtx[j + 0].v.ob[1] + 2;

                pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                    pauseCtx->questVtx[j + 2].v.ob[1] - 4;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestSongsPrimRed[i], sQuestSongsPrimGreen[i],
                            sQuestSongsPrimBlue[i], pauseCtx->alpha);
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }
    }

    // Draw Bombers Notebook
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIconBombersNotebookTex, 32, 32, 0);
    }

    j += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    // Loop over quest item upgrades
    for (i = 0; i < 2; i++, j += 4) {
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (GET_CUR_UPG_VALUE(sQuestUpgrades[i]) != 0) {
            KaleidoScope_DrawTexQuadRGBA32(
                play->state.gfxCtx, sQuestUpgradeTextures[i][GET_CUR_UPG_VALUE(sQuestUpgrades[i]) - 1], 32, 32, 0);
        }
    }

    // Skip over &pauseCtx->questVtx[84], which should be `QUEST_SKULL_TOKEN`
    j += 4;

    sp1CA = ABS_ALT(sQuestHpPrimRed - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0]) / sQuestHpColorTimer;
    sp1C8 = ABS_ALT(sQuestHpPrimGreen - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1]) / sQuestHpColorTimer;
    sp1C6 = ABS_ALT(sQuestHpPrimBlue - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2]) / sQuestHpColorTimer;
    sp1C4 = ABS_ALT(sQuestHpPrimAlpha - sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3]) / sQuestHpColorTimer;

    if (sQuestHpPrimRed >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0]) {
        sQuestHpPrimRed -= sp1CA;
    } else {
        sQuestHpPrimRed += sp1CA;
    }

    if (sQuestHpPrimGreen >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1]) {
        sQuestHpPrimGreen -= sp1C8;
    } else {
        sQuestHpPrimGreen += sp1C8;
    }

    if (sQuestHpPrimBlue >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2]) {
        sQuestHpPrimBlue -= sp1C6;
    } else {
        sQuestHpPrimBlue += sp1C6;
    }

    if (sQuestHpPrimAlpha >= sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3]) {
        sQuestHpPrimAlpha -= sp1C4;
    } else {
        sQuestHpPrimAlpha += sp1C4;
    }

    sQuestHpColorTimer--;
    if (sQuestHpColorTimer == 0) {
        sQuestHpPrimRed = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][0];
        sQuestHpPrimGreen = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][1];
        sQuestHpPrimBlue = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][2];
        sQuestHpPrimAlpha = sQuestHpPrimColorTargets[sQuestHpPrimColorTargetIndex][3];
        sQuestHpColorTimer = sQuestHpColorTimerInits[sQuestHpPrimColorTargetIndex];
        if (++sQuestHpPrimColorTargetIndex > 3) {
            sQuestHpPrimColorTargetIndex = 0;
        }
    }

    //! FAKE: Used to load `0xF0000000` early
    if ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) != 0) {}
    questItemsPtr = &gSaveContext.save.saveInfo.inventory.questItems;
    if (1) {}

    if ((*questItemsPtr >> QUEST_HEART_PIECE_COUNT) != 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        if ((pauseCtx->state == PAUSE_STATE_OPENING_3) || (pauseCtx->state == PAUSE_STATE_UNPAUSE_SETUP)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestHpPrimColorTargets[0][0], sQuestHpPrimColorTargets[0][1],
                            sQuestHpPrimColorTargets[0][2], pauseCtx->alpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sQuestHpPrimRed, sQuestHpPrimGreen, sQuestHpPrimBlue,
                            sQuestHpPrimAlpha);
        }

        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

        POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
            POLY_OPA_DISP,
            gItemIcons[(0x7A + ((GET_SAVE_INVENTORY_QUEST_ITEMS & 0xF0000000) >> QUEST_HEART_PIECE_COUNT))], 48, 48, 0);
    }

    j += 4;

    if (pauseCtx->state == PAUSE_STATE_MAIN) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK) {
            // Draw ocarina buttons as they are played back
            pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();

            if (pauseCtx->ocarinaStaff->pos != 0) {
                if (sQuestSongPlayedOcarinaButtonsNum == (pauseCtx->ocarinaStaff->pos - 1)) {
                    sQuestSongPlayedOcarinaButtonsNum++;
                    sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos - 1] =
                        pauseCtx->ocarinaStaff->buttonIndex;
                }

                for (i = 0, k = 0; i < 8; i++, k += 4, j += 4) {
                    if (sQuestSongPlayedOcarinaButtons[i] == OCARINA_BTN_INVALID) {
                        break;
                    }

                    if (sQuestSongPlayedOcarinaButtonsAlpha[i] != 255) {
                        sQuestSongPlayedOcarinaButtonsAlpha[i] += 50;
                        if (sQuestSongPlayedOcarinaButtonsAlpha[i] >= 255) {
                            sQuestSongPlayedOcarinaButtonsAlpha[i] = 255;
                        }
                    }

                    pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                        pauseCtx->ocarinaButtonsY[sQuestSongPlayedOcarinaButtons[i]];

                    pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                        pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (sQuestSongPlayedOcarinaButtons[i] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, sQuestSongPlayedOcarinaButtonsAlpha[i]);
                    }

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                        POLY_OPA_DISP, sOcarinaButtonTextures[sQuestSongPlayedOcarinaButtons[i]], 16, 16, 0);
                }
            }
        } else if (IS_PAUSE_MAIN_STATE_SONG_PROMPT(pauseCtx) ||
                   (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) {
            // Draw the buttons for playing a song
            sp1C8 = pauseCtx->ocarinaSongIndex;
            sp1CA = gOcarinaSongButtons[sp1C8].numButtons;

            i = j;

            for (k = 0; k < sp1CA; k++, j += 4) {
                pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                    pauseCtx->ocarinaButtonsY[gOcarinaSongButtons[sp1C8].buttonIndex[k]];

                pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                    pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                gDPPipeSync(POLY_OPA_DISP++);

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                    // Draw ocarina buttons colored
                    if (gOcarinaSongButtons[sp1C8].buttonIndex[k] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, 200);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, 200);
                    }
                } else {
                    // Gray out buttons while reading ocarina song inputs
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 150, 150, 150);
                }

                gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                    POLY_OPA_DISP, sOcarinaButtonTextures[gOcarinaSongButtons[sp1C8].buttonIndex[k]], 16, 16, 0);
            }

            if (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) {
                pauseCtx->ocarinaStaff = AudioOcarina_GetPlayingStaff();

                // Update ocarina song inputs
                if (pauseCtx->ocarinaStaff->pos != 0) {
                    if (sQuestSongPlayedOcarinaButtonsNum == (pauseCtx->ocarinaStaff->pos - 1)) {
                        if (pauseCtx->ocarinaStaff->buttonIndex <= OCARINA_BTN_C_UP) {
                            sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos - 1] =
                                pauseCtx->ocarinaStaff->buttonIndex;
                            sQuestSongPlayedOcarinaButtons[pauseCtx->ocarinaStaff->pos] = OCARINA_BTN_INVALID;
                            sQuestSongPlayedOcarinaButtonsNum++;
                        }
                    }
                }

                // Draw the buttons colored as the ocarina song inputs are read from
                j = i + 32;
                k = 0;
                for (; k < 8; k++, j += 4) {
                    if (sQuestSongPlayedOcarinaButtons[k] == OCARINA_BTN_INVALID) {
                        continue;
                    }

                    if (sQuestSongPlayedOcarinaButtonsAlpha[k] != 255) {
                        sQuestSongPlayedOcarinaButtonsAlpha[k] += 50;
                        if (sQuestSongPlayedOcarinaButtonsAlpha[k] >= 255) {
                            sQuestSongPlayedOcarinaButtonsAlpha[k] = 255;
                        }
                    }

                    pauseCtx->questVtx[j + 0].v.ob[1] = pauseCtx->questVtx[j + 1].v.ob[1] =
                        pauseCtx->ocarinaButtonsY[sQuestSongPlayedOcarinaButtons[k]];

                    pauseCtx->questVtx[j + 2].v.ob[1] = pauseCtx->questVtx[j + 3].v.ob[1] =
                        pauseCtx->questVtx[j + 0].v.ob[1] - 12;

                    gDPPipeSync(POLY_OPA_DISP++);

                    if (sQuestSongPlayedOcarinaButtons[k] == OCARINA_BTN_A) {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 150, 255, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                    } else {
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 50, sQuestSongPlayedOcarinaButtonsAlpha[k]);
                    }

                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[j], 4, 0);

                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(
                        POLY_OPA_DISP, sOcarinaButtonTextures[sQuestSongPlayedOcarinaButtons[k]], 16, 16, 0);
                }

                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT_INIT) {
                    for (k = 0; k < 8; k++) {
                        sQuestSongPlayedOcarinaButtons[k] = OCARINA_BTN_INVALID;
                        sQuestSongPlayedOcarinaButtonsAlpha[k] = 0;
                    }

                    sQuestSongPlayedOcarinaButtonsNum = 0;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                    AudioOcarina_StartDefault((1 << pauseCtx->ocarinaSongIndex) | 0x80000000);
                    pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                    pauseCtx->ocarinaStaff->pos = 0;
                    pauseCtx->ocarinaStaff->state = 0xFE;
                    pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PROMPT;
                }
            }
        }
    }

    // Draw Skull Token Count
    // QUEST_SKULL_TOKEN never properly set, see Item_Give(),
    // Vertices not well placed, digits are not aligned and placed in unintended positions
    if (CHECK_QUEST_ITEM(QUEST_SKULL_TOKEN) && ((play->sceneId == SCENE_KINSTA1) || (play->sceneId == SCENE_KINDAN2))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        skullTokenDigits[0] = skullTokenDigits[1] = 0;
        skullTokenDigits[2] = Inventory_GetSkullTokenCount(play->sceneId);

        while (skullTokenDigits[2] >= 100) {
            skullTokenDigits[0]++;
            skullTokenDigits[2] -= 100;
        }

        while (skullTokenDigits[2] >= 10) {
            skullTokenDigits[1]++;
            skullTokenDigits[2] -= 10;
        }

        //! @bug: &pauseCtx->questVtx[84] is the questVtx for skull tokens
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->questVtx[152], 24, 0);

        // Loop over two sets of digits, the first is shadowed, the second is colored
        for (k = 0, i = 0; k < 2; k++) {
            if (k == 0) {
                // shadow
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);
            } else {
                if (Inventory_GetSkullTokenCount(play->sceneId) == 100) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 50, 50, pauseCtx->alpha);
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
                }
            }

            isDigitDrawn = false;
            for (j = 0; j < 3; j++, i += 4) {
                if ((j >= 2) || (skullTokenDigits[j] != 0) || isDigitDrawn) {
                    // 2S2H [Port] Don't do pointer math and access the texture directly.
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sCounterTextures[skullTokenDigits[j]], G_IM_FMT_I, G_IM_SIZ_8b,
                                        8, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, i, i + 2, i + 3, i + 1, 0);

                    isDigitDrawn = true;
                }
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

typedef enum {
    /* -3 */ CURSOR_TO_LEFT = -3, // Cursor on the "scroll to left page" position
    /* -2 */ CURSOR_TO_RIGHT,     // Cursor on the "scroll to right page" position
    /* -1 */ CURSOR_NONE          // No position in that direction, cursor stays where it is
} CursorPointNext;

typedef struct {
    /* 0x0 */ s16 up;
    /* 0x2 */ s16 down;
    /* 0x4 */ s16 left;
    /* 0x6 */ s16 right;
} CursorPointDirection; // size = 0x8

void KaleidoScope_UpdateQuestCursor(PlayState* play) {
    static s16 sQuestSongPlaybackDelayTimer = 0;
    static CursorPointDirection sCursorPointLinks[] = {
        { CURSOR_NONE, QUEST_REMAINS_TWINMOLD, QUEST_REMAINS_GYORG, QUEST_REMAINS_GOHT },     // QUEST_REMAINS_ODOLWA
        { QUEST_REMAINS_ODOLWA, QUEST_SHIELD, QUEST_REMAINS_TWINMOLD, CURSOR_TO_RIGHT },      // QUEST_REMAINS_GOHT
        { QUEST_REMAINS_ODOLWA, QUEST_SWORD, QUEST_HEART_PIECE, QUEST_REMAINS_TWINMOLD },     // QUEST_REMAINS_GYORG
        { QUEST_REMAINS_ODOLWA, QUEST_SHIELD, QUEST_REMAINS_GYORG, QUEST_REMAINS_GOHT },      // QUEST_REMAINS_TWINMOLD
        { QUEST_REMAINS_GOHT, QUEST_BOMB_BAG, QUEST_SWORD, CURSOR_TO_RIGHT },                 // QUEST_SHIELD
        { QUEST_REMAINS_GYORG, QUEST_QUIVER, QUEST_SONG_STORMS, QUEST_SHIELD },               // QUEST_SWORD
        { QUEST_SONG_TIME, CURSOR_NONE, CURSOR_TO_LEFT, QUEST_SONG_LULLABY },                 // QUEST_SONG_SONATA
        { QUEST_SONG_HEALING, CURSOR_NONE, QUEST_SONG_SONATA, QUEST_SONG_BOSSA_NOVA },        // QUEST_SONG_LULLABY
        { QUEST_SONG_EPONA, CURSOR_NONE, QUEST_SONG_LULLABY, QUEST_SONG_ELEGY },              // QUEST_SONG_BOSSA_NOVA
        { QUEST_SONG_SOARING, CURSOR_NONE, QUEST_SONG_BOSSA_NOVA, QUEST_SONG_OATH },          // QUEST_SONG_ELEGY
        { QUEST_SONG_STORMS, CURSOR_NONE, QUEST_SONG_ELEGY, QUEST_QUIVER },                   // QUEST_SONG_OATH
        { QUEST_SONG_SUN, CURSOR_NONE, QUEST_SONG_OATH, QUEST_SWORD },                        // QUEST_SONG_SARIA
        { QUEST_BOMBERS_NOTEBOOK, QUEST_SONG_SONATA, CURSOR_TO_LEFT, QUEST_SONG_HEALING },    // QUEST_SONG_TIME
        { QUEST_BOMBERS_NOTEBOOK, QUEST_SONG_LULLABY, QUEST_SONG_TIME, QUEST_SONG_EPONA },    // QUEST_SONG_HEALING
        { QUEST_HEART_PIECE, QUEST_SONG_BOSSA_NOVA, QUEST_SONG_HEALING, QUEST_SONG_SOARING }, // QUEST_SONG_EPONA
        { QUEST_HEART_PIECE, QUEST_SONG_ELEGY, QUEST_SONG_EPONA, QUEST_SONG_STORMS },         // QUEST_SONG_SOARING
        { QUEST_HEART_PIECE, QUEST_SONG_OATH, QUEST_SONG_SOARING, QUEST_SWORD },              // QUEST_SONG_STORMS
        { QUEST_HEART_PIECE, QUEST_SONG_SARIA, QUEST_SONG_STORMS, QUEST_SWORD },              // QUEST_SONG_SUN
        { CURSOR_NONE, QUEST_SONG_TIME, CURSOR_TO_LEFT, QUEST_HEART_PIECE },                  // QUEST_BOMBERS_NOTEBOOK
        { QUEST_SWORD, CURSOR_NONE, QUEST_SONG_OATH, QUEST_BOMB_BAG },                        // QUEST_QUIVER
        { QUEST_SHIELD, CURSOR_NONE, QUEST_QUIVER, CURSOR_TO_RIGHT },                         // QUEST_BOMB_BAG
        { QUEST_QUIVER, QUEST_SONG_TIME, CURSOR_TO_LEFT, QUEST_HEART_PIECE },                 // QUEST_SKULL_TOKEN
        { CURSOR_NONE, QUEST_SONG_STORMS, QUEST_BOMBERS_NOTEBOOK, QUEST_REMAINS_GYORG },      // QUEST_HEART_PIECE
    };
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s32 pad;
    s16 nextCursorPoint;
    s16 oldCursorPoint;
    s16 i;
    u16 cursor;
    u16 cursorItem;

    pauseCtx->nameColorSet = PAUSE_NAME_COLOR_SET_WHITE;
    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;

    // != PAUSE_MAIN_STATE_IDLE
    if ((pauseCtx->state == PAUSE_STATE_MAIN) &&
        (!pauseCtx->mainState || (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) ||
         (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) &&
        (pauseCtx->pageIndex == PAUSE_QUEST) && !pauseCtx->itemDescriptionOn) {
        if (pauseCtx->cursorSpecialPos == 0) {
            oldCursorPoint = pauseCtx->cursorPoint[PAUSE_QUEST];

            if (pauseCtx->stickAdjX < -30) {
                // Move cursor left
                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                pauseCtx->cursorShrinkRate = 4.0f;

                nextCursorPoint = sCursorPointLinks[oldCursorPoint].left;
                if (nextCursorPoint == CURSOR_TO_LEFT) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    if (interfaceCtx->aButtonDoActionDelayed == DO_ACTION_DECIDE) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
                    }
                    return;
                } else {
                    while (nextCursorPoint > CURSOR_NONE) {
                        if (KaleidoScope_UpdateQuestStatusPoint(pauseCtx, nextCursorPoint)) {
                            break;
                        }
                        nextCursorPoint = sCursorPointLinks[nextCursorPoint].left;
                    }
                }
            } else if (pauseCtx->stickAdjX > 30) {
                // Move cursor right
                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                pauseCtx->cursorShrinkRate = 4.0f;
                nextCursorPoint = sCursorPointLinks[oldCursorPoint].right;

                if (nextCursorPoint == CURSOR_TO_RIGHT) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    return;
                }

                while (nextCursorPoint > CURSOR_NONE) {
                    if (KaleidoScope_UpdateQuestStatusPoint(pauseCtx, nextCursorPoint)) {
                        break;
                    }
                    nextCursorPoint = sCursorPointLinks[nextCursorPoint].right;
                }
            }

            if (pauseCtx->stickAdjY < -30) {
                // Move cursor down
                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                nextCursorPoint = sCursorPointLinks[oldCursorPoint].down;

                while (nextCursorPoint > CURSOR_NONE) {
                    pauseCtx->cursorShrinkRate = 4.0f;
                    if (KaleidoScope_UpdateQuestStatusPoint(pauseCtx, nextCursorPoint)) {
                        break;
                    }
                    nextCursorPoint = sCursorPointLinks[nextCursorPoint].down;
                }
            } else if (pauseCtx->stickAdjY > 30) {
                // Move cursor up
                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                nextCursorPoint = sCursorPointLinks[oldCursorPoint].up;
                while (nextCursorPoint > CURSOR_NONE) {
                    pauseCtx->cursorShrinkRate = 4.0f;
                    if (KaleidoScope_UpdateQuestStatusPoint(pauseCtx, nextCursorPoint)) {
                        break;
                    }
                    nextCursorPoint = sCursorPointLinks[nextCursorPoint].up;
                }
            }

            // if the cursor point changed
            if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_QUEST]) {
                pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                Audio_PlaySfx(NA_SE_SY_CURSOR);
            }

            // Update cursor item and slot
            if (pauseCtx->cursorPoint[PAUSE_QUEST] != QUEST_HEART_PIECE) {
                if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_REMAINS_TWINMOLD) {
                    // Boss Remains
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        cursorItem = ITEM_REMAINS_ODOLWA + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_BOMBERS_NOTEBOOK) {
                    // Bombers Notebook
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        cursorItem = ITEM_BOMBERS_NOTEBOOK;
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_SHIELD) {
                    // Shield
                    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
                        cursorItem = (ITEM_SHIELD_HERO - EQUIP_TYPE_SHIELD) + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_SWORD) {
                    // Sword
                    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) != EQUIP_VALUE_SWORD_NONE) {
                        cursorItem =
                            (ITEM_SWORD_KOKIRI - EQUIP_VALUE_SWORD_KOKIRI) + GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] <= QUEST_SONG_SUN) {
                    // Songs
                    if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                        cursorItem = ITEM_WALLET_GIANT + pauseCtx->cursorPoint[PAUSE_QUEST];
                    } else if ((pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_SONG_LULLABY) &&
                               CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
                        cursorItem = ITEM_SONG_LULLABY_INTRO;
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_QUIVER) {
                    // Quiver Upgrade
                    if (GET_CUR_UPG_VALUE(UPG_QUIVER) != 0) {
                        cursorItem = (ITEM_QUIVER_30 - 1) + GET_CUR_UPG_VALUE(UPG_QUIVER);
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_BOMB_BAG) {
                    // Bomb Bag Upgrade
                    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) != 0) {
                        cursorItem = (ITEM_BOMB_BAG_20 - 1) + GET_CUR_UPG_VALUE(UPG_BOMB_BAG);
                    } else {
                        cursorItem = PAUSE_ITEM_NONE;
                    }
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
            } else {
                // Heart Piece Count
                if ((GET_SAVE_INVENTORY_QUEST_ITEMS >> QUEST_HEART_PIECE_COUNT) != 0) {
                    cursorItem = ITEM_HEART_CONTAINER;
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }
            }

            cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
            pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
            pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;

            if ((pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
                (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->cursorSpecialPos == 0)) {
                if ((cursor >= QUEST_SONG_SONATA) && (cursor <= QUEST_SONG_SUN)) {
                    // Handle part of the ocarina songs playback
                    if ((CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST]) ||
                         ((cursor == QUEST_SONG_LULLABY) && !CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST]) &&
                          CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) &&
                        (msgCtx->msgLength == 0)) {
                        // The cursor is on a learned song
                        // Set some things up for song playback

                        if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                            cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
                        } else {
                            cursor = QUEST_BOMB_BAG;
                        }

                        pauseCtx->ocarinaSongIndex = gOcarinaSongItemMap[cursor - QUEST_SONG_SONATA];
                        sQuestSongPlaybackDelayTimer = 10;

                        for (i = 0; i < 8; i++) {
                            sQuestSongPlayedOcarinaButtons[i] = OCARINA_BTN_INVALID;
                            sQuestSongPlayedOcarinaButtonsAlpha[i] = 0;
                        }

                        sQuestSongPlayedOcarinaButtonsNum = 0;

                        // Setup the song to receive user input, immediately cancelled below
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
                        AudioOcarina_StartDefault((1 << pauseCtx->ocarinaSongIndex) | 0x80000000);

                        // Clear the playback staff
                        pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
                        pauseCtx->ocarinaStaff->pos = 0;
                        pauseCtx->ocarinaStaff->state = 0xFF;

                        pauseCtx->ocarinaButtonsY[OCARINA_BTN_A] = -62;
                        pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_DOWN] = -56;
                        pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_RIGHT] = -49;
                        pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_LEFT] = -46;
                        pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_UP] = -41;

                        pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG;

                        if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                        }

                        // Stop receiving input to play a song as mentioned above
                        AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);

                        if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
                            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                        }
                    } else {
                        if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                        }
                        if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
                            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
                            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                        }
                    }
                } else {
                    if ((cursor == QUEST_BOMBERS_NOTEBOOK) && (pauseCtx->cursorItem[PAUSE_QUEST] != PAUSE_ITEM_NONE)) {
                        if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
                            Interface_SetAButtonDoAction(play, DO_ACTION_DECIDE);
                        }
                        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;
                    } else if (interfaceCtx->aButtonDoActionDelayed == DO_ACTION_DECIDE) {
                        Interface_SetAButtonDoAction(play, DO_ACTION_INFO);
                    }

                    if ((pauseCtx->cursorItem[PAUSE_QUEST] != PAUSE_ITEM_NONE) && (msgCtx->msgLength == 0)) {
                        if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
                            gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                            gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                            Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                        }

                        if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) && (msgCtx->msgLength == 0)) {
                            if (pauseCtx->cursorPoint[PAUSE_QUEST] == QUEST_BOMBERS_NOTEBOOK) {
                                play->pauseCtx.bombersNotebookOpen = true;
                                pauseCtx->mainState = PAUSE_MAIN_STATE_BOMBERS_NOTEBOOK_OPEN;
                                Audio_PlaySfx(NA_SE_SY_DECIDE);
                            } else {
                                pauseCtx->itemDescriptionOn = true;
                                if (pauseCtx->cursorYIndex[PAUSE_QUEST] < 2) {
                                    if (pauseCtx->cursorItem[PAUSE_QUEST] < ITEM_REMAINS_ODOLWA) {
                                        func_801514B0(play, 0x1737 + pauseCtx->cursorItem[PAUSE_QUEST], 1);
                                    } else {
                                        func_801514B0(play, 0x173B + pauseCtx->cursorItem[PAUSE_QUEST], 3);
                                    }
                                } else {
                                    if (pauseCtx->cursorItem[PAUSE_QUEST] < ITEM_REMAINS_ODOLWA) {
                                        func_801514B0(play, 0x1737 + pauseCtx->cursorItem[PAUSE_QUEST], 1);
                                    } else {
                                        func_801514B0(play, 0x173B + pauseCtx->cursorItem[PAUSE_QUEST], 1);
                                    }
                                }
                            }
                        }
                    } else if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
                        gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
                        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                    }
                }
            } else if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                // Abort reading ocarina song input if the stick is moved
                if ((pauseCtx->stickAdjX != 0) || (pauseCtx->stickAdjY != 0)) {
                    pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
            } else if ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG) &&
                       CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) && (msgCtx->msgLength == 0) &&
                       (cursor >= QUEST_SONG_SONATA) && (cursor <= QUEST_SONG_SUN)) {
                pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PLAYBACK_INIT;
                sQuestSongPlaybackDelayTimer = 10;
            }

            if (pauseCtx->cursorSpecialPos == 0) {
                if ((pauseCtx->cursorSlot[PAUSE_QUEST] >= 6) && (pauseCtx->cursorSlot[PAUSE_QUEST] < 0x12) &&
                    ((pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PLAYBACK) ||
                     (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) ||
                     (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG)) &&
                    (pauseCtx->cursorItem[pauseCtx->pageIndex] != PAUSE_ITEM_NONE)) {
                    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;
                    if ((pauseCtx->mainState >= PAUSE_MAIN_STATE_SONG_PLAYBACK) &&
                        (pauseCtx->mainState <= PAUSE_MAIN_STATE_SONG_PROMPT_DONE)) {
                        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
                    }
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }

                KaleidoScope_MoveCursorFromSpecialPos(play);

                pauseCtx->cursorPoint[PAUSE_QUEST] = QUEST_BOMBERS_NOTEBOOK;
                if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
                    cursorItem = ITEM_BOMBERS_NOTEBOOK;
                } else {
                    cursorItem = PAUSE_ITEM_NONE;
                }

                cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
                pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
                pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;
            }
        } else if (pauseCtx->stickAdjX < -30) {
            if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PROMPT) {
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
            }

            KaleidoScope_MoveCursorFromSpecialPos(play);

            pauseCtx->cursorPoint[PAUSE_QUEST] = QUEST_REMAINS_GOHT;
            if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                cursorItem = (ITEM_REMAINS_GOHT - 1) + pauseCtx->cursorPoint[PAUSE_QUEST];
                if (pauseCtx->cursorPoint[PAUSE_QUEST] != QUEST_REMAINS_ODOLWA) {
                    // This condition is always true as `cursorPoint` is set three lines above
                    cursorItem = ITEM_MASK_GIANT;
                }
            } else {
                cursorItem = PAUSE_ITEM_NONE;
            }

            cursor = pauseCtx->cursorPoint[PAUSE_QUEST];
            pauseCtx->cursorItem[pauseCtx->pageIndex] = cursorItem;
            pauseCtx->cursorSlot[pauseCtx->pageIndex] = cursor;
        }
    } else if (pauseCtx->mainState == PAUSE_MAIN_STATE_SONG_PLAYBACK_INIT) {
        // After a short delay, start the playback of the selected song

        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_BLUE;
        sQuestSongPlaybackDelayTimer--;
        if (sQuestSongPlaybackDelayTimer == 0) {
            for (i = 0; i < 8; i++) {
                sQuestSongPlayedOcarinaButtons[i] = OCARINA_BTN_INVALID;
                sQuestSongPlayedOcarinaButtonsAlpha[i] = 0;
            }
            sQuestSongPlayedOcarinaButtonsNum = 0;

            pauseCtx->ocarinaButtonsY[OCARINA_BTN_A] = -62;
            pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_DOWN] = -56;
            pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_RIGHT] = -49;
            pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_LEFT] = -46;
            pauseCtx->ocarinaButtonsY[OCARINA_BTN_C_UP] = -41;

            if (CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                cursor = pauseCtx->cursorSlot[PAUSE_QUEST];
            } else {
                cursor = QUEST_BOMB_BAG;
            }

            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
            pauseCtx->ocarinaSongIndex = gOcarinaSongItemMap[cursor - QUEST_SONG_SONATA];
            AudioOcarina_SetPlaybackSong(pauseCtx->ocarinaSongIndex + 1, 1);
            pauseCtx->mainState = PAUSE_MAIN_STATE_SONG_PLAYBACK;
            pauseCtx->ocarinaStaff = AudioOcarina_GetPlaybackStaff();
            pauseCtx->ocarinaStaff->pos = 0;
        }
    }
}

s32 KaleidoScope_UpdateQuestStatusPoint(PauseContext* pauseCtx, s16 point) {
    pauseCtx->cursorPoint[PAUSE_QUEST] = point;

    return true;
}
