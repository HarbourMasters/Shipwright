#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/HudEditor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ObjectExtension/ObjectExtension.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "archives/icon_item_static/icon_item_static_yar.h"
#include "assets/objects/object_gi_bomb_1/object_gi_bomb_1.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
Gfx* ResourceMgr_LoadGfxByName(const char* path);
extern s16 sEquipState;
extern s16 sEquipAnimTimer;
extern const char* gAmmoDigitTextures[10];
}

#define CVAR_NAME "gEnhancements.Equipment.BombArrows"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define BOMB_ARROW_FLAG (1 << 0)
#define BOMB_ARROW_IS_SET(arrow) (((arrow)->actor.home.rot.x & BOMB_ARROW_FLAG) != 0)
#define BOMB_ARROW_SET(arrow) ((arrow)->actor.home.rot.x |= BOMB_ARROW_FLAG)

struct BombArrowData {
    std::vector<Gfx> clonedGfx;
    s32 primIdx = -1;
    s32 envIdx = -1;
    f32 flashBrightness = 0.0f;
    s16 timer = 70;
    s16 flashSpeedScale = 7;
    bool bombConsumed = false;
};

static ObjectExtension::Register<BombArrowData> BombArrowDataRegister;

bool sBombSlotIsBombArrowMode = false;

static bool sInventorySwapActive = false;
static int sBombPrevKaleidoCursorSlot = SLOT_NONE;

struct PendingBombArrowEquip {
    s32 targetBtn;
    bool isDpad;
    s32 newEquipSlot;
    ItemId newItem;
    bool isBowGroup;
    bool isBombArrow;
};

static PendingBombArrowEquip sPendingEquip = {};
static bool sHasPendingEquip = false;

// 14x14 units (50% of 28-unit kaleido grid quad), matching HUD overlay ratio.
static Vtx sBombOverlayVtx[] = {
    VTX(-7, 7, 0, 0 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(7, 7, 0, 32 << 5, 0 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-7, -7, 0, 0 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(7, -7, 0, 32 << 5, 32 << 5, 0xFF, 0xFF, 0xFF, 0xFF),
};

static const s16 sBCButtonXPositions[] = {
    167, // EQUIP_SLOT_B
    227, // EQUIP_SLOT_C_LEFT
    249, // EQUIP_SLOT_C_DOWN
    271, // EQUIP_SLOT_C_RIGHT
};

static const s16 sBCButtonYPositions[] = {
    17, // EQUIP_SLOT_B
    18, // EQUIP_SLOT_C_LEFT
    34, // EQUIP_SLOT_C_DOWN
    18, // EQUIP_SLOT_C_RIGHT
};

static const s16 sItemIconTextureDimensions[] = {
    30, // EQUIP_SLOT_B
    24, // EQUIP_SLOT_C_LEFT
    24, // EQUIP_SLOT_C_DOWN
    24, // EQUIP_SLOT_C_RIGHT
};

static const s16 sDpadItemIconLeft[] = {
    295, // EQUIP_SLOT_D_RIGHT
    263, // EQUIP_SLOT_D_LEFT
    279, // EQUIP_SLOT_D_DOWN
    279, // EQUIP_SLOT_D_UP
};

static const s16 sDpadItemIconTop[] = {
    63, // EQUIP_SLOT_D_RIGHT
    63, // EQUIP_SLOT_D_LEFT
    79, // EQUIP_SLOT_D_DOWN
    47, // EQUIP_SLOT_D_UP
};

static bool IsBowItem(ItemId item) {
    return (item == ITEM_BOW) || ((item >= ITEM_BOW_FIRE) && (item <= ITEM_BOW_LIGHT));
}

static bool ApplyHudEditorAdjustments(HudEditorElementID element, s16* rectLeft, s16* rectTop, s16* rectWidth,
                                      s16* rectHeight, s16* dsdx, s16* dtdy) {
    HudEditor_SetActiveElement(element);
    if (HudEditor_ShouldOverrideDraw()) {
        if (HudEditor_IsActiveElementHidden()) {
            return false;
        }
        HudEditor_ModifyDrawValues(rectLeft, rectTop, rectWidth, rectHeight, dsdx, dtdy);
    }
    return true;
}

static bool ResolvePauseEquipTarget(const PauseContext* pauseCtx, s32* targetSlot, bool* isDpad) {
    switch (pauseCtx->equipTargetCBtn) {
        case PAUSE_EQUIP_C_LEFT:
            *targetSlot = EQUIP_SLOT_C_LEFT;
            *isDpad = false;
            return true;
        case PAUSE_EQUIP_C_DOWN:
            *targetSlot = EQUIP_SLOT_C_DOWN;
            *isDpad = false;
            return true;
        case PAUSE_EQUIP_C_RIGHT:
            *targetSlot = EQUIP_SLOT_C_RIGHT;
            *isDpad = false;
            return true;
        case PAUSE_EQUIP_D_RIGHT:
            *targetSlot = EQUIP_SLOT_D_RIGHT;
            *isDpad = true;
            return true;
        case PAUSE_EQUIP_D_LEFT:
            *targetSlot = EQUIP_SLOT_D_LEFT;
            *isDpad = true;
            return true;
        case PAUSE_EQUIP_D_DOWN:
            *targetSlot = EQUIP_SLOT_D_DOWN;
            *isDpad = true;
            return true;
        case PAUSE_EQUIP_D_UP:
            *targetSlot = EQUIP_SLOT_D_UP;
            *isDpad = true;
            return true;
        default:
            return false;
    }
}

static bool CanCycleBombSlot() {
    return INV_CONTENT(ITEM_BOW) == ITEM_BOW && INV_CONTENT(ITEM_BOMB) == ITEM_BOMB;
}

bool IsBombArrowButton(s32 slot, bool isDpad) {
    if (slot < 0 || slot >= 8)
        return false;
    s32 effectiveSlot = isDpad ? (slot + 4) : slot;
    return (gSaveContext.save.shipSaveInfo.bombArrowsEquipped & (1 << effectiveSlot)) != 0;
}

void SetBombArrowButton(s32 slot, bool state, bool isDpad) {
    if (slot < 0 || slot >= 8)
        return;
    s32 effectiveSlot = isDpad ? (slot + 4) : slot;
    if (state) {
        gSaveContext.save.shipSaveInfo.bombArrowsEquipped |= (1 << effectiveSlot);
    } else {
        gSaveContext.save.shipSaveInfo.bombArrowsEquipped &= ~(1 << effectiveSlot);
    }
}

static void GetSlotItem(s32 slot, bool isDpad, ItemId* item, s32* equipSlot) {
    if (isDpad) {
        if (item)
            *item = (ItemId)DPAD_BUTTON_ITEM_EQUIP(0, slot);
        if (equipSlot)
            *equipSlot = DPAD_SLOT_EQUIP(0, slot);
    } else {
        if (item)
            *item = (ItemId)BUTTON_ITEM_EQUIP(0, slot);
        if (equipSlot)
            *equipSlot = C_SLOT_EQUIP(0, slot);
    }
}

static void SetSlotItem(s32 slot, bool isDpad, ItemId item, s32 equipSlot) {
    if (isDpad) {
        DPAD_BUTTON_ITEM_EQUIP(0, slot) = item;
        DPAD_SLOT_EQUIP(0, slot) = equipSlot;
        Interface_Dpad_LoadItemIcon(gPlayState, slot);
    } else {
        BUTTON_ITEM_EQUIP(0, slot) = item;
        C_SLOT_EQUIP(0, slot) = equipSlot;
        Interface_LoadItemIcon(gPlayState, slot);
    }
}

static bool FindSlotWithItem(s32 itemEquipSlot, s32* outSlot, bool* outIsDpad, s32 ignoreSlot = -1,
                             bool ignoreIsDpad = false) {
    // C-buttons
    for (int i = EQUIP_SLOT_C_LEFT; i <= EQUIP_SLOT_C_RIGHT; i++) {
        if (!ignoreIsDpad && i == ignoreSlot)
            continue;

        if (C_SLOT_EQUIP(0, i) == itemEquipSlot) {
            *outSlot = i;
            *outIsDpad = false;
            return true;
        }
    }

    // D-pad
    for (int i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++) {
        if (ignoreIsDpad && i == ignoreSlot)
            continue;

        if (DPAD_SLOT_EQUIP(0, i) == itemEquipSlot) {
            *outSlot = i;
            *outIsDpad = true;
            return true;
        }
    }

    return false;
}

static bool FindEquippedBowType(s32* outSlot, bool* outIsDpad, s32 ignoreSlot = -1, bool ignoreIsDpad = false) {
    for (int i = 0; i < 7; i++) {
        bool isDpad = (i >= 3);
        s32 slotIndex = isDpad ? (i - 3) : (i + 1);

        if (isDpad == ignoreIsDpad && slotIndex == ignoreSlot)
            continue;

        s32 equippedSlot = isDpad ? DPAD_GET_CUR_FORM_BTN_SLOT(slotIndex) : GET_CUR_FORM_BTN_SLOT(slotIndex);
        if (equippedSlot == SLOT_BOW || (equippedSlot == SLOT_BOMB && IsBombArrowButton(slotIndex, isDpad))) {
            *outSlot = slotIndex;
            *outIsDpad = isDpad;
            return true;
        }
    }
    return false;
}

static void HandleSmartSwap(s32 targetBtn, bool targetIsDpad, s32 newEquipSlot, bool isBowType) {
    s32 dupBtn;
    bool dupIsDpad;
    bool found = false;

    if (isBowType) {
        found = FindEquippedBowType(&dupBtn, &dupIsDpad, targetBtn, targetIsDpad);
    } else {
        found = FindSlotWithItem(newEquipSlot, &dupBtn, &dupIsDpad, targetBtn, targetIsDpad);
    }

    if (found) {
        s32 dupSlot = dupIsDpad ? DPAD_GET_CUR_FORM_BTN_SLOT(dupBtn) : GET_CUR_FORM_BTN_SLOT(dupBtn);
        bool isSameItemSlot = (dupSlot == newEquipSlot);

        if (isSameItemSlot) {
            ItemId targetItem;
            s32 targetEquipSlot;
            GetSlotItem(targetBtn, targetIsDpad, &targetItem, &targetEquipSlot);
            bool targetWasBombArrow = IsBombArrowButton(targetBtn, targetIsDpad);

            SetSlotItem(dupBtn, dupIsDpad, targetItem, targetEquipSlot);
            SetBombArrowButton(dupBtn, targetWasBombArrow, dupIsDpad);
        } else {
            // Exclusivity clear within the same group (e.g., Bow vs Magic Arrow).
            SetSlotItem(dupBtn, dupIsDpad, ITEM_NONE, SLOT_NONE);
            SetBombArrowButton(dupBtn, false, dupIsDpad);
        }
    }
}

static void InitiateEquipAnimation(PauseContext* pauseCtx, ItemId item, s32 slot) {
    pauseCtx->equipTargetItem = item;
    pauseCtx->equipTargetSlot = slot;

    pauseCtx->mainState = PAUSE_MAIN_STATE_EQUIP_ITEM;
    u16 vtxIndex = pauseCtx->cursorSlot[pauseCtx->pageIndex] * 4;
    pauseCtx->equipAnimX = pauseCtx->itemVtx[vtxIndex].v.ob[0] * 10;
    pauseCtx->equipAnimY = pauseCtx->itemVtx[vtxIndex].v.ob[1] * 10;
    pauseCtx->equipAnimAlpha = 255;
    sEquipState = EQUIP_STATE_MOVE_TO_C_BTN;
    sEquipAnimTimer = 10;
}

static void HandleBombArrowEquip(u16 cursorItem, bool* should, PauseContext* pauseCtx) {
    s32 targetBtn = -1;
    bool isDpad = false;

    if (!ResolvePauseEquipTarget(pauseCtx, &targetBtn, &isDpad)) {
        return;
    }

    ItemId equippedItem;
    GetSlotItem(targetBtn, isDpad, &equippedItem, nullptr);
    bool equippedIsBombArrow = IsBombArrowButton(targetBtn, isDpad);

    if (equippedItem == cursorItem || (equippedItem == ITEM_BOW && cursorItem == ITEM_BOMB && equippedIsBombArrow)) {
        if (cursorItem == ITEM_BOW && equippedIsBombArrow) {
            *should = false;
            sPendingEquip = { targetBtn, isDpad, SLOT_BOW, ITEM_BOW, true, false };
            sHasPendingEquip = true;
            InitiateEquipAnimation(pauseCtx, ITEM_BOW, SLOT_BOW);
            Audio_PlaySfx(NA_SE_SY_DECIDE);
            return;
        } else if (cursorItem != ITEM_BOMB && equippedIsBombArrow) {
            *should = false;
            return;
        }
    }

    // Magic arrow unequip logic
    if (IsBowItem(equippedItem)) {
        if (cursorItem == ITEM_ARROW_FIRE && equippedItem == ITEM_BOW_FIRE)
            return;
        if (cursorItem == ITEM_ARROW_ICE && equippedItem == ITEM_BOW_ICE)
            return;
        if (cursorItem == ITEM_ARROW_LIGHT && equippedItem == ITEM_BOW_LIGHT)
            return;
        if (cursorItem == ITEM_BOW &&
            (equippedItem == ITEM_BOW_FIRE || equippedItem == ITEM_BOW_ICE || equippedItem == ITEM_BOW_LIGHT))
            return;
    }

    // Equipping Magic Arrow, Bomb Arrow, or regular Bow
    bool isMagicArrow = (cursorItem >= ITEM_ARROW_FIRE && cursorItem <= ITEM_ARROW_LIGHT);
    bool isBombArrow = (cursorItem == ITEM_BOMB && sBombSlotIsBombArrowMode);
    bool isRegularBow = (cursorItem == ITEM_BOW);

    if (isMagicArrow || isBombArrow || isRegularBow) {
        *should = false;

        ItemId finalAnimItem = ITEM_BOW;
        s32 finalEquipSlot = SLOT_BOW;

        if (isMagicArrow) {
            // Use vanilla magic arrow "item action" IDs for the animation sequence.
            finalAnimItem = (ItemId)(0xB5 + (cursorItem - ITEM_ARROW_FIRE));
        }

        bool isBowGroup = true;

        // Defer swap/state changes until the equip animation reaches the button.
        sPendingEquip = { targetBtn, isDpad, finalEquipSlot, finalAnimItem, isBowGroup, isBombArrow };
        sHasPendingEquip = true;

        InitiateEquipAnimation(pauseCtx, finalAnimItem, finalEquipSlot);

        if (isMagicArrow) {
            // Magic arrows have a special 2-part animation.
            pauseCtx->equipAnimAlpha = sEquipState = 0; // EQUIP_STATE_MAGIC_ARROW_GROW_ORB
            sEquipAnimTimer = 6;
            Audio_PlaySfx(NA_SE_SY_SET_FIRE_ARROW + (cursorItem - ITEM_ARROW_FIRE));
        } else {
            Audio_PlaySfx(NA_SE_SY_DECIDE);
        }
        return;
    }

    // Equipping bombs normally
    if (cursorItem == ITEM_BOMB) {
        *should = false;

        sPendingEquip = { targetBtn, isDpad, SLOT_BOMB, ITEM_BOMB, false, false };
        sHasPendingEquip = true;
        InitiateEquipAnimation(pauseCtx, ITEM_BOMB, SLOT_BOMB);
        Audio_PlaySfx(NA_SE_SY_DECIDE);
        return;
    }
}

static void DrawBombArrowCycleExtras(PlayState* play, bool canCycle, ItemId alternateItem) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u8 slot = SLOT_BOMB;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL42_Opa(play->state.gfxCtx);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (canCycle && alternateItem != ITEM_NONE) {
        Matrix_Push();

        Vtx* itemTopLeft = &pauseCtx->itemVtx[slot * 4];
        Vtx* itemBottomRight = &itemTopLeft[3];

        s16 halfX = (itemBottomRight->v.ob[0] - itemTopLeft->v.ob[0]) / 2;
        s16 halfY = (itemBottomRight->v.ob[1] - itemTopLeft->v.ob[1]) / 2;

        Matrix_Translate(itemTopLeft->v.ob[0] + halfX, itemTopLeft->v.ob[1] + halfY, 0, MTXMODE_APPLY);

        // Rest position: 32.5% scale, 15 units below the main icon.
        Matrix_Translate(0, -15.0f, 0, MTXMODE_APPLY);
        Matrix_Scale(0.325f, 0.325f, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        // Show A-button prompt when hovered.
        if (pauseCtx->cursorSlot[PAUSE_ITEM] == slot && pauseCtx->cursorSpecialPos == 0) {
            Ship_DrawKaleidoCycleAButtonPrompt(play, pauseCtx->alpha);
        }

        // Draw the right alternate item.
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        gSPVertex(POLY_OPA_DISP++, (uintptr_t)sCycleExtraItemVtx, 8, 0);
        KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[alternateItem], 32, 32, 4);

        // If the alternate is the bow (bomb arrows), overlay a bomb icon on the preview.
        if (alternateItem == ITEM_BOW) {
            Matrix_Push();
            Matrix_Translate(32.0f + 3.0f, 0.0f + 3.0f, 0, MTXMODE_APPLY);
            Matrix_Scale(16.0f / 14.0f, 16.0f / 14.0f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPVertex(POLY_OPA_DISP++, (uintptr_t)sBombOverlayVtx, 4, 0);
            KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[ITEM_BOMB], 32, 32, 0);
            Matrix_Pop();
        }

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static void DrawBombArrowSlotIcon(PlayState* play) {
    PauseContext* pauseCtx = &play->pauseCtx;
    u8 slot = SLOT_BOMB;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL42_Opa(play->state.gfxCtx);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    Matrix_Push();

    Vtx* itemTopLeft = &pauseCtx->itemVtx[slot * 4];
    Vtx* itemBottomRight = &itemTopLeft[3];

    s16 centerX = itemTopLeft->v.ob[0] + (itemBottomRight->v.ob[0] - itemTopLeft->v.ob[0]) / 2;
    s16 centerY = itemTopLeft->v.ob[1] + (itemBottomRight->v.ob[1] - itemTopLeft->v.ob[1]) / 2;
    Matrix_Translate(centerX + 2, centerY + 2, 1, MTXMODE_APPLY);

    // Scaling the Bomb Arrow icon when it is being hovered
    if (pauseCtx->cursorSlot[PAUSE_ITEM] == slot && pauseCtx->pageIndex == PAUSE_ITEM &&
        pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE && pauseCtx->cursorSpecialPos == 0) {
        Matrix_Scale(1.1f, 1.1f, 1.0f, MTXMODE_APPLY);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gSPVertex(POLY_OPA_DISP++, (uintptr_t)sBombOverlayVtx, 4, 0);
    KaleidoScope_DrawTexQuadRGBA32(play->state.gfxCtx, gItemIcons[ITEM_BOMB], 32, 32, 0);

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

static void HandleSlotCycling(PauseContext* pauseCtx) {
    InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;

    if (pauseCtx->state != PAUSE_STATE_MAIN) {
        sBombPrevKaleidoCursorSlot = SLOT_NONE;
        return;
    }

    u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];

    if ((pauseCtx->debugEditor != DEBUG_EDITOR_NONE) || (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE) ||
        slot != SLOT_BOMB || !CanCycleBombSlot()) {
        if (sBombPrevKaleidoCursorSlot == SLOT_BOMB) {
            // Restore A-button action when leaving the bomb slot.
            if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
                Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
            }
        }

        sBombPrevKaleidoCursorSlot = slot;
        return;
    }

    // Cursor is on SLOT_BOMB and cycling is available; toggle on A press.
    if (CHECK_BTN_ALL(CONTROLLER1(&gPlayState->state)->press.button, BTN_A)) {
        Audio_PlaySfx(NA_SE_SY_DECIDE);
        sBombSlotIsBombArrowMode = !sBombSlotIsBombArrowMode;
    }

    if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
        Interface_SetAButtonDoAction(gPlayState, DO_ACTION_DECIDE);
    }
    if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_ENABLED) {
        gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    }

    sBombPrevKaleidoCursorSlot = slot;
}

static void HandleEquipCleanup(PauseContext* pauseCtx) {
    // Only run when the pause menu is idle.
    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE)) {
        for (int i = EQUIP_SLOT_C_LEFT; i <= EQUIP_SLOT_C_RIGHT; i++) {
            ItemId item = ITEM_NONE;
            GetSlotItem(i, false, &item, nullptr);
            if (!IsBowItem(item) && !(item == ITEM_BOMB && IsBombArrowButton(i, false)))
                SetBombArrowButton(i, false, false);
        }
        for (int i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++) {
            ItemId item = ITEM_NONE;
            GetSlotItem(i, true, &item, nullptr);
            if (!IsBowItem(item) && !(item == ITEM_BOMB && IsBombArrowButton(i, true)))
                SetBombArrowButton(i, false, true);
        }
    }
}

static void DrawBombArrowAmmoCount(PlayState* play, s16 button, s16 alpha, bool isDpad, bool has_bombs) {
    if (!IsBombArrowButton(button, isDpad)) {
        return;
    }

    s16 arrows = AMMO(ITEM_BOW);
    s16 bombs = AMMO(ITEM_BOMB);
    s16 maxArrows = CUR_CAPACITY(UPG_QUIVER);
    s16 maxBombs = CUR_CAPACITY(UPG_BOMB_BAG);

    s16 effectiveAmmo = (arrows < bombs) ? arrows : bombs;
    s16 effectiveMax = (maxArrows < maxBombs) ? maxArrows : maxBombs;
    s16 ammo;
    bool turnGreen;

    // When out of bombs displays arrow count instead
    if (has_bombs) {
        ammo = effectiveAmmo;
        turnGreen = (effectiveAmmo == effectiveMax);
    } else {
        ammo = arrows;
        turnGreen = (arrows == maxArrows);
    }

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    GameInteractor_Should(VB_SET_BUTTON_ENV_COLOR, false);

    if (ammo == 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, alpha);
    } else if (turnGreen) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, alpha);
    }

    s16 tens = 0;
    for (tens = 0; ammo >= 10; tens++) {
        ammo -= 10;
    }

    static s16 sAmmoDigitsXPositions[] = { 162, 228, 250, 272 };
    static s16 sAmmoDigitsYPositions[] = { 35, 35, 51, 35 };

    static s16 sDpadItemAmmoX[] = {
        295, // EQUIP_SLOT_D_RIGHT
        263, // EQUIP_SLOT_D_LEFT
        279, // EQUIP_SLOT_D_DOWN
        279, // EQUIP_SLOT_D_UP
    };

    static s16 sDpadItemAmmoY[] = {
        63 + 11, // EQUIP_SLOT_D_RIGHT
        63 + 11, // EQUIP_SLOT_D_LEFT
        79 + 11, // EQUIP_SLOT_D_DOWN
        47 + 11, // EQUIP_SLOT_D_UP
    };

    s16 x = isDpad ? sDpadItemAmmoX[button] : sAmmoDigitsXPositions[button];
    s16 y = isDpad ? sDpadItemAmmoY[button] : sAmmoDigitsYPositions[button];
    HudEditorElementID elementId = isDpad ? HUD_EDITOR_ELEMENT_D_PAD : (HudEditorElementID)button;

    if (tens != 0) {
        HudEditor_SetActiveElement(elementId);
        OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gAmmoDigitTextures[tens], AMMO_DIGIT_TEX_WIDTH,
                                          AMMO_DIGIT_TEX_HEIGHT, x, y, AMMO_DIGIT_TEX_WIDTH, AMMO_DIGIT_TEX_HEIGHT,
                                          1 << 10, 1 << 10);
    }

    HudEditor_SetActiveElement(elementId);
    OVERLAY_DISP = Gfx_DrawTexRectIA8(OVERLAY_DISP, (TexturePtr)gAmmoDigitTextures[ammo], AMMO_DIGIT_TEX_WIDTH,
                                      AMMO_DIGIT_TEX_HEIGHT, x + 6, y, AMMO_DIGIT_TEX_WIDTH, AMMO_DIGIT_TEX_HEIGHT,
                                      1 << 10, 1 << 10);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void DrawBombArrowOverlayCButton(PlayState* play, EquipSlot slot, s16 alpha) {
    s16 rectLeft = sBCButtonXPositions[slot];
    s16 rectTop = sBCButtonYPositions[slot];
    s16 baseSize = sItemIconTextureDimensions[slot];
    s16 overlaySize = 12;
    s16 rectWidth = overlaySize;
    s16 rectHeight = overlaySize;
    s16 overlayLeft = rectLeft + (baseSize - overlaySize) / 2 + 2;
    s16 overlayTop = rectTop + (baseSize - overlaySize) / 2 - 2;
    s16 dsdx = ((ICON_ITEM_TEX_WIDTH << 10) / rectWidth) >> 1;
    s16 dtdy = ((ICON_ITEM_TEX_HEIGHT << 10) / rectHeight) >> 1;

    if (!ApplyHudEditorAdjustments((HudEditorElementID)slot, &overlayLeft, &overlayTop, &rectWidth, &rectHeight, &dsdx,
                                   &dtdy)) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL39_Overlay(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPLoadTextureBlock(OVERLAY_DISP++, gItemIcons[ITEM_BOMB], G_IM_FMT_RGBA, G_IM_SIZ_32b, ICON_ITEM_TEX_WIDTH,
                        ICON_ITEM_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(OVERLAY_DISP++, overlayLeft << 2, overlayTop << 2, (overlayLeft + rectWidth) << 2,
                        (overlayTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx << 1, dtdy << 1);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void DrawBombArrowOverlayDpad(PlayState* play, DpadEquipSlot slot, s16 alpha) {
    s16 rectLeft = sDpadItemIconLeft[slot];
    s16 rectTop = sDpadItemIconTop[slot];
    s16 baseSize = 16;
    s16 overlaySize = 8;
    s16 rectWidth = overlaySize;
    s16 rectHeight = overlaySize;
    s16 overlayLeft = rectLeft + (baseSize - overlaySize) / 2 + 2;
    s16 overlayTop = rectTop + (baseSize - overlaySize) / 2 - 2;
    s16 dsdx = ((ICON_ITEM_TEX_WIDTH << 10) / rectWidth) >> 1;
    s16 dtdy = ((ICON_ITEM_TEX_HEIGHT << 10) / rectHeight) >> 1;

    if (!ApplyHudEditorAdjustments(HUD_EDITOR_ELEMENT_D_PAD, &overlayLeft, &overlayTop, &rectWidth, &rectHeight, &dsdx,
                                   &dtdy)) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL39_Overlay(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    gDPLoadTextureBlock(OVERLAY_DISP++, gItemIcons[ITEM_BOMB], G_IM_FMT_RGBA, G_IM_SIZ_32b, ICON_ITEM_TEX_WIDTH,
                        ICON_ITEM_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(OVERLAY_DISP++, overlayLeft << 2, overlayTop << 2, (overlayLeft + rectWidth) << 2,
                        (overlayTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx << 1, dtdy << 1);

    CLOSE_DISPS(play->state.gfxCtx);
}

static void TryConsumeBombAmmo(BombArrowData* data) {
    if (data == nullptr || data->bombConsumed || (AMMO(ITEM_BOMB) <= 0)) {
        return;
    }

    Inventory_ChangeAmmo(ITEM_BOMB, -1);
    data->bombConsumed = true;
}

// Mirrors func_80831194 ammo consumption; do not call func_80831194 here (detaches the actor).
static void TryConsumeArrowOnHeldExpiry(PlayState* play, Player* player) {
    if (player == nullptr || player->transformation == PLAYER_FORM_DEKU || (player->stateFlags3 & PLAYER_STATE3_400)) {
        return;
    }

    if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE) {
        if ((play->sceneId != SCENE_SYATEKI_MIZU) && (play->sceneId != SCENE_F01) &&
            (play->sceneId != SCENE_SYATEKI_MORI)) {
            play->interfaceCtx.minigameAmmo--;
        }
    } else if (play->bButtonAmmoPlusOne != 0) {
        play->bButtonAmmoPlusOne--;
    } else {
        Inventory_ChangeAmmo(ITEM_BOW, -1);
    }

    if (play->bButtonAmmoPlusOne == 1) {
        play->bButtonAmmoPlusOne = -10;
    }
}

static void BombArrowDetonate(EnArrow* arrow, BombArrowData* data) {
    if (!data->bombConsumed) {
        if (arrow->actor.child != NULL) {
            Actor_Kill(arrow->actor.child);
        }
        Actor_Kill(&arrow->actor);
        return;
    }

    EnBom* bomb = (EnBom*)Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_BOM, arrow->actor.world.pos.x,
                                      arrow->actor.world.pos.y, arrow->actor.world.pos.z, 0, 0, 0, BOMB_TYPE_BODY);
    if (bomb != NULL) {
        bomb->timer = 0;
    }

    if (arrow->actor.child != NULL) {
        Actor_Kill(arrow->actor.child);
    }

    Actor_Kill(&arrow->actor);
}

static void OnPlayerReleaseHeldActor(PlayState* play, Player* player, Actor* heldActor) {
    if (heldActor == nullptr || heldActor->id != ACTOR_EN_ARROW) {
        return;
    }

    EnArrow* arrow = (EnArrow*)heldActor;

    if (!BOMB_ARROW_IS_SET(arrow) || !ARROW_IS_ARROW(arrow->actor.params)) {
        return;
    }

    auto data = ObjectExtension::GetInstance().Get<BombArrowData>(heldActor);
    if (data != nullptr) {
        TryConsumeBombAmmo(data);
    }
}

static void UpdateBombArrowFlash(PlayState* play, BombArrowData* data) {
    if (data == nullptr) {
        return;
    }

    bool blink = (data->timer < 120 && (data->timer & (data->flashSpeedScale + 1)));

    if (blink) {
        Math_ApproachF(&data->flashBrightness, 140.0f, 1.0f, 140.0f / data->flashSpeedScale);
    } else {
        Math_ApproachZeroF(&data->flashBrightness, 1.0f, 140.0f / data->flashSpeedScale);
    }

    u8 colorIdx = (u8)data->flashBrightness;

    if (data->primIdx >= 0) {
        data->clonedGfx[data->primIdx] = gsDPSetPrimColor(0, 0, colorIdx, 0, 40, 255);
    }
    if (data->envIdx >= 0) {
        data->clonedGfx[data->envIdx] = gsDPSetEnvColor(colorIdx, 0, 40, 255);
    }
}

static void OnBombArrowInit(Actor* actor) {
    if (gPlayState == nullptr) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);

    if (player == nullptr || actor->parent != &player->actor) {
        return;
    }

    if (player->heldItemButton < 0 || (AMMO(ITEM_BOMB) <= 0)) {
        return;
    }

    if (IS_HELD_DPAD(player->heldItemButton)) {
        s32 dpadSlot = HELD_ITEM_TO_DPAD(player->heldItemButton);
        if (!IsBombArrowButton(dpadSlot, true)) {
            return;
        }
    } else {
        if (player->heldItemButton == EQUIP_SLOT_B || !IsBombArrowButton(player->heldItemButton, false)) {
            return;
        }
    }

    EnArrow* arrow = (EnArrow*)actor;

    if (!ARROW_IS_ARROW(arrow->actor.params)) {
        return;
    }

    BOMB_ARROW_SET(arrow);

    BombArrowData data;

    Gfx* originalDl = ResourceMgr_LoadGfxByName(gGiBombDL);
    if (originalDl != nullptr) {
        for (s32 i = 0; i < 256; i++) {
            data.clonedGfx.push_back(originalDl[i]);
            if ((originalDl[i].words.w0 >> 24) == G_ENDDL) {
                break;
            }

            u8 opcode = (u8)(originalDl[i].words.w0 >> 24);
            u32 rgba = originalDl[i].words.w1;
            u8 r = (rgba >> 24) & 0xFF;
            u8 g = (rgba >> 16) & 0xFF;

            if (opcode == G_SETPRIMCOLOR && data.primIdx < 0) {
                if (r == 0x00 && g == 0x3C) {
                    data.primIdx = i;
                }
            } else if (opcode == G_SETENVCOLOR && data.envIdx < 0) {
                if (r == 0x00 && g == 0x14) {
                    data.envIdx = i;
                }
            }
        }
    }

    ObjectExtension::GetInstance().Set<BombArrowData>(actor, std::move(data));
}

static void OnBombArrowUpdate(Actor* actor) {
    if (gPlayState == nullptr) {
        return;
    }

    EnArrow* arrow = (EnArrow*)actor;

    if (!BOMB_ARROW_IS_SET(arrow) || !ARROW_IS_ARROW(arrow->actor.params)) {
        return;
    }

    auto data = ObjectExtension::GetInstance().Get<BombArrowData>(actor);
    if (data == nullptr) {
        return;
    }

    if (data->timer > 0) {
        data->timer--;
    }

    if ((data->timer == 40) || (data->timer == 20) || (data->timer == 3)) {
        data->flashSpeedScale >>= 1;
    }

    if (data->timer <= 0) {
        if (actor->parent != NULL) {
            TryConsumeArrowOnHeldExpiry(gPlayState, GET_PLAYER(gPlayState));
        }
        TryConsumeBombAmmo(data);
        BombArrowDetonate(arrow, data);
        return;
    }

    Actor_PlaySfx(actor, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);

    Vec3f effectPos;
    Matrix_Push();
    Matrix_SetTranslateRotateYXZ(actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
                                 actor->world.pos.z, &actor->shape.rot);
    Matrix_Scale(actor->scale.x, actor->scale.y, actor->scale.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 1000.0f, MTXMODE_APPLY);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    Vec3f effectLocal = { 0.0f, 31.0f, 0.0f };
    Matrix_MultVec3f(&effectLocal, &effectPos);
    Matrix_Pop();

    if ((gPlayState->gameplayFrames % 2) == 0) {
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Color_RGBA8 prim = { 255, 255, 150, 255 };
        Color_RGBA8 env = { 255, 0, 0, 0 };
        EffectSsGSpk_SpawnNoAccel(gPlayState, actor, &effectPos, &velocity, &accel, &prim, &env, 30, 5);
    }

    {
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.2f, 0.0f };
        Color_RGBA8 color = { 255, 255, 255, 255 };
        func_800B0DE0(gPlayState, &effectPos, &velocity, &accel, &color, &color, 22, 3);
    }

    bool hit = (arrow->collider.base.atFlags & AT_HIT) || (arrow->unk_262 != 0);

    // Only explode on valid hits once the arrow is detached from the player.
    if (!hit || (actor->parent != NULL)) {
        return;
    }

    TryConsumeBombAmmo(data);
    BombArrowDetonate(arrow, data);
}

static void OnBombArrowDraw(Actor* actor) {
    if (gPlayState == nullptr) {
        return;
    }

    EnArrow* arrow = (EnArrow*)actor;
    if (!BOMB_ARROW_IS_SET(arrow) || !ARROW_IS_ARROW(arrow->actor.params)) {
        return;
    }

    auto data = ObjectExtension::GetInstance().Get<BombArrowData>(actor);
    if (data == nullptr) {
        return;
    }

    UpdateBombArrowFlash(gPlayState, data);

    OPEN_DISPS(gPlayState->state.gfxCtx);

    Gfx_SetupDL25_Opa(gPlayState->state.gfxCtx);

    Matrix_Push();

    Matrix_SetTranslateRotateYXZ(actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
                                 actor->world.pos.z, &actor->shape.rot);
    Matrix_Scale(actor->scale.x, actor->scale.y, actor->scale.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 1000.0f, MTXMODE_APPLY);
    Matrix_Scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, data->clonedGfx.data());

    Matrix_Pop();

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

static void OnKaleidoEquipItemToButton(bool* should, va_list args) {
    va_arg(args, int);
    u16 cursorItem = va_arg(args, int);

    if (*should) {
        HandleBombArrowEquip(cursorItem, should, &gPlayState->pauseCtx);
    }
}

static void OnDrawItemEquippedOutline(bool* should, va_list args) {
    ItemId* item = va_arg(args, ItemId*);
    s32 btn = va_arg(args, s32);
    s32 isDpad = va_arg(args, s32);
    s32 pageIndex = va_arg(args, s32);

    if (pageIndex != PAUSE_ITEM) {
        return;
    }

    s32 slot = isDpad ? DPAD_GET_CUR_FORM_BTN_SLOT(btn) : GET_CUR_FORM_BTN_SLOT(btn);
    // Don't show the border if the item is a mask (on the item page)
    if (slot >= ITEM_NUM_SLOTS) {
        return;
    }

    bool isBombArrow = IsBombArrowButton(btn, isDpad);
    bool shouldShow = false;

    if (isBombArrow) {
        shouldShow = sBombSlotIsBombArrowMode;
        if (shouldShow) {
            // Redirect highlight to SLOT_BOMB by copying slot vtx and applying the vanilla expansion.
            s32 bIndex = isDpad ? (27 + btn) : (24 + (btn - 1));
            Vtx* destVtx = &gPlayState->pauseCtx.itemVtx[bIndex * 4];
            Vtx* srcVtx = &gPlayState->pauseCtx.itemVtx[SLOT_BOMB * 4];

            memcpy(destVtx, srcVtx, sizeof(Vtx) * 4);

            destVtx[0].v.ob[0] = destVtx[2].v.ob[0] = srcVtx[0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_MARGIN;
            destVtx[1].v.ob[0] = destVtx[3].v.ob[0] = destVtx[0].v.ob[0] + ITEM_GRID_SELECTED_QUAD_WIDTH;
            destVtx[0].v.ob[1] = destVtx[1].v.ob[1] = srcVtx[0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_MARGIN;
            destVtx[2].v.ob[1] = destVtx[3].v.ob[1] = destVtx[0].v.ob[1] - ITEM_GRID_SELECTED_QUAD_WIDTH;

            for (int i = 0; i < 4; i++) {
                destVtx[i].v.cn[3] = gPlayState->pauseCtx.alpha;
            }
        }
    } else if (slot == SLOT_BOW) {
        shouldShow = true;
    } else if (slot == SLOT_BOMB) {
        shouldShow = !sBombSlotIsBombArrowMode;
    } else {
        shouldShow = true;
    }

    *should = shouldShow;
}

static void OnKaleidoDrawEquipAnimIcon(bool* should, va_list args) {
    ItemId* drawItem = va_arg(args, ItemId*);
    if (*drawItem != ITEM_BOW) {
        return;
    }

    PauseContext* pauseCtx = &gPlayState->pauseCtx;
    s32 targetBtn;
    bool isDpad;
    if (ResolvePauseEquipTarget(pauseCtx, &targetBtn, &isDpad) && IsBombArrowButton(targetBtn, isDpad)) {
        // Draw composite bomb-arrow icon during the equip animation.
        *should = false;

        OPEN_DISPS(gPlayState->state.gfxCtx);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, pauseCtx->equipAnimAlpha);
        gSPVertex(OVERLAY_DISP++, (uintptr_t)&pauseCtx->cursorVtx[16], 4, 0);
        gDPLoadTextureBlock(OVERLAY_DISP++, gItemIcons[ITEM_BOW], G_IM_FMT_RGBA, G_IM_SIZ_32b, ICON_ITEM_TEX_WIDTH,
                            ICON_ITEM_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

        f32 currentWidth = (f32)(pauseCtx->equipAnimScale / 10.0f);
        f32 scaleFactor = currentWidth / 32.0f;

        f32 centerX = pauseCtx->cursorVtx[16].v.ob[0] + (currentWidth / 2.0f);
        f32 centerY = pauseCtx->cursorVtx[16].v.ob[1] - (currentWidth / 2.0f);

        Matrix_Push();
        Matrix_Translate(centerX + (2.0f * scaleFactor), centerY + (2.0f * scaleFactor), 0.0f, MTXMODE_NEW);
        Matrix_Scale(scaleFactor, scaleFactor, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, gPlayState->state.gfxCtx);

        gSPVertex(OVERLAY_DISP++, (uintptr_t)sBombOverlayVtx, 4, 0);
        gDPLoadTextureBlock(OVERLAY_DISP++, gItemIcons[ITEM_BOMB], G_IM_FMT_RGBA, G_IM_SIZ_32b, ICON_ITEM_TEX_WIDTH,
                            ICON_ITEM_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

        Matrix_Pop();

        gSPMatrix(OVERLAY_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        CLOSE_DISPS(gPlayState->state.gfxCtx);
    }
}

static void OnDrawHudAmmoCount(bool* should, va_list args) {
    s16 button = (s16)va_arg(args, int);
    s16 alpha = (s16)va_arg(args, int);
    bool isDpad = (bool)va_arg(args, int);
    bool has_bombs = (AMMO(ITEM_BOMB) > 0);

    if (IsBombArrowButton(button, isDpad)) {
        *should = false;
        DrawBombArrowAmmoCount(gPlayState, button, alpha, isDpad, has_bombs);

        // When bombs is 0, reduce alpha for bomb texture.
        if (!has_bombs) {
            alpha /= 2;
        }

        // Draw the bomb overlay here so it shares the same render order as the C/D-button icons.
        if (isDpad) {
            DrawBombArrowOverlayDpad(gPlayState, (DpadEquipSlot)button, alpha);
        } else {
            DrawBombArrowOverlayCButton(gPlayState, (EquipSlot)button, alpha);
        }
    }
}

static void OnKaleidoDisplayItemText(bool* should, va_list args) {
    PauseContext* pauseCtx = &gPlayState->pauseCtx;
    u16 slot = pauseCtx->cursorSlot[PAUSE_ITEM];

    if (slot == SLOT_BOMB && CanCycleBombSlot()) {
        *should = false;
    }
}

static void OnBeforeKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    if (sBombSlotIsBombArrowMode && CanCycleBombSlot()) {
        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMB] = ITEM_BOW;
        sInventorySwapActive = true;
    }
}

static void OnAfterKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    if (sInventorySwapActive) {
        gSaveContext.save.saveInfo.inventory.items[SLOT_BOMB] = ITEM_BOMB;
        sInventorySwapActive = false;

        DrawBombArrowSlotIcon(gPlayState);
    }

    bool canCycle = CanCycleBombSlot();
    ItemId alternateItem = sBombSlotIsBombArrowMode ? ITEM_BOMB : ITEM_BOW;
    DrawBombArrowCycleExtras(gPlayState, canCycle, alternateItem);
}

static void OnKaleidoDrawAmmoCount(bool* should, va_list args) {
    PauseContext* pauseCtx = va_arg(args, PauseContext*);
    GraphicsContext* gfxCtx = va_arg(args, GraphicsContext*);
    s16 item = va_arg(args, int);
    u16 ammoIndex = va_arg(args, int);

    if (ammoIndex == 1 && sBombSlotIsBombArrowMode) {
        *should = false;

        s16 arrows = AMMO(ITEM_BOW);
        s16 bombs = AMMO(ITEM_BOMB);
        s16 maxArrows = CUR_CAPACITY(UPG_QUIVER);
        s16 maxBombs = CUR_CAPACITY(UPG_BOMB_BAG);

        s16 effectiveAmmo = (arrows < bombs) ? arrows : bombs;
        s16 effectiveMax = (maxArrows < maxBombs) ? maxArrows : maxBombs;

        s16 ammo = effectiveAmmo;
        bool turnGreen = (effectiveAmmo == effectiveMax);

        s16 ammoUpperDigit = 0;
        for (ammoUpperDigit = 0; ammo >= 10; ammoUpperDigit++) {
            ammo -= 10;
        }

        // Coordinates for SLOT_BOMB (ammoIndex=1) in vanilla kaleido item ammo rect.
        s16 sAmmoRectLeft = 62;
        s16 sAmmoRectHeight = 117;

        OPEN_DISPS(gfxCtx);

        gDPPipeSync(POLY_OPA_DISP++);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        if (effectiveAmmo == 0) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 130, 130, 130, pauseCtx->alpha);
        } else if (turnGreen) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 255, 0, pauseCtx->alpha);
        }

        if (ammoUpperDigit != 0) {
            POLY_OPA_DISP = Gfx_DrawTexRectIA8(POLY_OPA_DISP, (TexturePtr)gAmmoDigitTextures[ammoUpperDigit], 8, 8,
                                               sAmmoRectLeft, sAmmoRectHeight, 8, 8, 1 << 10, 1 << 10);
        }

        POLY_OPA_DISP = Gfx_DrawTexRectIA8(POLY_OPA_DISP, (TexturePtr)gAmmoDigitTextures[ammo], 8, 8, sAmmoRectLeft + 6,
                                           sAmmoRectHeight, 8, 8, 1 << 10, 1 << 10);

        CLOSE_DISPS(gfxCtx);
    }
}

static void OnKaleidoUpdate(PauseContext* pauseCtx) {
    HandleEquipCleanup(pauseCtx);
    HandleSlotCycling(pauseCtx);

    if (sHasPendingEquip) {
        if (((sEquipState == EQUIP_STATE_MOVE_TO_C_BTN) && (sEquipAnimTimer == 1)) ||
            (pauseCtx->state != PAUSE_STATE_MAIN)) {
            HandleSmartSwap(sPendingEquip.targetBtn, sPendingEquip.isDpad, sPendingEquip.newEquipSlot,
                            sPendingEquip.isBowGroup);
            SetSlotItem(sPendingEquip.targetBtn, sPendingEquip.isDpad, sPendingEquip.newItem,
                        sPendingEquip.newEquipSlot);
            SetBombArrowButton(sPendingEquip.targetBtn, sPendingEquip.isBombArrow, sPendingEquip.isDpad);
            sHasPendingEquip = false;
        }
    }
}

static void RegisterBombArrows() {
    COND_HOOK(OnKaleidoUpdate, CVAR, OnKaleidoUpdate);
    COND_HOOK(OnPlayerReleaseHeldActor, CVAR, OnPlayerReleaseHeldActor);

    COND_ID_HOOK(AfterKaleidoDrawPage, PAUSE_ITEM, CVAR, OnAfterKaleidoDrawPage);
    COND_ID_HOOK(BeforeKaleidoDrawPage, PAUSE_ITEM, CVAR, OnBeforeKaleidoDrawPage);
    COND_ID_HOOK(OnActorDraw, ACTOR_EN_ARROW, CVAR, OnBombArrowDraw);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ARROW, CVAR, OnBombArrowInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_ARROW, CVAR, OnBombArrowUpdate);

    COND_VB_SHOULD(VB_DRAW_HUD_AMMO_COUNT, CVAR, OnDrawHudAmmoCount(should, args));
    COND_VB_SHOULD(VB_DRAW_ITEM_EQUIPPED_OUTLINE, CVAR, OnDrawItemEquippedOutline(should, args));
    COND_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, CVAR, OnKaleidoDisplayItemText(should, args));
    COND_VB_SHOULD(VB_KALEIDO_DRAW_AMMO_COUNT, CVAR, OnKaleidoDrawAmmoCount(should, args));
    COND_VB_SHOULD(VB_KALEIDO_DRAW_EQUIP_ANIM_ICON, CVAR, OnKaleidoDrawEquipAnimIcon(should, args));
    COND_VB_SHOULD(VB_KALEIDO_EQUIP_ITEM_TO_BUTTON, CVAR, OnKaleidoEquipItemToButton(should, args));
}

static RegisterShipInitFunc initFunc(RegisterBombArrows, { CVAR_NAME });
