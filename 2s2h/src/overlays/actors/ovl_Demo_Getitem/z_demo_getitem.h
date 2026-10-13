#ifndef Z_DEMO_GETITEM_H
#define Z_DEMO_GETITEM_H

#include "global.h"

#define DEMOGETITEM_GET_F(thisx) ((thisx)->params & 0xF)

struct DemoGetitem;

typedef void (*DemoGetitemActionFunc)(struct DemoGetitem*, PlayState*);

typedef struct DemoGetitem {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 getItemDrawId;
    /* 0x146 */ u16 cueType;
    /* 0x148 */ s8 objectSlot;
    /* 0x14C */ DemoGetitemActionFunc actionFunc;
} DemoGetitem; // size = 0x150

#endif // Z_DEMO_GETITEM_H
