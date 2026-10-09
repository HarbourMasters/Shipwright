#include "global.h"
#include "z64viszbuf.h"
#include "sys_cfb.h"
#include "libc/stdbool.h"

#define VISZBUF_ZBUFFRAG_HEIGHT (TMEM_SIZE / (gCfbWidth * G_IM_SIZ_16b_BYTES))

void VisZbuf_Init(VisZbuf* this) {
    this->useRgba = false;
    this->setScissor = false;
    this->primColor.r = 255;
    this->primColor.g = 255;
    this->primColor.b = 255;
    this->primColor.a = 255;
    this->envColor.r = 0;
    this->envColor.g = 0;
    this->envColor.b = 0;
    this->envColor.a = 255;
}

void VisZbuf_Destroy(VisZbuf* this) {
}

void VisZbuf_Draw(VisZbuf* this, Gfx** gfxP, void* zbuffer) {
    Gfx* gfx = *gfxP;
    s32 height = VISZBUF_ZBUFFRAG_HEIGHT;
    s32 y;
    s32 fmt = !this->useRgba ? G_IM_FMT_IA : G_IM_FMT_RGBA;

    if (zbuffer == NULL) {
        return;
    }

    gDPPipeSync(gfx++);
    if (this->setScissor == true) {
        gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(setScissor));
    }

    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_OPA_SURF | G_RM_OPA_SURF2);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, this->envColor.rgba);

    for (y = 0; y < (gCfbHeight - height) + 1; y += height) {
        gDPLoadTextureTile(gfx++, zbuffer, fmt, G_IM_SIZ_16b, gCfbWidth, 0, 0, y, gCfbWidth - 1, (y + height) - 1, 0,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

        gSPTextureRectangle(gfx++, 0, y << 2, gCfbWidth << 2, (y + height) << 2, G_TX_RENDERTILE, 0, y << 5, 1 << 10,
                            1 << 10);
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}
