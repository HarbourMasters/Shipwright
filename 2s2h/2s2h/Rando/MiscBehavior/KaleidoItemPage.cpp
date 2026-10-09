#include "MiscBehavior.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64interface.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "archives/icon_item_static/icon_item_static_yar.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
}

// Vertices for the circle behind the items
static Vtx sCycleCircleVtx[] = {
    // Left Item
    VTX(-56, 24, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-8, 24, 0, 48 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-56, -24, 0, 0 << 5, 48 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-8, -24, 0, 48 << 5, 48 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    // Right Item
    VTX(8, 24, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(56, 24, 0, 48 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(8, -24, 0, 0 << 5, 48 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(56, -24, 0, 48 << 5, 48 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
};

static int sCycleActiveAnimTimer = 0;
static int sCurrentItemCyclingSlot = SLOT_NONE;
static int sCurrentAnimatingSlot = SLOT_NONE;
static int sPrevKaleidoCursorSlot = SLOT_NONE;

enum CycleDirection {
    CYCLE_NONE,
    CYCLE_LEFT,
    CYCLE_RIGHT,
};

u8 GetPreviousListEntry(const std::vector<u8>& list, u8 current) {
    if (list.size() == 0) {
        return current;
    }
    if (list.size() < 2) {
        return list.at(0);
    }

    for (size_t i = 0; i < list.size(); i++) {
        u8 entry = list.at(i);

        if (entry != current) {
            continue;
        }

        if (i == 0) {
            return list.at(list.size() - 1);
        } else {
            return list.at(i - 1);
        }
    }

    return list.at(list.size() - 1);
}

u8 GetNextListEntry(const std::vector<u8>& list, u8 current) {
    if (list.size() == 0) {
        return current;
    }
    if (list.size() < 2) {
        return list.at(0);
    }

    for (size_t i = 0; i < list.size(); i++) {
        u8 entry = list.at(i);

        if (entry != current) {
            continue;
        }

        if (i == list.size() - 1) {
            return list.at(0);
        } else {
            return list.at(i + 1);
        }
    }

    return list.at(0);
}

std::vector<u8> BuildAvailableItemsList(u8 slot) {
    std::vector<u8> availableItems;
    switch (slot) {
        case SLOT_TRADE_DEED:
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR)) {
                availableItems.push_back(ITEM_MOONS_TEAR);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND)) {
                availableItems.push_back(ITEM_DEED_LAND);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP)) {
                availableItems.push_back(ITEM_DEED_SWAMP);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN)) {
                availableItems.push_back(ITEM_DEED_OCEAN);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN)) {
                availableItems.push_back(ITEM_DEED_MOUNTAIN);
            }
            break;
        case SLOT_TRADE_KEY_MAMA:
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY)) {
                availableItems.push_back(ITEM_ROOM_KEY);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA)) {
                availableItems.push_back(ITEM_LETTER_MAMA);
            }
            break;
        case SLOT_TRADE_COUPLE:
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES)) {
                availableItems.push_back(ITEM_PENDANT_OF_MEMORIES);
            }
            if (Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI)) {
                availableItems.push_back(ITEM_LETTER_TO_KAFEI);
            }
            break;
    }

    return availableItems;
}

void DrawItemCycleExtras(PlayState* play, u8 slot, u8 canCycle, u8 leftItem, u8 rightItem) {
    PauseContext* pauseCtx = &play->pauseCtx;

    u8 isCycling = sCurrentItemCyclingSlot == slot;

    u8 itemId = gSaveContext.save.saveInfo.inventory.items[slot];
    u8 showLeftItem = leftItem != ITEM_NONE && itemId != leftItem && leftItem != rightItem;
    u8 showRightItem = rightItem != ITEM_NONE && itemId != rightItem;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL42_Opa(play->state.gfxCtx);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // Render the extra cycle items if at least the left or right item are valid
    if (canCycle && (showLeftItem || showRightItem)) {
        Matrix_Push();

        Vtx* itemTopLeft = &pauseCtx->itemVtx[slot * 4];
        Vtx* itemBottomRight = &itemTopLeft[3];

        s16 halfX = (itemBottomRight->v.ob[0] - itemTopLeft->v.ob[0]) / 2;
        s16 halfY = (itemBottomRight->v.ob[1] - itemTopLeft->v.ob[1]) / 2;

        Matrix_Translate(itemTopLeft->v.ob[0] + halfX, itemTopLeft->v.ob[1] + halfY, 0, MTXMODE_APPLY);

        f32 animScale = (f32)(5 - (sCurrentAnimatingSlot == slot ? sCycleActiveAnimTimer : 0)) / 5;

        // When not cycling or actively animating, shrink and move the items under the main slot item
        if (!isCycling || sCycleActiveAnimTimer < 5) {
            f32 finalScale = 1.0f - (0.675f * animScale);
            Matrix_Translate(0, -15.0f * animScale, 0, MTXMODE_APPLY);
            Matrix_Scale(finalScale, finalScale, 1.0f, MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        // Render A button indicator when hovered and not cycling
        if (!isCycling && sCycleActiveAnimTimer == 0 && pauseCtx->cursorSlot[PAUSE_ITEM] == slot &&
            pauseCtx->cursorSpecialPos == 0) {
            Ship_DrawKaleidoCycleAButtonPrompt(play, pauseCtx->alpha);
        }

        // Render a dark circle behind the extra items when cycling
        if (isCycling) {
            gSPVertex(POLY_OPA_DISP++, (uintptr_t)sCycleCircleVtx, 8, 0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha * (1.0f - animScale));
            gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gPausePromptCursorTex, G_IM_FMT_I, 48, 48, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);

            if (showLeftItem) {
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            }
            if (showRightItem) {
                gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
            }
        }

        // Render left and right items
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        gSPVertex(POLY_OPA_DISP++, (uintptr_t)sCycleExtraItemVtx, 8, 0);

        if (showLeftItem) {
            KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[leftItem], 32, 32, 0);
        }
        if (showRightItem) {
            KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[rightItem], 32, 32, 4);
        }

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Rando::MiscBehavior::InitKaleidoItemPage() {
    COND_HOOK(OnKaleidoUpdate, IS_RANDO, [](PauseContext* pauseCtx) {
        InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;

        if ((pauseCtx->state != PAUSE_STATE_MAIN)) {
            sCycleActiveAnimTimer = 0;
            sCurrentItemCyclingSlot = SLOT_NONE;
            sCurrentAnimatingSlot = SLOT_NONE;
            sPrevKaleidoCursorSlot = SLOT_NONE;
            return;
        }

        // Update active cycling animation timer
        if (sCurrentItemCyclingSlot == SLOT_NONE) {
            if (sCycleActiveAnimTimer > 0) {
                sCycleActiveAnimTimer--;
            } else {
                sCurrentAnimatingSlot = SLOT_NONE;
            }
        } else {
            if (sCycleActiveAnimTimer < 5) {
                sCycleActiveAnimTimer++;
            }
        }

        u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];

        if ((pauseCtx->debugEditor != DEBUG_EDITOR_NONE) || (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE) ||
            (slot != SLOT_TRADE_COUPLE && slot != SLOT_TRADE_DEED && slot != SLOT_TRADE_KEY_MAMA)) {
            if (sPrevKaleidoCursorSlot == SLOT_TRADE_DEED || sPrevKaleidoCursorSlot == SLOT_TRADE_KEY_MAMA ||
                sPrevKaleidoCursorSlot == SLOT_TRADE_COUPLE) {
                // Reset A button back to Info when going away from a cycle-able item
                if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
                    Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
                }
            }

            sPrevKaleidoCursorSlot = slot;
            return;
        }

        std::vector<u8> availableItems = BuildAvailableItemsList(slot);
        u16 itemId = pauseCtx->cursorItem[PAUSE_ITEM];
        CycleDirection direction = CYCLE_NONE;

        if (availableItems.size() == 0) {
            // Nothing to cycle, switch back to Info on A button
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
            }

            sPrevKaleidoCursorSlot = slot;
            return;
        }

        if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_A)) {
            if (slot == sCurrentItemCyclingSlot) {
                Audio_PlaySfx(NA_SE_SY_CANCEL);
                sCurrentItemCyclingSlot = SLOT_NONE;
                pauseCtx->itemDescriptionOn = false;
            } else {
                if (availableItems.size() > 2 || (itemId == PAUSE_ITEM_NONE && availableItems.size() > 1)) {
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    sCurrentItemCyclingSlot = slot;
                    // Used to make Kaleido stop processing inputs and updating on the main pages
                    pauseCtx->itemDescriptionOn = true;

                    if (sCurrentAnimatingSlot != slot) {
                        // Animating a new slot so start the animation over
                        sCurrentAnimatingSlot = slot;
                        sCycleActiveAnimTimer = 0;
                    }
                } else if (availableItems.size() > 1) { // Short circuit to the only other item
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    direction = CYCLE_RIGHT;
                }
            }
        }

        if (sCurrentItemCyclingSlot != SLOT_NONE) {
            // Update HUD A button
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_STOP) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_STOP);
            }
            if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_ENABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }

            // Cancel cycling
            if (CHECK_BTN_ANY(CONTROLLER1(&gPlayState->state)->press.button, BTN_B | BTN_START)) {
                Audio_PlaySfx(NA_SE_SY_CANCEL);
                sCurrentItemCyclingSlot = SLOT_NONE;
                pauseCtx->itemDescriptionOn = false;
            }

            // Check for cycle switch
            if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_DLEFT) || pauseCtx->stickAdjX < -30) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                direction = CYCLE_LEFT;
            }
            if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_DRIGHT) || pauseCtx->stickAdjX > 30) {
                Audio_PlaySfx(NA_SE_SY_CURSOR);
                direction = CYCLE_RIGHT;
            }
        } else if (itemId == PAUSE_ITEM_NONE || availableItems.size() > 1) {
            // Update HUD A button
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_DECIDE);
            }
            if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_ENABLED) {
                gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
                gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
                Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
            }
        } else {
            // Nothing to cycle, switch back to Info on A button
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
            }
        }

        // Change the item
        if (direction != CYCLE_NONE) {
            // Get new item an update Kaleido info panel text
            u8 newItem = direction == CYCLE_LEFT ? GetPreviousListEntry(availableItems, itemId)
                                                 : GetNextListEntry(availableItems, itemId);
            pauseCtx->cursorItem[PAUSE_ITEM] = newItem;

            // Update item slot itself
            if (itemId == PAUSE_ITEM_NONE) {
                INV_CONTENT(newItem) = newItem;
            } else {
                Inventory_ReplaceItem(gPlayState, (u8)itemId, newItem);
            }
        }

        sPrevKaleidoCursorSlot = slot;
    });

    COND_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, IS_RANDO, {
        PauseContext* pauseCtx = &gPlayState->pauseCtx;
        u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];

        if (slot != SLOT_TRADE_COUPLE && slot != SLOT_TRADE_DEED && slot != SLOT_TRADE_KEY_MAMA) {
            return;
        }

        *should = false;
    });

    COND_ID_HOOK(AfterKaleidoDrawPage, PAUSE_ITEM, IS_RANDO, [](PauseContext* pauseCtx, u16 pauseIndex) {
        std::vector<u8> availableDeedItems = BuildAvailableItemsList(SLOT_TRADE_DEED);
        std::vector<u8> availableKeyMamaItems = BuildAvailableItemsList(SLOT_TRADE_KEY_MAMA);
        std::vector<u8> availableCoupleItems = BuildAvailableItemsList(SLOT_TRADE_COUPLE);

        u8 currentDeedItem = gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED];
        u8 currentKeyMamaItem = gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA];
        u8 currentCoupleItem = gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE];

        DrawItemCycleExtras(gPlayState, SLOT_TRADE_DEED, true,
                            GetPreviousListEntry(availableDeedItems, currentDeedItem),
                            GetNextListEntry(availableDeedItems, currentDeedItem));
        DrawItemCycleExtras(gPlayState, SLOT_TRADE_KEY_MAMA, true,
                            GetPreviousListEntry(availableKeyMamaItems, currentKeyMamaItem),
                            GetNextListEntry(availableKeyMamaItems, currentKeyMamaItem));
        DrawItemCycleExtras(gPlayState, SLOT_TRADE_COUPLE, true,
                            GetPreviousListEntry(availableCoupleItems, currentCoupleItem),
                            GetNextListEntry(availableCoupleItems, currentCoupleItem));
    });
}
