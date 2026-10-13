#ifndef Z64_VISFBUF_H
#define Z64_VISFBUF_H

#ifdef __cplusplus
extern "C" {
#define this thisx
#endif

#include "ultra64.h"
#include "color.h"
#include "PR/gs2dex.h"

typedef enum VisFbufBgMode {
    /* 0 */ VIS_FBUF_BG_CYC_1CYC, //!< Enable rescaling
    /* 1 */ VIS_FBUF_BG_CYC_COPY  //!< Direct copy, no scaling
} VisFbufBgMode;

typedef enum VisFbufMode {
    /* 0 */ VIS_FBUF_MODE_NONE,    //!< Do nothing but waste time loading microcode
    /* 1 */ VIS_FBUF_MODE_GENERAL, //!< Interpolation, filling and scaling
    /* 2 */ VIS_FBUF_MODE_INTERPOLATE
} VisFbufMode;

typedef struct VisFbuf {
    /* 0x00 */ u8 mode;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 lodProportion; // expected to be between 0.0f and 1.0f
    /* 0x0C */ Color_RGBA8_u32 primColor;
    /* 0x10 */ Color_RGBA8_u32 envColor;
} VisFbuf; // size = 0x14

void VisFbuf_Init(VisFbuf* this);
void VisFbuf_Destroy(VisFbuf* this);
void VisFbuf_DrawBgToColorImage(Gfx** gfxP, uObjBg* bg, void* img, s32 width, s32 height, VisFbufBgMode cycleMode);
void VisFbuf_SetBg(Gfx** gfxP, void* source, void* img, s32 width, s32 height, f32 x, f32 y, f32 scaleX, f32 scaleY,
                   VisFbufBgMode cycleMode);
void VisFbuf_SetBgSimple(Gfx** gfxP, void* source, void* img, s32 width, s32 height, VisFbufBgMode cycleMode);
void VisFbuf_SetBgGeneral(Gfx** gfxP, void* source, void* img, s32 width, s32 height, f32 x, f32 y, f32 scaleX,
                          f32 scaleY, VisFbufBgMode cycleMode);
void VisFbuf_ApplyEffects(VisFbuf* this, Gfx** gfxP, void* source, void* img, s32 width, s32 height);
void VisFbuf_DrawGeneral(VisFbuf* this, Gfx** gfxP, void* source, void* img, s32 width, s32 height);
void VisFbuf_DrawInterpolate(VisFbuf* this, Gfx** gfxP, void* img, s32 width, s32 height);
void VisFbuf_Draw(VisFbuf* this, Gfx** gfxP, void* img);

#ifdef __cplusplus
}
#undef this
#endif

#endif
