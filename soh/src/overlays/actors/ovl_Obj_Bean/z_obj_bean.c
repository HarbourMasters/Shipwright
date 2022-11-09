/*
 * File: z_obj_bean.c
 * Overlay: ovl_Obj_Bean
 * Description: Bean plant spot
 */

#include "z_obj_bean.h"
#include "objects/object_mamenoki/object_mamenoki.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_22

void ObjBean_Init(Actor* thisx, PlayState* play);
void ObjBean_Destroy(Actor* thisx, PlayState* play);
void ObjBean_Update(Actor* thisx, PlayState* play);
void ObjBean_Draw(Actor* thisx, PlayState* play);

void ObjBean_WaitForPlayer(ObjBean* this, PlayState* play);
void ObjBean_Fly(ObjBean* this, PlayState* play);
void ObjBean_SetupFly(ObjBean* this);
void ObjBean_WaitForWater(ObjBean* this, PlayState* play);
void ObjBean_SetupWaitForWater(ObjBean* this);
void ObjBean_SetupGrowWaterPhase1(ObjBean* this);
void ObjBean_GrowWaterPhase1(ObjBean* this, PlayState* play);
void ObjBean_GrowWaterPhase2(ObjBean* this, PlayState* play);
void ObjBean_SetupGrowWaterPhase2(ObjBean* this);
void ObjBean_GrowWaterPhase3(ObjBean* this, PlayState* play);
void ObjBean_SetupGrowWaterPhase3(ObjBean* this);
void ObjBean_SetupGrown(ObjBean* this);
void ObjBean_FlattenLeaves(ObjBean* this);
void ObjBean_Grown(ObjBean* this);
void ObjBean_LeavesStill(ObjBean* this);
void ObjBean_SetupShakeLeaves(ObjBean* this);
void ObjBean_ShakeLeaves(ObjBean* this);
void ObjBean_SetupWaitForBean(ObjBean* this);
void ObjBean_WaitForBean(ObjBean* this, PlayState* play);
void func_80B8FE3C(ObjBean* this, PlayState* play);
void func_80B8FE00(ObjBean* this);
void func_80B8FE6C(ObjBean* this);
void func_80B8FEAC(ObjBean* this, PlayState* play);
void func_80B8FF50(ObjBean* this);
void ObjBean_SetupGrowWaterPhase4(ObjBean* this);
void func_80B8FF8C(ObjBean* this, PlayState* play);
void func_80B90050(ObjBean* this, PlayState* play);
void func_80B90010(ObjBean* this);
void func_80B908EC(ObjBean* this);
void func_80B90918(ObjBean* this, PlayState* play);
void func_80B90970(ObjBean* this);
void func_80B909B0(ObjBean* this, PlayState* play);
void func_80B909F8(ObjBean* this);
void func_80B90A34(ObjBean* this, PlayState* play);
void ObjBean_SetupWaitForPlayer(ObjBean* this);
void ObjBean_GrowWaterPhase4(ObjBean* this, PlayState* play);
void ObjBean_GrowWaterPhase5(ObjBean* this, PlayState* play);
void ObjBean_SetupGrowWaterPhase5(ObjBean* this);
void ObjBean_SetupShakeLeavesFast(ObjBean* this);
void ObjBean_ShakeLeavesFast(ObjBean* this);
void ObjBean_Grow(ObjBean* this);
void ObjBean_SetupGrow(ObjBean* this);
void ObjBean_SetupWaitForStepOff(ObjBean* this);
void ObjBean_WaitForStepOff(ObjBean* this, PlayState* play);

#define BEAN_STATE_DRAW_LEAVES (1 << 0)
#define BEAN_STATE_DRAW_SOIL (1 << 1)
#define BEAN_STATE_DRAW_PLANT (1 << 2)
#define BEAN_STATE_DRAW_STALK (1 << 3)
#define BEAN_STATE_COLLIDER_SET (1 << 4)
#define BEAN_STATE_DYNAPOLY_SET (1 << 5)
#define BEAN_STATE_BEEN_WATERED (1 << 6)
#define BEAN_STATE_PLAYER_ON_TOP (1 << 7)

static ObjBean* D_80B90E30 = NULL;

const ActorInit Obj_Bean_InitVars = {
    ACTOR_OBJ_BEAN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MAMENOKI,
    sizeof(ObjBean),
    (ActorFunc)ObjBean_Init,
    (ActorFunc)ObjBean_Destroy,
    (ActorFunc)ObjBean_Update,
    (ActorFunc)ObjBean_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 64, 30, -31, { 0, 0, 0 } },
};

typedef struct {
    f32 velocity;
    f32 accel;
} BeenSpeedInfo;

static BeenSpeedInfo sBeanSpeeds[] = {
    { 3.0f, 0.3f },
    { 10.0f, 0.5f },
    { 30.0f, 0.5f },
    { 3.0f, 0.3f },
};

static Gfx* sBreakDlists[] = { gCuttableShrubStalkDL, gCuttableShrubTipDL };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1600, ICHAIN_STOP),
};

void ObjBean_InitCollider(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
}

void ObjBean_InitDynaPoly(ObjBean* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader;
    s32 pad2;

    colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void ObjBean_FindFloor(ObjBean* this, PlayState* play) {
    Vec3f vec;
    s32 sp20;

    vec.x = this->dyna.actor.world.pos.x;
    vec.y = this->dyna.actor.world.pos.y + 29.999998f;
    vec.z = this->dyna.actor.world.pos.z;
    this->dyna.actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&play->colCtx, &this->dyna.actor.floorPoly, &sp20, &this->dyna.actor, &vec);
}

void func_80B8EBC8(ObjBean* this) {
    this->unk_1B6.x = this->unk_1B6.y = this->unk_1B6.z = 0;
    this->unk_1E4 = 0.0f;
}

void ObjBean_UpdatePosition(ObjBean* this) {
    f32 temp_f20;

    this->unk_1B6.x += 0xB6;
    this->unk_1B6.y += 0xFB;
    this->unk_1B6.z += 0x64;

    Math_StepToF(&this->unk_1E4, 2.0f, 0.1f);
    temp_f20 = Math_SinS(this->unk_1B6.x * 3);
    this->posOffsetX = (Math_SinS(((this->unk_1B6.y * 3))) + temp_f20) * this->unk_1E4;
    temp_f20 = Math_CosS(this->unk_1B6.x * 4);
    this->posOffsetZ = (Math_CosS((this->unk_1B6.y * 4)) + temp_f20) * this->unk_1E4;
    temp_f20 = Math_SinS(this->unk_1B6.z * 5);

    this->dyna.actor.scale.x = this->dyna.actor.scale.z =
        ((Math_SinS((this->unk_1B6.y * 8)) * 0.01f) + (temp_f20 * 0.06f) + 1.07f) * 0.1f;

    this->dyna.actor.scale.y = ((Math_CosS(((this->unk_1B6.z * 10))) * 0.2f) + 1.0f) * 0.1f;
    temp_f20 = Math_SinS(this->unk_1B6.x * 3);
    this->dyna.actor.shape.rot.y =
        (Math_SinS((s16)(this->unk_1B6.z * 2)) * 2100.0f) + ((f32)this->dyna.actor.home.rot.y + (temp_f20 * 1000.0f));
}

void func_80B8EDF4(ObjBean* this) {
    this->unk_1B6.x = this->unk_1B6.y = this->unk_1B6.z = 0;

    Actor_SetScale(&this->dyna.actor, 0.0f);
}

void func_80B8EE24(ObjBean* this) {
    this->unk_1B6.x += 0x384;
    if (this->unk_1B6.x > 0x5FFF) {
        this->unk_1B6.x = 0x5FFF;
    }
    this->unk_1B6.y += 0x258;
    if (this->unk_1B6.y > 0x4000) {
        this->unk_1B6.y = 0x4000;
    }
    this->dyna.actor.scale.y = Math_SinS(this->unk_1B6.x) * 0.16970563f;

    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_SinS(this->unk_1B6.y) * 0.10700001f;

    Math_StepToF(&this->posOffsetX, 0.0f, 0.1f);
    Math_StepToF(&this->posOffsetZ, 0.0f, 0.1f);
    Math_ScaledStepToS(&this->dyna.actor.shape.rot.y, this->dyna.actor.home.rot.y, 0x64);
}

void ObjBean_Move(ObjBean* this) {
    this->dyna.actor.world.pos.x = this->pathPoints.x + this->posOffsetX;
    this->dyna.actor.world.pos.y = this->pathPoints.y;
    this->dyna.actor.world.pos.z = this->pathPoints.z + this->posOffsetZ;
}

void ObjBean_SetDrawMode(ObjBean* this, u8 drawFlag) {
    this->stateFlags &=
        ~(BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_PLANT | BEAN_STATE_DRAW_STALK | BEAN_STATE_DRAW_SOIL);
    this->stateFlags |= drawFlag;
}

void ObjBean_SetupPathCount(ObjBean* this, PlayState* play) {
    this->pathCount = play->setupPathList[(this->dyna.actor.params >> 8) & 0x1F].count - 1;
    this->currentPointIndex = 0;
    this->nextPointIndex = 1;
}

void ObjBean_SetupPath(ObjBean* this, PlayState* play) {
    Path* path = &play->setupPathList[(this->dyna.actor.params >> 8) & 0x1F];
    Math_Vec3s_ToVec3f(&this->pathPoints, SEGMENTED_TO_VIRTUAL(path->points));
}

void ObjBean_FollowPath(ObjBean* this, PlayState* play) {
    Path* path;
    Vec3f acell;
    Vec3f pathPointsFloat;
    f32 speed;
    Vec3s* nextPathPoint;
    Vec3s* currentPoint;
    Vec3s* sp4C;
    Vec3f sp40;
    Vec3f sp34;
    f32 sp30;
    f32 mag;

    Math_StepToF(&this->dyna.actor.speedXZ, sBeanSpeeds[this->unk_1F6].velocity, sBeanSpeeds[this->unk_1F6].accel);
    path = &play->setupPathList[(this->dyna.actor.params >> 8) & 0x1F];
    nextPathPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->nextPointIndex];

    Math_Vec3s_ToVec3f(&pathPointsFloat, nextPathPoint);

    Math_Vec3f_Diff(&pathPointsFloat, &this->pathPoints, &acell);
    mag = Math3D_Vec3fMagnitude(&acell);
    speed = CLAMP_MIN(this->dyna.actor.speedXZ, 0.5f);
    if (speed > mag) {
        currentPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->currentPointIndex];

        Math_Vec3f_Copy(&this->pathPoints, &pathPointsFloat);
        this->currentPointIndex = this->nextPointIndex;

        if (this->pathCount <= this->currentPointIndex) {
            this->nextPointIndex = 0;
        } else {
            this->nextPointIndex++;
        }
        sp4C = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->nextPointIndex];
        Math_Vec3s_DiffToVec3f(&sp40, nextPathPoint, currentPoint);
        Math_Vec3s_DiffToVec3f(&sp34, sp4C, nextPathPoint);
        if (Math3D_CosOut(&sp40, &sp34, &sp30)) {
            this->dyna.actor.speedXZ = 0.0f;
        } else {
            this->dyna.actor.speedXZ *= (sp30 + 1.0f) * 0.5f;
        }
    } else {
        Math_Vec3f_Scale(&acell, this->dyna.actor.speedXZ / mag);
        this->pathPoints.x += acell.x;
        this->pathPoints.y += acell.y;
        this->pathPoints.z += acell.z;
    }
}

s32 ObjBean_CheckForHorseTrample(ObjBean* this, PlayState* play) {
    Actor* currentActor = play->actorCtx.actorLists[ACTORCAT_BG].head;

    while (currentActor != NULL) {
        if ((currentActor->id == ACTOR_EN_HORSE) &&
            (Math3D_Vec3fDistSq(&currentActor->world.pos, &this->dyna.actor.world.pos) < 10000.0f)) {
            return true;
        }
        currentActor = currentActor->next;
    }

    return false;
}

void ObjBean_Break(ObjBean* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f20;
    s16 angle;
    s32 scale;
    s32 i;
    s16 gravity;
    s16 arg5;

    angle = 0;
    for (i = 0; i < 36; i++) {
        angle += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 60.0f;

        pos.x = (Math_SinS(angle) * temp_f20) + this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y;
        pos.z = (Math_CosS(angle) * temp_f20) + this->dyna.actor.world.pos.z;

        velocity.x = Math_SinS(angle) * 3.5f;
        velocity.y = Rand_ZeroOne() * 13.0f;
        velocity.z = Math_CosS(angle) * 3.5f;

        velocity.x += this->dyna.actor.world.pos.x - this->dyna.actor.prevPos.x;
        velocity.y += this->dyna.actor.world.pos.y - this->dyna.actor.prevPos.y;
        velocity.z += this->dyna.actor.world.pos.z - this->dyna.actor.prevPos.z;

        scale = (s32)(Rand_ZeroOne() * 180.0f) + 30;
        if (scale < 90) {
            if (Rand_ZeroOne() < 0.1f) {
                gravity = -80;
                arg5 = 96;
            } else {
                gravity = -80;
                arg5 = 64;
            }
        } else {
            gravity = -100;
            arg5 = 64;
        }
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, gravity, arg5, 40, 3, 0, scale, 0, 0,
                             (s16)((scale >> 3) + 40), -1, 1, sBreakDlists[i & 1]);
    }
}

void ObjBean_UpdateLeaves(ObjBean* this) {
    Math_StepToS(&this->unk_1C2, this->unk_1C4, this->unk_1C6);
    Math_StepToS(&this->unk_1C8, this->unk_1CA, this->unk_1CC);
    this->unk_1CE += this->unk_1C8;
    this->leafRotFactor = 6372.0f - Math_SinS(this->unk_1CE) * (f32)this->unk_1C2;
    this->dyna.actor.scale.y = Math_SinS(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_CosS(this->leafRotFactor) * 0.12207746f;
}

void ObjBean_SetupLeavesStill(ObjBean* this) {
    this->transformFunc = ObjBean_LeavesStill;
    this->unk_1C0 = Rand_S16Offset(12, 40);
    this->unk_1C4 = Rand_S16Offset(0xC8, 0x190);
    this->unk_1C6 = 0x14;
    this->unk_1CA = Rand_S16Offset(0x64, 0x320);
    this->unk_1CC = 0x14;
}

void ObjBean_LeavesStill(ObjBean* this) {
    this->unk_1C0--;
    ObjBean_UpdateLeaves(this);
    if (this->unk_1C0 < 0) {
        ObjBean_SetupShakeLeaves(this);
    }
}

void ObjBean_SetupShakeLeaves(ObjBean* this) {
    this->transformFunc = ObjBean_ShakeLeaves;
    this->unk_1C0 = Rand_S16Offset(30, 4);
    this->unk_1C4 = Rand_S16Offset(0x7D0, 0x3E8);
    this->unk_1C6 = 0xC8;
    this->unk_1CA = Rand_S16Offset(0x36B0, 0x1770);
    this->unk_1CC = 0xFA0;
    this->leafRotFactor = 0x18E4;
}

void ObjBean_ShakeLeaves(ObjBean* this) {
    this->unk_1C0 += -1;
    if (this->unk_1C0 == 14) {
        this->unk_1C4 = Rand_S16Offset(0xC8, 0x190);
        this->unk_1CA = Rand_S16Offset(0x64, 0x1F4);
        this->unk_1CC = 0x7D0;
    }
    ObjBean_UpdateLeaves(this);
    if (this->unk_1C0 < 0) {
        ObjBean_SetupLeavesStill(this);
    }
}

void ObjBean_SetupShakeLeavesFast(ObjBean* this) {
    this->transformFunc = ObjBean_ShakeLeavesFast;
    this->unk_1C0 = 0x28;
    this->unk_1C4 = 0xBB8;
    this->unk_1C6 = 0x12C;
    this->unk_1CA = 0x3A98;
    this->unk_1CC = 0xFA0;
    this->leafRotFactor = 0x18E4;
}

void ObjBean_ShakeLeavesFast(ObjBean* this) {
    this->unk_1C0 += -1;
    if (Rand_ZeroOne() < 0.1f) {
        this->unk_1C4 = Rand_S16Offset(0x898, 0x3E8);
        this->unk_1CA = Rand_S16Offset(0x2EE0, 0x1F40);
    }
    ObjBean_UpdateLeaves(this);
    if ((s32)this->unk_1C0 < 0) {
        ObjBean_SetupGrow(this);
    }
}

void ObjBean_SetupGrow(ObjBean* this) {
    this->transformFunc = ObjBean_Grow;
}

void ObjBean_Grow(ObjBean* this) {
    Math_StepToS(&this->leafRotFactor, 0x33E9, 0x168);
    this->dyna.actor.scale.y = Math_SinS(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_CosS(this->leafRotFactor) * 0.12207746f;
    ;
}

void ObjBean_SetupFlattenLeaves(ObjBean* this) {
    this->transformFunc = ObjBean_FlattenLeaves;
    this->leafRotFactor = 0x33E9;
}

void ObjBean_FlattenLeaves(ObjBean* this) {
    this->leafRotFactor -= 0x960;
    this->dyna.actor.scale.y = Math_SinS(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_CosS(this->leafRotFactor) * 0.12207746f;

    if (this->leafRotFactor < 0x18E4) {
        ObjBean_SetupGrown(this);
    }
}

void ObjBean_SetupGrown(ObjBean* this) {
    this->transformFunc = ObjBean_Grown;
    this->unk_1C2 = 0xBB8;
    this->unk_1C4 = 0;
    this->unk_1C6 = 0xC8;
    this->unk_1C8 = 0x3E80;
    this->unk_1CA = 0x1F4;
    this->unk_1CC = 0;
    this->unk_1C0 = 0x10;
}

void ObjBean_Grown(ObjBean* this) {
    this->unk_1C0--;
    if (this->unk_1C0 == 6) {
        this->unk_1CC = 0x7D0;
    }
    ObjBean_UpdateLeaves(this);
    if (this->unk_1C2 <= 0) {
        ObjBean_SetupLeavesStill(this);
    }
}

void ObjBean_Init(Actor* thisx, PlayState* play) {
    s32 path;
    s32 linkAge;
    ObjBean* this = (ObjBean*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F) || (mREG(1) == 1)) {
            path = (this->dyna.actor.params >> 8) & 0x1F;
            if (path == 0x1F) {
                osSyncPrintf(VT_COL(RED, WHITE));
                // "No path data?"
                osSyncPrintf("パスデータが無い？(%s %d)(arg_data %xH)\n", __FILE__, __LINE__,
                             this->dyna.actor.params);
                osSyncPrintf(VT_RST);
                Actor_Kill(&this->dyna.actor);
                return;
            }
            if (play->setupPathList[path].count < 3) {
                osSyncPrintf(VT_COL(RED, WHITE));
                // "Incorrect number of path data"
                osSyncPrintf("パスデータ数が不正(%s %d)(arg_data %xH)\n", __FILE__, __LINE__,
                             this->dyna.actor.params);
                osSyncPrintf(VT_RST);
                Actor_Kill(&this->dyna.actor);
                return;
            }
            ObjBean_SetupPathCount(this, play);
            ObjBean_SetupPath(this, play);
            ObjBean_Move(this);
            ObjBean_SetupWaitForPlayer(this);

            ObjBean_InitDynaPoly(this, play, &gMagicBeanPlatformCol, DPM_UNK3);
            this->stateFlags |= BEAN_STATE_DYNAPOLY_SET;
            ObjBean_InitCollider(&this->dyna.actor, play);
            this->stateFlags |= BEAN_STATE_COLLIDER_SET;

            ActorShape_Init(&this->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 8.8f);
            ObjBean_FindFloor(this, play);
            this->unk_1F6 = this->dyna.actor.home.rot.z & 3;
        } else {
            Actor_Kill(&this->dyna.actor);
            return;
        }
    } else if ((Flags_GetSwitch(play, this->dyna.actor.params & 0x3F) != 0) || (mREG(1) == 1)) {
        ObjBean_SetupWaitForWater(this);
    } else {
        ObjBean_SetupWaitForBean(this);
    }
    this->dyna.actor.world.rot.z = this->dyna.actor.home.rot.z = this->dyna.actor.shape.rot.z = 0;
    // "Magic bean tree lift"
    osSyncPrintf("(魔法の豆の木リフト)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void ObjBean_Destroy(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    if (this->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    if (this->stateFlags & BEAN_STATE_COLLIDER_SET) {
        Collider_DestroyCylinder(play, &this->collider);
    }
    if (D_80B90E30 == this) {
        D_80B90E30 = NULL;
    }
}

void ObjBean_SetupWaitForBean(ObjBean* this) {
    this->actionFunc = ObjBean_WaitForBean;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES);
    this->dyna.actor.textId = 0x2F;
}

void ObjBean_WaitForBean(ObjBean* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->dyna.actor, play)) {
        if (func_8002F368(play) == EXCH_ITEM_BEAN) {
            func_80B8FE00(this);
            Flags_SetSwitch(play, this->dyna.actor.params & 0x3F);
        }
    } else {
        func_8002F298(&this->dyna.actor, play, 40.0f, EXCH_ITEM_BEAN);
    }
}

void func_80B8FE00(ObjBean* this) {
    this->actionFunc = func_80B8FE3C;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES);
    this->timer = 60;
}

// Link is looking at the soft soil
void func_80B8FE3C(ObjBean* this, PlayState* play) {
    if (this->timer <= 0) {
        func_80B8FE6C(this);
    }
}

void func_80B8FE6C(ObjBean* this) {
    this->actionFunc = func_80B8FEAC;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_SetScale(&this->dyna.actor, 0.01f);
}

// The leaves are visable and growing
void func_80B8FEAC(ObjBean* this, PlayState* play) {
    s32 temp_v1 = true;

    temp_v1 &= Math_StepToF(&this->dyna.actor.scale.y, 0.16672663f, 0.01f);
    temp_v1 &= Math_StepToF(&this->dyna.actor.scale.x, 0.03569199f, 0.00113f);

    this->dyna.actor.scale.z = this->dyna.actor.scale.x;
    if (temp_v1) {
        if (this->timer <= 0) {
            func_80B8FF50(this);
        }
    } else {
        this->timer = 1;
    }
    func_8002F974(&this->dyna.actor, NA_SE_PL_PLANT_GROW_UP - SFX_FLAG);
}

void func_80B8FF50(ObjBean* this) {
    this->actionFunc = func_80B8FF8C;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->unk_1B6.x = 0x33E9;
}

void func_80B8FF8C(ObjBean* this, PlayState* play) {
    this->unk_1B6.x -= 0x960;
    this->dyna.actor.scale.y = Math_SinS(this->unk_1B6.x) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_CosS(this->unk_1B6.x) * 0.12207746f;
    if (this->unk_1B6.x < 0x18E4) {
        func_80B90010(this);
    }
}

void func_80B90010(ObjBean* this) {
    this->actionFunc = func_80B90050;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->unk_1B6.x = 0;
    this->unk_1B6.y = 0xBB8;
}

// Control is returned to the player and the leaves start to flatten out
void func_80B90050(ObjBean* this, PlayState* play) {
    s16 temp_a0;
    f32 temp_f2;

    this->unk_1B6.x += 0x3E80;
    this->unk_1B6.y += -0xC8;
    temp_a0 = 6372.0f - Math_SinS(this->unk_1B6.x) * this->unk_1B6.y;

    this->dyna.actor.scale.y = Math_SinS(temp_a0) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = Math_CosS(temp_a0) * 0.12207746f;
    if (this->unk_1B6.y < 0) {
        ObjBean_SetupWaitForWater(this);
    }
}

void ObjBean_SetupWaitForWater(ObjBean* this) {
    this->actionFunc = ObjBean_WaitForWater;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_SetScale(&this->dyna.actor, 0.1f);
    ObjBean_SetupLeavesStill(this);
}

void ObjBean_WaitForWater(ObjBean* this, PlayState* play) {
    this->transformFunc(this);

    if (!(this->stateFlags & BEAN_STATE_BEEN_WATERED) && Flags_GetEnv(play, 5) && (D_80B90E30 == NULL) &&
        (this->dyna.actor.xzDistToPlayer < 50.0f)) {
        ObjBean_SetupGrowWaterPhase1(this);
        D_80B90E30 = this;
        OnePointCutscene_Init(play, 2210, -99, &this->dyna.actor, MAIN_CAM);
        this->dyna.actor.flags |= ACTOR_FLAG_4;
        return;
    }

    if ((D_80B90E30 == this) && !Flags_GetEnv(play, 5)) {
        D_80B90E30 = NULL;
        if (D_80B90E30) {}
    }
}

void ObjBean_SetupGrowWaterPhase1(ObjBean* this) {
    this->actionFunc = ObjBean_GrowWaterPhase1;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    ObjBean_SetupShakeLeavesFast(this);
    this->timer = 50;
}

// Camera moves and leaves move quickly
void ObjBean_GrowWaterPhase1(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    if (this->timer <= 0) {
        ObjBean_SetupGrowWaterPhase2(this);
    }
}

void ObjBean_SetupGrowWaterPhase2(ObjBean* this) {
    this->actionFunc = ObjBean_GrowWaterPhase2;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    this->stalkSizeMultiplier = 0.0f;
}

// BeanStalk is visable and is growing
void ObjBean_GrowWaterPhase2(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    this->stalkSizeMultiplier += 0.001f;
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y + (s16)(this->stalkSizeMultiplier * 700000.0f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->stalkSizeMultiplier * 800.0f;
    if (this->stalkSizeMultiplier >= 0.1f) { // 100 Frames
        ObjBean_SetupGrowWaterPhase3(this);
    }
    func_8002F974(&this->dyna.actor, NA_SE_PL_PLANT_TALLER - SFX_FLAG);
}

void ObjBean_SetupGrowWaterPhase3(ObjBean* this) {
    this->actionFunc = ObjBean_GrowWaterPhase3;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    this->timer = 60;
}

// Fully grown and drops items
void ObjBean_GrowWaterPhase3(ObjBean* this, PlayState* play) {
    s32 i;
    Vec3f itemDropPos;

    this->transformFunc(this);
    if (this->timer == 40) {
        ObjBean_SetupFlattenLeaves(this);
    } else if (this->timer == 30) {
        if (!(this->stateFlags & BEAN_STATE_BEEN_WATERED)) {
            itemDropPos.x = this->dyna.actor.world.pos.x;
            itemDropPos.y = this->dyna.actor.world.pos.y - 25.0f;
            itemDropPos.z = this->dyna.actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_DropCollectible(play, &itemDropPos, ITEM00_FLEXIBLE);
            }
            this->stateFlags |= BEAN_STATE_BEEN_WATERED;
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (this->timer <= 0) {
        ObjBean_SetupGrowWaterPhase4(this);
    }
}

void ObjBean_SetupGrowWaterPhase4(ObjBean* this) {
    this->actionFunc = ObjBean_GrowWaterPhase4;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    ObjBean_SetupGrow(this);
}

// Return control back to the player and start to shrink back down
void ObjBean_GrowWaterPhase4(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    this->stalkSizeMultiplier -= 0.001f;
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y + (s16)(this->stalkSizeMultiplier * 700000.0f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + (this->stalkSizeMultiplier * 800.0f);
    if (this->stalkSizeMultiplier <= 0.0f) {
        this->stalkSizeMultiplier = 0.0f;
        this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
        ObjBean_SetupGrowWaterPhase5(this);
    }
}

void ObjBean_SetupGrowWaterPhase5(ObjBean* this) {
    this->actionFunc = ObjBean_GrowWaterPhase5;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->timer = 30;
}

void ObjBean_GrowWaterPhase5(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    if (this->timer <= 0) {
        func_80B8FF50(this);
        this->dyna.actor.flags &= ~ACTOR_FLAG_4;
    }
}

void ObjBean_SetupWaitForPlayer(ObjBean* this) {
    this->actionFunc = ObjBean_WaitForPlayer;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_PLANT);
}

void ObjBean_WaitForPlayer(ObjBean* this, PlayState* play) {
    if (func_8004356C(&this->dyna)) { // Player is standing on
        ObjBean_SetupFly(this);
        if (play->sceneNum == SCENE_SPOT10) { // Lost woods
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_LOST_WOODS);
        } else {
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_GENERIC);
        }
    }
    ObjBean_UpdatePosition(this);
}

void ObjBean_SetupFly(ObjBean* this) {
    this->actionFunc = ObjBean_Fly;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_PLANT);
    this->dyna.actor.speedXZ = 0.0f;
    this->dyna.actor.flags |= ACTOR_FLAG_4; // Never stop updating
}

void ObjBean_Fly(ObjBean* this, PlayState* play) {
    Camera* camera;

    ObjBean_FollowPath(this, play);
    if (this->currentPointIndex == this->pathCount) {
        ObjBean_SetupPathCount(this, play);
        ObjBean_SetupPath(this, play);
        ObjBean_SetupWaitForStepOff(this);

        this->dyna.actor.flags &= ~ACTOR_FLAG_4; // Never stop updating (disable)
        camera = play->cameraPtrs[MAIN_CAM];

        if ((camera->setting == CAM_SET_BEAN_LOST_WOODS) || (camera->setting == CAM_SET_BEAN_GENERIC)) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }

    } else if (func_8004356C(&this->dyna) != 0) { // Player is on top

        func_8002F974(&this->dyna.actor, NA_SE_PL_PLANT_MOVE - SFX_FLAG);

        if (play->sceneNum == SCENE_SPOT10) {
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_LOST_WOODS);
        } else {
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_BEAN_GENERIC);
        }
    } else if (this->stateFlags & BEAN_STATE_PLAYER_ON_TOP) {
        camera = play->cameraPtrs[MAIN_CAM];

        if ((camera->setting == CAM_SET_BEAN_LOST_WOODS) || (camera->setting == CAM_SET_BEAN_GENERIC)) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }
    }

    ObjBean_UpdatePosition(this);
}

void ObjBean_SetupWaitForStepOff(ObjBean* this) {
    this->actionFunc = ObjBean_WaitForStepOff;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_PLANT);
}

void ObjBean_WaitForStepOff(ObjBean* this, PlayState* play) {
    if (!func_80043590(&this->dyna)) {
        ObjBean_SetupWaitForPlayer(this);
    }
    ObjBean_UpdatePosition(this);
}

void func_80B908EC(ObjBean* this) {
    this->actionFunc = func_80B90918;
    ObjBean_SetDrawMode(this, 0);
}

void func_80B90918(ObjBean* this, PlayState* play) {
    if (!func_8004356C(&this->dyna)) {
        ObjBean_SetupPathCount(this, play);
        ObjBean_SetupPath(this, play);
        ObjBean_Move(this);
        func_80B90970(this);
    }
}

void func_80B90970(ObjBean* this) {
    this->actionFunc = func_80B909B0;
    ObjBean_SetDrawMode(this, 0);
    this->timer = 100;
    func_80B8EDF4(this);
}

void func_80B909B0(ObjBean* this, PlayState* play) {
    if (ObjBean_CheckForHorseTrample(this, play)) {
        this->timer = 100;
    } else if (this->timer <= 0) {
        func_80B909F8(this);
    }
}

void func_80B909F8(ObjBean* this) {
    this->actionFunc = func_80B90A34;
    ObjBean_SetDrawMode(this, BEAN_STATE_DRAW_PLANT);
    this->timer = 30;
}

void func_80B90A34(ObjBean* this, PlayState* play) {
    s32 trampled = ObjBean_CheckForHorseTrample(this, play);

    func_80B8EE24(this);
    if (trampled) {
        func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    if ((this->timer <= 0) && (!trampled)) {
        func_80B8EBC8(this);
        ObjBean_SetupWaitForPlayer(this);
    }
}
void ObjBean_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjBean* this = (ObjBean*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }

    this->actionFunc(this, play);

    if (this->stateFlags & BEAN_STATE_DRAW_PLANT) {
        ObjBean_Move(this);
        if (this->dyna.actor.xzDistToPlayer < 150.0f) {
            this->collider.dim.radius = this->dyna.actor.scale.x * 640.0f + 0.5f;
            Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        ObjBean_FindFloor(this, play);

        this->dyna.actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->dyna.actor.shape.shadowScale = this->dyna.actor.scale.x * 88.0f;

        if (ObjBean_CheckForHorseTrample(this, play)) {
            osSyncPrintf(VT_FGCOL(CYAN));
            // "Horse and bean tree lift collision"
            osSyncPrintf("馬と豆の木リフト衝突！！！\n");
            osSyncPrintf(VT_RST);
            ObjBean_Break(this, play);
            func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
            func_80B908EC(this);
        }
    } else {
        this->dyna.actor.shape.shadowDraw = NULL;
    }
    Actor_SetFocus(&this->dyna.actor, 6.0f);
    if (this->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        if (func_8004356C(&this->dyna)) {
            this->stateFlags |= BEAN_STATE_PLAYER_ON_TOP;
        } else {
            this->stateFlags &= ~BEAN_STATE_PLAYER_ON_TOP;
        }
    }
}

void ObjBean_DrawSoftSoilSpot(ObjBean* this, PlayState* play) {
    Matrix_Translate(this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y, this->dyna.actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->dyna.actor.home.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Gfx_DrawDListOpa(play, gMagicBeanSoftSoilDL);
}

void ObjBean_DrawBeanstalk(ObjBean* this, PlayState* play) {
    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateY(this->dyna.actor.shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Scale(0.1f, this->stalkSizeMultiplier, 0.1f, MTXMODE_APPLY);
    Gfx_DrawDListOpa(play, gMagicBeanStemDL);
}

void ObjBean_Draw(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    if (this->stateFlags & BEAN_STATE_DRAW_SOIL) {
        Gfx_DrawDListOpa(play, gMagicBeanSeedlingDL);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_PLANT) {
        Gfx_DrawDListOpa(play, gMagicBeanPlatformDL);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_LEAVES) {
        ObjBean_DrawSoftSoilSpot(this, play);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_STALK) {
        ObjBean_DrawBeanstalk(this, play);
    }
}
