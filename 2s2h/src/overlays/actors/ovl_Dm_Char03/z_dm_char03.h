#ifndef Z_DM_CHAR03_H
#define Z_DM_CHAR03_H

#include "global.h"

struct DmChar03;

typedef void (*DmChar03ActionFunc)(struct DmChar03*, PlayState*);

typedef struct DmChar03 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmChar03ActionFunc actionFunc;
    /* 0x18C */ s16 animIndex;
    /* 0x18E */ u8 unk_18E;
    /* 0x18F */ u8 objectSlot;
    /* 0x190 */ Vec3f offset;
} DmChar03; // size = 0x19C

#endif // Z_DM_CHAR03_H
