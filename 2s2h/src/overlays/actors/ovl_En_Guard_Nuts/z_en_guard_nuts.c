/*
 * File: z_en_guard_nuts.c
 * Overlay: ovl_En_Guard_Nuts
 * Description: Deku Palace - Entrance Guard
 */

#include "z_en_guard_nuts.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnGuardNuts_Init(Actor* thisx, PlayState* play);
void EnGuardNuts_Destroy(Actor* thisx, PlayState* play);
void EnGuardNuts_Update(Actor* thisx, PlayState* play);
void EnGuardNuts_Draw(Actor* thisx, PlayState* play);
void EnGuardNuts_Reset(void);

void EnGuardNuts_ChangeAnim(EnGuardNuts* this, s32 animIndex);
void EnGuardNuts_SetupWait(EnGuardNuts* this);
void EnGuardNuts_Wait(EnGuardNuts* this, PlayState* play);
void func_80ABB540(EnGuardNuts* this);
void func_80ABB590(EnGuardNuts* this, PlayState* play);
void EnGuardNuts_Burrow(EnGuardNuts* this, PlayState* play);
void EnGuardNuts_SetupUnburrow(EnGuardNuts* this, PlayState* play);
void EnGuardNuts_Unburrow(EnGuardNuts* this, PlayState* play);

ActorProfile En_Guard_Nuts_Profile = {
    /**/ ACTOR_EN_GUARD_NUTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNK,
    /**/ sizeof(EnGuardNuts),
    /**/ EnGuardNuts_Init,
    /**/ EnGuardNuts_Destroy,
    /**/ EnGuardNuts_Update,
    /**/ EnGuardNuts_Draw,
    /**/ EnGuardNuts_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON | OCELEM_UNK3,
    },
    { 50, 50, 0, { 0, 0, 0 } },
};

static s32 sGuardCount = 0;

static s32 D_80ABBE20 = 0;

static u16 sTextIds[] = { 0x0824, 0x0825, 0x0826, 0x082D, 0x0827, 0x0828, 0x0829, 0x082A, 0x082B, 0x082C };

static s16 D_80ABBE38[] = { 0x0000, 0x0000, 0x0002, 0x0001, 0x0000, 0x0000, 0x0002, 0x0000, 0x0000, 0x0002 };

typedef enum {
    /* -1 */ GUARD_NUTS_ANIM_NONE = -1,
    /*  0 */ GUARD_NUTS_ANIM_WAIT_HEAD_TILT,
    /*  1 */ GUARD_NUTS_ANIM_WALK,
    /*  2 */ GUARD_NUTS_ANIM_DIG,
    /*  3 */ GUARD_NUTS_ANIM_WALK_2,
    /*  4 */ GUARD_NUTS_ANIM_MAX
} EnGuardNutsAnimation;

static AnimationHeader* sAnimations[GUARD_NUTS_ANIM_MAX] = {
    &gDekuPalaceGuardWaitAnim, // GUARD_NUTS_ANIM_WAIT_HEAD_TILT
    &gDekuPalaceGuardWalkAnim, // GUARD_NUTS_ANIM_WALK
    &gDekuPalaceGuardDigAnim,  // GUARD_NUTS_ANIM_DIG
    &gDekuPalaceGuardWalkAnim, // GUARD_NUTS_ANIM_WALK_2
};

static u8 sAnimationModes[GUARD_NUTS_ANIM_MAX] = {
    ANIMMODE_LOOP, // GUARD_NUTS_ANIM_WAIT_HEAD_TILT
    ANIMMODE_LOOP, // GUARD_NUTS_ANIM_WALK
    ANIMMODE_ONCE, // GUARD_NUTS_ANIM_DIG
    ANIMMODE_ONCE, // GUARD_NUTS_ANIM_WALK_2
};

static TexturePtr sEyeTextures[] = {
    gDekuPalaceGuardEyeOpenTex,
    gDekuPalaceGuardEyeHalfTex,
    gDekuPalaceGuardEyeClosedTex,
};

typedef enum {
    /* 0 */ GUARD_NUTS_WAIT_STATE,
    /* 1 */ GUARD_NUTS_TALKING_STATE,
    /* 3 */ GUARD_NUTS_BURROWED_STATE = 3,
    /* 4 */ GUARD_NUTS_UNK_STATE
} EnGuardNutsState;

void EnGuardNuts_Init(Actor* thisx, PlayState* play) {
    EnGuardNuts* this = (EnGuardNuts*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuPalaceGuardSkel, &gDekuPalaceGuardWaitAnim, this->jointTable,
                   this->morphTable, DEKU_PALACE_GUARD_LIMB_MAX);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Actor_SetScale(&this->actor, 0.015f);
    Math_Vec3f_Copy(&this->guardPos, &this->actor.world.pos);
    this->guardNumber = sGuardCount;
    sGuardCount++;

    // If you have returned deku princess guards will init burrowed.
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        EnGuardNuts_SetupWait(this);
    } else {
        EnGuardNuts_Burrow(this, play);
    }
}

void EnGuardNuts_Destroy(Actor* thisx, PlayState* play) {
    EnGuardNuts* this = (EnGuardNuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * @brief Changes the animation to the provided index. Updates animIndex and animEndFrame for the animation.
 *
 * @param this
 * @param animIndex the index of sAnimations to change to
 */
void EnGuardNuts_ChangeAnim(EnGuardNuts* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -2.0f);
}

void EnGuardNuts_SetupWait(EnGuardNuts* this) {
    EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_WAIT_HEAD_TILT);
    this->state = GUARD_NUTS_WAIT_STATE;
    this->actionFunc = EnGuardNuts_Wait;
}

void EnGuardNuts_Wait(EnGuardNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 phi_a1;
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80ABB540(this);
        return;
    }
    if (yawDiff > 0x6000) {
        D_80ABBE20 = 2;
    }
    if (player->transformation == PLAYER_FORM_DEKU) {
        // this is the palace of...
        this->guardTextIndex = 0;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_17_04) && (!this->hasCompletedConversation)) {
            // I told you not to enter!!
            this->guardTextIndex = 7;
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_12_40)) {
            // come to see the monkey again?
            this->guardTextIndex = 4;
        }
    } else {
        // No place for an outsider
        this->guardTextIndex = 3;
    }
    this->actor.textId = sTextIds[this->guardTextIndex];
    phi_a1 = this->actor.world.rot.y;
    if (D_80ABBE20 == 2) {
        EnGuardNuts_Burrow(this, play);
        return;
    }
    if (D_80ABBE20 == 1) {
        if (this->animIndex != GUARD_NUTS_ANIM_WALK) {
            EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_WALK);
        }
        phi_a1 = (this->actor.home.rot.y + 0x8000);
    }
    if (fabsf((f32)(this->actor.shape.rot.y - phi_a1)) < 100.0f) {
        this->actor.shape.rot.y = phi_a1;
        if ((D_80ABBE20 == 1) && (this->animIndex != GUARD_NUTS_ANIM_WALK_2)) {
            EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_WALK_2);
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, phi_a1, 1, 0xBB8, 0);
    }
    if (this->targetHeadPos.x == 0) {
        if (fabsf((f32)this->actor.world.rot.y - (f32)this->actor.yawTowardsPlayer) < 10000.0f) {
            this->targetHeadPos.y = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
            this->targetHeadPos.y = -this->targetHeadPos.y;
        }
    }
    Actor_OfferTalk(&this->actor, play, 70.0f);
}

void func_80ABB540(EnGuardNuts* this) {
    EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_WALK);
    this->targetHeadPos.x = this->targetHeadPos.y = 0;
    this->timer = 16;
    this->state = GUARD_NUTS_TALKING_STATE;
    this->actionFunc = func_80ABB590;
}

void func_80ABB590(EnGuardNuts* this, PlayState* play) {
    s16 yaw = this->actor.yawTowardsPlayer;
    f32 curFrame;

    if (D_80ABBE20 == 1) {
        yaw = (this->actor.home.rot.y + 0x8000);
    }
    if (fabsf((this->actor.shape.rot.y - yaw)) < 100.0f) {
        this->actor.shape.rot.y = yaw;
        curFrame = this->skelAnime.curFrame;
        if ((curFrame < this->animEndFrame) || (this->guardTextIndex >= 7)) {
            SkelAnime_Update(&this->skelAnime);
        }
    } else {
        SkelAnime_Update(&this->skelAnime);
        Math_SmoothStepToS(&this->actor.shape.rot.y, yaw, 1, 0xBB8, 0);
    }
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) {
        this->targetHeadPos.y = 0;
        this->targetHeadPos.x = 0;
        if ((this->guardTextIndex == 3) && (this->animIndex == GUARD_NUTS_ANIM_WAIT_HEAD_TILT)) {
            EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_WAIT_HEAD_TILT);
        }
        if (Message_ShouldAdvance(play)) {
            if (D_80ABBE38[this->guardTextIndex] != 1) {
                if (D_80ABBE38[this->guardTextIndex] == 2) {
                    Message_CloseTextbox(play);
                    D_80ABBE20 = 2;
                    SET_WEEKEVENTREG(WEEKEVENTREG_12_40);
                    EnGuardNuts_Burrow(this, play);
                } else {
                    this->guardTextIndex++;
                    Message_StartTextbox(play, sTextIds[this->guardTextIndex], &this->actor);
                    if (D_80ABBE38[this->guardTextIndex] == 2) {
                        D_80ABBE20 = 1;
                    }
                    this->timer = 16;
                }
            } else if (this->guardTextIndex != 3) {
                this->targetHeadPos.x = 0;
                this->targetHeadPos.y = this->targetHeadPos.x;
                Message_CloseTextbox(play);
                this->state = GUARD_NUTS_UNK_STATE;
                this->actionFunc = EnGuardNuts_Unburrow;
            } else {
                Message_CloseTextbox(play);
                EnGuardNuts_SetupWait(this);
            }
        }
    } else if ((Message_GetState(&play->msgCtx) >= TEXT_STATE_FADING) && (D_80ABBE20 == 0)) {
        if ((this->guardTextIndex == 0) || (this->guardTextIndex == 3) || (this->guardTextIndex >= 7)) {
            if (this->timer == 0) {
                this->timer = 2;
                this->targetHeadPos.y ^= 0x2000;
            }
        } else if (this->timer == 0) {
            this->timer = 2;
            this->targetHeadPos.x ^= 0x2000;
        }
    }
}

void EnGuardNuts_Burrow(EnGuardNuts* this, PlayState* play) {
    Vec3f digPos;

    EnGuardNuts_ChangeAnim(this, GUARD_NUTS_ANIM_DIG);
    Math_Vec3f_Copy(&digPos, &this->actor.world.pos);
    digPos.y = this->actor.floorHeight;
    EffectSsHahen_SpawnBurst(play, &digPos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
    this->targetHeadPos.y = 0;
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->targetHeadPos.x = this->targetHeadPos.y;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
    this->actionFunc = EnGuardNuts_SetupUnburrow;
}

void EnGuardNuts_SetupUnburrow(EnGuardNuts* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (curFrame >= this->animEndFrame) {
        this->state = GUARD_NUTS_BURROWED_STATE;
        this->actionFunc = EnGuardNuts_Unburrow;
        this->actor.world.rot.y = this->actor.home.rot.y;
        this->actor.shape.rot.y = this->actor.home.rot.y;
    }
}

void EnGuardNuts_Unburrow(EnGuardNuts* this, PlayState* play) {
    s16 yawDiff;
    Vec3f digPos;

    // If you have returned Deku Princess, guards will not unburrow
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y));
        if ((yawDiff < 0x4000) && ((D_80ABBE20 == 0) || (this->actor.xzDistToPlayer > 150.0f))) {
            Math_Vec3f_Copy(&digPos, &this->actor.world.pos);
            digPos.y = this->actor.floorHeight;
            EffectSsHahen_SpawnBurst(play, &digPos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
            D_80ABBE20 = 0;
            if (this->guardTextIndex == 9) {
                this->hasCompletedConversation = true;
            }
            this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
            EnGuardNuts_SetupWait(this);
        }
    }
}

void EnGuardNuts_Update(Actor* thisx, PlayState* play) {
    EnGuardNuts* this = (EnGuardNuts*)thisx;
    s32 pad;

    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState >= 3) {
            this->eyeState = 0;
            this->blinkTimer = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }
    if ((this->animIndex == GUARD_NUTS_ANIM_WALK) &&
        (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 5.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
    }

    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 40.0f);
    Math_SmoothStepToS(&this->headRot.x, this->targetHeadPos.x, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->headRot.y, this->targetHeadPos.y, 1, 0xBB8, 0);

    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->timer != 0) {
        this->timer--;
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    if ((this->state != GUARD_NUTS_BURROWED_STATE) && (this->state != GUARD_NUTS_UNK_STATE)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnGuardNuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnGuardNuts* this = (EnGuardNuts*)thisx;

    if (limbIndex == DEKU_PALACE_GUARD_LIMB_HEAD) {
        rot->x += this->headRot.x;
        rot->y += this->headRot.y;
        rot->z += this->headRot.z;
    }
    return false;
}

void EnGuardNuts_Draw(Actor* thisx, PlayState* play) {
    EnGuardNuts* this = (EnGuardNuts*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeState]));

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnGuardNuts_OverrideLimbDraw, NULL,
                      &this->actor);
    Matrix_Translate(this->guardPos.x, this->actor.floorHeight, this->guardPos.z, MTXMODE_NEW);
    Matrix_Scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gDekuPalaceGuardFlowerDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnGuardNuts_Reset(void) {
    sGuardCount = 0;
    D_80ABBE20 = 0;
}