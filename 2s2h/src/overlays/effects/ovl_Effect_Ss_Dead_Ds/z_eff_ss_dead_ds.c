/*
 * File: z_eff_ss_dead_ds.c
 * Overlay: ovl_Effect_Ss_Dead_Ds
 * Description: Burn mark on the floor
 */

#include "z_eff_ss_dead_ds.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTimer regs[1]
#define rRoll regs[2]
#define rPitch regs[3]
#define rYaw regs[4]
#define rAlpha regs[5]
#define rScaleStep regs[9]
#define rAlphaStep regs[10]
#define rHalfOfLife regs[11]

#define PARAMS ((EffectSsDeadDsInitParams*)initParamsx)

u32 EffectSsDeadDs_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsDeadDs_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsDeadDs_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Dead_Ds_Profile = {
    EFFECT_SS_DEAD_DS,
    EffectSsDeadDs_Init,
};

u32 EffectSsDeadDs_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDeadDsInitParams* initParams = PARAMS;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = initParams->life;
    this->rScaleStep = initParams->scaleStep;
    this->rHalfOfLife = initParams->life / 2;
    this->rAlphaStep = initParams->alpha / this->rHalfOfLife;
    this->draw = EffectSsDeadDs_Draw;
    this->update = EffectSsDeadDs_Update;
    this->rScale = initParams->scale;
    this->rAlpha = initParams->alpha;
    this->rTimer = 0;

    return 1;
}

void EffectSsDeadDs_Draw(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    f32 scale;
    s32 pad2[2];
    MtxF mf;
    f32 yIntersect;
    Vec3f pos;
    CollisionPoly* floorPoly;

    OPEN_DISPS(play->state.gfxCtx);

    scale = this->rScale * 0.01f;
    Gfx_SetupDL60_XluNoCD(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 0);
    pos = this->pos;

    if (this->rTimer == 0) {
        Vec3s rot;
        Vec3f prevPos;

        prevPos.x = pos.x - this->velocity.x;
        prevPos.y = pos.y - this->velocity.y;
        prevPos.z = pos.z - this->velocity.z;

        if (BgCheck_EntitySphVsWall1(&play->colCtx, &this->pos, &pos, &prevPos, 1.5f, &floorPoly, 1.0f)) {
            func_800C0094(floorPoly, this->pos.x, this->pos.y, this->pos.z, &mf);
            Matrix_Put(&mf);
        } else {
            pos.y++;
            yIntersect = BgCheck_EntityRaycastFloor1(&play->colCtx, &floorPoly, &pos);

            if (floorPoly != NULL) {
                func_800C0094(floorPoly, this->pos.x, yIntersect + 1.5f, this->pos.z, &mf);
                Matrix_Put(&mf);
            } else {
                Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
                Matrix_Get(&mf);
            }
        }

        Matrix_MtxFToZYXRot(&mf, &rot, false);
        this->rRoll = rot.x;
        this->rPitch = rot.y;
        this->rYaw = rot.z;
        this->pos.y = mf.yw;
        this->rTimer++;
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->rRoll, this->rPitch, this->rYaw, MTXMODE_APPLY);
    Matrix_RotateXFApply(1.57f); // (M_PI / 2)
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EffectSsDeadDs_Update(PlayState* play, u32 index, EffectSs* this) {
    if (this->life < this->rHalfOfLife) {

        this->rScale += this->rScaleStep;
        if (this->rScale < 0) {
            this->rScale = 0;
        }

        this->rAlpha -= this->rAlphaStep;
        if (this->rAlpha < 0) {
            this->rAlpha = 0;
        }
    }
}
