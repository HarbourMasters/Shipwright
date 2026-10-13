#ifndef Z_EN_ZOD_H
#define Z_EN_ZOD_H

#include "global.h"
#include "assets/objects/object_zod/object_zod.h"

#define ENZOD_GET_ENTRANCE_INDEX(thisx) (((thisx)->params & 0xFE00) >> 9)
#define ENZOD_GET_TYPE(thisx) ((thisx)->params & 0xF)

struct EnZod;

typedef void (*EnZodActionFunc)(struct EnZod*, PlayState*);

typedef struct EnZod {
    /* 0x000 */ Actor actor;
    /* 0x144 */ Vec3s morphTable[TIJO_LIMB_MAX];
    /* 0x180 */ Vec3s jointTable[TIJO_LIMB_MAX];
    /* 0x1BC */ SkelAnime skelAnime;
    /* 0x200 */ ColliderCylinder collider;
    /* 0x24C */ s16 eyeIndex;
    /* 0x24E */ s16 blinkTimer;
    /* 0x250 */ Vec3s headRot;
    /* 0x256 */ u16 stateFlags;
    /* 0x258 */ s16 curAnimIndex;
    /* 0x25A */ s16 nextAnimIndex;
    /* 0x25C */ s16 cymbalRots[3];
    /* 0x262 */ s16 cymbalRotVels[3];
    /* 0x268 */ f32 drumScales[6];
    /* 0x280 */ f32 drumScaleVels[6];
    /* 0x298 */ s16 unk_298; // Set but not used.
    /* 0x29A */ s16 fogNear;
    /* 0x29C */ EnZodActionFunc actionFunc;
} EnZod; // size = 0x2A0

typedef enum {
    /* 0x1 */ ENZOD_TYPE_1 = 1,
    /* 0x2 */ ENZOD_TYPE_2,
    /* 0xF */ ENZOD_TYPE_0xF = 0xF
} EnZodType;

#endif // Z_EN_ZOD_H
