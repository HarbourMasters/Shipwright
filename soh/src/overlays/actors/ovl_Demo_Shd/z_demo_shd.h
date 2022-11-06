#ifndef Z_DEMO_SHD_H
#define Z_DEMO_SHD_H

#include "ultra64.h"
#include "global.h"

struct DemoShd;

typedef void (*DemoShdActionFunc)(struct DemoShd*, PlayState*);

typedef struct DemoShd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ u16 unk_14C;
    /* 0x014E */ u16 unk_14E;
    /* 0x0150 */ DemoShdActionFunc actionFunc;
} DemoShd; // size = 0x0154

#endif
