/*
 * File: z_eff_stk.c
 * Overlay: ovl_Eff_Stk
 * Description: Skullkid Effects (calling down moon / cursing Link)
 */

#include "z_eff_stk.h"
#include "objects/object_stk2/object_stk2.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EffStk_Init(Actor* thisx, PlayState* play);
void EffStk_Destroy(Actor* thisx, PlayState* play);
void EffStk_Update(Actor* thisx, PlayState* play);
void EffStk_Draw(Actor* thisx, PlayState* play);

void func_80BF0DE0(EffStk* this, PlayState* play);

ActorProfile Eff_Stk_Profile = {
    /**/ ACTOR_EFF_STK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_STK2,
    /**/ sizeof(EffStk),
    /**/ EffStk_Init,
    /**/ EffStk_Destroy,
    /**/ EffStk_Update,
    /**/ EffStk_Draw,
};

void EffStk_Init(Actor* thisx, PlayState* play) {
    EffStk* this = (EffStk*)thisx;

    Actor_SetScale(&this->actor, 0.2f);
    this->actionFunc = func_80BF0DE0;
}

void EffStk_Destroy(Actor* thisx, PlayState* play) {
}

void func_80BF0DE0(EffStk* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_512)) {
        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_512)]->id) {
            case 2:
                this->actor.draw = EffStk_Draw;
                if (this->unk146 < 0x3C00) {
                    this->unk146 += 0x400;
                    this->unk148 = Math_SinS(this->unk146) * -630.0f;
                }
                this->unk144++;
                break;

            case 3:
                this->actor.draw = EffStk_Draw;
                if (this->unk146 < 0x3C00) {
                    this->unk146 += 0x400;
                    this->unk148 = Math_SinS(this->unk146) * -630.0f;
                }
                this->unk144--;
                break;

            default:
                this->actor.draw = NULL;
                this->unk146 = 0;
                this->unk148 = 0.0f;
                break;
        }
    }
}

void EffStk_Update(Actor* thisx, PlayState* play) {
    EffStk* this = (EffStk*)thisx;

    this->actionFunc(this, play);
}

void EffStk_Draw(Actor* thisx, PlayState* play) {
    EffStk* this = (EffStk*)thisx;
    s32 pad;
    Camera* activeCam = GET_ACTIVE_CAM(play);
    Vec3f eye = activeCam->eye;
    Vec3f quakeOffset;

    quakeOffset = Camera_GetQuakeOffset(activeCam);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Translate(0.0f, 0.0f, this->unk148, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    AnimatedMat_DrawAlphaStep(play, Lib_SegmentedToVirtual(object_stk2_Matanimheader_009F60), 1.0f, this->unk144);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gSPDisplayList(POLY_XLU_DISP++, object_stk2_DL_008920);
    gSPDisplayList(POLY_XLU_DISP++, object_stk2_DL_008A38);

    CLOSE_DISPS(play->state.gfxCtx);
}
