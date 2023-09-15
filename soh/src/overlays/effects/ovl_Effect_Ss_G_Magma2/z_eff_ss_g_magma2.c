/*
 * File: z_eff_ss_g_magma2.c
 * Overlay: ovl_Effect_Ss_G_Magma2
 * Description:
 */

#include "z_eff_ss_g_magma2.h"
#include "objects/object_kingdodongo/object_kingdodongo.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorA regs[2]
#define rEnvColorR regs[3]
#define rEnvColorG regs[4]
#define rEnvColorA regs[5]
#define rTexIdx regs[6]
#define rTimer regs[7]
#define rUpdateRate regs[8]
#define rDrawMode regs[9]
#define rObjBankIdx regs[10]
#define rScale regs[11]

u32 EffectSsGMagma2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsGMagma2_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsGMagma2_Update(PlayState* play, u32 index, EffectSs* this);

static void* sTextures[] = {
    object_kingdodongo_Tex_02E4E0, object_kingdodongo_Tex_02E8E0, object_kingdodongo_Tex_02ECE0,
    object_kingdodongo_Tex_02F0E0, object_kingdodongo_Tex_02F4E0, object_kingdodongo_Tex_02F8E0,
    object_kingdodongo_Tex_02FCE0, object_kingdodongo_Tex_0300E0, object_kingdodongo_Tex_0304E0,
    object_kingdodongo_Tex_0308E0, object_kingdodongo_Tex_0308E0, object_kingdodongo_Tex_0308E0,
    object_kingdodongo_Tex_0308E0,
};

EffectSsInit Effect_Ss_G_Magma2_InitVars = {
    EFFECT_SS_G_MAGMA2,
    EffectSsGMagma2_Init,
};

u32 EffectSsGMagma2_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    s32 objBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_KINGDODONGO);
    s32 pad;

    if ((objBankIndex >= 0) && Object_IsLoaded(&play->objectCtx, objBankIndex)) {
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
        EffectSsGMagma2InitParams* initParams = (EffectSsGMagma2InitParams*)initParamsx;

        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[objBankIndex].segment);
        this->rObjBankIdx = objBankIndex;
        this->pos = initParams->pos;
        this->velocity = zeroVec;
        this->accel = zeroVec;
        this->life = 100;
        this->draw = EffectSsGMagma2_Draw;
        this->update = EffectSsGMagma2_Update;
        this->gfx = SEGMENTED_TO_VIRTUAL(object_kingdodongo_DL_025A90);
        this->rTexIdx = 0;
        this->rDrawMode = initParams->drawMode;
        this->rUpdateRate = initParams->updateRate;
        this->rScale = initParams->scale;
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorA = initParams->primColor.a;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorA = initParams->envColor.a;

        return 1;
    }

    return 0;
}

void EffectSsGMagma2_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;
    void* object;

    scale = this->rScale / 100.0f;
    object = play->objectCtx.status[this->rObjBankIdx].segment;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(object);
    gSPSegment(POLY_XLU_DISP++, 0x06, object);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->rDrawMode == 0) {
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0x3D);
    } else {
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0);
    }

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, 0, this->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, 0, this->rEnvColorA);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sTextures[this->rTexIdx]));
    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsGMagma2_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rTimer += this->rUpdateRate;

    if (this->rTimer >= 10) {
        this->rTimer -= 10;
        this->rTexIdx++;

        if (this->rTexIdx >= 10) {
            this->life = 0;
        }

        if (this->rDrawMode == 0) {
            this->rPrimColorG -= 26;

            if (this->rPrimColorG <= 0) {
                this->rPrimColorG = 0;
            }

            this->rEnvColorR -= 26;

            if (this->rEnvColorR <= 0) {
                this->rEnvColorR = 0;
            }
        }
    }
}
