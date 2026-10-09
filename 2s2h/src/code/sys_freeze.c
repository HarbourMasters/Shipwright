#include "global.h"

NORETURN void Sys_Freeze(void) {
    for (;;) {
        Sleep_Msec(1000);
    }
}
