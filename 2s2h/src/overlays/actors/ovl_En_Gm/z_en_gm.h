#ifndef Z_EN_GM_H
#define Z_EN_GM_H

#include "global.h"
#include "objects/object_in2/object_in2.h"

struct EnGm;

typedef void (*EnGmActionFunc)(struct EnGm*, PlayState*);
typedef void (*EnGmUnkFunc2)(struct EnGm*, PlayState*);

#define ENGM_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

#define ENGM_FIDGET_TABLE_LEN 3

typedef struct EnGm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnGmActionFunc actionFunc;
    /* 0x18C */ EnGmUnkFunc2 unk_18C;
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
    /* 0x259 */ u8 cueId;
    /* 0x25C */ s32 msgScriptPos;
    /* 0x260 */ s8 unk_260;
    /* 0x261 */ s8 unk_261;
    /* 0x262 */ s8 objectSlot;
    /* 0x264 */ MsgScript* msgScript;
    /* 0x268 */ Actor* unk_268;
    /* 0x26C */ UNK_TYPE1 unk_26C[0xC];
    /* 0x278 */ Vec3f unk_278;
    /* 0x284 */ Vec3f unk_284;
    /* 0x290 */ Vec3f unk_290;
    /* 0x29C */ Vec3f unk_29C;
    /* 0x2A8 */ Vec3s unk_2A8;
    /* 0x2AE */ Vec3s unk_2AE;
    /* 0x2B4 */ Vec3s jointTable[OBJECT_IN2_LIMB_MAX];
    /* 0x32C */ Vec3s morphTable[OBJECT_IN2_LIMB_MAX];
    /* 0x3A4 */ u16 unk_3A4;
    /* 0x3A6 */ u16 unk_3A6;
    /* 0x3A8 */ f32 animPlaySpeed;
    /* 0x3AC */ f32 unk_3AC;
    /* 0x3B0 */ f32 unk_3B0;
    /* 0x3B4 */ f32 unk_3B4;
    /* 0x3B8 */ s16 unk_3B8;
    /* 0x3BA */ s16 unk_3BA;
    /* 0x3BC */ s16 unk_3BC;
    /* 0x3BE */ s16 unk_3BE;
    /* 0x3C0 */ s16 unk_3C0;
    /* 0x3C2 */ s16 unk_3C2;
    /* 0x3C4 */ s16 timePathTimeSpeed;
    /* 0x3C6 */ s16 unk_3C6;
    /* 0x3C8 */ s16 unk_3C8;
    /* 0x3CA */ s16 unk_3CA;
    /* 0x3CC */ s16 unk_3CC;
    /* 0x3CE */ s16 unk_3CE;
    /* 0x3D0 */ s16 unk_3D0;
    /* 0x3D2 */ s16 fidgetTableZ[ENGM_FIDGET_TABLE_LEN];
    /* 0x3D8 */ s16 fidgetTableY[ENGM_FIDGET_TABLE_LEN];
    /* 0x3DE */ s16 unk_3DE;
    /* 0x3E0 */ s16 unk_3E0;
    /* 0x3E2 */ s16 unk_3E2;
    /* 0x3E4 */ MsgScriptCallback msgScriptCallback;
    /* 0x3E8 */ s32 animIndex;
    /* 0x3EC */ UNK_TYPE1 unk3EC[0x4];
    /* 0x3F0 */ s32 prevTalkState;
    /* 0x3F4 */ s32 unk_3F4;
    /* 0x3F8 */ s32 unk_3F8;
    /* 0x3FC */ s32 unk_3FC;
    /* 0x400 */ s32 unk_400;
} EnGm; // size = 0x404

#endif // Z_EN_GM_H
