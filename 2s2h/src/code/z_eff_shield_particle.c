#include "z64eff_shield_particle.h"
#include "z64effect.h"

#include "global.h"
#include "vt.h"

#include "assets/code/eff_shield_particle/eff_shield_particle.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

void EffectShieldParticle_Init(void* thisx, void* initParamsx) {
    EffectShieldParticle* this = (EffectShieldParticle*)thisx;
    EffectShieldParticleInit* initParams = (EffectShieldParticleInit*)initParamsx;
    EffectShieldParticleElement* elem;

    if ((this != NULL) && (initParams != NULL)) {
        this->numElements = initParams->numElements;
        if (this->numElements > ARRAY_COUNT(this->elements)) {
            return;
        }

        this->position = initParams->position;
        this->primColorStart = initParams->primColorStart;
        this->envColorStart = initParams->envColorStart;
        this->primColorMid = initParams->primColorMid;
        this->envColorMid = initParams->envColorMid;
        this->primColorEnd = initParams->primColorEnd;
        this->envColorEnd = initParams->envColorEnd;
        this->deceleration = initParams->deceleration;
        this->maxInitialSpeed = initParams->maxInitialSpeed;
        this->lengthCutoff = initParams->lengthCutoff;
        this->duration = initParams->duration;
        this->timer = 0;

        for (elem = &this->elements[0]; elem < &this->elements[this->numElements]; elem++) {
            elem->initialSpeed = (Rand_ZeroOne() * (this->maxInitialSpeed * 0.5f)) + (this->maxInitialSpeed * 0.5f);
            elem->endX = 0.0f;
            elem->startXChange = 0.0f;
            elem->startX = 0.0f;
            elem->endXChange = elem->initialSpeed;
            elem->yaw = Rand_ZeroOne() * 65534.0f;
            elem->pitch = Rand_ZeroOne() * 65534.0f;
        }

        this->lightDecay = initParams->lightDecay;
        if (this->lightDecay == true) {
            this->lightInfo.type = LIGHT_POINT_NOGLOW;
            this->lightInfo.params.point = initParams->lightPoint;
            this->lightNode =
                LightContext_InsertLight(Effect_GetPlayState(), &Effect_GetPlayState()->lightCtx, &this->lightInfo);
        } else {
            this->lightNode = NULL;
        }
    }
}

void EffectShieldParticle_Destroy(void* thisx) {
    EffectShieldParticle* this = (EffectShieldParticle*)thisx;

    if ((this != NULL) && (this->lightDecay == true)) {
        if (this->lightNode == Effect_GetPlayState()->lightCtx.listHead) {
            Effect_GetPlayState()->lightCtx.listHead = this->lightNode->next;
        }
        LightContext_RemoveLight(Effect_GetPlayState(), &Effect_GetPlayState()->lightCtx, this->lightNode);
    }
}

s32 EffectShieldParticle_Update(void* thisx) {
    EffectShieldParticle* this = (EffectShieldParticle*)thisx;
    EffectShieldParticleElement* elem;

    if (this == NULL) {
        return 0;
    }

    for (elem = &this->elements[0]; elem < &this->elements[this->numElements]; elem++) {
        elem->endXChange -= this->deceleration;
        if (elem->endXChange < 0.0f) {
            elem->endXChange = 0.0f;
        }

        if (elem->startXChange > 0.0f) {
            elem->startXChange -= this->deceleration;
            if (elem->startXChange < 0.0f) {
                elem->startXChange = 0.0f;
            }
        }

        elem->endX += elem->endXChange;
        elem->startX += elem->startXChange;

        if ((elem->startXChange == 0.0f) && (this->lengthCutoff < (elem->endX - elem->startX))) {
            elem->startXChange = elem->initialSpeed;
        }
    }

    if (this->lightDecay == true) {
        this->lightInfo.params.point.radius /= 2;
    }

    this->timer++;

    if (this->duration < this->timer) {
        return 1;
    }

    return 0;
}

void EffectShieldParticle_GetColors(EffectShieldParticle* this, Color_RGBA8* primColor, Color_RGBA8* envColor) {
    s32 halfDuration = this->duration * 0.5f;

    if (halfDuration == 0) {
        primColor->r = this->primColorStart.r;
        primColor->g = this->primColorStart.g;
        primColor->b = this->primColorStart.b;
        primColor->a = this->primColorStart.a;
        envColor->r = this->envColorStart.r;
        envColor->g = this->envColorStart.g;
        envColor->b = this->envColorStart.b;
        envColor->a = this->envColorStart.a;
    } else if (this->timer < halfDuration) {
        f32 ratio = this->timer / (f32)halfDuration;

        primColor->r = LERPIMP(this->primColorStart.r, this->primColorMid.r, ratio);
        primColor->g = LERPIMP(this->primColorStart.g, this->primColorMid.g, ratio);
        primColor->b = LERPIMP(this->primColorStart.b, this->primColorMid.b, ratio);
        primColor->a = LERPIMP(this->primColorStart.a, this->primColorMid.a, ratio);
        envColor->r = LERPIMP(this->envColorStart.r, this->envColorMid.r, ratio);
        envColor->g = LERPIMP(this->envColorStart.g, this->envColorMid.g, ratio);
        envColor->b = LERPIMP(this->envColorStart.b, this->envColorMid.b, ratio);
        envColor->a = LERPIMP(this->envColorStart.a, this->envColorMid.a, ratio);
    } else {
        f32 ratio = (this->timer - halfDuration) / (f32)halfDuration;

        primColor->r = LERPIMP(this->primColorMid.r, this->primColorEnd.r, ratio);
        primColor->g = LERPIMP(this->primColorMid.g, this->primColorEnd.g, ratio);
        primColor->b = LERPIMP(this->primColorMid.b, this->primColorEnd.b, ratio);
        primColor->a = LERPIMP(this->primColorMid.a, this->primColorEnd.a, ratio);
        envColor->r = LERPIMP(this->envColorMid.r, this->envColorEnd.r, ratio);
        envColor->g = LERPIMP(this->envColorMid.g, this->envColorEnd.g, ratio);
        envColor->b = LERPIMP(this->envColorMid.b, this->envColorEnd.b, ratio);
        envColor->a = LERPIMP(this->envColorMid.a, this->envColorEnd.a, ratio);
    }
}

void EffectShieldParticle_Draw(void* thisx, GraphicsContext* gfxCtx) {
    EffectShieldParticle* this = (EffectShieldParticle*)thisx;
    EffectShieldParticleElement* elem;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    FrameInterpolation_RecordOpenChild(this, 0);

    OPEN_DISPS(gfxCtx);

    if (this != NULL) {
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_38);

        gDPSetCycleType(POLY_XLU_DISP++, G_CYC_2CYCLE);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

        gDPLoadTextureBlock(POLY_XLU_DISP++, gameplay_keep_Tex_054F20, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetCombineLERP(POLY_XLU_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, 0, 0,
                          0, COMBINED, 0, 0, 0, COMBINED);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
        gSPSetGeometryMode(POLY_XLU_DISP++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);

        EffectShieldParticle_GetColors(this, &primColor, &envColor);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, primColor.r, primColor.g, primColor.b, primColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, envColor.a);
        gDPPipeSync(POLY_XLU_DISP++);

        for (elem = &this->elements[0]; elem < &this->elements[this->numElements]; elem++) {
            Mtx* mtx;
            MtxF sp104;
            MtxF spC4;
            MtxF sp84;
            f32 temp1 = (elem->endX + elem->startX) / 2.0f;
            f32 temp2 = elem->endX - elem->startX;
            f32 temp3 = (temp2 * (1.0f / 64.0f)) / 0.02f;

            if (temp3 < 1.0f) {
                temp3 = 1.0f;
            }

            SkinMatrix_SetTranslate(&spC4, this->position.x, this->position.y, this->position.z);
            SkinMatrix_SetRotateRPY(&sp104, 0, elem->yaw, MTXMODE_NEW);
            SkinMatrix_MtxFMtxFMult(&spC4, &sp104, &sp84);
            SkinMatrix_SetRotateRPY(&sp104, 0, 0, elem->pitch);
            SkinMatrix_MtxFMtxFMult(&sp84, &sp104, &spC4);
            SkinMatrix_SetTranslate(&sp104, temp1, 0.0f, 0.0f);
            SkinMatrix_MtxFMtxFMult(&spC4, &sp104, &sp84);
            SkinMatrix_SetScale(&sp104, temp3 * 0.02f, 0.02f, 0.02f);
            SkinMatrix_MtxFMtxFMult(&sp84, &sp104, &spC4);

            mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &spC4);
            if (mtx == NULL) {
                break;
            }

            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPVertex(POLY_XLU_DISP++, sEffShieldParticleVtx, 4, 0);
            gSP2Triangles(POLY_XLU_DISP++, 0, 1, 2, 0, 0, 3, 1, 0);
        }
    }

    CLOSE_DISPS(gfxCtx);
    FrameInterpolation_RecordCloseChild();
}
