#include "prevent_bss_reordering.h"
#include "z64speed_meter.h"

#include "gfx.h"
#include "regs.h"
#include "sys_cfb.h"
#include "system_malloc.h"
#include "z64game.h"
#include "z64malloc.h"
#include "z64view.h"

#include "functions.h"
#include "variables.h"

/**
 * How much time the RSP ran audio tasks for over the course of `gGraphUpdatePeriod`.
 */
volatile OSTime gRSPAudioTimeTotal;

/**
 * How much time the RSP ran graphics tasks for over the course of `gGraphUpdatePeriod`.
 * Typically the RSP runs 1 graphics task per `Graph_Update` cycle, but may run 0 (see `Graph_Update`).
 */
volatile OSTime gRSPGfxTimeTotal;

/**
 * How much time the RDP ran for over the course of `gGraphUpdatePeriod`.
 */
volatile OSTime gRDPTimeTotal;

/**
 * How much time elapsed between the last two `Graph_Update` ending.
 * This is expected to be at least the duration of a single frame, since it includes the time spent waiting on the
 * graphics task to be done.
 */
volatile OSTime gGraphUpdatePeriod;

// Accumulator for `gRSPAudioTimeTotal`
volatile OSTime gRSPAudioTimeAcc;

// Accumulator for `gRSPGfxTimeTotal`.
volatile OSTime gRSPGfxTimeAcc;

volatile OSTime gRSPOtherTimeAcc;
volatile OSTime gUnkTimeAcc;

// Accumulator for `gRDPTimeTotal`
volatile OSTime gRDPTimeAcc;

typedef struct {
    /* 0x0 */ volatile OSTime* time;
    /* 0x4 */ u16 x;
    /* 0x5 */ u16 y;
    /* 0x6 */ u16 color;
} SpeedMeterTimeEntry; // size = 0x8

SpeedMeterTimeEntry* sSpeedMeterTimeEntryPtr;

SpeedMeterTimeEntry sSpeedMeterTimeEntryArray[] = {
    { &gRSPAudioTimeTotal, 0, 6, GPACK_RGBA5551(0, 0, 255, 1) },
    { &gRSPGfxTimeTotal, 0, 8, GPACK_RGBA5551(255, 128, 128, 1) },
    { &gRDPTimeTotal, 0, 10, GPACK_RGBA5551(0, 255, 0, 1) },
    { &gGraphUpdatePeriod, 0, 12, GPACK_RGBA5551(255, 0, 255, 1) },
};

typedef struct {
    /* 0x00 */ s32 maxVal;
    /* 0x04 */ s32 val;
    /* 0x08 */ u16 backColor;
    /* 0x0A */ u16 foreColor;
    /* 0x0C */ s32 ulx;
    /* 0x10 */ s32 lrx;
    /* 0x14 */ s32 uly;
    /* 0x18 */ s32 lry;
} SpeedMeterAllocEntry; // size = 0x1C

//! FAKE: if(1) in macro

#define gDrawRect(gfx, color, ulx, uly, lrx, lry)                      \
    do {                                                               \
        if (gSysCfbHiResEnabled == true) {                             \
            u32 tmp = color;                                           \
            gDPPipeSync(gfx);                                          \
            gDPSetFillColor(gfx, ((tmp) << 16) | (tmp));               \
            gDPFillRectangle(gfx, (ulx)*2, (uly)*2, (lrx)*2, (lry)*2); \
            if (1) {}                                                  \
        } else {                                                       \
            u32 tmp = color;                                           \
            gDPPipeSync(gfx);                                          \
            gDPSetFillColor(gfx, ((tmp) << 16) | (tmp));               \
            gDPFillRectangle(gfx, (ulx), (uly), (lrx), (lry));         \
        }                                                              \
    } while (0)

void SpeedMeter_InitImpl(SpeedMeter* this, s32 x, s32 y) {
    this->x = x;
    this->y = y;
}

void SpeedMeter_Init(SpeedMeter* this) {
    SpeedMeter_InitImpl(this, 32, 22);
}

void SpeedMeter_Destroy(SpeedMeter* this) {
}

void SpeedMeter_DrawTimeEntries(SpeedMeter* this, GraphicsContext* gfxCtx) {
    s32 pad[3];
    u32 baseX = 32;
    s32 width;
    s32 uly;
    s32 lry;
    s32 pad3;
    View view;
    s32 pad2[2];
    s32 i;
    Gfx* gfx;

    uly = this->y;
    lry = this->y + 2;

    OPEN_DISPS(gfxCtx);

    /*! @bug if gIrqMgrRetraceTime is 0, CLOSE_DISPS will never be reached */
    if (gIrqMgrRetraceTime == 0) {
        // 2S2H [Port] We need our close disps helpers called to prevent interpolation crashes
        CLOSE_DISPS_PORT_HELPERS(gfxCtx);
        return;
    }

    sSpeedMeterTimeEntryPtr = &sSpeedMeterTimeEntryArray[0];
    for (i = 0; i < ARRAY_COUNT(sSpeedMeterTimeEntryArray); i++) {
        width = ((f64)*sSpeedMeterTimeEntryPtr->time / gIrqMgrRetraceTime) * 64.0;
        sSpeedMeterTimeEntryPtr->x = baseX + width;
        sSpeedMeterTimeEntryPtr++;
    }

    View_Init(&view, gfxCtx);
    view.flags = VIEW_VIEWPORT | VIEW_PROJECTION_ORTHO;

    SET_FULLSCREEN_VIEWPORT_DYNAMIC(&view);

    gfx = OVERLAY_DISP;
    View_ApplyTo(&view, &gfx);

    gDPPipeSync(gfx++);
    gDPSetOtherMode(gfx++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

    gDrawRect(gfx++, GPACK_RGBA5551(0, 0, 255, 1), baseX + 64 * 0, uly, baseX + 64 * 1, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(0, 255, 0, 1), baseX + 64 * 1, uly, baseX + 64 * 2, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(255, 0, 0, 1), baseX + 64 * 2, uly, baseX + 64 * 3, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(255, 0, 255, 1), baseX + 64 * 3, uly, baseX + 64 * 4, lry);

    sSpeedMeterTimeEntryPtr = &sSpeedMeterTimeEntryArray[0];
    for (i = 0; i < ARRAY_COUNT(sSpeedMeterTimeEntryArray); i++) {
        gDrawRect(gfx++, sSpeedMeterTimeEntryPtr->color, baseX, lry + sSpeedMeterTimeEntryPtr->y,
                  sSpeedMeterTimeEntryPtr->x, lry + sSpeedMeterTimeEntryPtr->y + 1);
        sSpeedMeterTimeEntryPtr++;
    }
    gDPPipeSync(gfx++);

    OVERLAY_DISP = gfx;

    CLOSE_DISPS(gfxCtx);
}

void SpeedMeter_InitAllocEntry(SpeedMeterAllocEntry* this, u32 maxVal, u32 val, u16 backColor, u16 foreColor, u32 ulx,
                               u32 lrx, u32 uly, u32 lry) {
    this->maxVal = maxVal;
    this->val = val;
    this->backColor = backColor;
    this->foreColor = foreColor;
    this->ulx = ulx;
    this->lrx = lrx;
    this->uly = uly;
    this->lry = lry;
}

void SpeedMeter_DrawAllocEntry(SpeedMeterAllocEntry* this, GraphicsContext* gfxCtx) {
    s32 usedOff;
    View view;
    Gfx* gfx;

    if (this->maxVal != 0) {
        OPEN_DISPS(gfxCtx);

        View_Init(&view, gfxCtx);
        view.flags = VIEW_VIEWPORT | VIEW_PROJECTION_ORTHO;

        SET_FULLSCREEN_VIEWPORT_DYNAMIC(&view);

        gfx = OVERLAY_DISP;
        View_ApplyTo(&view, &gfx);

        gDPPipeSync(gfx++);
        gDPSetOtherMode(gfx++,
                        G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                        G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

        usedOff = ((s64)(this->lrx - this->ulx) * this->val) / this->maxVal + this->ulx;
        gDrawRect(gfx++, this->backColor, usedOff, this->uly, this->lrx, this->lry);
        gDrawRect(gfx++, this->foreColor, this->ulx, this->uly, usedOff, this->lry);

        gDPPipeSync(gfx++);

        //! FAKE:
        if (this && this && this) {}

        OVERLAY_DISP = gfx;
        CLOSE_DISPS(gfxCtx);
    }
}

void SpeedMeter_DrawAllocEntries(SpeedMeter* meter, GraphicsContext* gfxCtx, GameState* state) {
    s32 pad[2];
    u32 ulx = 30;
    u32 lrx = 290;
    SpeedMeterAllocEntry entry;
    TwoHeadArena* tha;
    s32 y;
    TwoHeadGfxArena* thga;
    size_t zeldaFreeMax;
    size_t zeldaFree;
    size_t zeldaAlloc;
    s32 sysFreeMax;
    s32 sysFree;
    s32 sysAlloc;

    y = 212;
    if (R_ENABLE_ARENA_DBG > 2) {
        if (ZeldaArena_IsInitialized()) {
            ZeldaArena_GetSizes(&zeldaFreeMax, &zeldaFree, &zeldaAlloc);
            SpeedMeter_InitAllocEntry(&entry, zeldaFree + zeldaAlloc, zeldaAlloc, GPACK_RGBA5551(0, 0, 255, 1),
                                      GPACK_RGBA5551(255, 255, 255, 1), ulx, lrx, y, y + 1);
            SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
            y++;
            y++;
        }
    }

    if (R_ENABLE_ARENA_DBG > 1) {
        SystemArena_GetSizes((size_t*)&sysFreeMax, (size_t*)&sysFree, (size_t*)&sysAlloc);
        SpeedMeter_InitAllocEntry(&entry, sysFree + sysAlloc - state->tha.size, sysAlloc - state->tha.size,
                                  GPACK_RGBA5551(0, 0, 255, 1), GPACK_RGBA5551(255, 128, 128, 1), ulx, lrx, y, y);
        SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
        y++;
    }

    tha = &state->tha;
    SpeedMeter_InitAllocEntry(&entry, tha->size, tha->size - THA_GetRemaining(tha), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(0, 255, 0, 1), ulx, lrx, y, y);
    SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->polyOpa;
    SpeedMeter_InitAllocEntry(&entry, thga->size, thga->size - THGA_GetRemaining(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 0, 255, 1), ulx, lrx, y, y);
    SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->polyXlu;
    SpeedMeter_InitAllocEntry(&entry, thga->size, thga->size - THGA_GetRemaining(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 255, 0, 1), ulx, lrx, y, y);
    SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->overlay;
    SpeedMeter_InitAllocEntry(&entry, thga->size, thga->size - THGA_GetRemaining(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 0, 0, 1), ulx, lrx, y, y);
    SpeedMeter_DrawAllocEntry(&entry, gfxCtx);
    y++;
}
