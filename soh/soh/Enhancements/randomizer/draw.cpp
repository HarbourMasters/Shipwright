#include "draw.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "randomizerTypes.h"
#include <array>
#include "objects/object_gi_key/object_gi_key.h"
#include "objects/object_gi_bosskey/object_gi_bosskey.h"

extern "C" void Randomizer_DrawSmallKey(GlobalContext* globalCtx, GetItemEntry* getItemEntry) {
    s32 pad;
    
    std::array<s16, 3> color;
    switch(getItemEntry->getItemId) {
        case RG_FOREST_TEMPLE_SMALL_KEY:
            color = {4, 195, 46};
            break;
        case RG_FIRE_TEMPLE_SMALL_KEY:
            color = {237, 95, 95};
            break;
        case RG_WATER_TEMPLE_SMALL_KEY:
            color = {85, 180, 223};
            break;
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
            color = {222, 158, 47};
            break;
        case RG_SHADOW_TEMPLE_SMALL_KEY:
            color = {126, 16, 177};
            break;
        case RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY:
            color = {221, 212, 60};
            break;
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            color = {227, 110, 255};
            break;
        case RG_GANONS_CASTLE_SMALL_KEY:
            color = {80, 80, 80};
            break;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
        G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gsDPSetGrayscaleColor(POLY_OPA_DISP++, color[0], color[1], color[2], 255);
    gsSPGrayscale(POLY_OPA_DISP++, true);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiSmallKeyDL);

    gsSPGrayscale(POLY_OPA_DISP++, false);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

extern "C" void Randomizer_DrawBossKey(GlobalContext* globalCtx, GetItemEntry* getItemEntry) {
    s32 pad;
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

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    if (color_slot == 5) { // Ganon's Boss Key
        gsDPSetGrayscaleColor(POLY_OPA_DISP++, 80, 80, 80, 255);
        gsSPGrayscale(POLY_OPA_DISP++, true);
    }

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiBossKeyDL);

    if (color_slot == 5) { // Ganon's Boss Key
        gsSPGrayscale(POLY_OPA_DISP++, false);
    }

    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    if (color_slot >= 0) {
        gsDPSetGrayscaleColor(POLY_XLU_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2],
                              255);
        gsSPGrayscale(POLY_XLU_DISP++, true);
    }

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBossKeyGemDL);

    if (color_slot >= 0) {
        gsSPGrayscale(POLY_XLU_DISP++, false);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}