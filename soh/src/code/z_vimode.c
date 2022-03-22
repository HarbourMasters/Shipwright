#include "global.h"

void ViMode_LogPrint(OSViMode* osViMode) {
    LOG_ADDRESS("osvimodep", osViMode, "../z_vimode.c", 87);
    LOG_ADDRESS("osvimodep->comRegs.ctrl", osViMode->comRegs.ctrl, "../z_vimode.c", 88);
    LOG_ADDRESS("osvimodep->comRegs.width", osViMode->comRegs.width, "../z_vimode.c", 89);
    LOG_ADDRESS("osvimodep->comRegs.burst", osViMode->comRegs.burst, "../z_vimode.c", 90);
    LOG_ADDRESS("osvimodep->comRegs.vSync", osViMode->comRegs.vSync, "../z_vimode.c", 91);
    LOG_ADDRESS("osvimodep->comRegs.hSync", osViMode->comRegs.hSync, "../z_vimode.c", 92);
    LOG_ADDRESS("osvimodep->comRegs.leap", osViMode->comRegs.leap, "../z_vimode.c", 93);
    LOG_ADDRESS("osvimodep->comRegs.hStart", osViMode->comRegs.hStart, "../z_vimode.c", 94);
    LOG_ADDRESS("osvimodep->comRegs.xScale", osViMode->comRegs.xScale, "../z_vimode.c", 95);
    LOG_ADDRESS("osvimodep->fldRegs[0].vStart", osViMode->fldRegs[0].vStart, "../z_vimode.c", 96);
    LOG_ADDRESS("osvimodep->fldRegs[0].vBurst", osViMode->fldRegs[0].vBurst, "../z_vimode.c", 97);
    LOG_ADDRESS("osvimodep->fldRegs[0].origin", osViMode->fldRegs[0].origin, "../z_vimode.c", 98);
    LOG_ADDRESS("osvimodep->fldRegs[0].yScale", osViMode->fldRegs[0].yScale, "../z_vimode.c", 99);
    LOG_ADDRESS("osvimodep->fldRegs[0].vIntr", osViMode->fldRegs[0].vIntr, "../z_vimode.c", 100);
    LOG_ADDRESS("osvimodep->fldRegs[1].vStart", osViMode->fldRegs[1].vStart, "../z_vimode.c", 101);
    LOG_ADDRESS("osvimodep->fldRegs[1].vBurst", osViMode->fldRegs[1].vBurst, "../z_vimode.c", 102);
    LOG_ADDRESS("osvimodep->fldRegs[1].origin", osViMode->fldRegs[1].origin, "../z_vimode.c", 103);
    LOG_ADDRESS("osvimodep->fldRegs[1].yScale", osViMode->fldRegs[1].yScale, "../z_vimode.c", 104);
    LOG_ADDRESS("osvimodep->fldRegs[1].vIntr", osViMode->fldRegs[1].vIntr, "../z_vimode.c", 105);
}

// This function configures the custom VI mode (`viMode.customViMode`) based on the other flags in `viMode`.
void ViMode_Configure(ViMode* viMode, s32 mode, s32 type, s32 unk_70, s32 unk_74, s32 unk_78, s32 unk_7C, s32 width,
                      s32 height, s32 unk_left, s32 unk_right, s32 unk_top, s32 unk_bottom) {
    s32 not_70;
    s32 not_74;
    s32 not_78;
    s32 not_7C;
    s32 cond_4C;
    s32 cond_48;
    s32 cond_44;
    s32 cond_40;
    s32 cond_3C;
    s32 cond_38;
    s32 cond_34;
    s32 yScaleLo;
    s32 yScaleHi0;
    s32 yScaleHi1;

    not_70 = !unk_70;
    not_74 = !unk_74;
    not_78 = !unk_78;
    not_7C = !unk_7C;

    cond_4C = not_70 && not_78;
    cond_48 = not_70 && unk_78;
    cond_44 = unk_70 && unk_78;
    cond_40 = unk_70 && not_78;
    cond_3C = unk_70 && not_74 && unk_78 && unk_7C;
    cond_38 = unk_70 && unk_74 && unk_78 && not_7C;
    cond_34 = not_70 && unk_74 && unk_78 && not_7C;

    unk_top &= ~1;
    unk_bottom &= ~1;

    yScaleLo = (cond_4C ? 2 : 1) * (((height << 11)) / (SCREEN_HEIGHT * 2 + unk_bottom - unk_top) / (unk_70 ? 1 : 2));

    yScaleHi0 = not_78 ? (cond_40 ? 0x1000000 : 0x2000000) : 0;
    yScaleHi1 = not_78 ? (cond_40 ? 0x3000000 : 0x2000000) : 0;

    viMode->customViMode.type = mode;
    viMode->customViMode.comRegs.ctrl = OS_VI_UNK2000 | OS_VI_UNK1000 | OS_VI_GAMMA | OS_VI_GAMMA_DITHER |
                                        (!cond_44 ? OS_VI_UNK40 : 0) | (not_74 ? OS_VI_DIVOT : 0) |
                                        (not_7C ? OS_VI_UNK2 | OS_VI_UNK1 : OS_VI_UNK2);

    if (cond_3C) {
        viMode->customViMode.comRegs.ctrl |= 0x100;
    } else if (cond_38 | cond_34) {
        viMode->customViMode.comRegs.ctrl |= 0x300;
    } else if (unk_74) {
        viMode->customViMode.comRegs.ctrl |= 0x200;
    } else {
        viMode->customViMode.comRegs.ctrl |= 0;
    }

    viMode->customViMode.comRegs.width = width * (cond_48 ? 2 : 1);

    if (type == 1) {
        viMode->customViMode.comRegs.burst = 0x3E52239;
        viMode->customViMode.comRegs.vSync = 0x20C;
        viMode->customViMode.comRegs.hSync = 0xC15;
        viMode->customViMode.comRegs.leap = 0xC150C15;
        viMode->customViMode.comRegs.hStart = 0x6C02EC;
        viMode->customViMode.fldRegs[0].vStart = 0x2501FF;
        viMode->customViMode.fldRegs[0].vBurst = 0xE0204;
    } else if (type == 0) {
        viMode->customViMode.comRegs.burst = 0x404233A;
        viMode->customViMode.comRegs.vSync = 0x270;
        viMode->customViMode.comRegs.hSync = 0x150C69;
        viMode->customViMode.comRegs.leap = 0xC6F0C6E;
        viMode->customViMode.comRegs.hStart = 0x800300;
        viMode->customViMode.fldRegs[0].vStart = 0x5F0239;
        viMode->customViMode.fldRegs[0].vBurst = 0x9026B;
    } else if (type == 2) {
        viMode->customViMode.comRegs.burst = 0x4651E39;
        viMode->customViMode.comRegs.vSync = 0x20C;
        viMode->customViMode.comRegs.hSync = 0xC10;
        viMode->customViMode.comRegs.leap = 0xC1C0C1C;
        viMode->customViMode.comRegs.hStart = 0x6C02EC;
        viMode->customViMode.fldRegs[0].vStart = 0x2501FF;
        viMode->customViMode.fldRegs[0].vBurst = 0xE0204;
    }

    viMode->customViMode.fldRegs[1].vStart = viMode->customViMode.fldRegs[0].vStart;

    viMode->customViMode.comRegs.hStart += (unk_left << 16) + (s16)unk_right;
    viMode->customViMode.fldRegs[0].vStart += (unk_top << 16) + (s16)unk_bottom;
    viMode->customViMode.fldRegs[1].vStart += (unk_top << 16) + (s16)unk_bottom;

    viMode->customViMode.fldRegs[1].vBurst = viMode->customViMode.fldRegs[0].vBurst;

    if (cond_44) {
        viMode->customViMode.comRegs.vSync++;
        if (type == 2) {
            viMode->customViMode.comRegs.hSync += 0x40001;
        }
        if (type == 2) {
            viMode->customViMode.comRegs.leap += 0xFFFCFFFE;
        }
    } else {
        viMode->customViMode.fldRegs[0].vStart += 0xFFFDFFFE;
        if (type == 2) {
            viMode->customViMode.fldRegs[0].vBurst += 0xFFFCFFFE;
        }
        if (type == 0) {
            viMode->customViMode.fldRegs[1].vBurst += 0x2FFFE;
        }
    }

    viMode->customViMode.comRegs.xScale = (width << 10) / (SCREEN_WIDTH * 2 + unk_right - unk_left);
    viMode->customViMode.comRegs.vCurrent = 0;

    viMode->customViMode.fldRegs[0].origin = width * 2 * (unk_7C ? 1 : 2);
    viMode->customViMode.fldRegs[1].origin = width * 2 * (unk_7C ? 1 : 2) * (unk_70 ? 1 : 2);

    viMode->customViMode.fldRegs[0].yScale = yScaleLo | yScaleHi0;
    viMode->customViMode.fldRegs[1].yScale = yScaleLo | yScaleHi1;

    viMode->customViMode.fldRegs[0].vIntr = 2;
    viMode->customViMode.fldRegs[1].vIntr = 2;
}

void ViMode_Save(ViMode* viMode) {
    SREG(48) = viMode->viModeBase;
    SREG(49) = viMode->viWidth;
    SREG(50) = viMode->viHeight;
    SREG(51) = viMode->unk_64;
    SREG(52) = viMode->unk_60;
    SREG(53) = viMode->unk_5C;
    SREG(54) = viMode->unk_58;
    if (SREG(58) == 1) {
        SREG(58) = 0;
        switch (SREG(59)) {
            case 1:
                osSyncPrintf("osViModePalLan1\n");
                ViMode_LogPrint(&osViModePalLan1);
                break;
            case 2:
                osSyncPrintf("osViModeFpalLan1\n");
                ViMode_LogPrint(&osViModeFpalLan1);
                break;
            default:
                osSyncPrintf("Custom\n");
                ViMode_LogPrint(&viMode->customViMode);
                break;
        }
    }
}

void ViMode_Load(ViMode* viMode) {
    if ((SREG(49) & ~3) == 1) {
        SREG(49) += 4;
    }

    viMode->viModeBase = SREG(48);
    viMode->viWidth = SREG(49) & ~3;
    viMode->viHeight = SREG(50);
    viMode->unk_64 = SREG(51);
    viMode->unk_60 = SREG(52);
    viMode->unk_5C = SREG(53);
    viMode->unk_58 = SREG(54);
}

void ViMode_Init(ViMode* viMode) {
    viMode->viModeBase = 0;
    viMode->viWidth = SCREEN_WIDTH;
    viMode->viHeight = SCREEN_HEIGHT;
    viMode->unk_5C = 0;
    viMode->unk_58 = 0;
    viMode->unk_64 = 0;
    viMode->unk_60 = 0;
    viMode->viFeatures = OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF;
    viMode->viTvType = osTvType;
    viMode->unk_7C = true;
    viMode->unk_78 = true;
    viMode->unk_74 = false;
    viMode->unk_70 = true;

    ViMode_Save(viMode);
}

void ViMode_Destroy(ViMode* viMode) {
}

void ViMode_ConfigureFeatures(ViMode* viMode, s32 viFeatures) {
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
}

// This function uses controller input (C buttons + D pad) to reconfigure the custom VI mode
void ViMode_Update(ViMode* viMode, Input* input) {
    ViMode_Load(viMode);
    if ((viMode->viModeBase == 1) || (viMode->viModeBase == 2) || (viMode->viModeBase == 3)) {
        gScreenWidth = viMode->viWidth;
        gScreenHeight = viMode->viHeight;
        if (CHECK_BTN_ALL(input->cur.button, BTN_START | BTN_CUP | BTN_CRIGHT)) {
            ViMode_Init(viMode);
        }
        if (CHECK_BTN_ALL(input->cur.button, BTN_CUP)) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                viMode->unk_64--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                viMode->unk_64++;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                viMode->unk_5C--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                viMode->unk_5C++;
            }
        }
        if (CHECK_BTN_ALL(input->cur.button, BTN_CRIGHT)) {
            if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
                viMode->unk_60--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
                viMode->unk_60++;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
                viMode->unk_58--;
            }
            if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
                viMode->unk_58++;
            }
        }
        if (CHECK_BTN_ALL(input->cur.button, BTN_CDOWN)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                viMode->unk_70 = !viMode->unk_70;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                viMode->unk_74 = !viMode->unk_74;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                viMode->unk_78 = !viMode->unk_78;
            }
            if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                viMode->unk_7C = !viMode->unk_7C;
            }
        }
        if (viMode->viModeBase >= 2) {
            if (viMode->unk_5C < -16) {
                viMode->unk_5C = -16;
            }
            if (viMode->unk_64 < -50) {
                viMode->unk_64 = -50;
            }
            if (viMode->unk_58 > 16) {
                viMode->unk_58 = 16;
            }
            if (viMode->unk_60 > 50) {
                viMode->unk_60 = 50;
            }
        } else {
            if (viMode->unk_5C < 0) {
                viMode->unk_5C = 0;
            }
            if (viMode->unk_64 < 0) {
                viMode->unk_64 = 0;
            }
            if (viMode->unk_58 > 0) {
                viMode->unk_58 = 0;
            }
            if (viMode->unk_60 > 0) {
                viMode->unk_60 = 0;
            }
        }
        ViMode_Configure(viMode, OS_VI_UNK28, osTvType, viMode->unk_70, viMode->unk_74, viMode->unk_78, viMode->unk_7C,
                         viMode->viWidth, viMode->viHeight, viMode->unk_5C, viMode->unk_58, viMode->unk_64,
                         viMode->unk_60);
        ViMode_ConfigureFeatures(viMode, viMode->viFeatures);
        if (viMode->viModeBase == 3) {
            ViMode_LogPrint(&osViModeNtscLan1);
            ViMode_LogPrint(&viMode->customViMode);
            viMode->viModeBase = 2;
        }
    }
    ViMode_Save(viMode);
}
