#ifndef Z64_VIS_H
#define Z64_VIS_H

// #include "ultra64.h"
// #include "color.h"
#include <libultraship/libultra.h>


#ifdef __cplusplus
#define this thisx
extern "C"
{
#endif

typedef enum FramebufferFilterType {
    /* 0 */ FB_FILTER_NONE,
    /* 1 */ FB_FILTER_CVG_RGB,
    /* 2 */ FB_FILTER_CVG_RGB_UNIFORM,
    /* 3 */ FB_FILTER_CVG_ONLY,
    /* 4 */ FB_FILTER_CVG_RGB_FOG, // Not recommended, easily overflows blender
    /* 5 */ FB_FILTER_ZBUF_IA,
    /* 6 */ FB_FILTER_ZBUF_RGBA,
    /* 7 */ FB_FILTER_MONO
} FramebufferFilterType;

typedef enum VisScissorType {
    /* 0 */ VIS_NO_SETSCISSOR,
    /* 1 */ VIS_SETSCISSOR
} VisScissorType;

typedef struct Vis {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 scissorType;
    /* 0x08 */ Color_RGBA8_u32 primColor;
    /* 0x0C */ Color_RGBA8_u32 envColor;
} Vis; // size = 0x10



/* Cvg: Coverage */

#define FB_FILTER_TO_CVG_TYPE(filter) (filter)

typedef enum VisCvgType {
    /* 0 */ VIS_CVG_TYPE_NONE = FB_FILTER_TO_CVG_TYPE(FB_FILTER_NONE),
    /* 1 */ VIS_CVG_TYPE_CVG_RGB = FB_FILTER_TO_CVG_TYPE(FB_FILTER_CVG_RGB),
    /* 2 */ VIS_CVG_TYPE_CVG_RGB_UNIFORM = FB_FILTER_TO_CVG_TYPE(FB_FILTER_CVG_RGB_UNIFORM),
    /* 3 */ VIS_CVG_TYPE_CVG_ONLY = FB_FILTER_TO_CVG_TYPE(FB_FILTER_CVG_ONLY),
    /* 4 */ VIS_CVG_TYPE_CVG_RGB_FOG = FB_FILTER_TO_CVG_TYPE(FB_FILTER_CVG_RGB_FOG)
} VisCvgType;

typedef struct VisCvg {
    /* 0x00 */ Vis vis;
} VisCvg; // size = 0x10

void VisCvg_Init(VisCvg* this);
void VisCvg_Destroy(VisCvg* this);
void VisCvg_Draw(VisCvg* this, Gfx** gfxP);



/* Mono: Desaturation */

// Only one type

typedef struct VisMono {
    /* 0x00 */ Vis vis;
    /* 0x10 */ u16* tlut;
    /* 0x14 */ Gfx* dList;
} VisMono; // size = 0x18

void VisMono_Init(VisMono* this);
void VisMono_Destroy(VisMono* this);
void VisMono_Draw(VisMono* this, Gfx** gfxP);



/* ZBuf: Z-Buffer */

#define FB_FILTER_TO_ZBUF_TYPE(filter) ((filter) - FB_FILTER_ZBUF_IA)

typedef enum VisZBufType {
    /* 0 */ VIS_ZBUF_TYPE_IA = FB_FILTER_TO_ZBUF_TYPE(FB_FILTER_ZBUF_IA),
    /* 1 */ VIS_ZBUF_TYPE_RGBA = FB_FILTER_TO_ZBUF_TYPE(FB_FILTER_ZBUF_RGBA)
} VisZBufType;

typedef struct VisZBuf {
    /* 0x00 */ Vis vis;
} VisZBuf; // size = 0x10

void VisZBuf_Init(VisZBuf* this);
void VisZBuf_Destroy(VisZBuf* this);
void VisZBuf_Draw(VisZBuf* this, Gfx** gfxP);


#ifdef __cplusplus
#undef this
}
#endif

#endif
