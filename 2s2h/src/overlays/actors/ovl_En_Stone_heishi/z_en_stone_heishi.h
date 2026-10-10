#ifndef Z_EN_STONE_HEISHI_H
#define Z_EN_STONE_HEISHI_H

#include "global.h"
#include "assets/objects/object_sdn/object_sdn.h"

struct EnStoneheishi;

typedef void (*EnStoneheishiActionFunc)(struct EnStoneheishi*, PlayState*);

typedef struct EnStoneheishi {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[SOLDIER_LIMB_MAX];
    /* 0x1EE */ Vec3s morphTable[SOLDIER_LIMB_MAX];
    /* 0x254 */ EnStoneheishiActionFunc actionFunc;
    /* 0x258 */ Vec3s headRot;
    /* 0x25E */ Vec3s targetHeadRot;
    /* 0x264 */ s32 animIndex;
    /* 0x268 */ s16 timer;
    /* 0x26A */ s16 drinkBottleState;
    /* 0x26C */ f32 animEndFrame;
    /* 0x270 */ s16 action;
    /* 0x272 */ s16 textIdIndex;
    /* 0x274 */ s16 textIdSet;
    /* 0x276 */ u8 bottleDisplay;
    /* 0x277 */ u8 playerGivesBluePotion;
    /* 0x278 */ ColliderCylinder collider;
} EnStoneheishi; // size = 0x2C4

#endif // Z_EN_STONE_HEISHI_H
