/**
 * @file z_fbdemo_wipe4.c
 * Overlay: ovl_fbdemo_wipe4
 * Description:
 *
 * @note The bug in TransitionWipe4_Draw() makes this transition do nothing apart from increase its timer until it is
 * finished.
 */

#include "global.h"
#include "z_fbdemo_wipe4.h"
#include "sys_cfb.h"
#include <string.h>

#define TRANS4_GET_COLORTYPE(type) (((type) >> 1) & 3)
#define TRANS4_GET_SPEEDTYPE(type) ((type)&1)

typedef enum TransitionWipe4Direction {
    /* 0 */ TRANS_WIPE4_DIR_IN,
    /* 1 */ TRANS_WIPE4_DIR_OUT
} TransitionWipe4Direction;

void* TransitionWipe4_Init(void* thisx);
void TransitionWipe4_Destroy(void* thisx);
void TransitionWipe4_Update(void* thisx, s32 updateRate);
void TransitionWipe4_Draw(void* thisx, Gfx** gfxP);
void TransitionWipe4_Start(void* thisx);
void TransitionWipe4_SetType(void* thisx, s32 type);
void TransitionWipe4_SetColor(void* thisx, u32 color);
void TransitionWipe4_SetEnvColor(void* thisx, u32 color);
s32 TransitionWipe4_IsDone(void* thisx);

TransitionProfile TransitionWipe4_Profile = {
    TransitionWipe4_Init,     TransitionWipe4_Destroy,     TransitionWipe4_Update,
    TransitionWipe4_Draw,     TransitionWipe4_Start,       TransitionWipe4_SetType,
    TransitionWipe4_SetColor, TransitionWipe4_SetEnvColor, TransitionWipe4_IsDone,
};

void TransitionWipe4_Start(void* thisx) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    this->isDone = false;

    switch (this->speedType) {
        default:
            this->baseSpeed = 0.2f;
            break;

        case 0:
            this->baseSpeed = 0.1f;
            break;

        case 1:
            this->baseSpeed = 0.05f;
            break;
    }

    switch (this->colorType) {
        default:
            this->primColor.rgba = RGBA8(160, 160, 160, 255);
            break;

        case 0:
            this->primColor.rgba = RGBA8(0, 0, 0, 255);
            break;

        case 1:
            this->primColor.rgba = RGBA8(160, 160, 160, 255);
            break;
    }
}

void* TransitionWipe4_Init(void* thisx) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    memset(this, 0, sizeof(TransitionWipe4));

    return this;
}

void TransitionWipe4_Destroy(void* thisx) {
}

void TransitionWipe4_Update(void* thisx, s32 updateRate) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    this->progress += (this->baseSpeed * 3.0f) / updateRate;
    if (this->progress >= 1.0f) {
        this->progress = 1.0f;
        this->isDone = true;
    }
}

// Use of ((TransitionWipe4*)thisx) in this function is required to match
void TransitionWipe4_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    VisFbuf* copyFx = &((TransitionWipe4*)thisx)->copyFx;

    copyFx->primColor.rgba = ((TransitionWipe4*)thisx)->primColor.rgba;

    if (((TransitionWipe4*)thisx)->direction != 0) {
        copyFx->scale = ((TransitionWipe4*)thisx)->progress;
        copyFx->lodProportion = 1.0f - ((TransitionWipe4*)thisx)->progress;
    } else {
        copyFx->scale = 1.0f - ((TransitionWipe4*)thisx)->progress;
        copyFx->lodProportion = ((TransitionWipe4*)thisx)->progress;
    }

    //! @bug (Possibly) Since copyFx->mode is never set after being initialised to 0, the switch in VisFbuf_Draw()
    //! does nothing, so this function call does nothing but change to the sprite microcode, then back to 3D microcode.
    VisFbuf_Draw(copyFx, &gfx, SysCfb_GetZBuffer());
    *gfxP = gfx;
}

s32 TransitionWipe4_IsDone(void* thisx) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    return this->isDone;
}

void TransitionWipe4_SetType(void* thisx, s32 type) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    if (type & TRANS_TYPE_SET_PARAMS) {
        this->colorType = TRANS4_GET_COLORTYPE(type);
        this->speedType = TRANS4_GET_SPEEDTYPE(type);
    } else if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->direction = TRANS_WIPE4_DIR_OUT;
    } else {
        this->direction = TRANS_WIPE4_DIR_IN;
    }
}

void TransitionWipe4_SetColor(void* thisx, u32 color) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    this->primColor.rgba = color;
}

void TransitionWipe4_SetEnvColor(void* thisx, u32 color) {
    TransitionWipe4* this = (TransitionWipe4*)thisx;

    this->envColor.rgba = color;
}
