#include "global.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/map_i_static/map_i_static.h"
#include "textures/map_grand_static/map_grand_static.h"

MapData* gMapData;

s16 sPlayerInitialPosX = 0;
s16 sPlayerInitialPosZ = 0;
s16 sPlayerInitialDirection = 0;
s16 sEntranceIconMapIndex = 0;

void Map_SavePlayerInitialInfo(GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    sPlayerInitialPosX = player->actor.world.pos.x;
    sPlayerInitialPosZ = player->actor.world.pos.z;
    sPlayerInitialDirection = (s16)((0x7FFF - player->actor.shape.rot.y) / 0x400);
}

void Map_SetPaletteData(GlobalContext* globalCtx, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 paletteIndex = gMapData->roomPalette[mapIndex][room];

    if (interfaceCtx->mapRoomNum == room) {
        interfaceCtx->mapPaletteIndex = paletteIndex;
    }

    osSyncPrintf(VT_FGCOL(YELLOW));
    // "PALETE Set"
    osSyncPrintf("ＰＡＬＥＴＥセット 【 i=%x : room=%x 】Room_Inf[%d][4]=%x  ( map_palete_no = %d )\n", paletteIndex,
                 room, mapIndex, gSaveContext.sceneFlags[mapIndex].rooms, interfaceCtx->mapPaletteIndex);
    osSyncPrintf(VT_RST);

    interfaceCtx->mapPalette[paletteIndex * 2] = 2;
    interfaceCtx->mapPalette[paletteIndex * 2 + 1] = 0xBF;
}

void Map_SetFloorPalettesData(GlobalContext* globalCtx, s16 floor) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 room;
    s16 i;

    for (i = 0; i < 16; i++) {
        interfaceCtx->mapPalette[i] = 0;
        interfaceCtx->mapPalette[i + 16] = 0;
    }

    if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
        interfaceCtx->mapPalette[30] = 0;
        interfaceCtx->mapPalette[31] = 1;
    }

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
        case SCENE_MIZUSIN_BS:
        case SCENE_JYASINBOSS:
        case SCENE_HAKADAN_BS:
            for (i = 0; i < gMapData->maxPaletteCount[mapIndex]; i++) {
                room = gMapData->paletteRoom[mapIndex][floor][i];
                if ((room != 0xFF) && (gSaveContext.sceneFlags[mapIndex].rooms & gBitFlags[room])) {
                    Map_SetPaletteData(globalCtx, room);
                }
            }
            break;
    }
}

const char* minimapTableOW[] =
{
    gHyruleFieldMinimapTex,
    gKakarikoVillageMinimapTex,
    gGraveyardMinimapTex,
    gZorasRiverMinimapTex,
    gKokiriForestMinimapTex,
    gSacredMeadowMinimapTex,
    gLakeHyliaMinimapTex,
    gZorasDomainMinimapTex,
    gZorasFountainMinimapTex,
    gGerudoValleyMinimapTex,
    gHauntedWastelandMinimapTex,
    gDesertColossusMinimapTex,
    gGerudosFortessMinimapTex,
    gLostWoodsMinimapTex,
    gHyruleCastleAreaMinimapTex,
    gDeathMountainTrailMinimapTex,
    gDeathMountainCraterMinimapTex,
    gGoronCityMinimapTex,
    gLonLonRanchMinimapTex,
    gOutsideGanonsCastleMinimapTex,
    gExploredShadowGraveyardMinimapTex,
    gDrainedLakeHyliaMinimapTex,
    gGerudoValleyWithBrokenBridgeMinimapTex,
    gGerudosFortressMinimapTex,
};

const char* minimapTableDangeon[] =
{
    gDekuTreeRoom0Floor1MinimapTex,
    gDekuTreeRoom1MinimapTex,
    gDekuTreeRoom2MinimapTex,
    gDekuTreeRoom3MinimapTex,
    gDekuTreeRoom4MinimapTex,
    gDekuTreeRoom5MinimapTex,
    gDekuTreeRoom6MinimapTex,
    gDekuTreeRoom7MinimapTex,
    gDekuTreeRoom8MinimapTex,
    gDekuTreeRoom9MinimapTex,
    gDekuTreeRoom10MinimapTex,
    gDekuTreeRoom0Floor2MinimapTex,
    gDekuTreeRoom0Floor3MinimapTex,
    gDodongosCavernRoom0MinimapTex,
    gDodongosCavernRoom1MinimapTex,
    gDodongosCavernRoom2MinimapTex,
    gDodongosCavernRoom3MinimapTex,
    gDodongosCavernRoom4MinimapTex,
    gDodongosCavernRoom5MinimapTex,
    gDodongosCavernRoom6MinimapTex,
    gDodongosCavernRoom7MinimapTex,
    gDodongosCavernRoom8MinimapTex,
    gDodongosCavernRoom9MinimapTex,
    gDodongosCavernRoom10MinimapTex,
    gDodongosCavernRoom11MinimapTex,
    gDodongosCavernRoom12MinimapTex,
    gDodongosCavernRoom13MinimapTex,
    gDodongosCavernRoom14MinimapTex,
    gDodongosCavernRoom15MinimapTex,
    gDodongosCavernRoom0Floor3MinimapTex,
    gDodongosCavernRoom2Floor3MinimapTex,
    gDodongosCavernRoom3Floor3MinimapTex,
    gJabuRoom0MinimapTex,
    gJabuRoom1MinimapTex,
    gJabuRoom2MinimapTex,
    gJabuRoom3MinimapTex,
    gJabuRoom4MinimapTex,
    gJabuRoom5MinimapTex,
    gJabuRoom6MinimapTex,
    gJabuRoom7MinimapTex,
    gJabuRoom8MinimapTex,
    gJabuRoom9MinimapTex,
    gJabuRoom10MinimapTex,
    gJabuRoom11MinimapTex,
    gJabuRoom12MinimapTex,
    gJabuRoom13MinimapTex,
    gJabuRoom14MinimapTex,
    gJabuRoom1Basement1MinimapTex,
    gJabuRoom6Basement1MinimapTex,
    gForestTempleRoom0Floor1MinimapTex,
    gForestTempleRoom1MinimapTex,
    gForestTempleRoom2MinimapTex,
    gForestTempleRoom3MinimapTex,
    gForestTempleRoom4MinimapTex,
    gForestTempleRoom5MinimapTex,
    gForestTempleRoom6Floor1MinimapTex,
    gForestTempleRoom7Floor1MinimapTex,
    gForestTempleRoom8Floor1MinimapTex,
    gForestTempleRoom9MinimapTex,
    gForestTempleRoom10MinimapTex,
    gForestTempleRoom11Floor1MinimapTex,
    gForestTempleRoom12MinimapTex,
    gForestTempleRoom13MinimapTex,
    gForestTempleRoom14MinimapTex,
    gForestTempleRoom15MinimapTex,
    gForestTempleRoom16MinimapTex,
    gForestTempleRoom17MinimapTex,
    gForestTempleRoom18MinimapTex,
    gForestTempleRoom19MinimapTex,
    gForestTempleRoom20MinimapTex,
    gForestTempleRoom21MinimapTex,
    gForestTempleRoom0Floor3MinimapTex,
    gForestTempleRoom6Floor3MinimapTex,
    gForestTempleRoom7Floor3MinimapTex,
    gForestTempleRoom8Floor3MinimapTex,
    gForestTempleRoom11Floor3MinimapTex,
    gFireTempleRoom0MinimapTex,
    gFireTempleRoom1MinimapTex,
    gFireTempleRoom2MinimapTex,
    gFireTempleRoom3MinimapTex,
    gFireTempleRoom4Floor1MinimapTex,
    gFireTempleRoom5Floor1MinimapTex,
    gFireTempleRoom6Floor1MinimapTex,
    gFireTempleRoom7Floor1MinimapTex,
    gFireTempleRoom8MinimapTex,
    gFireTempleRoom9MinimapTex,
    gFireTempleRoom10Floor1MinimapTex,
    gFireTempleRoom11Floor1MinimapTex,
    gFireTempleRoom12Floor1MinimapTex,
    gFireTempleRoom13Floor1MinimapTex,
    gFireTempleRoom14MinimapTex,
    gFireTempleRoom15MinimapTex,
    gFireTempleRoom16MinimapTex,
    gFireTempleRoom17MinimapTex,
    gFireTempleRoom18MinimapTex,
    gFireTempleRoom19MinimapTex,
    gFireTempleRoom20MinimapTex,
    gFireTempleRoom21Floor1MinimapTex,
    gFireTempleRoom22MinimapTex,
    gFireTempleRoom23MinimapTex,
    gFireTempleRoom24Floor1MinimapTex,
    gFireTempleRoom25MinimapTex,
    gFireTempleRoom4Floor3MinimapTex,
    gFireTempleRoom5Floor3MinimapTex,
    gFireTempleRoom6Floor2MinimapTex,
    gFireTempleRoom6Floor3MinimapTex,
    gFireTempleRoom7Floor3MinimapTex,
    gFireTempleRoom10Floor2MinimapTex,
    gFireTempleRoom10Floor3MinimapTex,
    gFireTempleRoom11Floor3MinimapTex,
    gFireTempleRoom12Floor3MinimapTex,
    gFireTempleRoom13Floor3MinimapTex,
    gFireTempleRoom21Floor3MinimapTex,
    gFireTempleRoom24Floor3MinimapTex,
    gWaterTempleRoom0Floor3MinimapTex,
    gWaterTempleRoom1Floor3MinimapTex,
    gWaterTempleRoom2MinimapTex,
    gWaterTempleRoom3Floor3MinimapTex,
    gWaterTempleRoom4Floor3MinimapTex,
    gWaterTempleRoom5Floor3MinimapTex,
    gWaterTempleRoom6MinimapTex,
    gWaterTempleRoom7MinimapTex,
    gWaterTempleRoom8Floor3MinimapTex,
    gWaterTempleRoom9Floor3MinimapTex,
    gWaterTempleRoom10Floor3MinimapTex,
    gWaterTempleRoom11MinimapTex,
    gWaterTempleRoom12Floor3MinimapTex,
    gWaterTempleRoom13MinimapTex,
    gWaterTempleRoom14Floor3MinimapTex,
    gWaterTempleRoom15Floor3MinimapTex,
    gWaterTempleRoom16MinimapTex,
    gWaterTempleRoom17Floor3MinimapTex,
    gWaterTempleRoom18MinimapTex,
    gWaterTempleRoom19MinimapTex,
    gWaterTempleRoom20Floor3MinimapTex,
    gWaterTempleRoom21Floor3MinimapTex,
    gWaterTempleRoom0Floor2MinimapTex,
    gWaterTempleRoom0Floor1MinimapTex,
    gWaterTempleRoom0Basement1MinimapTex,
    gWaterTempleRoom1Floor2MinimapTex,
    gWaterTempleRoom1Floor1MinimapTex,
    gWaterTempleRoom1Basement1MinimapTex,
    gWaterTempleRoom3Basement1MinimapTex,
    gWaterTempleRoom4Floor2MinimapTex,
    gWaterTempleRoom5Floor2MinimapTex,
    gWaterTempleRoom5Floor1MinimapTex,
    gWaterTempleRoom6Floor2MinimapTex,
    gWaterTempleRoom8Basement1MinimapTex,
    gWaterTempleRoom9Basement1MinimapTex,
    gWaterTempleRoom10Floor2MinimapTex,
    gWaterTempleRoom12Basement1MinimapTex,
    gWaterTempleRoom14Basement1MinimapTex,
    gWaterTempleRoom15Basement1MinimapTex,
    gWaterTempleRoom17Floor2MinimapTex,
    gWaterTempleRoom17Floor1MinimapTex,
    gWaterTempleRoom20Floor2MinimapTex,
    gWaterTempleRoom21Floor1MinimapTex,
    gWaterTempleRoom5Basement1MinimapTex,
    gSpiritTempleRoom0MinimapTex,
    gSpiritTempleRoom1MinimapTex,
    gSpiritTempleRoom2MinimapTex,
    gSpiritTempleRoom3MinimapTex,
    gSpiritTempleRoom4Floor1MinimapTex,
    gSpiritTempleRoom5Floor2MinimapTex,
    gSpiritTempleRoom6MinimapTex,
    gSpiritTempleRoom7MinimapTex,
    gSpiritTempleRoom8MinimapTex,
    gSpiritTempleRoom9MinimapTex,
    gSpiritTempleRoom10MinimapTex,
    gBlankSpiritTempleRoom11MinimapTex,
    gSpiritTempleRoom12MinimapTex,
    gSpiritTempleRoom13MinimapTex,
    gSpiritTempleRoom14MinimapTex,
    gSpiritTempleRoom15Floor1MinimapTex,
    gSpiritTempleRoom16MinimapTex,
    gSpiritTempleRoom17MinimapTex,
    gSpiritTempleRoom18MinimapTex,
    gSpiritTempleRoom19MinimapTex,
    gSpiritTempleRoom20MinimapTex,
    gSpiritTempleRoom21MinimapTex,
    gSpiritTempleRoom22MinimapTex,
    gSpiritTempleRoom23Floor3MinimapTex,
    gSpiritTempleRoom24MinimapTex,
    gSpiritTempleRoom25MinimapTex,
    gSpiritTempleRoom26MinimapTex,
    gSpiritTempleRoom27MinimapTex,
    gSpiritTempleRoom4Floor2MinimapTex,
    gSpiritTempleRoom5Floor3MinimapTex,
    gSpiritTempleRoom15Floor2MinimapTex,
    gSpiritTempleRoom23Floor4MinimapTex,
    gShadowTempleRoom0MinimapTex,
    gShadowTempleRoom1MinimapTex,
    gShadowTempleRoom2MinimapTex,
    gShadowTempleRoom3MinimapTex,
    gShadowTempleRoom4MinimapTex,
    gShadowTempleRoom5MinimapTex,
    gShadowTempleRoom6MinimapTex,
    gShadowTempleRoom7MinimapTex,
    gShadowTempleRoom8Basement2MinimapTex,
    gShadowTempleRoom9Basement3MinimapTex,
    gShadowTempleRoom10MinimapTex,
    gShadowTempleRoom11MinimapTex,
    gShadowTempleRoom12Basement3MinimapTex,
    gShadowTempleRoom13MinimapTex,
    gShadowTempleRoom14Basement3MinimapTex,
    gShadowTempleRoom15MinimapTex,
    gShadowTempleRoom16MinimapTex,
    gShadowTempleRoom17MinimapTex,
    gShadowTempleRoom18MinimapTex,
    gShadowTempleRoom19MinimapTex,
    gShadowTempleRoom20MinimapTex,
    gShadowTempleRoom21Basement3MinimapTex,
    gShadowTempleRoom8Basement3MinimapTex,
    gShadowTempleRoom9Basement4MinimapTex,
    gShadowTempleRoom12Basement4MinimapTex,
    gShadowTempleRoom14Basement4MinimapTex,
    gShadowTempleRoom21Basement4MinimapTex,
    gBottomOfTheWellRoom0Basement1MinimapTex,
    gBottomOfTheWellRoom1Basement1MinimapTex,
    gBottomOfTheWellRoom2MinimapTex,
    gBottomOfTheWellRoom3MinimapTex,
    gBottomOfTheWellRoom4MinimapTex,
    gBottomOfTheWellRoom5MinimapTex,
    gBottomOfTheWellRoom6MinimapTex,
    gBottomOfTheWellRoom0Basement2MinimapTex,
    gBottomOfTheWellRoom1Basement2MinimapTex,
    gBottomOfTheWellRoom1Basement3MinimapTex,
    gIceCavernRoom0MinimapTex,
    gIceCavernRoom1MinimapTex,
    gIceCavernRoom2MinimapTex,
    gIceCavernRoom3MinimapTex,
    gIceCavernRoom4MinimapTex,
    gIceCavernRoom5MinimapTex,
    gIceCavernRoom6MinimapTex,
    gIceCavernRoom7MinimapTex,
    gIceCavernRoom8MinimapTex,
    gIceCavernRoom9MinimapTex,
    gIceCavernRoom10MinimapTex,
    gIceCavernRoom11MinimapTex,
};

void Map_InitData(GlobalContext* globalCtx, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 extendedMapIndex;

    switch (globalCtx->sceneNum) {
        case SCENE_SPOT00:
        case SCENE_SPOT01:
        case SCENE_SPOT02:
        case SCENE_SPOT03:
        case SCENE_SPOT04:
        case SCENE_SPOT05:
        case SCENE_SPOT06:
        case SCENE_SPOT07:
        case SCENE_SPOT08:
        case SCENE_SPOT09:
        case SCENE_SPOT10:
        case SCENE_SPOT11:
        case SCENE_SPOT12:
        case SCENE_SPOT13:
        case SCENE_SPOT15:
        case SCENE_SPOT16:
        case SCENE_SPOT17:
        case SCENE_SPOT18:
        case SCENE_SPOT20:
        case SCENE_GANON_TOU:
            extendedMapIndex = mapIndex;
            if (globalCtx->sceneNum == SCENE_SPOT02) {
                if (CHECK_QUEST_ITEM(QUEST_SONG_NOCTURNE)) {
                    extendedMapIndex = 0x14;
                }
            } else if (globalCtx->sceneNum == SCENE_SPOT06) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) && !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
                    extendedMapIndex = 0x15;
                }
            } else if (globalCtx->sceneNum == SCENE_SPOT09) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) && !((gSaveContext.eventChkInf[9] & 0xF) == 0xF)) {
                    extendedMapIndex = 0x16;
                }
            } else if (globalCtx->sceneNum == SCENE_SPOT12) {
                if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
                    extendedMapIndex = 0x17;
                }
            }
            osSyncPrintf(VT_FGCOL(BLUE));
            osSyncPrintf("ＫＫＫ＝%d\n", extendedMapIndex);
            osSyncPrintf(VT_RST);
            sEntranceIconMapIndex = extendedMapIndex;
            //DmaMgr_SendRequest1(interfaceCtx->mapSegment,
                                //(uintptr_t)_map_grand_staticSegmentRomStart + gMapData->owMinimapTexOffset[extendedMapIndex],
                                //gMapData->owMinimapTexSize[mapIndex], "../z_map_exp.c", 309);

            if (sEntranceIconMapIndex < 24)
                memcpy(globalCtx->interfaceCtx.mapSegment, ResourceMgr_LoadTexByName(minimapTableOW[sEntranceIconMapIndex]), gMapData->owMinimapTexSize[mapIndex]);

            interfaceCtx->unk_258 = mapIndex;
            break;
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
        case SCENE_MIZUSIN_BS:
        case SCENE_JYASINBOSS:
        case SCENE_HAKADAN_BS:
            osSyncPrintf(VT_FGCOL(YELLOW));
            // "Deku Tree Dungeon MAP Texture DMA"
            osSyncPrintf("デクの樹ダンジョンＭＡＰ テクスチャＤＭＡ(%x) scene_id_offset=%d  VREG(30)=%d\n", room,
                         mapIndex, VREG(30));
            osSyncPrintf(VT_RST);
            //DmaMgr_SendRequest1(globalCtx->interfaceCtx.mapSegment,
                                //(uintptr_t)_map_i_staticSegmentRomStart +
                                    //((gMapData->dgnMinimapTexIndexOffset[mapIndex] + room) * 0xFF0),
                                //0xFF0, "../z_map_exp.c", 346);

            memcpy(globalCtx->interfaceCtx.mapSegment, ResourceMgr_LoadTexByName(minimapTableDangeon[gMapData->dgnMinimapTexIndexOffset[mapIndex] + room]), 0xFF0);

            R_COMPASS_OFFSET_X = gMapData->roomCompassOffsetX[mapIndex][room];
            R_COMPASS_OFFSET_Y = gMapData->roomCompassOffsetY[mapIndex][room];
            Map_SetFloorPalettesData(globalCtx, VREG(30));
            osSyncPrintf("ＭＡＰ 各階ＯＮチェック\n"); // "MAP Individual Floor ON Check"
            break;
    }
}

void Map_InitRoomData(GlobalContext* globalCtx, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    osSyncPrintf("＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\nroom_no=%d (%d)(%d)\n＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\n", room,
                 mapIndex, globalCtx->sceneNum);

    if (room >= 0) {
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
            case SCENE_MIZUSIN_BS:
            case SCENE_JYASINBOSS:
            case SCENE_HAKADAN_BS:
                gSaveContext.sceneFlags[mapIndex].rooms |= gBitFlags[room];
                osSyncPrintf("ＲＯＯＭ＿ＩＮＦ＝%d\n", gSaveContext.sceneFlags[mapIndex].rooms);
                interfaceCtx->mapRoomNum = room;
                interfaceCtx->unk_25A = mapIndex;
                Map_SetPaletteData(globalCtx, room);
                osSyncPrintf(VT_FGCOL(YELLOW));
                osSyncPrintf("部屋部屋＝%d\n", room); // "Room Room = %d"
                osSyncPrintf(VT_RST);
                Map_InitData(globalCtx, room);
                break;
        }
    } else {
        interfaceCtx->mapRoomNum = 0;
    }

    if (gSaveContext.sunsSongState != SUNSSONG_SPEED_TIME) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }
}

void Map_Destroy(GlobalContext* globalCtx) {
    MapMark_ClearPointers(globalCtx);
    gMapData = NULL;
}

void Map_Init(GlobalContext* globalCtx) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    gMapData = &gMapDataTable;

    interfaceCtx->unk_258 = -1;
    interfaceCtx->unk_25A = -1;

    interfaceCtx->mapSegment = GameState_Alloc(&globalCtx->state, 0x1000, "../z_map_exp.c", 457);
    // "ＭＡＰ texture initialization scene_data_ID=%d mapSegment=%x"
    osSyncPrintf("\n\n\nＭＡＰ テクスチャ初期化   scene_data_ID=%d\nmapSegment=%x\n\n", globalCtx->sceneNum,
                 interfaceCtx->mapSegment, globalCtx);
    ASSERT(interfaceCtx->mapSegment != NULL, "parameter->mapSegment != NULL", "../z_map_exp.c", 459);

    switch (globalCtx->sceneNum) {
        case SCENE_SPOT00:
        case SCENE_SPOT01:
        case SCENE_SPOT02:
        case SCENE_SPOT03:
        case SCENE_SPOT04:
        case SCENE_SPOT05:
        case SCENE_SPOT06:
        case SCENE_SPOT07:
        case SCENE_SPOT08:
        case SCENE_SPOT09:
        case SCENE_SPOT10:
        case SCENE_SPOT11:
        case SCENE_SPOT12:
        case SCENE_SPOT13:
        case SCENE_SPOT15:
        case SCENE_SPOT16:
        case SCENE_SPOT17:
        case SCENE_SPOT18:
        case SCENE_SPOT20:
        case SCENE_GANON_TOU:
            mapIndex = globalCtx->sceneNum - SCENE_SPOT00;
            R_MAP_INDEX = gSaveContext.mapIndex = mapIndex;
            R_COMPASS_SCALE_X = gMapData->owCompassInfo[mapIndex][0];
            R_COMPASS_SCALE_Y = gMapData->owCompassInfo[mapIndex][1];
            R_COMPASS_OFFSET_X = gMapData->owCompassInfo[mapIndex][2];
            R_COMPASS_OFFSET_Y = gMapData->owCompassInfo[mapIndex][3];
            Map_InitData(globalCtx, mapIndex);
            R_OW_MINIMAP_X = gMapData->owMinimapPosX[mapIndex];
            R_OW_MINIMAP_Y = gMapData->owMinimapPosY[mapIndex];
            break;
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
        case SCENE_GANON:
        case SCENE_MEN:
        case SCENE_GERUDOWAY:
        case SCENE_GANONTIKA:
        case SCENE_GANON_SONOGO:
        case SCENE_GANONTIKA_SONOGO:
        case SCENE_TAKARAYA:
        case SCENE_YDAN_BOSS:
        case SCENE_DDAN_BOSS:
        case SCENE_BDAN_BOSS:
        case SCENE_MORIBOSSROOM:
        case SCENE_FIRE_BS:
        case SCENE_MIZUSIN_BS:
        case SCENE_JYASINBOSS:
        case SCENE_HAKADAN_BS:
            mapIndex =
                (globalCtx->sceneNum >= SCENE_YDAN_BOSS) ? globalCtx->sceneNum - SCENE_YDAN_BOSS : globalCtx->sceneNum;
            R_MAP_INDEX = gSaveContext.mapIndex = mapIndex;
            if ((globalCtx->sceneNum <= SCENE_ICE_DOUKUTO) || (globalCtx->sceneNum >= SCENE_YDAN_BOSS)) {
                R_COMPASS_SCALE_X = gMapData->dgnCompassInfo[mapIndex][0];
                R_COMPASS_SCALE_Y = gMapData->dgnCompassInfo[mapIndex][1];
                R_COMPASS_OFFSET_X = gMapData->dgnCompassInfo[mapIndex][2];
                R_COMPASS_OFFSET_Y = gMapData->dgnCompassInfo[mapIndex][3];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE = gMapData->dgnMinimapTexIndexBase[mapIndex];
                Map_InitRoomData(globalCtx, globalCtx->roomCtx.curRoom.num);
                MapMark_Init(globalCtx);
            }
            break;
    }
}

void Minimap_DrawCompassIcons(GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s16 tempX, tempZ;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_map_exp.c", 565);

    if (globalCtx->interfaceCtx.minimapAlpha >= 0xAA) {
        func_80094A14(globalCtx->state.gfxCtx);

        gSPMatrix(OVERLAY_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        tempX = player->actor.world.pos.x;
        tempZ = player->actor.world.pos.z;
        tempX /= R_COMPASS_SCALE_X;
        tempZ /= R_COMPASS_SCALE_Y;
        Matrix_Translate(OTRGetDimensionFromRightEdge((R_COMPASS_OFFSET_X + tempX) / 10.0f), (R_COMPASS_OFFSET_Y - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        Matrix_Scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
        Matrix_RotateX(-1.6f, MTXMODE_APPLY);
        tempX = (0x7FFF - player->actor.shape.rot.y) / 0x400;
        Matrix_RotateY(tempX / 10.0f, MTXMODE_APPLY);
        gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_map_exp.c", 585),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 255, 0, 255);
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);

        tempX = sPlayerInitialPosX;
        tempZ = sPlayerInitialPosZ;
        tempX /= R_COMPASS_SCALE_X;
        tempZ /= R_COMPASS_SCALE_Y;
        Matrix_Translate(OTRGetDimensionFromRightEdge((R_COMPASS_OFFSET_X + tempX) / 10.0f), (R_COMPASS_OFFSET_Y - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        Matrix_Scale(VREG(9) / 100.0f, VREG(9) / 100.0f, VREG(9) / 100.0f, MTXMODE_APPLY);
        Matrix_RotateX(VREG(52) / 10.0f, MTXMODE_APPLY);
        Matrix_RotateY(sPlayerInitialDirection / 10.0f, MTXMODE_APPLY);
        gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_map_exp.c", 603),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, 200, 0, 0, 255);
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_map_exp.c", 607);
}

void Minimap_Draw(GlobalContext* globalCtx) {
    s32 pad[2];
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s32 mapIndex = gSaveContext.mapIndex;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_map_exp.c", 626);

    if (globalCtx->pauseCtx.state < 4) {
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
                if (!R_MINIMAP_DISABLED) {
                    func_80094520(globalCtx->state.gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                      TEXEL0, 0, PRIMITIVE, 0);

                    if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 255, interfaceCtx->minimapAlpha);

                        gSPInvalidateTexCache(OVERLAY_DISP++, interfaceCtx->mapSegment);
                        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegment, G_IM_FMT_I, 96, 85, 0,
                                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                               G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                        const s16 dgnMiniMapX = OTRGetRectDimensionFromRightEdge(R_DGN_MINIMAP_X);

                        gSPWideTextureRectangle(OVERLAY_DISP++, dgnMiniMapX << 2, R_DGN_MINIMAP_Y << 2,
                                            (dgnMiniMapX + 96) << 2, (R_DGN_MINIMAP_Y + 85) << 2, G_TX_RENDERTILE,
                                            0, 0, 1 << 10, 1 << 10);
                    }

                    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, mapIndex)) {
                        Minimap_DrawCompassIcons(globalCtx); // Draw icons for the player spawn and current position
                        func_80094520(globalCtx->state.gfxCtx);
                        MapMark_Draw(globalCtx);
                    }
                }

                if (CHECK_BTN_ALL(globalCtx->state.input[0].press.button, BTN_L) && !Gameplay_InCsMode(globalCtx)) {
                    osSyncPrintf("Game_play_demo_mode_check=%d\n", Gameplay_InCsMode(globalCtx));
                    // clang-format off
                    if (!R_MINIMAP_DISABLED) { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_UP, &D_801333D4, 4,
                                                                      &D_801333E0, &D_801333E0, &D_801333E8); }
                    else { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_DOWN, &D_801333D4, 4,
                                                  &D_801333E0, &D_801333E0, &D_801333E8); }
                    // clang-format on
                    R_MINIMAP_DISABLED ^= 1;
                }

                break;
            case SCENE_SPOT00:
            case SCENE_SPOT01:
            case SCENE_SPOT02:
            case SCENE_SPOT03:
            case SCENE_SPOT04:
            case SCENE_SPOT05:
            case SCENE_SPOT06:
            case SCENE_SPOT07:
            case SCENE_SPOT08:
            case SCENE_SPOT09:
            case SCENE_SPOT10:
            case SCENE_SPOT11:
            case SCENE_SPOT12:
            case SCENE_SPOT13:
            case SCENE_SPOT15:
            case SCENE_SPOT16:
            case SCENE_SPOT17:
            case SCENE_SPOT18:
            case SCENE_SPOT20:
            case SCENE_GANON_TOU:
                if (!R_MINIMAP_DISABLED) {
                    func_80094520(globalCtx->state.gfxCtx);

                    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_MINIMAP_COLOR(0), R_MINIMAP_COLOR(1), R_MINIMAP_COLOR(2),
                                    interfaceCtx->minimapAlpha);

                    gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegment, G_IM_FMT_IA,
                                           gMapData->owMinimapWidth[mapIndex], gMapData->owMinimapHeight[mapIndex], 0,
                                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    const s16 oWMiniMapX = OTRGetRectDimensionFromRightEdge(R_OW_MINIMAP_X);

                    gSPWideTextureRectangle(OVERLAY_DISP++, oWMiniMapX << 2, R_OW_MINIMAP_Y << 2,
                                           (oWMiniMapX + gMapData->owMinimapWidth[mapIndex]) << 2,
                                           (R_OW_MINIMAP_Y + gMapData->owMinimapHeight[mapIndex]) << 2, G_TX_RENDERTILE, 0,
                                            0, 1 << 10, 1 << 10);

                    if (((globalCtx->sceneNum != SCENE_SPOT01) && (globalCtx->sceneNum != SCENE_SPOT04) &&
                         (globalCtx->sceneNum != SCENE_SPOT08)) ||
                        (LINK_AGE_IN_YEARS != YEARS_ADULT)) {
                        s16 IconSize = 8;
                        s16 PosX = gMapData->owEntranceIconPosX[sEntranceIconMapIndex];
                        s16 PosY = gMapData->owEntranceIconPosY[sEntranceIconMapIndex];
                        //gFixDungeonMinimapIcon fix both Y position of visible icon and hide these non needed.
                        if (CVar_GetS32("gFixDungeonMinimapIcon", 1) != 0){
                            //No idea why and how Original value work but this does actually fix them all.
                            PosY = PosY+1024;
                        }
                        s16 TopLeftX = OTRGetRectDimensionFromRightEdge(PosX) << 2;
                        s16 TopLeftY = PosY << 2;
                        s16 TopLeftW = OTRGetRectDimensionFromRightEdge(PosX + IconSize) << 2;
                        s16 TopLeftH = (PosY + IconSize) << 2;
                        if ((gMapData->owEntranceFlag[sEntranceIconMapIndex] == 0xFFFF) ||
                            ((gMapData->owEntranceFlag[sEntranceIconMapIndex] != 0xFFFF) &&
                             (gSaveContext.infTable[26] & gBitFlags[gMapData->owEntranceFlag[mapIndex]]))) {
                            if (gMapData->owEntranceIconPosY[sEntranceIconMapIndex] << 2 != 0 && CVar_GetS32("gFixDungeonMinimapIcon", 1) != 0){
                                gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                                                    IconSize, IconSize, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                                    G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                                gSPWideTextureRectangle(OVERLAY_DISP++, TopLeftX, TopLeftY, TopLeftW, TopLeftH, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                            }
                        } else if (CVar_GetS32("gAlwaysShowDungeonMinimapIcon", 0) != 0){ //Ability to show entrance Before beating the dungeon itself
                            gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                                                IconSize, IconSize, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                            gSPWideTextureRectangle(OVERLAY_DISP++, TopLeftX, TopLeftY, TopLeftW, TopLeftH, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                        } 
                    }

                    if ((globalCtx->sceneNum == SCENE_SPOT08) && (gSaveContext.infTable[26] & gBitFlags[9])) {
                        gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8,
                                            8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                        const s16 entranceX = OTRGetRectDimensionFromRightEdge(270);  

                        gSPWideTextureRectangle(OVERLAY_DISP++, entranceX << 2, 154 << 2, (entranceX + 32) << 2, (154 + 8) << 2, 
                                                G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                    }

                    Minimap_DrawCompassIcons(globalCtx); // Draw icons for the player spawn and current position
                }

                if (CHECK_BTN_ALL(globalCtx->state.input[0].press.button, BTN_L) && !Gameplay_InCsMode(globalCtx)) {
                    // clang-format off
                    if (!R_MINIMAP_DISABLED) { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_UP, &D_801333D4, 4,
                                                                      &D_801333E0, &D_801333E0, &D_801333E8); }
                    else { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_DOWN, &D_801333D4, 4,
                                                  &D_801333E0, &D_801333E0, &D_801333E8); }
                    // clang-format on
                    R_MINIMAP_DISABLED ^= 1;
                }

                break;
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_map_exp.c", 782);
}

s16 Map_GetFloorTextIndexOffset(s32 mapIndex, s32 floor) {
    return gMapData->floorTexIndexOffset[mapIndex][floor];
}

void Map_Update(GlobalContext* globalCtx) {
    static s16 sLastRoomNum = 99;
    Player* player = GET_PLAYER(globalCtx);
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 floor;
    s16 i;

    if ((globalCtx->pauseCtx.state == 0) && (globalCtx->pauseCtx.debugState == 0)) {
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
                interfaceCtx->mapPalette[30] = 0;
                if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
                    interfaceCtx->mapPalette[31] = 1;
                } else {
                    interfaceCtx->mapPalette[31] = 0;
                }

                for (floor = 0; floor < 8; floor++) {
                    if (player->actor.world.pos.y > gMapData->floorCoordY[mapIndex][floor]) {
                        break;
                    }
                }

                gSaveContext.sceneFlags[mapIndex].floors |= gBitFlags[floor];
                VREG(30) = floor;
                if (R_MAP_TEX_INDEX != (R_MAP_TEX_INDEX_BASE + Map_GetFloorTextIndexOffset(mapIndex, floor))) {
                    R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE + Map_GetFloorTextIndexOffset(mapIndex, floor);
                }
                if (1) {} // Appears to be necessary to match

                if (interfaceCtx->mapRoomNum != sLastRoomNum) {
                    // "Current floor = %d Current room = %x Number of rooms = %d"
                    osSyncPrintf("現在階＝%d  現在部屋＝%x  部屋数＝%d\n", floor, interfaceCtx->mapRoomNum,
                                 gMapData->switchEntryCount[mapIndex]);
                    sLastRoomNum = interfaceCtx->mapRoomNum;
                }

                for (i = 0; i < gMapData->switchEntryCount[mapIndex]; i++) {
                    if ((interfaceCtx->mapRoomNum == gMapData->switchFromRoom[mapIndex][i]) &&
                        (floor == gMapData->switchFromFloor[mapIndex][i])) {
                        interfaceCtx->mapRoomNum = gMapData->switchToRoom[mapIndex][i];
                        osSyncPrintf(VT_FGCOL(YELLOW));
                        // "Layer switching = %x"
                        osSyncPrintf("階層切替＝%x\n", interfaceCtx->mapRoomNum);
                        osSyncPrintf(VT_RST);
                        Map_InitData(globalCtx, interfaceCtx->mapRoomNum);
                        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
                        Map_SavePlayerInitialInfo(globalCtx);
                    }
                }

                VREG(10) = interfaceCtx->mapRoomNum;
                break;
            case SCENE_YDAN_BOSS:
            case SCENE_DDAN_BOSS:
            case SCENE_BDAN_BOSS:
            case SCENE_MORIBOSSROOM:
            case SCENE_FIRE_BS:
            case SCENE_MIZUSIN_BS:
            case SCENE_JYASINBOSS:
            case SCENE_HAKADAN_BS:
                VREG(30) = gMapData->bossFloor[globalCtx->sceneNum - SCENE_YDAN_BOSS];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE +
                                  gMapData->floorTexIndexOffset[globalCtx->sceneNum - SCENE_YDAN_BOSS][VREG(30)];
                break;
        }
    }
}
