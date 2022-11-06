/*
 * File: z_eff_ss_en_ice.c
 * Overlay: ovl_Effect_Ss_En_Ice
 * Description: Ice clumps
 */

#include "z_eff_ss_en_ice.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rLifespan regs[0]
#define rYaw regs[1]
#define rPitch regs[2]
#define rRotSpeed regs[3]
#define rPrimColorR regs[4]
#define rPrimColorG regs[5]
#define rPrimColorB regs[6]
#define rPrimColorA regs[7]
#define rEnvColorR regs[8]
#define rEnvColorG regs[9]
#define rEnvColorB regs[10]
#define rAlphaMode regs[11]
#define rScale regs[12]

u32 EffectSsEnIce_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsEnIce_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsEnIce_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsEnIce_UpdateFlying(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_En_Ice_InitVars = {
    EFFECT_SS_EN_ICE,
    EffectSsEnIce_Init,
};

u32 EffectSsEnIce_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsEnIceInitParams* initParams = (EffectSsEnIceInitParams*)initParamsx;

    if (initParams->type == 0) {
        Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

        this->pos = initParams->pos;
        this->vec.x = this->pos.x - initParams->actor->world.pos.x;
        this->vec.y = this->pos.y - initParams->actor->world.pos.y;
        this->vec.z = this->pos.z - initParams->actor->world.pos.z;
        this->velocity = zeroVec;
        this->accel = zeroVec;
        this->life = 10;
        this->actor = initParams->actor;
        this->draw = EffectSsEnIce_Draw;
        this->update = EffectSsEnIce_UpdateFlying;
        this->rScale = initParams->scale * 100.0f;
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorB = initParams->primColor.b;
        this->rPrimColorA = initParams->primColor.a;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorB = initParams->envColor.b;
        this->rAlphaMode = 1;
        this->rPitch = Rand_CenteredFloat(65536.0f);
    } else if (initParams->type == 1) {
        this->pos = initParams->pos;
        this->vec = initParams->pos;
        this->velocity = initParams->velocity;
        this->accel = initParams->accel;
        this->life = initParams->life;
        this->draw = EffectSsEnIce_Draw;
        this->update = EffectSsEnIce_Update;
        this->rLifespan = initParams->life;
        this->rScale = initParams->scale * 100.0f;
        this->rYaw = Math_Atan2S(initParams->velocity.z, initParams->velocity.x);
        this->rPitch = 0;
        this->rPrimColorR = initParams->primColor.r;
        this->rPrimColorG = initParams->primColor.g;
        this->rPrimColorB = initParams->primColor.b;
        this->rPrimColorA = initParams->primColor.a;
        this->rEnvColorR = initParams->envColor.r;
        this->rEnvColorG = initParams->envColor.g;
        this->rEnvColorB = initParams->envColor.b;
        this->rAlphaMode = 0;
    } else {
        osSyncPrintf("Effect_Ss_En_Ice_ct():pid->mode_swがエラーです。\n");
        return 0;
    }

    return 1;
}

void EffectSsEnIce_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale;
    Vec3f hiliteLightDir;
    u32 gameplayFrames;
    f32 alpha;

    scale = this->rScale * 0.01f;
    gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(gfxCtx);

    if (this->rAlphaMode != 0) {
        alpha = this->life * 12;
    } else {
        if ((this->rLifespan > 0) && (this->life < (this->rLifespan >> 1))) {
            alpha = ((this->life * 2.0f) / this->rLifespan);
            alpha *= 255.0f;
        } else {
            alpha = 255.0f;
        }
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateY(this->rYaw * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_RotateX(this->rPitch * (M_PI / 0x8000), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    hiliteLightDir.x = 89.8f;
    hiliteLightDir.y = 0.0f;
    hiliteLightDir.z = 89.8f;

    func_80093D84(play->state.gfxCtx);
    func_8002EB44(&this->pos, &play->view.eye, &hiliteLightDir, play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, gameplayFrames & 0xFF, 0x20, 0x10, 1, 0,
                                (gameplayFrames * 2) & 0xFF, 0x40, 0x20));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB,
                    this->rPrimColorA);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, (u32)alpha);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment2DL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsEnIce_UpdateFlying(PlayState* play, u32 index, EffectSs* this) {
    s16 rand;

    if ((this->actor != NULL) && (this->actor->update != NULL)) {
        if ((this->life >= 9) && (this->actor->colorFilterTimer != 0) && (!(this->actor->colorFilterParams & 0xC000))) {
            this->pos.x = this->actor->world.pos.x + this->vec.x;
            this->pos.y = this->actor->world.pos.y + this->vec.y;
            this->pos.z = this->actor->world.pos.z + this->vec.z;
            this->life++;
        } else if (this->life == 9) {
            this->accel.x = Math_SinS(Math_Vec3f_Yaw(&this->actor->world.pos, &this->pos)) * (Rand_ZeroOne() + 1.0f);
            this->accel.z = Math_CosS(Math_Vec3f_Yaw(&this->actor->world.pos, &this->pos)) * (Rand_ZeroOne() + 1.0f);
            this->accel.y = -1.5f;
            this->velocity.y = 5.0f;
        }
    } else {
        if (this->life >= 9) {
            rand = Rand_CenteredFloat(65535.0f);
            this->accel.x = Math_SinS(rand) * (Rand_ZeroOne() + 1.0f);
            this->accel.z = Math_CosS(rand) * (Rand_ZeroOne() + 1.0f);
            this->life = 8;
            this->accel.y = -1.5f;
            this->velocity.y = 5.0f;
        }
    }
}

void EffectSsEnIce_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rPitch += this->rRotSpeed; // rRotSpeed is not initialized so this does nothing
}
