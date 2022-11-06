#ifndef Z_EN_DEKUBABA_H
#define Z_EN_DEKUBABA_H

#include "ultra64.h"
#include "global.h"

struct EnDekubaba;

typedef void (*EnDekubabaActionFunc)(struct EnDekubaba*, PlayState*);

typedef enum {
    /* 0 */ DEKUBABA_NORMAL,
    /* 1 */ DEKUBABA_BIG
} DekuBabaType;

typedef struct EnDekubaba {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3f bodyPartsPos[4];
    /* 0x017C */ SkelAnime skelAnime;
    /* 0x01C0 */ EnDekubabaActionFunc actionFunc;
    /* 0x01C4 */ char pad[0x2];
    /* 0x01C6 */ s16 timer;
    /* 0x01C8 */ s16 targetSwayAngle;
    /* 0x01CA */ s16 stemSectionAngle[3]; // Used to calculate the position of the stem sections and head with spherical trigonometry
    /* 0x01D0 */ Vec3s jointTable[8];
    /* 0x0200 */ Vec3s morphTable[8];
    /* 0x0230 */ f32 size; // Used everywhere to rescale offsets etc. for Big ones
    /* 0x0234 */ CollisionPoly* boundFloor;
    /* 0x0238 */ ColliderJntSph collider;
    /* 0x0258 */ ColliderJntSphElement colliderElements[7];
} EnDekubaba; // size = 0x0418

#endif
