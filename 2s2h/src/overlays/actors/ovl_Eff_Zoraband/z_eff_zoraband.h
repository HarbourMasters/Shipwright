#ifndef Z_EFF_ZORABAND_H
#define Z_EFF_ZORABAND_H

#include "global.h"

#define EFFZORABAND_GET_F(thisx) ((thisx)->params & 0xF)

struct EffZoraband;

typedef void (*EffZorabandActionFunc)(struct EffZoraband*, PlayState*);

typedef struct EffZoraband {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 alpha;
    /* 0x146 */ u16 stateFlags;
    /* 0x148 */ EffZorabandActionFunc actionFunc;
} EffZoraband; // size = 0x14C

#endif // Z_EFF_ZORABAND_H
