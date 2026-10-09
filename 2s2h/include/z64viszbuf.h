#ifndef Z64_VISZBUF_H
#define Z64_VISZBUF_H

#include "ultra64.h"
#include "color.h"

typedef struct VisZbuf {
    /* 0x0 */ u32 useRgba;
    /* 0x4 */ u32 setScissor;
    /* 0x8 */ Color_RGBA8_u32 primColor;
    /* 0xC */ Color_RGBA8_u32 envColor;
} VisZbuf; // size = 0x10

void VisZbuf_Init(VisZbuf* thisx);
void VisZbuf_Destroy(VisZbuf* thisx);
void VisZbuf_Draw(VisZbuf* thisx, Gfx** gfxP, void* zbuffer);

#endif
