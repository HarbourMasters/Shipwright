/*
 * File: z_en_demo_heishi.c
 * Overlay: ovl_En_Demo_heishi
 * Description: Unused(?) version of Shiro
 */

#include "z_en_demo_heishi.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDemoheishi_Init(Actor* thisx, PlayState* play);
void EnDemoheishi_Destroy(Actor* thisx, PlayState* play);
void EnDemoheishi_Update(Actor* thisx, PlayState* play);
void EnDemoheishi_Draw(Actor* thisx, PlayState* play);

void EnDemoheishi_SetupIdle(EnDemoheishi* this);
void EnDemoheishi_Idle(EnDemoheishi* this, PlayState* play);
void EnDemoheishi_SetupTalk(EnDemoheishi* this);
void EnDemoheishi_Talk(EnDemoheishi* this, PlayState* play);

ActorProfile En_Demo_heishi_Profile = {
    /**/ ACTOR_EN_DEMO_HEISHI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SDN,
    /**/ sizeof(EnDemoheishi),
    /**/ EnDemoheishi_Init,
    /**/ EnDemoheishi_Destroy,
    /**/ EnDemoheishi_Update,
    /**/ EnDemoheishi_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 40, 0, { 0, 0, 0 } },
};

static u16 sTextIds[] = { 0x1473 }; // Shiro initial intro text

void EnDemoheishi_Init(Actor* thisx, PlayState* play) {
    EnDemoheishi* this = (EnDemoheishi*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSoldierSkel, &gSoldierWaveAnim, this->jointTable, this->morphTable,
                       SOLDIER_LIMB_MAX);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    EnDemoheishi_SetupIdle(this);
}

void EnDemoheishi_Destroy(Actor* thisx, PlayState* play) {
    EnDemoheishi* this = (EnDemoheishi*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
}

typedef enum {
    /* 0 */ DEMOHEISHI_ANIM_STAND_HAND_ON_HIP,
    /* 1 */ DEMOHEISHI_ANIM_CHEER_WITH_SPEAR,
    /* 2 */ DEMOHEISHI_ANIM_WAVE,
    /* 3 */ DEMOHEISHI_ANIM_SIT_AND_REACH,
    /* 4 */ DEMOHEISHI_ANIM_STAND_UP,
    /* 5 */ DEMOHEISHI_ANIM_MAX
} EnDemoheishiAnimation;

static AnimationHeader* sAnimations[DEMOHEISHI_ANIM_MAX] = {
    &gSoldierStandHandOnHipAnim, // DEMOHEISHI_ANIM_STAND_HAND_ON_HIP
    &gSoldierCheerWithSpearAnim, // DEMOHEISHI_ANIM_CHEER_WITH_SPEAR
    &gSoldierWaveAnim,           // DEMOHEISHI_ANIM_WAVE
    &gSoldierSitAndReachAnim,    // DEMOHEISHI_ANIM_SIT_AND_REACH
    &gSoldierStandUpAnim,        // DEMOHEISHI_ANIM_STAND_UP
};

static u8 sAnimationModes[DEMOHEISHI_ANIM_MAX] = {
    ANIMMODE_LOOP, // DEMOHEISHI_ANIM_STAND_HAND_ON_HIP
    ANIMMODE_LOOP, // DEMOHEISHI_ANIM_CHEER_WITH_SPEAR
    ANIMMODE_LOOP, // DEMOHEISHI_ANIM_WAVE
    ANIMMODE_LOOP, // DEMOHEISHI_ANIM_SIT_AND_REACH
    ANIMMODE_ONCE, // DEMOHEISHI_ANIM_STAND_UP
};

void EnDemoheishi_ChangeAnim(EnDemoheishi* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void EnDemoheishi_SetHeadRotation(EnDemoheishi* this) {
    s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    s32 absYawDiff = ABS_ALT(yawDiff);

    this->headRotXTarget = 0;
    if ((this->actor.xzDistToPlayer < 200.0f) && (absYawDiff < 0x4E20)) {
        this->headRotXTarget = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (this->headRotXTarget > 0x2710) {
            this->headRotXTarget = 0x2710;
        } else if (this->headRotXTarget < -0x2710) {
            this->headRotXTarget = -0x2710;
        }
    }
}

void EnDemoheishi_SetupIdle(EnDemoheishi* this) {
    EnDemoheishi_ChangeAnim(this, DEMOHEISHI_ANIM_STAND_HAND_ON_HIP);
    this->textIdIndex = 0;
    this->actor.textId = sTextIds[this->textIdIndex];
    this->isTalking = false;
    this->actionFunc = EnDemoheishi_Idle;
}

void EnDemoheishi_Idle(EnDemoheishi* this, PlayState* play) {
    s32 absYawDiff;
    s16 yawDiff;

    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    absYawDiff = ABS_ALT(yawDiff);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnDemoheishi_SetupTalk(this);
    } else if (absYawDiff <= 0x4BB8) {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void EnDemoheishi_SetupTalk(EnDemoheishi* this) {
    this->isTalking = true;
    this->actionFunc = EnDemoheishi_Talk;
}

void EnDemoheishi_Talk(EnDemoheishi* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        EnDemoheishi_SetupIdle(this);
    }
}

void EnDemoheishi_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDemoheishi* this = (EnDemoheishi*)thisx;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetScale(&this->actor, 0.01f);
    EnDemoheishi_SetHeadRotation(this);

    Actor_SetFocus(&this->actor, 60.0f);
    Math_SmoothStepToS(&this->headRotX, this->headRotXTarget, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->headRotY, this->headRotYTarget, 1, 0x3E8, 0);
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

s32 EnDemoheishi_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDemoheishi* this = (EnDemoheishi*)thisx;

    if (limbIndex == SOLDIER_LIMB_HEAD) {
        rot->x += this->headRotX;
        rot->y += this->headRotY;
        rot->z += this->headRotZ;
    }

    return false;
}

void EnDemoheishi_Draw(Actor* thisx, PlayState* play) {
    EnDemoheishi* this = (EnDemoheishi*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDemoheishi_OverrideLimbDraw, NULL, &this->actor);
}
