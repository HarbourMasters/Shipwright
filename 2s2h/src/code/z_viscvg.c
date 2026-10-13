#include "global.h"
#include "z64viscvg.h"

Gfx D_801C5DD0[] = {
    gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | G_RM_VISCVG | G_RM_VISCVG2),
    gsSPBranchList(D_0E000000_TO_SEGMENTED(fillRect)),
};

Gfx D_801C5DE0[] = {
    gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | FORCE_BL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_A_MEM) |
                         GBL_c2(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_A_MEM)),
    gsSPBranchList(D_0E000000_TO_SEGMENTED(fillRect)),
};

Gfx D_801C5DF0[] = {
    gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | FORCE_BL |
                         GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_A_MEM) |
                         GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_A_MEM)),
    gsSPBranchList(D_0E000000_TO_SEGMENTED(fillRect)),
};

Gfx D_801C5E00[] = {
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_DISABLE | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsSPBranchList(D_0E000000_TO_SEGMENTED(fillRect)),
    gsDPSetOtherMode(G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | FORCE_BL |
                         GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_A_MEM) |
                         GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_MEM, G_BL_A_MEM)),
    gsSPBranchList(D_0E000000_TO_SEGMENTED(fillRect)),
};

void VisCvg_Init(VisCvg* this) {
    this->type = 0;
    this->setScissor = false;
    this->color.r = 255;
    this->color.g = 255;
    this->color.b = 255;
    this->color.a = 255;
}

void VisCvg_Destroy(VisCvg* this) {
}

void VisCvg_Draw(VisCvg* this, Gfx** gfxp) {
    Gfx* gfx = *gfxp;

    gDPPipeSync(gfx++);
    gDPSetPrimDepth(gfx++, -1, -1);

    if (this->setScissor == true) {
        gSPDisplayList(gfx++, D_0E000000_TO_SEGMENTED(setScissor));
    }

    switch (this->type) {
        case 1:
            gSPDisplayList(gfx++, D_801C5DF0);
            break;

        case 2:
            gDPSetColor(gfx++, G_SETPRIMCOLOR, this->color.rgba);
            gSPDisplayList(gfx++, D_801C5E00);
            break;

        case 3:
            gDPSetColor(gfx++, G_SETBLENDCOLOR, this->color.rgba);
            gSPDisplayList(gfx++, D_801C5DD0);
            break;

        case 4:
            gDPSetColor(gfx++, G_SETFOGCOLOR, this->color.rgba);
            gSPDisplayList(gfx++, D_801C5DE0);
            break;

        default:
            break;
    }

    gDPPipeSync(gfx++);
    *gfxp = gfx;
}
