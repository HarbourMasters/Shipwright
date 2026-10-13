/*
 * File: z_eff_kamejima_wave.c
 * Overlay: ovl_Eff_Kamejima_Wave
 * Description: Wave Created by Turtle Awakening
 */
#include "prevent_bss_reordering.h"
#include "z_eff_kamejima_wave.h"
#include "objects/object_kamejima/object_kamejima.h"

#include "2s2h/BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EffKamejimaWave_Init(Actor* thisx, PlayState* play);
void EffKamejimaWave_Destroy(Actor* thisx, PlayState* play);
void EffKamejimaWave_Update(Actor* thisx, PlayState* play);
void EffKamejimaWave_Draw(Actor* thisx, PlayState* play);

void func_80BCEC6C(EffKamejimaWave* this, PlayState* play);
void func_80BCEDE0(EffKamejimaWave* this, PlayState* play);
void func_80BCEBC0(EffKamejimaWave* this, PlayState* play);
void func_80BCED34(EffKamejimaWave* this, PlayState* play);
void EffKamejimaWave_SetVtxAlpha(u8 alpha);

ActorProfile Eff_Kamejima_Wave_Profile = {
    /**/ ACTOR_EFF_KAMEJIMA_WAVE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_KAMEJIMA,
    /**/ sizeof(EffKamejimaWave),
    /**/ EffKamejimaWave_Init,
    /**/ EffKamejimaWave_Destroy,
    /**/ EffKamejimaWave_Update,
    /**/ NULL,
};

static Color_RGBA8 sPrimColors[] = {
    { 255, 240, 255, 255 }, { 255, 255, 255, 255 }, { 255, 240, 230, 255 }, { 140, 170, 220, 255 }
};
static Color_RGBA8 sEnvColors[] = {
    { 130, 80, 90, 255 }, { 90, 120, 130, 255 }, { 130, 80, 70, 255 }, { 40, 60, 110, 255 }
};

s16 sVtxAlpha;
AnimatedMaterial* D_80BCF1C4;

void EffKamejimaWave_Init(Actor* thisx, PlayState* play) {
    EffKamejimaWave* this = (EffKamejimaWave*)thisx;

    Actor_SetScale(&this->actor, 0.2f);
    this->actor.scale.y = 0.0f;
    if (EFFKAMEJIMAWAVE_GET_F(thisx) == EFFKAMEJIMAWAVE_PARAM_1) {
        this->actionFunc = func_80BCEDE0;
        sVtxAlpha = 255;
    } else {
        this->actionFunc = func_80BCEC6C;
        sVtxAlpha = 255;
    }
    D_80BCF1C4 = Lib_SegmentedToVirtual(gTurtleWaveTexAnim);
}

void EffKamejimaWave_Destroy(Actor* thisx, PlayState* play) {
}

void func_80BCEBC0(EffKamejimaWave* this, PlayState* play) {
    this->actor.scale.y += this->scaleOffset;
    this->scaleOffset -= 0.003f;
    this->actor.shape.rot.x += 0x64;
    if (this->scaleOffset < 0.0f) {
        this->actor.scale.x += 0.003f;
    }
    if (this->scaleOffset < -0.01f) {
        this->scaleOffset = -0.01f;
    }
    if (this->actor.scale.y < 0.0f) {
        this->actor.scale.y = 0.0f;
        this->actor.scale.x = 0.2f;
        this->actionFunc = func_80BCEC6C;
        this->actor.draw = NULL;
    }
}

void func_80BCEC6C(EffKamejimaWave* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_480)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_480));
        if (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_480)]->id == 2) {
            this->scaleOffset = 0.05f;
            this->actionFunc = func_80BCEBC0;
            this->actor.draw = EffKamejimaWave_Draw;
            sVtxAlpha = 255;
            this->actor.shape.rot.x = 0;
            Actor_PlaySfx(&this->actor, NA_SE_EV_TORTOISE_WAKE_UP);
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80BCED34(EffKamejimaWave* this, PlayState* play) {
    this->actor.scale.y += this->scaleOffset;
    this->scaleOffset -= 0.002f;
    this->actor.shape.rot.x += 0x8C;
    if (this->scaleOffset < 0.0f) {
        this->actor.scale.x += 0.002f;
    }
    if (this->scaleOffset < 0.0f) {
        this->scaleOffset = 0;
    }
    if (sVtxAlpha >= 4) {
        sVtxAlpha -= 4;
    } else {
        this->actor.scale.y = 0.0f;
        this->actionFunc = func_80BCEDE0;
        this->actor.draw = NULL;
    }
}

void func_80BCEDE0(EffKamejimaWave* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_502)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_502));
        if (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_502)]->id == 2) {
            this->scaleOffset = 0.03f;
            this->actor.scale.x = 0.35f;
            this->actionFunc = func_80BCED34;
            this->actor.draw = EffKamejimaWave_Draw;
            this->actor.shape.rot.x = 0;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void EffKamejimaWave_Update(Actor* thisx, PlayState* play) {
    EffKamejimaWave* this = (EffKamejimaWave*)thisx;

    this->actionFunc(this, play);
}

void EffKamejimaWave_SetVtxAlpha(u8 alpha) {
    Vtx* vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(&gTurtleWaveVtx));

    vtx[2].v.cn[3] = alpha;
    vtx[6].v.cn[3] = alpha;
    vtx[8].v.cn[3] = alpha;
    vtx[9].v.cn[3] = alpha;
    vtx[12].v.cn[3] = alpha;
    vtx[16].v.cn[3] = alpha;
    vtx[18].v.cn[3] = alpha;
    vtx[19].v.cn[3] = alpha;
}

void EffKamejimaWave_Draw(Actor* thisx, PlayState* play) {
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    Environment_LerpSandstormColors(sPrimColors, &primColor);
    Environment_LerpSandstormColors(sEnvColors, &envColor);
    EffKamejimaWave_SetVtxAlpha(sVtxAlpha);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Scale(6.0f, 5.0f, 5.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    AnimatedMat_Draw(play, D_80BCF1C4);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColor.r, primColor.g, primColor.b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, 255);
    gSPDisplayList(POLY_XLU_DISP++, gTurtleWaveDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
