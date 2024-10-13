#include "global.h"
#include "vt.h"
#include "soh/SceneDB.h"
#include "textures/parameter_static/parameter_static.h"

typedef struct {
    /* 0x00 */ void* texture;
    /* 0x04 */ u32 imageFormat;
    /* 0x08 */ u32 imageSize;
    /* 0x0C */ u32 textureWidth;
    /* 0x10 */ u32 textureHeight;
    /* 0x14 */ u32 rectWidth;
    /* 0x18 */ u32 rectHeight;
    /* 0x1C */ u32 dsdx;
    /* 0x20 */ u32 dtdy;
} MapMarkInfo; // size = 0x24

typedef struct {
    /* 0x00 */ void* loadedRamAddr; // original name: "allocp"
    /* 0x04 */ uintptr_t vromStart;
    /* 0x08 */ uintptr_t vromEnd;
    /* 0x0C */ void* vramStart;
    /* 0x10 */ void* vramEnd;
    /* 0x14 */ void* vramTable;
} MapMarkDataOverlay; // size = 0x18

static u32 sBaseImageSizes[] = { 0, 1, 2, 3 };
static u32 sLoadBlockImageSizes[] = { 2, 2, 2, 3 };
static u32 sIncrImageSizes[] = { 3, 1, 0, 0 };
static u32 sShiftImageSizes[] = { 2, 1, 0, 0 };
static u32 sBytesImageSizes[] = { 0, 1, 2, 4 };
static u32 sLineBytesImageSizes[] = { 0, 1, 2, 2 };

#define G_IM_SIZ_MARK sBaseImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_LOAD_BLOCK sLoadBlockImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_INCR sIncrImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_SHIFT sShiftImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_BYTES sBytesImageSizes[markInfo->imageSize]
#define G_IM_SIZ_MARK_LINE_BYTES sLineBytesImageSizes[markInfo->imageSize]

static MapMarkInfo sMapMarkInfoTable[] = {
    { gMapChestIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 8, 32, 32, 1 << 10, 1 << 10 }, // Chest Icon
    { gMapBossIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 32, 32, 1 << 10, 1 << 10 },     // Boss Skull Icon
};

//static MapMarkDataOverlay sMapMarkDataOvl = {
//    NULL,
//    //(uintptr_t)_ovl_map_mark_dataSegmentRomStart,
//    //(uintptr_t)_ovl_map_mark_dataSegmentRomEnd,
//    //_ovl_map_mark_dataSegmentStart,
//    //_ovl_map_mark_dataSegmentEnd,
//    0, 0, 0, 0,
//    gMapMarkDataTableVanilla,
//};

void MapMark_Init(PlayState* play) {
    SceneDB_SetMapMarkData(play->sceneNum, ResourceMgr_IsGameMasterQuest());
}

void MapMark_ClearPointers(PlayState* play) {
}

void MapMark_DrawForDungeon(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    MapMarkPoint* markPoint;
    MapMarkInfo* markInfo;
    u16 dungeon = gSaveContext.mapIndex;
    s32 i;
    s32 rectLeft;
    s32 rectTop;
    SceneDBEntry* entry = SceneDB_Retrieve(dungeon);
    SceneDBRoom* room = &entry->dungeonData.rooms[interfaceCtx->mapRoomNum];

    s32 Top_MC_Margin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.T"), 0);
    s32 Left_MC_Margin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.L"), 0);
    s32 Right_MC_Margin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.R"), 0);
    s32 Bottom_MC_Margin = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.B"), 0);

    s32 X_Margins_Minimap_ic;
    s32 Y_Margins_Minimap_ic;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.UseMargins"), 0) != 0) {
        if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 0) { X_Margins_Minimap_ic = Right_MC_Margin; };
        Y_Margins_Minimap_ic = Bottom_MC_Margin;
    } else {
        X_Margins_Minimap_ic = 0;
        Y_Margins_Minimap_ic = 0;
    }

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetTextureLUT(OVERLAY_DISP++, G_TT_NONE);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->minimapAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, interfaceCtx->minimapAlpha);

    // TODO condense
    for (i = 0; i < room->numChestMarks; i++) {
        markPoint = &room->chestMarks[i];
        if (!Flags_GetTreasure(play, markPoint->chestFlag)) {
            markInfo = &sMapMarkInfoTable[MAP_MARK_CHEST];
            int height = markInfo->textureHeight * 1.0f; //Adjust Height with scale
            int width = markInfo->textureWidth * 1.0f; //Adjust Width with scale
            int height_factor = (1 << 10) * markInfo->textureHeight / height;
            int width_factor = (1 << 10) * markInfo->textureWidth / width;

            // The original mark point X originates from the left edge of the map
            // For mirror mode, we compute the new mark point X by subtracting it from the right side of the
            // dungeon map and the textures width
            s16 markPointX = CVarGetInteger("gMirroredWorld", 0) ? 96 - markPoint->x - width : markPoint->x;

            //Minimap chest / boss icon
            const s32 PosX_Minimap_ori = GREG(94) + OTRGetRectDimensionFromRightEdge(markPointX + X_Margins_Minimap_ic) + 204;
            const s32 PosY_Minimap_ori = GREG(95) + markPoint->y + Y_Margins_Minimap_ic + 140;
            if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                rectTop = (markPoint->y + Y_Margins_Minimap_ic + 140 + CVarGetInteger("gMinimapPosY", 0));
                if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                    if (CVarGetInteger("gMinimapUseMargins", 0) != 0) { X_Margins_Minimap_ic = Left_MC_Margin; };
                    rectLeft = OTRGetRectDimensionFromLeftEdge(markPointX + CVarGetInteger("gMinimapPosX", 0) + 204 + X_Margins_Minimap_ic);
                } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                    if (CVarGetInteger("gMinimapUseMargins", 0) != 0) { X_Margins_Minimap_ic = Right_MC_Margin; };
                    rectLeft = OTRGetRectDimensionFromRightEdge(markPointX + CVarGetInteger("gMinimapPosX", 0) + 204 + X_Margins_Minimap_ic);
                } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                    rectLeft = markPointX + CVarGetInteger("gMinimapPosX", 0) + 204 + X_Margins_Minimap_ic;
                } else if (CVarGetInteger("gMinimapPosType", 0) == 4) {//Hidden
                    rectLeft = -9999;
                }
            } else {
                rectLeft = PosX_Minimap_ori;
                rectTop = PosY_Minimap_ori;
            }

            gDPPipeSync(OVERLAY_DISP++);

            gDPLoadTextureBlock(OVERLAY_DISP++, markInfo->texture, markInfo->imageFormat, G_IM_SIZ_MARK,
                markInfo->textureWidth, markInfo->textureHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            //Changed to a Wide texture to support Left anchor.
            gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, rectLeft + width << 2,
                rectTop + height << 2, G_TX_RENDERTILE, 0, 0, width_factor,
                height_factor);
        }
    }

    for (i = 0; i < room->numBossMarks; i++) {
        markPoint = &room->bossMarks[i];

        markInfo = &sMapMarkInfoTable[MAP_MARK_BOSS];
        int height = markInfo->textureHeight * 1.0f; //Adjust Height with scale
        int width = markInfo->textureWidth * 1.0f; //Adjust Width with scale
        int height_factor = (1 << 10) * markInfo->textureHeight / height;
        int width_factor = (1 << 10) * markInfo->textureWidth / width;

        // The original mark point X originates from the left edge of the map
        // For mirror mode, we compute the new mark point X by subtracting it from the right side of the
        // dungeon map and the textures width
        s16 markPointX = CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? 96 - markPoint->x - width : markPoint->x;

        //Minimap chest / boss icon
        const s32 PosX_Minimap_ori = GREG(94) + OTRGetRectDimensionFromRightEdge(markPointX + X_Margins_Minimap_ic) + 204;
        const s32 PosY_Minimap_ori = GREG(95) + markPoint->y + Y_Margins_Minimap_ic + 140;
        if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) != 0) {
            rectTop = (markPoint->y + Y_Margins_Minimap_ic + 140 + CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosY"), 0));
            if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 1) {//Anchor Left
                if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.UseMargins"), 0) != 0) { X_Margins_Minimap_ic = Left_MC_Margin; };
                rectLeft = OTRGetRectDimensionFromLeftEdge(markPointX + CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0) + 204 + X_Margins_Minimap_ic);
            } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 2) {//Anchor Right
                if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.UseMargins"), 0) != 0) { X_Margins_Minimap_ic = Right_MC_Margin; };
                rectLeft = OTRGetRectDimensionFromRightEdge(markPointX + CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0) + 204 + X_Margins_Minimap_ic);
            } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 3) {//Anchor None
                rectLeft = markPointX + CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0) + 204 + X_Margins_Minimap_ic;
            } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 4) {//Hidden
                rectLeft = -9999;
            }
        } else {
            rectLeft = PosX_Minimap_ori;
            rectTop = PosY_Minimap_ori;
        }

        gDPPipeSync(OVERLAY_DISP++);

        gDPLoadTextureBlock(OVERLAY_DISP++, markInfo->texture, markInfo->imageFormat, G_IM_SIZ_MARK,
            markInfo->textureWidth, markInfo->textureHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        //Changed to a Wide texture to support Left anchor.
        gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, rectLeft + width << 2,
            rectTop + height << 2, G_TX_RENDERTILE, 0, 0, width_factor,
            height_factor);
    }

    /*
    while (true) {
       if (mapMarkIconData->markType == MAP_MARK_NONE) {
            break;
        }

        markPoint = &mapMarkIconData->points[0];
        //Place each chest / boss room icon
        for (i = 0; i < mapMarkIconData->count; i++) {
            if ((mapMarkIconData->markType != MAP_MARK_CHEST) || !Flags_GetTreasure(play, markPoint->chestFlag)) {
                markInfo = &sMapMarkInfoTable[mapMarkIconData->markType];
                int height = markInfo->textureHeight * 1.0f; //Adjust Height with scale
                int width = markInfo->textureWidth * 1.0f; //Adjust Width with scale
                int height_factor = (1 << 10) * markInfo->textureHeight / height;
                int width_factor = (1 << 10) * markInfo->textureWidth / width;

                // The original mark point X originates from the left edge of the map
                // For mirror mode, we compute the new mark point X by subtracting it from the right side of the
                // dungeon map and the textures width
                s16 markPointX = CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0) ? 96 - markPoint->x - width : markPoint->x;

                //Minimap chest / boss icon 
                const s32 PosX_Minimap_ori = GREG(94) + OTRGetRectDimensionFromRightEdge(markPointX+X_Margins_Minimap_ic) + 204;
                const s32 PosY_Minimap_ori = GREG(95) + markPoint->y + Y_Margins_Minimap_ic + 140;
                if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) != 0) {
                    rectTop = (markPoint->y + Y_Margins_Minimap_ic + 140 + CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosY"), 0));
                    if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 1) {//Anchor Left
                        if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.UseMargins"), 0) != 0) {X_Margins_Minimap_ic = Left_MC_Margin;};
                        if (play->sceneNum == SCENE_DEKU_TREE || play->sceneNum == SCENE_DODONGOS_CAVERN || play->sceneNum == SCENE_JABU_JABU || 
                            play->sceneNum == SCENE_FOREST_TEMPLE || play->sceneNum == SCENE_FIRE_TEMPLE || play->sceneNum == SCENE_WATER_TEMPLE || 
                            play->sceneNum == SCENE_SPIRIT_TEMPLE || play->sceneNum == SCENE_SHADOW_TEMPLE || play->sceneNum == SCENE_BOTTOM_OF_THE_WELL || 
                            play->sceneNum == SCENE_ICE_CAVERN) {
                            rectLeft = OTRGetRectDimensionFromLeftEdge(markPointX+CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0)+204+X_Margins_Minimap_ic);
                        } else {
                            rectLeft = OTRGetRectDimensionFromLeftEdge(markPointX+CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0)+204+X_Margins_Minimap_ic);
                        }
                    } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 2) {//Anchor Right
                        if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.UseMargins"), 0) != 0) {X_Margins_Minimap_ic = Right_MC_Margin;};
                        rectLeft = OTRGetRectDimensionFromRightEdge(markPointX+CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0)+204+X_Margins_Minimap_ic);
                    } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 3) {//Anchor None
                        rectLeft = markPointX+CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosX"), 0)+204+X_Margins_Minimap_ic;
                    } else if (CVarGetInteger(CVAR_COSMETIC("HUD.Minimap.PosType"), 0) == 4) {//Hidden
                        rectLeft = -9999;
                    }
                } else {
                    rectLeft = PosX_Minimap_ori;
                    rectTop = PosY_Minimap_ori;
                }

                gDPPipeSync(OVERLAY_DISP++);

                gDPLoadTextureBlock(OVERLAY_DISP++, markInfo->texture, markInfo->imageFormat, G_IM_SIZ_MARK,
                                    markInfo->textureWidth, markInfo->textureHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                //Changed to a Wide texture to support Left anchor.
                gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, rectLeft + width  << 2,
                                    rectTop + height  << 2, G_TX_RENDERTILE, 0, 0, width_factor,
                                    height_factor);
            }

            markPoint++;
        }
        mapMarkIconData++;
    }
    */

    CLOSE_DISPS(play->state.gfxCtx);
}

void MapMark_Draw(PlayState* play) {
    MapMark_DrawForDungeon(play);
}
