/*
 * File: z_fbdemo_wipe3.c
 * Overlay: ovl_fbdemo_wipe3
 * Description:
 */

#include "global.h"
#include "z_fbdemo_wipe3.h"
#include <string.h>
#include "BenPort.h"

void* TransitionWipe3_Init(void* thisx);
void TransitionWipe3_Destroy(void* thisx);
void TransitionWipe3_Update(void* thisx, s32 updateRate);
void TransitionWipe3_Draw(void* thisx, Gfx** gfxP);
void TransitionWipe3_Start(void* thisx);
void TransitionWipe3_SetType(void* thisx, s32 type);
void TransitionWipe3_SetColor(void* thisx, u32 color);
void TransitionWipe3_SetEnvColor(void* thisx, u32 color);
s32 TransitionWipe3_IsDone(void* thisx);

#include "assets/overlays/ovl_fbdemo_wipe3/ovl_fbdemo_wipe3.h"

TexturePtr sTransWipe3Textures[] = {
    fbdemo_tex_000520, fbdemo_tex_000920, fbdemo_tex_000D20, fbdemo_tex_001120,
    fbdemo_tex_001520, fbdemo_tex_000520, fbdemo_tex_000520, fbdemo_tex_000520,
};

TransitionProfile TransitionWipe3_Profile = {
    TransitionWipe3_Init,     TransitionWipe3_Destroy,     TransitionWipe3_Update,
    TransitionWipe3_Draw,     TransitionWipe3_Start,       TransitionWipe3_SetType,
    TransitionWipe3_SetColor, TransitionWipe3_SetEnvColor, TransitionWipe3_IsDone,
};

#define TRANS3_GET_SPEED(type) ((type)&1)
#define TRANS3_GET_TEX_INDEX(type) (((type) >> 1) & 7)
#define TRANS3_GET_COLOR(type) (((type) >> 4) & 3)

typedef enum {
    /* 0 */ TRANS_WIPE3_DIR_IN,
    /* 1 */ TRANS_WIPE3_DIR_OUT
} TransitionWipe3Direction;

typedef enum {
    /* 0 */ TRANS_WIPE3_SPEED_SLOW,
    /* 1 */ TRANS_WIPE3_SPEED_FAST
} TransitionWipe3Speed;

typedef enum {
    /* 0 */ TRANS_WIPE3_COLOR_BLACK,
    /* 1 */ TRANS_WIPE3_COLOR_WHITE,
    /* 2 */ TRANS_WIPE3_COLOR_GRAY
} TransitionWipe3Color;

void TransitionWipe3_Start(void* thisx) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    this->isDone = false;
    this->curTexture = sTransWipe3Textures[this->texIndex % ARRAY_COUNTU(sTransWipe3Textures)];
    if (this->speedType == TRANS_WIPE3_SPEED_SLOW) {
        this->wipeSpeed = 20;
    } else {
        this->wipeSpeed = 10;
    }
    if (this->colorType == TRANS_WIPE3_COLOR_BLACK) {
        this->color.rgba = RGBA8(0, 0, 0, 255);
    } else if (this->colorType == TRANS_WIPE3_COLOR_WHITE) {
        this->color.rgba = RGBA8(255, 255, 255, 255);
    } else {
        this->color.rgba = RGBA8(160, 160, 160, 255);
    }
    if (this->dir != TRANS_WIPE3_DIR_IN) {
        this->scrollY = 0;
    } else {
        this->scrollY = 500;
        if (this->texIndex == 2) {
            Audio_PlaySfx_2(NA_SE_OC_SECRET_WARP_OUT);
        }
    }
    guPerspective(&this->projection, &this->normal, 60.0f, 4.0f / 3.0f, 10.0f, 12800.0f, 1.0f);
    guLookAt(&this->lookAt, 0.0f, 0.0f, 400.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void* TransitionWipe3_Init(void* thisx) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    memset(this, 0, sizeof(TransitionWipe3));
    return this;
}

void TransitionWipe3_Destroy(void* thisx) {
}

void TransitionWipe3_UpdateScrollY(void* thisx, f32 scroll) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    this->scrollY = (s32)(500.0f * scroll);
}

void TransitionWipe3_Update(void* thisx, s32 updateRate) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    if (this->dir != TRANS_WIPE3_DIR_IN) {
        if ((this->scrollY == 0) && (this->texIndex == 2)) {
            Audio_PlaySfx_2(NA_SE_OC_SECRET_WARP_IN);
        }
        this->scrollY += (this->wipeSpeed * 3) / updateRate;
        if (this->scrollY >= 500) {
            this->scrollY = 500;
            this->isDone = true;
        }
    } else {
        this->scrollY -= (this->wipeSpeed * 3) / updateRate;
        if (this->scrollY <= 0) {
            this->scrollY = 0;
            this->isDone = true;
        }
    }
}

void TransitionWipe3_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView = &((TransitionWipe3*)thisx)->modelView[((TransitionWipe3*)thisx)->frame];
    f32 scale = 14.8f;
    Gfx* texScroll;

    // #region 2S2H [Widescreen] Ocarina Effects
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        scale = 14.8f * (OTRGetAspectRatio() * 0.94f); // Widescreen value
    }
    // #endregion

    ((TransitionWipe3*)thisx)->frame ^= 1;
    gDPPipeSync(gfx++);
    texScroll =
        Gfx_BranchTexScroll(&gfx, ((TransitionWipe3*)thisx)->scrollX, ((TransitionWipe3*)thisx)->scrollY, 16, 64);
    gSPSegment(gfx++, 0x09, texScroll);
    gSPSegment(gfx++, 0x08, ((TransitionWipe3*)thisx)->curTexture);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, ((TransitionWipe3*)thisx)->color.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, ((TransitionWipe3*)thisx)->color.rgba);
    gSPMatrix(gfx++, &((TransitionWipe3*)thisx)->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(gfx++, ((TransitionWipe3*)thisx)->normal);
    gSPMatrix(gfx++, &((TransitionWipe3*)thisx)->lookAt, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    if (scale != 1.0f) {
        guScale(modelView, scale, scale, 1.0f);
        gSPMatrix(gfx++, modelView, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }
    gSPDisplayList(gfx++, sTransWipe3DL);
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 TransitionWipe3_IsDone(void* thisx) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    return this->isDone;
}

void TransitionWipe3_SetType(void* thisx, s32 type) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    if (type & TRANS_TYPE_SET_PARAMS) {
        this->speedType = TRANS3_GET_SPEED(type);
        this->texIndex = TRANS3_GET_TEX_INDEX(type);
        this->colorType = TRANS3_GET_COLOR(type);
    } else if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->dir = TRANS_WIPE3_DIR_OUT;
    } else {
        this->dir = TRANS_WIPE3_DIR_IN;
    }
}

void TransitionWipe3_SetColor(void* thisx, u32 color) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    this->color.rgba = color;
}

void TransitionWipe3_SetEnvColor(void* thisx, u32 color) {
    TransitionWipe3* this = (TransitionWipe3*)thisx;

    this->envColor.rgba = color;
}
