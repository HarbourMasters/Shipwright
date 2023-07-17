#include <libultraship/bridge.h>
#include "draw.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "randomizerTypes.h"
#include <array>
#include "objects/object_gi_key/object_gi_key.h"
#include "objects/object_gi_bosskey/object_gi_bosskey.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "soh_assets.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"

extern "C" void Randomizer_DrawSmallKey(PlayState* play, GetItemEntry* getItemEntry) {
    s32 pad;
    s8 isColoredKeysEnabled = CVarGetInteger("gRandoMatchKeyColors", 1);
    s16 color_slot = getItemEntry->getItemId - RG_FOREST_TEMPLE_SMALL_KEY;
    s16 colors[9][3] = {
        { 4, 195, 46 },    // Forest Temple
        { 237, 95, 95 },   // Fire Temple
        { 85, 180, 223 },  // Water Temple
        { 222, 158, 47 },  // Spirit Temple
        { 126, 16, 177 },  // Shadow Temple
        { 227, 110, 255 }, // Bottom of the Well
        { 221, 212, 60 },  // Gerudo Training Grounds
        { 255, 255, 255 }, // Thieves' Hideout
        { 80, 80, 80 }     // Ganon's Castle
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    if (isColoredKeysEnabled) {
        gDPSetGrayscaleColor(POLY_OPA_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);
        gSPGrayscale(POLY_OPA_DISP++, true);
    }

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiSmallKeyDL);

    if (isColoredKeysEnabled) {
        gSPGrayscale(POLY_OPA_DISP++, false);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawBossKey(PlayState* play, GetItemEntry* getItemEntry) {
    s32 pad;
    s8 isColoredKeysEnabled = CVarGetInteger("gRandoMatchKeyColors", 1);
    s16 color_slot;
    color_slot = getItemEntry->getItemId - RG_FOREST_TEMPLE_BOSS_KEY;
    s16 colors[6][3] = {
        { 4, 195, 46 },   // Forest Temple
        { 237, 95, 95 },  // Fire Temple
        { 85, 180, 223 }, // Water Temple
        { 222, 158, 47 }, // Spirit Temple
        { 126, 16, 177 }, // Shadow Temple
        { 210, 0, 0 }     // Ganon's Castle
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    if (color_slot == 5 && isColoredKeysEnabled) { // Ganon's Boss Key
        gDPSetGrayscaleColor(POLY_OPA_DISP++, 80, 80, 80, 255);
        gSPGrayscale(POLY_OPA_DISP++, true);
    }

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiBossKeyDL);

    if (color_slot == 5 && isColoredKeysEnabled) { // Ganon's Boss Key
        gSPGrayscale(POLY_OPA_DISP++, false);
    }

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    if (isColoredKeysEnabled) {
        gDPSetGrayscaleColor(POLY_XLU_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);
        gSPGrayscale(POLY_XLU_DISP++, true);
    }

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBossKeyGemDL);

    if (isColoredKeysEnabled) {
        gSPGrayscale(POLY_XLU_DISP++, false);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawKeyRing(PlayState* play, GetItemEntry* getItemEntry) {
    s32 pad;

    s16 color_slot = getItemEntry->getItemId - RG_FOREST_TEMPLE_KEY_RING;
    s16 colors[9][3] = {
        { 4, 195, 46 },    // Forest Temple
        { 237, 95, 95 },   // Fire Temple
        { 85, 180, 223 },  // Water Temple
        { 222, 158, 47 },  // Spirit Temple
        { 126, 16, 177 },  // Shadow Temple
        { 227, 110, 255 }, // Bottom of the Well
        { 221, 212, 60 },  // Gerudo Training Grounds
        { 255, 255, 255 }, // Thieves' Hideout
        { 80, 80, 80 }     // Ganon's Castle
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPSetGrayscaleColor(POLY_OPA_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);
    gSPGrayscale(POLY_OPA_DISP++, true);

    Matrix_Scale(0.5f, 0.5f, 0.5f, MTXMODE_APPLY);
    Matrix_RotateZ(0.8f, MTXMODE_APPLY);
    Matrix_RotateX(-2.16f, MTXMODE_APPLY);
    Matrix_RotateY(-0.56f, MTXMODE_APPLY);
    Matrix_RotateZ(-0.86f, MTXMODE_APPLY);
    Matrix_Translate(28.29f, 0, 0, MTXMODE_APPLY);
    Matrix_Translate(-(3.12f * 2), -(-0.34f * 2), -(17.53f * 2), MTXMODE_APPLY);
    Matrix_RotateX(-(-0.31f * 2), MTXMODE_APPLY);
    Matrix_RotateY(-(0.19f * 2), MTXMODE_APPLY);
    Matrix_RotateZ(-(0.20f * 2), MTXMODE_APPLY);
    for (int i = 0; i < 5; i++) {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                G_MTX_MODELVIEW | G_MTX_LOAD);
        Matrix_Translate(3.12f, -0.34f, 17.53f, MTXMODE_APPLY);
        Matrix_RotateX(-0.31f, MTXMODE_APPLY);
        Matrix_RotateY(0.19f, MTXMODE_APPLY);
        Matrix_RotateZ(0.20f, MTXMODE_APPLY);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiSmallKeyDL);
    }

    gSPGrayscale(POLY_OPA_DISP++, false);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawDoubleDefense(PlayState* play, GetItemEntry getItemEntry) {
    s32 pad;
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
   
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetGrayscaleColor(POLY_XLU_DISP++, 255, 255, 255, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartBorderDL);

    gSPGrayscale(POLY_XLU_DISP++, false);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartContainerDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawOcarinaButton(PlayState* play, GetItemEntry* getItemEntry) {
    Color_RGB8 aButtonColor = { 80, 150, 255 };
    if (CVarGetInteger("gCosmetics.Hud_AButton.Changed", 0)) {
        aButtonColor = CVarGetColor24("gCosmetics.Hud_AButton.Value", aButtonColor);
    } else if (CVarGetInteger("gCosmetics.DefaultColorScheme", COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        aButtonColor = { 80, 255, 150 };
    }

    Color_RGB8 cButtonsColor = { 255, 255, 50 };
    if (CVarGetInteger("gCosmetics.Hud_CButtons.Changed", 0)) {
        cButtonsColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
    }
    Color_RGB8 cUpButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CUpButton.Changed", 0)) {
        cUpButtonColor = CVarGetColor24("gCosmetics.Hud_CUpButton.Value", cUpButtonColor);
    }
    Color_RGB8 cDownButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CDownButton.Changed", 0)) {
        cDownButtonColor = CVarGetColor24("gCosmetics.Hud_CDownButton.Value", cDownButtonColor);
    }
    Color_RGB8 cLeftButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CLeftButton.Changed", 0)) {
        cLeftButtonColor = CVarGetColor24("gCosmetics.Hud_CLeftButton.Value", cLeftButtonColor);
    }
    Color_RGB8 cRightButtonColor = cButtonsColor;
    if (CVarGetInteger("gCosmetics.Hud_CRightButton.Changed", 0)) {
        cRightButtonColor = CVarGetColor24("gCosmetics.Hud_CRightButton.Value", cRightButtonColor);
    }

    s16 slot = getItemEntry->drawItemId - RG_OCARINA_A_BUTTON;
    
    Gfx* dLists[] = {
        (Gfx*)gOcarinaAButtonDL,
        (Gfx*)gOcarinaCLeftButtonDL,
        (Gfx*)gOcarinaCRightButtonDL,
        (Gfx*)gOcarinaCUpButtonDL,
        (Gfx*)gOcarinaCDownButtonDL
    };

    Color_RGB8 colors[] = {
        aButtonColor,
        cLeftButtonColor,
        cRightButtonColor,
        cUpButtonColor,
        cDownButtonColor
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    
    gDPSetGrayscaleColor(POLY_XLU_DISP++, colors[slot].r, colors[slot].g, colors[slot].b, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, dLists[slot]);

    CLOSE_DISPS(play->state.gfxCtx);
}