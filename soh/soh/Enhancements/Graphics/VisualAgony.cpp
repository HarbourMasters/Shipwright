#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/vanilla-behavior/GIVanillaBehavior.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"
#include "soh/ShipInit.hpp"
#include "textures/icon_item_24_static/icon_item_24_static.h"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"

extern PlayState* gPlayState;
int16_t OTRGetRectDimensionFromLeftEdge(float v);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
void FrameInterpolation_RecordOpenChild(const void* a, int b);
void FrameInterpolation_RecordCloseChild(void);
}

#define CVAR_VISUAL_AGONY_NAME CVAR_ENHANCEMENT("VisualAgony")
#define CVAR_VISUAL_AGONY_VALUE CVarGetInteger(CVAR_VISUAL_AGONY_NAME, 0)
#define SHOULD_HIDE_VISUAL_AGONY                                                                           \
    ((player->stateFlags1 &                                                                                \
      (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_DEAD | PLAYER_STATE1_TALKING | PLAYER_STATE1_LOADING)) || \
     GameInteractor_NoUIActive())

void DrawVisualAgony(Player* player, double temp) {
    if (SHOULD_HIDE_VISUAL_AGONY || player->closestSecretDistSq > 80000.0f) {
        return;
    }
    Color_RGB8 stoneOfAgonyColor = { 255, 255, 255 };
    if (CVarGetInteger(CVAR_COSMETIC("HUD.StoneOfAgony.Changed"), 0)) {
        stoneOfAgonyColor = CVarGetColor24(CVAR_COSMETIC("HUD.StoneOfAgony.Value"), stoneOfAgonyColor);
    }
    s16 Top_Margins = (CVarGetInteger(CVAR_COSMETIC("HUD.Margin.T"), 0) * -1);
    s16 Left_Margins = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.L"), 0);
    s16 Right_Margins = CVarGetInteger(CVAR_COSMETIC("HUD.Margin.R"), 0);
    s16 X_Margins_VSOA;
    s16 Y_Margins_VSOA;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
        if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == ORIGINAL_LOCATION) {
            X_Margins_VSOA = Left_Margins;
        } else {
            X_Margins_VSOA = 0;
        }
        Y_Margins_VSOA = Top_Margins;
    } else {
        X_Margins_VSOA = 0;
        Y_Margins_VSOA = 0;
    }
    s16 PosX_VSOA_ori = OTRGetRectDimensionFromLeftEdge(26) + X_Margins_VSOA;
    s16 PosY_VSOA_ori = 60 + Y_Margins_VSOA;
    s16 PosX_VSOA;
    s16 PosY_VSOA;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) != 0) {
        PosY_VSOA = CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosY"), 0) + Y_Margins_VSOA;
        if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == ANCHOR_LEFT) {
            if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
                X_Margins_VSOA = Left_Margins;
            };
            PosX_VSOA =
                OTRGetDimensionFromLeftEdge(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0) + X_Margins_VSOA);
        } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == ANCHOR_RIGHT) {
            if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.UseMargins"), 0) != 0) {
                X_Margins_VSOA = Right_Margins;
            }
            PosX_VSOA =
                OTRGetDimensionFromRightEdge(CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0) + X_Margins_VSOA);
        } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == ANCHOR_NONE) {
            PosX_VSOA = CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosX"), 0);
        } else if (CVarGetInteger(CVAR_COSMETIC("HUD.VisualSoA.PosType"), 0) == HIDDEN) {
            PosX_VSOA = -9999;
        } else {
            PosX_VSOA = PosX_VSOA_ori;
        }
    } else {
        PosY_VSOA = PosY_VSOA_ori;
        PosX_VSOA = PosX_VSOA_ori;
    }

    int rectLeft = PosX_VSOA; // Left X Pos
    int rectTop = PosY_VSOA;  // Top Y Pos
    int rectWidth = 24;       // Texture Width
    int rectHeight = 24;      // Texture Heigh
    int DefaultIconA = 50;    // Default icon alpha (55 on 255)

    OPEN_DISPS(gPlayState->state.gfxCtx);
    gDPPipeSync(OVERLAY_DISP++);

    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, DefaultIconA);

    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    if (player->unk_6A0 > 4000000.0f) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b, 255);
    } else {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b,
                        DefaultIconA);
    }
    if (temp == 0 || temp <= 0.1f) {
        /*Fail check, it is used to draw off the icon when
        link is standing out range but do not refresh unk_6A0.
        Also used to make a default value in my case.*/
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, stoneOfAgonyColor.r, stoneOfAgonyColor.g, stoneOfAgonyColor.b,
                        DefaultIconA);
    }
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
    gDPSetOtherMode(OVERLAY_DISP++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_IA16 | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
    gDPLoadTextureBlock(OVERLAY_DISP++, gQuestIconStoneOfAgonyTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gDPSetOtherMode(OVERLAY_DISP++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_IA16 | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_XLU_SURF | G_RM_XLU_SURF2);
    gSPWideTextureRectangle(OVERLAY_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                            (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void RegisterVisualAgony() {
    COND_VB_SHOULD(VB_RUMBLE_FOR_SECRET, CVAR_VISUAL_AGONY_VALUE, {
        Player* player = va_arg(args, Player*);
        double temp = va_arg(args, double);
        DrawVisualAgony(player, temp);
        if (*should == true && !SHOULD_HIDE_VISUAL_AGONY) {
            Audio_PlaySoundGeneral(NA_SE_SY_MESSAGE_WOMAN, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterVisualAgony, { CVAR_VISUAL_AGONY_NAME });