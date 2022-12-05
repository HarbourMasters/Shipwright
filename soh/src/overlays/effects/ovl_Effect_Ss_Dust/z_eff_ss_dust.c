/*
 * File: z_eff_ss_dust.c
 * Overlay: ovl_Effect_Ss_Dust
 * Description: Dust Particle Effect
 */

#include "z_eff_ss_dust.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rTexIdx regs[8] // this reg is also used to set specific colors in the fire update function
#define rScale regs[9]
#define rScaleStep regs[10]
#define rDrawFlags regs[11]
#define rLifespan regs[12]

u32 EffectSsDust_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsDust_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsBlast_UpdateFire(PlayState* play, u32 index, EffectSs* this);
void EffectSsDust_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Dust_InitVars = {
    EFFECT_SS_DUST,
    EffectSsDust_Init,
};

static EffectSsUpdateFunc sUpdateFuncs[] = {
    EffectSsDust_Update,
    EffectSsBlast_UpdateFire,
};

u32 EffectSsDust_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    s32 randColorOffset;
    EffectSsDustInitParams* initParams = (EffectSsDustInitParams*)initParamsx;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffDustDL);
    this->life = initParams->life;
    this->update = sUpdateFuncs[initParams->updateMode];
    this->draw = EffectSsDust_Draw;

    if (initParams->drawFlags & 4) {
        randColorOffset = Rand_ZeroOne() * 20.0f - 10.0f;
        this->rPrimColorR = initParams->primColor.r + randColorOffset;
        this->rPrimColorG = initParams->primColor.g + randColorOffset;
        this->rPrimColorB = initParams->primColor.b + randColorOffset;
        this->rEnvColorR = initParams->envColor.r + randColorOffset;
        this->rEnvColorG = initParams->envColor.g + randColorOffset;
        this->rEnvColorB = initParams->envColor.b + randColorOffset;
    } else {
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorB = initParams->primColor.b;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorB = initParams->envColor.b;
    }

    this->rPrimColorA = initParams->primColor.a;
    this->rEnvColorA = initParams->envColor.a;
    this->rTexIdx = 0;
    this->rScale = initParams->scale;
    this->rScaleStep = initParams->scaleStep;
    this->rLifespan = initParams->life;
    this->rDrawFlags = initParams->drawFlags;

    return 1;
}

void EffectSsDust_Draw(PlayState* play, u32 index, EffectSs* this) {
    static void* dustTextures[] = {
        gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    s32 pad;
    Mtx* mtx;
    f32 scale;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale * 0.0025f;
    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);
    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTextures[this->rTexIdx]));
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0);
        gDPPipeSync(POLY_XLU_DISP++);

        if (this->rDrawFlags & 1) {
            gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0,
                              COMBINED, 0, SHADE, 0, 0, 0, 0, COMBINED);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_CLD_SURF2);
            gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
        } else if (this->rDrawFlags & 2) {
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
            gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);
        } else {
            gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING);
        }

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsDust_Update(PlayState* play, u32 index, EffectSs* this) {
    this->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    this->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;

    if ((this->life <= this->rLifespan) && (this->life >= (this->rLifespan - 7))) {
        if (this->rLifespan >= 5) {
            this->rTexIdx = this->rLifespan - this->life;
        } else {
            this->rTexIdx = ((this->rLifespan - this->life) * (8 / this->rLifespan));
        }
    } else {
        this->rTexIdx = 7;
    }

    this->rScale += this->rScaleStep;
}

// this update mode is unused in the original game
void EffectSsBlast_UpdateFire(PlayState* play, u32 index, EffectSs* this) {
    this->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
    this->accel.z = (Rand_ZeroOne() * 0.4f) - 0.2f;

    switch (this->rTexIdx) {
        case 0:
            this->rPrimColorR = 255;
            this->rPrimColorG = 150;
            this->rPrimColorB = 0;
            this->rEnvColorR = 150;
            this->rEnvColorG = 50;
            this->rEnvColorB = 0;
            break;
        case 1:
            this->rPrimColorR = 200;
            this->rPrimColorG = 50;
            this->rPrimColorB = 0;
            this->rEnvColorR = 100;
            this->rEnvColorG = 0;
            this->rEnvColorB = 0;
            break;
        case 2:
            this->rPrimColorR = 50;
            this->rPrimColorG = 0;
            this->rPrimColorB = 0;
            this->rEnvColorR = 0;
            this->rEnvColorG = 0;
            this->rEnvColorB = 0;
            break;
        case 3:
            this->rPrimColorR = 50;
            this->rEnvColorR = this->rPrimColorG = this->rEnvColorG = this->rPrimColorB = this->rEnvColorB = 0;
            break;
    }

    if (this->rTexIdx < 7) {
        this->rTexIdx++;
    }

    this->rScale += this->rScaleStep;
}
