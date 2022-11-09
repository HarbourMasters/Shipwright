#include "z_kaleido_scope.h"
#include "textures/parameter_static/parameter_static.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"

u8 gAmmoItems[] = {
    ITEM_STICK,   ITEM_NUT,  ITEM_BOMB, ITEM_BOW,  ITEM_NONE, ITEM_NONE, ITEM_SLINGSHOT, ITEM_NONE,
    ITEM_BOMBCHU, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_BEAN,      ITEM_NONE,
};

static s16 sEquipState = 0;
static s16 sEquipAnimTimer = 0;
static s16 sEquipMoveTimer = 10;
bool gSelectingMask;
bool gSelectingAdultTrade;

static s16 sAmmoVtxOffset[] = {
    0, 2, 4, 6, 99, 99, 8, 99, 10, 99, 99, 99, 99, 99, 12,
};

extern const char* _gAmmoDigit0Tex[];

void KaleidoScope_DrawAmmoCount(PauseContext* pauseCtx, GraphicsContext* gfxCtx, s16 item, int slot) {
    s16 ammo;
    s16 i;

    OPEN_DISPS(gfxCtx);

    ammo = AMMO(item);

    gDPPipeSync(POLY_KAL_DISP++);

    if (!CHECK_SLOT_AGE(SLOT(item))) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 100, 100, 100, pauseCtx->alpha);
    } else {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (ammo == 0) {
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 130, 130, 130, pauseCtx->alpha);
        } else if ((item == ITEM_BOMB && AMMO(item) == CUR_CAPACITY(UPG_BOMB_BAG)) ||
                   (item == ITEM_BOW && AMMO(item) == CUR_CAPACITY(UPG_QUIVER)) ||
                   (item == ITEM_SLINGSHOT && AMMO(item) == CUR_CAPACITY(UPG_BULLET_BAG)) ||
                   (item == ITEM_STICK && AMMO(item) == CUR_CAPACITY(UPG_STICKS)) ||
                   (item == ITEM_NUT && AMMO(item) == CUR_CAPACITY(UPG_NUTS)) || (item == ITEM_BOMBCHU && ammo == 50) ||
                   (item == ITEM_BEAN && ammo == 15)) {
            gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 120, 255, 0, pauseCtx->alpha);
        }
    }

    for (i = 0; ammo >= 10; i++) {
        ammo -= 10;
    }

    gDPPipeSync(POLY_KAL_DISP++);

    if (i != 0) {
        gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[(sAmmoVtxOffset[slot] + 31) * 4], 4, 0);

        gDPLoadTextureBlock(POLY_KAL_DISP++, ((u8*)_gAmmoDigit0Tex[i]), G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);
    }

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[(sAmmoVtxOffset[slot] + 32) * 4], 4, 0);

    gDPLoadTextureBlock(POLY_KAL_DISP++, ((u8*)_gAmmoDigit0Tex[ammo]), G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(gfxCtx);
}

void KaleidoScope_SetCursorVtx(PauseContext* pauseCtx, u16 index, Vtx* vtx) {
    pauseCtx->cursorVtx[0].v.ob[0] = vtx[index].v.ob[0];
    pauseCtx->cursorVtx[0].v.ob[1] = vtx[index].v.ob[1];
    KaleidoScope_UpdateCursorSize(pauseCtx); // OTRTODO Why is this needed?
}

void KaleidoScope_SetItemCursorVtx(PauseContext* pauseCtx) {
    KaleidoScope_SetCursorVtx(pauseCtx, pauseCtx->cursorSlot[PAUSE_ITEM] * 4, pauseCtx->itemVtx);
}

void KaleidoScope_DrawItemSelect(PlayState* play) {
    static s16 magicArrowEffectsR[] = { 255, 100, 255 };
    static s16 magicArrowEffectsG[] = { 0, 100, 255 };
    static s16 magicArrowEffectsB[] = { 0, 255, 100 };
    Input* input = &play->state.input[0];
    PauseContext* pauseCtx = &play->pauseCtx;
    u16 i;
    u16 j;
    u16 cursorItem;
    u16 cursorSlot = 0;
    u16 index;
    s16 cursorPoint;
    s16 cursorX;
    s16 cursorY;
    s16 oldCursorPoint;
    s16 moveCursorResult;
    bool dpad = (CVar_GetS32("gDpadPause", 0) && !CHECK_BTN_ALL(input->cur.button, BTN_CUP));

    OPEN_DISPS(play->state.gfxCtx);

    func_800949A8(play->state.gfxCtx);

    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    pauseCtx->cursorColorSet = 0;
    pauseCtx->nameColorSet = 0;

    if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0) && (pauseCtx->pageIndex == PAUSE_ITEM)) {
        moveCursorResult = 0 || gSelectingMask || gSelectingAdultTrade;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];

        cursorItem = pauseCtx->cursorItem[PAUSE_ITEM];
        cursorSlot = pauseCtx->cursorSlot[PAUSE_ITEM];

        if (pauseCtx->cursorSpecialPos == 0) {
            pauseCtx->cursorColorSet = 4;

            if (cursorItem == PAUSE_ITEM_NONE) {
                pauseCtx->stickRelX = 40;
            }

            if ((ABS(pauseCtx->stickRelX) > 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DRIGHT))) {
                cursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];
                cursorX = pauseCtx->cursorX[PAUSE_ITEM];
                cursorY = pauseCtx->cursorY[PAUSE_ITEM];

                osSyncPrintf("now=%d  ccc=%d\n", cursorPoint, cursorItem);

                // Seem necessary to match
                if (pauseCtx->cursorX[PAUSE_ITEM]) {}
                if (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]]) {}

                while (moveCursorResult == 0) {
                    if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                        if (pauseCtx->cursorX[PAUSE_ITEM] != 0) {
                            pauseCtx->cursorX[PAUSE_ITEM] -= 1;
                            pauseCtx->cursorPoint[PAUSE_ITEM] -= 1;
                            if ((gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) ||
                                CVar_GetS32("gPauseAnyCursor", 0)) {
                                moveCursorResult = 1;
                            }
                        } else {
                            pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                            pauseCtx->cursorY[PAUSE_ITEM] += 1;

                            if (pauseCtx->cursorY[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursorY[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_ITEM] =
                                pauseCtx->cursorX[PAUSE_ITEM] + (pauseCtx->cursorY[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_ITEM] >= 24) {
                                pauseCtx->cursorPoint[PAUSE_ITEM] = pauseCtx->cursorX[PAUSE_ITEM];
                            }

                            if (cursorY == pauseCtx->cursorY[PAUSE_ITEM]) {
                                pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);

                                moveCursorResult = 2;
                            }
                        }
                    } else if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                        if (pauseCtx->cursorX[PAUSE_ITEM] < 5) {
                            pauseCtx->cursorX[PAUSE_ITEM] += 1;
                            pauseCtx->cursorPoint[PAUSE_ITEM] += 1;
                            if ((gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) ||
                                CVar_GetS32("gPauseAnyCursor", 0)) {
                                moveCursorResult = 1;
                            }
                        } else {
                            pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                            pauseCtx->cursorY[PAUSE_ITEM] += 1;

                            if (pauseCtx->cursorY[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursorY[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursorPoint[PAUSE_ITEM] =
                                pauseCtx->cursorX[PAUSE_ITEM] + (pauseCtx->cursorY[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursorPoint[PAUSE_ITEM] >= 24) {
                                pauseCtx->cursorPoint[PAUSE_ITEM] = pauseCtx->cursorX[PAUSE_ITEM];
                            }

                            if (cursorY == pauseCtx->cursorY[PAUSE_ITEM]) {
                                pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);

                                moveCursorResult = 2;
                            }
                        }
                    }
                }

                if (moveCursorResult == 1) {
                    cursorItem = gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
                }

                osSyncPrintf("【Ｘ cursor=%d(%) (cur_xpt=%d)(ok_fg=%d)(ccc=%d)(key_angle=%d)】  ",
                             pauseCtx->cursorPoint[PAUSE_ITEM], pauseCtx->cursorX[PAUSE_ITEM], moveCursorResult,
                             cursorItem, pauseCtx->cursorSpecialPos);
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;

                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

                cursorPoint = cursorX = cursorY = 0;
                while (true) {
                    if (gSaveContext.inventory.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                        pauseCtx->cursorY[PAUSE_ITEM] = cursorY;
                        moveCursorResult = 1;
                        break;
                    }

                    cursorY = cursorY + 1;
                    cursorPoint = cursorPoint + 6;
                    if (cursorY < 4) {
                        continue;
                    }

                    cursorY = 0;
                    cursorPoint = cursorX + 1;
                    cursorX = cursorPoint;
                    if (cursorX < 6) {
                        continue;
                    }

                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    break;
                }
            }
        } else {
            if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;

                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

                cursorPoint = cursorX = 5;
                cursorY = 0;
                while (true) {
                    if (gSaveContext.inventory.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursorX[PAUSE_ITEM] = cursorX;
                        pauseCtx->cursorY[PAUSE_ITEM] = cursorY;
                        moveCursorResult = 1;
                        break;
                    }

                    cursorY = cursorY + 1;
                    cursorPoint = cursorPoint + 6;
                    if (cursorY < 4) {
                        continue;
                    }

                    cursorY = 0;
                    cursorPoint = cursorX - 1;
                    cursorX = cursorPoint;
                    if (cursorX >= 0) {
                        continue;
                    }

                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    break;
                }
            }
        }

        if (pauseCtx->cursorSpecialPos == 0) {
            if (cursorItem != PAUSE_ITEM_NONE) {
                if ((ABS(pauseCtx->stickRelY) > 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
                    moveCursorResult = 0 || gSelectingMask || gSelectingAdultTrade;

                    cursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];
                    cursorY = pauseCtx->cursorY[PAUSE_ITEM];
                    while (moveCursorResult == 0) {
                        if ((pauseCtx->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                            if (pauseCtx->cursorY[PAUSE_ITEM] != 0) {
                                pauseCtx->cursorY[PAUSE_ITEM] -= 1;
                                pauseCtx->cursorPoint[PAUSE_ITEM] -= 6;
                                if ((gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) ||
                                    CVar_GetS32("gPauseAnyCursor", 0)) {
                                    moveCursorResult = 1;
                                }
                            } else {
                                pauseCtx->cursorY[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        } else if ((pauseCtx->stickRelY < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
                            if (pauseCtx->cursorY[PAUSE_ITEM] < 3) {
                                pauseCtx->cursorY[PAUSE_ITEM] += 1;
                                pauseCtx->cursorPoint[PAUSE_ITEM] += 6;
                                if ((gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) ||
                                    CVar_GetS32("gPauseAnyCursor", 0)) {
                                    moveCursorResult = 1;
                                }
                            } else {
                                pauseCtx->cursorY[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursorPoint[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        }
                    }

                    cursorPoint = PAUSE_ITEM;
                    osSyncPrintf("【Ｙ cursor=%d(%) (cur_ypt=%d)(ok_fg=%d)(ccc=%d)】  ",
                                 pauseCtx->cursorPoint[cursorPoint], pauseCtx->cursorY[PAUSE_ITEM], moveCursorResult,
                                 cursorItem);
                }
            }

            cursorSlot = pauseCtx->cursorPoint[PAUSE_ITEM];

            pauseCtx->cursorColorSet = 4;

            if (moveCursorResult == 1) {
                cursorItem = gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
            } else if (moveCursorResult != 2) {
                cursorItem = gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]];
            }

            pauseCtx->cursorItem[PAUSE_ITEM] = cursorItem;
            pauseCtx->cursorSlot[PAUSE_ITEM] = cursorSlot;

            gSlotAgeReqs[SLOT_TRADE_CHILD] = gItemAgeReqs[ITEM_MASK_BUNNY] =
                ((CVar_GetS32("gMMBunnyHood", 0) || CVar_GetS32("gTimelessEquipment", 0)) &&
                 INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_MASK_BUNNY)
                    ? 9
                    : 1;

            if (!CHECK_SLOT_AGE(cursorSlot)) {
                pauseCtx->nameColorSet = 1;
            }

            if (cursorItem != PAUSE_ITEM_NONE) {
                index = cursorSlot * 4; // required to match?
                KaleidoScope_SetCursorVtx(pauseCtx, index, pauseCtx->itemVtx);

                if ((pauseCtx->debugState == 0) && (pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0)) {
                    // only allow mask select when:
                    // the shop is open:
                    // * zelda's letter check: gSaveContext.eventChkInf[4] & 1
                    // * kak gate check: gSaveContext.infTable[7] & 0x40
                    // and the mask quest is complete: gSaveContext.eventChkInf[8] & 0x8000
                    if (CVar_GetS32("gMaskSelect", 0) &&
                        (gSaveContext.eventChkInf[8] & 0x8000) &&
                        cursorSlot == SLOT_TRADE_CHILD && CHECK_BTN_ALL(input->press.button, BTN_A) &&
                        (gSaveContext.eventChkInf[4] & 1) &&
                        (gSaveContext.infTable[7] & 0x40)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        gSelectingMask = !gSelectingMask;
                    }
                    if (gSelectingMask) {
                        pauseCtx->cursorColorSet = 8;
                        if (((pauseCtx->stickRelX > 30 || pauseCtx->stickRelY > 30) ||
                             dpad && CHECK_BTN_ANY(input->press.button, BTN_DRIGHT | BTN_DUP)) &&
                            INV_CONTENT(ITEM_TRADE_CHILD) < ITEM_MASK_TRUTH) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                            ++INV_CONTENT(ITEM_TRADE_CHILD);
                        } else if (((pauseCtx->stickRelX < -30 || pauseCtx->stickRelY < -30) ||
                                    dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DDOWN)) &&
                                   INV_CONTENT(ITEM_TRADE_CHILD) > ITEM_MASK_KEATON) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                            --INV_CONTENT(ITEM_TRADE_CHILD);
                        } else if ((pauseCtx->stickRelX < -30 || pauseCtx->stickRelX > 30 || pauseCtx->stickRelY < -30 || pauseCtx->stickRelY > 30) ||
                                   dpad && CHECK_BTN_ANY(input->press.button, BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT)) {
                            // Change to keaton mask if no mask is in child trade slot. Catches Zelda's letter and bottle duping over this slot.
                            if (INV_CONTENT(ITEM_TRADE_CHILD) < ITEM_MASK_KEATON || INV_CONTENT(ITEM_TRADE_CHILD) > ITEM_MASK_TRUTH) {
                                INV_CONTENT(ITEM_TRADE_CHILD) = ITEM_MASK_KEATON;
                            } else {
                                INV_CONTENT(ITEM_TRADE_CHILD) ^= ITEM_MASK_KEATON ^ ITEM_MASK_TRUTH;
                            }
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        }
                        for (uint16_t cSlotIndex = 0; cSlotIndex < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); cSlotIndex++) {
                            if (gSaveContext.equips.cButtonSlots[cSlotIndex] == SLOT_TRADE_CHILD) {
                                if (!LINK_IS_ADULT || CVar_GetS32("gTimelessEquipment", 0)) {
                                    gSaveContext.equips.buttonItems[cSlotIndex+1] = INV_CONTENT(ITEM_TRADE_CHILD);
                                } else if (INV_CONTENT(ITEM_TRADE_CHILD) != gSaveContext.equips.buttonItems[cSlotIndex+1]) {
                                    gSaveContext.equips.cButtonSlots[cSlotIndex] = SLOT_NONE;
                                    gSaveContext.equips.buttonItems[cSlotIndex+1] = ITEM_NONE;
                                }
                            }
                        }
                        gSelectingMask = cursorSlot == SLOT_TRADE_CHILD;
                    }
                    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE) &&
                        cursorSlot == SLOT_TRADE_ADULT && CHECK_BTN_ALL(input->press.button, BTN_A)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        gSelectingAdultTrade = !gSelectingAdultTrade;
                    }
                    if (gSelectingAdultTrade) {
                        pauseCtx->cursorColorSet = 8;
                        if (((pauseCtx->stickRelX > 30 || pauseCtx->stickRelY > 30) ||
                             dpad && CHECK_BTN_ANY(input->press.button, BTN_DRIGHT | BTN_DUP))) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                            Inventory_ReplaceItem(play, INV_CONTENT(ITEM_TRADE_ADULT), Randomizer_GetNextAdultTradeItem());
                        } else if (((pauseCtx->stickRelX < -30 || pauseCtx->stickRelY < -30) ||
                            dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DDOWN))) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                            Inventory_ReplaceItem(play, INV_CONTENT(ITEM_TRADE_ADULT), Randomizer_GetPrevAdultTradeItem());
                        }
                        gSelectingAdultTrade = cursorSlot == SLOT_TRADE_ADULT;
                    }
                    u16 buttonsToCheck = BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT;
                    if (CVar_GetS32("gDpadEquips", 0) && (!CVar_GetS32("gDpadPause", 0) || CHECK_BTN_ALL(input->cur.button, BTN_CUP))) {
                        buttonsToCheck |= BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT;
                    }
                    if (CHECK_BTN_ANY(input->press.button, buttonsToCheck)) {
                        if (CHECK_SLOT_AGE(cursorSlot) &&
                            (cursorItem != ITEM_SOLD_OUT) && (cursorItem != ITEM_NONE)) {
                            KaleidoScope_SetupItemEquip(play, cursorItem, cursorSlot,
                                                        pauseCtx->itemVtx[index].v.ob[0] * 10,
                                                        pauseCtx->itemVtx[index].v.ob[1] * 10);
                        } else {
                            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }
                    }
                }
            } else {
                pauseCtx->cursorVtx[0].v.ob[0] = pauseCtx->cursorVtx[2].v.ob[0] = pauseCtx->cursorVtx[1].v.ob[0] =
                    pauseCtx->cursorVtx[3].v.ob[0] = 0;

                pauseCtx->cursorVtx[0].v.ob[1] = pauseCtx->cursorVtx[1].v.ob[1] = pauseCtx->cursorVtx[2].v.ob[1] =
                    pauseCtx->cursorVtx[3].v.ob[1] = -200;
            }
        } else {
            pauseCtx->cursorItem[PAUSE_ITEM] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_ITEM]) {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    } else if ((pauseCtx->unk_1E4 == 3) && (pauseCtx->pageIndex == PAUSE_ITEM)) {
        KaleidoScope_SetCursorVtx(pauseCtx, cursorSlot * 4, pauseCtx->itemVtx);
        pauseCtx->cursorColorSet = 4;
    }

    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_KAL_DISP++, 0, 0, 0, 0);

    for (i = 0, j = 24 * 4; i < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); i++, j += 4) {
        if ((gSaveContext.equips.buttonItems[i + 1] != ITEM_NONE) &&
            !((gSaveContext.equips.buttonItems[i + 1] >= ITEM_SHIELD_DEKU) &&
              (gSaveContext.equips.buttonItems[i + 1] <= ITEM_BOOTS_HOVER))) {
            gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[j], 4, 0);
            POLY_KAL_DISP = KaleidoScope_QuadTextureIA8(POLY_KAL_DISP, gEquippedItemOutlineTex, 32, 32, 0);
        }
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (i = j = 0; i < 24; i++, j += 4) {
        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (gSaveContext.inventory.items[i] != ITEM_NONE) {
            if ((pauseCtx->unk_1E4 == 0) && (pauseCtx->pageIndex == PAUSE_ITEM) && (pauseCtx->cursorSpecialPos == 0)) {
                if (CHECK_SLOT_AGE(i)) {
                    if ((sEquipState == 2) && (i == 3)) {
                        gDPSetPrimColor(POLY_KAL_DISP++, 0, 0, magicArrowEffectsR[pauseCtx->equipTargetItem - 0xBF],
                                        magicArrowEffectsG[pauseCtx->equipTargetItem - 0xBF],
                                        magicArrowEffectsB[pauseCtx->equipTargetItem - 0xBF], pauseCtx->alpha);

                        pauseCtx->itemVtx[j + 0].v.ob[0] = pauseCtx->itemVtx[j + 2].v.ob[0] =
                            pauseCtx->itemVtx[j + 0].v.ob[0] - 2;

                        pauseCtx->itemVtx[j + 1].v.ob[0] = pauseCtx->itemVtx[j + 3].v.ob[0] =
                            pauseCtx->itemVtx[j + 0].v.ob[0] + 32;

                        pauseCtx->itemVtx[j + 0].v.ob[1] = pauseCtx->itemVtx[j + 1].v.ob[1] =
                            pauseCtx->itemVtx[j + 0].v.ob[1] + 2;

                        pauseCtx->itemVtx[j + 2].v.ob[1] = pauseCtx->itemVtx[j + 3].v.ob[1] =
                            pauseCtx->itemVtx[j + 0].v.ob[1] - 32;
                    } else if (i == cursorSlot) {
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
            }

            gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[j + 0], 4, 0);
            int itemId = gSaveContext.inventory.items[i];
            bool not_acquired = !CHECK_ITEM_AGE(itemId);
            if (not_acquired) {
                gsDPSetGrayscaleColor(POLY_KAL_DISP++, 109, 109, 109, 255);
                gsSPGrayscale(POLY_KAL_DISP++, true);
            }
            KaleidoScope_DrawQuadTextureRGBA32(play->state.gfxCtx, gItemIcons[itemId], 32,
                                               32, 0);
            gsSPGrayscale(POLY_KAL_DISP++, false);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        KaleidoScope_DrawCursor(play, PAUSE_ITEM);
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    for (i = 0; i < 15; i++) {
        if ((gAmmoItems[i] != ITEM_NONE) && (gSaveContext.inventory.items[i] != ITEM_NONE)) {
            KaleidoScope_DrawAmmoCount(pauseCtx, play->state.gfxCtx, gSaveContext.inventory.items[i], i);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void KaleidoScope_SetupItemEquip(PlayState* play, u16 item, u16 slot, s16 animX, s16 animY) {
    Input* input = &play->state.input[0];
    PauseContext* pauseCtx = &play->pauseCtx;
    gSelectingMask = false;
    gSelectingAdultTrade = false;

    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
        pauseCtx->equipTargetCBtn = 0;
    } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
        pauseCtx->equipTargetCBtn = 1;
    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
        pauseCtx->equipTargetCBtn = 2;
    } else if (CVar_GetS32("gDpadEquips", 0)) {
        if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            pauseCtx->equipTargetCBtn = 3;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            pauseCtx->equipTargetCBtn = 4;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
            pauseCtx->equipTargetCBtn = 5;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
            pauseCtx->equipTargetCBtn = 6;
        }
    }

    pauseCtx->equipTargetItem = item;
    pauseCtx->equipTargetSlot = slot;
    pauseCtx->unk_1E4 = 3;
    pauseCtx->equipAnimX = animX;
    pauseCtx->equipAnimY = animY;
    pauseCtx->equipAnimAlpha = 255;
    sEquipAnimTimer = 0;
    sEquipState = 3;
    sEquipMoveTimer = 10;
    if ((pauseCtx->equipTargetItem == ITEM_ARROW_FIRE) || (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) ||
        (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT)) {
        if (CVar_GetS32("gSkipArrowAnimation", 0)) {
            Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            u16 index = 0;
            if (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) {
                index = 1;
            }
            if (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT) {
                index = 2;
            }
            Audio_PlaySoundGeneral(NA_SE_SY_SET_FIRE_ARROW + index, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            pauseCtx->equipTargetItem = 0xBF + index;
            sEquipState = 0;
            pauseCtx->equipAnimAlpha = 0;
            sEquipMoveTimer = 6;
        }
    } else {
        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

// TODO update for final positions
static s16 sCButtonPosX[] = { 66, 90, 114, 110, 110, 86, 134 };
static s16 sCButtonPosY[] = { 110, 92, 110, 76, 44, 62, 62 };

void KaleidoScope_UpdateItemEquip(PlayState* play) {
    static s16 D_8082A488 = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

    s16 Top_HUD_Margin = CVar_GetS32("gHUDMargin_T", 0);
    s16 Left_HUD_Margin = CVar_GetS32("gHUDMargin_L", 0);
    s16 Right_HUD_Margin = CVar_GetS32("gHUDMargin_R", 0);
    s16 Bottom_HUD_Margin = CVar_GetS32("gHUDMargin_B", 0);

    s16 X_Margins_CL;
    s16 X_Margins_CR;
    s16 X_Margins_CD;
    s16 Y_Margins_CL;
    s16 Y_Margins_CR;
    s16 Y_Margins_CD;
    s16 X_Margins_BtnB;
    s16 Y_Margins_BtnB;
    s16 X_Margins_DPad_Items;
    s16 Y_Margins_DPad_Items;
    if (CVar_GetS32("gBBtnUseMargins", 0) != 0) {
        if (CVar_GetS32("gBBtnPosType", 0) == 0) {X_Margins_BtnB = Right_HUD_Margin;};
        Y_Margins_BtnB = (Top_HUD_Margin*-1);
    } else {
        X_Margins_BtnB = 0;
        Y_Margins_BtnB = 0;
    }
    if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnLPosType", 0) == 0) {X_Margins_CL = Right_HUD_Margin;};
        Y_Margins_CL = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CL = 0;
        Y_Margins_CL = 0;
    }
    if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnRPosType", 0) == 0) {X_Margins_CR = Right_HUD_Margin;};
        Y_Margins_CR = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CR = 0;
        Y_Margins_CR = 0;
    }
    if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {
        if (CVar_GetS32("gCBtnDPosType", 0) == 0) {X_Margins_CD = Right_HUD_Margin;};
        Y_Margins_CD = (Top_HUD_Margin*-1);
    } else {
        X_Margins_CD = 0;
        Y_Margins_CD = 0;
    }
    if (CVar_GetS32("gDPadUseMargins", 0) != 0) {
        if (CVar_GetS32("gDPadPosType", 0) == 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
        Y_Margins_DPad_Items = (Top_HUD_Margin*-1);
    } else {
        X_Margins_DPad_Items = 0;
        Y_Margins_DPad_Items = 0;
    }
    const s16 ItemIconPos_ori[7][2] = {
        { C_LEFT_BUTTON_X+X_Margins_CL, C_LEFT_BUTTON_Y+Y_Margins_CL },
        { C_DOWN_BUTTON_X+X_Margins_CD, C_DOWN_BUTTON_Y+Y_Margins_CD },
        { C_RIGHT_BUTTON_X+X_Margins_CR, C_RIGHT_BUTTON_Y+Y_Margins_CR },
        { DPAD_UP_X+X_Margins_DPad_Items, DPAD_UP_Y+Y_Margins_DPad_Items },
        { DPAD_DOWN_X+X_Margins_DPad_Items, DPAD_DOWN_Y+Y_Margins_DPad_Items }, 
        { DPAD_LEFT_X+X_Margins_DPad_Items, DPAD_LEFT_Y+Y_Margins_DPad_Items }, 
        { DPAD_RIGHT_X+X_Margins_DPad_Items, DPAD_RIGHT_Y+Y_Margins_DPad_Items }
    };
    s16 DPad_ItemsOffset[4][2] = {
        { 7,-8},//Up
        { 7,24},//Down
        {-9, 8},//Left
        {23, 8},//Right
    }; //(X,Y) Used with custom position to place it properly.

    //DPadItems
    if (CVar_GetS32("gDPadPosType", 0) != 0) {
        sCButtonPosY[3] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[0][1];//Up
        sCButtonPosY[4] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[1][1];//Down
        sCButtonPosY[5] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[2][1];//Left
        sCButtonPosY[6] = CVar_GetS32("gDPadPosY", 0)+Y_Margins_DPad_Items+DPad_ItemsOffset[3][1];//Right
        if (CVar_GetS32("gDPadPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Left_HUD_Margin;};
            sCButtonPosX[3] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            sCButtonPosX[4] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            sCButtonPosX[5] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            sCButtonPosX[6] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gDPadUseMargins", 0) != 0) {X_Margins_DPad_Items = Right_HUD_Margin;};
            sCButtonPosX[3] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[0][0]);
            sCButtonPosX[4] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[1][0]);
            sCButtonPosX[5] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[2][0]);
            sCButtonPosX[6] = OTRGetDimensionFromRightEdge(CVar_GetS32("gDPadPosX", 0)+X_Margins_DPad_Items+DPad_ItemsOffset[3][0]);
        } else if (CVar_GetS32("gDPadPosType", 0) == 3) {//Anchor None
            sCButtonPosX[3] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[0][0];
            sCButtonPosX[4] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[1][0];
            sCButtonPosX[5] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[2][0];
            sCButtonPosX[6] = CVar_GetS32("gDPadPosX", 0)+DPad_ItemsOffset[3][0];
        }
    } else {
        sCButtonPosX[3] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[3][0]);
        sCButtonPosX[4] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[4][0]);
        sCButtonPosX[5] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[5][0]);
        sCButtonPosX[6] = OTRGetDimensionFromRightEdge(ItemIconPos_ori[6][0]);
        sCButtonPosY[3] = ItemIconPos_ori[3][1];
        sCButtonPosY[4] = ItemIconPos_ori[4][1];
        sCButtonPosY[5] = ItemIconPos_ori[5][1];
        sCButtonPosY[6] = ItemIconPos_ori[6][1];
    }
    //C button Left
    if (CVar_GetS32("gCBtnLPosType", 0) != 0) {
        sCButtonPosY[0] = CVar_GetS32("gCBtnLPosY", 0)+Y_Margins_CL;
        if (CVar_GetS32("gCBtnLPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Left_HUD_Margin;};
            sCButtonPosX[0] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnLUseMargins", 0) != 0) {X_Margins_CL = Right_HUD_Margin;};
            sCButtonPosX[0] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnLPosX", 0)+X_Margins_CL);
        } else if (CVar_GetS32("gCBtnLPosType", 0) == 3) {//Anchor None
            sCButtonPosX[0] = CVar_GetS32("gCBtnLPosX", 0);
        }
    } else {
        sCButtonPosX[0] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[0][0]);
        sCButtonPosY[0] = ItemIconPos_ori[0][1];
    }
    //C Button down
    if (CVar_GetS32("gCBtnDPosType", 0) != 0) {
        sCButtonPosY[1] = CVar_GetS32("gCBtnDPosY", 0)+Y_Margins_CD;
        if (CVar_GetS32("gCBtnDPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Left_HUD_Margin;};
            sCButtonPosX[1] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnDUseMargins", 0) != 0) {X_Margins_CD = Right_HUD_Margin;};
            sCButtonPosX[1] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnDPosX", 0)+X_Margins_CD);
        } else if (CVar_GetS32("gCBtnDPosType", 0) == 3) {//Anchor None
            sCButtonPosX[1] = CVar_GetS32("gCBtnDPosX", 0);
        }
    } else {
        sCButtonPosX[1] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[1][0]);
        sCButtonPosY[1] = ItemIconPos_ori[1][1];
    }
    //C button Right
    if (CVar_GetS32("gCBtnRPosType", 0) != 0) {
        sCButtonPosY[2] = CVar_GetS32("gCBtnRPosY", 0)+Y_Margins_CR;
        if (CVar_GetS32("gCBtnRPosType", 0) == 1) {//Anchor Left
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Left_HUD_Margin;};
            sCButtonPosX[2] = OTRGetDimensionFromLeftEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 2) {//Anchor Right
            if (CVar_GetS32("gCBtnRUseMargins", 0) != 0) {X_Margins_CR = Right_HUD_Margin;};
            sCButtonPosX[2] = OTRGetDimensionFromRightEdge(CVar_GetS32("gCBtnRPosX", 0)+X_Margins_CR);
        } else if (CVar_GetS32("gCBtnRPosType", 0) == 3) {//Anchor None
            sCButtonPosX[2] = CVar_GetS32("gCBtnRPosX", 0);
        }
    } else {
        sCButtonPosX[2] = OTRGetRectDimensionFromRightEdge(ItemIconPos_ori[2][0]);
        sCButtonPosY[2] = ItemIconPos_ori[2][1];
    }

    sCButtonPosX[0] = sCButtonPosX[0] - 160;
    sCButtonPosY[0] = 120 - sCButtonPosY[0];
    sCButtonPosX[1] = sCButtonPosX[1] - 160;
    sCButtonPosY[1] = 120 - sCButtonPosY[1];
    sCButtonPosX[2] = sCButtonPosX[2] - 160;
    sCButtonPosY[2] = 120 - sCButtonPosY[2];
    sCButtonPosX[3] = sCButtonPosX[3] - 160;
    sCButtonPosY[3] = 120 - sCButtonPosY[3];
    sCButtonPosX[4] = sCButtonPosX[4] - 160;
    sCButtonPosY[4] = 120 - sCButtonPosY[4];
    sCButtonPosX[5] = sCButtonPosX[5] - 160;
    sCButtonPosY[5] = 120 - sCButtonPosY[5];
    sCButtonPosX[6] = sCButtonPosX[6] - 160;
    sCButtonPosY[6] = 120 - sCButtonPosY[6];

    if (sEquipState == 0) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
            sEquipState++;
        }
        sEquipAnimTimer = 5;
        return;
    }

    if (sEquipState == 2) {
        D_8082A488--;

        if (D_8082A488 == 0) {
            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
            if (!CVar_GetS32("gSeparateArrows", 0)) {
                pauseCtx->equipTargetSlot = SLOT_BOW;
            }
            sEquipMoveTimer = 6;
            WREG(90) = 320;
            WREG(87) = WREG(91);
            sEquipState++;
            Audio_PlaySoundGeneral(NA_SE_SY_SYNTH_MAGIC_ARROW, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        return;
    }

    if (sEquipState == 1) {
        bowItemVtx = &pauseCtx->itemVtx[12];
        offsetX = ABS(pauseCtx->equipAnimX - bowItemVtx->v.ob[0] * 10) / sEquipMoveTimer;
        offsetY = ABS(pauseCtx->equipAnimY - bowItemVtx->v.ob[1] * 10) / sEquipMoveTimer;
    } else {
        offsetX = ABS(pauseCtx->equipAnimX - sCButtonPosX[pauseCtx->equipTargetCBtn] * 10) / sEquipMoveTimer;
        offsetY = ABS(pauseCtx->equipAnimY - sCButtonPosY[pauseCtx->equipTargetCBtn] * 10) / sEquipMoveTimer;
    }

    if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipAnimAlpha < 254)) {
        pauseCtx->equipAnimAlpha += 14;
        if (pauseCtx->equipAnimAlpha > 255) {
            pauseCtx->equipAnimAlpha = 254;
        }
        sEquipAnimTimer = 5;
        return;
    }

    if (sEquipAnimTimer == 0) {
        WREG(90) -= WREG(87) / sEquipMoveTimer;
        WREG(87) -= WREG(87) / sEquipMoveTimer;

        if (sEquipState == 1) {
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
            if (pauseCtx->equipAnimX >= sCButtonPosX[pauseCtx->equipTargetCBtn] * 10) {
                pauseCtx->equipAnimX -= offsetX;
            } else {
                pauseCtx->equipAnimX += offsetX;
            }

            if (pauseCtx->equipAnimY >= sCButtonPosY[pauseCtx->equipTargetCBtn] * 10) {
                pauseCtx->equipAnimY -= offsetY;
            } else {
                pauseCtx->equipAnimY += offsetY;
            }
        }

        sEquipMoveTimer--;

        if (sEquipMoveTimer == 0) {
            if (sEquipState == 1) {
                sEquipState++;
                D_8082A488 = 4;
                return;
            }

            osSyncPrintf("\n＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝\n");

			// Skipping the arrow animation: need to change the item's type and
            // slot when it hits the button since it didn't get set earlier
            if (pauseCtx->equipTargetItem == ITEM_ARROW_FIRE || pauseCtx->equipTargetItem == ITEM_ARROW_ICE ||
                pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT) {
                switch (pauseCtx->equipTargetItem) {
                    case ITEM_ARROW_FIRE:
                        pauseCtx->equipTargetItem = ITEM_BOW_ARROW_FIRE;
                        break;
                    case ITEM_ARROW_ICE:
                        pauseCtx->equipTargetItem = ITEM_BOW_ARROW_ICE;
                        break;
                    case ITEM_ARROW_LIGHT:
                        pauseCtx->equipTargetItem = ITEM_BOW_ARROW_LIGHT;
                        break;
                }
                if (!CVar_GetS32("gSeparateArrows", 0)) {
                    pauseCtx->equipTargetSlot = SLOT_BOW;
                }
            }
            
            // If the item is on another button already, swap the two
            uint16_t targetButtonIndex = pauseCtx->equipTargetCBtn + 1;
            for (uint16_t otherSlotIndex = 0; otherSlotIndex < ARRAY_COUNT(gSaveContext.equips.cButtonSlots);
                 otherSlotIndex++) {
                uint16_t otherButtonIndex = otherSlotIndex + 1;
                if (otherSlotIndex == pauseCtx->equipTargetCBtn) {
                    continue;
                }

                if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[otherSlotIndex]) {
                    // Assign the other button to the target's current item
                    if (gSaveContext.equips.buttonItems[targetButtonIndex] != ITEM_NONE) {
                        gSaveContext.equips.buttonItems[otherButtonIndex] =
                            gSaveContext.equips.buttonItems[targetButtonIndex];
                        gSaveContext.equips.cButtonSlots[otherSlotIndex] =
                            gSaveContext.equips.cButtonSlots[pauseCtx->equipTargetCBtn];
                        Interface_LoadItemIcon2(play, otherButtonIndex);
                    } else {
                        gSaveContext.equips.buttonItems[otherButtonIndex] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[otherSlotIndex] = SLOT_NONE;
                    }
                    break; // Assume there is only one possible pre-existing equip
                }
            }

            gSaveContext.equips.buttonItems[targetButtonIndex] = pauseCtx->equipTargetItem;
            gSaveContext.equips.cButtonSlots[pauseCtx->equipTargetCBtn] = pauseCtx->equipTargetSlot;
            Interface_LoadItemIcon1(play, targetButtonIndex);

            pauseCtx->unk_1E4 = 0;
            sEquipMoveTimer = 10;
            WREG(90) = 320;
            WREG(87) = WREG(91);
        }
    } else {
        sEquipAnimTimer--;
        if (sEquipAnimTimer == 0) {
            pauseCtx->equipAnimAlpha = 255;
        }
    }
}
