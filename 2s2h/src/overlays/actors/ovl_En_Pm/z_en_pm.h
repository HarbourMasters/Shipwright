#ifndef Z_EN_PM_H
#define Z_EN_PM_H

#include "global.h"
#include "assets/objects/object_mm/object_mm.h"

struct EnPm;

typedef void (*EnPmActionFunc)(struct EnPm*, PlayState*);
typedef s32 (*EnPmFunc2)(struct EnPm*, PlayState*);

#define ENPM_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

typedef struct EnPm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnPmActionFunc actionFunc;
    /* 0x18C */ EnPmFunc2 unk_18C;
    /* 0x190 */ ColliderCylinder colliderCylinder;
    /* 0x1DC */ ColliderSphere colliderSphere;
    /* 0x234 */ Path* timePath;
    /* 0x238 */ Vec3f timePathTargetPos;
    /* 0x244 */ f32 timePathProgress;
    /* 0x248 */ s32 timePathTotalTime;
    /* 0x24C */ s32 timePathWaypointTime;
    /* 0x250 */ s32 timePathWaypoint;
    /* 0x254 */ s32 timePathElapsedTime;
    /* 0x258 */ u8 scheduleResult;
    /* 0x25C */ MsgScript* msgScript;
    /* 0x260 */ s8 unk_260;
    /* 0x264 */ s32 msgScriptPos;
    /* 0x268 */ Actor* unk_268;
    /* 0x26C */ Vec3f unk_26C;
    /* 0x278 */ Vec3f unk_278;
    /* 0x284 */ Vec3f unk_284;
    /* 0x290 */ Vec3s unk_290;
    /* 0x296 */ Vec3s jointTable[OBJECT_MM_LIMB_MAX];
    /* 0x2F6 */ Vec3s morphTable[OBJECT_MM_LIMB_MAX];
    /* 0x356 */ u16 unk_356;
    /* 0x358 */ u16 unk_358;
    /* 0x35C */ f32 animPlaySpeed;
    /* 0x360 */ f32 unk_360;
    /* 0x364 */ f32 unk_364;
    /* 0x368 */ f32 unk_368;
    /* 0x36C */ s16 unk_36C;
    /* 0x36E */ s16 unk_36E;
    /* 0x370 */ s16 unk_370;
    /* 0x372 */ s16 unk_372;
    /* 0x374 */ s16 timePathTimeSpeed;
    /* 0x376 */ s16 unk_376;
    /* 0x378 */ s16 unk_378;
    /* 0x37C */ MsgScriptCallback msgScriptCallback;
    /* 0x380 */ s32 unk_380;
    /* 0x384 */ s32 animIndex;
    /* 0x388 */ s32 prevTalkState;
    /* 0x38C */ s32 unk_38C;
    /* 0x390 */ UNK_TYPE1 unk_390[0x4];
    /* 0x394 */ s32 unk_394;
    /* 0x398 */ s32 unk_398;
    /* 0x39C */ UNK_TYPE1 unk_39C[0x4];
} EnPm; // size = 0x3A0

#endif // Z_EN_PM_H
