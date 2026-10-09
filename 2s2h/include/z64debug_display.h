#ifndef Z64DEBUG_DISPLAY_H
#define Z64DEBUG_DISPLAY_H

#include "color.h"
#include "ultra64.h"
#include "z64math.h"

struct GraphicsContext;
struct PlayState;

typedef struct DebugDispObject {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3s rot;
    /* 0x14 */ Vec3f scale;
    /* 0x20 */ Color_RGBA8 color;
    /* 0x24 */ s16 type;
    /* 0x28 */ struct DebugDispObject* next;
    /* 0x2C */ s32 pad; // Padding is not in the OoT version
} DebugDispObject; // size = 0x30

DebugDispObject* DebugDisplay_Init(void);
void DebugDisplay_DrawObjects(struct PlayState* play);
DebugDispObject* DebugDisplay_AddObject(f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, f32 scaleX, f32 scaleY, f32 scaleZ, u8 red, u8 green, u8 blue, u8 alpha, s16 type, struct GraphicsContext* gfxCtx);

#endif
