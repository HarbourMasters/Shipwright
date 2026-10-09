/*
 * File: z_eff_ss_bomb2.c
 * Overlay: ovl_Effect_Ss_Bomb2
 * Description: Bomb Blast
 */

#include "z_eff_ss_bomb2.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIndex regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rEnvColorR regs[6]
#define rEnvColorG regs[7]
#define rEnvColorB regs[8]
#define rScaleStep regs[9]
#define rDepth regs[10]

#define PARAMS ((EffectSsBomb2InitParams*)initParamsx)

u32 EffectSsBomb2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBomb2_DrawFade(PlayState* play, u32 index, EffectSs* this);
void EffectSsBomb2_DrawLayered(PlayState* play, u32 index, EffectSs* this);
void EffectSsBomb2_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Bomb2_Profile = {
    EFFECT_SS_BOMB2,
    EffectSsBomb2_Init,
};

static EffectSsDrawFunc sDrawFuncs[] = {
    EffectSsBomb2_DrawFade,
    EffectSsBomb2_DrawLayered,
};

static TexturePtr sTextures[] = {
    gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
    gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
};

static TexturePtr sLayeredTextures[] = {
    gEffBombExplosion1Tex, gEffBombExplosion2Tex, gEffBombExplosion3Tex, gEffBombExplosion4Tex,
    gEffBombExplosion5Tex, gEffBombExplosion6Tex, gEffBombExplosion7Tex, gEffBombExplosion8Tex,
};

u32 EffectSsBomb2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBomb2InitParams* initParams = PARAMS;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->gfx = gEffBombExplosion1DL;
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

    return 1;
}

// unused in the original game. looks like EffectSsBomb but with color
void EffectSsBomb2_DrawFade(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    s32 pad;
    f32 scale;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale * 0.01f;
    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    SkinMatrix_MtxFMtxFMult(&mfTransBillboard, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Gfx_SetupDL60_XluNoCD(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                        this->rPrimColorA);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sTextures[this->rTexIndex]));
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBomb2_DrawLayered(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
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
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    SkinMatrix_MtxFMtxFMult(&mfTransBillboard, &mfScale, &mfResult);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        mtx2 = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

        if (mtx2 != NULL) {
            Gfx_SetupDL60_XluNoCD(gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                            this->rPrimColorA);
            gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sLayeredTextures[this->rTexIndex]));
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion2DL);
            gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);

            Matrix_MtxToMtxF(mtx2, &mtx2F);
            Matrix_Put(&mtx2F);

            for (i = 1; i >= 0; i--) {
                Matrix_Translate(0.0f, 0.0f, depth, MTXMODE_APPLY);
                Matrix_RotateZF((this->life * 0.02f) + 180.0f, MTXMODE_APPLY);
                Matrix_Scale(layer2Scale, layer2Scale, layer2Scale, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gEffBombExplosion3DL);
                layer2Scale -= 0.15f;
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBomb2_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 divisor;

    this->rTexIndex = (23 - this->life) / 3;
    this->rScale += this->rScaleStep;

    if (this->rScaleStep == 30) {
        this->rDepth += 4;
    } else {
        this->rDepth += 2;
    }

    if ((this->life < 23) && (this->life > 13)) {
        divisor = this->life - 13;
        this->rPrimColorR = EffectSs_LerpInv(this->rPrimColorR, 255, divisor);
        this->rPrimColorG = EffectSs_LerpInv(this->rPrimColorG, 255, divisor);
        this->rPrimColorB = EffectSs_LerpInv(this->rPrimColorB, 150, divisor);
        this->rPrimColorA = EffectSs_LerpInv(this->rPrimColorA, 255, divisor);
        this->rEnvColorR = EffectSs_LerpInv(this->rEnvColorR, 150, divisor);
        this->rEnvColorG = EffectSs_LerpInv(this->rEnvColorG, 0, divisor);
        this->rEnvColorB = EffectSs_LerpInv(this->rEnvColorB, 0, divisor);
    } else if ((this->life < 14) && (this->life > -1)) {
        divisor = this->life + 1;
        this->rPrimColorR = EffectSs_LerpInv(this->rPrimColorR, 50, divisor);
        this->rPrimColorG = EffectSs_LerpInv(this->rPrimColorG, 50, divisor);
        this->rPrimColorB = EffectSs_LerpInv(this->rPrimColorB, 50, divisor);
        this->rPrimColorA = EffectSs_LerpInv(this->rPrimColorA, 150, divisor);
        this->rEnvColorR = EffectSs_LerpInv(this->rEnvColorR, 10, divisor);
        this->rEnvColorG = EffectSs_LerpInv(this->rEnvColorG, 10, divisor);
        this->rEnvColorB = EffectSs_LerpInv(this->rEnvColorB, 10, divisor);
    }
}
