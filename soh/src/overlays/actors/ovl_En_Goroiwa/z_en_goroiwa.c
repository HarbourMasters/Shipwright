/*
 * File: z_en_goroiwa.c
 * Overlay: ovl_En_Goroiwa
 * Description: Rolling boulders
 */

#include "z_en_goroiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_goroiwa/object_goroiwa.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

typedef s32 (*EnGoroiwaUnkFunc1)(EnGoroiwa* this, PlayState* play);
typedef void (*EnGoroiwaUnkFunc2)(EnGoroiwa* this);

#define ENGOROIWA_ENABLE_AT (1 << 0)
#define ENGOROIWA_ENABLE_OC (1 << 1)
#define ENGOROIWA_PLAYER_IN_THE_WAY (1 << 2)
#define ENGOROIWA_RETAIN_ROT_SPEED (1 << 3)
#define ENGOROIWA_IN_WATER (1 << 4)

#define ENGOROIWA_LOOPMODE_ONEWAY 0
/* same as ENGOROIWA_LOOPMODE_ONEWAY but display rock fragments as if the boulder broke at the end of the path*/
#define ENGOROIWA_LOOPMODE_ONEWAY_BREAK 1
#define ENGOROIWA_LOOPMODE_ROUNDTRIP 3

void EnGoroiwa_Init(Actor* thisx, PlayState* play);
void EnGoroiwa_Destroy(Actor* thisx, PlayState* play);
void EnGoroiwa_Update(Actor* thisx, PlayState* play);
void EnGoroiwa_Draw(Actor* thisx, PlayState* play);

void EnGoroiwa_SetupRoll(EnGoroiwa* this);
void EnGoroiwa_Roll(EnGoroiwa* this, PlayState* play);
void EnGoroiwa_SetupMoveAndFallToGround(EnGoroiwa* this);
void EnGoroiwa_MoveAndFallToGround(EnGoroiwa* this, PlayState* play);
void EnGoroiwa_SetupWait(EnGoroiwa* this);
void EnGoroiwa_Wait(EnGoroiwa* this, PlayState* play);
void EnGoroiwa_SetupMoveUp(EnGoroiwa* this);
void EnGoroiwa_MoveUp(EnGoroiwa* this, PlayState* play);
void EnGoroiwa_SetupMoveDown(EnGoroiwa* this);
void EnGoroiwa_MoveDown(EnGoroiwa* this, PlayState* play);

const ActorInit En_Goroiwa_InitVars = {
    ACTOR_EN_GOROIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GOROIWA,
    sizeof(EnGoroiwa),
    (ActorFunc)EnGoroiwa_Init,
    (ActorFunc)EnGoroiwa_Destroy,
    (ActorFunc)EnGoroiwa_Update,
    (ActorFunc)EnGoroiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_HEAVY };

static f32 sUnused[] = { 10.0f, 9.2f };

void EnGoroiwa_UpdateCollider(EnGoroiwa* this) {
    static f32 yOffsets[] = { 0.0f, 59.5f };
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + yOffsets[(this->actor.params >> 10) & 1];
    worldSphere->center.z = this->actor.world.pos.z;
}

void EnGoroiwa_InitCollider(EnGoroiwa* this, PlayState* play) {
    s32 pad;

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    EnGoroiwa_UpdateCollider(this);
    this->collider.elements[0].dim.worldSphere.radius = 58;
}

void EnGoroiwa_UpdateFlags(EnGoroiwa* this, u8 setFlags) {
    this->stateFlags &= ~(ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->stateFlags |= setFlags;
}

s32 EnGoroiwa_Vec3fNormalize(Vec3f* ret, Vec3f* a) {
    f32 magnitude = Math3D_Vec3fMagnitude(a);
    f32 scale;

    if (magnitude < 0.001f) {
        return false;
    }

    scale = 1.0f / magnitude;

    ret->x = a->x * scale;
    ret->y = a->y * scale;
    ret->z = a->z * scale;

    return true;
}

void EnGoroiwa_SetSpeed(EnGoroiwa* this, PlayState* play) {
    if (play->sceneNum == SCENE_KOKIRI_FOREST) {
        this->isInKokiri = true;
        R_EN_GOROIWA_SPEED = 920;
    } else {
        this->isInKokiri = false;
        R_EN_GOROIWA_SPEED = 1000;
    }
}

void EnGoroiwa_FaceNextWaypoint(EnGoroiwa* this, PlayState* play) {
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    Vec3s* nextPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3f nextPosF;

    nextPosF.x = nextPos->x;
    nextPosF.y = nextPos->y;
    nextPosF.z = nextPos->z;

    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &nextPosF);
}

void EnGoroiwa_GetPrevWaypointDiff(EnGoroiwa* this, PlayState* play, Vec3f* dest) {
    s16 loopMode = (this->actor.params >> 8) & 3;
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    s16 prevWaypoint = this->currentWaypoint - this->pathDirection;
    Vec3s* prevPointPos;
    Vec3s* currentPointPos;

    if (prevWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = this->endWaypoint;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = 1;
        }
    } else if (prevWaypoint > this->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            prevWaypoint = 0;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            prevWaypoint = this->endWaypoint - 1;
        }
    }

    currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;
    prevPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + prevWaypoint;
    dest->x = currentPointPos->x - prevPointPos->x;
    dest->y = currentPointPos->x - prevPointPos->y;
    dest->z = currentPointPos->x - prevPointPos->z;
}

void EnGoroiw_CheckEndOfPath(EnGoroiwa* this) {
    s16 loopMode = (this->actor.params >> 8) & 3;

    if (this->nextWaypoint < 0) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            this->currentWaypoint = this->endWaypoint;
            this->nextWaypoint = this->endWaypoint - 1;
            this->pathDirection = -1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            this->currentWaypoint = 0;
            this->nextWaypoint = 1;
            this->pathDirection = 1;
        }
    } else if (this->nextWaypoint > this->endWaypoint) {
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
            this->currentWaypoint = 0;
            this->nextWaypoint = 1;
            this->pathDirection = 1;
        } else if (loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) {
            this->currentWaypoint = this->endWaypoint;
            this->nextWaypoint = this->endWaypoint - 1;
            this->pathDirection = -1;
        }
    }
}

void EnGoroiwa_SetNextWaypoint(EnGoroiwa* this) {
    this->currentWaypoint = this->nextWaypoint;
    this->nextWaypoint += this->pathDirection;
    EnGoroiw_CheckEndOfPath(this);
}

void EnGoroiwa_ReverseDirection(EnGoroiwa* this) {
    this->pathDirection *= -1;
    this->currentWaypoint = this->nextWaypoint;
    this->nextWaypoint += this->pathDirection;
}

void EnGoroiwa_InitPath(EnGoroiwa* this, PlayState* play) {
    this->endWaypoint = play->setupPathList [this->actor.params & 0xFF].count - 1;
    this->currentWaypoint = 0;
    this->nextWaypoint = 1;
    this->pathDirection = 1;
}

void EnGoroiwa_TeleportToWaypoint(EnGoroiwa* this, PlayState* play, s32 waypoint) {
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    Vec3s* pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + waypoint;

    this->actor.world.pos.x = pointPos->x;
    this->actor.world.pos.y = pointPos->y;
    this->actor.world.pos.z = pointPos->z;
}

void EnGoroiwa_InitRotation(EnGoroiwa* this) {
    this->prevUnitRollAxis.x = 1.0f;
    this->rollRotSpeed = 1.0f;
}

s32 EnGoroiwa_GetAscendDirection(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;

    if (nextPointPos->x == currentPointPos->x && nextPointPos->z == currentPointPos->z) {
        if (nextPointPos->y == currentPointPos->y) {
            // "Error: Invalid path data (points overlap)"
            osSyncPrintf("Error : レールデータ不正(点が重なっている)");
            osSyncPrintf("(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__, this->actor.params);
        }

        if (nextPointPos->y > currentPointPos->y) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

void EnGoroiwa_SpawnDust(PlayState* play, Vec3f* pos) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f randPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 8; i++) {
        angle += 0x4E20;
        randPos.x = pos->x + (47.0f * (Rand_ZeroOne() * 0.5f + 0.5f)) * Math_SinS(angle);
        randPos.y = pos->y + (Rand_ZeroOne() - 0.5f) * 40.0f;
        randPos.z = pos->z + ((47.0f * (Rand_ZeroOne() * 0.5f + 0.5f))) * Math_CosS(angle);
        func_800286CC(play, &randPos, &velocity, &accel, (s16)(Rand_ZeroOne() * 30.0f) + 100, 80);
        func_800286CC(play, &randPos, &velocity, &accel, (s16)(Rand_ZeroOne() * 20.0f) + 80, 80);
    }
}

void EnGoroiwa_SpawnWaterEffects(PlayState* play, Vec3f* contactPos) {
    Vec3f splashPos;
    s32 i;
    s16 angle = 0;

    for (i = 0; i < 11; i++) {
        angle += 0x1746;
        splashPos.x = contactPos->x + (Math_SinS(angle) * 55.0f);
        splashPos.y = contactPos->y;
        splashPos.z = contactPos->z + (Math_CosS(angle) * 55.0f);
        EffectSsGSplash_Spawn(play, &splashPos, 0, 0, 0, 350);
    }

    EffectSsGRipple_Spawn(play, contactPos, 300, 700, 0);
    EffectSsGRipple_Spawn(play, contactPos, 500, 900, 4);
    EffectSsGRipple_Spawn(play, contactPos, 500, 1300, 8);
}

s32 EnGoroiwa_MoveAndFall(EnGoroiwa* this, PlayState* play) {
    Path* path;
    s32 result;
    s32 pad;
    Vec3s* nextPointPos;

    Math_StepToF(&this->actor.speedXZ, R_EN_GOROIWA_SPEED * 0.01f, 0.3f);
    func_8002D868(&this->actor);
    path = &play->setupPathList[this->actor.params & 0xFF];
    nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    result = true;
    result &= Math_StepToF(&this->actor.world.pos.x, nextPointPos->x, fabsf(this->actor.velocity.x));
    result &= Math_StepToF(&this->actor.world.pos.z, nextPointPos->z, fabsf(this->actor.velocity.z));
    this->actor.world.pos.y += this->actor.velocity.y;
    return result;
}

s32 EnGoroiwa_Move(EnGoroiwa* this, PlayState* play) {
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    s32 pad;
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    Vec3s* currentPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->currentWaypoint;
    s32 nextPointReached;
    Vec3f posDiff;
    Vec3f nextPointPosF;

    nextPointPosF.x = nextPointPos->x;
    nextPointPosF.y = nextPointPos->y;
    nextPointPosF.z = nextPointPos->z;
    Math_StepToF(&this->actor.speedXZ, R_EN_GOROIWA_SPEED * 0.01f, 0.3f);
    if (Math3D_Vec3fDistSq(&nextPointPosF, &this->actor.world.pos) < SQ(5.0f)) {
        Math_Vec3f_Diff(&nextPointPosF, &this->actor.world.pos, &posDiff);
    } else {
        posDiff.x = nextPointPosF.x - currentPointPos->x;
        posDiff.y = nextPointPosF.y - currentPointPos->y;
        posDiff.z = nextPointPosF.z - currentPointPos->z;
    }
    EnGoroiwa_Vec3fNormalize(&this->actor.velocity, &posDiff);
    this->actor.velocity.x *= this->actor.speedXZ;
    this->actor.velocity.y *= this->actor.speedXZ;
    this->actor.velocity.z *= this->actor.speedXZ;
    nextPointReached = true;
    nextPointReached &= Math_StepToF(&this->actor.world.pos.x, nextPointPosF.x, fabsf(this->actor.velocity.x));
    nextPointReached &= Math_StepToF(&this->actor.world.pos.y, nextPointPosF.y, fabsf(this->actor.velocity.y));
    nextPointReached &= Math_StepToF(&this->actor.world.pos.z, nextPointPosF.z, fabsf(this->actor.velocity.z));
    return nextPointReached;
}

s32 EnGoroiwa_MoveUpToNextWaypoint(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;

    Math_StepToF(&this->actor.velocity.y, (R_EN_GOROIWA_SPEED * 0.01f) * 0.5f, 0.18f);
    this->actor.world.pos.x = nextPointPos->x;
    this->actor.world.pos.z = nextPointPos->z;
    return Math_StepToF(&this->actor.world.pos.y, nextPointPos->y, fabsf(this->actor.velocity.y));
}

s32 EnGoroiwa_MoveDownToNextWaypoint(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    Path* path = &play->setupPathList[this->actor.params & 0xFF];
    Vec3s* nextPointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->nextWaypoint;
    f32 nextPointY;
    f32 thisY;
    f32 yDistToFloor;
    s32 quakeIdx;
    CollisionPoly* floorPoly;
    Vec3f raycastFrom;
    f32 floorY;
    s32 pad2;
    s32 floorBgId;
    Vec3f dustPos;
    WaterBox* waterBox;
    f32 ySurface;
    Vec3f waterHitPos;

    nextPointY = nextPointPos->y;
    Math_StepToF(&this->actor.velocity.y, -14.0f, 1.0f);
    this->actor.world.pos.x = nextPointPos->x;
    this->actor.world.pos.z = nextPointPos->z;
    thisY = this->actor.world.pos.y;
    this->actor.world.pos.y += this->actor.velocity.y;
    if (this->actor.velocity.y < 0.0f && this->actor.world.pos.y <= nextPointY) {
        if (this->bounceCount == 0) {
            if (this->actor.xzDistToPlayer < 600.0f) {
                quakeIdx = Quake_Add(GET_ACTIVE_CAM(play), 3);
                Quake_SetSpeed(quakeIdx, -0x3CB0);
                Quake_SetQuakeValues(quakeIdx, 3, 0, 0, 0);
                Quake_SetCountdown(quakeIdx, 7);
            }
            this->rollRotSpeed = 0.0f;
            if (!(this->stateFlags & ENGOROIWA_IN_WATER)) {
                raycastFrom.x = this->actor.world.pos.x;
                raycastFrom.y = this->actor.world.pos.y + 50.0f;
                raycastFrom.z = this->actor.world.pos.z;
                floorY = BgCheck_EntityRaycastFloor5(play, &play->colCtx, &floorPoly, &floorBgId,
                                                     &this->actor, &raycastFrom);
                yDistToFloor = floorY - (this->actor.world.pos.y - 59.5f);
                if (fabsf(yDistToFloor) < 15.0f) {
                    dustPos.x = this->actor.world.pos.x;
                    dustPos.y = floorY + 10.0f;
                    dustPos.z = this->actor.world.pos.z;
                    EnGoroiwa_SpawnDust(play, &dustPos);
                }
            }
        }
        if (this->bounceCount >= 1) {
            return true;
        }
        this->bounceCount++;
        this->actor.velocity.y *= -0.3f;
        this->actor.world.pos.y = nextPointY - ((this->actor.world.pos.y - nextPointY) * 0.3f);
    }
    if (this->bounceCount == 0 &&
        WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                &ySurface, &waterBox) &&
        this->actor.world.pos.y <= ySurface) {
        this->stateFlags |= ENGOROIWA_IN_WATER;
        if (ySurface < thisY) {
            waterHitPos.x = this->actor.world.pos.x;
            waterHitPos.y = ySurface;
            waterHitPos.z = this->actor.world.pos.z;
            EnGoroiwa_SpawnWaterEffects(play, &waterHitPos);
            this->actor.velocity.y *= 0.2f;
        }
        if (this->actor.velocity.y < -8.0f) {
            this->actor.velocity.y = -8.0f;
        }
    }
    return false;
}

void EnGoroiwa_UpdateRotation(EnGoroiwa* this, PlayState* play) {
    static Vec3f unitY = { 0.0f, 1.0f, 0.0f };
    s32 pad;
    Vec3f* rollAxisPtr;
    f32 rollAngleDiff;
    Vec3f rollAxis;
    Vec3f unitRollAxis;
    MtxF mtx;
    Vec3f unusedDiff;

    if (this->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        rollAngleDiff = this->prevRollAngleDiff;
    } else {
        this->prevRollAngleDiff = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.prevPos) * (1.0f / 59.5f);
        rollAngleDiff = this->prevRollAngleDiff;
    }
    rollAngleDiff *= this->rollRotSpeed;
    rollAxisPtr = &rollAxis;
    if (this->stateFlags & ENGOROIWA_RETAIN_ROT_SPEED) {
        /*
         * EnGoroiwa_GetPrevWaypointDiff has no side effects and its result goes unused,
         * its result was probably meant to be used instead of the actor's velocity in the
         * Math3D_Vec3f_Cross call.
         */
        EnGoroiwa_GetPrevWaypointDiff(this, play, &unusedDiff);
        Math3D_Vec3f_Cross(&unitY, &this->actor.velocity, rollAxisPtr);
    } else {
        Math3D_Vec3f_Cross(&unitY, &this->actor.velocity, rollAxisPtr);
    }

    if (EnGoroiwa_Vec3fNormalize(&unitRollAxis, rollAxisPtr)) {
        this->prevUnitRollAxis = unitRollAxis;
    } else {
        unitRollAxis = this->prevUnitRollAxis;
    }

    Matrix_RotateAxis(rollAngleDiff, &unitRollAxis, MTXMODE_NEW);
    Matrix_RotateY(this->actor.shape.rot.y * (2.0f * M_PI / 0x10000), MTXMODE_APPLY);
    Matrix_RotateX(this->actor.shape.rot.x * (2.0f * M_PI / 0x10000), MTXMODE_APPLY);
    Matrix_RotateZ(this->actor.shape.rot.z * (2.0f * M_PI / 0x10000), MTXMODE_APPLY);
    Matrix_Get(&mtx);
    Matrix_MtxFToYXZRotS(&mtx, &this->actor.shape.rot, 0);
}

void EnGoroiwa_NextWaypoint(EnGoroiwa* this, PlayState* play) {
    s16 loopMode = (this->actor.params >> 8) & 3;

    EnGoroiwa_SetNextWaypoint(this);

    if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY || loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK) {
        if (this->currentWaypoint == 0 || this->currentWaypoint == this->endWaypoint) {
            EnGoroiwa_TeleportToWaypoint(this, play, this->currentWaypoint);
        }
    }

    EnGoroiwa_FaceNextWaypoint(this, play);
}

void EnGoroiwa_SpawnFragments(EnGoroiwa* this, PlayState* play) {
    static f32 yOffsets[] = { 0.0f, 59.5f };
    s16 angle1;
    s16 angle2;
    s32 pad;
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f effectPos;
    Vec3f fragmentVelocity;
    f32 cos1;
    f32 sin1;
    f32 sin2;
    s16 yOffsetIdx = (this->actor.params >> 10) & 1;
    s32 i;

    for (i = 0, angle1 = 0; i < 16; i++, angle1 += 0x4E20) {
        sin1 = Math_SinS(angle1);
        cos1 = Math_CosS(angle1);
        angle2 = Rand_ZeroOne() * 0xFFFF;
        effectPos.x = Rand_ZeroOne() * 50.0f * sin1 * Math_SinS(angle2);
        sin2 = Math_SinS(angle2);
        effectPos.y = (Rand_ZeroOne() - 0.5f) * 100.0f * sin2 + yOffsets[yOffsetIdx];
        effectPos.z = Rand_ZeroOne() * 50.0f * cos1 * Math_SinS(angle2);
        fragmentVelocity.x = effectPos.x * 0.2f;
        fragmentVelocity.y = Rand_ZeroOne() * 15.0f + 2.0f;
        fragmentVelocity.z = effectPos.z * 0.2f;
        Math_Vec3f_Sum(&effectPos, thisPos, &effectPos);
        EffectSsKakera_Spawn(play, &effectPos, &fragmentVelocity, &effectPos, -340, 33, 28, 2, 0,
                             Rand_ZeroOne() * 7.0f + 1.0f, 1, 0, 70, KAKERA_COLOR_NONE, 1, gBoulderFragmentsDL);
    }

    effectPos.x = thisPos->x;
    effectPos.y = thisPos->y + yOffsets[yOffsetIdx];
    effectPos.z = thisPos->z;
    func_80033480(play, &effectPos, 80.0f, 5, 70, 110, 1);
    func_80033480(play, &effectPos, 90.0f, 5, 110, 160, 1);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -860, ICHAIN_CONTINUE), ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneForward, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 150, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

void EnGoroiwa_Init(Actor* thisx, PlayState* play) {
    static f32 yOffsets[] = { 0.0f, 595.0f };
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    s32 pathIdx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnGoroiwa_InitCollider(this, play);
    pathIdx = this->actor.params & 0xFF;
    if (pathIdx == 0xFF) {
        // "Error: Invalid arg_data"
        osSyncPrintf("Ｅｒｒｏｒ : arg_data が不正(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->actor.params);
        Actor_Kill(&this->actor);
        return;
    }
    if (play->setupPathList[pathIdx].count < 2) {
        // "Error: Invalid Path Data"
        osSyncPrintf("Ｅｒｒｏｒ : レールデータ が不正(%s %d)\n", __FILE__, __LINE__);
        Actor_Kill(&this->actor);
        return;
    }
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, yOffsets[(this->actor.params >> 10) & 1], ActorShadow_DrawCircle, 9.4f);
    this->actor.shape.shadowAlpha = 200;
    EnGoroiwa_SetSpeed(this, play);
    EnGoroiwa_InitPath(this, play);
    EnGoroiwa_TeleportToWaypoint(this, play, 0);
    EnGoroiwa_InitRotation(this);
    EnGoroiwa_FaceNextWaypoint(this, play);
    EnGoroiwa_SetupRoll(this);
    // "(Goroiwa)"
    osSyncPrintf("(ごろ岩)(arg 0x%04x)(rail %d)(end %d)(bgc %d)(hit %d)\n", this->actor.params,
                 this->actor.params & 0xFF, (this->actor.params >> 8) & 3, (this->actor.params >> 10) & 1,
                 this->actor.home.rot.z & 1);
}

void EnGoroiwa_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGoroiwa* this = (EnGoroiwa*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnGoroiwa_SetupRoll(EnGoroiwa* this) {
    this->actionFunc = EnGoroiwa_Roll;
    EnGoroiwa_UpdateFlags(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 1.0f;
}

void EnGoroiwa_Roll(EnGoroiwa* this, PlayState* play) {
    static EnGoroiwaUnkFunc1 moveFuncs[] = { EnGoroiwa_Move, EnGoroiwa_MoveAndFall };
    static EnGoroiwaUnkFunc2 onHitSetupFuncs[] = { EnGoroiwa_SetupWait, EnGoroiwa_SetupMoveAndFallToGround };

    s32 ascendDirection;
    s16 yawDiff;
    s16 loopMode;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->stateFlags &= ~ENGOROIWA_PLAYER_IN_THE_WAY;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (yawDiff > -0x4000 && yawDiff < 0x4000) {
            this->stateFlags |= ENGOROIWA_PLAYER_IN_THE_WAY;
            if (((this->actor.params >> 10) & 1) || (this->actor.home.rot.z & 1) != 1) {
                EnGoroiwa_ReverseDirection(this);
                EnGoroiwa_FaceNextWaypoint(this, play);
            }
        }
        func_8002F6D4(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
        osSyncPrintf(VT_FGCOL(CYAN));
        osSyncPrintf("Player ぶっ飛ばし\n"); // "Player knocked down"
        osSyncPrintf(VT_RST);
        onHitSetupFuncs[(this->actor.params >> 10) & 1](this);
        Player_PlaySfx(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (moveFuncs[(this->actor.params >> 10) & 1](this, play)) {
        loopMode = (this->actor.params >> 8) & 3;
        if (loopMode == ENGOROIWA_LOOPMODE_ONEWAY_BREAK &&
            (this->nextWaypoint == 0 || this->nextWaypoint == this->endWaypoint)) {
            EnGoroiwa_SpawnFragments(this, play);
        }
        EnGoroiwa_NextWaypoint(this, play);
        if ((loopMode == ENGOROIWA_LOOPMODE_ROUNDTRIP) &&
            (this->currentWaypoint == 0 || this->currentWaypoint == this->endWaypoint)) {
            EnGoroiwa_SetupWait(this);
        } else if (!((this->actor.params >> 10) & 1) && this->currentWaypoint != 0 &&
                   this->currentWaypoint != this->endWaypoint) {
            ascendDirection = EnGoroiwa_GetAscendDirection(this, play);
            if (ascendDirection > 0) {
                EnGoroiwa_SetupMoveUp(this);
            } else if (ascendDirection < 0) {
                EnGoroiwa_SetupMoveDown(this);
            } else {
                EnGoroiwa_SetupRoll(this);
            }
        } else {
            EnGoroiwa_SetupRoll(this);
        }
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

void EnGoroiwa_SetupMoveAndFallToGround(EnGoroiwa* this) {
    this->actionFunc = EnGoroiwa_MoveAndFallToGround;
    EnGoroiwa_UpdateFlags(this, ENGOROIWA_ENABLE_OC);
    this->actor.gravity = -0.86f;
    this->actor.minVelocityY = -15.0f;
    this->actor.speedXZ *= 0.15f;
    this->actor.velocity.y = 5.0f;
    this->rollRotSpeed = 1.0f;
}

void EnGoroiwa_MoveAndFallToGround(EnGoroiwa* this, PlayState* play) {
    EnGoroiwa_MoveAndFall(this, play);
    if ((this->actor.bgCheckFlags & 1) && this->actor.velocity.y < 0.0f) {
        if ((this->stateFlags & ENGOROIWA_PLAYER_IN_THE_WAY) && (this->actor.home.rot.z & 1) == 1) {
            EnGoroiwa_ReverseDirection(this);
            EnGoroiwa_FaceNextWaypoint(this, play);
        }
        EnGoroiwa_SetupWait(this);
    }
}

void EnGoroiwa_SetupWait(EnGoroiwa* this) {
    static s16 waitDurations[] = { 20, 6 };

    this->actionFunc = EnGoroiwa_Wait;
    this->actor.speedXZ = 0.0f;
    EnGoroiwa_UpdateFlags(this, ENGOROIWA_ENABLE_OC);
    this->waitTimer = waitDurations[this->actor.home.rot.z & 1];
    this->rollRotSpeed = 0.0f;
}

void EnGoroiwa_Wait(EnGoroiwa* this, PlayState* play) {
    if (this->waitTimer > 0) {
        this->waitTimer--;
    } else {
        this->collider.base.atFlags &= ~AT_HIT;
        EnGoroiwa_SetupRoll(this);
    }
}

void EnGoroiwa_SetupMoveUp(EnGoroiwa* this) {
    this->actionFunc = EnGoroiwa_MoveUp;
    EnGoroiwa_UpdateFlags(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 0.0f;
    this->actor.velocity.y = fabsf(this->actor.speedXZ) * 0.1f;
}

void EnGoroiwa_MoveUp(EnGoroiwa* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_8002F6D4(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 4);
        Player_PlaySfx(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (EnGoroiwa_MoveUpToNextWaypoint(this, play)) {
        EnGoroiwa_NextWaypoint(this, play);
        EnGoroiwa_SetupRoll(this);
        this->actor.speedXZ = 0.0f;
    }
}

void EnGoroiwa_SetupMoveDown(EnGoroiwa* this) {
    this->actionFunc = EnGoroiwa_MoveDown;
    EnGoroiwa_UpdateFlags(this, ENGOROIWA_ENABLE_AT | ENGOROIWA_ENABLE_OC);
    this->rollRotSpeed = 0.3f;
    this->bounceCount = 0;
    this->actor.velocity.y = fabsf(this->actor.speedXZ) * -0.3f;
    this->stateFlags |= ENGOROIWA_RETAIN_ROT_SPEED;
    this->stateFlags &= ~ENGOROIWA_IN_WATER;
}

void EnGoroiwa_MoveDown(EnGoroiwa* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_8002F6D4(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 4);
        Player_PlaySfx(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
        if ((this->actor.home.rot.z & 1) == 1) {
            this->collisionDisabledTimer = 50;
        }
    } else if (EnGoroiwa_MoveDownToNextWaypoint(this, play)) {
        EnGoroiwa_NextWaypoint(this, play);
        EnGoroiwa_SetupRoll(this);
        this->stateFlags &= ~ENGOROIWA_RETAIN_ROT_SPEED;
        this->actor.speedXZ = 0.0f;
    }
}

void EnGoroiwa_Update(Actor* thisx, PlayState* play) {
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;
    s32 sp30;

    if (!(player->stateFlags1 & 0x300000C0)) {
        if (this->collisionDisabledTimer > 0) {
            this->collisionDisabledTimer--;
        }
        this->actionFunc(this, play);
        switch ((this->actor.params >> 10) & 1) {
            case 1:
                Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 0x1C);
                break;
            case 0:
                this->actor.floorHeight = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &sp30,
                                                                      &this->actor, &this->actor.world.pos);
                break;
        }
        EnGoroiwa_UpdateRotation(this, play);
        if (this->actor.xzDistToPlayer < 300.0f) {
            EnGoroiwa_UpdateCollider(this);
            if ((this->stateFlags & ENGOROIWA_ENABLE_AT) && this->collisionDisabledTimer <= 0) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            }
            if ((this->stateFlags & ENGOROIWA_ENABLE_OC) && this->collisionDisabledTimer <= 0) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void EnGoroiwa_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gRollingRockDL);
}
