#ifndef Z_EN_DODONGO_H
#define Z_EN_DODONGO_H

#include "global.h"
#include "objects/object_dodongo/object_dodongo.h"

struct EnDodongo;

typedef void (*EnDodongoActionFunc)(struct EnDodongo*, PlayState*);

typedef enum DodongoBodyPart {
    /* 0 */ DODONGO_BODYPART_0,
    /* 1 */ DODONGO_BODYPART_1,
    /* 2 */ DODONGO_BODYPART_2,
    /* 3 */ DODONGO_BODYPART_3,
    /* 4 */ DODONGO_BODYPART_4,
    /* 5 */ DODONGO_BODYPART_5,
    /* 6 */ DODONGO_BODYPART_6,
    /* 7 */ DODONGO_BODYPART_7,
    /* 8 */ DODONGO_BODYPART_8,
    /* 9 */ DODONGO_BODYPART_MAX
} DodongoBodyPart;

typedef struct EnDodongo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_DODONGO_LIMB_MAX];
    /* 0x242 */ Vec3s morphTable[OBJECT_DODONGO_LIMB_MAX];
    /* 0x2FC */ EnDodongoActionFunc actionFunc;
    /* 0x300 */ u8 drawDmgEffType;
    /* 0x302 */ s16 timer;
    /* 0x304 */ s16 unk_304;
    /* 0x306 */ s16 unk_306;
    /* 0x308 */ Vec3f unk_308;
    /* 0x314 */ Vec3f unk_314;
    /* 0x320 */ Vec3f unk_320;
    /* 0x32C */ Color_RGBA8 unk_32C;
    /* 0x330 */ Color_RGBA8 unk_330;
    /* 0x334 */ f32 unk_334;
    /* 0x338 */ s32 unk_338;
    /* 0x33C */ f32 drawDmgEffAlpha;
    /* 0x340 */ f32 drawDmgEffScale;
    /* 0x344 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x348 */ Vec3f bodyPartsPos[DODONGO_BODYPART_MAX];
    /* 0x3B4 */ ColliderJntSph collider1;
    /* 0x3D4 */ ColliderJntSphElement collider1Elements[10];
    /* 0x654 */ ColliderJntSph collider2;
    /* 0x674 */ ColliderJntSphElement collider2Elements[3];
    /* 0x734 */ ColliderJntSph collider3;
    /* 0x754 */ ColliderJntSphElement collider3Elements[3];
} EnDodongo; // size = 0x814

#endif // Z_EN_DODONGO_H
