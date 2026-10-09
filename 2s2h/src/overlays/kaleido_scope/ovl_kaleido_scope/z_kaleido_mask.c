/*
 * File: z_kaleido_mask.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu - Mask Page
 */

#include "z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"

#include "2s2h/BenGui/HudEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

s16 sMaskEquipState = EQUIP_STATE_MAGIC_ARROW_GROW_ORB;

// Timer to hold magic arrow icon over magic arrow slot before moving when equipping.
s16 sMaskEquipMagicArrowSlotHoldTimer = 0;

// Number of frames to move icon from slot to target position when equipping.
s16 sMaskEquipAnimTimer = 10;

u8 gMaskPlayerFormSlotRestrictions[PLAYER_FORM_MAX][MASK_NUM_SLOTS] = {
    // PLAYER_FORM_FIERCE_DEITY
    {
        false, // SLOT_MASK_POSTMAN
        false, // SLOT_MASK_ALL_NIGHT
        false, // SLOT_MASK_BLAST
        false, // SLOT_MASK_STONE
        false, // SLOT_MASK_GREAT_FAIRY
        false, // SLOT_MASK_DEKU
        false, // SLOT_MASK_KEATON
        false, // SLOT_MASK_BREMEN
        false, // SLOT_MASK_BUNNY
        false, // SLOT_MASK_DON_GERO
        false, // SLOT_MASK_SCENTS
        false, // SLOT_MASK_GORON
        false, // SLOT_MASK_ROMANI
        false, // SLOT_MASK_CIRCUS_LEADER
        false, // SLOT_MASK_KAFEIS_MASK
        false, // SLOT_MASK_COUPLE
        false, // SLOT_MASK_TRUTH
        false, // SLOT_MASK_ZORA
        false, // SLOT_MASK_KAMARO
        false, // SLOT_MASK_GIBDO
        false, // SLOT_MASK_GARO
        false, // SLOT_MASK_CAPTAIN
        false, // SLOT_MASK_GIANT
        true,  // SLOT_MASK_FIERCE_DEITY
    },
    // PLAYER_FORM_GORON
    {
        false, // SLOT_MASK_POSTMAN
        false, // SLOT_MASK_ALL_NIGHT
        false, // SLOT_MASK_BLAST
        false, // SLOT_MASK_STONE
        false, // SLOT_MASK_GREAT_FAIRY
        true,  // SLOT_MASK_DEKU
        false, // SLOT_MASK_KEATON
        false, // SLOT_MASK_BREMEN
        false, // SLOT_MASK_BUNNY
        false, // SLOT_MASK_DON_GERO
        false, // SLOT_MASK_SCENTS
        true,  // SLOT_MASK_GORON
        false, // SLOT_MASK_ROMANI
        false, // SLOT_MASK_CIRCUS_LEADER
        false, // SLOT_MASK_KAFEIS_MASK
        false, // SLOT_MASK_COUPLE
        false, // SLOT_MASK_TRUTH
        true,  // SLOT_MASK_ZORA
        false, // SLOT_MASK_KAMARO
        false, // SLOT_MASK_GIBDO
        false, // SLOT_MASK_GARO
        false, // SLOT_MASK_CAPTAIN
        false, // SLOT_MASK_GIANT
        true,  // SLOT_MASK_FIERCE_DEITY
    },
    // PLAYER_FORM_ZORA
    {
        false, // SLOT_MASK_POSTMAN
        false, // SLOT_MASK_ALL_NIGHT
        false, // SLOT_MASK_BLAST
        false, // SLOT_MASK_STONE
        false, // SLOT_MASK_GREAT_FAIRY
        true,  // SLOT_MASK_DEKU
        false, // SLOT_MASK_KEATON
        false, // SLOT_MASK_BREMEN
        false, // SLOT_MASK_BUNNY
        false, // SLOT_MASK_DON_GERO
        false, // SLOT_MASK_SCENTS
        true,  // SLOT_MASK_GORON
        false, // SLOT_MASK_ROMANI
        false, // SLOT_MASK_CIRCUS_LEADER
        false, // SLOT_MASK_KAFEIS_MASK
        false, // SLOT_MASK_COUPLE
        false, // SLOT_MASK_TRUTH
        true,  // SLOT_MASK_ZORA
        false, // SLOT_MASK_KAMARO
        false, // SLOT_MASK_GIBDO
        false, // SLOT_MASK_GARO
        false, // SLOT_MASK_CAPTAIN
        false, // SLOT_MASK_GIANT
        true,  // SLOT_MASK_FIERCE_DEITY
    },
    // PLAYER_FORM_DEKU
    {
        false, // SLOT_MASK_POSTMAN
        false, // SLOT_MASK_ALL_NIGHT
        false, // SLOT_MASK_BLAST
        false, // SLOT_MASK_STONE
        false, // SLOT_MASK_GREAT_FAIRY
        true,  // SLOT_MASK_DEKU
        false, // SLOT_MASK_KEATON
        false, // SLOT_MASK_BREMEN
        false, // SLOT_MASK_BUNNY
        false, // SLOT_MASK_DON_GERO
        false, // SLOT_MASK_SCENTS
        true,  // SLOT_MASK_GORON
        false, // SLOT_MASK_ROMANI
        false, // SLOT_MASK_CIRCUS_LEADER
        false, // SLOT_MASK_KAFEIS_MASK
        false, // SLOT_MASK_COUPLE
        false, // SLOT_MASK_TRUTH
        true,  // SLOT_MASK_ZORA
        false, // SLOT_MASK_KAMARO
        false, // SLOT_MASK_GIBDO
        false, // SLOT_MASK_GARO
        false, // SLOT_MASK_CAPTAIN
        false, // SLOT_MASK_GIANT
        true,  // SLOT_MASK_FIERCE_DEITY
    },
    // PLAYER_FORM_HUMAN
    {
        true, // SLOT_MASK_POSTMAN
        true, // SLOT_MASK_ALL_NIGHT
        true, // SLOT_MASK_BLAST
        true, // SLOT_MASK_STONE
        true, // SLOT_MASK_GREAT_FAIRY
        true, // SLOT_MASK_DEKU
        true, // SLOT_MASK_KEATON
        true, // SLOT_MASK_BREMEN
        true, // SLOT_MASK_BUNNY
        true, // SLOT_MASK_DON_GERO
        true, // SLOT_MASK_SCENTS
        true, // SLOT_MASK_GORON
        true, // SLOT_MASK_ROMANI
        true, // SLOT_MASK_CIRCUS_LEADER
        true, // SLOT_MASK_KAFEIS_MASK
        true, // SLOT_MASK_COUPLE
        true, // SLOT_MASK_TRUTH
        true, // SLOT_MASK_ZORA
        true, // SLOT_MASK_KAMARO
        true, // SLOT_MASK_GIBDO
        true, // SLOT_MASK_GARO
        true, // SLOT_MASK_CAPTAIN
        true, // SLOT_MASK_GIANT
        true, // SLOT_MASK_FIERCE_DEITY
    },
};

#define SET_MOON_MASK_BIT(masksGivenOnMoonIndex, masksGivenOnMoonFlag) \
    ((masksGivenOnMoonIndex) << 8 | (masksGivenOnMoonFlag))
#define CHECK_GIVEN_MASK_ON_MOON(maskIndex) \
    (gSaveContext.masksGivenOnMoon[sMasksGivenOnMoonBits[maskIndex] >> 8] & (u8)sMasksGivenOnMoonBits[maskIndex])

u16 sMasksGivenOnMoonBits[] = {
    SET_MOON_MASK_BIT(1, 0x1),  // SLOT_MASK_POSTMAN
    SET_MOON_MASK_BIT(0, 0x4),  // SLOT_MASK_ALL_NIGHT
    SET_MOON_MASK_BIT(2, 0x2),  // SLOT_MASK_BLAST
    SET_MOON_MASK_BIT(1, 0x80), // SLOT_MASK_STONE
    SET_MOON_MASK_BIT(1, 0x4),  // SLOT_MASK_GREAT_FAIRY
    SET_MOON_MASK_BIT(2, 0x10), // SLOT_MASK_DEKU
    SET_MOON_MASK_BIT(0, 0x10), // SLOT_MASK_KEATON
    SET_MOON_MASK_BIT(2, 0x1),  // SLOT_MASK_BREMEN
    SET_MOON_MASK_BIT(0, 0x8),  // SLOT_MASK_BUNNY
    SET_MOON_MASK_BIT(1, 0x10), // SLOT_MASK_DON_GERO
    SET_MOON_MASK_BIT(2, 0x4),  // SLOT_MASK_SCENTS
    SET_MOON_MASK_BIT(2, 0x20), // SLOT_MASK_GORON
    SET_MOON_MASK_BIT(0, 0x40), // SLOT_MASK_ROMANI
    SET_MOON_MASK_BIT(0, 0x80), // SLOT_MASK_CIRCUS_LEADER
    SET_MOON_MASK_BIT(0, 0x2),  // SLOT_MASK_KAFEIS_MASK
    SET_MOON_MASK_BIT(1, 0x2),  // SLOT_MASK_COUPLE
    SET_MOON_MASK_BIT(0, 0x1),  // SLOT_MASK_TRUTH
    SET_MOON_MASK_BIT(2, 0x40), // SLOT_MASK_ZORA
    SET_MOON_MASK_BIT(1, 0x20), // SLOT_MASK_KAMARO
    SET_MOON_MASK_BIT(1, 0x8),  // SLOT_MASK_GIBDO
    SET_MOON_MASK_BIT(0, 0x20), // SLOT_MASK_GARO
    SET_MOON_MASK_BIT(1, 0x40), // SLOT_MASK_CAPTAIN
    SET_MOON_MASK_BIT(2, 0x8),  // SLOT_MASK_GIANT
    SET_MOON_MASK_BIT(2, 0X80), // SLOT_MASK_FIERCE_DEITY
};

s16 sMaskMagicArrowEffectsR[] = { 255, 100, 255 };
s16 sMaskMagicArrowEffectsG[] = { 0, 100, 255 };
s16 sMaskMagicArrowEffectsB[] = { 0, 255, 100 };

void KaleidoScope_DrawMaskSelect(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 i;
    u16 j;

    OPEN_DISPS(play->state.gfxCtx);

    KaleidoScope_SetCursorVtxPos(pauseCtx, pauseCtx->cursorSlot[PAUSE_MASK] * 4, pauseCtx->maskVtx);

    Gfx_SetupDL42_Opa(play->state.gfxCtx);

    // Draw a white box around the items that are equipped on the C buttons
    // Loop over c-buttons (i) and vtx offset (j)
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    for (i = 0, j = MASK_NUM_SLOTS * 4; i < 3; i++, j += 4) {
        if (GET_CUR_FORM_BTN_ITEM(i + 1) != ITEM_NONE) {
            ItemId item = GET_CUR_FORM_BTN_ITEM(i + 1);
            if (GameInteractor_Should(VB_DRAW_ITEM_EQUIPPED_OUTLINE, (GET_CUR_FORM_BTN_SLOT(i + 1) >= ITEM_NUM_SLOTS),
                                      &item, (s32)(i + 1), 0, PAUSE_MASK)) {
                gSPVertex(POLY_OPA_DISP++, &pauseCtx->maskVtx[j], 4, 0);
                POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gEquippedItemOutlineTex, 32, 32, 0);
            }
        }
    }
    // #region 2S2H [Dpad]
    if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
        for (i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++, j += 4) {
            if (DPAD_GET_CUR_FORM_BTN_ITEM(i) != ITEM_NONE) {
                ItemId item = DPAD_GET_CUR_FORM_BTN_ITEM(i);
                if (GameInteractor_Should(VB_DRAW_ITEM_EQUIPPED_OUTLINE,
                                          (DPAD_GET_CUR_FORM_BTN_SLOT(i) >= ITEM_NUM_SLOTS), &item, (s32)(i), 1,
                                          PAUSE_MASK)) {
                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->maskVtx[j], 4, 0);
                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, gEquippedItemOutlineTex, 32, 32, 0);
                }
            }
        }
    }
    // #endregion

    gDPPipeSync(POLY_OPA_DISP++);

    // Draw the item icons
    // Loop over slots (i) and vtx offset (j)
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    for (j = 0, i = 0; i < MASK_NUM_SLOTS; i++, j += 4) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (((void)0, gSaveContext.save.saveInfo.inventory.items[i + ITEM_NUM_SLOTS]) != ITEM_NONE) {
            if (!CHECK_GIVEN_MASK_ON_MOON(i)) {
                if ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->pageIndex == PAUSE_MASK) &&
                    (pauseCtx->cursorSpecialPos == 0) && gMaskPlayerFormSlotRestrictions[GET_PLAYER_FORM][i]) {
                    if ((sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) && (i == SLOT_ARROW_ICE)) {
                        // Possible bug:
                        // Supposed to be `SLOT_BOW`, unchanged from OoT, instead increase size of ice arrow icon
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0,
                                        sMaskMagicArrowEffectsR[pauseCtx->equipTargetItem - 0xB5],
                                        sMaskMagicArrowEffectsG[pauseCtx->equipTargetItem - 0xB5],
                                        sMaskMagicArrowEffectsB[pauseCtx->equipTargetItem - 0xB5], pauseCtx->alpha);

                        pauseCtx->maskVtx[j + 0].v.ob[0] = pauseCtx->maskVtx[j + 2].v.ob[0] =
                            pauseCtx->maskVtx[j + 0].v.ob[0] - 2;
                        pauseCtx->maskVtx[j + 1].v.ob[0] = pauseCtx->maskVtx[j + 3].v.ob[0] =
                            pauseCtx->maskVtx[j + 0].v.ob[0] + 32;
                        pauseCtx->maskVtx[j + 0].v.ob[1] = pauseCtx->maskVtx[j + 1].v.ob[1] =
                            pauseCtx->maskVtx[j + 0].v.ob[1] + 2;
                        pauseCtx->maskVtx[j + 2].v.ob[1] = pauseCtx->maskVtx[j + 3].v.ob[1] =
                            pauseCtx->maskVtx[j + 0].v.ob[1] - 32;

                    } else if (i == pauseCtx->cursorSlot[PAUSE_MASK]) {
                        // Increase the size of the selected item
                        pauseCtx->maskVtx[j + 0].v.ob[0] = pauseCtx->maskVtx[j + 2].v.ob[0] =
                            pauseCtx->maskVtx[j + 0].v.ob[0] - 2;
                        pauseCtx->maskVtx[j + 1].v.ob[0] = pauseCtx->maskVtx[j + 3].v.ob[0] =
                            pauseCtx->maskVtx[j + 0].v.ob[0] + 32;
                        pauseCtx->maskVtx[j + 0].v.ob[1] = pauseCtx->maskVtx[j + 1].v.ob[1] =
                            pauseCtx->maskVtx[j + 0].v.ob[1] + 2;
                        pauseCtx->maskVtx[j + 2].v.ob[1] = pauseCtx->maskVtx[j + 3].v.ob[1] =
                            pauseCtx->maskVtx[j + 0].v.ob[1] - 32;
                    }
                }

                gSPVertex(POLY_OPA_DISP++, &pauseCtx->maskVtx[j + 0], 4, 0);
                KaleidoScope_DrawTexQuadRGBA32(
                    play->state.gfxCtx,
                    gItemIcons[((void)0, gSaveContext.save.saveInfo.inventory.items[i + ITEM_NUM_SLOTS])], 32, 32, 0);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u8 sMaskPlayerFormItems[PLAYER_FORM_MAX] = {
    ITEM_MASK_FIERCE_DEITY, // PLAYER_FORM_FIERCE_DEITY
    ITEM_MASK_GORON,        // PLAYER_FORM_GORON
    ITEM_MASK_ZORA,         // PLAYER_FORM_ZORA
    ITEM_MASK_DEKU,         // PLAYER_FORM_DEKU
    ITEM_NONE,              // PLAYER_FORM_HUMAN
};

void KaleidoScope_UpdateMaskCursor(PlayState* play) {
    Input* input = CONTROLLER1(&play->state);
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    u16 vtxIndex;
    u16 cursorItem;
    u16 cursorSlot;
    s16 cursorPoint;
    s16 cursorXIndex;
    s16 cursorYIndex;
    s16 oldCursorPoint;
    s16 moveCursorResult;
    s16 pad2;

    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
    pauseCtx->nameColorSet = PAUSE_NAME_COLOR_SET_WHITE;

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MASK) && !pauseCtx->itemDescriptionOn) {
        moveCursorResult = PAUSE_CURSOR_RESULT_NONE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_MASK];

        cursorItem = pauseCtx->cursorItem[PAUSE_MASK];

        // Move cursor left/right
        if (pauseCtx->cursorSpecialPos == 0) {
            // cursor is currently on a slot
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;

            if (ABS_ALT(pauseCtx->stickAdjX) > 30) {
                cursorPoint = pauseCtx->cursorPoint[PAUSE_MASK];
                cursorXIndex = pauseCtx->cursorXIndex[PAUSE_MASK];
                cursorYIndex = pauseCtx->cursorYIndex[PAUSE_MASK];

                // Search for slot to move to
                while (moveCursorResult == PAUSE_CURSOR_RESULT_NONE) {
                    if (pauseCtx->stickAdjX < -30) {
                        // move cursor left
                        pauseCtx->cursorShrinkRate = 4.0f;
                        if (pauseCtx->cursorXIndex[PAUSE_MASK] != 0) {
                            pauseCtx->cursorXIndex[PAUSE_MASK]--;
                            pauseCtx->cursorPoint[PAUSE_MASK]--;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_MASK]++;

                            if (pauseCtx->cursorYIndex[PAUSE_MASK] >= 4) {
                                pauseCtx->cursorYIndex[PAUSE_MASK] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_MASK] =
                                pauseCtx->cursorXIndex[PAUSE_MASK] + (pauseCtx->cursorYIndex[PAUSE_MASK] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_MASK] >= MASK_NUM_SLOTS) {
                                pauseCtx->cursorPoint[PAUSE_MASK] = pauseCtx->cursorXIndex[PAUSE_MASK];
                            }

                            if (cursorYIndex == pauseCtx->cursorYIndex[PAUSE_MASK]) {
                                pauseCtx->cursorXIndex[PAUSE_MASK] = cursorXIndex;
                                pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);

                                moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                            }
                        }
                    } else if (pauseCtx->stickAdjX > 30) {
                        // move cursor right
                        pauseCtx->cursorShrinkRate = 4.0f;
                        if (pauseCtx->cursorXIndex[PAUSE_MASK] <= 4) {
                            pauseCtx->cursorXIndex[PAUSE_MASK]++;
                            pauseCtx->cursorPoint[PAUSE_MASK]++;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_MASK]++;

                            if (pauseCtx->cursorYIndex[PAUSE_MASK] >= 4) {
                                pauseCtx->cursorYIndex[PAUSE_MASK] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_MASK] =
                                pauseCtx->cursorXIndex[PAUSE_MASK] + (pauseCtx->cursorYIndex[PAUSE_MASK] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_MASK] >= MASK_NUM_SLOTS) {
                                pauseCtx->cursorPoint[PAUSE_MASK] = pauseCtx->cursorXIndex[PAUSE_MASK];
                            }

                            if (cursorYIndex == pauseCtx->cursorYIndex[PAUSE_MASK]) {
                                pauseCtx->cursorXIndex[PAUSE_MASK] = cursorXIndex;
                                pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);

                                moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                            }
                        }
                    }
                }

                if (moveCursorResult == PAUSE_CURSOR_RESULT_SLOT) {
                    cursorItem =
                        gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_MASK] + ITEM_NUM_SLOTS];
                    if (CHECK_GIVEN_MASK_ON_MOON(pauseCtx->cursorPoint[PAUSE_MASK])) {
                        cursorItem = ITEM_NONE;
                    }
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                KaleidoScope_MoveCursorFromSpecialPos(play);
                cursorYIndex = 0;
                cursorXIndex = 0;
                cursorPoint = 0; // top row, left column (SLOT_MASK_POSTMAN)

                // Search for slot to move to
                while (true) {
                    // Check if current cursor has an item in its slot
                    if ((gSaveContext.save.saveInfo.inventory.items[cursorPoint + ITEM_NUM_SLOTS] != ITEM_NONE) &&
                        !CHECK_GIVEN_MASK_ON_MOON(cursorPoint)) {
                        pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;
                        pauseCtx->cursorXIndex[PAUSE_MASK] = cursorXIndex;
                        pauseCtx->cursorYIndex[PAUSE_MASK] = cursorYIndex;
                        moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        break;
                    }

                    // move 1 row down and retry
                    cursorYIndex++;
                    cursorPoint += 6;
                    if (cursorYIndex < 4) {
                        continue;
                    }

                    // move 1 column right and retry
                    cursorYIndex = 0;
                    cursorPoint = cursorXIndex + 1;
                    cursorXIndex = cursorPoint;
                    if (cursorXIndex < 6) {
                        continue;
                    }

                    // No item available
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    break;
                }
            }
        } else { // PAUSE_CURSOR_PAGE_RIGHT
            //! FAKE:
            if (1) {}
            if (pauseCtx->stickAdjX < -30) {
                KaleidoScope_MoveCursorFromSpecialPos(play);
                cursorXIndex = 5;
                cursorPoint = 5; // top row, right column (SLOT_MASK_DEKU)
                cursorYIndex = 0;

                // Search for slot to move to
                while (true) {
                    // Check if current cursor has an item in its slot
                    if ((gSaveContext.save.saveInfo.inventory.items[cursorPoint + ITEM_NUM_SLOTS] != ITEM_NONE) &&
                        !CHECK_GIVEN_MASK_ON_MOON(cursorPoint)) {
                        pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;
                        pauseCtx->cursorXIndex[PAUSE_MASK] = cursorXIndex;
                        pauseCtx->cursorYIndex[PAUSE_MASK] = cursorYIndex;
                        moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        break;
                    }

                    // move 1 row down and retry
                    cursorYIndex++;
                    cursorPoint += 6;
                    if (cursorYIndex < 4) {
                        continue;
                    }

                    // move 1 column left and retry
                    cursorYIndex = 0;
                    cursorPoint = cursorXIndex - 1;
                    cursorXIndex = cursorPoint;
                    if (cursorXIndex >= 0) {
                        continue;
                    }

                    // No item available
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    break;
                }
            }
        }

        if (pauseCtx->cursorSpecialPos == 0) {
            // move cursor up/down
            if (ABS_ALT(pauseCtx->stickAdjY) > 30) {
                moveCursorResult = PAUSE_CURSOR_RESULT_NONE;

                cursorPoint = pauseCtx->cursorPoint[PAUSE_MASK];
                cursorYIndex = pauseCtx->cursorYIndex[PAUSE_MASK];

                while (moveCursorResult == PAUSE_CURSOR_RESULT_NONE) {
                    if (pauseCtx->stickAdjY > 30) {
                        // move cursor up
                        moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                        if (pauseCtx->cursorYIndex[PAUSE_MASK] != 0) {
                            pauseCtx->cursorShrinkRate = 4.0f;
                            pauseCtx->cursorYIndex[PAUSE_MASK]--;
                            pauseCtx->cursorPoint[PAUSE_MASK] -= 6;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_MASK] = cursorYIndex;
                            pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;
                        }
                    } else if (pauseCtx->stickAdjY < -30) {
                        // move cursor down
                        moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                        if (pauseCtx->cursorYIndex[PAUSE_MASK] < 3) {
                            pauseCtx->cursorShrinkRate = 4.0f;
                            pauseCtx->cursorYIndex[PAUSE_MASK]++;
                            pauseCtx->cursorPoint[PAUSE_MASK] += 6;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_MASK] = cursorYIndex;
                            pauseCtx->cursorPoint[PAUSE_MASK] = cursorPoint;
                        }
                    }
                }
            }

            cursorSlot = pauseCtx->cursorPoint[PAUSE_MASK];
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;

            if (moveCursorResult == PAUSE_CURSOR_RESULT_SLOT) {
                cursorItem =
                    gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_MASK] + ITEM_NUM_SLOTS];
                if (CHECK_GIVEN_MASK_ON_MOON(pauseCtx->cursorPoint[PAUSE_MASK])) {
                    cursorItem = ITEM_NONE;
                }
            } else if (moveCursorResult != PAUSE_CURSOR_RESULT_SPECIAL_POS) {
                cursorItem =
                    gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_MASK] + ITEM_NUM_SLOTS];
                if (CHECK_GIVEN_MASK_ON_MOON(pauseCtx->cursorPoint[PAUSE_MASK])) {
                    cursorItem = ITEM_NONE;
                }
            }

            if (cursorItem == ITEM_NONE) {
                cursorItem = PAUSE_ITEM_NONE;
                pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
            }

            if ((cursorItem != PAUSE_ITEM_NONE) && (msgCtx->msgLength == 0)) {
                if (gSaveContext.buttonStatus[EQUIP_SLOT_A] == BTN_DISABLED) {
                    gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                    gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                    Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
                }
            } else if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_DISABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_DISABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }

            pauseCtx->cursorItem[PAUSE_MASK] = cursorItem;
            pauseCtx->cursorSlot[PAUSE_MASK] = cursorSlot;
            if (cursorItem != PAUSE_ITEM_NONE) {
                // Equip item to the C buttons
                if ((pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && !pauseCtx->itemDescriptionOn &&
                    (pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
                    CHECK_BTN_ANY(input->press.button, BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT | BTN_DPAD_EQUIP)) {

                    // Ensure that a mask is not unequipped while being used
                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                             (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT))) ||
                            ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                             (sMaskPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT)))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                        if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                             (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN))) ||
                            ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                             (sMaskPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN)))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                             (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT))) ||
                            ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                             (sMaskPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT)))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                    }
                    // #region 2S2H [Dpad]
                    else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                            if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                 (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT))) ||
                                ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                                 (sMaskPlayerFormItems[GET_PLAYER_FORM] ==
                                  DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT)))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                            if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                 (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT))) ||
                                ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                                 (sMaskPlayerFormItems[GET_PLAYER_FORM] ==
                                  DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT)))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                            if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                 (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN))) ||
                                ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                                 (sMaskPlayerFormItems[GET_PLAYER_FORM] ==
                                  DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN)))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                            if (((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                 (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP))) ||
                                ((sMaskPlayerFormItems[GET_PLAYER_FORM] != ITEM_NONE) &&
                                 (sMaskPlayerFormItems[GET_PLAYER_FORM] ==
                                  DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP)))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        }
                    }
                    // #endregion

                    if ((Player_GetEnvironmentalHazard(play) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) &&
                        (Player_GetEnvironmentalHazard(play) <= PLAYER_ENV_HAZARD_UNDERWATER_FREE) &&
                        ((cursorSlot == (SLOT_MASK_DEKU - ITEM_NUM_SLOTS)) ||
                         (cursorSlot == (SLOT_MASK_GORON - ITEM_NUM_SLOTS)))) {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        return;
                    }

                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_LEFT;
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_DOWN;
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_RIGHT;
                    }
                    // #region 2S2H [Dpad]
                    else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_RIGHT;
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_LEFT;
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_DOWN;
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_UP;
                        }
                    }
                    // #endregion

                    if (!GameInteractor_Should(VB_KALEIDO_EQUIP_ITEM_TO_BUTTON, true, cursorSlot + ITEM_NUM_SLOTS,
                                               cursorItem)) {
                        return;
                    }

                    // Equip item to the C buttons
                    pauseCtx->equipTargetItem = cursorItem;
                    pauseCtx->equipTargetSlot = cursorSlot + ITEM_NUM_SLOTS;
                    pauseCtx->mainState = PAUSE_MAIN_STATE_EQUIP_MASK;
                    vtxIndex = cursorSlot * 4;
                    pauseCtx->equipAnimX = pauseCtx->maskVtx[vtxIndex].v.ob[0] * 10;
                    pauseCtx->equipAnimY = pauseCtx->maskVtx[vtxIndex].v.ob[1] * 10;
                    pauseCtx->equipAnimAlpha = 255;
                    sMaskEquipMagicArrowSlotHoldTimer = 0;
                    sMaskEquipState = EQUIP_STATE_MOVE_TO_C_BTN;
                    sMaskEquipAnimTimer = 10;
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                } else if ((pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
                           (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
                           CHECK_BTN_ALL(input->press.button, BTN_A) && (msgCtx->msgLength == 0)) {
                    if (GameInteractor_Should(VB_KALEIDO_DISPLAY_ITEM_TEXT, true, &pauseCtx->cursorItem[PAUSE_MASK])) {
                        // Give description on item through a message box
                        pauseCtx->itemDescriptionOn = true;
                        if (pauseCtx->cursorYIndex[PAUSE_MASK] < 2) {
                            func_801514B0(play, 0x1700 + pauseCtx->cursorItem[PAUSE_MASK], 3);
                        } else {
                            func_801514B0(play, 0x1700 + pauseCtx->cursorItem[PAUSE_MASK], 1);
                        }
                    }
                }
            }
        } else {
            pauseCtx->cursorItem[PAUSE_MASK] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_MASK]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((pauseCtx->mainState == PAUSE_MAIN_STATE_EQUIP_MASK) && (pauseCtx->pageIndex == PAUSE_MASK)) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;
    }
}

s16 sMaskCButtonPosX[] = {
    660, 900, 1140,
    // #region 2S2H
    1350, 1030, 1190, 1190
    // #endregion
};
s16 sMaskCButtonPosY[] = {
    1100, 920, 1100,
    // #region 2S2H
    570, 570, 410, 730
    // #endregion
};

// #region 2S2H [Dpad]
void KaleidoScope_SwapDpadMaskToCMask(PlayState* play, EquipSlot cEquipSlot) {
    PauseContext* pauseCtx = &play->pauseCtx;

    if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = C_SLOT_EQUIP(0, cEquipSlot);
            Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = C_SLOT_EQUIP(0, cEquipSlot);
            Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = C_SLOT_EQUIP(0, cEquipSlot);
            Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = C_SLOT_EQUIP(0, cEquipSlot);
            Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
        }
    }
}

void KaleidoScope_UpdateDpadMaskEquip(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_RIGHT) {
        // Swap if mask is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Equip mask on DRight
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_RIGHT);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_LEFT) {
        // Swap if mask is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Equip mask on DLeft
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_LEFT);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_DOWN) {
        // Swap if mask is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Equip mask on DDown
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_DOWN);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_UP) {
        // Swap if mask is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        }

        // Equip mask on DUp
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_UP);
    }
}
// #endregion

void KaleidoScope_UpdateMaskEquip(PlayState* play) {
    static s16 sMaskEquipMagicArrowBowSlotHoldTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

    // Grow glowing orb when equipping magic arrows
    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_GROW_ORB) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
            sMaskEquipState++;
        }
        // Hover over magic arrow slot when the next state is reached
        sMaskEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) {
        sMaskEquipMagicArrowBowSlotHoldTimer--;

        if (sMaskEquipMagicArrowBowSlotHoldTimer == 0) {
            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
            pauseCtx->equipTargetSlot = SLOT_BOW;
            sMaskEquipAnimTimer = 6;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
            sMaskEquipState++;
            Audio_PlaySfx(NA_SE_SY_SYNTH_MAGIC_ARROW);
        }
        return;
    }

    // #region 2S2H [Cosmetic] Track the C button position vanilla values or HUD editor adjusted values
    s16 maskCButtonPosX = sMaskCButtonPosX[pauseCtx->equipTargetCBtn];
    s16 maskCButtonPosY = sMaskCButtonPosY[pauseCtx->equipTargetCBtn];

    HudEditor_SetActiveElement(pauseCtx->equipTargetCBtn < 3 ? HUD_EDITOR_ELEMENT_C_LEFT + pauseCtx->equipTargetCBtn
                                                             : HUD_EDITOR_ELEMENT_D_PAD);

    if (sMaskEquipState == EQUIP_STATE_MOVE_TO_C_BTN && HudEditor_ShouldOverrideDraw()) {
        s16 equipAnimShrinkRate = 40;
        HudEditor_ModifyKaleidoEquipAnimValues(&maskCButtonPosX, &maskCButtonPosY, &equipAnimShrinkRate);

        // Override the anim shrink rate at the beginning (value is 320)
        if (pauseCtx->equipAnimScale == 320) {
            pauseCtx->equipAnimShrinkRate = equipAnimShrinkRate;
        }

        if (CVarGetInteger(hudEditorElements[hudEditorActiveElement].modeCvar, HUD_EDITOR_ELEMENT_MODE_VANILLA) ==
            HUD_EDITOR_ELEMENT_MODE_HIDDEN) {
            pauseCtx->equipAnimScale = 0;
            pauseCtx->equipAnimShrinkRate = 0;
        }
    } else if (sMaskEquipState == EQUIP_STATE_MOVE_TO_C_BTN && pauseCtx->equipTargetCBtn >= 3) {
        // Equips to DPad need to shrink fast to be have a final smaller size (16px),
        // So we override the anim shrink rate at the beginning (value is 320)
        if (pauseCtx->equipAnimScale == 320) {
            pauseCtx->equipAnimShrinkRate = 160;
        }
    }

    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_NONE);
    // #endregion

    if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
        //! Note: Copied from OoT when `SLOT_BOW` was still valued at 3.
        // Due to a shift, `SLOT_ARROW_ICE` now occupies slot 3 but this value was not updated
        // Block is never reached as you can not equip magic arrows from the mask page
        bowItemVtx = &pauseCtx->itemVtx[SLOT_ARROW_ICE * 4];
        offsetX = ABS_ALT(pauseCtx->equipAnimX - bowItemVtx->v.ob[0] * 10) / sMaskEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - bowItemVtx->v.ob[1] * 10) / sMaskEquipAnimTimer;
    } else {
        // 2S2H [Cosmetic] Use position vars from above
        offsetX = ABS_ALT(pauseCtx->equipAnimX - maskCButtonPosX) / sMaskEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - maskCButtonPosY) / sMaskEquipAnimTimer;
    }

    if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipAnimAlpha < 254)) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
        }
        sMaskEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sMaskEquipMagicArrowSlotHoldTimer == 0) {
        pauseCtx->equipAnimScale -= pauseCtx->equipAnimShrinkRate / sMaskEquipAnimTimer;
        pauseCtx->equipAnimShrinkRate -= pauseCtx->equipAnimShrinkRate / sMaskEquipAnimTimer;

        // Update coordinates of item icon while being equipped
        if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
            // target is the bow slot
            if (pauseCtx->equipAnimX >= (pauseCtx->itemVtx[12].v.ob[0] * 10)) {
                pauseCtx->equipAnimX -= offsetX;
            } else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= (pauseCtx->itemVtx[12].v.ob[1] * 10)) {
                pauseCtx->equipAnimY -= offsetY;
            } else {
                pauseCtx->equipAnimY += offsetY;
            }
        } else {
            // target is the c button
            // 2S2H [Cosmetic] Use position vars from above
            if (pauseCtx->equipAnimX >= maskCButtonPosX) {
                pauseCtx->equipAnimX -= offsetX;
            } else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= maskCButtonPosY) {
                pauseCtx->equipAnimY -= offsetY;
            } else {
                pauseCtx->equipAnimY += offsetY;
            }
        }

        sMaskEquipAnimTimer--;
        if (sMaskEquipAnimTimer == 0) {
            if (sMaskEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
                sMaskEquipState++;
                sMaskEquipMagicArrowBowSlotHoldTimer = 4;
                return;
            }

            // Equip mask onto c buttons
            if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_LEFT) {
                // Swap if mask is already equipped on CDown or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadMaskToCMask(play, EQUIP_SLOT_C_LEFT);
                // #endregion

                // Equip mask on CLeft
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_LEFT);
            } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_DOWN) {
                // Swap if mask is already equipped on CLeft or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadMaskToCMask(play, EQUIP_SLOT_C_DOWN);
                // #endregion

                // Equip mask on CDown
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_DOWN);
            } else if (pauseCtx->equipTargetCBtn ==
                       PAUSE_EQUIP_C_RIGHT) { // #region 2S2H [Dpad] Added condition here to allow for other cases
                // Swap if mask is already equipped on CLeft or CDown.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadMaskToCMask(play, EQUIP_SLOT_C_RIGHT);
                // #endregion

                // Equip mask on CRight
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_RIGHT);
            }
            // #region 2S2H [Dpad]
            else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                KaleidoScope_UpdateDpadMaskEquip(play);
            }
            // #endregion

            // Reset params
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            sMaskEquipAnimTimer = 10;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
        }
    } else {
        sMaskEquipMagicArrowSlotHoldTimer--;
        if (sMaskEquipMagicArrowSlotHoldTimer == 0) {
            pauseCtx->equipAnimAlpha = 255;
        }
    }
}
