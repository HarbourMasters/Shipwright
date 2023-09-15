#include "global.h"
#include <libultraship/libultra.h>
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "soh/frame_interpolation.h"

typedef enum {
    /* 0 */ LENS_FLARE_CIRCLE0,
    /* 1 */ LENS_FLARE_CIRCLE1,
    /* 2 */ LENS_FLARE_RING
} LensFlareType;

typedef enum {
    /* 0x00 */ LIGHTNING_BOLT_START,
    /* 0x01 */ LIGHTNING_BOLT_WAIT,
    /* 0x02 */ LIGHTNING_BOLT_DRAW,
    /* 0xFF */ LIGHTNING_BOLT_INACTIVE = 0xFF
} LightningBoltState;

typedef struct {
    /* 0x00 */ u16 startTime;
    /* 0x02 */ u16 endTime;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
} struct_8011FB48; // size = 0x6

typedef struct {
    /* 0x00 */ u8 state;
    /* 0x04 */ Vec3f offset;
    /* 0x10 */ Vec3f pos;
    /* 0x1C */ s8 pitch;
    /* 0x1D */ s8 roll;
    /* 0x1E */ u8 textureIndex;
    /* 0x1F */ u8 delayTimer;
} LightningBolt; // size = 0x20

typedef struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} Struct_8011FAF0; // size = 0x8

Struct_8011FAF0 D_8011FAF0[] = {
    { 6, 0x00000000 }, { 5, 0x00020000 }, { 4, 0x00030000 }, { 3, 0x00038000 },
    { 2, 0x0003C000 }, { 1, 0x0003E000 }, { 0, 0x0003F000 }, { 0, 0x0003F800 },
};

u8 gWeatherMode = 0; // "E_wether_flg"

u8 D_8011FB34 = 0;

u8 D_8011FB38 = 0;

u8 gSkyboxBlendingEnabled = false;

u16 gTimeIncrement = 0;

u16 D_8011FB44 = 0xFFFC;

struct_8011FB48 D_8011FB48[][7] = {
    {
        { 0x0000, 0x2AAC, 3, 3 },
        { 0x2AAC, 0x4000, 3, 0 },
        { 0x4000, 0x5556, 0, 1 },
        { 0x5556, 0xAAAB, 1, 1 },
        { 0xAAAB, 0xB556, 1, 2 },
        { 0xB556, 0xCAAC, 2, 3 },
        { 0xCAAC, 0xFFFF, 3, 3 },
    },
    {
        { 0x0000, 0x2AAC, 7, 7 },
        { 0x2AAC, 0x4000, 7, 4 },
        { 0x4000, 0x5556, 4, 5 },
        { 0x5556, 0xAAAB, 5, 5 },
        { 0xAAAB, 0xB556, 5, 6 },
        { 0xB556, 0xCAAC, 6, 7 },
        { 0xCAAC, 0xFFFF, 7, 7 },
    },
    {
        { 0x0000, 0x2AAC, 11, 11 },
        { 0x2AAC, 0x4000, 11, 8 },
        { 0x4000, 0x5556, 8, 9 },
        { 0x5556, 0xAAAB, 9, 9 },
        { 0xAAAB, 0xB556, 9, 10 },
        { 0xB556, 0xCAAC, 10, 11 },
        { 0xCAAC, 0xFFFF, 11, 11 },
    },
    {
        { 0x0000, 0x2AAC, 15, 15 },
        { 0x2AAC, 0x4000, 15, 12 },
        { 0x4000, 0x5556, 12, 13 },
        { 0x5556, 0xAAAB, 13, 13 },
        { 0xAAAB, 0xB556, 13, 14 },
        { 0xB556, 0xCAAC, 14, 15 },
        { 0xCAAC, 0xFFFF, 15, 15 },
    },
    {
        { 0x0000, 0x2AAC, 23, 23 },
        { 0x2AAC, 0x4000, 23, 20 },
        { 0x4000, 0x5556, 20, 21 },
        { 0x5556, 0xAAAB, 21, 21 },
        { 0xAAAB, 0xB556, 21, 22 },
        { 0xB556, 0xCAAC, 22, 23 },
        { 0xCAAC, 0xFFFF, 23, 23 },
    },
};

struct_8011FC1C D_8011FC1C[][9] = {
    {
        { 0x0000, 0x2AAC, 0, 3, 3 },
        { 0x2AAC, 0x3556, 1, 3, 0 },
        { 0x3556, 0x4000, 0, 0, 0 },
        { 0x4000, 0x5556, 1, 0, 1 },
        { 0x5556, 0xAAAB, 0, 1, 1 },
     // { 0x5556, 0xAAAB, 0, 2, 2 },
        { 0xAAAB, 0xB556, 1, 1, 2 },
        { 0xB556, 0xC001, 0, 2, 2 },
        { 0xC001, 0xCAAC, 1, 2, 3 },
        { 0xCAAC, 0xFFFF, 0, 3, 3 },
    },
    {
        { 0x0000, 0x2AAC, 0, 7, 7 },
        { 0x2AAC, 0x3556, 1, 7, 4 },
        { 0x3556, 0x4000, 0, 4, 4 },
        { 0x4000, 0x5556, 1, 4, 5 },
        { 0x5556, 0xAAAB, 0, 5, 5 },
        { 0xAAAB, 0xB556, 1, 5, 6 },
        { 0xB556, 0xC001, 0, 6, 6 },
        { 0xC001, 0xCAAC, 1, 6, 7 },
        { 0xCAAC, 0xFFFF, 0, 7, 7 },
    },
    {
        { 0x0000, 0x1556, 0, 3, 3 },
        { 0x1556, 0x2AAC, 1, 3, 0 },
        { 0x2AAC, 0x5556, 0, 0, 0 },
        { 0x5556, 0x6AAB, 1, 0, 1 },
        { 0x6AAB, 0x9556, 0, 1, 1 },
        { 0x9556, 0xAAAB, 1, 1, 2 },
        { 0xAAAB, 0xD556, 0, 2, 2 },
        { 0xD556, 0xEAAB, 1, 2, 3 },
        { 0xEAAB, 0xFFFF, 0, 3, 3 },
    },
    {
        { 0x0000, 0x3556, 0, 11, 11 },
        { 0x3556, 0x4000, 1, 11, 8 },
        { 0x4000, 0x4AAB, 0, 8, 8 },
        { 0x4AAB, 0x5556, 1, 8, 9 },
        { 0x5556, 0xAAAB, 0, 9, 9 },
        { 0xAAAB, 0xB556, 1, 9, 10 },
        { 0xB556, 0xC001, 0, 10, 10 },
        { 0xC001, 0xCAAC, 1, 10, 11 },
        { 0xCAAC, 0xFFFF, 0, 11, 11 },
    },
};

SkyboxFile gSkyboxFiles[] = {
    {
        ROM_FILE(vr_fine0_static),
        ROM_FILE(vr_fine0_pal_static),
    },
    {
        ROM_FILE(vr_fine1_static),
        ROM_FILE(vr_fine1_pal_static),
    },
    {
        ROM_FILE(vr_fine2_static),
        ROM_FILE(vr_fine2_pal_static),
    },
    {
        ROM_FILE(vr_fine3_static),
        ROM_FILE(vr_fine3_pal_static),
    },
    {
        ROM_FILE(vr_cloud0_static),
        ROM_FILE(vr_cloud0_pal_static),
    },
    {
        ROM_FILE(vr_cloud1_static),
        ROM_FILE(vr_cloud1_pal_static),
    },
    {
        ROM_FILE(vr_cloud2_static),
        ROM_FILE(vr_cloud2_pal_static),
    },
    {
        ROM_FILE(vr_cloud3_static),
        ROM_FILE(vr_cloud3_pal_static),
    },
    {
        ROM_FILE(vr_holy0_static),
        ROM_FILE(vr_holy0_pal_static),
    },
};

u8 D_8011FDCC = 0;
u8 D_8011FDD0 = 0;
f32 D_8011FDD4 = 0.0f;

u8 gCustomLensFlareOn;
Vec3f gCustomLensFlarePos;
s16 gLensFlareUnused;
s16 gLensFlareScale;
f32 gLensFlareColorIntensity;
s16 gLensFlareScreenFillAlpha;
LightningBolt sLightningBolts[3];
LightningStrike gLightningStrike;
s16 sLightningFlashAlpha;
s16 D_8015FD7E;
s16 D_8015FD80;
LightNode* sNGameOverLightNode;
LightInfo sNGameOverLightInfo;
LightNode* sSGameOverLightNode;
LightInfo sSGameOverLightInfo;
u8 sGameOverLightsIntensity;
u16 D_8015FDB0;

void LoadSkyboxPalette(SkyboxContext* skyboxCtx, int paletteIndex, char* palTex, int width,
                       int height);
void LoadSkyboxTex(SkyboxContext* skyboxCtx, int segmentIndex, int imageIndex, char* tex, int width, int height, int offsetW, int offsetH);
void Skybox_Update(SkyboxContext* skyboxCtx);

s32 func_8006F0A0(s32 a0) {
    s32 ret = ((a0 >> 4 & 0x7FF) << D_8011FAF0[a0 >> 15 & 7].unk_00) + D_8011FAF0[a0 >> 15 & 7].unk_04;

    return ret;
}

u16 Environment_GetPixelDepth(s32 x, s32 y) {
    return OTRGetPixelDepth(x, y);
}

void Environment_GraphCallback(GraphicsContext* gfxCtx, void* param) {
    PlayState* play = (PlayState*)param;

    OTRGetPixelDepthPrepare(D_8015FD7E, D_8015FD80);
    Lights_GlowCheckPrepare(play);

    D_8011FB44 = Environment_GetPixelDepth(D_8015FD7E, D_8015FD80);
    Lights_GlowCheck(play);
}

void Environment_Init(PlayState* play2, EnvironmentContext* envCtx, s32 unused) {
    u8 i;
    PlayState* play = play2;

    gSaveContext.sunsSongState = SUNSSONG_INACTIVE;

    if (((void)0, gSaveContext.dayTime) > 0xC000 || ((void)0, gSaveContext.dayTime) < 0x4555) {
        ((void)0, gSaveContext.nightFlag = 1);
    } else {
        ((void)0, gSaveContext.nightFlag = 0);
    }

    play->state.gfxCtx->callback = Environment_GraphCallback;
    play->state.gfxCtx->callbackParam = play;

    Lights_DirectionalSetInfo(&envCtx->dirLight1, 80, 80, 80, 80, 80, 80);
    LightContext_InsertLight(play, &play->lightCtx, &envCtx->dirLight1);

    Lights_DirectionalSetInfo(&envCtx->dirLight2, 80, 80, 80, 80, 80, 80);
    LightContext_InsertLight(play, &play->lightCtx, &envCtx->dirLight2);

    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;
    envCtx->unk_19 = 0;
    envCtx->unk_1A = 0;
    envCtx->unk_21 = 0;
    envCtx->unk_22 = 0;
    envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
    envCtx->unk_1F = 0;
    envCtx->unk_20 = 0;
    envCtx->unk_84 = 0.0f;
    envCtx->unk_88 = 0.0f;
    envCtx->unk_BD = 0;
    envCtx->unk_BE = 0;
    envCtx->unk_D8 = 1.0f;
    envCtx->unk_DC = 0;
    envCtx->gloomySkyMode = 0;
    envCtx->unk_DE = 0;
    envCtx->lightningMode = LIGHTNING_MODE_OFF;
    envCtx->unk_E0 = 0;
    envCtx->fillScreen = false;
    envCtx->screenFillColor[0] = 0;
    envCtx->screenFillColor[1] = 0;
    envCtx->screenFillColor[2] = 0;
    envCtx->screenFillColor[3] = 0;
    envCtx->customSkyboxFilter = false;
    envCtx->skyboxFilterColor[0] = 0;
    envCtx->skyboxFilterColor[1] = 0;
    envCtx->skyboxFilterColor[2] = 0;
    envCtx->skyboxFilterColor[3] = 0;
    envCtx->sandstormState = 0;
    envCtx->sandstormPrimA = 0;
    envCtx->sandstormEnvA = 0;

    gLightningStrike.state = LIGHTNING_STRIKE_WAIT;
    gLightningStrike.flashRed = 0;
    gLightningStrike.flashGreen = 0;
    gLightningStrike.flashBlue = 0;

    sLightningFlashAlpha = 0;

    gSaveContext.cutsceneTransitionControl = 0;

    envCtx->adjAmbientColor[0] = envCtx->adjAmbientColor[1] = envCtx->adjAmbientColor[2] = envCtx->adjLight1Color[0] =
        envCtx->adjLight1Color[1] = envCtx->adjLight1Color[2] = envCtx->adjFogColor[0] = envCtx->adjFogColor[1] =
            envCtx->adjFogColor[2] = envCtx->adjFogNear = envCtx->adjFogFar = 0;

    envCtx->sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    envCtx->sunPos.y = +(Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
    envCtx->sunPos.z = +(Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f) * 25.0f;

    envCtx->windDirection.x = 80;
    envCtx->windDirection.y = 80;
    envCtx->windDirection.z = 80;

    envCtx->blendIndoorLights = false;
    envCtx->unk_BF = 0xFF;
    envCtx->unk_D6 = 0xFFFF;
    R_ENV_TIME_INCREMENT = gTimeIncrement = envCtx->timeIncrement = 0;
    R_ENV_DISABLE_DBG = true;

    if (CREG(3) != 0) {
        gSaveContext.chamberCutsceneNum = CREG(3) - 1;
    }

    play->envCtx.unk_EE[0] = 0;
    play->envCtx.unk_EE[1] = 0;
    play->envCtx.unk_EE[2] = 0;
    play->envCtx.unk_EE[3] = 0;

    play->envCtx.unk_F2[0] = 0;

    if (gSaveContext.retainWeatherMode != 0) {
        if (((void)0, gSaveContext.sceneSetupIndex) < 4) {
            switch (gWeatherMode) {
                case 1:
                    envCtx->unk_17 = 1;
                    envCtx->unk_18 = 1;
                    envCtx->unk_1F = 3;
                    envCtx->unk_20 = 3;
                    play->envCtx.unk_EE[3] = 0;
                    play->envCtx.unk_EE[2] = 0;
                    break;
                case 2:
                case 3:
                case 4:
                    envCtx->unk_17 = 1;
                    envCtx->unk_18 = 1;
                    envCtx->unk_1F = 2;
                    envCtx->unk_20 = 2;
                    play->envCtx.unk_EE[3] = 0;
                    play->envCtx.unk_EE[2] = 0;
                    break;
                case 5:
                    envCtx->unk_17 = 1;
                    envCtx->unk_18 = 1;
                    envCtx->unk_1F = 4;
                    envCtx->unk_20 = 4;
                    play->envCtx.unk_EE[3] = 0;
                    play->envCtx.unk_EE[2] = 0;
                    break;
                default:
                    break;
            }

            if (play->skyboxId == SKYBOX_NORMAL_SKY) {
                if (gWeatherMode == 3) {
                    play->envCtx.unk_EE[2] = play->envCtx.unk_EE[3] = 0x40;
                } else if (gWeatherMode == 4) {
                    play->envCtx.unk_EE[0] = 0x14;
                    play->envCtx.unk_EE[1] = 0x14;
                } else if (gWeatherMode == 5) {
                    play->envCtx.unk_EE[0] = 0x1E;
                    play->envCtx.unk_EE[1] = 0x1E;
                }
            }
        }
    } else {
        gWeatherMode = 0;
    }

    D_8011FB38 = 0;
    D_8011FB34 = 0;
    gSkyboxBlendingEnabled = false;
    gSaveContext.retainWeatherMode = 0;
    R_ENV_LIGHT1_DIR(0) = 80;
    R_ENV_LIGHT1_DIR(1) = 80;
    R_ENV_LIGHT1_DIR(2) = 80;
    R_ENV_LIGHT2_DIR(0) = -80;
    R_ENV_LIGHT2_DIR(1) = -80;
    R_ENV_LIGHT2_DIR(2) = -80;
    cREG(9) = 10;
    cREG(10) = 0;
    cREG(11) = 0;
    cREG(12) = 0;
    cREG(13) = 0;
    cREG(14) = 0;
    D_8015FCC8 = 1;

    for (i = 0; i < ARRAY_COUNT(sLightningBolts); i++) {
        sLightningBolts[i].state = LIGHTNING_BOLT_INACTIVE;
    }

    play->roomCtx.unk_74[0] = 0;
    play->roomCtx.unk_74[1] = 0;

    for (i = 0; i < ARRAY_COUNT(play->csCtx.npcActions); i++) {
        play->csCtx.npcActions[i] = 0;
    }

    if (Object_GetIndex(&play->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP) < 0 && !play->envCtx.sunMoonDisabled) {
        play->envCtx.sunMoonDisabled = true;
        // "Sun setting other than field keep! So forced release!"
        osSyncPrintf(VT_COL(YELLOW, BLACK) "\n\nフィールド常駐以外、太陽設定！よって強制解除！\n" VT_RST);
    }

    gCustomLensFlareOn = false;
    func_800AA15C();
}

u8 Environment_SmoothStepToU8(u8* pvalue, u8 target, u8 scale, u8 step, u8 minStep) {
    s16 stepSize = 0;
    s16 diff = target - *pvalue;

    if (target != *pvalue) {
        stepSize = diff / scale;
        if ((stepSize >= (s16)minStep) || ((s16)-minStep >= stepSize)) {
            if ((s16)step < stepSize) {
                stepSize = step;
            }
            if ((s16)-step > stepSize) {
                stepSize = -step;
            }
            *pvalue += (u8)stepSize;
        } else {
            if (stepSize < (s16)minStep) {
                stepSize = minStep;
                *pvalue += (u8)stepSize;
                if (target < *pvalue) {
                    *pvalue = target;
                }
            }
            if ((s16)-minStep < stepSize) {
                stepSize = -minStep;
                *pvalue += (u8)stepSize;
                if (*pvalue < target) {
                    *pvalue = target;
                }
            }
        }
    }
    return diff;
}

u8 Environment_SmoothStepToS8(s8* pvalue, s8 target, u8 scale, u8 step, u8 minStep) {
    s16 stepSize = 0;
    s16 diff = target - *pvalue;

    if (target != *pvalue) {
        stepSize = diff / scale;
        if ((stepSize >= (s16)minStep) || ((s16)-minStep >= stepSize)) {
            if ((s16)step < stepSize) {
                stepSize = step;
            }
            if ((s16)-step > stepSize) {
                stepSize = -step;
            }
            *pvalue += (s8)stepSize;
        } else {
            if (stepSize < (s16)minStep) {
                stepSize = minStep;
                *pvalue += (s8)stepSize;
                if (target < *pvalue) {
                    *pvalue = target;
                }
            }
            if ((s16)-minStep < stepSize) {
                stepSize = -minStep;
                *pvalue += (s8)stepSize;
                if (*pvalue < target) {
                    *pvalue = target;
                }
            }
        }
    }
    return diff;
}

f32 Environment_LerpWeight(u16 max, u16 min, u16 val) {
    f32 diff = max - min;
    f32 ret;

    if (diff != 0.0f) {
        ret = 1.0f - (max - val) / diff;

        if (!(ret >= 1.0f)) {
            return ret;
        }
    }

    return 1.0f;
}

f32 Environment_LerpWeightAccelDecel(u16 endFrame, u16 startFrame, u16 curFrame, u16 accelDuration, u16 decelDuration) {
    f32 endFrameF;
    f32 startFrameF;
    f32 curFrameF;
    f32 accelDurationF;
    f32 decelDurationF;
    f32 totalFrames;
    f32 temp;
    f32 framesElapsed;
    f32 ret;

    if (curFrame <= startFrame) {
        return 0.0f;
    }

    if (curFrame >= endFrame) {
        return 1.0f;
    }

    endFrameF = (s32)endFrame;
    startFrameF = (s32)startFrame;
    curFrameF = (s32)curFrame;
    totalFrames = endFrameF - startFrameF;
    framesElapsed = curFrameF - startFrameF;
    accelDurationF = (s32)accelDuration;
    decelDurationF = (s32)decelDuration;

    if ((startFrameF >= endFrameF) || (accelDurationF + decelDurationF > totalFrames)) {
        // "The frame relation between end_frame and start_frame is wrong!!!"
        osSyncPrintf(VT_COL(RED, WHITE) "\nend_frameとstart_frameのフレーム関係がおかしい!!!" VT_RST);
        osSyncPrintf(VT_COL(RED, WHITE) "\nby get_parcent_forAccelBrake!!!!!!!!!" VT_RST);

        return 0.0f;
    }

    temp = 1.0f / ((totalFrames * 2.0f) - accelDurationF - decelDurationF);

    if (accelDurationF != 0.0f) {
        if (framesElapsed <= accelDurationF) {
            return temp * framesElapsed * framesElapsed / accelDurationF;
        }
        ret = temp * accelDurationF;
    } else {
        ret = 0.0f;
    }

    if (framesElapsed <= totalFrames - decelDurationF) {
        ret += 2.0f * temp * (framesElapsed - accelDurationF);
        return ret;
    }

    ret += 2.0f * temp * (totalFrames - accelDurationF - decelDurationF);

    if (decelDurationF != 0.0f) {
        ret += temp * decelDurationF;
        if (framesElapsed < totalFrames) {
            ret -= temp * (totalFrames - framesElapsed) * (totalFrames - framesElapsed) / decelDurationF;
        }
    }

    return ret;
}

void func_8006FB94(EnvironmentContext* envCtx, u8 unused) {
    if (envCtx->gloomySkyMode != 0) {
        switch (envCtx->unk_DE) {
            case 0:
                if ((envCtx->gloomySkyMode == 1) && !gSkyboxBlendingEnabled) {
                    envCtx->unk_19 = 1;
                    envCtx->unk_17 = 0;
                    envCtx->unk_18 = 1;
                    envCtx->unk_1A = 100;
                    envCtx->unk_21 = 1;
                    envCtx->unk_1F = 0;
                    envCtx->unk_20 = 2;
                    D_8011FB34 = 2;
                    envCtx->unk_22 = envCtx->unk_24 = 100;
                    envCtx->unk_DE++;
                }
                break;
            case 1:
                if (!gSkyboxBlendingEnabled && (envCtx->gloomySkyMode == 2)) {
                    gWeatherMode = 0;
                    envCtx->unk_19 = 1;
                    envCtx->unk_17 = 1;
                    envCtx->unk_18 = 0;
                    envCtx->unk_1A = 100;
                    envCtx->unk_21 = 1;
                    envCtx->unk_1F = 2;
                    envCtx->unk_20 = 0;
                    D_8011FB34 = 0;
                    envCtx->unk_22 = envCtx->unk_24 = 100;
                    envCtx->unk_EE[0] = 0;
                    envCtx->gloomySkyMode = 0;
                    envCtx->unk_DE = 0;
                }
                break;
        }
    }
}

extern SkyboxTableEntry sSkyboxTable[];

void Environment_UpdateSkybox(PlayState* play, u8 skyboxId, EnvironmentContext* envCtx, SkyboxContext* skyboxCtx) {
    size_t size;
    u8 i;
    u8 newSkybox1Index = 0xFF;
    u8 newSkybox2Index = 0xFF;
    u8 skyboxBlend = 0;

    if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        envCtx->unk_17 = 3;

        for (i = 0; i < ARRAY_COUNT(D_8011FC1C[envCtx->unk_17]); i++) {
            if (gSaveContext.skyboxTime >= D_8011FC1C[envCtx->unk_17][i].startTime &&
                (gSaveContext.skyboxTime < D_8011FC1C[envCtx->unk_17][i].endTime ||
                 D_8011FC1C[envCtx->unk_17][i].endTime == 0xFFFF)) {
                if (D_8011FC1C[envCtx->unk_17][i].blend) {
                    envCtx->skyboxBlend = Environment_LerpWeight(D_8011FC1C[envCtx->unk_17][i].endTime,
                                                                 D_8011FC1C[envCtx->unk_17][i].startTime,
                                                                 ((void)0, gSaveContext.skyboxTime)) *
                                          255;
                } else {
                    envCtx->skyboxBlend = 0;
                }
                break;
            }
        }
    } else if (skyboxId == SKYBOX_NORMAL_SKY && !envCtx->skyboxDisabled) {
        for (i = 0; i < ARRAY_COUNT(D_8011FC1C[envCtx->unk_17]); i++) {
            if (gSaveContext.skyboxTime >= D_8011FC1C[envCtx->unk_17][i].startTime &&
                (gSaveContext.skyboxTime < D_8011FC1C[envCtx->unk_17][i].endTime ||
                 D_8011FC1C[envCtx->unk_17][i].endTime == 0xFFFF)) {
                newSkybox1Index = D_8011FC1C[envCtx->unk_17][i].skybox1Index;
                newSkybox2Index = D_8011FC1C[envCtx->unk_17][i].skybox2Index;
                gSkyboxBlendingEnabled = D_8011FC1C[envCtx->unk_17][i].blend;

                if (gSkyboxBlendingEnabled) {
                    skyboxBlend = Environment_LerpWeight(D_8011FC1C[envCtx->unk_17][i].endTime,
                                                         D_8011FC1C[envCtx->unk_17][i].startTime,
                                                         ((void)0, gSaveContext.skyboxTime)) *
                                  255;
                } else {
                    skyboxBlend = Environment_LerpWeight(D_8011FC1C[envCtx->unk_17][i].endTime,
                                                         D_8011FC1C[envCtx->unk_17][i].startTime,
                                                         ((void)0, gSaveContext.skyboxTime)) *
                                  255;

                    skyboxBlend = (skyboxBlend < 0x80) ? 0xFF : 0;

                    if ((envCtx->unk_19 != 0) && (envCtx->unk_19 < 3)) {
                        envCtx->unk_19++;
                        skyboxBlend = 0;
                    }
                }
                break;
            }
        }

        func_8006FB94(envCtx, skyboxBlend);

        if (envCtx->unk_19 >= 3) {
            newSkybox1Index = D_8011FC1C[envCtx->unk_17][i].skybox1Index;
            newSkybox2Index = D_8011FC1C[envCtx->unk_18][i].skybox2Index;

            skyboxBlend = ((f32)envCtx->unk_24 - envCtx->unk_1A--) / (f32)envCtx->unk_24 * 255;

            if (envCtx->unk_1A <= 0) {
                envCtx->unk_19 = 0;
                envCtx->unk_17 = envCtx->unk_18;
            }
        }

        if (newSkybox1Index == 0xFF) {
            // "Environment VR data acquisition failed! Report to Sasaki!"
            osSyncPrintf(VT_COL(RED, WHITE) "\n環境ＶＲデータ取得失敗！ ささきまでご報告を！" VT_RST);
        }

        if ((envCtx->skybox1Index != newSkybox1Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_FILE1_START;

            SkyboxTableEntry entryA = sSkyboxTable[newSkybox1Index];

            for (int i = 0; i < 5; i++)
                LoadSkyboxTex(skyboxCtx, 0, i, entryA.textures[i], 128, i == 4 ? 128 : 64, 128, 64);

            Skybox_Update(skyboxCtx);

            envCtx->skybox1Index = newSkybox1Index;

            //size = gSkyboxFiles[newSkybox1Index].file.vromEnd - gSkyboxFiles[newSkybox1Index].file.vromStart;

            //osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
            //DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->staticSegments[0],
                                //gSkyboxFiles[newSkybox1Index].file.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                //__FILE__, __LINE__);
        }

        if ((envCtx->skybox2Index != newSkybox2Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_FILE2_START;

            SkyboxTableEntry entryA = sSkyboxTable[newSkybox2Index];

            for (int i = 0; i < 5; i++)
                LoadSkyboxTex(skyboxCtx, 1, i, entryA.textures[i], 128, i == 4 ? 128 : 64, 128, 64);

            Skybox_Update(skyboxCtx);

            envCtx->skybox2Index = newSkybox2Index;

            //size = gSkyboxFiles[newSkybox2Index].file.vromEnd - gSkyboxFiles[newSkybox2Index].file.vromStart;
            //osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
            //DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->staticSegments[1],
                                //gSkyboxFiles[newSkybox2Index].file.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                //__FILE__, __LINE__);
        }

        if (envCtx->skyboxDmaState == SKYBOX_DMA_FILE1_DONE) {
            envCtx->skyboxDmaState = SKYBOX_DMA_PAL1_START;

            if ((newSkybox1Index & 1) ^ ((newSkybox1Index & 4) >> 2)) {

                SkyboxTableEntry entryA = sSkyboxTable[newSkybox1Index];
                LoadSkyboxPalette(skyboxCtx, 0, entryA.palettes[0], 16, 8);

                //size = gSkyboxFiles[newSkybox1Index].palette.vromEnd - gSkyboxFiles[newSkybox1Index].palette.vromStart;
                //osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                //DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->palettes,
                                    //gSkyboxFiles[newSkybox1Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                    //__FILE__, __LINE__);
            } else {
                SkyboxTableEntry entryA = sSkyboxTable[newSkybox1Index];
                LoadSkyboxPalette(skyboxCtx, 1, entryA.palettes[0], 16, 8);

                //size = gSkyboxFiles[newSkybox1Index].palette.vromEnd - gSkyboxFiles[newSkybox1Index].palette.vromStart;
                //osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                //DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->palettes + size,
                                    //gSkyboxFiles[newSkybox1Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                    //__FILE__, __LINE__);
            }

            Skybox_Update(skyboxCtx);
        }

        if (envCtx->skyboxDmaState == SKYBOX_DMA_FILE2_DONE) {
            envCtx->skyboxDmaState = SKYBOX_DMA_PAL2_START;

            if ((newSkybox2Index & 1) ^ ((newSkybox2Index & 4) >> 2))
            {
                SkyboxTableEntry entryA = sSkyboxTable[newSkybox2Index];
                LoadSkyboxPalette(skyboxCtx, 0, entryA.palettes[0], 16, 8);

                /*size = gSkyboxFiles[newSkybox2Index].palette.vromEnd - gSkyboxFiles[newSkybox2Index].palette.vromStart;
                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->palettes,
                                    gSkyboxFiles[newSkybox2Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                    __FILE__, __LINE__);*/
            } else
            {
                SkyboxTableEntry entryA = sSkyboxTable[newSkybox2Index];
                LoadSkyboxPalette(skyboxCtx, 1, entryA.palettes[0], 16, 8);

                /*size = gSkyboxFiles[newSkybox2Index].palette.vromEnd - gSkyboxFiles[newSkybox2Index].palette.vromStart;
                osCreateMesgQueue(&envCtx->loadQueue, &envCtx->loadMsg, 1);
                DmaMgr_SendRequest2(&envCtx->dmaRequest, (uintptr_t)skyboxCtx->palettes + size,
                                    gSkyboxFiles[newSkybox2Index].palette.vromStart, size, 0, &envCtx->loadQueue, NULL,
                                    __FILE__, __LINE__);*/
            }

            Skybox_Update(skyboxCtx);
        }

        if ((envCtx->skyboxDmaState == SKYBOX_DMA_FILE1_START) || (envCtx->skyboxDmaState == SKYBOX_DMA_FILE2_START)) {
            //if (osRecvMesg(&envCtx->loadQueue, 0, OS_MESG_NOBLOCK) == 0)
            {
                envCtx->skyboxDmaState++;
            }
        } else if (envCtx->skyboxDmaState >= SKYBOX_DMA_FILE1_DONE) {
            //if (osRecvMesg(&envCtx->loadQueue, 0, OS_MESG_NOBLOCK) == 0)
            {
                envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
            }
        }

        envCtx->skyboxBlend = skyboxBlend;
    }
}

void Environment_EnableUnderwaterLights(PlayState* play, s32 waterLightsIndex) {
    if (waterLightsIndex == 0x1F) {
        waterLightsIndex = 0;
        // "Underwater color is not set in the water poly data!"
        osSyncPrintf(VT_COL(YELLOW, BLACK) "\n水ポリゴンデータに水中カラーが設定されておりません!" VT_RST);
    }

    if (!play->envCtx.indoors) {
        D_8011FB34 = play->envCtx.unk_20;

        if (play->envCtx.unk_1F != waterLightsIndex) {
            play->envCtx.unk_1F = waterLightsIndex;
            play->envCtx.unk_20 = waterLightsIndex;
        }
    } else {
        play->envCtx.blendIndoorLights = false; // instantly switch to water lights
        play->envCtx.unk_BF = waterLightsIndex;
    }
}

void Environment_DisableUnderwaterLights(PlayState* play) {
    if (!play->envCtx.indoors) {
        play->envCtx.unk_1F = D_8011FB34;
        play->envCtx.unk_20 = D_8011FB34;
    } else {
        play->envCtx.blendIndoorLights = false; // instantly switch to previous lights
        play->envCtx.unk_BF = 0xFF;
        play->envCtx.unk_D8 = 1.0f;
    }
}

void Environment_PrintDebugInfo(PlayState* play, Gfx** gfx) {
    GfxPrint printer;
    s32 pad[2];

    GfxPrint_Init(&printer);
    GfxPrint_Open(&printer, *gfx);

    GfxPrint_SetPos(&printer, 22, 7);
    GfxPrint_SetColor(&printer, 155, 155, 255, 64);
    GfxPrint_Printf(&printer, "T%03d ", ((void)0, gSaveContext.totalDays));
    GfxPrint_Printf(&printer, "E%03d", ((void)0, gSaveContext.bgsDayCount));

    GfxPrint_SetColor(&printer, 255, 255, 55, 64);
    GfxPrint_SetPos(&printer, 22, 8);
    GfxPrint_Printf(&printer, "%s", "ZELDATIME ");

    GfxPrint_SetColor(&printer, 255, 255, 255, 64);
    GfxPrint_Printf(&printer, "%02d", (u8)(24 * 60 / (f32)0x10000 * ((void)0, gSaveContext.dayTime) / 60.0f));

    if ((gSaveContext.dayTime & 0x1F) >= 0x10 || gTimeIncrement >= 6) {
        GfxPrint_Printf(&printer, "%s", ":");
    } else {
        GfxPrint_Printf(&printer, "%s", " ");
    }

    GfxPrint_Printf(&printer, "%02d", (s16)(24 * 60 / (f32)0x10000 * ((void)0, gSaveContext.dayTime)) % 60);

    GfxPrint_SetColor(&printer, 255, 255, 55, 64);
    GfxPrint_SetPos(&printer, 22, 9);
    GfxPrint_Printf(&printer, "%s", "VRBOXTIME ");

    GfxPrint_SetColor(&printer, 255, 255, 255, 64);
    GfxPrint_Printf(&printer, "%02d", (u8)(24 * 60 / (f32)0x10000 * ((void)0, gSaveContext.skyboxTime) / 60.0f));

    if ((((void)0, gSaveContext.skyboxTime) & 0x1F) >= 0x10 || gTimeIncrement >= 6) {
        GfxPrint_Printf(&printer, "%s", ":");
    } else {
        GfxPrint_Printf(&printer, "%s", " ");
    }

    GfxPrint_Printf(&printer, "%02d", (s16)(24 * 60 / (f32)0x10000 * ((void)0, gSaveContext.skyboxTime)) % 60);

    GfxPrint_SetColor(&printer, 55, 255, 255, 64);
    GfxPrint_SetPos(&printer, 22, 6);

    if (!IS_DAY) {
        GfxPrint_Printf(&printer, "%s", "YORU"); // "night"
    } else {
        GfxPrint_Printf(&printer, "%s", "HIRU"); // "day"
    }

    *gfx = GfxPrint_Close(&printer);
    GfxPrint_Destroy(&printer);
}

#define TIME_ENTRY_1F (D_8011FB48[envCtx->unk_1F][i])
#define TIME_ENTRY_20 (D_8011FB48[envCtx->unk_20][i])

void func_80075B44(PlayState* play);
void func_800766C4(PlayState* play);

void Environment_Update(PlayState* play, EnvironmentContext* envCtx, LightContext* lightCtx,
                        PauseContext* pauseCtx, MessageContext* msgCtx, GameOverContext* gameOverCtx,
                        GraphicsContext* gfxCtx) {
    f32 sp8C;
    f32 sp88 = 0.0f;
    u16 i;
    u16 j;
    u16 time;
    EnvLightSettings* lightSettingsList = play->envCtx.lightSettingsList;
    s32 adjustment;

    if ((((void)0, gSaveContext.gameMode) != 0) && (((void)0, gSaveContext.gameMode) != 3)) {
        func_800AA16C(play);
    }

    if (pauseCtx->state == 0) {
        if ((play->pauseCtx.state == 0) && (play->pauseCtx.debugState == 0)) {
            if (play->skyboxId == SKYBOX_NORMAL_SKY) {
                play->skyboxCtx.rot.y -= 0.001f;
            } else if (play->skyboxId == SKYBOX_CUTSCENE_MAP) {
                play->skyboxCtx.rot.y -= 0.005f;
            }
        }

        func_800766C4(play); // increments or decrements unk_EE[1] depending on some condition
        func_80075B44(play); // updates bgm/sfx and other things as the day progresses

        if (((void)0, gSaveContext.nextDayTime) >= 0xFF00 && ((void)0, gSaveContext.nextDayTime) != 0xFFFF) {
            gSaveContext.nextDayTime -= 0x10;
            osSyncPrintf("\nnext_zelda_time=[%x]", ((void)0, gSaveContext.nextDayTime));

            if (((void)0, gSaveContext.nextDayTime) == 0xFF0E) {
                func_80078884(NA_SE_EV_CHICKEN_CRY_M);
                gSaveContext.nextDayTime = 0xFFFF;
            } else if (((void)0, gSaveContext.nextDayTime) == 0xFF0D) {
                func_800788CC(NA_SE_EV_DOG_CRY_EVENING);
                gSaveContext.nextDayTime = 0xFFFF;
            }
        }

        if ((pauseCtx->state == 0) && (gameOverCtx->state == GAMEOVER_INACTIVE)) {
            if (((msgCtx->msgLength == 0) && (msgCtx->msgMode == 0)) || (((void)0, gSaveContext.gameMode) == 3)) {
                if ((envCtx->unk_1A == 0) && !FrameAdvance_IsEnabled(play) &&
                    (play->transitionMode == 0 || ((void)0, gSaveContext.gameMode) != 0)) {

                    if (IS_DAY || gTimeIncrement >= 0x190) {
                        gSaveContext.dayTime += gTimeIncrement;
                    } else {
                        gSaveContext.dayTime += gTimeIncrement * 2; // time moves twice as fast at night
                    }
                }
            }
        }

        //! @bug `gTimeIncrement` is unsigned, it can't be negative
        if (((((void)0, gSaveContext.sceneSetupIndex) >= 5 || gTimeIncrement != 0) &&
             ((void)0, gSaveContext.dayTime) > gSaveContext.skyboxTime) ||
            (((void)0, gSaveContext.dayTime) < 0xAAB || gTimeIncrement < 0)) {
            gSaveContext.skyboxTime = ((void)0, gSaveContext.dayTime);
        }

        time = gSaveContext.dayTime;

        if (time > 0xC000 || time < 0x4555) {
            gSaveContext.nightFlag = 1;
        } else {
            gSaveContext.nightFlag = 0;
        }

        if (SREG(0) != 0 || CREG(2) != 0) {
            Gfx* displayList;
            Gfx* prevDisplayList;

            OPEN_DISPS(play->state.gfxCtx);
            prevDisplayList = POLY_OPA_DISP;
            displayList = Graph_GfxPlusOne(POLY_OPA_DISP);
            gSPDisplayList(OVERLAY_DISP++, displayList);
            Environment_PrintDebugInfo(play, &displayList);
            gSPEndDisplayList(displayList++);
            Graph_BranchDlist(prevDisplayList, displayList);
            POLY_OPA_DISP = displayList;
            CLOSE_DISPS(play->state.gfxCtx);
        }

        if ((envCtx->unk_BF != 0xFF) && (envCtx->unk_DC != 2) && (envCtx->unk_BD != envCtx->unk_BF) &&
            (envCtx->unk_D8 >= 1.0f) && (envCtx->unk_BF < 0x20)) {
            envCtx->unk_BE = envCtx->unk_BD;
            envCtx->unk_BD = envCtx->unk_BF;
            envCtx->unk_D8 = 0.0f;
        }

        if (envCtx->unk_BF != 0xFE) {
            if (!envCtx->indoors && (envCtx->unk_BF == 0xFF)) {
                for (i = 0; i < ARRAY_COUNT(D_8011FB48[envCtx->unk_1F]); i++) {
                    if ((gSaveContext.skyboxTime >= TIME_ENTRY_1F.startTime) &&
                        ((gSaveContext.skyboxTime < TIME_ENTRY_1F.endTime) || TIME_ENTRY_1F.endTime == 0xFFFF)) {
                        u8 blend8[2];
                        s16 blend16[2];

                        sp8C = Environment_LerpWeight(TIME_ENTRY_1F.endTime, TIME_ENTRY_1F.startTime,
                                                      ((void)0, gSaveContext.skyboxTime));

                        D_8011FDCC = TIME_ENTRY_1F.unk_04 & 3;
                        D_8011FDD0 = TIME_ENTRY_1F.unk_05 & 3;
                        D_8011FDD4 = sp8C;

                        if (envCtx->unk_21) {
                            sp88 = ((f32)envCtx->unk_24 - envCtx->unk_22) / envCtx->unk_24;
                            envCtx->unk_22--;

                            if (envCtx->unk_22 <= 0) {
                                envCtx->unk_21 = 0;
                                envCtx->unk_1F = envCtx->unk_20;
                            }
                        }

                        for (j = 0; j < 3; j++) {
                            // blend ambient color
                            blend8[0] = LERP(lightSettingsList[TIME_ENTRY_1F.unk_04].ambientColor[j],
                                             lightSettingsList[TIME_ENTRY_1F.unk_05].ambientColor[j], sp8C);
                            blend8[1] = LERP(lightSettingsList[TIME_ENTRY_20.unk_04].ambientColor[j],
                                             lightSettingsList[TIME_ENTRY_20.unk_05].ambientColor[j], sp8C);
                            *(envCtx->lightSettings.ambientColor + j) = LERP(blend8[0], blend8[1], sp88);
                        }

                        // set light1 direction for the sun
                        envCtx->lightSettings.light1Dir[0] =
                            -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f);
                        envCtx->lightSettings.light1Dir[1] =
                            Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f;
                        envCtx->lightSettings.light1Dir[2] =
                            (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f);

                        // set light2 direction for the moon
                        envCtx->lightSettings.light2Dir[0] = -envCtx->lightSettings.light1Dir[0];
                        envCtx->lightSettings.light2Dir[1] = -envCtx->lightSettings.light1Dir[1];
                        envCtx->lightSettings.light2Dir[2] = -envCtx->lightSettings.light1Dir[2];

                        for (j = 0; j < 3; j++) {
                            // blend light1Color
                            blend8[0] = LERP(lightSettingsList[TIME_ENTRY_1F.unk_04].light1Color[j],
                                             lightSettingsList[TIME_ENTRY_1F.unk_05].light1Color[j], sp8C);
                            blend8[1] = LERP(lightSettingsList[TIME_ENTRY_20.unk_04].light1Color[j],
                                             lightSettingsList[TIME_ENTRY_20.unk_05].light1Color[j], sp8C);
                            *(envCtx->lightSettings.light1Color + j) = LERP(blend8[0], blend8[1], sp88);

                            // blend light2Color
                            blend8[0] = LERP(lightSettingsList[TIME_ENTRY_1F.unk_04].light2Color[j],
                                             lightSettingsList[TIME_ENTRY_1F.unk_05].light2Color[j], sp8C);
                            blend8[1] = LERP(lightSettingsList[TIME_ENTRY_20.unk_04].light2Color[j],
                                             lightSettingsList[TIME_ENTRY_20.unk_05].light2Color[j], sp8C);
                            *(envCtx->lightSettings.light2Color + j) = LERP(blend8[0], blend8[1], sp88);
                        }

                        // blend fogColor
                        for (j = 0; j < 3; j++) {
                            blend8[0] = LERP(lightSettingsList[TIME_ENTRY_1F.unk_04].fogColor[j],
                                             lightSettingsList[TIME_ENTRY_1F.unk_05].fogColor[j], sp8C);
                            blend8[1] = LERP(lightSettingsList[TIME_ENTRY_20.unk_04].fogColor[j],
                                             lightSettingsList[TIME_ENTRY_20.unk_05].fogColor[j], sp8C);
                            *(envCtx->lightSettings.fogColor + j) = LERP(blend8[0], blend8[1], sp88);
                        }

                        blend16[0] = LERP16((lightSettingsList[TIME_ENTRY_1F.unk_04].fogNear & 0x3FF),
                                            (lightSettingsList[TIME_ENTRY_1F.unk_05].fogNear & 0x3FF), sp8C);
                        blend16[1] = LERP16(lightSettingsList[TIME_ENTRY_20.unk_04].fogNear & 0x3FF,
                                            lightSettingsList[TIME_ENTRY_20.unk_05].fogNear & 0x3FF, sp8C);

                        envCtx->lightSettings.fogNear = LERP16(blend16[0], blend16[1], sp88);

                        blend16[0] = LERP16(lightSettingsList[TIME_ENTRY_1F.unk_04].fogFar,
                                            lightSettingsList[TIME_ENTRY_1F.unk_05].fogFar, sp8C);
                        blend16[1] = LERP16(lightSettingsList[TIME_ENTRY_20.unk_04].fogFar,
                                            lightSettingsList[TIME_ENTRY_20.unk_05].fogFar, sp8C);

                        envCtx->lightSettings.fogFar = LERP16(blend16[0], blend16[1], sp88);

                        if (TIME_ENTRY_20.unk_05 >= envCtx->numLightSettings) {
                            // "The color palette setting seems to be wrong!"
                            osSyncPrintf(VT_COL(RED, WHITE) "\nカラーパレットの設定がおかしいようです！" VT_RST);

                            // "Palette setting = [] Last palette number = []"
                            osSyncPrintf(VT_COL(RED, WHITE) "\n設定パレット＝[%d] 最後パレット番号＝[%d]\n" VT_RST,
                                         TIME_ENTRY_20.unk_05, envCtx->numLightSettings - 1);
                        }
                        break;
                    }
                }
            } else {
                if (!envCtx->blendIndoorLights) {
                    for (i = 0; i < 3; i++) {
                        envCtx->lightSettings.ambientColor[i] = lightSettingsList[envCtx->unk_BD].ambientColor[i];
                        envCtx->lightSettings.light1Dir[i] = lightSettingsList[envCtx->unk_BD].light1Dir[i];
                        envCtx->lightSettings.light1Color[i] = lightSettingsList[envCtx->unk_BD].light1Color[i];
                        envCtx->lightSettings.light2Dir[i] = lightSettingsList[envCtx->unk_BD].light2Dir[i];
                        envCtx->lightSettings.light2Color[i] = lightSettingsList[envCtx->unk_BD].light2Color[i];
                        envCtx->lightSettings.fogColor[i] = lightSettingsList[envCtx->unk_BD].fogColor[i];
                    }

                    envCtx->lightSettings.fogNear = lightSettingsList[envCtx->unk_BD].fogNear & 0x3FF;
                    envCtx->lightSettings.fogFar = lightSettingsList[envCtx->unk_BD].fogFar;
                    envCtx->unk_D8 = 1.0f;
                } else {
                    u8 blendRate = (lightSettingsList[envCtx->unk_BD].fogNear >> 0xA) * 4;

                    if (blendRate == 0) {
                        blendRate++;
                    }

                    if (envCtx->unk_D6 != 0xFFFF) {
                        blendRate = envCtx->unk_D6;
                    }

                    if (envCtx->unk_DC == 0) {
                        envCtx->unk_D8 += blendRate / 255.0f;
                    }

                    if (envCtx->unk_D8 > 1.0f) {
                        envCtx->unk_D8 = 1.0f;
                    }

                    for (i = 0; i < 3; i++) {
                        envCtx->lightSettings.ambientColor[i] =
                            LERP(lightSettingsList[envCtx->unk_BE].ambientColor[i],
                                 lightSettingsList[envCtx->unk_BD].ambientColor[i], envCtx->unk_D8);
                        envCtx->lightSettings.light1Dir[i] =
                            LERP16(lightSettingsList[envCtx->unk_BE].light1Dir[i],
                                   lightSettingsList[envCtx->unk_BD].light1Dir[i], envCtx->unk_D8);
                        envCtx->lightSettings.light1Color[i] =
                            LERP(lightSettingsList[envCtx->unk_BE].light1Color[i],
                                 lightSettingsList[envCtx->unk_BD].light1Color[i], envCtx->unk_D8);
                        envCtx->lightSettings.light2Dir[i] =
                            LERP16(lightSettingsList[envCtx->unk_BE].light2Dir[i],
                                   lightSettingsList[envCtx->unk_BD].light2Dir[i], envCtx->unk_D8);
                        envCtx->lightSettings.light2Color[i] =
                            LERP(lightSettingsList[envCtx->unk_BE].light2Color[i],
                                 lightSettingsList[envCtx->unk_BD].light2Color[i], envCtx->unk_D8);
                        envCtx->lightSettings.fogColor[i] =
                            LERP(lightSettingsList[envCtx->unk_BE].fogColor[i],
                                 lightSettingsList[envCtx->unk_BD].fogColor[i], envCtx->unk_D8);
                    }
                    envCtx->lightSettings.fogNear =
                        LERP16(lightSettingsList[envCtx->unk_BE].fogNear & 0x3FF,
                               lightSettingsList[envCtx->unk_BD].fogNear & 0x3FF, envCtx->unk_D8);
                    envCtx->lightSettings.fogFar = LERP16(lightSettingsList[envCtx->unk_BE].fogFar,
                                                          lightSettingsList[envCtx->unk_BD].fogFar, envCtx->unk_D8);
                }

                if (envCtx->unk_BD >= envCtx->numLightSettings) {
                    // "The color palette seems to be wrong!"
                    osSyncPrintf("\n" VT_FGCOL(RED) "カラーパレットがおかしいようです！");

                    // "Palette setting = [] Last palette number = []"
                    osSyncPrintf("\n" VT_FGCOL(YELLOW) "設定パレット＝[%d] パレット数＝[%d]\n" VT_RST, envCtx->unk_BD,
                                 envCtx->numLightSettings);
                }
            }
        }

        envCtx->blendIndoorLights = true;

        // Apply lighting adjustments
        for (i = 0; i < 3; i++) {
            if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]) > 255) {
                lightCtx->ambientColor[i] = 255;
            } else if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]) < 0) {
                lightCtx->ambientColor[i] = 0;
            } else {
                lightCtx->ambientColor[i] = (s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjAmbientColor[i]);
            }

            if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]) > 255) {
                envCtx->dirLight1.params.dir.color[i] = 255;
            } else if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]) < 0) {
                envCtx->dirLight1.params.dir.color[i] = 0;
            } else {
                envCtx->dirLight1.params.dir.color[i] =
                    (s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLight1Color[i]);
            }

            if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]) > 255) {
                envCtx->dirLight2.params.dir.color[i] = 255;
            } else if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]) < 0) {
                envCtx->dirLight2.params.dir.color[i] = 0;
            } else {
                envCtx->dirLight2.params.dir.color[i] =
                    (s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLight1Color[i]);
            }

            if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]) > 255) {
                lightCtx->fogColor[i] = 255;
            } else if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]) < 0) {
                lightCtx->fogColor[i] = 0;
            } else {
                lightCtx->fogColor[i] = (s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjFogColor[i]);
            }
        }

        // Set both directional light directions
        envCtx->dirLight1.params.dir.x = envCtx->lightSettings.light1Dir[0];
        envCtx->dirLight1.params.dir.y = envCtx->lightSettings.light1Dir[1];
        envCtx->dirLight1.params.dir.z = envCtx->lightSettings.light1Dir[2];

        envCtx->dirLight2.params.dir.x = envCtx->lightSettings.light2Dir[0];
        envCtx->dirLight2.params.dir.y = envCtx->lightSettings.light2Dir[1];
        envCtx->dirLight2.params.dir.z = envCtx->lightSettings.light2Dir[2];

        // Adjust fog near and far if necessary
        adjustment = envCtx->lightSettings.fogNear + envCtx->adjFogNear;

        if (adjustment <= 996) {
            lightCtx->fogNear = adjustment;
        } else {
            lightCtx->fogNear = 996;
        }

        adjustment = envCtx->lightSettings.fogFar + envCtx->adjFogFar;

        if (adjustment <= 12800) {
            lightCtx->fogFar = adjustment;
        } else {
            lightCtx->fogFar = 12800;
        }

        // When environment debug is enabled, various environment related variables can be configured via the reg editor
        if (R_ENV_DISABLE_DBG) {
            R_ENV_AMBIENT_COLOR(0) = lightCtx->ambientColor[0];
            R_ENV_AMBIENT_COLOR(1) = lightCtx->ambientColor[1];
            R_ENV_AMBIENT_COLOR(2) = lightCtx->ambientColor[2];

            R_ENV_LIGHT1_COLOR(0) = envCtx->dirLight1.params.dir.color[0];
            R_ENV_LIGHT1_COLOR(1) = envCtx->dirLight1.params.dir.color[1];
            R_ENV_LIGHT1_COLOR(2) = envCtx->dirLight1.params.dir.color[2];

            R_ENV_LIGHT2_COLOR(0) = envCtx->dirLight2.params.dir.color[0];
            R_ENV_LIGHT2_COLOR(1) = envCtx->dirLight2.params.dir.color[1];
            R_ENV_LIGHT2_COLOR(2) = envCtx->dirLight2.params.dir.color[2];

            R_ENV_FOG_COLOR(0) = lightCtx->fogColor[0];
            R_ENV_FOG_COLOR(1) = lightCtx->fogColor[1];
            R_ENV_FOG_COLOR(2) = lightCtx->fogColor[2];

            R_ENV_FOG_FAR = lightCtx->fogFar;
            R_ENV_FOG_NEAR = lightCtx->fogNear;

            R_ENV_LIGHT1_DIR(0) = envCtx->dirLight1.params.dir.x;
            R_ENV_LIGHT1_DIR(1) = envCtx->dirLight1.params.dir.y;
            R_ENV_LIGHT1_DIR(2) = envCtx->dirLight1.params.dir.z;

            R_ENV_LIGHT2_DIR(0) = envCtx->dirLight2.params.dir.x;
            R_ENV_LIGHT2_DIR(1) = envCtx->dirLight2.params.dir.y;
            R_ENV_LIGHT2_DIR(2) = envCtx->dirLight2.params.dir.z;

            R_ENV_WIND_DIR(0) = envCtx->windDirection.x;
            R_ENV_WIND_DIR(1) = envCtx->windDirection.y;
            R_ENV_WIND_DIR(2) = envCtx->windDirection.z;
            R_ENV_WIND_SPEED = envCtx->windSpeed;
        } else {
            lightCtx->ambientColor[0] = R_ENV_AMBIENT_COLOR(0);
            lightCtx->ambientColor[1] = R_ENV_AMBIENT_COLOR(1);
            lightCtx->ambientColor[2] = R_ENV_AMBIENT_COLOR(2);

            envCtx->dirLight1.params.dir.color[0] = R_ENV_LIGHT1_COLOR(0);
            envCtx->dirLight1.params.dir.color[1] = R_ENV_LIGHT1_COLOR(1);
            envCtx->dirLight1.params.dir.color[2] = R_ENV_LIGHT1_COLOR(2);

            envCtx->dirLight2.params.dir.color[0] = R_ENV_LIGHT2_COLOR(0);
            envCtx->dirLight2.params.dir.color[1] = R_ENV_LIGHT2_COLOR(1);
            envCtx->dirLight2.params.dir.color[2] = R_ENV_LIGHT2_COLOR(2);
            lightCtx->fogColor[0] = R_ENV_FOG_COLOR(0);
            lightCtx->fogColor[1] = R_ENV_FOG_COLOR(1);
            lightCtx->fogColor[2] = R_ENV_FOG_COLOR(2);
            lightCtx->fogNear = R_ENV_FOG_NEAR;
            lightCtx->fogFar = R_ENV_FOG_FAR;

            if (cREG(14)) {
                R_ENV_LIGHT1_DIR(0) = Math_CosS(cREG(10)) * Math_CosS(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.x = R_ENV_LIGHT1_DIR(0);
                R_ENV_LIGHT1_DIR(1) = Math_SinS(cREG(10)) * Math_CosS(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.y = R_ENV_LIGHT1_DIR(1);
                R_ENV_LIGHT1_DIR(2) = Math_SinS(cREG(11)) * 120.0f;
                envCtx->dirLight1.params.dir.z = R_ENV_LIGHT1_DIR(2);

                R_ENV_LIGHT2_DIR(0) = Math_CosS(cREG(12)) * Math_CosS(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.x = R_ENV_LIGHT2_DIR(0);
                R_ENV_LIGHT2_DIR(1) = Math_SinS(cREG(12)) * Math_CosS(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.y = R_ENV_LIGHT2_DIR(1);
                R_ENV_LIGHT2_DIR(2) = Math_SinS(cREG(13)) * 120.0f;
                envCtx->dirLight2.params.dir.z = R_ENV_LIGHT2_DIR(2);
            } else {
                envCtx->dirLight1.params.dir.x = R_ENV_LIGHT1_DIR(0);
                envCtx->dirLight1.params.dir.y = R_ENV_LIGHT1_DIR(1);
                envCtx->dirLight1.params.dir.z = R_ENV_LIGHT1_DIR(2);

                envCtx->dirLight2.params.dir.x = R_ENV_LIGHT2_DIR(0);
                envCtx->dirLight2.params.dir.y = R_ENV_LIGHT2_DIR(1);
                envCtx->dirLight2.params.dir.z = R_ENV_LIGHT2_DIR(2);
            }

            envCtx->windDirection.x = R_ENV_WIND_DIR(0);
            envCtx->windDirection.y = R_ENV_WIND_DIR(1);
            envCtx->windDirection.z = R_ENV_WIND_DIR(2);
            envCtx->windSpeed = R_ENV_WIND_SPEED;
        }

        if ((envCtx->dirLight1.params.dir.x == 0) && (envCtx->dirLight1.params.dir.y == 0) &&
            (envCtx->dirLight1.params.dir.z == 0)) {
            envCtx->dirLight1.params.dir.x = 1;
        }

        if ((envCtx->dirLight2.params.dir.x == 0) && (envCtx->dirLight2.params.dir.y == 0) &&
            (envCtx->dirLight2.params.dir.z == 0)) {
            envCtx->dirLight2.params.dir.x = 1;
        }
    }
}

void Environment_DrawSunAndMoon(PlayState* play) {
    f32 alpha;
    f32 color;
    f32 y;
    f32 scale;
    f32 temp;

    OPEN_DISPS(play->state.gfxCtx);

    if (play->csCtx.state != 0) {
        Math_SmoothStepToF(&play->envCtx.sunPos.x,
                           -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f, 1.0f, 0.8f, 0.8f);
        Math_SmoothStepToF(&play->envCtx.sunPos.y,
                           (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f, 1.0f, 0.8f, 0.8f);
        //! @bug This should be z.
        Math_SmoothStepToF(&play->envCtx.sunPos.y,
                           (Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f) * 25.0f, 1.0f, 0.8f, 0.8f);
    } else {
        play->envCtx.sunPos.x = -(Math_SinS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
        play->envCtx.sunPos.y = +(Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f) * 25.0f;
        play->envCtx.sunPos.z = +(Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 20.0f) * 25.0f;
    }

    if (gSaveContext.entranceIndex != 0xCD || ((void)0, gSaveContext.sceneSetupIndex) != 5) {
        Matrix_Translate(play->view.eye.x + play->envCtx.sunPos.x,
                         play->view.eye.y + play->envCtx.sunPos.y,
                         play->view.eye.z + play->envCtx.sunPos.z, MTXMODE_NEW);

        y = play->envCtx.sunPos.y / 25.0f;
        temp = y / 80.0f;

        alpha = temp * 255.0f;
        if (alpha < 0.0f) {
            alpha = 0.0f;
        }
        if (alpha > 255.0f) {
            alpha = 255.0f;
        }

        alpha = 255.0f - alpha;

        color = temp;
        if (color < 0.0f) {
            color = 0.0f;
        }

        if (color > 1.0f) {
            color = 1.0f;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, (u8)(color * 75.0f) + 180, (u8)(color * 155.0f) + 100, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, (u8)(color * 255.0f), (u8)(color * 255.0f), alpha);

        scale = (color * 2.0f) + 10.0f;
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_LOAD);
        Gfx_SetupDL_54Opa(play->state.gfxCtx);

        static Vtx vertices[] = {
            VTX(-31, -31, 0, 0, 0, 255, 255, 255, 255),
	        VTX(32, -31, 0, 2016, 0, 255, 255, 255, 255),
	        VTX(-31, 32, 0, 0, 2016, 255, 255, 255, 255),
	        VTX(32, 32, 0, 2016, 2016, 255, 255, 255, 255),
        };

        // Replacement for gSunDL
        gSPMatrix(POLY_OPA_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gSun1Tex, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                               G_TX_NOMIRROR | G_TX_CLAMP,
                               6, 6, G_TX_NOLOD, G_TX_NOLOD);
        gDPLoadMultiBlock_4b(POLY_OPA_DISP++, gSunEvening1Tex, 0x0100, 1, G_IM_FMT_I, 64, 64, 0,
                             G_TX_NOMIRROR | G_TX_CLAMP,
                             G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
        gSPVertex(POLY_OPA_DISP++, vertices, 4, 0);
        gSP2Triangles(POLY_OPA_DISP++, 0, 1, 2, 0, 2, 1, 3, 0);

        Matrix_Translate(play->view.eye.x - play->envCtx.sunPos.x,
                         play->view.eye.y - play->envCtx.sunPos.y,
                         play->view.eye.z - play->envCtx.sunPos.z, MTXMODE_NEW);

        color = -y / 120.0f;
        color = CLAMP_MIN(color, 0.0f);

        scale = -15.0f * color + 25.0f;
        scale *= CVarGetFloat("gCosmetics.Moon_Size", 1.0f);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

        temp = -y / 80.0f;
        temp = CLAMP_MAX(temp, 1.0f);

        alpha = temp * 255.0f;

        if (alpha > 0.0f) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_LOAD);
            Gfx_SetupDL_51Opa(play->state.gfxCtx);
            gDPPipeSync(POLY_OPA_DISP++);
            if (CVarGetInteger("gCosmetics.World_Moon.Changed", 0)) {
                Color_RGB8 moonColor = CVarGetColor24("gCosmetics.World_Moon.Value", (Color_RGB8){ 0, 0, 240 });
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, moonColor.r, moonColor.g, moonColor.b, alpha);
                gDPSetEnvColor(POLY_OPA_DISP++, moonColor.r / 2, moonColor.g / 2, moonColor.b / 2, alpha);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 240, 255, 180, alpha);
                gDPSetEnvColor(POLY_OPA_DISP++, 80, 70, 20, alpha);
            }
            gSPDisplayList(POLY_OPA_DISP++, gMoonDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Environment_DrawSunLensFlare(PlayState* play, EnvironmentContext* envCtx, View* view,
                                  GraphicsContext* gfxCtx, Vec3f pos, s32 unused) {
    if ((play->envCtx.unk_EE[1] == 0) && (play->envCtx.unk_17 == 0)) {
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, pos, 2000,
                                  370, Math_CosS(((void)0, gSaveContext.dayTime) - 0x8000) * 120.0f, 400, 1);
    }
}

f32 sLensFlareScales[] = { 23.0f, 12.0f, 7.0f, 5.0f, 3.0f, 10.0f, 6.0f, 2.0f, 3.0f, 1.0f };

void Environment_DrawLensFlare(PlayState* play, EnvironmentContext* envCtx, View* view,
                               GraphicsContext* gfxCtx, Vec3f pos, s32 unused, s16 scale, f32 colorIntensity,
                               s16 screenFillAlpha, u8 arg9) {
    s16 i;
    f32 tempX;
    f32 tempY;
    f32 tempZ;
    f32 lookDirX;
    f32 lookDirY;
    f32 lookDirZ;
    f32 tempX2;
    f32 tempY2;
    f32 tempZ2;
    f32 posDirX;
    f32 posDirY;
    f32 posDirZ;
    f32 length;
    f32 dist;
    f32 halfPosX;
    f32 halfPosY;
    f32 halfPosZ;
    f32 cosAngle;
    f32 pad160;
    f32 unk88Target;
    u32 isOffScreen = false;
    f32 alpha;
    f32 adjScale;
    Vec3f screenPos;
    f32 fogInfluence;
    f32 temp;
    f32 alphaScale;
    Color_RGB8 lensFlareColors[] = {
        { 155, 205, 255 }, // blue
        { 255, 255, 205 }, // yellow
        { 255, 255, 205 }, // yellow
        { 255, 255, 205 }, // yellow
        { 155, 255, 205 }, // green
        { 205, 255, 255 }, // light blue
        { 155, 155, 255 }, // dark blue
        { 205, 175, 255 }, // purple
        { 175, 255, 205 }, // light green
        { 255, 155, 235 }, // pink
    };
    u32 lensFlareAlphas[] = {
        50, 10, 25, 40, 70, 30, 50, 70, 50, 40,
    };
    u32 lensFlareTypes[] = {
        LENS_FLARE_RING,    LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
        LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
    };

    OPEN_DISPS(gfxCtx);

    dist = Math3D_Vec3f_DistXYZ(&pos, &view->eye) / 12.0f;

    // compute a unit vector in the look direction
    tempX = view->lookAt.x - view->eye.x;
    tempY = view->lookAt.y - view->eye.y;
    tempZ = view->lookAt.z - view->eye.z;

    length = sqrtf(SQ(tempX) + SQ(tempY) + SQ(tempZ));

    lookDirX = tempX / length;
    lookDirY = tempY / length;
    lookDirZ = tempZ / length;

    // compute a position along the look vector half as far as pos
    halfPosX = view->eye.x + lookDirX * (dist * 6.0f);
    halfPosY = view->eye.y + lookDirY * (dist * 6.0f);
    halfPosZ = view->eye.z + lookDirZ * (dist * 6.0f);

    // compute a unit vector in the direction from halfPos to pos
    tempX2 = pos.x - halfPosX;
    tempY2 = pos.y - halfPosY;
    tempZ2 = pos.z - halfPosZ;

    length = sqrtf(SQ(tempX2) + SQ(tempY2) + SQ(tempZ2));

    posDirX = tempX2 / length;
    posDirY = tempY2 / length;
    posDirZ = tempZ2 / length;

    // compute the cosine of the angle between lookDir and posDir
    cosAngle = (lookDirX * posDirX + lookDirY * posDirY + lookDirZ * posDirZ) /
               sqrtf((SQ(lookDirX) + SQ(lookDirY) + SQ(lookDirZ)) * (SQ(posDirX) + SQ(posDirY) + SQ(posDirZ)));

    unk88Target = cosAngle * 3.5f;
    unk88Target = CLAMP_MAX(unk88Target, 1.0f);

    if (arg9 == 0) {
        unk88Target = cosAngle;
    }

    if (!(cosAngle < 0.0f)) {
        if (arg9) {
            u32 shrink = ShrinkWindow_GetCurrentVal();
            func_800C016C(play, &pos, &screenPos);
            D_8015FD7E = (s16)screenPos.x;
            D_8015FD80 = (s16)screenPos.y - 5.0f;
            if (D_8011FB44 != 0xFFFC || screenPos.x < 0.0f || screenPos.y < shrink || screenPos.x > SCREEN_WIDTH ||
                screenPos.y > (SCREEN_HEIGHT - shrink)) {
                isOffScreen = true;
            }
        }

        for (i = 0; i < ARRAY_COUNT(lensFlareTypes); i++) {
            FrameInterpolation_RecordOpenChild("Lens Flare", i);

            Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);

            if (arg9) {
                temp = Environment_LerpWeight(60, 15, play->view.fovy);
            }

            Matrix_Translate(-posDirX * i * dist, -posDirY * i * dist, -posDirZ * i * dist, MTXMODE_APPLY);

            adjScale = sLensFlareScales[i] * cosAngle;

            if (arg9) {
                adjScale *= 0.001 * (scale + 630.0f * temp);
            } else {
                adjScale *= 0.0001f * scale * (2.0f * dist);
            }

            Matrix_Scale(adjScale, adjScale, adjScale, MTXMODE_APPLY);

            alpha = colorIntensity / 10.0f;
            alpha = CLAMP_MAX(alpha, 1.0f);
            alpha = alpha * lensFlareAlphas[i];
            alpha = CLAMP_MIN(alpha, 0.0f);

            fogInfluence = (996 - play->lightCtx.fogNear) / 50.0f;

            fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

            alpha *= 1.0f - fogInfluence;


            if (!(isOffScreen ^ 0)) {
                Math_SmoothStepToF(&envCtx->unk_88, unk88Target, 0.5f, 0.05f, 0.001f);
            } else {
                Math_SmoothStepToF(&envCtx->unk_88, 0.0f, 0.5f, 0.05f, 0.001f);
            }

            POLY_XLU_DISP = func_800947AC(POLY_XLU_DISP++);
             gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, lensFlareColors[i].r, lensFlareColors[i].g, lensFlareColors[i].b,
                             alpha * envCtx->unk_88);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0,
                              0, PRIMITIVE, 0);
            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);

            switch (lensFlareTypes[i]) {
                case LENS_FLARE_CIRCLE0:
                case LENS_FLARE_CIRCLE1:
                    gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);
                    break;
                case LENS_FLARE_RING:
                    gSPDisplayList(POLY_XLU_DISP++, gLensFlareRingDL);
                    break;
            }

            FrameInterpolation_RecordCloseChild();
        }

        alphaScale = cosAngle - (1.5f - cosAngle);

        if (screenFillAlpha != 0) {
            if (alphaScale > 0.0f) {
                POLY_XLU_DISP = Gfx_SetupDL_57(POLY_XLU_DISP);

                alpha = colorIntensity / 10.0f;
                alpha = CLAMP_MAX(alpha, 1.0f);
                alpha = alpha * screenFillAlpha;
                alpha = CLAMP_MIN(alpha, 0.0f);

                fogInfluence = (996 - play->lightCtx.fogNear) / 50.0f;

                fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

                alpha *= 1.0f - fogInfluence;

                gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
                gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

                if (!(isOffScreen ^ 0)) {
                    Math_SmoothStepToF(&envCtx->unk_84, alpha * alphaScale, 0.5f, 50.0f, 0.1f);
                } else {
                    Math_SmoothStepToF(&envCtx->unk_84, 0.0f, 0.5f, 50.0f, 0.1f);
                }

                temp = colorIntensity / 120.0f;
                temp = CLAMP_MIN(temp, 0.0f);

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, (u8)(temp * 75.0f) + 180, (u8)(temp * 155.0f) + 100,
                                (u8)envCtx->unk_84);
                gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
            } else {
                envCtx->unk_84 = 0.0f;
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

f32 func_800746DC(void) {
    return Rand_ZeroOne() - 0.5f;
}

void Environment_DrawRain(PlayState* play, View* view, GraphicsContext* gfxCtx) {
    s16 i;
    s32 pad;
    Vec3f vec;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 length;
    f32 rotX;
    f32 rotY;
    f32 x50;
    f32 y50;
    f32 z50;
    f32 x280;
    f32 z280;
    Vec3f unused = { 0.0f, 0.0f, 0.0f };
    Vec3f windDirection = { 0.0f, 0.0f, 0.0f };
    Player* player = GET_PLAYER(play);

    if (!(play->cameraPtrs[0]->unk_14C & 0x100) && (play->envCtx.unk_EE[2] == 0)) {
        OPEN_DISPS(gfxCtx);

        vec.x = view->lookAt.x - view->eye.x;
        vec.y = view->lookAt.y - view->eye.y;
        vec.z = view->lookAt.z - view->eye.z;

        length = sqrtf(SQXYZ(vec));

        temp1 = vec.x / length;
        temp2 = vec.y / length;
        temp3 = vec.z / length;

        x50 = view->eye.x + temp1 * 50.0f;
        y50 = view->eye.y + temp2 * 50.0f;
        z50 = view->eye.z + temp3 * 50.0f;

        x280 = view->eye.x + temp1 * 280.0f;
        z280 = view->eye.z + temp3 * 280.0f;

        if (play->envCtx.unk_EE[1]) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 30);
            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 20);
        }

        // draw rain drops
        for (i = 0; i < play->envCtx.unk_EE[1]; i++) {
            FrameInterpolation_RecordOpenChild("Rain Drop", i);

            temp2 = Rand_ZeroOne();
            temp1 = Rand_ZeroOne();
            temp3 = Rand_ZeroOne();

            Matrix_Translate((temp2 - 0.7f) * 100.0f + x50, (temp1 - 0.7f) * 100.0f + y50,
                             (temp3 - 0.7f) * 100.0f + z50, MTXMODE_NEW);

            windDirection.x = play->envCtx.windDirection.x;
            windDirection.y = play->envCtx.windDirection.y;
            windDirection.z = play->envCtx.windDirection.z;

            vec.x = windDirection.x;
            vec.y = windDirection.y + 500.0f + Rand_ZeroOne() * 200.0f;
            vec.z = windDirection.z;
            length = sqrtf(SQXZ(vec));

            gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            rotX = Math_Atan2F(length, -vec.y);
            rotY = Math_Atan2F(vec.z, vec.x);
            Matrix_RotateY(-rotY, MTXMODE_APPLY);
            Matrix_RotateX(M_PI / 2 - rotX, MTXMODE_APPLY);
            Matrix_Scale(0.4f, 1.2f, 0.4f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gRaindropDL);

            FrameInterpolation_RecordCloseChild();
        }

        // draw droplet rings on the ground
        if (player->actor.world.pos.y < view->eye.y) {
            u8 firstDone = false;

            for (i = 0; i < play->envCtx.unk_EE[1]; i++) {
                FrameInterpolation_RecordOpenChild("Droplet Ring", i);
                
                if (!firstDone) {
                    Gfx_SetupDL_25Xlu(gfxCtx);
                    gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 120);
                    firstDone++;
                }

                Matrix_Translate(func_800746DC() * 280.0f + x280, player->actor.world.pos.y + 2.0f,
                                 func_800746DC() * 280.0f + z280, MTXMODE_NEW);

                if ((LINK_IS_ADULT && ((player->actor.world.pos.y + 2.0f - view->eye.y) > -48.0f)) ||
                    (!LINK_IS_ADULT && ((player->actor.world.pos.y + 2.0f - view->eye.y) > -30.0f))) {
                    Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);
                } else {
                    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
                }

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gEffShockwaveDL);

                FrameInterpolation_RecordCloseChild();
            }
        }

        CLOSE_DISPS(gfxCtx);
    }
}

void func_80074CE8(PlayState* play, u32 arg1) {
    if ((play->envCtx.unk_BD != arg1) && (play->envCtx.unk_D8 >= 1.0f) &&
        (play->envCtx.unk_BF == 0xFF)) {
        if (arg1 > 30) {
            arg1 = 0;
        }

        play->envCtx.unk_D8 = 0.0f;
        play->envCtx.unk_BE = play->envCtx.unk_BD;
        play->envCtx.unk_BD = arg1;
    }
}

/**
 * Draw color filters over the skybox. There are two filters.
 * The first uses the global fog color, and an alpha calculated with `fogNear`.
 * This filter draws unconditionally for skybox 29 at full alpha.
 * (note: skybox 29 is unused in the original game)
 * For the rest of the skyboxes it will draw if fogNear is less than 980.
 *
 * The second filter uses a custom color specified in `skyboxFilterColor`
 * and can be enabled with `customSkyboxFilter`.
 *
 * An example usage of a filter is to dim the skybox in cloudy conditions.
 */
void Environment_DrawSkyboxFilters(PlayState* play) {
    if (((play->skyboxId != SKYBOX_NONE) && (play->lightCtx.fogNear < 980)) ||
        (play->skyboxId == SKYBOX_UNSET_1D)) {
        f32 alpha;

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_57Opa(play->state.gfxCtx);

        alpha = (1000 - play->lightCtx.fogNear) * 0.02f;

        if (play->skyboxId == SKYBOX_UNSET_1D) {
            alpha = 1.0f;
        }

        if (alpha > 1.0f) {
            alpha = 1.0f;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->lightCtx.fogColor[0], play->lightCtx.fogColor[1],
                        play->lightCtx.fogColor[2], 255.0f * alpha);
        gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (play->envCtx.customSkyboxFilter) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_57Opa(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->envCtx.skyboxFilterColor[0],
                        play->envCtx.skyboxFilterColor[1], play->envCtx.skyboxFilterColor[2],
                        play->envCtx.skyboxFilterColor[3]);
        gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Environment_DrawLightningFlash(PlayState* play, u8 red, u8 green, u8 blue, u8 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_57Opa(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
    gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Environment_UpdateLightningStrike(PlayState* play) {
    if (play->envCtx.lightningMode != LIGHTNING_MODE_OFF) {
        switch (gLightningStrike.state) {
            case LIGHTNING_STRIKE_WAIT:
                // every frame theres a 10% chance of the timer advancing 50 units
                if (Rand_ZeroOne() < 0.1f) {
                    gLightningStrike.delayTimer += 50.0f;
                }

                gLightningStrike.delayTimer += Rand_ZeroOne();

                if (gLightningStrike.delayTimer > 500.0f) {
                    gLightningStrike.flashRed = 200;
                    gLightningStrike.flashGreen = 200;
                    gLightningStrike.flashBlue = 255;
                    gLightningStrike.flashAlphaTarget = 200;

                    gLightningStrike.delayTimer = 0.0f;
                    Environment_AddLightningBolts(play,
                                                  (u8)(Rand_ZeroOne() * (ARRAY_COUNT(sLightningBolts) - 0.1f)) + 1);
                    sLightningFlashAlpha = 0;
                    gLightningStrike.state++;
                }
                break;
            case LIGHTNING_STRIKE_START:
                gLightningStrike.flashRed = 200;
                gLightningStrike.flashGreen = 200;
                gLightningStrike.flashBlue = 255;

                play->envCtx.adjAmbientColor[0] += 80;
                play->envCtx.adjAmbientColor[1] += 80;
                play->envCtx.adjAmbientColor[2] += 100;

                sLightningFlashAlpha += 100;

                if (sLightningFlashAlpha >= gLightningStrike.flashAlphaTarget) {
                    Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_0, 0);
                    gLightningStrike.state++;
                    gLightningStrike.flashAlphaTarget = 0;
                }
                break;
            case LIGHTNING_STRIKE_END:
                if (play->envCtx.adjAmbientColor[0] > 0) {
                    play->envCtx.adjAmbientColor[0] -= 10;
                    play->envCtx.adjAmbientColor[1] -= 10;
                }

                if (play->envCtx.adjAmbientColor[2] > 0) {
                    play->envCtx.adjAmbientColor[2] -= 10;
                }

                sLightningFlashAlpha -= 10;

                if (sLightningFlashAlpha <= gLightningStrike.flashAlphaTarget) {
                    play->envCtx.adjAmbientColor[0] = 0;
                    play->envCtx.adjAmbientColor[1] = 0;
                    play->envCtx.adjAmbientColor[2] = 0;

                    gLightningStrike.state = LIGHTNING_STRIKE_WAIT;

                    if (play->envCtx.lightningMode == LIGHTNING_MODE_LAST) {
                        play->envCtx.lightningMode = LIGHTNING_MODE_OFF;
                    }
                }
                break;
        }
    }

    if (gLightningStrike.state != LIGHTNING_STRIKE_WAIT) {
        Environment_DrawLightningFlash(play, gLightningStrike.flashRed, gLightningStrike.flashGreen,
                                       gLightningStrike.flashBlue, sLightningFlashAlpha);
    }
}

/**
 * Request the number of lightning bolts specified by `num`
 * Note: only 3 lightning bolts can be active at the same time.
 */
void Environment_AddLightningBolts(PlayState* play, u8 num) {
    s16 boltsAdded = 0;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sLightningBolts); i++) {
        if (sLightningBolts[i].state == LIGHTNING_BOLT_INACTIVE) {
            sLightningBolts[i].state = LIGHTNING_BOLT_START;
            boltsAdded++;

            if (boltsAdded >= num) {
                break;
            }
        }
    }
}

/**
 * Draw any active lightning bolt entries contained in `sLightningBolts`
 */
void Environment_DrawLightning(PlayState* play, s32 unused) {
    static void* lightningTextures[] = {
        gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex,
        gEffLightning4Tex, gEffLightning5Tex, gEffLightning6Tex,
        gEffLightning7Tex, gEffLightning8Tex, NULL,
    };
    s16 i;
    f32 dx;
    f32 dz;
    f32 x;
    f32 z;
    s32 pad[2];
    Vec3f unused1 = { 0.0f, 0.0f, 0.0f };
    Vec3f unused2 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(sLightningBolts); i++) {
        FrameInterpolation_RecordOpenChild("Lightning Bolt", i);

        switch (sLightningBolts[i].state) {
            case LIGHTNING_BOLT_START:
                dx = play->view.lookAt.x - play->view.eye.x;
                dz = play->view.lookAt.z - play->view.eye.z;

                x = dx / sqrtf(SQ(dx) + SQ(dz));
                z = dz / sqrtf(SQ(dx) + SQ(dz));

                sLightningBolts[i].pos.x = play->view.eye.x + x * 9500.0f;
                sLightningBolts[i].pos.y = Rand_ZeroOne() * 1000.0f + 4000.0f;
                sLightningBolts[i].pos.z = play->view.eye.z + z * 9500.0f;

                sLightningBolts[i].offset.x = (Rand_ZeroOne() - 0.5f) * 5000.0f;
                sLightningBolts[i].offset.y = 0.0f;
                sLightningBolts[i].offset.z = (Rand_ZeroOne() - 0.5f) * 5000.0f;

                sLightningBolts[i].textureIndex = 0;
                sLightningBolts[i].pitch = (Rand_ZeroOne() - 0.5f) * 40.0f;
                sLightningBolts[i].roll = (Rand_ZeroOne() - 0.5f) * 40.0f;
                sLightningBolts[i].delayTimer = 3 * (i + 1);
                sLightningBolts[i].state++;
                break;
            case LIGHTNING_BOLT_WAIT:
                sLightningBolts[i].delayTimer--;

                if (sLightningBolts[i].delayTimer <= 0) {
                    sLightningBolts[i].state++;
                }
                break;
            case LIGHTNING_BOLT_DRAW:
                if (sLightningBolts[i].textureIndex < 7) {
                    sLightningBolts[i].textureIndex++;
                } else {
                    sLightningBolts[i].state = LIGHTNING_BOLT_INACTIVE;
                }
                break;
        }

        if (sLightningBolts[i].state == LIGHTNING_BOLT_DRAW) {
            Matrix_Translate(sLightningBolts[i].pos.x + sLightningBolts[i].offset.x,
                             sLightningBolts[i].pos.y + sLightningBolts[i].offset.y,
                             sLightningBolts[i].pos.z + sLightningBolts[i].offset.z, MTXMODE_NEW);
            Matrix_RotateX(sLightningBolts[i].pitch * (M_PI / 180.0f), MTXMODE_APPLY);
            Matrix_RotateZ(sLightningBolts[i].roll * (M_PI / 180.0f), MTXMODE_APPLY);
            Matrix_Scale(22.0f, 100.0f, 22.0f, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 128);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 128);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(lightningTextures[sLightningBolts[i].textureIndex]));
            Gfx_SetupDL_61Xlu(play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffLightningDL);
        }
        
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Environment_PlaySceneSequence(PlayState* play) {
    play->envCtx.unk_E0 = 0xFF;

    // both lost woods exits on the bridge from kokiri to hyrule field
    if (((void)0, gSaveContext.entranceIndex) == 0x4DE || ((void)0, gSaveContext.entranceIndex) == 0x5E0) {
        Audio_PlayNatureAmbienceSequence(NATURE_ID_KOKIRI_REGION);
    } else if (((void)0, gSaveContext.forcedSeqId) != NA_BGM_GENERAL_SFX) {
        if (!Environment_IsForcedSequenceDisabled()) {
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | (s32)((void)0, gSaveContext.forcedSeqId));
        }
        gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    } else if (play->sequenceCtx.seqId == NA_BGM_NO_MUSIC) {
        if (play->sequenceCtx.natureAmbienceId == NATURE_ID_NONE) {
            return;
        }
        if (((void)0, gSaveContext.natureAmbienceId) != play->sequenceCtx.natureAmbienceId) {
            Audio_PlayNatureAmbienceSequence(play->sequenceCtx.natureAmbienceId);
        }
    } else if (play->sequenceCtx.natureAmbienceId == NATURE_ID_NONE) {
        // "BGM Configuration"
        osSyncPrintf("\n\n\nBGM設定game_play->sound_info.BGM=[%d] old_bgm=[%d]\n\n", play->sequenceCtx.seqId,
                     ((void)0, gSaveContext.seqId));
        if (((void)0, gSaveContext.seqId) != play->sequenceCtx.seqId) {
            func_800F5550(play->sequenceCtx.seqId);
        }
    } else if (((void)0, gSaveContext.dayTime) > 0x4AAA && ((void)0, gSaveContext.dayTime) < 0xB71D) {
        if (((void)0, gSaveContext.seqId) != play->sequenceCtx.seqId) {
            func_800F5550(play->sequenceCtx.seqId);
        }

        play->envCtx.unk_E0 = 1;
    } else {
        if (((void)0, gSaveContext.natureAmbienceId) != play->sequenceCtx.natureAmbienceId) {
            Audio_PlayNatureAmbienceSequence(play->sequenceCtx.natureAmbienceId);
        }

        if (((void)0, gSaveContext.dayTime) > 0xB71C && ((void)0, gSaveContext.dayTime) < 0xCAAC) {
            play->envCtx.unk_E0 = 3;
        } else if (((void)0, gSaveContext.dayTime) > 0xCAAC || ((void)0, gSaveContext.dayTime) < 0x4555) {
            play->envCtx.unk_E0 = 5;
        } else {
            play->envCtx.unk_E0 = 7;
        }
    }

    osSyncPrintf("\n-----------------\n", ((void)0, gSaveContext.forcedSeqId));
    osSyncPrintf("\n 強制ＢＧＭ=[%d]", ((void)0, gSaveContext.forcedSeqId)); // "Forced BGM"
    osSyncPrintf("\n     ＢＧＭ=[%d]", play->sequenceCtx.seqId);
    osSyncPrintf("\n     エンブ=[%d]", play->sequenceCtx.natureAmbienceId);
    osSyncPrintf("\n     status=[%d]", play->envCtx.unk_E0);

    Audio_SetEnvReverb(play->roomCtx.curRoom.echo);
}

// updates bgm/sfx and other things as the day progresses
void func_80075B44(PlayState* play) {
    switch (play->envCtx.unk_E0) {
        case 0:
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_4 << 4 | NATURE_CHANNEL_CRITTER_5,
                                             CHANNEL_IO_PORT_1, 0);
            if (play->envCtx.unk_EE[0] == 0 && play->envCtx.unk_F2[0] == 0) {
                osSyncPrintf("\n\n\nNa_StartMorinigBgm\n\n");
                func_800F5510(play->sequenceCtx.seqId);
            }
            play->envCtx.unk_E0++;
            break;
        case 1:
            if (gSaveContext.dayTime > 0xB71C) {
                if (play->envCtx.unk_EE[0] == 0 && play->envCtx.unk_F2[0] == 0) {
                    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xF000FF);
                }
                play->envCtx.unk_E0++;
            }
            break;
        case 2:
            if (gSaveContext.dayTime > 0xC000) {
                func_800788CC(NA_SE_EV_DOG_CRY_EVENING);
                play->envCtx.unk_E0++;
            }
            break;
        case 3:
            if (play->envCtx.unk_EE[0] == 0 && play->envCtx.unk_F2[0] == 0) {
                Audio_PlayNatureAmbienceSequence(play->sequenceCtx.natureAmbienceId);
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_1, 1);
            }
            play->envCtx.unk_E0++;
            break;
        case 4:
            if (gSaveContext.dayTime > 0xCAAB) {
                play->envCtx.unk_E0++;
            }
            break;
        case 5:
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_1, 0);
            if (play->envCtx.unk_EE[0] == 0 && play->envCtx.unk_F2[0] == 0) {
                Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_1 << 4 | NATURE_CHANNEL_CRITTER_3,
                                                 CHANNEL_IO_PORT_1, 1);
            }
            play->envCtx.unk_E0++;
            break;
        case 6:
            if ((gSaveContext.dayTime < 0xCAAC) && (gSaveContext.dayTime > 0x4555)) {
                gSaveContext.totalDays++;
                gSaveContext.bgsDayCount++;
                gSaveContext.dogIsLost = true;
                func_80078884(NA_SE_EV_CHICKEN_CRY_M);
                if ((Inventory_ReplaceItem(play, ITEM_WEIRD_EGG, ITEM_CHICKEN) ||
                     Inventory_HatchPocketCucco(play)) &&
                    play->csCtx.state == 0 && !Player_InCsMode(play)) {
                    Message_StartTextbox(play, 0x3066, NULL);
                }
                play->envCtx.unk_E0++;
            }
            break;
        case 7:
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_1 << 4 | NATURE_CHANNEL_CRITTER_3,
                                             CHANNEL_IO_PORT_1, 0);
            if (play->envCtx.unk_EE[0] == 0 && play->envCtx.unk_F2[0] == 0) 
            {
                // OTRTODO: This is where corrupt audio happens. Commenting this out seems to not introduce any side effects?
                // Further investigation is needed...
                //Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_CRITTER_4 << 4 | NATURE_CHANNEL_CRITTER_5,
                                                    //CHANNEL_IO_PORT_1, 1);
            }
            play->envCtx.unk_E0++;
            break;
        case 8:
            if (gSaveContext.dayTime > 0x4AAB) {
                play->envCtx.unk_E0 = 0;
            }
            break;
    }
}

void Environment_DrawCustomLensFlare(PlayState* play) {
    Vec3f pos;

    if (gCustomLensFlareOn) {
        pos.x = gCustomLensFlarePos.x;
        pos.y = gCustomLensFlarePos.y;
        pos.z = gCustomLensFlarePos.z;

        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, pos,
                                  gLensFlareUnused, gLensFlareScale, gLensFlareColorIntensity,
                                  gLensFlareScreenFillAlpha, 0);
    }
}

void Environment_InitGameOverLights(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    sGameOverLightsIntensity = 0;

    Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f, 0, 0, 0,
                              255);
    sNGameOverLightNode = LightContext_InsertLight(play, &play->lightCtx, &sNGameOverLightInfo);

    Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f, 0, 0, 0,
                              255);
    sSGameOverLightNode = LightContext_InsertLight(play, &play->lightCtx, &sSGameOverLightInfo);
}

void Environment_FadeInGameOverLights(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;

    Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f,
                              sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
    Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f,
                              sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);

    if (sGameOverLightsIntensity < 254) {
        sGameOverLightsIntensity += 2;
    }

    if (func_800C0CB8(play)) {
        for (i = 0; i < 3; i++) {
            if (play->envCtx.adjAmbientColor[i] > -255) {
                play->envCtx.adjAmbientColor[i] -= 12;
                play->envCtx.adjLight1Color[i] -= 12;
            }
            play->envCtx.adjFogColor[i] = -255;
        }

        if (play->envCtx.lightSettings.fogFar + play->envCtx.adjFogFar > 900) {
            play->envCtx.adjFogFar -= 100;
        }

        if (play->envCtx.lightSettings.fogNear + play->envCtx.adjFogNear > 950) {
            play->envCtx.adjFogNear -= 10;
        }
    } else {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = 0;
        play->envCtx.screenFillColor[1] = 0;
        play->envCtx.screenFillColor[2] = 0;
        play->envCtx.screenFillColor[3] = sGameOverLightsIntensity;
    }
}

void Environment_FadeOutGameOverLights(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;

    if (sGameOverLightsIntensity >= 3) {
        sGameOverLightsIntensity -= 3;
    } else {
        sGameOverLightsIntensity = 0;
    }

    if (sGameOverLightsIntensity == 1) {
        LightContext_RemoveLight(play, &play->lightCtx, sNGameOverLightNode);
        LightContext_RemoveLight(play, &play->lightCtx, sSGameOverLightNode);
    } else if (sGameOverLightsIntensity >= 2) {
        Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                                  (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z - 10.0f,
                                  sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
        Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                                  (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f,
                                  sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
    }

    if (func_800C0CB8(play)) {
        for (i = 0; i < 3; i++) {
            Math_SmoothStepToS(&play->envCtx.adjAmbientColor[i], 0, 5, 12, 1);
            Math_SmoothStepToS(&play->envCtx.adjLight1Color[i], 0, 5, 12, 1);
            play->envCtx.adjFogColor[i] = 0;
        }
        play->envCtx.adjFogFar = 0;
        play->envCtx.adjFogNear = 0;
    } else {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = 0;
        play->envCtx.screenFillColor[1] = 0;
        play->envCtx.screenFillColor[2] = 0;
        play->envCtx.screenFillColor[3] = sGameOverLightsIntensity;
        if (sGameOverLightsIntensity == 0) {
            play->envCtx.fillScreen = false;
        }
    }
}

void func_800766C4(PlayState* play) {
    u8 max = MAX(play->envCtx.unk_EE[0], play->envCtx.unk_F2[0]);

    if (play->envCtx.unk_EE[1] != max && ((play->state.frames % 8) == 0)) {
        if (play->envCtx.unk_EE[1] < max) {
            play->envCtx.unk_EE[1] += 2;
        } else {
            play->envCtx.unk_EE[1] -= 2;
        }
    }
}

void Environment_FillScreen(GraphicsContext* gfxCtx, u8 red, u8 green, u8 blue, u8 alpha, u8 drawFlags) {
    if (alpha != 0) {
        OPEN_DISPS(gfxCtx);

        if (drawFlags & FILL_SCREEN_OPA) {
            POLY_OPA_DISP = Gfx_SetupDL_57(POLY_OPA_DISP);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
            gDPSetAlphaDither(POLY_OPA_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_OPA_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_OPA_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        if (drawFlags & FILL_SCREEN_XLU) {
            POLY_XLU_DISP = Gfx_SetupDL_57(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, red, green, blue, alpha);

            if ((u32)alpha == 255) {
                gDPSetRenderMode(POLY_XLU_DISP++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
            }

            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
            gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
        }

        CLOSE_DISPS(gfxCtx);
    }
}

Color_RGB8 sSandstormPrimColors[] = {
    { 210, 156, 85 },
    { 255, 200, 100 },
    { 225, 160, 50 },
    { 105, 90, 40 },
};

Color_RGB8 sSandstormEnvColors[] = {
    { 155, 106, 35 },
    { 200, 150, 50 },
    { 170, 110, 0 },
    { 50, 40, 0 },
};

u16 previousPatchedSandstormScreenSize = 0;

void Environment_PatchSandstorm(PlayState* play) {
    if (previousPatchedSandstormScreenSize == ABS(OTRGetRectDimensionFromLeftEdge(0)) + ABS(OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH))) {
        return;
    }

    Gfx gfxPatchSandstormRect[] = {
        gsSPWideTextureRectangle(OTRGetRectDimensionFromLeftEdge(0) << 2, 0,
            OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) << 2, 0x03C0, G_TX_RENDERTILE, 0, 0, 0x008C, -0x008C),
    };
    ResourceMgr_PatchGfxByName(gFieldSandstormDL, "gfxPatchSandstormRect0", 24, gfxPatchSandstormRect[0]);
    ResourceMgr_PatchGfxByName(gFieldSandstormDL, "gfxPatchSandstormRect1", 25, gfxPatchSandstormRect[1]);
    ResourceMgr_PatchGfxByName(gFieldSandstormDL, "gfxPatchSandstormRect2", 26, gfxPatchSandstormRect[2]);

    previousPatchedSandstormScreenSize = ABS(OTRGetRectDimensionFromLeftEdge(0)) + ABS(OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH));
}

void Environment_DrawSandstorm(PlayState* play, u8 sandstormState) {
    s32 primA1;
    s32 envA1;
    s32 primA = play->envCtx.sandstormPrimA;
    s32 envA = play->envCtx.sandstormEnvA;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 pad;
    f32 sp98;
    u16 sp96;
    u16 sp94;
    u16 sp92;

    Environment_PatchSandstorm(play);

    switch (sandstormState) {
        case 3:
            if ((play->sceneNum == SCENE_HAUNTED_WASTELAND) && (play->roomCtx.curRoom.num == 0)) {
                envA1 = 0;
                primA1 = (play->envCtx.sandstormEnvA > 128) ? 255 : play->envCtx.sandstormEnvA >> 1;
            } else {
                primA1 = play->state.frames % 128;
                if (primA1 > 64) {
                    primA1 = 128 - primA1;
                }
                primA1 += 73;
                envA1 = 128;
            }
            break;
        case 1:
            primA1 = 255;
            envA1 = (play->envCtx.sandstormPrimA >= 255) ? 255 : 128;
            break;
        case 2:
            envA1 = 128;
            if (play->envCtx.sandstormEnvA > 128) {
                primA1 = 0xFF;
            } else {
                primA1 = play->state.frames % 128;
                if (primA1 > 64) {
                    primA1 = 128 - primA1;
                }
                primA1 += 73;
            }
            if ((primA1 >= primA) && (primA1 != 255)) {
                play->envCtx.sandstormState = 3;
            }
            break;
        case 4:
            envA1 = 0;
            primA1 = (play->envCtx.sandstormEnvA > 128) ? 255 : play->envCtx.sandstormEnvA >> 1;

            if (primA == 0) {
                play->envCtx.sandstormState = 0;
            }
            break;
    }

    if (ABS(primA - primA1) < 9) {
        primA = primA1;
    } else if (primA1 < primA) {
        primA = primA - 9;
    } else {
        primA = primA + 9;
    }
    if (ABS(envA - envA1) < 9) {
        envA = envA1;
    } else if (envA1 < envA) {
        envA = envA - 9;
    } else {
        envA = envA + 9;
    }
    play->envCtx.sandstormPrimA = primA;
    play->envCtx.sandstormEnvA = envA;

    sp98 = (512.0f - (primA + envA)) * (3.0f / 128.0f);
    if (sp98 > 6.0f) {
        sp98 = 6.0f;
    }
    if (play->envCtx.indoors || (play->envCtx.unk_BF != 0xFF)) {
        primColor.r = sSandstormPrimColors[1].r;
        primColor.g = sSandstormPrimColors[1].g;
        primColor.b = sSandstormPrimColors[1].b;
        envColor.r = sSandstormEnvColors[1].r;
        envColor.g = sSandstormEnvColors[1].g;
        envColor.b = sSandstormEnvColors[1].b;
    } else if (D_8011FDCC == D_8011FDD0) {
        primColor.r = sSandstormPrimColors[D_8011FDCC].r;
        primColor.g = sSandstormPrimColors[D_8011FDCC].g;
        primColor.b = sSandstormPrimColors[D_8011FDCC].b;
        envColor.r = sSandstormEnvColors[D_8011FDCC].r;
        envColor.g = sSandstormEnvColors[D_8011FDCC].g;
        envColor.b = sSandstormEnvColors[D_8011FDCC].b;
    } else {
        primColor.r = (s32)F32_LERP(sSandstormPrimColors[D_8011FDCC].r, sSandstormPrimColors[D_8011FDD0].r, D_8011FDD4);
        primColor.g = (s32)F32_LERP(sSandstormPrimColors[D_8011FDCC].g, sSandstormPrimColors[D_8011FDD0].g, D_8011FDD4);
        primColor.b = (s32)F32_LERP(sSandstormPrimColors[D_8011FDCC].b, sSandstormPrimColors[D_8011FDD0].b, D_8011FDD4);
        envColor.r = (s32)F32_LERP(sSandstormEnvColors[D_8011FDCC].r, sSandstormEnvColors[D_8011FDD0].r, D_8011FDD4);
        envColor.g = (s32)F32_LERP(sSandstormEnvColors[D_8011FDCC].g, sSandstormEnvColors[D_8011FDD0].g, D_8011FDD4);
        envColor.b = (s32)F32_LERP(sSandstormEnvColors[D_8011FDCC].b, sSandstormEnvColors[D_8011FDD0].b, D_8011FDD4);
    }

    envColor.r = ((envColor.r * sp98) + ((6.0f - sp98) * primColor.r)) * (1.0f / 6.0f);
    envColor.g = ((envColor.g * sp98) + ((6.0f - sp98) * primColor.g)) * (1.0f / 6.0f);
    envColor.b = ((envColor.b * sp98) + ((6.0f - sp98) * primColor.b)) * (1.0f / 6.0f);

    sp96 = (s32)(D_8015FDB0 * (11.0f / 6.0f));
    sp94 = (s32)(D_8015FDB0 * (9.0f / 6.0f));
    sp92 = (s32)(D_8015FDB0 * (6.0f / 6.0f));

    OPEN_DISPS(play->state.gfxCtx);

    POLY_XLU_DISP = Gfx_SetupDL_64(POLY_XLU_DISP);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColor.r, primColor.g, primColor.b, play->envCtx.sandstormPrimA);
    gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, play->envCtx.sandstormEnvA);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, (u32)sp96 % 0x1000, 0, 0x200, 0x20, 1, (u32)sp94 % 0x1000,
                                0xFFF - ((u32)sp92 % 0x1000), 0x100, 0x40));
    gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);

    gSPDisplayList(POLY_XLU_DISP++, gFieldSandstormDL);
    CLOSE_DISPS(play->state.gfxCtx);

    D_8015FDB0 += (s32)sp98;
}

void Environment_AdjustLights(PlayState* play, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp;
    s32 i;

    if (play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_5 && func_800C0CB8(play)) {
        arg1 = CLAMP_MIN(arg1, 0.0f);
        arg1 = CLAMP_MAX(arg1, 1.0f);

        temp = arg1 - arg3;
        if (arg1 < arg3) {
            temp = 0.0f;
        }

        play->envCtx.adjFogNear = (arg2 - play->envCtx.lightSettings.fogNear) * temp;

        if (arg1 == 0.0f) {
            for (i = 0; i < 3; i++) {
                play->envCtx.adjFogColor[i] = 0;
            }
        } else {
            temp = arg1 * 5.0f;
            temp = CLAMP_MAX(temp, 1.0f);

            for (i = 0; i < 3; i++) {
                play->envCtx.adjFogColor[i] = -(s16)(play->envCtx.lightSettings.fogColor[i] * temp);
            }
        }

        if (arg4 <= 0.0f) {
            return;
        }

        arg1 *= arg4;

        for (i = 0; i < 3; i++) {
            play->envCtx.adjAmbientColor[i] = -(s16)(play->envCtx.lightSettings.ambientColor[i] * arg1);
            play->envCtx.adjLight1Color[i] = -(s16)(play->envCtx.lightSettings.light1Color[i] * arg1);
        }
    }
}

s32 Environment_GetBgsDayCount(void) {
    return gSaveContext.bgsDayCount;
}

void Environment_ClearBgsDayCount(void) {
    gSaveContext.bgsDayCount = 0;
}

s32 Environment_GetTotalDays(void) {
    return gSaveContext.totalDays;
}

void Environment_ForcePlaySequence(u16 seqId) {
    gSaveContext.forcedSeqId = seqId;
}

s32 Environment_IsForcedSequenceDisabled(void) {
    s32 isDisabled = false;

    if (gSaveContext.forcedSeqId == NA_BGM_DISABLED) {
        isDisabled = true;
    }

    return isDisabled;
}

void Environment_PlayStormNatureAmbience(PlayState* play) {
    if (play->sequenceCtx.natureAmbienceId == NATURE_ID_NONE) {
        Audio_PlayNatureAmbienceSequence(NATURE_ID_MARKET_NIGHT);
    } else {
        Audio_PlayNatureAmbienceSequence(play->sequenceCtx.natureAmbienceId);
    }

    Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 1);
    Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 1);
}

void Environment_StopStormNatureAmbience(PlayState* play) {
    Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
    Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);

    if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
        gSaveContext.seqId = NA_BGM_NATURE_SFX_RAIN;
        Environment_PlaySceneSequence(play);
    }
}

void Environment_WarpSongLeave(PlayState* play) {
    gWeatherMode = 0;
    gSaveContext.cutsceneIndex = 0;
    gSaveContext.respawnFlag = -3;
    play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex;
    play->sceneLoadFlag = 0x14;
    play->fadeTransition = 3;
    gSaveContext.nextTransitionType = 3;

    switch (play->nextEntranceIndex) {
        case 0x147:
            Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEATH_MOUNTAIN_CRATER);
            break;
        case 0x0102:
            Flags_SetEventChkInf(EVENTCHKINF_ENTERED_LAKE_HYLIA);
            break;
        case 0x0123:
            Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DESERT_COLOSSUS);
            break;
        case 0x00E4:
            Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GRAVEYARD);
            break;
        case 0x0053:
            Flags_SetEventChkInf(EVENTCHKINF_ENTERED_TEMPLE_OF_TIME);
            break;
        case 0x00FC:
            break;
    }
}
