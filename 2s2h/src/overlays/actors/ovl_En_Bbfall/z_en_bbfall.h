#ifndef Z_EN_BBFALL_H
#define Z_EN_BBFALL_H

#include "global.h"
#include "objects/object_bb/object_bb.h"
#include "overlays/actors/ovl_En_Bb/z_en_bb.h"

struct EnBbfall;

typedef void (*EnBbfallActionFunc)(struct EnBbfall*, PlayState*);

typedef struct EnBbfall {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BUBBLE_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[BUBBLE_LIMB_MAX];
    /* 0x248 */ EnBbfallActionFunc actionFunc;
    /* 0x24C */ u8 flameOpacity;
    /* 0x24D */ u8 isBgCheckCollisionEnabled;
    /* 0x24E */ s8 bodyPartDrawStatus;
    /* 0x24F */ u8 drawDmgEffType;
    /* 0x250 */ s16 timer;
    /* 0x254 */ f32 flameScaleY;
    /* 0x258 */ f32 flameScaleX;
    /* 0x25C */ f32 drawDmgEffAlpha;
    /* 0x260 */ f32 drawDmgEffScale;
    /* 0x264 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x268 */ Vec3f flamePos[6];
    /* 0x2B0 */ Vec3f bodyPartsPos[BUBBLE_BODYPART_MAX];
    /* 0x2EC */ Vec3f bodyPartsVelocity[BUBBLE_BODYPART_MAX];
    /* 0x328 */ Gfx* limbDList;
    /* 0x32C */ UNK_TYPE1 unk_32C[0x58];
    /* 0x384 */ ColliderJntSph collider;
    /* 0x3A4 */ ColliderJntSphElement colliderElements[3];
} EnBbfall; // size = 0x464

#endif // Z_EN_BBFALL_H
