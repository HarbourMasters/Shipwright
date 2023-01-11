#include "global.h"
#include "textures/parameter_static/parameter_static.h"
#include "soh/frame_interpolation.h"

s16 Top_LM_Margin = 0;
s16 Left_LM_Margin = 0;
s16 Right_LM_Margin = 0;
s16 Bottom_LM_Margin = 0;

static s16 sHeartsPrimColors[3][3] = {
    { HEARTS_PRIM_R, HEARTS_PRIM_G, HEARTS_PRIM_B },
    { HEARTS_BURN_PRIM_R, HEARTS_BURN_PRIM_G, HEARTS_BURN_PRIM_B },    // unused
    { HEARTS_DROWN_PRIM_R, HEARTS_DROWN_PRIM_G, HEARTS_DROWN_PRIM_B }, // unused
};

static s16 sHeartsEnvColors[3][3] = {
    { HEARTS_ENV_R, HEARTS_ENV_G, HEARTS_ENV_B },
    { HEARTS_BURN_ENV_R, HEARTS_BURN_ENV_G },                       // unused
    { HEARTS_DROWN_ENV_R, HEARTS_DROWN_ENV_G, HEARTS_DROWN_ENV_B }, // unused
};

static s16 sHeartsPrimFactors[3][3] = {
    {
        HEARTS_PRIM_R - HEARTS_PRIM_R,
        HEARTS_PRIM_G - HEARTS_PRIM_G,
        HEARTS_PRIM_B - HEARTS_PRIM_B,
    },
    // unused
    {
        HEARTS_BURN_PRIM_R - HEARTS_PRIM_R,
        HEARTS_BURN_PRIM_G - HEARTS_PRIM_G,
        HEARTS_BURN_PRIM_B - HEARTS_PRIM_B,
    },
    // unused
    {
        HEARTS_DROWN_PRIM_R - HEARTS_PRIM_R,
        HEARTS_DROWN_PRIM_G - HEARTS_PRIM_G,
        HEARTS_DROWN_PRIM_B - HEARTS_PRIM_B,
    },
};

static s16 sHeartsEnvFactors[3][3] = {
    {
        HEARTS_ENV_R - HEARTS_ENV_R,
        HEARTS_ENV_G - HEARTS_ENV_G,
        HEARTS_ENV_B - HEARTS_ENV_B,
    },
    // unused
    {
        HEARTS_BURN_ENV_R - HEARTS_ENV_R,
        HEARTS_BURN_ENV_G - HEARTS_ENV_G,
        HEARTS_BURN_ENV_B - HEARTS_ENV_B,
    },
    // unused
    {
        HEARTS_DROWN_ENV_R - HEARTS_ENV_R,
        HEARTS_DROWN_ENV_G - HEARTS_ENV_G,
        HEARTS_DROWN_ENV_B - HEARTS_ENV_B,
    },
};

static s16 sHeartsDDPrimColors[3][3] = {
    { HEARTS_DD_PRIM_R, HEARTS_DD_PRIM_G, HEARTS_DD_PRIM_B },
    { HEARTS_BURN_PRIM_R, HEARTS_BURN_PRIM_G, HEARTS_BURN_PRIM_B },    // unused
    { HEARTS_DROWN_PRIM_R, HEARTS_DROWN_PRIM_G, HEARTS_DROWN_PRIM_B }, // unused
};

static s16 sHeartsDDEnvColors[3][3] = {
    { HEARTS_DD_ENV_R, HEARTS_DD_ENV_G, HEARTS_DD_ENV_B },
    { HEARTS_BURN_ENV_R, HEARTS_BURN_ENV_G, HEARTS_BURN_ENV_B },    // unused
    { HEARTS_DROWN_ENV_R, HEARTS_DROWN_ENV_G, HEARTS_DROWN_ENV_B }, // unused
};

static s16 sHeartsDDPrimFactors[3][3] = {
    {
        HEARTS_DD_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_DD_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_DD_PRIM_B - HEARTS_DD_PRIM_B,
    },
    // unused
    {
        HEARTS_BURN_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_BURN_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_BURN_PRIM_B - HEARTS_DD_PRIM_B,
    },
    // unused
    {
        HEARTS_DROWN_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_DROWN_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_DROWN_PRIM_B - HEARTS_DD_PRIM_B,
    },
};

static s16 sHeartsDDEnvFactors[3][3] = {
    {
        HEARTS_DD_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_DD_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_DD_ENV_B - HEARTS_DD_ENV_B,
    },
    // unused
    {
        HEARTS_BURN_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_BURN_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_BURN_ENV_B - HEARTS_DD_ENV_B,
    },
    // unused
    {
        HEARTS_DROWN_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_DROWN_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_DROWN_ENV_B - HEARTS_DD_ENV_B,
    },
};

// Current colors for the double defense hearts
s16 sBeatingHeartsDDPrim[3];
s16 sBeatingHeartsDDEnv[3];
s16 sHeartsDDPrim[2][3];
s16 sHeartsDDEnv[2][3];

void HealthMeter_Init(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Color_RGB8 mainColor = {HEARTS_PRIM_R, HEARTS_PRIM_G, HEARTS_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_Hearts.Changed", 0)) {
        mainColor = CVarGetColor24("gCosmetics.Consumable_Hearts.Value", mainColor);
    }
    Color_RGB8 mainBorder = {HEARTS_ENV_R, HEARTS_ENV_G, HEARTS_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_HeartBorder.Changed", 0)) {
        mainBorder = CVarGetColor24("gCosmetics.Consumable_HeartBorder.Value", mainBorder);
    }
    Color_RGB8 ddColor = {HEARTS_DD_ENV_R, HEARTS_DD_ENV_G, HEARTS_DD_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHearts.Changed", 0)) {
        ddColor = CVarGetColor24("gCosmetics.Consumable_DDHearts.Value", ddColor);
    }
    Color_RGB8 ddBorder = {HEARTS_DD_PRIM_R, HEARTS_DD_PRIM_G, HEARTS_DD_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHeartBorder.Changed", 0)) {
        ddBorder = CVarGetColor24("gCosmetics.Consumable_DDHeartBorder.Value", ddBorder);
    }

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 = gSaveContext.health;
    interfaceCtx->unk_22A = interfaceCtx->unk_1FE = 0;
    interfaceCtx->unk_22C = interfaceCtx->unk_200 = 0;

    interfaceCtx->heartsPrimR[0] = mainColor.r;
    interfaceCtx->heartsPrimG[0] = mainColor.g;
    interfaceCtx->heartsPrimB[0] = mainColor.b;

    interfaceCtx->heartsEnvR[0] = mainBorder.r;
    interfaceCtx->heartsEnvG[0] = mainBorder.g;
    interfaceCtx->heartsEnvB[0] = mainBorder.b;

    interfaceCtx->heartsPrimR[1] = mainColor.r;
    interfaceCtx->heartsPrimG[1] = mainColor.g;
    interfaceCtx->heartsPrimB[1] = mainColor.b;

    interfaceCtx->heartsEnvR[1] = mainBorder.r;
    interfaceCtx->heartsEnvG[1] = mainBorder.g;
    interfaceCtx->heartsEnvB[1] = mainBorder.b;

    sHeartsDDPrim[0][0] = sHeartsDDPrim[1][0] = ddBorder.r;
    sHeartsDDPrim[0][1] = sHeartsDDPrim[1][1] = ddBorder.g;
    sHeartsDDPrim[0][2] = sHeartsDDPrim[1][2] = ddBorder.b;

    sHeartsDDEnv[0][0] = sHeartsDDEnv[1][0] = ddColor.r;
    sHeartsDDEnv[0][1] = sHeartsDDEnv[1][1] = ddColor.g;
    sHeartsDDEnv[0][2] = sHeartsDDEnv[1][2] = ddColor.b;
}

void HealthMeter_Update(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    f32 factor = interfaceCtx->unk_1FE * 0.1f;
    f32 ddFactor;
    s32 type = 0;
    s32 ddType;
    s16 rFactor;
    s16 gFactor;
    s16 bFactor;

    Top_LM_Margin = CVarGetInteger("gHUDMargin_T", 0);
    Left_LM_Margin = CVarGetInteger("gHUDMargin_L", 0);
    Right_LM_Margin = CVarGetInteger("gHUDMargin_R", 0);
    Bottom_LM_Margin = CVarGetInteger("gHUDMargin_B", 0);

    Color_RGB8 mainColor = {HEARTS_PRIM_R, HEARTS_PRIM_G, HEARTS_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_Hearts.Changed", 0)) {
        mainColor = CVarGetColor24("gCosmetics.Consumable_Hearts.Value", mainColor);
    }
    Color_RGB8 mainBorder = {HEARTS_ENV_R, HEARTS_ENV_G, HEARTS_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_HeartBorder.Changed", 0)) {
        mainBorder = CVarGetColor24("gCosmetics.Consumable_HeartBorder.Value", mainBorder);
    }
    Color_RGB8 ddColor = {HEARTS_DD_ENV_R, HEARTS_DD_ENV_G, HEARTS_DD_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHearts.Changed", 0)) {
        ddColor = CVarGetColor24("gCosmetics.Consumable_DDHearts.Value", ddColor);
    }
    Color_RGB8 ddBorder = {HEARTS_DD_PRIM_R, HEARTS_DD_PRIM_G, HEARTS_DD_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHeartBorder.Changed", 0)) {
        ddBorder = CVarGetColor24("gCosmetics.Consumable_DDHeartBorder.Value", ddBorder);
    }

    if (interfaceCtx->unk_200 != 0) {
        interfaceCtx->unk_1FE--;
        if (interfaceCtx->unk_1FE <= 0) {
            interfaceCtx->unk_1FE = 0;
            interfaceCtx->unk_200 = 0;
        }
    } else {
        interfaceCtx->unk_1FE++;
        if (interfaceCtx->unk_1FE >= 10) {
            interfaceCtx->unk_1FE = 10;
            interfaceCtx->unk_200 = 1;
        }
    }

    ddFactor = factor;

    interfaceCtx->heartsPrimR[0] = mainColor.r;
    interfaceCtx->heartsPrimG[0] = mainColor.g;
    interfaceCtx->heartsPrimB[0] = mainColor.b;

    interfaceCtx->heartsEnvR[0] = mainBorder.r;
    interfaceCtx->heartsEnvG[0] = mainBorder.g;
    interfaceCtx->heartsEnvB[0] = mainBorder.b;

    interfaceCtx->heartsPrimR[1] = mainColor.r;
    interfaceCtx->heartsPrimG[1] = mainColor.g;
    interfaceCtx->heartsPrimB[1] = mainColor.b;

    interfaceCtx->heartsEnvR[1] = mainBorder.r;
    interfaceCtx->heartsEnvG[1] = mainBorder.g;
    interfaceCtx->heartsEnvB[1] = mainBorder.b;

    rFactor = sHeartsPrimFactors[0][0] * factor;
    gFactor = sHeartsPrimFactors[0][1] * factor;
    bFactor = sHeartsPrimFactors[0][2] * factor;

    interfaceCtx->beatingHeartPrim[0] = (u8)(rFactor + mainColor.r) & 0xFF;
    interfaceCtx->beatingHeartPrim[1] = (u8)(gFactor + mainColor.g) & 0xFF;
    interfaceCtx->beatingHeartPrim[2] = (u8)(bFactor + mainColor.b) & 0xFF;

    rFactor = sHeartsEnvFactors[0][0] * factor;
    gFactor = sHeartsEnvFactors[0][1] * factor;
    bFactor = sHeartsEnvFactors[0][2] * factor;

    if (1) {}
    ddType = type;

    interfaceCtx->beatingHeartEnv[0] = (u8)(rFactor + mainBorder.r) & 0xFF;
    interfaceCtx->beatingHeartEnv[1] = (u8)(gFactor + mainBorder.g) & 0xFF;
    interfaceCtx->beatingHeartEnv[2] = (u8)(bFactor + mainBorder.b) & 0xFF;

    sHeartsDDPrim[0][0] = ddBorder.r;
    sHeartsDDPrim[0][1] = ddBorder.g;
    sHeartsDDPrim[0][2] = ddBorder.b;

    sHeartsDDEnv[0][0] = ddColor.r;
    sHeartsDDEnv[0][1] = ddColor.g;
    sHeartsDDEnv[0][2] = ddColor.b;

    sHeartsDDPrim[1][0] = ddBorder.r;
    sHeartsDDPrim[1][1] = ddBorder.g;
    sHeartsDDPrim[1][2] = ddBorder.b;

    sHeartsDDEnv[1][0] = ddColor.r;
    sHeartsDDEnv[1][1] = ddColor.g;
    sHeartsDDEnv[1][2] = ddColor.b;

    rFactor = sHeartsDDPrimFactors[ddType][0] * ddFactor;
    gFactor = sHeartsDDPrimFactors[ddType][1] * ddFactor;
    bFactor = sHeartsDDPrimFactors[ddType][2] * ddFactor;

    sBeatingHeartsDDPrim[0] = (u8)(rFactor + ddBorder.r) & 0xFF;
    sBeatingHeartsDDPrim[1] = (u8)(gFactor + ddBorder.g) & 0xFF;
    sBeatingHeartsDDPrim[2] = (u8)(bFactor + ddBorder.b) & 0xFF;

    rFactor = sHeartsDDEnvFactors[ddType][0] * ddFactor;
    gFactor = sHeartsDDEnvFactors[ddType][1] * ddFactor;
    bFactor = sHeartsDDEnvFactors[ddType][2] * ddFactor;

    sBeatingHeartsDDEnv[0] = (u8)(rFactor + ddColor.r) & 0xFF;
    sBeatingHeartsDDEnv[1] = (u8)(gFactor + ddColor.g) & 0xFF;
    sBeatingHeartsDDEnv[2] = (u8)(bFactor + ddColor.b) & 0xFF;
}

s32 func_80078E18(PlayState* play) {
    gSaveContext.health = play->interfaceCtx.unk_226;
    return 1;
}

s32 func_80078E34(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 += 0x10;

    if (interfaceCtx->unk_226 >= gSaveContext.health) {
        interfaceCtx->unk_226 = gSaveContext.health;
        return 1;
    }

    return 0;
}

s32 func_80078E84(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (interfaceCtx->unk_228 != 0) {
        interfaceCtx->unk_228--;
    } else {
        interfaceCtx->unk_228 = 0x140;
        interfaceCtx->unk_226 -= 0x10;
        if (interfaceCtx->unk_226 <= 0) {
            interfaceCtx->unk_226 = 0;
            play->damagePlayer(play, -(gSaveContext.health + 1));
            return 1;
        }
    }
    return 0;
}

static void* sHeartTextures[] = {
    gHeartFullTex,         gHeartQuarterTex,      gHeartQuarterTex,      gHeartQuarterTex,
    gHeartQuarterTex,      gHeartQuarterTex,      gHeartHalfTex,         gHeartHalfTex,
    gHeartHalfTex,         gHeartHalfTex,         gHeartHalfTex,         gHeartThreeQuarterTex,
    gHeartThreeQuarterTex, gHeartThreeQuarterTex, gHeartThreeQuarterTex, gHeartThreeQuarterTex,
};

static void* sHeartDDTextures[] = {
    gDefenseHeartFullTex,         gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,
    gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,
    gDefenseHeartHalfTex,         gDefenseHeartHalfTex,         gDefenseHeartHalfTex,
    gDefenseHeartHalfTex,         gDefenseHeartHalfTex,         gDefenseHeartThreeQuarterTex,
    gDefenseHeartThreeQuarterTex, gDefenseHeartThreeQuarterTex, gDefenseHeartThreeQuarterTex,
    gDefenseHeartThreeQuarterTex,
};

s16 getHealthMeterXOffset() {
    s16 X_Margins;
    if (CVarGetInteger("gHeartsUseMargins", 0) != 0)
        X_Margins = Left_LM_Margin;
    else
        X_Margins = 0;

    if (CVarGetInteger("gHeartsCountPosType", 0) != 0) {
        if (CVarGetInteger("gHeartsCountPosType", 0) == 1) {//Anchor Left
            return OTRGetDimensionFromLeftEdge(CVarGetInteger("gHeartsCountPosX", 0)+X_Margins+70.0f);
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 2) {//Anchor Right
            X_Margins = Right_LM_Margin;
            return OTRGetDimensionFromRightEdge(CVarGetInteger("gHeartsCountPosX", 0)+X_Margins+70.0f);
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 3) {//Anchor None
            return CVarGetInteger("gHeartsCountPosX", 0)+70.0f;;
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 4) {//Hidden
            return -9999;
        }
    } else {
        return OTRGetDimensionFromLeftEdge(0.0f)+X_Margins;
    }
}

s16 getHealthMeterYOffset() {
    s16 Y_Margins;
    if (CVarGetInteger("gHeartsUseMargins", 0) != 0)
        Y_Margins = (Top_LM_Margin*-1);
    else
        Y_Margins = 0;

    f32 HeartsScale = 0.7f; 
    if (CVarGetInteger("gHeartsCountPosType", 0) != 0) {
        HeartsScale = CVarGetFloat("gHeartsCountScale", 0.7f);
        return CVarGetInteger("gHeartsCountPosY", 0)+Y_Margins+(HeartsScale*15);
    } else {
        return 0.0f+Y_Margins;
    }
}

void HealthMeter_Draw(PlayState* play) {
    s32 pad[5];
    void* heartBgImg;
    u32 curColorSet;
    f32 PosX_anchor;
    f32 offsetX;
    f32 offsetY;
    s32 i;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vtx* sp154 = interfaceCtx->beatingHeartVtx;
    s32 curHeartFraction = gSaveContext.health % 0x10;
    s16 totalHeartCount = gSaveContext.healthCapacity / 0x10;
    s16 fullHeartCount = gSaveContext.health / 0x10;
    s32 pad2;
    f32 sp144 = interfaceCtx->unk_22A * 0.1f;
    s32 curCombineModeSet = 0;
    u8* curBgImgLoaded = NULL;
    s32 ddHeartCountMinusOne = gSaveContext.isDoubleDefenseAcquired ? totalHeartCount - 1 : -1;
    f32 HeartsScale = 0.7f; 
    if (CVarGetInteger("gHeartsCountPosType", 0) != 0) {
        HeartsScale = CVarGetFloat("gHeartsCountScale", 0.7f);
    }
    static u32 epoch = 0;
    epoch++;

    OPEN_DISPS(gfxCtx);

    if (!(gSaveContext.health % 0x10)) {
        fullHeartCount--;
    }

    curColorSet = -1;
/*
    s16 X_Margins;
    s16 Y_Margins;
    if (CVarGetInteger("gHeartsUseMargins", 0) != 0) {
        X_Margins = Left_LM_Margin;
        Y_Margins = (Top_LM_Margin*-1);
    } else {
        X_Margins = 0;
        Y_Margins = 0;
    }
    s16 PosX_original = OTRGetDimensionFromLeftEdge(0.0f)+X_Margins;
    s16 PosY_original = 0.0f+Y_Margins;
    if (CVarGetInteger("gHeartsCountPosType", 0) != 0) {
        offsetY = CVarGetInteger("gHeartsCountPosY", 0)+Y_Margins+(HeartsScale*15);
        if (CVarGetInteger("gHeartsCountPosType", 0) == 1) {//Anchor Left
            offsetX = OTRGetDimensionFromLeftEdge(CVarGetInteger("gHeartsCountPosX", 0)+X_Margins+70.0f);
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 2) {//Anchor Right
            X_Margins = Right_LM_Margin;
            offsetX = OTRGetDimensionFromRightEdge(CVarGetInteger("gHeartsCountPosX", 0)+X_Margins+70.0f);
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 3) {//Anchor None
            offsetX = CVarGetInteger("gHeartsCountPosX", 0)+70.0f;
        } else if (CVarGetInteger("gHeartsCountPosType", 0) == 4) {//Hidden
            offsetX = -9999;
        }
    } else {
        offsetY = PosY_original;
        offsetX = PosX_original;
    }
*/
    offsetX = PosX_anchor = getHealthMeterXOffset();
    offsetY = getHealthMeterYOffset();

    for (i = 0; i < totalHeartCount; i++) {
        FrameInterpolation_RecordOpenChild("HealthMeter Heart", i);
        
        if ((ddHeartCountMinusOne < 0) || (i > ddHeartCountMinusOne)) {
            if (i < fullHeartCount) {
                if (curColorSet != 0) {
                    curColorSet = 0;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[0], interfaceCtx->heartsPrimG[0],
                                    interfaceCtx->heartsPrimB[0], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[0], interfaceCtx->heartsEnvG[0],
                                   interfaceCtx->heartsEnvB[0], 255);
                }
            } else if (i == fullHeartCount) {
                if (curColorSet != 1) {
                    curColorSet = 1;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->beatingHeartPrim[0],
                                    interfaceCtx->beatingHeartPrim[1], interfaceCtx->beatingHeartPrim[2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->beatingHeartEnv[0], interfaceCtx->beatingHeartEnv[1],
                                   interfaceCtx->beatingHeartEnv[2], 255);
                }
            } else if (i > fullHeartCount) {
                if (curColorSet != 2) {
                    curColorSet = 2;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[0], interfaceCtx->heartsPrimG[0],
                                    interfaceCtx->heartsPrimB[0], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[0], interfaceCtx->heartsEnvG[0],
                                   interfaceCtx->heartsEnvB[0], 255);
                }
            } else {
                if (curColorSet != 3) {
                    curColorSet = 3;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[1], interfaceCtx->heartsPrimG[1],
                                    interfaceCtx->heartsPrimB[1], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[1], interfaceCtx->heartsEnvG[1],
                                   interfaceCtx->heartsEnvB[1], 255);
                }
            }

            if (i < fullHeartCount) {
                heartBgImg = gHeartFullTex;
            } else if (i == fullHeartCount) {
                heartBgImg = sHeartTextures[curHeartFraction];
            } else {
                heartBgImg = gHeartEmptyTex;
            }
        } else {
            if (i < fullHeartCount) {
                if (curColorSet != 4) {
                    curColorSet = 4;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[0][0], sHeartsDDPrim[0][1], sHeartsDDPrim[0][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[0][0], sHeartsDDEnv[0][1], sHeartsDDEnv[0][2], 255);
                }
            } else if (i == fullHeartCount) {
                if (curColorSet != 5) {
                    curColorSet = 5;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sBeatingHeartsDDPrim[0], sBeatingHeartsDDPrim[1],
                                    sBeatingHeartsDDPrim[2], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sBeatingHeartsDDEnv[0], sBeatingHeartsDDEnv[1],
                                   sBeatingHeartsDDEnv[2], 255);
                }
            } else if (i > fullHeartCount) {
                if (curColorSet != 6) {
                    curColorSet = 6;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[0][0], sHeartsDDPrim[0][1], sHeartsDDPrim[0][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[0][0], sHeartsDDEnv[0][1], sHeartsDDEnv[0][2], 255);
                }
            } else {
                if (curColorSet != 7) {
                    curColorSet = 7;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[1][0], sHeartsDDPrim[1][1], sHeartsDDPrim[1][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[1][0], sHeartsDDEnv[1][1], sHeartsDDEnv[1][2], 255);
                }
            }

            if (i < fullHeartCount) {
                heartBgImg = gDefenseHeartFullTex;
            } else if (i == fullHeartCount) {
                heartBgImg = sHeartDDTextures[curHeartFraction];
            } else {
                heartBgImg = gDefenseHeartEmptyTex;
            }
        }

        if (curBgImgLoaded != heartBgImg) {
            curBgImgLoaded = heartBgImg;
            gDPLoadTextureBlock(OVERLAY_DISP++, heartBgImg, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
        }

        if (i != fullHeartCount) {
            if ((ddHeartCountMinusOne < 0) || (i > ddHeartCountMinusOne)) {
                if (curCombineModeSet != 1) {
                    curCombineModeSet = 1;
                    Gfx_SetupDL_39Overlay(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 3) {
                    curCombineModeSet = 3;
                    Gfx_SetupDL_39Overlay(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            temp3 = offsetY;
            temp2 = offsetX;
            temp4 = 1.0f;//Heart texture size
            temp4 /= 0.68f; //Hearts Scaled size
            temp4 *= 1 << 10;
            temp1 = 8.0f;
            temp1 *= 0.68f;
            /*gSPWideTextureRectangle(OVERLAY_DISP++, (s32)((temp2 - temp1) * 4), (s32)((temp3 - temp1) * 4),
                                (s32)((temp2 + temp1) * 4), (s32)((temp3 + temp1) * 4), G_TX_RENDERTILE, 0, 0,
                                (s32)temp4, (s32)temp4);*/
            Mtx* matrix = Graph_Alloc(gfxCtx, sizeof(Mtx));
            Matrix_SetTranslateScaleMtx2(matrix, 
                HeartsScale, //Scale X
                HeartsScale, //Scale Y
                HeartsScale, //Scale Z
                -130+offsetX, //Pos X
                (-94+offsetY) *-1, //Pos Y
                0.0f); //Pos Z
            gSPMatrix(OVERLAY_DISP++, matrix, G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPVertex(OVERLAY_DISP++, sp154, 4, 0);
            gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
        } else {
            if ((ddHeartCountMinusOne < 0) || (i > ddHeartCountMinusOne)) {
                if (curCombineModeSet != 2) {
                    curCombineModeSet = 2;
                    Gfx_SetupDL_42Overlay(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 4) {
                    curCombineModeSet = 4;
                    Gfx_SetupDL_42Overlay(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            {
                Mtx* matrix = Graph_Alloc(gfxCtx, sizeof(Mtx));
                Matrix_SetTranslateScaleMtx2(matrix, 
                HeartsScale+(HeartsScale/3) - ((HeartsScale/3) * sp144), 
                HeartsScale+(HeartsScale/3) - ((HeartsScale/3) * sp144),
                HeartsScale+(HeartsScale/3) - ((HeartsScale/3) * sp144), 
                -130+offsetX, //Pos X
                (-94+offsetY) *-1, //Pos Y
                0.0f);
                gSPMatrix(OVERLAY_DISP++, matrix, G_MTX_MODELVIEW | G_MTX_LOAD);
                gSPVertex(OVERLAY_DISP++, sp154, 4, 0);
                gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
            }
        }

        offsetX += 10.0f;
        s32 lineLength = CVarGetInteger("gHeartsLineLength", 10);
        if (lineLength != 0 && (i+1)%lineLength == 0) {
            offsetX = PosX_anchor;
            offsetY += 10.0f;
        }
        
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(gfxCtx);
}

void HealthMeter_HandleCriticalAlarm(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (interfaceCtx->unk_22C != 0) {
        interfaceCtx->unk_22A--;
        if (interfaceCtx->unk_22A <= 0) {
            interfaceCtx->unk_22A = 0;
            interfaceCtx->unk_22C = 0;
            if (CVarGetInteger("gLowHpAlarm", 0) == 0 && !Player_InCsMode(play) && (play->pauseCtx.state == 0) &&
            (play->pauseCtx.debugState == 0) && HealthMeter_IsCritical() && !Play_InCsMode(play)) {
                func_80078884(NA_SE_SY_HITPOINT_ALARM);
            }
        }
    } else {
        interfaceCtx->unk_22A++;
        if (interfaceCtx->unk_22A >= 10) {
            interfaceCtx->unk_22A = 10;
            interfaceCtx->unk_22C = 1;
        }
    }
}

u32 HealthMeter_IsCritical(void) {
    s32 var;

    if (gSaveContext.healthCapacity <= 0x50) {
        var = 0x10;
    } else if (gSaveContext.healthCapacity <= 0xA0) {
        var = 0x18;
    } else if (gSaveContext.healthCapacity <= 0xF0) {
        var = 0x20;
    } else {
        var = 0x2C;
    }

    if ((var >= gSaveContext.health) && (gSaveContext.health > 0)) {
        return true;
    } else {
        return false;
    }
}
