#ifndef Z64_VISCVG_H
#define Z64_VISCVG_H

#include "ultra64.h"
#include "color.h"

typedef struct VisCvg {
    /* 0x0 */ u32 type;
    /* 0x4 */ u32 setScissor;
    /* 0x8 */ Color_RGBA8_u32 color;
    /* 0xC */ Color_RGBA8 envColor;
} VisCvg; // size = 0x10

void VisCvg_Init(VisCvg* thisx);
void VisCvg_Destroy(VisCvg* thisx);
void VisCvg_Draw(VisCvg* thisx, Gfx** gfxp);

#endif
