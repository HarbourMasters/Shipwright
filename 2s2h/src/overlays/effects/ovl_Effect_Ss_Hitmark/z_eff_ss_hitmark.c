/*
 * File: z_eff_ss_hitmark.c
 * Overlay: ovl_Effect_Ss_Hitmark
 * Description: Hit Marks
 */

#include "z_eff_ss_hitmark.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rTexIndex regs[0]
#define rType regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rEnvColorR regs[5]
#define rEnvColorG regs[6]
#define rEnvColorB regs[7]
#define rScale regs[8]

#define PARAMS ((EffectSsHitmarkInitParams*)initParamsx)

u32 EffectSsHitmark_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsHitmark_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsHitmark_Draw(PlayState* play, u32 index, EffectSs* this);

static Color_RGB8 sColors[] = {
    { 255, 255, 255 }, { 255, 255, 0 }, // EFFECT_HITMARK_WHITE Init
    { 255, 255, 255 }, { 255, 0, 0 },   // EFFECT_HITMARK_WHITE Update
    { 255, 200, 100 }, { 200, 150, 0 }, // EFFECT_HITMARK_DUST Init
    { 150, 100, 0 },   { 100, 50, 0 },  // EFFECT_HITMARK_DUST Update
    { 255, 255, 255 }, { 255, 0, 0 },   // EFFECT_HITMARK_RED Init
    { 255, 255, 0 },   { 255, 0, 0 },   // EFFECT_HITMARK_RED Update
    { 255, 255, 255 }, { 0, 255, 200 }, // EFFECT_HITMARK_METAL Init
    { 255, 255, 255 }, { 150, 0, 255 }, // EFFECT_HITMARK_METAL Update
};

static TexturePtr sTextures[] = {
    // EFFECT_HITMARK_WHITE
    gEffHitmarkWhiteMetal1Tex,
    gEffHitmarkWhiteMetal2Tex,
    gEffHitmarkWhiteMetal3Tex,
    gEffHitmarkWhiteMetal4Tex,
    gEffHitmarkWhiteMetal5Tex,
    gEffHitmarkWhiteMetal6Tex,
    gEffHitmarkWhiteMetal7Tex,
    gEffHitmarkWhiteMetal8Tex,
    // EFFECT_HITMARK_DUST
    gEffHitmarkDust1Tex,
    gEffHitmarkDust2Tex,
    gEffHitmarkDust3Tex,
    gEffHitmarkDust4Tex,
    gEffHitmarkDust5Tex,
    gEffHitmarkDust6Tex,
    gEffHitmarkDust7Tex,
    gEffHitmarkDust8Tex,
    // EFFECT_HITMARK_RED
    gEffHitmarkRed1Tex,
    gEffHitmarkRed2Tex,
    gEffHitmarkRed3Tex,
    gEffHitmarkRed4Tex,
    gEffHitmarkRed5Tex,
    gEffHitmarkRed6Tex,
    gEffHitmarkRed7Tex,
    gEffHitmarkRed8Tex,
    // EFFECT_HITMARK_METAL
    gEffHitmarkWhiteMetal1Tex,
    gEffHitmarkWhiteMetal2Tex,
    gEffHitmarkWhiteMetal3Tex,
    gEffHitmarkWhiteMetal4Tex,
    gEffHitmarkWhiteMetal5Tex,
    gEffHitmarkWhiteMetal6Tex,
    gEffHitmarkWhiteMetal7Tex,
    gEffHitmarkWhiteMetal8Tex,
};

EffectSsProfile Effect_Ss_Hitmark_Profile = {
    EFFECT_SS_HITMARK,
    EffectSsHitmark_Init,
};

u32 EffectSsHitmark_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsHitmarkInitParams* initParams = PARAMS;
    s32 colorIndex;

    this->pos = initParams->pos;
    this->gfx = gEffHitmarkDL;

    if (initParams->type == EFFECT_HITMARK_DUST) {
        this->life = 16;
    } else {
        this->life = 8;
    }

    this->draw = EffectSsHitmark_Draw;
    this->update = EffectSsHitmark_Update;

    colorIndex = initParams->type * 4;
    this->rTexIndex = 0;
    this->rType = initParams->type;
    this->rPrimColorR = sColors[colorIndex].r;
    this->rPrimColorG = sColors[colorIndex].g;
    this->rPrimColorB = sColors[colorIndex].b;
    this->rEnvColorR = sColors[colorIndex + 1].r;
    this->rEnvColorG = sColors[colorIndex + 1].g;
    this->rEnvColorB = sColors[colorIndex + 1].b;
    this->rScale = initParams->scale;

    return 1;
}

void EffectSsHitmark_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTransBillboard;
    Mtx* mtx;
    f32 scale;
    TexturePtr tex;
    s32 pad;

    OPEN_DISPS(gfxCtx);

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    scale = this->rScale / 100.0f;
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTransBillboard);
    SkinMatrix_MtxFMtxFMult(&mfTransBillboard, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        tex = sTextures[(this->rType * 8) + (this->rTexIndex)];
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(tex));
        Gfx_SetupDL61_Xlu(gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }
    CLOSE_DISPS(gfxCtx);
}

void EffectSsHitmark_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 colorIndex;

    if (this->rType == EFFECT_HITMARK_DUST) {
        this->rTexIndex = (15 - this->life) / 2;
    } else {
        this->rTexIndex = 7 - this->life;
    }

    if (this->rTexIndex != 0) {
        colorIndex = this->rType * 4 + 2;
        this->rPrimColorR = EffectSs_LerpInv(this->rPrimColorR, sColors[colorIndex].r, this->life + 1);
        this->rPrimColorG = EffectSs_LerpInv(this->rPrimColorG, sColors[colorIndex].g, this->life + 1);
        this->rPrimColorB = EffectSs_LerpInv(this->rPrimColorB, sColors[colorIndex].b, this->life + 1);
        this->rEnvColorR = EffectSs_LerpInv(this->rEnvColorR, sColors[colorIndex + 1].r, this->life + 1);
        this->rEnvColorG = EffectSs_LerpInv(this->rEnvColorG, sColors[colorIndex + 1].g, this->life + 1);
        this->rEnvColorB = EffectSs_LerpInv(this->rEnvColorB, sColors[colorIndex + 1].b, this->life + 1);
    }
}
