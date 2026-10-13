#ifndef Z_EN_SYATEKI_WF_H
#define Z_EN_SYATEKI_WF_H

#include "global.h"
#include "objects/object_wf/object_wf.h"

#define SG_WOLFOS_GET_POINT_TO_HOWL(thisx) (((thisx)->params & 0xF0) >> 4)
#define SG_WOLFOS_GET_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)
#define SG_WOLFOS_PARAMS(index, pointToHowl, unused) (((index << 8) & 0xFF00) | ((pointToHowl << 4) & 0xF0) | (unused & 0xF))

struct EnSyatekiWf;

typedef void (*EnSyatekiWfActionFunc)(struct EnSyatekiWf*, PlayState*);

typedef struct EnSyatekiWf {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[WOLFOS_NORMAL_LIMB_MAX];
    /* 0x20C */ Vec3s morphTable[WOLFOS_NORMAL_LIMB_MAX];
    /* 0x290 */ UNK_TYPE1 unk_290[0x4];
    /* 0x294 */ EnSyatekiWfActionFunc actionFunc;
    /* 0x298 */ s16 isActive;
    /* 0x29A */ s16 timer;
    /* 0x29C */ s16 waitTimer;
    /* 0x2A0 */ Vec3s* pathPoints;
    /* 0x2A4 */ s16 currentPointIndex;
    /* 0x2A6 */ s16 maxPointIndex;
    /* 0x2A8 */ s16 yawTarget;
    /* 0x2AC */ f32 unk_2AC; // set to 10.0f, but never used
    /* 0x2B0 */ u8 eyeIndex;
    /* 0x2B4 */ ColliderCylinder bodyCollider;
    /* 0x300 */ ColliderCylinder tailCollider;
    /* 0x34C */ ColliderJntSph headCollider;
    /* 0x36C */ ColliderJntSphElement headColliderElements[1];
    /* 0x3AC */ UNK_TYPE1 unk_3AC[0x20];
} EnSyatekiWf; // size = 0x3CC

#endif // Z_EN_SYATEKI_WF_H
