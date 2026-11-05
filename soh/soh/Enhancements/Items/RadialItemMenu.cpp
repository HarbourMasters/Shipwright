#include <libultraship/bridge/consolevariablebridge.h>
#include <cmath>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "soh/frame_interpolation.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "textures/parameter_static/parameter_static.h"
#include "overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
extern PlayState* gPlayState;
int16_t OTRGetRectDimensionFromLeftEdge(float v);
int16_t OTRGetRectDimensionFromRightEdge(float v);
}

// Hold (or tap) a button to bring up every inventory item on a wheel, point at one with the control stick and press a
// C-Button (or D-pad button with "Equip Items on Dpad") to assign it without going through the pause menu.

#define CVAR_RADIAL_MENU_NAME CVAR_ENHANCEMENT("RadialItemMenu.Enabled")
#define CVAR_RADIAL_MENU_DEFAULT 0
#define CVAR_RADIAL_MENU_VALUE CVarGetInteger(CVAR_RADIAL_MENU_NAME, CVAR_RADIAL_MENU_DEFAULT)
#define CVAR_RADIAL_MENU_BTN CVAR_ENHANCEMENT("RadialItemMenu.Btn")
#define CVAR_RADIAL_MENU_TOGGLE CVAR_ENHANCEMENT("RadialItemMenu.Toggle")
#define CVAR_RADIAL_MENU_FREEZE CVAR_ENHANCEMENT("RadialItemMenu.Freeze")

#define RADIAL_SLOT_COUNT 24
#define RADIAL_OPEN_FRAMES 4
#define RADIAL_FLASH_FRAMES 10
#define RADIAL_RADIUS 98.0f
#define RADIAL_SLOT_BG_SIZE_MIN 25.0f
#define RADIAL_SLOT_BG_SIZE_MAX 34.0f
#define RADIAL_STICK_DEADZONE 45
// How far (in radians) the stick has to move past the edge of the highlighted item's slice before the highlight moves
#define RADIAL_STICK_HYSTERESIS (M_PI / 36.0f)

static const u16 sEquipButtons[] = {
    BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT,
};

static const char* sEmptyCButtonTextures[] = {
    gEmptyCLeftArrowTex,
    gEmptyCDownArrowTex,
    gEmptyCRightArrowTex,
};

static bool sOpen = false;
static bool sFrozeGame = false;
static s32 sPrevFrameAdvance = false;
static s32 sCursorSlot = -1;
// Inventory slots shown on the wheel, in inventory order, spread evenly around it
static s32 sEntrySlots[RADIAL_SLOT_COUNT];
static s32 sEntryCount = 0;
static s32 sOpenTimer = 0;
static s32 sFlashTimer[3] = { 0 };
// Buttons held while the menu was open. They are hidden from the game until released so that closing the menu doesn't
// leak a held C-Button or the menu button into gameplay
static u32 sHeldMask = 0;

static bool SlotMeetsAgeReq(s32 slot) {
    return CHECK_AGE_REQ_SLOT(slot);
}

// Only items Link can equip right now are shown on the wheel
static bool SlotIsEquippable(s32 slot) {
    u8 item = gSaveContext.inventory.items[slot];
    return item != ITEM_NONE && item != ITEM_SOLD_OUT && SlotMeetsAgeReq(slot);
}

static void RefreshEntries() {
    sEntryCount = 0;
    for (s32 slot = 0; slot < RADIAL_SLOT_COUNT; slot++) {
        if (SlotIsEquippable(slot)) {
            sEntrySlots[sEntryCount++] = slot;
        }
    }
}

static f32 EntryAngle(s32 index) {
    // First item at the top, going clockwise
    return (index * 2.0f * M_PI / sEntryCount) - (M_PI / 2.0f);
}

static f32 AngleDistance(f32 a, f32 b) {
    return fabsf(remainderf(a - b, 2.0f * M_PI));
}

static void PlaySfx(u16 sfxId) {
    Audio_PlaySfxGeneral(sfxId, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultReverb);
}

// The menu stands in for the pause menu, so it can open whenever the pause menu could (see Play_Update)
static bool CanShowMenu(PlayState* play) {
    return gSaveContext.gameMode == GAMEMODE_NORMAL && play->msgCtx.msgMode == MSGMODE_NONE && gTrnsnUnkState != 3 &&
           R_PAUSE_MENU_MODE == 0 && KaleidoSetup_CanOpen(play);
}

static void RadialItemMenu_Open(PlayState* play) {
    sOpen = true;
    sOpenTimer = 0;
    sCursorSlot = -1;
    for (s32 i = 0; i < ARRAY_COUNT(sFlashTimer); i++) {
        sFlashTimer[i] = 0;
    }

    // Start on whatever is on C-Left (or the first item) so the center panel isn't empty
    RefreshEntries();
    for (s32 i = 0; i < sEntryCount; i++) {
        if (sCursorSlot == -1 || gSaveContext.equips.cButtonSlots[0] == sEntrySlots[i]) {
            sCursorSlot = sEntrySlots[i];
        }
    }

    // Frame advance halts Play_Update while still drawing the scene, which is exactly a "soft pause"
    sFrozeGame = CVarGetInteger(CVAR_RADIAL_MENU_FREEZE, 1);
    if (sFrozeGame) {
        sPrevFrameAdvance = play->frameAdvCtx.enabled;
        play->frameAdvCtx.enabled = true;
    }

    PlaySfx(NA_SE_SY_WIN_OPEN);
}

static void RadialItemMenu_Close(PlayState* play, bool playSound) {
    if (!sOpen) {
        return;
    }

    sOpen = false;

    if (sFrozeGame && play != nullptr) {
        play->frameAdvCtx.enabled = sPrevFrameAdvance;
    }
    sFrozeGame = false;

    if (playSound) {
        PlaySfx(NA_SE_SY_WIN_CLOSE);
    }
}

// Mirrors KaleidoScope_UpdateItemEquip, minus the flying-icon animation
static void RadialItemMenu_Equip(PlayState* play, s32 slot, s32 cButton) {
    u16 item = gSaveContext.inventory.items[slot];

    if (!SlotIsEquippable(slot)) {
        PlaySfx(NA_SE_SY_ERROR);
        return;
    }

    if (!GameInteractor_Should(VB_EQUIP_ITEM_TO_C_BUTTON, true, play, slot, item)) {
        return;
    }

    u16 equipSlot = slot;
    if (item == ITEM_ARROW_FIRE || item == ITEM_ARROW_ICE || item == ITEM_ARROW_LIGHT) {
        item = ITEM_BOW_ARROW_FIRE + (item == ITEM_ARROW_ICE ? 1 : 0) + (item == ITEM_ARROW_LIGHT ? 2 : 0);
        if (!CVarGetInteger(CVAR_ENHANCEMENT("SeparateArrows"), 0)) {
            equipSlot = SLOT_BOW;
        }
    }

    // If the item is on another button already, swap the two
    u16 targetButtonIndex = cButton + 1;
    for (u16 otherSlotIndex = 0; otherSlotIndex < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); otherSlotIndex++) {
        u16 otherButtonIndex = otherSlotIndex + 1;
        if (otherSlotIndex == cButton) {
            continue;
        }

        if (equipSlot == gSaveContext.equips.cButtonSlots[otherSlotIndex]) {
            if (gSaveContext.equips.buttonItems[targetButtonIndex] != ITEM_NONE) {
                gSaveContext.equips.buttonItems[otherButtonIndex] = gSaveContext.equips.buttonItems[targetButtonIndex];
                gSaveContext.equips.cButtonSlots[otherSlotIndex] = gSaveContext.equips.cButtonSlots[cButton];
                if (otherButtonIndex <= 3) {
                    Interface_LoadItemIcon2(play, otherButtonIndex);
                }
            } else {
                gSaveContext.equips.buttonItems[otherButtonIndex] = ITEM_NONE;
                gSaveContext.equips.cButtonSlots[otherSlotIndex] = SLOT_NONE;
            }
        }

        // Fix for Equip Dupe
        if (item == ITEM_BOW) {
            if (gSaveContext.equips.buttonItems[otherButtonIndex] >= ITEM_BOW_ARROW_FIRE &&
                gSaveContext.equips.buttonItems[otherButtonIndex] <= ITEM_BOW_ARROW_LIGHT &&
                !CVarGetInteger(CVAR_ENHANCEMENT("SeparateArrows"), 0)) {
                gSaveContext.equips.buttonItems[otherButtonIndex] = gSaveContext.equips.buttonItems[targetButtonIndex];
                gSaveContext.equips.cButtonSlots[otherSlotIndex] = gSaveContext.equips.cButtonSlots[cButton];
                if (otherButtonIndex <= 3) {
                    Interface_LoadItemIcon2(play, otherButtonIndex);
                }
            }
        }
    }

    gSaveContext.equips.buttonItems[targetButtonIndex] = item;
    gSaveContext.equips.cButtonSlots[cButton] = equipSlot;
    if (targetButtonIndex <= 3) {
        Interface_LoadItemIcon1(play, targetButtonIndex);
        sFlashTimer[cButton] = RADIAL_FLASH_FRAMES;
    }

    PlaySfx(NA_SE_SY_DECIDE);
}

static void RadialItemMenu_UpdateCursor(Input* input) {
    s32 stickX = input->cur.stick_x;
    s32 stickY = input->cur.stick_y;

    if ((stickX * stickX) + (stickY * stickY) < (RADIAL_STICK_DEADZONE * RADIAL_STICK_DEADZONE)) {
        return;
    }

    // Pick the item closest to the stick direction (screen space has Y pointing down)
    f32 stickAngle = atan2f(-stickY, stickX);
    s32 best = -1;
    f32 bestDist = 0.0f;
    f32 cursorDist = -1.0f;
    for (s32 i = 0; i < sEntryCount; i++) {
        f32 dist = AngleDistance(stickAngle, EntryAngle(i));
        if (best == -1 || dist < bestDist) {
            best = i;
            bestDist = dist;
        }
        if (sEntrySlots[i] == sCursorSlot) {
            cursorDist = dist;
        }
    }

    // Hold on to the highlighted item until the stick is clearly in a neighbour's slice, so it doesn't flicker
    // between two items when the stick sits near the edge
    if (best == -1 || sEntrySlots[best] == sCursorSlot ||
        (cursorDist >= 0.0f && cursorDist - bestDist < RADIAL_STICK_HYSTERESIS)) {
        return;
    }

    sCursorSlot = sEntrySlots[best];
    PlaySfx(NA_SE_SY_CURSOR);
}

static void RadialItemMenu_OnGameStateMainStart() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        sOpen = false;
        sFrozeGame = false;
        sHeldMask = 0;
        return;
    }

    PlayState* play = gPlayState;
    Input* input = &play->state.input[0];
    u32 menuBtn = CVarGetInteger(CVAR_RADIAL_MENU_BTN, BTN_L);
    bool menuBtnHeld = menuBtn != 0 && CHECK_BTN_ALL(input->cur.button, menuBtn);
    bool menuBtnPressed = menuBtnHeld && CHECK_BTN_ANY(input->press.button, menuBtn);
    bool wasOpen = sOpen;

    if (!sOpen) {
        if (menuBtnPressed && CanShowMenu(play)) {
            RadialItemMenu_Open(play);
        }
    } else if (!CanShowMenu(play)) {
        RadialItemMenu_Close(play, false);
    } else if (CVarGetInteger(CVAR_RADIAL_MENU_TOGGLE, 0)
                   ? (menuBtnPressed || CHECK_BTN_ANY(input->press.button, BTN_B | BTN_START))
                   : !menuBtnHeld) {
        RadialItemMenu_Close(play, true);
    } else {
        if (sOpenTimer < RADIAL_OPEN_FRAMES) {
            sOpenTimer++;
        }
        for (s32 i = 0; i < ARRAY_COUNT(sFlashTimer); i++) {
            if (sFlashTimer[i] > 0) {
                sFlashTimer[i]--;
            }
        }

        RefreshEntries();
        RadialItemMenu_UpdateCursor(input);

        s32 buttonCount = CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0) ? ARRAY_COUNT(sEquipButtons) : 3;
        for (s32 i = 0; i < buttonCount; i++) {
            if (!(menuBtn & sEquipButtons[i]) && CHECK_BTN_ALL(input->press.button, sEquipButtons[i])) {
                if (sCursorSlot >= 0) {
                    RadialItemMenu_Equip(play, sCursorSlot, i);
                }
                break;
            }
        }
    }

    // Keep everything the menu touched away from Link, including the press that closed it
    if (sOpen || wasOpen) {
        sHeldMask |= input->cur.button;
    }
    sHeldMask &= input->cur.button;

    input->cur.button &= ~sHeldMask;
    input->press.button &= ~sHeldMask;

    if (sOpen || wasOpen) {
        input->cur.stick_x = input->cur.stick_y = 0;
        input->rel.stick_x = input->rel.stick_y = 0;
        input->press.stick_x = input->press.stick_y = 0;
        input->cur.right_stick_x = input->cur.right_stick_y = 0;
        input->rel.right_stick_x = input->rel.right_stick_y = 0;
    }
}

static Gfx* DrawTextureRGBA32(Gfx* gfx, const void* texture, f32 x, f32 y, f32 size) {
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(gfx++, (s32)(x * 4.0f), (s32)(y * 4.0f), (s32)((x + size) * 4.0f), (s32)((y + size) * 4.0f),
                            G_TX_RENDERTILE, 0, 0, (s32)((32 << 10) / size), (s32)((32 << 10) / size));
    return gfx;
}

static Gfx* DrawTextureIA8(Gfx* gfx, const void* texture, f32 x, f32 y, f32 size) {
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(gfx++, (s32)(x * 4.0f), (s32)(y * 4.0f), (s32)((x + size) * 4.0f), (s32)((y + size) * 4.0f),
                            G_TX_RENDERTILE, 0, 0, (s32)((32 << 10) / size), (s32)((32 << 10) / size));
    return gfx;
}

static Gfx* DrawItemName(Gfx* gfx, u8 item, f32 x, f32 y) {
    gDPLoadTextureBlock_4b(gfx++, KaleidoScope_GetItemNameTexture(item), G_IM_FMT_IA, 128, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gSPWideTextureRectangle(gfx++, (s32)(x * 4.0f), (s32)(y * 4.0f), (s32)((x + 128.0f) * 4.0f),
                            (s32)((y + 16.0f) * 4.0f), G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    return gfx;
}

static Color_RGB8 GetCButtonColor(s32 cButton) {
    static const char* sCButtonCosmetics[] = { "HUD.CLeftButton", "HUD.CDownButton", "HUD.CRightButton" };
    Color_RGB8 color = { 255, 160, 0 };

    if (CVarGetInteger(CVAR_COSMETIC("HUD.CButtons.Changed"), 0)) {
        color = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), color);
    }
    std::string base = std::string(CVAR_COSMETIC("")) + sCButtonCosmetics[cButton];
    if (CVarGetInteger((base + ".Changed").c_str(), 0)) {
        color = CVarGetColor24((base + ".Value").c_str(), color);
    }
    return color;
}

static void RadialItemMenu_Draw() {
    if (!sOpen || gPlayState == nullptr) {
        return;
    }

    PlayState* play = gPlayState;
    f32 t = (sOpenTimer + 1) / (f32)(RADIAL_OPEN_FRAMES + 1);
    f32 ease = 1.0f - ((1.0f - t) * (1.0f - t));
    u8 alpha = (u8)(255 * ease);
    f32 radius = RADIAL_RADIUS * (0.7f + (0.3f * ease));
    f32 centerX = SCREEN_WIDTH / 2.0f;
    f32 centerY = SCREEN_HEIGHT / 2.0f;

    OPEN_DISPS(play->state.gfxCtx);

    // Dim the scene, HUD included
    gDPPipeSync(OVERLAY_DISP++);
    gSPTexture(OVERLAY_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetCycleType(OVERLAY_DISP++, G_CYC_1CYCLE);
    gDPSetRenderMode(OVERLAY_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetAlphaCompare(OVERLAY_DISP++, G_AC_NONE);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, (u8)(130 * ease));
    gDPFillWideRectangle(OVERLAY_DISP++, OTRGetRectDimensionFromLeftEdge(0), 0,
                         OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH), SCREEN_HEIGHT);

    Gfx_SetupDL_39Overlay(play->state.gfxCtx);

    // The fewer items there are, the bigger each one gets
    f32 spacing = (sEntryCount > 0) ? (2.0f * M_PI * RADIAL_RADIUS / sEntryCount) : RADIAL_SLOT_BG_SIZE_MAX;
    f32 slotSize = CLAMP(spacing * 0.8f, RADIAL_SLOT_BG_SIZE_MIN, RADIAL_SLOT_BG_SIZE_MAX);
    f32 iconSize = slotSize * 0.8f;

    // Slot backgrounds
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    for (s32 i = 0; i < sEntryCount; i++) {
        f32 x = centerX + cosf(EntryAngle(i)) * radius;
        f32 y = centerY + sinf(EntryAngle(i)) * radius;
        f32 size = slotSize;

        gDPPipeSync(OVERLAY_DISP++);
        if (sEntrySlots[i] == sCursorSlot) {
            size += 7.0f;
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 215, 70, alpha);
        } else {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 30, 45, 90, (u8)(210 * ease));
        }
        OVERLAY_DISP = DrawTextureIA8(OVERLAY_DISP, gButtonBackgroundTex, x - (size / 2.0f), y - (size / 2.0f), size);
    }

    // Center panel C-Button backgrounds, laid out like the HUD
    static const f32 sCButtonOffsets[][2] = { { -26.0f, 30.0f }, { 0.0f, 42.0f }, { 26.0f, 30.0f } };
    for (s32 i = 0; i < 3; i++) {
        Color_RGB8 color = GetCButtonColor(i);
        f32 size = 24.0f + (sFlashTimer[i] > 0 ? 4.0f : 0.0f);
        f32 x = centerX + sCButtonOffsets[i][0] - (size / 2.0f);
        f32 y = centerY + sCButtonOffsets[i][1] - (size / 2.0f);

        gDPPipeSync(OVERLAY_DISP++);
        if (sFlashTimer[i] > 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        } else {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, color.r, color.g, color.b, (u8)(200 * ease));
        }
        OVERLAY_DISP = DrawTextureIA8(OVERLAY_DISP, gButtonBackgroundTex, x, y, size);

        if (gSaveContext.equips.buttonItems[i + 1] == ITEM_NONE) {
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, color.r, color.g, color.b, alpha);
            OVERLAY_DISP = DrawTextureIA8(OVERLAY_DISP, sEmptyCButtonTextures[i], x, y, size);
        }
    }

    // Item name of the highlighted item
    if (sCursorSlot >= 0 && SlotIsEquippable(sCursorSlot)) {
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        OVERLAY_DISP =
            DrawItemName(OVERLAY_DISP, gSaveContext.inventory.items[sCursorSlot], centerX - 64.0f, centerY - 2.0f);
    }

    // Item icons
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
    for (s32 i = 0; i < sEntryCount; i++) {
        s32 slot = sEntrySlots[i];
        f32 x = centerX + cosf(EntryAngle(i)) * radius;
        f32 y = centerY + sinf(EntryAngle(i)) * radius;
        f32 size = iconSize + ((slot == sCursorSlot) ? 6.0f : 0.0f);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        OVERLAY_DISP = DrawTextureRGBA32(OVERLAY_DISP, gItemIcons[gSaveContext.inventory.items[slot]],
                                         x - (size / 2.0f), y - (size / 2.0f), size);
    }

    // Large preview of the highlighted item
    if (sCursorSlot >= 0 && SlotIsEquippable(sCursorSlot)) {
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        OVERLAY_DISP = DrawTextureRGBA32(OVERLAY_DISP, gItemIcons[gSaveContext.inventory.items[sCursorSlot]],
                                         centerX - 16.0f, centerY - 38.0f, 32.0f);
    }

    // What is currently assigned to each C-Button
    for (s32 i = 0; i < 3; i++) {
        u8 item = gSaveContext.equips.buttonItems[i + 1];
        if (item == ITEM_NONE || item >= ARRAY_COUNT(gItemIcons)) {
            continue;
        }

        f32 size = 18.0f;
        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
        OVERLAY_DISP =
            DrawTextureRGBA32(OVERLAY_DISP, gItemIcons[item], centerX + sCButtonOffsets[i][0] - (size / 2.0f),
                              centerY + sCButtonOffsets[i][1] - (size / 2.0f), size);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static void RegisterRadialItemMenu() {
    if (!CVAR_RADIAL_MENU_VALUE) {
        RadialItemMenu_Close(gPlayState, false);
        sHeldMask = 0;
    }

    COND_HOOK(OnGameStateMainStart, CVAR_RADIAL_MENU_VALUE, RadialItemMenu_OnGameStateMainStart);
    COND_HOOK(OnPlayDrawOverlayEnd, CVAR_RADIAL_MENU_VALUE, RadialItemMenu_Draw);
    COND_HOOK(OnPlayDestroy, CVAR_RADIAL_MENU_VALUE, []() {
        sOpen = false;
        sFrozeGame = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterRadialItemMenu, { CVAR_RADIAL_MENU_NAME });
