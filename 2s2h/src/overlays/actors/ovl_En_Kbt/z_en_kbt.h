#ifndef Z_EN_KBT_H
#define Z_EN_KBT_H

#include "global.h"
#include "objects/object_kbt/object_kbt.h"

struct EnKbt;

typedef void (*EnKbtActionFunc)(struct EnKbt*, PlayState*);

typedef struct EnKbt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_KBT_LIMB_MAX];
    /* 0x200 */ Vec3s morphTable[OBJECT_KBT_LIMB_MAX];
    /* 0x278 */ Actor* unk_278;
    /* 0x27C */ u16 unk_27C;
    /* 0x27E */ u8 unk_27E;
    /* 0x27F */ u8 unk_27F;
    /* 0x280 */ s16 animIndex;
    /* 0x282 */ s16 unk_282;
    /* 0x284 */ s16 unk_284;
    /* 0x288 */ EnKbtActionFunc actionFunc;
} EnKbt; // size = 0x28C

#endif // Z_EN_KBT_H
