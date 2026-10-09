#ifndef Z_EN_TOTO_H
#define Z_EN_TOTO_H

#include "global.h"
#include "face_change.h"
#include "objects/object_zm/object_zm.h"

struct EnToto;

typedef void (*EnTotoActionFunc)(struct EnToto*, PlayState*);
typedef s32 (*EnTotoUnkFunc)(struct EnToto*, PlayState*);

#define ENTOTO_GET_SWITCH_FLAG_1(thisx) ((thisx)->params & 0x7F)
#define ENTOTO_GET_SWITCH_FLAG_2(thisx) (((thisx)->params >> 7) & 0x7F)
#define ENTOTO_GET_SWITCH_FLAG_3(thisx) ((thisx)->home.rot.x)

typedef struct EnTotoText {
    /* 0x0 */ u8 unk0;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ u16 textId;
} EnTotoText; // size = 0x4

typedef struct EnTotoUnkStruct2 {
    /* 0x0 */ u16 unk0;
    /* 0x2 */ u16 unk2;
    /* 0x4 */ u16 unk4;
    /* 0x6 */ Vec3s unk6;
} EnTotoUnkStruct2; // size = 0xC

typedef struct EnToto {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_ZM_LIMB_MAX];
    /* 0x1F4 */ Vec3s morphTable[OBJECT_ZM_LIMB_MAX];
    /* 0x260 */ FaceChange faceChange;
    /* 0x264 */ ColliderCylinder collider;
    /* 0x2B0 */ u8 actionFuncIndex;
    /* 0x2B1 */ u8 unk2B1;
    /* 0x2B2 */ s8 csId;
    /* 0x2B3 */ u8 unk2B3;
    /* 0x2B4 */ u8 animIndex;
    /* 0x2B5 */ u8 cueId;
    /* 0x2B6 */ u8 unk2B6;
    /* 0x2B7 */ u8 unk2B7;
    /* 0x2B8 */ EnTotoText* text;
    /* 0x2BC */ PlayerOverrideInputEntry overrideInputEntry;
    /* 0x2C4 */ Actor* spotlights;
    /* 0x2C8 */ s32 pad2C8;
} EnToto; // size = 0x2CC

#endif // Z_EN_TOTO_H
