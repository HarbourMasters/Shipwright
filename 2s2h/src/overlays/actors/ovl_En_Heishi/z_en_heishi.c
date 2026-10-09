/*
 * File: z_en_heishi.c
 * Overlay: ovl_En_Heishi
 * Description: Soldier (Mayor's Residence only)
 */

#include "z_en_heishi.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnHeishi_Init(Actor* thisx, PlayState* play);
void EnHeishi_Destroy(Actor* thisx, PlayState* play);
void EnHeishi_Update(Actor* thisx, PlayState* play);
void EnHeishi_Draw(Actor* thisx, PlayState* play);

void EnHeishi_ChangeAnim(EnHeishi* this, s32 animIndex);
void EnHeishi_SetHeadRotation(EnHeishi* this);
void EnHeishi_SetupIdle(EnHeishi* this);
void EnHeishi_Idle(EnHeishi* this, PlayState* play);

ActorProfile En_Heishi_Profile = {
    /**/ ACTOR_EN_HEISHI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SDN,
    /**/ sizeof(EnHeishi),
    /**/ EnHeishi_Init,
    /**/ EnHeishi_Destroy,
    /**/ EnHeishi_Update,
    /**/ EnHeishi_Draw,
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
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

void EnHeishi_Init(Actor* thisx, PlayState* play) {
    EnHeishi* this = (EnHeishi*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSoldierSkel, &gSoldierWaveAnim, this->jointTable, this->morphTable,
                       SOLDIER_LIMB_MAX);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->paramsCopy = this->actor.params;
    this->yawTowardsPlayer = this->actor.world.rot.y;

    if (this->paramsCopy == 0) {
        this->shouldSetHeadRotation = 1;
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) &&
            !((gSaveContext.save.day == 3) && gSaveContext.save.isNight)) {
            Actor_Kill(&this->actor);
        }
    } else {
        this->colliderCylinder.dim.radius = 30;
        this->colliderCylinder.dim.height = 60;
        this->colliderCylinder.dim.yShift = 0;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) ||
            ((gSaveContext.save.day == 3) && gSaveContext.save.isNight)) {
            Actor_Kill(&this->actor);
        }
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    EnHeishi_SetupIdle(this);
}

void EnHeishi_Destroy(Actor* thisx, PlayState* play) {
    EnHeishi* this = (EnHeishi*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
}

typedef enum EnHeishiAnimation {
    /* 0 */ HEISHI_ANIM_STAND_HAND_ON_HIP,
    /* 1 */ HEISHI_ANIM_CHEER_WITH_SPEAR,
    /* 2 */ HEISHI_ANIM_WAVE,
    /* 3 */ HEISHI_ANIM_SIT_AND_REACH,
    /* 4 */ HEISHI_ANIM_STAND_UP,
    /* 5 */ HEISHI_ANIM_MAX
} EnHeishiAnimation;

static AnimationHeader* sAnimations[HEISHI_ANIM_MAX] = {
    &gSoldierStandHandOnHipAnim, // HEISHI_ANIM_STAND_HAND_ON_HIP
    &gSoldierCheerWithSpearAnim, // HEISHI_ANIM_CHEER_WITH_SPEAR
    &gSoldierWaveAnim,           // HEISHI_ANIM_WAVE
    &gSoldierSitAndReachAnim,    // HEISHI_ANIM_SIT_AND_REACH
    &gSoldierStandUpAnim,        // HEISHI_ANIM_STAND_UP
};

static u8 sAnimationModes[HEISHI_ANIM_MAX] = {
    ANIMMODE_LOOP, // HEISHI_ANIM_STAND_HAND_ON_HIP
    ANIMMODE_LOOP, // HEISHI_ANIM_CHEER_WITH_SPEAR
    ANIMMODE_LOOP, // HEISHI_ANIM_WAVE
    ANIMMODE_LOOP, // HEISHI_ANIM_SIT_AND_REACH
    ANIMMODE_ONCE, // HEISHI_ANIM_STAND_UP
};

void EnHeishi_ChangeAnim(EnHeishi* this, s32 animIndex) {

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void EnHeishi_SetHeadRotation(EnHeishi* this) {
    s16 yawDiff = this->yawTowardsPlayer - this->actor.world.rot.y;
    s32 absYawDiff = ABS_ALT(yawDiff);

    this->headRotXTarget = 0;
    if ((this->actor.xzDistToPlayer < 200.0f) && (absYawDiff < 0x4E20)) {
        this->headRotXTarget = this->yawTowardsPlayer - this->actor.world.rot.y;
        if (this->headRotXTarget > 0x2710) {
            this->headRotXTarget = 0x2710;
        } else if (this->headRotXTarget < -0x2710) {
            this->headRotXTarget = -0x2710;
        }
    }
}

void EnHeishi_SetupIdle(EnHeishi* this) {
    s8 animIndex = HEISHI_ANIM_STAND_HAND_ON_HIP;

    EnHeishi_ChangeAnim(this, animIndex);
    this->unk278 = animIndex;
    this->actionFunc = EnHeishi_Idle;
}

void EnHeishi_Idle(EnHeishi* this, PlayState* play) {
}

void EnHeishi_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHeishi* this = (EnHeishi*)thisx;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((this->paramsCopy != 0) && (gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetScale(&this->actor, 0.01f);
    if (this->shouldSetHeadRotation) {
        EnHeishi_SetHeadRotation(this);
    }

    Actor_SetFocus(&this->actor, 60.0f);
    Math_SmoothStepToS(&this->headRotX, this->headRotXTarget, 1, 0xBB8, 0);
    Math_SmoothStepToS(&this->headRotY, this->headRotYTarget, 1, 0x3E8, 0);
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

s32 EnHeishi_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnHeishi* this = (EnHeishi*)thisx;

    if (limbIndex == SOLDIER_LIMB_HEAD) {
        rot->x += this->headRotX;
        rot->y += this->headRotY;
        rot->z += this->headRotZ;
    }

    return false;
}

void EnHeishi_Draw(Actor* thisx, PlayState* play) {
    EnHeishi* this = (EnHeishi*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnHeishi_OverrideLimbDraw, NULL, &this->actor);
}
