/*
 * File: z_kaleido_item.c
 * Overlay: ovl_kaleido_scope
 * Description: Pause Menu - Item Page
 */

#include "z_kaleido_scope.h"
#include "interface/parameter_static/parameter_static.h"

#include "2s2h/BenGui/HudEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

s16 sEquipState = EQUIP_STATE_MAGIC_ARROW_GROW_ORB;

// Timer to hold magic arrow icon over magic arrow slot before moving when equipping.
s16 sEquipMagicArrowSlotHoldTimer = 0;

// Number of frames to move icon from slot to target position when equipping.
s16 sEquipAnimTimer = 10;

u8 gPlayerFormSlotRestrictions[PLAYER_FORM_MAX][ITEM_NUM_SLOTS] = {
    // PLAYER_FORM_FIERCE_DEITY
    {
        false, // SLOT_OCARINA
        false, // SLOT_BOW
        false, // SLOT_ARROW_FIRE
        false, // SLOT_ARROW_ICE
        false, // SLOT_ARROW_LIGHT
        false, // SLOT_TRADE_DEED
        false, // SLOT_BOMB
        false, // SLOT_BOMBCHU
        false, // SLOT_DEKU_STICK
        false, // SLOT_DEKU_NUT
        false, // SLOT_MAGIC_BEANS
        false, // SLOT_TRADE_KEY_MAMA
        false, // SLOT_POWDER_KEG
        false, // SLOT_PICTOGRAPH_BOX
        false, // SLOT_LENS_OF_TRUTH
        false, // SLOT_HOOKSHOT
        false, // SLOT_SWORD_GREAT_FAIRY
        false, // SLOT_TRADE_COUPLE
        true,  // SLOT_BOTTLE_1
        true,  // SLOT_BOTTLE_2
        true,  // SLOT_BOTTLE_3
        true,  // SLOT_BOTTLE_4
        true,  // SLOT_BOTTLE_5
        true,  // SLOT_BOTTLE_6
    },
    // PLAYER_FORM_GORON
    {
        true,  // SLOT_OCARINA
        false, // SLOT_BOW
        false, // SLOT_ARROW_FIRE
        false, // SLOT_ARROW_ICE
        false, // SLOT_ARROW_LIGHT
        true,  // SLOT_TRADE_DEED
        false, // SLOT_BOMB
        false, // SLOT_BOMBCHU
        false, // SLOT_DEKU_STICK
        false, // SLOT_DEKU_NUT
        false, // SLOT_MAGIC_BEANS
        true,  // SLOT_TRADE_KEY_MAMA
        true,  // SLOT_POWDER_KEG
        true,  // SLOT_PICTOGRAPH_BOX
        true,  // SLOT_LENS_OF_TRUTH
        false, // SLOT_HOOKSHOT
        false, // SLOT_SWORD_GREAT_FAIRY
        true,  // SLOT_TRADE_COUPLE
        true,  // SLOT_BOTTLE_1
        true,  // SLOT_BOTTLE_2
        true,  // SLOT_BOTTLE_3
        true,  // SLOT_BOTTLE_4
        true,  // SLOT_BOTTLE_5
        true,  // SLOT_BOTTLE_6
    },
    // PLAYER_FORM_ZORA
    {
        true,  // SLOT_OCARINA
        false, // SLOT_BOW
        false, // SLOT_ARROW_FIRE
        false, // SLOT_ARROW_ICE
        false, // SLOT_ARROW_LIGHT
        true,  // SLOT_TRADE_DEED
        false, // SLOT_BOMB
        false, // SLOT_BOMBCHU
        false, // SLOT_DEKU_STICK
        false, // SLOT_DEKU_NUT
        false, // SLOT_MAGIC_BEANS
        true,  // SLOT_TRADE_KEY_MAMA
        false, // SLOT_POWDER_KEG
        true,  // SLOT_PICTOGRAPH_BOX
        true,  // SLOT_LENS_OF_TRUTH
        false, // SLOT_HOOKSHOT
        false, // SLOT_SWORD_GREAT_FAIRY
        true,  // SLOT_TRADE_COUPLE
        true,  // SLOT_BOTTLE_1
        true,  // SLOT_BOTTLE_2
        true,  // SLOT_BOTTLE_3
        true,  // SLOT_BOTTLE_4
        true,  // SLOT_BOTTLE_5
        true,  // SLOT_BOTTLE_6
    },
    // PLAYER_FORM_DEKU
    {
        true,  // SLOT_OCARINA
        false, // SLOT_BOW
        false, // SLOT_ARROW_FIRE
        false, // SLOT_ARROW_ICE
        false, // SLOT_ARROW_LIGHT
        true,  // SLOT_TRADE_DEED
        false, // SLOT_BOMB
        false, // SLOT_BOMBCHU
        false, // SLOT_DEKU_STICK
        true,  // SLOT_DEKU_NUT
        false, // SLOT_MAGIC_BEANS
        true,  // SLOT_TRADE_KEY_MAMA
        false, // SLOT_POWDER_KEG
        true,  // SLOT_PICTOGRAPH_BOX
        true,  // SLOT_LENS_OF_TRUTH
        false, // SLOT_HOOKSHOT
        false, // SLOT_SWORD_GREAT_FAIRY
        true,  // SLOT_TRADE_COUPLE
        true,  // SLOT_BOTTLE_1
        true,  // SLOT_BOTTLE_2
        true,  // SLOT_BOTTLE_3
        true,  // SLOT_BOTTLE_4
        true,  // SLOT_BOTTLE_5
        true,  // SLOT_BOTTLE_6
    },
    // PLAYER_FORM_HUMAN
    {
        true,  // SLOT_OCARINA
        true,  // SLOT_BOW
        true,  // SLOT_ARROW_FIRE
        true,  // SLOT_ARROW_ICE
        true,  // SLOT_ARROW_LIGHT
        true,  // SLOT_TRADE_DEED
        true,  // SLOT_BOMB
        true,  // SLOT_BOMBCHU
        true,  // SLOT_DEKU_STICK
        true,  // SLOT_DEKU_NUT
        true,  // SLOT_MAGIC_BEANS
        true,  // SLOT_TRADE_KEY_MAMA
        false, // SLOT_POWDER_KEG
        true,  // SLOT_PICTOGRAPH_BOX
        true,  // SLOT_LENS_OF_TRUTH
        true,  // SLOT_HOOKSHOT
        true,  // SLOT_SWORD_GREAT_FAIRY
        true,  // SLOT_TRADE_COUPLE
        true,  // SLOT_BOTTLE_1
        true,  // SLOT_BOTTLE_2
        true,  // SLOT_BOTTLE_3
        true,  // SLOT_BOTTLE_4
        true,  // SLOT_BOTTLE_5
        true,  // SLOT_BOTTLE_6
    },
};

s16 sAmmoRectLeft[] = {
    95,  // SLOT_BOW
    62,  // SLOT_BOMB
    95,  // SLOT_BOMBCHU
    128, // SLOT_DEKU_STICK
    161, // SLOT_DEKU_NUT
    194, // SLOT_MAGIC_BEANS
    62,  // SLOT_POWDER_KEG
    95,  // SLOT_PICTOGRAPH_BOX
};

s16 sAmmoRectHeight[] = {
    85,  // SLOT_BOW
    117, // SLOT_BOMB
    117, // SLOT_BOMBCHU
    117, // SLOT_DEKU_STICK
    117, // SLOT_DEKU_NUT
    117, // SLOT_MAGIC_BEANS
    150, // SLOT_POWDER_KEG
    150, // SLOT_PICTOGRAPH_BOX
};

extern const char* gAmmoDigitTextures[10];

void KaleidoScope_DrawAmmoCount(PauseContext* pauseCtx, GraphicsContext* gfxCtx, s16 item, u16 ammoIndex) {
    s16 ammoUpperDigit;
    s16 ammo;

    if (!GameInteractor_Should(VB_KALEIDO_DRAW_AMMO_COUNT, true, pauseCtx, gfxCtx, item, ammoIndex)) {
        return;
    }

    OPEN_DISPS(gfxCtx);

    if (item == ITEM_PICTOGRAPH_BOX) {
        if (!CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
            ammo = 0;
        } else {
            ammo = 1;
        }
    } else {
        ammo = AMMO(item);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    if (!gPlayerFormSlotRestrictions[GET_PLAYER_FORM][SLOT(item)]) {
        // Ammo item is restricted
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 100, 100, pauseCtx->alpha);
    } else {
        // Default ammo
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        if (ammo == 0) {
            // Out of ammo
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 130, 130, 130, pauseCtx->alpha);
        } else if (((item == ITEM_BOMB) && (AMMO(item) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
                   ((item == ITEM_BOW) && (AMMO(item) == CUR_CAPACITY(UPG_QUIVER))) ||
                   ((item == ITEM_DEKU_STICK) && (AMMO(item) == CUR_CAPACITY(UPG_DEKU_STICKS))) ||
                   ((item == ITEM_DEKU_NUT) && (AMMO(item) == CUR_CAPACITY(UPG_DEKU_NUTS))) ||
                   ((item == ITEM_BOMBCHU) && (AMMO(item) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
                   ((item == ITEM_POWDER_KEG) && GameInteractor_Should(VB_POWDER_KEG_AMMO_AT_CAPACITY, ammo == 1)) ||
                   ((item == ITEM_PICTOGRAPH_BOX) && (ammo == 1)) || ((item == ITEM_MAGIC_BEANS) && (ammo == 20))) {
            // Ammo at capacity
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 255, 0, pauseCtx->alpha);
        }
    }

    // Separate ammo into upper and lower digits
    for (ammoUpperDigit = 0; ammo >= 10; ammoUpperDigit++) {
        ammo -= 10;
    }

    gDPPipeSync(POLY_OPA_DISP++);

    // Draw upper digit
    if (ammoUpperDigit != 0) {
        POLY_OPA_DISP =
            Gfx_DrawTexRectIA8(POLY_OPA_DISP, gAmmoDigitTextures[ammoUpperDigit], 8, 8, sAmmoRectLeft[ammoIndex],
                               sAmmoRectHeight[ammoIndex], 8, 8, 1 << 10, 1 << 10);
    }

    // Draw lower digit
    POLY_OPA_DISP = Gfx_DrawTexRectIA8(POLY_OPA_DISP, gAmmoDigitTextures[ammo], 8, 8, sAmmoRectLeft[ammoIndex] + 6,
                                       sAmmoRectHeight[ammoIndex], 8, 8, 1 << 10, 1 << 10);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_SetCursorVtxPos(PauseContext* pauseCtx, u16 vtxIndex, Vtx* vtx) {
    pauseCtx->cursorVtx[0].v.ob[0] = vtx[vtxIndex].v.ob[0];
    pauseCtx->cursorVtx[0].v.ob[1] = vtx[vtxIndex].v.ob[1];
}

static s16 sMagicArrowEffectsR[] = { 255, 100, 255 };
static s16 sMagicArrowEffectsG[] = { 0, 100, 255 };
static s16 sMagicArrowEffectsB[] = { 0, 255, 100 };

void KaleidoScope_DrawItemSelect(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 i;
    u16 j;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL42_Opa(play->state.gfxCtx);

    // Draw a white box around the items that are equipped on the C buttons
    // Loop over c-buttons (i) and vtx offset (j)
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    for (i = 0, j = ITEM_NUM_SLOTS * 4; i < 3; i++, j += 4) {
        if (GET_CUR_FORM_BTN_ITEM(i + 1) != ITEM_NONE) {
            ItemId item = GET_CUR_FORM_BTN_ITEM(i + 1);
            if (GameInteractor_Should(VB_DRAW_ITEM_EQUIPPED_OUTLINE, (GET_CUR_FORM_BTN_SLOT(i + 1) < ITEM_NUM_SLOTS),
                                      &item, (s32)(i + 1), 0, PAUSE_ITEM)) {
                gSPVertex(POLY_OPA_DISP++, &pauseCtx->itemVtx[j], 4, 0);
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
                                          (DPAD_GET_CUR_FORM_BTN_SLOT(i) < ITEM_NUM_SLOTS), &item, (s32)i, 1,
                                          PAUSE_ITEM)) {
                    gSPVertex(POLY_OPA_DISP++, &pauseCtx->itemVtx[j], 4, 0);
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
    for (j = 0, i = 0; i < ITEM_NUM_SLOTS; i++, j += 4) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (((void)0, gSaveContext.save.saveInfo.inventory.items[i]) != ITEM_NONE) {
            if ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->pageIndex == PAUSE_ITEM) &&
                (pauseCtx->cursorSpecialPos == 0) && gPlayerFormSlotRestrictions[GET_PLAYER_FORM][i]) {
                if ((sEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) && (i == SLOT_ARROW_ICE)) {
                    // Possible bug:
                    // Supposed to be `SLOT_BOW`, unchanged from OoT, instead increase size of ice arrow icon
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sMagicArrowEffectsR[pauseCtx->equipTargetItem - 0xB5],
                                    sMagicArrowEffectsG[pauseCtx->equipTargetItem - 0xB5],
                                    sMagicArrowEffectsB[pauseCtx->equipTargetItem - 0xB5], pauseCtx->alpha);

                    pauseCtx->itemVtx[j + 0].v.ob[0] = pauseCtx->itemVtx[j + 2].v.ob[0] =
                        pauseCtx->itemVtx[j + 0].v.ob[0] - 2;
                    pauseCtx->itemVtx[j + 1].v.ob[0] = pauseCtx->itemVtx[j + 3].v.ob[0] =
                        pauseCtx->itemVtx[j + 0].v.ob[0] + 32;
                    pauseCtx->itemVtx[j + 0].v.ob[1] = pauseCtx->itemVtx[j + 1].v.ob[1] =
                        pauseCtx->itemVtx[j + 0].v.ob[1] + 2;
                    pauseCtx->itemVtx[j + 2].v.ob[1] = pauseCtx->itemVtx[j + 3].v.ob[1] =
                        pauseCtx->itemVtx[j + 0].v.ob[1] - 32;

                } else if (i == pauseCtx->cursorSlot[PAUSE_ITEM]) {
                    // Increase the size of the selected item
                    pauseCtx->itemVtx[j + 0].v.ob[0] = pauseCtx->itemVtx[j + 2].v.ob[0] =
                        pauseCtx->itemVtx[j + 0].v.ob[0] - 2;
                    pauseCtx->itemVtx[j + 1].v.ob[0] = pauseCtx->itemVtx[j + 3].v.ob[0] =
                        pauseCtx->itemVtx[j + 0].v.ob[0] + 32;
                    pauseCtx->itemVtx[j + 0].v.ob[1] = pauseCtx->itemVtx[j + 1].v.ob[1] =
                        pauseCtx->itemVtx[j + 0].v.ob[1] + 2;
                    pauseCtx->itemVtx[j + 2].v.ob[1] = pauseCtx->itemVtx[j + 3].v.ob[1] =
                        pauseCtx->itemVtx[j + 0].v.ob[1] - 32;
                }
            }
            // #region 2S2H [Port] Originally this was done in KaleidoScope_Update, but now we are using gSPGrayscale.
            ItemId itemId = gSaveContext.save.saveInfo.inventory.items[i];
            u8 itemRestricted = GameInteractor_Should(
                VB_ITEM_BE_RESTRICTED, !gPlayerFormItemRestrictions[GET_PLAYER_FORM][(s32)itemId], &itemId);
            if (itemRestricted) {
                gDPSetGrayscaleColor(POLY_OPA_DISP++, 109, 109, 109, 255);
                gSPGrayscale(POLY_OPA_DISP++, true);
            }
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->itemVtx[j + 0], 4, 0);
            KaleidoScope_DrawTexQuadRGBA32(
                play->state.gfxCtx, gItemIcons[((void)0, gSaveContext.save.saveInfo.inventory.items[i])], 32, 32, 0);
            if (itemRestricted) {
                gSPGrayscale(POLY_OPA_DISP++, false);
            }
            // #endregion
        }
    }

    // Draw the ammo digits
    if (pauseCtx->pageIndex == PAUSE_ITEM) {
        if ((pauseCtx->state == PAUSE_STATE_MAIN) &&
            ((pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) || (pauseCtx->mainState == PAUSE_MAIN_STATE_EQUIP_ITEM)) &&
            (pauseCtx->state != PAUSE_STATE_SAVEPROMPT) && !IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
            Gfx_SetupDL39_Opa(play->state.gfxCtx);
            gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

            // Loop over slots (i) and ammoIndex (j)
            for (j = 0, i = 0; i < ITEM_NUM_SLOTS; i++) {
                if (gAmmoItems[i] != ITEM_NONE) {
                    if (((void)0, gSaveContext.save.saveInfo.inventory.items[i]) != ITEM_NONE) {
                        KaleidoScope_DrawAmmoCount(pauseCtx, play->state.gfxCtx,
                                                   ((void)0, gSaveContext.save.saveInfo.inventory.items[i]), j);
                    }
                    j++;
                }
            }
            Gfx_SetupDL42_Opa(play->state.gfxCtx);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u8 sPlayerFormItems[PLAYER_FORM_MAX] = {
    ITEM_MASK_FIERCE_DEITY, // PLAYER_FORM_FIERCE_DEITY
    ITEM_MASK_GORON,        // PLAYER_FORM_GORON
    ITEM_MASK_ZORA,         // PLAYER_FORM_ZORA
    ITEM_MASK_DEKU,         // PLAYER_FORM_DEKU
    ITEM_NONE,              // PLAYER_FORM_HUMAN
};

void KaleidoScope_UpdateItemCursor(PlayState* play) {
    s32 pad1;
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    u16 vtxIndex;
    u16 cursorItem;
    u16 cursorSlot;
    u8 magicArrowIndex;
    s16 cursorPoint;
    s16 cursorXIndex;
    s16 cursorYIndex;
    s16 oldCursorPoint;
    s16 moveCursorResult;
    s16 pad2;

    pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
    pauseCtx->nameColorSet = PAUSE_NAME_COLOR_SET_WHITE;

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_ITEM) && !pauseCtx->itemDescriptionOn) {
        moveCursorResult = PAUSE_CURSOR_RESULT_NONE;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];

        cursorItem = pauseCtx->cursorItem[PAUSE_ITEM];

        // Move cursor left/right
        if (pauseCtx->cursorSpecialPos == 0) {
            // cursor is currently on a slot
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;

            if (ABS_ALT(pauseCtx->stickAdjX) > 30) {
                cursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];
                cursorXIndex = pauseCtx->cursorXIndex[PAUSE_ITEM];
                cursorYIndex = pauseCtx->cursorYIndex[PAUSE_ITEM];

                // Search for slot to move to
                while (moveCursorResult == PAUSE_CURSOR_RESULT_NONE) {
                    if (pauseCtx->stickAdjX < -30) {
                        // move cursor left
                        pauseCtx->cursorShrinkRate = 4.0f;
                        if (pauseCtx->cursorXIndex[PAUSE_ITEM] != 0) {
                            pauseCtx->cursorXIndex[PAUSE_ITEM]--;
                            pauseCtx->cursorPoint[PAUSE_ITEM]--;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                            pauseCtx->cursorYIndex[PAUSE_ITEM]++;

                            if (pauseCtx->cursorYIndex[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursorYIndex[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_ITEM] =
                                pauseCtx->cursorXIndex[PAUSE_ITEM] + (pauseCtx->cursorYIndex[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_ITEM] >= ITEM_NUM_SLOTS) {
                                pauseCtx->cursorPoint[PAUSE_ITEM] = pauseCtx->cursorXIndex[PAUSE_ITEM];
                            }

                            if (cursorYIndex == pauseCtx->cursorYIndex[PAUSE_ITEM]) {
                                pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);

                                moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                            }
                        }
                    } else if (pauseCtx->stickAdjX > 30) {
                        // move cursor right
                        pauseCtx->cursorShrinkRate = 4.0f;
                        if (pauseCtx->cursorXIndex[PAUSE_ITEM] <= 4) {
                            pauseCtx->cursorXIndex[PAUSE_ITEM]++;
                            pauseCtx->cursorPoint[PAUSE_ITEM]++;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                            pauseCtx->cursorYIndex[PAUSE_ITEM]++;

                            if (pauseCtx->cursorYIndex[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursorYIndex[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_ITEM] =
                                pauseCtx->cursorXIndex[PAUSE_ITEM] + (pauseCtx->cursorYIndex[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_ITEM] >= ITEM_NUM_SLOTS) {
                                pauseCtx->cursorPoint[PAUSE_ITEM] = pauseCtx->cursorXIndex[PAUSE_ITEM];
                            }

                            if (cursorYIndex == pauseCtx->cursorYIndex[PAUSE_ITEM]) {
                                pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);

                                moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                            }
                        }
                    }
                }

                if (moveCursorResult == PAUSE_CURSOR_RESULT_SLOT) {
                    cursorItem = gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                KaleidoScope_MoveCursorFromSpecialPos(play);
                cursorYIndex = 0;
                cursorXIndex = 0;
                cursorPoint = 0; // top row, left column (SLOT_OCARINA)

                // Search for slot to move to
                while (true) {
                    // Check if current cursor has an item in its slot
                    if (gSaveContext.save.saveInfo.inventory.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                        pauseCtx->cursorYIndex[PAUSE_ITEM] = cursorYIndex;
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
            if (pauseCtx->stickAdjX < -30) {
                KaleidoScope_MoveCursorFromSpecialPos(play);
                cursorXIndex = 5;
                cursorPoint = 5; // top row, right columne (SLOT_TRADE_DEED)
                cursorYIndex = 0;

                // Search for slot to move to
                while (true) {
                    // Check if current cursor has an item in its slot
                    if (gSaveContext.save.saveInfo.inventory.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursorXIndex[PAUSE_ITEM] = cursorXIndex;
                        pauseCtx->cursorYIndex[PAUSE_ITEM] = cursorYIndex;
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

                cursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];
                cursorYIndex = pauseCtx->cursorYIndex[PAUSE_ITEM];

                while (moveCursorResult == PAUSE_CURSOR_RESULT_NONE) {
                    if (pauseCtx->stickAdjY > 30) {
                        // move cursor up
                        moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                        if (pauseCtx->cursorYIndex[PAUSE_ITEM] != 0) {
                            pauseCtx->cursorYIndex[PAUSE_ITEM]--;
                            pauseCtx->cursorShrinkRate = 4.0f;
                            pauseCtx->cursorPoint[PAUSE_ITEM] -= 6;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_ITEM] = cursorYIndex;
                            pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        }
                    } else if (pauseCtx->stickAdjY < -30) {
                        // move cursor down
                        moveCursorResult = PAUSE_CURSOR_RESULT_SPECIAL_POS;
                        if (pauseCtx->cursorYIndex[PAUSE_ITEM] < 3) {
                            pauseCtx->cursorYIndex[PAUSE_ITEM]++;
                            pauseCtx->cursorShrinkRate = 4.0f;
                            pauseCtx->cursorPoint[PAUSE_ITEM] += 6;
                            moveCursorResult = PAUSE_CURSOR_RESULT_SLOT;
                        } else {
                            pauseCtx->cursorYIndex[PAUSE_ITEM] = cursorYIndex;
                            pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        }
                    }
                }
            }

            cursorSlot = pauseCtx->cursorPoint[PAUSE_ITEM];
            pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;

            if (moveCursorResult == PAUSE_CURSOR_RESULT_SLOT) {
                cursorItem = gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
            } else if (moveCursorResult != PAUSE_CURSOR_RESULT_SPECIAL_POS) {
                cursorItem = gSaveContext.save.saveInfo.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
            }

            if (cursorItem == ITEM_NONE) {
                cursorItem = PAUSE_ITEM_NONE;
                pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_WHITE;
            }

            if ((cursorItem != (u32)PAUSE_ITEM_NONE) && (msgCtx->msgLength == 0)) {
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

            pauseCtx->cursorItem[PAUSE_ITEM] = cursorItem;
            pauseCtx->cursorSlot[PAUSE_ITEM] = cursorSlot;
            if (cursorItem != PAUSE_ITEM_NONE) {
                // Equip item to the C buttons
                if ((pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && !pauseCtx->itemDescriptionOn &&
                    (pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
                    CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                                  BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT | BTN_DPAD_EQUIP)) {

                    // Ensure that a transformation mask can not be unequipped while being used
                    if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                        if (1) {}
                        if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CLEFT)) {
                            if (sPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CDOWN)) {
                            if (sPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CRIGHT)) {
                            if (sPlayerFormItems[GET_PLAYER_FORM] == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                        }
                        // #region 2S2H [Dpad]
                        else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                            if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DRIGHT)) {
                                if (sPlayerFormItems[GET_PLAYER_FORM] ==
                                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    return;
                                }
                            } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DLEFT)) {
                                if (sPlayerFormItems[GET_PLAYER_FORM] == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    return;
                                }
                            } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DDOWN)) {
                                if (sPlayerFormItems[GET_PLAYER_FORM] == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    return;
                                }
                            } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DUP)) {
                                if (sPlayerFormItems[GET_PLAYER_FORM] == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP)) {
                                    Audio_PlaySfx(NA_SE_SY_ERROR);
                                    return;
                                }
                            }
                        }
                        // #endregion
                    }

                    // Ensure that a non-transformation mask can not be unequipped while being used
                    if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CLEFT)) {
                        if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                            (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_LEFT;
                    } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CDOWN)) {
                        if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                            (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_DOWN;
                    } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_CRIGHT)) {
                        if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                            (Player_GetCurMaskItemId(play) == BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT))) {
                            Audio_PlaySfx(NA_SE_SY_ERROR);
                            return;
                        }
                        pauseCtx->equipTargetCBtn = PAUSE_EQUIP_C_RIGHT;
                    }
                    // #region 2S2H [Dpad]
                    else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                        if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DRIGHT)) {
                            if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_RIGHT;
                        } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DLEFT)) {
                            if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_LEFT;
                        } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DDOWN)) {
                            if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_DOWN;
                        } else if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_DUP)) {
                            if ((Player_GetCurMaskItemId(play) != ITEM_NONE) &&
                                (Player_GetCurMaskItemId(play) == DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP))) {
                                Audio_PlaySfx(NA_SE_SY_ERROR);
                                return;
                            }
                            pauseCtx->equipTargetCBtn = PAUSE_EQUIP_D_UP;
                        }
                    }
                    // #endregion
                    if (!GameInteractor_Should(VB_KALEIDO_EQUIP_ITEM_TO_BUTTON, true, cursorSlot, cursorItem)) {
                        return;
                    }

                    // Equip item to the C buttons
                    pauseCtx->equipTargetItem = cursorItem;
                    pauseCtx->equipTargetSlot = cursorSlot;
                    pauseCtx->mainState = PAUSE_MAIN_STATE_EQUIP_ITEM;
                    vtxIndex = cursorSlot * 4;
                    pauseCtx->equipAnimX = pauseCtx->itemVtx[vtxIndex].v.ob[0] * 10;
                    pauseCtx->equipAnimY = pauseCtx->itemVtx[vtxIndex].v.ob[1] * 10;
                    pauseCtx->equipAnimAlpha = 255;
                    sEquipMagicArrowSlotHoldTimer = 0;
                    sEquipState = EQUIP_STATE_MOVE_TO_C_BTN;
                    sEquipAnimTimer = 10;

                    if ((pauseCtx->equipTargetItem == ITEM_ARROW_FIRE) ||
                        (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) ||
                        (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT)) {
                        magicArrowIndex = 0;
                        if (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) {
                            magicArrowIndex = 1;
                        }
                        if (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT) {
                            magicArrowIndex = 2;
                        }
                        Audio_PlaySfx(NA_SE_SY_SET_FIRE_ARROW + magicArrowIndex);
                        pauseCtx->equipTargetItem = 0xB5 + magicArrowIndex;
                        pauseCtx->equipAnimAlpha = sEquipState = 0; // EQUIP_STATE_MAGIC_ARROW_GROW_ORB
                        sEquipAnimTimer = 6;
                    } else {
                        Audio_PlaySfx(NA_SE_SY_DECIDE);
                    }
                } else if ((pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
                           (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
                           CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, BTN_A) && (msgCtx->msgLength == 0)) {
                    if (GameInteractor_Should(VB_KALEIDO_DISPLAY_ITEM_TEXT, true, &cursorItem)) {
                        // Give description on item through a message box
                        pauseCtx->itemDescriptionOn = true;
                        if (pauseCtx->cursorYIndex[PAUSE_ITEM] < 2) {
                            func_801514B0(play, 0x1700 + pauseCtx->cursorItem[PAUSE_ITEM], 3);
                        } else {
                            func_801514B0(play, 0x1700 + pauseCtx->cursorItem[PAUSE_ITEM], 1);
                        }
                    }
                }
            }
        } else {
            pauseCtx->cursorItem[PAUSE_ITEM] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_ITEM]) {
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if ((pauseCtx->mainState == PAUSE_MAIN_STATE_EQUIP_ITEM) && (pauseCtx->pageIndex == PAUSE_ITEM)) {
        pauseCtx->cursorColorSet = PAUSE_CURSOR_COLOR_SET_YELLOW;
    }
}

s16 sCButtonPosX[] = {
    660, 900, 1140,
    // #region 2S2H [Dpad]
    1350, 1030, 1190, 1190
    // #endregion
};
s16 sCButtonPosY[] = {
    1100, 920, 1100,
    // #region 2S2H [Dpad]
    570, 570, 410, 730
    // #endregion
};

// #region 2S2H [Dpad]
void KaleidoScope_SwapDpadItemToCItem(PlayState* play, EquipSlot cEquipSlot) {
    PauseContext* pauseCtx = &play->pauseCtx;

    if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) == ITEM_BOW) ||
                 (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) >= ITEM_BOW_FIRE) &&
                  ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = C_SLOT_EQUIP(0, cEquipSlot);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            }
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) == ITEM_BOW) ||
                 (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) >= ITEM_BOW_FIRE) &&
                  ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = C_SLOT_EQUIP(0, cEquipSlot);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            }
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) == ITEM_BOW) ||
                 (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) >= ITEM_BOW_FIRE) &&
                  ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = C_SLOT_EQUIP(0, cEquipSlot);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            }
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
        }
    } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
        if ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) != ITEM_NONE) {
            if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) == ITEM_BOW) ||
                 (((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) >= ITEM_BOW_FIRE) &&
                  ((BUTTON_ITEM_EQUIP(0, cEquipSlot) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = BUTTON_ITEM_EQUIP(0, cEquipSlot);
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = C_SLOT_EQUIP(0, cEquipSlot);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            }
        } else {
            DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
            DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
        }
    }
}

void KaleidoScope_UpdateDpadItemEquip(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;

    if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_RIGHT) {
        // Swap if item is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Special case for magic arrows
        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) == ITEM_BOW) ||
                ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                 (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            }
        } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
            if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            }
        }

        // Equip item on DRight
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_RIGHT);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_LEFT) {
        // Swap if item is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Special case for magic arrows
        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) == ITEM_BOW) ||
                ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                 (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            }
        } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
            if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            }
        }

        // Equip item on DLeft
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_LEFT);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_DOWN) {
        // Swap if item is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = SLOT_NONE;
            }
        }

        // Special case for magic arrows
        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) == ITEM_BOW) ||
                ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                 (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            }
        } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
            if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
            }
        }

        // Equip item on DDown
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_DOWN);
    } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_D_UP) {
        // Swap if item is already equipped on other Item Buttons.
        if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                }
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = SLOT_NONE;
            }
        } else if (pauseCtx->equipTargetSlot == DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) != ITEM_NONE) {
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                    (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) == ITEM_BOW) ||
                     (((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) >= ITEM_BOW_FIRE) &&
                      ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                    pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                } else {
                    DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                    DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP);
                    Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                }
            } else {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = ITEM_NONE;
                DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = SLOT_NONE;
            }
        }

        // Special case for magic arrows
        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
            if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) == ITEM_BOW) ||
                ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                 (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT))) {
                pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                pauseCtx->equipTargetSlot = SLOT_BOW;
            }
        } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
            if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
            } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                       (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
            } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                       (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP);
                Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
            }
        }

        // Equip item on DUp
        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = pauseCtx->equipTargetItem;
        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = pauseCtx->equipTargetSlot;
        Interface_Dpad_LoadItemIconImpl(play, EQUIP_SLOT_D_UP);
    }
}
// #endregion

void KaleidoScope_UpdateItemEquip(PlayState* play) {
    static s16 sEquipMagicArrowBowSlotHoldTimer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

    // Grow glowing orb when equipping magic arrows
    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_GROW_ORB) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
            sEquipState++;
        }
        // Hover over magic arrow slot when the next state is reached
        sEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT) {
        sEquipMagicArrowBowSlotHoldTimer--;

        if (sEquipMagicArrowBowSlotHoldTimer == 0) {
            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
            pauseCtx->equipTargetSlot = SLOT_BOW;
            sEquipAnimTimer = 6;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
            sEquipState++;
            Audio_PlaySfx(NA_SE_SY_SYNTH_MAGIC_ARROW);
        }
        return;
    }

    // #region 2S2H [Cosmetic] Track the C button position vanilla values or HUD editor adjusted values
    s16 cButtonPosX = sCButtonPosX[pauseCtx->equipTargetCBtn];
    s16 cButtonPosY = sCButtonPosY[pauseCtx->equipTargetCBtn];

    HudEditor_SetActiveElement(pauseCtx->equipTargetCBtn < 3 ? HUD_EDITOR_ELEMENT_C_LEFT + pauseCtx->equipTargetCBtn
                                                             : HUD_EDITOR_ELEMENT_D_PAD);

    if (sEquipState == EQUIP_STATE_MOVE_TO_C_BTN && HudEditor_ShouldOverrideDraw()) {
        s16 equipAnimShrinkRate = 40;
        HudEditor_ModifyKaleidoEquipAnimValues(&cButtonPosX, &cButtonPosY, &equipAnimShrinkRate);

        // Override the anim shrink rate at the beginning (value is 320)
        if (pauseCtx->equipAnimScale == 320) {
            pauseCtx->equipAnimShrinkRate = equipAnimShrinkRate;
        }

        if (CVarGetInteger(hudEditorElements[hudEditorActiveElement].modeCvar, HUD_EDITOR_ELEMENT_MODE_VANILLA) ==
            HUD_EDITOR_ELEMENT_MODE_HIDDEN) {
            pauseCtx->equipAnimScale = 0;
            pauseCtx->equipAnimShrinkRate = 0;
        }
    } else if (sEquipState == EQUIP_STATE_MOVE_TO_C_BTN && pauseCtx->equipTargetCBtn >= 3) {
        // Equips to DPad need to shrink fast to be have a final smaller size (16px),
        // So we override the anim shrink rate at the beginning (value is 320)
        if (pauseCtx->equipAnimScale == 320) {
            pauseCtx->equipAnimShrinkRate = 160;
        }
    }

    HudEditor_SetActiveElement(HUD_EDITOR_ELEMENT_NONE);
    // #endregion

    if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
        bowItemVtx = &pauseCtx->itemVtx[SLOT_BOW * 4];
        offsetX = ABS_ALT(pauseCtx->equipAnimX - bowItemVtx->v.ob[0] * 10) / sEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - bowItemVtx->v.ob[1] * 10) / sEquipAnimTimer;
    } else {
        // 2S2H [Cosmetic] Use position vars from above
        offsetX = ABS_ALT(pauseCtx->equipAnimX - cButtonPosX) / sEquipAnimTimer;
        offsetY = ABS_ALT(pauseCtx->equipAnimY - cButtonPosY) / sEquipAnimTimer;
    }

    if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipAnimAlpha < 254)) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
        }
        sEquipMagicArrowSlotHoldTimer = 5;
        return;
    }

    if (sEquipMagicArrowSlotHoldTimer == 0) {
        pauseCtx->equipAnimScale -= pauseCtx->equipAnimShrinkRate / sEquipAnimTimer;
        pauseCtx->equipAnimShrinkRate -= pauseCtx->equipAnimShrinkRate / sEquipAnimTimer;

        // Update coordinates of item icon while being equipped
        if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
            // target is the bow slot
            if (pauseCtx->equipAnimX >= (pauseCtx->itemVtx[SLOT_BOW * 4].v.ob[0] * 10)) {
                pauseCtx->equipAnimX -= offsetX;
            } else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= (pauseCtx->itemVtx[SLOT_BOW * 4].v.ob[1] * 10)) {
                pauseCtx->equipAnimY -= offsetY;
            } else {
                pauseCtx->equipAnimY += offsetY;
            }
        } else {
            // target is the c button
            // 2S2H [Cosmetic] Use position vars from above
            if (pauseCtx->equipAnimX >= cButtonPosX) {
                pauseCtx->equipAnimX -= offsetX;
            } else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= cButtonPosY) {
                pauseCtx->equipAnimY -= offsetY;
            } else {
                pauseCtx->equipAnimY += offsetY;
            }
        }

        sEquipAnimTimer--;
        if (sEquipAnimTimer == 0) {
            if (sEquipState == EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT) {
                sEquipState++;
                sEquipMagicArrowBowSlotHoldTimer = 4;
                return;
            }

            // Equip item onto c buttons
            if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_LEFT) {
                // Swap if item is already equipped on CDown or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadItemToCItem(play, EQUIP_SLOT_C_LEFT);
                // #endregion

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                         (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                               (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                    // #region 2S2H [Dpad]
                    // Note Only C-Left has the swap of 'slot equips' here
                    if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                        (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        DPAD_SLOT_EQUIP(0, EQUIP_SLOT_D_UP) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                    }
                    // #endregion
                }

                // Equip item on CLeft
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_LEFT);
            } else if (pauseCtx->equipTargetCBtn == PAUSE_EQUIP_C_DOWN) {
                // Swap if item is already equipped on CLeft or CRight.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadItemToCItem(play, EQUIP_SLOT_C_DOWN);
                // #endregion

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                         (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                               (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_RIGHT);
                    }
                    // #region 2S2H [Dpad]
                    // Note Only C-Left has the swap of 'slot equips' here
                    if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                        (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                    }
                    // #endregion
                }

                // Equip item on CDown
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_DOWN);
            } else if (pauseCtx->equipTargetCBtn ==
                       PAUSE_EQUIP_C_RIGHT) { // #Region 2S2H [Dpad] Added condition here to allow for other cases
                // Swap if item is already equipped on CLeft or CDown.
                if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_LEFT) = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8) &&
                            (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) == ITEM_BOW) ||
                             (((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) >= ITEM_BOW_FIRE) &&
                              ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) & 0xFF) <= ITEM_BOW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                            Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                        }
                    } else {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = ITEM_NONE;
                        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = SLOT_NONE;
                    }
                }
                // #region 2S2H [Dpad]
                KaleidoScope_SwapDpadItemToCItem(play, EQUIP_SLOT_C_RIGHT);
                // #endregion

                // Special case for magic arrows
                if ((pauseCtx->equipTargetItem >= 0xB5) && (pauseCtx->equipTargetItem < 0xB8)) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) == ITEM_BOW) ||
                        ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) >= ITEM_BOW_FIRE) &&
                         (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) <= ITEM_BOW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xB5 - ITEM_BOW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) >= ITEM_BOW_FIRE) &&
                        (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_LEFT);
                    } else if ((BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) >= ITEM_BOW_FIRE) &&
                               (BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) <= ITEM_BOW_LIGHT)) {
                        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_LoadItemIcon(play, EQUIP_SLOT_C_DOWN);
                    }
                    // #region 2S2H [Dpad]
                    // Note Only C-Left has the swap of 'slot equips' here
                    if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) >= ITEM_BOW_FIRE) &&
                        (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_RIGHT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_RIGHT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_LEFT) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_LEFT);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_DOWN) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_DOWN);
                    } else if ((DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) >= ITEM_BOW_FIRE) &&
                               (DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) <= ITEM_BOW_LIGHT)) {
                        DPAD_BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_D_UP) = BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT);
                        Interface_Dpad_LoadItemIcon(play, EQUIP_SLOT_D_UP);
                    }
                    // #endregion
                }

                // Equip item on CRight
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetItem;
                C_SLOT_EQUIP(0, EQUIP_SLOT_C_RIGHT) = pauseCtx->equipTargetSlot;
                Interface_LoadItemIconImpl(play, EQUIP_SLOT_C_RIGHT);
            }
            // #region 2S2H [Dpad]
            else if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                KaleidoScope_UpdateDpadItemEquip(play);
            }
            // #endregion

            // Reset params
            pauseCtx->mainState = PAUSE_MAIN_STATE_IDLE;
            sEquipAnimTimer = 10;
            pauseCtx->equipAnimScale = 320;
            pauseCtx->equipAnimShrinkRate = 40;
        }
    } else {
        sEquipMagicArrowSlotHoldTimer--;
        if (sEquipMagicArrowSlotHoldTimer == 0) {
            pauseCtx->equipAnimAlpha = 255;
        }
    }
}
