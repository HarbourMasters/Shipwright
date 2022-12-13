/*
 * File: z_en_fd.c
 * Overlay: ovl_En_Fd
 * Description: Flare Dancer (enflamed form)
 */

#include "z_en_fd.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_fw/object_fw.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_9)

#define FLG_COREDEAD (0x4000)
#define FLG_COREDONE (0x8000)

void EnFd_Init(Actor* thisx, PlayState* play);
void EnFd_Destroy(Actor* thisx, PlayState* play);
void EnFd_Update(Actor* thisx, PlayState* play);
void EnFd_Draw(Actor* thisx, PlayState* play);
void EnFd_Run(EnFd* this, PlayState* play);
void EnFd_SpinAndSpawnFire(EnFd* this, PlayState* play);
void EnFd_Reappear(EnFd* this, PlayState* play);
void EnFd_SpinAndGrow(EnFd* this, PlayState* play);
void EnFd_JumpToGround(EnFd* this, PlayState* play);
void EnFd_WaitForCore(EnFd* this, PlayState* play);
void EnFd_UpdateFlames(EnFd* this);
void EnFd_UpdateDots(EnFd* this);
void EnFd_AddEffect(EnFd*, u8, Vec3f*, Vec3f*, Vec3f*, u8, f32, f32);
void EnFd_DrawDots(EnFd* this, PlayState* play);
void EnFd_DrawFlames(EnFd* this, PlayState* play);
void EnFd_Land(EnFd* this, PlayState* play);

const ActorInit En_Fd_InitVars = {
    ACTOR_EN_FD,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_FW,
    sizeof(EnFd),
    (ActorFunc)EnFd_Init,
    (ActorFunc)EnFd_Destroy,
    (ActorFunc)EnFd_Update,
    (ActorFunc)EnFd_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[12] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040088, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON | BUMP_HOOKABLE,
            OCELEM_ON,
        },
        { 21, { { 1600, 0, 0 }, 5 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 12, { { 1600, 0, 0 }, 5 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 14, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 15, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 16, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 17, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 18, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 19, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 4, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 4, { { 5000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 8, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x01, 0x04 },
            { 0x00040008, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_ON,
        },
        { 8, { { 5000, 0, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    12,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInit = { 24, 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENFD_ANIM_0,
    /* 1 */ ENFD_ANIM_1,
    /* 2 */ ENFD_ANIM_2,
    /* 3 */ ENFD_ANIM_3,
    /* 4 */ ENFD_ANIM_4
} EnFdAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gFlareDancerCastingFireAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, 0.0f },
    { &gFlareDancerBackflipAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerGettingUpAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerChasingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gFlareDancerTwirlAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
};

s32 EnFd_SpawnCore(EnFd* this, PlayState* play) {
    if (this->invincibilityTimer != 0) {
        return false;
    }

    if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FW, this->corePos.x, this->corePos.y,
                           this->corePos.z, 0, this->actor.shape.rot.y, 0, this->runDir) == NULL) {
        return false;
    }

    this->actor.child->colChkInfo.health = this->actor.colChkInfo.health % 8;

    if (this->actor.child->colChkInfo.health == 0) {
        this->actor.child->colChkInfo.health = 8;
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        func_8002DE04(play, &this->actor, this->actor.child);
    }

    this->coreActive = true;

    return true;
}

void EnFd_SpawnChildFire(EnFd* this, PlayState* play, s16 fireCnt, s16 color) {
    s32 i;

    for (i = 0; i < fireCnt; i++) {
        s16 angle = (s16)((((i * 360.0f) / fireCnt) * (0x10000 / 360.0f))) + this->actor.yawTowardsPlayer;
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FD_FIRE, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, angle, 0, (color << 0xF) | i);
    }
}

void EnFd_SpawnDot(EnFd* this, PlayState* play) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    if (this->actionFunc == EnFd_Run || this->actionFunc == EnFd_SpinAndSpawnFire) {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.floorHeight + 4.0f;
        pos.z = this->actor.world.pos.z;
        accel.x = (Rand_ZeroOne() - 0.5f) * 2.0f;
        accel.y = ((Rand_ZeroOne() - 0.5f) * 0.2f) + 0.3f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 2.0f;
        EnFd_AddEffect(this, FD_EFFECT_FLAME, &pos, &velocity, &accel, 8, 0.6f, 0.2f);
    }
}

/**
 * Checks to see if the hammer effect is active, and if it should be applied
 */
s32 EnFd_CheckHammer(EnFd* this, PlayState* play) {
    if (this->actionFunc == EnFd_Reappear || this->actionFunc == EnFd_SpinAndGrow ||
        this->actionFunc == EnFd_JumpToGround || this->actionFunc == EnFd_WaitForCore) {
        return false;
    } else if (play->actorCtx.unk_02 != 0 && this->actor.xzDistToPlayer < 300.0f &&
               this->actor.yDistToPlayer < 60.0f) {
        return true;
    } else {
        return false;
    }
}

s32 EnFd_ColliderCheck(EnFd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    ColliderInfo* info;

    if (this->collider.base.acFlags & AC_HIT || EnFd_CheckHammer(this, play)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->invincibilityTimer != 0) {
            return false;
        }
        info = &this->collider.elements[0].info;
        if (info->acHitInfo != NULL && (info->acHitInfo->toucher.dmgFlags & 0x80)) {
            return false;
        }

        if (!EnFd_SpawnCore(this, play)) {
            return false;
        }
        this->invincibilityTimer = 30;
        this->actor.flags &= ~ACTOR_FLAG_0;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FLAME_DAMAGE);
        Enemy_StartFinishingBlow(play, &this->actor);
        return true;
    } else if (DECR(this->attackTimer) == 0 && this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (this->invincibilityTimer != 0) {
            return false;
        }

        if (this->collider.base.atFlags & AT_BOUNCED) {
            return false;
        }
        this->attackTimer = 30;
        Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        func_8002F71C(play, &this->actor, this->actor.speedXZ + 2.0f, this->actor.yawTowardsPlayer, 6.0f);
    }
    return false;
}

/**
 * Determines if `actor` is within an acceptable range for `this` to be able to "see" `actor`
 * `actor` must be within 400 units of `this`, `actor` must be within +/- 40 degrees facing angle
 * towards `actor`, and there must not be a collision poly between `this` and `actor`
 */
s32 EnFd_CanSeeActor(EnFd* this, Actor* actor, PlayState* play) {
    CollisionPoly* colPoly;
    s32 bgId;
    Vec3f colPoint;
    s16 angle;
    s32 pad;

    // Check to see if `actor` is within 400 units of `this`
    if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &actor->world.pos) > 400.0f) {
        return false;
    }

    // Check to see if the angle between this facing angle and `actor` is withing ~40 degrees
    angle = (f32)Math_Vec3f_Yaw(&this->actor.world.pos, &actor->world.pos) - this->actor.shape.rot.y;
    if (ABS(angle) > 0x1C70) {
        return false;
    }

    // check to see if the line between `this` and `actor` does not intersect a collision poly
    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &actor->world.pos, &colPoint, &colPoly,
                                true, false, false, true, &bgId)) {
        return false;
    }

    return true;
}

Actor* EnFd_FindBomb(EnFd* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (actor != NULL) {
        if (actor->params != 0 || actor->parent != NULL) {
            actor = actor->next;
            continue;
        }

        if (actor->id != ACTOR_EN_BOM) {
            actor = actor->next;
            continue;
        }

        if (EnFd_CanSeeActor(this, actor, play) != 1) {
            actor = actor->next;
            continue;
        }

        return actor;
    }
    return NULL;
}

Actor* EnFd_FindPotentialTheat(EnFd* this, PlayState* play) {
    Player* player;
    Actor* bomb = EnFd_FindBomb(this, play);

    if (bomb != NULL) {
        return bomb;
    }

    if (this->attackTimer != 0) {
        return NULL;
    }

    player = GET_PLAYER(play);
    if (!EnFd_CanSeeActor(this, &player->actor, play)) {
        return NULL;
    }

    return &player->actor;
}

/**
 * Creates a delta in `dst` for the position from `this`'s current position to the next
 * position in a circle formed by `radius` with center at `this`'s initial position.
 */
Vec3f* EnFd_GetPosAdjAroundCircle(Vec3f* dst, EnFd* this, f32 radius, s16 dir) {
    s16 angle;
    Vec3f newPos;

    angle = Math_Vec3f_Yaw(&this->actor.home.pos, &this->actor.world.pos) + (dir * 0x1554); // ~30 degrees
    newPos.x = (Math_SinS(angle) * radius) + this->actor.home.pos.x;
    newPos.z = (Math_CosS(angle) * radius) + this->actor.home.pos.z;
    newPos.x -= this->actor.world.pos.x;
    newPos.z -= this->actor.world.pos.z;
    *dst = newPos;
    return dst;
}

s32 EnFd_ShouldStopRunning(EnFd* this, PlayState* play, f32 radius, s16* runDir) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f colPoint;
    Vec3f pos;

    // Check to see if the next position on the rotation around the circle
    // will result in a background collision
    EnFd_GetPosAdjAroundCircle(&pos, this, radius, *runDir);

    pos.x += this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z += this->actor.world.pos.z;

    if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &pos, &colPoint, &poly, true, false, false,
                                true, &bgId)) {
        *runDir = -*runDir;
        return true;
    }

    if (this->circlesToComplete != 0 || DECR(this->spinTimer) != 0) {
        return false;
    }

    if (Rand_ZeroOne() > 0.5f) {
        *runDir = -*runDir;
    }
    return true;
}

void EnFd_Fade(EnFd* this, PlayState* play) {
    if (this->invincibilityTimer != 0) {
        Math_SmoothStepToF(&this->fadeAlpha, 0.0f, 0.3f, 10.0f, 0.0f);
        this->actor.shape.shadowAlpha = this->fadeAlpha;
        if (!(this->fadeAlpha >= 0.9f)) {
            this->invincibilityTimer = 0;
            this->spinTimer = 0;
            this->actionFunc = EnFd_WaitForCore;
            this->actor.speedXZ = 0.0f;
        }
    }
}

void EnFd_Init(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;

    SkelAnime_InitFlex(play, &this->skelAnime, &gFlareDancerSkel, NULL, this->jointTable, this->morphTable, 27);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 32.0f);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colSphs);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0xF), &sColChkInit);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.flags |= ACTOR_FLAG_24;
    Actor_SetScale(&this->actor, 0.01f);
    this->firstUpdateFlag = true;
    this->actor.gravity = -1.0f;
    this->runDir = Rand_ZeroOne() < 0.5f ? -1 : 1;
    this->actor.naviEnemyId = 0x22;
    this->actionFunc = EnFd_Reappear;
}

void EnFd_Destroy(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnFd_Reappear(EnFd* this, PlayState* play) {
    this->actor.world.pos = this->actor.home.pos;
    this->actor.params = 0;
    this->actor.shape.shadowAlpha = 0xFF;
    this->coreActive = false;
    this->actor.scale.y = 0.0f;
    this->fadeAlpha = 255.0f;
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_0);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FLAME_LAUGH);
    this->actionFunc = EnFd_SpinAndGrow;
}

void EnFd_SpinAndGrow(EnFd* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->actor.velocity.y = 6.0f;
        this->actor.scale.y = 0.01f;
        this->actor.world.rot.y ^= 0x8000;
        this->actor.flags |= ACTOR_FLAG_0;
        this->actor.speedXZ = 8.0f;
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_1);
        this->actionFunc = EnFd_JumpToGround;
    } else {
        this->actor.scale.y = this->skelAnime.curFrame * (0.01f / this->skelAnime.animLength);
        this->actor.shape.rot.y += 0x2000;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnFd_JumpToGround(EnFd* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & 1) && !(this->actor.velocity.y > 0.0f)) {
        this->actor.velocity.y = 0.0f;
        this->actor.speedXZ = 0.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_2);
        this->actionFunc = EnFd_Land;
    }
}

void EnFd_Land(EnFd* this, PlayState* play) {
    Vec3f adjPos;

    Math_SmoothStepToF(&this->skelAnime.playSpeed, 1.0f, 0.1f, 1.0f, 0.0f);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->spinTimer = Rand_S16Offset(60, 90);
        this->runRadius = Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos);
        EnFd_GetPosAdjAroundCircle(&adjPos, this, this->runRadius, this->runDir);
        this->actor.world.rot.y = Math_FAtan2F(adjPos.x, adjPos.z) * (0x8000 / M_PI);
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_4);
        this->actionFunc = EnFd_SpinAndSpawnFire;
    }
}

void EnFd_SpinAndSpawnFire(EnFd* this, PlayState* play) {
    f32 deceleration;
    f32 tgtSpeed;
    f32 rotSpeed;

    if ((this->spinTimer < 31) && (this->invincibilityTimer == 0)) {
        func_8002F974(&this->actor, NA_SE_EN_FLAME_FIRE_ATTACK - SFX_FLAG);
    } else {
        func_8002F974(&this->actor, NA_SE_EN_FLAME_ROLL - SFX_FLAG);
    }

    if (DECR(this->spinTimer) != 0) {
        this->actor.shape.rot.y += (this->runDir * 0x2000);
        if (this->spinTimer == 30 && this->invincibilityTimer == 0) {
            if (this->actor.xzDistToPlayer > 160.0f) {
                // orange flames
                EnFd_SpawnChildFire(this, play, 8, 0);
            } else {
                // blue flames
                EnFd_SpawnChildFire(this, play, 8, 1);
            }
        }
    } else {
        // slow shape rotation down to meet `this` rotation within ~1.66 degrees
        deceleration = this->actor.world.rot.y;
        deceleration -= this->actor.shape.rot.y;
        rotSpeed = 0.0f;
        tgtSpeed = fabsf(deceleration);
        deceleration /= tgtSpeed;
        Math_ApproachF(&rotSpeed, tgtSpeed, 0.6f, 0x2000);
        rotSpeed *= deceleration;
        this->actor.shape.rot.y += (s16)rotSpeed;
        rotSpeed = fabsf(rotSpeed);
        if ((s32)rotSpeed <= 300) {
            // ~1.6 degrees
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }

        if (this->actor.shape.rot.y == this->actor.world.rot.y) {
            this->initYawToInitPos = Math_Vec3f_Yaw(&this->actor.home.pos, &this->actor.world.pos);
            this->curYawToInitPos = this->runDir < 0 ? 0xFFFF : 0;
            this->circlesToComplete = (play->state.frames & 7) + 2;
            this->spinTimer = Rand_S16Offset(30, 120);
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_3);
            this->actionFunc = EnFd_Run;
        }
    }
}

/**
 * Run around in a circle with the center being the initial position, and
 * the radius being the distance from the initial position to the nearest
 * threat (bomb or player).
 */
void EnFd_Run(EnFd* this, PlayState* play) {
    Actor* potentialThreat;
    s16 yawToYawTarget;
    f32 runRadiusTarget;
    Vec3f adjPos;

    if (EnFd_ShouldStopRunning(this, play, this->runRadius, &this->runDir)) {
        if (this->invincibilityTimer == 0) {
            this->actor.world.rot.y ^= 0x8000;
            this->actor.velocity.y = 6.0f;
            this->actor.speedXZ = 0.0f;
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENFD_ANIM_1);
            this->actionFunc = EnFd_JumpToGround;
            return;
        }
    }

    yawToYawTarget = Math_Vec3f_Yaw(&this->actor.home.pos, &this->actor.world.pos) - this->initYawToInitPos;
    if (this->runDir > 0) {
        if ((u16)this->curYawToInitPos > (u16)(yawToYawTarget)) {
            this->circlesToComplete--;
        }
    } else if ((u16)this->curYawToInitPos < (u16)(yawToYawTarget)) {
        this->circlesToComplete--;
    }

    if (this->circlesToComplete < 0) {
        this->circlesToComplete = 0;
    }
    this->curYawToInitPos = yawToYawTarget;

    // If there is a bomb out, or if the player exists, set radius to
    // the distance to that threat, otherwise default to 200.
    potentialThreat = EnFd_FindPotentialTheat(this, play);
    if ((potentialThreat != NULL) && (this->invincibilityTimer == 0)) {
        runRadiusTarget = Math_Vec3f_DistXYZ(&this->actor.home.pos, &potentialThreat->world.pos);
    } else {
        runRadiusTarget = 200.0f;
    }
    Math_SmoothStepToF(&this->runRadius, runRadiusTarget, 0.3f, 100.0f, 0.0f);
    EnFd_GetPosAdjAroundCircle(&adjPos, this, this->runRadius, this->runDir);
    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(adjPos.x, adjPos.z) * (0x8000 / M_PI), 4, 0xFA0, 1);
    this->actor.world.rot = this->actor.shape.rot;
    func_8002F974(&this->actor, NA_SE_EN_FLAME_RUN - SFX_FLAG);
    if (this->skelAnime.curFrame == 6.0f || this->skelAnime.curFrame == 13.0f || this->skelAnime.curFrame == 28.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FLAME_KICK);
    }
    Math_SmoothStepToF(&this->actor.speedXZ, 8.0f, 0.1f, 1.0f, 0.0f);
}

/**
 * En_Fw will set `this` params when it is done with its action.
 * It will set FLG_COREDONE when the core has returned to `this`'s initial
 * position, and FLG_COREDEAD when there is no health left
 */
void EnFd_WaitForCore(EnFd* this, PlayState* play) {
    if (this->spinTimer != 0) {
        this->spinTimer--;
        if (this->spinTimer == 0) {
            Actor_Kill(&this->actor);
        }
    } else if (this->actor.params & FLG_COREDONE) {
        this->actionFunc = EnFd_Reappear;
    } else if (this->actor.params & FLG_COREDEAD) {
        this->actor.params = 0;
        this->spinTimer = 30;
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_FLARE_DANCER]++;
    }
}

void EnFd_Update(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;
    s32 pad;

    if (this->firstUpdateFlag) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
        this->firstUpdateFlag = false;
    }

    if (this->actionFunc != EnFd_Reappear) {
        SkelAnime_Update(&this->skelAnime);
        EnFd_SpawnDot(this, play);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        // has been hookshoted
        if (EnFd_SpawnCore(this, play)) {
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->invincibilityTimer = 30;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FLAME_DAMAGE);
            Enemy_StartFinishingBlow(play, &this->actor);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_13;
        }
    } else if (this->actionFunc != EnFd_WaitForCore) {
        EnFd_ColliderCheck(this, play);
    }
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    EnFd_Fade(this, play);
    this->actionFunc(this, play);
    EnFd_UpdateDots(this);
    EnFd_UpdateFlames(this);
    if (this->actionFunc != EnFd_Reappear && this->actionFunc != EnFd_SpinAndGrow &&
        this->actionFunc != EnFd_WaitForCore) {
        if (this->attackTimer == 0 && this->invincibilityTimer == 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((this->actionFunc == EnFd_Run) || (this->actionFunc == EnFd_SpinAndSpawnFire)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnFd_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfxP) {
    EnFd* this = (EnFd*)thisx;

    if (this->invincibilityTimer != 0) {
        switch (limbIndex) {
            case 13:
            case 21:
                *dList = NULL;
                break;
        }
    }

    return false;
}

void EnFd_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnFd* this = (EnFd*)thisx;
    Vec3f unused0 = { 6800.0f, 0.0f, 0.0f };
    Vec3f unused1 = { 6800.0f, 0.0f, 0.0f };
    Vec3f initialPos = { 0.0f, 0.0f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 i;

    if (limbIndex == 21) {
        Matrix_MultVec3f(&initialPos, &this->corePos);
    }

    if (limbIndex == 13) {
        Matrix_MultVec3f(&initialPos, &this->actor.focus.pos);
    }

    if (limbIndex == 3 || limbIndex == 6 || limbIndex == 7 || limbIndex == 10 || limbIndex == 14 || limbIndex == 15 ||
        limbIndex == 17 || limbIndex == 18 || limbIndex == 20 || limbIndex == 22 || limbIndex == 23 ||
        limbIndex == 24 || limbIndex == 25 || limbIndex == 26) {
        if ((play->state.frames % 2) != 0) {
            for (i = 0; i < 1; i++) {
                Matrix_MultVec3f(&initialPos, &pos);
                pos.x += (Rand_ZeroOne() - 0.5f) * 20.0f;
                pos.y += (Rand_ZeroOne() - 0.5f) * 40.0f;
                pos.z += (Rand_ZeroOne() - 0.5f) * 20.0f;
                accel.x = (Rand_ZeroOne() - 0.5f) * 0.4f;
                accel.y = ((Rand_ZeroOne() - 0.5f) * 0.2f) + 0.6f;
                accel.z = (Rand_ZeroOne() - 0.5f) * 0.4f;
                EnFd_AddEffect(this, FD_EFFECT_DOT, &pos, &velocity, &accel, 0, 0.006f, 0.0f);
            }
        }
    }

    Collider_UpdateSpheres(limbIndex, &this->collider);
}

void EnFd_Draw(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;
    s32 clampedHealth;
    Color_RGBA8 primColors[] = {
        { 255, 255, 200, 255 },
        { 200, 200, 200, 255 },
        { 255, 255, 0, 255 },
    };
    Color_RGBA8 envColors[] = {
        { 0, 255, 0, 255 },
        { 0, 0, 255, 255 },
        { 255, 0, 0, 255 },
    };
    u32 frames;
    s32 pad;

    frames = play->state.frames;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();
    EnFd_DrawDots(this, play);
    EnFd_DrawFlames(this, play);
    Matrix_Pop();
    if (this->actionFunc != EnFd_Reappear && !(this->fadeAlpha < 0.9f)) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        clampedHealth = CLAMP(thisx->colChkInfo.health - 1, 0, 23);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, primColors[clampedHealth / 8].r, primColors[clampedHealth / 8].g,
                        primColors[clampedHealth / 8].b, (u8)this->fadeAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, envColors[clampedHealth / 8].r, envColors[clampedHealth / 8].g,
                       envColors[clampedHealth / 8].b, (u8)this->fadeAlpha);
        gSPSegment(
            POLY_XLU_DISP++, 0x8,
            Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, 0xFF - (u8)(frames * 6), 8, 0x40));
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x9, D_80116280);

        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, EnFd_OverrideLimbDraw, EnFd_PostLimbDraw, this,
                                           POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFd_AddEffect(EnFd* this, u8 type, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 timer, f32 scale,
                    f32 scaleStep) {
    EnFdEffect* eff = this->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, eff++) {
        if (eff->type != FD_EFFECT_NONE) {
            continue;
        }
        eff->scale = scale;
        eff->scaleStep = scaleStep;
        eff->initialTimer = eff->timer = timer;
        eff->type = type;
        eff->pos = *pos;
        eff->accel = *accel;
        eff->velocity = *velocity;
        if (eff->type == FD_EFFECT_DOT) {
            eff->color.a = 255;
            eff->timer = (s16)(Rand_ZeroOne() * 10.0f);
        }
        eff->epoch++;
        return;
    }
}

void EnFd_UpdateFlames(EnFd* this) {
    s16 i;
    EnFdEffect* eff = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_FLAME) {
            eff->timer--;
            if (eff->timer == 0) {
                eff->type = FD_EFFECT_NONE;
            }
            eff->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;
            eff->scale += eff->scaleStep;
        }
    }
}

void EnFd_UpdateDots(EnFd* this) {
    EnFdEffect* eff = this->effects;
    s16 i;
    Color_RGBA8 dotColors[] = {
        { 255, 128, 0, 0 },
        { 255, 0, 0, 0 },
        { 255, 255, 0, 0 },
        { 255, 0, 0, 0 },
    };

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_DOT) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->timer++;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            eff->color.r = dotColors[eff->timer % 4].r;
            eff->color.g = dotColors[eff->timer % 4].g;
            eff->color.b = dotColors[eff->timer % 4].b;
            if (eff->color.a > 30) {
                eff->color.a -= 30;
            } else {
                eff->color.a = 0;
                eff->type = FD_EFFECT_NONE;
            }
        }
    }
}

void EnFd_DrawFlames(EnFd* this, PlayState* play) {
    static void* dustTextures[] = {
        gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
    };
    s32 firstDone;
    s16 i;
    s16 idx;
    EnFdEffect* eff = this->effects;

    OPEN_DISPS(play->state.gfxCtx);
    firstDone = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_FLAME) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);

            if (!firstDone) {
                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_7928);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, (u8)((this->fadeAlpha / 255.0f) * 255));
                firstDone = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (u8)((this->fadeAlpha / 255.0f) * 255));
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            idx = eff->timer * (8.0f / eff->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(dustTextures[idx]));
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerSquareParticleDL);

            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFd_DrawDots(EnFd* this, PlayState* play) {
    s16 i;
    s16 firstDone;
    EnFdEffect* eff = this->effects;

    OPEN_DISPS(play->state.gfxCtx);

    firstDone = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, eff++) {
        if (eff->type == FD_EFFECT_DOT) {
            FrameInterpolation_RecordOpenChild(eff, eff->epoch);

            if (!firstDone) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_79F8);
                firstDone = true;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b,
                            (u8)(eff->color.a * (this->fadeAlpha / 255.0f)));
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerTriangleParticleDL);
            
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
