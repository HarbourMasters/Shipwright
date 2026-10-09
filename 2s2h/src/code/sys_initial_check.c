/**
 * File: sys_initial_check.c
 * Description: Functions for checking for the presence of the Expansion Pak and the correct TV type (PAL/NTSC/MPAL),
 * and functions for printing error messages directly to screen if not found or incorrect.
 *
 * These checks are some of the first functions run in Main, before even setting up the system heap, and any image files
 * are DMA'd directly to fixed RAM addresses.
 */
#include "global.h"
#include "fault.h"
#include "misc/locerrmsg/locerrmsg.h"
#include "misc/memerrmsg/memerrmsg.h"

#define LOCERRMSG_WIDTH 208
#define LOCERRMSG_HEIGHT 16
#define NUMBEROF_LOCERRMSGS 1

#define MEMERRMSG_WIDTH 128
#define MEMERRMSG_HEIGHT 37
#define NUMBEROF_MEMERRMSGS 2

#define SIZEOF_LOCERRMSG (LOCERRMSG_WIDTH * LOCERRMSG_HEIGHT / 2 * NUMBEROF_LOCERRMSGS)
#define SIZEOF_MEMERRMSG (MEMERRMSG_WIDTH * MEMERRMSG_HEIGHT / 2 * NUMBEROF_MEMERRMSGS)

// Address with enough room after to load either of the error message image files before the fault screen buffer at the
// end of RDRAM
#define CHECK_ERRMSG_STATIC_SEGMENT (u8*)((uintptr_t)FAULT_FB_ADDRESS - MAX(SIZEOF_LOCERRMSG, SIZEOF_MEMERRMSG))

void Check_WriteRGBA16Pixel(u16* buffer, u32 x, u32 y, u32 value) {
    if (value & RGBA16_PIXEL_OPAQUE) {
        (&buffer[x])[y * SCREEN_WIDTH] = value;
    }
}

void Check_WriteI4Pixel(u16* buffer, u32 x, u32 y, u32 value) {
    Check_WriteRGBA16Pixel(buffer, x, y, value * GPACK_RGBA5551(16, 16, 16, 0) + GPACK_RGBA5551(12, 12, 12, 1));
}

/**
 * x and y are the coordinates of the bottom-left corner.
 */
void Check_DrawI4Texture(u16* buffer, s32 x, s32 y, s32 width, s32 height, u8* texture) {
    s32 v;
    s32 u;
    u8 pixelPair;
    u8* pixelPairPtr = texture;

    // I4 textures are bitpacked 2 pixels per byte, so this writes a pair of pixels in each iteration using bitmasking.
    for (v = 0; v < height; v++) {
        for (u = 0; u < width; u += 2, pixelPairPtr++) {
            pixelPair = *pixelPairPtr;
            Check_WriteI4Pixel(buffer, x + u, y + v, pixelPair >> 4);
            pixelPair = *pixelPairPtr;
            Check_WriteI4Pixel(buffer, x + u + 1, y + v, pixelPair & 0xF);
        }
    }
}

void Check_ClearRGBA16(u16* buffer) {
    u32 x;
    u32 y;

    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            Check_WriteRGBA16Pixel(buffer, x, y, 1);
        }
    }
}

/**
 * Draw error message textures directly to a screen buffer at the end of normal RDRAM
 */
void Check_DrawExpansionPakErrorMessage(void) {
    DmaMgr_SendRequest0(CHECK_ERRMSG_STATIC_SEGMENT, SEGMENT_ROM_START(memerrmsg), SEGMENT_SIZE(memerrmsg));
    Check_ClearRGBA16((u16*)FAULT_FB_ADDRESS);
    Check_DrawI4Texture((u16*)FAULT_FB_ADDRESS, 96, 71, MEMERRMSG_WIDTH, MEMERRMSG_HEIGHT, CHECK_ERRMSG_STATIC_SEGMENT);
    Check_DrawI4Texture((u16*)FAULT_FB_ADDRESS, 96, 127, MEMERRMSG_WIDTH, MEMERRMSG_HEIGHT,
                        CHECK_ERRMSG_STATIC_SEGMENT + MEMERRMSG_WIDTH * MEMERRMSG_HEIGHT / 2);
    osWritebackDCacheAll();
    osViSwapBuffer((u16*)FAULT_FB_ADDRESS);
    osViBlack(false);
}

/**
 * Draw error message texture directly to a screen buffer at the end of normal RDRAM
 */
void Check_DrawRegionLockErrorMessage(void) {
    DmaMgr_SendRequest0(CHECK_ERRMSG_STATIC_SEGMENT, SEGMENT_ROM_START(locerrmsg), SEGMENT_SIZE(locerrmsg));
    Check_ClearRGBA16((u16*)FAULT_FB_ADDRESS);
    Check_DrawI4Texture((u16*)FAULT_FB_ADDRESS, 56, 112, LOCERRMSG_WIDTH, LOCERRMSG_HEIGHT,
                        CHECK_ERRMSG_STATIC_SEGMENT);
    osWritebackDCacheAll();
    osViSwapBuffer((u16*)FAULT_FB_ADDRESS);
    osViBlack(false);
}

/**
 * If Expansion pak is not installed, display error message until console is turned off
 */
void Check_ExpansionPak(void) {
    // Expansion pak installed
    if (osMemSize >= 0x800000) {
        return;
    }

    Check_DrawExpansionPakErrorMessage();
    osDestroyThread(NULL);
    while (true) {}
}

/**
 * If region is not NTSC or MPAL, display error message until console is turned off
 */
void Check_RegionIsSupported(void) {
    s32 regionSupported = false;

    if ((osTvType == OS_TV_NTSC) || (osTvType == OS_TV_MPAL)) {
        regionSupported = true;
    }

    if (!regionSupported) {
        Check_DrawRegionLockErrorMessage();
        osDestroyThread(NULL);
        while (true) {}
    }
}
