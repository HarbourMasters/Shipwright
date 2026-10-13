#ifndef Z_EN_JSO_H
#define Z_EN_JSO_H

#include "global.h"
#include "assets/objects/object_jso/object_jso.h"

struct EnJso;

#define EN_JSO_GET_HINT_TYPE(thisx) ((thisx)->params)

#define EN_JSO_AFTERIMAGE_COUNT 20

typedef void (*EnJsoActionFunc)(struct EnJso*, PlayState*);

// For the four `VARIABLE` hint types, the former hint is used if the player hasn't restored the river in
// Ikana Canyon yet, and the latter hint is used if the player has. If the player has the Elegy of Emptiness,
// then all hints will be replaced with a hint telling the player to use the song to climb Stone Tower.
typedef enum EnJsoHintType {
    /* 0 */ EN_JSO_HINT_FREEZE_OCTOROKS,
    /* 1 */ EN_JSO_HINT_FLATS_LOCATION,
    /* 2 */ EN_JSO_HINT_VARIABLE_1, // Hints that Pamela's family is in the house or that the Song of Healing can heal Pamela's father
    /* 3 */ EN_JSO_HINT_VARIABLE_2, // Hints that Pamela leaves the house to check something or that the well is connected to Ikana Castle
    /* 4 */ EN_JSO_HINT_VARIABLE_3, // Hints that they can restore the river by going to Sharp's cave or that the Gidbos in the well want items
    /* 5 */ EN_JSO_HINT_VARIABLE_4, // Hints that they need to Song of Storms to stop Sharp or that the items the Gibdos want can be found in the well
    /* 6 */ EN_JSO_HINT_DANCING_REDEADS,
    /* 7 */ EN_JSO_HINT_DESTRUCTIBLE_CEILING_IN_CASTLE,
    /* 8 */ EN_JSO_HINT_SECOND_ENTRANCE_TO_CASTLE,
    /* 9 */ EN_JSO_HINT_KING_WEAK_TO_MIRROR_SHIELD
} EnJsoHintType;

typedef enum EnJsoBodyPart {
    /*  0 */ EN_JSO_BODYPART_LEFT_SWORD,
    /*  1 */ EN_JSO_BODYPART_RIGHT_SWORD,
    /*  2 */ EN_JSO_BODYPART_ROBE_TOP,
    /*  3 */ EN_JSO_BODYPART_ROBE_BACK,
    /*  4 */ EN_JSO_BODYPART_ROBE_LEFT,
    /*  5 */ EN_JSO_BODYPART_ROBE_RIGHT,
    /*  6 */ EN_JSO_BODYPART_ROBE_FRONT,
    /*  7 */ EN_JSO_BODYPART_HEAD,
    /*  8 */ EN_JSO_BODYPART_RIGHT_THIGH,
    /*  9 */ EN_JSO_BODYPART_RIGHT_FOOT,
    /* 10 */ EN_JSO_BODYPART_LEFT_THIGH,
    /* 11 */ EN_JSO_BODYPART_LEFT_FOOT,
    /* 12 */ EN_JSO_BODYPART_MAX
} EnJsoBodyPart;

typedef struct EnJso {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[GARO_LIMB_MAX];
    /* 0x200 */ Vec3s morphTable[GARO_LIMB_MAX];
    /* 0x278 */ EnJsoActionFunc actionFunc;
    /* 0x27C */ s16 action;
    /* 0x27E */ s16 circlingAngle;
    /* 0x280 */ s16 circlingAngularVelocity;
    /* 0x282 */ s16 attackMovementTimer;
    /* 0x284 */ UNK_TYPE1 unk_284[0x2];
    /* 0x286 */ s16 attackTimer; // while bouncing, the Garo cannot start an attack until this reaches 0
    /* 0x288 */ s16 timer;
    /* 0x28A */ s16 isGuarding; // Set to false when the guard animation ends, never checked or set otherwise. Name is inferred based on how it's being set.
    /* 0x28C */ s16 isPlayerLockedOn;
    /* 0x28E */ s16 isAttacking;
    /* 0x290 */ Vec3s robeRightRot;
    /* 0x296 */ Vec3s targetRobeRightRot;
    /* 0x29C */ Vec3s rightArmRot;
    /* 0x2A2 */ Vec3s targetRightArmRot;
    /* 0x2A8 */ s16 drawDmgEffAlpha;
    /* 0x2AA */ s16 drawDmgEffType;
    /* 0x2AC */ f32 drawDmgEffScale;
    /* 0x2B0 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2B4 */ Vec3f bodyPartsPos[EN_JSO_BODYPART_MAX];
    /* 0x344 */ s16 bodyPartIndex;
    /* 0x346 */ s16 disableAnimations; // Checked but never set, so it's implicitly always false. Name is inferred based on how it's being checked.
    /* 0x348 */ s16 textIndex;
    /* 0x34A */ s16 hintType;
    /* 0x34C */ u8 disableBlure;
    /* 0x34D */ u8 swordState;
    /* 0x34E */ u8 slashHitSomething;
    /* 0x350 */ f32 animEndFrame;
    /* 0x354 */ f32 scale;
    /* 0x358 */ UNK_TYPE1 unk_358[0x4];
    /* 0x35C */ ColliderCylinder bodyCollider;
    /* 0x3A8 */ ColliderQuad rightSwordCollider;
    /* 0x428 */ ColliderQuad leftSwordCollider;
    /* 0x4A8 */ s32 rightSwordBlureIndex;
    /* 0x4AC */ s32 leftSwordBlureIndex;
    /* 0x4B0 */ s16 afterimageIndex;
    /* 0x4B4 */ s32 afterimageCount;
    /* 0x4B8 */ s16 csId;
    /* 0x4BC */ u32 cutsceneTimer;
    /* 0x4C0 */ s16 cutsceneState;
    /* 0x4C2 */ s16 subCamId;
    /* 0x4C4 */ UNK_TYPE1 unk_4C4[0x4];
    /* 0x4C8 */ Vec3f subCamEye;
    /* 0x4D4 */ Vec3f subCamAt;
    /* 0x4E0 */ Vec3f subCamUp;  
    /* 0x4EC */ Vec3f subCamEyeNext;
    /* 0x4F8 */ Vec3f subCamAtNext;
    /* 0x504 */ UNK_TYPE1 unk_504[0x10];
    /* 0x514 */ s16 introCsType;
    /* 0x518 */ Vec3f afterimagePos[EN_JSO_AFTERIMAGE_COUNT];
    /* 0x608 */ Vec3s afterimageRot[EN_JSO_AFTERIMAGE_COUNT];
    /* 0x680 */ Vec3s afterimageJointTable[EN_JSO_AFTERIMAGE_COUNT][GARO_LIMB_MAX];
    /* 0xFE0 */ Vec3f knockbackVelocity; // Adds a little push backwards when the Garo is bounced off the player's shield, damaged, or stunned
} EnJso; // size = 0xFEC

#endif // Z_EN_JSO_H
