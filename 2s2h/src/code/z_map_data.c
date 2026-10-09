#include "global.h"
#include "z64map.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

typedef struct {
    /* 0x0 */ u8 texWidth;
    /* 0x1 */ u8 texHeight;
    /* 0x2 */ s16 offsetX;
    /* 0x4 */ s16 offsetY;
    /* 0x6 */ u8 drawType;
    /* 0x7 */ u8 colorIndex;
    /* 0x8 */ s16 scale;
} MapSpriteInfo; // size = 0xA

typedef struct {
    /* 0x0 */ TexturePtr spriteTex;
    /* 0x4 */ u8 width;
    /* 0x5 */ u8 height;
    /* 0x6 */ u8 offsetX;
    /* 0x7 */ u8 offsetY;
    /* 0x8 */ u8 drawType;
    /* 0x9 */ u8 colorIndex;
    /* 0xA */ s16 scale;
} MapSpriteInfo2; // size = 0xC

static Color_RGBA8 sMapColorTable[] = {
    { 255, 255, 255, 255 },
    { 0, 255, 255, 160 }, // Blue
    { 100, 255, 255, 255 },
};

static MapSpriteInfo2 sGameplayDangeonKeepInfo[5] = {
    { gameplay_dangeon_keep_Tex_0022C8, 96, 85, 48, 42, MAPDATA_DRAW_0, 0, 20 },
    { gameplay_dangeon_keep_Tex_0042C8, 96, 85, 48, 62, MAPDATA_DRAW_0, 0, 20 },
    { gameplay_dangeon_keep_Tex_0052C8, 96, 85, 68, 42, MAPDATA_DRAW_0, 0, 20 },
    { gameplay_dangeon_keep_Tex_0032C8, 96, 85, 48, 62, MAPDATA_DRAW_0, 0, 20 },
    { gameplay_dangeon_keep_Tex_0062C8, 96, 85, 68, 62, MAPDATA_DRAW_0, 0, 20 },
};

static MapSpriteInfo sMapGrandStaticInfo[98] = {
    { 96, 85, 48, 42, MAPDATA_DRAW_0, 0, 20 },   { 96, 85, 48, 62, MAPDATA_DRAW_0, 0, 20 },
    { 96, 85, 68, 42, MAPDATA_DRAW_0, 0, 20 },   { 96, 85, 48, 62, MAPDATA_DRAW_0, 0, 20 },
    { 96, 85, 68, 62, MAPDATA_DRAW_0, 0, 20 },   { 80, 72, 40, 35, MAPDATA_DRAW_1, 1, 52 },
    { 96, 85, 49, 43, MAPDATA_DRAW_3, 2, 20 },   { 96, 85, 49, 43, MAPDATA_DRAW_3, 2, 20 },
    { 96, 85, 49, 43, MAPDATA_DRAW_3, 2, 20 },   { 96, 85, 49, 49, MAPDATA_DRAW_3, 2, 20 },
    { 48, 44, 25, 23, MAPDATA_DRAW_3, 2, 20 },   { 96, 85, 49, 43, MAPDATA_DRAW_3, 2, 20 },
    { 48, 46, 25, 24, MAPDATA_DRAW_3, 2, 20 },   { 64, 61, 32, 31, MAPDATA_DRAW_3, 2, 20 },
    { 96, 85, 49, 50, MAPDATA_DRAW_3, 2, 20 },   { 96, 85, 49, 62, MAPDATA_DRAW_3, 2, 20 },
    { 96, 85, 49, 43, MAPDATA_DRAW_3, 2, 20 },   { 48, 80, 21, 41, MAPDATA_DRAW_1, 1, 198 },
    { 32, 128, 17, 87, MAPDATA_DRAW_1, 1, 138 }, { 80, 53, 40, 27, MAPDATA_DRAW_1, 1, 54 },
    { 32, 81, 16, 15, MAPDATA_DRAW_3, 2, 20 },   { 80, 45, 35, 23, MAPDATA_DRAW_3, 2, 20 },
    { 64, 87, 36, 44, MAPDATA_DRAW_3, 2, 20 },   { 64, 51, 31, 23, MAPDATA_DRAW_3, 2, 20 },
    { 80, 79, 40, 40, MAPDATA_DRAW_3, 2, 20 },   { 96, 75, 36, 26, MAPDATA_DRAW_3, 2, 20 },
    { 48, 41, 24, 20, MAPDATA_DRAW_3, 2, 20 },   { 64, 45, 31, 20, MAPDATA_DRAW_3, 2, 20 },
    { 80, 52, 38, 28, MAPDATA_DRAW_3, 2, 20 },   { 48, 45, 23, 20, MAPDATA_DRAW_3, 2, 20 },
    { 48, 59, 25, 37, MAPDATA_DRAW_3, 2, 20 },   { 48, 60, 21, 38, MAPDATA_DRAW_3, 2, 20 },
    { 48, 81, 24, 40, MAPDATA_DRAW_3, 2, 20 },   { 48, 81, 24, 40, MAPDATA_DRAW_3, 2, 20 },
    { 80, 89, 41, 26, MAPDATA_DRAW_3, 2, 20 },   { 80, 53, 39, 26, MAPDATA_DRAW_3, 2, 20 },
    { 64, 78, 37, 36, MAPDATA_DRAW_3, 2, 20 },   { 64, 68, 33, 33, MAPDATA_DRAW_3, 2, 20 },
    { 48, 68, 23, 35, MAPDATA_DRAW_3, 2, 20 },   { 48, 36, 25, 17, MAPDATA_DRAW_3, 2, 20 },
    { 48, 25, 24, 12, MAPDATA_DRAW_3, 2, 20 },   { 64, 75, 24, 37, MAPDATA_DRAW_3, 2, 20 },
    { 32, 53, 17, 26, MAPDATA_DRAW_3, 2, 20 },   { 80, 62, 35, 27, MAPDATA_DRAW_1, 1, 105 },
    { 64, 76, 31, 31, MAPDATA_DRAW_1, 1, 133 },  { 96, 85, 47, 43, MAPDATA_DRAW_3, 2, 20 },
    { 80, 71, 48, 33, MAPDATA_DRAW_1, 1, 180 },  { 80, 87, 44, 44, MAPDATA_DRAW_1, 1, 72 },
    { 64, 65, 31, 36, MAPDATA_DRAW_1, 1, 57 },   { 48, 41, 28, 20, MAPDATA_DRAW_1, 1, 60 },
    { 64, 76, 17, 36, MAPDATA_DRAW_1, 1, 58 },   { 80, 60, 27, 22, MAPDATA_DRAW_1, 1, 83 },
    { 80, 47, 38, 23, MAPDATA_DRAW_1, 1, 69 },   { 80, 60, 40, 38, MAPDATA_DRAW_1, 1, 29 },
    { 48, 19, 17, 8, MAPDATA_DRAW_1, 1, 25 },    { 32, 99, 17, 16, MAPDATA_DRAW_1, 1, 88 },
    { 48, 69, 19, 28, MAPDATA_DRAW_1, 1, 76 },   { 48, 83, 34, 47, MAPDATA_DRAW_1, 1, 104 },
    { 80, 54, 45, 27, MAPDATA_DRAW_1, 1, 61 },   { 80, 92, 35, 50, MAPDATA_DRAW_1, 1, 60 },
    { 48, 75, 24, 37, MAPDATA_DRAW_3, 2, 20 },   { 64, 49, 32, 23, MAPDATA_DRAW_3, 2, 20 },
    { 64, 49, 25, 24, MAPDATA_DRAW_3, 2, 20 },   { 64, 52, 32, 24, MAPDATA_DRAW_3, 2, 34 },
    { 64, 49, 31, 24, MAPDATA_DRAW_3, 2, 34 },   { 32, 33, 18, 14, MAPDATA_DRAW_3, 2, 34 },
    { 48, 50, 25, 25, MAPDATA_DRAW_3, 2, 34 },   { 64, 52, 32, 25, MAPDATA_DRAW_3, 2, 34 },
    { 48, 35, 24, 18, MAPDATA_DRAW_3, 2, 34 },   { 64, 51, 32, 26, MAPDATA_DRAW_3, 2, 34 },
    { 48, 35, 24, 18, MAPDATA_DRAW_3, 2, 34 },   { 48, 53, 24, 28, MAPDATA_DRAW_3, 2, 34 },
    { 64, 53, 33, 25, MAPDATA_DRAW_3, 2, 34 },   { 64, 47, 31, 24, MAPDATA_DRAW_3, 2, 34 },
    { 64, 52, 32, 35, MAPDATA_DRAW_3, 2, 34 },   { 48, 48, 24, 23, MAPDATA_DRAW_3, 2, 34 },
    { 64, 87, 32, 26, MAPDATA_DRAW_3, 2, 34 },   { 48, 30, 24, 14, MAPDATA_DRAW_3, 2, 34 },
    { 80, 74, 40, 36, MAPDATA_DRAW_1, 1, 50 },   { 80, 87, 6, 41, MAPDATA_DRAW_1, 1, 42 },
    { 80, 56, 19, 27, MAPDATA_DRAW_1, 1, 40 },   { 80, 65, 25, 33, MAPDATA_DRAW_1, 1, 40 },
    { 16, 38, 8, 12, MAPDATA_DRAW_1, 1, 47 },    { 80, 78, 46, 33, MAPDATA_DRAW_1, 1, 142 },
    { 80, 55, 32, 29, MAPDATA_DRAW_1, 1, 155 },  { 80, 98, 67, 48, MAPDATA_DRAW_1, 1, 51 },
    { 80, 62, 47, 19, MAPDATA_DRAW_1, 1, 55 },   { 32, 123, 15, 83, MAPDATA_DRAW_1, 1, 140 },
    { 80, 60, 47, 14, MAPDATA_DRAW_1, 1, 170 },  { 48, 77, 18, 59, MAPDATA_DRAW_1, 1, 79 },
    { 80, 64, 35, 30, MAPDATA_DRAW_1, 1, 33 },   { 64, 50, 29, 22, MAPDATA_DRAW_1, 1, 40 },
    { 80, 65, 39, 5, MAPDATA_DRAW_1, 1, 37 },    { 32, 61, 8, 56, MAPDATA_DRAW_1, 1, 35 },
    { 80, 83, 37, 42, MAPDATA_DRAW_1, 1, 95 },   { 80, 27, 30, 21, MAPDATA_DRAW_1, 1, 134 },
    { 80, 23, 122, -18, MAPDATA_DRAW_1, 1, 53 }, { 80, 60, 27, 22, MAPDATA_DRAW_1, 1, 83 },
};

static s32 D_801BF15C[5] = {
    0x50, 0x50, 0x8D, 0x50, 0x50,
};

static MapSpriteInfo sMapIStaticInfo[MAPDATA_MAP_I_MAX] = {
    { 96, 85, 73, 67, MAPDATA_DRAW_0, 0, 60 }, { 96, 85, 73, 74, MAPDATA_DRAW_0, 0, 60 },
    { 96, 85, 80, 67, MAPDATA_DRAW_0, 0, 60 }, { 96, 85, 73, 67, MAPDATA_DRAW_0, 0, 60 },
    { 96, 85, 80, 74, MAPDATA_DRAW_0, 0, 60 }, { 32, 29, 17, 14, MAPDATA_DRAW_2, 0, 50 },
    { 32, 27, 16, 13, MAPDATA_DRAW_2, 0, 50 }, { 32, 30, 16, 15, MAPDATA_DRAW_2, 0, 50 },
    { 16, 32, 8, 20, MAPDATA_DRAW_2, 0, 50 },  { 16, 12, 8, 6, MAPDATA_DRAW_2, 0, 50 },
    { 32, 27, 16, 13, MAPDATA_DRAW_2, 0, 50 }, { 16, 14, 8, 7, MAPDATA_DRAW_2, 0, 50 },
    { 32, 19, 16, 9, MAPDATA_DRAW_2, 0, 50 },  { 32, 26, 16, 16, MAPDATA_DRAW_2, 0, 50 },
    { 16, 32, 8, 22, MAPDATA_DRAW_2, 0, 50 },  { 32, 27, 16, 13, MAPDATA_DRAW_2, 0, 50 },
    { 16, 35, 9, 5, MAPDATA_DRAW_2, 0, 45 },   { 32, 18, 14, 9, MAPDATA_DRAW_2, 0, 45 },
    { 32, 38, 18, 19, MAPDATA_DRAW_2, 0, 45 }, { 32, 21, 16, 9, MAPDATA_DRAW_2, 0, 45 },
    { 32, 34, 16, 17, MAPDATA_DRAW_2, 0, 45 }, { 48, 32, 19, 11, MAPDATA_DRAW_2, 0, 45 },
    { 32, 17, 16, 8, MAPDATA_DRAW_2, 0, 45 },  { 32, 17, 16, 7, MAPDATA_DRAW_2, 0, 45 },
    { 32, 21, 16, 11, MAPDATA_DRAW_2, 0, 45 }, { 32, 18, 16, 8, MAPDATA_DRAW_2, 0, 45 },
    { 16, 24, 9, 15, MAPDATA_DRAW_2, 0, 45 },  { 16, 25, 7, 16, MAPDATA_DRAW_2, 0, 45 },
    { 32, 35, 16, 17, MAPDATA_DRAW_2, 0, 45 }, { 16, 19, 5, 9, MAPDATA_DRAW_2, 0, 80 },
    { 32, 28, 17, 7, MAPDATA_DRAW_2, 0, 66 },  { 32, 16, 16, 8, MAPDATA_DRAW_2, 0, 66 },
    { 32, 24, 18, 11, MAPDATA_DRAW_2, 0, 66 }, { 32, 21, 16, 10, MAPDATA_DRAW_2, 0, 66 },
    { 16, 21, 8, 11, MAPDATA_DRAW_2, 0, 66 },  { 16, 10, 8, 4, MAPDATA_DRAW_2, 0, 66 },
    { 16, 7, 8, 3, MAPDATA_DRAW_2, 0, 66 },    { 32, 23, 13, 11, MAPDATA_DRAW_2, 0, 66 },
    { 16, 16, 8, 8, MAPDATA_DRAW_2, 0, 66 },   { 16, 17, 8, 7, MAPDATA_DRAW_2, 0, 50 },
    { 16, 23, 9, 10, MAPDATA_DRAW_2, 0, 66 },  { 16, 15, 8, 7, MAPDATA_DRAW_2, 0, 66 },
    { 32, 15, 15, 7, MAPDATA_DRAW_2, 0, 66 },  { 32, 17, 16, 8, MAPDATA_DRAW_2, 0, 90 },
    { 32, 16, 16, 8, MAPDATA_DRAW_2, 0, 90 },  { 16, 10, 8, 5, MAPDATA_DRAW_2, 0, 90 },
    { 16, 17, 8, 8, MAPDATA_DRAW_2, 0, 90 },   { 32, 17, 17, 8, MAPDATA_DRAW_2, 0, 90 },
    { 16, 11, 8, 5, MAPDATA_DRAW_2, 0, 90 },   { 32, 17, 15, 9, MAPDATA_DRAW_2, 0, 90 },
    { 16, 12, 8, 6, MAPDATA_DRAW_2, 0, 90 },   { 16, 18, 9, 9, MAPDATA_DRAW_2, 0, 90 },
    { 32, 17, 17, 8, MAPDATA_DRAW_2, 0, 90 },  { 32, 15, 16, 8, MAPDATA_DRAW_2, 0, 90 },
    { 16, 18, 9, 13, MAPDATA_DRAW_2, 0, 90 },  { 16, 16, 8, 7, MAPDATA_DRAW_2, 0, 90 },
    { 16, 30, 9, 8, MAPDATA_DRAW_2, 0, 90 },   { 16, 9, 9, 4, MAPDATA_DRAW_2, 0, 90 },
};

static s32 sMapIForGameplayDangeonKeep[5] = {
    0x00, 0x01, 0x02, 0x03, 0x04,
};

static s32 sMapIForMapGrand[98] = {
    MAPDATA_MAP_I_MAX,
    0x01,
    0x02,
    0x03,
    0x04,
    MAPDATA_MAP_I_MAX,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0A,
    0x0B,
    0x0C,
    0x0D,
    0x0E,
    0x0F,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1A,
    0x1B,
    0x1C,
    MAPDATA_MAP_I_MAX,
    0x1E,
    0x1F,
    0x20,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    0x27,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
    0x2D,
    0x2E,
    0x2F,
    0x30,
    0x31,
    0x32,
    0x33,
    0x34,
    0x35,
    0x36,
    0x37,
    0x38,
    0x39,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
    MAPDATA_MAP_I_MAX,
};

void MapData_GetMapColor(s32 colorIndex, Color_RGBA8* color) {
    *color = sMapColorTable[colorIndex];
}

void MapData_GetMapTexDimGameplayDangeonKeep(s32 mapId, s32* width, s32* height) {
    *width = sGameplayDangeonKeepInfo[mapId].width;
    *height = sGameplayDangeonKeepInfo[mapId].height;
}

void MapData_GetMapTexOffsetGameplayDangeonKeep(s32 mapId, s32* offsetX, s32* offsetY) {
    *offsetX = sGameplayDangeonKeepInfo[mapId].offsetX;
    *offsetY = sGameplayDangeonKeepInfo[mapId].offsetY;
}

void MapData_GetMapDrawTypeGameplayDangeonKeep(s32 mapId, s32* drawType) {
    *drawType = sGameplayDangeonKeepInfo[mapId].drawType;
}

u8 MapData_GetMapColorIndexGameplayDangeonKeep(s32 mapId) {
    return sGameplayDangeonKeepInfo[mapId].colorIndex;
}

s16 MapData_GetMapGameplayDangeonKeepScale(s32 mapId) {
    return sGameplayDangeonKeepInfo[mapId].scale;
}

TexturePtr MapData_GetMapTexGameplayDangeonKeep(s32 mapId) {
    return sGameplayDangeonKeepInfo[mapId].spriteTex;
}

void MapData_GetMapTexGameplayDangeonKeepDim(s32 mapId, s32* width, s32* height) {
    *width = sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].texWidth;
    *height = sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].texHeight;
}

void MapData_GetMapTexGameplayDangeonKeepOffset(s32 mapId, s32* offsetX, s32* offsetY) {
    *offsetX = sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].offsetX;
    *offsetY = sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].offsetY;
}

void MapData_GetMapGrandDrawType(s32 mapId, s32* drawType) {
    *drawType = sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].drawType;
}

u8 MapData_GetMapGrandColorIndex(s32 mapId) {
    return sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].colorIndex;
}

s16 MapData_GetMapGrandScale(s32 mapId) {
    return sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].scale;
}

s32 MapData_MapGrandTexSizeTest(s32 mapId) {
    s32 endMap = mapId - MAPDATA_MAP_GRAND;
    s32 size = 0;
    s32 i;

    for (i = 1; i < endMap + 1; i++) {
        size += MapData_GetSizeOfMapGrandTex((i - 1) + MAPDATA_MAP_GRAND);
    }
    return size;
}

s32 MapData_GetSizeOfMapGrandTex(s32 mapId) {
    return (sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].texWidth *
            sMapGrandStaticInfo[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)].texHeight) /
           2;
}

s32 MapData_MID_GetType(s32 mapId) {
    if ((mapId >= 0) && (mapId < 5)) {
        return MAPDATA_MID_GAMEPLAY_DANGEON_KEEP;
    }
    if ((mapId >= 0x100) && (mapId < 0x162)) {
        return MAPDATA_MID_MAP_GRAND_STATIC;
    }
    return MAPDATA_MID_MAP_I_STATIC;
}

void MapData_GetMapTexDim(s32 mapId, s32* width, s32* height) {
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            MapData_GetMapTexDimGameplayDangeonKeep(mapId, width, height);
            return;

        case MAPDATA_MID_MAP_GRAND_STATIC:
            MapData_GetMapTexGameplayDangeonKeepDim(mapId, width, height);
            return;

        default:
            *width = *height = 0;
            return;
    }
}

void MapData_GetMapTexOffset(s32 mapId, s32* offsetX, s32* offsetY) {
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            MapData_GetMapTexOffsetGameplayDangeonKeep(mapId, offsetX, offsetY);
            return;

        case MAPDATA_MID_MAP_GRAND_STATIC:
            MapData_GetMapTexGameplayDangeonKeepOffset(mapId, offsetX, offsetY);
            return;

        default:
            *offsetX = *offsetY = 0;
            return;
    }
}

void MapData_GetMapScale(s32 mapId, s32* scale) {
    switch (MapData_MID_GetType(mapId)) {
        default:
            *scale = 0;
            return;

        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            *scale = MapData_GetMapGameplayDangeonKeepScale(mapId);
            return;

        case MAPDATA_MID_MAP_GRAND_STATIC:
            *scale = MapData_GetMapGrandScale(mapId);
            return;
    }
}

void MapData_GetDrawType(s32 mapId, s32* drawType) {
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            MapData_GetMapDrawTypeGameplayDangeonKeep(mapId, drawType);
            return;

        case MAPDATA_MID_MAP_GRAND_STATIC:
            MapData_GetMapGrandDrawType(mapId, drawType);
            return;

        default:
            *drawType = 0;
            return;
    }
}

s32 MapData_GetMapColorIndex(s32 mapId) {
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            return MapData_GetMapColorIndexGameplayDangeonKeep(mapId);

        case MAPDATA_MID_MAP_GRAND_STATIC:
            return MapData_GetMapGrandColorIndex(mapId);

        default:
            return 0;
    }
}

s32 func_80109964(s32 arg0) {
    return D_801BF15C[arg0];
}

void MapData_GetMapITexDim(s32 mapId, s32* width, s32* height) {
    *width = sMapIStaticInfo[mapId].texWidth;
    *height = sMapIStaticInfo[mapId].texHeight;
}

void MapData_GetMapITexOffset(s32 mapId, s32* offsetX, s32* offsetY) {
    *offsetX = sMapIStaticInfo[mapId].offsetX;
    *offsetY = sMapIStaticInfo[mapId].offsetY;
}

void MapData_GetMapIDrawType(s32 mapId, s32* drawType) {
    *drawType = sMapIStaticInfo[mapId].drawType;
}

u8 MapData_GetMapIColorIndex(s32 mapId) {
    return sMapIStaticInfo[mapId].colorIndex;
}

s16 MapData_GetMapIScale(s32 mapId) {
    return sMapIStaticInfo[mapId].scale;
}

s32 MapData_MapITexSizeTest(s32 mapId) {
    s32 i;
    s32 size = 0;

    for (i = 1; i < mapId + 1; i++) {
        size += MapDisp_GetSizeOfMapITex(i - 1);
    }

    return size;
}

s32 MapDisp_GetSizeOfMapITex(s32 mapCompactId) {
    return (sMapIStaticInfo[mapCompactId].texWidth * sMapIStaticInfo[mapCompactId].texHeight) / 2;
}

s32 MapData_GetMapIId(s32 mapId) {
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            return sMapIForGameplayDangeonKeep[mapId];

        case MAPDATA_MID_MAP_GRAND_STATIC:
            return sMapIForMapGrand[MAPDATA_GET_MAP_GRAND_ID_FROM_MAP_ID(mapId)];

        default:
            return 0;
    }
}

s32 func_80109B38(s32 mapId) {
    if (MapData_GetMapIId(mapId) != MAPDATA_MAP_I_MAX) {
        return 1;
    }
    switch (MapData_MID_GetType(mapId)) {
        case MAPDATA_MID_GAMEPLAY_DANGEON_KEEP:
            return 2;

        case MAPDATA_MID_MAP_GRAND_STATIC:
            return 0;

        default:
            return 0;
    }
}

s32 MapData_GetMapCompactId(s32 mapId) {
    s32 mapI = MapData_GetMapIId(mapId);

    if (mapI != MAPDATA_MAP_I_MAX) {
        return mapI;
    }
    if ((mapId >= 0x100) && (mapId < 0x162)) {
        return mapId - 0xC6;
    }
    return -1;
}

s32 MapData_CPID_GetType(s32 mapCompactId) {
    if (mapCompactId == -1) {
        return MAPDATA_CPID_2;
    }
    if (mapCompactId < MAPDATA_MAP_I_MAX) {
        return MAPDATA_CPID_MAP_I_STATIC;
    }
    if (mapCompactId >= MAPDATA_MAP_I_MAX) {
        return MAPDATA_CPID_MAP_GRAND_STATIC;
    }
    return MAPDATA_CPID_2;
}

// Unused
s32 func_80109C38(s32 mapCompactId) {
    if (mapCompactId == -1) {
        return 0;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            return MapData_MapGrandTexSizeTest(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId));

        case MAPDATA_CPID_MAP_I_STATIC:
            return MapData_MapITexSizeTest(mapCompactId);

        case MAPDATA_CPID_2:
            return 0;

        default:
            return 0;
    }
}

s32 MapData_CPID_GetSizeOfMapTex(s32 mapCompactId) {
    if (mapCompactId == -1) {
        return 0;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            return MapData_GetSizeOfMapGrandTex(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId));

        case MAPDATA_CPID_MAP_I_STATIC:
            return MapDisp_GetSizeOfMapITex(mapCompactId);

        case MAPDATA_CPID_2:
            return 0;

        default:
            return 0;
    }
}

void MapData_CPID_GetTexDim(s32 mapCompactId, s32* width, s32* height) {
    if (mapCompactId == -1) {
        *width = *height = 0;
        return;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            MapData_GetMapTexGameplayDangeonKeepDim(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId), width,
                                                    height);
            return;

        case MAPDATA_CPID_MAP_I_STATIC:
            MapData_GetMapITexDim(mapCompactId, width, height);
            return;

        case MAPDATA_CPID_2:
        default:
            *width = *height = 0;
            return;
    }
}

void MapData_CPID_GetTexOffset(s32 mapCompactId, s32* offsetX, s32* offsetY) {
    if (mapCompactId == -1) {
        *offsetX = *offsetY = 0;
        return;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            MapData_GetMapTexGameplayDangeonKeepOffset(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId), offsetX,
                                                       offsetY);
            return;

        case MAPDATA_CPID_MAP_I_STATIC:
            MapData_GetMapITexOffset(mapCompactId, offsetX, offsetY);
            return;

        case MAPDATA_CPID_2:
        default:
            *offsetX = *offsetY = 0;
            return;
    }
}

// Unused
void MapData_CPID_GetDrawType(s32 mapCompactId, s32* drawType) {
    if (mapCompactId == -1) {
        *drawType = MAPDATA_DRAW_0;
        return;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            MapData_GetMapGrandDrawType(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId), drawType);
            return;

        case MAPDATA_CPID_MAP_I_STATIC:
            MapData_GetMapIDrawType(mapCompactId, drawType);
            return;

        case MAPDATA_CPID_2:
        default:
            *drawType = MAPDATA_DRAW_0;
            return;
    }
}

// Unused
u8 MapData_CPID_GetMapColorIndex(s32 mapCompactId) {
    if (mapCompactId == -1) {
        return 0;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            return MapData_GetMapGrandColorIndex(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId));

        case MAPDATA_CPID_MAP_I_STATIC:
            return MapData_GetMapIColorIndex(mapCompactId);

        case MAPDATA_CPID_2:
        default:
            return 0;
    }
}

s16 MapData_CPID_GetMapScale(s32 mapCompactId) {
    if (mapCompactId == -1) {
        return 0;
    }
    switch (MapData_CPID_GetType(mapCompactId)) {
        case MAPDATA_CPID_MAP_GRAND_STATIC:
            return MapData_GetMapGrandScale(MAPDATA_GET_MAP_GRAND_ID_FROM_COMPACT_ID(mapCompactId));

        case MAPDATA_CPID_MAP_I_STATIC:
            return MapData_GetMapIScale(mapCompactId);

        case MAPDATA_CPID_2:
        default:
            return 0;
    }
}
