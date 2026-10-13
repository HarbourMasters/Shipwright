#ifndef Z_EN_POH_H
#define Z_EN_POH_H

#include "global.h"
#include "objects/object_po/object_po.h"

struct EnPoh;

typedef void (*EnPohActionFunc)(struct EnPoh*, PlayState*);

typedef enum EnPohBodyPart {
    /*  0 */ POE_BODYPART_LEFT_FOREARM,
    /*  1 */ POE_BODYPART_LEFT_UPPER_ARM,
    /*  2 */ POE_BODYPART_RIGHT_ARM_HAND,
    /*  3 */ POE_BODYPART_RIGHT_FOREARM,
    /*  4 */ POE_BODYPART_BOTTOM_CLOAK,
    /*  5 */ POE_BODYPART_TOP_CLOAK,
    /*  6 */ POE_BODYPART_6,
    /*  7 */ POE_BODYPART_7,
    /*  8 */ POE_BODYPART_8,
    /*  9 */ POE_BODYPART_9,
    /* 10 */ POE_BODYPART_MAX
} EnPohBodyPart;

typedef struct EnPoh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnPohActionFunc actionFunc;
    /* 0x18C */ u8 unk_18C;
    /* 0x18D */ u8 unk_18D;
    /* 0x18E */ s16 unk_18E;
    /* 0x190 */ s16 unk_190;
    /* 0x192 */ s16 unk_192;
    /* 0x194 */ u8 unk_194;
    /* 0x195 */ u8 unk_195;
    /* 0x196 */ u8 unk_196;
    /* 0x197 */ u8 unk_197;
    /* 0x198 */ u8 unk_198;
    /* 0x199 */ u8 unk_199;
    /* 0x19A */ u8 unk_19A;
    /* 0x19B */ u8 unk_19B;
    /* 0x19C */ Vec3s jointTable[POE_LIMB_MAX];
    /* 0x21A */ Vec3s morphTable[POE_LIMB_MAX];
    /* 0x298 */ f32 drawDmgEffAlpha;
    /* 0x29C */ f32 drawDmgEffScale;
    /* 0x2A0 */ Vec3f bodyPartsPos[POE_BODYPART_MAX];
    /* 0x318 */ LightNode* lightNode;
    /* 0x31C */ LightInfo lightInfo;
    /* 0x32C */ ColliderCylinder colliderCylinder;
    /* 0x378 */ ColliderJntSph colliderSph;
    /* 0x398 */ ColliderJntSphElement colliderSphElements[1];
    /* 0x3D8 */ MtxF unk_3D8;
} EnPoh; // size = 0x418

#endif // Z_EN_POH_H
