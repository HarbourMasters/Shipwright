/*
 * File: z_en_light.c
 * Overlay: ovl_En_Light
 * Description: Flame
 */

#include "z_en_light.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS 0

void EnLight_Init(Actor* thisx, PlayState* play);
void EnLight_Destroy(Actor* thisx, PlayState* play);
void EnLight_Update(Actor* thisx, PlayState* play);
void EnLight_Draw(Actor* thisx, PlayState* play);
void EnLight_UpdateSwitch(Actor* thisx, PlayState* play);

const ActorInit En_Light_InitVars = {
    ACTOR_EN_LIGHT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnLight),
    (ActorFunc)EnLight_Init,
    (ActorFunc)EnLight_Destroy,
    (ActorFunc)EnLight_Update,
    (ActorFunc)EnLight_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ Color_RGBA8 primColor;
    /* 0x04 */ Color_RGB8 envColor;
    /* 0x07 */ u8 scale;
} FlameParams;

static FlameParams D_80A9E840[] = {
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },     { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 40 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },     { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },
    { { 255, 0, 170, 255 }, { 200, 0, 0 }, 75 },     { { 255, 255, 170, 255 }, { 255, 50, 0 }, 75 },
    { { 255, 255, 170, 255 }, { 255, 255, 0 }, 75 }, { { 255, 255, 170, 255 }, { 100, 255, 0 }, 75 },
    { { 255, 170, 255, 255 }, { 255, 0, 100 }, 75 }, { { 255, 170, 255, 255 }, { 100, 0, 255 }, 75 },
    { { 170, 255, 255, 255 }, { 0, 0, 255 }, 75 },   { { 170, 255, 255, 255 }, { 0, 150, 255 }, 75 },
};

void EnLight_Init(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;
    s16 yOffset;

    if (gSaveContext.gameMode == 3) {
        // special case for the credits
        yOffset = (this->actor.params < 0) ? 1 : 40;
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, yOffset + (s16)this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 180, -1);
    } else {
        yOffset = (this->actor.params < 0) ? 1 : 40;
        Lights_PointGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, yOffset + (s16)this->actor.world.pos.y,
                                this->actor.world.pos.z, 255, 255, 180, -1);
    }

    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Actor_SetScale(&this->actor, D_80A9E840[this->actor.params & 0xF].scale * 0.0001f);
    this->timer = (s32)(Rand_ZeroOne() * 255.0f);

    if ((this->actor.params & 0x400) != 0) {
        this->actor.update = EnLight_UpdateSwitch;
    }
}

void EnLight_Destroy(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void EnLight_UpdatePosRot(EnLight* this, PlayState* play) {
    // update yaw for billboard effect
    this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;

    if (this->actor.parent != NULL) {
        Math_Vec3f_Copy(&this->actor.world.pos, &(this->actor.parent)->world.pos);
        this->actor.world.pos.y += 17.0f;
    }

    this->timer++;
}

void EnLight_Update(Actor* thisx, PlayState* play) {
    f32 intensity;
    FlameParams* flameParams;
    s16 radius;
    EnLight* this = (EnLight*)thisx;

    flameParams = &D_80A9E840[this->actor.params & 0xF];
    intensity = (Rand_ZeroOne() * 0.5f) + 0.5f;
    radius = (this->actor.params < 0) ? 100 : 300;
    Lights_PointSetColorAndRadius(&this->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  radius);
    EnLight_UpdatePosRot(this, play);

    if (this->actor.params >= 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void EnLight_UpdateSwitch(Actor* thisx, PlayState* play) {
    f32 intensity;
    FlameParams* flameParams;
    EnLight* this = (EnLight*)thisx;
    f32 scale;

    flameParams = &D_80A9E840[this->actor.params & 0xF];
    scale = this->actor.scale.x / ((f32)flameParams->scale * 0.0001);

    if ((this->actor.params & 0x800) != 0) {
        if (Flags_GetSwitch(play, (this->actor.params & 0x3F0) >> 4)) {
            Math_StepToF(&scale, 1.0f, 0.05f);
        } else {
            if (scale < 0.1f) {
                Actor_SetScale(&this->actor, 0.0f);
                return;
            }
            Math_StepToF(&scale, 0.0f, 0.05f);
        }
    } else {
        if (Flags_GetSwitch(play, (this->actor.params & 0x3F0) >> 4)) {
            if (scale < 0.1f) {
                Actor_SetScale(&this->actor, 0.0f);
                return;
            }
            Math_StepToF(&scale, 0.0f, 0.05f);
        } else {
            Math_StepToF(&scale, 1.0f, 0.05f);
        }
    }

    Actor_SetScale(&this->actor, ((f32)flameParams->scale * 0.0001) * scale);
    intensity = (Rand_ZeroOne() * 0.5f) + 0.5f;
    Lights_PointSetColorAndRadius(&this->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  300.0f * scale);
    EnLight_UpdatePosRot(this, play);

    if (this->actor.params >= 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void EnLight_Draw(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;
    s32 pad;
    FlameParams* flameParams;
    Gfx* dList;

    flameParams = &D_80A9E840[this->actor.params & 0xF];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    if (this->actor.params >= 0) {
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (this->timer * -20) & 511, 32, 128));

        dList = gEffFire1DL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, flameParams->primColor.r, flameParams->primColor.g,
                        flameParams->primColor.b, flameParams->primColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, flameParams->envColor.r, flameParams->envColor.g, flameParams->envColor.b, 0);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 16, 32, 1, ((this->timer * 2) & 63),
                                    (this->timer * -6) & 127 * 1, 16, 32));

        dList = gUnusedCandleDL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0xC0, 0xC0, 255, 200, 0, 0);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    }

    Matrix_RotateY((s16)((Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000) *
                       (M_PI / 32768.0f),
                   MTXMODE_APPLY);

    if (this->actor.params & 1) {
        Matrix_RotateY(M_PI, MTXMODE_APPLY);
    }

    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, dList);

    CLOSE_DISPS(play->state.gfxCtx);
}
