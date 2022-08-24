#include "draw.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "randomizerTypes.h"
#include <array>
#include "objects/object_gi_key/object_gi_key.h"
#include "objects/object_gi_bosskey/object_gi_bosskey.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"

extern "C" void Randomizer_DrawSmallKey(GlobalContext* globalCtx, GetItemEntry* getItemEntry) {
    s32 pad;

    s16 color_slot = getItemEntry->getItemId - RG_FOREST_TEMPLE_SMALL_KEY;
    s16 colors[9][3] = {
        { 4, 195, 46 },    // Forest Temple
        { 237, 95, 95 },   // Fire Temple
        { 85, 180, 223 },  // Water Temple
        { 222, 158, 47 },  // Spirit Temple
        { 126, 16, 177 },  // Shadow Temple
        { 227, 110, 255 }, // Bottom of the Well
        { 221, 212, 60 },  // Gerudo Training Grounds
        { 255, 255, 255 }, // Theive's Hideout (unused)
        { 80, 80, 80 }     // Ganon's Castle
    };

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gsDPSetGrayscaleColor(POLY_OPA_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);
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

    gsDPSetGrayscaleColor(POLY_XLU_DISP++, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2],
                            255);
    gsSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBossKeyGemDL);

    gsSPGrayscale(POLY_XLU_DISP++, false);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

extern "C" void Randomizer_DrawDoubleDefense(GlobalContext* globalCtx, GetItemEntry getItemEntry) {
    s32 pad;
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);
   
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    gsDPSetGrayscaleColor(POLY_XLU_DISP++, 255, 255, 255, 255);
    gsSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartBorderDL);

    gsSPGrayscale(POLY_XLU_DISP++, false);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartContainerDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
