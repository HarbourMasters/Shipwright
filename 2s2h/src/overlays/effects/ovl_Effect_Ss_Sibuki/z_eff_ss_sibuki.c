/*
 * File: z_eff_ss_sibuki.c
 * Overlay: ovl_Effect_Ss_Sibuki
 * Description:
 */

#include "z_eff_ss_sibuki.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rPrimColorR regs[0]
#define rPrimColorG regs[1]
#define rPrimColorB regs[2]
#define rPrimColorA regs[3]
#define rEnvColorR regs[4]
#define rEnvColorG regs[5]
#define rEnvColorB regs[6]
#define rEnvColorA regs[7]
#define rMoveDelay regs[8]
#define rDirection regs[9]
#define rScale regs[10]

#define PARAMS ((EffectSsSibukiInitParams*)initParamsx)

u32 EffectSsSibuki_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsSibuki_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsSibuki_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Sibuki_Profile = {
    EFFECT_SS_SIBUKI,
    EffectSsSibuki_Init,
};

u32 EffectSsSibuki_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsSibukiInitParams* initParams = PARAMS;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;

    {
        TexturePtr tex = (KREG(2) != 0) ? gEffBubble2Tex : gEffBubble1Tex;

        this->gfx = (void*)OS_K0_TO_PHYSICAL(SEGMENTED_TO_K0(tex));
    }

    this->life = ((s32)((Rand_ZeroOne() * (500.0f + KREG(64))) * 0.01f)) + KREG(65) + 10;
    this->rMoveDelay = initParams->moveDelay + 1;
    this->draw = EffectSsSibuki_Draw;
    this->update = EffectSsSibuki_Update;
    this->rDirection = initParams->direction;
    this->rScale = initParams->scale;
    this->rPrimColorR = 100;
    this->rPrimColorG = 100;
    this->rPrimColorB = 100;
    this->rPrimColorA = 100;
    this->rEnvColorR = 255;
    this->rEnvColorG = 255;
    this->rEnvColorB = 255;
    this->rEnvColorA = 255;

    return 1;
}

void EffectSsSibuki_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
    Gfx_SetupDL25_Opa(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, this->rPrimColorA);
    gDPSetEnvColor(POLY_OPA_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, this->rEnvColorA);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, gEffBubbleDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsSibuki_Update(PlayState* play, u32 index, EffectSs* this) {
    Player* player = GET_PLAYER(play);
    s32 pad[2];
    f32 xzVelScale;

    if (this->pos.y <= player->actor.floorHeight) {
        this->life = 0;
    }

    if (this->rMoveDelay != 0) {
        this->rMoveDelay--;

        if (this->rMoveDelay == 0) {
            s16 yaw = Camera_GetInputDirYaw(Play_GetCamera(play, CAM_ID_MAIN));

            xzVelScale = ((200.0f + KREG(20)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * (KREG(23) + 20.0f));

            if (this->rDirection != 0) {
                xzVelScale *= -1.0f;
            }

            this->velocity.x = Math_CosS(yaw) * xzVelScale;
            this->velocity.z = -Math_SinS(yaw) * xzVelScale;

            this->velocity.y = ((700.0f + KREG(21)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * (KREG(24) + 20.0f));
            this->accel.y = ((-100.0f + KREG(22)) * 0.01f) + ((0.1f * Rand_ZeroOne()) * KREG(25));

            if (KREG(3) != 0) {
                this->velocity.x *= (KREG(3) * 0.01f);
                this->velocity.y *= (KREG(3) * 0.01f);
                this->velocity.z *= (KREG(3) * 0.01f);
                this->accel.y *= (KREG(4) * 0.01f);
            }
        }
    } else {
        if (this->rScale != 0) {
            this->rScale = (this->rScale - KREG(26)) - 3;
        }
    }
}
