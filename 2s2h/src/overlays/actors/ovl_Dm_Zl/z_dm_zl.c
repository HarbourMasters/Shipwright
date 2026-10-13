/*
 * File: z_dm_zl.c
 * Overlay: ovl_Dm_Zl
 * Description: Child Zelda (Re-learning Song of Time cutscene)
 */

#include "z_dm_zl.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void DmZl_Init(Actor* thisx, PlayState* play);
void DmZl_Destroy(Actor* thisx, PlayState* play);
void DmZl_Update(Actor* thisx, PlayState* play);
void DmZl_Draw(Actor* thisx, PlayState* play);

void DmZl_DoNothing(DmZl* this, PlayState* play);

ActorProfile Dm_Zl_Profile = {
    /**/ ACTOR_DM_ZL,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_ZL4,
    /**/ sizeof(DmZl),
    /**/ DmZl_Init,
    /**/ DmZl_Destroy,
    /**/ DmZl_Update,
    /**/ DmZl_Draw,
};

typedef enum {
    /* 0 */ ZELDA_ANIM_FACING_AWAY,
    /* 1 */ ZELDA_ANIM_TURNING_TOWARD_PLAYER,
    /* 2 */ ZELDA_ANIM_FACING_PLAYER,
    /* 3 */ ZELDA_ANIM_GIVING_OCARINA_START,
    /* 4 */ ZELDA_ANIM_GIVING_OCARINA,
    /* 5 */ ZELDA_ANIM_PLAYING_OCARINA_START,
    /* 6 */ ZELDA_ANIM_PLAYING_OCARINA,
    /* 7 */ ZELDA_ANIM_MAX
} DmZlAnimation;

static AnimationInfo sAnimationInfo[ZELDA_ANIM_MAX] = {
    { &gDmZl4FacingAwayHandsOverEmblemLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f }, // ZELDA_ANIM_FACING_AWAY
    { &gDmZl4TurningAround2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },       // ZELDA_ANIM_TURNING_TOWARD_PLAYER
    { &gDmZl4HandsOverEmblemLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },  // ZELDA_ANIM_FACING_PLAYER
    { &gDmZl4GivingItemStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },      // ZELDA_ANIM_GIVING_OCARINA_START
    { &gDmZl4GivingItemLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },       // ZELDA_ANIM_GIVING_OCARINA
    { &gDmZl4RaisingOcarinaToPlayAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f }, // ZELDA_ANIM_PLAYING_OCARINA_START
    { &gDmZl4PlayingOcarinaAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },       // ZELDA_ANIM_PLAYING_OCARINA
};

static TexturePtr sMouthTextures[] = {
    gZl4MouthNeutralTex,
    gZl4MouthOpenSmilingTex,
    gZl4MouthFrowningTex,
    gZl4MouthOpenTex,
};

typedef enum {
    /* 0 */ ZELDA_MOUTH_NEUTRAL,
    /* 1 */ ZELDA_MOUTH_SMILING,
    /* 2 */ ZELDA_MOUTH_FROWNING,
    /* 3 */ ZELDA_MOUTH_OPEN
} DmZlMouthTextures;

static TexturePtr sEyeTextures[] = {
    gDmZl4EyeOpenNormalTex,
    gDmZl4EyeHalfTex,
    gDmZl4EyeClosedTex,
    gDmZl4EyeWideTex,
    gDmZl4EyeHappyTex,
    gDmZl4EyeOpenLookingLeftTex,
    gDmZl4EyeOpenLookingRightTex,
};

typedef enum {
    /* 0 */ ZELDA_EYE_OPEN_NORMAL,
    /* 1 */ ZELDA_EYE_BLINKING,
    /* 2 */ ZELDA_EYE_CLOSED,
    /* 3 */ ZELDA_EYE_WIDE,
    /* 4 */ ZELDA_EYE_HAPPY,
    /* 5 */ ZELDA_EYE_OPEN_LOOKING_LEFT,
    /* 6 */ ZELDA_EYE_OPEN_LOOKING_RIGHT
} DmZlEyeTextures;

// Unused in MM
typedef enum {
    /* 0 */ ZELDA_EYE_STATE_NORMAL,
    /* 1 */ ZELDA_EYE_STATE_CLOSED,
    /* 2 */ ZELDA_EYE_STATE_LOOKING_LEFT,
    /* 3 */ ZELDA_EYE_STATE_LOOKING_RIGHT,
    /* 4 */ ZELDA_EYE_STATE_WIDE,
    /* 5 */ ZELDA_EYE_STATE_HAPPY,
    /* 6 */ ZELDA_EYE_STATE_CLOSED2
} DmZlEyeStates;

void DmZl_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
    f32 endFrame;

    animInfo += animIndex;

    if (animInfo->frameCount < 0.0f) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

void DmZl_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DmZl* this = (DmZl*)thisx;

    this->animIndex = ZELDA_ANIM_FACING_AWAY;
    this->unk_2BA = 0;
    this->actor.lockOnArrowOffset = 1000.0f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    // these three set to NULL should mean they are dynamically allocated
    SkelAnime_InitFlex(play, &this->skelAnime, &gZl4Skel, NULL, NULL, NULL, 0);
    DmZl_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmZl_DoNothing;
}

void DmZl_Destroy(Actor* thisx, PlayState* play) {
}

void DmZl_DoNothing(DmZl* this, PlayState* play) {
}

void DmZl_HandleCutscene(DmZl* this, PlayState* play) {
    s32 cueChannel;
    s32 animIndex;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_102)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_102);
        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            animIndex = ZELDA_ANIM_FACING_AWAY;

            switch (play->csCtx.actorCues[cueChannel]->id) {
                default:
                case 1:
                    break;

                case 2:
                    animIndex = ZELDA_ANIM_TURNING_TOWARD_PLAYER;
                    break;

                case 3:
                    animIndex = ZELDA_ANIM_GIVING_OCARINA_START;
                    break;

                case 4:
                    animIndex = ZELDA_ANIM_PLAYING_OCARINA_START;
                    break;
            }

            if (this->animIndex != animIndex) {
                this->animIndex = animIndex;
                DmZl_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        animIndex = cueChannel = this->animIndex;

        if ((animIndex == ZELDA_ANIM_TURNING_TOWARD_PLAYER) || (animIndex == ZELDA_ANIM_GIVING_OCARINA_START) ||
            (animIndex == ZELDA_ANIM_PLAYING_OCARINA_START)) {
            // these animations don't loop at the end, they lead into the next animation
            this->animIndex++;
            DmZl_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        }
    }
}

/**
 * Updates the eye blinking and state, and mouth textures.
 */
void DmZl_UpdateFace(DmZl* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else {
        this->blinkTimer = 0;
    }

    if (this->blinkTimer < 3) {
        this->eyeTextureIndexRight = this->blinkTimer;
        this->eyeTextureIndexLeft = this->blinkTimer;
    }

    // nextEyeState is never changed by this actor in MM, only ZELDA_EYE_STATE_NORMAL used.
    switch (this->nextEyeState) {
        case ZELDA_EYE_STATE_NORMAL:
            if (this->blinkTimer == 0) {
                this->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;
        case ZELDA_EYE_STATE_CLOSED:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndexLeft = this->eyeTextureIndexRight = ZELDA_EYE_CLOSED;
            }
            break;
        case ZELDA_EYE_STATE_LOOKING_LEFT:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndexLeft = ZELDA_EYE_OPEN_LOOKING_LEFT;
                this->eyeTextureIndexRight = ZELDA_EYE_OPEN_LOOKING_RIGHT;
            }
            break;
        case ZELDA_EYE_STATE_LOOKING_RIGHT:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndexLeft = ZELDA_EYE_OPEN_LOOKING_RIGHT;
                this->eyeTextureIndexRight = ZELDA_EYE_OPEN_LOOKING_LEFT;
            }
            break;
        case ZELDA_EYE_STATE_WIDE:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndexLeft = this->eyeTextureIndexRight = ZELDA_EYE_WIDE;
            }
            break;
        case ZELDA_EYE_STATE_HAPPY:
            if (this->blinkTimer == 0) {
                this->eyeTextureIndexLeft = this->eyeTextureIndexRight = ZELDA_EYE_HAPPY;
            }
            break;
        case ZELDA_EYE_STATE_CLOSED2:
            if (this->blinkTimer >= 3) {
                this->blinkTimer = 0;
            }
            break;
    }

    // nextMouthState is never changed by this actor in MM, only ZELDA_MOUTH_NEUTRAL used.
    switch (this->nextMouthState) {
        default:
            this->mouthTextureIndex = ZELDA_MOUTH_NEUTRAL;
            break;
        case ZELDA_MOUTH_SMILING:
            this->mouthTextureIndex = ZELDA_MOUTH_SMILING;
            break;
        case ZELDA_MOUTH_FROWNING:
            this->mouthTextureIndex = ZELDA_MOUTH_FROWNING;
            break;
        case ZELDA_MOUTH_OPEN:
            this->mouthTextureIndex = ZELDA_MOUTH_OPEN;
            break;
    }

    if (this->animIndex == ZELDA_ANIM_PLAYING_OCARINA) {
        this->eyeTextureIndexLeft = this->eyeTextureIndexRight = ZELDA_EYE_CLOSED;
    }
}

void DmZl_Update(Actor* thisx, PlayState* play) {
    DmZl* this = (DmZl*)thisx;

    DmZl_UpdateFace(this);
    SkelAnime_Update(&this->skelAnime);
    DmZl_HandleCutscene(this, play);
    this->actionFunc(this, play);
}

s32 DmZl_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void DmZl_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    DmZl* this = (DmZl*)thisx;

    if (limbIndex == ZL4_LIMB_RIGHT_HAND) {
        if ((this->animIndex >= ZELDA_ANIM_GIVING_OCARINA_START) && (this->animIndex <= ZELDA_ANIM_PLAYING_OCARINA)) {
            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gDmZl4OcarinaDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void DmZl_Draw(Actor* thisx, PlayState* play) {
    DmZl* this = (DmZl*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTextureIndexRight]));

    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTextureIndexLeft]));

    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(sMouthTextures[this->mouthTextureIndex]));

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          DmZl_OverrideLimbDraw, DmZl_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
