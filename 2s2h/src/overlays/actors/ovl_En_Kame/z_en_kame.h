#ifndef Z_EN_KAME_H
#define Z_EN_KAME_H

#include "global.h"
#include "objects/object_tl/object_tl.h"

struct EnKame;

typedef void (*EnKameActionFunc)(struct EnKame*, PlayState*);

typedef enum SnapperBodyPart {
    /*  0 */ SNAPPER_BODYPART_JAW,
    /*  1 */ SNAPPER_BODYPART_FRONT_LEFT_FOOT,
    /*  2 */ SNAPPER_BODYPART_FRONT_RIGHT_FOOT,
    /*  3 */ SNAPPER_BODYPART_BACK_LEFT_FOOT,
    /*  4 */ SNAPPER_BODYPART_BACK_RIGHT_FOOT,
    /*  5 */ SNAPPER_BODYPART_BODY_1,
    /*  6 */ SNAPPER_BODYPART_BODY_2,
    /*  7 */ SNAPPER_BODYPART_BODY_3,
    /*  8 */ SNAPPER_BODYPART_BODY_4,
    /*  9 */ SNAPPER_BODYPART_BODY_5,
    /* 10 */ SNAPPER_BODYPART_MAX
} SnapperBodyPart;

typedef struct EnKame {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime snapperSkelAnime;
    /* 0x188 */ Vec3s snapperJointTable[SNAPPER_LIMB_MAX];
    /* 0x1D6 */ Vec3s snapperMorphTable[SNAPPER_LIMB_MAX];
    /* 0x224 */ SkelAnime spikedSnapperSkelAnime;
    /* 0x268 */ Vec3s spikedSnapperJointTable[SPIKED_SNAPPER_LIMB_MAX];
    /* 0x280 */ Vec3s spikedSnapperMorphTable[SPIKED_SNAPPER_LIMB_MAX];
    /* 0x298 */ EnKameActionFunc actionFunc;
    /* 0x29C */ u8 eyeIndex;
    /* 0x29D */ u8 drawDmgEffType;
    /* 0x29E */ union {
                    s16 timer;
                    s16 targetAngularVelocityY;
                    s16 flipType;
                };
    /* 0x2A0 */ s16 voiceTimer;
    /* 0x2A2 */ s16 stunTimer;
    /* 0x2A4 */ s16 targetRotY;
    /* 0x2A6 */ s16 angularVelocityY;
    /* 0x2A8 */ f32 limbScaleXZ;
    /* 0x2AC */ union {
                    f32 limbScaleY; // used the Snapper's regular form (i.e., when it's walking around on its legs)
                    f32 spikesScale; // used for the Snapper's "spiked" form
                };
    /* 0x2B0 */ f32 drawDmgEffAlpha;
    /* 0x2B4 */ f32 drawDmgEffScale;
    /* 0x2B8 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2BC */ Vec3f targetPos; // this actor relies on the y-position of this variable to be set in a specific way, see EnKame_SetupPrepareToAttack
    /* 0x2C8 */ Vec3f bodyPartsPos[SNAPPER_BODYPART_MAX];
    /* 0x340 */ ColliderCylinder collider;
} EnKame; // size = 0x38C

#endif // Z_EN_KAME_H
