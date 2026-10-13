/*
 * File: z_demo_moonend.c
 * Overlay: ovl_Demo_Moonend
 * Description: Moon Disappearing (cutscene)
 */

#include "z_demo_moonend.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoMoonend_Init(Actor* thisx, PlayState* play);
void DemoMoonend_Destroy(Actor* thisx, PlayState* play);
void DemoMoonend_Update(Actor* thisx, PlayState* play);
void DemoMoonend_Draw(Actor* thisx, PlayState* play);

void DemoMoonend_DoNothing(DemoMoonend* this, PlayState* play);
void func_80C17B60(DemoMoonend* this, PlayState* play);
void func_80C17C48(DemoMoonend* this, PlayState* play);
void func_80C17FCC(Actor* thisx, PlayState* play);

ActorProfile Demo_Moonend_Profile = {
    /**/ ACTOR_DEMO_MOONEND,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_MOONEND,
    /**/ sizeof(DemoMoonend),
    /**/ DemoMoonend_Init,
    /**/ DemoMoonend_Destroy,
    /**/ DemoMoonend_Update,
    /**/ DemoMoonend_Draw,
};

void DemoMoonend_Init(Actor* thisx, PlayState* play) {
    DemoMoonend* this = (DemoMoonend*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->actionFunc = DemoMoonend_DoNothing;

    if (DEMOMOONEND_GET_PARAM_F(thisx) == 1) {
        Actor_SetScale(&this->actor, 0.05f);
        Actor_SetScale(&this->actor, 6.0f);
        this->actor.draw = NULL;
        this->cueType = CS_CMD_ACTOR_CUE_558;
        this->actionFunc = func_80C17B60;
    } else {
        Actor_SetScale(&this->actor, 0.095f);
        Keyframe_InitFlex(&this->kfSkelAnime, gMoonendKFSkel_B5A0, gMoonendKFAnim_1214, this->jointTable,
                          this->morphTable, NULL);
        Keyframe_FlexPlayOnce(&this->kfSkelAnime, gMoonendKFAnim_1214);
        this->cueType = CS_CMD_ACTOR_CUE_560;
        this->actionFunc = func_80C17C48;
        this->actor.home.rot.z = 0;
        this->actor.draw = NULL;
        this->kfSkelAnime.frameCtrl.speed = 2.0f / 3.0f;
    }
}

void DemoMoonend_Destroy(Actor* thisx, PlayState* play) {
    DemoMoonend* this = (DemoMoonend*)thisx;

    if (DEMOMOONEND_GET_PARAM_F(thisx) != 1) {
        Keyframe_DestroyFlex(&this->kfSkelAnime);
    }
}

void DemoMoonend_DoNothing(DemoMoonend* this, PlayState* play) {
}

void func_80C17B60(DemoMoonend* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        u16 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;

        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (this->cueId) {
                case 1:
                    this->actor.draw = NULL;
                    break;

                case 2:
                    this->actor.draw = func_80C17FCC;
                    break;

                default:
                    break;
            }
        }
        if (this->cueId == 2) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_RAINBOW - SFX_FLAG);
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80C17C48(DemoMoonend* this, PlayState* play) {
    if (Keyframe_UpdateFlex(&this->kfSkelAnime)) {
        this->actor.home.rot.z = 0;
    }
    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        u16 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;

        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

        if (this->cueId != cueId) {
            this->cueId = cueId;
            switch (this->cueId) {
                case 1:
                    this->actor.draw = DemoMoonend_Draw;
                    Keyframe_FlexPlayOnce(&this->kfSkelAnime, gMoonendKFAnim_1214);
                    this->kfSkelAnime.frameCtrl.speed = 0.0f;
                    break;

                case 2:
                    this->actor.draw = DemoMoonend_Draw;
                    Keyframe_FlexPlayOnce(&this->kfSkelAnime, gMoonendKFAnim_1214);
                    this->kfSkelAnime.frameCtrl.speed = 2.0f / 3.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EV_MOON_EXPLOSION);
                    this->actor.home.rot.z = 1;
                    break;

                default:
                    break;
            }
        }
        if (this->actor.home.rot.z != 0) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_EARTHQUAKE_LAST2 - SFX_FLAG);
        }
    } else {
        this->actor.draw = NULL;
    }
    if ((play->csCtx.state != CS_STATE_IDLE) && (gSaveContext.sceneLayer == 8) && (play->csCtx.scriptIndex == 0)) {
        switch (play->csCtx.curFrame) {
            case 5:
                Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM1);
                break;

            case 50:
                Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM2);
                break;

            case 100:
                Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM3);
                break;

            case 150:
                Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM2);
                break;

            case 200:
                Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM4);
                break;

            default:
                break;
        }
    }
}

void DemoMoonend_Update(Actor* thisx, PlayState* play) {
    DemoMoonend* this = (DemoMoonend*)thisx;

    this->actionFunc(this, play);
}

s32 DemoMoonend_OverrideLimbDraw(PlayState* play, KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex, Gfx** dList, u8* flags,
                                 void* thisx, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    DemoMoonend* this = (DemoMoonend*)thisx;

    if (limbIndex == 2) {
        Matrix_Push();
        Matrix_RotateYS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000, MTXMODE_APPLY);
    }

    return true;
}

s32 DemoMoonend_PostLimbDraw(PlayState* play, KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex, Gfx** dList, u8* flags,
                             void* thisx, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    DemoMoonend* this = (DemoMoonend*)thisx;

    if (limbIndex == 8) {
        Matrix_Pop();
    }

    return true;
}

void DemoMoonend_Draw(Actor* thisx, PlayState* play) {
    DemoMoonend* this = (DemoMoonend*)thisx;
    Mtx* mtxStack;

    AnimatedMat_Draw(play, (AnimatedMaterial*)Lib_SegmentedToVirtual(object_moonend_Matanimheader_00B540));

    mtxStack = GRAPH_ALLOC(play->state.gfxCtx, this->kfSkelAnime.skeleton->dListCount * sizeof(Mtx));

    if (mtxStack != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Keyframe_DrawFlex(play, &this->kfSkelAnime, mtxStack, DemoMoonend_OverrideLimbDraw, DemoMoonend_PostLimbDraw,
                          &this->actor);
    }
}

void func_80C17FCC(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual((void*)object_moonend_Matanimheader_0129F0));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_moonend_DL_010C40);

    CLOSE_DISPS(play->state.gfxCtx);
}
