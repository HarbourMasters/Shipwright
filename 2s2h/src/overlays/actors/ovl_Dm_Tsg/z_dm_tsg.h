#ifndef Z_DM_TSG_H
#define Z_DM_TSG_H

#include "global.h"

struct DmTsg;

#define DMTSG_UNK_SIZE 100

typedef struct DmTsg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x4];
    /* 0x148 */ Vec3f unk_148[DMTSG_UNK_SIZE];
    /* 0x5F8 */ f32 unk_5F8[DMTSG_UNK_SIZE];
    /* 0x788 */ f32 unk_788;
    /* 0x78C */ s16 unk_78C[DMTSG_UNK_SIZE];
    /* 0x854 */ s16 unk_854;
    /* 0x856 */ s16 unk_856[DMTSG_UNK_SIZE];
    /* 0x91E */ u8 canDraw;
} DmTsg; // size = 0x920

#endif // Z_DM_TSG_H
