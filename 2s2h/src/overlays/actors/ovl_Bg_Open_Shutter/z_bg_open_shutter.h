#ifndef Z_BG_OPEN_SHUTTER_H
#define Z_BG_OPEN_SHUTTER_H

#include "global.h"
#include "z64door.h"

struct BgOpenShutter;

typedef void (*BgOpenShutterActionFunc)(struct BgOpenShutter*, PlayState*);

typedef struct BgOpenShutter {
    /* 0x000 */ SlidingDoorActor slidingDoor;
    /* 0x160 */ BgOpenShutterActionFunc actionFunc;
    /* 0x164 */ s32 unk_164;
} BgOpenShutter; // size = 0x168

#endif // Z_BG_OPEN_SHUTTER_H
