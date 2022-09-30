#include "global.h"
#include "vt.h"
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

MapMarkData** sLoadedMarkDataTable;

void MapMark_Init(GlobalContext* globalCtx) {
    //MapMarkDataOverlay* overlay = &sMapMarkDataOvl;
    //u32 overlaySize = (uintptr_t)overlay->vramEnd - (uintptr_t)overlay->vramStart;

    //overlay->loadedRamAddr = GAMESTATE_ALLOC_MC(&globalCtx->state, overlaySize);
    //LOG_CHECK_NULL_POINTER("dlftbl->allocp", overlay->loadedRamAddr);

    //Overlay_Load(overlay->vromStart, overlay->vromEnd, overlay->vramStart, overlay->vramEnd, overlay->loadedRamAddr);

    if(ResourceMgr_IsGameMasterQuest()) {
        sLoadedMarkDataTable = gMapMarkDataTableMq;
    } else {
        sLoadedMarkDataTable = gMapMarkDataTableVanilla;
    }
    //sLoadedMarkDataTable = gMapMarkDataTableVanilla;
    //sLoadedMarkDataTable = (void*)(uintptr_t)(
        //(overlay->vramTable != NULL)
            //? (void*)((uintptr_t)overlay->vramTable - ((intptr_t)overlay->vramStart - (intptr_t)overlay->loadedRamAddr))
            //: NULL);
}

void MapMark_ClearPointers(GlobalContext* globalCtx) {
    //sMapMarkDataOvl.loadedRamAddr = NULL;
    sLoadedMarkDataTable = NULL;
}

void MapMark_DrawForDungeon(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx;
    MapMarkIconData* mapMarkIconData;
    MapMarkPoint* markPoint;
    MapMarkInfo* markInfo;
    u16 dungeon = gSaveContext.mapIndex;
    s32 i;
    s32 rectLeft;
    s32 rectTop;

    interfaceCtx = &globalCtx->interfaceCtx;

    if ((gMapData != NULL) && (globalCtx->interfaceCtx.mapRoomNum >= gMapData->dgnMinimapCount[dungeon])) {
        // "Room number exceeded, yikes %d/%d  MapMarkDraw processing interrupted"
        osSyncPrintf(VT_COL(RED, WHITE) "部屋番号がオーバーしてるで,ヤバイで %d/%d  \nMapMarkDraw の処理を中断します\n",
                     VT_RST, globalCtx->interfaceCtx.mapRoomNum, gMapData->dgnMinimapCount[dungeon]);
        return;
    }

    mapMarkIconData = &sLoadedMarkDataTable[dungeon][interfaceCtx->mapRoomNum][0];

    OPEN_DISPS(globalCtx->state.gfxCtx);

    while (true) {
       if (mapMarkIconData->markType == MAP_MARK_NONE) {
            break;
        }

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetTextureLUT(OVERLAY_DISP++, G_TT_NONE);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->minimapAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, interfaceCtx->minimapAlpha);

        s32 Top_MC_Margin = CVar_GetS32("gHUDMargin_T", 0);
        s32 Left_MC_Margin = CVar_GetS32("gHUDMargin_L", 0);
        s32 Right_MC_Margin = CVar_GetS32("gHUDMargin_R", 0);
        s32 Bottom_MC_Margin = CVar_GetS32("gHUDMargin_B", 0);

        s32 X_Margins_Minimap_ic;
        s32 Y_Margins_Minimap_ic;
        if (CVar_GetS32("gMinimapUseMargins", 0) != 0) {
            if (CVar_GetS32("gMinimapPosType", 0) == 0) {X_Margins_Minimap_ic = Right_MC_Margin;};
            Y_Margins_Minimap_ic = Bottom_MC_Margin;
        } else {
            X_Margins_Minimap_ic = 0;
            Y_Margins_Minimap_ic = 0;
        }

        markPoint = &mapMarkIconData->points[0];
        //Place each chest / boss room icon
        for (i = 0; i < mapMarkIconData->count; i++) {
            if ((mapMarkIconData->markType != MAP_MARK_CHEST) || !Flags_GetTreasure(globalCtx, markPoint->chestFlag)) {
                //Minimap chest / boss icon 
                const s32 PosX_Minimap_ori = GREG(94) + OTRGetRectDimensionFromRightEdge(markPoint->x+X_Margins_Minimap_ic) + 204;
                const s32 PosY_Minimap_ori = GREG(95) + markPoint->y + Y_Margins_Minimap_ic + 140;
                if (CVar_GetS32("gMinimapPosType", 0) != 0) {
                    rectTop = (markPoint->y + Y_Margins_Minimap_ic + 140 + CVar_GetS32("gMinimapPosY", 0));
                    if (CVar_GetS32("gMinimapPosType", 0) == 1) {//Anchor Left
                        if (CVar_GetS32("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap_ic = Left_MC_Margin;};
                        if (globalCtx->sceneNum == SCENE_YDAN || globalCtx->sceneNum == SCENE_DDAN || globalCtx->sceneNum == SCENE_BDAN || 
                            globalCtx->sceneNum == SCENE_BMORI1 || globalCtx->sceneNum == SCENE_HIDAN || globalCtx->sceneNum == SCENE_MIZUSIN || 
                            globalCtx->sceneNum == SCENE_JYASINZOU || globalCtx->sceneNum == SCENE_HAKADAN || globalCtx->sceneNum == SCENE_HAKADANCH || 
                            globalCtx->sceneNum == SCENE_ICE_DOUKUTO) {
                            rectLeft = OTRGetRectDimensionFromLeftEdge(markPoint->x+CVar_GetS32("gMinimapPosX", 0)+204+X_Margins_Minimap_ic);
                        } else {
                            rectLeft = OTRGetRectDimensionFromLeftEdge(markPoint->x+CVar_GetS32("gMinimapPosX", 0)+204+X_Margins_Minimap_ic);
                        }
                    } else if (CVar_GetS32("gMinimapPosType", 0) == 2) {//Anchor Right
                        if (CVar_GetS32("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap_ic = Right_MC_Margin;};
                        rectLeft = OTRGetRectDimensionFromRightEdge(markPoint->x+CVar_GetS32("gMinimapPosX", 0)+204+X_Margins_Minimap_ic);
                    } else if (CVar_GetS32("gMinimapPosType", 0) == 3) {//Anchor None
                        rectLeft = markPoint->x+CVar_GetS32("gMinimapPosX", 0)+204+X_Margins_Minimap_ic;
                    } else if (CVar_GetS32("gMinimapPosType", 0) == 4) {//Hidden
                        rectLeft = -9999;
                    }
                } else {
                    rectLeft = PosX_Minimap_ori;
                    rectTop = PosY_Minimap_ori;
                }

                int height = 8 * 1.0f; //Adjust Height with scale
                int width = 8 * 1.0f; //Adjust Width with scale
                int height_factor = (1 << 10) * 8 / height;
                int width_factor = (1 << 10) * 8 / width;

                markInfo = &sMapMarkInfoTable[mapMarkIconData->markType];

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

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void MapMark_Draw(GlobalContext* globalCtx) {
    switch (globalCtx->sceneNum) {
        case SCENE_YDAN:
        case SCENE_DDAN:
        case SCENE_BDAN:
        case SCENE_BMORI1:
        case SCENE_HIDAN:
        case SCENE_MIZUSIN:
        case SCENE_JYASINZOU:
        case SCENE_HAKADAN:
        case SCENE_HAKADANCH:
        case SCENE_ICE_DOUKUTO:
        case SCENE_YDAN_BOSS:
        case SCENE_DDAN_BOSS:
        case SCENE_BDAN_BOSS:
        case SCENE_MORIBOSSROOM:
        case SCENE_FIRE_BS:
            MapMark_DrawForDungeon(globalCtx);
            break;
    }
}
