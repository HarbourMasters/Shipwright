/*
 * File: z_fbdemo_wipe5.c
 * Overlay: ovl_fbdemo_wipe5
 * Description:
 */

#include "global.h"
#include "PR/gs2dex.h"
#include "sys_cfb.h"
#include "z_fbdemo_wipe5.h"
#include <string.h>
#include <libultraship/bridge/gfxbridge.h>

void* TransitionWipe5_Init(void* thisx);
void TransitionWipe5_Destroy(void* thisx);
void TransitionWipe5_Update(void* thisx, s32 updateRate);
void TransitionWipe5_Draw(void* thisx, Gfx** gfxP);
void TransitionWipe5_Start(void* thisx);
void TransitionWipe5_SetType(void* thisx, s32 type);
void TransitionWipe5_SetColor(void* thisx, u32 color);
void TransitionWipe5_SetEnvColor(void* thisx, u32 color);
s32 TransitionWipe5_IsDone(void* thisx);

TransitionProfile TransitionWipe5_Profile = {
    TransitionWipe5_Init,     TransitionWipe5_Destroy,     TransitionWipe5_Update,
    TransitionWipe5_Draw,     TransitionWipe5_Start,       TransitionWipe5_SetType,
    TransitionWipe5_SetColor, TransitionWipe5_SetEnvColor, TransitionWipe5_IsDone,
};

void TransitionWipe5_Start(void* thisx) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    this->isDone = false;
    switch (this->unk_12) {
        case 0:
            this->unk_08 = 0.02f;
            break;
        case 1:
            this->unk_08 = 0.05f;
            break;
        default:
            this->unk_08 = 0.2f;
            break;
    }
    switch (this->unk_11) {
        case 0:
            this->primColor.rgba = RGBA8(0, 0, 0, 255);
            break;
        case 1:
            this->primColor.rgba = RGBA8(160, 160, 160, 255);
            break;
        default:
            this->primColor.rgba = RGBA8(160, 160, 160, 255);
            break;
    }
}

void* TransitionWipe5_Init(void* thisx) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    memset(this, 0, sizeof(TransitionWipe5));
    return this;
}

void TransitionWipe5_Destroy(void* thisx) {
}

void TransitionWipe5_Update(void* thisx, s32 updateRate) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    if (this->unk_10 == 0) {
        this->isDone = true;
    } else {
        this->unk_0C += (this->unk_08 * 3.0f) / updateRate;
        if (this->unk_0C >= 1.0f) {
            this->unk_0C = 1.0f;
            this->isDone = true;
        }
    }
}

void TransitionWipe5_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    s32 width = gScreenWidth;
    s32 height = gScreenHeight;
    void* workBuffer = gWorkBuffer;
    TransitionWipe5* this = (TransitionWipe5*)thisx;
    s32 alpha = (1.0f - this->unk_0C) * 255.0f;

    gDPPipeSync(gfx++);
    gSPLoadUcodeL(gfx++, ucode_s2dex);
    if (this->unk_10 == 0) {
        gDPSetOtherMode(gfx++,
                        G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_COPY | G_PM_NPRIMITIVE,
                        G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);
        VisFbuf_SetBgSimple(&gfx, D_0F000000_TO_SEGMENTED, workBuffer, width, height, VIS_FBUF_BG_CYC_COPY);
    } else {
        if (alpha == 255) {
            gDPSetOtherMode(gfx++,
                            G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_OPA_SURF | G_RM_OPA_SURF2);
        } else {
            gDPSetOtherMode(gfx++,
                            G_AD_NOISE | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PRIM | G_RM_CLD_SURF | G_RM_CLD_SURF2);
        }
        gDPSetEnvColor(gfx++, 255, 255, 255, alpha);
        gDPSetCombineLERP(gfx++, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0,
                          ENVIRONMENT);
        VisFbuf_SetBgSimple(&gfx, workBuffer, D_0F000000_TO_SEGMENTED, width, height, VIS_FBUF_BG_CYC_1CYC);
    }
    gDPPipeSync(gfx++);
    gSPLoadUcode(gfx++, SysUcode_GetUCode());
    *gfxP = gfx;
}

s32 TransitionWipe5_IsDone(void* thisx) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    return this->isDone;
}

void TransitionWipe5_SetType(void* thisx, s32 type) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    if (type & 0x80) {
        this->unk_11 = 0;
        this->unk_12 = type & 1;
    } else if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->unk_10 = 1;
    } else {
        this->unk_10 = 0;
    }
}

void TransitionWipe5_SetColor(void* thisx, u32 color) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    this->primColor.rgba = color;
}

void TransitionWipe5_SetEnvColor(void* thisx, u32 color) {
    TransitionWipe5* this = (TransitionWipe5*)thisx;

    this->envColor.rgba = color;
}
