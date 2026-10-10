/*
 * File: z_dm_char02.c
 * Overlay: ovl_Dm_Char02
 * Description: The Ocarina of Time that for the scenes on the Clock Tower's rooftop.
 */

#include "z_dm_char02.h"
#include "objects/object_stk2/object_stk2.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar02_Init(Actor* thisx, PlayState* play);
void DmChar02_Destroy(Actor* thisx, PlayState* play);
void DmChar02_Update(Actor* thisx, PlayState* play);
void DmChar02_Draw(Actor* thisx, PlayState* play);

void DmChar02_HandleCutscene(DmChar02* this, PlayState* play);

ActorProfile Dm_Char02_Profile = {
    /**/ ACTOR_DM_CHAR02,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_STK2,
    /**/ sizeof(DmChar02),
    /**/ DmChar02_Init,
    /**/ DmChar02_Destroy,
    /**/ DmChar02_Update,
    /**/ DmChar02_Draw,

};

typedef enum {
    /* 0 */ DMCHAR02_ANIM_HIT_GROUND,
    /* 1 */ DMCHAR02_ANIM_TURN_AROUND,
    /* 2 */ DMCHAR02_ANIM_JUGGLE,
    /* 3 */ DMCHAR02_ANIM_FALL,
    /* 4 */ DMCHAR02_ANIM_MAX
} DmChar02Animation;

static AnimationInfo sAnimationInfo[DMCHAR02_ANIM_MAX] = {
    { &gClockTowerOcarinaOfTimeHitGroundAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },  // DMCHAR02_ANIM_HIT_GROUND
    { &gClockTowerOcarinaOfTimeTurnAroundAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // DMCHAR02_ANIM_TURN_AROUND
    { &gClockTowerOcarinaOfTimeJuggleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },     // DMCHAR02_ANIM_JUGGLE
    { &gClockTowerOcarinaOfTimeFallAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },       // DMCHAR02_ANIM_FALL
};

void DmChar02_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
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

void DmChar02_PlaySfxForDroppingOcarinaCutscene(DmChar02* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 95:
            Actor_PlaySfx(&this->actor, NA_SE_EV_OCARINA_BOUND_0);
            break;

        case 101:
        case 105:
        case 112:
            Actor_PlaySfx(&this->actor, NA_SE_EV_OCARINA_BOUND_1);
            break;
    }
}

void DmChar02_PlaySfxForCutscenes(DmChar02* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->sceneId == SCENE_OKUJOU) && (play->csCtx.scriptIndex == 1)) {
        DmChar02_PlaySfxForDroppingOcarinaCutscene(this, play);
    }
}

void DmChar02_Init(Actor* thisx, PlayState* play) {
    DmChar02* this = (DmChar02*)thisx;

    if (GameInteractor_Should(VB_STK_HAVE_OCARINA,
                              gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE)) {
        this->animIndex = DMCHAR02_ANIM_HIT_GROUND;
        this->actor.lockOnArrowOffset = 3000.0f;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gClockTowerOcarinaOfTimeSkel, NULL, NULL, NULL, 0);
        DmChar02_ChangeAnim(&this->skelAnime, &sAnimationInfo[DMCHAR02_ANIM_HIT_GROUND], 0);
        Actor_SetScale(&this->actor, 0.01f);
        this->actionFunc = DmChar02_HandleCutscene;
    } else {
        Actor_Kill(&this->actor);
    }
}

void DmChar02_Destroy(Actor* thisx, PlayState* play) {
}

void DmChar02_HandleCutscene(DmChar02* this, PlayState* play) {
    u8 changeAnim = true;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_131)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_131);
        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            switch (play->csCtx.actorCues[cueChannel]->id) {
                default:
                    this->animIndex = DMCHAR02_ANIM_HIT_GROUND;
                    changeAnim = false;
                    break;

                case 1:
                    this->animIndex = DMCHAR02_ANIM_HIT_GROUND;
                    break;

                case 2:
                    this->animIndex = DMCHAR02_ANIM_TURN_AROUND;
                    break;

                case 4:
                    this->animIndex = DMCHAR02_ANIM_FALL;
                    break;
            }

            if (changeAnim) {
                DmChar02_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->animIndex == DMCHAR02_ANIM_TURN_AROUND) {
            this->animIndex++;
            DmChar02_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        }
    }
}

void DmChar02_Update(Actor* thisx, PlayState* play) {
    DmChar02* this = (DmChar02*)thisx;

    SkelAnime_Update(&this->skelAnime);

    this->unk_2F0 = this->unk_2F0; // Set to itself

    this->actionFunc(this, play);

    if (!Actor_HasParent(&this->actor, play)) {
        Actor_OfferGetItem(&this->actor, play, GI_OCARINA_OF_TIME, 30.0f, 80.0f);
    } else {
        gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
        Actor_Kill(&this->actor);
    }

    DmChar02_PlaySfxForCutscenes(this, play);
}

s32 DmChar02_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return GameInteractor_Should(VB_OVERRIDE_CHAR02_LIMB, false, dList);
}

void DmChar02_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    GameInteractor_Should(VB_POST_CHAR02_LIMB, true, thisx); // no-op
}

void DmChar02_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void DmChar02_Draw(Actor* thisx, PlayState* play) {
    s32 pad[2];
    DmChar02* this = (DmChar02*)thisx;
    s32 shouldDraw = false;

    if ((play->csCtx.state == CS_STATE_IDLE) && (this->actor.world.pos.y < 100.0f)) {
        shouldDraw = true;
    } else if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_107)) {
        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_107)]->id) {
            case 0x17:
            case 0x1C:
            case 0x26:
                shouldDraw = true;
                break;
        }
    }

    if (shouldDraw) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, DmChar02_OverrideLimbDraw, DmChar02_PostLimbDraw,
                                       DmChar02_TransformLimbDraw, &this->actor);
    }
}
