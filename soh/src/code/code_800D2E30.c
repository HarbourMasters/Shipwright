#include "global.h"
#include <string.h>

void func_800D2E30(UnkRumbleStruct* arg0) {
    static u8 D_8012DBB0 = 1;
    s32 i;
    s32 unk_a3;
    s32 index = -1;

    for (i = 0; i < 4; i++) {
        arg0->rumbleEnable[i] = 0;
    }

    if (arg0->unk_105 == 0) {
        if (D_8012DBB0 != 0) {
            for (i = 0; i < 4; i++) {
                gPadMgr.pakType[i] = 0;
            }
        }
        D_8012DBB0 = arg0->unk_105;
        return;
    }

    D_8012DBB0 = arg0->unk_105;

    if (arg0->unk_104 == 2) {
        for (i = 0; i < 4; ++i) {
            gPadMgr.pakType[i] = 0;
        }

        for (i = 0; i < 0x40; i++) {
            arg0->unk_C4[i] = 0;
            arg0->unk_84[i] = 0;
            arg0->unk_44[i] = 0;
            arg0->unk_04[i] = 0;
        }
        arg0->unk_106 = arg0->unk_108 = arg0->unk_10A = arg0->unk_10B = arg0->unk_10C = arg0->unk_10D = 0;
        arg0->unk_104 = 1;
    }
    if (arg0->unk_104 != 0) {
        for (i = 0; i < 0x40; i++) {
            if (arg0->unk_04[i] != 0) {
                if (arg0->unk_44[i] > 0) {
                    arg0->unk_44[i]--;
                } else {
                    unk_a3 = arg0->unk_04[i] - arg0->unk_84[i];
                    if (unk_a3 > 0) {
                        arg0->unk_04[i] = unk_a3;
                    } else {
                        arg0->unk_04[i] = 0;
                    }
                }

                unk_a3 = arg0->unk_C4[i] + arg0->unk_04[i];
                arg0->unk_C4[i] = unk_a3;
                if (index == -1) {
                    index = i;
                    arg0->rumbleEnable[0] = (unk_a3 >= 0x100);
                } else if (arg0->unk_04[index] < arg0->unk_04[i]) {
                    index = i;
                    arg0->rumbleEnable[0] = (unk_a3 >= 0x100);
                }
            }
        }
        if (arg0->unk_10A != 0) {
            if (arg0->unk_10B > 0) {
                arg0->unk_10B--;
            } else {
                unk_a3 = arg0->unk_10A - arg0->unk_10C;
                if (unk_a3 > 0) {
                    arg0->unk_10A = unk_a3;
                } else {
                    arg0->unk_10A = 0;
                }
            }
            unk_a3 = arg0->unk_10D + arg0->unk_10A;
            arg0->unk_10D = unk_a3;
            arg0->rumbleEnable[0] = (unk_a3 >= 0x100);
        }
        if (arg0->unk_10A != 0) {
            unk_a3 = arg0->unk_10A;
        } else {
            if (index == -1) {
                unk_a3 = 0;
            } else {
                unk_a3 = arg0->unk_04[index];
            }
        }
        if (unk_a3 == 0) {
            if ((++arg0->unk_108) >= 6) {
                arg0->unk_106 = 0;
                arg0->unk_108 = 5;
            }
        } else {
            arg0->unk_108 = 0;
            if ((++arg0->unk_106) >= 0x1C21) {
                arg0->unk_104 = 0;
            }
        }
    } else {
        for (i = 0; i < 0x40; i++) {
            arg0->unk_C4[i] = 0;
            arg0->unk_84[i] = 0;
            arg0->unk_44[i] = 0;
            arg0->unk_04[i] = 0;
        }

        arg0->unk_106 = arg0->unk_108 = arg0->unk_10A = arg0->unk_10B = arg0->unk_10C = arg0->unk_10D = 0;
    }
}

void func_800D3140(UnkRumbleStruct* arg0) {
    memset(arg0, 0, sizeof(UnkRumbleStruct));
    arg0->unk_104 = 2;
    arg0->unk_105 = 1;
}

void func_800D3178(UnkRumbleStruct* arg0) {
    memset(arg0, 0, sizeof(UnkRumbleStruct));
}
