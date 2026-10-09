#include "global.h"
#include "debug.h"
#include "z64vimode.h"
#include "PR/viint.h"

typedef struct {
    /* 0x00 */ u32 burst;
    /* 0x04 */ u32 vSync;
    /* 0x08 */ u32 hSync;
    /* 0x0C */ u32 leap;
    /* 0x10 */ u32 hStart;
    /* 0x14 */ u32 vStart;
    /* 0x18 */ u32 vBurst;
} ViModeStruct; // size = 0x1C

#define BURST(hsync_width, color_width, vsync_width, color_start)                                      \
    (((u32)(hsync_width)&0xFF) | (((u32)(color_width)&0xFF) << 8) | (((u32)(vsync_width)&0xF) << 16) | \
     (((u32)(color_start)&0xFFFF) << 20))
#define WIDTH(v) (v)
#define VSYNC(v) (v)
#define HSYNC(duration, leap) (((u32)(leap) << 16) | ((u32)(duration)&0xFFFF))
#define LEAP(upper, lower) (((u32)(upper) << 16) | ((u32)(lower)&0xFFFF))
#define START(start, end) (((u32)(start) << 16) | ((u32)(end)&0xFFFF))

#define FTOFIX(val, i, f) ((u32)((val) * (f32)(1 << (f))) & ((1 << ((i) + (f))) - 1))

#define F210(val) FTOFIX(val, 2, 10)
#define SCALE(scaleup, off) (F210((1.0f / (f32)(scaleup))) | (F210((f32)(off)) << 16))

#define VCURRENT(v) v
#define ORIGIN(v) v
#define VINTR(v) v
#define HSTART START
s32 osTvType = OS_TV_NTSC;
void ViMode_LogPrint(OSViMode* osViMode) {
}

/**
 * Configures the custom OSViMode for this ViMode
 *
 * @param viMode        ViMode to configure the custom OSViMode for
 * @param type          Identifying type for the OSViMode
 * @param tvType        TV Type: NTSC, PAL, MPAL or FPAL
 * @param loRes         Boolean: true = low resolution, false = high resolution.
 *                      Corresponds to "L" or "H" in libultra VI mode names
 * @param antialiasOff  Boolean: true = point-sampling, false = anti-aliasing.
 *                      Corresponds to "P" or "A" in libultra VI mode names
 * @param modeN         Boolean: controls interlacing mode, different based on resolution.
 *                      Corresponds to "N" or "F" in libultra VI mode names
 * @param fb16Bit       Bolean: true = 16-bit framebuffer, false = 32-bit framebuffer.
 *                      Corresponds to "1" or "2" in libultra VI mode names
 * @param width         Screen width
 * @param height        Screen height
 * @param leftAdjust    Left edge adjustment
 * @param rightAdjust   Right edge adjustment
 * @param upperAdjust   Upper edge adjustment
 * @param lowerAdjust   Lower edge adjustment
 */
void ViMode_Configure(OSViMode* viMode, s32 type, s32 tvType, s32 loRes, s32 antialiasOff, s32 modeN, s32 fb16Bit,
                      s32 width, s32 height, s32 leftAdjust, s32 rightAdjust, s32 upperAdjust, s32 lowerAdjust) {
    s32 hiRes;
    s32 antialiasOn;
    s32 modeF;
    s32 fb32Bit;
    s32 hiResDeflicker; // deflickered interlacing
    s32 hiResInterlaced;
    s32 loResDeinterlaced;
    s32 loResInterlaced;
    s32 modeLAN1; // L=(lo res) A=(antialias)     N=(deinterlace)        1=(16-bit)
    s32 modeLPN2; // L=(lo res) P=(point-sampled) N=(deinterlace)        2=(32-bit)
    s32 modeHPN2; // H=(hi res) P=(point-sampled) N=(normal interlacing) 2=(32-bit)
    s32 yScaleLo;
    s32 yScaleHiEvenField;
    s32 yScaleHiOddField;

    hiRes = !loRes;
    antialiasOn = !antialiasOff;
    modeF = !modeN;
    fb32Bit = !fb16Bit;

    hiResDeflicker = hiRes && modeF;
    hiResInterlaced = hiRes && modeN;
    loResDeinterlaced = loRes && modeN;
    loResInterlaced = loRes && modeF;

    modeLAN1 = loRes && antialiasOn && modeN && fb16Bit;
    modeLPN2 = loRes && antialiasOff && modeN && fb32Bit;
    modeHPN2 = hiRes && antialiasOff && modeN && fb32Bit;

    upperAdjust &= ~1;
    lowerAdjust &= ~1;

    yScaleLo = (hiResDeflicker ? 2 : 1) * (((SCREEN_HEIGHT << 11) / SCREEN_HEIGHT) / (loRes ? 1 : 2));

    yScaleHiEvenField = modeF ? (loResInterlaced ? 0x1000000 : 0x2000000) : 0;
    yScaleHiOddField = modeF ? (loResInterlaced ? 0x3000000 : 0x2000000) : 0;

    viMode->type = type;
    viMode->comRegs.ctrl = OS_VI_UNK2000 | OS_VI_UNK1000 | OS_VI_GAMMA | OS_VI_GAMMA_DITHER |
                           (!loResDeinterlaced ? OS_VI_UNK40 : 0) | (antialiasOn ? OS_VI_DIVOT : 0) |
                           (fb32Bit ? OS_VI_UNK2 | OS_VI_UNK1 : OS_VI_UNK2);

    if (modeLAN1) {
        // Anti-aliased, fetch extra lines as-needed
        viMode->comRegs.ctrl |= 0x100;
    } else if (modeLPN2 | modeHPN2) {
        // Point-sampled, resampling disabled
        viMode->comRegs.ctrl |= 0x300;
    } else {
        if (antialiasOff) {
            // Point-sampled, resampling enabled
            viMode->comRegs.ctrl |= 0x200;
        } else {
            // Anti-aliased, always fetch extra lines
            viMode->comRegs.ctrl |= 0;
        }
    }

    viMode->comRegs.width = width * (hiResInterlaced ? 2 : 1);

    if (tvType < 3) {
        static const ViModeStruct D_801DF0C0[] = {
            {
                // OS_TV_PAL
                BURST(58, 35, 4, 64),
                VSYNC(624),
                HSYNC(3177, 21),
                LEAP(3183, 3182),
                HSTART(128, 768),
                START(95, 569),
                BURST(107, 2, 9, 0),
            },
            {
                // OS_TV_NTSC
                BURST(57, 34, 5, 62),
                VSYNC(524),
                HSYNC(3093, 0),
                LEAP(3093, 3093),
                HSTART(108, 748),
                START(37, 511),
                BURST(4, 2, 14, 0),
            },
            {
                // OS_TV_MPAL
                BURST(57, 30, 5, 70),
                VSYNC(524),
                HSYNC(3088, 0),
                LEAP(3100, 3100),
                HSTART(108, 748),
                START(37, 511),
                BURST(4, 2, 14, 0),
            },
        };
        const ViModeStruct* ptr = &D_801DF0C0[tvType];

        viMode->comRegs.burst = ptr->burst;
        viMode->comRegs.vSync = ptr->vSync;
        viMode->comRegs.hSync = ptr->hSync;
        viMode->comRegs.leap = ptr->leap;
        viMode->comRegs.hStart = ptr->hStart;
        viMode->fldRegs[0].vStart = ptr->vStart;
        viMode->fldRegs[1].vStart = ptr->vStart;
        viMode->fldRegs[0].vBurst = ptr->vBurst;
        viMode->fldRegs[1].vBurst = ptr->vBurst;
    } else {
        _dbg_hungup("../z_vimode.c", 216);
    }

    viMode->comRegs.hStart += (leftAdjust << 16) + (s16)rightAdjust;
    viMode->fldRegs[0].vStart += (upperAdjust << 16) + (s16)lowerAdjust;
    viMode->fldRegs[1].vStart += (upperAdjust << 16) + (s16)lowerAdjust;

    if (loResDeinterlaced) {
        viMode->comRegs.vSync++;
        if (tvType == OS_TV_MPAL) {
            viMode->comRegs.hSync += HSYNC(1, 4);
            viMode->comRegs.leap += LEAP(-4, -2);
        }
    } else {
        viMode->fldRegs[0].vStart += START(-3, -2);
        if (tvType == OS_TV_MPAL) {
            viMode->fldRegs[0].vBurst += BURST(-2, -1, 12, -1);
        } else if (tvType == OS_TV_PAL) {
            viMode->fldRegs[1].vBurst += BURST(-2, -1, 2, 0);
        }
    }

    viMode->comRegs.xScale = (SCREEN_WIDTH << 10) / SCREEN_WIDTH;
    viMode->comRegs.vCurrent = 0;

    viMode->fldRegs[0].origin = ORIGIN(width * 2 * (fb16Bit ? 1 : 2));
    viMode->fldRegs[1].origin = ORIGIN(width * 2 * (fb16Bit ? 1 : 2) * (loRes ? 1 : 2));

    viMode->fldRegs[0].yScale = yScaleLo | yScaleHiEvenField;
    viMode->fldRegs[1].yScale = yScaleLo | yScaleHiOddField;

    viMode->fldRegs[0].vIntr = 2;
    viMode->fldRegs[1].vIntr = 2;
}

extern OSViMode osViModeNtscHpf1;
extern OSViMode osViModePalLan1;
extern OSViMode osViModeNtscHpn1;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeMpalLan1;
extern OSViMode osViModeFpalLan1;

void ViMode_Save(ViMode* viMode) {
#if 0
    R_VI_MODE_EDIT_STATE = viMode->editState;
    R_VI_MODE_EDIT_WIDTH = viMode->viWidth;
    R_VI_MODE_EDIT_HEIGHT = viMode->viHeight;
    R_VI_MODE_EDIT_ULY_ADJ = viMode->upperAdjust;
    R_VI_MODE_EDIT_LRY_ADJ = viMode->lowerAdjust;
    R_VI_MODE_EDIT_ULX_ADJ = viMode->leftAdjust;
    R_VI_MODE_EDIT_LRX_ADJ = viMode->rightAdjust;

    if (SREG(58) == 1) {
        SREG(58) = 0;
        switch (SREG(59)) {
            case 1:
                ViMode_LogPrint(&osViModePalLan1);
                break;

            case 2:
                ViMode_LogPrint(&osViModeFpalLan1);
                break;

            default:
                ViMode_LogPrint(&viMode->customViMode);
                break;
        }
    }
#endif
}

void ViMode_Load(ViMode* viMode) {
    //! @bug This condition always fails as the lowest bit is masked out to 0
    if ((R_VI_MODE_EDIT_WIDTH & ~3) == 1) {
        R_VI_MODE_EDIT_WIDTH += 4;
    }

    viMode->editState = R_VI_MODE_EDIT_STATE;
    viMode->viWidth = R_VI_MODE_EDIT_WIDTH & ~3;
    viMode->viHeight = R_VI_MODE_EDIT_HEIGHT;
    viMode->upperAdjust = R_VI_MODE_EDIT_ULY_ADJ;
    viMode->lowerAdjust = R_VI_MODE_EDIT_LRY_ADJ;
    viMode->leftAdjust = R_VI_MODE_EDIT_ULX_ADJ;
    viMode->rightAdjust = R_VI_MODE_EDIT_LRX_ADJ;
}

void ViMode_Init(ViMode* viMode) {
    viMode->editState = VI_MODE_EDIT_STATE_INACTIVE;
    viMode->viWidth = SCREEN_WIDTH;
    viMode->viHeight = SCREEN_HEIGHT;
    viMode->leftAdjust = 0;
    viMode->rightAdjust = 0;
    viMode->upperAdjust = 0;
    viMode->lowerAdjust = 0;
    viMode->viFeatures = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
    viMode->tvType = osTvType;
    viMode->fb16Bit = true;
    viMode->modeN = true;
    viMode->antialiasOff = false;
    viMode->loRes = true;

    ViMode_Save(viMode);
}

void ViMode_Destroy(ViMode* viMode) {
}

void ViMode_ConfigureFeatures(ViMode* viMode, s32 viFeatures) {
#if 0
    u32 ctrl = viMode->customViMode.comRegs.ctrl;

    if (viFeatures & OS_VI_GAMMA_ON) {
        ctrl |= OS_VI_GAMMA;
    }
    if (viFeatures & OS_VI_GAMMA_OFF) {
        ctrl &= ~OS_VI_GAMMA;
    }
    if (viFeatures & OS_VI_GAMMA_DITHER_ON) {
        ctrl |= OS_VI_GAMMA_DITHER;
    }
    if (viFeatures & OS_VI_GAMMA_DITHER_OFF) {
        ctrl &= ~OS_VI_GAMMA_DITHER;
    }
    if (viFeatures & OS_VI_DIVOT_ON) {
        ctrl |= OS_VI_DIVOT;
    }
    if (viFeatures & OS_VI_DIVOT_OFF) {
        ctrl &= ~OS_VI_DIVOT;
    }
    viMode->customViMode.comRegs.ctrl = ctrl;
#endif
}

/**
 * Updates the custom VI mode with controller input and any edits made with the REG editor
 * (through R_VI_MODE_EDIT_* entries)
 */
void ViMode_Update(ViMode* viMode, Input* input) {
#if 0
    ViMode_Load(viMode);

    if ((viMode->editState == VI_MODE_EDIT_STATE_ACTIVE) || (viMode->editState == VI_MODE_EDIT_STATE_2) ||
        (viMode->editState == VI_MODE_EDIT_STATE_3)) {
        gScreenWidth = viMode->viWidth;
        gScreenHeight = viMode->viHeight;

        // Controls to reset the ViMode to defaults
        if (CHECK_BTN_ALL(input->cur.button, BTN_START | BTN_CUP | BTN_CRIGHT)) {
            ViMode_Init(viMode);
        }

        // Controls to adjust the screen dimensions (upper-left)
        if (CHECK_BTN_ALL(input->cur.button, BTN_CUP)) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                viMode->upperAdjust--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                viMode->upperAdjust++;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                viMode->leftAdjust--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                viMode->leftAdjust++;
            }
        }

        // Controls to adjust the screen dimensions (lower-right)
        if (CHECK_BTN_ALL(input->cur.button, BTN_CRIGHT)) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                viMode->lowerAdjust--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                viMode->lowerAdjust++;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                viMode->rightAdjust--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                viMode->rightAdjust++;
            }
        }

        // Controls to adjust key features
        if (CHECK_BTN_ALL(input->cur.button, BTN_CDOWN)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                viMode->loRes = !viMode->loRes;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                viMode->antialiasOff = !viMode->antialiasOff;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                viMode->modeN = !viMode->modeN;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                viMode->fb16Bit = !viMode->fb16Bit;
            }
        }

        // Clamp adjustments
        if (viMode->editState >= VI_MODE_EDIT_STATE_2) {
            // Allow parts of the framebuffer to possibly be offscreen by a small margin
            if (viMode->leftAdjust < -16) {
                viMode->leftAdjust = -16;
            }
            if (viMode->upperAdjust < -50) {
                viMode->upperAdjust = -50;
            }
            if (viMode->rightAdjust > 16) {
                viMode->rightAdjust = 16;
            }
            if (viMode->lowerAdjust > 50) {
                viMode->lowerAdjust = 50;
            }
        } else {
            // Do not allow parts of the framebuffer to end up offscreen
            if (viMode->leftAdjust < 0) {
                viMode->leftAdjust = 0;
            }
            if (viMode->upperAdjust < 0) {
                viMode->upperAdjust = 0;
            }
            if (viMode->rightAdjust > 0) {
                viMode->rightAdjust = 0;
            }
            if (viMode->lowerAdjust > 0) {
                viMode->lowerAdjust = 0;
            }
        }

        ViMode_Configure(&viMode->customViMode, OS_VI_UNK28, osTvType, viMode->loRes, viMode->antialiasOff,
                         viMode->modeN, viMode->fb16Bit, viMode->viWidth, viMode->viHeight, viMode->leftAdjust,
                         viMode->rightAdjust, viMode->upperAdjust, viMode->lowerAdjust);
        ViMode_ConfigureFeatures(viMode, viMode->viFeatures);

        if (viMode->editState == VI_MODE_EDIT_STATE_3) {
            // Log comparison between the NTSC LAN1 mode and the custom mode
            ViMode_LogPrint(&osViModeNtscLan1);
            ViMode_LogPrint(&viMode->customViMode);
            viMode->editState = VI_MODE_EDIT_STATE_2;
        }
    }

    ViMode_Save(viMode);
#endif
}
