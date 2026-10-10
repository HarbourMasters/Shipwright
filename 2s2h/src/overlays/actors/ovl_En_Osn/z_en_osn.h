#ifndef Z_EN_OSN_H
#define Z_EN_OSN_H

#include "global.h"

struct EnOsn;

typedef void (*EnOsnActionFunc)(struct EnOsn*, PlayState*);

#define ENOSN_GET_TYPE(this) ((thisx)->params & 3) 

typedef enum {
    /* 0 */ OSN_TYPE_CHOOSE, // Will choose between Idle and Cutscene
    /* 1 */ OSN_TYPE_LIE_FACE_DOWN,
    /* 2 */ OSN_TYPE_LIE_FACE_UP,
    /* 3 */ OSN_TYPE_CUTSCENE
} OsnType;

typedef struct EnOsn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnOsnActionFunc actionFunc;
    /* 0x1D8 */ Vec3s headRot;
    /* 0x1DE */ Vec3s torsoRot;
    /* 0x1E4 */ UNK_TYPE1 unk_1E4[0x6];
    /* 0x1EA */ u16 stateFlags;
    /* 0x1EC */ u8 animIndex;
    /* 0x1ED */ u8 cueId;
    /* 0x1EE */ s16 csId;
    /* 0x1F0 */ u8 shouldRotateHead;
    /* 0x1F1 */ UNK_TYPE1 unk_1F1[0x3];
    /* 0x1F4 */ u16 textId;
    /* 0x1F6 */ s32 maskTextFlags;
    /* 0x1FA */ u8 alpha;
} EnOsn; // size = 0x1FB

#endif // Z_EN_OSN_H
