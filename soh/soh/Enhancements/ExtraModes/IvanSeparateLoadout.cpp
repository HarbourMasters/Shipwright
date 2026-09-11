#include <array>
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"
#include "soh/frame_interpolation.h" // IWYU pragma: keep

extern "C" {
#include "textures/parameter_static/parameter_static.h"
#include "soh_assets.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
extern const char* _gAmmoDigit0Tex[];
Gfx* Gfx_TextureIA8(Gfx* displayListHead, void* texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                    s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
}

#define CVAR_IVAN_MODE_NAME CVAR_ENHANCEMENT("IvanCoopModeEnabled")
#define CVAR_IVAN_MODE_VALUE CVarGetInteger(CVAR_IVAN_MODE_NAME, 0)

#define CVAR_SEPARATE_LOADOUT_NAME CVAR_ENHANCEMENT("IvanSeparateLoadout")
#define CVAR_SEPARATE_LOADOUT_VALUE CVarGetInteger(CVAR_SEPARATE_LOADOUT_NAME, 0)

enum class IvanItemIndex : u8 {
    CLeft,
    CDown,
    CRight,
    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,
    Max, // Sentinel
};

static constexpr size_t kIvanItemMax = static_cast<size_t>(IvanItemIndex::Max);
static_assert(kIvanItemMax == ARRAY_COUNT(gSaveContext.ship.ivanButtonItems));

static const u16 kEquipButtons[] = {
    BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, BTN_DUP, BTN_DDOWN, BTN_DLEFT, BTN_DRIGHT,
};
static_assert(ARRAY_COUNT(kEquipButtons) == kIvanItemMax);

struct EquipAnim {
    s16 startX, startY;
    s16 framesLeft;
};
static EquipAnim sEquipAnim[kIvanItemMax];
static const s16 kAnimFrames = 10;

static bool CanIvanUseItem(s16 itemId) {
    switch (itemId) {
        case ITEM_STICK:
        case ITEM_NUT:
        case ITEM_BOMB:
        case ITEM_BOMBCHU:
        case ITEM_BOW:
        case ITEM_ARROW_FIRE:
        case ITEM_ARROW_ICE:
        case ITEM_ARROW_LIGHT:
        case ITEM_SLINGSHOT:
        case ITEM_OCARINA_FAIRY:
        case ITEM_OCARINA_TIME:
        case ITEM_HOOKSHOT:
        case ITEM_LONGSHOT:
        case ITEM_DINS_FIRE:
        case ITEM_NAYRUS_LOVE:
        case ITEM_FARORES_WIND:
        case ITEM_HAMMER:
        case ITEM_BOOMERANG:
        case ITEM_LENS:
        case ITEM_BEAN:
            return true;
        default:
            return false;
    }
}

static void StartEquipAnim(PlayState* play, u8 slot) {
    s16 idx = play->pauseCtx.cursorSlot[PAUSE_ITEM] * 4;
    sEquipAnim[slot].startX = play->pauseCtx.itemVtx[idx].v.ob[0] + 160 + 16;
    sEquipAnim[slot].startY = 120 - play->pauseCtx.itemVtx[idx].v.ob[1] + 16;
    sEquipAnim[slot].framesLeft = kAnimFrames;
}

static void OnKaleidoUpdate() {
    PlayState* play = gPlayState;

    if (play->pauseCtx.state != 6 || play->pauseCtx.pageIndex != PAUSE_ITEM) {
        return;
    }

    u16 cursorItem = play->pauseCtx.cursorItem[PAUSE_ITEM];
    if (cursorItem == PAUSE_ITEM_NONE) {
        return;
    }

    u32 ivanButtons = play->state.input[1].press.button;
    if (cursorItem == ITEM_SOLD_OUT || cursorItem == ITEM_NONE) {
        return;
    }

    bool dpadEquips = CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0);

    for (size_t i = 0; i < kIvanItemMax; i++) {
        if (!dpadEquips && i >= static_cast<size_t>(IvanItemIndex::DPadUp)) {
            continue;
        }
        if (!CHECK_BTN_ALL(ivanButtons, kEquipButtons[i])) {
            continue;
        }
        if (!CanIvanUseItem(cursorItem)) {
            Audio_PlaySfxGeneral(NA_SE_SY_ERROR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            break;
        }
        u8 targetSlot = i;

        if (CVarGetInteger(CVAR_ENHANCEMENT("ItemUnequip"), 0) &&
            gSaveContext.ship.ivanButtonItems[targetSlot] == cursorItem) {
            gSaveContext.ship.ivanButtonItems[targetSlot] = ITEM_NONE;
            sEquipAnim[targetSlot].framesLeft = 0;
            Audio_PlaySfxGeneral(NA_SE_SY_DECIDE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            break;
        }

        u8 displacedItem = gSaveContext.ship.ivanButtonItems[targetSlot];
        for (size_t j = 0; j < kIvanItemMax; j++) {
            if (j != targetSlot && gSaveContext.ship.ivanButtonItems[j] == cursorItem) {
                gSaveContext.ship.ivanButtonItems[j] = displacedItem;
                sEquipAnim[j].framesLeft = 0;
            }
        }
        gSaveContext.ship.ivanButtonItems[targetSlot] = cursorItem;
        StartEquipAnim(play, targetSlot);
        Audio_PlaySfxGeneral(NA_SE_SY_DECIDE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        break;
    }
}

static bool IsAmmoItem(s16 itemId) {
    if ((itemId >= ITEM_BOW_ARROW_FIRE) && (itemId <= ITEM_BOW_ARROW_LIGHT)) {
        return true;
    }

    switch (itemId) {
        case ITEM_STICK:
        case ITEM_NUT:
        case ITEM_BOMB:
        case ITEM_BOW:
        case ITEM_SLINGSHOT:
        case ITEM_BOMBCHU:
        case ITEM_BEAN:
            return true;
        default:
            return false;
    }
}

static Gfx* DrawAmmoCount(Gfx* gfx, s16 itemId, s16 x, s16 y, s16 alpha) {
    if (!GameInteractor_Should(VB_DRAW_AMMO_COUNT, IsAmmoItem(itemId), &itemId)) {
        return gfx;
    }

    if ((itemId >= ITEM_BOW_ARROW_FIRE) && (itemId <= ITEM_BOW_ARROW_LIGHT)) {
        itemId = ITEM_BOW;
    }

    s16 ammo = AMMO(itemId);
    if (ammo < 0) {
        ammo = 0;
    }

    s16 tens = ammo / 10;
    s16 ones = ammo % 10;

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, alpha);
    if (((itemId == ITEM_BOW) && (ammo == CUR_CAPACITY(UPG_QUIVER))) ||
        ((itemId == ITEM_BOMB) && (ammo == CUR_CAPACITY(UPG_BOMB_BAG))) ||
        ((itemId == ITEM_SLINGSHOT) && (ammo == CUR_CAPACITY(UPG_BULLET_BAG))) ||
        ((itemId == ITEM_STICK) && (ammo == CUR_CAPACITY(UPG_STICKS))) ||
        ((itemId == ITEM_NUT) && (ammo == CUR_CAPACITY(UPG_NUTS))) || ((itemId == ITEM_BOMBCHU) && (ammo == 50)) ||
        ((itemId == ITEM_BEAN) && (ammo == 15)) || GameInteractor_Should(VB_COLOR_AMMO_GREEN, false, itemId)) {
        gDPSetPrimColor(gfx++, 0, 0, 120, 255, 0, alpha);
    }
    if (ammo == 0) {
        gDPSetPrimColor(gfx++, 0, 0, 100, 100, 100, alpha);
    }
    if (tens != 0) {
        gfx = Gfx_TextureIA8(gfx, (u8*)_gAmmoDigit0Tex[tens], 8, 8, x, y, 8, 8, 1 << 10, 1 << 10);
    }
    gfx = Gfx_TextureIA8(gfx, (u8*)_gAmmoDigit0Tex[ones], 8, 8, x + 6, y, 8, 8, 1 << 10, 1 << 10);

    return gfx;
}

static Gfx* DrawItemIcon(Gfx* gfx, u8 slot, s16 centerX, s16 centerY, s16 alpha) {
    s16 item = gSaveContext.ship.ivanButtonItems[slot];
    if (item == ITEM_NONE) {
        return gfx;
    }

    if (item == ITEM_ARROW_FIRE)
        item = ITEM_BOW_ARROW_FIRE;
    else if (item == ITEM_ARROW_ICE)
        item = ITEM_BOW_ARROW_ICE;
    else if (item == ITEM_ARROW_LIGHT)
        item = ITEM_BOW_ARROW_LIGHT;

    EquipAnim& anim = sEquipAnim[slot];
    bool animating = anim.framesLeft > 0;
    if (animating) {
        float frac = static_cast<float>(anim.framesLeft) / kAnimFrames;
        centerX = centerX + (anim.startX - centerX) * frac;
        centerY = centerY + (anim.startY - centerY) * frac;
        anim.framesLeft--;
    }

    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, alpha);
    gDPLoadTextureBlock(gfx++, gItemIcons[item], G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(gfx++, (centerX - 8) << 2, (centerY - 8) << 2, (centerX + 8) << 2, (centerY + 8) << 2,
                            G_TX_RENDERTILE, 0, 0, 2 << 10, 2 << 10);

    if (!animating) {
        gfx = DrawAmmoCount(gfx, item, centerX - 8, centerY - 8, alpha);
    }

    return gfx;
}

static Color_RGB8 CalculateTint() {
    Color_RGB8 primary = { 255, 255, 255 };
    if (CVarGetInteger(CVAR_COSMETIC("Ivan.IdlePrimary.Changed"), 0)) {
        primary = CVarGetColor24(CVAR_COSMETIC("Ivan.IdlePrimary.Value"), (Color_RGB8){ 255, 255, 255 });
    }
    Color_RGB8 secondary = { 0, 255, 0 };
    if (CVarGetInteger(CVAR_COSMETIC("Ivan.IdleSecondary.Changed"), 0)) {
        secondary = CVarGetColor24(CVAR_COSMETIC("Ivan.IdleSecondary.Value"), (Color_RGB8){ 0, 255, 0 });
    }
    Color_RGB8 tint = {
        (primary.r + secondary.r) / 2,
        (primary.g + secondary.g) / 2,
        (primary.b + secondary.b) / 2,
    };
    return tint;
}

static std::array<s16, 2> CalculateCButtonsCenter() {
    s16 bottomMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.B"), 0);
    s16 leftMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.L"), 0);
    s16 rightMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.R"), 0);

    s16 posType = CVarGetInteger(CVAR_COSMETIC("Ivan.CButtons.PosType"), ORIGINAL_LOCATION);
    bool useMargins = CVarGetInteger(CVAR_COSMETIC("Ivan.CButtons.UseMargins"), 0);
    s16 yMargin = useMargins ? bottomMargin : 0;

    if (posType == ORIGINAL_LOCATION) {
        return { 256 + (useMargins ? rightMargin : 0), 208 + yMargin };
    }

    s16 posX = CVarGetInteger(CVAR_COSMETIC("Ivan.CButtons.PosX"), 0);
    s16 y = CVarGetInteger(CVAR_COSMETIC("Ivan.CButtons.PosY"), 0) + yMargin;
    switch (posType) {
        case ANCHOR_LEFT:
            return { OTRGetDimensionFromLeftEdge(posX + (useMargins ? leftMargin : 0)), y };
        case ANCHOR_RIGHT:
            return { OTRGetDimensionFromRightEdge(posX + (useMargins ? rightMargin : 0)), y };
        case ANCHOR_NONE:
            return { posX, y };
        case HIDDEN:
        default:
            return { -9999, y };
    }
}

static std::array<s16, 2> CalculateDpadCenter() {
    s16 bottomMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.B"), 0);
    s16 leftMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.L"), 0);
    s16 rightMargin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.R"), 0);

    s16 posType = CVarGetInteger(CVAR_COSMETIC("Ivan.Dpad.PosType"), ORIGINAL_LOCATION);
    bool useMargins = CVarGetInteger(CVAR_COSMETIC("Ivan.Dpad.UseMargins"), 0);
    s16 yMargin = useMargins ? bottomMargin : 0;

    if (posType == ORIGINAL_LOCATION) {
        return { 64 + (useMargins ? leftMargin : 0), 208 + yMargin };
    }

    s16 posX = CVarGetInteger(CVAR_COSMETIC("Ivan.Dpad.PosX"), 0);
    s16 y = CVarGetInteger(CVAR_COSMETIC("Ivan.Dpad.PosY"), 0) + yMargin;
    switch (posType) {
        case ANCHOR_LEFT:
            return { OTRGetDimensionFromLeftEdge(posX + (useMargins ? leftMargin : 0)), y };
        case ANCHOR_RIGHT:
            return { OTRGetDimensionFromRightEdge(posX + (useMargins ? rightMargin : 0)), y };
        case ANCHOR_NONE:
            return { posX, y };
        case HIDDEN:
        default:
            return { -9999, y };
    }
}

static void OnInterfaceDraw() {
    PlayState* play = gPlayState;

    Color_RGB8 tint = CalculateTint();
    s16 alpha = play->interfaceCtx.healthAlpha;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_39Overlay(play->state.gfxCtx);

    auto [cButtonsX, cButtonsY] = CalculateCButtonsCenter();
    s16 cButtonIconCenters[4][2] = {
        { cButtonsX - 16, cButtonsY }, // C-Left
        { cButtonsX, cButtonsY + 16 }, // C-Down
        { cButtonsX + 16, cButtonsY }, // C-Right
        { cButtonsX, cButtonsY - 16 }, // C-Up
    };

    // Draw C button backgrounds
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, tint.r, tint.g, tint.b, alpha);
    gDPLoadTextureBlock(OVERLAY_DISP++, gButtonBackgroundTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    for (auto [x, y] : cButtonIconCenters) {
        gSPWideTextureRectangle(OVERLAY_DISP++, (x - 8) << 2, (y - 8) << 2, (x + 8) << 2, (y + 8) << 2, G_TX_RENDERTILE,
                                0, 0, 2 << 10, 2 << 10);
    }

    // Draw C button assignments
    for (size_t i = 0; i < 3; i++) {
        u8 slot = static_cast<size_t>(IvanItemIndex::CLeft) + i;
        OVERLAY_DISP = DrawItemIcon(OVERLAY_DISP, slot, cButtonIconCenters[i][0], cButtonIconCenters[i][1], alpha);
    }

    // Draw "Ivan" label on C-Up
    s16 ivanLabelX = cButtonIconCenters[3][0] - 16;
    s16 ivanLabelY = cButtonIconCenters[3][1] - 4;
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);
    gDPLoadTextureBlock_4b(OVERLAY_DISP++, (void*)gNaviCUpENGTex, G_IM_FMT_IA, 32, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPWideTextureRectangle(OVERLAY_DISP++, ivanLabelX << 2, ivanLabelY << 2, (ivanLabelX + 32) << 2,
                            (ivanLabelY + 8) << 2, G_TX_RENDERTILE, (31 << 5), 0, -(1 << 10), 1 << 10);

    if (CVarGetInteger(CVAR_ENHANCEMENT("DpadEquips"), 0)) {
        auto [dpadX, dpadY] = CalculateDpadCenter();

        // Draw D-Pad background
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, tint.r, tint.g, tint.b, alpha);
        gDPLoadTextureBlock(OVERLAY_DISP++, gDPadTex, G_IM_FMT_IA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPWideTextureRectangle(OVERLAY_DISP++, (dpadX - 16) << 2, (dpadY - 16) << 2, (dpadX + 16) << 2,
                                (dpadY + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

        // Draw D-Pad assignments
        s16 dpadIconCenters[4][2] = {
            { dpadX, dpadY - 16 }, // Dpad-Up
            { dpadX, dpadY + 16 }, // Dpad-Down
            { dpadX - 16, dpadY }, // Dpad-Left
            { dpadX + 16, dpadY }, // Dpad-Right
        };
        for (size_t i = 0; i < ARRAY_COUNT(dpadIconCenters); i++) {
            u8 slot = static_cast<size_t>(IvanItemIndex::DPadUp) + i;
            OVERLAY_DISP = DrawItemIcon(OVERLAY_DISP, slot, dpadIconCenters[i][0], dpadIconCenters[i][1], alpha);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static void RegisterIvanSeparateLoadout() {
    bool enabled = CVAR_IVAN_MODE_VALUE && CVAR_SEPARATE_LOADOUT_VALUE;
    COND_HOOK(OnKaleidoUpdate, enabled, OnKaleidoUpdate);
    COND_HOOK(OnInterfaceDraw, enabled, OnInterfaceDraw);
}

static RegisterShipInitFunc initFunc(RegisterIvanSeparateLoadout, { CVAR_IVAN_MODE_NAME, CVAR_SEPARATE_LOADOUT_NAME });
