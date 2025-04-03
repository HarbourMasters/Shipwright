#include <libultraship/bridge.h>
#include "draw.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "randomizerTypes.h"
#include <array>
#include "objects/object_gi_key/object_gi_key.h"
#include "objects/object_gi_bosskey/object_gi_bosskey.h"
#include "objects/object_gi_compass/object_gi_compass.h"
#include "objects/object_gi_map/object_gi_map.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
#include "objects/object_gi_scale/object_gi_scale.h"
#include "objects/object_gi_fire/object_gi_fire.h"
#include "objects/object_fish/object_fish.h"
#include "objects/object_toki_objects/object_toki_objects.h"
#include "objects/object_gi_bomb_2/object_gi_bomb_2.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_goma/object_goma.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"
#include "objects/object_bv/object_bv.h"
#include "objects/object_gnd/object_gnd.h"
#include "objects/object_fd/object_fd.h"
#include "objects/object_mo/object_mo.h"
#include "objects/object_sst/object_sst.h"
#include "overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "overlays/ovl_Boss_Sst/ovl_Boss_Sst.h"
#include "objects/object_tw/object_tw.h"
#include "objects/object_ganon2/object_ganon2.h"
#include "soh_assets.h"
#include "dungeon.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"

extern "C" {
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

const char* SmallBodyCvarValue[10] = {
    CVAR_COSMETIC("Key.ForestSmallBody.Value"), CVAR_COSMETIC("Key.FireSmallBody.Value"),
    CVAR_COSMETIC("Key.WaterSmallBody.Value"),  CVAR_COSMETIC("Key.SpiritSmallBody.Value"),
    CVAR_COSMETIC("Key.ShadowSmallBody.Value"), CVAR_COSMETIC("Key.WellSmallBody.Value"),
    CVAR_COSMETIC("Key.GTGSmallBody.Value"),    CVAR_COSMETIC("Key.FortSmallBody.Value"),
    CVAR_COSMETIC("Key.GanonsSmallBody.Value"), CVAR_COSMETIC("Key.ChestGameSmallBody.Value"),
};

const char* SmallEmblemCvarValue[10] = {
    CVAR_COSMETIC("Key.ForestSmallEmblem.Value"), CVAR_COSMETIC("Key.FireSmallEmblem.Value"),
    CVAR_COSMETIC("Key.WaterSmallEmblem.Value"),  CVAR_COSMETIC("Key.SpiritSmallEmblem.Value"),
    CVAR_COSMETIC("Key.ShadowSmallEmblem.Value"), CVAR_COSMETIC("Key.WellSmallEmblem.Value"),
    CVAR_COSMETIC("Key.GTGSmallEmblem.Value"),    CVAR_COSMETIC("Key.FortSmallEmblem.Value"),
    CVAR_COSMETIC("Key.GanonsSmallEmblem.Value"), CVAR_COSMETIC("Key.ChestGameEmblem.Value"),
};

Color_RGB8 SmallEmblemDefaultValue[10] = {
    { 4, 195, 46 },    // Forest
    { 237, 95, 95 },   // Fire
    { 85, 180, 223 },  // Water
    { 222, 158, 47 },  // Spirit
    { 126, 16, 177 },  // Shadow
    { 227, 110, 255 }, // Well
    { 221, 212, 60 },  // GTG
    { 255, 255, 255 }, // Fortress
    { 80, 80, 80 },    // Ganons
    { 255, 255, 255 }, // Chest Game
};

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

extern "C" void Randomizer_DrawSmallKey(PlayState* play, GetItemEntry* getItemEntry) {
    s8 isCustomKeysEnabled = CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("CustomKeyModels"), 1);
    int slot = getItemEntry->drawItemId - RG_FOREST_TEMPLE_SMALL_KEY;

    Gfx* customIconDLs[] = {
        (Gfx*)gSmallKeyIconForestTempleDL,         (Gfx*)gSmallKeyIconFireTempleDL,
        (Gfx*)gSmallKeyIconWaterTempleDL,          (Gfx*)gSmallKeyIconSpiritTempleDL,
        (Gfx*)gSmallKeyIconShadowTempleDL,         (Gfx*)gSmallKeyIconBottomoftheWellDL,
        (Gfx*)gSmallKeyIconGerudoTrainingGroundDL, (Gfx*)gSmallKeyIconGerudoFortressDL,
        (Gfx*)gSmallKeyIconGanonsCastleDL,         (Gfx*)gSmallKeyIconTreasureChestGameDL,
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Color_RGB8 keyColor = { 255, 255, 255 };
    keyColor = CVarGetColor24(SmallBodyCvarValue[slot], keyColor);

    if (isCustomKeysEnabled) {
        gDPSetEnvColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSmallKeyCustomDL);

        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        Color_RGB8 emblemColor = SmallEmblemDefaultValue[slot];
        emblemColor = CVarGetColor24(SmallEmblemCvarValue[slot], emblemColor);

        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gDPSetEnvColor(POLY_XLU_DISP++, emblemColor.r, emblemColor.g, emblemColor.b, 255);

        gSPDisplayList(POLY_XLU_DISP++, customIconDLs[slot]);

    } else {
        gDPSetGrayscaleColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
        gSPGrayscale(POLY_OPA_DISP++, true);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiSmallKeyDL);
        gSPGrayscale(POLY_OPA_DISP++, false);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawMap(PlayState* play, GetItemEntry* getItemEntry) {
    s16 color_slot = getItemEntry->drawItemId - RG_DEKU_TREE_MAP;
    s16 colors[12][3] = {
        { 4, 100, 46 },    // Deku Tree
        { 140, 30, 30 },   // Dodongo's Cavern
        { 30, 60, 255 },   // Jabu Jabu's Belly
        { 4, 195, 46 },    // Forest Temple
        { 237, 95, 95 },   // Fire Temple
        { 85, 180, 223 },  // Water Temple
        { 222, 158, 47 },  // Spirit Temple
        { 126, 16, 177 },  // Shadow Temple
        { 227, 110, 255 }, // Bottom of the Well
        { 0, 255, 255 },   // Ice Cavern
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiDungeonMapDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawCompass(PlayState* play, GetItemEntry* getItemEntry) {
    s16 color_slot = getItemEntry->drawItemId - RG_DEKU_TREE_COMPASS;
    s16 colors[12][3] = {
        { 4, 100, 46 },    // Deku Tree
        { 140, 30, 30 },   // Dodongo's Cavern
        { 30, 60, 255 },   // Jabu Jabu's Belly
        { 4, 195, 46 },    // Forest Temple
        { 237, 95, 95 },   // Fire Temple
        { 85, 180, 223 },  // Water Temple
        { 222, 158, 47 },  // Spirit Temple
        { 126, 16, 177 },  // Shadow Temple
        { 227, 110, 255 }, // Bottom of the Well
        { 221, 212, 60 },  // Gerudo Training Ground
        { 255, 255, 255 }, // Thieves' Hideout
        { 80, 80, 80 }     // Ganon's Castle
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, colors[color_slot][0], colors[color_slot][1], colors[color_slot][2], 255);
    gDPSetEnvColor(POLY_OPA_DISP++, colors[color_slot][0] / 2, colors[color_slot][1] / 2, colors[color_slot][2] / 2,
                   255);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiCompassDL);

    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 5);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiCompassGlassDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawBossKey(PlayState* play, GetItemEntry* getItemEntry) {
    s8 isCustomKeysEnabled = CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("CustomKeyModels"), 1);
    s16 slot = getItemEntry->getItemId - RG_FOREST_TEMPLE_BOSS_KEY;

    std::string CvarValue[6] = {
        "gCosmetics.Key.ForestBoss", "gCosmetics.Key.FireBoss",   "gCosmetics.Key.WaterBoss",
        "gCosmetics.Key.SpiritBoss", "gCosmetics.Key.ShadowBoss", "gCosmetics.Key.GanonsBoss",
    };

    Gfx* CustomdLists[] = {
        (Gfx*)gBossKeyIconForestTempleDL, (Gfx*)gBossKeyIconFireTempleDL,   (Gfx*)gBossKeyIconWaterTempleDL,
        (Gfx*)gBossKeyIconSpiritTempleDL, (Gfx*)gBossKeyIconShadowTempleDL, (Gfx*)gBossKeyIconGanonsCastleDL,
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Color_RGB8 keyColor = { 255, 255, 0 };
    // Supposed to use CVAR_COSMETIC but I can't figure out the syntax
    keyColor = CVarGetColor24((CvarValue[slot] + "Body.Value").c_str(), keyColor);

    if (isCustomKeysEnabled) {
        gDPSetEnvColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gBossKeyCustomDL);
    } else {
        if (CVarGetInteger((CvarValue[slot] + "Body.Changed").c_str(), false)) {
            gDPSetGrayscaleColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
            gSPGrayscale(POLY_OPA_DISP++, true);
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiBossKeyDL);
            gSPGrayscale(POLY_OPA_DISP++, false);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiBossKeyDL);
        }
    }

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Color_RGB8 gemColor = { 255, 0, 0 };
    gemColor = CVarGetColor24((CvarValue[slot] + "Gem.Value").c_str(), gemColor);

    if (isCustomKeysEnabled) {
        gDPSetEnvColor(POLY_XLU_DISP++, gemColor.r, gemColor.g, gemColor.b, 255);
        gSPDisplayList(POLY_XLU_DISP++, CustomdLists[slot]);
    } else {
        if (CVarGetInteger((CvarValue[slot] + "Gem.Changed").c_str(), false)) {
            gDPSetGrayscaleColor(POLY_XLU_DISP++, gemColor.r, gemColor.g, gemColor.b, 255);
            gSPGrayscale(POLY_XLU_DISP++, true);
            gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBossKeyGemDL);
            gSPGrayscale(POLY_XLU_DISP++, false);
        } else {
            gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBossKeyGemDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawKeyRing(PlayState* play, GetItemEntry* getItemEntry) {
    s8 isCustomKeysEnabled = CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("CustomKeyModels"), 1);
    int slot = getItemEntry->drawItemId - RG_FOREST_TEMPLE_KEY_RING;

    Gfx* CustomIconDLs[] = {
        (Gfx*)gKeyringIconForestTempleDL,         (Gfx*)gKeyringIconFireTempleDL,
        (Gfx*)gKeyringIconWaterTempleDL,          (Gfx*)gKeyringIconSpiritTempleDL,
        (Gfx*)gKeyringIconShadowTempleDL,         (Gfx*)gKeyringIconBottomoftheWellDL,
        (Gfx*)gKeyringIconGerudoTrainingGroundDL, (Gfx*)gKeyringIconGerudoFortressDL,
        (Gfx*)gKeyringIconGanonsCastleDL,         (Gfx*)gKeyringIconTreasureChestGameDL,
    };

    Gfx* CustomKeysDLs[] = {
        (Gfx*)gKeyringKeysForestTempleDL,         (Gfx*)gKeyringKeysFireTempleDL,
        (Gfx*)gKeyringKeysWaterTempleDL,          (Gfx*)gKeyringKeysSpiritTempleDL,
        (Gfx*)gKeyringKeysShadowTempleDL,         (Gfx*)gKeyringKeysBottomoftheWellDL,
        (Gfx*)gKeyringKeysGerudoTrainingGroundDL, (Gfx*)gKeyringKeysGerudoFortressDL,
        (Gfx*)gKeyringKeysGanonsCastleDL,         (Gfx*)gKeyringKeysTreasureChestGameDL,
    };

    Gfx* CustomKeysMQDLs[] = {
        (Gfx*)gKeyringKeysForestTempleMQDL,         (Gfx*)gKeyringKeysFireTempleMQDL,
        (Gfx*)gKeyringKeysWaterTempleMQDL,          (Gfx*)gKeyringKeysSpiritTempleMQDL,
        (Gfx*)gKeyringKeysShadowTempleMQDL,         (Gfx*)gKeyringKeysBottomoftheWellMQDL,
        (Gfx*)gKeyringKeysGerudoTrainingGroundMQDL, (Gfx*)gKeyringKeysGerudoFortressDL,
        (Gfx*)gKeyringKeysGanonsCastleMQDL,         (Gfx*)gKeyringKeysTreasureChestGameDL,
    };

    // RANDOTODO make DungeonInfo static and vanilla accessible to allow all these key model data vars to be stored
    // there. (Rando::DungeonKey)0 means the keyring is not tied to a dungeon and should not be checked for an MQ
    // variant
    Rando::DungeonKey SlotToDungeon[10] = {
        Rando::FOREST_TEMPLE, Rando::FIRE_TEMPLE,        Rando::WATER_TEMPLE,           Rando::SPIRIT_TEMPLE,
        Rando::SHADOW_TEMPLE, Rando::BOTTOM_OF_THE_WELL, Rando::GERUDO_TRAINING_GROUND,
        (Rando::DungeonKey)0, // Gerudo Fortress
        Rando::GANONS_CASTLE,
        (Rando::DungeonKey)0, // Treasure Chest Game
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Color_RGB8 keyColor = { 255, 255, 255 };
    keyColor = CVarGetColor24(SmallBodyCvarValue[slot], keyColor);

    if (isCustomKeysEnabled) {
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_MODELVIEW | G_MTX_LOAD);

        gDPSetEnvColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
        if (SlotToDungeon[slot] != 0 && Rando::Context::GetInstance()->GetDungeon(SlotToDungeon[slot])->IsMQ()) {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)CustomKeysMQDLs[slot]);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, (Gfx*)CustomKeysDLs[slot]);
        }

        Color_RGB8 ringColor = { 255, 255, 255 };
        ringColor = CVarGetColor24(CVAR_COSMETIC("Key.KeyringRing.Value"), ringColor);
        gDPSetEnvColor(POLY_OPA_DISP++, ringColor.r, ringColor.g, ringColor.b, 255);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gKeyringRingDL);

        Color_RGB8 emblemColor = SmallEmblemDefaultValue[slot];
        emblemColor = CVarGetColor24(SmallEmblemCvarValue[slot], emblemColor);

        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gDPSetEnvColor(POLY_OPA_DISP++, emblemColor.r, emblemColor.g, emblemColor.b, 255);

        gSPDisplayList(POLY_OPA_DISP++, CustomIconDLs[slot]);
    } else {
        gDPSetGrayscaleColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
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
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawDoubleDefense(PlayState* play, GetItemEntry* getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetGrayscaleColor(POLY_XLU_DISP++, 255, 255, 255, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartBorderDL);

    gSPGrayscale(POLY_XLU_DISP++, false);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiHeartContainerDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawMasterSword(PlayState* play, GetItemEntry* getItemEntry) {
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

    uint8_t current = gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected;

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

    uint8_t current = gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected;
    uint8_t required = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED) + 1;

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

extern "C" void Randomizer_DrawMysteryItem(PlayState* play, GetItemEntry* getItemEntry) {
    Color_RGB8 color = { 0, 60, 100 };
    if (CVarGetInteger(CVAR_COSMETIC("World.MysteryItem.Changed"), 0)) {
        color = CVarGetColor24(CVAR_COSMETIC("World.MysteryItem.Value"), color);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetGrayscaleColor(POLY_XLU_DISP++, color.r, color.g, color.b, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gMysteryItemDL);

    gSPGrayscale(POLY_XLU_DISP++, false);

    CLOSE_DISPS(play->state.gfxCtx);
}

Gfx* GetEmptyDlist(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = dListHead = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx) * 1);

    gSPEndDisplayList(dListHead++);

    return dList;
}

extern "C" s32 OverrideLimbDrawGohma(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 255, 170, 255);

    switch (limbIndex) {
        case BOSSGOMA_LIMB_EYE:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 63);
            break;

        case BOSSGOMA_LIMB_IRIS:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return false;
}

#define LIMB_COUNT_GOHMA 86
extern "C" void DrawGohma(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_GOHMA];
    static Vec3s otherTable[LIMB_COUNT_GOHMA];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(play, &skelAnime, (SkeletonHeader*)&gGohmaSkel, (AnimationHeader*)&gGohmaIdleCrouchedAnim,
                       jointTable, otherTable, LIMB_COUNT_GOHMA);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)GetEmptyDlist(play->state.gfxCtx));
    SkelAnime_DrawSkeletonOpa(play, &skelAnime, OverrideLimbDrawGohma, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_KING_DODONGO 49
extern "C" void DrawKingDodongo(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_KING_DODONGO];
    static Vec3s otherTable[LIMB_COUNT_KING_DODONGO];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(play, &skelAnime, (SkeletonHeader*)&object_kingdodongo_Skel_01B310,
                       (AnimationHeader*)&object_kingdodongo_Anim_00F0D8, jointTable, otherTable,
                       LIMB_COUNT_KING_DODONGO);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, -20.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.003f, 0.003f, 0.003f, MTXMODE_APPLY);

    SkelAnime_DrawSkeletonOpa(play, &skelAnime, NULL, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" s32 OverrideLimbDrawBarinade(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    s16 unk_1AC = play->gameplayFrames * 0xC31;
    f32 unk_1A0 = 0.0f;
    f32 unk_1A4 = 0.0f;

    if (limbIndex == 20) {
        gDPPipeSync(POLY_OPA_DISP++);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 8, 16, 1, 0,
                                               (play->gameplayFrames * -2) % 64, 16, 16));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 200);
        Matrix_RotateX(-M_PI / 2, MTXMODE_APPLY);
    } else if ((limbIndex >= 10) && (limbIndex < 20)) {
        rot->x -= 0x4000;
        *dList = NULL;
    } else if (limbIndex == 6) {
        unk_1A4 = (Math_SinS(unk_1AC) * 0.05f) + 1.0f;
        Matrix_Scale(unk_1A4, unk_1A4, unk_1A4, MTXMODE_APPLY);
    } else if (limbIndex == 61) {
        unk_1A0 = (Math_CosS(unk_1AC) * 0.1f) + 1.0f;
        Matrix_Scale(unk_1A0, unk_1A0, unk_1A0, MTXMODE_APPLY);
    } else if (limbIndex == 7) {
        rot->x -= 0xCCC;
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return false;
}

extern "C" void PostLimbDrawBarinade(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 25) {
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (play->gameplayFrames * 10) % 128, 16, 32, 1,
                                               0, (play->gameplayFrames * 5) % 128, 16, 32));
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gBarinadeDL_008D70);
    } else if ((limbIndex >= 10) && (limbIndex < 20)) {
        if (((limbIndex >= 16) || (limbIndex == 10))) {
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gBarinadeDL_008BB8);
        } else if ((limbIndex >= 11)) {
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gBarinadeDL_008BB8);
        }
    } else if ((*dList != NULL) && (limbIndex >= 29) && (limbIndex < 56)) {
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, *dList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_BARINADE 64
extern "C" void DrawBarinade(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_BARINADE];
    static Vec3s otherTable[LIMB_COUNT_BARINADE];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(play, &skelAnime, (SkeletonHeader*)&gBarinadeBodySkel, (AnimationHeader*)&gBarinadeBodyAnim,
                       jointTable, otherTable, LIMB_COUNT_BARINADE);

        // Freeze barniade on the last frame
        f32 lastFrame = Animation_GetLastFrame((AnimationHeader*)&gBarinadeBodyAnim);
        Animation_Change(&skelAnime, (AnimationHeader*)&gBarinadeBodyAnim, 1.0f, lastFrame, lastFrame, ANIMMODE_ONCE,
                         0.0f);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Matrix_Translate(0.0f, -25.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 8, 16, 1, 0, (play->gameplayFrames * -10) % 16,
                                           16, 16));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (play->gameplayFrames * -10) % 32, 16, 0x20, 1, 0,
                                           (play->gameplayFrames * -5) % 32, 16, 32));

    SkelAnime_DrawSkeletonOpa(play, &skelAnime, OverrideLimbDrawBarinade, PostLimbDrawBarinade, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_PHANTOM_GANON 26
extern "C" void DrawPhantomGanon(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_PHANTOM_GANON];
    static Vec3s otherTable[LIMB_COUNT_PHANTOM_GANON];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(play, &skelAnime, (SkeletonHeader*)&gPhantomGanonSkel,
                       (AnimationHeader*)&gPhantomGanonNeutralAnim, jointTable, otherTable, LIMB_COUNT_PHANTOM_GANON);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, 10.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.007f, 0.007f, 0.007f, MTXMODE_APPLY);

    // Eye color
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)GetEmptyDlist(play->state.gfxCtx));
    SkelAnime_DrawSkeletonOpa(play, &skelAnime, NULL, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_VOLVAGIA 7
extern "C" void DrawVolvagia(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_VOLVAGIA];
    static Vec3s otherTable[LIMB_COUNT_VOLVAGIA];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_Init(play, &skelAnime, (SkeletonHeader*)&gVolvagiaHeadSkel,
                       (AnimationHeader*)&gVolvagiaHeadEmergeAnim, jointTable, otherTable, LIMB_COUNT_VOLVAGIA);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Scale(0.007f, 0.007f, 0.007f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x09, (uintptr_t)gVolvagiaEyeOpenTex);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->state.frames * 4, 120, 0x20, 0x20, 1,
                                           play->state.frames * 3, play->state.frames * -2, 0x20, 0x20));

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

    SkelAnime_DrawSkeletonOpa(play, &skelAnime, NULL, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void DrawMorpha(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    Matrix_Scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->state.frames * 3, play->state.frames * 3, 32,
                                           32, 1, play->state.frames * -3, play->state.frames * -3, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 0x09,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->state.frames * 3, 0, 32, 32, 1, 0,
                                           play->state.frames * -5, 32, 32));

    Matrix_RotateX(play->state.frames * 0.1f, MTXMODE_APPLY);
    Matrix_RotateZ(play->state.frames * 0.16f, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gMorphaCoreMembraneDL);

    gDPPipeSync(POLY_XLU_DISP++);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 220, 255, 128);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gMorphaCoreNucleusDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_BONGO_BONGO 27
extern "C" void DrawBongoBongo(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_BONGO_BONGO];
    static Vec3s otherTable[LIMB_COUNT_BONGO_BONGO];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(play, &skelAnime, (FlexSkeletonHeader*)&gBongoLeftHandSkel,
                           (AnimationHeader*)&gBongoLeftHandIdleAnim, jointTable, otherTable, LIMB_COUNT_BONGO_BONGO);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, -25.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.006f, 0.006f, 0.006f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)GetEmptyDlist(play->state.gfxCtx));

    gDPSetPrimColor(POLY_OPA_DISP++, 0x80, 0x80, 255, 255, 255, 255);
    SkelAnime_DrawSkeletonOpa(play, &skelAnime, NULL, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" s32 OverrideLimbDrawKotake(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                      void* thisx) {
    if (limbIndex == 21) { // Head
        *dList = (Gfx*)gTwinrovaKotakeHeadDL;
    }

    return false;
}

extern "C" void PostLimbDrawKotake(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 21) { // Head
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gTwinrovaKotakeIceHairDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#define LIMB_COUNT_KOTAKE 27
extern "C" void DrawKotake(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_KOTAKE];
    static Vec3s otherTable[LIMB_COUNT_KOTAKE];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(play, &skelAnime, (FlexSkeletonHeader*)&gTwinrovaKotakeSkel,
                           (AnimationHeader*)&gTwinrovaKotakeKoumeFlyAnim, jointTable, otherTable, LIMB_COUNT_KOTAKE);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Matrix_Translate(0.0f, -10.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 10, (uintptr_t)gTwinrovaKotakeKoumeEyeOpenTex);
    gSPSegment(POLY_XLU_DISP++, 10, (uintptr_t)gTwinrovaKotakeKoumeEyeOpenTex);
    gSPSegment(POLY_XLU_DISP++, 8,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0 & 0x7F, 0 & 0x7F, 0x20, 0x20, 1,
                                           play->state.frames & 0x7F, (play->state.frames * -7) & 0xFF, 0x20, 0x40));

    gSPSegment(POLY_XLU_DISP++, 9,
               (uintptr_t)Gfx_TexScroll(play->state.gfxCtx, 0 & 0x7F, play->state.frames & 0xFF, 0x20, 0x40));

    SkelAnime_DrawSkeletonOpa(play, &skelAnime, OverrideLimbDrawKotake, PostLimbDrawKotake, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" s32 OverrideLimbDrawGanon(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex >= 42) { // Tail
        // Brighten up tail
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return false;
}

#define LIMB_COUNT_GANON 47
extern "C" void DrawGanon(PlayState* play) {
    static bool initialized = false;
    static SkelAnime skelAnime;
    static Vec3s jointTable[LIMB_COUNT_GANON];
    static Vec3s otherTable[LIMB_COUNT_GANON];
    static u32 lastUpdate = 0;

    if (!initialized) {
        initialized = true;
        SkelAnime_InitFlex(play, &skelAnime, (FlexSkeletonHeader*)&gGanonSkel, (AnimationHeader*)&gGanonGuardIdleAnim,
                           jointTable, otherTable, LIMB_COUNT_GANON);
    }

    if (lastUpdate != play->state.frames) {
        lastUpdate = play->state.frames;
        SkelAnime_Update(&skelAnime);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, -33.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(0.005f, 0.005f, 0.005f, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gGanonEyeOpenTex);

    SkelAnime_DrawSkeletonOpa(play, &skelAnime, OverrideLimbDrawGanon, NULL, NULL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawBossSoul(PlayState* play, GetItemEntry* getItemEntry) {
    s16 slot;
    if (getItemEntry->getItemId != RG_ICE_TRAP) {
        slot = getItemEntry->getItemId - RG_GOHMA_SOUL;
    } else {
        slot = getItemEntry->drawItemId - RG_GOHMA_SOUL;
    }

    s16 flameColors[9][3] = {
        { 0, 255, 0 },     // Gohma
        { 255, 0, 100 },   // King Dodongo
        { 50, 255, 255 },  // Barinade
        { 4, 195, 46 },    // Phantom Ganon
        { 237, 95, 95 },   // Volvagia
        { 85, 180, 223 },  // Morpha
        { 126, 16, 177 },  // Bongo Bongo
        { 222, 158, 47 },  // Twinrova
        { 150, 150, 150 }, // Ganon/Dorf
    };

    // Draw the blue fire DL but coloured to the boss soul.
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 8,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0 * (play->state.frames * 0),
                                           0 * (play->state.frames * 0), 16, 32, 1, 1 * (play->state.frames * 1),
                                           -1 * (play->state.frames * 8), 16, 32));
    Matrix_Push();
    Matrix_Translate(0.0f, -70.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(5.0f, 5.0f, 5.0f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gDPSetGrayscaleColor(POLY_XLU_DISP++, flameColors[slot][0], flameColors[slot][1], flameColors[slot][2], 255);
    gSPGrayscale(POLY_XLU_DISP++, true);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBlueFireFlameDL);
    gSPGrayscale(POLY_XLU_DISP++, false);
    Matrix_Pop();
    CLOSE_DISPS(play->state.gfxCtx);

    // Draw the generic boss soul model
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("SimplerBossSoulModels"), 0)) {
        OPEN_DISPS(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        if (slot == 8) { // For Ganon only...
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 255);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
        }
        gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gBossSoulSkullDL);
        CLOSE_DISPS(play->state.gfxCtx);
        // Draw the boss' skeleton
    } else {
        switch (slot) {
            case 0: // Gohma
                DrawGohma(play);
                break;
            case 1: // King Dodongo
                DrawKingDodongo(play);
                break;
            case 2: // Barinade
                DrawBarinade(play);
                break;
            case 3: // Phantom Ganon
                DrawPhantomGanon(play);
                break;
            case 4: // Volvagia
                DrawVolvagia(play);
                break;
            case 5: // Morpha
                DrawMorpha(play);
                break;
            case 6: // Bongo Bongo
                DrawBongoBongo(play);
                break;
            case 7: // Twinrova
                DrawKotake(play);
                break;
            case 8: // Ganon
                DrawGanon(play);
                break;
            default:
                break;
        }
    }
}

extern "C" void Randomizer_DrawOcarinaButton(PlayState* play, GetItemEntry* getItemEntry) {
    Color_RGB8 aButtonColor = { 80, 150, 255 };
    if (CVarGetInteger(CVAR_COSMETIC("HUD.AButton.Changed"), 0)) {
        aButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.AButton.Value"), aButtonColor);
    } else if (CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        aButtonColor = { 80, 255, 150 };
    }

    Color_RGB8 cButtonsColor = { 255, 255, 50 };
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CButtons.Changed"), 0)) {
        cButtonsColor = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), cButtonsColor);
    }
    Color_RGB8 cUpButtonColor = cButtonsColor;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CUpButton.Changed"), 0)) {
        cUpButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CUpButton.Value"), cUpButtonColor);
    }
    Color_RGB8 cDownButtonColor = cButtonsColor;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CDownButton.Changed"), 0)) {
        cDownButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CDownButton.Value"), cDownButtonColor);
    }
    Color_RGB8 cLeftButtonColor = cButtonsColor;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CLeftButton.Changed"), 0)) {
        cLeftButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CLeftButton.Value"), cLeftButtonColor);
    }
    Color_RGB8 cRightButtonColor = cButtonsColor;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CRightButton.Changed"), 0)) {
        cRightButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CRightButton.Value"), cRightButtonColor);
    }

    s16 slot = getItemEntry->drawItemId - RG_OCARINA_A_BUTTON;

    Gfx* dLists[] = {
        (Gfx*)gOcarinaAButtonDL,     (Gfx*)gOcarinaCUpButtonDL,    (Gfx*)gOcarinaCDownButtonDL,
        (Gfx*)gOcarinaCLeftButtonDL, (Gfx*)gOcarinaCRightButtonDL,
    };

    Color_RGB8 colors[] = {
        aButtonColor, cUpButtonColor, cDownButtonColor, cLeftButtonColor, cRightButtonColor,
    };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gDPSetGrayscaleColor(POLY_XLU_DISP++, colors[slot].r, colors[slot].g, colors[slot].b, 255);
    gSPGrayscale(POLY_XLU_DISP++, true);

    gSPDisplayList(POLY_XLU_DISP++, dLists[slot]);

    gSPGrayscale(POLY_XLU_DISP++, false);

    CLOSE_DISPS(play->state.gfxCtx);
}

static Gfx gGiBronzeScaleWaterColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0x60, 255, 255, 255, 255),
    gsDPSetEnvColor(255, 123, 0, 255),
    gsSPEndDisplayList(),
};

static Gfx gGiBronzeScaleColorDL[] = {
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0x80, 255, 255, 255, 255),
    gsDPSetEnvColor(91, 51, 18, 255),
    gsSPEndDisplayList(),
};

extern "C" void Randomizer_DrawBronzeScale(PlayState* play, GetItemEntry* getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               (uintptr_t)Gfx_TwoTexScroll(play->state.gfxCtx, 0, 1 * (play->state.frames * 2),
                                           -1 * (play->state.frames * 2), 64, 64, 1, 1 * (play->state.frames * 4),
                                           1 * -(play->state.frames * 4), 32, 32));

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBronzeScaleColorDL);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiScaleDL);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiBronzeScaleWaterColorDL);
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gGiScaleWaterDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawFishingPoleGI(PlayState* play, GetItemEntry* getItemEntry) {
    Vec3f pos;
    OPEN_DISPS(play->state.gfxCtx);

    // Draw rod
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_Scale(0.2, 0.2, 0.2, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingPoleGiDL);

    // Draw lure
    Matrix_Push();
    Matrix_Scale(5.0f, 5.0f, 5.0f, MTXMODE_APPLY);
    pos = { 0.0f, -25.5f, -4.0f };
    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_APPLY);
    Matrix_RotateZ(-M_PI_2, MTXMODE_APPLY);
    Matrix_RotateY(-M_PI_2 - 0.2f, MTXMODE_APPLY);
    Matrix_Scale(0.006f, 0.006f, 0.006f, MTXMODE_APPLY);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingLureFloatDL);

    // Draw hooks
    Matrix_RotateY(0.2f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -300.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingLureHookDL);
    Matrix_RotateZ(M_PI_2, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingLureHookDL);

    Matrix_Translate(0.0f, -2200.0f, 700.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingLureHookDL);
    Matrix_RotateZ(M_PI / 2, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gFishingLureHookDL);

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawSkeletonKey(PlayState* play, GetItemEntry* getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Color_RGB8 keyColor = { 255, 255, 170 };
    keyColor = CVarGetColor24(CVAR_COSMETIC("Key.Skeleton.Value"), keyColor);
    gDPSetEnvColor(POLY_OPA_DISP++, keyColor.r, keyColor.g, keyColor.b, 255);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSkeletonKeyDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawBombchuBag(PlayState* play, GetItemEntry* getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_26Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    Color_RGB8 maskColor = { 0, 100, 150 };
    maskColor = CVarGetColor24(CVAR_COSMETIC("Equipment.ChuFace.Value"), maskColor);
    gDPSetEnvColor(POLY_OPA_DISP++, maskColor.r, maskColor.g, maskColor.b, 255);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gBombchuBagMaskDL);

    Color_RGB8 bodyColor = { 180, 130, 50 };
    bodyColor = CVarGetColor24(CVAR_COSMETIC("Equipment.ChuBody.Value"), bodyColor);
    gDPSetEnvColor(POLY_OPA_DISP++, bodyColor.r, bodyColor.g, bodyColor.b, 255);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gBombchuBagBodyDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void Randomizer_DrawBombchuBagInLogic(PlayState* play, GetItemEntry* getItemEntry) {
    if (IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOMBCHU_BAG)) {
        Randomizer_DrawBombchuBag(play, getItemEntry);
    } else {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_26Opa(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gGiBombchuDL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

extern "C" void Randomizer_DrawOverworldKey(PlayState* play, GetItemEntry* getItemEntry) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
              G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gHouseKeyDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
