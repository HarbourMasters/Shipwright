#include "z_kaleido_scope.h"
#include "textures/parameter_static/parameter_static.h"

u8 gAmmoItems[] = {
    ITEM_STICK,   ITEM_NUT,  ITEM_BOMB, ITEM_BOW,  ITEM_NONE, ITEM_NONE, ITEM_SLINGSHOT, ITEM_NONE,
    ITEM_BOMBCHU, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_BEAN,      ITEM_NONE,
};

static s16 sEquipState = 0;
static s16 sEquipAnimTimer = 0;
static s16 sEquipMoveTimer = 10;

static s16 sAmmoVtxOffset[] = {
    0, 2, 4, 6, 99, 99, 8, 99, 99, 10, 99, 99, 99, 99, 99, 99, 12,
};

extern const char* _gAmmoDigit0Tex[];

void KaleidoScope_DrawAmmoCount(PauseContext* pauseCtx, GraphicsContext* gfxCtx, s16 item) {
    s16 ammo;
    s16 i;

    OPEN_DISPS(gfxCtx, "../z_kaleido_item.c", 69);

    ammo = AMMO(item);

    gDPPipeSync(POLY_KAL_DISP++);

    if (!((gSlotAgeReqs[SLOT(item)] == 9) || gSlotAgeReqs[SLOT(item)] == ((void)0, gSaveContext.linkAge))) {
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
        gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[(sAmmoVtxOffset[item] + 27) * 4], 4, 0);

        gDPLoadTextureBlock(POLY_KAL_DISP++, ((u8*)_gAmmoDigit0Tex[i]), G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);
    }

    gSPVertex(POLY_KAL_DISP++, &pauseCtx->itemVtx[(sAmmoVtxOffset[item] + 28) * 4], 4, 0);

    gDPLoadTextureBlock(POLY_KAL_DISP++, ((u8*)_gAmmoDigit0Tex[ammo]), G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_KAL_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(gfxCtx, "../z_kaleido_item.c", 116);
}

void KaleidoScope_SetCursorVtx(PauseContext* pauseCtx, u16 index, Vtx* vtx) {
    pauseCtx->cursorVtx[0].v.ob[0] = vtx[index].v.ob[0];
    pauseCtx->cursorVtx[0].v.ob[1] = vtx[index].v.ob[1];
    KaleidoScope_UpdateCursorSize(pauseCtx); // OTRTODO Why is this needed?
}

void KaleidoScope_SetItemCursorVtx(PauseContext* pauseCtx) {
    KaleidoScope_SetCursorVtx(pauseCtx, pauseCtx->cursorSlot[PAUSE_ITEM] * 4, pauseCtx->itemVtx);
}

void KaleidoScope_DrawItemSelect(GlobalContext* globalCtx) {
    static s16 magicArrowEffectsR[] = { 255, 100, 255 };
    static s16 magicArrowEffectsG[] = { 0, 100, 255 };
    static s16 magicArrowEffectsB[] = { 0, 255, 100 };
    Input* input = &globalCtx->state.input[0];
    PauseContext* pauseCtx = &globalCtx->pauseCtx;
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
    bool dpad = CVar_GetS32("gDpadPauseName", 0);

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_kaleido_item.c", 234);

    func_800949A8(globalCtx->state.gfxCtx);

    gDPSetCombineMode(POLY_KAL_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    pauseCtx->cursorColorSet = 0;
    pauseCtx->nameColorSet = 0;

    if ((pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0) && (pauseCtx->pageIndex == PAUSE_ITEM)) {
        moveCursorResult = 0;
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

                            if (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) {
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

                                KaleidoScope_MoveCursorToSpecialPos(globalCtx, PAUSE_CURSOR_PAGE_LEFT);

                                moveCursorResult = 2;
                            }
                        }
                    } else if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                        if (pauseCtx->cursorX[PAUSE_ITEM] < 5) {
                            pauseCtx->cursorX[PAUSE_ITEM] += 1;
                            pauseCtx->cursorPoint[PAUSE_ITEM] += 1;

                            if (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) {
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

                                KaleidoScope_MoveCursorToSpecialPos(globalCtx, PAUSE_CURSOR_PAGE_RIGHT);

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

                    KaleidoScope_MoveCursorToSpecialPos(globalCtx, PAUSE_CURSOR_PAGE_RIGHT);
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

                    KaleidoScope_MoveCursorToSpecialPos(globalCtx, PAUSE_CURSOR_PAGE_LEFT);
                    break;
                }
            }
        }

        if (pauseCtx->cursorSpecialPos == 0) {
            if (cursorItem != PAUSE_ITEM_NONE) {
                if ((ABS(pauseCtx->stickRelY) > 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
                    moveCursorResult = 0;

                    cursorPoint = pauseCtx->cursorPoint[PAUSE_ITEM];
                    cursorY = pauseCtx->cursorY[PAUSE_ITEM];
                    while (moveCursorResult == 0) {
                        if ((pauseCtx->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                            if (pauseCtx->cursorY[PAUSE_ITEM] != 0) {
                                pauseCtx->cursorY[PAUSE_ITEM] -= 1;
                                pauseCtx->cursorPoint[PAUSE_ITEM] -= 6;

                                if (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) {
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

                                if (gSaveContext.inventory.items[pauseCtx->cursorPoint[PAUSE_ITEM]] != ITEM_NONE) {
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

            if (!((gSlotAgeReqs[cursorSlot] == 9) || (gSlotAgeReqs[cursorSlot] == ((void)0, gSaveContext.linkAge)))) {
                pauseCtx->nameColorSet = 1;
            }

            if (cursorItem != PAUSE_ITEM_NONE) {
                index = cursorSlot * 4; // required to match?
                KaleidoScope_SetCursorVtx(pauseCtx, index, pauseCtx->itemVtx);

                if ((pauseCtx->debugState == 0) && (pauseCtx->state == 6) && (pauseCtx->unk_1E4 == 0)) {
                    if (CHECK_BTN_ANY(input->press.button, BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT)) {
                        if (((gSlotAgeReqs[cursorSlot] == 9) ||
                             (gSlotAgeReqs[cursorSlot] == ((void)0, gSaveContext.linkAge))) &&
                            (cursorItem != ITEM_SOLD_OUT)) {
                            if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                                pauseCtx->equipTargetCBtn = 0;
                            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                                pauseCtx->equipTargetCBtn = 1;
                            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                                pauseCtx->equipTargetCBtn = 2;
                            }

                            pauseCtx->equipTargetItem = cursorItem;
                            pauseCtx->equipTargetSlot = cursorSlot;
                            pauseCtx->unk_1E4 = 3;
                            pauseCtx->equipAnimX = pauseCtx->itemVtx[index].v.ob[0] * 10;
                            pauseCtx->equipAnimY = pauseCtx->itemVtx[index].v.ob[1] * 10;
                            pauseCtx->equipAnimAlpha = 255;
                            sEquipAnimTimer = 0;
                            sEquipState = 3;
                            sEquipMoveTimer = 10;
                            if ((pauseCtx->equipTargetItem == ITEM_ARROW_FIRE) ||
                                (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) ||
                                (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT)) {
                                index = 0;
                                if (pauseCtx->equipTargetItem == ITEM_ARROW_ICE) {
                                    index = 1;
                                }
                                if (pauseCtx->equipTargetItem == ITEM_ARROW_LIGHT) {
                                    index = 2;
                                }
                                Audio_PlaySoundGeneral(NA_SE_SY_SET_FIRE_ARROW + index, &D_801333D4, 4, &D_801333E0,
                                                       &D_801333E0, &D_801333E8);
                                pauseCtx->equipTargetItem = 0xBF + index;
                                sEquipState = 0;
                                pauseCtx->equipAnimAlpha = 0;
                                sEquipMoveTimer = 6;
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                       &D_801333E8);
                            }
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

    for (i = 0, j = 24 * 4; i < 3; i++, j += 4) {
        if (gSaveContext.equips.buttonItems[i + 1] != ITEM_NONE) {
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
                if ((gSlotAgeReqs[i] == 9) || (gSlotAgeReqs[i] == ((void)0, gSaveContext.linkAge))) {
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
            bool not_acquired = (gItemAgeReqs[itemId] != 9) && (gItemAgeReqs[itemId] != gSaveContext.linkAge);
            if (not_acquired) {
                gsDPSetGrayscaleColor(POLY_KAL_DISP++, 109, 109, 109, 255);
                gsSPGrayscale(POLY_KAL_DISP++, true);
            }
            KaleidoScope_DrawQuadTextureRGBA32(globalCtx->state.gfxCtx, gItemIcons[itemId], 32,
                                               32, 0);
            gsSPGrayscale(POLY_KAL_DISP++, false);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        KaleidoScope_DrawCursor(globalCtx, PAUSE_ITEM);
    }

    gDPPipeSync(POLY_KAL_DISP++);
    gDPSetCombineLERP(POLY_KAL_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    for (i = 0; i < 15; i++) {
        if ((gAmmoItems[i] != ITEM_NONE) && (gSaveContext.inventory.items[i] != ITEM_NONE)) {
            KaleidoScope_DrawAmmoCount(pauseCtx, globalCtx->state.gfxCtx, gSaveContext.inventory.items[i]);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_kaleido_item.c", 516);
}

static s16 sCButtonPosX[] = { 66, 90, 114 };
static s16 sCButtonPosY[] = { 110, 92, 110 };

void KaleidoScope_UpdateItemEquip(GlobalContext* globalCtx) {
    static s16 D_8082A488 = 0;
    PauseContext* pauseCtx = &globalCtx->pauseCtx;
    Vtx* bowItemVtx;
    u16 offsetX;
    u16 offsetY;

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
            pauseCtx->equipTargetSlot = SLOT_BOW;
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
        offsetX = ABS(pauseCtx->equipAnimX - OTRGetRectDimensionFromRightEdge(sCButtonPosX[pauseCtx->equipTargetCBtn]) * 10) / sEquipMoveTimer;
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
            if (pauseCtx->equipAnimX >= OTRGetRectDimensionFromRightEdge(sCButtonPosX[pauseCtx->equipTargetCBtn]) * 10) {
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

            if (pauseCtx->equipTargetCBtn == 0) {

                if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[1]) {
                    if (gSaveContext.equips.buttonItems[1] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[1] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[2] = gSaveContext.equips.buttonItems[1];
                            gSaveContext.equips.cButtonSlots[1] = gSaveContext.equips.cButtonSlots[0];
                            Interface_LoadItemIcon2(globalCtx, 2);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[2] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[1] = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[2]) {
                    if (gSaveContext.equips.buttonItems[1] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[1] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[3] = gSaveContext.equips.buttonItems[1];
                            gSaveContext.equips.cButtonSlots[2] = gSaveContext.equips.cButtonSlots[0];
                            Interface_LoadItemIcon2(globalCtx, 3);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[3] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[2] = SLOT_NONE;
                    }
                }

                if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1)) {
                    if ((gSaveContext.equips.buttonItems[1] == ITEM_BOW) ||
                        ((gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE) &&
                         (gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE) &&
                        (gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[2] = gSaveContext.equips.buttonItems[1];
                        gSaveContext.equips.cButtonSlots[1] = gSaveContext.equips.cButtonSlots[0];
                        Interface_LoadItemIcon2(globalCtx, 2);
                    } else if ((gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE) &&
                               (gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[3] = gSaveContext.equips.buttonItems[1];
                        gSaveContext.equips.cButtonSlots[2] = gSaveContext.equips.cButtonSlots[0];
                        Interface_LoadItemIcon2(globalCtx, 3);
                    }
                }

                gSaveContext.equips.buttonItems[1] = pauseCtx->equipTargetItem;
                gSaveContext.equips.cButtonSlots[0] = pauseCtx->equipTargetSlot;
                Interface_LoadItemIcon1(globalCtx, 1);

                osSyncPrintf("Ｃ左sl_item_no=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetItem,
                             gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                             gSaveContext.equips.buttonItems[3]);
                osSyncPrintf("Ｃ左sl_number=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetSlot,
                             gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                             gSaveContext.equips.cButtonSlots[2]);
            } else if (pauseCtx->equipTargetCBtn == 1) {
                osSyncPrintf("Ｃ下sl_item_no=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetItem,
                             gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                             gSaveContext.equips.buttonItems[3]);
                osSyncPrintf("Ｃ下sl_number=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetSlot,
                             gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                             gSaveContext.equips.cButtonSlots[2]);

                if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[0]) {
                    if (gSaveContext.equips.buttonItems[2] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[2] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[1] = gSaveContext.equips.buttonItems[2];
                            gSaveContext.equips.cButtonSlots[0] = gSaveContext.equips.cButtonSlots[1];
                            Interface_LoadItemIcon2(globalCtx, 1);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[1] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[0] = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[2]) {
                    if (gSaveContext.equips.buttonItems[2] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[2] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[3] = gSaveContext.equips.buttonItems[2];
                            gSaveContext.equips.cButtonSlots[2] = gSaveContext.equips.cButtonSlots[1];
                            Interface_LoadItemIcon2(globalCtx, 3);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[3] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[2] = SLOT_NONE;
                    }
                }

                if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1)) {
                    if ((gSaveContext.equips.buttonItems[2] == ITEM_BOW) ||
                        ((gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE) &&
                         (gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE) &&
                        (gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[1] = gSaveContext.equips.buttonItems[2];
                        Interface_LoadItemIcon2(globalCtx, 1);
                    } else if ((gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE) &&
                               (gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[3] = gSaveContext.equips.buttonItems[2];
                        Interface_LoadItemIcon2(globalCtx, 3);
                    }
                }

                gSaveContext.equips.buttonItems[2] = pauseCtx->equipTargetItem;
                gSaveContext.equips.cButtonSlots[1] = pauseCtx->equipTargetSlot;
                Interface_LoadItemIcon1(globalCtx, 2);

                osSyncPrintf("Ｃ下sl_item_no=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetItem,
                             gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                             gSaveContext.equips.buttonItems[3]);
                osSyncPrintf("Ｃ下sl_number=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetSlot,
                             gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                             gSaveContext.equips.cButtonSlots[2]);
            } else {
                osSyncPrintf("Ｃ右sl_item_no=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetItem,
                             gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                             gSaveContext.equips.buttonItems[3]);
                osSyncPrintf("Ｃ右sl_number=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetSlot,
                             gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                             gSaveContext.equips.cButtonSlots[2]);

                if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[0]) {
                    if (gSaveContext.equips.buttonItems[3] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[3] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[1] = gSaveContext.equips.buttonItems[3];
                            gSaveContext.equips.cButtonSlots[0] = gSaveContext.equips.cButtonSlots[2];
                            Interface_LoadItemIcon2(globalCtx, 1);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[1] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[0] = SLOT_NONE;
                    }
                } else if (pauseCtx->equipTargetSlot == gSaveContext.equips.cButtonSlots[1]) {
                    if (gSaveContext.equips.buttonItems[3] != ITEM_NONE) {
                        if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1) &&
                            ((gSaveContext.equips.buttonItems[3] == ITEM_BOW) ||
                             ((gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE) &&
                              (gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT)))) {
                            pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                            pauseCtx->equipTargetSlot = SLOT_BOW;
                        } else {
                            gSaveContext.equips.buttonItems[2] = gSaveContext.equips.buttonItems[3];
                            gSaveContext.equips.cButtonSlots[1] = gSaveContext.equips.cButtonSlots[2];
                            Interface_LoadItemIcon2(globalCtx, 2);
                        }
                    } else {
                        gSaveContext.equips.buttonItems[2] = ITEM_NONE;
                        gSaveContext.equips.cButtonSlots[1] = SLOT_NONE;
                    }
                }

                if ((pauseCtx->equipTargetItem >= 0xBF) && (pauseCtx->equipTargetItem <= 0xC1)) {
                    if ((gSaveContext.equips.buttonItems[3] == ITEM_BOW) ||
                        ((gSaveContext.equips.buttonItems[3] >= ITEM_BOW_ARROW_FIRE) &&
                         (gSaveContext.equips.buttonItems[3] <= ITEM_BOW_ARROW_LIGHT))) {
                        pauseCtx->equipTargetItem -= 0xBF - ITEM_BOW_ARROW_FIRE;
                        pauseCtx->equipTargetSlot = SLOT_BOW;
                    }
                } else if (pauseCtx->equipTargetItem == ITEM_BOW) {
                    if ((gSaveContext.equips.buttonItems[1] >= ITEM_BOW_ARROW_FIRE) &&
                        (gSaveContext.equips.buttonItems[1] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[1] = gSaveContext.equips.buttonItems[3];
                        Interface_LoadItemIcon2(globalCtx, 1);
                    } else if ((gSaveContext.equips.buttonItems[2] >= ITEM_BOW_ARROW_FIRE) &&
                               (gSaveContext.equips.buttonItems[2] <= ITEM_BOW_ARROW_LIGHT)) {
                        gSaveContext.equips.buttonItems[2] = gSaveContext.equips.buttonItems[3];
                        Interface_LoadItemIcon2(globalCtx, 2);
                    }
                }

                gSaveContext.equips.buttonItems[3] = pauseCtx->equipTargetItem;
                gSaveContext.equips.cButtonSlots[2] = pauseCtx->equipTargetSlot;
                Interface_LoadItemIcon1(globalCtx, 3);

                osSyncPrintf("Ｃ右sl_item_no=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetItem,
                             gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                             gSaveContext.equips.buttonItems[3]);
                osSyncPrintf("Ｃ右sl_number=%d (1)=%d (2)=%d (3)=%d\n", pauseCtx->equipTargetSlot,
                             gSaveContext.equips.cButtonSlots[0], gSaveContext.equips.cButtonSlots[1],
                             gSaveContext.equips.cButtonSlots[2]);
            }

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
