#ifndef SYS_CFB_H
#define SYS_CFB_H

#include "ultra64.h"
#include "macros.h"

void SysCfb_SetLoResMode(void);
void SysCfb_SetHiResMode(void);
void SysCfb_Init(void);
void* SysCfb_GetFramebuffer(s32 index);
void* SysCfb_GetZBuffer(void);
void* SysCfb_GetWorkBuffer(void);
u16 SysCfb_GetZBufferPixel(s32 x, s32 y);
s32 SysCfb_GetZBufferInt(s32 x, s32 y);

extern void* gFramebuffers[2];
extern OSViMode* gActiveViMode;
extern u16* gZBufferPtr;
extern void* gWorkBuffer;
extern u64* gGfxSPTaskOutputBufferPtr;
extern void* gGfxSPTaskOutputBufferEnd;

extern u16* gZBufferLoRes[SCREEN_WIDTH * SCREEN_HEIGHT];
extern u16* gWorkBufferLoRes[SCREEN_WIDTH * SCREEN_HEIGHT];
extern u64* gGfxSPTaskOutputBufferLoRes[0x3000];
extern void* gGfxSPTaskOutputBufferEndLoRes;

extern u16* gZBufferHiRes[HIRES_BUFFER_WIDTH * HIRES_BUFFER_HEIGHT];
extern u16* gWorkBufferHiRes[HIRES_BUFFER_WIDTH * HIRES_BUFFER_HEIGHT];
extern u64* gGfxSPTaskOutputBufferHiRes[0x3000];
extern void* gGfxSPTaskOutputBufferEndHiRes;
extern s16 gCfbWidth;
extern s16 gCfbHeight;
extern s16 gCfbLeftAdjust;
extern s16 gCfbUpperAdjust;
extern u8 gSysCfbHiResEnabled;

#endif
