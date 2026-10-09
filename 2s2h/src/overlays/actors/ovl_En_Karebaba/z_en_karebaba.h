#ifndef Z_EN_KAREBABA_H
#define Z_EN_KAREBABA_H

#include "global.h"
#include "objects/object_dekubaba/object_dekubaba.h"

struct EnKarebaba;

typedef void (*EnKarebabaActionFunc)(struct EnKarebaba*, PlayState*);

typedef enum {
    /* 0 */ ENKAREBABA_0, // Unused, always shrinks and drops a Deku Nut
    /* 1 */ ENKAREBABA_1, // Drops sticks (Only type OoT has) 
    /* 2 */ KAREBABA_MINI
} KarebabaType;

typedef enum KarebabaBodyPart {
    /* 0 */ KAREBABA_BODYPART_0,
    /* 1 */ KAREBABA_BODYPART_1,
    /* 2 */ KAREBABA_BODYPART_2,
    /* 3 */ KAREBABA_BODYPART_3,
    /* 4 */ KAREBABA_BODYPART_MAX
} KarebabaBodyPart;

typedef struct EnKarebaba {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnKarebabaActionFunc actionFunc;
    /* 0x18C */ Vec3s jointTable[DEKUBABA_LIMB_MAX];
    /* 0x1BC */ Vec3s morphTable[DEKUBABA_LIMB_MAX];
    /* 0x1EC */ u8 drawDmgEffType;
    /* 0x1EE */ s16 timer;
    /* 0x1F0 */ f32 drawDmgEffAlpha;
    /* 0x1F4 */ f32 drawDmgEffScale;
    /* 0x1F8 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x1FC */ Vec3f bodyPartsPos[KAREBABA_BODYPART_MAX];
    /* 0x22C */ CollisionPoly* boundFloor;
    /* 0x230 */ ColliderCylinder attackCollider;
    /* 0x27C */ ColliderCylinder hurtCollider;
} EnKarebaba; // size = 0x2C8

#endif // Z_EN_KAREBABA_H
