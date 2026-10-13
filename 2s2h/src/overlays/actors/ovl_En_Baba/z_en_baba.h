#ifndef Z_EN_BABA_H
#define Z_EN_BABA_H

#include "global.h"
#include "overlays/actors/ovl_En_Sob1/z_en_sob1.h"
#include "objects/object_bba/object_bba.h"

struct EnBaba;

typedef void (*EnBabaActionFunc)(struct EnBaba*, PlayState*);

#define BOMB_SHOP_LADY_GET_PATH_INDEX(thisx) (((thisx)->params & 0x3F00) >> 8)
#define BOMB_SHOP_LADY_GET_TYPE(thisx) (((thisx)->params & 0xC000) >> 0xE)

#define BOMB_SHOP_LADY_PATH_INDEX_NONE 0x3F

// Types BOMB_SHOP_LADY_TYPE_FOLLOW_SCHEDULE and BOMB_SHOP_LADY_TYPE_IDLE can only be used in SCENE_BACKTOWN
// Type BOMB_SHOP_LADY_TYPE_SWAY can be used anywhere except SCENE_BACKTOWN and SCENE_BOMYA
typedef enum {
    /* 0 */ BOMB_SHOP_LADY_TYPE_FOLLOW_SCHEDULE,
    /* 1 */ BOMB_SHOP_LADY_TYPE_IDLE,
    /* 2 */ BOMB_SHOP_LADY_TYPE_SWAY
} BombShopLadyType;

typedef struct EnBaba {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnSob1* bombShopkeeper;
    /* 0x148 */ EnBabaActionFunc actionFunc;
    /* 0x14C */ UNK_TYPE1 unk14C[4];
    /* 0x150 */ SkelAnime skelAnime;
    /* 0x194 */ ColliderCylinder collider;
    /* 0x1E0 */ u16 textId;
    /* 0x1E2 */ u8 msgFading;
    /* 0x1E4 */ Path* path;
    /* 0x1E8 */ s16 waypoint;
    /* 0x1EA */ UNK_TYPE1 unk1EA[0x1C];
    /* 0x206 */ Vec3s jointTable[BOMB_SHOP_LADY_LIMB_MAX];
    /* 0x272 */ Vec3s morphTable[BOMB_SHOP_LADY_LIMB_MAX];
    /* 0x2DE */ Vec3s trackTarget;
    /* 0x2E4 */ Vec3s headRot;
    /* 0x2EA */ Vec3s torsoRot;
    /* 0x2F0 */ UNK_TYPE1 unk2F0[0x12];
    /* 0x302 */ s16 fidgetTableY[BOMB_SHOP_LADY_LIMB_MAX];
    /* 0x326 */ s16 fidgetTableZ[BOMB_SHOP_LADY_LIMB_MAX];
    /* 0x34A */ UNK_TYPE1 unk34A[0xBA];
    /* 0x404 */ s16 sakonDeadTimer;
    /* 0x406 */ UNK_TYPE1 unk406[4];
    /* 0x40A */ u16 stateFlags;
    /* 0x40C */ s32 animIndex;
    /* 0x410 */ Path* timePath;
    /* 0x414 */ Vec3f timePathPoint;
    /* 0x420 */ f32 timePathProgress;
    /* 0x424 */ s32 timePathTotalTime;
    /* 0x428 */ s32 timePathWaypointTime;
    /* 0x42C */ s32 timePathWaypoint;
    /* 0x430 */ s32 timePathElapsedTime;
    /* 0x434 */ u8 scheduleResult;
    /* 0x436 */ s16 timePathTimeSpeed;
    /* 0x438 */ s32 timePathIsSetup;
    /* 0x43C */ s32 timePathHasReachedEnd;
} EnBaba; // size = 0x440

#endif // Z_EN_BABA_H
