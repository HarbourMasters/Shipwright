/*
 * File: z_eff_ss_hitmark.c
 * Overlay: ovl_Effect_Ss_HitMark
 * Description: Hit Marks
 */

#include "z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rTexIdx regs[0]
#define rType regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rEnvColorR regs[5]
#define rEnvColorG regs[6]
#define rEnvColorB regs[7]
#define rScale regs[8]

u32 EffectSsHitMark_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsHitMark_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsHitMark_Update(PlayState* play, u32 index, EffectSs* this);

static Color_RGB8 sColors[] = {
    { 255, 255, 255 }, { 255, 255, 0 }, { 255, 255, 255 }, { 255, 0, 0 },   { 255, 200, 100 }, { 200, 150, 0 },
    { 150, 100, 0 },   { 100, 50, 0 },  { 255, 255, 255 }, { 255, 0, 0 },   { 255, 255, 0 },   { 255, 0, 0 },
    { 255, 255, 255 }, { 0, 255, 200 }, { 255, 255, 255 }, { 150, 0, 255 },
};

static void* sTextures[] = {
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,  gEffHitMark9Tex,  gEffHitMark10Tex, gEffHitMark11Tex, gEffHitMark12Tex,
    gEffHitMark13Tex, gEffHitMark14Tex, gEffHitMark15Tex, gEffHitMark16Tex, gEffHitMark17Tex, gEffHitMark18Tex,
    gEffHitMark19Tex, gEffHitMark20Tex, gEffHitMark21Tex, gEffHitMark22Tex, gEffHitMark23Tex, gEffHitMark24Tex,
    gEffHitMark1Tex,  gEffHitMark2Tex,  gEffHitMark3Tex,  gEffHitMark4Tex,  gEffHitMark5Tex,  gEffHitMark6Tex,
    gEffHitMark7Tex,  gEffHitMark8Tex,
};

EffectSsInit Effect_Ss_HitMark_InitVars = {
    EFFECT_SS_HITMARK,
    EffectSsHitMark_Init,
};

u32 EffectSsHitMark_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    s32 colorIdx;
    EffectSsHitMarkInitParams* initParams = (EffectSsHitMarkInitParams*)initParamsx;
    this->pos = initParams->pos;
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffHitMarkDL);

    if (initParams->type == EFFECT_HITMARK_DUST) {
        this->life = 16;
    } else {
        this->life = 8;
    }

    this->draw = EffectSsHitMark_Draw;
    this->update = EffectSsHitMark_Update;
    colorIdx = initParams->type * 4;
    this->rTexIdx = 0;
    this->rType = initParams->type;
    this->rPrimColorR = sColors[colorIdx].r;
    this->rPrimColorG = sColors[colorIdx].g;
    this->rPrimColorB = sColors[colorIdx].b;
    this->rEnvColorR = sColors[colorIdx + 1].r;
    this->rEnvColorG = sColors[colorIdx + 1].g;
    this->rEnvColorB = sColors[colorIdx + 1].b;
    this->rScale = initParams->scale;

    return 1;
}

void EffectSsHitMark_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    f32 scale;
    s32 pad;

    OPEN_DISPS(gfxCtx);

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    scale = this->rScale / 100.0f;
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[(this->rType * 8) + (this->rTexIdx)]));
        func_80094C50(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }
    CLOSE_DISPS(gfxCtx);
}

void EffectSsHitMark_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 colorIdx;

    if (this->rType == EFFECT_HITMARK_DUST) {
        this->rTexIdx = (15 - this->life) / 2;
    } else {
        this->rTexIdx = 7 - this->life;
    }

    if (this->rTexIdx != 0) {
        colorIdx = this->rType * 4 + 2;
        this->rPrimColorR = func_80027DD4(this->rPrimColorR, sColors[colorIdx].r, this->life + 1);
        this->rPrimColorG = func_80027DD4(this->rPrimColorG, sColors[colorIdx].g, this->life + 1);
        this->rPrimColorB = func_80027DD4(this->rPrimColorB, sColors[colorIdx].b, this->life + 1);
        this->rEnvColorR = func_80027DD4(this->rEnvColorR, sColors[colorIdx + 1].r, this->life + 1);
        this->rEnvColorG = func_80027DD4(this->rEnvColorG, sColors[colorIdx + 1].g, this->life + 1);
        this->rEnvColorB = func_80027DD4(this->rEnvColorB, sColors[colorIdx + 1].b, this->life + 1);
    }
}
