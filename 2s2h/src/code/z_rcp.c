#include "global.h"
#include "sys_cfb.h"

#include "BenPort.h"

Gfx gSetupDLs[SETUPDL_MAX][6] = {
    {
        /* SETUPDL_0 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, 0, 0, 0, COMBINED, 0,
                           0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_NOISE | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_1 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_2 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_3 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_4 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_5 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_6 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_TEX_EDGE2),
        gsSPLoadGeometryMode(G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_7 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_8 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_9 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_10 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_11 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_12 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_13 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_14 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_15 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_16 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_17 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_18 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_19 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | AA_EN | Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_20 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_21 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_22 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_23 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_24 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_25 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_26 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_27 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_28 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_RGBA16 | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_AA_TEX_EDGE | G_RM_AA_TEX_EDGE2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_29 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, SHADE, 0, NOISE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_30 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_LINE | G_RM_AA_ZB_XLU_LINE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_31 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_HILITERGBA, G_CC_HILITERGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_32 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_HILITERGBA, G_CC_HILITERGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_33 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, SHADE, 1, 0, TEXEL0, SHADE, ENVIRONMENT, COMBINED, TEXEL0, COMBINED,
                           ENVIRONMENT, COMBINED, TEXEL0, COMBINED),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_34 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_35 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_DECAL | G_RM_AA_ZB_XLU_DECAL2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_36 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_37 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_38 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_39 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_40 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, PRIMITIVE_ALPHA, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0, 0, 0, 0,
                           COMBINED, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_FRONT | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_41 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_XLU_SURF | G_RM_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_42 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_43 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_RGBA16 | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_44 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_ZB_OVL_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_45 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_46 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_47 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_XLU_SURF | G_RM_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_48 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_49 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_50 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_51 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_52 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_53 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_54 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_55 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_56 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_57 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_58 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE,
                           0),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_59 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, SHADE, 1, 0, TEXEL0, SHADE, ENVIRONMENT, COMBINED, TEXEL0, COMBINED,
                           ENVIRONMENT, COMBINED, TEXEL0, COMBINED),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_60 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, ENVIRONMENT,
                           TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_61 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, ENVIRONMENT,
                           TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0),
        gsDPSetOtherMode(G_AD_NOISE | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_62 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_63 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_UPD | IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL |
                             GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_64 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_65 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_66 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_67 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_68 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_69 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_70 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_71 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_BAYER | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_72 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_PATTERN | G_CD_BAYER | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
};

Gfx sFillSetupDL[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
    gsSPDisplayList(D_0E000000_TO_SEGMENTED(setScissor)),
    gsDPSetBlendColor(0x00, 0x00, 0x00, 0x08),
    gsSPClipRatio(FRUSTRATIO_2),
    gsSPEndDisplayList(),
};

Gfx gEmptyDL[] = {
    gsSPEndDisplayList(),
};

Gfx* Gfx_SetFog(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 n, s32 f) {
    gDPSetFogColor(gfx, r, g, b, a);

    if (n >= 997) {
        if (n >= 1000) {
            gSPFogFactor(&gfx[1], 0, 0); // gSPFogPosition(gfx++, ?, ?)
        } else {
            gSPFogFactor(&gfx[1], 0x7FFF, 0x8100); // gSPFogPosition(gfx++, ?, ?)
        }
    } else {
        if (n < 0) {
            gSPFogFactor(&gfx[1], 0, 255); // gSPFogPosition(gfx++, ?, ?)
        } else {
            gSPFogPosition(&gfx[1], n, f);
        }
    }

    return &gfx[2];
}

Gfx* Gfx_SetFogWithSync(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 n, s32 f) {
    gDPPipeSync(gfx++);
    gDPSetFogColor(gfx++, r, g, b, a);

    if (n >= 1000) {
        gSPFogFactor(gfx++, 0, 0); // gSPFogPosition(gfx++, ?, ?)
    } else if (n >= 997) {
        gSPFogFactor(gfx++, 0x7FFF, 0x8100); // gSPFogPosition(gfx++, ?, ?)
    } else if (n < 0) {
        gSPFogFactor(gfx++, 0, 255); // gSPFogPosition(gfx++, ?, ?)
    } else {
        gSPFogPosition(gfx++, n, f);
    }

    return gfx;
}

Gfx* Gfx_SetFog2(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 n, s32 f) {
    return Gfx_SetFog(gfx, r, g, b, a, n, f);
}

Gfx* Gfx_SetupDLImpl(Gfx* gfx, u32 i) {
    s32 dListIndex = i * ARRAY_COUNT(gSetupDLs[i]);

    // Equivalent to gSPDisplayList(gfx++, gSetupDLs[i])
    gSPDisplayList(gfx++, &((Gfx*)gSetupDLs)[dListIndex]);
    return gfx;
}

Gfx* Gfx_SetupDL(Gfx* gfx, u32 i) {
    return Gfx_SetupDLImpl(gfx, i);
}

void Gfx_SetupDLAtPtr(Gfx** gfxP, u32 i) {
    *gfxP = Gfx_SetupDL(*gfxP, i);
}

Gfx* Gfx_SetupDL57(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_57]);
    return gfx;
}

Gfx* Gfx_SetupDL57_b(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_57]);
    return gfx;
}

Gfx* Gfx_SetupDL52_NoCD(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_52]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

void Gfx_SetupDL58_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_58]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL57_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_57]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL50_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_50]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL51_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_51]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL52_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_52]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL53_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_53]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL54_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_54]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL55_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_55]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL26(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_26]);
    return gfx;
}

void Gfx_SetupDL26_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL26(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL23(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_23]);
    return gfx;
}

void Gfx_SetupDL23_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL23(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL25_Xlu2(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx);
}

void func_8012C268(GameState* gameState) {
    Gfx_SetupDL25_Opa(gameState->gfxCtx);
}

void Gfx_SetupDL25_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL71(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_71]);
    return gfx;
}

void Gfx_SetupDL25_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL72(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_72]);
    return gfx;
}

void Gfx_SetupDL31_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_31]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL32_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_32]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL33_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_33]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL64(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_64]);
    return gfx;
}

Gfx* Gfx_SetupDL34(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_34]);
    return gfx;
}

void Gfx_SetupDL34_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL34(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL35_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_35]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL44_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_44]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL36_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_36]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL28(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_28]);
    return gfx;
}

void Gfx_SetupDL28_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_28]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL43_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_43]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL45_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_45]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL46_Overlay(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(OVERLAY_DISP++, gSetupDLs[SETUPDL_46]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL38_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_38]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL4_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_4]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL37_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_37]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL2_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_2]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL39(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_39]);
    return gfx;
}

void Gfx_SetupDL39_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL39(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL39_Overlay(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    OVERLAY_DISP = Gfx_SetupDL39(OVERLAY_DISP);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL39_Ptr(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_39]);
    *gfxP = gfx;
}

void Gfx_SetupDL40_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_40]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL41_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_41]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL47_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_47]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_SetupDL66(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_66]);
    return gfx;
}

Gfx* Gfx_SetupDL67(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_67]);
    return gfx;
}

Gfx* Gfx_SetupDL68_NoCD(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_68]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

Gfx* Gfx_SetupDL69_NoCD(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_69]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

Gfx* Gfx_SetupDL65_NoCD(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_65]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

Gfx* Gfx_SetupDL70(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_70]);
    return gfx;
}

Gfx* Gfx_SetupDL20_NoCD(Gfx* gfx) {
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_20]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

void Gfx_SetupDL42_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_42]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL42_Overlay(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(OVERLAY_DISP++, gSetupDLs[SETUPDL_42]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL48_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_48]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL49_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_49]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL27_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_27]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL60_XluNoCD(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_60]);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL61_Xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gSetupDLs[SETUPDL_61]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL56_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_56]);

    CLOSE_DISPS(gfxCtx);
}

void Gfx_SetupDL56_Ptr(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_56]);
    *gfxP = gfx;
}

void Gfx_SetupDL59_Opa(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSetupDLs[SETUPDL_59]);

    CLOSE_DISPS(gfxCtx);
}

Gfx* Gfx_BranchTexScroll(Gfx** gfxP, u32 x, u32 y, s32 width, s32 height) {
    Gfx* gfx = Graph_DlistAlloc(gfxP, 3 * sizeof(Gfx));

    gDPTileSync(&gfx[0]);
    gDPSetTileSize(&gfx[1], 0, x, y, (x + ((width - 1) << 2)), (y + ((height - 1) << 2)));
    gSPEndDisplayList(&gfx[2]);

    return gfx;
}

void func_8012CB04(Gfx** gfxP, u32 x, u32 y) {
    Gfx_BranchTexScroll(gfxP, x, y, 0, 0);
}

Gfx* func_8012CB28(GraphicsContext* gfxCtx, u32 x, u32 y) {
    return Gfx_TexScroll(gfxCtx, x, y, 0, 0);
}

Gfx* Gfx_TexScrollEx(GraphicsContext* gfxCtx, u32 x, u32 y, s32 width, s32 height, s32 xStep, s32 yStep) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 7 * sizeof(Gfx));

    x %= 2048;
    y %= 2048;

    s32 xEnd = (s32)x + xStep;
    s32 yEnd = (s32)y + yStep;

    gDPTileSync(&gfx[0]);
    gDPSetTileSizeLerp(&gfx[1], 0, x, y, (x + ((width - 1) << 2)), (y + ((height - 1) << 2)), xEnd, yEnd,
                       (xEnd + ((width - 1) << 2)), (yEnd + ((height - 1) << 2)));
    gSPEndDisplayList(&gfx[6]);

    return gfx;
}

Gfx* Gfx_TexScroll(GraphicsContext* gfxCtx, u32 x, u32 y, s32 width, s32 height) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Gfx));

    x %= 2048;
    y %= 2048;

    gDPTileSync(&gfx[0]);
    gDPSetTileSize(&gfx[1], 0, x, y, (x + ((width - 1) << 2)), (y + ((height - 1) << 2)));
    gSPEndDisplayList(&gfx[2]);

    return gfx;
}

Gfx* Gfx_TwoTexScrollEx(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2,
                        u32 y2, s32 width2, s32 height2, s32 xStep1, s32 yStep1, s32 xStep2, s32 yStep2) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 12 * sizeof(Gfx));

    x1 %= 2048;
    y1 %= 2048;
    x2 %= 2048;
    y2 %= 2048;

    s32 x1End = (s32)x1 + xStep1;
    s32 y1End = (s32)y1 + yStep1;
    s32 x2End = (s32)x2 + xStep2;
    s32 y2End = (s32)y2 + yStep2;

    gDPTileSync(&gfx[0]);
    gDPSetTileSizeLerp(&gfx[1], tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)), x1End, y1End,
                       (x1End + ((width1 - 1) << 2)), (y1End + ((height1 - 1) << 2)));
    gDPSetTileSizeLerp(&gfx[6], tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)), x2End, y2End,
                       (x2End + ((width2 - 1) << 2)), (y2End + ((height2 - 1) << 2)));
    gSPEndDisplayList(&gfx[11]);

    return gfx;
}

Gfx* Gfx_TwoTexScroll(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2,
                      u32 y2, s32 width2, s32 height2) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 5 * sizeof(Gfx));

    x1 %= 2048;
    y1 %= 2048;
    x2 %= 2048;
    y2 %= 2048;

    gDPTileSync(&gfx[0]);
    gDPSetTileSize(&gfx[1], tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)));
    gDPTileSync(&gfx[2]);
    gDPSetTileSize(&gfx[3], tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)));
    gSPEndDisplayList(&gfx[4]);

    return gfx;
}

Gfx* Gfx_TwoTexScrollEnvColor(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                              u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 6 * sizeof(Gfx));

    x1 %= 2048;
    y1 %= 2048;
    x2 %= 2048;
    y2 %= 2048;

    gDPTileSync(&gfx[0]);
    gDPSetTileSize(&gfx[1], tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)));
    gDPTileSync(&gfx[2]);
    gDPSetTileSize(&gfx[3], tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)));
    gDPSetEnvColor(&gfx[4], r, g, b, a);
    gSPEndDisplayList(&gfx[5]);

    return gfx;
}

Gfx* Gfx_TwoTexScrollEnvColorEx(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                                u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a, s32 xStep1,
                                s32 yStep1, s32 xStep2, s32 yStep2) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 13 * sizeof(Gfx));

    x1 %= 2048;
    y1 %= 2048;
    x2 %= 2048;
    y2 %= 2048;

    s32 x1End = (s32)x1 + xStep1;
    s32 y1End = (s32)y1 + yStep1;
    s32 x2End = (s32)x2 + xStep2;
    s32 y2End = (s32)y2 + yStep2;

    gDPTileSync(&gfx[0]);
    gDPSetTileSizeLerp(&gfx[1], tile1, x1, y1, (x1 + ((width1 - 1) << 2)), (y1 + ((height1 - 1) << 2)), x1End, y1End,
                       (x1End + ((width1 - 1) << 2)), (y1End + ((height1 - 1) << 2)));
    gDPSetTileSizeLerp(&gfx[6], tile2, x2, y2, (x2 + ((width2 - 1) << 2)), (y2 + ((height2 - 1) << 2)), x2End, y2End,
                       (x2End + ((width2 - 1) << 2)), (y2End + ((height2 - 1) << 2)));
    gDPSetEnvColor(&gfx[11], r, g, b, a);
    gSPEndDisplayList(&gfx[12]);

    return gfx;
}

Gfx* Gfx_EnvColor(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetEnvColor(&gfx[0], r, g, b, a);
    gSPEndDisplayList(&gfx[1]);

    return gfx;
}

Gfx* Gfx_PrimColor(GraphicsContext* gfxCtx, s32 lodfrac, s32 r, s32 g, s32 b, s32 a) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetPrimColor(&gfx[0], 0, lodfrac, r, g, b, a);
    gSPEndDisplayList(&gfx[1]);

    return gfx;
}

void func_8012CF0C(GraphicsContext* gfxCtx, s32 clearFb, s32 clearZb, u8 r, u8 g, u8 b) {
    Gfx* masterGfx;
    void* zbuffer;
    s32 i;

    gSegments[0x00] = 0;
    gSegments[0x0F] = (uintptr_t)gfxCtx->curFrameBuffer;
    gSegments[0x0E] = (uintptr_t)gGfxMasterDL;

    zbuffer = gfxCtx->zbuffer;

    // Set up Framebuffer and Z-Buffer

    masterGfx = gGfxMasterDL->setupBuffers;

    gSPDisplayList(&masterGfx[0], D_0E000000_TO_SEGMENTED(syncSegments));

    gSPDisplayList(&masterGfx[1], sFillSetupDL);

    gDPSetColorImage(&masterGfx[2], G_IM_FMT_RGBA, G_IM_SIZ_16b, gCfbWidth, D_0F000000_TO_SEGMENTED);
    if (zbuffer != NULL) {
        gDPSetDepthImage(&masterGfx[3], zbuffer);
    } else {
        gDPSetDepthImage(&masterGfx[3], D_0F000000_TO_SEGMENTED);
    }
    gSPEndDisplayList(&masterGfx[4]);

    // Set Scissor

    masterGfx = gGfxMasterDL->setScissor;

    gDPSetScissor(&masterGfx[0], G_SC_NON_INTERLACE, 0, 0, gCfbWidth, gCfbHeight);
    gSPEndDisplayList(&masterGfx[1]);

    // Clear Z-Buffer

    masterGfx = gGfxMasterDL->clearZBuffer;

    if (zbuffer == NULL) {
        gSPEndDisplayList(&masterGfx[0]);
    } else {
        gDPSetColorImage(&masterGfx[0], G_IM_FMT_RGBA, G_IM_SIZ_16b, gCfbWidth, zbuffer);
        gDPPipeSync(&masterGfx[1]);
        gDPSetCycleType(&masterGfx[2], G_CYC_FILL);
        gDPSetRenderMode(&masterGfx[3], G_RM_NOOP, G_RM_NOOP2);
        gDPSetFillColor(&masterGfx[4], (GPACK_RGBA5551(255, 255, 240, 0) << 16) | GPACK_RGBA5551(255, 255, 240, 0));

        gSPDisplayList(&masterGfx[5], D_0E000000_TO_SEGMENTED(clearFillRect));

        gDPSetColorImage(&masterGfx[6], G_IM_FMT_RGBA, G_IM_SIZ_16b, gCfbWidth, D_0F000000_TO_SEGMENTED);
        gSPEndDisplayList(&masterGfx[7]);
    }

    // Clear Framebuffer

    masterGfx = gGfxMasterDL->clearFrameBuffer;

    gDPSetColorImage(&masterGfx[0], G_IM_FMT_RGBA, G_IM_SIZ_16b, gCfbWidth, D_0F000000_TO_SEGMENTED);
    gDPSetCycleType(&masterGfx[1], G_CYC_FILL);
    gDPSetRenderMode(&masterGfx[2], G_RM_NOOP, G_RM_NOOP2);
    gDPSetFillColor(&masterGfx[3], (GPACK_RGBA5551(r, g, b, 1) << 16) | GPACK_RGBA5551(r, g, b, 1));

    gSPBranchList(&masterGfx[4], D_0E000000_TO_SEGMENTED(clearFillRect));

    // Fillrect used by the above buffer clearing routines

    masterGfx = gGfxMasterDL->clearFillRect;

    // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3
    // WideRectangle consumes two instructions and requires ++ for the macro to work
    // Remove `- 1` on the sizes so the rectangle fills the whole screen like `fillRect`
    Gfx* tmpGfx = masterGfx;
    gDPFillWideRectangle(tmpGfx++, OTRGetRectDimensionFromLeftEdge(0), 0, OTRGetRectDimensionFromRightEdge(gCfbWidth),
                         gCfbHeight);
    gDPPipeSync(&masterGfx[2]);
    gSPEndDisplayList(&masterGfx[3]);
    // #endregion

    // General Fillrect?

    masterGfx = gGfxMasterDL->fillRect;

    // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3
    // WideRectangle consumes two instructions and requires ++ for the macro to work
    tmpGfx = masterGfx;
    gDPFillWideRectangle(tmpGfx++, OTRGetRectDimensionFromLeftEdge(0), 0, OTRGetRectDimensionFromRightEdge(gCfbWidth),
                         gCfbHeight);
    gDPPipeSync(&masterGfx[2]);
    gSPEndDisplayList(&masterGfx[3]);
    // #endregion

    // Sync SP Segments with current CPU Segments

    masterGfx = gGfxMasterDL->syncSegments;

    for (i = 0; i < ARRAY_COUNT(gSegments); i++) {
        if (i == 0x0E) {
            gSPNoOp(&masterGfx[i]);
        } else {
            gSPSegment(&masterGfx[i], i, gSegments[i]);
        }
    }
    gSPEndDisplayList(&masterGfx[i]);

    OPEN_DISPS(gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gGfxMasterDL->setupBuffers);
    gSPDisplayList(POLY_XLU_DISP++, gGfxMasterDL->setupBuffers);
    gSPDisplayList(OVERLAY_DISP++, gGfxMasterDL->setupBuffers);
    gSPDisplayList(DEBUG_DISP++, gGfxMasterDL->setupBuffers);

    if (clearZb) {
        gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearZBuffer));
    }
    if (clearFb) {
        gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearFrameBuffer));
    }

    CLOSE_DISPS(gfxCtx);
}

void func_8012D374(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b) {
    if ((R_PAUSE_BG_PRERENDER_STATE <= PAUSE_BG_PRERENDER_SETUP) && (gTransitionTileState <= TRANS_TILE_SETUP)) {
        func_8012CF0C(gfxCtx, true, true, r, g, b);
    } else {
        func_8012CF0C(gfxCtx, false, false, r, g, b);
    }
}

void func_8012D40C(f32* param_1, f32* param_2, s16* param_3) {
    param_1[0] = param_3[0] * 0.25f;
    param_1[1] = param_3[1] * 0.25f;
    param_1[2] = param_3[4] * 0.25f;
    param_1[3] = param_3[5] * 0.25f;

    param_2[0] = param_1[0] / (gScreenWidth / 2);
    param_2[2] = param_1[2] - param_1[0];
    param_2[1] = param_1[1] / (gScreenHeight / 2);
    param_2[3] = param_1[3] - param_1[1];
}
