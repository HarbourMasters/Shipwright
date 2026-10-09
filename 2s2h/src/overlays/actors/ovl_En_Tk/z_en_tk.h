#ifndef Z_EN_TK_H
#define Z_EN_TK_H

#include "global.h"
#include "objects/object_tk/object_tk.h"

struct EnTk;

typedef void (*EnTkActionFunc)(struct EnTk*, PlayState*);
typedef void (*EnTkUnkFunc)(struct EnTk*, PlayState*);

#define ENTK_GET_F(thisx) ((thisx)->params & 0xF)
#define ENTK_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 4) & 0x7F)
#define ENTK_GET_PATH_INDEX(thisx) (((thisx)->params >> 0xB) & 0x1F)

typedef struct EnTk {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnTkActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ Vec3s jointTable[OBJECT_TK_LIMB_MAX];
    /* 0x244 */ Vec3s morphTable[OBJECT_TK_LIMB_MAX];
    /* 0x2B0 */ s8 unk_2B0;
    /* 0x2B1 */ s8 switchFlag;
    /* 0x2B4 */ Vec3f unk_2B4;
    /* 0x2C0 */ s16 unk_2C0;
    /* 0x2C2 */ s16 unk_2C2;
    /* 0x2C4 */ s16 unk_2C4;
    /* 0x2C6 */ s16 unk_2C6;
    /* 0x2C8 */ UNK_TYPE1 unk2C8[0x2];
    /* 0x2CA */ u16 unk_2CA;
    /* 0x2CC */ s16 unk_2CC;
    /* 0x2D0 */ s32 unk_2D0;
    /* 0x2D4 */ s32 animIndex;
    /* 0x2D8 */ f32 unk_2D8;
    /* 0x2DC */ f32 unk_2DC;
    /* 0x2E0 */ s32 unk_2E0;
    /* 0x2E4 */ s16 unk_2E4;
    /* 0x2E6 */ u16 unk_2E6;
    /* 0x2E8 */ s16 csLength;
    /* 0x2EC */ Vec3f unk_2EC;
    /* 0x2F8 */ Vec3s unk_2F8;
    /* 0x300 */ Vec3f unk_300;
    /* 0x30C */ EnTkUnkFunc unk_30C;
    /* 0x310 */ s16 unk_310;
    /* 0x312 */ s16 csIdList[2];
    /* 0x316 */ s16 unk_316;
    /* 0x318 */ s16 unk_318;
    /* 0x31A */ s16 unk_31A;
    /* 0x31C */ s16 unk_31C;
    /* 0x320 */ f32 animCurFrame;
    /* 0x324 */ Vec3f unk_324[6];
    /* 0x36C */ s32 unk_36C;
    /* 0x370 */ UNK_TYPE1 unk370[0x58];
    /* 0x3C8 */ Path* timePath;
    /* 0x3CC */ u8 scheduleResult;
    /* 0x3CE */ u16 unk_3CE;
    /* 0x3D0 */ s32 timePathTimeSpeed;
    /* 0x3D4 */ Vec3f timePathTargetPos;
    /* 0x3E0 */ f32 timePathProgress;
    /* 0x3E4 */ s32 timePathTotalTime;
    /* 0x3E8 */ s32 timePathWaypointTime;
    /* 0x3EC */ s32 timePathWaypoint;
    /* 0x3F0 */ s32 timePathElapsedTime;
} EnTk; // size = 0x3F4

#endif // Z_EN_TK_H
