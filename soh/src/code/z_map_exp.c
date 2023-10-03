#include "global.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/map_i_static/map_i_static.h"
#include "textures/map_grand_static/map_grand_static.h"
#include <assert.h>
#ifdef ENABLE_REMOTE_CONTROL
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Anchor.h"
#endif

MapData* gMapData;

s16 sPlayerInitialPosX = 0;
s16 sPlayerInitialPosZ = 0;
s16 sPlayerInitialDirection = 0;
s16 sEntranceIconMapIndex = 0;

s16 Top_MM_Margin = 0;
s16 Left_MM_Margin = 0;
s16 Right_MM_Margin = 0;
s16 Bottom_MM_Margin = 0;

void Map_SavePlayerInitialInfo(PlayState* play) {
    Player* player = GET_PLAYER(play);

    sPlayerInitialPosX = player->actor.world.pos.x;
    sPlayerInitialPosZ = player->actor.world.pos.z;
    sPlayerInitialDirection = (s16)((0x7FFF - player->actor.shape.rot.y) / 0x400);
}

void Map_SetPaletteData(PlayState* play, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
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

void Map_SetFloorPalettesData(PlayState* play, s16 floor) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
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

    switch (play->sceneNum) {
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            for (i = 0; i < gMapData->maxPaletteCount[mapIndex]; i++) {
                room = gMapData->paletteRoom[mapIndex][floor][i];
                if ((room != 0xFF) && (gSaveContext.sceneFlags[mapIndex].rooms & gBitFlags[room])) {
                    Map_SetPaletteData(play, room);
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

void Map_InitData(PlayState* play, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 extendedMapIndex;

    switch (play->sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_RIVER:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LAKE_HYLIA:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDO_VALLEY:
        case SCENE_LOST_WOODS:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
        case SCENE_LON_LON_RANCH:
        case SCENE_OUTSIDE_GANONS_CASTLE:
            extendedMapIndex = mapIndex;
            if (play->sceneNum == SCENE_GRAVEYARD) {
                if (CHECK_QUEST_ITEM(QUEST_SONG_NOCTURNE)) {
                    extendedMapIndex = 0x14;
                }
            } else if (play->sceneNum == SCENE_LAKE_HYLIA) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) &&
                    ((!IS_RANDO && !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) ||
                     (IS_RANDO && !Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_WATER_TEMPLE)))) {
                    extendedMapIndex = 0x15;
                }
            } else if (play->sceneNum == SCENE_GERUDO_VALLEY) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) && !GET_EVENTCHKINF_CARPENTERS_FREE_ALL()) {
                    extendedMapIndex = 0x16;
                }
            } else if (play->sceneNum == SCENE_GERUDOS_FORTRESS) {
                if ((!IS_RANDO && GET_EVENTCHKINF_CARPENTERS_FREE_ALL()) ||
                    (IS_RANDO && CHECK_QUEST_ITEM(QUEST_GERUDO_CARD))) {
                    extendedMapIndex = 0x17;
                }
            }
            osSyncPrintf(VT_FGCOL(BLUE));
            osSyncPrintf("ＫＫＫ＝%d\n", extendedMapIndex);
            osSyncPrintf(VT_RST);
            sEntranceIconMapIndex = extendedMapIndex;
            //DmaMgr_SendRequest1(interfaceCtx->mapSegment,
                                //(uintptr_t)_map_grand_staticSegmentRomStart + gMapData->owMinimapTexOffset[extendedMapIndex],
                                //gMapData->owMinimapTexSize[mapIndex], __FILE__, __LINE__);

            if (sEntranceIconMapIndex < 24) {
                play->interfaceCtx.mapSegment[0] = ResourceGetDataByName(minimapTableOW[sEntranceIconMapIndex]);
                play->interfaceCtx.mapSegmentName[0] = minimapTableOW[sEntranceIconMapIndex];
            }

            interfaceCtx->unk_258 = mapIndex;
            break;
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            osSyncPrintf(VT_FGCOL(YELLOW));
            // "Deku Tree Dungeon MAP Texture DMA"
            osSyncPrintf("デクの樹ダンジョンＭＡＰ テクスチャＤＭＡ(%x) scene_id_offset=%d  VREG(30)=%d\n", room,
                         mapIndex, VREG(30));
            osSyncPrintf(VT_RST);
            //DmaMgr_SendRequest1(play->interfaceCtx.mapSegment,
                                //(uintptr_t)_map_i_staticSegmentRomStart +
                                    //((gMapData->dgnMinimapTexIndexOffset[mapIndex] + room) * 0xFF0),
                                //0xFF0, __FILE__, __LINE__);

            play->interfaceCtx.mapSegment[0] = ResourceGetDataByName(
                    minimapTableDangeon[gMapData->dgnMinimapTexIndexOffset[mapIndex] + room]);
            play->interfaceCtx.mapSegmentName[0] = minimapTableDangeon[gMapData->dgnMinimapTexIndexOffset[mapIndex] + room];
            R_COMPASS_OFFSET_X = gMapData->roomCompassOffsetX[mapIndex][room];
            R_COMPASS_OFFSET_Y = gMapData->roomCompassOffsetY[mapIndex][room];
            Map_SetFloorPalettesData(play, VREG(30));
            osSyncPrintf("ＭＡＰ 各階ＯＮチェック\n"); // "MAP Individual Floor ON Check"
            break;
    }
}

void Map_InitRoomData(PlayState* play, s16 room) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osSyncPrintf("＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\nroom_no=%d (%d)(%d)\n＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\n", room,
                 mapIndex, play->sceneNum);

    if (room >= 0) {
        switch (play->sceneNum) {
            case SCENE_DEKU_TREE:
            case SCENE_DODONGOS_CAVERN:
            case SCENE_JABU_JABU:
            case SCENE_FOREST_TEMPLE:
            case SCENE_FIRE_TEMPLE:
            case SCENE_WATER_TEMPLE:
            case SCENE_SPIRIT_TEMPLE:
            case SCENE_SHADOW_TEMPLE:
            case SCENE_BOTTOM_OF_THE_WELL:
            case SCENE_ICE_CAVERN:
            case SCENE_DEKU_TREE_BOSS:
            case SCENE_DODONGOS_CAVERN_BOSS:
            case SCENE_JABU_JABU_BOSS:
            case SCENE_FOREST_TEMPLE_BOSS:
            case SCENE_FIRE_TEMPLE_BOSS:
            case SCENE_WATER_TEMPLE_BOSS:
            case SCENE_SPIRIT_TEMPLE_BOSS:
            case SCENE_SHADOW_TEMPLE_BOSS:
                gSaveContext.sceneFlags[mapIndex].rooms |= gBitFlags[room];
                osSyncPrintf("ＲＯＯＭ＿ＩＮＦ＝%d\n", gSaveContext.sceneFlags[mapIndex].rooms);
                interfaceCtx->mapRoomNum = room;
                interfaceCtx->unk_25A = mapIndex;
                Map_SetPaletteData(play, room);
                osSyncPrintf(VT_FGCOL(YELLOW));
                osSyncPrintf("部屋部屋＝%d\n", room); // "Room Room = %d"
                osSyncPrintf(VT_RST);
                Map_InitData(play, room);
                break;
        }
    } else {
        interfaceCtx->mapRoomNum = 0;
    }

    if (gSaveContext.sunsSongState != SUNSSONG_SPEED_TIME) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }
}

void Map_Destroy(PlayState* play) {
    MapMark_ClearPointers(play);
    gMapData = NULL;
}

void Map_Init(PlayState* play) {
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    gMapData = &gMapDataTable;

    interfaceCtx->unk_258 = -1;
    interfaceCtx->unk_25A = -1;

    interfaceCtx->mapSegment = GAMESTATE_ALLOC_MC(&play->state, 2 * sizeof(char*));
    interfaceCtx->mapSegmentName = GAMESTATE_ALLOC_MC(&play->state, 2 * sizeof(char*));
    // "ＭＡＰ texture initialization scene_data_ID=%d mapSegment=%x"
    osSyncPrintf("\n\n\nＭＡＰ テクスチャ初期化   scene_data_ID=%d\nmapSegment=%x\n\n", play->sceneNum,
                 interfaceCtx->mapSegment, play);
    assert(interfaceCtx->mapSegment != NULL);

    switch (play->sceneNum) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_RIVER:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LAKE_HYLIA:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDO_VALLEY:
        case SCENE_LOST_WOODS:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
        case SCENE_LON_LON_RANCH:
        case SCENE_OUTSIDE_GANONS_CASTLE:
            mapIndex = play->sceneNum - SCENE_HYRULE_FIELD;
            R_MAP_INDEX = gSaveContext.mapIndex = mapIndex;
            R_COMPASS_SCALE_X = gMapData->owCompassInfo[mapIndex][0];
            R_COMPASS_SCALE_Y = gMapData->owCompassInfo[mapIndex][1];
            R_COMPASS_OFFSET_X = gMapData->owCompassInfo[mapIndex][2];
            R_COMPASS_OFFSET_Y = gMapData->owCompassInfo[mapIndex][3];
            Map_InitData(play, mapIndex);
            R_OW_MINIMAP_X = gMapData->owMinimapPosX[mapIndex];
            R_OW_MINIMAP_Y = gMapData->owMinimapPosY[mapIndex];
            break;
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_GANONS_TOWER:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_THIEVES_HIDEOUT:
        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_TREASURE_BOX_SHOP:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            mapIndex =
                (play->sceneNum >= SCENE_DEKU_TREE_BOSS) ? play->sceneNum - SCENE_DEKU_TREE_BOSS : play->sceneNum;
            R_MAP_INDEX = gSaveContext.mapIndex = mapIndex;
            if ((play->sceneNum <= SCENE_ICE_CAVERN) || (play->sceneNum >= SCENE_DEKU_TREE_BOSS)) {
                R_COMPASS_SCALE_X = gMapData->dgnCompassInfo[mapIndex][0];
                R_COMPASS_SCALE_Y = gMapData->dgnCompassInfo[mapIndex][1];
                R_COMPASS_OFFSET_X = gMapData->dgnCompassInfo[mapIndex][2];
                R_COMPASS_OFFSET_Y = gMapData->dgnCompassInfo[mapIndex][3];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE = gMapData->dgnMinimapTexIndexBase[mapIndex];
                Map_InitRoomData(play, play->roomCtx.curRoom.num);
                MapMark_Init(play);
            }
            break;
    }
}

extern s16 gEnPartnerId;

void Minimap_DrawCompassIcons(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 tempX, tempZ;
    Color_RGB8 lastEntranceColor = { 200, 0, 0 };
    if (CVarGetInteger("gCosmetics.Hud_MinimapEntrance.Changed", 0)) {
        lastEntranceColor = CVarGetColor24("gCosmetics.Hud_MinimapEntrance.Value", lastEntranceColor);
    }
    Color_RGB8 currentPositionColor = { 200, 255, 0 };
    if (CVarGetInteger("gCosmetics.Hud_MinimapPosition.Changed", 0)) {
        currentPositionColor = CVarGetColor24("gCosmetics.Hud_MinimapPosition.Value", currentPositionColor);
    }
    s16 X_Margins_Minimap;
    s16 Y_Margins_Minimap;
    if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {
        if (CVarGetInteger("gMinimapPosType", 0) == 0) {X_Margins_Minimap = Right_MM_Margin;};
        Y_Margins_Minimap = Bottom_MM_Margin;
    } else {
        X_Margins_Minimap = 0;
        Y_Margins_Minimap = 0;
    }

    OPEN_DISPS(play->state.gfxCtx);

    if (play->interfaceCtx.minimapAlpha >= 0xAA) {
        Gfx_SetupDL_42Overlay(play->state.gfxCtx);

        //Player current position (yellow arrow)
        gSPMatrix(OVERLAY_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);

        s16 mapWidth = 0;
        s16 mapStartPosX = 0;
        if (play->sceneNum >= SCENE_HYRULE_FIELD && play->sceneNum <= SCENE_OUTSIDE_GANONS_CASTLE) { // Overworld
            mapStartPosX = R_OW_MINIMAP_X;
            mapWidth = gMapData->owMinimapWidth[R_MAP_INDEX];
        } else if (play->sceneNum >= SCENE_DEKU_TREE && play->sceneNum <= SCENE_ICE_CAVERN) { // Dungeons
            mapStartPosX = R_DGN_MINIMAP_X;
            mapWidth = 96;
        }

        // The compass offset value is a factor of 10 compared to N64 screen pixels and originates in the center of the screen
        // Compute the additional mirror offset value by normalizing the original offset position
        // and taking it's distance to the center of the map, duplicating that result and casting back to a factor of 10
        s16 mirrorOffset = ((mapWidth / 2) - ((R_COMPASS_OFFSET_X / 10) - (mapStartPosX - SCREEN_WIDTH / 2))) * 2 * 10;

        tempX = player->actor.world.pos.x;
        tempZ = player->actor.world.pos.z;
        tempX /= R_COMPASS_SCALE_X * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
        tempZ /= R_COMPASS_SCALE_Y;

        s16 tempXOffset = R_COMPASS_OFFSET_X + (CVarGetInteger("gMirroredWorld", 0) ? mirrorOffset : 0);
        if (CVarGetInteger("gMinimapPosType", 0) != 0) {
            if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                Matrix_Translate(
                    OTRGetDimensionFromLeftEdge((tempXOffset + (X_Margins_Minimap*10) + tempX + (CVarGetInteger("gMinimapPosX", 0)*10)) / 10.0f),
                    (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap*10)*-1) - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                Matrix_Translate(
                    OTRGetDimensionFromRightEdge((tempXOffset + (X_Margins_Minimap*10) + tempX + (CVarGetInteger("gMinimapPosX", 0)*10)) / 10.0f),
                    (R_COMPASS_OFFSET_Y +((Y_Margins_Minimap*10)*-1) - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                Matrix_Translate(
                    (tempXOffset + tempX + (CVarGetInteger("gMinimapPosX", 0)*10) / 10.0f),
                    (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap*10)*-1) - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            }
        } else {
            Matrix_Translate(OTRGetDimensionFromRightEdge((tempXOffset+(X_Margins_Minimap*10) + tempX) / 10.0f), (R_COMPASS_OFFSET_Y+((Y_Margins_Minimap*10)*-1) - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        }
        Matrix_Scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
        Matrix_RotateX(-1.6f, MTXMODE_APPLY);
        tempX = ((0x7FFF - player->actor.shape.rot.y) / 0x400) * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
        Matrix_RotateY(tempX / 10.0f, MTXMODE_APPLY);
        gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, currentPositionColor.r, currentPositionColor.g, currentPositionColor.b, 255);
#ifdef ENABLE_REMOTE_CONTROL
        if (CVarGetInteger("gRemote.Scheme", 0) == GI_SCHEME_ANCHOR) {
            Color_RGB8 myColor = CVarGetColor24("gRemote.AnchorColor", (Color_RGB8){ 100, 255, 100 });
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, myColor.r, myColor.g, myColor.b, 255);
        }
#endif
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);

        //Player map entry (red arrow)
        tempX = sPlayerInitialPosX;
        tempZ = sPlayerInitialPosZ;
        tempX /= R_COMPASS_SCALE_X * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
        tempZ /= R_COMPASS_SCALE_Y;
        if (CVarGetInteger("gMinimapPosType", 0) != 0) {
            if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                Matrix_Translate(
                    OTRGetDimensionFromLeftEdge((tempXOffset + (X_Margins_Minimap*10) + tempX + (CVarGetInteger("gMinimapPosX", 0)*10)) / 10.0f),
                    (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap*10)*-1) - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                Matrix_Translate(
                    OTRGetDimensionFromRightEdge((tempXOffset + (X_Margins_Minimap*10) + tempX + (CVarGetInteger("gMinimapPosX", 0)*10)) / 10.0f),
                    (R_COMPASS_OFFSET_Y +((Y_Margins_Minimap*10)*-1) - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                Matrix_Translate(
                    (tempXOffset + tempX + (CVarGetInteger("gMinimapPosX", 0)*10) / 10.0f),
                    (R_COMPASS_OFFSET_Y - tempZ + ((CVarGetInteger("gMinimapPosY", 0)*10)*-1)) / 10.0f, 0.0f, MTXMODE_NEW);
            }
        } else {
            Matrix_Translate(OTRGetDimensionFromRightEdge((tempXOffset+(X_Margins_Minimap*10) + tempX) / 10.0f), (R_COMPASS_OFFSET_Y+((Y_Margins_Minimap*10)*-1) - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        }
        Matrix_Scale(VREG(9) / 100.0f, VREG(9) / 100.0f, VREG(9) / 100.0f, MTXMODE_APPLY);
        Matrix_RotateX(VREG(52) / 10.0f, MTXMODE_APPLY);
        Matrix_RotateY((sPlayerInitialDirection * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1)) / 10.0f, MTXMODE_APPLY);
        gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, lastEntranceColor.r, lastEntranceColor.g, lastEntranceColor.b, 255);
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);

#ifdef ENABLE_REMOTE_CONTROL
        // Other Anchor Players Arrow
        Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
        while (actor != NULL) {
            if (gEnPartnerId == actor->id && Anchor_GetClientRoomIndex(actor->params - 3) == gPlayState->roomCtx.curRoom.num) {
                if (actor->world.pos.x != -9999.0 && Anchor_GetClientScene(actor->params - 3) == gPlayState->sceneNum) {
                    Color_RGB8 playerColor = Anchor_GetClientColor(actor->params - 3);

                    tempX = actor->world.pos.x;
                    tempZ = actor->world.pos.z;
                    tempX /= R_COMPASS_SCALE_X * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
                    tempZ /= R_COMPASS_SCALE_Y;

                    s16 tempXOffset = R_COMPASS_OFFSET_X + (CVarGetInteger("gMirroredWorld", 0) ? mirrorOffset : 0);
                    if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                        if (CVarGetInteger("gMinimapPosType", 0) == 1) { // Anchor Left
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {
                                X_Margins_Minimap = Left_MM_Margin;
                            };
                            Matrix_Translate(
                                OTRGetDimensionFromLeftEdge((tempXOffset + (X_Margins_Minimap * 10) + tempX +
                                                             (CVarGetInteger("gMinimapPosX", 0) * 10)) /
                                                            10.0f),
                                (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap * 10) * -1) - tempZ +
                                 ((CVarGetInteger("gMinimapPosY", 0) * 10) * -1)) /
                                    10.0f,
                                0.0f, MTXMODE_NEW);
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 2) { // Anchor Right
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {
                                X_Margins_Minimap = Right_MM_Margin;
                            };
                            Matrix_Translate(
                                OTRGetDimensionFromRightEdge((tempXOffset + (X_Margins_Minimap * 10) + tempX +
                                                              (CVarGetInteger("gMinimapPosX", 0) * 10)) /
                                                             10.0f),
                                (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap * 10) * -1) - tempZ +
                                 ((CVarGetInteger("gMinimapPosY", 0) * 10) * -1)) /
                                    10.0f,
                                0.0f, MTXMODE_NEW);
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 3) { // Anchor None
                            Matrix_Translate((tempXOffset + tempX + (CVarGetInteger("gMinimapPosX", 0) * 10) / 10.0f),
                                             (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap * 10) * -1) - tempZ +
                                              ((CVarGetInteger("gMinimapPosY", 0) * 10) * -1)) /
                                                 10.0f,
                                             0.0f, MTXMODE_NEW);
                        }
                    } else {
                        Matrix_Translate(
                            OTRGetDimensionFromRightEdge((tempXOffset + (X_Margins_Minimap * 10) + tempX) / 10.0f),
                            (R_COMPASS_OFFSET_Y + ((Y_Margins_Minimap * 10) * -1) - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
                    }
                    Matrix_Scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
                    Matrix_RotateX(-1.6f, MTXMODE_APPLY);
                    tempX = ((0x7FFF - actor->shape.rot.y) / 0x400) * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
                    Matrix_RotateY(tempX / 10.0f, MTXMODE_APPLY);
                    gSPMatrix(OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, playerColor.r, playerColor.g, playerColor.b, 255);
                    gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);
                }
            }
            actor = actor->next;
        }
#endif
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Minimap_Draw(PlayState* play) {
    s32 pad[2];
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s32 mapIndex = gSaveContext.mapIndex;
    Color_RGB8 minimapColor = {0, 255, 255};
    if (CVarGetInteger("gCosmetics.Hud_Minimap.Changed", 0)) {
        minimapColor = CVarGetColor24("gCosmetics.Hud_Minimap.Value", minimapColor);
    }

    OPEN_DISPS(play->state.gfxCtx);

    // If any of these CVars are enabled, disable toggling the minimap with L, unless gEnableMapToggle is set
    bool enableMapToggle =
        !(CVarGetInteger("gDebugEnabled", 0) || CVarGetInteger("gMoonJumpOnL", 0) || CVarGetInteger("gTurboOnL", 0)) ||
        CVarGetInteger("gEnableMapToggle", 0);

    if (play->pauseCtx.state < 4) {
        //Minimap margins
        s16 X_Margins_Minimap;
        s16 Y_Margins_Minimap;
        if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {
            if (CVarGetInteger("gMinimapPosType", 0) == 0) {X_Margins_Minimap = Right_MM_Margin;};
            Y_Margins_Minimap = Bottom_MM_Margin;
        } else {
            X_Margins_Minimap = 0;
            Y_Margins_Minimap = 0;
        }

        switch (play->sceneNum) {
            case SCENE_DEKU_TREE:
            case SCENE_DODONGOS_CAVERN:
            case SCENE_JABU_JABU:
            case SCENE_FOREST_TEMPLE:
            case SCENE_FIRE_TEMPLE:
            case SCENE_WATER_TEMPLE:
            case SCENE_SPIRIT_TEMPLE:
            case SCENE_SHADOW_TEMPLE:
            case SCENE_BOTTOM_OF_THE_WELL:
            case SCENE_ICE_CAVERN:
                if (!R_MINIMAP_DISABLED && CVarGetInteger("gMinimapPosType", 0) != 4) { // Not Hidden
                    Gfx_SetupDL_39Overlay(play->state.gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                      TEXEL0, 0, PRIMITIVE, 0);

                    if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, minimapColor.r, minimapColor.g, minimapColor.b, interfaceCtx->minimapAlpha);

                        u8 mirrorMode = CVarGetInteger("gMirroredWorld", 0) ? G_TX_MIRROR : G_TX_NOMIRROR;
                        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegmentName[0], G_IM_FMT_I, 96, 85, 0,
                                               mirrorMode | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                               G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                        s16 dgnMiniMapX = OTRGetRectDimensionFromRightEdge(R_DGN_MINIMAP_X + X_Margins_Minimap);
                        s16 dgnMiniMapY = R_DGN_MINIMAP_Y + Y_Margins_Minimap;
                        if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                            dgnMiniMapY = R_DGN_MINIMAP_Y+CVarGetInteger("gMinimapPosY", 0)+Y_Margins_Minimap;
                            if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                                dgnMiniMapX = OTRGetDimensionFromLeftEdge(R_DGN_MINIMAP_X+CVarGetInteger("gMinimapPosX", 0)+X_Margins_Minimap);
                            } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                                dgnMiniMapX = OTRGetDimensionFromRightEdge(R_DGN_MINIMAP_X+CVarGetInteger("gMinimapPosX", 0)+X_Margins_Minimap);
                            } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                                dgnMiniMapX = CVarGetInteger("gMinimapPosX", 0);
                            }
                        }

                        s32 sValue = 0;
                        if (CVarGetInteger("gMirroredWorld", 0)) {
                            // Flip the minimap on the x-axis (s-axis) by setting s to the textures mirror boundary
                            sValue = 96 << 5;
                        }

                        gSPWideTextureRectangle(OVERLAY_DISP++, dgnMiniMapX << 2, dgnMiniMapY << 2,
                                            (dgnMiniMapX + 96) << 2, (dgnMiniMapY + 85) << 2, G_TX_RENDERTILE,
                                            sValue, 0, 1 << 10, 1 << 10);
                    }

                    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, mapIndex)) {
                        Minimap_DrawCompassIcons(play); // Draw icons for the player spawn and current position
                        Gfx_SetupDL_39Overlay(play->state.gfxCtx);
                        MapMark_Draw(play);
                    }
                }

                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_L) && !Play_InCsMode(play) && enableMapToggle) {
                    osSyncPrintf("Game_play_demo_mode_check=%d\n", Play_InCsMode(play));
                    // clang-format off
                    if (!R_MINIMAP_DISABLED) { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_UP, &D_801333D4, 4,
                                                                      &D_801333E0, &D_801333E0, &D_801333E8); }
                    else { Audio_PlaySoundGeneral(NA_SE_SY_CAMERA_ZOOM_DOWN, &D_801333D4, 4,
                                                  &D_801333E0, &D_801333E0, &D_801333E8); }
                    // clang-format on
                    R_MINIMAP_DISABLED ^= 1;
                }

                break;
            case SCENE_HYRULE_FIELD:
            case SCENE_KAKARIKO_VILLAGE:
            case SCENE_GRAVEYARD:
            case SCENE_ZORAS_RIVER:
            case SCENE_KOKIRI_FOREST:
            case SCENE_SACRED_FOREST_MEADOW:
            case SCENE_LAKE_HYLIA:
            case SCENE_ZORAS_DOMAIN:
            case SCENE_ZORAS_FOUNTAIN:
            case SCENE_GERUDO_VALLEY:
            case SCENE_LOST_WOODS:
            case SCENE_DESERT_COLOSSUS:
            case SCENE_GERUDOS_FORTRESS:
            case SCENE_HAUNTED_WASTELAND:
            case SCENE_HYRULE_CASTLE:
            case SCENE_DEATH_MOUNTAIN_TRAIL:
            case SCENE_DEATH_MOUNTAIN_CRATER:
            case SCENE_GORON_CITY:
            case SCENE_LON_LON_RANCH:
            case SCENE_OUTSIDE_GANONS_CASTLE:
                if (!R_MINIMAP_DISABLED && CVarGetInteger("gMinimapPosType", 0) != 4) { // Not Hidden
                    Gfx_SetupDL_39Overlay(play->state.gfxCtx);

                    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, minimapColor.r, minimapColor.g, minimapColor.b, interfaceCtx->minimapAlpha);

                    u8 mirrorMode = CVarGetInteger("gMirroredWorld", 0) ? G_TX_MIRROR : G_TX_NOMIRROR;
                    gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegmentName[0], G_IM_FMT_IA,
                                           gMapData->owMinimapWidth[mapIndex], gMapData->owMinimapHeight[mapIndex], 0,
                                           mirrorMode | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    s16 oWMiniMapX = OTRGetRectDimensionFromRightEdge(R_OW_MINIMAP_X + X_Margins_Minimap);
                    s16 oWMiniMapY = R_OW_MINIMAP_Y + Y_Margins_Minimap;
                    if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                        oWMiniMapY = R_OW_MINIMAP_Y+CVarGetInteger("gMinimapPosY", 0)+Y_Margins_Minimap;
                        if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                            oWMiniMapX = OTRGetDimensionFromLeftEdge(R_OW_MINIMAP_X+CVarGetInteger("gMinimapPosX", 0)+X_Margins_Minimap);
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                            oWMiniMapX = OTRGetDimensionFromRightEdge(R_OW_MINIMAP_X+CVarGetInteger("gMinimapPosX", 0)+X_Margins_Minimap);
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                            oWMiniMapX = CVarGetInteger("gMinimapPosX", 0);
                        }
                    }

                    s32 sValue = 0;
                    if (CVarGetInteger("gMirroredWorld", 0)) {
                        // Flip the minimap on the x-axis (s-axis) by setting s to the textures mirror boundary
                        sValue = gMapData->owMinimapWidth[mapIndex] << 5;
                    }

                    gSPWideTextureRectangle(OVERLAY_DISP++, oWMiniMapX << 2, oWMiniMapY << 2,
                                           (oWMiniMapX + gMapData->owMinimapWidth[mapIndex]) << 2,
                                           (oWMiniMapY + gMapData->owMinimapHeight[mapIndex]) << 2, G_TX_RENDERTILE, sValue,
                                            0, 1 << 10, 1 << 10);

                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, minimapColor.r, minimapColor.g, minimapColor.b, interfaceCtx->minimapAlpha);

                    s16 iconSize = 8;

                    if (((play->sceneNum != SCENE_KAKARIKO_VILLAGE) && (play->sceneNum != SCENE_KOKIRI_FOREST) &&
                         (play->sceneNum != SCENE_ZORAS_FOUNTAIN)) ||
                        (LINK_AGE_IN_YEARS != YEARS_ADULT)) {
                        s16 origX = gMapData->owEntranceIconPosX[sEntranceIconMapIndex];

                        // Compute the distance of the center of the original texture location to the center of the map
                        // Then duplicate that and right-align the texture (extra 2 pixels are due to the texture being a 6px left-aligned in a 8px tex)
                        s16 distFromCenter = (R_OW_MINIMAP_X + (gMapData->owMinimapWidth[mapIndex] / 2)) - (origX + (iconSize / 2));
                        s16 mirrorOffset = distFromCenter * 2 + (iconSize / 2) - 2;
                        s16 newX = origX + (CVarGetInteger("gMirroredWorld", 0) ? mirrorOffset : 0);

                        // The game authentically uses larger negative values for the entrance icon Y pos value. Normally only the first 12 bits
                        // would be read when the final value is passed into `gSPTextureRectangle`, but our cosmetic hud placements requires using
                        // `gSPWideTextureRectangle` which reads the first 24 bits instead. This caused the icon to be placed off screen.
                        // To address this, we take only the first 10 bits (which are later left-shifted by 2 to get our final 12 bits)
                        // to fix the entrance icon position when used with `gSPWideTextureRectangle`
                        s16 newY = gMapData->owEntranceIconPosY[sEntranceIconMapIndex] & 0x3FF;

                        s16 entranceX = OTRGetRectDimensionFromRightEdge(newX + X_Margins_Minimap);
                        s16 entranceY = newY + Y_Margins_Minimap;
                        if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                            entranceY = newY + CVarGetInteger("gMinimapPosY", 0) + Y_Margins_Minimap;
                            if (CVarGetInteger("gMinimapPosType", 0) == 1) { // Anchor Left
                                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                                entranceX = OTRGetRectDimensionFromLeftEdge(newX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0));
                            } else if (CVarGetInteger("gMinimapPosType", 0) == 2) { // Anchor Right
                                if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                                entranceX = OTRGetRectDimensionFromRightEdge(newX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0));
                            } else if (CVarGetInteger("gMinimapPosType", 0) == 3) { // Anchor None
                                entranceX = newX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0);
                            }
                        }

                        // For icons that normally would be placed in 0,0 leave them there based on the left edge dimension
                        // or hide them entirely if the fix is applied
                        if (gMapData->owEntranceIconPosY[sEntranceIconMapIndex] == 0) {
                            entranceY = 0;
                            entranceX = CVarGetInteger("gFixDungeonMinimapIcon", 0) ? -9999 : OTRGetRectDimensionFromLeftEdge(0);
                        }

                        //! @bug UB: sEntranceIconMapIndex can be up to 23 and is accessing owEntranceFlag which is size 20
                        if ((gMapData->owEntranceFlag[sEntranceIconMapIndex] == 0xFFFF) ||
                            ((gMapData->owEntranceFlag[sEntranceIconMapIndex] != 0xFFFF) &&
                              ((gSaveContext.infTable[26] & gBitFlags[gMapData->owEntranceFlag[mapIndex]]) ||
                               CVarGetInteger("gAlwaysShowDungeonMinimapIcon", 0)))) {
                            gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                                                iconSize, iconSize, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                            gSPWideTextureRectangle(OVERLAY_DISP++, entranceX << 2, entranceY << 2, (entranceX + iconSize) << 2,
                                                    (entranceY + iconSize) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                        }
                    }

                    s16 origX = CVarGetInteger("gMirroredWorld", 0) ? 256 : 270;
                    s16 entranceX = OTRGetRectDimensionFromRightEdge(origX + X_Margins_Minimap);
                    s16 entranceY = 154 + Y_Margins_Minimap;
                    if (CVarGetInteger("gMinimapPosType", 0) != 0) {
                        entranceY = 154 + Y_Margins_Minimap + CVarGetInteger("gMinimapPosY", 0);
                        if (CVarGetInteger("gMinimapPosType", 0) == 1) {//Anchor Left
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Left_MM_Margin;};
                            entranceX = OTRGetRectDimensionFromLeftEdge(origX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0));
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 2) {//Anchor Right
                            if (CVarGetInteger("gMinimapUseMargins", 0) != 0) {X_Margins_Minimap = Right_MM_Margin;};
                            entranceX = OTRGetRectDimensionFromRightEdge(origX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0));
                        } else if (CVarGetInteger("gMinimapPosType", 0) == 3) {//Anchor None
                            entranceX = origX + X_Margins_Minimap + CVarGetInteger("gMinimapPosX", 0);
                        }
                    }

                    // Ice Cavern entrance icon
                    if ((play->sceneNum == SCENE_ZORAS_FOUNTAIN) && ((gSaveContext.infTable[26] & gBitFlags[9]) ||
                        CVarGetInteger("gAlwaysShowDungeonMinimapIcon", 0))) {
                        gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, iconSize,
                                            iconSize, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                        gSPWideTextureRectangle(OVERLAY_DISP++, entranceX << 2, entranceY << 2, (entranceX + iconSize) << 2,
                                               (entranceY + iconSize) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                    }

                    Minimap_DrawCompassIcons(play); // Draw icons for the player spawn and current position
                }

                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_L) && !Play_InCsMode(play) && enableMapToggle) {
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

    CLOSE_DISPS(play->state.gfxCtx);
}

s16 Map_GetFloorTextIndexOffset(s32 mapIndex, s32 floor) {
    return gMapData->floorTexIndexOffset[mapIndex][floor];
}

void Map_Update(PlayState* play) {
    static s16 sLastRoomNum = 99;
    Player* player = GET_PLAYER(play);
    s32 mapIndex = gSaveContext.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 floor;
    s16 i;

    Top_MM_Margin = CVarGetInteger("gHUDMargin_T", 0);
    Left_MM_Margin = CVarGetInteger("gHUDMargin_L", 0);
    Right_MM_Margin = CVarGetInteger("gHUDMargin_R", 0);
    Bottom_MM_Margin = CVarGetInteger("gHUDMargin_B", 0);

    if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0)) {
        switch (play->sceneNum) {
            case SCENE_DEKU_TREE:
            case SCENE_DODONGOS_CAVERN:
            case SCENE_JABU_JABU:
            case SCENE_FOREST_TEMPLE:
            case SCENE_FIRE_TEMPLE:
            case SCENE_WATER_TEMPLE:
            case SCENE_SPIRIT_TEMPLE:
            case SCENE_SHADOW_TEMPLE:
            case SCENE_BOTTOM_OF_THE_WELL:
            case SCENE_ICE_CAVERN:
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
                        Map_InitData(play, interfaceCtx->mapRoomNum);
                        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
                        Map_SavePlayerInitialInfo(play);
                    }
                }

                VREG(10) = interfaceCtx->mapRoomNum;
                break;
            case SCENE_DEKU_TREE_BOSS:
            case SCENE_DODONGOS_CAVERN_BOSS:
            case SCENE_JABU_JABU_BOSS:
            case SCENE_FOREST_TEMPLE_BOSS:
            case SCENE_FIRE_TEMPLE_BOSS:
            case SCENE_WATER_TEMPLE_BOSS:
            case SCENE_SPIRIT_TEMPLE_BOSS:
            case SCENE_SHADOW_TEMPLE_BOSS:
                VREG(30) = gMapData->bossFloor[play->sceneNum - SCENE_DEKU_TREE_BOSS];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE +
                                  gMapData->floorTexIndexOffset[play->sceneNum - SCENE_DEKU_TREE_BOSS][VREG(30)];
                break;
        }
    }
}
