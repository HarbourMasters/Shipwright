#ifndef Z_EN_FLOORMAS_H
#define Z_EN_FLOORMAS_H

#include "global.h"
#include "objects/object_wallmaster/object_wallmaster.h"

struct EnFloormas;

typedef void (*EnFloormasActionFunc)(struct EnFloormas*, PlayState*);

#define ENFLOORMAS_GET_7FFF(thisx) ((thisx)->params & 0x7FFF)
#define ENFLOORMAS_GET_8000(thisx) ((thisx)->params & 0x8000)

typedef enum {
    /* 0x00 */ ENFLOORMAS_GET_7FFF_0,
    /* 0x10 */ ENFLOORMAS_GET_7FFF_10 = 0x10,
    /* 0x20 */ ENFLOORMAS_GET_7FFF_20 = 0x20,
    /* 0x40 */ ENFLOORMAS_GET_7FFF_40 = 0x40
} EnFloormasParam;

typedef enum EnFloormasBodyPart {
    /*  0 */ ENFLOORMAS_BODYPART_0,
    /*  1 */ ENFLOORMAS_BODYPART_1,
    /*  2 */ ENFLOORMAS_BODYPART_2,
    /*  3 */ ENFLOORMAS_BODYPART_3,
    /*  4 */ ENFLOORMAS_BODYPART_4,
    /*  5 */ ENFLOORMAS_BODYPART_5,
    /*  6 */ ENFLOORMAS_BODYPART_6,
    /*  7 */ ENFLOORMAS_BODYPART_7,
    /*  8 */ ENFLOORMAS_BODYPART_8,
    /*  9 */ ENFLOORMAS_BODYPART_9,
    /* 10 */ ENFLOORMAS_BODYPART_10,
    /* 11 */ ENFLOORMAS_BODYPART_MAX
} EnFloormasBodyPart;

typedef struct EnFloormas {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnFloormasActionFunc actionFunc;
    /* 0x18C */ u8 drawDmgEffType;
    /* 0x18E */ s16 unk_18E;
    /* 0x190 */ s16 unk_190;
    /* 0x192 */ s16 unk_192;
    /* 0x194 */ s16 unk_194;
    /* 0x196 */ Vec3s jointTable[WALLMASTER_LIMB_MAX];
    /* 0x22C */ Vec3s morphTable[WALLMASTER_LIMB_MAX];
    /* 0x2C4 */ f32 drawDmgEffAlpha;
    /* 0x2C8 */ f32 drawDmgEffScale;
    /* 0x2CC */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2D0 */ Vec3f bodyPartsPos[ENFLOORMAS_BODYPART_MAX];
    /* 0x354 */ ColliderCylinder collider;
} EnFloormas; // size = 0x3A0

#endif // Z_EN_FLOORMAS_H
