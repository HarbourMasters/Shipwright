#ifndef Z64_VISMONO_H
#define Z64_VISMONO_H

#include "ultra64.h"
#include "color.h"

typedef struct VisMono {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u32 setScissor;
    /* 0x08 */ Color_RGBA8_u32 primColor;
    /* 0x0C */ Color_RGBA8_u32 envColor;
    /* 0x10 */ u16* tlut;
    /* 0x14 */ Gfx* dList;
} VisMono; // size = 0x18

void VisMono_Init(VisMono* thisx);
void VisMono_Destroy(VisMono* thisx);
void VisMono_Draw(VisMono* thisx, Gfx** gfxp);

#endif
