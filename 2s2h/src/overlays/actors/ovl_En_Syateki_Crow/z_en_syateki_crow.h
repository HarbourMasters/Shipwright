#ifndef Z_EN_SYATEKI_CROW_H
#define Z_EN_SYATEKI_CROW_H

#include "global.h"
#include "objects/object_crow/object_crow.h"

#define SG_GUAY_GET_WAIT_MOD(thisx) ((thisx)->params & 0xF)
#define SG_GUAY_GET_SPEED_MOD(thisx) (((thisx)->params & 0xF0) >> 4)
#define SG_GUAY_GET_INDEX(thisx) (((thisx)->params & 0xFF00) >> 8)
#define SG_GUAY_PARAMS(index, speedMod, waitMod) (((index << 8) & 0xFF00) | ((speedMod << 4) & 0xF0) | (waitMod & 0xF))

struct EnSyatekiCrow;

typedef void (*EnSyatekiCrowActionFunc)(struct EnSyatekiCrow*, PlayState*);

typedef enum EnSyatekiCrowBodyPart {
    /* 0 */ SG_GUAY_BODYPART_BODY,
    /* 1 */ SG_GUAY_BODYPART_RIGHT_WING_TIP,
    /* 2 */ SG_GUAY_BODYPART_LEFT_WING_TIP,
    /* 3 */ SG_GUAY_BODYPART_TAIL,
    /* 4 */ SG_GUAY_BODYPART_MAX
} EnSyatekiCrowBodyPart;

typedef struct EnSyatekiCrow {
    /* 0x000 */ Actor actor;
    /* 0x144 */ Vec3f bodyPartsPos[SG_GUAY_BODYPART_MAX];
    /* 0x174 */ SkelAnime skelAnime;
    /* 0x1B8 */ EnSyatekiCrowActionFunc actionFunc;
    /* 0x1BC */ s16 waitTimer;
    /* 0x1BE */ s16 pitchTarget;
    /* 0x1C0 */ s16 yawTarget;
    /* 0x1C2 */ s16 isActive;
    /* 0x1C4 */ s16 deathTimer;
    /* 0x1C8 */ Vec3s* pathPoints;
    /* 0x1CC */ s16 currentPointIndex;
    /* 0x1CE */ s16 maxPointIndex;
    /* 0x1D0 */ Vec3s jointTable[OBJECT_CROW_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[OBJECT_CROW_LIMB_MAX];
    /* 0x23C */ ColliderJntSph collider;
    /* 0x25C */ ColliderJntSphElement colliderElements[1];
} EnSyatekiCrow; // size = 0x29C

#endif // Z_EN_SYATEKI_CROW_H
