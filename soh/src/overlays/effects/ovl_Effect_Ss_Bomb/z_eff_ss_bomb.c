/*
 * File: z_eff_ss_bomb.c
 * Overlay: ovl_Effect_Ss_Bomb
 * Description: Bomb Blast. Unused in the orignal game.
 */

#include "z_eff_ss_bomb.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rTexIdx regs[1]

u32 EffectSsBomb_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBomb_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsBomb_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Bomb_InitVars = {
    EFFECT_SS_BOMB,
    EffectSsBomb_Init,
};

u32 EffectSsBomb_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBombInitParams* initParams = (EffectSsBombInitParams*)initParamsx;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &initParams->velocity);
    Math_Vec3f_Copy(&this->accel, &initParams->accel);
    this->gfx = SEGMENTED_TO_VIRTUAL(gEffBombExplosion1DL);
    this->life = 20;
    this->draw = EffectSsBomb_Draw;
    this->update = EffectSsBomb_Update;
    this->rScale = 100;
    this->rTexIdx = 0;

    return 1;
}

void EffectSsBomb_Draw(PlayState* play, u32 index, EffectSs* this) {
    static void* explosionTextures[] = {
        gEffBombExplosion1Tex,
        gEffBombExplosion2Tex,
        gEffBombExplosion3Tex,
        gEffBombExplosion4Tex,
    };
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MtxF mfTrans;
    MtxF mfScale;
    MtxF mfResult;
    MtxF mfTrans11DA0;
    Mtx* mtx;
    s32 pad;
    f32 scale;
    s16 color;

    OPEN_DISPS(gfxCtx);

    scale = this->rScale / 100.0f;

    SkinMatrix_SetTranslate(&mfTrans, this->pos.x, this->pos.y, this->pos.z);
    SkinMatrix_SetScale(&mfScale, scale, scale, 1.0f);
    SkinMatrix_MtxFMtxFMult(&mfTrans, &play->billboardMtxF, &mfTrans11DA0);
    SkinMatrix_MtxFMtxFMult(&mfTrans11DA0, &mfScale, &mfResult);

    gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &mfResult);

    if (mtx != NULL) {
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(explosionTextures[this->rTexIdx]));
        gDPPipeSync(POLY_XLU_DISP++);
        func_80094C50(gfxCtx);
        color = this->life * 12.75f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, color, color, color, color);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
        gDPPipeSync(POLY_XLU_DISP++);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBomb_Update(PlayState* play, u32 index, EffectSs* this) {
    if ((this->life < 21) && (this->life > 16)) {
        this->rTexIdx = (20 - this->life);
    } else {
        this->rScale += 0;
        this->rTexIdx = 3;
    }

    this->accel.x = ((Rand_ZeroOne() * 0.4f) - 0.2f);
    this->accel.z = ((Rand_ZeroOne() * 0.4f) - 0.2f);
}
