#ifndef Z_EN_DEKUNUTS_H
#define Z_EN_DEKUNUTS_H

#include "global.h"
#include "objects/object_dekunuts/object_dekunuts.h"

struct EnDekunuts;

typedef void (*EnDekunutsActionFunc)(struct EnDekunuts*, PlayState*);

#define ENDEKUNUTS_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef enum {
    /* 0x00 */ ENDEKUNUTS_GET_FF00_0,
    /* 0x01 */ ENDEKUNUTS_GET_FF00_1,
    /* 0x02 */ ENDEKUNUTS_GET_FF00_2,
    /* 0xFF */ ENDEKUNUTS_GET_FF00_FF = 0xFF
} EnDekunutsParam;

typedef enum DekunutsBodyPart {
    /* 0 */ ENDEKUNUTS_BODYPART_0,
    /* 1 */ ENDEKUNUTS_BODYPART_1,
    /* 2 */ ENDEKUNUTS_BODYPART_2,
    /* 3 */ ENDEKUNUTS_BODYPART_3,
    /* 4 */ ENDEKUNUTS_BODYPART_4,
    /* 5 */ ENDEKUNUTS_BODYPART_5,
    /* 6 */ ENDEKUNUTS_BODYPART_6,
    /* 7 */ ENDEKUNUTS_BODYPART_7,
    /* 8 */ ENDEKUNUTS_BODYPART_MAX
} DekunutsBodyPart;

typedef struct EnDekunuts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnDekunutsActionFunc actionFunc;
    /* 0x18C */ u8 unk_18C;
    /* 0x18D */ u8 unk_18D;
    /* 0x18E */ u8 drawDmgEffType;
    /* 0x190 */ s16 unk_190;
    /* 0x192 */ s16 unk_192;
    /* 0x194 */ s16 unk_194;
    /* 0x196 */ Vec3s jointTable[DEKU_SCRUB_LIMB_MAX];
    /* 0x1D2 */ Vec3s morphTable[DEKU_SCRUB_LIMB_MAX];
    /* 0x210 */ f32 drawDmgEffAlpha;
    /* 0x214 */ f32 drawDmgEffScale;
    /* 0x218 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x21C */ Vec3f bodyPartsPos[ENDEKUNUTS_BODYPART_MAX];
    /* 0x27C */ ColliderCylinder collider;
} EnDekunuts; // size = 0x2C8

#endif // Z_EN_DEKUNUTS_H
