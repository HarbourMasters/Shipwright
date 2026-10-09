#ifndef Z_EN_STREAM_H
#define Z_EN_STREAM_H

#include "global.h"

/**
 * Even in OoT, where this actor was used, every single instance of it had
 * a params of 0x0000, so it's hard to know how they intended to use this.
 * In the final game, only an EN_STREAM_SIZE of 1 does anything different.
 */
#define EN_STREAM_SIZE(thisx) ((thisx)->params & 0xFF)

typedef enum {
    /* 0 */ EN_STREAM_SIZE_NORMAL,
    /* 1 */ EN_STREAM_SIZE_SMALL
} EnStreamSize;

typedef enum {
    /* 0 */ EN_STREAM_PLAYER_OUTSIDE_RANGE,
    /* 1 */ EN_STREAM_PLAYER_WITHIN_RANGE_INSIDE_VORTEX,
    /* 2 */ EN_STREAM_PLAYER_WITHIN_RANGE_BELOW_VORTEX
} EnStreamPlayerLocation;

struct EnStream;

typedef void (*EnStreamActionFunc)(struct EnStream*, PlayState*);

typedef struct EnStream {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnStreamActionFunc actionFunc;
    /* 0x148 */ s32 size;
    /* 0x14C */ UNK_TYPE1 unk_14C[0x4];
} EnStream; // size = 0x150

#endif // Z_EN_STREAM_H
