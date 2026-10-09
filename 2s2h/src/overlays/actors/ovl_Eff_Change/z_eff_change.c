/*
 * File: z_eff_change.c
 * Overlay: ovl_Eff_Change
 * Description: Elegy of Emptiness - Beam of Light When Creating Statue
 */

#include "z_eff_change.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EffChange_Init(Actor* thisx, PlayState* play);
void EffChange_Destroy(Actor* thisx, PlayState* play);
void EffChange_Update(Actor* thisx, PlayState* play);
void EffChange_Draw(Actor* thisx, PlayState* play);

void EffChange_SetColors(EffChange* this, s32 arg1);
void func_80A4C5CC(EffChange* this, PlayState* play);

ActorProfile Eff_Change_Profile = {
    /**/ ACTOR_EFF_CHANGE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EffChange),
    /**/ EffChange_Init,
    /**/ EffChange_Destroy,
    /**/ EffChange_Update,
    /**/ NULL,
};

static u8 D_80A4C920[] = {
    // prim r g b   env r g b
    255, 255, 170, 0,   100, 0,   // COLOR_0
    255, 255, 170, 200, 0,   0,   // COLOR_1
    170, 255, 255, 0,   100, 255, // COLOR_2
    255, 255, 170, 200, 150, 0,   // COLOR_3
    255, 255, 170, 0,   100, 0,   // COLOR_4
    255, 255, 170, 0,   100, 0,   // COLOR_5
    255, 255, 170, 0,   100, 0,   // COLOR_6
    255, 255, 170, 0,   100, 0    // COLOR_7
};

void EffChange_Init(Actor* thisx, PlayState* play) {
    EffChange* this = (EffChange*)thisx;

    this->actionFunc = func_80A4C5CC;
    this->actor.draw = EffChange_Draw;
    EffChange_SetColors(this, EFFCHANGE_GET_COLORS(thisx));
    Actor_SetScale(&this->actor, 0.075f);
    this->primColors[3] = 0;
    Keyframe_InitFlex(&this->kfSkelAnime, gGameplayKeepKFSkel_2900C, gGameplayKeepKFAnim_281DC, this->jointTable,
                      this->morphTable, NULL);
    Keyframe_FlexPlayOnce(&this->kfSkelAnime, gGameplayKeepKFAnim_281DC);
    this->step = 0;
    this->actor.shape.rot.y = 0;
    this->kfSkelAnime.frameCtrl.speed = (2.0f / 3.0f);
    CutsceneManager_Queue(CS_ID_GLOBAL_ELEGY);
}

void EffChange_Destroy(Actor* thisx, PlayState* play) {
    EffChange* this = (EffChange*)thisx;

    Keyframe_DestroyFlex(&this->kfSkelAnime);
}

void EffChange_SetColors(EffChange* this, s32 arg1) {
    arg1 *= 6;
    this->primColors[0] = D_80A4C920[arg1];
    this->primColors[1] = D_80A4C920[arg1 + 1];
    this->primColors[2] = D_80A4C920[arg1 + 2];
    this->envColors[0] = D_80A4C920[arg1 + 3];
    this->envColors[1] = D_80A4C920[arg1 + 4];
    this->envColors[2] = D_80A4C920[arg1 + 5];
}

void func_80A4C5CC(EffChange* this, PlayState* play) {
    f32 phi_fv0;

    if (Keyframe_UpdateFlex(&this->kfSkelAnime)) {
        Actor_Kill(&this->actor);
        CutsceneManager_Stop(CS_ID_GLOBAL_ELEGY);
        Environment_AdjustLights(play, 0.0f, 850.0f, 0.2f, 0.0f);
        return;
    }

    this->step++;
    if (this->kfSkelAnime.frameCtrl.curTime < 20.0f) {
        if ((this->primColors[3]) < 242) {
            this->primColors[3] += 13;
        } else {
            this->primColors[3] = 255;
        }
    } else if (this->kfSkelAnime.frameCtrl.curTime > 70.0f) {
        if ((this->primColors[3]) >= 14) {
            this->primColors[3] -= 13;
        } else {
            this->primColors[3] = 0;
        }
    } else {
        this->primColors[3] = 255;
    }

    phi_fv0 = this->primColors[3] * (1.0f / 255.0f);
    if (phi_fv0 > 1.0f) {
        phi_fv0 = 1.0f;
    } else if (phi_fv0 < 0.0f) {
        phi_fv0 = 0.0f;
    }
    Environment_AdjustLights(play, phi_fv0, 850.0f, 0.2f, 0.0f);
    if (CutsceneManager_GetCurrentCsId() != CS_ID_GLOBAL_ELEGY) {
        if (CutsceneManager_IsNext(CS_ID_GLOBAL_ELEGY)) {
            CutsceneManager_Start(CS_ID_GLOBAL_ELEGY, &this->actor);
        } else {
            CutsceneManager_Queue(CS_ID_GLOBAL_ELEGY);
        }
    }
}

void EffChange_Update(Actor* thisx, PlayState* play) {
    EffChange* this = (EffChange*)thisx;

    this->actionFunc(this, play);
}

void EffChange_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Mtx* mtxStack;
    EffChange* this = (EffChange*)thisx;

    AnimatedMat_DrawStepXlu(play, Lib_SegmentedToVirtual(&gameplay_keep_Matanimheader_028FEC), this->step);
    mtxStack = GRAPH_ALLOC(play->state.gfxCtx, this->kfSkelAnime.skeleton->dListCount * sizeof(Mtx));

    if (mtxStack != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_RotateYS((Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000), MTXMODE_APPLY);

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->primColors[0], this->primColors[1], this->primColors[2],
                        this->primColors[3]);
        gDPSetEnvColor(POLY_XLU_DISP++, this->envColors[0], this->envColors[1], this->envColors[2], 255);
        Keyframe_DrawFlex(play, &this->kfSkelAnime, mtxStack, NULL, NULL, &this->actor);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
