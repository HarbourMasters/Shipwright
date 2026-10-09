/*
 * File: z_eff_ss_ice_piece.c
 * Overlay: ovl_Effect_Ss_Ice_Piece
 * Description: Ice Shards
 */

#include "z_eff_ss_ice_piece.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rLifespan regs[0]
#define rYaw regs[1]
#define rPitch regs[2]
#define rRotSpeed regs[3]
#define rScale regs[4]

#define PARAMS ((EffectSsIcePieceInitParams*)initParamsx)

u32 EffectSsIcePiece_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsIcePiece_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsIcePiece_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Ice_Piece_Profile = {
    EFFECT_SS_ICE_PIECE,
    EffectSsIcePiece_Init,
};

u32 EffectSsIcePiece_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsIcePieceInitParams* initParams = PARAMS;

    this->pos = initParams->pos;
    this->vec = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = initParams->life;
    this->draw = EffectSsIcePiece_Draw;
    this->update = EffectSsIcePiece_Update;
    this->rLifespan = initParams->life;
    this->rScale = initParams->scale * 100.0f;
    this->rYaw = Math_Atan2S_XY(initParams->velocity.z, initParams->velocity.x);
    this->rPitch = 0;
    this->rRotSpeed =
        ((fabsf(initParams->velocity.x) + fabsf(initParams->velocity.y)) * 100.0f) * (Rand_ZeroFloat(1.0f) + 0.5f);

    return 1;
}

void EffectSsIcePiece_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    f32 scale = this->rScale * 0.01f;
    u32 frames = play->state.frames;
    f32 alpha;

    OPEN_DISPS(gfxCtx);

    if ((this->rLifespan > 0) && (this->life < (this->rLifespan >> 1))) {
        alpha = (this->life * 2.0f) / this->rLifespan;
        alpha *= 255.0f;
    } else {
        alpha = 255.0f;
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateYS(this->rYaw, MTXMODE_APPLY);
    Matrix_RotateXS(this->rPitch, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, (u8)(s32)alpha);
    func_800BCC68(&this->pos, play);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, frames % 256, 0x20, 0x10, 1, 0, (2 * frames) % 256, 0x40,
                                  0x20, 0, 1, 0, 2));
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment1DL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsIcePiece_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rPitch += this->rRotSpeed;
    this->velocity.x *= 0.85f;
    this->velocity.y *= 0.85f;
    this->velocity.z *= 0.85f;
}
