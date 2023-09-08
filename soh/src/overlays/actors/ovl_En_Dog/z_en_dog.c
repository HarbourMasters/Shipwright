/*
 * File: z_en_dog.c
 * Overlay: ovl_En_Dog
 * Description: Dog
 */

#include "z_en_dog.h"
#include "objects/object_dog/object_dog.h"

#define FLAGS 0

void EnDog_Init(Actor* thisx, PlayState* play);
void EnDog_Destroy(Actor* thisx, PlayState* play);
void EnDog_Update(Actor* thisx, PlayState* play);
void EnDog_Draw(Actor* thisx, PlayState* play);

void EnDog_FollowPath(EnDog* this, PlayState* play);
void EnDog_ChooseMovement(EnDog* this, PlayState* play);
void EnDog_FollowPlayer(EnDog* this, PlayState* play);
void EnDog_RunAway(EnDog* this, PlayState* play);
void EnDog_FaceLink(EnDog* this, PlayState* play);
void EnDog_Wait(EnDog* this, PlayState* play);

const ActorInit En_Dog_InitVars = {
    ACTOR_EN_DOG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DOG,
    sizeof(EnDog),
    (ActorFunc)EnDog_Init,
    (ActorFunc)EnDog_Destroy,
    (ActorFunc)EnDog_Update,
    (ActorFunc)EnDog_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 16, 20, 0, { 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, 50 };

typedef enum {
    /* 0 */ ENDOG_ANIM_0,
    /* 1 */ ENDOG_ANIM_1,
    /* 2 */ ENDOG_ANIM_2,
    /* 3 */ ENDOG_ANIM_3,
    /* 4 */ ENDOG_ANIM_4,
    /* 5 */ ENDOG_ANIM_5,
    /* 6 */ ENDOG_ANIM_6,
    /* 7 */ ENDOG_ANIM_7
} EnDogAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gDogWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogRunAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogBarkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
    { &gDogSitAnim, 1.0f, 0.0f, 4.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogSitAnim, 1.0f, 5.0f, 25.0f, ANIMMODE_LOOP_PARTIAL, -6.0f },
    { &gDogBowAnim, 1.0f, 0.0f, 6.0f, ANIMMODE_ONCE, -6.0f },
    { &gDogBow2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -6.0f },
};

typedef enum {
    /* 0x00 */ DOG_WALK,
    /* 0x01 */ DOG_RUN,
    /* 0x02 */ DOG_BARK,
    /* 0x03 */ DOG_SIT,
    /* 0x04 */ DOG_SIT_2,
    /* 0x05 */ DOG_BOW,
    /* 0x06 */ DOG_BOW_2
} DogBehavior;

void EnDog_PlayWalkSFX(EnDog* this) {
    AnimationHeader* walk = &gDogWalkAnim;

    if (this->skelAnime.animation == walk) {
        if ((this->skelAnime.curFrame == 1.0f) || (this->skelAnime.curFrame == 7.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void EnDog_PlayRunSFX(EnDog* this) {
    AnimationHeader* run = &gDogRunAnim;

    if (this->skelAnime.animation == run) {
        if ((this->skelAnime.curFrame == 2.0f) || (this->skelAnime.curFrame == 4.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    }
}

void EnDog_PlayBarkSFX(EnDog* this) {
    AnimationHeader* bark = &gDogBarkAnim;

    if (this->skelAnime.animation == bark) {
        if ((this->skelAnime.curFrame == 13.0f) || (this->skelAnime.curFrame == 19.0f)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
        }
    }
}

s32 EnDog_PlayAnimAndSFX(EnDog* this) {
    s32 animation;

    if (this->behavior != this->nextBehavior) {
        if (this->nextBehavior == DOG_SIT_2) {
            this->nextBehavior = DOG_SIT;
        }
        if (this->nextBehavior == DOG_BOW_2) {
            this->nextBehavior = DOG_BOW;
        }

        this->behavior = this->nextBehavior;
        switch (this->behavior) {
            case DOG_WALK:
                animation = ENDOG_ANIM_1;
                break;
            case DOG_RUN:
                animation = ENDOG_ANIM_2;
                break;
            case DOG_BARK:
                animation = ENDOG_ANIM_3;
                break;
            case DOG_SIT:
                animation = ENDOG_ANIM_4;
                break;
            case DOG_BOW:
                animation = ENDOG_ANIM_6;
                break;
        }
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, animation);
    }

    switch (this->behavior) {
        case DOG_SIT:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDOG_ANIM_5);
                this->behavior = this->nextBehavior = DOG_SIT_2;
            }
            break;
        case DOG_BOW:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDOG_ANIM_7);
                this->behavior = this->nextBehavior = DOG_BOW_2;
            }
            break;
        case DOG_WALK:
            EnDog_PlayWalkSFX(this);
            break;
        case DOG_RUN:
            EnDog_PlayRunSFX(this);
            break;
        case DOG_BARK:
            EnDog_PlayBarkSFX(this);
            break;
    }
    return 0;
}

s8 EnDog_CanFollow(EnDog* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return 2;
    }

    if (play->sceneNum == SCENE_MARKET_DAY) {
        return 0;
    }

    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        if (gSaveContext.dogParams != 0) {
            return 0;
        }
        gSaveContext.dogParams = (this->actor.params & 0x7FFF);
        return 1;
    }

    return 0;
}

s32 EnDog_UpdateWaypoint(EnDog* this, PlayState* play) {
    s32 change;

    if (this->path == NULL) {
        return 0;
    }

    if (this->reverse) {
        change = -1;
    } else {
        change = 1;
    }

    this->waypoint += change;

    if (this->reverse) {
        if (this->waypoint < 0) {
            this->waypoint = this->path->count - 1;
        }
    } else {
        if ((this->path->count - 1) < this->waypoint) {
            this->waypoint = 0;
        }
    }

    return 1;
}

s32 EnDog_Orient(EnDog* this, PlayState* play) {
    s16 targetYaw;
    f32 waypointDistSq;

    waypointDistSq = Path_OrientAndGetDistSq(&this->actor, this->path, this->waypoint, &targetYaw);
    Math_SmoothStepToS(&this->actor.world.rot.y, targetYaw, 10, 1000, 1);

    if ((waypointDistSq > 0.0f) && (waypointDistSq < 1000.0f)) {
        return EnDog_UpdateWaypoint(this, play);
    } else {
        return 0;
    }
}

void EnDog_Init(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    s16 followingDog;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDogSkel, NULL, this->jointTable, this->morphTable, 13);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENDOG_ANIM_0);

    if ((this->actor.params & 0x8000) == 0) {
        this->actor.params = (this->actor.params & 0xF0FF) | ((((this->actor.params & 0x0F00) >> 8) + 1) << 8);
    }

    followingDog = ((gSaveContext.dogParams & 0x0F00) >> 8);
    if (followingDog == ((this->actor.params & 0x0F00) >> 8) && ((this->actor.params & 0x8000) == 0)) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, 0, &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.0075f);
    this->waypoint = 0;
    this->actor.gravity = -1.0f;
    this->path = Path_GetByIndex(play, (this->actor.params & 0x00F0) >> 4, 0xF);

    switch (play->sceneNum) {
        case SCENE_MARKET_NIGHT:
            if ((!gSaveContext.dogIsLost) && (((this->actor.params & 0x0F00) >> 8) == 1)) {
                Actor_Kill(&this->actor);
            }
            break;
        case SCENE_DOG_LADY_HOUSE: // Richard's Home
            if (!(this->actor.params & 0x8000)) {
                if (!gSaveContext.dogIsLost) {
                    this->nextBehavior = DOG_SIT;
                    this->actionFunc = EnDog_Wait;
                    this->actor.speedXZ = 0.0f;
                    return;
                } else {
                    Actor_Kill(&this->actor);
                    return;
                }
            }
            break;
    }

    if (this->actor.params & 0x8000) {
        this->nextBehavior = DOG_WALK;
        this->actionFunc = EnDog_FollowPlayer;
    } else {
        this->nextBehavior = DOG_SIT;
        this->actionFunc = EnDog_ChooseMovement;
    }
}

void EnDog_Destroy(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnDog_FollowPath(EnDog* this, PlayState* play) {
    s32 behaviors[] = { DOG_SIT, DOG_BOW, DOG_BARK };
    s32 unused[] = { 40, 80, 20 };
    f32 speed;
    s32 frame;

    if (EnDog_CanFollow(this, play) == 1) {
        this->actionFunc = EnDog_FollowPlayer;
    }

    if (DECR(this->behaviorTimer) != 0) {
        if (this->nextBehavior == DOG_WALK) {
            speed = 1.0f;
        } else {
            speed = 4.0f;
        }
        Math_SmoothStepToF(&this->actor.speedXZ, speed, 0.4f, 1.0f, 0.0f);
        EnDog_Orient(this, play);
        this->actor.shape.rot = this->actor.world.rot;

        // Used to change between two text boxes for Richard's owner in the Market Day scene
        // depending on where he is on his path. En_Hy checks these event flags.
        if (this->waypoint < 9) {
            // Richard is close to her, text says something about his coat
            gSaveContext.eventInf[3] |= 1;
        } else {
            // Richard is far, text says something about running fast
            gSaveContext.eventInf[3] &= ~1;
        }
    } else {
        frame = play->state.frames % 3;
        this->nextBehavior = behaviors[frame];
        // no clue why they're using the behavior id to calculate timer. possibly meant to use the unused array?
        this->behaviorTimer = Rand_S16Offset(60, behaviors[frame]);
        this->actionFunc = EnDog_ChooseMovement;
    }
}

void EnDog_ChooseMovement(EnDog* this, PlayState* play) {
    if (EnDog_CanFollow(this, play) == 1) {
        this->actionFunc = EnDog_FollowPlayer;
    }

    if (DECR(this->behaviorTimer) == 0) {
        this->behaviorTimer = Rand_S16Offset(200, 100);
        if (play->state.frames % 2) {
            this->nextBehavior = DOG_WALK;
        } else {
            this->nextBehavior = DOG_RUN;
        }

        if (this->nextBehavior == DOG_RUN) {
            this->behaviorTimer /= 2;
        }
        this->actionFunc = EnDog_FollowPath;
    }
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.4f, 1.0f, 0.0f);
}

void EnDog_FollowPlayer(EnDog* this, PlayState* play) {
    f32 speed;

    if (gSaveContext.dogParams == 0) {
        this->nextBehavior = DOG_SIT;
        this->actionFunc = EnDog_Wait;
        this->actor.speedXZ = 0.0f;
        return;
    }

    if (CVarGetInteger("gDogFollowsEverywhere", 0)) {
        // If the dog is too far away it's usually because they are stuck in a hole or on a different floor, this gives them a push
        if (this->actor.xyzDistToPlayerSq > 250000.0f) {
            Player* player = GET_PLAYER(play);
            if (PlayerGrounded(player)) this->actor.world.pos.y = player->actor.world.pos.y;
        }

        // If doggo is in the water make sure it's floating
        if (this->actor.bgCheckFlags & 0x20) {
            this->actor.gravity = 0.0f;
            if (this->actor.yDistToWater > 11.0f) {
                this->actor.world.pos.y += 2.0f;
            } else if (this->actor.yDistToWater < 8.0f) {
                this->actor.world.pos.y -= 2.0f;
            }
        } else {
            this->actor.gravity = -1.0f;
        }
    }

    if (this->actor.xzDistToPlayer > 400.0f) {
        if (CVarGetInteger("gDogFollowsEverywhere", 0)) {
            // Instead of stopping following when the dog gets too far, just speed them up.
            speed = this->actor.xzDistToPlayer / 25.0f;
        } else {
            if (this->nextBehavior != DOG_SIT && this->nextBehavior != DOG_SIT_2) {
                this->nextBehavior = DOG_BOW;
            }
            gSaveContext.dogParams = 0;
            speed = 0.0f;
        }
    } else if (this->actor.xzDistToPlayer > 100.0f) {
        this->nextBehavior = DOG_RUN;
        speed = 4.0f;
    } else if (this->actor.xzDistToPlayer < 40.0f) {
        if (this->nextBehavior != DOG_BOW && this->nextBehavior != DOG_BOW_2) {
            this->nextBehavior = DOG_BOW;
        }
        speed = 0.0f;
    } else {
        this->nextBehavior = DOG_WALK;
        speed = 1.0f;
    }

    Math_ApproachF(&this->actor.speedXZ, speed, 0.6f, 1.0f);

    if (!(this->actor.xzDistToPlayer > 400.0f) || CVarGetInteger("gDogFollowsEverywhere", 0)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 1000, 1);
        this->actor.shape.rot = this->actor.world.rot;
    }
}

void EnDog_RunAway(EnDog* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 200.0f) {
        Math_ApproachF(&this->actor.speedXZ, 4.0f, 0.6f, 1.0f);
        Math_SmoothStepToS(&this->actor.world.rot.y, (this->actor.yawTowardsPlayer ^ 0x8000), 10, 1000, 1);
    } else {
        this->actionFunc = EnDog_FaceLink;
    }
    this->actor.shape.rot = this->actor.world.rot;
}

void EnDog_FaceLink(EnDog* this, PlayState* play) {
    s16 rotTowardLink;
    s16 prevRotY;
    f32 absAngleDiff;

    // if the dog is more than 200 units away from Link, turn to face him then wait
    if (200.0f <= this->actor.xzDistToPlayer) {
        this->nextBehavior = DOG_WALK;

        Math_ApproachF(&this->actor.speedXZ, 1.0f, 0.6f, 1.0f);

        rotTowardLink = this->actor.yawTowardsPlayer;
        prevRotY = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.world.rot.y, rotTowardLink, 10, 1000, 1);

        absAngleDiff = this->actor.world.rot.y;
        absAngleDiff -= prevRotY;
        absAngleDiff = fabsf(absAngleDiff);
        if (absAngleDiff < 200.0f) {
            this->nextBehavior = DOG_SIT;
            this->actionFunc = EnDog_Wait;
            this->actor.speedXZ = 0.0f;
        }
    } else {
        this->nextBehavior = DOG_RUN;
        this->actionFunc = EnDog_RunAway;
    }
    this->actor.shape.rot = this->actor.world.rot;
}

void EnDog_Wait(EnDog* this, PlayState* play) {
    this->unusedAngle = (this->actor.yawTowardsPlayer - this->actor.shape.rot.y);

    // If another dog is following Link and he gets within 200 units of waiting dog, run away
    if ((gSaveContext.dogParams != 0) && (this->actor.xzDistToPlayer < 200.0f)) {
        this->nextBehavior = DOG_RUN;
        this->actionFunc = EnDog_RunAway;
    }
}

void EnDog_Update(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    s32 pad;

    EnDog_PlayAnimAndSFX(this);
    SkelAnime_Update(&this->skelAnime);
    Actor_UpdateBgCheckInfo(play, &this->actor, this->collider.dim.radius, this->collider.dim.height * 0.5f, 0.0f,
                            5);
    Actor_MoveForward(&this->actor);
    this->actionFunc(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnDog_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return false;
}

void EnDog_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void EnDog_Draw(Actor* thisx, PlayState* play) {
    EnDog* this = (EnDog*)thisx;
    Color_RGB8 colors[] = { { 255, 255, 200 }, { 150, 100, 50 } };

    if (CVarGetInteger("gCosmetics.NPC_Dog1.Changed", 0)) {
        colors[0] = CVarGetColor24("gCosmetics.NPC_Dog1.Value", colors[0]);
    }
    if (CVarGetInteger("gCosmetics.NPC_Dog2.Changed", 0)) {
        colors[1] = CVarGetColor24("gCosmetics.NPC_Dog2.Value", colors[1]);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, colors[this->actor.params & 0xF].r, colors[this->actor.params & 0xF].g,
                   colors[this->actor.params & 0xF].b, 0);

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDog_OverrideLimbDraw, EnDog_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx);
}
