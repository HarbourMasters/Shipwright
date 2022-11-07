/*
 * File: z_eff_ss_d_fire.c
 * Overlay: ovl_Effect_Ss_D_Fire
 * Description: Dodongo Fire
 */

#include "z_eff_ss_d_fire.h"
#include "objects/object_dodongo/object_dodongo.h"

#define rScale regs[0]
#define rTexIdx regs[1]
#define rPrimColorR regs[2]
#define rPrimColorG regs[3]
#define rPrimColorB regs[4]
#define rPrimColorA regs[5]
#define rFadeDelay regs[6]
#define rScaleStep regs[9]
#define rObjBankIdx regs[10]
#define rYAccelStep regs[11] // has no effect due to how it's implemented

u32 EffectSsDFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsDFire_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsDFire_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_D_Fire_InitVars = {
    EFFECT_SS_D_FIRE,
    EffectSsDFire_Init,
};

u32 EffectSsDFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDFireInitParams* initParams = (EffectSsDFireInitParams*)initParamsx;
    s32 objBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_DODONGO);

    if (objBankIndex >= 0) {
        this->pos = initParams->pos;
        this->velocity = initParams->velocity;
        this->accel = initParams->accel;
        this->gfx = SEGMENTED_TO_VIRTUAL(gDodongoFireDL);
        this->life = initParams->life;
        this->rScale = initParams->scale;
        this->rScaleStep = initParams->scaleStep;
        this->rYAccelStep = 0;
        this->rObjBankIdx = objBankIndex;
        this->draw = EffectSsDFire_Draw;
        this->update = EffectSsDFire_Update;
        this->rTexIdx = ((s16)(play->state.frames % 4) ^ 3);
        this->rPrimColorR = 255;
        this->rPrimColorG = 255;
        this->rPrimColorB = 50;
        this->rPrimColorA = initParams->alpha;
        this->rFadeDelay = initParams->fadeDelay;

        return 1;
    }

    return 0;
}

static void* sTextures[] = { gDodongoFire0Tex, gDodongoFire1Tex, gDodongoFire2Tex, gDodongoFire3Tex };

void EffectSsDFire_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    s32 pad;
    void* object;
    Mtx* mtx;
    f32 scale;

    object = play->objectCtx.status[this->rObjBankIdx].segment;

    OPEN_DISPS(gfxCtx);

    if (Object_GetIndex(&play->objectCtx, OBJECT_DODONGO) > -1) {
        gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
        gSPSegment(POLY_XLU_DISP++, 0x06, object);
        scale = this->rScale / 100.0f;
        SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
        SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
        SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
        SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

        mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

        if (mtx != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            func_80094BC4(gfxCtx);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                            this->rPrimColorA);
            gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[this->rTexIdx]));
            gSPDisplayList(POLY_XLU_DISP++, this->gfx);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsDFire_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rTexIdx++;
    this->rTexIdx &= 3;
    this->rScale += this->rScaleStep;

    if (this->rFadeDelay >= this->life) {
        this->rPrimColorA -= 5;
        if (this->rPrimColorA < 0) {
            this->rPrimColorA = 0;
        }
    } else {
        this->rPrimColorA += 15;
        if (this->rPrimColorA > 255) {
            this->rPrimColorA = 255;
        }
    }

    if (this->accel.y < 0.0f) {
        this->accel.y += this->rYAccelStep * 0.01f;
    }

    if (this->life <= 0) {
        this->rYAccelStep += 0;
    }
}
