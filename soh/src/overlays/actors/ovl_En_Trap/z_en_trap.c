/*
 * File: z_en_trap.c
 * Overlay: ovl_En_Trap
 * Description: Metal Spike Trap
 */

#include "z_en_trap.h"
#include "objects/object_trap/object_trap.h"

#define FLAGS ACTOR_FLAG_4

#define BEGIN_MOVE_OUT 65535.0f

#define DIR_FWD 0
#define DIR_LEFT 0x4000
#define DIR_BACK -0x8000
#define DIR_RIGHT -0x4000

// Linear motion
#define vLinearVel upperParams
#define vContinue genericVar2

// Circular motion
#define vAngularVel upperParams
#define vAngularPos genericVar1
#define vRadius genericVar2

// Four-way motion
#define vClosestDirection genericVar1 // relative to spike trap's facing angle if moving out, absolute if moving in
#define vMovementMetric genericVar2

void EnTrap_Init(Actor* thisx, PlayState* play);
void EnTrap_Destroy(Actor* thisx, PlayState* play);
void EnTrap_Update(Actor* thisx, PlayState* play);
void EnTrap_Draw(Actor* thisx, PlayState* play);

const ActorInit En_Trap_InitVars = {
    ACTOR_EN_TRAP,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_TRAP,
    sizeof(EnTrap),
    (ActorFunc)EnTrap_Init,
    (ActorFunc)EnTrap_Destroy,
    (ActorFunc)EnTrap_Update,
    (ActorFunc)EnTrap_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_1 | OC1_TYPE_2,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    { ELEMTYPE_UNK0, { 0x00000000, 0x00, 0x00 }, { 0x00001000, 0x00, 0x00 }, TOUCH_NONE, BUMP_ON, OCELEM_ON },
    { 30, 20, 0, { 0, 0, 0 } },
};

void EnTrap_Init(Actor* thisx, PlayState* play) {
    f32 trapDist;
    f32 trapSpeed;
    s16 zSpeed;
    s16 xSpeed;
    EnTrap* this = (EnTrap*)thisx;
    ColliderCylinder* unused = &this->collider; // required to match

    this->upperParams = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;
    Actor_SetScale(thisx, 0.1f);
    thisx->gravity = -2.0f;
    if (thisx->params & SPIKETRAP_MODE_LINEAR) {
        thisx->speedXZ = this->moveSpeedForwardBack.z = this->upperParams & 0xF;
        Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
    } else if (thisx->params & SPIKETRAP_MODE_CIRCULAR) {
        this->vRadius = (this->upperParams & 0xF) * 40.0f;
        this->vAngularVel = ((this->upperParams & 0xF0) + 0x10) << 5;
        thisx->world.pos.x = thisx->home.pos.x + (Math_SinS(0) * this->vRadius);
        thisx->world.pos.z = thisx->home.pos.z + (Math_CosS(0) * this->vRadius);
    } else { // Four-way motion
        if (this->upperParams != 0) {
            trapDist = (this->upperParams >> 4) * 40;
            trapSpeed = (this->upperParams & 0xF);
        } else {
            trapDist = 200.0f;
            trapSpeed = 10.0f;
            thisx->params = 0xF;
        }
        Actor_UpdateBgCheckInfo(play, thisx, 10.0f, 20.0f, 20.0f, 0x1D);
        thisx->home.pos = thisx->world.pos;
        this->targetPosLeft.x = thisx->world.pos.x + (trapDist * Math_CosS(thisx->world.rot.y));
        this->targetPosLeft.z = thisx->world.pos.z - (trapDist * Math_SinS(thisx->world.rot.y));
        this->targetPosRight.x = thisx->world.pos.x + (trapDist * Math_CosS(thisx->world.rot.y + 0x8000));
        this->targetPosRight.z = thisx->world.pos.z - (trapDist * Math_SinS(thisx->world.rot.y + 0x8000));
        this->targetPosFwd.x = thisx->world.pos.x + (trapDist * Math_SinS(thisx->world.rot.y));
        this->targetPosFwd.z = thisx->world.pos.z + (trapDist * Math_CosS(thisx->world.rot.y));
        this->targetPosBack.x = thisx->world.pos.x + (trapDist * Math_SinS(thisx->world.rot.y + 0x8000));
        this->targetPosBack.z = thisx->world.pos.z + (trapDist * Math_CosS(thisx->world.rot.y + 0x8000));

        zSpeed = trapSpeed * Math_CosS(thisx->world.rot.y);
        xSpeed = trapSpeed * Math_SinS(thisx->world.rot.y);
        zSpeed = ABS(zSpeed);
        xSpeed = ABS(xSpeed);
        this->moveSpeedLeftRight.x = this->moveSpeedForwardBack.z = zSpeed;
        this->moveSpeedLeftRight.z = this->moveSpeedForwardBack.x = xSpeed;
    }
    thisx->focus.pos = thisx->world.pos;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    thisx->targetMode = 3;
    thisx->colChkInfo.mass = 0xFF;
}

void EnTrap_Destroy(Actor* thisx, PlayState* play) {
    EnTrap* this = (EnTrap*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void EnTrap_Update(Actor* thisx, PlayState* play) {
    EnTrap* this = (EnTrap*)thisx;
    Vec3f posTemp;
    s16 angleToKnockPlayer;
    s16 angleToCollidedActor;
    s16 touchingActor;
    s16 blockedOnReturn;
    s32 pad;
    s16 angleToWall;
    Vec3f icePos;
    Vec3f posAhead;
    Vec3f colPoint;         // unused return value from function
    CollisionPoly* colPoly; // unused return value from function
    s32 bgId;               // unused return value from function
    f32 temp_cond;

    touchingActor = false;
    blockedOnReturn = false;
    angleToWall = thisx->wallYaw - thisx->world.rot.y;
    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->collider.base.ocFlags1 &= ~OC1_HIT;
        angleToCollidedActor =
            thisx->world.rot.y + Math_Vec3f_Yaw(&this->collider.base.oc->world.pos, &thisx->world.pos);
        touchingActor = true;
    }
    // Freeze the trap if hit by ice arrows:
    if ((this->collider.base.acFlags & AC_HIT) != 0) {
        icePos = thisx->world.pos;
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetColorFilter(thisx, 0, 250, 0, 250);
        icePos.y += 10.0f;
        icePos.z += 10.0f;
        EffectSsEnIce_SpawnFlyingVec3f(play, thisx, &icePos, 150, 150, 150, 250, 235, 245, 255, 1.8f);
        icePos.x += 10.0f;
        icePos.z -= 20.0f;
        EffectSsEnIce_SpawnFlyingVec3f(play, thisx, &icePos, 150, 150, 150, 250, 235, 245, 255, 1.8f);
        icePos.x -= 20.0f;
        EffectSsEnIce_SpawnFlyingVec3f(play, thisx, &icePos, 150, 150, 150, 250, 235, 245, 255, 1.8f);
    }
    // If not frozen:
    if (thisx->colorFilterTimer == 0) {
        DECR(this->playerDmgTimer);
        // Handles damaging player:
        //! @bug there is no yDistToPlayer check for player being below. Therefore hitbox extends down infinitely
        if ((thisx->xzDistToPlayer <= 40.0f) && (this->playerDmgTimer == 0) && (thisx->yDistToPlayer <= 20.0f)) {
            if (!(thisx->params & (SPIKETRAP_MODE_LINEAR | SPIKETRAP_MODE_CIRCULAR))) { // if in 4-way mode:
                if ((s16)(this->vClosestDirection - thisx->yawTowardsPlayer) >= 0) {
                    angleToKnockPlayer = this->vClosestDirection - 0x4000;
                } else {
                    angleToKnockPlayer = this->vClosestDirection + 0x4000;
                }
            } else {
                angleToKnockPlayer = thisx->yawTowardsPlayer;
            }
            play->damagePlayer(play, -4);
            func_8002F7A0(play, thisx, 6.0f, angleToKnockPlayer, 6.0f);
            this->playerDmgTimer = 15;
        }
        if (thisx->params & SPIKETRAP_MODE_LINEAR) {
            this->vContinue = 1.0f;
            // If physically touching a wall and wall faces towards spike trap
            if ((thisx->bgCheckFlags & 8) && (ABS(angleToWall) >= 0x6000)) {
                this->vContinue = 0.0f;
            }
            // If there is a collision poly between current position and a position 30 units ahead of spike trap
            if (this->vContinue != 0.0f) {
                posAhead.x = (Math_SinS(thisx->world.rot.y) * 30.0f) + thisx->world.pos.x;
                posAhead.z = (Math_CosS(thisx->world.rot.y) * 30.0f) + thisx->world.pos.z;
                posAhead.y = thisx->world.pos.y;
                if (BgCheck_EntityLineTest1(&play->colCtx, &thisx->world.pos, &posAhead, &colPoint, &colPoly, true,
                                            true, false, true, &bgId) == true) {
                    this->vContinue = 0.0f;
                }
            }
            // If spike trap is touching an actor which is in the path of the spike trap
            if (touchingActor && (this->vContinue != 0.0f)) {
                angleToCollidedActor =
                    Math_Vec3f_Yaw(&thisx->world.pos, &this->collider.base.oc->world.pos) - thisx->world.rot.y;
                if (ABS(angleToCollidedActor) < 0x1000) {
                    this->vContinue = 0.0f;
                }
            }
            // If any of the above three conditions are met, turn around
            if (this->vContinue == 0.0f) {
                thisx->world.rot.y += 0x8000;
                Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
            }
        } else if (thisx->params & SPIKETRAP_MODE_CIRCULAR) {
            temp_cond = Math_SinS(this->vAngularPos);
            this->vAngularPos += this->vAngularVel;
            // Every full circle make a sound:
            if ((temp_cond < 0.0f) && (Math_SinS(this->vAngularPos) >= 0.0f)) {
                Audio_PlayActorSound2(thisx, NA_SE_EV_ROUND_TRAP_MOVE);
            }
            thisx->world.pos.x = (this->vRadius * Math_SinS(this->vAngularPos)) + thisx->home.pos.x;
            thisx->world.pos.z = (this->vRadius * Math_CosS(this->vAngularPos)) + thisx->home.pos.z;
            thisx->world.pos.y = thisx->floorHeight;
            thisx->prevPos = thisx->world.pos;
        } else { // 4 way movement
            // if moving outwards:
            if (this->vMovementMetric != 0.0f) {
                switch (this->vClosestDirection) { // movement direction relative to spike trap
                    case DIR_FWD:
                        if (!(thisx->params & SPIKETRAP_FOURWAY_FWD_ALLOWED)) {
                            this->vMovementMetric = 0.0f;
                        } else if ((thisx->bgCheckFlags & 8) && (ABS(angleToWall) > 0x6000)) {
                            this->vMovementMetric = 0.0f;
                        }
                        if (touchingActor && (this->vMovementMetric != 0.0f) && (ABS(angleToCollidedActor) > 0x6000)) {
                            this->vMovementMetric = 0.0f;
                        }
                        if (this->vMovementMetric != 0.0f) {
                            if (this->vMovementMetric == BEGIN_MOVE_OUT) {
                                Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
                            }
                            this->vMovementMetric = Math_SmoothStepToF(&thisx->world.pos.z, this->targetPosFwd.z, 1.0f,
                                                                       this->moveSpeedForwardBack.z, 0.0f);
                            this->vMovementMetric += Math_SmoothStepToF(&thisx->world.pos.x, this->targetPosFwd.x, 1.0f,
                                                                        this->moveSpeedForwardBack.x, 0.0f);
                        }
                        break;
                    case DIR_LEFT:
                        if (!(thisx->params & SPIKETRAP_FOURWAY_LEFT_ALLOWED)) {
                            this->vMovementMetric = 0.0f;
                        } else if ((thisx->bgCheckFlags & 8) && (angleToWall < -0x2000) && (angleToWall > -0x6000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (touchingActor && (this->vMovementMetric != 0.0f) && (angleToCollidedActor <= -0x2000) &&
                            (angleToCollidedActor > -0x6000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (this->vMovementMetric != 0.0f) {
                            if (this->vMovementMetric == BEGIN_MOVE_OUT) {
                                Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
                            }
                            this->vMovementMetric = Math_SmoothStepToF(&thisx->world.pos.x, this->targetPosLeft.x, 1.0f,
                                                                       this->moveSpeedLeftRight.x, 0.0f);
                            this->vMovementMetric += Math_SmoothStepToF(&thisx->world.pos.z, this->targetPosLeft.z,
                                                                        1.0f, this->moveSpeedLeftRight.z, 0.0f);
                        }
                        break;
                    case DIR_BACK:
                        if (!(thisx->params & SPIKETRAP_FOURWAY_BACK_ALLOWED)) {
                            this->vMovementMetric = 0.0f;
                        } else if ((thisx->bgCheckFlags & 8) && (ABS(angleToWall) < 0x2000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (touchingActor && (this->vMovementMetric != 0.0f) && (ABS(angleToCollidedActor) < 0x2000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (this->vMovementMetric != 0.0f) {
                            if (this->vMovementMetric == BEGIN_MOVE_OUT) {
                                Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
                            }
                            this->vMovementMetric = Math_SmoothStepToF(&thisx->world.pos.z, this->targetPosBack.z, 1.0f,
                                                                       this->moveSpeedForwardBack.z, 0.0f);
                            this->vMovementMetric += Math_SmoothStepToF(&thisx->world.pos.x, this->targetPosBack.x,
                                                                        1.0f, this->moveSpeedForwardBack.x, 0.0f);
                        }
                        break;
                    case DIR_RIGHT:
                        if (!(thisx->params & SPIKETRAP_FOURWAY_RIGHT_ALLOWED)) {
                            this->vMovementMetric = 0.0f;
                        } else if ((thisx->bgCheckFlags & 8) && (angleToWall > 0x2000) && (angleToWall < 0x6000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (touchingActor && (this->vMovementMetric != 0.0f) && (angleToCollidedActor > 0x2000) &&
                            (angleToCollidedActor < 0x6000)) {
                            this->vMovementMetric = 0.0f;
                            break;
                        }
                        if (this->vMovementMetric != 0.0f) {
                            if (this->vMovementMetric == BEGIN_MOVE_OUT) {
                                Audio_PlayActorSound2(thisx, NA_SE_EV_SPINE_TRAP_MOVE);
                            }
                            this->vMovementMetric = Math_SmoothStepToF(&thisx->world.pos.x, this->targetPosRight.x,
                                                                       1.0f, this->moveSpeedLeftRight.x, 0.0f);
                            this->vMovementMetric += Math_SmoothStepToF(&thisx->world.pos.z, this->targetPosRight.z,
                                                                        1.0f, this->moveSpeedLeftRight.z, 0.0f);
                        }
                        break;
                }
                if (!Actor_TestFloorInDirection(thisx, play, 50.0f, this->vClosestDirection)) {
                    this->vMovementMetric = 0.0f;
                }
                // if in initial position:
            } else if ((thisx->world.pos.x == thisx->home.pos.x) && (thisx->world.pos.z == thisx->home.pos.z)) {
                // of the available 4-way directions, get the one which is closest to the direction of player:
                this->vClosestDirection = ((thisx->yawTowardsPlayer - thisx->world.rot.y) + 0x2000) & 0xC000;
                this->vMovementMetric = 0.0f;
                if (thisx->xzDistToPlayer < 200.0f) {
                    this->vMovementMetric = BEGIN_MOVE_OUT;
                }
                // If returning to origin:
            } else {
                // Of the four real world compass directions, get the one which is closest to the movement direction of
                // the returning spike. Note that this is different from the previous usages of vClosestDirection
                this->vClosestDirection = (Math_Vec3f_Yaw(&thisx->world.pos, &thisx->home.pos) + 0x2000) & 0xC000;
                switch (this->vClosestDirection) {
                    case 0: // movement is closest to +z direction
                        if (thisx->bgCheckFlags & 8) {
                            if (ABS(thisx->wallYaw) > 0x6000) {
                                blockedOnReturn = true;
                            }
                        } else if (touchingActor && (ABS(angleToCollidedActor) > 0x6000)) {
                            blockedOnReturn = true;
                        }
                        break;
                    case 0x4000: // movement is closest to +x direction
                        if (thisx->bgCheckFlags & 8) {
                            if ((thisx->wallYaw < -0x2000) && (thisx->wallYaw > -0x6000)) {
                                blockedOnReturn = true;
                            }
                        } else if (touchingActor && (angleToCollidedActor < -0x2000) &&
                                   (angleToCollidedActor > -0x6000)) {
                            blockedOnReturn = true;
                        }
                        break;
                    case -0x8000: // movement is closest to -z direction
                        if (thisx->bgCheckFlags & 8) {
                            if (ABS(thisx->wallYaw) < 0x2000) {
                                blockedOnReturn = true;
                            }
                        } else if (touchingActor && (ABS(angleToCollidedActor) < 0x2000)) {
                            blockedOnReturn = true;
                        }
                        break;
                    case -0x4000: // movement is closest to -x direction
                        if (thisx->bgCheckFlags & 8) {
                            if ((thisx->wallYaw > 0x2000) && (thisx->wallYaw < 0x6000)) {
                                blockedOnReturn = true;
                            }
                        } else if (touchingActor && (angleToCollidedActor > 0x2000) &&
                                   (angleToCollidedActor < 0x6000)) {
                            blockedOnReturn = true;
                        }
                        break;
                }
                if (!blockedOnReturn) {
                    Math_SmoothStepToF(&thisx->world.pos.x, thisx->home.pos.x, 1.0f, 3.0f, 0.0f);
                    Math_SmoothStepToF(&thisx->world.pos.z, thisx->home.pos.z, 1.0f, 3.0f, 0.0f);
                }
            }
        }
        Actor_MoveForward(thisx); // Only used by straight line logic
        // Adjust position using bgcheck, but do not adjust x, z position if in straight line mode:
        if (thisx->params & SPIKETRAP_MODE_LINEAR) {
            posTemp = thisx->world.pos;
        }
        Actor_UpdateBgCheckInfo(play, thisx, 25.0f, 20.0f, 20.0f, 0x1D);
        if (thisx->params & SPIKETRAP_MODE_LINEAR) {
            thisx->world.pos.x = posTemp.x;
            thisx->world.pos.z = posTemp.z;
        }
    }
    Collider_UpdateCylinder(thisx, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (thisx->colorFilterTimer == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnTrap_Draw(Actor* thisx, PlayState* play) {
    func_8002EBCC(thisx, play, 1);
    Gfx_DrawDListOpa(play, gSlidingBladeTrapDL);
}
