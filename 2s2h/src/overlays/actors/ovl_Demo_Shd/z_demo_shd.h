#ifndef Z_DEMO_SHD_H
#define Z_DEMO_SHD_H

#include "global.h"

struct DemoShd;

typedef struct DemoShd {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ char unk_0144[0x118C];
} DemoShd; // size = 0x12D0

#endif // Z_DEMO_SHD_H
