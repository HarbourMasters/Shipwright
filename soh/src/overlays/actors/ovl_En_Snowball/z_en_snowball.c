/*
 * File: z_en_snowball.c
 * Overlay: ovl_En_Snowball
 * Description: Rollable Snowball
 */

#include "z_en_snowball.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_goroiwa/object_goroiwa.h"
#include "soh_assets.h"

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

void EnSnowball_Init(Actor* thisx, PlayState* play);
void EnSnowball_Destroy(Actor* thisx, PlayState* play);
void EnSnowball_Update(Actor* thisx, PlayState* play);
void EnSnowball_Draw(Actor* thisx, PlayState* play);

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
        { 0, { { 0, 0, 0 }, 14 }, 100 },
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

static CollisionCheckInfoInit sColChkInfoInit = { 0, 3, 15, MASS_HEAVY };

void EnSnowball_UpdateCollider(EnSnowball* this) {
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + (this->actor.scale.x * 500.0f);
    worldSphere->center.z = this->actor.world.pos.z;
    worldSphere->radius = (this->actor.scale.x * 500.0f);
}

void EnSnowball_InitCollider(EnSnowball* this, PlayState* play) {
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    EnSnowball_UpdateCollider(this);
    this->collider.elements[0].dim.worldSphere.radius = (this->actor.scale.x * 500.0f);
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -860, ICHAIN_CONTINUE), ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneForward, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 150, ICHAIN_CONTINUE),  ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

void EnSnowball_Init(Actor* thisx, PlayState* play) {
    EnSnowball* this = (EnSnowball*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnSnowball_InitCollider(this, play);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 595.0f, ActorShadow_DrawCircle, 9.4f);
    this->actor.shape.shadowAlpha = 200;

    if (thisx->params == 1) {
        this->actor.speedXZ += 5.0f;
        this->actor.world.rot.y = Rand_ZeroFloat(65536.0f);
    }
}

void EnSnowball_Destroy(Actor* thisx, PlayState* play) {
    EnSnowball* this = (EnSnowball*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnSnowball_Update(Actor* thisx, PlayState* play) {
    EnSnowball* this = (EnSnowball*)thisx;
    Actor* player = GET_PLAYER(play);

    // Kill the actor if it falls too far
    if (thisx->world.pos.y < -10000.0f) {
        Actor_Kill(thisx);
        return;
    }

    u8 meanBoulder = thisx->params == 1 && this->actor.scale.x > 0.1f;

    // Check if the player is close enough to start rolling
    if (this->actor.xzDistToPlayer < MAX(20.0f, this->actor.scale.x * 600.0f) && !meanBoulder) {
        /// Flip the actor's rotation away from the player
        thisx->world.rot.y = thisx->yawTowardsPlayer + 0x8000;
        this->actor.speedXZ = MAX(5.0f, this->actor.speedXZ);
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        // Flip the actor's rotation away from the player
        thisx->world.rot.y = thisx->yawTowardsPlayer + 0x8000;
        
        func_8002F6D4(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
        Player_PlaySfx(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);
    }

    // Slow down the actor and increase it's scale
    if (this->actor.speedXZ > 0.0f) {
        CollisionPoly snowballPoly;
        u8 goingUp = this->actor.world.pos.y - this->prevY > 0.001f;
        u8 goingDown = this->actor.world.pos.y - this->prevY < -0.001f;

        // friction
        if (thisx->params != 1) {
            this->actor.speedXZ -= 0.1f;
        }

        if (goingDown) {
            // Increase the speed if going down hill
            f32 speed = (this->prevY - this->actor.world.pos.y) * 0.15f;
            this->actor.speedXZ += MIN(speed, 0.5f);
        } else if (goingUp) {
            // Reduce the speed if going up hill
            this->actor.speedXZ -= (this->actor.world.pos.y - this->prevY) * 0.1f;
        }

        if (goingUp || goingDown) {
            // Check if going straight, one degree right, or one degree left will result in steeper slope
            // Check straight
            Vec3f snowballPos = this->actor.world.pos;
            snowballPos.y += 100.0f;
            snowballPos.x += Math_SinS(this->actor.world.rot.y) * 1.0f;
            snowballPos.z += Math_CosS(this->actor.world.rot.y) * 1.0f;
            float straightSlope = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &snowballPoly, &snowballPos);

            // Check one degree right
            snowballPos = this->actor.world.pos;
            snowballPos.y += 100.0f;
            snowballPos.x += Math_SinS(this->actor.world.rot.y + 0x100) * 1.0f;
            snowballPos.z += Math_CosS(this->actor.world.rot.y + 0x100) * 1.0f;
            float rightSlope = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &snowballPoly, &snowballPos);

            // Check one degree left
            snowballPos = this->actor.world.pos;
            snowballPos.y += 100.0f;
            snowballPos.x += Math_SinS(this->actor.world.rot.y - 0x100) * 1.0f;
            snowballPos.z += Math_CosS(this->actor.world.rot.y - 0x100) * 1.0f;
            float leftSlope = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &snowballPoly, &snowballPos);

            if (straightSlope > rightSlope || straightSlope > leftSlope) {
                if (rightSlope < leftSlope) {
                    this->actor.world.rot.y += 0x100;
                } else {
                    this->actor.world.rot.y -= 0x100;
                }
            }
        }

        // Check if the actor is colliding with a wall and bounce off
        if (thisx->bgCheckFlags & 8) {
            if (ABS((s16)(thisx->wallYaw - thisx->world.rot.y)) > 0x4000) {
                thisx->world.rot.y = ((thisx->wallYaw - thisx->world.rot.y) + thisx->wallYaw) - 0x8000;
            }
            if (thisx->params != 1) {
                thisx->speedXZ *= 0.7f;
            }
            thisx->bgCheckFlags &= ~8;
            if (this->actor.speedXZ > 5.0f) {
                Audio_PlayActorSound2(thisx, NA_SE_EV_BOMB_BOUND);
            }
        }

        Actor_SetScale(&this->actor, MIN(0.15f, this->actor.scale.x + (this->actor.speedXZ * 0.00001f)));
    }

    if (this->actor.speedXZ < 0.0f) {
        this->actor.speedXZ = 0.0f;
    }

    // Based on speed and scale, rotate the snowball
    // The larger the snowball, the slower it rotates
    this->sRot += (this->actor.speedXZ * 15.0f) / this->actor.scale.x;

    // record the actor's position
    this->prevY = this->actor.world.pos.y;

    // Process movement (moves foward based on speed and rotation)
    Actor_MoveForward(thisx);

    // Prevent actor from going through the ground or walls
    Actor_UpdateBgCheckInfo(play, &this->actor, MAX(10.0f, this->actor.scale.x * 250.0f), MAX(10.0f, this->actor.scale.x * 500.0f), 0.0f, 0xFF);

    EnSnowball_UpdateCollider(this);
    // Add collision checks if the actor is a mean boulder
    if (meanBoulder) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnSnowball_Draw(Actor* thisx, PlayState* play) {
    EnSnowball* this = (EnSnowball*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_RotateZYX(this->sRot, thisx->world.rot.y, 0, MTXMODE_APPLY);
    
    Matrix_Translate(0.0f, 7600.0f, -148.649f, MTXMODE_APPLY);
    Matrix_Scale(4.844f, 4.844f, 4.844f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gLinkAdultPompomDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
