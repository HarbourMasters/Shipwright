#include <string.h>
#include "z64transition.h"

#include "gfx.h"
#include "regs.h"
#include "z64math.h"
#include "z64save.h"
#include "macros.h"

typedef enum TransitionFadeDirection {
    /* 0 */ TRANS_FADE_DIR_IN,
    /* 1 */ TRANS_FADE_DIR_OUT
} TransitionFadeDirection;

typedef enum TransitionFadeType {
    /* 0 */ TRANS_FADE_TYPE_NONE,
    /* 1 */ TRANS_FADE_TYPE_ONE_WAY,
    /* 2 */ TRANS_FADE_TYPE_FLASH
} TransitionFadeType;

static Gfx sTransFadeSetupDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_BAYER | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE),
    gsSPEndDisplayList(),
};

TransitionProfile TransitionFade_Profile = {
    TransitionFade_Init,   TransitionFade_Destroy, TransitionFade_Update,   TransitionFade_Draw,
    TransitionFade_Start,  TransitionFade_SetType, TransitionFade_SetColor, NULL,
    TransitionFade_IsDone,
};

void TransitionFade_Start(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    switch (this->type) {
        case TRANS_FADE_TYPE_NONE:
            break;

        case TRANS_FADE_TYPE_ONE_WAY:
            this->timer = 0;
            this->color.a = (this->direction != TRANS_FADE_DIR_IN) ? 255 : 0;
            break;

        case TRANS_FADE_TYPE_FLASH:
            this->color.a = 0;
            break;

        default:
            break;
    }
    this->isDone = false;
}

void* TransitionFade_Init(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    memset(this, 0, sizeof(TransitionFade));
    return this;
}

void TransitionFade_Destroy(void* thisx) {
}

void TransitionFade_Update(void* thisx, s32 updateRate) {
    s32 alpha;
    s16 newAlpha;
    TransitionFade* this = (TransitionFade*)thisx;

    switch (this->type) {
        case TRANS_FADE_TYPE_NONE:
            break;

        case TRANS_FADE_TYPE_ONE_WAY:
            //! FAKE:
            ((TransitionFade*)thisx)->timer += updateRate;

            if (this->timer >= ((void)0, gSaveContext.transFadeDuration)) {
                this->timer = ((void)0, gSaveContext.transFadeDuration);
                this->isDone = true;
            }

            alpha = (255.0f * this->timer) / ((void)0, gSaveContext.transFadeDuration);
            this->color.a = (this->direction != TRANS_FADE_DIR_IN) ? 255 - alpha : alpha;
            break;

        case TRANS_FADE_TYPE_FLASH:
            newAlpha = this->color.a;
            if (R_TRANS_FADE_FLASH_ALPHA_STEP != 0) {
                if (R_TRANS_FADE_FLASH_ALPHA_STEP < 0) {
                    if (Math_StepToS(&newAlpha, 255, 255)) {
                        R_TRANS_FADE_FLASH_ALPHA_STEP = 150;
                    }
                } else {
                    Math_StepToS(&R_TRANS_FADE_FLASH_ALPHA_STEP, 20, 60);
                    if (Math_StepToS(&newAlpha, 0, R_TRANS_FADE_FLASH_ALPHA_STEP)) {
                        R_TRANS_FADE_FLASH_ALPHA_STEP = 0;
                        this->isDone = true;
                    }
                }
            }
            this->color.a = newAlpha;
            break;

        default:
            break;
    }
}

void TransitionFade_Draw(void* thisx, Gfx** gfxP) {
    TransitionFade* this = (TransitionFade*)thisx;
    Gfx* gfx;
    Color_RGBA8_u32* color = &this->color;

    if (color->a != 0) {
        gfx = *gfxP;
        gSPDisplayList(gfx++, sTransFadeSetupDL);
        gDPSetPrimColor(gfx++, 0, 0, color->r, color->g, color->b, color->a);
        gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(fillRect));
        *gfxP = gfx;
    }
}

s32 TransitionFade_IsDone(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    return this->isDone;
}

void TransitionFade_SetColor(void* thisx, u32 color) {
    TransitionFade* this = (TransitionFade*)thisx;

    this->color.rgba = color;
}

void TransitionFade_SetType(void* thisx, s32 type) {
    TransitionFade* this = (TransitionFade*)thisx;

    if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->type = TRANS_FADE_TYPE_ONE_WAY;
        this->direction = TRANS_FADE_DIR_OUT;
    } else if (type == TRANS_INSTANCE_TYPE_FILL_IN) {
        this->type = TRANS_FADE_TYPE_ONE_WAY;
        this->direction = TRANS_FADE_DIR_IN;
    } else if (type == TRANS_INSTANCE_TYPE_FADE_FLASH) {
        this->type = TRANS_FADE_TYPE_FLASH;
    } else {
        this->type = TRANS_FADE_TYPE_NONE;
    }
}
