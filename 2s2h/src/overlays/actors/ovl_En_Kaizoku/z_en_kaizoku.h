#ifndef Z_EN_KAIZOKU_H
#define Z_EN_KAIZOKU_H

#include "global.h"
#include "z64snap.h"
#include "objects/object_kz/object_kz.h"

struct EnKaizoku;

typedef void (*EnKaizokuActionFunc)(struct EnKaizoku*, PlayState*);

#define KAIZOKU_GET_EXIT_INDEX(thisx) ((thisx)->params & 0x3F)
#define KAIZOKU_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 6) & 0x7F)

// used for text and for outfit color
#define KAIZOKU_GET_TYPE(this) ((this)->picto.actor.world.rot.z)

#define KAIZOKU_SWITCH_FLAG_NONE 0x7F

typedef enum EnKaizokuAnimation {
    /*  0 */ KAIZOKU_ANIM_FIGHTING_IDLE,
    /*  1 */ KAIZOKU_ANIM_UNUSED_CONVERSATION, // from OOT: giving player membership card
    /*  2 */ KAIZOKU_ANIM_UNUSED_JUMP, // from OOT: when the enemy would escape by jumping up
    /*  3 */ KAIZOKU_ANIM_SIDESTEP,
    /*  4 */ KAIZOKU_ANIM_WALK,
    /*  5 */ KAIZOKU_ANIM_DAMAGE,
    /*  6 */ KAIZOKU_ANIM_UNUSED_DEFEAT, // from OOT: replaced with KAIZOKU_ANIM_DEFEAT
    /*  7 */ KAIZOKU_ANIM_BLOCK,
    /*  8 */ KAIZOKU_ANIM_FLIP,
    /*  9 */ KAIZOKU_ANIM_SLASH_ATTCK,
    /* 10 */ KAIZOKU_ANIM_SPIN_ATTACK,
    /* 11 */ KAIZOKU_ANIM_LAND,
    /* 12 */ KAIZOKU_ANIM_CHALLENGE,
    /* 13 */ KAIZOKU_ANIM_UNSHEATHE,
    /* 14 */ KAIZOKU_ANIM_LOWER_WEAPONS,
    /* 15 */ KAIZOKU_ANIM_DEMONSTRATIVE_SWORD_SWING,
    /* 16 */ KAIZOKU_ANIM_DEFEAT,
    /* 17 */ KAIZOKU_ANIM_DEFEAT_IDLE,
    /* 18 */ KAIZOKU_ANIM_THROW_FLASH,
    /* 19 */ KAIZOKU_ANIM_MAX
} EnKaizokuAnimation;

typedef enum EnKaizokuBodyPart {
    /*  0 */ KAIZOKU_BODYPART_0,
    /*  1 */ KAIZOKU_BODYPART_1,
    /*  2 */ KAIZOKU_BODYPART_2,
    /*  3 */ KAIZOKU_BODYPART_3,
    /*  4 */ KAIZOKU_BODYPART_4,
    /*  5 */ KAIZOKU_BODYPART_5,
    /*  6 */ KAIZOKU_BODYPART_6,
    /*  7 */ KAIZOKU_BODYPART_7,
    /*  8 */ KAIZOKU_BODYPART_8,
    /*  9 */ KAIZOKU_BODYPART_9,
    /* 10 */ KAIZOKU_BODYPART_10,
    /* 11 */ KAIZOKU_BODYPART_11,
    /* 12 */ KAIZOKU_BODYPART_12,
    /* 13 */ KAIZOKU_BODYPART_13,
    /* 14 */ KAIZOKU_BODYPART_14,
    /* 15 */ KAIZOKU_BODYPART_MAX
} EnKaizokuBodyPart;

typedef enum KaizokuTextOffset {
    /*  0 */ KAIZOKU_TALK_SHOUT_FROM_ABOVE,  // shout before landing
    /*  1 */ KAIZOKU_TALK_INTRO,            // after landing
    /*  2 */ KAIZOKU_TALK_PLAYER_VICTORY,  // after losing to player
    /*  3 */ KAIZOKU_TALK_PLAYER_DEFEAT   // after defeating player
} KaizokuTextOffset;

typedef struct EnKaizoku {
    /* 0x000 */ PictoActor picto;
    /* 0x148 */ SkelAnime skelAnime;
    /* 0x18C */ Vec3s jointTable[KAIZOKU_LIMB_MAX];
    /* 0x21C */ Vec3s morphTable[KAIZOKU_LIMB_MAX];
    /* 0x2AC */ EnKaizokuActionFunc actionFunc;
    /* 0x2B0 */ s16 action;
    /* 0x2B2 */ s16 combatTimer;
    /* 0x2B4 */ s16 lookTimer;
    /* 0x2B6 */ s16 iceTimer;
    /* 0x2B8 */ s16 drawDmgEffTimer;
    /* 0x2BA */ s16 drawDmgEffType;
    /* 0x2BC */ s16 switchFlag;
    /* 0x2C0 */ f32 drawDmgEffScale;
    /* 0x2C4 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2C8 */ s16 textIdOffset; // which of the 4 text states during interaction 
    /* 0x2CA */ s16 textType;
    /* 0x2CC */ s16 eyeIndex;
    /* 0x2CE */ s16 blinkTimer;
    /* 0x2D0 */ s16 spinAttackState;
    /* 0x2D2 */ s16 swordState;
    /* 0x2D4 */ s16 exitIndex;
    /* 0x2D6 */ s16 csId;
    /* 0x2D8 */ u8 animationsDisabled;
    /* 0x2D9 */ u8 defeatBreathingStarted;
    /* 0x2DC */ f32 animEndFrame;
    /* 0x2E0 */ f32 circlingRate;
    /* 0x2E4 */ EnKaizokuAnimation animIndex;
    /* 0x2E8 */ s32 bodyPartIndex;
    /* 0x2EC */ s32 colorType;
    /* 0x2F0 */ f32 boyoBounceVelocity;
    /* 0x2F4 */ s16 boyoBounceAngle;
    /* 0x2F8 */ Vec3f swordScaleRight;
    /* 0x304 */ Vec3f swordScaleLeft;
    /* 0x310 */ Vec3f bodyPartsPos[KAIZOKU_BODYPART_MAX];
    /* 0x3C4 */ Vec3f velocity;
    /* 0x3D0 */ s32 blureIndex;
    /* 0x3D4 */ ColliderCylinder bodyCollider;
    /* 0x420 */ ColliderQuad swordCollider;
    /* 0x4A0 */ ColliderTris blockCollider; // Unused. Inferred from OoT
    /* 0x4C0 */ ColliderTrisElement blockElements[2]; // Unused. Inferred from OoT
    /* 0x578 */ Vec3s headRot;
    /* 0x580 */ Vec3f rightFootPos;
    /* 0x58C */ Vec3f leftFootPos;
    /* 0x598 */ s32 cutsceneTimer;
    /* 0x59C */ s16 cutsceneState; 
    /* 0x59E */ s16 subCamId;
    /* 0x5A0 */ s16 flashTimer;
    /* 0x5A4 */ Vec3f subCamEye;
    /* 0x5B0 */ Vec3f subCamAt;
    /* 0x5BC */ Vec3f subCamUp;
    /* 0x5C8 */ Vec3f subCamEyeTarget;
    /* 0x5D4 */ Vec3f subCamAtTarget;
    /* 0x5E0 */ f32 subCamVelocity;
    /* 0x5E4 */ f32 unk_5E4; // unused
    /* 0x5E8 */ f32 flashScreenAlpha;
    /* 0x5EC */ f32 flashScreenAlphaTarget;
} EnKaizoku; // size = 0x5F0

#endif // Z_EN_KAIZOKU_H
