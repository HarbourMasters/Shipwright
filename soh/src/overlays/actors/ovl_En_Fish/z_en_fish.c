/*
 * File: z_en_fish.c
 * Overlay: ovl_En_Fish
 * Description: Fish
 */

#include "z_en_fish.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS 0

void EnFish_Init(Actor* thisx, PlayState* play);
void EnFish_Destroy(Actor* thisx, PlayState* play);
void EnFish_Update(Actor* thisx, PlayState* play);
void EnFish_Draw(Actor* thisx, PlayState* play);

void EnFish_Respawning_SetupSlowDown(EnFish* this);
void EnFish_Respawning_SlowDown(EnFish* this, PlayState* play);
void EnFish_Respawning_SetupFollowChild(EnFish* this);
void EnFish_Respawning_FollowChild(EnFish* this, PlayState* play);
void EnFish_Respawning_SetupFleePlayer(EnFish* this);
void EnFish_Respawning_FleePlayer(EnFish* this, PlayState* play);
void EnFish_Respawning_SetupApproachPlayer(EnFish* this);
void EnFish_Respawning_ApproachPlayer(EnFish* this, PlayState* play);
void EnFish_Dropped_SetupFall(EnFish* this);
void EnFish_Dropped_Fall(EnFish* this, PlayState* play);
void EnFish_Dropped_SetupFlopOnGround(EnFish* this);
void EnFish_Dropped_FlopOnGround(EnFish* this, PlayState* play);
void EnFish_Dropped_SetupSwimAway(EnFish* this);
void EnFish_Dropped_SwimAway(EnFish* this, PlayState* play);
void EnFish_Unique_SetupSwimIdle(EnFish* this);
void EnFish_Unique_SwimIdle(EnFish* this, PlayState* play);

// Used in the cutscene functions
static Actor* D_80A17010 = NULL;
static f32 D_80A17014 = 0.0f;
static f32 D_80A17018 = 0.0f;

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

const ActorInit En_Fish_InitVars = {
    ACTOR_EN_FISH,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnFish),
    (ActorFunc)EnFish_Init,
    (ActorFunc)EnFish_Destroy,
    (ActorFunc)EnFish_Update,
    (ActorFunc)EnFish_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 40, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 700, ICHAIN_STOP),
};

f32 EnFish_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void EnFish_SetInWaterAnimation(EnFish* this) {
    Animation_Change(&this->skelAnime, &gFishInWaterAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gFishInWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void EnFish_SetOutOfWaterAnimation(EnFish* this) {
    Animation_Change(&this->skelAnime, &gFishOutOfWaterAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gFishOutOfWaterAnim),
                     ANIMMODE_LOOP_INTERP, 2.0f);
}

void EnFish_BeginRespawn(EnFish* this) {
    this->respawnTimer = 400;
    Actor_SetScale(&this->actor, 0.001f);
    this->actor.draw = NULL;
}

void EnFish_SetCutsceneData(EnFish* this) {
    Actor* thisx = &this->actor;

    if (D_80A17010 == NULL) {
        D_80A17010 = thisx;
        Actor_SetScale(thisx, 0.01f);
        thisx->draw = EnFish_Draw;
        thisx->shape.rot.x = 0;
        thisx->shape.rot.y = -0x6410;
        thisx->shape.rot.z = 0x4000;
        thisx->shape.yOffset = 600.0f;
        D_80A17014 = 10.0f;
        D_80A17018 = 0.0f;
        thisx->flags |= ACTOR_FLAG_4;
        EnFish_SetOutOfWaterAnimation(this);
    }
}

void EnFish_ClearCutsceneData(EnFish* this) {
    D_80A17010 = NULL;
    D_80A17014 = 0.0f;
    D_80A17018 = 0.0f;
}

void EnFish_Init(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;
    s16 params = this->actor.params;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gFishSkel, &gFishInWaterAnim, this->jointTable, this->morphTable,
                       7);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderItems);
    this->actor.colChkInfo.mass = 50;
    this->slowPhase = Rand_ZeroOne() * (0xFFFF + 0.5f);
    this->fastPhase = Rand_ZeroOne() * (0xFFFF + 0.5f);

    if (params == FISH_DROPPED) {
        this->actor.flags |= ACTOR_FLAG_4;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 8.0f);
        EnFish_Dropped_SetupFall(this);
    } else if (params == FISH_SWIMMING_UNIQUE) {
        EnFish_Unique_SetupSwimIdle(this);
    } else {
        EnFish_Respawning_SetupSlowDown(this);
    }
}

void EnFish_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFish* this = (EnFish*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnFish_SetYOffset(EnFish* this) {
    this->actor.shape.yOffset += (Math_SinS(this->slowPhase) * 10.0f + Math_SinS(this->fastPhase) * 5.0f);
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -200.0f, 200.0f);
}

s32 EnFish_InBottleRange(EnFish* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f sp1C;

    if (this->actor.xzDistToPlayer < 32.0f) {
        sp1C.x = (Math_SinS(this->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.x;
        sp1C.y = player->actor.world.pos.y;
        sp1C.z = (Math_CosS(this->actor.yawTowardsPlayer + 0x8000) * 16.0f) + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnFish_XZDistanceSquared(&sp1C, &this->actor.world.pos) <= SQ(20.0f)) {
            return true;
        }
    }

    return false;
}

s32 EnFish_CheckXZDistanceToPlayer(EnFish* this, PlayState* play) {
    return (this->actor.xzDistToPlayer < 60.0f);
}

// Respawning type functions

void EnFish_Respawning_SetupSlowDown(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = Rand_S16Offset(5, 35);
    this->unk_250 = 0;
    EnFish_SetInWaterAnimation(this);
    this->actionFunc = EnFish_Respawning_SlowDown;
}

void EnFish_Respawning_SlowDown(EnFish* this, PlayState* play) {
    EnFish_SetYOffset(this);
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.05f, 0.3f, 0.0f);
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speedXZ * 1.4f + 0.8f, 2.0f);
    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->timer <= 0) {
        EnFish_Respawning_SetupFollowChild(this);
    } else if (&this->actor == this->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(this);
    } else if (EnFish_CheckXZDistanceToPlayer(this, play)) {
        EnFish_Respawning_SetupFleePlayer(this);
    }
}

// The three following actionfunctions also turn the yaw to home if the fish is too far from it.

void EnFish_Respawning_SetupFollowChild(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = Rand_S16Offset(15, 45);
    this->unk_250 = 0;
    EnFish_SetInWaterAnimation(this);
    this->actionFunc = EnFish_Respawning_FollowChild;
}

void EnFish_Respawning_FollowChild(EnFish* this, PlayState* play) {
    s32 pad;

    EnFish_SetYOffset(this);
    Math_SmoothStepToF(&this->actor.speedXZ, 1.8f, 0.08f, 0.4f, 0.0f);

    if ((EnFish_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(80.0f)) || (this->timer < 4)) {
        Math_StepToAngleS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                          3000);
    } else if ((this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        Math_StepToAngleS(&this->actor.world.rot.y,
                          Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.child->world.pos), 3000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speedXZ * 1.5f + 0.8f, 4.0f);
    SkelAnime_Update(&this->skelAnime);

    if (this->timer <= 0) {
        EnFish_Respawning_SetupSlowDown(this);
    } else if (&this->actor == this->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(this);
    } else if (EnFish_CheckXZDistanceToPlayer(this, play)) {
        EnFish_Respawning_SetupFleePlayer(this);
    }
}

void EnFish_Respawning_SetupFleePlayer(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = Rand_S16Offset(10, 40);
    this->unk_250 = 0;
    EnFish_SetInWaterAnimation(this);
    this->actionFunc = EnFish_Respawning_FleePlayer;
}

void EnFish_Respawning_FleePlayer(EnFish* this, PlayState* play) {
    s32 pad;
    s16 pad2;
    s16 frames;
    s16 yaw;
    s16 playerClose;

    EnFish_SetYOffset(this);
    playerClose = EnFish_CheckXZDistanceToPlayer(this, play);
    Math_SmoothStepToF(&this->actor.speedXZ, 4.2f, 0.08f, 1.4f, 0.0f);

    if (EnFish_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(160.0f)) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        Math_StepToAngleS(&this->actor.world.rot.y, yaw, 3000);
    } else if ((this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.child->world.pos);
        Math_StepToAngleS(&this->actor.world.rot.y, yaw, 2000);
    } else if (playerClose) {
        yaw = this->actor.yawTowardsPlayer + 0x8000;
        frames = play->state.frames;

        if (frames & 0x10) {
            if (frames & 0x20) {
                yaw += 0x2000;
            }
        } else {
            if (frames & 0x20) {
                yaw -= 0x2000;
            }
        }
        if (play) {}
        Math_StepToAngleS(&this->actor.world.rot.y, yaw, 2000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX(this->actor.speedXZ * 1.5f + 0.8f, 4.0f);

    SkelAnime_Update(&this->skelAnime);

    if ((this->timer <= 0) || !playerClose) {
        EnFish_Respawning_SetupSlowDown(this);
    } else if (&this->actor == this->actor.child) {
        EnFish_Respawning_SetupApproachPlayer(this);
    }
}

void EnFish_Respawning_SetupApproachPlayer(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    EnFish_SetInWaterAnimation(this);
    this->timer = Rand_S16Offset(10, 40);
    this->unk_250 = 0;
    this->actionFunc = EnFish_Respawning_ApproachPlayer;
}

void EnFish_Respawning_ApproachPlayer(EnFish* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2;
    Vec3f sp38;
    s16 yaw;
    s16 temp_a0_2;

    EnFish_SetYOffset(this);
    Math_SmoothStepToF(&this->actor.speedXZ, 1.8f, 0.1f, 0.5f, 0.0f);

    if (EnFish_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(80.0f)) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        Math_StepToAngleS(&this->actor.world.rot.y, yaw, 3000);
    } else {
        if ((s16)play->state.frames & 0x40) {
            temp_a0_2 = (this->actor.yawTowardsPlayer + 0x9000);
        } else {
            temp_a0_2 = (this->actor.yawTowardsPlayer + 0x7000);
        }

        sp38.x = player->actor.world.pos.x + (Math_SinS(temp_a0_2) * 20.0f);
        sp38.y = player->actor.world.pos.y;
        sp38.z = player->actor.world.pos.z + (Math_CosS(temp_a0_2) * 20.0f);

        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &sp38);
        Math_StepToAngleS(&this->actor.world.rot.y, yaw, 3000);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->skelAnime.playSpeed = CLAMP_MAX((this->actor.speedXZ * 1.5f) + 0.8f, 4.0f);

    SkelAnime_Update(&this->skelAnime);

    if (this->timer <= 0) {
        EnFish_Respawning_SetupSlowDown(this);
    }
}

// Dropped type functions

void EnFish_Dropped_SetupFall(EnFish* this) {
    this->actor.gravity = -1.0f;
    this->actor.minVelocityY = -10.0f;
    this->actor.shape.yOffset = 0.0f;
    EnFish_SetOutOfWaterAnimation(this);
    this->unk_250 = 5;
    this->actionFunc = EnFish_Dropped_Fall;
    this->timer = 300;
}

void EnFish_Dropped_Fall(EnFish* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.1f, 0.1f, 0.0f);
    Math_StepToAngleS(&this->actor.world.rot.x, 0x4000, 100);
    Math_StepToAngleS(&this->actor.world.rot.z, -0x4000, 100);
    this->actor.shape.rot.x = this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.z = this->actor.world.rot.z;
    SkelAnime_Update(&this->skelAnime);

    if (this->actor.bgCheckFlags & 1) { // On floor
        this->timer = 400;
        EnFish_Dropped_SetupFlopOnGround(this);
    } else if (this->actor.bgCheckFlags & 0x20) { // In water
        EnFish_Dropped_SetupSwimAway(this);
    } else if ((this->timer <= 0) && (this->actor.params == FISH_DROPPED) &&
               (this->actor.floorHeight < BGCHECK_Y_MIN + 10.0f)) {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "BG missing? Running Actor_delete"
        osSyncPrintf("BG 抜け？ Actor_delete します(%s %d)\n", __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
        Actor_Kill(&this->actor);
    }
}

/**
 * If the fish is on a floor, this function is looped back to by EnFish_Dropped_FlopOnGround to set a new flopping
 * height and whether the sound should play again.
 */
void EnFish_Dropped_SetupFlopOnGround(EnFish* this) {
    s32 pad;
    f32 randomFloat;
    s32 playSound;

    this->actor.gravity = -1.0f;
    this->actor.minVelocityY = -10.0f;
    randomFloat = Rand_ZeroOne();

    if (randomFloat < 0.1f) {
        this->actor.velocity.y = (Rand_ZeroOne() * 3.0f) + 2.5f;
        playSound = true;
    } else if (randomFloat < 0.2f) {
        this->actor.velocity.y = (Rand_ZeroOne() * 1.2f) + 0.2f;
        playSound = true;
    } else {
        this->actor.velocity.y = 0.0f;

        if (Rand_ZeroOne() < 0.2f) {
            playSound = true;
        } else {
            playSound = false;
        }
    }

    this->actor.shape.yOffset = 300.0f;
    EnFish_SetOutOfWaterAnimation(this);
    this->actionFunc = EnFish_Dropped_FlopOnGround;
    this->unk_250 = 5;

    if (playSound && (this->actor.draw != NULL)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_FISH_LEAP);
    }
}

void EnFish_Dropped_FlopOnGround(EnFish* this, PlayState* play) {
    s32 pad;
    s16 frames = play->state.frames;
    s16 targetXRot;

    Math_SmoothStepToF(&this->actor.speedXZ, Rand_ZeroOne() * 0.2f, 0.1f, 0.1f, 0.0f);

    targetXRot = (s16)((((frames >> 5) & 2) | ((frames >> 2) & 1)) << 0xB) * 0.3f;

    if (frames & 4) {
        targetXRot = -targetXRot;
    }

    Math_StepToAngleS(&this->actor.world.rot.x, targetXRot, 4000);
    Math_StepToAngleS(&this->actor.world.rot.z, 0x4000, 1000);
    this->actor.world.rot.y +=
        (s16)(((Math_SinS(this->slowPhase) * 2000.0f) + (Math_SinS(this->fastPhase) * 1000.0f)) * Rand_ZeroOne());
    this->actor.shape.rot = this->actor.world.rot;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer <= 0) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->timer <= 60) {
        // Blink when about to disappear
        if (frames & 4) {
            this->actor.draw = EnFish_Draw;
        } else {
            this->actor.draw = NULL;
        }
    } else if (this->actor.bgCheckFlags & 0x20) { // In water
        EnFish_Dropped_SetupSwimAway(this);
    } else if (this->actor.bgCheckFlags & 1) { // On floor
        EnFish_Dropped_SetupFlopOnGround(this);
    }
}

void EnFish_Dropped_SetupSwimAway(EnFish* this) {
    this->actor.home.pos = this->actor.world.pos;
    this->actor.flags |= ACTOR_FLAG_4;
    this->timer = 200;
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    EnFish_SetInWaterAnimation(this);
    this->actionFunc = EnFish_Dropped_SwimAway;
    this->unk_250 = 5;
}

void EnFish_Dropped_SwimAway(EnFish* this, PlayState* play) {
    s32 pad;

    Math_SmoothStepToF(&this->actor.speedXZ, 2.8f, 0.1f, 0.4f, 0.0f);

    // If touching wall or not in water, turn back and slow down for one frame.
    if ((this->actor.bgCheckFlags & 8) || !(this->actor.bgCheckFlags & 0x20)) {
        this->actor.home.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        this->actor.speedXZ *= 0.5f;
    }

    Math_StepToAngleS(&this->actor.world.rot.x, 0, 1500);
    Math_StepToAngleS(&this->actor.world.rot.y, this->actor.home.rot.y, 3000);
    Math_StepToAngleS(&this->actor.world.rot.z, 0, 1000);

    this->actor.shape.rot = this->actor.world.rot;

    // Raise if on a floor.
    if (this->actor.bgCheckFlags & 1) {
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - 4.0f, 2.0f);
    } else {
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y - 10.0f, 2.0f);
    }

    // Shrink when close to disappearing.
    if (this->timer < 100) {
        Actor_SetScale(&this->actor, this->actor.scale.x * 0.982f);
    }

    this->skelAnime.playSpeed = CLAMP_MAX((this->actor.speedXZ * 1.5f) + 1.0f, 4.0f);
    SkelAnime_Update(&this->skelAnime);

    if (this->timer <= 0) {
        Actor_Kill(&this->actor);
    }
}

// Unique type functions

void EnFish_Unique_SetupSwimIdle(EnFish* this) {
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    this->timer = Rand_S16Offset(5, 35);
    this->unk_250 = 0;
    EnFish_SetInWaterAnimation(this);
    this->actionFunc = EnFish_Unique_SwimIdle;
}

void EnFish_Unique_SwimIdle(EnFish* this, PlayState* play) {
    static f32 speedStopping[] = { 0.0f, 0.04f, 0.09f };
    static f32 speedMoving[] = { 0.5f, 0.1f, 0.15f };
    f32 playSpeed;
    u32 frames = play->gameplayFrames;
    f32* speed;
    s32 pad2;
    f32 extraPlaySpeed;
    s32 pad3;

    if (this->actor.xzDistToPlayer < 60.0f) {
        if (this->timer < 12) {
            speed = speedMoving;
        } else {
            speed = speedStopping;
        }
    } else {
        if (this->timer < 4) {
            speed = speedMoving;
        } else {
            speed = speedStopping;
        }
    }

    EnFish_SetYOffset(this);
    Math_SmoothStepToF(&this->actor.speedXZ, speed[0], speed[1], speed[2], 0.0f);

    extraPlaySpeed = 0.0f;

    if ((EnFish_XZDistanceSquared(&this->actor.world.pos, &this->actor.home.pos) > SQ(15.0f))) {
        if (!Math_ScaledStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                                200)) {
            extraPlaySpeed = 0.5f;
        }
    } else if ((this->timer < 4) && !Math_ScaledStepToS(&this->actor.world.rot.y, frames * 0x80, 100)) {
        extraPlaySpeed = 0.5f;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    playSpeed = (this->actor.speedXZ * 1.2f) + 0.2f + extraPlaySpeed;
    this->skelAnime.playSpeed = CLAMP(playSpeed, 1.5f, 0.5);
    SkelAnime_Update(&this->skelAnime);

    if (this->timer <= 0) {
        this->timer = Rand_S16Offset(5, 80);
    }
}

// Cutscene functions

void EnFish_Cutscene_FlopOnGround(EnFish* this, PlayState* play) {
    f32 sp24 = Math_SinS(this->slowPhase);
    f32 sp20 = Math_SinS(this->fastPhase);

    D_80A17014 += D_80A17018;

    if (D_80A17014 <= 1.0f) {
        D_80A17014 = 1.0f;

        if (Rand_ZeroOne() < 0.1f) {
            D_80A17018 = (Rand_ZeroOne() * 3.0f) + 2.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_FISH_LEAP);
        } else {
            D_80A17018 = 0.0f;
        }
    } else {
        D_80A17018 -= 0.4f;
    }

    this->skelAnime.playSpeed = ((sp24 + sp20) * 0.5f) + 2.0f;
    SkelAnime_Update(&this->skelAnime);
}

void EnFish_Cutscene_WiggleFlyingThroughAir(EnFish* this, PlayState* play) {
    s32 pad;
    f32 sp28 = Math_SinS(this->slowPhase);
    f32 sp24 = Math_SinS(this->fastPhase);

    this->actor.shape.rot.x -= 500;
    this->actor.shape.rot.z += 100;
    Math_StepToF(&D_80A17014, 0.0f, 1.0f);
    this->skelAnime.playSpeed = ((sp28 + sp24) * 0.5f) + 2.0f;
    SkelAnime_Update(&this->skelAnime);
}

void EnFish_UpdateCutscene(EnFish* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    CsCmdActorAction* csAction = play->csCtx.npcActions[1];
    Vec3f startPos;
    Vec3f endPos;
    f32 progress;
    s32 bgId;

    if (csAction == NULL) {
        // "Warning : DEMO ended without dousa (action) 3 termination being called"
        osSyncPrintf("Warning : dousa 3 消滅 が呼ばれずにデモが終了した(%s %d)(arg_data 0x%04x)\n", __FILE__,
                     __LINE__, this->actor.params);
        EnFish_ClearCutsceneData(this);
        Actor_Kill(&this->actor);
        return;
    }

    this->slowPhase += 0x111;
    this->fastPhase += 0x500;

    switch (csAction->action) {
        case 1:
            EnFish_Cutscene_FlopOnGround(this, play);
            break;
        case 2:
            EnFish_Cutscene_WiggleFlyingThroughAir(this, play);
            break;
        case 3:
            // "DEMO fish termination"
            osSyncPrintf("デモ魚消滅\n");
            EnFish_ClearCutsceneData(this);
            Actor_Kill(&this->actor);
            return;
        default:
            // "Improper DEMO action"
            osSyncPrintf("不正なデモ動作(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__, this->actor.params);
            break;
    }

    startPos.x = csAction->startPos.x;
    startPos.y = csAction->startPos.y;
    startPos.z = csAction->startPos.z;
    endPos.x = csAction->endPos.x;
    endPos.y = csAction->endPos.y;
    endPos.z = csAction->endPos.z;

    progress = Environment_LerpWeight(csAction->endFrame, csAction->startFrame, play->csCtx.frames);

    this->actor.world.pos.x = (endPos.x - startPos.x) * progress + startPos.x;
    this->actor.world.pos.y = (endPos.y - startPos.y) * progress + startPos.y + D_80A17014;
    this->actor.world.pos.z = (endPos.z - startPos.z) * progress + startPos.z;

    this->actor.floorHeight = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                          &this->actor, &this->actor.world.pos);
}

// Update functions and Draw

void EnFish_OrdinaryUpdate(EnFish* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    }

    this->slowPhase += 0x111;
    this->fastPhase += 0x500;

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if ((this->actionFunc == NULL) || (this->actionFunc(this, play), (this->actor.update != NULL))) {
        Actor_MoveForward(&this->actor);

        if (this->unk_250 != 0) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 17.5f, 4.0f, 0.0f, this->unk_250);
        }

        if (this->actor.xzDistToPlayer < 70.0f) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        Actor_SetFocus(&this->actor, this->actor.shape.yOffset * 0.01f);

        if (Actor_HasParent(&this->actor, play)) {
            this->actor.parent = NULL;

            if (this->actor.params == FISH_DROPPED) {
                Actor_Kill(&this->actor);
                return;
            }

            EnFish_BeginRespawn(this);
        } else if (EnFish_InBottleRange(this, play)) {
            // GI_MAX in this case allows the player to catch the actor in a bottle
            func_8002F434(&this->actor, play, GI_MAX, 80.0f, 20.0f);
        }
    }
}

void EnFish_RespawningUpdate(EnFish* this, PlayState* play) {
    if (this->actor.params == FISH_SWIMMING_UNIQUE) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if ((this->actionFunc == NULL) || (this->actionFunc(this, play), (this->actor.update != NULL))) {
        Actor_MoveForward(&this->actor);

        if (this->respawnTimer == 20) {
            this->actor.draw = EnFish_Draw;
        } else if (this->respawnTimer == 0) {
            Actor_SetScale(&this->actor, 0.01f);
        } else if (this->respawnTimer < 20) {
            Actor_SetScale(&this->actor, CLAMP_MAX(this->actor.scale.x + 0.001f, 0.01f));
        }
    }
}

void EnFish_Update(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;

    if ((D_80A17010 == NULL) && (this->actor.params == FISH_DROPPED) && (play->csCtx.state != 0) &&
        (play->csCtx.npcActions[1] != NULL)) {
        EnFish_SetCutsceneData(this);
    }

    if ((D_80A17010 != NULL) && (&this->actor == D_80A17010)) {
        EnFish_UpdateCutscene(this, play);
    } else if (this->respawnTimer > 0) {
        this->respawnTimer--;
        EnFish_RespawningUpdate(this, play);
    } else {
        EnFish_OrdinaryUpdate(this, play);
    }
}

void EnFish_Draw(Actor* thisx, PlayState* play) {
    EnFish* this = (EnFish*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, NULL);
    Collider_UpdateSpheres(0, &this->collider);
}
