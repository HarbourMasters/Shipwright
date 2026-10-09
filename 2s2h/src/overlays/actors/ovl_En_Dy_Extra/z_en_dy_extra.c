/*
 * File: z_en_dy_extra.c
 * Overlay: ovl_En_Dy_Extra
 * Description: Great Fairy beam
 */

#include "z_en_dy_extra.h"
#include "objects/object_dy_obj/object_dy_obj.h"

#include "2s2h/BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnDyExtra_Init(Actor* thisx, PlayState* play);
void EnDyExtra_Destroy(Actor* thisx, PlayState* play);
void EnDyExtra_Update(Actor* thisx, PlayState* play);
void EnDyExtra_Draw(Actor* thisx, PlayState* play);

void EnDyExtra_WaitForTrigger(EnDyExtra* this, PlayState* play);
void EnDyExtra_Fall(EnDyExtra* this, PlayState* play);

ActorProfile En_Dy_Extra_Profile = {
    /**/ ACTOR_EN_DY_EXTRA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DY_OBJ,
    /**/ sizeof(EnDyExtra),
    /**/ EnDyExtra_Init,
    /**/ EnDyExtra_Destroy,
    /**/ EnDyExtra_Update,
    /**/ EnDyExtra_Draw,
};

void EnDyExtra_Destroy(Actor* thisx, PlayState* play) {
}

void EnDyExtra_Init(Actor* thisx, PlayState* play) {
    EnDyExtra* this = (EnDyExtra*)thisx;

    this->type = this->actor.params;
    this->actor.scale.x = 0.025f;
    this->actor.scale.y = 0.039f;
    this->actor.scale.z = 0.025f;
    this->initPos = this->actor.world.pos;
    this->actor.gravity = -0.2f;
    this->alphaScale = 1.0f;
    this->timer = 60;
    this->actionFunc = EnDyExtra_WaitForTrigger;
}

void EnDyExtra_WaitForTrigger(EnDyExtra* this, PlayState* play) {
    Math_ApproachF(&this->actor.gravity, 0.0f, 0.1f, 0.005f);

    if (this->actor.world.pos.y < -85.0f) {
        this->actor.velocity.y = 0.0f;
    }

    if ((this->timer == 0) && this->trigger) {
        this->timer = 50;
        this->actionFunc = EnDyExtra_Fall;
    }
}

void EnDyExtra_Fall(EnDyExtra* this, PlayState* play) {
    Math_ApproachF(&this->actor.gravity, 0.0f, 0.1f, 0.005f);

    if ((this->timer == 0) || (this->alphaScale < 0.02f)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->alphaScale -= 0.02f;

    if (this->actor.world.pos.y < -85.0f) {
        this->actor.velocity.y = 0.0f;
    }
}

void EnDyExtra_Update(Actor* thisx, PlayState* play) {
    EnDyExtra* this = (EnDyExtra*)thisx;

    DECR(this->timer);
    Actor_PlaySfx(&this->actor, NA_SE_PL_SPIRAL_HEAL_BEAM - SFX_FLAG);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
}

static Color_RGBA8 sPrimColors[] = {
    { 255, 255, 170, 255 }, { 255, 170, 255, 255 }, { 255, 255, 170, 255 },
    { 170, 255, 255, 255 }, { 255, 255, 170, 255 },
};

static Color_RGBA8 sEnvColors[] = {
    { 255, 100, 0, 255 }, { 255, 0, 100, 255 }, { 100, 255, 0, 255 }, { 0, 100, 255, 255 }, { 255, 230, 0, 255 }
};

static u8 sAlphaTypeIndices[] = {
    2, 1, 1, 2, 0, 0, 2, 1, 0, 2, 1, 0, 2, 1, 0, 2, 1, 0, 2, 1, 0, 2, 1, 0, 1, 2, 0,
};

void EnDyExtra_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDyExtra* this = (EnDyExtra*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vtx* vertices = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(gGreatFairySpiralBeamVtx));
    s32 i;
    u8 alphas[3];

    alphas[0] = 0.0f;
    alphas[1] = (s32)(this->alphaScale * 240.0f);
    alphas[2] = (s32)(this->alphaScale * 255.0f);

    for (i = 0; i < ARRAY_COUNT(sAlphaTypeIndices); i++) {
        if (sAlphaTypeIndices[i]) {
            vertices[i].v.cn[3] = alphas[sAlphaTypeIndices[i]];
        }
    }

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, play->state.frames * 2, 0, 0x20, 0x40, 1, play->state.frames,
                                  play->state.frames * -8, 0x10, 0x10, 2, 0, 1, -8));
    gDPPipeSync(POLY_XLU_DISP++);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sPrimColors[this->type].r, sPrimColors[this->type].g,
                    sPrimColors[this->type].b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sEnvColors[this->type].r, sEnvColors[this->type].g, sEnvColors[this->type].b, 128);
    gSPDisplayList(POLY_XLU_DISP++, gGreatFairySpiralBeamDL);

    CLOSE_DISPS(gfxCtx);
}
