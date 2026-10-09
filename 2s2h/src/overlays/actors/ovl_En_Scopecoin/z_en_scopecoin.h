#ifndef Z_EN_SCOPECOIN_H
#define Z_EN_SCOPECOIN_H

#include "global.h"

struct EnScopecoin;

#define OBJMUPICT_GET_RUPEE_INDEX(thisx) ((thisx)->params & 0xF)
#define OBJMUPICT_GET_RUPEE_FLAG(thisx) (((thisx)->params & 0x7F0) >> 4)

typedef void (*EnScopecoinActionFunc)(struct EnScopecoin*, PlayState*);

typedef struct EnScopecoin {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnScopecoinActionFunc actionFunc;
    /* 0x148 */ s16 rupeeIndex;
} EnScopecoin; // size = 0x14C

#endif // Z_EN_SCOPECOIN_H
