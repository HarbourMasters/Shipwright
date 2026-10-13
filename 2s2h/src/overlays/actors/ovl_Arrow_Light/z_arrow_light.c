/*
 * File: z_arrow_light.c
 * Overlay: ovl_Arrow_Light
 * Description: Light Arrow
 */

#include "z_arrow_light.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ArrowLight_Init(Actor* thisx, PlayState* play);
void ArrowLight_Destroy(Actor* thisx, PlayState* play);
void ArrowLight_Update(Actor* thisx, PlayState* play);
void ArrowLight_Draw(Actor* thisx, PlayState* play);

void ArrowLight_Charge(ArrowLight* this, PlayState* play);
void ArrowLight_Fly(ArrowLight* this, PlayState* play);

#include "overlays/ovl_Arrow_Light/ovl_Arrow_Light.h"

ActorProfile Arrow_Light_Profile = {
    /**/ ACTOR_ARROW_LIGHT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ArrowLight),
    /**/ ArrowLight_Init,
    /**/ ArrowLight_Destroy,
    /**/ ArrowLight_Update,
    /**/ ArrowLight_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_STOP),
};

void ArrowLight_SetupAction(ArrowLight* this, ArrowLightActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ArrowLight_Init(Actor* thisx, PlayState* play) {
    ArrowLight* this = (ArrowLight*)thisx;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->radius = 0;
    this->height = 1.0f;
    ArrowLight_SetupAction(this, ArrowLight_Charge);
    Actor_SetScale(&this->actor, 0.01f);
    this->alpha = 130;
    this->timer = 0;
    this->screenFillIntensity = 0.0f;
}

void ArrowLight_Destroy(Actor* thisx, PlayState* play) {
    Magic_Reset(play);
    (void)"消滅"; // Unreferenced in retail, means "Disappearance"
}

void ArrowLight_Charge(ArrowLight* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->radius < 10) {
        this->radius++;
    }

    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_ARROW_CHARGE_LIGHT - SFX_FLAG);
    if (arrow->actor.parent == NULL) {
        this->firedPos = this->actor.world.pos;
        this->radius = 10;
        ArrowLight_SetupAction(this, ArrowLight_Fly);
        this->alpha = 255;
    }
}

void ArrowLight_Lerp(Vec3f* firedPos, Vec3f* pos, f32 scale) {
    VEC3F_LERPIMPDST(firedPos, firedPos, pos, scale);
}

void ArrowLight_Hit(ArrowLight* this, PlayState* play) {
    f32 scale;
    u16 timer;

    if (this->actor.projectedW < 50.0f) {
        scale = 10.0f;
    } else if (this->actor.projectedW > 950.0f) {
        scale = 310.0f;
    } else {
        scale = this->actor.projectedW;
        scale = ((scale - 50.0f) * (1.0f / 3.0f)) + 10.0f;
    }

    timer = this->timer;
    if (timer != 0) {
        this->timer--;
        if (this->timer >= 8) {
            f32 offset = (this->timer - 8) * (1.0f / 24.0f);

            offset = SQ(offset);
            this->radius = (((1.0f - offset) * scale) + 10.0f);
            this->height = F32_LERPIMP(this->height, 2.0f, 0.1f);
            if (this->timer < 16) {
                this->alpha = (this->timer * 35) - 280;
            }
        }
    }
    if (this->timer >= 9) {
        if (this->screenFillIntensity < 1.0f) {
            this->screenFillIntensity += 0.25f;
        }
    } else {
        if (this->screenFillIntensity > 0.0f) {
            this->screenFillIntensity -= 0.125f;
        }
    }
    if (this->timer < 8) {
        this->alpha = 0;
    }
    if (this->timer == 0) {
        this->timer = 255;
        Actor_Kill(&this->actor);
        return;
    }
}

void ArrowLight_Fly(ArrowLight* this, PlayState* play) {
    EnArrow* arrow = (EnArrow*)this->actor.parent;
    s32 pad[2];

    if ((arrow == NULL) || (arrow->actor.update == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.world.pos = arrow->actor.world.pos;
    this->actor.shape.rot = arrow->actor.shape.rot;

    this->height = Math_Vec3f_DistXYZ(&this->firedPos, &this->actor.world.pos) * (1.0f / 24.0f);
    if (this->height < 1.0f) {
        this->height = 1.0f;
    }

    ArrowLight_Lerp(&this->firedPos, &this->actor.world.pos, 0.05f);

    if (arrow->unk_261 & 1) {
        Actor_PlaySfx(&this->actor, NA_SE_IT_EXPLOSION_LIGHT);
        ArrowLight_SetupAction(this, ArrowLight_Hit);
        this->timer = 32;
        this->alpha = 255;
        return;
    }
    if (arrow->unk_260 < 34) {
        if (this->alpha < 35) {
            Actor_Kill(&this->actor);
            return;
        }
        this->alpha -= 25;
    }
}

void ArrowLight_Update(Actor* thisx, PlayState* play) {
    ArrowLight* this = (ArrowLight*)thisx;

    if ((play->msgCtx.msgMode == MSGMODE_E) || (play->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actionFunc(this, play);
}

void ArrowLight_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ArrowLight* this = (ArrowLight*)thisx;
    u32 frames = play->state.frames;
    EnArrow* arrow = (EnArrow*)this->actor.parent;

    if ((arrow != NULL) && (arrow->actor.update != NULL) && (this->timer < 255)) {
        Actor* transform = (arrow->unk_261 & 2) ? &this->actor : &arrow->actor;

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(transform->world.pos.x, transform->world.pos.y, transform->world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(transform->shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(transform->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(transform->shape.rot.z, MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        if (this->screenFillIntensity > 0.0f) {
            POLY_XLU_DISP = Gfx_SetupDL57(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (s32)(this->screenFillIntensity * 30.0f) & 0xFF,
                            (s32)(40.0f * this->screenFillIntensity) & 0xFF, 0,
                            (s32)(150.0f * this->screenFillIntensity) & 0xFF);
            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gSPDisplayList(POLY_XLU_DISP++, D_0E000000_TO_SEGMENTED(fillRect));
        }

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetPrimColorOverride(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, this->alpha,
                                COSMETIC_ID("Effects.LightArrowPrim"));
        gDPSetEnvColorOverride(POLY_XLU_DISP++, 255, 255, 0, 128, COSMETIC_ID("Effects.LightArrowSec"));

        Matrix_RotateZYX(0x4000, 0, 0, MTXMODE_APPLY);
        if (this->timer != 0) {
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, 1500.0f, 0.0f, MTXMODE_APPLY);
        }
        Matrix_Scale(this->radius * 0.2f, this->height * 4.0f, this->radius * 0.2f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -700.0f, 0.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightArrowMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 511 - ((frames * 5) % 512), 0, 4, 32,
                                                           1, 511 - ((frames * 10) % 512), 511 - ((frames * 30) % 512),
                                                           8, 16, -5, 0, -10, -30));
        gSPDisplayList(POLY_XLU_DISP++, gLightArrowModelDL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
