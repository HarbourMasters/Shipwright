/*
 * File: z_en_tanron4.c
 * Overlay: ovl_En_Tanron4
 * Description: Seagull
 */

#include "z_en_tanron4.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnTanron4_Init(Actor* thisx, PlayState* play2);
void EnTanron4_Destroy(Actor* thisx, PlayState* play);
void EnTanron4_Update(Actor* thisx, PlayState* play);
void EnTanron4_Draw(Actor* thisx, PlayState* play);

void EnTanron4_SetupFlyNearHome(EnTanron4* this);
void EnTanron4_SetupFlyNearActor(EnTanron4* this);

void EnTanron4_FlyNearHome(EnTanron4* this, PlayState* play);
void EnTanron4_FlyNearActor(EnTanron4* this, PlayState* play);

typedef enum SeagullFlyState {
    /* 0 */ SEAGULL_FLY_FLAP,
    /* 1 */ SEAGULL_FLY_GLIDE
} SeagullFlyState;

typedef enum SeagullTimer {
    /* 0 */ SEAGULL_TIMER_FLY_STATE,
    /* 1 */ SEAGULL_TIMER_CHOOSE_TARGET
} SeagullTimer;

ActorProfile En_Tanron4_Profile = {
    /**/ ACTOR_EN_TANRON4,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_TANRON4,
    /**/ sizeof(EnTanron4),
    /**/ EnTanron4_Init,
    /**/ EnTanron4_Destroy,
    /**/ EnTanron4_Update,
    /**/ EnTanron4_Draw,
};

void EnTanron4_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTanron4* this = (EnTanron4*)thisx;

    SkelAnime_InitFlex(play, &this->skelAnime, &gSeagullSkel, &gSeagullFlapAnim, this->jointTable, this->morphTable,
                       SEAGULL_LIMB_RIGHT_WING_MAX);

    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    thisx->speed = 3.0f + KREG(48);
    thisx->cullingVolumeDistance = 10000.0f + KREG(70);
    this->randRollTimer = Rand_ZeroFloat(10.0f);

    if (thisx->params == SEAGULL_FOLLOW_ACTOR) {
        EnTanron4_SetupFlyNearActor(this);
    } else {
        EnTanron4_SetupFlyNearHome(this);

        // spawn number of seagulls specified by params
        if (thisx->params >= 0) {
            s32 i;

            for (i = 0; i < thisx->params; i++) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TANRON4, thisx->world.pos.x + Rand_CenteredFloat(500.0f),
                            thisx->world.pos.y + Rand_CenteredFloat(100.0f),
                            thisx->world.pos.z + Rand_CenteredFloat(500.0f), 0, Rand_ZeroFloat(0x10000), 0,
                            SEAGULL_CLONE);
            }
        }

        if ((CURRENT_TIME > CLOCK_TIME(20, 0)) || (CURRENT_TIME < CLOCK_TIME(4, 0))) {
            this->timeInfluence = 1500.0f;
            thisx->world.pos.y += 1500.0f;
        }
    }
}

void EnTanron4_Destroy(Actor* thisx, PlayState* play) {
}

void EnTanron4_SetupFlyNearHome(EnTanron4* this) {
    this->actionFunc = EnTanron4_FlyNearHome;
    Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, 0.0f);
}

void EnTanron4_FlyNearHome(EnTanron4* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 distToTarget;
    s16 xRot;
    s16 yRot;
    s16 zRot;

    // `timeInfluence` controls both the height of the seagulls and when they are visible.
    // They fly higher in the sky as the night goes on, and they disapear as dawn approaches.
    if ((CURRENT_TIME > CLOCK_TIME(20, 0)) || (CURRENT_TIME < CLOCK_TIME(4, 0))) {
        Math_ApproachF(&this->timeInfluence, 1500.0f, 1.0f, 1.0f);
    } else {
        Math_ApproachZeroF(&this->timeInfluence, 1.0f, 1.0f);
    }

    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y + this->timeInfluence - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;

    distToTarget = sqrtf(SQ(xDiff) + SQ(zDiff));

    if ((this->timers[SEAGULL_TIMER_CHOOSE_TARGET] == 0) || (distToTarget < 100.0f)) {
        this->targetPos.x = this->actor.home.pos.x + Rand_CenteredFloat(500.0f);
        this->targetPos.y = this->actor.home.pos.y + Rand_CenteredFloat(100.0f);
        this->targetPos.z = this->actor.home.pos.z + Rand_CenteredFloat(500.0f);

        this->timers[SEAGULL_TIMER_CHOOSE_TARGET] = Rand_ZeroFloat(100.0f) + 60.0f;
        this->step = 0;
    }

    yRot = Math_Atan2S(xDiff, zDiff);
    xRot = Math_Atan2S(yDiff, distToTarget);
    zRot = Math_SmoothStepToS(&this->actor.world.rot.y, yRot, 0xA, this->step, 0);

    if (zRot > 0x1000) {
        zRot = 0x1000;
    } else if (zRot < -0x1000) {
        zRot = -0x1000;
    }

    Math_ApproachS(&this->actor.world.rot.x, xRot, 0xA, this->step);
    Math_ApproachS(&this->actor.world.rot.z, -zRot, 0xA, this->step);

    Math_ApproachS(&this->step, 0x200, 1, 0x10);

    SkelAnime_Update(&this->skelAnime);

    switch (this->flyState) {
        case SEAGULL_FLY_FLAP:
            if ((this->timers[SEAGULL_TIMER_FLY_STATE] == 0) && Animation_OnFrame(&this->skelAnime, 2.0f + KREG(42))) {
                this->flyState = SEAGULL_FLY_GLIDE;
                this->timers[SEAGULL_TIMER_FLY_STATE] = Rand_ZeroFloat(50.0f) + 50.0f;
                Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, -15.0f + KREG(43));
                this->skelAnime.curFrame = 2.0f + KREG(42);
                this->skelAnime.playSpeed = 0.0f;
            }
            break;

        case SEAGULL_FLY_GLIDE:
            if (((this->randRollTimer % 8) == 0) && (Rand_ZeroOne() < 0.5f)) {
                this->rollTarget = Rand_CenteredFloat(3000.0f + KREG(44));
            }

            if (this->timers[SEAGULL_TIMER_FLY_STATE] == 0) {
                this->flyState = SEAGULL_FLY_FLAP;
                this->timers[SEAGULL_TIMER_FLY_STATE] = Rand_ZeroFloat(50.0f) + 70.0f;
                Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, -10.0f + KREG(43));
                this->skelAnime.curFrame = 2.0f + KREG(42);
            }
            break;

        default:
            break;
    }

    this->actor.shape.rot.x = -this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.z = this->actor.world.rot.z;

    Math_ApproachS(&this->roll, this->rollTarget, 3, 0x3E8 + KREG(45));
}

void EnTanron4_SetupFlyNearActor(EnTanron4* this) {
    this->actionFunc = EnTanron4_FlyNearActor;
    Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, 0.0f);
}

void EnTanron4_FlyNearActor(EnTanron4* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 distToTarget;
    s16 xRot;
    s16 yRot;
    s16 zRot;
    Actor* targetActor = this->actor.parent;

    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;

    distToTarget = sqrtf(SQ(xDiff) + SQ(zDiff));

    if ((this->timers[SEAGULL_TIMER_CHOOSE_TARGET] == 0) || (distToTarget < 100.0f)) {
        this->targetPos.x = targetActor->world.pos.x + Rand_CenteredFloat(200.0f + KREG(82));
        this->targetPos.y = targetActor->world.pos.y + KREG(80) + 100.0f + Rand_CenteredFloat(100.0f + KREG(81));
        this->targetPos.z = targetActor->world.pos.z + Rand_CenteredFloat(200.0f + KREG(82));

        this->timers[SEAGULL_TIMER_CHOOSE_TARGET] = Rand_ZeroFloat(100.0f) + 60.0f;
        this->step = 0;
    }

    yRot = Math_Atan2S(xDiff, zDiff);
    xRot = Math_Atan2S(yDiff, distToTarget);
    zRot = Math_SmoothStepToS(&this->actor.world.rot.y, yRot, 0xA, this->step, 0);

    if (zRot > 0x1000) {
        zRot = 0x1000;
    } else if (zRot < -0x1000) {
        zRot = -0x1000;
    }

    Math_ApproachS(&this->actor.world.rot.x, xRot, 0xA, this->step);
    Math_ApproachS(&this->actor.world.rot.z, -zRot, 0xA, this->step);
    Math_ApproachS(&this->step, 0x200, 1, 0x10);

    SkelAnime_Update(&this->skelAnime);

    switch (this->flyState) {
        case SEAGULL_FLY_FLAP:
            if ((this->timers[SEAGULL_TIMER_FLY_STATE] == 0) && Animation_OnFrame(&this->skelAnime, 2.0f + KREG(42))) {
                this->flyState = SEAGULL_FLY_GLIDE;
                this->timers[SEAGULL_TIMER_FLY_STATE] = Rand_ZeroFloat(50.0f) + 50.0f;
                Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, -15.0f + KREG(43));
                this->skelAnime.curFrame = 2.0f + KREG(42);
                this->skelAnime.playSpeed = 0.0f;
            }
            break;

        case SEAGULL_FLY_GLIDE:
            if (((this->randRollTimer % 8) == 0) && (Rand_ZeroOne() < 0.5f)) {
                this->rollTarget = Rand_CenteredFloat(3000.0f + KREG(44));
            }

            if (this->timers[SEAGULL_TIMER_FLY_STATE] == 0) {
                this->flyState = SEAGULL_FLY_FLAP;
                this->timers[SEAGULL_TIMER_FLY_STATE] = Rand_ZeroFloat(50.0f) + 70.0f;
                Animation_MorphToLoop(&this->skelAnime, &gSeagullFlapAnim, -10.0f + KREG(43));
                this->skelAnime.curFrame = 2.0f + KREG(42);
            }
            break;

        default:
            break;
    }

    this->actor.shape.rot.x = -this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.z = this->actor.world.rot.z;

    Math_ApproachS(&this->roll, this->rollTarget, 3, 0x3E8 + KREG(45));
}

void EnTanron4_Update(Actor* thisx, PlayState* play) {
    EnTanron4* this = (EnTanron4*)thisx;

    Actor_SetScale(&this->actor, 0.001f * KREG(16) + 0.01f);

    if (KREG(63) == 0) {
        s32 i;

        this->randRollTimer++;

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        this->actionFunc(this, play);

        Actor_UpdateVelocityWithoutGravity(&this->actor);
        Actor_UpdatePos(&this->actor);
    }
}

void EnTanron4_Draw(Actor* thisx, PlayState* play) {
    EnTanron4* this = (EnTanron4*)thisx;

    if (this->timeInfluence < 1400.0f) {
        Matrix_RotateZS(this->roll, MTXMODE_APPLY);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              NULL, NULL, &this->actor);
    }
}
