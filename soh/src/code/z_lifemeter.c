#include "global.h"
#include "textures/parameter_static/parameter_static.h"
#include <colors/colorPaths.h>

static Color_RGB8 sHeartsPrimColors[3];
static Color_RGB8 sHeartsEnvColors[3];
static Color_RGB8 sHeartsPrimFactors[3];
static Color_RGB8 sHeartsEnvFactors[3];
static Color_RGB8 sHeartsDDPrimColors[3];
static Color_RGB8 sHeartsDDEnvColors[3];
static Color_RGB8 sHeartsDDPrimFactors[3];
static Color_RGB8 sHeartsDDEnvFactors[3];

// Current colors for the double defense hearts
Color_RGB8 sBeatingHeartsDDPrim;
Color_RGB8 sBeatingHeartsDDEnv;
Color_RGB8 sHeartsDDPrim[2];
Color_RGB8 sHeartsDDEnv[2];

void HealthMeter_Init(GlobalContext* globalCtx) {
    Color_RGB8 heartPrimColor = *(Color_RGB8*) ResourceMgr_LoadBlobByName(colorHeartFill);
    Color_RGB8 heartEnvColor = *(Color_RGB8*) ResourceMgr_LoadBlobByName(colorHeartOutline);
    Color_RGB8 heartDDPrimColor = *(Color_RGB8*) ResourceMgr_LoadBlobByName(colorHeartDDOutline);
    Color_RGB8 heartDDEnvColor = *(Color_RGB8*) ResourceMgr_LoadBlobByName(colorHeartDDFill);

    Color_RGB8 heartBurnPrimColor = { HEARTS_BURN_PRIM_R, HEARTS_BURN_PRIM_G, HEARTS_BURN_PRIM_B };
    Color_RGB8 heartDrownPrimColor = { HEARTS_BURN_ENV_R, HEARTS_BURN_ENV_G, HEARTS_BURN_ENV_B };
    Color_RGB8 heartBurnEnvColor = { HEARTS_DROWN_PRIM_R, HEARTS_DROWN_PRIM_G, HEARTS_DROWN_PRIM_B };
    Color_RGB8 heartDrownEnvColor = { HEARTS_DROWN_ENV_R, HEARTS_DROWN_ENV_G, HEARTS_DROWN_ENV_B };

    sHeartsPrimColors[0].r = heartPrimColor.r;
    sHeartsPrimColors[0].g = heartPrimColor.g;
    sHeartsPrimColors[0].b = heartPrimColor.b;
    sHeartsPrimColors[1].r = heartBurnPrimColor.r;
    sHeartsPrimColors[1].g = heartBurnPrimColor.g;
    sHeartsPrimColors[1].b = heartBurnPrimColor.b;
    sHeartsPrimColors[2].r = heartDrownPrimColor.r;
    sHeartsPrimColors[2].g = heartDrownPrimColor.g;
    sHeartsPrimColors[2].b = heartDrownPrimColor.b;

    sHeartsEnvColors[0].r = heartEnvColor.r;
    sHeartsEnvColors[0].g = heartEnvColor.g;
    sHeartsEnvColors[0].b = heartEnvColor.b;
    sHeartsEnvColors[1].r = heartBurnEnvColor.r;
    sHeartsEnvColors[1].g = heartBurnEnvColor.g;
    sHeartsEnvColors[1].b = heartBurnEnvColor.b;
    sHeartsEnvColors[2].r = heartDrownEnvColor.r;
    sHeartsEnvColors[2].g = heartDrownEnvColor.g;
    sHeartsEnvColors[2].b = heartDrownEnvColor.b;

    sHeartsPrimFactors[0].r = heartPrimColor.r - heartPrimColor.r;
    sHeartsPrimFactors[0].g = heartPrimColor.g - heartPrimColor.g;
    sHeartsPrimFactors[0].b = heartPrimColor.b - heartPrimColor.b;
    sHeartsPrimFactors[1].r = heartBurnPrimColor.r - heartPrimColor.r;
    sHeartsPrimFactors[1].g = heartBurnPrimColor.g - heartPrimColor.g;
    sHeartsPrimFactors[1].b = heartBurnPrimColor.b - heartPrimColor.b;
    sHeartsPrimFactors[2].r = heartDrownPrimColor.r - heartPrimColor.r;
    sHeartsPrimFactors[2].g = heartDrownPrimColor.g - heartPrimColor.g;
    sHeartsPrimFactors[2].b = heartDrownPrimColor.b - heartPrimColor.b;

    sHeartsEnvFactors[0].r = heartEnvColor.r - heartEnvColor.r;
    sHeartsEnvFactors[0].g = heartEnvColor.g - heartEnvColor.g;
    sHeartsEnvFactors[0].b = heartEnvColor.b - heartEnvColor.b;
    sHeartsEnvFactors[1].r = heartBurnEnvColor.r - heartEnvColor.r;
    sHeartsEnvFactors[1].g = heartBurnEnvColor.g - heartEnvColor.g;
    sHeartsEnvFactors[1].b = heartBurnEnvColor.b - heartEnvColor.b;
    sHeartsEnvFactors[2].r = heartDrownEnvColor.r - heartEnvColor.r;
    sHeartsEnvFactors[2].g = heartDrownEnvColor.g - heartEnvColor.g;
    sHeartsEnvFactors[2].b = heartDrownEnvColor.b - heartEnvColor.b;

    sHeartsDDPrimColors[0].r = heartDDPrimColor.r;
    sHeartsDDPrimColors[0].g = heartDDPrimColor.g;
    sHeartsDDPrimColors[0].b = heartDDPrimColor.b;
    sHeartsDDPrimColors[1].r = heartBurnPrimColor.r;
    sHeartsDDPrimColors[1].g = heartBurnPrimColor.g;
    sHeartsDDPrimColors[1].b = heartBurnPrimColor.b;
    sHeartsDDPrimColors[2].r = heartDrownPrimColor.r;
    sHeartsDDPrimColors[2].g = heartDrownPrimColor.g;
    sHeartsDDPrimColors[2].b = heartDrownPrimColor.b;

    sHeartsDDEnvColors[0].r = heartDDEnvColor.r;
    sHeartsDDEnvColors[0].g = heartDDEnvColor.g;
    sHeartsDDEnvColors[0].b = heartDDEnvColor.b;
    sHeartsDDEnvColors[1].r = heartBurnEnvColor.r;
    sHeartsDDEnvColors[1].g = heartBurnEnvColor.g;
    sHeartsDDEnvColors[1].b = heartBurnEnvColor.b;
    sHeartsDDEnvColors[2].r = heartDrownEnvColor.r;
    sHeartsDDEnvColors[2].g = heartDrownEnvColor.g;
    sHeartsDDEnvColors[2].b = heartDrownEnvColor.b;

    sHeartsDDPrimFactors[0].r = heartDDPrimColor.r - heartDDPrimColor.r;
    sHeartsDDPrimFactors[0].g = heartDDPrimColor.g - heartDDPrimColor.g;
    sHeartsDDPrimFactors[0].b = heartDDPrimColor.b - heartDDPrimColor.b;
    sHeartsDDPrimFactors[1].r = heartBurnPrimColor.r - heartDDPrimColor.r;
    sHeartsDDPrimFactors[1].g = heartBurnPrimColor.g - heartDDPrimColor.g;
    sHeartsDDPrimFactors[1].b = heartBurnPrimColor.b - heartDDPrimColor.b;
    sHeartsDDPrimFactors[2].r = heartDrownPrimColor.r - heartDDPrimColor.r;
    sHeartsDDPrimFactors[2].g = heartDrownPrimColor.g - heartDDPrimColor.g;
    sHeartsDDPrimFactors[2].b = heartDrownPrimColor.b - heartDDPrimColor.b;

    sHeartsDDEnvFactors[0].r = heartDDEnvColor.r - heartDDEnvColor.r;
    sHeartsDDEnvFactors[0].g = heartDDEnvColor.g - heartDDEnvColor.g;
    sHeartsDDEnvFactors[0].b = heartDDEnvColor.b - heartDDEnvColor.b;
    sHeartsDDEnvFactors[1].r = heartBurnEnvColor.r - heartDDEnvColor.r;
    sHeartsDDEnvFactors[1].g = heartBurnEnvColor.g - heartDDEnvColor.g;
    sHeartsDDEnvFactors[1].b = heartBurnEnvColor.b - heartDDEnvColor.b;
    sHeartsDDEnvFactors[2].r = heartDrownEnvColor.r - heartDDEnvColor.r;
    sHeartsDDEnvFactors[2].g = heartDrownEnvColor.g - heartDDEnvColor.g;
    sHeartsDDEnvFactors[2].b = heartDrownEnvColor.b - heartDDEnvColor.b;

    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 = gSaveContext.health;
    interfaceCtx->unk_22A = interfaceCtx->unk_1FE = 0;
    interfaceCtx->unk_22C = interfaceCtx->unk_200 = 0;

    interfaceCtx->heartsPrimR[0] = heartPrimColor.r;
    interfaceCtx->heartsPrimG[0] = heartPrimColor.g;
    interfaceCtx->heartsPrimB[0] = heartPrimColor.b;

    interfaceCtx->heartsEnvR[0] = heartEnvColor.r;
    interfaceCtx->heartsEnvG[0] = heartEnvColor.g;
    interfaceCtx->heartsEnvB[0] = heartEnvColor.b;

    interfaceCtx->heartsPrimR[1] = heartPrimColor.r;
    interfaceCtx->heartsPrimG[1] = heartPrimColor.g;
    interfaceCtx->heartsPrimB[1] = heartPrimColor.b;

    interfaceCtx->heartsEnvR[1] = heartEnvColor.r;
    interfaceCtx->heartsEnvG[1] = heartEnvColor.g;
    interfaceCtx->heartsEnvB[1] = heartEnvColor.b;

    sHeartsDDPrim[0].r = sHeartsDDPrim[1].r = heartDDPrimColor.r;
    sHeartsDDPrim[0].g = sHeartsDDPrim[1].g = heartDDPrimColor.g;
    sHeartsDDPrim[0].b = sHeartsDDPrim[1].b = heartDDPrimColor.b;

    sHeartsDDEnv[0].r = sHeartsDDEnv[1].r = heartDDEnvColor.r;
    sHeartsDDEnv[0].g = sHeartsDDEnv[1].g = heartDDEnvColor.g;
    sHeartsDDEnv[0].b = sHeartsDDEnv[1].b = heartDDEnvColor.b;
}

void HealthMeter_Update(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    f32 factor = interfaceCtx->unk_1FE * 0.1f;
    f32 ddFactor;
    s32 type = 0;
    s32 ddType;
    s16 rFactor;
    s16 gFactor;
    s16 bFactor;

    if (interfaceCtx) {}

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

    interfaceCtx->heartsPrimR[0] = sHeartsPrimColors[0].r;
    interfaceCtx->heartsPrimG[0] = sHeartsPrimColors[0].g;
    interfaceCtx->heartsPrimB[0] = sHeartsPrimColors[0].b;

    interfaceCtx->heartsEnvR[0] = sHeartsEnvColors[0].r;
    interfaceCtx->heartsEnvG[0] = sHeartsEnvColors[0].g;
    interfaceCtx->heartsEnvB[0] = sHeartsEnvColors[0].b;

    interfaceCtx->heartsPrimR[1] = sHeartsPrimColors[type].r;
    interfaceCtx->heartsPrimG[1] = sHeartsPrimColors[type].g;
    interfaceCtx->heartsPrimB[1] = sHeartsPrimColors[type].b;

    interfaceCtx->heartsEnvR[1] = sHeartsEnvColors[type].r;
    interfaceCtx->heartsEnvG[1] = sHeartsEnvColors[type].g;
    interfaceCtx->heartsEnvB[1] = sHeartsEnvColors[type].b;

    rFactor = sHeartsPrimFactors[0].r * factor;
    gFactor = sHeartsPrimFactors[0].g * factor;
    bFactor = sHeartsPrimFactors[0].b * factor;

    interfaceCtx->beatingHeartPrim[0] = (u8)(rFactor + sHeartsPrimColors[0].r) & 0xFF;
    interfaceCtx->beatingHeartPrim[1] = (u8)(gFactor + sHeartsPrimColors[0].g) & 0xFF;
    interfaceCtx->beatingHeartPrim[2] = (u8)(bFactor + sHeartsPrimColors[0].b) & 0xFF;

    rFactor = sHeartsEnvFactors[0].r * factor;
    gFactor = sHeartsEnvFactors[0].g * factor;
    bFactor = sHeartsEnvFactors[0].b * factor;

    if (1) {}
    ddType = type;

    interfaceCtx->beatingHeartEnv[0] = (u8)(rFactor + sHeartsEnvColors[0].r) & 0xFF;
    interfaceCtx->beatingHeartEnv[1] = (u8)(gFactor + sHeartsEnvColors[0].g) & 0xFF;
    interfaceCtx->beatingHeartEnv[2] = (u8)(bFactor + sHeartsEnvColors[0].b) & 0xFF;

    sHeartsDDPrim[0].r = sHeartsDDPrimColors[0].r;
    sHeartsDDPrim[0].g = sHeartsDDPrimColors[0].g;
    sHeartsDDPrim[0].b = sHeartsDDPrimColors[0].b;

    sHeartsDDEnv[0].r = sHeartsDDEnvColors[0].r;
    sHeartsDDEnv[0].g = sHeartsDDEnvColors[0].g;
    sHeartsDDEnv[0].b = sHeartsDDEnvColors[0].b;

    sHeartsDDPrim[1].r = sHeartsDDPrimColors[ddType].r;
    sHeartsDDPrim[1].g = sHeartsDDPrimColors[ddType].g;
    sHeartsDDPrim[1].b = sHeartsDDPrimColors[ddType].b;

    sHeartsDDEnv[1].r = sHeartsDDEnvColors[ddType].r;
    sHeartsDDEnv[1].g = sHeartsDDEnvColors[ddType].g;
    sHeartsDDEnv[1].b = sHeartsDDEnvColors[ddType].b;

    rFactor = sHeartsDDPrimFactors[ddType].r * ddFactor;
    gFactor = sHeartsDDPrimFactors[ddType].g * ddFactor;
    bFactor = sHeartsDDPrimFactors[ddType].b * ddFactor;

    sBeatingHeartsDDPrim.r = (u8)(rFactor + sHeartsDDPrimColors[0].r) & 0xFF;
    sBeatingHeartsDDPrim.g = (u8)(gFactor + sHeartsDDPrimColors[0].g) & 0xFF;
    sBeatingHeartsDDPrim.b = (u8)(bFactor + sHeartsDDPrimColors[0].b) & 0xFF;

    rFactor = sHeartsDDEnvFactors[ddType].r * ddFactor;
    gFactor = sHeartsDDEnvFactors[ddType].g * ddFactor;
    bFactor = sHeartsDDEnvFactors[ddType].b * ddFactor;

    sBeatingHeartsDDEnv.r = (u8)(rFactor + sHeartsDDEnvColors[0].r) & 0xFF;
    sBeatingHeartsDDEnv.g = (u8)(gFactor + sHeartsDDEnvColors[0].g) & 0xFF;
    sBeatingHeartsDDEnv.b = (u8)(bFactor + sHeartsDDEnvColors[0].b) & 0xFF;
}

s32 func_80078E18(GlobalContext* globalCtx) {
    gSaveContext.health = globalCtx->interfaceCtx.unk_226;
    return 1;
}

s32 func_80078E34(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 += 0x10;

    if (interfaceCtx->unk_226 >= gSaveContext.health) {
        interfaceCtx->unk_226 = gSaveContext.health;
        return 1;
    }

    return 0;
}

s32 func_80078E84(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    if (interfaceCtx->unk_228 != 0) {
        interfaceCtx->unk_228--;
    } else {
        interfaceCtx->unk_228 = 0x140;
        interfaceCtx->unk_226 -= 0x10;
        if (interfaceCtx->unk_226 <= 0) {
            interfaceCtx->unk_226 = 0;
            globalCtx->damagePlayer(globalCtx, -(gSaveContext.health + 1));
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

void HealthMeter_Draw(GlobalContext* globalCtx) {
    s32 pad[5];
    void* heartBgImg;
    u32 curColorSet;
    f32 offsetX;
    f32 offsetY;
    s32 i;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp4;
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    Vtx* sp154 = interfaceCtx->beatingHeartVtx;
    s32 curHeartFraction = gSaveContext.health % 0x10;
    s16 totalHeartCount = gSaveContext.healthCapacity / 0x10;
    s16 fullHeartCount = gSaveContext.health / 0x10;
    s32 pad2;
    f32 sp144 = interfaceCtx->unk_22A * 0.1f;
    s32 curCombineModeSet = 0;
    u8* curBgImgLoaded = NULL;
    s32 ddHeartCountMinusOne = gSaveContext.inventory.defenseHearts - 1;

    OPEN_DISPS(gfxCtx, "../z_lifemeter.c", 353);

    if (!(gSaveContext.health % 0x10)) {
        fullHeartCount--;
    }

    curColorSet = -1;
    offsetY = 0.0f;
    offsetX = OTRGetDimensionFromLeftEdge(0.0f);

    for (i = 0; i < totalHeartCount; i++) {
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
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[0].r, sHeartsDDPrim[0].g, sHeartsDDPrim[0].b,
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[0].r, sHeartsDDEnv[0].g, sHeartsDDEnv[0].b, 255);
                }
            } else if (i == fullHeartCount) {
                if (curColorSet != 5) {
                    curColorSet = 5;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sBeatingHeartsDDPrim.r, sBeatingHeartsDDPrim.g,
                                    sBeatingHeartsDDPrim.b, interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sBeatingHeartsDDEnv.r, sBeatingHeartsDDEnv.g,
                                   sBeatingHeartsDDEnv.b, 255);
                }
            } else if (i > fullHeartCount) {
                if (curColorSet != 6) {
                    curColorSet = 6;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[0].r, sHeartsDDPrim[0].g, sHeartsDDPrim[0].b,
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[0].r, sHeartsDDEnv[0].g, sHeartsDDEnv[0].b, 255);
                }
            } else {
                if (curColorSet != 7) {
                    curColorSet = 7;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sHeartsDDPrim[1].r, sHeartsDDPrim[1].g, sHeartsDDPrim[1].b,
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, sHeartsDDEnv[1].r, sHeartsDDEnv[1].g, sHeartsDDEnv[1].b, 255);
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
                    func_80094520(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 3) {
                    curCombineModeSet = 3;
                    func_80094520(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            temp3 = 26.0f + offsetY;
            temp2 = 30.0f + offsetX;
            temp4 = 1.0f;
            temp4 /= 0.68f;
            temp4 *= 1 << 10;
            temp1 = 8.0f;
            temp1 *= 0.68f;
            gSPWideTextureRectangle(OVERLAY_DISP++, (s32)((temp2 - temp1) * 4), (s32)((temp3 - temp1) * 4),
                                (s32)((temp2 + temp1) * 4), (s32)((temp3 + temp1) * 4), G_TX_RENDERTILE, 0, 0,
                                (s32)temp4, (s32)temp4);
        } else {
            if ((ddHeartCountMinusOne < 0) || (i > ddHeartCountMinusOne)) {
                if (curCombineModeSet != 2) {
                    curCombineModeSet = 2;
                    func_80094A14(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 4) {
                    curCombineModeSet = 4;
                    func_80094A14(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            {
                Mtx* matrix = Graph_Alloc(gfxCtx, sizeof(Mtx));
                Matrix_SetTranslateScaleMtx2(matrix, 1.0f - (0.32f * sp144), 1.0f - (0.32f * sp144),
                                             1.0f - (0.32f * sp144), -130.0f + offsetX,
                                             94.5f - offsetY, 0.0f);
                gSPMatrix(OVERLAY_DISP++, matrix, G_MTX_MODELVIEW | G_MTX_LOAD);
                gSPVertex(OVERLAY_DISP++, sp154, 4, 0);
                gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
            }
        }

        offsetX += 10.0f;
        if (i == 9) {
            offsetY += 10.0f;
            offsetX = OTRGetDimensionFromLeftEdge(0.0f);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_lifemeter.c", 606);
}

void HealthMeter_HandleCriticalAlarm(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    if (interfaceCtx->unk_22C != 0) {
        interfaceCtx->unk_22A--;
        if (interfaceCtx->unk_22A <= 0) {
            interfaceCtx->unk_22A = 0;
            interfaceCtx->unk_22C = 0;
            if (!Player_InCsMode(globalCtx) && (globalCtx->pauseCtx.state == 0) &&
                (globalCtx->pauseCtx.debugState == 0) && HealthMeter_IsCritical() && !Gameplay_InCsMode(globalCtx)) {
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
