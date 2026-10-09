#ifndef Z_EN_JSO2_H
#define Z_EN_JSO2_H

#include "global.h"
#include "assets/objects/object_jso/object_jso.h"

struct EnJso2;

#define EN_JSO2_GET_TYPE(thisx) ((thisx)->params)

#define EN_JSO2_AFTERIMAGE_COUNT 20
#define EN_JSO2_FLAME_COUNT 6

typedef void (*EnJso2ActionFunc)(struct EnJso2*, PlayState*);

typedef enum EnJso2Type {
    // This type is exclusively used by the Garo Master in Stone Tower Temple who guards the Light Arrow.
    // It has an intro and death cutscene, and the intro cutscene uses many hardcoded coordinates which
    // assume the Garo Master is in the Light Arrow room.
    /* 0 */ EN_JSO2_TYPE_LIGHT_ARROW_ROOM,
    
    // This type is used everywhere outside of Stone Tower Temple, and it lacks an intro or death cutscene.
    /* 1 */ EN_JSO2_TYPE_NORMAL
} EnJso2Type;

typedef enum EnJso2BodyPart {
    /*  0 */ EN_JSO2_BODYPART_LEFT_SWORD,
    /*  1 */ EN_JSO2_BODYPART_RIGHT_SWORD,
    /*  2 */ EN_JSO2_BODYPART_ROBE_TOP,
    /*  3 */ EN_JSO2_BODYPART_ROBE_BACK,
    /*  4 */ EN_JSO2_BODYPART_ROBE_LEFT,
    /*  5 */ EN_JSO2_BODYPART_ROBE_RIGHT,
    /*  6 */ EN_JSO2_BODYPART_ROBE_FRONT,
    /*  7 */ EN_JSO2_BODYPART_HEAD,
    /*  8 */ EN_JSO2_BODYPART_RIGHT_THIGH,
    /*  9 */ EN_JSO2_BODYPART_RIGHT_FOOT,
    /* 10 */ EN_JSO2_BODYPART_LEFT_THIGH,
    /* 11 */ EN_JSO2_BODYPART_LEFT_FOOT,
    /* 12 */ EN_JSO2_BODYPART_MAX
} EnJso2BodyPart;

typedef struct EnJso2 {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ Vec3s jointTable[GARO_MASTER_LIMB_MAX];
    /* 0x0200 */ Vec3s morphTable[GARO_MASTER_LIMB_MAX];
    /* 0x0278 */ EnJso2ActionFunc actionFunc;
    /* 0x027C */ PlayerOverrideInputEntry overrideInputEntry;
    /* 0x0284 */ s16 action;
    /* 0x0286 */ s16 circlingAngle;
    /* 0x0288 */ s16 circlingAngularVelocity;
    /* 0x028A */ s16 attackMovementTimer;
    /* 0x028C */ UNK_TYPE1 unk_28C[2];
    /* 0x028E */ s16 attackTimer;
    /* 0x0290 */ s16 timer;
    /* 0x0294 */ f32 subCamMaxVelocityFrac;
    /* 0x0298 */ f32 subCamVelocity;
    /* 0x029C */ s32 type;
    /* 0x02A0 */ s16 drawDmgEffAlpha;
    /* 0x02A2 */ s16 drawDmgEffType;
    /* 0x02A4 */ f32 drawDmgEffScale;
    /* 0x02A8 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x02AC */ UNK_TYPE1 unk_2AC[4];
    /* 0x02B0 */ s16 bodyPartsCount;
    /* 0x02B4 */ s32 isFadingAway;
    /* 0x02B8 */ Vec3f baseSubCamEye;
    /* 0x02C4 */ Vec3f bombPos;
    /* 0x02D0 */ Actor* bomb;
    /* 0x02D4 */ Vec3f bodyPartsPos[EN_JSO2_BODYPART_MAX];
    /* 0x0364 */ s16 bodyPartIndex;
    /* 0x0366 */ s16 alpha;
    /* 0x0368 */ u8 disableBlure;
    /* 0x036C */ s32 swordState;
    /* 0x0370 */ u8 isTeleporting; // set, but never checked
    /* 0x0371 */ u8 slashHitSomething;
    /* 0x0374 */ f32 animEndFrame;
    /* 0x0378 */ f32 scale;
    /* 0x037C */ UNK_TYPE1 unk_37C[4];
    /* 0x0380 */ s32 rightSwordBlureIndex;
    /* 0x0384 */ s32 leftSwordBlureIndex;
    /* 0x0388 */ s32 afterimageCount;
    /* 0x038C */ s16 afterimageIndex;
    /* 0x038E */ s16 flameScroll;
    /* 0x0390 */ Vec3f afterimagePos[EN_JSO2_AFTERIMAGE_COUNT];
    /* 0x0480 */ Vec3s afterimageRot[EN_JSO2_AFTERIMAGE_COUNT];
    /* 0x04F8 */ Vec3s afterimageJointTable[EN_JSO2_AFTERIMAGE_COUNT][GARO_MASTER_LIMB_MAX];
    /* 0x0E58 */ Vec3f knockbackVelocity; // Adds a little push backwards when the Garo Master is bounced off the player's shield, damaged, or stunned
    /* 0x0E64 */ Vec3f flamePos[EN_JSO2_FLAME_COUNT];
    /* 0x0EAC */ Vec3f flameScale[EN_JSO2_FLAME_COUNT];
    /* 0x0EF4 */ ColliderCylinder bodyCollider;
    /* 0x0F40 */ ColliderQuad rightSwordCollider;
    /* 0x0FC0 */ ColliderQuad leftSwordCollider;
    /* 0x1040 */ s32 animIndex;
    /* 0x1044 */ s16 cutsceneTimer;
    /* 0x1046 */ union {
                    s16 cutsceneState; // exclusively used by the EN_JSO2_TYPE_LIGHT_ARROW_ROOM type
                    s16 appearState; // exclusively used by the EN_JSO2_TYPE_NORMAL type
                };
    /* 0x1048 */ s16 subCamId;
    /* 0x104A */ s16 blowUpFrameCounter;
    /* 0x104C */ f32 subCamFov;
    /* 0x1050 */ f32 subCamFovNext;
    /* 0x1054 */ Vec3f subCamEye;
    /* 0x1060 */ Vec3f subCamAt;
    /* 0x106C */ Vec3f unk_106C; // unused, type inferred from surrounding members
    /* 0x1078 */ Vec3f subCamEyeNext;
    /* 0x1084 */ Vec3f subCamAtNext;
} EnJso2; // size = 0x1090

#endif // Z_EN_JSO2_H
