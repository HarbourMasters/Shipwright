#ifndef Z64DEBUG_TEXT_H
#define Z64DEBUG_TEXT_H

#include "ultra64.h"

struct GraphicsContext;

typedef enum {
    /* 0 */ DEBUG_TEXT_BLACK,
    /* 1 */ DEBUG_TEXT_BLUE,
    /* 2 */ DEBUG_TEXT_RED,
    /* 3 */ DEBUG_TEXT_PURPLE,
    /* 4 */ DEBUG_TEXT_GREEN,
    /* 5 */ DEBUG_TEXT_CYAN,
    /* 6 */ DEBUG_TEXT_YELLOW,
    /* 7 */ DEBUG_TEXT_WHITE
} DebugTextColor;

typedef enum {
    /* 0 */ DEBUG_CAM_TEXT_YELLOW,
    /* 1 */ DEBUG_CAM_TEXT_PEACH,
    /* 2 */ DEBUG_CAM_TEXT_BROWN,
    /* 3 */ DEBUG_CAM_TEXT_ORANGE,
    /* 4 */ DEBUG_CAM_TEXT_GOLD,
    /* 5 */ DEBUG_CAM_TEXT_WHITE,
    /* 6 */ DEBUG_CAM_TEXT_BLUE,
    /* 7 */ DEBUG_CAM_TEXT_GREEN
} DebugCamTextColor;

void Debug_ClearTextDrawFlags(void);
void Debug_ScreenText(s32 index, s32 colorIndex, const char* text);
void DebugCamera_ScreenText(u8 x, u8 y, u8 colorIndex, const char* text);
void Debug_DrawText(struct GraphicsContext* gfxCtx);


#endif
