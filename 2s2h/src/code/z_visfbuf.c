/**
 * @file z_visfbuf.c
 * @brief Copies images between color images (generally framebuffers), possibly with filling, scaling, and colour
 * interpolation.
 *
 * Used for several transition effects, and in z_play to shrink the screen at the end of the First and Second Days.
 *
 * @note to use the functions in this file, with the exception of VisFbuf_Draw(), it is necessary to load the S2DEX2
 * microcode first, and then re-load the 3D microcode afterwards for the rest of the drawing in the frame; see
 * VisFbuf_Draw() for how to do this.
 */

#include "global.h"
#include "z64visfbuf.h"
#include "sys_cfb.h"
#include <string.h>
#include "2s2h/framebuffer_effects.h"
#include <libultraship/bridge/gfxbridge.h>
#include "BenPort.h"

#define SCALE_MIN 0.032f
#define SCALE_MAX 1.0f //!< also unchanged scale

void VisFbuf_Init(VisFbuf* this) {
    memset(this, 0, sizeof(VisFbuf));
}

void VisFbuf_Destroy(VisFbuf* this) {
}

// internal, only used in VisFbuf_SetBg
/**
 * Draw a bg to the specified color image.
 *
 * @param[in,out] gfxP      Pointer to current displaylist
 * @param[in]     bg        BG object to draw
 * @param[out]    img       Pointer to beginning of destination color image
 * @param[in]     width     Output width in pixels
 * @param[in]     height    Output height in pixels
 * @param[in]     cycleMode Which BG mode to use
 */
void VisFbuf_DrawBgToColorImage(Gfx** gfxP, uObjBg* bg, void* img, s32 width, s32 height, VisFbufBgMode cycleMode) {
    Gfx* gfx = *gfxP;

    gDPPipeSync(gfx++);
    // Set up color image to draw bg to
    gDPSetColorImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, img);
    gDPSetScissor(gfx++, 0, 0, 0, width, height);

    // Draw bg in appropriate type
    gSPObjRenderMode(gfx++, G_OBJRM_ANTIALIAS | G_OBJRM_BILERP);
    if (!!(cycleMode & VIS_FBUF_BG_CYC_COPY) != 0) { //! FAKE: may possibly be a better way
        gSPBgRectCopy(gfx++, bg);
    } else {
        gSPBgRect1Cyc(gfx++, bg);
    }

    gDPPipeSync(gfx++);
    // Reset the color image and scissor to frame's defaults
    gDPSetColorImage(gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gCfbWidth, D_0F000000_TO_SEGMENTED);
    gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(setScissor));

    *gfxP = gfx;
}

// internal
/**
 * Set up a BG from a specified source image and draw it to the specified color image with
 * VisFbuf_DrawBgToColorImage(), using the BG's settings. Use VisFbuf_SetBgSimple() or
 * VisFbuf_SetBgGeneral()
 *
 * @param[out] gfxP     Pointer to current displaylist
 * @param[in] source    Pointer to beginning of source color image
 * @param[out] img      Pointer to beginning of destination color image
 * @param[in] width     Output width in pixels
 * @param[in] height    Output height in pixels
 * @param[in] x         Left of drawn output
 * @param[in] y         Top of drawn output
 * @param[in] scaleX    Amount to rescale the image, (dsdx). No effect if `cycleMode` is `VIS_FBUF_BG_CYC_COPY`.
 * @param[in] scaleY    Amount to rescale the image, (dtdy). No effect if `cycleMode` is `VIS_FBUF_BG_CYC_COPY`.
 * @param[in] cycleMode Which BG mode to use
 */
void VisFbuf_SetBg(Gfx** gfxP, void* source, void* img, s32 width, s32 height, f32 x, f32 y, f32 scaleX, f32 scaleY,
                   VisFbufBgMode cycleMode) {
    Gfx* gfx = *gfxP;
    Gfx* gfxTemp;
    uObjBg* bg;

    // Allocate for BG
    gfxTemp = gfx;
    bg = Graph_DlistAlloc(&gfxTemp, sizeof(uObjBg));
    gfx = gfxTemp;

    // Set up BG
    bg->b.imageX = 0;
    bg->b.imageW = width << 2;
    bg->b.frameX = x * 4;
    bg->b.frameW = width << 2;
    bg->b.imageY = 0;
    bg->b.imageH = height << 2;
    bg->b.frameY = y * 4;
    bg->b.frameH = height << 2;
    bg->b.imageLoad = G_BGLT_LOADTILE;
    bg->b.imageFmt = G_IM_FMT_RGBA;
    bg->b.imageSiz = G_IM_SIZ_16b;
    bg->b.imagePal = 0;
    bg->b.imageFlip = 0;
    bg->b.imagePtr = source;

    if (!!(cycleMode & VIS_FBUF_BG_CYC_COPY) != 0) { //! FAKE: may possibly be a better way
        guS2DInitBg(bg);
    } else {
        bg->s.scaleW = (s32)((1 << 10) / scaleX);
        bg->s.scaleH = (s32)((1 << 10) / scaleY);
        bg->s.imageYorig = bg->b.imageY;
    }

    // draw BG to `img`
    VisFbuf_DrawBgToColorImage(&gfx, bg, img, width, height, cycleMode);

    *gfxP = gfx;
}

// used in FbdemoWipe5 and internally
// "default settings" wrapper for VisFbuf_SetBg
/**
 * Set up a BG from a specified source image and draw it to the specified color image with
 * VisFbuf_DrawBgToColorImage(), using the BG's settings. Position uses the default (0,0), and no rescaling is done
 * even if 1-cycle mode is enabled.
 *
 * @see VisFbuf_SetBg() for arguments.
 */
void VisFbuf_SetBgSimple(Gfx** gfxP, void* source, void* img, s32 width, s32 height, VisFbufBgMode cycleMode) {
    VisFbuf_SetBg(gfxP, source, img, width, height, 0.0f, 0.0f, SCALE_MAX, SCALE_MAX, cycleMode);
}

// wrapper for VisFbuf_SetBg with general arguments, used in VisFbuf_ApplyEffects
/**
 * Set up a BG from a specified source image and draw it to the specified color image with
 * VisFbuf_DrawBgToColorImage(). Fully general settings are available.
 *
 * @see VisFbuf_SetBg() for arguments.
 */
void VisFbuf_SetBgGeneral(Gfx** gfxP, void* source, void* img, s32 width, s32 height, f32 x, f32 y, f32 scaleX,
                          f32 scaleY, VisFbufBgMode cycleMode) {
    VisFbuf_SetBg(gfxP, source, img, width, height, x, y, scaleX, scaleY, cycleMode);
}

// internal, used in VisFbuf_DrawGeneral
/**
 * Most general redrawing function.
 *
 *  - Copies `source` to `img`,
 *  - fills the current framebuffer with `this->primColor`,
 *  - applies colour interpolation to `img`,
 *  - copies `img` back to `source`, with rescaling.
 *
 * @param[in]     this
 * @param[in,out] gfxP   Pointer to current displaylist
 * @param[in,out] source Pointer to beginning of in/out color image
 * @param[out]    img    Pointer to beginning of destination color image
 * @param[in]     width  Output width in pixels
 * @param[in]     height Output height in pixels
 */
void VisFbuf_ApplyEffects(VisFbuf* this, Gfx** gfxP, void* source, void* img, s32 width, s32 height) {
    Gfx* gfx = *gfxP;
    s32 pad[3];

    gDPPipeSync(gfx++);
    // copy source to img
    gDPSetOtherMode(gfx++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_COPY | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

    // The function FB_CopyToFramebuffer uses f3dex2 opcodes, clear and set geometry ode. We need to load f3dex2, call
    // the function, and then reload s2dex.
    gSPLoadUcode(gfx++, ucode_f3dex2);
    FB_CopyToFramebuffer(&gfx, 0, gReusableFrameBuffer, false, NULL);
    gSPLoadUcode(gfx++, ucode_s2dex);

    gDPPipeSync(gfx++);
    // fill framebuffer with primColor
    gDPSetOtherMode(gfx++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);
    {
        s32 color = GPACK_RGBA5551(this->primColor.r, this->primColor.g, (u32)this->primColor.b, 1);

        gDPSetFillColor(gfx++, (color << 0x10) | color);
    }
    //! @bug VisFbuf_SetBgSimple() sets the current color image back to the frame's default framebuffer at the end,
    //! so this will always fill in the default framebuffer, whatever are used as `source` and `img`. This does not
    //! arise in-game since this function is always used with `source = D_0F000000`.

    gSPLoadUcode(gfx++, ucode_f3dex2);
    gDPFillWideRectangle(gfx++, OTRGetRectDimensionFromLeftEdge(0), 0, OTRGetRectDimensionFromRightEdge(width - 1),
                         height - 1);
    gSPLoadUcode(gfx++, ucode_s2dex);

    gDPPipeSync(gfx++);
    // Set lod and primColor from struct, perform interpolation, draw image with scaling (this is the most general
    // alteration this system can carry out).
    {
        s32 lodFrac = this->lodProportion * 255;

        gDPSetLODColor(gfx++, G_SETPRIMCOLOR, 0, lodFrac, this->primColor.rgba);
    }
    if (this->envColor.a == 0) {
        // Interpolate between primColor and texel color using lod (but with max alpha)
        gDPSetOtherMode(gfx++,
                        G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PIXEL | AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | ALPHA_CVG_SEL |
                            GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_BL, G_BL_1MA) |
                            GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_BL, G_BL_1MA));
        gDPSetCombineLERP(gfx++, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, 1, PRIMITIVE, TEXEL0, PRIM_LOD_FRAC,
                          TEXEL0, 0, 0, 0, 1);
    } else {
        // Interpolate between envColor (the new black) and primColor (the new white)
        gDPSetOtherMode(gfx++,
                        G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_NOOP2);
        gDPSetColor(gfx++, G_SETENVCOLOR, this->envColor.rgba);
        gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                          ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,
                          ENVIRONMENT);
    }

    // Draw scaled image in centre of `width * height` rectangle in `source`
    {
        f32 scale = CLAMP_ALT(this->scale, SCALE_MIN, SCALE_MAX);

        // 2S2H [Port][Widescreen]
        // Draw shrunk window using an adjusted horizontal scale accounting for different aspect ratios
        // The function FB_DrawFromFramebufferScaled uses f3dex2 opcodes. We need to load f3dex2, call the function, and
        // then reload s2dex.

        gSPLoadUcode(gfx++, ucode_f3dex2);
        FB_DrawFromFramebufferScaled(&gfx, gReusableFrameBuffer, 255,
                                     (1.0f - scale) * (OTRGetAspectRatio() / ((float)SCREEN_WIDTH / SCREEN_HEIGHT)),
                                     (1.0f - scale));
        gSPLoadUcode(gfx++, ucode_s2dex);
    }

    gDPPipeSync(gfx++);

    *gfxP = gfx;
}

// internal, used in VisFbuf_Draw, VIS_FBUF_MODE_GENERAL
/**
 * If scale is within `(SCALE_MIN, SCALE_MAX)`, apply VisFbuf_ApplyEffects().
 * If it is smaller than `SCALE_MIN`, fill the framebuffer with `this->primColor`
 *
 * @param[in]     this
 * @param[in,out] gfxP   Pointer to current displaylist
 * @param[in,out] source Pointer to beginning of source color image
 * @param[in,out] img    Pointer to beginning of destination color image
 * @param[in]     width  Output width in pixels
 * @param[in]     height Output height in pixels
 */
void VisFbuf_DrawGeneral(VisFbuf* this, Gfx** gfxP, void* source, void* img, s32 width, s32 height) {
    if (this->scale < SCALE_MAX) {
        Gfx* gfx = *gfxP;
        u32 color;

        if (this->scale > SCALE_MIN) {
            VisFbuf_ApplyEffects(this, &gfx, source, img, width, height);
        } else {
            gDPPipeSync(gfx++);
            // fill the framebuffer with this->primColor
            gDPSetOtherMode(gfx++,
                            G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                                G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                            G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);
            color = GPACK_RGBA5551(this->primColor.r, this->primColor.g, (u32)this->primColor.b, 1);
            gDPSetFillColor(gfx++, (color << 0x10) | color);
            //! @bug See corresponding note in VisFbuf_ApplyEffects()
            gDPFillRectangle(gfx++, 0, 0, width - 1, height - 1);

            gDPPipeSync(gfx++);
        }

        *gfxP = gfx;
    }
}

/**
 * Redraw `img` in-place, using texel color to interpolate between envColor (the new black) and primColor (the new
 * white)
 *
 * @param[in]     this
 * @param[in,out] gfxP   Pointer to current displaylist
 * @param[in,out] img    Pointer to beginning of color image to modify in-place
 * @param[in]     width  width of img
 * @param[in]     height height of img
 */
void VisFbuf_DrawInterpolate(VisFbuf* this, Gfx** gfxP, void* img, s32 width, s32 height) {
    Gfx* gfx = *gfxP;

    gDPPipeSync(gfx++);

    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_NOOP2);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->primColor.rgba);
    gDPSetColor(gfx++, G_SETENVCOLOR, this->envColor.rgba);
    gDPSetCombineLERP(gfx++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT);
    VisFbuf_SetBgSimple(&gfx, img, img, width, height, VIS_FBUF_BG_CYC_1CYC);

    gDPPipeSync(gfx++);

    *gfxP = gfx;
}

// used in Play and FbdemoWipe4
/**
 * main drawing function in this file, draws depending on settings specified in `this`.
 *
 * @param[in]     this
 * @param[in,out] gfxP Pointer to current displaylist
 * @param[in,out] img  Pointer to beginning of destination color image
 */
void VisFbuf_Draw(VisFbuf* this, Gfx** gfxP, void* img) {
    Gfx* gfx = *gfxP;

    gSPLoadUcodeL(gfx++, ucode_s2dex);

    switch (this->mode) {
        case VIS_FBUF_MODE_GENERAL:
            VisFbuf_DrawGeneral(this, &gfx, D_0F000000_TO_SEGMENTED, img, gScreenWidth, gScreenHeight);
            break;

        case VIS_FBUF_MODE_INTERPOLATE:
            VisFbuf_DrawInterpolate(this, &gfx, D_0F000000_TO_SEGMENTED, gScreenWidth, gScreenHeight);
            break;

        default:
            break;
    }

    gSPLoadUcode(gfx++, SysUcode_GetUCode());

    *gfxP = gfx;
}
