#ifndef Z_EN_ZOS_H
#define Z_EN_ZOS_H

#include "global.h"
#include "objects/object_zos/object_zos.h"

struct EnZos;

typedef void (*EnZosActionFunc)(struct EnZos*, PlayState*);

#define ENZOS_GET_F(thisx) ((thisx)->params & 0xF)

typedef enum {
    /* 1 */ ENZOS_F_1 = 1,
    /* 2 */ ENZOS_F_2
} EnZosParam;

typedef struct EnZos {
    /* 0x000 */ Actor actor;
    /* 0x144 */ Vec3s jointTable[EVAN_LIMB_MAX];
    /* 0x1B0 */ Vec3s morphTable[EVAN_LIMB_MAX];
    /* 0x21C */ SkelAnime skelAnime;
    /* 0x260 */ ColliderCylinder collider;
    /* 0x2AC */ s16 eyeIndex;
    /* 0x2AE */ s16 blinkTimer;
    /* 0x2B0 */ UNK_TYPE1 unk2B0[6];
    /* 0x2B6 */ u16 unk_2B6;
    /* 0x2B8 */ s16 animIndex;
    /* 0x2BA */ s16 cueId;
    /* 0x2BC */ s16 unk_2BC;
    /* 0x2C0 */ EnZosActionFunc actionFunc;
} EnZos; // size = 0x2C4

#endif // Z_EN_ZOS_H
