/*
 * File: z_eff_ss_fire_tail.c
 * Overlay: ovl_Effect_Ss_Fire_Tail
 * Description: Fire (burned by something)
 */

#include "z_eff_ss_fire_tail.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define rScale regs[0]
#define rLifespan regs[1]
#define rReg2 regs[2]
#define rReg3 regs[3]
#define rPrimColorR regs[4]
#define rPrimColorG regs[5]
#define rPrimColorB regs[6]
#define rEnvColorR regs[7]
#define rEnvColorG regs[8]
#define rEnvColorB regs[9]
#define rReg10 regs[10]
#define rBodyPart regs[11]
#define rType regs[12]

u32 EffectSsFireTail_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsFireTail_Draw(PlayState* play, u32 index, EffectSs* this);
void EffectSsFireTail_Update(PlayState* play, u32 index, EffectSs* this);

EffectSsInit Effect_Ss_Fire_Tail_InitVars = {
    EFFECT_SS_FIRE_TAIL,
    EffectSsFireTail_Init,
};

u32 EffectSsFireTail_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsFireTailInitParams* initParams = (EffectSsFireTailInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->vec = initParams->unk_14;
    this->velocity.x = 0.0f;
    this->velocity.y = 0.0f;
    this->velocity.z = 0.0f;
    this->accel.x = 0.0f;
    this->accel.y = 0.0f;
    this->accel.z = 0.0f;
    this->life = initParams->life;
    this->actor = initParams->actor;
    this->draw = EffectSsFireTail_Draw;
    this->update = EffectSsFireTail_Update;
    this->rScale = initParams->scale * 1000.0f;
    this->rLifespan = initParams->life;
    this->rReg2 = -0xA;
    this->rReg3 = -0xF;
    if (initParams->unk_20 == 0) {
        initParams->unk_20 = 1;
    }
    this->rReg10 = initParams->unk_20;
    this->rPrimColorR = initParams->primColor.r;
    this->rPrimColorG = initParams->primColor.g;
    this->rPrimColorB = initParams->primColor.b;
    this->rEnvColorR = initParams->envColor.r;
    this->rEnvColorG = initParams->envColor.g;
    this->rEnvColorB = initParams->envColor.b;
    this->rBodyPart = initParams->bodyPart;
    this->rType = initParams->type;

    return 1;
}

void EffectSsFireTail_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    s16 yaw;
    Vec3f scale;
    f32 temp1;
    f32 temp2;
    f32 dist;

    OPEN_DISPS(gfxCtx);

    scale.x = scale.y = scale.z = 0.0f;

    if (this->actor != NULL) {

        this->vec = this->actor->velocity;

        if (this->rBodyPart < 0) {
            Matrix_Translate(this->pos.x + this->actor->world.pos.x, this->pos.y + this->actor->world.pos.y,
                             this->pos.z + this->actor->world.pos.z, MTXMODE_NEW);
        } else {
            Player* player = GET_PLAYER(play);
            s16 bodyPart = this->rBodyPart;

            this->pos.x =
                player->bodyPartsPos[bodyPart].x - (Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) * 5.0f);
            this->pos.y = player->bodyPartsPos[bodyPart].y;
            this->pos.z =
                player->bodyPartsPos[bodyPart].z - (Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) * 5.0f);

            Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
        }
    } else {
        Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    }

    yaw = Math_Vec3f_Yaw(&scale, &this->vec) - Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    temp1 = fabsf(Math_CosS(yaw));
    temp2 = Math_SinS(yaw);
    dist = Math_Vec3f_DistXZ(&scale, &this->vec) / (this->rReg10 * 0.1f);
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000) * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_RotateZ(temp2 * this->rReg2 * dist * (M_PI / 180.0f), MTXMODE_APPLY);
    temp2 = 1.0f - ((f32)(this->life + 1) / this->rLifespan);
    temp2 = 1.0f - SQ(temp2);
    scale.x = scale.y = scale.z = temp2 * (this->rScale * 0.000010000001f);
    Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    temp1 = (this->rReg3 * 0.01f * temp1 * dist) + 1.0f;

    if (temp1 < 0.1f) {
        temp1 = 0.1f;
    }

    Matrix_Scale(1.0f, temp1, 1.0f / temp1, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->rPrimColorR, this->rPrimColorG, this->rPrimColorB, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, this->rEnvColorR, this->rEnvColorG, this->rEnvColorB, 0);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                (play->state.frames * -0x14) & 0x1FF, 32, 128));

    if (this->rType != 0) {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire2DL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(gfxCtx);
}

void EffectSsFireTail_Update(PlayState* play, u32 index, EffectSs* this) {
    this->rScale *= 0.9f;
}
