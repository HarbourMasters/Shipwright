#ifndef Z_EN_RSN_H
#define Z_EN_RSN_H

#include "global.h"

struct EnRsn;

typedef void (*EnRsnActionFunc)(struct EnRsn*, PlayState*);

typedef struct EnRsn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk144[0x4C];
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnRsnActionFunc actionFunc;
    /* 0x1D8 */ Vec3s headRot;
    /* 0x1DE */ Vec3s torsoRot;
} EnRsn; // size = 0x1E4

#endif // Z_EN_RSN_H
