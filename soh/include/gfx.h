#ifndef GFX_H
#define GFX_H

#include <libultraship/libultra.h>
#include <libultraship/libultra/gbi.h>
#include "alignment.h"
#include "sched.h"
#include "thga.h"
#include "z64.h"

#include "extern_c_helper.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Texture memory size, 4 KiB
#define TMEM_SIZE 0x1000

// #region SoH [Interpolation]: Due to interpolation frames also using these buffers, we've doubled them in size
// TODO: Fix this

typedef struct GfxPool {
    /* 0x00000 */ u16 headMagic; // GFXPOOL_HEAD_MAGIC
    /* 0x00008 */ Gfx polyOpaBuffer[0x2FC0];
    /* 0x0BF08 */ Gfx polyXluBuffer[0x1000];
    /* 0x0FF08 */ Gfx overlayBuffer[0x800];
    /* 0x11F08 */ Gfx workBuffer[0x100];
    /* 0x11308 */ Gfx unusedBuffer[0x40];
    /* 0x12408 */ u16 tailMagic; // GFXPOOL_TAIL_MAGIC
} GfxPool; // size = 0x24820

// #endregion

typedef struct GraphicsContext {
    /* 0x0000 */ Gfx* polyOpaBuffer; // Pointer to "Zelda 0"
    /* 0x0004 */ Gfx* polyXluBuffer; // Pointer to "Zelda 1"
    /* 0x0008 */ char unk_008[0x08]; // Unused, could this be pointers to "Zelda 2" / "Zelda 3"
    /* 0x0010 */ Gfx* overlayBuffer; // Pointer to "Zelda 4"
    /* 0x0014 */ u32 unk_014;
    /* 0x0018 */ char unk_018[0x20];
    /* 0x0038 */ OSMesg msgBuff[0x08];
    /* 0x0058 */ OSMesgQueue* schedMsgQ; // ASDAFG: schedMsgQueue
    /* 0x005C */ OSMesgQueue queue;
    /* 0x0078 */ OSScTask task;
    /* 0x00E0 */ char unk_0D0[0xD0];
    /* 0x01B0 */ Gfx* workBuffer;
    /* 0x01B4 */ TwoHeadGfxArena work;
    /* 0x01C4 */ char unk_01C4[0xC0];
    /* 0x0284 */ OSViMode* viMode;
    /* 0x0288 */ char unk_0288[0x20]; // Unused, could this be Zelda 2/3 ?
    /* 0x02A8 */ TwoHeadGfxArena overlay; // "Zelda 4"
    /* 0x02B8 */ TwoHeadGfxArena polyOpa; // "Zelda 0"
    /* 0x02C8 */ TwoHeadGfxArena polyXlu; // "Zelda 1"
    /* 0x02D8 */ u32 gfxPoolIdx;
    /* 0x02DC */ u16* curFrameBuffer;
    /* 0x02E0 */ char unk_2E0[0x04];
    /* 0x02E4 */ u32 viFeatures;
    /* 0x02E8 */ s32 fbIdx;
    /* 0x02EC */ void (*callback)(struct GraphicsContext*, void*);
    /* 0x02F0 */ void* callbackParam;
// TODO: Define & use OOT_VERSION
#if true//OOT_VERSION >= PAL_1_0
    /* 0x02F4 */ f32 xScale;
    /* 0x02F8 */ f32 yScale;
    /* 0x02FC */ char unk_2FC[0x04];
#endif
} GraphicsContext; // size = 0x300

extern Gfx gEmptyDL[];

EXTERN_C Gfx* Gfx_SetFog(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far);
EXTERN_C Gfx* Gfx_SetFogWithSync(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far);
EXTERN_C Gfx* Gfx_SetFog2(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far);

EXTERN_C Gfx* Gfx_BranchTexScroll(Gfx** gfxp, u32 x, u32 y, s32 width, s32 height);
EXTERN_C Gfx* func_80094E78(GraphicsContext* gfxCtx, u32 x, u32 y);
EXTERN_C Gfx* Gfx_TexScroll(GraphicsContext* gfxCtx, u32 x, u32 y, s32 width, s32 height);
EXTERN_C Gfx* Gfx_TwoTexScroll(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2,
                      u32 y2, s32 width2, s32 height2);
EXTERN_C Gfx* Gfx_TwoTexScrollEnvColor(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                              u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a);
EXTERN_C Gfx* Gfx_EnvColor(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a);
EXTERN_C void Gfx_SetupFrame(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b);
EXTERN_C void func_80095974(GraphicsContext* gfxCtx);

// #region SoH [Interpolation]: Versions with xStep & yStep to aid texture scrolling interpolation
EXTERN_C Gfx* Gfx_TexScrollEx(GraphicsContext* gfxCtx, u32 x, u32 y, s32 width, s32 height, s32 xStep, s32 yStep);
EXTERN_C Gfx* Gfx_TwoTexScrollEx(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2,
                        u32 y2, s32 width2, s32 height2, s32 xStep1, s32 yStep1, s32 xStep2, s32 yStep2);
EXTERN_C Gfx* Gfx_TwoTexScrollEnvColorEx(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                              u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a, s32 xStep1, s32 yStep1, s32 xStep2, s32 yStep2);
// #endregion

EXTERN_C void* Graph_Alloc(GraphicsContext* gfxCtx, size_t size);
EXTERN_C void* Graph_Alloc2(GraphicsContext* gfxCtx, size_t size);

#define WORK_DISP          __gfxCtx->work.p
#define POLY_OPA_DISP      __gfxCtx->polyOpa.p
#define POLY_XLU_DISP      __gfxCtx->polyXlu.p
#define OVERLAY_DISP       __gfxCtx->overlay.p

EXTERN_C void Graph_OpenDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, s32 line);
EXTERN_C void Graph_CloseDisps(Gfx** dispRefs, GraphicsContext* gfxCtx, const char* file, s32 line);

// __gfxCtx shouldn't be used directly.
// Use the DISP macros defined above when writing to display buffers.
// #region SOH [General]
// Augmented to provide debug information in debug build and support interpolation
#ifndef NDEBUG
#define OPEN_DISPS(gfxCtx) \
    { \
        void FrameInterpolation_RecordOpenChild(const void* a, int b); \
        FrameInterpolation_RecordOpenChild(__FILE__, __LINE__); \
        GraphicsContext* __gfxCtx; \
        Gfx* dispRefs[4]; \
        __gfxCtx = gfxCtx; \
        (void)__gfxCtx; \
        Graph_OpenDisps(dispRefs, gfxCtx, __FILE__, __LINE__)
#else
#define OPEN_DISPS(gfxCtx) \
    { \
        void FrameInterpolation_RecordOpenChild(const void* a, int b); \
        FrameInterpolation_RecordOpenChild(__FILE__, __LINE__); \
        GraphicsContext* __gfxCtx; \
        __gfxCtx = gfxCtx; \
        (void)__gfxCtx;
#endif

#ifndef NDEBUG
#define CLOSE_DISPS(gfxCtx) \
    {void FrameInterpolation_RecordCloseChild(void); \
    FrameInterpolation_RecordCloseChild();} \
    Graph_CloseDisps(dispRefs, gfxCtx, __FILE__, __LINE__); \
    } \
    (void)0
#else
#define CLOSE_DISPS(gfxCtx) \
    {void FrameInterpolation_RecordCloseChild(void); \
    FrameInterpolation_RecordCloseChild();} \
    (void)0; \
    } \
    (void)0
#endif
// #endregion

// TODO: GRAPH_ALLOC macro

void Graph_ThreadEntry(void*);

extern u64 gMojiFontTLUTs[4][4]; // original name: "moji_tlut"
extern u64 gMojiFontTex[]; // original name: "font_ff"

/**
 * `x` vertex x
 * `y` vertex y
 * `z` vertex z
 * `s` texture s coordinate
 * `t` texture t coordinate
 * `crnx` red component of color vertex, or x component of normal vertex
 * `cgny` green component of color vertex, or y component of normal vertex
 * `cbnz` blue component of color vertex, or z component of normal vertex
 * `a` alpha
 */
#define VTX(x,y,z,s,t,crnx,cgny,cbnz,a) { { { x, y, z }, 0, { s, t }, { crnx, cgny, cbnz, a } } }

#define VTX_T(x,y,z,s,t,cr,cg,cb,a) { { x, y, z }, 0, { s, t }, { cr, cg, cb, a } }

// TODO: Update the macro to the decomp version
#define gDPSetTileCustom(pkt, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt)                    \
    do {                                                                                                               \
        gDPPipeSync(pkt);                                                                                              \
        gDPTileSync(pkt);                                                                                              \
        gDPSetTile(pkt, fmt, siz, (((width)*siz##_TILE_BYTES) + 7) >> 3, 0, G_TX_LOADTILE, 0, cmt, maskt, shiftt, cms, \
                   masks, shifts);                                                                                     \
        gDPTileSync(pkt);                                                                                              \
        gDPSetTile(pkt, fmt, siz, (((width)*siz##_TILE_BYTES) + 7) >> 3, 0, G_TX_RENDERTILE, pal, cmt, maskt, shiftt,  \
                   cms, masks, shifts);                                                                                \
        gDPSetTileSize(pkt, G_TX_RENDERTILE, 0, 0, ((width)-1) << G_TEXTURE_IMAGE_FRAC,                                \
                       ((height)-1) << G_TEXTURE_IMAGE_FRAC);                                                          \
    } while (0)

#endif
