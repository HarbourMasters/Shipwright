#ifndef Z_EN_IG_H
#define Z_EN_IG_H

#include "global.h"
#include "objects/object_dai/object_dai.h"

struct EnIg;

typedef void (*EnIgActionFunc)(struct EnIg*, PlayState*);
typedef void (*EnIgUnkFunc2)(struct EnIg*, PlayState*);

#define ENIG_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

typedef struct EnIg {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnIgActionFunc actionFunc;
    /* 0x18C */ EnIgUnkFunc2 unk_18C;
    /* 0x190 */ MtxF unk_190;
    /* 0x1D0 */ ColliderCylinder collider1;
    /* 0x21C */ ColliderSphere collider2;
    /* 0x274 */ Path* timePath;
    /* 0x278 */ Vec3f timePathTargetPos;
    /* 0x284 */ f32 timePathProgress;
    /* 0x288 */ s32 timePathTotalTime;
    /* 0x28C */ s32 timePathWaypointTime;
    /* 0x290 */ s32 timePathWaypoint;
    /* 0x294 */ s32 timePathElapsedTime;
    /* 0x298 */ u8 scheduleResult;
    /* 0x29C */ MsgScript* msgScript;
    /* 0x2A0 */ s32 msgScriptPos;
    /* 0x2A4 */ s8 unk_2A4;
    /* 0x2A8 */ Actor* unk_2A8;
    /* 0x2AC */ UNK_TYPE1 unk2AC[0x4];
    /* 0x2B0 */ Vec3f unk_2B0;
    /* 0x2BC */ Vec3f unk_2BC;
    /* 0x2C8 */ UNK_TYPE1 unk2C8[0xC];
    /* 0x2D4 */ Vec3f unk_2D4;
    /* 0x2E0 */ UNK_TYPE1 unk2E0[0x6];
    /* 0x2E6 */ Vec3s unk_2E6;
    /* 0x2EC */ Vec3s jointTable[OBJECT_DAI_LIMB_MAX];
    /* 0x35E */ Vec3s morphTable[OBJECT_DAI_LIMB_MAX];
    /* 0x3D0 */ u16 unk_3D0;
    /* 0x3D2 */ u16 unk_3D2;
    /* 0x3D4 */ f32 animPlaySpeed;
    /* 0x3D8 */ UNK_TYPE1 unk3D8[0x8];
    /* 0x3E0 */ s16 unk_3E0;
    /* 0x3E2 */ s16 unk_3E2;
    /* 0x3E4 */ s16 unk_3E4;
    /* 0x3E6 */ s16 unk_3E6;
    /* 0x3E8 */ s16 unk_3E8;
    /* 0x3EA */ s16 unk_3EA;
    /* 0x3EC */ s16 timePathTimeSpeed;
    /* 0x3EE */ s16 unk_3EE;
    /* 0x3F0 */ s16 unk_3F0;
    /* 0x3F2 */ s16 unk_3F2;
    /* 0x3F4 */ s16 unk_3F4;
    /* 0x3F6 */ s16 unk_3F6;
    /* 0x3F8 */ MsgScriptCallback msgScriptCallback;
    /* 0x3FC */ s32 animIndex;
    /* 0x400 */ UNK_TYPE1 unk400[0x8];
    /* 0x408 */ s32 unk_408;
} EnIg; // size = 0x40C

#endif // Z_EN_IG_H
