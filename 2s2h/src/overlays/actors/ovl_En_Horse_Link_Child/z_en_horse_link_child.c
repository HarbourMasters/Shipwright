/*
 * File: z_en_horse_link_child.c
 * Overlay: ovl_En_Horse_Link_Child
 * Description: Child Epona from OoT, when player learns Epona's song
 *   (Unused in MM and broken)
 */

#include "z_en_horse_link_child.h"
#include "z64horse.h"
#include "objects/object_horse_link_child/object_horse_link_child.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnHorseLinkChild_Init(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Destroy(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Update(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Draw(Actor* thisx, PlayState* play);

void EnHorseLinkChild_ActionFunc0(EnHorseLinkChild* this, PlayState* play);
void EnHorseLinkChild_SetupWaitForPlayer(EnHorseLinkChild* this, s32 animIndex);
void EnHorseLinkChild_WaitForPlayer(EnHorseLinkChild* this, PlayState* play);
void EnHorseLinkChild_SetupGreetPlayer(EnHorseLinkChild* this);
void EnHorseLinkChild_GreetPlayer(EnHorseLinkChild* this, PlayState* play);
void EnHorseLinkChild_SetupLonLonIdle(EnHorseLinkChild* this);
void EnHorseLinkChild_LonLonIdle(EnHorseLinkChild* this, PlayState* play);
void EnHorseLinkChild_SetupActionFunc5(EnHorseLinkChild* this);
void EnHorseLinkChild_ActionFunc5(EnHorseLinkChild* this, PlayState* play);
void EnHorseLinkChild_SetupActionFunc4(EnHorseLinkChild* this);
void EnHorseLinkChild_ActionFunc4(EnHorseLinkChild* this, PlayState* play);

ActorProfile En_Horse_Link_Child_Profile = {
    /**/ ACTOR_EN_HORSE_LINK_CHILD,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE_LINK_CHILD,
    /**/ sizeof(EnHorseLinkChild),
    /**/ EnHorseLinkChild_Init,
    /**/ EnHorseLinkChild_Destroy,
    /**/ EnHorseLinkChild_Update,
    /**/ EnHorseLinkChild_Draw,
};

typedef enum {
    /* 0 */ OOT_CHILD_EPONA_ANIM_IDLE,   // head down + pawing
    /* 1 */ OOT_CHILD_EPONA_ANIM_WHINNY, // head shake
    /* 2 */ OOT_CHILD_EPONA_ANIM_WALK,   // slow
    /* 3 */ OOT_CHILD_EPONA_ANIM_TROT,   // mid
    /* 4 */ OOT_CHILD_EPONA_ANIM_GALLOP, // fast
    /* 5 */ OOT_CHILD_EPONA_ANIM_MAX
} OoTEponaAnimation;

static AnimationHeader* sAnimations[OOT_CHILD_EPONA_ANIM_MAX] = {
    &gEponaIdleAnim,   // OOT_CHILD_EPONA_ANIM_IDLE
    &gEponaWhinnyAnim, // OOT_CHILD_EPONA_ANIM_WHINNY
    &gEponaWalkAnim,   // OOT_CHILD_EPONA_ANIM_WALK
    &gEponaTrotAnim,   // OOT_CHILD_EPONA_ANIM_TROT
    &gEponaGallopAnim, // OOT_CHILD_EPONA_ANIM_GALLOP
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, MASS_HEAVY };

// the animation frames that should have sfx
static s32 sAnimSoundFrames[] = { 1, 19 };

static f32 sAnimPlaySpeeds[OOT_CHILD_EPONA_ANIM_MAX] = {
    1.0f, // OOT_CHILD_EPONA_ANIM_IDLE
    1.0f, // OOT_CHILD_EPONA_ANIM_WHINNY
    1.5f, // OOT_CHILD_EPONA_ANIM_WALK
    1.5f, // OOT_CHILD_EPONA_ANIM_TROT
    1.5f  // OOT_CHILD_EPONA_ANIM_GALLOP
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_STOP),
};

typedef enum {
    /* 0 */ OOT_CHILD_EPONA_ACTION_0,            // unfinished
    /* 1 */ OOT_CHILD_EPONA_ACTION_GREET_PLAYER, // approaching
    /* 2 */ OOT_CHILD_EPONA_ACTION_WAIT_FOR_PLAYER,
    /* 3 */ OOT_CHILD_EPONA_ACTION_LONLON_IDLE,
    /* 4 */ OOT_CHILD_EPONA_ACTION_4,
    /* 5 */ OOT_CHILD_EPONA_ACTION_5,
    /* 6 */ OOT_CHILD_EPONA_ACTION_MAX
} OoTEponaAction;

static EnHorseLinkChildActionFunc sActionFuncs[] = {
    EnHorseLinkChild_ActionFunc0,   // OOT_CHILD_EPONA_ACTION_0
    EnHorseLinkChild_GreetPlayer,   // OOT_CHILD_EPONA_ACTION_GREET_PLAYER
    EnHorseLinkChild_WaitForPlayer, // OOT_CHILD_EPONA_ACTION_WAIT_FOR_PLAYER
    EnHorseLinkChild_LonLonIdle,    // OOT_CHILD_EPONA_ACTION_LONLON_IDLE
    EnHorseLinkChild_ActionFunc4,   // OOT_CHILD_EPONA_ACTION_4
    EnHorseLinkChild_ActionFunc5,   // OOT_CHILD_EPONA_ACTION_5
};

static TexturePtr sEyeTextures[] = { gEponaEyeOpenTex, gEponaEyeHalfTex, gEponaEyeClosedTex };

s8 sEyeTextureIndexOverride[] = { 0, 1, 2, 1 };

// This plays the walking/gallop sfx, which needs to be timed based on horse speed,
// this is only used for animation OOT_CHILD_EPONA_ANIM_WALK
void EnHorseLinkChild_PlayWalkingSound(EnHorseLinkChild* this) {
    if ((sAnimSoundFrames[this->footstepCounter] < this->skin.skelAnime.curFrame) &&
        ((this->footstepCounter != 0) || !(sAnimSoundFrames[1] < this->skin.skelAnime.curFrame))) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_WALK);

        this->footstepCounter++;
        if (this->footstepCounter >= 2) {
            this->footstepCounter = 0;
        }
    }
}

void EnHorseLinkChild_PlaySound(EnHorseLinkChild* this) {
    if (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK) {
        EnHorseLinkChild_PlayWalkingSound(this);
    } else if (this->skin.skelAnime.curFrame == 0.0f) {
        if ((this->animIndex == OOT_CHILD_EPONA_ANIM_TROT) || (this->animIndex == OOT_CHILD_EPONA_ANIM_GALLOP)) {
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_RUN);
        } else if (this->animIndex == OOT_CHILD_EPONA_ANIM_WHINNY) {
            if (Rand_ZeroOne() > 0.5f) {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_GROAN);
            } else {
                Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
            }
        }
    }
}

f32 EnHorseLinkChild_GetAnimSpeed(EnHorseLinkChild* this) {
    f32 animSpeed;

    if (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK) {
        animSpeed = sAnimPlaySpeeds[this->animIndex] * this->actor.speed * (1.0f / 2.0f);
    } else if (this->animIndex == OOT_CHILD_EPONA_ANIM_TROT) {
        animSpeed = sAnimPlaySpeeds[this->animIndex] * this->actor.speed * (1.0f / 3.0f);
    } else if (this->animIndex == OOT_CHILD_EPONA_ANIM_GALLOP) {
        animSpeed = sAnimPlaySpeeds[this->animIndex] * this->actor.speed * (1.0f / 5.0f);
    } else {
        animSpeed = sAnimPlaySpeeds[this->animIndex];
    }

    return animSpeed;
}

void EnHorseLinkChild_Init(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 64.8f * 0.0001f);

    this->actor.gravity = -3.5f;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);

    this->action = OOT_CHILD_EPONA_ACTION_GREET_PLAYER;
    this->actor.speed = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;

    Skin_Init(&play->state, &this->skin, &gEponaSkel, &gEponaGallopAnim);
    this->animIndex = OOT_CHILD_EPONA_ANIM_IDLE;

    // this gets overwritten by EnHorseLinkChild_SetupLonLonIdle below
    Animation_PlayOnce(&this->skin.skelAnime, sAnimations[OOT_CHILD_EPONA_ANIM_IDLE]);

    Collider_InitCylinder(play, &this->colldierCylinder);
    Collider_InitJntSph(play, &this->colliderJntSph);
    Collider_SetJntSph(play, &this->colliderJntSph, &this->actor, &sJntSphInit, this->colliderJntSphElements);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    this->footstepCounter = 0;
    this->eyeTexIndex = 0;

    if (gSaveContext.sceneLayer >= 4) { // IS_CUTSCENE_LAYER in OoT
        EnHorseLinkChild_SetupLonLonIdle(this);
    } else {
        EnHorseLinkChild_SetupLonLonIdle(this);
    }

    this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
}

void EnHorseLinkChild_Destroy(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Skin_Free(&play->state, &this->skin);
    Collider_DestroyCylinder(play, &this->colldierCylinder);
    Collider_DestroyJntSph(play, &this->colliderJntSph);
}

void EnHorseLinkChild_SetupActionFunc0(EnHorseLinkChild* this) {
    this->action = OOT_CHILD_EPONA_ACTION_0;
    this->animIndex++;
    if (this->animIndex >= OOT_CHILD_EPONA_ANIM_MAX) {
        this->animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
    }
    Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animIndex]);
    this->skin.skelAnime.playSpeed = EnHorseLinkChild_GetAnimSpeed(this);
}

/**
 * Mostly broken
 */
void EnHorseLinkChild_ActionFunc0(EnHorseLinkChild* this, PlayState* play) {
    this->actor.speed = 0.0f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorseLinkChild_SetupActionFunc0(this);
    }
}

void EnHorseLinkChild_SetupWaitForPlayer(EnHorseLinkChild* this, s32 animIndex) {
    this->action = OOT_CHILD_EPONA_ACTION_WAIT_FOR_PLAYER;
    this->actor.speed = 0.0f;

    if ((animIndex != OOT_CHILD_EPONA_ANIM_IDLE) && (animIndex != OOT_CHILD_EPONA_ANIM_WHINNY)) {
        animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
    }

    if (this->animIndex != animIndex) {
        this->animIndex = animIndex;
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
    }
}

/**
 * Waiting for the player to get close, to notice and begin to approach the player.
 */
void EnHorseLinkChild_WaitForPlayer(EnHorseLinkChild* this, PlayState* play) {
    f32 distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
    s32 animIndex;

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if ((distToPlayer < 1000.0f) && (distToPlayer > 70.0f)) {
            EnHorseLinkChild_SetupGreetPlayer(this);
            return;
        }

        if (this->animIndex == OOT_CHILD_EPONA_ANIM_WHINNY) {
            animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
        } else {
            animIndex = OOT_CHILD_EPONA_ANIM_WHINNY;
        }

        if (this->animIndex != animIndex) {
            this->animIndex = animIndex;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void EnHorseLinkChild_SetupGreetPlayer(EnHorseLinkChild* this) {
    this->action = OOT_CHILD_EPONA_ACTION_GREET_PLAYER;
    this->animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
    this->actor.speed = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
}

/**
 * Chasing the player down.
 */
void EnHorseLinkChild_GreetPlayer(EnHorseLinkChild* this, PlayState* play) {
    f32 distToPlayer;
    s16 yawTowardPlayerDiff;
    s32 animIndex;

    if ((this->animIndex == OOT_CHILD_EPONA_ANIM_GALLOP) || (this->animIndex == OOT_CHILD_EPONA_ANIM_TROT) ||
        (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK)) {
        yawTowardPlayerDiff =
            Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
        if (yawTowardPlayerDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (yawTowardPlayerDiff < -0x12C) {
            this->actor.world.rot.y -= 0x12C;
        } else {
            this->actor.world.rot.y += yawTowardPlayerDiff;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
        if (distToPlayer > 1000.0f) {
            EnHorseLinkChild_SetupWaitForPlayer(this, 0);
            return;
        }

        if ((distToPlayer < 1000.0f) && (distToPlayer >= 300.0f)) {
            animIndex = OOT_CHILD_EPONA_ANIM_GALLOP;
            this->actor.speed = 6.0f;
        } else if ((distToPlayer < 300.0f) && (distToPlayer >= 150.0f)) {
            animIndex = OOT_CHILD_EPONA_ANIM_TROT;
            this->actor.speed = 4.0f;
        } else if ((distToPlayer < 150.0f) && (distToPlayer >= 70.0f)) {
            animIndex = OOT_CHILD_EPONA_ANIM_WALK;
            this->footstepCounter = 0;
            this->actor.speed = 2.0f;
        } else {
            EnHorseLinkChild_SetupWaitForPlayer(this, 1);
            return;
        }

        if (this->animIndex != animIndex) {
            this->animIndex = animIndex;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void EnHorseLinkChild_SetupLonLonIdle(EnHorseLinkChild* this) {
    this->action = OOT_CHILD_EPONA_ACTION_LONLON_IDLE;
    this->animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
    this->actor.speed = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
}

void func_808DF088(EnHorseLinkChild* this, PlayState* play) {
    if ((this->animIndex == OOT_CHILD_EPONA_ANIM_GALLOP) || (this->animIndex == OOT_CHILD_EPONA_ANIM_TROT) ||
        (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK)) {
        Player* player = GET_PLAYER(play);
        s16 newYawDiff;
        s32 newYawDir;
        s32 pad;

        if (Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos) < 250.0f) {
            newYawDiff = player->actor.shape.rot.y;
            if (Actor_WorldYawTowardActor(&this->actor, &player->actor) > 0) {
                newYawDir = 1;
            } else {
                newYawDir = -1;
            }
            newYawDiff += (newYawDir * 0x4000);
        } else {
            newYawDiff = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (newYawDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (newYawDiff < -0x12C) {
            this->actor.world.rot.y += -0x12C;
        } else {
            this->actor.world.rot.y += newYawDiff;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

/**
 * Idle in one spot, waiting for player to finish getting Epona song.
 * However, the flag was not migrated from OoT corectly, so it checks for the wrong flag.
 */
void EnHorseLinkChild_LonLonIdle(EnHorseLinkChild* this, PlayState* play) {
    Player* player;
    f32 distToPlayer;
    s32 isAnimFinished;
    s32 animIndex;

    func_808DF088(this, play);

    player = GET_PLAYER(play);
    distToPlayer = Actor_WorldDistXZToActor(&this->actor, &player->actor);
    animIndex = this->animIndex;
    isAnimFinished = SkelAnime_Update(&this->skin.skelAnime);

    if (isAnimFinished || (this->animIndex == OOT_CHILD_EPONA_ANIM_WHINNY) ||
        (this->animIndex == OOT_CHILD_EPONA_ANIM_IDLE)) {
        //! @bug: The carry-over of this flag from OoT was not done correctly
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ENTERED_ZORA_HALL)) {
            f32 distToHome = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos);
            s32 pad;

            if (Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos) > 250.0f) {
                if (distToHome >= 300.0f) {
                    animIndex = OOT_CHILD_EPONA_ANIM_GALLOP;
                    this->actor.speed = 6.0f;
                } else if ((distToHome < 300.0f) && (distToHome >= 150.0f)) {
                    animIndex = OOT_CHILD_EPONA_ANIM_TROT;
                    this->actor.speed = 4.0f;
                } else if ((distToHome < 150.0f) && (distToHome >= 70.0f)) {
                    animIndex = OOT_CHILD_EPONA_ANIM_WALK;
                    this->footstepCounter = 0;
                    this->actor.speed = 2.0f;
                } else {
                    this->actor.speed = 0.0f;
                    if (this->animIndex == OOT_CHILD_EPONA_ANIM_IDLE) {
                        animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_WHINNY : OOT_CHILD_EPONA_ANIM_IDLE;
                    } else {
                        animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_IDLE : OOT_CHILD_EPONA_ANIM_WHINNY;
                    }
                }
            } else if (distToPlayer < 200.0f) {
                animIndex = OOT_CHILD_EPONA_ANIM_GALLOP;
                this->actor.speed = 6.0f;
            } else if (distToPlayer < 300.0f) {
                animIndex = OOT_CHILD_EPONA_ANIM_TROT;
                this->actor.speed = 4.0f;
            } else if (distToPlayer < 400.0f) {
                animIndex = OOT_CHILD_EPONA_ANIM_WALK;
                this->footstepCounter = 0;
                this->actor.speed = 2.0f;
            } else {
                this->actor.speed = 0.0f;
                if (this->animIndex == OOT_CHILD_EPONA_ANIM_IDLE) {
                    animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_WHINNY : OOT_CHILD_EPONA_ANIM_IDLE;
                } else {
                    animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_IDLE : OOT_CHILD_EPONA_ANIM_WHINNY;
                }
            }
        } else {
            this->actor.speed = 0.0f;
            if (this->animIndex == OOT_CHILD_EPONA_ANIM_IDLE) {
                animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_WHINNY : OOT_CHILD_EPONA_ANIM_IDLE;
            } else {
                animIndex = (isAnimFinished == true) ? OOT_CHILD_EPONA_ANIM_IDLE : OOT_CHILD_EPONA_ANIM_WHINNY;
            }
        }
    }

    if ((this->animIndex != animIndex) || (isAnimFinished == true)) {
        this->animIndex = animIndex;
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                         this->skin.skelAnime.curFrame, Animation_GetLastFrame(sAnimations[this->animIndex]),
                         ANIMMODE_ONCE, 0.0f);
    }
}

void EnHorseLinkChild_SetupActionFunc5(EnHorseLinkChild* this) {
    this->action = OOT_CHILD_EPONA_ACTION_5;

    if (Rand_ZeroOne() > 0.5f) {
        this->animIndex = OOT_CHILD_EPONA_ANIM_IDLE;
    } else {
        this->animIndex = OOT_CHILD_EPONA_ANIM_WHINNY;
    }

    gHorsePlayedEponasSong = false;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, 0.0f);
}

void EnHorseLinkChild_ActionFunc5(EnHorseLinkChild* this, PlayState* play) {
    s16 yawTowardsPlayer;

    if (gHorsePlayedEponasSong) {
        gHorsePlayedEponasSong = false;
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_EV_KID_HORSE_NEIGH);
        EnHorseLinkChild_SetupActionFunc4(this);
        return;
    }

    this->actor.speed = 0.0f;
    yawTowardsPlayer = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

    if ((Math_CosS(yawTowardsPlayer) < 0.7071f) && (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK)) {
        Horse_RotateToPoint(&this->actor, &GET_PLAYER(play)->actor.world.pos, 0x12C);
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (Math_CosS(yawTowardsPlayer) < 0.0f) {
            this->animIndex = OOT_CHILD_EPONA_ANIM_WALK;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], sAnimPlaySpeeds[this->animIndex],
                             0.0f, Animation_GetLastFrame(sAnimations[OOT_CHILD_EPONA_ANIM_WALK]), ANIMMODE_ONCE,
                             -5.0f);
        } else {
            EnHorseLinkChild_SetupActionFunc5(this);
        }
    }
}

void EnHorseLinkChild_SetupActionFunc4(EnHorseLinkChild* this) {
    this->timer = 0;
    this->action = OOT_CHILD_EPONA_ACTION_4;
    this->animIndex = OOT_CHILD_EPONA_ANIM_WALK;
    this->isReturningHome = false;
    this->actor.speed = 2.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
}

void EnHorseLinkChild_ActionFunc4(EnHorseLinkChild* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToTargetPos;
    s32 animIndex;

    this->timer++;
    if (this->timer > 300) {
        this->isReturningHome = true;
    }

    if ((this->animIndex == OOT_CHILD_EPONA_ANIM_GALLOP) || (this->animIndex == OOT_CHILD_EPONA_ANIM_TROT) ||
        (this->animIndex == OOT_CHILD_EPONA_ANIM_WALK)) {
        if (!this->isReturningHome) {
            Horse_RotateToPoint(&this->actor, &player->actor.world.pos, 0x12C);
        } else {
            Horse_RotateToPoint(&this->actor, &this->actor.home.pos, 0x12C);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (!this->isReturningHome) {
            distToTargetPos = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
        } else {
            distToTargetPos = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos);
        }

        if (!this->isReturningHome) {
            if (distToTargetPos >= 300.0f) {
                animIndex = OOT_CHILD_EPONA_ANIM_GALLOP;
                this->actor.speed = 6.0f;
            } else if (distToTargetPos >= 150.0f) {
                animIndex = OOT_CHILD_EPONA_ANIM_TROT;
                this->actor.speed = 4.0f;
            } else {
                animIndex = OOT_CHILD_EPONA_ANIM_WALK;
                this->footstepCounter = 0;
                this->actor.speed = 2.0f;
            }
        } else if (distToTargetPos >= 300.0f) {
            animIndex = OOT_CHILD_EPONA_ANIM_GALLOP;
            this->actor.speed = 6.0f;
        } else if (distToTargetPos >= 150.0f) {
            animIndex = OOT_CHILD_EPONA_ANIM_TROT;
            this->actor.speed = 4.0f;
        } else if (distToTargetPos >= 70.0f) {
            animIndex = OOT_CHILD_EPONA_ANIM_WALK;
            this->footstepCounter = 0;
            this->actor.speed = 2.0f;
        } else {
            EnHorseLinkChild_SetupActionFunc5(this);
            return;
        }

        if (this->animIndex != animIndex) {
            this->animIndex = animIndex;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animIndex], EnHorseLinkChild_GetAnimSpeed(this),
                             0.0f, Animation_GetLastFrame(sAnimations[this->animIndex]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void EnHorseLinkChild_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    sActionFuncs[this->action](this, play);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 55.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;

    if ((Rand_ZeroOne() < 0.025f) && (this->eyeTexIndex == 0)) {
        this->eyeTexIndex++;
    } else if (this->eyeTexIndex > 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 4) {
            this->eyeTexIndex = 0;
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->colldierCylinder);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colldierCylinder.base);

    EnHorseLinkChild_PlaySound(this);
}

void EnHorseLinkChild_PostSkinDraw(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 i;

    for (i = 0; i < this->colliderJntSph.count; i++) {
        sp4C.x = this->colliderJntSph.elements[i].dim.modelSphere.center.x;
        sp4C.y = this->colliderJntSph.elements[i].dim.modelSphere.center.y;
        sp4C.z = this->colliderJntSph.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, this->colliderJntSph.elements[i].dim.limb, &sp4C, &sp40);

        this->colliderJntSph.elements[i].dim.worldSphere.center.x = sp40.x;
        this->colliderJntSph.elements[i].dim.worldSphere.center.y = sp40.y;
        this->colliderJntSph.elements[i].dim.worldSphere.center.z = sp40.z;
        this->colliderJntSph.elements[i].dim.worldSphere.radius =
            this->colliderJntSph.elements[i].dim.modelSphere.radius * this->colliderJntSph.elements[i].dim.scale;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderJntSph.base);
}

s32 EnHorseLinkChild_OverrideSkinDraw(Actor* thisx, PlayState* play, s32 limbIndex, Skin* skin) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == OBJECT_HORSE_LINK_CHILD_LIMB_0D) {
        u8 index = sEyeTextureIndexOverride[this->eyeTexIndex];

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[index]));
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return true;
}

void EnHorseLinkChild_Draw(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_80138258(&this->actor, play, &this->skin, EnHorseLinkChild_PostSkinDraw, EnHorseLinkChild_OverrideSkinDraw,
                  true);
}
