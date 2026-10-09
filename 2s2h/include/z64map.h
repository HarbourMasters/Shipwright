#ifndef Z64MAP_H
#define Z64MAP_H
#include "ultra64.h"
#include "z64scene.h"

struct PlayState;

/*
Handles the minimap and pause screen dungeon map system.

z_map_data.c stores MapSpriteInfo and helper functions to "simplify" accessing this data

Two separate Id systems are used to access data

mapId
0x000-0x004 fetches minimap sprite data from gameplay_dangeon_keep
0x005-0x039 fetches dungeon map sprite data from map_i_static
0x100-0x161 fetches minimap sprite data from map_grand_static

mapCompactId
0x000-0x039 fetches dungeon map sprite data from map_i_static
0x03A-0x09B fetches minimap sprite data from map_grand_static
*/

#define FLOOR_INDEX_MAX 4
#define FLOOR_MIN_Y -32767

/* z_map_data */
#define MAPDATA_GAMEPLAY_DANGEON_KEEP_MAX 5
#define MAPDATA_MAP_I_MAX 0x3A
#define MAPDATA_MAP_GRAND 0x100
#define MAPDATA_MAP_GRAND_MAX 0x162
#define MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(id) ((id) + (MAPDATA_MAP_GRAND - MAPDATA_MAP_I_MAX))
#define MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId) ((mapId) - MAPDATA_MAP_GRAND)

#define MAPDATA_MID_GAMEPLAY_DANGEON_KEEP 0
#define MAPDATA_MID_MAP_GRAND_STATIC 1
#define MAPDATA_MID_MAP_I_STATIC 2

#define MAPDATA_CPID_MAP_GRAND_STATIC 0
#define MAPDATA_CPID_MAP_I_STATIC 1
#define MAPDATA_CPID_2 2

#define MAPDATA_DRAW_0 0
#define MAPDATA_DRAW_1 1
#define MAPDATA_DRAW_2 2
#define MAPDATA_DRAW_3 3

/* z_map_disp */
typedef struct {
    /* 0x00 */ MapDataScene* mapDataScene;
    /* 0x04 */ s32 curRoom;
    /* 0x08 */ s16 minimapBaseX;
    /* 0x0A */ s16 minimapBaseY;
    /* 0x0C */ s16 minimapCurX;
    /* 0x0E */ s16 minimapCurY;
    /* 0x10 */ TexturePtr minimapCurTex; // gameplay cur minimap room
    /* 0x14 */ s32 prevRoom;
    /* 0x18 */ TexturePtr minimapPrevTex;
    /* 0x1C */ s16 minimapPrevX; // for room swap animation
    /* 0x1E */ s16 minimapPrevY; // for room swap animation
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 swapAnimTimer;
    /* 0x28 */ void* texBuff0;
    /* 0x2C */ void* texBuff1;
    /* 0x30 */ s16 sceneMinX; //scene minBounds.x
    /* 0x32 */ s16 sceneMinZ; //scene minBounds.z
    /* 0x34 */ s16 sceneWidth; //scene boundsWidth.x
    /* 0x36 */ s16 sceneHeight; //scene boundsWidth.z
    /* 0x38 */ s16 sceneMidX; //scene boundsMidpoint.x
    /* 0x3A */ s16 sceneMidZ; //scene boundsMidpoint.z
    /* 0x3C */ s16* roomStoreyList; // list of lowest storey each room crosses
    /* 0x40 */ s16 numStoreys; // number of distinct storeys
    /* 0x42 */ s16 pauseMapCurStorey;
    /* 0x44 */ s16 bottomStorey; // configures what storey 0 is displayed as
    /* 0x48 */ s16* storeyYList; // list of min Ys for each storey
    /* 0x4C */ s16 timer;
    /* 0x50 */ s32 numChests;
    /* 0x54 */ MapDataChest* mapDataChests;
    /* 0x58 */ s16 bossRoomStorey;
    /* 0x5A */ s16 unk5A;
} MapDisp; // size = 0x5C

typedef struct {
    /* 0x000 */ s32 textureCount;
    /* 0x004 */ s32 mapI_mapCompactId[ROOM_MAX];
    /* 0x084 */ void* mapI_roomTextures[ROOM_MAX];
    /* 0x104 */ void* roomSprite[ROOM_MAX];
    /* 0x184 */ s32 animTimer;
} PauseDungeonMap; // size = 0x188

/* z_map_disp */
void func_80102EB4(u32 flag);
void func_80102ED0(u32 flag);
s32 MapDisp_CurRoomHasMapI(struct PlayState* play);
void MapDisp_Init(struct PlayState* play);
s32 MapDisp_GetBossIconY(void);
s16 MapDisp_GetBossRoomStorey(void);
void MapDisp_InitMapData(struct PlayState* play, void* segmentAddress);
void MapDisp_InitChestData(struct PlayState* play, s32 num, void* segmentAddress);
void MapDisp_InitTransitionActorData(struct PlayState* play, s32 num, TransitionActorEntry* transitionActorList);
void MapDisp_Destroy(struct PlayState* play);
void MapDisp_Update(struct PlayState* play);
void MapDisp_SwapRooms(s16 nextRoom);
s32 MapDisp_IsMinimapToggleBlocked(struct PlayState* play);
void MapDisp_DrawMinimap(struct PlayState* play, s32 playerInitX, s32 playerInitZ, s32 playerInitDir);
void* MapDisp_AllocDungeonMap(struct PlayState* play, void* heap);
void MapDisp_DrawDungeonFloorSelect(struct PlayState* play);
s32 MapDisp_IsValidStorey(s32 storey);
s32 MapDisp_GetPlayerStorey(s16 checkY);
void MapDisp_DrawDungeonMap(struct PlayState* play);
void MapDisp_UpdateDungeonMap(struct PlayState* play);

/* z_map_data */
void MapData_GetMapColor(s32 colorIndex, Color_RGBA8* color);
TexturePtr MapData_GetMapTexGameplayDangeonKeep(s32);
s32 MapData_GetMapCompactId(s32);
s32 MapData_MID_GetType(s32);
s32 MapData_CPID_GetSizeOfMapTex(s32);
void MapData_GetDrawType(s32, s32*);
s32 MapData_GetMapColorIndex(s32);
s32 MapDisp_GetSizeOfMapITex(s32 mapCompactId);
s32 MapData_GetMapIId(s32);
s32 MapData_GetSizeOfMapGrandTex(s32 mapId);
void MapData_GetMapTexDim(s32 mapId, s32* width, s32* height);
void MapData_GetMapTexOffset(s32 mapId, s32* offsetX, s32* offsetY);
void MapData_GetMapScale(s32 mapId, s32 *scale);
void MapData_CPID_GetTexDim(s32 mapCompactId, s32* width, s32* height);
void MapData_CPID_GetTexOffset(s32 mapCompactId, s32* offsetX, s32* offsetY);
s16 MapData_CPID_GetMapScale(s32 mapCompactId);

/* z_map_exp */
s32 Map_IsInDungeonOrBossScene(struct PlayState* play);
s32 Map_CurRoomHasMapI(struct PlayState* play);
s32 Map_IsInBossScene(struct PlayState* play);
void Map_SetAreaEntrypoint(struct PlayState* play);
void Map_InitRoomData(struct PlayState* play, s16 room);
void Map_Destroy(struct PlayState* play);
void Map_Init(struct PlayState* play);
void Map_DrawMinimap(struct PlayState* play);
void Map_Update(struct PlayState* play);

#endif
