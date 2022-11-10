/*
 * File: z_eff_ss_bomb2.c
 * Overlay: ovl_Effect_Ss_Bomb2
 * Description: Bomb Blast
 */

#include "z_eff_ss_bomb2.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "soh/frame_interpolation.h"

#define rScale regs[0]
#define rTexIdx regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rDepth regs[10]

u32 EffectSsBomb2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBomb2_DrawFade(PlayState* play, u32 index, EffectSs* this);
void EffectSsBomb2_DrawLayered(PlayState* play, u32 index, EffectSs* this);
void EffectSsBomb2_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Bomb2_InitVars = {
    EFFECT_SS_BOMB2,
    EffectSsBomb2_Init,
};

static EffectSsDrawFunc sDrawFuncs[] = {
    EffectSsBomb2_DrawFade,
    EffectSsBomb2_DrawLayered,
};

u32 EffectSsBomb2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {

    EffectSsBomb2InitParams* initParams = (EffectSsBomb2InitParams*)initParamsx;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    this->life = 24;
    this->update = EffectSsBomb2_Update;
    this->draw = sDrawFuncs[initParams->drawMode];
    this->rScale = initParams->scale;
    this->rScaleStep = initParams->scaleStep;
    this->rPrimColorR = 255;
    this->rPrimColorG = 255;
    this->rPrimColorB = 255;
    this->rPrimColorA = 255;
    this->rEnvColorR = 0;
    this->rEnvColorG = 0;
    this->rEnvColorB = 200;
    this->epoch++;

    return 1;
}

// unused in the original game. looks like EffectSsBomb but with color
void EffectSsBomb2_DrawFade(PlayState* play, u32 index, EffectSs* this) {
    static void* textures[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    s32 pad;
    f32 scale;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        func_80094BC4(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(textures[this->rTexIdx]));
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBomb2_DrawLayered(PlayState* play, u32 index, EffectSs* this) {
    static void* textures[] = {
        gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
        gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    MtxF mtx2F;
    Mtx* mtx2;
    Mtx* mtx;
    s32 pad[3];
    f32 scale;
    f32 depth;
    f32 layer2Scale = 0.925f;
    s32 i;

    OPEN_DISPS(gfxCtx);

    depth = this->rDepth;
    scale = this->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        mtx2 = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

        if (mtx2 != NULL) {
            func_80094BC4(gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                            this->rPrimColorA);
            gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(textures[this->rTexIdx]));
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion2DL);
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);

            Matrix_MtxToMtxF(mtx2, &mtx2F);
            Matrix_Put(&mtx2F);

            for (i = 1; i >= 0; i--) {
                FrameInterpolation_RecordOpenChild(this, this->epoch + i * 25);
                Matrix_Translate(0.0f, 0.0f, depth, MTXMODE_APPLY);
                Matrix_RotateZ((this->life * 0.02f) + 180.0f, MTXMODE_APPLY);
                Matrix_Scale(layer2Scale, layer2Scale, layer2Scale, MTXMODE_APPLY);
                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);
                layer2Scale -= 0.15f;
                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBomb2_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 divisor;

    this->rTexIdx = (23 - this->life) / 3;
    this->rScale += this->rScaleStep;

    if (this->rScaleStep == 30) {
        this->rDepth += 4.0f;
    } else {
        this->rDepth += 2.0f;
    }

    if ((this->life < 23) && (this->life > 13)) {
        divisor = this->life - 13;
        this->rPrimColorR = func_80027DD4(this->rPrimColorR, 255, divisor);
        this->rPrimColorG = func_80027DD4(this->rPrimColorG, 255, divisor);
        this->rPrimColorB = func_80027DD4(this->rPrimColorB, 150, divisor);
        this->rPrimColorA = func_80027DD4(this->rPrimColorA, 255, divisor);
        this->rEnvColorR = func_80027DD4(this->rEnvColorR, 150, divisor);
        this->rEnvColorG = func_80027DD4(this->rEnvColorG, 0, divisor);
        this->rEnvColorB = func_80027DD4(this->rEnvColorB, 0, divisor);
    } else if ((this->life < 14) && (this->life > -1)) {
        divisor = this->life + 1;
        this->rPrimColorR = func_80027DD4(this->rPrimColorR, 50, divisor);
        this->rPrimColorG = func_80027DD4(this->rPrimColorG, 50, divisor);
        this->rPrimColorB = func_80027DD4(this->rPrimColorB, 50, divisor);
        this->rPrimColorA = func_80027DD4(this->rPrimColorA, 150, divisor);
        this->rEnvColorR = func_80027DD4(this->rEnvColorR, 10, divisor);
        this->rEnvColorG = func_80027DD4(this->rEnvColorG, 10, divisor);
        this->rEnvColorB = func_80027DD4(this->rEnvColorB, 10, divisor);
    }
}
