#ifndef Z_EN_DNH_H
#define Z_EN_DNH_H

#include "global.h"
#include "objects/object_tro/object_tro.h"

struct EnDnh;

typedef void (*EnDnhActionFunc)(struct EnDnh*, PlayState*);

typedef void (*EnDnhActionFunc)(struct EnDnh*, PlayState*);

typedef struct EnDnh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnDnhActionFunc actionFunc;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ u16 unk18C;
    /* 0x190 */ MsgScript* msgScript;
    /* 0x194 */ s32 msgScriptPos;
    /* 0x198 */ s16 unk198;
    /* 0x19A */ UNK_TYPE1 pad19A[0x2];
    /* 0x19C */ s16 blinkTimer;
    /* 0x19E */ s16 eyeTexIndex;
    /* 0x1A0 */ MsgScriptCallback msgScriptCallback;
    /* 0x1A4 */ Vec3s jointTable[KOUME_KIOSK_LIMB_MAX];
    /* 0x1B0 */ Vec3s morphTable[KOUME_KIOSK_LIMB_MAX];
} EnDnh; // size = 0x1BC

#endif // Z_EN_DNH_H
