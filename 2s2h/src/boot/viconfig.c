#include "libc/stdbool.h"
#include "idle.h"
#include "functions.h"

extern OSViMode osViModeNtscHpf1;
extern OSViMode osViModePalLan1;
extern OSViMode osViModeNtscHpn1;
extern OSViMode osViModeNtscLan1;
extern OSViMode osViModeMpalLan1;
extern OSViMode osViModeFpalLan1;

void ViConfig_UpdateVi(u32 black) {
#if 0
    if (black) {
        switch (osTvType) {
            case OS_TV_MPAL:
                osViSetMode(&osViModeMpalLan1);
                break;

            case OS_TV_PAL:
                osViSetMode(&osViModePalLan1);
                break;

            case OS_TV_NTSC:
            default:
                osViSetMode(&osViModeNtscLan1);
                break;
        }

        if (gViConfigFeatures != 0) {
            osViSetSpecialFeatures(gViConfigFeatures);
        }

        if (gViConfigYScale != 1) {
            osViSetYScale(1);
        }
    } else {
        osViSetMode(&gViConfigMode);

        if (gViConfigAdditionalScanLines != 0) {
            osViExtendVStart(gViConfigAdditionalScanLines);
        }

        if (gViConfigFeatures != 0) {
            osViSetSpecialFeatures(gViConfigFeatures);
        }

        if (gViConfigXScale != 1) {
            osViSetXScale(gViConfigXScale);
        }

        if (gViConfigYScale != 1) {
            osViSetYScale(gViConfigYScale);
        }
    }

    gViConfigUseBlack = black;
#endif
}

void ViConfig_UpdateBlack(void) {
    if (gViConfigUseBlack) {
        osViBlack(true);
    } else {
        osViBlack(false);
    }
}
