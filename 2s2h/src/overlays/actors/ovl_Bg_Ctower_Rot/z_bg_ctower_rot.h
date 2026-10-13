#ifndef Z_BG_CTOWER_ROT_H
#define Z_BG_CTOWER_ROT_H

#include "global.h"

struct BgCtowerRot;

typedef void (*BgCtowerRotActionFunc)(struct BgCtowerRot*, PlayState*);

typedef enum {
    /* 0 */ BGCTOWERROT_CORRIDOR,
    /* 1 */ BGCTOWERROT_STONE_DOOR_MAIN, // Main door controls cutscene flow (updating Link fields and playing sounds). 
                                         // It is the one on the left when entering from the Lost Woods
    /* 2 */ BGCTOWERROT_STONE_DOOR
} BgCtowerRotType;

typedef struct BgCtowerRot {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgCtowerRotActionFunc actionFunc;
    /* 0x160 */ f32 unk160; // Set to 0 but never used
    /* 0x164 */ f32 timer;
} BgCtowerRot; // size = 0x168

#endif // Z_BG_CTOWER_ROT_H
