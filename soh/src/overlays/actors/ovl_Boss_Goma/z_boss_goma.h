#ifndef Z_BOSS_GOMA_H
#define Z_BOSS_GOMA_H

#include "ultra64.h"
#include "global.h"

struct BossGoma;

typedef void (*BossGomaActionFunc)(struct BossGoma*, PlayState*);

typedef enum {
    /*  0 */ BOSSGOMA_LIMB_NONE,
    /*  1 */ BOSSGOMA_LIMB_ROOT1,
    /*  2 */ BOSSGOMA_LIMB_ROOT2,
    /*  3 */ BOSSGOMA_LIMB_BODY,
    /*  4 */ BOSSGOMA_LIMB_BODY_SHELL,
    /*  5 */ BOSSGOMA_LIMB_EYE,
    /*  6 */ BOSSGOMA_LIMB_TAIL_ROOT,
    /*  7 */ BOSSGOMA_LIMB_TAIL1_ROOT,
    /*  8 */ BOSSGOMA_LIMB_TAIL2_ROOT,
    /*  9 */ BOSSGOMA_LIMB_TAIL3_ROOT,
    /* 10 */ BOSSGOMA_LIMB_TAIL4_ROOT,
    /* 11 */ BOSSGOMA_LIMB_TAIL4,
    /* 12 */ BOSSGOMA_LIMB_TAIL3,
    /* 13 */ BOSSGOMA_LIMB_TAIL2,
    /* 14 */ BOSSGOMA_LIMB_TAIL1,
    /* 15 */ BOSSGOMA_LIMB_R_LEG_ROOT,
    /* 16 */ BOSSGOMA_LIMB_R_THIGH_ROOT,
    /* 17 */ BOSSGOMA_LIMB_R_LEG_LOWER_ROOT,
    /* 18 */ BOSSGOMA_LIMB_R_FEET_ROOT,
    /* 19 */ BOSSGOMA_LIMB_R_FEET_BACK_ROOT1,
    /* 20 */ BOSSGOMA_LIMB_R_FEET_BACK_ROOT2,
    /* 21 */ BOSSGOMA_LIMB_R_FEET_BACK,
    /* 22 */ BOSSGOMA_LIMB_R_FEET,
    /* 23 */ BOSSGOMA_LIMB_R_SHIN,
    /* 24 */ BOSSGOMA_LIMB_R_KNEE_ROOT1,
    /* 25 */ BOSSGOMA_LIMB_R_KNEE_ROOT2,
    /* 26 */ BOSSGOMA_LIMB_R_KNEE,
    /* 27 */ BOSSGOMA_LIMB_R_THIGH_SHELL_ROOT1,
    /* 28 */ BOSSGOMA_LIMB_R_THIGH_SHELL_ROOT2,
    /* 29 */ BOSSGOMA_LIMB_R_THIGH_SHELL,
    /* 30 */ BOSSGOMA_LIMB_R_THIGH,
    /* 31 */ BOSSGOMA_LIMB_EYE_LID_BOTTOM_ROOT1,
    /* 32 */ BOSSGOMA_LIMB_EYE_LID_BOTTOM_ROOT2,
    /* 33 */ BOSSGOMA_LIMB_EYE_LID_BOTTOM,
    /* 34 */ BOSSGOMA_LIMB_EYE_LID_TOP_ROOT1,
    /* 35 */ BOSSGOMA_LIMB_EYE_LID_TOP_ROOT2,
    /* 36 */ BOSSGOMA_LIMB_EYE_LID_TOP,
    /* 37 */ BOSSGOMA_LIMB_IRIS_ROOT1,
    /* 38 */ BOSSGOMA_LIMB_IRIS_ROOT2,
    /* 39 */ BOSSGOMA_LIMB_IRIS,
    /* 40 */ BOSSGOMA_LIMB_MANDIBLES_ROOT1,
    /* 41 */ BOSSGOMA_LIMB_MANDIBLES_ROOT2,
    /* 42 */ BOSSGOMA_LIMB_MANDIBLES_BODY,
    /* 43 */ BOSSGOMA_LIMB_L_MANDIBLES_ROOT,
    /* 44 */ BOSSGOMA_LIMB_L_MANDIBLES1_ROOT,
    /* 45 */ BOSSGOMA_LIMB_L_MANDIBLES2_ROOT,
    /* 46 */ BOSSGOMA_LIMB_L_MANDIBLES2,
    /* 47 */ BOSSGOMA_LIMB_L_MANDIBLES1,
    /* 48 */ BOSSGOMA_LIMB_R_MANDIBLES_ROOT,
    /* 49 */ BOSSGOMA_LIMB_R_MANDIBLES1_ROOT,
    /* 50 */ BOSSGOMA_LIMB_R_MANDIBLES2_ROOT,
    /* 51 */ BOSSGOMA_LIMB_R_MANDIBLES2,
    /* 52 */ BOSSGOMA_LIMB_R_MANDIBLES1,
    /* 53 */ BOSSGOMA_LIMB_L_ANTENNA_ROOT,
    /* 54 */ BOSSGOMA_LIMB_L_ANTENNA_BODY_ROOT,
    /* 55 */ BOSSGOMA_LIMB_L_ANTENNA_SHELL_ROOT,
    /* 56 */ BOSSGOMA_LIMB_L_ANTENNA_CLAW_ROOT,
    /* 57 */ BOSSGOMA_LIMB_L_ANTENNA_CLAW,
    /* 58 */ BOSSGOMA_LIMB_L_ANTENNA_SHELL,
    /* 59 */ BOSSGOMA_LIMB_L_ANTENNA_BODY,
    /* 60 */ BOSSGOMA_LIMB_R_ANTENNA_ROOT,
    /* 61 */ BOSSGOMA_LIMB_R_ANTENNA_BODY_ROOT,
    /* 62 */ BOSSGOMA_LIMB_R_ANTENNA_SHELL_ROOT,
    /* 63 */ BOSSGOMA_LIMB_R_ANTENNA_CLAW_ROOT,
    /* 64 */ BOSSGOMA_LIMB_R_ANTENNA_CLAW,
    /* 65 */ BOSSGOMA_LIMB_R_ANTENNA_SHELL,
    /* 66 */ BOSSGOMA_LIMB_R_ANTENNA_BODY,
    /* 67 */ BOSSGOMA_LIMB_L_LEG_ROOT,
    /* 68 */ BOSSGOMA_LIMB_L_THIGH_ROOT,
    /* 69 */ BOSSGOMA_LIMB_L_LEG_LOWER_ROOT,
    /* 70 */ BOSSGOMA_LIMB_L_FEET_ROOT,
    /* 71 */ BOSSGOMA_LIMB_L_FEET_BACK_ROOT1,
    /* 72 */ BOSSGOMA_LIMB_L_FEET_BACK_ROOT2,
    /* 73 */ BOSSGOMA_LIMB_L_FEET_BACK,
    /* 74 */ BOSSGOMA_LIMB_L_FEET,
    /* 75 */ BOSSGOMA_LIMB_L_SHIN,
    /* 76 */ BOSSGOMA_LIMB_L_KNEE_ROOT1,
    /* 77 */ BOSSGOMA_LIMB_L_KNEE_ROOT2,
    /* 78 */ BOSSGOMA_LIMB_L_KNEE,
    /* 79 */ BOSSGOMA_LIMB_L_THIGH_SHELL_ROOT1,
    /* 80 */ BOSSGOMA_LIMB_L_THIGH_SHELL_ROOT2,
    /* 81 */ BOSSGOMA_LIMB_L_THIGH_SHELL,
    /* 82 */ BOSSGOMA_LIMB_L_THIGH,
    /* 83 */ BOSSGOMA_LIMB_BODY_SHELL_BACK_ROOT1,
    /* 84 */ BOSSGOMA_LIMB_BODY_SHELL_BACK_ROOT2,
    /* 85 */ BOSSGOMA_LIMB_BODY_SHELL_BACK,
    /* 86 */ BOSSGOMA_LIMB_MAX
} BossGomaLimb;

typedef struct BossGoma {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelanime;
    /* 0x0190 */ BossGomaActionFunc actionFunc;
    /* 0x0194 */ s16 frameCount; // also used as a timer
    /* 0x0196 */ s16 patienceTimer; // when non-0: walk towards player, can be stunned
    /* 0x0198 */ s16 eyeLidBottomRotX;
    /* 0x019A */ s16 eyeLidTopRotX;
    /* 0x019C */ s16 eyeClosedTimer; // also used as a second invincibility frames source
    /* 0x019E */ s16 eyeIrisRotX;
    /* 0x01A0 */ s16 eyeIrisRotY;
    /* 0x01A2 */ s16 unusedTimer;
    /* 0x01A4 */ s16 childrenGohmaState[3]; // 0 not spawned, 1 spawned, -1 dead (-1 set by child gohma)
    /* 0x01AA */ s16 tailLimbsScaleTimers[4];
    /* 0x01B2 */ s16 spawnGohmasActionTimer;
    /* 0x01B4 */ s16 eyeState;
    /* 0x01B6 */ s16 doNotMoveThisFrame;
    /* 0x01B8 */ s16 visualState;
    /* 0x01BA */ s16 invincibilityFrames;
    /* 0x01BC */ s16 subCameraId;
    /* 0x01BE */ s16 disableGameplayLogic;
    /* 0x01C0 */ s16 decayingProgress; // when defeated, textures are progressively cleared
    /* 0x01C2 */ s16 noBackfaceCulling;
    /* 0x01C4 */ s16 blinkTimer;
    /* 0x01C6 */ s16 lookedAtFrames;
    /* 0x01C8 */ char unk_1C8[0x8];
    /* 0x01D0 */ s16 actionState;
    /* 0x01D2 */ s16 framesUntilNextAction; // not always used as named
    /* 0x01D4 */ s16 timer;
    /* 0x01D6 */ s16 sfxFaintTimer;
    /* 0x01D8 */ char unk_1D8[0x10];
    /* 0x01E8 */ f32 tailLimbsScale[4];
    /* 0x01F8 */ f32 eyeIrisScaleX;
    /* 0x01FC */ f32 unusedInitX;
    /* 0x0200 */ f32 unusedInitZ;
    /* 0x0204 */ f32 mainEnvColor[3];
    /* 0x0210 */ f32 eyeEnvColor[3];
    /* 0x021C */ f32 currentAnimFrameCount; // not used consistently
    /* 0x0220 */ f32 subCameraFollowSpeed;
    /* 0x0224 */ f32 eyeIrisScaleY;
    /* 0x0228 */ f32 defeatedCameraEyeDist;
    /* 0x022C */ f32 defeatedCameraEyeAngle;
    /* 0x0230 */ char unk_230[0x30];
    /* 0x0260 */ Vec3f lastTailLimbWorldPos;
    /* 0x026C */ Vec3f firstTailLimbWorldPos;
    /* 0x0278 */ Vec3f rightHandBackLimbWorldPos;
    /* 0x0284 */ Vec3f leftHandBackLimbWorldPos;
    /* 0x0290 */ Vec3f subCameraEye;
    /* 0x029C */ Vec3f subCameraAt;
    /* 0x02A8 */ Vec3f defeatedLimbPositions[100]; // only 85/86 first indices actually used
    /* 0x0758 */ u8 deadLimbsState[100]; // only 85/90 first indices actually used
    /* 0x07BC */ ColliderJntSph collider;
    /* 0x07DC */ ColliderJntSphElement colliderItems[13];
} BossGoma; // size = 0x0B1C

#endif
