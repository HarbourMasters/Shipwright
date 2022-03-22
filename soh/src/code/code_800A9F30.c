#include "global.h"

UnkRumbleStruct D_80160FD0;

void func_800A9F30(PadMgr* a, s32 b) {
    func_800D2E30(&D_80160FD0);
    PadMgr_RumbleSet(a, D_80160FD0.rumbleEnable);
}

void func_800A9F6C(f32 a, u8 b, u8 c, u8 d) {
    s32 temp1;
    s32 temp2;

    if (1000000.0f < a) {
        temp1 = 1000;
    } else {
        temp1 = sqrtf(a);
    }

    if ((temp1 < 1000) && (b != 0) && (d != 0)) {
        temp2 = b - (temp1 * 255) / 1000;
        if (temp2 > 0) {
            D_80160FD0.unk_10A = temp2;
            D_80160FD0.unk_10B = c;
            D_80160FD0.unk_10C = d;
        }
    }
}

void func_800AA000(f32 a, u8 b, u8 c, u8 d) {
    s32 temp1;
    s32 temp2;
    s32 i;

    if (1000000.0f < a) {
        temp1 = 1000;
    } else {
        temp1 = sqrtf(a);
    }

    if (temp1 < 1000 && b != 0 && d != 0) {
        temp2 = b - (temp1 * 255) / 1000;

        for (i = 0; i < 0x40; i++) {
            if (D_80160FD0.unk_04[i] == 0) {
                if (temp2 > 0) {
                    D_80160FD0.unk_04[i] = temp2;
                    D_80160FD0.unk_44[i] = c;
                    D_80160FD0.unk_84[i] = d;
                }
                break;
            }
        }
    }
}

void func_800AA0B4(void) {
    func_800D3140(&D_80160FD0);

    gPadMgr.retraceCallback = func_800A9F30;
    gPadMgr.retraceCallbackValue = 0;

    if (1) {}
}

void func_800AA0F0(void) {
    PadMgr* padmgr = &gPadMgr;

    if ((padmgr->retraceCallback == func_800A9F30) && (padmgr->retraceCallbackValue == 0)) {
        padmgr->retraceCallback = NULL;
        padmgr->retraceCallbackValue = 0;
    }

    func_800D3178(&D_80160FD0);
}

u32 func_800AA148(void) {
    return gPadMgr.pakType[0] == 1;
}

void func_800AA15C(void) {
    D_80160FD0.unk_104 = 2;
}

void func_800AA16C(void) {
    D_80160FD0.unk_104 = 0;
}

void func_800AA178(u32 a) {
    D_80160FD0.unk_105 = !!a;
}
