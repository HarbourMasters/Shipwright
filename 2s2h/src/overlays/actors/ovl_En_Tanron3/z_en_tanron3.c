/*
 * File: z_en_tanron3.c
 * Overlay: ovl_En_Tanron3
 * Description: Small fish summoned by Gyorg
 */

#include "z_en_tanron3.h"
#include "overlays/actors/ovl_Boss_03/z_boss_03.h"
#include "objects/object_boss03/object_boss03.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define WORK_TIMER_PICK_NEW_DEVIATION 0
#define WORK_TIMER_DIE 0
#define WORK_TIMER_OUT_OF_WATER 1
#define WORK_TIMER_ATTACK 2
#define WORK_TIMER_WAIT 2

void EnTanron3_Init(Actor* thisx, PlayState* play);
void EnTanron3_Destroy(Actor* thisx, PlayState* play);
void EnTanron3_Update(Actor* thisx, PlayState* play);
void EnTanron3_Draw(Actor* thisx, PlayState* play);

void EnTanron3_SetupLive(EnTanron3* this, PlayState* play);
void EnTanron3_Live(EnTanron3* this, PlayState* play);
void EnTanron3_Die(EnTanron3* this, PlayState* play);

static Vec3f sZeroVec[] = { 0.0f, 0.0f, 0.0f };

static Boss03* sGyorg = NULL;

ActorProfile En_Tanron3_Profile = {
    /**/ ACTOR_EN_TANRON3,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS03,
    /**/ sizeof(EnTanron3),
    /**/ EnTanron3_Init,
    /**/ EnTanron3_Destroy,
    /**/ EnTanron3_Update,
    /**/ EnTanron3_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x02 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 7, 10, -5, { 0, 0, 0 } },
};

// This actor has two colliders (one for AC and one for AT), but uses the same
// ColliderCylinderInit for both of them, leaving this one totally unused.
static ColliderCylinderInit sUnusedCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x02 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 20, -10, { 0, 0, 0 } },
};

void EnTanron3_CreateEffect(PlayState* play, Vec3f* effectPos) {
    GyorgEffect* effectPtr = play->specialEffects;
    s16 i;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++, effectPtr++) {
        if ((effectPtr->type == GYORG_EFFECT_NONE) || (effectPtr->type == GYORG_EFFECT_BUBBLE)) {
            effectPtr->type = GYORG_EFFECT_DROPLET;
            effectPtr->pos = *effectPos;
            effectPtr->velocity = *sZeroVec;
            effectPtr->accel = *sZeroVec;
            effectPtr->accel.y = -2.0f;
            effectPtr->unk_34.x = 0.1f;
            effectPtr->unk_34.y = 0.0f;
            effectPtr->unk_34.z = Rand_ZeroFloat(2 * M_PIf);
            effectPtr->unk_02 = Rand_ZeroFloat(100.0f);
            effectPtr->velocity.x = Rand_CenteredFloat(25.0f);
            effectPtr->velocity.z = Rand_CenteredFloat(25.0f);
            break;
        }
    }
}

void EnTanron3_Init(Actor* thisx, PlayState* play) {
    EnTanron3* this = (EnTanron3*)thisx;

    this->actor.gravity = -1.0f;
    Collider_InitAndSetCylinder(play, &this->atCollider, &this->actor, &sCylinderInit);
    Collider_InitAndSetCylinder(play, &this->acCollider, &this->actor, &sCylinderInit);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGyorgSmallFishSkel, &gGyorgSmallFishSwimAnim, this->jointTable,
                       this->morphTable, GYORG_SMALL_FISH_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.02f);
    EnTanron3_SetupLive(this, play);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->currentRotationAngle = Rand_ZeroFloat(500000.0f);
    this->waterSurfaceYPos = 430.0f;
    sGyorg = (Boss03*)this->actor.parent;
}

void EnTanron3_Destroy(Actor* thisx, PlayState* play) {
    sGyorg->numSpawnedSmallFish--;
}

void EnTanron3_SpawnBubbles(EnTanron3* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 100, 55, 55, 255 };
    static Color_RGBA8 sEnvColor = { 50, 10, 10, 255 };
    s32 i;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        Matrix_RotateYF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_NEW);
        Matrix_RotateXFApply(Rand_ZeroFloat(2 * M_PIf));
        Matrix_MultVecZ(Rand_ZeroFloat(3.0f) + 2.0f, &velocity);
        accel.x = velocity.x * -0.05f;
        accel.y = velocity.y * -0.05f;
        accel.z = velocity.z * -0.05f;
        EffectSsDtBubble_SpawnCustomColor(play, &this->actor.world.pos, &velocity, &accel, &sPrimColor, &sEnvColor,
                                          Rand_ZeroFloat(30.0f) + 70.0f, Rand_ZeroFloat(5.0f) + 15.0f, false);
    }
}

void EnTanron3_SetupLive(EnTanron3* this, PlayState* play) {
    this->actionFunc = EnTanron3_Live;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgSmallFishSwimAnim, -10.0f);
    this->rotationStep = 0;
    this->rotationScale = 5;
    this->workTimer[WORK_TIMER_PICK_NEW_DEVIATION] = 50;
    this->actor.speed = 5.0f;
    this->speedMaxStep = 0.5f;
    this->deviation.x = Rand_CenteredFloat(500.0f);
    this->deviation.y = Rand_CenteredFloat(100.0f);
    this->deviation.z = Rand_CenteredFloat(500.0f);
    Math_Vec3f_Copy(&this->targetPos, &this->actor.world.pos);
    this->timer = Rand_ZeroFloat(100.0f);
}

/**
 * This controls the vast majority of the fish's behavior while it's alive, including:
 * - deciding whether to be hostile or not
 * - determing whether the fish is beached or not
 * - swimming towards the player to attack them
 * - swimming around idly if the player is out of range
 * - flopping around on land if it beaches itself
 */
void EnTanron3_Live(EnTanron3* this, PlayState* play) {
    s32 atanTemp;
    f32 xDistance;
    f32 yDistance;
    f32 zDistance;
    f32 xzDistance;
    f32 extraScaleY = 0.0f;
    Player* player = GET_PLAYER(play);

    this->skelAnime.curFrame = 4.0f;
    if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (player->actor.shape.feetPos[0].y >= 438.0f)) {
        // Player is standing on the central platform, so stop chasing them
        this->isNonHostile = true;
    } else if (this->isNonHostile && (this->workTimer[WORK_TIMER_WAIT] == 0) && !(this->timer & 0x1F)) {
        xDistance = this->targetPos.x - player->actor.world.pos.x;
        zDistance = this->targetPos.z - player->actor.world.pos.z;
        if (sqrtf(SQ(xDistance) + SQ(zDistance)) < 500.0f) {
            // Player is in the water and close enough, so start chasing them
            this->isNonHostile = false;
            this->workTimer[WORK_TIMER_ATTACK] = 150;
        }
    }

    if (this->actor.world.pos.y < this->waterSurfaceYPos) {
        // The fish is below the water's surface, so it's no longer beached if it was before
        this->isBeached = false;
        switch (this->isNonHostile) {
            case false:
                this->targetSpeedXZ = 5.0f;
                this->targetRotationStep = 0x1000;
                this->nextRotationAngle = 0x3A98;

                Math_Vec3f_Copy(&this->targetPos, &player->actor.world.pos);
                if (!(this->timer & 0xF)) {
                    if ((Rand_ZeroOne() < 0.5f) && (this->actor.xzDistToPlayer <= 200.0f)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_PIRANHA_ATTACK);
                    }
                }

                // If the player gets eaten by Gyorg, or if the attack timer ran out,
                // stop chasing the player for a little bit.
                if ((this->workTimer[WORK_TIMER_ATTACK] == 0) || (player->stateFlags2 & PLAYER_STATE2_80)) {
                    this->workTimer[WORK_TIMER_WAIT] = 150;
                    this->isNonHostile = true;
                }
                break;

            case true:
                if ((sGyorg->unk_324 != 0) && !(this->timer & 0x7)) {
                    this->nextRotationAngle = 0x4E20;
                    this->actor.speed = 6.0f;
                } else {
                    this->nextRotationAngle = 0x1F40;
                }
                this->targetRotationStep = 0x200;
                this->targetSpeedXZ = 2.0f;

                // If the fish is idly swimming around, this code will make it spin in a circle.
                // Its target is constantly updated, so it constantly rotates to face it, and its
                // momentum keeps it spinning. This code can also be reached when the fish "give up"
                // on attacking the player; in that case, this code will make it turn in the
                // opposite direction and swim away. In both cases, the fish's target y-position
                // will be slightly above the halfway point of the water.
                atanTemp = Math_Atan2S_XY(this->targetPos.z, this->targetPos.x);
                Matrix_RotateYS(atanTemp, MTXMODE_NEW);
                Matrix_MultVecZ(700.0f, &this->targetPos);
                this->targetPos.y = 250.0f;

                extraScaleY = 150.0f;
                break;

            default:
                break;
        }

        if (this->workTimer[WORK_TIMER_OUT_OF_WATER] == 0) {
            if ((this->workTimer[WORK_TIMER_PICK_NEW_DEVIATION] == 0) && (this->actor.speed > 1.0f)) {
                this->workTimer[WORK_TIMER_PICK_NEW_DEVIATION] = Rand_ZeroFloat(20.0f);
                this->deviation.x = Rand_CenteredFloat(100.0f);
                this->deviation.y = Rand_CenteredFloat(50.0f + extraScaleY);
                this->deviation.z = Rand_CenteredFloat(100.0f);
            }
            this->targetPosWithDeviation.y = this->targetPos.y + this->deviation.y + 50.0f;
        }

        this->targetPosWithDeviation.x = this->targetPos.x + this->deviation.x;
        this->targetPosWithDeviation.z = this->targetPos.z + this->deviation.z;
        xDistance = this->targetPosWithDeviation.x - this->actor.world.pos.x;
        yDistance = this->targetPosWithDeviation.y - this->actor.world.pos.y;
        zDistance = this->targetPosWithDeviation.z - this->actor.world.pos.z;

        // Rotate the fish to look towards its target
        xzDistance = sqrtf(SQ(xDistance) + SQ(zDistance));
        atanTemp = Math_Atan2S_XY(xzDistance, -yDistance);
        Math_ApproachS(&this->actor.world.rot.x, atanTemp, this->rotationScale, this->rotationStep);
        atanTemp = Math_Atan2S_XY(zDistance, xDistance);
        Math_SmoothStepToS(&this->actor.world.rot.y, atanTemp, this->rotationScale, this->rotationStep, 0);
        Math_ApproachS(&this->rotationStep, this->targetRotationStep, 1, 0x100);

        Math_ApproachF(&this->actor.speed, this->targetSpeedXZ, 1.0f, this->speedMaxStep);
        Actor_MoveWithoutGravityReverse(&this->actor);
    } else {
        switch (this->isBeached) {
            case false:
                // Fish is above water but hasn't touched land before
                this->actor.gravity = -1.0f;
                this->targetPosWithDeviation.y = this->waterSurfaceYPos - 50.0f;
                this->workTimer[WORK_TIMER_OUT_OF_WATER] = 25;
                Math_ApproachS(&this->actor.world.rot.x, 0x3000, 5, 0xBD0);
                if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                    this->actor.speed = 0.0f;
                    if (this->actor.velocity.y > 0.0f) {
                        this->actor.velocity.y = -1.0f;
                    }
                }
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    // Fish has touched land
                    this->isBeached = true;
                }
                break;

            case true:
                this->nextRotationAngle = 0x3A98;
                this->actor.gravity = -1.5f;
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    // Fish is still touching land, so it's still beached. Randomly flop around
                    this->actor.velocity.y = Rand_ZeroFloat(5.0f) + 5.0f;
                    this->actor.speed = Rand_ZeroFloat(2.0f) + 2.0f;
                    if (Rand_ZeroOne() < 0.5f) {
                        this->targetShapeRotation.x =
                            TRUNCF_BINANG(Rand_CenteredFloat(0x1F4)) + this->targetShapeRotation.x + 0x8000;
                    }
                    if (Rand_ZeroOne() < 0.5f) {
                        this->targetShapeRotation.z =
                            TRUNCF_BINANG(Rand_CenteredFloat(0x1F4)) + this->targetShapeRotation.z + 0x8000;
                    }
                    if (Rand_ZeroOne() < 0.5f) {
                        this->targetShapeRotation.y = TRUNCF_BINANG(Rand_ZeroFloat(0x10000));
                    }
                    this->actor.world.rot.y = Math_Atan2S_XY(this->actor.world.pos.z, this->actor.world.pos.x) +
                                              TRUNCF_BINANG(Rand_CenteredFloat(0xCE20));
                }

                Math_ApproachS(&this->actor.shape.rot.y, this->targetShapeRotation.y, 3, 0x500);
                Math_ApproachS(&this->actor.shape.rot.x, this->targetShapeRotation.x, 3, 0xC00);
                Math_ApproachS(&this->actor.shape.rot.z, this->targetShapeRotation.z, 3, 0xC00);
                if ((Rand_ZeroOne() < 0.5f) & !(this->timer & 0x3)) {
                    Vec3f effectPos;

                    effectPos.x = Rand_CenteredFloat(30.0f) + this->actor.world.pos.x;
                    effectPos.y = this->actor.world.pos.y;
                    effectPos.z = Rand_CenteredFloat(30.0f) + this->actor.world.pos.z;
                    EnTanron3_CreateEffect(play, &effectPos);
                }
                break;

            default:
                break;
        }
        Actor_MoveWithGravity(&this->actor);
    }

    this->currentRotationAngle += this->nextRotationAngle;
    this->tailRotation = Math_SinS(this->currentRotationAngle) * 5000.0f;
    this->bodyRotation = Math_SinS(this->currentRotationAngle + 0x6978) * 5000.0f;
    this->trunkRotation = Math_SinS(this->currentRotationAngle) * 5000.0f;
    if (!this->isBeached) {
        this->actor.shape.rot = this->actor.world.rot;
    }
}

void EnTanron3_SetupDie(EnTanron3* this, PlayState* play) {
    f32 xDistance;
    f32 yDistance;
    f32 zDistance;
    Player* player = GET_PLAYER(play);

    this->actionFunc = EnTanron3_Die;
    xDistance = this->actor.world.pos.x - player->actor.world.pos.x;
    yDistance = this->actor.world.pos.y - player->actor.world.pos.y + 30.0f;
    zDistance = this->actor.world.pos.z - player->actor.world.pos.z;
    this->actor.world.rot.x = Math_Atan2S_XY(sqrtf(SQ(xDistance) + SQ(zDistance)), -yDistance);
    this->actor.world.rot.y = Math_Atan2S_XY(zDistance, xDistance);
    this->workTimer[WORK_TIMER_DIE] = 6;
    this->actor.speed = 10.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_MINI_DEAD);
}

void EnTanron3_Die(EnTanron3* this, PlayState* play) {
    Actor_MoveWithoutGravityReverse(&this->actor);
    if (this->workTimer[WORK_TIMER_DIE] == 0) {
        EnTanron3_SpawnBubbles(this, play);
        Actor_Kill(&this->actor);
        if (Rand_ZeroOne() < 0.3f) {
            Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x60);
        }
    }
}

void EnTanron3_CheckCollisions(EnTanron3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.y > 350.0f) {
        if (this->atCollider.base.atFlags & AT_HIT) {
            this->atCollider.base.atFlags &= ~AT_HIT;
            func_800B8D50(play, NULL, 3.0f, Math_Atan2S_XY(-player->actor.world.pos.z, -player->actor.world.pos.x),
                          5.0f, 0);
        }
    }
    if (this->acCollider.base.acFlags & AC_HIT) {
        this->acCollider.base.acFlags &= ~AC_HIT;
        if (this->deathTimer == 0) {
            this->deathTimer = 15;
            this->fogTimer = 15;
            EnTanron3_SetupDie(this, play);
            sGyorg->unk_324 = 20;
        }
    }
}

void EnTanron3_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTanron3* this = (EnTanron3*)thisx;
    s16 i;
    Vec3f splashPos;

    if (KREG(63) == 0) {
        this->timer++;

        for (i = 0; i < TANRON3_WORK_TIMER_MAX; i++) {
            if (this->workTimer[i] != 0) {
                this->workTimer[i]--;
            }
        }
        if (this->deathTimer != 0) {
            this->deathTimer--;
        }
        if (this->fogTimer != 0) {
            this->fogTimer--;
        }

        this->actionFunc(this, play);
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 20.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

        // The fish has either just entered or just exited the water, so create a splash effect
        if (((this->actor.prevPos.y < this->waterSurfaceYPos) && (this->waterSurfaceYPos <= this->actor.world.pos.y)) ||
            ((this->actor.prevPos.y > this->waterSurfaceYPos) && (this->waterSurfaceYPos >= this->actor.world.pos.y))) {
            splashPos.x = this->actor.world.pos.x;
            splashPos.y = this->waterSurfaceYPos + 10.0f;
            splashPos.z = this->actor.world.pos.z;
            EffectSsGSplash_Spawn(play, &splashPos, NULL, NULL, 1, 500);
            Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER);
        }
    }

    EnTanron3_CheckCollisions(this, play);
    Collider_UpdateCylinder(&this->actor, &this->atCollider);
    Collider_UpdateCylinder(&this->actor, &this->acCollider);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->atCollider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->acCollider.base);

    if (((s8)sGyorg->actor.colChkInfo.health <= 0) && (this->actionFunc != EnTanron3_Die)) {
        EnTanron3_SetupDie(this, play);
        this->workTimer[WORK_TIMER_DIE] = 0;
    }
}

s32 EnTanron3_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTanron3* this = (EnTanron3*)thisx;

    if (limbIndex == GYORG_SMALL_FISH_LIMB_ROOT) {
        rot->y += this->bodyRotation;
    }

    if (limbIndex == GYORG_SMALL_FISH_LIMB_TRUNK_ROOT) {
        rot->y += this->trunkRotation;
    }

    if (limbIndex == GYORG_SMALL_FISH_LIMB_TAIL_FIN) {
        rot->y += this->tailRotation;
    }

    return false;
}

void EnTanron3_Draw(Actor* thisx, PlayState* play) {
    EnTanron3* this = (EnTanron3*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if ((this->fogTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnTanron3_OverrideLimbDraw, NULL, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}
