#ifndef Z_EN_THIEFBIRD_H
#define Z_EN_THIEFBIRD_H

#include "global.h"
#include "objects/object_thiefbird/object_thiefbird.h"

struct EnThiefbird;

typedef void (*EnThiefbirdActionFunc)(struct EnThiefbird*, PlayState*);

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
} EnThiefbirdUnkStruct; // size = 0x24

typedef enum EnThiefbirdBodyPart {
    /*  0 */ EN_THIEFBIRD_BODYPART_0,
    /*  1 */ EN_THIEFBIRD_BODYPART_1,
    /*  2 */ EN_THIEFBIRD_BODYPART_2,
    /*  3 */ EN_THIEFBIRD_BODYPART_3,
    /*  4 */ EN_THIEFBIRD_BODYPART_4,
    /*  5 */ EN_THIEFBIRD_BODYPART_5,
    /*  6 */ EN_THIEFBIRD_BODYPART_6,
    /*  7 */ EN_THIEFBIRD_BODYPART_7,
    /*  8 */ EN_THIEFBIRD_BODYPART_8,
    /*  9 */ EN_THIEFBIRD_BODYPART_9,
    /* 10 */ EN_THIEFBIRD_BODYPART_10,
    /* 11 */ EN_THIEFBIRD_BODYPART_MAX
} EnThiefbirdBodyPart;

typedef struct EnThiefbird {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnThiefbirdActionFunc actionFunc;
    /* 0x18C */ u8 drawDmgEffType;
    /* 0x18E */ s16 unk_18E;
    /* 0x190 */ s16 unk_190;
    /* 0x192 */ s16 unk_192;
    /* 0x194 */ s16 unk_194;
    /* 0x196 */ s16 unk_196[6];
    /* 0x1A2 */ Vec3s jointTable[TAKKURI_LIMB_MAX];
    /* 0x208 */ Vec3s morphTable[TAKKURI_LIMB_MAX];
    /* 0x270 */ ColliderJntSph collider;
    /* 0x290 */ ColliderJntSphElement colliderElements[3];
    /* 0x350 */ Vec3f bodyPartsPos[EN_THIEFBIRD_BODYPART_MAX];
    /* 0x3D4 */ f32 drawDmgEffAlpha;
    /* 0x3D8 */ f32 drawDmgEffScale;
    /* 0x3DC */ f32 drawDmgEffFrozenSteamScale;
    /* 0x3E0 */ f32 unk_3E0;
    /* 0x3E4 */ Gfx* unk_3E4;
    /* 0x3E8 */ Gfx* unk_3E8;
    /* 0x3EC */ EnItem00* unk_3EC;
    /* 0x3F0 */ EnThiefbirdUnkStruct unk_3F0[40];
} EnThiefbird; // size = 0x990

#endif // Z_EN_THIEFBIRD_H
