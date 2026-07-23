#ifndef LIBU64_GFXPRINT_H
#define LIBU64_GFXPRINT_H

#include <libultraship/libultra.h>
#include <libultraship/color.h>

#include "extern_c_helper.h"

typedef struct GfxPrint {
    /* 0x00 */ PrintCallback callback;
    /* 0x04 */ Gfx* dList;
    /* 0x08 */ u16 posX;
    /* 0x0A */ u16 posY;
    /* 0x0C */ u16 baseX;
    /* 0x0E */ u8 baseY;
    /* 0x0F */ u8 flags;
    /* 0x10 */ Color_RGBA8_u32 color;
    /* 0x14 */ char unk_14[0x1C]; // unused
} GfxPrint; // size = 0x30

// TODO: GFX_CHAR_X_SPACING & GFX_CHAR_Y_SPACING macros

#define GFXP_UNUSED "\x8E"
#define GFXP_UNUSED_CHAR 0x8E
#define GFXP_HIRAGANA "\x8D"
#define GFXP_HIRAGANA_CHAR 0x8D
#define GFXP_KATAKANA "\x8C"
#define GFXP_KATAKANA_CHAR 0x8C
#define GFXP_RAINBOW_ON "\x8B"
#define GFXP_RAINBOW_ON_CHAR 0x8B
#define GFXP_RAINBOW_OFF "\x8A"
#define GFXP_RAINBOW_OFF_CHAR 0x8A

#define GFXP_FLAG_HIRAGANA (1 << 0)
#define GFXP_FLAG_RAINBOW  (1 << 1)
#define GFXP_FLAG_SHADOW   (1 << 2)
#define GFXP_FLAG_UPDATE   (1 << 3)
// TODO: PLATFORM macros
#if true//!PLATFORM_N64
#define GFXP_FLAG_ENLARGE  (1 << 6)
#endif
#define GFXP_FLAG_OPEN     (1 << 7)

EXTERN_C void GfxPrint_SetColor(GfxPrint* this, u32 r, u32 g, u32 b, u32 a);
EXTERN_C void GfxPrint_SetPosPx(GfxPrint* this, s32 x, s32 y);
EXTERN_C void GfxPrint_SetPos(GfxPrint* this, s32 x, s32 y);
EXTERN_C void GfxPrint_SetBasePosPx(GfxPrint* this, s32 x, s32 y);
EXTERN_C void GfxPrint_Init(GfxPrint* this);
EXTERN_C void GfxPrint_Destroy(GfxPrint* this);
EXTERN_C void GfxPrint_Open(GfxPrint* this, Gfx* dList);
EXTERN_C Gfx* GfxPrint_Close(GfxPrint* this);
EXTERN_C s32 GfxPrint_Printf(GfxPrint* this, const char* fmt, ...);

#endif