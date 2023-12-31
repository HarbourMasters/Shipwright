#include <libultraship/bridge.h>
#include "draw.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/OTRGlobals.h"
#include "randomizerTypes.h"
#include <array>
#include "objects/object_gi_key/object_gi_key.h"
#include "objects/object_gi_bosskey/object_gi_bosskey.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
#include "objects/object_toki_objects/object_toki_objects.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "soh_assets.h"

extern "C" {
extern SaveContext gSaveContext;
}

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

extern "C" void Randomizer_DrawSmallKey(PlayState* play, GetItemEntry* getItemEntry) {
    s8 keysCanBeOutsideDungeon = getItemEntry->getItemId == RG_GERUDO_FORTRESS_SMALL_KEY ?
        Randomizer_GetSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA :
        DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_KEYSANITY);
    s8 isColoredKeysEnabled = keysCanBeOutsideDungeon && CVarGetInteger("gRandoMatchKeyColors", 1);
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
    s8 keysCanBeOutsideDungeon = getItemEntry->getItemId == RG_GANONS_CASTLE_BOSS_KEY ?
        DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_GANONS_BOSS_KEY) :
        DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_BOSS_KEYSANITY);
    s8 isColoredKeysEnabled = keysCanBeOutsideDungeon && CVarGetInteger("gRandoMatchKeyColors", 1);
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

extern "C" void Randomizer_DrawMasterSword(PlayState* play, GetItemEntry getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 1 * (play->state.frames * 1),
                                0 * (play->state.frames * 1), 32, 32, 1, 0 * (play->state.frames * 1),
                                0 * (play->state.frames * 1), 32, 32));

    Matrix_Scale(0.05f, 0.05f, 0.05f, MTXMODE_APPLY);
    Matrix_RotateZ(2.1f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)object_toki_objects_DL_001BD0);
    
    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* Randomizer_GetTriforcePieceDL(uint8_t index) {
    switch (index) {
        case 1:
            return (Gfx*)gTriforcePiece1DL;
        case 2:
            return (Gfx*)gTriforcePiece2DL;
        default:
            return (Gfx*)gTriforcePiece0DL;
    }
}

extern "C" void Randomizer_DrawTriforcePiece(PlayState* play, GetItemEntry getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    uint8_t current = gSaveContext.triforcePiecesCollected;

    Matrix_Scale(0.035f, 0.035f, 0.035f, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Gfx* triforcePieceDL = Randomizer_GetTriforcePieceDL(current % 3);

    gSPDisplayList(POLY_XLU_DISP++, triforcePieceDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// Seperate draw function for drawing the Triforce piece when in the GI state.
// Needed for delaying showing the triforce piece slightly so the triforce shard doesn't
// suddenly snap to the new piece model or completed triforce because the piece is
// given mid textbox. Also makes it so the overworld models don't turn into the completed
// model when the player has exactly the required amount of pieces.
extern "C" void Randomizer_DrawTriforcePieceGI(PlayState* play, GetItemEntry getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    uint8_t current = gSaveContext.triforcePiecesCollected;
    uint8_t required = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED);

    Matrix_Scale(triforcePieceScale, triforcePieceScale, triforcePieceScale, MTXMODE_APPLY);

    // For creating a delay before showing the model so the model doesn't swap visually when the triforce piece
    // is given when the textbox just appears.
    if (triforcePieceScale < 0.0001f) {
        triforcePieceScale += 0.00003f;
    }
    
    // Animation. When not the completed triforce, create delay before showing the piece to bypass interpolation.
    // If the completed triforce, make it grow slowly.
    if (current != required) {
        if (triforcePieceScale > 0.00008f && triforcePieceScale < 0.034f) {
            triforcePieceScale = 0.034f;
        } else if (triforcePieceScale < 0.035f) {
            triforcePieceScale += 0.0005f;
        }
    } else if (triforcePieceScale > 0.00008f && triforcePieceScale < 0.035f) { 
        triforcePieceScale += 0.0005f;
    }

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    // Show piece when not currently completing the triforce. Use the scale to create a delay so interpolation doesn't
    // make the triforce twitch when the size is set to a higher value.
    if (current != required && triforcePieceScale > 0.035f) {
        // Get shard DL. Remove one before division to account for triforce piece given in the textbox
        // to match up the shard from the overworld model.
        Gfx* triforcePieceDL = Randomizer_GetTriforcePieceDL((current - 1) % 3);

        gSPDisplayList(POLY_XLU_DISP++, triforcePieceDL);
    } else if (current == required && triforcePieceScale > 0.00008f) {
        gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gTriforcePieceCompletedDL);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
