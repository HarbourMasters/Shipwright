/*
 * File: z_eff_ss_solder_srch_ball.c
 * Overlay: ovl_Effect_Ss_Solder_Srch_Ball
 * Description: Vision sphere
 */

#include "z_eff_ss_solder_srch_ball.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rFlags regs[0]

#define PARAMS ((EffectSsSolderSrchBallInitParams*)initParamsx)

u32 EffectSsSolderSrchBall_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsSolderSrchBall_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsSolderSrchBall_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Solder_Srch_Ball_Profile = {
    EFFECT_SS_SOLDER_SRCH_BALL,
    EffectSsSolderSrchBall_Init,
};

u32 EffectSsSolderSrchBall_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSolderSrchBallInitParams* initParams = PARAMS;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->update = EffectSsSolderSrchBall_Update;
    if (!(initParams->flags & SOLDERSRCHBALL_INVISIBLE)) {
        this->draw = EffectSsSolderSrchBall_Draw;
    }
    this->life = 10;
    this->rgScale = initParams->scale;
    this->rFlags = initParams->flags;

    //! @bug actor field used to store an s16*
    // This bug is purely cosmetic. Nothing external will ever read this as an Actor, so there are no unintended
    // side-effects.
    this->actor = (Actor*)initParams->playerDetected;
    return 1;
}

void EffectSsSolderSrchBall_Draw(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rgScale / 100.0f;

    Gfx_SetupDL25_Opa(gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
    gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gSun1Tex));
    gSPDisplayList(POLY_XLU_DISP++, gSunSparkleMaterialDL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 255, 255);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateZF(DEG_TO_RAD(20.0f * play->state.frames), MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gSunSparkleModelDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsSolderSrchBall_Update(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    f32 diffX;
    f32 diffY;
    f32 diffZ;
    s16* playerDetected = (s16*)this->actor;
    Player* player = GET_PLAYER(play);

    diffX = player->actor.world.pos.x - this->pos.x;
    diffY = player->actor.world.pos.y - this->pos.y;
    diffZ = player->actor.world.pos.z - this->pos.z;

    if (this->rFlags >= SOLDERSRCHBALL_SMALL_DETECT_RADIUS) {
        if ((sqrtf(SQ(diffX) + SQ(diffZ)) < 10.0f) && (sqrtf(SQ(diffY)) < 10.0f)) {
            *playerDetected = true;
        }
    } else if (!BgCheck_SphVsFirstWall(&play->colCtx, &this->pos, 30.0f)) {
        if ((sqrtf(SQ(diffX) + SQ(diffZ)) < 40.0f) && (sqrtf(SQ(diffY)) < 80.0f)) {
            *playerDetected = true;
        }
    } else if (this->life > 1) {
        this->life = 1;
    }
}
