/*
 * File: z_eff_ss_bubble.c
 * Overlay: ovl_Effect_Ss_Bubble
 * Description: Water Bubbles
 */

#include "z_eff_ss_bubble.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rVecAdjX regs[1]
#define rVecAdjZ regs[2]

#define PARAMS ((EffectSsBubbleInitParams*)initParamsx)

u32 EffectSsBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsBubble_Update(PlayState* play2, u32 index, EffectSs* this);
void EffectSsBubble_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Bubble_Profile = {
    EFFECT_SS_BUBBLE,
    EffectSsBubble_Init,
};

static f32 sVecAdjMaximums[] = {
    291.0f,  // CONVEYOR_SPEED_SLOW
    582.0f,  // CONVEYOR_SPEED_MEDIUM
    1600.0f, // CONVEYOR_SPEED_FAST
};

u32 EffectSsBubble_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsBubbleInitParams* initParams = (EffectSsBubbleInitParams*)initParamsx;

    {
        TexturePtr tex = (Rand_ZeroOne() < 0.5f) ? gEffBubble1Tex : gEffBubble2Tex;

        this->gfx = (void*)OS_K0_TO_PHYSICAL(SEGMENTED_TO_K0(tex));
    }

    this->pos.x = ((Rand_ZeroOne() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.x;
    this->pos.y = (((Rand_ZeroOne() - 0.5f) * initParams->yPosRandScale) + initParams->yPosOffset) + initParams->pos.y;
    this->pos.z = ((Rand_ZeroOne() - 0.5f) * initParams->xzPosRandScale) + initParams->pos.z;
    Math_Vec3f_Copy(&this->vec, &this->pos);
    this->life = 1;
    this->rScale = (((Rand_ZeroOne() * 0.5f) + 1.0f) * initParams->scale) * 100.0f;
    this->draw = EffectSsBubble_Draw;
    this->update = EffectSsBubble_Update;

    return 1;
}

void EffectSsBubble_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
    Gfx_SetupDL25_Opa(gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 150, 150, 150, 0);
    gSPSegment(POLY_OPA_DISP++, 0x08, this->gfx);
    gSPDisplayList(POLY_OPA_DISP++, gEffBubbleDL);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsBubble_Update(PlayState* play2, u32 index, EffectSs* this) {
    WaterBox* waterBox;
    f32 waterSurfaceY = this->pos.y;
    Vec3f ripplePos;
    PlayState* play = play2;

    if (!WaterBox_GetSurface1(play, &play->colCtx, this->pos.x, this->pos.z, &waterSurfaceY, &waterBox)) {
        this->life = -1;
        return;
    }
    if (waterSurfaceY < this->pos.y) {
        ripplePos.x = this->pos.x;
        ripplePos.y = waterSurfaceY;
        ripplePos.z = this->pos.z;
        EffectSsGRipple_Spawn(play, &ripplePos, 0, 80, 0);
        this->life = -1;
        return;
    }
    if (((play->gameplayFrames + index) % 8) == 0) {
        CollisionPoly* colPoly;
        ConveyorSpeed conveyorSpeed;
        s16 conveyorDir;
        f32 rVecAdjMax;

        BgCheck_EntityRaycastFloor2_1(play, &play->colCtx, &colPoly, &this->pos);
        conveyorSpeed = SurfaceType_GetConveyorSpeed(&play->colCtx, colPoly, BGCHECK_SCENE);
        if ((conveyorSpeed != CONVEYOR_SPEED_DISABLED) &&
            !SurfaceType_IsFloorConveyor(&play->colCtx, colPoly, BGCHECK_SCENE)) {
            conveyorDir = SurfaceType_GetConveyorDirection(&play->colCtx, colPoly, BGCHECK_SCENE) << 10;
            rVecAdjMax = sVecAdjMaximums[conveyorSpeed - 1];
            this->rVecAdjX = Math_SinS(conveyorDir) * rVecAdjMax;
            this->rVecAdjZ = Math_CosS(conveyorDir) * rVecAdjMax;
        }
    }
    this->vec.x += this->rVecAdjX / 100.0f;
    this->vec.z += this->rVecAdjZ / 100.0f;
    this->pos.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + this->vec.x;
    this->accel.y = (Rand_ZeroOne() - 0.3f) * 0.2f;
    this->pos.z = (Rand_ZeroOne() * 0.5f - 0.25f) + this->vec.z;
    this->life++;
}
