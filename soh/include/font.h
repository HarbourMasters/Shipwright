#ifndef FONT_H
#define FONT_H

#include <libultraship/libultra.h>

#include "extern_c_helper.h"

// #region SoH [Port]: Increased char buffer because texture paths could be bigger than (16 * 16 / 2)
#define FONT_CHAR_MULTIPLIER 256
// #endregion

// TODO get these properties from the textures themselves
#define FONT_CHAR_TEX_WIDTH  16
#define FONT_CHAR_TEX_HEIGHT 16
#define FONT_CHAR_TEX_SIZE ((FONT_CHAR_TEX_WIDTH * FONT_CHAR_TEX_HEIGHT) / 2) // 16x16 I4 texture

// TODO: Update the type to upstream
typedef struct Font {
    /* 0x0000 */ uintptr_t    msgOffset;
    /* 0x0004 */ u32          msgLength;
    /* 0x0008 */ u8           charTexBuf[FONT_CHAR_TEX_SIZE * FONT_CHAR_MULTIPLIER];
    /* 0x3C08 */ u8           iconBuf[FONT_CHAR_TEX_SIZE * FONT_CHAR_MULTIPLIER];
    /* 0x3C88 */ u8           fontBuf[FONT_CHAR_TEX_SIZE * FONT_CHAR_MULTIPLIER];
    union {
         /* 0xDC88 */ char   msgBuf[1280];
         /* 0xDC88 */ u16    msgBufWide[640];
    };
} Font; // size = 0xE188

// TODO: PLATFORM macros
#if false//PLATFORM_IQUE
EXTERN_C void Font_LoadCharCHN(Font* font, u16 character, u16 codePointIndex);
#endif
EXTERN_C void Font_LoadCharWide(Font* font, u16 character, u16 codePointIndex);
EXTERN_C void Font_LoadChar(Font* font, u8 character, u16 codePointIndex);
EXTERN_C void Font_LoadMessageBoxIcon(Font* font, u16 icon);
EXTERN_C void Font_LoadOrderedFont(Font* font);

#endif