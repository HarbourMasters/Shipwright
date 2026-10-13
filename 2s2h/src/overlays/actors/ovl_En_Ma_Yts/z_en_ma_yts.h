#ifndef Z_EN_MA_YTS_H
#define Z_EN_MA_YTS_H

#include "global.h"
#include "objects/object_ma1/object_ma1.h"

struct EnMaYts;

typedef void (*EnMaYtsActionFunc)(struct EnMaYts*, PlayState*);

typedef struct EnMaYts {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnMaYtsActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ NpcInteractInfo interactInfo;
    /* 0x200 */ s32 unk_200; // Set, but not used
    /* 0x204 */ Vec3s jointTable[ROMANI_LIMB_MAX];
    /* 0x28E */ UNK_TYPE1 unk_28E[0x6];
    /* 0x294 */ Vec3s morphTable[ROMANI_LIMB_MAX];
    /* 0x31E */ UNK_TYPE1 unk_31E[0x8];
    /* 0x326 */ s16 blinkTimer;
    /* 0x328 */ s16 overrideEyeTexIndex; // If non-zero, then this index will be used instead of eyeTexIndex
    /* 0x32A */ s16 eyeTexIndex;
    /* 0x32C */ s16 unk_32C; // flag?
    /* 0x32E */ s16 mouthTexIndex;
    /* 0x330 */ s16 type;
    /* 0x332 */ UNK_TYPE1 unk_332[0x2];
    /* 0x334 */ s16 endCreditsFlag;
    /* 0x336 */ s16 hasBow;
    /* 0x338 */ u16 textId;
} EnMaYts; // size = 0x33C

typedef enum {
    /* 0 */ MA_YTS_TYPE_BARN,
    /* 1 */ MA_YTS_TYPE_SITTING,
    /* 2 */ MA_YTS_TYPE_SLEEPING,
    /* 3 */ MA_YTS_TYPE_ENDCREDITS
} EnMaYtsType;

#define EN_MA_YTS_GET_TYPE(thisx) (((thisx)->params & 0xF000) >> 12)
#define EN_MA_YTS_PARAM(enMaYtsType) ((enMaYtsType) << 12)

/**
 * MA_YTS_TYPE_BARN: In the barn, accompanying Cremia. Cremia's actor will handle her dialogue if spoken to.
 * MA_YTS_TYPE_SITTING: Having dinner or looking after the ranch.
 * MA_YTS_TYPE_SLEEPING: Sleeping. Can be seen in the second night in her house if the player failed the alien invasion.
 * MA_YTS_TYPE_ENDCREDITS: Used in the end credits sequence.
 */

#endif // Z_EN_MA_YTS_H
