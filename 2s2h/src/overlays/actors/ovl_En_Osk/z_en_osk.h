#ifndef Z_EN_OSK_H
#define Z_EN_OSK_H

#include "global.h"
#include "objects/object_ikn_demo/object_ikn_demo.h"

struct EnOsk;

typedef void (*EnOskActionFunc)(struct EnOsk*, PlayState*);

#define ENOSK_GET_TYPE(thisx) ((thisx)->params & 0xF)

typedef enum EnOskType {
    /* 0 */ ENOSK_TYPE_0,
    /* 1 */ ENOSK_TYPE_1,
    /* 2 */ ENOSK_TYPE_2
} EnOskType;

#define OSK_LIMB_MAX MAX(MAX((s32)OBJECT_IKN_DEMO_1_LIMB_MAX, (s32)OBJECT_IKN_DEMO_2_LIMB_MAX), (s32)OBJECT_IKN_DEMO_3_LIMB_MAX)

typedef struct EnOsk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OSK_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[OSK_LIMB_MAX];
    /* 0x254 */ s16 animIndex;
    /* 0x256 */ s16 cueId;
    /* 0x258 */ u16 cueType;
    /* 0x25C */ f32 animSfxFrame;
    /* 0x260 */ EnOskActionFunc actionFunc;
} EnOsk; // size = 0x264

#endif // Z_EN_OSK_H
