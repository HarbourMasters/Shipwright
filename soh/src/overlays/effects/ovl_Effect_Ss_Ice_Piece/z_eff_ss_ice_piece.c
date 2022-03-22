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

u32 EffectSsIcePiece_Init(GlobalContext* globalCtx, u32 index, EffectSs* this, void* initParamsx);
void EffectSsIcePiece_Draw(GlobalContext* globalCtx, u32 index, EffectSs* this);
void EffectSsIcePiece_Update(GlobalContext* globalCtx, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Ice_Piece_InitVars = {
    EFFECT_SS_ICE_PIECE,
    EffectSsIcePiece_Init,
};

u32 EffectSsIcePiece_Init(GlobalContext* globalCtx, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsIcePieceInitParams* initParams = (EffectSsIcePieceInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->vec = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->life = initParams->life;
    this->draw = EffectSsIcePiece_Draw;
    this->update = EffectSsIcePiece_Update;
    this->rLifespan = initParams->life;
    this->rScale = initParams->scale * 100.0f;
    this->rYaw = Math_Atan2S(initParams->velocity.z, initParams->velocity.x);
    this->rPitch = 0;
    this->rRotSpeed =
        ((fabsf(initParams->velocity.x) + fabsf(initParams->velocity.y)) * 100.0f) * (Rand_ZeroFloat(1.0f) + 0.5f);

    return 1;
}

void EffectSsIcePiece_Draw(GlobalContext* globalCtx, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    s32 pad;
    f32 scale;
    u32 frames;
    f32 alpha;

    scale = this->rScale * 0.01f;
    frames = globalCtx->state.frames;

    OPEN_DISPS(gfxCtx, "../z_eff_ice_piece.c", 161);

    if ((this->rLifespan > 0) && (this->life < (this->rLifespan >> 1))) {
        alpha = ((this->life * 2.0f) / this->rLifespan);
        alpha *= 255.0f;
    } else {
        alpha = 255.0f;
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateY(this->rYaw * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_RotateX(this->rPitch * (M_PI / 0x8000), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(gfxCtx, "../z_eff_ice_piece.c", 185),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, (s32)alpha & 0xFF);
    func_8003435C(&this->pos, globalCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (1 * frames) % 256, 0x20, 0x10, 1, 0, (2 * frames) % 256,
                                0x40, 0x20));
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment1DL);

    CLOSE_DISPS(gfxCtx, "../z_eff_ice_piece.c", 209);
}

void EffectSsIcePiece_Update(GlobalContext* globalCtx, u32 index, EffectSs* this) {
    this->rPitch += this->rRotSpeed;
    this->velocity.x = this->velocity.x * 0.85f;
    this->velocity.y = this->velocity.y * 0.85f;
    this->velocity.z = this->velocity.z * 0.85f;
}
