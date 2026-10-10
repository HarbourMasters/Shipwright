/**
 * @file fault_drawer.c
 *
 * Implements routines for drawing text with a fixed font directly to a framebuffer, used in displaying
 * the crash screen implemented by fault.c
 */

#include "fault.h"
#include "fault_internal.h"
#include "global.h"
#include "vt.h"

typedef struct {
    /* 0x00 */ u16* frameBuffer;
    /* 0x04 */ u16 w;
    /* 0x06 */ u16 h;
    /* 0x08 */ u16 yStart;
    /* 0x0A */ u16 yEnd;
    /* 0x0C */ u16 xStart;
    /* 0x0E */ u16 xEnd;
    /* 0x10 */ u16 foreColor;
    /* 0x12 */ u16 backColor;
    /* 0x14 */ u16 cursorX;
    /* 0x16 */ u16 cursorY;
    /* 0x18 */ const u32* fontData;
    /* 0x1C */ u8 charW;
    /* 0x1D */ u8 charH;
    /* 0x1E */ s8 charWPad;
    /* 0x1F */ s8 charHPad;
    /* 0x20 */ u16 printColors[10];
    /* 0x34 */ u8 escCode; // bool
    /* 0x35 */ u8 osSyncPrintfEnabled;
    /* 0x38 */ FaultDrawerCallback inputCallback;
} FaultDrawer; // size = 0x3C
#if 0
extern const u32 sFaultDrawerFont[];

FaultDrawer sFaultDrawer;

FaultDrawer* sFaultDrawerInstance = &sFaultDrawer;

#define FAULT_DRAWER_CURSOR_X 22
#define FAULT_DRAWER_CURSOR_Y 16

FaultDrawer sFaultDrawerDefault = {
    FAULT_FB_ADDRESS,                          // frameBuffer
    SCREEN_WIDTH,                              // w
    SCREEN_HEIGHT,                             // h
    FAULT_DRAWER_CURSOR_Y,                     // yStart
    SCREEN_HEIGHT - FAULT_DRAWER_CURSOR_Y - 1, // yEnd
    FAULT_DRAWER_CURSOR_X,                     // xStart
    SCREEN_WIDTH - FAULT_DRAWER_CURSOR_X - 1,  // xEnd
    GPACK_RGBA5551(255, 255, 255, 255),        // foreColor
    GPACK_RGBA5551(0, 0, 0, 0),                // backColor
    FAULT_DRAWER_CURSOR_X,                     // cursorX
    FAULT_DRAWER_CURSOR_Y,                     // cursorY
    sFaultDrawerFont,                          // fontData
    8,                                         // charW
    8,                                         // charH
    0,                                         // charWPad
    0,                                         // charHPad
    {
        // printColors
        GPACK_RGBA5551(0, 0, 0, 1),       // BLACK
        GPACK_RGBA5551(255, 0, 0, 1),     // RED
        GPACK_RGBA5551(0, 255, 0, 1),     // GREEN
        GPACK_RGBA5551(255, 255, 0, 1),   // YELLOW
        GPACK_RGBA5551(0, 0, 255, 1),     // BLUE
        GPACK_RGBA5551(255, 0, 255, 1),   // MAGENTA
        GPACK_RGBA5551(0, 255, 255, 1),   // CYAN
        GPACK_RGBA5551(255, 255, 255, 1), // WHITE
        GPACK_RGBA5551(120, 120, 120, 1), // DARK GRAY
        GPACK_RGBA5551(176, 176, 176, 1), // LIGHT GRAY
    },
    false, // escCode
    false, // osSyncPrintfEnabled
    NULL,  // inputCallback
};
#endif
//! TODO: Needs to be extracted
//#pragma GLOBAL_ASM("asm/non_matchings/boot/fault_drawer/sFaultDrawerFont.s")

void FaultDrawer_SetOsSyncPrintfEnabled(u32 enabled) {
    // sFaultDrawerInstance->osSyncPrintfEnabled = enabled;
}

void FaultDrawer_DrawRecImpl(s32 xStart, s32 yStart, s32 xEnd, s32 yEnd, u16 color) {
#if 0
    u16* frameBuffer;
    s32 x;
    s32 y;
    s32 xDiff = sFaultDrawerInstance->w - xStart;
    s32 yDiff = sFaultDrawerInstance->h - yStart;
    s32 xSize = xEnd - xStart + 1;
    s32 ySize = yEnd - yStart + 1;

    if ((xDiff > 0) && (yDiff > 0)) {
        if (xSize > xDiff) {
            xSize = xDiff;
        }

        if (ySize > yDiff) {
            ySize = yDiff;
        }

        frameBuffer = sFaultDrawerInstance->frameBuffer + sFaultDrawerInstance->w * yStart + xStart;
        for (y = 0; y < ySize; y++) {
            for (x = 0; x < xSize; x++) {
                *frameBuffer++ = color;
            }
            frameBuffer += sFaultDrawerInstance->w - xSize;
        }

        osWritebackDCacheAll();
    }
#endif
}

void FaultDrawer_DrawChar(char c) {
#if 0
    s32 x;
    s32 y;
    u32 data;
    s32 cursorX = sFaultDrawerInstance->cursorX;
    s32 cursorY = sFaultDrawerInstance->cursorY;
    s32 shift = c % 4;
    const u32* dataPtr = &sFaultDrawerInstance->fontData[(((c / 8) * 16) + ((c & 4) >> 2))];
    u16* frameBuffer = sFaultDrawerInstance->frameBuffer + (sFaultDrawerInstance->w * cursorY) + cursorX;

    if ((sFaultDrawerInstance->xStart <= cursorX) &&
        ((sFaultDrawerInstance->charW + cursorX - 1) <= sFaultDrawerInstance->xEnd) &&
        (sFaultDrawerInstance->yStart <= cursorY) &&
        ((sFaultDrawerInstance->charH + cursorY - 1) <= sFaultDrawerInstance->yEnd)) {
        for (y = 0; y < sFaultDrawerInstance->charH; y++) {
            u32 mask = 0x10000000 << shift;

            data = *dataPtr;
            for (x = 0; x < sFaultDrawerInstance->charW; x++) {
                if (mask & data) {
                    frameBuffer[x] = sFaultDrawerInstance->foreColor;
                } else if (sFaultDrawerInstance->backColor & 1) {
                    frameBuffer[x] = sFaultDrawerInstance->backColor;
                }
                mask >>= 4;
            }
            frameBuffer += sFaultDrawerInstance->w;
            dataPtr += 2;
        }
    }
#endif
}

s32 FaultDrawer_ColorToPrintColor(u16 color) {
    return 0;
#if 0
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sFaultDrawerInstance->printColors); i++) {
        if (color == sFaultDrawerInstance->printColors[i]) {
            return i;
        }
    }
    return -1;
#endif
}

void FaultDrawer_UpdatePrintColor(void) {
#if 0
    s32 index;

    if (sFaultDrawerInstance->osSyncPrintfEnabled) {
        osSyncPrintf(VT_RST);

        index = FaultDrawer_ColorToPrintColor(sFaultDrawerInstance->foreColor);
        if ((index >= 0) && (index < 8)) {
            osSyncPrintf(VT_SGR("3%d"), index);
        }

        index = FaultDrawer_ColorToPrintColor(sFaultDrawerInstance->backColor);
        if ((index >= 0) && (index < 8)) {
            osSyncPrintf(VT_SGR("4%d"), index);
        }
    }
#endif
}

void FaultDrawer_SetForeColor(u16 color) {
    // sFaultDrawerInstance->foreColor = color;
    // FaultDrawer_UpdatePrintColor();
}

void FaultDrawer_SetBackColor(u16 color) {
    // sFaultDrawerInstance->backColor = color;
    // FaultDrawer_UpdatePrintColor();
}

void FaultDrawer_SetFontColor(u16 color) {
    // FaultDrawer_SetForeColor(color | 1); // force alpha to be set
}

void FaultDrawer_SetCharPad(s8 padW, s8 padH) {
    // sFaultDrawerInstance->charWPad = padW;
    // sFaultDrawerInstance->charHPad = padH;
}

void FaultDrawer_SetCursor(s32 x, s32 y) {
    // if (sFaultDrawerInstance->osSyncPrintfEnabled) {
    //     osSyncPrintf(
    //         VT_CUP("%d", "%d"),
    //         (y - sFaultDrawerInstance->yStart) / (sFaultDrawerInstance->charH + sFaultDrawerInstance->charHPad),
    //         (x - sFaultDrawerInstance->xStart) / (sFaultDrawerInstance->charW + sFaultDrawerInstance->charWPad));
    // }
    // sFaultDrawerInstance->cursorX = x;
    // sFaultDrawerInstance->cursorY = y;
}

void FaultDrawer_FillScreen() {
    // if (sFaultDrawerInstance->osSyncPrintfEnabled) {
    //     osSyncPrintf(VT_CLS);
    // }
    //
    // FaultDrawer_DrawRecImpl(sFaultDrawerInstance->xStart, sFaultDrawerInstance->yStart, sFaultDrawerInstance->xEnd,
    //                         sFaultDrawerInstance->yEnd, sFaultDrawerInstance->backColor | 1);
    // FaultDrawer_SetCursor(sFaultDrawerInstance->xStart, sFaultDrawerInstance->yStart);
}

void* FaultDrawer_FormatStringFunc(void* arg, const char* str, size_t count) {
#if 0
    for (; count > 0; count--, str++) {
        if (sFaultDrawerInstance->escCode) {
            sFaultDrawerInstance->escCode = false;
            if ((*str >= '1') && (*str <= '9')) {
                FaultDrawer_SetForeColor(sFaultDrawerInstance->printColors[*str - '0']);
            }
        } else {
            switch (*str) {
                case '\n':
                    if (sFaultDrawerInstance->osSyncPrintfEnabled) {
                        osSyncPrintf("\n");
                    }

                    sFaultDrawerInstance->cursorX = sFaultDrawerInstance->w;
                    break;

                case FAULT_ESC:
                    sFaultDrawerInstance->escCode = true;
                    break;

                default:
                    if (sFaultDrawerInstance->osSyncPrintfEnabled) {
                        osSyncPrintf("%c", *str);
                    }

                    FaultDrawer_DrawChar(*str);
                    sFaultDrawerInstance->cursorX += sFaultDrawerInstance->charW + sFaultDrawerInstance->charWPad;
            }
        }

        if (sFaultDrawerInstance->cursorX >= (sFaultDrawerInstance->xEnd - sFaultDrawerInstance->charW)) {
            sFaultDrawerInstance->cursorX = sFaultDrawerInstance->xStart;
            sFaultDrawerInstance->cursorY += sFaultDrawerInstance->charH + sFaultDrawerInstance->charHPad;
            if (sFaultDrawerInstance->yEnd - sFaultDrawerInstance->charH <= sFaultDrawerInstance->cursorY) {
                if (sFaultDrawerInstance->inputCallback != NULL) {
                    sFaultDrawerInstance->inputCallback();
                    FaultDrawer_FillScreen();
                }
                sFaultDrawerInstance->cursorY = sFaultDrawerInstance->yStart;
            }
        }
    }

    osWritebackDCacheAll();
#endif
    return NULL;
}

// const char D_80099080[] = "(null)";

s32 FaultDrawer_VPrintf(const char* fmt, va_list ap) {
    // return _Printf(FaultDrawer_FormatStringFunc, sFaultDrawerInstance, fmt, ap);
}

s32 FaultDrawer_Printf(const char* fmt, ...) {
    return 0;
#if 0
    s32 ret;
    va_list args;

    va_start(args, fmt);

    ret = FaultDrawer_VPrintf(fmt, args);

    va_end(args);

    return ret;
#endif
}

void FaultDrawer_DrawText(s32 x, s32 y, const char* fmt, ...) {
    // va_list args;
    // va_start(args, fmt);
    //
    // FaultDrawer_SetCursor(x, y);
    // FaultDrawer_VPrintf(fmt, args);
    //
    // va_end(args);
}

void FaultDrawer_SetDrawerFrameBuffer(void* frameBuffer, u16 w, u16 h) {
    // sFaultDrawerInstance->frameBuffer = frameBuffer;
    // sFaultDrawerInstance->w = w;
    // sFaultDrawerInstance->h = h;
}

void FaultDrawer_SetInputCallback(FaultDrawerCallback callback) {
    // sFaultDrawerInstance->inputCallback = callback;
}

void FaultDrawer_Init() {
    // sFaultDrawerInstance = &sFaultDrawer;
    // bcopy(&sFaultDrawerDefault, sFaultDrawerInstance, sizeof(FaultDrawer));
    // sFaultDrawerInstance->frameBuffer = (u16*)(PHYS_TO_K0(osMemSize) - SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(u16));
}
