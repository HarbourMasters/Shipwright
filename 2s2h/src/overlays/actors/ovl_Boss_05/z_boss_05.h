#ifndef Z_BOSS_05_H
#define Z_BOSS_05_H

#include "global.h"
#include "assets/objects/object_boss05/object_boss05.h"

struct Boss05;

typedef void (*Boss05ActionFunc)(struct Boss05*, PlayState*);

typedef enum BioDekuBabaType {
    /*  0 */ BIO_BABA_TYPE_LILY_PAD_WITH_HEAD,
    /*  1 */ BIO_BABA_TYPE_NO_LEAF_LILY_PAD_WITH_HEAD,
    /*  2 */ BIO_BABA_TYPE_LILY_PAD,
    /*  3 */ BIO_BABA_TYPE_FALLING_HEAD,
    /*  4 */ BIO_BABA_TYPE_WALKING_HEAD,
    /* 10 */ BIO_BABA_TYPE_FRAGMENT_BASE = 10,
    /* 10 */ BIO_BABA_TYPE_FRAGMENT_LOWER_JAW = BIO_BABA_TYPE_FRAGMENT_BASE,
    /* 11 */ BIO_BABA_TYPE_FRAGMENT_UPPER_JAW,
    /* 12 */ BIO_BABA_TYPE_FRAGMENT_BODY,
    /* 13 */ BIO_BABA_TYPE_FRAGMENT_BACK_UPPER_LEG,
    /* 14 */ BIO_BABA_TYPE_FRAGMENT_BACK_LOWER_LEG,
    /* 15 */ BIO_BABA_TYPE_FRAGMENT_LEFT_UPPER_LEG,
    /* 16 */ BIO_BABA_TYPE_FRAGMENT_LEFT_LOWER_LEG,
    /* 17 */ BIO_BABA_TYPE_FRAGMENT_LEFT_LOWER_EYESTALK,
    /* 18 */ BIO_BABA_TYPE_FRAGMENT_LEFT_UPPER_EYESTALK,
    /* 19 */ BIO_BABA_TYPE_FRAGMENT_RIGHT_LOWER_EYESTALK,
    /* 20 */ BIO_BABA_TYPE_FRAGMENT_RIGHT_UPPER_EYESTALK,
    /* 21 */ BIO_BABA_TYPE_FRAGMENT_RIGHT_UPPER_LEG,
    /* 22 */ BIO_BABA_TYPE_FRAGMENT_RIGHT_LOWER_LEG,
    /* 23 */ BIO_BABA_TYPE_FRAGMENT_LEAVES,
    /* 24 */ BIO_BABA_TYPE_MAX
} BioDekuBabaType;

typedef enum BioDekuBabaBodyPart {
    /* 0 */ BIO_BABA_BODYPART_HEAD,
    /* 1 */ BIO_BABA_BODYPART_MAX
} BioDekuBabaBodyPart;

typedef enum BioDekuBabaHeadCollider {
    /* 0 */ BIO_BABA_HEAD_COLLIDER_HEAD,
    /* 1 */ BIO_BABA_HEAD_COLLIDER_MAX
} BioDekuBabaHeadCollider;

typedef enum BioDekuBabaLilyPadCollider {
    /* 0 */ BIO_BABA_LILY_PAD_COLLIDER_UPPER_STEM,
    /* 1 */ BIO_BABA_LILY_PAD_COLLIDER_MIDDLE_STEM,
    /* 2 */ BIO_BABA_LILY_PAD_COLLIDER_MAX
} BioDekuBabaLilyPadCollider;

typedef enum BioDekuBabaLilyPadWithHeadLimbRotIndex {
    /* -1 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_NONE = -1,
    /*  0 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_UPPER_STEM,
    /*  1 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MIDDLE_STEM,
    /*  2 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LOWER_STEM,
    /*  3 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_UPPER_ARM,
    /*  4 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_LEFT_LOWER_ARM,
    /*  5 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_UPPER_ARM,
    /*  6 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_RIGHT_LOWER_ARM,
    /*  7 */ BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX
} BioDekuBabaLilyPadWithHeadLimbRotIndex;

#define BIO_BABA_GET_TYPE(thisx) ((thisx)->params)
#define BIO_BABA_GET_FRAGMENT_INDEX(thisx) ((thisx)->params - BIO_BABA_TYPE_FRAGMENT_BASE)
#define BIO_BABA_FORCE_DETACH_TIMER(thisx) ((thisx)->world.rot.z)

typedef struct Boss05 {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ union {
                    u8 lilyPadWithHeadAttackState;
                    u8 fragmentState;
                };
    /* 0x15E */ s16 flipAttackFrameCounter;
    /* 0x160 */ s16 frameCounter;
    /* 0x162 */ s16 timers[3];
    /* 0x168 */ s16 forceDetachTimer;
    /* 0x16A */ s16 damagedTimer;
    /* 0x16C */ s16 damagedFlashTimer;
    /* 0x170 */ Boss05ActionFunc actionFunc;
    /* 0x174 */ u8 lilyPadWithHeadMovementState;
    /* 0x176 */ s16 lilyPadRotY;
    /* 0x178 */ s16 lilyPadRotX;
    /* 0x17A */ s16 drawDmgEffTimer;
    /* 0x17C */ f32 drawDmgEffScale;
    /* 0x180 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x184 */ f32 drawDmgEffAlpha;
    /* 0x188 */ u8 drawDmgEffState;
    /* 0x189 */ u8 drawDmgEffType;
    /* 0x18C */ Vec3f bodyPartsPos[BIO_BABA_BODYPART_MAX];
    /* 0x198 */ f32 fallingHeadLilyPadLimbScale;
    /* 0x19C */ s16 lilyPadWithHeadStemRotX;
    /* 0x19E */ Vec3s lilyPadWithHeadLimbRot[BIO_BABA_LILY_PAD_WITH_HEAD_LIMB_ROT_INDEX_MAX];
    /* 0x1C8 */ ColliderJntSph lilyPadCollider;
    /* 0x1E8 */ ColliderJntSphElement lilyPadColliderElements[BIO_BABA_LILY_PAD_COLLIDER_MAX];
    /* 0x268 */ SkelAnime lilyPadSkelAnime;
    /* 0x2AC */ Vec3s lilyPadJointTable[BIO_DEKU_BABA_LILY_PAD_LIMB_MAX];
    /* 0x2E8 */ Vec3s lilyPadMorphTable[BIO_DEKU_BABA_LILY_PAD_LIMB_MAX];
    /* 0x324 */ union {
                    Vec3f headPos;
                    Vec3f fragmentPos;
                };
    /* 0x330 */ Vec3s headRot;
    /* 0x338 */ f32 knockbackMagnitude;
    /* 0x33C */ Vec3f knockbackVelocity;
    /* 0x348 */ s16 knockbackAngle;
    /* 0x34C */ Vec3f walkTargetPos;
    /* 0x35C */ f32 walkAngularVelocityY;
    /* 0x35C */ f32 lowerJawScaleXZ;
    /* 0x360 */ f32 headBodyScale;
    /* 0x364 */ f32 headLimbScale;
    /* 0x368 */ ColliderJntSph headCollider;
    /* 0x388 */ ColliderJntSphElement headColliderElements[BIO_BABA_HEAD_COLLIDER_MAX];
    /* 0x3C8 */ SkelAnime headSkelAnime;
    /* 0x40C */ Vec3s headJointTable[BIO_DEKU_BABA_HEAD_LIMB_MAX];
    /* 0x484 */ Vec3s headMorphTable[BIO_DEKU_BABA_HEAD_LIMB_MAX];
    /* 0x4FC */ f32 animEndFrame;
    /* 0x500 */ Vec3s fragmentAngularVelocity;
} Boss05; // size = 0x508

#endif // Z_BOSS_05_H
