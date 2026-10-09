/*
 * File: z_dm_gm.c
 * Overlay: ovl_Dm_Gm
 * Description: Complete duplicate of Dm_An
 */

#include "z_dm_gm.h"
#include "objects/object_an4/object_an4.h"
#include "objects/object_msmo/object_msmo.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void DmGm_Init(Actor* thisx, PlayState* play);
void DmGm_Destroy(Actor* thisx, PlayState* play);
void DmGm_Update(Actor* thisx, PlayState* play);

void DmGm_WaitForObject(DmGm* this, PlayState* play);
void DmGm_HandleCouplesMaskCs(DmGm* this, PlayState* play);
void DmGm_DoNothing(DmGm* this, PlayState* play);
void DmGm_Draw(Actor* thisx, PlayState* play);

ActorProfile Dm_Gm_Profile = {
    /**/ ACTOR_DM_GM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AN1,
    /**/ sizeof(DmGm),
    /**/ DmGm_Init,
    /**/ DmGm_Destroy,
    /**/ DmGm_Update,
    /**/ NULL,
};

/**
 * Anju stores her animations across different objects and the ones used by this actor are put together right next to
 * each other in the sAnimationInfo array. Due to this, animation functions check which object to load by comparing
 * index ranges. To make this a bit easier to read, this enum includes `DMGM_ANIMOBJ_*` values that mark when a range of
 * animations of a certain object start
 */
typedef enum DmGmAnimation {
    /* -1 */ DMGM_ANIM_NONE = -1,
    /*  0 */ DMGM_ANIM_SITTING_IN_DISBELIEF,
    /*  1 */ DMGM_ANIM_SIT,

    /*  2 */ DMGM_ANIMOBJ_AN4,
    /*  2 */ DMGM_ANIM_MASK_STAND_LOOP = DMGM_ANIMOBJ_AN4,
    /*  3 */ DMGM_ANIM_HOLD_HANDS, //! @bug See note at `sAnimationInfo`
    /*  4 */ DMGM_ANIM_MASK_KNEEL,
    /*  5 */ DMGM_ANIM_MASK_KNEEL_LOOP,
    /*  6 */ DMGM_ANIM_HUG,
    /*  7 */ DMGM_ANIM_HUG_LOOP,
    /*  8 */ DMGM_ANIM_HUG_RELEASE,
    /*  9 */ DMGM_ANIM_HUG_RELEASE_LOOP,
    /* 10 */ DMGM_ANIM_COMBINE_MASKS_1,
    /* 11 */ DMGM_ANIM_COMBINE_MASKS_2,
    /* 12 */ DMGM_ANIM_LOOK_UP,
    /* 13 */ DMGM_ANIM_LOOK_UP_LOOP,
    /* 14 */ DMGM_ANIM_MAX
} DmGmAnimation;

static AnimationInfoS sAnimationInfo[DMGM_ANIM_MAX] = {
    { &gAnju1SittingInDisbeliefAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // DMGM_ANIM_SITTING_IN_DISBELIEF
    { &gAnju1SitAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                // DMGM_ANIM_SIT

    // DMGM_ANIMOBJ_AN4
    { &gAnju4MaskStandLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // DMGM_ANIM_MASK_STAND_LOOP
    //! @bug Uses symbol from OBJECT_AN1 instead of OBJECT_AN4
    { &gAnju1HoldHandsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },     // DMGM_ANIM_HOLD_HANDS
    { &gAnju4MaskKneelAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },      // DMGM_ANIM_MASK_KNEEL
    { &gAnju4MaskKneelLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // DMGM_ANIM_MASK_KNEEL_LOOP
    { &gAnju4HugAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },            // DMGM_ANIM_HUG
    { &gAnju4HugLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // DMGM_ANIM_HUG_LOOP
    { &gAnju4HugReleaseAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },     // DMGM_ANIM_HUG_RELEASE
    { &gAnju4HugReleaseLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // DMGM_ANIM_HUG_RELEASE_LOOP
    { &gAnju4CombineMasks1Anim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // DMGM_ANIM_COMBINE_MASKS_1
    { &gAnju4CombineMasks2Anim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // DMGM_ANIM_COMBINE_MASKS_2
    { &gAnju4LookUpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },         // DMGM_ANIM_LOOK_UP
    { &gAnju4LookUpLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },     // DMGM_ANIM_LOOK_UP_LOOP
};

s32 DmGm_UpdateSkelAnime(DmGm* this, PlayState* play) {
    s8 objectSlot = this->actor.objectSlot;
    s8 objectIndex2;
    s32 isAnimFinished = false;

    if (this->animIndex < DMGM_ANIMOBJ_AN4) {
        objectIndex2 = this->actor.objectSlot;
    } else {
        objectIndex2 = this->an4ObjectSlot;
    }

    if (objectIndex2 > OBJECT_SLOT_NONE) {
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectIndex2].segment);
        isAnimFinished = SkelAnime_Update(&this->skelAnime);
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
    }

    return isAnimFinished;
}

s32 DmGm_ChangeAnim(DmGm* this, PlayState* play, DmGmAnimation animIndex) {
    s8 objectSlot = this->actor.objectSlot;
    s8 objectIndex2;
    s32 didAnimChange = false;

    if (animIndex < DMGM_ANIMOBJ_AN4) {
        objectIndex2 = this->actor.objectSlot;
    } else {
        objectIndex2 = this->an4ObjectSlot;
    }

    if ((objectIndex2 > OBJECT_SLOT_NONE) && (this->animIndex != animIndex)) {
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectIndex2].segment);
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
    }

    return didAnimChange;
}

typedef enum DmGmEyes {
    /* 0 */ DMGM_EYES_OPEN,
    /* 1 */ DMGM_EYES_HALF1,
    /* 2 */ DMGM_EYES_CLOSED,
    /* 3 */ DMGM_EYES_HALF2,
    /* 4 */ DMGM_EYES_COMFORT,
    /* 5 */ DMGM_EYES_SAD,
    /* 6 */ DMGM_EYES_RELIEVED_CLOSED,
    /* 7 */ DMGM_EYES_MAX
} DmGmEyes;

typedef enum DmGmMouth {
    /* 0 */ DMGM_MOUTH_CLOSED,
    /* 1 */ DMGM_MOUTH_HAPPY,
    /* 2 */ DMGM_MOUTH_OPEN,
    /* 3 */ DMGM_MOUTH_MAX
} DmGmMouth;

void DmGm_Blink(DmGm* this) {
    if (DECR(this->eyeTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex > DMGM_EYES_HALF2) {
            this->eyeTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

s32 DmGm_UpdateHeadRot(DmGm* this, PlayState* play) {
    s32 pad;
    Vec3f lookAtActorPos;
    Vec3f pos;
    s16 yaw;

    Math_Vec3f_Copy(&lookAtActorPos, &this->lookAtActor->world.pos);
    Math_Vec3f_Copy(&pos, &this->actor.world.pos);
    yaw = Math_Vec3f_Yaw(&pos, &lookAtActorPos);

    Math_ApproachS(&this->headRotY, (yaw - this->torsoRotY) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->headRotY = CLAMP(this->headRotY, -0x1FFE, 0x1FFE);

    Math_ApproachS(&this->torsoRotY, (yaw - this->headRotY) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->torsoRotY = CLAMP(this->torsoRotY, -0x1C70, 0x1C70);

    if (this->lookAtActor->id == ACTOR_PLAYER) {
        lookAtActorPos.y = ((Player*)this->lookAtActor)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&lookAtActorPos, &this->lookAtActor->focus.pos);
    }

    Math_Vec3f_Copy(&pos, &this->actor.focus.pos);
    Math_ApproachS(&this->headRotZ, Math_Vec3f_Pitch(&pos, &lookAtActorPos) - this->torsoRotZ, 4, 0x2AA8);
    this->headRotZ = CLAMP(this->headRotZ, -0x1C70, 0x1C70);

    Math_ApproachS(&this->torsoRotZ, Math_Vec3f_Pitch(&pos, &lookAtActorPos) - this->headRotZ, 4, 0x2AA8);
    this->torsoRotZ = CLAMP(this->torsoRotZ, -0x1C70, 0x1C70);

    return true;
}

s32 DmGm_UpdateAttention(DmGm* this, PlayState* play) {
    if (this->lookAtActor != NULL) {
        DmGm_UpdateHeadRot(this, play);
        this->stateFlags &= ~DMGM_STATE_LOST_ATTENTION;
        this->stateFlags |= DMGM_STATE_FACE_TARGET;
    } else if (this->stateFlags & DMGM_STATE_FACE_TARGET) {
        this->stateFlags &= ~DMGM_STATE_FACE_TARGET;
        this->headRotZ = 0;
        this->headRotY = 0;
        this->torsoRotZ = 0;
        this->torsoRotY = 0;
        this->loseAttentionTimer = 20;
    } else if (DECR(this->loseAttentionTimer) == 0) {
        this->stateFlags |= DMGM_STATE_LOST_ATTENTION;
    }
    return true;
}

Actor* DmGm_FindAnjusMotherActor(PlayState* play) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, ACTORCAT_NPC, ACTOR_DM_AH);

        if (actorIter == NULL) {
            break;
        }

        if (actorIter->update != NULL) {
            break;
        }

        if ((actorIter->next == NULL) || false) {
            actorIter = NULL;
            break;
        }
        actorIter = actorIter->next;
    }
    return actorIter;
}

void DmGm_WaitForObject(DmGm* this, PlayState* play) {
    if ((this->an4ObjectSlot > OBJECT_SLOT_NONE) && SubS_IsObjectLoaded(this->an4ObjectSlot, play) &&
        (this->msmoObjectSlot > OBJECT_SLOT_NONE) && SubS_IsObjectLoaded(this->msmoObjectSlot, play)) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 14.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gAnju1Skel, NULL, this->jointTable, this->morphTable,
                           ANJU1_LIMB_MAX);

        this->animIndex = DMGM_ANIM_NONE;
        DmGm_ChangeAnim(this, play, DMGM_ANIM_SITTING_IN_DISBELIEF);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_SetScale(&this->actor, 0.01f);
        this->stateFlags |= 1;
        this->actor.draw = DmGm_Draw;

        if ((play->sceneId == SCENE_YADOYA) && (play->curSpawn == 4)) {
            this->lookAtActor = DmGm_FindAnjusMotherActor(play);
            DmGm_ChangeAnim(this, play, DMGM_ANIM_SIT);
            this->actionFunc = DmGm_DoNothing;
        } else {
            this->actionFunc = DmGm_HandleCouplesMaskCs;
        }
    }
}

void DmGm_HandleCouplesMaskCs(DmGm* this, PlayState* play) {
    s32 csAnimIndex[] = {
        /* 0 */ 0, // no cue
        /* 1 */ DMGM_ANIM_SITTING_IN_DISBELIEF,
        /* 2 */ DMGM_ANIM_LOOK_UP,
        /* 3 */ DMGM_ANIM_MASK_STAND_LOOP,
        /* 4 */ DMGM_ANIM_MASK_KNEEL,
        /* 5 */ DMGM_ANIM_HUG,
        /* 6 */ DMGM_ANIM_HUG_RELEASE,
        /* 7 */ DMGM_ANIM_COMBINE_MASKS_1,
        /* 8 */ DMGM_ANIM_COMBINE_MASKS_2,
        /* 9 */ DMGM_ANIM_HOLD_HANDS,
    };
    u16 cueId;
    s32 cueChannel;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (!this->isCutscenePlaying) {
            this->cueId = 255;
            this->isCutscenePlaying = true;
            this->didAnimChangeInCs = false;
            this->prevAnimIndex = this->animIndex;
        }

        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_557)) {
            cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_557);
            cueId = play->csCtx.actorCues[cueChannel]->id;

            if (this->cueId != (u8)cueId) {
                this->cueId = cueId;
                this->didAnimChangeInCs = true;
                DmGm_ChangeAnim(this, play, csAnimIndex[cueId]);
            }

            switch (this->cueId) {
                case 2:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                    if ((this->animIndex == DMGM_ANIM_LOOK_UP) || (this->animIndex == DMGM_ANIM_MASK_KNEEL) ||
                        (this->animIndex == DMGM_ANIM_HUG) || (this->animIndex == DMGM_ANIM_HUG_RELEASE)) {
                        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                            DmGm_ChangeAnim(this, play, this->animIndex + 1);
                        }
                    }
                    break;

                default:
                    break;
            }
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        }
    } else if (this->isCutscenePlaying) {
        this->isCutscenePlaying = false;
        this->didAnimChangeInCs = false;
        DmGm_ChangeAnim(this, play, this->prevAnimIndex);
    }
}

void DmGm_DoNothing(DmGm* this, PlayState* play) {
}

void DmGm_Init(Actor* thisx, PlayState* play) {
    DmGm* this = (DmGm*)thisx;

    this->an4ObjectSlot = SubS_GetObjectSlot(OBJECT_AN4, play);
    this->msmoObjectSlot = SubS_GetObjectSlot(OBJECT_MSMO, play);
    this->actionFunc = DmGm_WaitForObject;
}

void DmGm_Destroy(Actor* thisx, PlayState* play) {
}

void DmGm_Update(Actor* thisx, PlayState* play) {
    DmGm* this = (DmGm*)thisx;

    this->actionFunc(this, play);

    DmGm_UpdateAttention(this, play);

    if (this->actor.draw != NULL) {
        DmGm_UpdateSkelAnime(this, play);
        DmGm_Blink(this);
    }
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void DmGm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad[2];
    DmGm* this = (DmGm*)thisx;
    s8 objectSlot = this->actor.objectSlot;
    s8 msmoObjectSlot = this->msmoObjectSlot;

    if ((limbIndex == ANJU1_LIMB_LEFT_HAND) && this->didAnimChangeInCs) {
        static Vec3f D_80C25218 = { 450.0f, 700.0f, -760.0f };
        static Vec3s D_80C25224 = { 0x238C, 0, -0x3FFC };

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_TranslateRotateZYX(&D_80C25218, &D_80C25224);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[msmoObjectSlot].segment);
        gSPDisplayList(POLY_OPA_DISP++, gMoonMaskDL);
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (limbIndex == ANJU1_LIMB_HEAD) {
        static Vec3f D_80C2522C = { 1000.0f, 0.0f, 0.0f };

        Matrix_MultVec3f(&D_80C2522C, &this->actor.focus.pos);
        Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
    }
}

void DmGm_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    DmGm* this = (DmGm*)thisx;
    s16 stepRot;
    s16 overrideRot;

    if (!(this->stateFlags & DMGM_STATE_LOST_ATTENTION)) {
        if (this->stateFlags & DMGM_STATE_FACE_TARGET) {
            overrideRot = true;
        } else {
            overrideRot = false;
        }
        stepRot = true;
    } else {
        stepRot = false;
        overrideRot = false;
    }

    if (limbIndex == ANJU1_LIMB_HEAD) {
        SubS_UpdateLimb(this->headRotZ + this->torsoRotZ + 0x4000,
                        this->headRotY + this->torsoRotY + this->actor.shape.rot.y + 0x4000, &this->headComputedPos,
                        &this->headComputedRot, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->headComputedPos.x, this->headComputedPos.y, this->headComputedPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->headComputedRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->headComputedRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->headComputedRot.z, MTXMODE_APPLY);
        Matrix_Push();
    } else if (limbIndex == ANJU1_LIMB_TORSO) {
        SubS_UpdateLimb(this->torsoRotZ + 0x4000, this->torsoRotY + this->actor.shape.rot.y + 0x4000,
                        &this->torsoComputedPos, &this->torsoComputedRot, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->torsoComputedPos.x, this->torsoComputedPos.y, this->torsoComputedPos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->torsoComputedRot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->torsoComputedRot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->torsoComputedRot.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

void DmGm_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sMouthTextures[DMGM_MOUTH_MAX] = {
        gAnju1MouthClosedTex, // DMGM_MOUTH_CLOSED
        gAnju1MouthHappyTex,  // DMGM_MOUTH_HAPPY
        gAnju1MouthOpenTex,   // DMGM_MOUTH_OPEN
    };
    static TexturePtr sEyeTextures[DMGM_EYES_MAX] = {
        gAnju1EyeOpenTex,           // DMGM_EYES_OPEN
        gAnju1EyeHalfTex,           // DMGM_EYES_HALF1
        gAnju1EyeClosedTex,         // DMGM_EYES_CLOSED
        gAnju1EyeHalfTex,           // DMGM_EYES_HALF2
        gAnju1EyeComfortTex,        // DMGM_EYES_COMFORT
        gAnju1EyeSadTex,            // DMGM_EYES_SAD
        gAnju1EyeRelievedClosedTex, // DMGM_EYES_RELIEVED_CLOSED
    };
    DmGm* this = (DmGm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sMouthTextures[DMGM_MOUTH_CLOSED]));

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, DmGm_PostLimbDraw, DmGm_TransformLimbDraw,
                                   &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
