#ifndef Z_EN_EGOL_H
#define Z_EN_EGOL_H

#include "global.h"
#include "objects/object_eg/object_eg.h"

struct EnEgol;

typedef void (*EnEgolActionFunc)(struct EnEgol*, PlayState*);

#define EYEGORE_GET_PATH_INDEX(thisx) ((thisx)->params & 0x3F)
#define EYEGORE_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 6) & 0x7F)

#define EYEGORE_PATH_INDEX_NONE 0x3F
#define EYEGORE_SWITCH_FLAG_NONE 0x7F

#define EYEGORE_PARAMS(switch, pathIndex) ((((switch) & 0x7F) << 6) | ((pathIndex) & 0x3F))

#define EYEGORE_EFFECT_COUNT 100

typedef struct {
    /* 0x00 */ u8 isActive;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ s16 alpha;
    /* 0x12 */ s16 timer;
    /* 0x14 */ s16 type;
    /* 0x18 */ Vec3f velocity;
    /* 0x24 */ Vec3f accel;
    /* 0x30 */ Vec3s rot;
    /* 0x38 */ f32 scale;
} EnEgolEffect; // size = 0x3C

typedef enum EyegoreBodyPart {
    /*  0 */ EYEGORE_BODYPART_0,
    /*  1 */ EYEGORE_BODYPART_1,
    /*  2 */ EYEGORE_BODYPART_2,
    /*  3 */ EYEGORE_BODYPART_3,
    /*  4 */ EYEGORE_BODYPART_4,
    /*  5 */ EYEGORE_BODYPART_5,
    /*  6 */ EYEGORE_BODYPART_6,
    /*  7 */ EYEGORE_BODYPART_7,
    /*  8 */ EYEGORE_BODYPART_8,
    /*  9 */ EYEGORE_BODYPART_9,
    /* 10 */ EYEGORE_BODYPART_10,
    /* 11 */ EYEGORE_BODYPART_11,
    /* 12 */ EYEGORE_BODYPART_MAX
} EyegoreBodyPart;

typedef struct EnEgol {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ Vec3s jointTable[EYEGORE_LIMB_MAX];
    /* 0x0218 */ Vec3s morphTable[EYEGORE_LIMB_MAX];
    /* 0x02A8 */ EnEgolActionFunc actionFunc;
    /* 0x02AC */ Path* path;
    /* 0x02B0 */ s16 waypoint;
    /* 0x02B2 */ s16 action;
    /* 0x02B4 */ s16 waitTimer;
    /* 0x02B6 */ s16 eyeShutTimer;
    /* 0x02B4 */ s16 headRot;
    /* 0x02BA */ s16 switchFlag;
    /* 0x02BC */ s16 dmgEffectTimer;
    /* 0x02C0 */ Vec3f bodyPartsPos[EYEGORE_BODYPART_MAX];
    /* 0x0350 */ s16 bodyPartIndex;
    /* 0x0352 */ s16 pathIndex;
    /* 0x0354 */ s32 animIndex;
    /* 0x0358 */ u8 isRetreating;
    /* 0x035C */ f32 animEndFrame;
    /* 0x0360 */ f32 wakeupRange;
    /* 0x0364 */ f32 minLaserRange;
    /* 0x0368 */ UNK_TYPE1 unk_368[4]; // f32?
    /* 0x036C */ f32 laserLightScale;
    /* 0x0370 */ f32 chargeLightScale;
    /* 0x0374 */ s16 eyelidRot;
    /* 0x0376 */ s16 eyelidRotTarget;
    /* 0x0378 */ s16 actionTimer;
    /* 0x037C */ s32 laserCount;
    /* 0x0380 */ s32 chargingLaser;
    /* 0x0384 */ s16 chargeLightRot;
    /* 0x0388 */ s32 chargeLevel;
    /* 0x038C */ s32 hitPlayer;
    /* 0x0390 */ UNK_TYPE1 unk_390[0xC]; // Vec3f?
    /* 0x039C */ Vec3f eyePos;
    /* 0x03A8 */ Vec3f laserBase;
    /* 0x03B4 */ Vec3f laserScale;
    /* 0x03C0 */ Vec3f laserScaleTarget;
    /* 0x03CC */ UNK_TYPE1 unk_3CC[0xC]; // Vec3f?
    /* 0x03D8 */ Vec3f leftFootPos;
    /* 0x03E4 */ Vec3f rightFootPos;
    /* 0x03F0 */ Vec3f leftHandPos;
    /* 0x03FC */ Vec3f rightHandPos;
    /* 0x0408 */ Vec3f waypointPos;
    /* 0x0414 */ Vec3s pupilRot;
    /* 0x041A */ Vec3s laserRot;
    /* 0x0420 */ Color_RGB16 eyePrimColor;
    /* 0x0426 */ Color_RGB16 eyeEnvColor;
    /* 0x042C */ s16 laserState;
    /* 0x0430 */ s32 texScroll;
    /* 0x0434 */ EnEgolEffect effects[EYEGORE_EFFECT_COUNT];
    /* 0x1BA4 */ ColliderJntSph bodyCollider;
    /* 0x1BC4 */ ColliderJntSphElement bodyElements[6];
    /* 0x1D44 */ ColliderJntSph eyeCollider;
    /* 0x1D64 */ ColliderJntSphElement eyeElements[1];
    /* 0x1DA4 */ ColliderQuad laserCollider;
    /* 0x1E24 */ UNK_TYPE1 unk_1E24[4];
    /* 0x1E28 */ s16 subCamId;
    /* 0x1E2C */ f32 subCamFov;
    /* 0x1E30 */ f32 subCamFovTarget;
    /* 0x1E34 */ Vec3f subCamAt;
    /* 0x1E40 */ Vec3f subCamEye;
    /* 0x1E4C */ UNK_TYPE1 unk_1E4C[0x24]; // Vec3f[3]?
} EnEgol; // size = 0x1E70

#endif // Z_EN_EGOL_H
