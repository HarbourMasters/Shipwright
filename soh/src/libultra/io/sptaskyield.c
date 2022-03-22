#include "global.h"

void osSpTaskYield(void) {
    __osSpSetStatus(SP_STATUS_SIG3);
}
