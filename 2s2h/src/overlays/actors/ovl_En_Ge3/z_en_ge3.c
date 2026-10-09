/*
 * File: z_en_ge3.c
 * Overlay: ovl_En_Ge3
 * Description: Aviel, leader of the Gerudo Pirates
 */

#include "z_en_ge3.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnGe3_Init(Actor* thisx, PlayState* play);
void EnGe3_Destroy(Actor* thisx, PlayState* play);
void EnGe3_Update(Actor* thisx, PlayState* play);
void EnGe3_Draw(Actor* thisx, PlayState* play);

void EnGe3_ChangeAnim(EnGe3* this, s16 animIndex, u8 animMode, f32 morphFrames);
void EnGe3_SetupPath(EnGe3* this, PlayState* play);
void EnGe3_Idle(EnGe3* this, PlayState* play);
void EnGe3_AveilsChamberIdle(EnGe3* this, PlayState* play);
s32 EnGe3_ValidatePictograph(PlayState* play, Actor* thisx);

typedef enum GerudoAveilAnimation {
    /* -1 */ GERUDO_AVEIL_ANIM_NONE = -1,
    /*  0 */ GERUDO_AVEIL_ANIM_STAND,
    /*  1 */ GERUDO_AVEIL_ANIM_WALK,
    /*  2 */ GERUDO_AVEIL_ANIM_HAND_ON_HIP_WAIT,
    /*  3 */ GERUDO_AVEIL_ANIM_TURN_HEAD,
    /*  4 */ GERUDO_AVEIL_ANIM_WAIT,
    /*  5 */ GERUDO_AVEIL_ANIM_DEMAND,
    /*  6 */ GERUDO_AVEIL_ANIM_DISMISS,
    /*  7 */ GERUDO_AVEIL_ANIM_BEG,
    /*  8 */ GERUDO_AVEIL_ANIM_RUN_AWAY,
    /*  9 */ GERUDO_AVEIL_ANIM_MAX
} GerudoAveilAnimation;

ActorProfile En_Ge3_Profile = {
    /**/ ACTOR_EN_GE3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GELDB,
    /**/ sizeof(EnGe3),
    /**/ EnGe3_Init,
    /**/ EnGe3_Destroy,
    /**/ EnGe3_Update,
    /**/ EnGe3_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01000222, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

void EnGe3_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe3* this = (EnGe3*)thisx;

    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGerudoRedSkel, NULL, this->jointTable, this->morphTable,
                       GERUDO_RED_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gGerudoRedHandOnHipWaitAnim);
    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->picto.actor, 0.01f);

    this->animIndex = GERUDO_AVEIL_ANIM_NONE;
    this->cueId = GERUDO_AVEIL_CUEID_NONE;
    this->picto.validationFunc = EnGe3_ValidatePictograph;
    EnGe3_SetupPath(this, play);

    if (GERUDO_AVEIL_GET_TYPE(&this->picto.actor) == GERUDO_AVEIL_TYPE_AVEILS_CHAMBER) {
        EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_HAND_ON_HIP_WAIT, ANIMMODE_LOOP, 0.0f);
        this->actionFunc = EnGe3_AveilsChamberIdle;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_83_02)) {
            Actor_Kill(&this->picto.actor);
            return;
        }
    } else {
        EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_WALK, ANIMMODE_LOOP, 0.0f);
        this->actionFunc = EnGe3_Idle;
        this->picto.actor.speed = 1.5f;
    }

    this->stateFlags = 0;
    this->picto.actor.attentionRangeType = 6;
    this->picto.actor.terminalVelocity = -4.0f;
    this->picto.actor.gravity = -1.0f;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_80_08);
}

void EnGe3_Destroy(Actor* thisx, PlayState* play) {
    EnGe3* this = (EnGe3*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGe3_ChangeAnim(EnGe3* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    static AnimationHeader* sAnimations[GERUDO_AVEIL_ANIM_MAX] = {
        &gGerudoRedStandAnim,         // GERUDO_AVEIL_ANIM_STAND
        &gGerudoRedWalkAnim,          // GERUDO_AVEIL_ANIM_WALK
        &gGerudoRedHandOnHipWaitAnim, // GERUDO_AVEIL_ANIM_HAND_ON_HIP_WAIT
        &gGerudoRedTurnHeadAnim,      // GERUDO_AVEIL_ANIM_TURN_HEAD
        &gGerudoRedWaitAnim,          // GERUDO_AVEIL_ANIM_WAIT
        &gGerudoRedDemandAnim,        // GERUDO_AVEIL_ANIM_DEMAND
        &gGerudoRedDismissAnim,       // GERUDO_AVEIL_ANIM_DISMISS
        &gGerudoRedBegAnim,           // GERUDO_AVEIL_ANIM_BEG
        &gGerudoRedRunAwayAnim,       // GERUDO_AVEIL_ANIM_RUN_AWAY
    };

    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[animIndex]), animMode, morphFrames);
    this->animIndex = animIndex;
}

void EnGe3_LookAtPlayer(EnGe3* this, PlayState* play) {
    s16 yawDiff = this->picto.actor.yawTowardsPlayer - this->picto.actor.shape.rot.y;

    if ((ABS_ALT(yawDiff) <= 0x2300) && (this->picto.actor.xzDistToPlayer < 100.0f)) {
        Actor_TrackPlayer(play, &this->picto.actor, &this->headRot, &this->torsoRot, this->picto.actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    }
}

void EnGe3_SetupPath(EnGe3* this, PlayState* play) {
    this->curPointIndex = 0;

    if (GERUDO_AVEIL_GET_PATH(&this->picto.actor) != GERUDO_AVEIL_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[GERUDO_AVEIL_GET_PATH(&this->picto.actor)];
    } else {
        this->path = NULL;
    }
}

/**
 * @return true if no path or reached end of path
 */
s32 EnGe3_FollowPath(EnGe3* this) {
    Path* path = this->path;
    Vec3s* curPoint;
    f32 diffX;
    f32 diffZ;

    if (path == NULL) {
        return true;
    }

    curPoint = Lib_SegmentedToVirtual(path->points);
    curPoint += this->curPointIndex;
    diffX = curPoint->x - this->picto.actor.world.pos.x;
    diffZ = curPoint->z - this->picto.actor.world.pos.z;
    this->picto.actor.world.rot.y = Math_Atan2S(diffX, diffZ);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0x7D0, 0xC8);

    if ((SQ(diffX) + SQ(diffZ)) < SQ(10.0f)) {
        this->curPointIndex++;
        if (this->curPointIndex >= path->count) {
            return true;
        }
    }
    return false;
}

void EnGe3_Scream(EnGe3* this) {
    if ((s32)Rand_ZeroFloat(2.0f) == 0) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO00);
    } else {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO01);
    }
}

void EnGe3_Idle(EnGe3* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnGe3_LookAtPlayer(this, play);

    if (EnGe3_FollowPath(this)) {
        EnGe3_SetupPath(this, play);
    }
}

void EnGe3_ThrowPlayerOut(EnGe3* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x400, 0x100);

    if (this->actionTimer > 0) {
        this->actionTimer--;
    } else if (play->nextEntrance != play->setupExitList[GERUDO_AVEIL_GET_EXIT(&this->picto.actor)]) {
        play->nextEntrance = play->setupExitList[GERUDO_AVEIL_GET_EXIT(&this->picto.actor)];
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_38;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_80_08);
    }
}

void EnGe3_AveilsChamberIdle(EnGe3* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->cueId == GERUDO_AVEIL_CUEID_INSTRUCT)) {
        EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_WAIT, ANIMMODE_ONCE, 0.0f);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_108)) {
        s16 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_108)]->id;

        if (this->cueId != GERUDO_AVEIL_CUEID_BEEHIVE_RUN_AWAY) {
            Cutscene_ActorTranslateAndYaw(&this->picto.actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_108));
        }

        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (this->cueId) {
                // Both cutscenes
                case GERUDO_AVEIL_CUEID_WAIT:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_HAND_ON_HIP_WAIT, ANIMMODE_LOOP, 0.0f);
                    break;

                // Instructing expedition leader cutscene
                case GERUDO_AVEIL_CUEID_PAUSE:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_TURN_HEAD, ANIMMODE_ONCE, 0.0f);
                    this->skelAnime.playSpeed = 0.0f;
                    break;

                case GERUDO_AVEIL_CUEID_INSTRUCT:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_TURN_HEAD, ANIMMODE_ONCE, 0.0f);
                    break;

                case GERUDO_AVEIL_CUEID_DEMAND:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_DEMAND, ANIMMODE_LOOP, 0.0f);
                    break;

                case GERUDO_AVEIL_CUEID_DISMISS:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_DISMISS, ANIMMODE_ONCE, 0.0f);
                    break;

                case GERUDO_AVEIL_CUEID_BEG:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_BEG, ANIMMODE_LOOP, 0.0f);
                    break;

                // Fleeing from bees cutscene
                case GERUDO_AVEIL_CUEID_BEEHIVE_RUN_AWAY:
                    EnGe3_ChangeAnim(this, GERUDO_AVEIL_ANIM_RUN_AWAY, ANIMMODE_LOOP, 0.0f);
                    this->picto.actor.speed = 5.0f;
                    this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                    break;

                case GERUDO_AVEIL_CUEID_BEEHIVE_VANISH:
                    Actor_Kill(&this->picto.actor);
                    break;

                default:
                    break;
            }

            SkelAnime_Update(&this->skelAnime);
        }
    } else if ((this->picto.actor.xzDistToPlayer < 700.0f) && (fabsf(this->picto.actor.playerHeightRel) < 100.0f) &&
               !Play_InCsMode(play)) {
        Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_26);
        Lib_PlaySfx(NA_SE_SY_FOUND);

        if (Player_GetMask(play) == PLAYER_MASK_STONE) { // Not fooled by Stone Mask
            Message_StartTextbox(play, 0x11AF, &this->picto.actor);
        } else {
            Message_StartTextbox(play, 0x11AE, &this->picto.actor);
        }

        this->actionFunc = EnGe3_ThrowPlayerOut;
        this->actionTimer = 50;
        SET_WEEKEVENTREG(WEEKEVENTREG_80_08);
    }

    if (this->cueId == GERUDO_AVEIL_CUEID_BEEHIVE_RUN_AWAY) {
        this->picto.actor.speed = 5.0f;
        EnGe3_FollowPath(this);

        if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_PIRATE_WALK);
        }

        if (this->screamTimer > 0) {
            this->screamTimer--;
        } else {
            this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
            EnGe3_Scream(this);
        }
    }
}

void EnGe3_UpdateColliderAndMove(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe3* this = (EnGe3*)thisx;

    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_MoveWithGravity(&this->picto.actor);
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_4);
}

void EnGe3_Blink(EnGe3* this, PlayState* play) {
    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

void EnGe3_Update(Actor* thisx, PlayState* play) {
    EnGe3* this = (EnGe3*)thisx;

    EnGe3_UpdateColliderAndMove(&this->picto.actor, play);
    this->actionFunc(this, play);
    EnGe3_Blink(this, play);
}

s32 EnGe3_ValidatePictograph(PlayState* play, Actor* thisx) {
    s32 ret = Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_GOOD, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                      400.0f, -1);

    ret |= Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_TOO_FAR, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                   1200.0f, -1);

    return ret;
}

s32 EnGe3_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnGe3* this = (EnGe3*)thisx;

    switch (limbIndex) {
        case GERUDO_RED_LIMB_VEIL:
        case GERUDO_RED_LIMB_RIGHT_SWORD:
        case GERUDO_RED_LIMB_LEFT_SWORD:
            *dList = NULL;
            return false;

        case GERUDO_RED_LIMB_HEAD:
            rot->x += this->headRot.y;
            // fallthrough
        default:
            // This is required since EnGe3 shares a skeleton with EnKaizoku; it avoids stale colours being used in the
            // displaylists.
            OPEN_DISPS(play->state.gfxCtx);

            switch (limbIndex) {
                case GERUDO_RED_LIMB_NECK:
                    break;

                case GERUDO_RED_LIMB_HEAD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
                    break;

                case GERUDO_RED_LIMB_RIGHT_SWORD:
                case GERUDO_RED_LIMB_LEFT_SWORD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                    break;

                default:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
                    break;
            }

            CLOSE_DISPS(play->state.gfxCtx);

            if (this->animIndex == GERUDO_AVEIL_ANIM_HAND_ON_HIP_WAIT) {
                // Fidget
                if ((limbIndex == GERUDO_RED_LIMB_RIGHT_FOREARM) || (limbIndex == GERUDO_RED_LIMB_LEFT_FOREARM) ||
                    (limbIndex == GERUDO_RED_LIMB_TORSO)) {
                    s32 temp = limbIndex * 50;

                    // required to match
                    rot->y = rot->y + (s32)(Math_SinS(play->state.frames * (temp + 0x814)) * 200.0f);
                    rot->z = rot->z + (s32)(Math_CosS(play->state.frames * (temp + 0x940)) * 200.0f);
                }
            }
            break;
    }
    return false;
}

void EnGe3_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnGe3* this = (EnGe3*)thisx;
    Vec3f sFocusOffset = { 600.0f, 700.0f, 0.0f };

    if (limbIndex == GERUDO_RED_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->picto.actor.focus.pos);
    }
}

void EnGe3_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gGerudoRedEyeOpenTex,
        gGerudoRedEyeHalfTex,
        gGerudoRedEyeClosedTex,
    };
    s32 pad;
    EnGe3* this = (EnGe3*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    func_800B8050(&this->picto.actor, play, 0);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe3_OverrideLimbDraw, EnGe3_PostLimbDraw, &this->picto.actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
