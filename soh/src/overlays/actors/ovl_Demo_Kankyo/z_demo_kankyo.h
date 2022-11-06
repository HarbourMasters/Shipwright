#ifndef Z_DEMO_KANKYO_H
#define Z_DEMO_KANKYO_H

#include "ultra64.h"
#include "global.h"

struct DemoKankyo;

typedef void (*DemoKankyoActionFunc)(struct DemoKankyo*, PlayState*);

typedef enum {
    /* 0x00 */ DEMOKANKYO_BLUE_RAIN,
    /* 0x01 */ DEMOKANKYO_BLUE_RAIN_2,
    /* 0x02 */ DEMOKANKYO_ROCK_1,
    /* 0x03 */ DEMOKANKYO_ROCK_2,
    /* 0x04 */ DEMOKANKYO_ROCK_3,
    /* 0x05 */ DEMOKANKYO_ROCK_4,
    /* 0x06 */ DEMOKANKYO_ROCK_5,
    /* 0x07 */ DEMOKANKYO_CLOUDS,
    /* 0x08 */ DEMOKANKYO_8,        // These unnamed types are removed types that would have used object_gi_melody
    /* 0x09 */ DEMOKANKYO_9,
    /* 0x0A */ DEMOKANKYO_A,
    /* 0x0B */ DEMOKANKYO_B,
    /* 0x0C */ DEMOKANKYO_C,
    /* 0x0D */ DEMOKANKYO_DOOR_OF_TIME,
    /* 0x0E */ DEMOKANKYO_LIGHT_PLANE,
    /* 0x0F */ DEMOKANKYO_WARP_OUT,
    /* 0x10 */ DEMOKANKYO_WARP_IN,
    /* 0x11 */ DEMOKANKYO_SPARKLES
} DemoKankyoType;

typedef struct {
    /* 0x00 */ Vec3f unk_0;
    /* 0x0C */ Vec3f unk_C;
    /* 0x18 */ f32 unk_18;     // For Door of Time, this is the amount to translate it by used for when it's opening
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ u8 unk_22;      // mode ?
    /* 0x23 */ u8 unk_23;
    /* 0x24 */ s16 unk_24;
} DemoKankyoUnk150; // size = 0x28

typedef struct DemoKankyo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ u8 objBankIndex;
    /* 0x014D */ u8 sparkleCounter;
    /* 0x014E */ u8 warpTimer;
    /* 0x0150 */ DemoKankyoUnk150 unk_150[30];
    /* 0x0600 */ DemoKankyoActionFunc actionFunc;
} DemoKankyo; // size = 0x0604

#endif
