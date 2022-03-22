#include "global.h"
#include "vt.h"

// this should probably go elsewhere but right now viconfig.o is the only object between idle and z_std_dma
OSPiHandle* gCartHandle = 0;

void ViConfig_UpdateVi(u32 mode) {
    if (mode != 0) {
        osSyncPrintf(VT_COL(YELLOW, BLACK) "osViSetYScale1(%f);\n" VT_RST, 1.0f);

        if (osTvType == OS_TV_PAL) {
            osViSetMode(&osViModePalLan1);
        }

        osViSetYScale(1.0f);
    } else {
        osViSetMode(&gViConfigMode);

        if (gViConfigAdditionalScanLines != 0) {
            osViExtendVStart(gViConfigAdditionalScanLines);
        }

        if (gViConfigFeatures != 0) {
            osViSetSpecialFeatures(gViConfigFeatures);
        }

        if (gViConfigXScale != 1.0f) {
            osViSetXScale(gViConfigXScale);
        }

        if (gViConfigYScale != 1.0f) {
            osSyncPrintf(VT_COL(YELLOW, BLACK) "osViSetYScale3(%f);\n" VT_RST, gViConfigYScale);
            osViSetYScale(gViConfigYScale);
        }
    }

    gViConfigUseDefault = mode;
}

void ViConfig_UpdateBlack(void) {
    if (gViConfigUseDefault != 0) {
        osViBlack(1);
    } else {
        osViBlack(0);
    }
}
