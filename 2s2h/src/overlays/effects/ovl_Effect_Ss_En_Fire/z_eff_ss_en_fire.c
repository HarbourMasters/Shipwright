/*
 * File: z_eff_ss_en_fire.c
 * Overlay: ovl_Effect_Ss_En_Fire
 * Description:
 */

#include "z_eff_ss_en_fire.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScaleMax regs[0]
#define rScale regs[1]
#define rLifespan regs[2]
#define rUnused regs[3]
#define rPitch regs[4]
#define rYaw regs[5]
#define rReg6 regs[6]
#define rBodyPart regs[7]
#define rFlags regs[8]
#define rScroll regs[9]

#define PARAMS ((EffectSsEnFireInitParams*)initParamsx)

u32 EffectSsEnFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsEnFire_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsEnFire_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_En_Fire_Profile = {
    EFFECT_SS_EN_FIRE,
    EffectSsEnFire_Init,
};

u32 EffectSsEnFire_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsEnFireInitParams* initParams = PARAMS;

    Math_Vec3f_Copy(&this->pos, &initParams->pos);
    Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
    Math_Vec3f_Copy(&this->accel, &gZeroVec3f);
    this->life = 20;
    this->rLifespan = this->life;
    this->actor = initParams->actor;
    this->rScroll = Rand_ZeroOne() * 20.0f;
    this->draw = EffectSsEnFire_Draw;
    this->update = EffectSsEnFire_Update;
    this->rUnused = -15;

    if (initParams->bodyPart <= BODYPART_NONE) {
        this->rYaw = Math_Vec3f_Yaw(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.y;
        this->rPitch =
            Math_Vec3f_Pitch(&initParams->actor->world.pos, &initParams->pos) - initParams->actor->shape.rot.x;
        this->vec.z = Math_Vec3f_DistXYZ(&initParams->pos, &initParams->actor->world.pos);
    }

    this->rScaleMax = initParams->scale;

    if (initParams->params & ENFIRE_PARAM_USE_SCALE) {
        this->rScale = initParams->scale;
    } else {
        this->rScale = 0;
    }

    this->rReg6 = initParams->params & 0x7FFF;
    this->rBodyPart = initParams->bodyPart;
    this->rFlags = initParams->flags;

    return 1;
}

void EffectSsEnFire_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale;
    s16 camYaw;
    s32 pad[3];
    s16 redGreen;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    camYaw = (Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000);
    Matrix_RotateYS(camYaw, MTXMODE_APPLY);

    scale = Math_SinS(this->life * 0x333) * (this->rScale * 0.00005f);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    redGreen = this->life - 5;

    if (redGreen < 0) {
        redGreen = 0;
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, redGreen * 12.7f, 0, 0, 0);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x0, 0x80, redGreen * 12.7f, redGreen * 12.7f, 0, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (this->rScroll * -20) & 0x1FF, 0x20,
                                  0x80, 0, 0, 0, -20));

    if ((this->rFlags & 0x7FFF) || (this->life < 18)) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx);
}

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3f firePos[10];
} FireActorF; // size = 0x1BC

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x14C */ Vec3s firePos[10];
} FireActorS; // size = 0x180

void EffectSsEnFire_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rScroll++;

    if (this->actor != NULL) {
        if (this->actor->colorFilterTimer >= 22) {
            this->life++;
        }
        if (this->actor->update != NULL) {
            Math_SmoothStepToS(&this->rScale, this->rScaleMax, 1, this->rScaleMax >> 3, 0);

            if (this->rBodyPart <= BODYPART_NONE) {
                Matrix_Translate(this->actor->world.pos.x, this->actor->world.pos.y, this->actor->world.pos.z,
                                 MTXMODE_NEW);
                Matrix_RotateYS(this->rYaw + this->actor->shape.rot.y, MTXMODE_APPLY);
                Matrix_RotateXS(this->rPitch + this->actor->shape.rot.x, MTXMODE_APPLY);
                Matrix_MultVec3f(&this->vec, &this->pos);
            } else if (this->rFlags & ENFIRE_FLAGS_BODYPART_POS_VEC3S) {
                this->pos.x = ((FireActorS*)this->actor)->firePos[this->rBodyPart].x;
                this->pos.y = ((FireActorS*)this->actor)->firePos[this->rBodyPart].y;
                this->pos.z = ((FireActorS*)this->actor)->firePos[this->rBodyPart].z;
            } else {
                this->pos.x = ((FireActorF*)this->actor)->firePos[this->rBodyPart].x;
                this->pos.y = ((FireActorF*)this->actor)->firePos[this->rBodyPart].y;
                this->pos.z = ((FireActorF*)this->actor)->firePos[this->rBodyPart].z;
            }
        } else if (this->rReg6 != 0) {
            this->life = 0;
        } else {
            this->actor = NULL;
        }
    }
}
