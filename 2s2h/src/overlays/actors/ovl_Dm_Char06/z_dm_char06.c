/*
 * File: z_dm_char06.c
 * Overlay: ovl_Dm_Char06
 * Description: Mountain Village Snowy landscape fadeout in post-Snowhead thawing cutscene
 */

#include "z_dm_char06.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar06_Init(Actor* thisx, PlayState* play);
void DmChar06_Destroy(Actor* thisx, PlayState* play);
void DmChar06_Update(Actor* thisx, PlayState* play);
void DmChar06_Draw(Actor* thisx, PlayState* play);

void DmChar06_SetupAction(DmChar06* this, DmChar06ActionFunc actionFunc);
void DmChar06_HandleCutscene(DmChar06* this, PlayState* play);

ActorProfile Dm_Char06_Profile = {
    /**/ ACTOR_DM_CHAR06,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_YUKIYAMA,
    /**/ sizeof(DmChar06),
    /**/ DmChar06_Init,
    /**/ DmChar06_Destroy,
    /**/ DmChar06_Update,
    /**/ DmChar06_Draw,
};

void DmChar06_SetupAction(DmChar06* this, DmChar06ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DmChar06_Init(Actor* thisx, PlayState* play) {
    DmChar06* this = (DmChar06*)thisx;

    SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
    Actor_SetScale(&this->actor, 1.0f);
    this->alpha = 255;
    DmChar06_SetupAction(this, DmChar06_HandleCutscene);
}

void DmChar06_Destroy(Actor* thisx, PlayState* play) {
}

void DmChar06_HandleCutscene(DmChar06* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_463)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_463);

        if ((play->csCtx.curFrame >= play->csCtx.actorCues[cueChannel]->startFrame) &&
            (play->csCtx.actorCues[cueChannel]->endFrame >= play->csCtx.curFrame)) {
            if (play->csCtx.actorCues[cueChannel]->id == 1) {
                this->alpha = 255;
            } else if (play->csCtx.actorCues[cueChannel]->id == 2) {
                f32 lerp =
                    1.0f - Environment_LerpWeight(play->csCtx.actorCues[cueChannel]->endFrame,
                                                  play->csCtx.actorCues[cueChannel]->startFrame, play->csCtx.curFrame);
                this->alpha = 255 * lerp;
            }
        }
    }
}

void DmChar06_Update(Actor* thisx, PlayState* play) {
    DmChar06* this = (DmChar06*)thisx;

    this->actionFunc(this, play);
}

void DmChar06_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DmChar06* this = (DmChar06*)thisx;

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_yukiyama_Matanimheader_006868));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Scene_SetRenderModeXlu(play, 1, 2);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_yukiyama_DL_0013A8);

    CLOSE_DISPS(play->state.gfxCtx);
}
