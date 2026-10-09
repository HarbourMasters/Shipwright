/*
 * File: z_en_ge1.c
 * Overlay: ovl_En_Ge1
 * Description: White-clad Gerudo Pirate
 */

#include "z_en_ge1.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnGe1_Init(Actor* thisx, PlayState* play);
void EnGe1_Destroy(Actor* thisx, PlayState* play);
void EnGe1_Update(Actor* thisx, PlayState* play);
void EnGe1_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Ge1_Profile = {
    /**/ ACTOR_EN_GE1,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GE1,
    /**/ sizeof(EnGe1),
    /**/ EnGe1_Init,
    /**/ EnGe1_Destroy,
    /**/ EnGe1_Update,
    /**/ EnGe1_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01000202, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

//! State flags

//! Disable normal movement to let pathing function control it completely.
#define GERUDO_WHITE_STATE_DISABLE_MOVEMENT (1 << 3)

typedef enum {
    /* 0 */ GERUDO_WHITE_HAIR_BOB,
    /* 1 */ GERUDO_WHITE_HAIR_STRAIGHT,
    /* 2 */ GERUDO_WHITE_HAIR_SPIKEY
} GerudoWhiteHairstyle;

typedef enum {
    /* -1 */ GERUDO_WHITE_ANIM_NONE = -1,
    /*  0 */ GERUDO_WHITE_ANIM_ARMS_FOLDED,
    /*  1 */ GERUDO_WHITE_ANIM_UNFOLDING_ARMS, // Unused
    /*  2 */ GERUDO_WHITE_ANIM_STANDING_HEAD_BOWED,
    /*  3 */ GERUDO_WHITE_ANIM_STIFFENING,
    /*  4 */ GERUDO_WHITE_ANIM_STIFF_SHIVERING,
    /*  5 */ GERUDO_WHITE_ANIM_TRUDGING_OFF,
    /*  6 */ GERUDO_WHITE_ANIM_EXCITED_CLAPPING,
    /*  7 */ GERUDO_WHITE_ANIM_SALUTE,
    /*  8 */ GERUDO_WHITE_ANIM_LEADING_BOAT,
    /*  9 */ GERUDO_WHITE_ANIM_BLOWN_AWAY,
    /* 10 */ GERUDO_WHITE_ANIM_MAX
} GerudoWhiteAnimation;

void EnGe1_ChangeAnim(EnGe1* this, s16 animIndex, u8 animMode, f32 morphFrames);
void EnGe1_ShadowDraw(Actor* thisx, Lights* lights, PlayState* play);
void EnGe1_Wait(EnGe1* this, PlayState* play);
void EnGe1_PerformCutsceneActions(EnGe1* this, PlayState* play);
s32 EnGe1_ValidatePictograph(PlayState* play, Actor* thisx);

void EnGe1_Init(Actor* thisx, PlayState* play) {
    EnGe1* this = (EnGe1*)thisx;

    ActorShape_Init(&this->picto.actor.shape, 0.0f, EnGe1_ShadowDraw, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGerudoWhiteSkel, &gGerudoWhiteArmsFoldedAnim, this->jointTable,
                       this->morphTable, GERUDO_WHITE_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->picto.actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_SetScale(&this->picto.actor, 0.01f);
    this->animIndex = this->cueId = -1; // GERUDO_WHITE_ANIM_NONE
    this->stateFlags = 0;
    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_ARMS_FOLDED, ANIMMODE_LOOP, 0.0f);
    this->actionFunc = EnGe1_Wait;
    this->picto.validationFunc = EnGe1_ValidatePictograph;

    switch (GERUDO_WHITE_GET_TYPE(&this->picto.actor)) {
        default:
            this->hairstyle = GERUDO_WHITE_HAIR_BOB;
            break;

        case GERUDO_WHITE_TYPE_HAIR_STRAIGHT:
            this->hairstyle = GERUDO_WHITE_HAIR_STRAIGHT;
            break;

        case GERUDO_WHITE_TYPE_HAIR_SPIKEY:
            this->hairstyle = GERUDO_WHITE_HAIR_SPIKEY;
            break;

        case GERUDO_WHITE_TYPE_CUTSCENE: // Only type used in-game
            this->hairstyle = GERUDO_WHITE_HAIR_BOB;
            this->actionFunc = EnGe1_PerformCutsceneActions;
            this->picto.actor.draw = NULL;
            this->picto.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
            this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            break;
    }

    this->picto.actor.terminalVelocity = -9.0f;
    this->picto.actor.velocity.y = -9.0f;
    this->picto.actor.gravity = -1.0f;
}

void EnGe1_Destroy(Actor* thisx, PlayState* play) {
    EnGe1* this = (EnGe1*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGe1_ChangeAnim(EnGe1* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    static AnimationHeader* sAnimations[GERUDO_WHITE_ANIM_MAX] = {
        &gGerudoWhiteArmsFoldedAnim,        // GERUDO_WHITE_ANIM_ARMS_FOLDED,
        &gGerudoWhiteUnfoldingArmsAnim,     // GERUDO_WHITE_ANIM_UNFOLDING_ARMS
        &gGerudoWhiteStandingHeadBowedAnim, // GERUDO_WHITE_ANIM_STANDING_HEAD_BOWED,
        &gGerudoWhiteStiffeningAnim,        // GERUDO_WHITE_ANIM_STIFFENING,
        &gGerudoWhiteStiffShiveringAnim,    // GERUDO_WHITE_ANIM_STIFF_SHIVERING,
        &gGerudoWhiteTrudgingOffAnim,       // GERUDO_WHITE_ANIM_TRUDGING_OFF,
        &gGerudoWhiteExcitedClappingAnim,   // GERUDO_WHITE_ANIM_EXCITED_CLAPPING,
        &gGerudoWhiteSaluteAnim,            // GERUDO_WHITE_ANIM_SALUTE,
        &gGerudoWhiteGreatBayCutsceneAnim,  // GERUDO_WHITE_ANIM_LEADING_BOAT,
        &gGerudoWhiteGreatBayCutsceneAnim,  // GERUDO_WHITE_ANIM_BLOWN_AWAY,
    };

    // The GERUDO_WHITE_ANIM_LEADING_BOAT / GERUDO_WHITE_ANIM_BLOWN_AWAY cases are single frames of an "animation" used
    // as static poses.
    switch (animIndex) {
        case GERUDO_WHITE_ANIM_LEADING_BOAT:
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
            break;

        case GERUDO_WHITE_ANIM_BLOWN_AWAY:
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f);
            break;

        default:
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]), animMode, morphFrames);
            break;
    }

    this->animIndex = animIndex;
}

void EnGe1_LookAtPlayer(EnGe1* this, PlayState* play) {
    s16 yawDiff = this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y;

    if ((ABS_ALT(yawDiff) <= 0x4300) && (this->picto.actor.xzDistToPlayer < 100.0f)) {
        Actor_TrackPlayer(play, &this->picto.actor, &this->headRot, &this->torsoRot, this->picto.actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
    }
}

void EnGe1_ShadowDraw(Actor* thisx, Lights* lights, PlayState* play) {
    Vec3f pos;
    EnGe1* this = (EnGe1*)thisx;

    Math_Vec3f_Copy(&pos, &this->picto.actor.world.pos);
    Math_Vec3f_Copy(&this->picto.actor.world.pos, &this->waistPos);
    func_800B4AEC(play, &this->picto.actor, 50.0f);
    this->picto.actor.world.pos.y = MAX(this->picto.actor.floorHeight, pos.y);
    ActorShadow_DrawCircle(&this->picto.actor, lights, play);
    Math_Vec3f_Copy(&this->picto.actor.world.pos, &pos);
}

void EnGe1_SetupPath(EnGe1* this, PlayState* play) {
    s32 pad;
    Vec3s* point;
    Vec3f nextPoint;

    this->curPointIndex = 0;

    if (GERUDO_WHITE_GET_PATH_INDEX(&this->picto.actor) != GERUDO_WHITE_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[GERUDO_WHITE_GET_PATH_INDEX(&this->picto.actor)];
        if (this->path != NULL) {
            point = Lib_SegmentedToVirtual(this->path->points);
            Math_Vec3s_ToVec3f(&this->picto.actor.world.pos, point);
            this->curPointIndex++;
            point++;
            Math_Vec3s_ToVec3f(&nextPoint, point);

            this->picto.actor.world.rot.y = Math_Vec3f_Yaw(&this->picto.actor.world.pos, &nextPoint);
            this->picto.actor.world.rot.x = Math_Vec3f_Pitch(&this->picto.actor.world.pos, &nextPoint);
            this->picto.actor.speed = 15.0f;
        }
    } else {
        this->path = NULL;
    }
}

/**
 * @return s32 true if close to next point or no path set.
 */
s32 EnGe1_FollowPath(EnGe1* this) {
    s32 pad;
    Path* path = this->path;
    Vec3s* curPoint;
    Vec3f point;
    s16 yawTarget;
    s16 pitchTarget;

    if (path == NULL) {
        return true;
    }

    curPoint = Lib_SegmentedToVirtual(this->path->points);
    curPoint += this->curPointIndex;

    Math_Vec3s_ToVec3f(&point, curPoint);
    yawTarget = Math_Vec3f_Yaw(&this->picto.actor.world.pos, &point);
    pitchTarget = Math_Vec3f_Pitch(&this->picto.actor.world.pos, &point);
    Math_SmoothStepToS(&this->picto.actor.world.rot.y, yawTarget, 0xA, 0x3E8, 0x64);
    Math_SmoothStepToS(&this->picto.actor.world.rot.x, pitchTarget, 6, 0x7D0, 0xC8);
    Actor_MoveWithoutGravityReverse(&this->picto.actor);

    if (Math_Vec3f_DistXYZ(&this->picto.actor.world.pos, &point) < 40.0f) {
        return true;
    } else {
        return false;
    }
}

void EnGe1_Scream(EnGe1* this) {
    if ((s32)Rand_ZeroFloat(2.0f) == 0) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO00);
    } else {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO01);
    }
}

void EnGe1_Wait(EnGe1* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnGe1_LookAtPlayer(this, play);
}

void EnGe1_PerformCutsceneActions(EnGe1* this, PlayState* play) {
    s16 cueId;

    if (SkelAnime_Update(&this->skelAnime) && (this->cueId == 3)) {
        EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_STIFF_SHIVERING, ANIMMODE_LOOP, 0.0f);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_121)) {
        this->picto.actor.draw = EnGe1_Draw;
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_121)]->id;
        switch (cueId) {
            case 8:
                this->stateFlags &= ~GERUDO_WHITE_STATE_DISABLE_MOVEMENT;
                break;

            case 9:
                this->stateFlags |= GERUDO_WHITE_STATE_DISABLE_MOVEMENT;
                break;

            default:
                this->stateFlags &= ~GERUDO_WHITE_STATE_DISABLE_MOVEMENT;
                Cutscene_ActorTranslateAndYaw(&this->picto.actor, play,
                                              Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_121));
                break;
        }

        if (this->cueId != cueId) {
            this->cueId = cueId;

            switch (this->cueId) {
                // Aveil cutscene
                case 1:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_ARMS_FOLDED, ANIMMODE_LOOP, 0.0f);
                    break;

                case 2:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_STANDING_HEAD_BOWED, ANIMMODE_LOOP, 0.0f);
                    break;

                case 3:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_STIFFENING, ANIMMODE_ONCE, 0.0f);
                    break;

                case 4:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_TRUDGING_OFF, ANIMMODE_ONCE, 0.0f);
                    break;

                case 5:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_EXCITED_CLAPPING, ANIMMODE_LOOP, 0.0f);
                    break;

                case 6:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_SALUTE, ANIMMODE_ONCE, 0.0f);
                    break;

                case 7:
                    Actor_Kill(&this->picto.actor);
                    break;

                    // Twister cutscene
                case 8:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_LEADING_BOAT, ANIMMODE_ONCE, 0.0f);
                    break;

                case 9:
                    EnGe1_ChangeAnim(this, GERUDO_WHITE_ANIM_BLOWN_AWAY, ANIMMODE_ONCE, 0.0f);
                    EnGe1_SetupPath(this, play);
                    this->stateFlags |= GERUDO_WHITE_STATE_DISABLE_MOVEMENT;
                    this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                    break;
            }

            SkelAnime_Update(&this->skelAnime);
        }

        if ((this->animIndex == GERUDO_WHITE_ANIM_TRUDGING_OFF) &&
            (Animation_OnFrame(&this->skelAnime, 12.0f) || Animation_OnFrame(&this->skelAnime, 25.0f))) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_PIRATE_WALK);
        }

        if ((this->animIndex == GERUDO_WHITE_ANIM_SALUTE) && Animation_OnFrame(&this->skelAnime, 14.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_PIRATE_WALK);
        }
    } else {
        this->picto.actor.draw = NULL;
    }

    switch (this->cueId) {
        case 9:
            if ((this->curPointIndex < this->path->count) && EnGe1_FollowPath(this)) {
                this->curPointIndex++;
            }

            // Tumble in the air
            this->picto.actor.shape.rot.x += 0x3E8;
            this->picto.actor.shape.rot.y += 0x7D0;
            this->picto.actor.shape.rot.z += 0x1F4;

            if (this->screamTimer > 0) {
                this->screamTimer--;
            } else {
                this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                EnGe1_Scream(this);
            }
            break;

        default:
            break;
    }
}

void EnGe1_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    if (this->picto.actor.draw != NULL) {
        Collider_UpdateCylinder(&this->picto.actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    if (!(this->stateFlags & GERUDO_WHITE_STATE_DISABLE_MOVEMENT)) {
        Actor_MoveWithGravity(&this->picto.actor);
    }
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 40.0f, 25.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);
    this->torsoRot.x = this->torsoRot.y = this->torsoRot.z = 0;

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

s32 EnGe1_ValidatePictograph(PlayState* play, Actor* thisx) {
    s32 ret = Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_GOOD, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                      400.0f, -1);

    ret |= Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_TOO_FAR, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                   1200.0f, -1);

    return ret;
}

s32 EnGe1_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    if (limbIndex == GERUDO_WHITE_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    if (this->animIndex == GERUDO_WHITE_ANIM_ARMS_FOLDED) {
        // Make small fidgeting movements if in standing animation.
        if ((limbIndex == GERUDO_WHITE_LIMB_TORSO) || (limbIndex == GERUDO_WHITE_LIMB_LEFT_FOREARM) ||
            (limbIndex == GERUDO_WHITE_LIMB_RIGHT_FOREARM)) {
            rot->y += TRUNCF_BINANG(Math_SinS(play->state.frames * (limbIndex * 50 + 0x814)) * 200.0f);
            rot->z += TRUNCF_BINANG(Math_CosS(play->state.frames * (limbIndex * 50 + 0x940)) * 200.0f);
        }
    }
    return false;
}

void EnGe1_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Gfx* sHairstyleDLs[] = {
        gGerudoWhiteHairstyleBobDL,
        gGerudoWhiteHairstyleStraightFringeDL,
        gGerudoWhiteHairstyleSpikyDL,
    };
    static Vec3f sInitialFocusPos = { 600.0f, 700.0f, 0.0f };
    static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    switch (limbIndex) {
        case GERUDO_WHITE_LIMB_WAIST:
            Matrix_MultVec3f(&sZeroVec, &this->waistPos);
            break;

        case GERUDO_WHITE_LIMB_HEAD:
            gSPDisplayList(POLY_OPA_DISP++, sHairstyleDLs[this->hairstyle]);
            Matrix_MultVec3f(&sInitialFocusPos, &this->picto.actor.focus.pos);
            break;

        default:
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnGe1_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gGerudoWhiteEyeOpenTex,
        gGerudoWhiteEyeHalfTex,
        gGerudoWhiteEyeClosedTex,
    };
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe1_OverrideLimbDraw, EnGe1_PostLimbDraw, &this->picto.actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
