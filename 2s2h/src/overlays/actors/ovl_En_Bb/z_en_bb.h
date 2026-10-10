#ifndef Z_EN_BB_H
#define Z_EN_BB_H

#include "global.h"
#include "objects/object_bb/object_bb.h"

#define EN_BB_ATTACK_RANGE(thisx) (((thisx)->params >> 8) & 0xFF)

struct EnBb;

typedef void (*EnBbActionFunc)(struct EnBb*, PlayState*);

typedef enum BubbleBodyPart {
    /* 0 */ BUBBLE_BODYPART_JAW,
    /* 1 */ BUBBLE_BODYPART_LEFT_WING_WEBBING,
    /* 2 */ BUBBLE_BODYPART_RIGHT_WING_WEBBING,
    /* 3 */ BUBBLE_BODYPART_CRANIUM,
    /* 4 */ BUBBLE_BODYPART_EFFECTS,
    /* 5 */ BUBBLE_BODYPART_MAX
} BubbleBodyPart;

typedef struct EnBb {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[BUBBLE_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[BUBBLE_LIMB_MAX];
    /* 0x248 */ EnBbActionFunc actionFunc;
    /* 0x24C */ s8 bodyPartDrawStatus;
    /* 0x24D */ u8 drawDmgEffType;
    /* 0x24E */ UNK_TYPE1 unk_24E[0x2];
    /* 0x250 */ s16 timer;
    /* 0x252 */ s16 attackWaitTimer; // Cannot start attacking the player if this is non-zero
    /* 0x254 */ s16 targetYRotation;
    /* 0x256 */ s16 bobPhase;
    /* 0x258 */ f32 maxSpeed;
    /* 0x25C */ f32 flyHeightMod;
    /* 0x260 */ f32 attackRange;
    /* 0x264 */ f32 flameScaleX;
    /* 0x268 */ f32 flameScaleY;
    /* 0x26C */ f32 drawDmgEffAlpha;
    /* 0x270 */ f32 drawDmgEffScale;
    /* 0x274 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x278 */ Vec3f bodyPartsPos[BUBBLE_BODYPART_MAX];
    /* 0x2B4 */ Vec3f bodyPartsVelocity[BUBBLE_BODYPART_MAX];
    /* 0x2F0 */ Gfx* limbDList;
    /* 0x2F4 */ ColliderSphere collider;
} EnBb; // size = 0x34C

#endif // Z_EN_BB_H
