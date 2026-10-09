/*
 * File: z_dm_char03.c
 * Overlay: ovl_Dm_Char03
 * Description: Deku Mask (cutscene)
 */

#include "z_dm_char03.h"
#include "objects/object_osn/object_osn.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar03_Init(Actor* thisx, PlayState* play);
void DmChar03_Destroy(Actor* thisx, PlayState* play);
void DmChar03_Update(Actor* thisx, PlayState* play);
void DmChar03_Draw(Actor* thisx, PlayState* play);

void func_80AAB644(DmChar03* this, PlayState* play);
void DmChar03_DoNothing(DmChar03* this, PlayState* play);
void func_80AABA84(PlayState* play, DmChar03* this);

ActorProfile Dm_Char03_Profile = {
    /**/ ACTOR_DM_CHAR03,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_OSN,
    /**/ sizeof(DmChar03),
    /**/ DmChar03_Init,
    /**/ DmChar03_Destroy,
    /**/ DmChar03_Update,
    /**/ DmChar03_Draw,
};

typedef enum {
    /* 0 */ DMCHAR03_ANIM_FALL_OVER,
    /* 1 */ DMCHAR03_ANIM_MAX
} DmChar03Animation;

static AnimationInfo sAnimationInfo[DMCHAR03_ANIM_MAX] = {
    { &gDekuMaskFallOverAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // DMCHAR03_ANIM_FALL_OVER
};

void DmChar03_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
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

void DmChar03_Init(Actor* thisx, PlayState* play) {
    DmChar03* this = (DmChar03*)thisx;

    this->animIndex = DMCHAR03_ANIM_FALL_OVER;
    this->actor.lockOnArrowOffset = 3000.0f;
    this->unk_18E = false;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDekuMaskSkel, NULL, NULL, NULL, 0);
    DmChar03_ChangeAnim(&this->skelAnime, &sAnimationInfo[DMCHAR03_ANIM_FALL_OVER], 0);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = DmChar03_DoNothing;
}

void DmChar03_Destroy(Actor* thisx, PlayState* play) {
}

void func_80AAB5F8(DmChar03* this, PlayState* play) {
    s32 objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_NUTSMASK);

    if (objectSlot > OBJECT_SLOT_NONE) {
        this->objectSlot = objectSlot;
        this->actionFunc = func_80AAB644;
    }
}

void func_80AAB644(DmChar03* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_136)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_136);

        if (play->csCtx.actorCues[cueChannel]->id == 4) {
            this->unk_18E = true;
            this->offset.x = play->csCtx.actorCues[cueChannel]->startPos.x;
            this->offset.y = play->csCtx.actorCues[cueChannel]->startPos.y;
            this->offset.z = play->csCtx.actorCues[cueChannel]->startPos.z;
        }
    }
}

void DmChar03_DoNothing(DmChar03* this, PlayState* play) {
}

void func_80AAB710(DmChar03* this, PlayState* play) {
    u8 changeAnim = true;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_136)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_136);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    changeAnim = false;
                    break;

                case 2:
                    this->animIndex = DMCHAR03_ANIM_FALL_OVER;
                    break;

                case 3:
                    this->unk_18E = false;
                    changeAnim = false;
                    Actor_Kill(&this->actor);
                    break;

                case 4:
                    Item_Give(play, ITEM_MASK_DEKU);
                    changeAnim = false;
                    this->actionFunc = func_80AAB5F8;
                    break;

                default:
                    changeAnim = false;
                    break;
            }

            if (changeAnim) {
                DmChar03_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }
}

void func_80AAB838(DmChar03* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_IT_MASK_BOUND_0);
    } else if (Animation_OnFrame(&this->skelAnime, 10.0f) || Animation_OnFrame(&this->skelAnime, 18.0f) ||
               Animation_OnFrame(&this->skelAnime, 30.0f) || Animation_OnFrame(&this->skelAnime, 38.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_IT_MASK_BOUND_1);
    }
}

void DmChar03_Update(Actor* thisx, PlayState* play) {
    DmChar03* this = (DmChar03*)thisx;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_136) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_136)]->id == 2)) {
        SkelAnime_Update(&this->skelAnime);
    }
    this->actionFunc(this, play);
    func_80AAB710(this, play);
    func_80AAB838(this, play);
}

s32 DmChar03_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void DmChar03_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void DmChar03_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void DmChar03_Draw(Actor* thisx, PlayState* play) {
    DmChar03* this = (DmChar03*)thisx;

    if (!this->unk_18E) {
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_136) &&
            (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_136)]->id != 1)) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, DmChar03_OverrideLimbDraw, DmChar03_PostLimbDraw,
                                           DmChar03_TransformLimbDraw, &this->actor);
        }
    } else {
        func_80AABA84(play, this);
    }
}

void func_80AABA84(PlayState* play, DmChar03* this) {
    s32 pad;

    if (this->actor.objectSlot == this->objectSlot) {
        Matrix_Translate(this->offset.x, this->offset.y, this->offset.z, MTXMODE_NEW);
        Matrix_RotateZYX(0, (play->gameplayFrames * 0x3E8), 0, MTXMODE_APPLY);
        Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);
        GetItem_Draw(play, GID_MASK_DEKU);
    }
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
    }
}
