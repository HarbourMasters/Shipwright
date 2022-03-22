#include "global.h"

// TODO: name magic constants
void osViBlack(u8 active) {
    register u32 prevInt = __osDisableInt();

    if (active) {
        __osViNext->state |= 0x20;
    } else {
        __osViNext->state &= ~0x20;
    }
    __osRestoreInt(prevInt);
}
