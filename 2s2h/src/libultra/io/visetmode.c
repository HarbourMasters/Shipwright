#include "ultra64.h"

void osViSetMode(OSViMode* modep) {
    register u32 saveMask = __osDisableInt();

    __osViNext->modep = modep;
    __osViNext->state = VI_STATE_MODE_UPDATED;
    __osViNext->features = __osViNext->modep->comRegs.ctrl;

    __osRestoreInt(saveMask);
}
