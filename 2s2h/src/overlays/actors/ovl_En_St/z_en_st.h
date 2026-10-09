#ifndef Z_EN_ST_H
#define Z_EN_ST_H

#include "global.h"
#include "objects/object_st/object_st.h"

struct EnSt;

typedef void (*EnStActionFunc)(struct EnSt*, PlayState*);

#define ENST_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0x3F) & 0xFF)
#define ENST_GET_1C0(thisx) ((((thisx)->params & 0x1C0) >> 6) & 0xFF)

#define ENST_1C0_1 1

typedef enum EnStBodyPart {
    /*  0 */ ENST_BODYPART_0,
    /*  1 */ ENST_BODYPART_1,
    /*  2 */ ENST_BODYPART_2,
    /*  3 */ ENST_BODYPART_3,
    /*  4 */ ENST_BODYPART_4,
    /*  5 */ ENST_BODYPART_5,
    /*  6 */ ENST_BODYPART_6,
    /*  7 */ ENST_BODYPART_7,
    /*  8 */ ENST_BODYPART_8,
    /*  9 */ ENST_BODYPART_9,
    /* 10 */ ENST_BODYPART_10,
    /* 11 */ ENST_BODYPART_11,
    /* 12 */ ENST_BODYPART_MAX
} EnStBodyPart;

typedef struct EnSt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnStActionFunc actionFunc;
    /* 0x18C */ u16 unk_18C;
    /* 0x18E */ u8 drawDmgEffType;
    /* 0x190 */ ColliderCylinder collider1;
    /* 0x1DC */ ColliderCylinder collider2;
    /* 0x228 */ ColliderCylinder collider3;
    /* 0x274 */ ColliderCylinder collider4;
    /* 0x2C0 */ s8 objectSlot;
    /* 0x2C4 */ f32 unk_2C4;
    /* 0x2C8 */ f32 unk_2C8;
    /* 0x2CC */ f32 unk_2CC;
    /* 0x2D0 */ f32 unk_2D0;
    /* 0x2D4 */ f32 unk_2D4;
    /* 0x2D8 */ f32 unk_2D8;
    /* 0x2DC */ f32 drawDmgEffFrozenSteamScales[12];
    /* 0x30C */ s16 unk_30C;
    /* 0x30E */ s16 unk_30E;
    /* 0x310 */ s16 unk_310;
    /* 0x312 */ s16 unk_312;
    /* 0x314 */ s16 unk_314;
    /* 0x316 */ s16 unk_316;
    /* 0x318 */ s16 unk_318;
    /* 0x31A */ s16 unk_31A;
    /* 0x31C */ s16 unk_31C[ENST_BODYPART_MAX];
    /* 0x334 */ s16 unk_334[ENST_BODYPART_MAX];
    /* 0x34C */ f32 unk_34C;
    /* 0x350 */ UNK_TYPE1 unk_350[0x8];
    /* 0x358 */ Vec3f bodyPartsPos[ENST_BODYPART_MAX];
    /* 0x3E8 */ Vec3s jointTable[OBJECT_ST_LIMB_MAX];
    /* 0x49C */ Vec3s morphTable[OBJECT_ST_LIMB_MAX];
} EnSt; // size = 0x550

#endif // Z_EN_ST_H
