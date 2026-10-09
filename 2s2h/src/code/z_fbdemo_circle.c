#include "functions.h"
#include "z64transition.h"
#include "code/fbdemo_circle/fbdemo_circle.h"
#include <string.h>

#include "BenPort.h"

#include "main.h"
#include "sys_cfb.h"
#include "z64math.h"

typedef enum TransitionCircleDirection {
    /* 0 */ TRANS_CIRCLE_DIR_IN,
    /* 1 */ TRANS_CIRCLE_DIR_OUT
} TransitionCircleDirection;

Gfx sTransCircleSetupDL[] = {
    gsDPPipeSync(),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_THRESHOLD | G_ZS_PRIM | AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA | FORCE_BL |
                         GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) |
                         GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
    gsDPSetPrimColor(0, 255, 0, 0, 0, 1),
    gsDPSetBlendColor(0, 0, 0, 1),
    gsSPEndDisplayList(),
};

void TransitionCircle_Start(void* thisx);
void* TransitionCircle_Init(void* thisx);
void TransitionCircle_Destroy(void* thisx);
void TransitionCircle_Update(void* thisx, s32 updateRate);
void TransitionCircle_SetColor(void* thisx, u32 color);
void TransitionCircle_SetType(void* thisx, s32 type);
void TransitionCircle_Draw(void* thisx, Gfx** gfxP);
s32 TransitionCircle_IsDone(void* thisx);

TransitionProfile TransitionCircle_Profile = {
    TransitionCircle_Init,   TransitionCircle_Destroy, TransitionCircle_Update,   TransitionCircle_Draw,
    TransitionCircle_Start,  TransitionCircle_SetType, TransitionCircle_SetColor, NULL,
    TransitionCircle_IsDone,
};

void TransitionCircle_Start(void* thisx) {
    TransitionCircle* this = (TransitionCircle*)thisx;

    this->stepValue = 0.1f;
    if (this->direction == TRANS_CIRCLE_DIR_IN) {
        this->targetRadius = 0.0f;
        this->startingRadius = 1.0f;
    } else {
        this->startingRadius = 0.0f;
        this->targetRadius = 1.0f;
    }
    this->referenceRadius = this->startingRadius;
}

void* TransitionCircle_Init(void* thisx) {
    TransitionCircle* this = (TransitionCircle*)thisx;

    memset(this, 0, sizeof(TransitionCircle));
    this->maskType = 1;
    this->texture = gCircleTex;
    this->masks = 6;
    this->maskt = 6;
    this->unk_1E = 4;
    this->unk_1F = 0;

    return this;
}

void TransitionCircle_Destroy(void* thisx) {
}

void TransitionCircle_Update(void* thisx, s32 updateRate) {
    TransitionCircle* this = (TransitionCircle*)thisx;
    s32 unused = updateRate ? 0 : 0;

    this->isDone = Math_StepToF(&this->referenceRadius, this->targetRadius, this->stepValue);
}

void TransitionCircle_SetColor(void* thisx, u32 color) {
    TransitionCircle* this = (TransitionCircle*)thisx;

    this->color.rgba = color;
}

void TransitionCircle_SetType(void* thisx, s32 type) {
    TransitionCircle* this = (TransitionCircle*)thisx;

    if (type & TC_SET_PARAMS) {
        this->maskType = FBDEMO_CIRCLE_GET_MASK_TYPE(type);
    } else if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->direction = TRANS_CIRCLE_DIR_OUT;
    } else {
        this->direction = TRANS_CIRCLE_DIR_IN;
    }
}

void TransitionCircle_LoadAndSetTexture(Gfx** gfxP, void const* texture, s32 fmt, s32 arg3, s32 masks, s32 maskt,
                                        f32 arg6) {
    Gfx* gfx = *gfxP;
    s32 xh = gCfbWidth;
    s32 yh = gCfbHeight;
    s32 width = 1 << masks;
    s32 height = 1 << maskt;
    f32 s;
    f32 t;
    s32 dtdy;
    s32 dsdx;

    gDPLoadTextureBlock_4b(gfx++, texture, fmt, width, height, 0, G_TX_MIRROR | G_TX_CLAMP, G_TX_MIRROR | G_TX_CLAMP,
                           masks, maskt, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetTileSize(gfx++, G_TX_RENDERTILE, ((SCREEN_WIDTH / 2) - width) << 2, ((SCREEN_HEIGHT / 2) - height) << 2,
                   ((SCREEN_WIDTH / 2) + (width - 1)) << 2, ((SCREEN_HEIGHT / 2) + (height - 1)) << 2);

    s = ((1.0f - (1.0f / arg6)) * (SCREEN_WIDTH / 2)) + 70.0f;
    t = ((1.0f - (1.0f / arg6)) * (SCREEN_HEIGHT / 2)) + 50.0f;

    if (s < -1023.0f) {
        s = -1023.0f;
    }
    if (t < -1023.0f) {
        t = -1023.0f;
    }

    if ((s <= -1023.0f) || (t <= -1023.0f)) {
        dsdx = 0;
        dtdy = 0;
    } else {
        dsdx = ((SCREEN_WIDTH - (2.0f * s)) / gScreenWidth) * (1 << 10);
        dtdy = ((SCREEN_HEIGHT - (2.0f * t)) / gScreenHeight) * (1 << 10);
    }

    // #region 2S2H [Cosmetic] Adjust circle overlay to support widescreen
    // The first wide rectangle instruction renders the tile information on the extra space off the left edge
    // The second instruction renders the original overlay in the center and has it extended to the right edge
    s32 x = OTRGetRectDimensionFromLeftEdge(0) << 2;
    if (x < 0) {
        // Only render if the screen is wider then original
        gSPWideTextureRectangle(gfx++, x, 0, 0, yh << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
    }
    gSPWideTextureRectangle(gfx++, 0, 0, OTRGetRectDimensionFromRightEdge(xh) << 2, yh << 2, G_TX_RENDERTILE,
                            (s32)(s * (1 << 5)), (s32)(t * (1 << 5)), dsdx, dtdy);
    // #endregion
    gDPPipeSync(gfx++);

    *gfxP = gfx;
}

void TransitionCircle_Draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    TransitionCircle* this = (TransitionCircle*)thisx;

    gDPPipeSync(gfx++);
    gSPDisplayList(gfx++, sTransCircleSetupDL);
    gDPSetPrimColor(gfx++, 0, this->color.a, this->color.r, this->color.g, this->color.b, 1);
    if (this->maskType == 0) {
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIM_LOD_FRAC, PRIMITIVE, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIM_LOD_FRAC, PRIMITIVE);
    } else {
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, 1, TEXEL0, PRIM_LOD_FRAC, PRIMITIVE, 0, 0, 0, PRIMITIVE, 1, TEXEL0,
                          PRIM_LOD_FRAC, PRIMITIVE);
    }
    // 2S2H [Port] We need to set the render mode to XLU_SURF for alpha logic to be used in Fast3D
    // If Fast3D changes how it decides alpha in the future, we may be able to remove this line
    gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    TransitionCircle_LoadAndSetTexture(&gfx, this->texture, G_IM_FMT_I, 0, this->masks, this->maskt,
                                       this->referenceRadius);
    gDPPipeSync(gfx++);

    *gfxP = gfx;
}

s32 TransitionCircle_IsDone(void* thisx) {
    TransitionCircle* this = (TransitionCircle*)thisx;

    return this->isDone;
}
