#include "ultra64.h"
#include "z64light.h"
#include "z64math.h"
#include <libultraship/bridge/consolevariablebridge.h>

typedef enum {
    /* 0x00 */ LIGHTNING_BOLT_START,
    /* 0x01 */ LIGHTNING_BOLT_WAIT,
    /* 0x02 */ LIGHTNING_BOLT_DRAW,
    /* 0xFF */ LIGHTNING_BOLT_INACTIVE = 0xFF
} LightningBoltState;

typedef struct {
    /* 0x00 */ u8 state;
    /* 0x04 */ Vec3f offset;
    /* 0x10 */ Vec3f pos;
    /* 0x1C */ s8 pitch;
    /* 0x1D */ s8 roll;
    /* 0x1E */ u8 textureIndex;
    /* 0x1F */ u8 delayTimer;
} LightningBolt; // size = 0x20

// Variables are put before most headers as a hacky way to bypass bss reordering
#include "z64environment.h"
#include "global.h"
#include "sys_cfb.h"
#include "BenPort.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

u8 D_801F4E30;
u8 sInitSkyboxConfig;
u8 gCustomLensFlare1On;
Vec3f gCustomLensFlare1Pos;
f32 D_801F4E44;
f32 D_801F4E48;
s16 D_801F4E4C;
u8 gCustomLensFlare2On;
Vec3f gCustomLensFlare2Pos;
f32 D_801F4E5C;
f32 D_801F4E60;
s16 D_801F4E64;
struct LightningStrike gLightningStrike;
f32 D_801F4E74;
u16 D_801F4E78;
u16 gSkyboxNumStars;
LightningBolt sLightningBolts[3];
LightNode* sNGameOverLightNode;
LightInfo sNGameOverLightInfo;
LightNode* sSGameOverLightNode;
LightInfo sSGameOverLightInfo;
f32 sSunEnvAlpha;
f32 sSunColor;
f32 sSunScale;
f32 sSunPrimAlpha;
s32 sSunDepthTestX;
s32 sSunDepthTestY;
f32 D_801F4F28;
s16 sLightningFlashAlpha;
u16 sSandstormScroll;
u8 D_801F4F30;
u8 D_801F4F31;
u8 sEnvIsTimeStopped;
u8 D_801F4F33;
u8 sGameOverLightsIntensity;
Gfx* sSkyboxStarsDList;

// Data
f32 sSandstormLerpScale = 0.0f;
s32 sSunScreenDepth = GPACK_ZDZ(G_MAXFBZ, 0);
s32 sEnvSkyboxNumStars = 0;
u16 sTimeJump = 0;
u8 sSandstormColorIndex = 0;
u8 sNextSandstormColorIndex = 0;
u8 D_801BDBA8 = false;
u8 D_801BDBAC = 0;
u8 gWeatherMode = WEATHER_MODE_CLEAR;
u8 gLightConfigAfterUnderwater = 0;
u8 gInterruptSongOfStorms = false;
u8 sSkyboxIsChanging = false;
u8 D_801BDBC0 = 0;
u8 D_801BDBC4 = 0;
u8 gSceneSeqState = SCENESEQ_DEFAULT;

typedef struct {
    /* 0x0 */ u16 startTime;
    /* 0x2 */ u16 endTime;
    /* 0x4 */ u8 lightSetting;
    /* 0x5 */ u8 nextLightSetting;
} TimeBasedLightEntry; // size = 0x6

TimeBasedLightEntry sTimeBasedLightConfigs[][7] = {
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 3, 3 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 3, 12 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 1 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 2 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 2, 3 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 3, 3 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 7, 7 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 7, 8 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 4, 5 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 5, 5 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 5, 6 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 6, 7 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 7, 7 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 11, 11 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 11, 8 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 8, 9 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 9, 9 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 9, 10 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 10, 11 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 11, 11 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 15, 15 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 15, 16 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 12, 13 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 13, 13 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 13, 14 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 14, 15 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 15, 15 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 19, 19 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 19, 16 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 16, 17 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 17, 17 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 17, 18 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 18, 19 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 19, 19 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 23, 23 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 23, 20 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 20, 21 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 21, 21 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 21, 22 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 22, 23 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 23, 23 },
    },
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 27, 27 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(6, 0), 27, 24 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 24, 25 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 25, 25 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 25, 26 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(19, 0), 26, 27 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 27, 27 },
    },
};

typedef struct {
    /* 0x0 */ u16 startTime;
    /* 0x2 */ u16 endTime;
    /* 0x4 */ u8 skybox1Index;
    /* 0x5 */ u8 skybox2Index;
    /* 0x6 */ u8 color1Index;
    /* 0x7 */ u8 color2Index;
} TimeBasedSkyboxEntry; // size = 0x8

TimeBasedSkyboxEntry sTimeBasedSkyboxConfigs[SKYBOX_CONFIG_MAX][9] = {
    // SKYBOX_CONFIG_0
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 3, 3 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 3, 4 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 4, 4 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 0, 1 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 1, 2 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 2, 2 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 2, 3 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 3, 3 },
    },
    // SKYBOX_CONFIG_1
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 7, 7 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 7, 8 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 8, 8 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 0, 4, 5 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 5, 5 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 1, 5, 6 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 6, 6 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 6, 7 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 7, 7 },
    },
    // SKYBOX_CONFIG_2
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 11, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 8, 9 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 9, 9 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 9, 10 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 10, 10 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 10, 11 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 11, 11 },
    },
    // SKYBOX_CONFIG_3
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 15, 15 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 15, 16 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 16, 16 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 12, 13 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 13, 13 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 13, 14 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 14, 14 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 14, 15 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 15, 15 },
    },
    // SKYBOX_CONFIG_4
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 19, 19 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 19, 20 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 20, 20 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 0, 16, 17 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 17, 17 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 1, 17, 18 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 18, 18 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 0, 18, 19 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 19, 19 },
    },
    // SKYBOX_CONFIG_5
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 23, 23 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 23, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 20, 21 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 21, 21 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 21, 22 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 22, 22 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 22, 23 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 23, 23 },
    },
    // SKYBOX_CONFIG_6
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 27, 27 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 27, 28 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 28, 28 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 24, 25 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 25, 25 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 25, 26 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 26, 26 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 26, 27 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 27, 27 },
    },
    // SKYBOX_CONFIG_7
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 31, 31 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 31, 32 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 32, 32 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 0, 28, 29 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 29, 29 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 1, 29, 30 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 30, 30 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 0, 30, 31 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 31, 31 },
    },
    // SKYBOX_CONFIG_8
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 35, 35 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 35, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 32, 33 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 33, 33 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 33, 34 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 34, 34 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 34, 35 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 35, 35 },
    },
    // SKYBOX_CONFIG_9
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 39, 39 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 39, 40 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 40, 40 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 36, 37 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 37, 37 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 37, 38 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 38, 38 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 38, 39 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 39, 39 },
    },
    // SKYBOX_CONFIG_10
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 43, 43 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 43, 44 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 44, 44 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 0, 40, 41 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 41, 41 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 1, 41, 42 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 42, 42 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 0, 42, 43 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 43, 43 },
    },
    // SKYBOX_CONFIG_11
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 47, 47 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 47, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 44, 45 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 45, 45 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 45, 46 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 46, 46 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 46, 47 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 47, 47 },
    },
    // SKYBOX_CONFIG_12
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 51, 51 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 51, 48 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 48, 49 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 49, 49 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 49, 49 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 49, 50 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 50, 50 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 50, 51 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 51, 51 },
    },
    // SKYBOX_CONFIG_13
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 55, 55 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 55, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 52, 53 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 53, 53 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 53, 54 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 54, 54 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 54, 54 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 55, 55 },
    },
    // SKYBOX_CONFIG_14
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 59, 59 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 59, 56 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 56, 56 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 56, 57 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 57, 57 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 57, 58 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 58, 58 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 58, 58 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 59, 59 },
    },
    // SKYBOX_CONFIG_15
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 63, 63 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 63, 60 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 60, 60 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 60, 61 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 61, 61 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 61, 62 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 62, 62 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 62, 62 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 63, 63 },
    },
    // SKYBOX_CONFIG_16
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 67, 67 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 67, 64 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 64, 64 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 64, 65 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 65, 65 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 65, 66 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 66, 66 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 66, 66 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 67, 67 },
    },
    // SKYBOX_CONFIG_17
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 1, 1 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 1, 1 },
    },
    // SKYBOX_CONFIG_18
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 71, 71 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 71, 72 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 72, 72 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 68, 69 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 69, 69 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 69, 70 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 70, 70 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 70, 71 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 71, 71 },
    },
    // SKYBOX_CONFIG_19
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 75, 75 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 75, 76 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 76, 76 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 72, 73 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 73, 73 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 73, 74 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 74, 74 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 74, 75 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 75, 75 },
    },
    // SKYBOX_CONFIG_20
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 79, 79 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 79, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 76, 77 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 77, 77 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 77, 78 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 78, 78 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 78, 79 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 79, 79 },
    },
    // SKYBOX_CONFIG_21
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 83, 83 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 83, 84 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 84, 84 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 80, 81 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 81, 81 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 81, 82 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 82, 82 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 82, 83 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 83, 83 },
    },
    // SKYBOX_CONFIG_22
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 87, 87 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 87, 88 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 88, 88 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 84, 85 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 85, 85 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 85, 86 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 86, 86 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 86, 87 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 87, 87 },
    },
    // SKYBOX_CONFIG_23
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 91, 91 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 91, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 88, 89 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 89, 89 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 89, 90 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 90, 90 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 90, 91 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 91, 91 },
    },
    // SKYBOX_CONFIG_24
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 11, 11 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 11, 11 },
    },
    // SKYBOX_CONFIG_25
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 95, 95 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 95, 96 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 96, 96 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 92, 93 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 93, 93 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 93, 94 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 94, 94 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 94, 95 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 95, 95 },
    },
    // SKYBOX_CONFIG_26
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 1, 1, 99, 99 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 1, 1, 99, 100 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 100, 100 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 96, 97 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 97, 97 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 1, 97, 98 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 1, 1, 98, 98 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 1, 1, 98, 99 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 1, 1, 99, 99 },
    },
    // SKYBOX_CONFIG_27
    {
        { CLOCK_TIME(0, 0), CLOCK_TIME(4, 0), 0, 0, 103, 103 },
        { CLOCK_TIME(4, 0), CLOCK_TIME(5, 0), 0, 1, 103, 52 },
        { CLOCK_TIME(5, 0), CLOCK_TIME(6, 0), 1, 1, 52, 52 },
        { CLOCK_TIME(6, 0), CLOCK_TIME(8, 0), 1, 1, 100, 101 },
        { CLOCK_TIME(8, 0), CLOCK_TIME(16, 0), 1, 1, 101, 101 },
        { CLOCK_TIME(16, 0), CLOCK_TIME(17, 0), 1, 0, 101, 102 },
        { CLOCK_TIME(17, 0), CLOCK_TIME(18, 0), 0, 0, 102, 102 },
        { CLOCK_TIME(18, 0), CLOCK_TIME(19, 0), 0, 0, 102, 103 },
        { CLOCK_TIME(19, 0), CLOCK_TIME(24, 0) - 1, 0, 0, 103, 103 },
    },
};

typedef struct {
    /* 0x0 */ RomFile file;
    /* 0x8 */ RomFile palette;
} SkyboxFile; // size = 0x10

SkyboxFile sNormalSkyFiles[] = {
    {
        ROM_FILE(d2_fine_static),
        ROM_FILE_UNSET,
    },
    {
        ROM_FILE(d2_cloud_static),
        ROM_FILE_UNSET,
    },
};

void Environment_UpdatePostmanEvents(PlayState* play);
void Environment_UpdateRain(PlayState* play);
void Environment_UpdateTimeBasedSequence(PlayState* play);
u8 func_800FE5D0(struct PlayState* play);
void Environment_JumpForwardInTime(void);

void Environment_GraphCallback(GraphicsContext* gfxCtx, void* arg) {
    PlayState* play = (PlayState*)arg;

    // 2S2H [Port] Batch all the coordinates that need their depth checked by calling prepare
    OTRGetPixelDepthPrepare(sSunDepthTestX, sSunDepthTestY);
    Lights_GlowCheckPrepare(play);

    sSunScreenDepth = SysCfb_GetZBufferPixel(sSunDepthTestX, sSunDepthTestY);
    Lights_GlowCheck(play);
}

typedef struct {
    /* 0x0 */ s32 mantissaShift; // shift applied to the mantissa of the z buffer value
    /* 0x4 */ s32 base;          // 15.3 fixed-point base value for the exponent
} ZBufValConversionEntry;        // size = 0x8

ZBufValConversionEntry sZBufValConversionTable[1 << 3] = {
    { 6, 0x0000 << 3 }, { 5, 0x4000 << 3 }, { 4, 0x6000 << 3 }, { 3, 0x7000 << 3 },
    { 2, 0x7800 << 3 }, { 1, 0x7C00 << 3 }, { 0, 0x7E00 << 3 }, { 0, 0x7F00 << 3 },
};

#define ZBUFVAL_EXPONENT(v) (((v) >> 15) & 7)
#define ZBUFVAL_MANTISSA(v) (((v) >> 4) & 0x7FF)

/**
 * Convert an 18-bits Z buffer value to a fixed point 15.3 value
 *
 * zBufferVal is 18 bits:
 *   3: Exponent of z value
 *  11: Mantissa of z value
 *   4: dz value (unused)
 */
s32 Environment_ZBufValToFixedPoint(s32 zBufferVal) {
    // base[exp] + mantissa << shift[exp]
    s32 ret = (ZBUFVAL_MANTISSA(zBufferVal) << sZBufValConversionTable[ZBUFVAL_EXPONENT(zBufferVal)].mantissaShift) +
              sZBufValConversionTable[ZBUFVAL_EXPONENT(zBufferVal)].base;

    return ret;
}

void Environment_Init(PlayState* play2, EnvironmentContext* envCtx, s32 arg2) {
    PlayState* play = play2;
    f32 temp_ft4;
    u8 dayOffset;
    s16 i;

    CREG(1) = 0;

    gSaveContext.sunsSongState = SUNSSONG_INACTIVE;

    gSaveContext.skyboxTime = CURRENT_TIME;

    Environment_JumpForwardInTime();

    if ((CURRENT_TIME >= CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        gSaveContext.save.isNight = true;
    } else {
        gSaveContext.save.isNight = false;
    }

    play->state.gfxCtx->callback = Environment_GraphCallback;
    play->state.gfxCtx->callbackArg = play;

    Lights_DirectionalSetInfo(&envCtx->dirLight1, 80, 80, 80, 80, 80, 80);
    LightContext_InsertLight(play, &play->lightCtx, &envCtx->dirLight1);

    Lights_DirectionalSetInfo(&envCtx->dirLight2, 80, 80, 80, 80, 80, 80);
    LightContext_InsertLight(play, &play->lightCtx, &envCtx->dirLight2);

    envCtx->changeSkyboxState = CHANGE_SKYBOX_INACTIVE;
    envCtx->changeSkyboxTimer = 0;
    envCtx->changeLightEnabled = false;
    envCtx->changeLightTimer = 0;
    envCtx->skyboxDmaState = 0;
    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;

    envCtx->glareAlpha = 0.0f;
    envCtx->lensFlareAlphaScale = 0.0f;

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 8)) {
        gSaveContext.save.day = 1;
    }

    switch (gSaveContext.save.day) {
        default:
        case 0:
        case 1:
            envCtx->lightConfig = 0;
            envCtx->changeLightNextConfig = 0;
            break;

        case 2:
            envCtx->lightConfig = 3;
            envCtx->changeLightNextConfig = 3;
            break;

        case 3:
            envCtx->lightConfig = 4;
            envCtx->changeLightNextConfig = 4;
            break;
    }

    envCtx->lightSetting = 0;
    envCtx->prevLightSetting = 0;
    envCtx->lightBlendOverride = LIGHT_BLEND_OVERRIDE_NONE;
    envCtx->stormRequest = STORM_REQUEST_NONE;
    envCtx->stormState = STORM_STATE_OFF;
    envCtx->lightningState = LIGHTNING_OFF;
    envCtx->timeSeqState = TIMESEQ_DAY_BGM;
    envCtx->fillScreen = 0;
    envCtx->screenFillColor[0] = 0;
    envCtx->screenFillColor[1] = 0;
    envCtx->screenFillColor[2] = 0;
    envCtx->screenFillColor[3] = 0;
    envCtx->customSkyboxFilter = false;
    envCtx->skyboxFilterColor[0] = 0;
    envCtx->skyboxFilterColor[1] = 0;
    envCtx->skyboxFilterColor[2] = 0;
    envCtx->skyboxFilterColor[3] = 0;
    envCtx->sandstormState = SANDSTORM_OFF;
    envCtx->sandstormPrimA = 0;
    envCtx->sandstormEnvA = 0;
    envCtx->lightBlend = 1.0f;

    gLightningStrike.state = LIGHTNING_STRIKE_WAIT;
    gLightningStrike.flashRed = 0;
    gLightningStrike.flashGreen = 0;
    gLightningStrike.flashBlue = 0;

    sLightningFlashAlpha = 0;

    D_801F4F30 = 0xFF;
    D_801F4F31 = 0;
    D_801F4E30 = 0;
    D_801F4F33 = 0;

    gSaveContext.cutsceneTransitionControl = 0;

    envCtx->adjLightSettings.ambientColor[0] = 0;
    envCtx->adjLightSettings.ambientColor[1] = 0;
    envCtx->adjLightSettings.ambientColor[2] = 0;
    envCtx->adjLightSettings.light1Color[0] = 0;
    envCtx->adjLightSettings.light1Color[1] = 0;
    envCtx->adjLightSettings.light1Color[2] = 0;
    envCtx->adjLightSettings.light2Color[0] = 0;
    envCtx->adjLightSettings.light2Color[1] = 0;
    envCtx->adjLightSettings.light2Color[2] = 0;
    envCtx->adjLightSettings.fogColor[0] = 0;
    envCtx->adjLightSettings.fogColor[1] = 0;
    envCtx->adjLightSettings.fogColor[2] = 0;
    envCtx->adjLightSettings.fogNear = 0;
    envCtx->adjLightSettings.zFar = 0;

    envCtx->sunPos.x = -(Math_SinS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    envCtx->sunPos.y = (Math_CosS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 120.0f) * 25.0f;
    envCtx->sunPos.z = (Math_CosS(CURRENT_TIME - CLOCK_TIME(12, 0)) * 20.0f) * 25.0f;

    envCtx->windDirection.x = 80;
    envCtx->windDirection.y = 80;
    envCtx->windDirection.z = 80;
    envCtx->windSpeed = 20.0f;

    envCtx->lightBlendEnabled = false;
    envCtx->lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    envCtx->lightBlendRateOverride = LIGHT_BLENDRATE_OVERRIDE_NONE;

    R_TIME_SPEED = envCtx->sceneTimeSpeed = 0;
    R_TIME_SPEED = R_TIME_SPEED; // Set to itself
    R_ENV_DISABLE_DBG = false;

    CREG(64) = 0;

    play->envCtx.precipitation[PRECIP_RAIN_MAX] = 0;
    play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
    play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;
    play->envCtx.precipitation[PRECIP_SOS_MAX] = 0;

    sInitSkyboxConfig = envCtx->skyboxConfig;

    dayOffset = 0;
    if (((void)0, gSaveContext.save.day) != 0) {
        dayOffset = ((void)0, gSaveContext.save.day) - 1;
    }
    envCtx->skyboxConfig = dayOffset + (sInitSkyboxConfig * 3);
    envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;

    if (sInitSkyboxConfig == SKYBOX_CONFIG_4) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_14;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_5) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_16;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_6) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_17;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_7) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_18 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_8) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_21 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_9) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_24;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_10) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_25 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    }

    if (dayOffset >= 3) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_13;
        envCtx->changeSkyboxNextConfig = SKYBOX_CONFIG_13;
    }

    if (envCtx->skyboxConfig >= SKYBOX_CONFIG_MAX) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_0;
        envCtx->changeSkyboxNextConfig = SKYBOX_CONFIG_0;
    }

    D_801F4E74 = 0.0f;

    if ((play->sceneId == SCENE_IKANA) &&
        ((((void)0, gSaveContext.sceneLayer) == 0) || (((void)0, gSaveContext.sceneLayer) == 1)) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
        play->skyboxId = SKYBOX_3;
        envCtx->lightConfig = 5;
        envCtx->changeLightNextConfig = 5;
        D_801F4E74 = 1.0f;
    }

    if (gSaveContext.retainWeatherMode || (gSaveContext.respawnFlag != 0)) {
        switch ((u32)gWeatherMode) {
            case WEATHER_MODE_2:
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) &&
                    (!CVarGetInteger("gFixes.FixIkanaGreatFairyFountainColor", 0) ||
                     (play->sceneId != SCENE_YOUSEI_IZUMI))) {
                    play->skyboxId = SKYBOX_3;
                    envCtx->lightConfig = 5;
                    envCtx->changeLightNextConfig = 5;
                    D_801F4E74 = 1.0f;
                } else {
                    gWeatherMode = WEATHER_MODE_CLEAR;
                }
                break;

            default:
                break;
        }

        play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
        play->envCtx.precipitation[PRECIP_SNOW_MAX] = 0;

        if (gWeatherMode == WEATHER_MODE_RAIN) {
            if (((u32)CURRENT_DAY == 2) && (CURRENT_TIME >= CLOCK_TIME(7, 0)) && (CURRENT_TIME < CLOCK_TIME(17, 30))) {
                if (Environment_GetStormState(play) != STORM_STATE_OFF) {
                    play->envCtx.precipitation[PRECIP_RAIN_MAX] = 60;
                }
                play->envCtx.precipitation[PRECIP_RAIN_CUR] = 60;
            } else {
                gWeatherMode = WEATHER_MODE_CLEAR;
                Environment_StopStormNatureAmbience(play);
            }
        } else if (gWeatherMode == WEATHER_MODE_SNOW) {
            play->envCtx.precipitation[PRECIP_SNOW_CUR] = 128;
            play->envCtx.precipitation[PRECIP_SNOW_MAX] = 128;
            Environment_StopStormNatureAmbience(play);
        } else {
            Environment_StopStormNatureAmbience(play);
        }
    } else {
        gWeatherMode = WEATHER_MODE_CLEAR;
        Environment_StopStormNatureAmbience(play);
    }

    gInterruptSongOfStorms = false;
    gLightConfigAfterUnderwater = 0;
    sSkyboxIsChanging = false;
    gSaveContext.retainWeatherMode = false;

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
    D_801F4DDC = 1;

    for (i = 0; i < ARRAY_COUNT(sLightningBolts); i++) {
        sLightningBolts[i].state = LIGHTNING_BOLT_INACTIVE;
    }

    play->roomCtx.unk7A[0] = 0;
    play->roomCtx.unk7A[1] = 0;

    for (i = 0; i < ARRAY_COUNT(play->csCtx.actorCues); i++) {
        play->csCtx.actorCues[i] = NULL;
    }

    gCustomLensFlare1On = false;
    gCustomLensFlare2On = false;
    Rumble_StateReset();

    sEnvSkyboxNumStars = 0;
    gSkyboxNumStars = 0;
    D_801BDBA8 = false;
    sEnvIsTimeStopped = false;
    sSunPrimAlpha = 255.0f;

    Environment_UpdatePostmanEvents(play);
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

f32 Environment_LerpWeight(u16 max, u16 min, u16 val) {
    f32 diff = max - min;

    if (diff != 0.0f) {
        f32 ret = 1.0f - (max - val) / diff;

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

Color_RGBA8 sSkyboxPrimColors[] = {
    { 181, 100, 72, 0 },  { 255, 255, 255, 0 }, { 255, 164, 63, 0 },  { 70, 90, 100, 0 },   { 180, 110, 70, 0 },
    { 140, 120, 90, 0 },  { 186, 107, 70, 0 },  { 50, 90, 120, 0 },   { 160, 104, 125, 0 }, { 225, 234, 150, 0 },
    { 243, 84, 33, 0 },   { 112, 37, 55, 0 },   { 181, 100, 27, 0 },  { 255, 255, 255, 0 }, { 186, 107, 69, 0 },
    { 10, 40, 90, 0 },    { 180, 120, 100, 0 }, { 140, 120, 110, 0 }, { 186, 107, 70, 0 },  { 0, 50, 60, 0 },
    { 160, 104, 215, 0 }, { 225, 234, 150, 0 }, { 243, 84, 33, 0 },   { 100, 25, 45, 0 },   { 181, 100, 27, 0 },
    { 255, 255, 255, 0 }, { 186, 107, 69, 0 },  { 10, 10, 50, 0 },    { 165, 110, 63, 0 },  { 150, 120, 90, 0 },
    { 166, 112, 65, 0 },  { 10, 40, 60, 0 },    { 160, 104, 215, 0 }, { 225, 234, 150, 0 }, { 243, 84, 33, 0 },
    { 100, 20, 40, 0 },   { 181, 100, 27, 0 },  { 255, 255, 255, 0 }, { 186, 107, 69, 0 },  { 0, 50, 70, 0 },
    { 180, 126, 70, 0 },  { 150, 140, 120, 0 }, { 225, 164, 63, 0 },  { 0, 70, 80, 0 },     { 160, 104, 215, 0 },
    { 225, 234, 150, 0 }, { 243, 84, 33, 0 },   { 100, 40, 65, 0 },   { 105, 72, 57, 0 },   { 150, 120, 90, 0 },
    { 80, 56, 21, 0 },    { 60, 46, 40, 0 },    { 210, 120, 180, 0 }, { 210, 120, 180, 0 }, { 210, 120, 180, 0 },
    { 210, 120, 180, 0 }, { 100, 90, 110, 0 },  { 100, 90, 110, 0 },  { 100, 90, 110, 0 },  { 100, 90, 110, 0 },
    { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, { 25, 53, 235, 0 },
    { 25, 53, 235, 0 },   { 25, 53, 235, 0 },   { 25, 53, 235, 0 },   { 198, 121, 88, 0 },  { 255, 255, 255, 0 },
    { 255, 164, 70, 0 },  { 55, 53, 91, 0 },    { 199, 126, 78, 0 },  { 159, 125, 103, 0 }, { 186, 107, 59, 0 },
    { 50, 90, 120, 0 },   { 171, 113, 131, 0 }, { 225, 234, 180, 0 }, { 237, 84, 37, 0 },   { 133, 48, 75, 0 },
    { 198, 121, 88, 0 },  { 255, 255, 255, 0 }, { 255, 164, 70, 0 },  { 55, 53, 91, 0 },    { 209, 135, 100, 0 },
    { 180, 177, 158, 0 }, { 196, 127, 79, 0 },  { 50, 90, 120, 0 },   { 171, 113, 131, 0 }, { 225, 234, 180, 0 },
    { 237, 84, 37, 0 },   { 133, 48, 75, 0 },   { 198, 121, 88, 0 },  { 235, 250, 235, 0 }, { 255, 164, 70, 0 },
    { 55, 53, 91, 0 },    { 205, 140, 110, 0 }, { 159, 125, 103, 0 }, { 186, 107, 59, 0 },  { 50, 90, 120, 0 },
    { 171, 113, 131, 0 }, { 225, 234, 180, 0 }, { 237, 84, 37, 0 },   { 133, 48, 75, 0 },
};

Color_RGBA8 sSkyboxEnvColors[] = {
    { 102, 135, 99, 0 }, { 0, 15, 69, 0 },    { 110, 55, 110, 0 }, { 0, 0, 100, 0 },   { 70, 10, 10, 0 },
    { 40, 0, 10, 0 },    { 40, 40, 40, 0 },   { 20, 20, 30, 0 },   { 60, 0, 10, 0 },   { 65, 45, 0, 0 },
    { 20, 25, 90, 0 },   { 0, 5, 25, 0 },     { 102, 135, 99, 0 }, { 0, 4, 199, 0 },   { 60, 65, 89, 0 },
    { 0, 0, 0, 0 },      { 70, 10, 10, 0 },   { 20, 0, 30, 0 },    { 40, 40, 40, 0 },  { 0, 0, 20, 0 },
    { 60, 0, 10, 0 },    { 65, 45, 0, 0 },    { 20, 25, 90, 0 },   { 0, 0, 30, 0 },    { 102, 135, 99, 0 },
    { 0, 4, 199, 0 },    { 60, 65, 89, 0 },   { 0, 0, 20, 0 },     { 66, 10, 10, 0 },  { 46, 5, 2, 0 },
    { 66, 10, 10, 0 },   { 0, 20, 50, 0 },    { 60, 0, 10, 0 },    { 65, 45, 0, 0 },   { 20, 25, 90, 0 },
    { 0, 0, 20, 0 },     { 102, 135, 99, 0 }, { 0, 4, 199, 0 },    { 60, 65, 89, 0 },  { 0, 0, 40, 0 },
    { 90, 90, 100, 0 },  { 60, 20, 30, 0 },   { 110, 55, 110, 0 }, { 0, 0, 50, 0 },    { 60, 0, 10, 0 },
    { 65, 45, 0, 0 },    { 20, 25, 90, 0 },   { 0, 0, 25, 0 },     { 10, 0, 0, 0 },    { 46, 5, 2, 0 },
    { 34, 14, 10, 0 },   { 21, 5, 13, 0 },    { 0, 0, 0, 0 },      { 0, 0, 0, 0 },     { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },      { 50, 20, 10, 0 },   { 50, 20, 10, 0 },   { 50, 20, 10, 0 },  { 50, 20, 10, 0 },
    { 30, 15, 250, 0 },  { 30, 15, 250, 0 },  { 30, 15, 250, 0 },  { 30, 15, 250, 0 }, { 177, 82, 93, 0 },
    { 177, 82, 93, 0 },  { 177, 82, 93, 0 },  { 177, 82, 93, 0 },  { 90, 82, 102, 0 }, { 0, 4, 199, 0 },
    { 101, 24, 131, 0 }, { 26, 7, 0, 0 },     { 70, 9, 6, 0 },     { 49, 6, 1, 0 },    { 40, 15, 59, 0 },
    { 0, 2, 28, 0 },     { 60, 0, 10, 0 },    { 75, 45, 0, 0 },    { 20, 20, 89, 0 },  { 0, 7, 17, 0 },
    { 90, 82, 102, 0 },  { 0, 4, 199, 0 },    { 101, 24, 131, 0 }, { 26, 7, 0, 0 },    { 70, 12, 16, 0 },
    { 51, 10, 15, 0 },   { 50, 15, 58, 0 },   { 0, 2, 28, 0 },     { 60, 0, 10, 0 },   { 75, 45, 0, 0 },
    { 20, 20, 89, 0 },   { 0, 7, 17, 0 },     { 90, 82, 102, 0 },  { 0, 4, 199, 0 },   { 101, 24, 131, 0 },
    { 26, 7, 0, 0 },     { 70, 20, 45, 0 },   { 49, 6, 1, 0 },     { 40, 15, 59, 0 },  { 0, 2, 28, 0 },
    { 60, 0, 10, 0 },    { 75, 45, 0, 0 },    { 20, 20, 89, 0 },   { 0, 7, 17, 0 },
};

void Environment_UpdateSkybox(u8 skyboxId, EnvironmentContext* envCtx, SkyboxContext* skyboxCtx) {
    size_t size;
    s32 i;
    u8 skybox1Index = 255;
    u8 skybox2Index = 255;
    u8 color1Index = 255;
    u8 color2Index = 255;
    u8 skyboxBlend = 0;
    f32 colorWeight = 0.0f;

    if (envCtx->skyboxConfig >= SKYBOX_CONFIG_MAX) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_0;
    }

    if ((skyboxId == SKYBOX_NORMAL_SKY) || ((skyboxId == SKYBOX_3) && (D_801F4E74 < 1.0f))) {
        if (envCtx->skyboxDisabled) {
            return;
        }

        for (i = 0; i < ARRAY_COUNT(sTimeBasedSkyboxConfigs[envCtx->skyboxConfig]); i++) {
            if ((gSaveContext.skyboxTime >= sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].startTime) &&
                ((gSaveContext.skyboxTime < sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].endTime) ||
                 (sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].endTime == (CLOCK_TIME(24, 0) - 1)))) {

                skybox1Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].skybox1Index;
                skybox2Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].skybox2Index;
                if (skybox1Index == skybox2Index) {
                    sSkyboxIsChanging = false;
                } else {
                    sSkyboxIsChanging = true;
                }

                if (sSkyboxIsChanging) {
                    skyboxBlend = Environment_LerpWeight(sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].endTime,
                                                         sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].startTime,
                                                         ((void)0, gSaveContext.skyboxTime)) *
                                  255.0f;
                } else {
                    skyboxBlend = Environment_LerpWeight(sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].endTime,
                                                         sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].startTime,
                                                         ((void)0, gSaveContext.skyboxTime)) *
                                  255.0f;

                    if (skyboxBlend < 128) {
                        skyboxBlend = 255;
                    } else {
                        skyboxBlend = 0;
                    }

                    if ((envCtx->changeSkyboxState != CHANGE_SKYBOX_INACTIVE) &&
                        (envCtx->changeSkyboxState < CHANGE_SKYBOX_ACTIVE)) {
                        envCtx->changeSkyboxState++;
                        skyboxBlend = 0;
                    }
                }
                color1Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].color1Index;
                color2Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].color2Index;
                colorWeight = Environment_LerpWeight(sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].endTime,
                                                     sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].startTime,
                                                     gSaveContext.skyboxTime);
                break;
            }
        }

        if (CREG(64) != 0) {
            skybox1Index = skybox2Index = CREG(71);
        } else {
            CREG(71) = skybox1Index;
        }

        if (envCtx->changeSkyboxState >= CHANGE_SKYBOX_ACTIVE) {
            skybox1Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].skybox1Index;
            skybox2Index = sTimeBasedSkyboxConfigs[envCtx->changeSkyboxNextConfig][i].skybox2Index;
            color1Index = sTimeBasedSkyboxConfigs[envCtx->skyboxConfig][i].color1Index;
            color2Index = sTimeBasedSkyboxConfigs[envCtx->changeSkyboxNextConfig][i].color2Index;
            skyboxBlend = ((envCtx->changeDuration - (f32)envCtx->changeSkyboxTimer) / envCtx->changeDuration) * 255.0f;
            colorWeight = (envCtx->changeDuration - (f32)envCtx->changeSkyboxTimer) / envCtx->changeDuration;
            envCtx->changeSkyboxTimer--;
            if (envCtx->changeSkyboxTimer <= 0) {
                envCtx->changeSkyboxState = CHANGE_SKYBOX_INACTIVE;
                envCtx->skyboxConfig = envCtx->changeSkyboxNextConfig;
            }
        }

        if ((envCtx->skybox1Index != skybox1Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TEXTURE1_START;
            // size = sNormalSkyFiles[skybox1Index].file.vromEnd - sNormalSkyFiles[skybox1Index].file.vromStart;
            // osCreateMesgQueue(&envCtx->loadQueue, envCtx->loadMsg, ARRAY_COUNT(envCtx->loadMsg));
            // DmaMgr_SendRequestImpl(&envCtx->dmaRequest, skyboxCtx->staticSegments[0],
            //                        sNormalSkyFiles[skybox1Index].file.vromStart, size, 0, &envCtx->loadQueue,
            //                        OS_MESG_PTR(NULL));

            // 2S2h [Port] Bypass DMA request and assign each skybox texture directly to the static segment
            for (size_t i = 0; i < ARRAY_COUNTU(skyboxCtx->staticSegments[0]); i++) {
                skyboxCtx->staticSegments[0][i] = sSkyboxTextures[skybox1Index][i];
            }

            envCtx->skybox1Index = skybox1Index;
        }

        if ((envCtx->skybox2Index != skybox2Index) && (envCtx->skyboxDmaState == SKYBOX_DMA_INACTIVE)) {
            envCtx->skyboxDmaState = SKYBOX_DMA_TEXTURE2_START;
            // size = sNormalSkyFiles[skybox2Index].file.vromEnd - sNormalSkyFiles[skybox2Index].file.vromStart;
            // osCreateMesgQueue(&envCtx->loadQueue, envCtx->loadMsg, ARRAY_COUNT(envCtx->loadMsg));
            // DmaMgr_SendRequestImpl(&envCtx->dmaRequest, skyboxCtx->staticSegments[1],
            //                        sNormalSkyFiles[skybox2Index].file.vromStart, size, 0, &envCtx->loadQueue,
            //                        OS_MESG_PTR(NULL));

            for (size_t i = 0; i < ARRAY_COUNTU(skyboxCtx->staticSegments[1]); i++) {
                skyboxCtx->staticSegments[1][i] = sSkyboxTextures[skybox2Index][i];
            }

            envCtx->skybox2Index = skybox2Index;
        }

        if ((envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE1_START) ||
            (envCtx->skyboxDmaState == SKYBOX_DMA_TEXTURE2_START)) {
            // if (osRecvMesg(&envCtx->loadQueue, NULL, 0) == 0) {
            if (true) {
                envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
                // 2S2H [Port] Since the skybox static segments point to OTR strings, we need to re-create the skybox
                // display lists to have the new textures loaded
                Skybox_Calculate128(skyboxCtx, 5);
                // #endregion
            }
        }

        envCtx->skyboxBlend = skyboxBlend;
        Skybox_SetColors(skyboxCtx,
                         LERPIMP_ALT(sSkyboxPrimColors[color1Index].r, sSkyboxPrimColors[color2Index].r, colorWeight),
                         LERPIMP_ALT(sSkyboxPrimColors[color1Index].g, sSkyboxPrimColors[color2Index].g, colorWeight),
                         LERPIMP_ALT(sSkyboxPrimColors[color1Index].b, sSkyboxPrimColors[color2Index].b, colorWeight),
                         LERPIMP_ALT(sSkyboxEnvColors[color1Index].r, sSkyboxEnvColors[color2Index].r, colorWeight),
                         LERPIMP_ALT(sSkyboxEnvColors[color1Index].g, sSkyboxEnvColors[color2Index].g, colorWeight),
                         LERPIMP_ALT(sSkyboxEnvColors[color1Index].b, sSkyboxEnvColors[color2Index].b, colorWeight));
    }
}

void Environment_EnableUnderwaterLights(PlayState* play, s32 waterLightsIndex) {
    if (waterLightsIndex == 0x1F) {
        waterLightsIndex = 0;
    }

    if (play->envCtx.lightMode == LIGHT_MODE_TIME) {
        if (!D_801F4F31) {
            D_801F4F31 = true;
            gLightConfigAfterUnderwater = play->envCtx.changeLightNextConfig;
            if (play->envCtx.lightConfig != waterLightsIndex) {
                play->envCtx.lightConfig = waterLightsIndex;
                play->envCtx.changeLightNextConfig = waterLightsIndex;
            }
        } else if ((play->envCtx.lightConfig != waterLightsIndex) && (play->envCtx.changeLightTimer == 0)) {
            play->envCtx.changeLightEnabled = true;
            play->envCtx.changeLightNextConfig = waterLightsIndex;
            play->envCtx.changeDuration = 20;
            play->envCtx.changeLightTimer = play->envCtx.changeDuration;
        }
    } else if (play->envCtx.lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE) {
        if (!D_801F4F31) {
            D_801F4F31 = true;
            gLightConfigAfterUnderwater = play->envCtx.lightSetting;
        }
        play->envCtx.lightBlendEnabled = false; // instantly switch to water lights
        play->envCtx.lightSettingOverride = waterLightsIndex;
    }
}

void Environment_DisableUnderwaterLights(PlayState* play) {
    if (play->envCtx.lightMode == LIGHT_MODE_TIME) {
        if (D_801F4F31) {
            D_801F4F31 = false;
            play->envCtx.lightConfig = gLightConfigAfterUnderwater;
            play->envCtx.changeLightNextConfig = gLightConfigAfterUnderwater;
        }
    } else if (D_801F4F31) {
        D_801F4F31 = false;
        play->envCtx.lightSetting = gLightConfigAfterUnderwater;
        play->envCtx.lightBlendEnabled = false; // instantly switch to previous lights
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        play->envCtx.lightBlend = 1.0f;
    }
}

void Environment_WipeRumbleRequests(void) {
    if ((gSaveContext.gameMode != GAMEMODE_NORMAL) && (gSaveContext.gameMode != GAMEMODE_END_CREDITS)) {
        Rumble_StateWipeRequests();
    }
}

void Environment_UpdateSkyboxRotY(PlayState* play) {
    if (!IS_PAUSED(&play->pauseCtx) && ((play->skyboxId == SKYBOX_NORMAL_SKY) || (play->skyboxId == SKYBOX_3))) {
        play->skyboxCtx.rot.y -= R_TIME_SPEED * 1.0e-4f;
    }
}

void Environment_UpdateNextDayTime(void) {
    if ((gSaveContext.nextDayTime >= 0xFF00) && (gSaveContext.nextDayTime != NEXT_TIME_NONE)) {
        gSaveContext.nextDayTime -= 0x10;

        // nextDayTime is used as both a time of day value and a timer to delay sfx when changing days.
        // When Sun's Song is played, nextDayTime is set to 0x8000 or 0 for day and night respectively.
        // These values will actually get used as a time of day value.
        // After this, nextDayTime is assigned magic values of 0xFFFE or 0xFFFD for day and night respectively.
        // From here, 0x10 is decremented from nextDayTime until it reaches either 0xFF0E or 0xFF0D, effectively
        // delaying the chicken crow or dog howl sfx by 15 frames when loading the new area.

        if (gSaveContext.nextDayTime == (NEXT_TIME_DAY_SET - (15 * 0x10))) {
            Audio_PlaySfx(NA_SE_EV_CHICKEN_CRY_M);
            gSaveContext.nextDayTime = NEXT_TIME_NONE;
        } else if (gSaveContext.nextDayTime == (NEXT_TIME_NIGHT_SET - (15 * 0x10))) {
            Audio_PlaySfx_2(NA_SE_EV_DOG_CRY_EVENING);
            gSaveContext.nextDayTime = NEXT_TIME_NONE;
        }
    }
}

void Environment_UpdateTime(PlayState* play, EnvironmentContext* envCtx, PauseContext* pauseCtx, MessageContext* msgCtx,
                            GameOverContext* gameOverCtx) {
    u16 time;

    if (!sEnvIsTimeStopped && (pauseCtx->state == PAUSE_STATE_OFF) && (gameOverCtx->state == GAMEOVER_INACTIVE)) {
        if ((msgCtx->msgMode == MSGMODE_NONE) || (msgCtx->currentTextId == 0xF7) || (msgCtx->currentTextId == 0x20D2) ||
            (msgCtx->currentTextId == 0x140C) ||
            ((msgCtx->currentTextId >= 0x100) && (msgCtx->currentTextId <= 0x200)) ||
            (gSaveContext.gameMode == GAMEMODE_END_CREDITS)) {
            if (!FrameAdvance_IsEnabled(play) &&
                ((play->transitionMode == TRANS_MODE_OFF) || (gSaveContext.gameMode != GAMEMODE_NORMAL))) {
                if (play->transitionTrigger == TRANS_TRIGGER_OFF) {
                    if ((CutsceneManager_GetCurrentCsId() == CS_ID_NONE) && !Play_InCsMode(play)) {
                        gSaveContext.save.time = CURRENT_TIME + (u16)R_TIME_SPEED;
                        if (R_TIME_SPEED != 0) {
                            gSaveContext.save.time = CURRENT_TIME + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
                        }
                    }
                }
            }
        }
    }

    if ((((void)0, gSaveContext.skyboxTime) >= CLOCK_TIME(6, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0)) ||
        (CURRENT_TIME >= (CLOCK_TIME(6, 0) + 0x10))) {
        gSaveContext.skyboxTime = CURRENT_TIME;
    }

    time = CURRENT_TIME;

    if ((time >= CLOCK_TIME(18, 0)) || (time < CLOCK_TIME(6, 0))) {
        gSaveContext.save.isNight = true;
    } else {
        gSaveContext.save.isNight = false;
    }
}

void func_800F6CEC(PlayState* play, u8 arg1, AdjLightSettings* adjLightSettings, EnvLightSettings* lightSettings) {
    s32 phi_t1;
    s32 temp_v1_2;
    s32 temp_v1 = (arg1 % 4);

    if ((((void)0, gSaveContext.save.day) >= 2) && (arg1 >= 4) && (arg1 < 8)) {
        temp_v1_2 = (((void)0, gSaveContext.save.day) + 1) * 4;
        for (phi_t1 = 0; phi_t1 != 3; phi_t1++) {
            adjLightSettings->ambientColor[phi_t1] =
                lightSettings[temp_v1_2 + temp_v1].ambientColor[phi_t1] - lightSettings[temp_v1].ambientColor[phi_t1];
            adjLightSettings->light1Color[phi_t1] =
                lightSettings[temp_v1_2 + temp_v1].light1Color[phi_t1] - lightSettings[temp_v1].light1Color[phi_t1];
            adjLightSettings->light2Color[phi_t1] =
                lightSettings[temp_v1_2 + temp_v1].light2Color[phi_t1] - lightSettings[temp_v1].light2Color[phi_t1];
            adjLightSettings->fogColor[phi_t1] =
                lightSettings[temp_v1_2 + temp_v1].fogColor[phi_t1] - lightSettings[temp_v1].fogColor[phi_t1];
        }
        adjLightSettings->fogNear =
            lightSettings[temp_v1_2 + temp_v1].blendRateAndFogNear - lightSettings[temp_v1].blendRateAndFogNear;
    }

    if ((arg1 >= 4) && (arg1 < 8) && (gWeatherMode == WEATHER_MODE_RAIN)) {
        adjLightSettings->ambientColor[0] = -50;
        adjLightSettings->ambientColor[1] = -100;
        adjLightSettings->ambientColor[2] = -100;
        adjLightSettings->light1Color[0] = -100;
        adjLightSettings->light1Color[1] = -100;
        adjLightSettings->light1Color[2] = -100;
        adjLightSettings->light2Color[0] = -100;
        adjLightSettings->light2Color[1] = -100;
        adjLightSettings->light2Color[2] = -100;

        temp_v1 = arg1;
        adjLightSettings->fogColor[0] = -lightSettings[temp_v1].fogColor[0] + 30;
        adjLightSettings->fogColor[1] = -lightSettings[temp_v1].fogColor[1] + 30;
        adjLightSettings->fogColor[2] = -lightSettings[temp_v1].fogColor[2] + 45;
    }
}

u8 Environment_LerpColor(f32 to, f32 from, f32 lerp) {
    to = CLAMP(to, 0.0f, 255.0f);
    from = CLAMP(from, 0.0f, 255.0f);

    return ((to - from) * lerp) + from;
}

s32 Environment_IsSceneUpsideDown(PlayState* play) {
    s32 ret = false;

    if ((play->sceneId == SCENE_F41) || (play->sceneId == SCENE_INISIE_R)) {
        ret = true;
    }
    return ret;
}

void Environment_UpdateLights(PlayState* play, EnvironmentContext* envCtx, LightContext* lightCtx) {
    EnvLightSettings* lightSettingsList;
    f32 var_fs3;
    s32 i;
    s32 j;
    f32 temp_fv0;
    s32 adjustment;
    u16 temp_s0_2;
    u16 var_v0;
    AdjLightSettings spA4[4];

    var_fs3 = 0.0f;
    __osMemset(spA4, 0, sizeof(AdjLightSettings) * ARRAY_COUNT(spA4));
    lightSettingsList = play->envCtx.lightSettingsList;

    if ((envCtx->lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE) &&
        (envCtx->lightBlendOverride != LIGHT_BLEND_OVERRIDE_FULL_CONTROL) &&
        (envCtx->lightSetting != envCtx->lightSettingOverride) && (envCtx->lightBlend >= 1.0f) &&
        (envCtx->lightSettingOverride <= LIGHT_SETTING_MAX)) {

        envCtx->lightBlend = 0.0f;
        envCtx->prevLightSetting = envCtx->lightSetting;
        envCtx->lightSetting = envCtx->lightSettingOverride;
    }

    if (envCtx->lightSettingOverride != LIGHT_SETTING_OVERRIDE_FULL_CONTROL) {
        if ((envCtx->lightMode == LIGHT_MODE_TIME) && (envCtx->lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE)) {
            for (i = 0; i < ARRAY_COUNT(sTimeBasedLightConfigs[envCtx->lightConfig]); i++) {
                f32 arg0;
                f32 arg1;
                u16 startTime;
                u16 endTime;
                u8 sp97;
                u8 sp96;
                u8 sp95;
                u8 sp94;

                startTime = sTimeBasedLightConfigs[envCtx->lightConfig][i].startTime;
                endTime = sTimeBasedLightConfigs[envCtx->lightConfig][i].endTime;

                sp97 = sTimeBasedLightConfigs[envCtx->lightConfig][i].lightSetting;
                sp95 = sTimeBasedLightConfigs[envCtx->lightConfig][i].nextLightSetting;
                sp96 = sTimeBasedLightConfigs[envCtx->changeLightNextConfig][i].lightSetting;
                sp94 = sTimeBasedLightConfigs[envCtx->changeLightNextConfig][i].nextLightSetting;

                if ((gSaveContext.skyboxTime >= startTime) &&
                    ((gSaveContext.skyboxTime < endTime) || (endTime == 0xFFFF))) {
                    u8 blend8[2];   // sp90
                    s16 blend16[2]; // sp8C

                    func_800F6CEC(play, sp97, &spA4[0], lightSettingsList);
                    func_800F6CEC(play, sp95, &spA4[1], lightSettingsList);
                    func_800F6CEC(play, sp96, &spA4[2], lightSettingsList);
                    func_800F6CEC(play, sp94, &spA4[3], lightSettingsList);

                    if ((sp94 >= envCtx->numLightSettings) && !D_801BDBA8) {
                        D_801BDBA8 = true;
                    }

                    if ((sp97 >= envCtx->numLightSettings) || (sp95 >= envCtx->numLightSettings) ||
                        (sp96 >= envCtx->numLightSettings) || (sp94 >= envCtx->numLightSettings)) {
                        sp97 = 0;
                        sp95 = 0;
                        sp96 = 0;
                        sp94 = 0;
                    }

                    temp_fv0 = Environment_LerpWeight(endTime, startTime, ((void)0, gSaveContext.skyboxTime));

                    sSandstormColorIndex = sp97 & 3;
                    sNextSandstormColorIndex = sp95 & 3;
                    sSandstormLerpScale = temp_fv0;

                    if (envCtx->changeLightEnabled) {
                        var_fs3 = ((f32)envCtx->changeDuration - envCtx->changeLightTimer) / envCtx->changeDuration;
                        envCtx->changeLightTimer--;

                        if (envCtx->changeLightTimer <= 0) {
                            envCtx->changeLightEnabled = false;
                            envCtx->lightConfig = envCtx->changeLightNextConfig;
                        }
                    }

                    for (j = 0; j < 3; j++) {
                        arg0 = lightSettingsList[(s32)sp95].ambientColor[j] + spA4[1].ambientColor[j];
                        arg1 = lightSettingsList[(s32)sp97].ambientColor[j] + spA4[0].ambientColor[j];
                        blend8[0] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        arg0 = lightSettingsList[(s32)sp94].ambientColor[j] + spA4[3].ambientColor[j];
                        arg1 = lightSettingsList[(s32)sp96].ambientColor[j] + spA4[2].ambientColor[j];
                        blend8[1] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        envCtx->lightSettings.ambientColor[j] = LERPIMP_ALT(blend8[0], blend8[1], var_fs3);
                    }

                    if (Environment_IsSceneUpsideDown(play)) {
                        var_v0 = CURRENT_TIME + CLOCK_TIME(12, 0);
                    } else {
                        var_v0 = CURRENT_TIME;
                    }
                    temp_s0_2 = var_v0 - CLOCK_TIME(12, 0);

                    envCtx->lightSettings.light1Dir[0] = -(Math_SinS(temp_s0_2) * 120.0f);
                    envCtx->lightSettings.light1Dir[1] = Math_CosS(temp_s0_2) * 120.0f;
                    envCtx->lightSettings.light1Dir[2] = Math_CosS(temp_s0_2) * 20.0f;

                    envCtx->lightSettings.light2Dir[0] = -envCtx->lightSettings.light1Dir[0];
                    envCtx->lightSettings.light2Dir[1] = -envCtx->lightSettings.light1Dir[1];
                    envCtx->lightSettings.light2Dir[2] = -envCtx->lightSettings.light1Dir[2];

                    for (j = 0; j < 3; j++) {
                        arg0 = lightSettingsList[(s32)sp95].light1Color[j] + spA4[1].light1Color[j];
                        arg1 = lightSettingsList[(s32)sp97].light1Color[j] + spA4[0].light1Color[j];
                        blend8[0] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        arg0 = lightSettingsList[(s32)sp94].light1Color[j] + spA4[3].light1Color[j];
                        arg1 = lightSettingsList[(s32)sp96].light1Color[j] + spA4[2].light1Color[j];
                        blend8[1] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        envCtx->lightSettings.light1Color[j] = LERPIMP_ALT(blend8[0], blend8[1], var_fs3);

                        arg0 = lightSettingsList[(s32)sp95].light2Color[j] + spA4[1].light2Color[j];
                        arg1 = lightSettingsList[(s32)sp97].light2Color[j] + spA4[0].light2Color[j];
                        blend8[0] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        arg0 = lightSettingsList[(s32)sp94].light2Color[j] + spA4[3].light2Color[j];
                        arg1 = lightSettingsList[(s32)sp96].light2Color[j] + spA4[2].light2Color[j];
                        blend8[1] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        envCtx->lightSettings.light2Color[j] = LERPIMP_ALT(blend8[0], blend8[1], var_fs3);
                    }

                    for (j = 0; j < 3; j++) {
                        arg0 = lightSettingsList[(s32)sp95].fogColor[j] + spA4[1].fogColor[j];
                        arg1 = lightSettingsList[(s32)sp97].fogColor[j] + spA4[0].fogColor[j];
                        blend8[0] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        arg0 = lightSettingsList[(s32)sp94].fogColor[j] + spA4[3].fogColor[j];
                        arg1 = lightSettingsList[(s32)sp96].fogColor[j] + spA4[2].fogColor[j];
                        blend8[1] = Environment_LerpColor(arg0, arg1, temp_fv0);

                        envCtx->lightSettings.fogColor[j] = LERPIMP_ALT(blend8[0], blend8[1], var_fs3);
                    }

                    blend16[0] = S16_LERP(ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)sp97].blendRateAndFogNear),
                                          ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)sp95].blendRateAndFogNear),
                                          temp_fv0);
                    blend16[1] = S16_LERP(ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)sp96].blendRateAndFogNear),
                                          ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)sp94].blendRateAndFogNear),
                                          temp_fv0);
                    envCtx->lightSettings.fogNear = LERPIMP_ALT(blend16[0], blend16[1], var_fs3);

                    blend16[0] =
                        S16_LERP(lightSettingsList[(s32)sp97].zFar, lightSettingsList[(s32)sp95].zFar, temp_fv0);
                    blend16[1] =
                        S16_LERP(lightSettingsList[(s32)sp96].zFar, lightSettingsList[(s32)sp94].zFar, temp_fv0);
                    envCtx->lightSettings.zFar = LERPIMP_ALT(blend16[0], blend16[1], var_fs3);

                    break;
                }
            }
        } else {
            u8 lightSetting;
            u8 var_v0_3;

            if ((envCtx->lightSetting >= envCtx->numLightSettings) && !D_801BDBA8) {
                D_801BDBA8 = true;
            }

            if (!envCtx->lightBlendEnabled) {
                for (i = 0; i < 3; i++) {
                    envCtx->lightSettings.ambientColor[i] = lightSettingsList[envCtx->lightSetting].ambientColor[i];
                    envCtx->lightSettings.light1Dir[i] = lightSettingsList[envCtx->lightSetting].light1Dir[i];
                    envCtx->lightSettings.light1Color[i] = lightSettingsList[envCtx->lightSetting].light1Color[i];
                    envCtx->lightSettings.light2Dir[i] = lightSettingsList[envCtx->lightSetting].light2Dir[i];
                    envCtx->lightSettings.light2Color[i] = lightSettingsList[envCtx->lightSetting].light2Color[i];
                    envCtx->lightSettings.fogColor[i] = lightSettingsList[envCtx->lightSetting].fogColor[i];
                }

                lightSetting = envCtx->lightSetting;
                envCtx->lightSettings.fogNear =
                    ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)lightSetting].blendRateAndFogNear);
                envCtx->lightSettings.zFar = lightSettingsList[(s32)lightSetting].zFar;
                envCtx->lightBlend = 1.0f;
            } else {
                var_v0_3 =
                    ENV_LIGHT_SETTINGS_BLEND_RATE_U8(lightSettingsList[(s32)envCtx->lightSetting].blendRateAndFogNear);

                if (var_v0_3 == 0) {
                    var_v0_3++;
                }

                if (envCtx->lightBlendRateOverride != LIGHT_BLENDRATE_OVERRIDE_NONE) {
                    var_v0_3 = envCtx->lightBlendRateOverride;
                }

                if (envCtx->lightBlendOverride == LIGHT_BLEND_OVERRIDE_NONE) {
                    envCtx->lightBlend += var_v0_3 / 255.0f;
                }

                if (envCtx->lightBlend > 1.0f) {
                    envCtx->lightBlend = 1.0f;
                }

                for (i = 0; i < 3; i++) {
                    envCtx->lightSettings.ambientColor[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].ambientColor[i],
                                    lightSettingsList[envCtx->lightSetting].ambientColor[i], envCtx->lightBlend);
                    envCtx->lightSettings.light1Dir[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].light1Dir[i],
                                    lightSettingsList[envCtx->lightSetting].light1Dir[i], envCtx->lightBlend);
                    envCtx->lightSettings.light1Color[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].light1Color[i],
                                    lightSettingsList[envCtx->lightSetting].light1Color[i], envCtx->lightBlend);
                    envCtx->lightSettings.light2Dir[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].light2Dir[i],
                                    lightSettingsList[envCtx->lightSetting].light2Dir[i], envCtx->lightBlend);
                    envCtx->lightSettings.light2Color[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].light2Color[i],
                                    lightSettingsList[envCtx->lightSetting].light2Color[i], envCtx->lightBlend);
                    envCtx->lightSettings.fogColor[i] =
                        LERPIMP_ALT(lightSettingsList[envCtx->prevLightSetting].fogColor[i],
                                    lightSettingsList[envCtx->lightSetting].fogColor[i], envCtx->lightBlend);
                }

                lightSetting = envCtx->lightSetting;
                envCtx->lightSettings.fogNear = LERPIMP_ALT(
                    ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)envCtx->prevLightSetting].blendRateAndFogNear),
                    ENV_LIGHT_SETTINGS_FOG_NEAR(lightSettingsList[(s32)lightSetting].blendRateAndFogNear),
                    envCtx->lightBlend);

                envCtx->lightSettings.zFar = LERPIMP_ALT(lightSettingsList[(s32)envCtx->prevLightSetting].zFar,
                                                         lightSettingsList[(s32)lightSetting].zFar, envCtx->lightBlend);
            }
        }
    }

    envCtx->lightBlendEnabled = true;

    for (i = 0; i < 3; i++) {
        if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjLightSettings.ambientColor[i]) > 255) {
            lightCtx->ambientColor[i] = 255;
        } else if ((s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjLightSettings.ambientColor[i]) < 0) {
            lightCtx->ambientColor[i] = 0;
        } else {
            lightCtx->ambientColor[i] =
                (s16)(envCtx->lightSettings.ambientColor[i] + envCtx->adjLightSettings.ambientColor[i]);
        }

        if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLightSettings.light1Color[i]) > 255) {
            envCtx->dirLight1.params.dir.color[i] = 255;
        } else if ((s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLightSettings.light1Color[i]) < 0) {
            envCtx->dirLight1.params.dir.color[i] = 0;
        } else {
            envCtx->dirLight1.params.dir.color[i] =
                (s16)(envCtx->lightSettings.light1Color[i] + envCtx->adjLightSettings.light1Color[i]);
        }

        if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLightSettings.light2Color[i]) > 255) {
            envCtx->dirLight2.params.dir.color[i] = 255;
        } else if ((s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLightSettings.light2Color[i]) < 0) {
            envCtx->dirLight2.params.dir.color[i] = 0;
        } else {
            envCtx->dirLight2.params.dir.color[i] =
                (s16)(envCtx->lightSettings.light2Color[i] + envCtx->adjLightSettings.light2Color[i]);
        }

        if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjLightSettings.fogColor[i]) > 255) {
            lightCtx->fogColor[i] = 255;
        } else if ((s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjLightSettings.fogColor[i]) < 0) {
            lightCtx->fogColor[i] = 0;
        } else {
            lightCtx->fogColor[i] = (s16)(envCtx->lightSettings.fogColor[i] + envCtx->adjLightSettings.fogColor[i]);
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
    adjustment = envCtx->lightSettings.fogNear + envCtx->adjLightSettings.fogNear;

    if (adjustment <= ENV_FOGNEAR_MAX) {
        lightCtx->fogNear = adjustment;
    } else {
        lightCtx->fogNear = ENV_FOGNEAR_MAX;
    }

    adjustment = envCtx->lightSettings.zFar + envCtx->adjLightSettings.zFar;

    if (adjustment <= ENV_ZFAR_MAX) {
        lightCtx->zFar = adjustment;
    } else {
        lightCtx->zFar = ENV_ZFAR_MAX;
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

void Environment_UpdateSun(PlayState* play) {
    f32 temp_f0;
    s16 temp_a0;
    u16 phi_v0;

    if (!play->envCtx.sunDisabled) {
        if (play->envCtx.precipitation[PRECIP_RAIN_CUR] != 0) {
            Math_SmoothStepToF(&sSunPrimAlpha, 0.0f, 0.5f, 4.0f, 0.01f);
        } else {
            Math_SmoothStepToF(&sSunPrimAlpha, 255.0f, 0.5f, 4.0f, 0.01f);
        }

        if (Environment_IsSceneUpsideDown(play)) {
            phi_v0 = CURRENT_TIME + CLOCK_TIME(12, 0);
        } else {
            phi_v0 = CURRENT_TIME;
        }

        temp_a0 = phi_v0 - CLOCK_TIME(12, 0);

        if (play->csCtx.state != CS_STATE_IDLE) {
            Math_SmoothStepToF(&play->envCtx.sunPos.x, -(Math_SinS(temp_a0) * 120.0f) * 25.0f, 1.0f, 0.8f, 0.8f);
            Math_SmoothStepToF(&play->envCtx.sunPos.y, Math_CosS(temp_a0) * 120.0f * 25.0f, 1.0f, 0.8f, 0.8f);
            //! @bug This should be z.
            Math_SmoothStepToF(&play->envCtx.sunPos.y, Math_CosS(temp_a0) * 20.0f * 25.0f, 1.0f, 0.8f, 0.8f);
        } else {
            play->envCtx.sunPos.x = -(Math_SinS(temp_a0) * 120.0f) * 25.0f;
            play->envCtx.sunPos.y = Math_CosS(temp_a0) * 120.0f * 25.0f;
            play->envCtx.sunPos.z = Math_CosS(temp_a0) * 20.0f * 25.0f;
        }

        temp_f0 = play->envCtx.sunPos.y / 25.0f;
        if (Environment_IsSceneUpsideDown(play)) {
            temp_f0 = -temp_f0;
        }

        sSunEnvAlpha = temp_f0 / 80.0f * 255.0f;
        if (sSunEnvAlpha < 0.0f) {
            sSunEnvAlpha = 0.0f;
        }
        if (sSunEnvAlpha > 255.0f) {
            sSunEnvAlpha = 255.0f;
        }
        sSunEnvAlpha = 255.0f - sSunEnvAlpha;

        sSunColor = temp_f0 / 80.0f;
        if (sSunColor < 0.0f) {
            sSunColor = 0.0f;
        }
        if (sSunColor > 1.0f) {
            sSunColor = 1.0f;
        }

        sSunScale = (2.0f * sSunColor) + 12.0f;
    }
}

void func_800F88C4(u16 weekEventFlag) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_28_08) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_28_10) &&
        CHECK_WEEKEVENTREG(weekEventFlag)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_91_04)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_28_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_90_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_51_02);
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_91_08)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_28_10);
            SET_WEEKEVENTREG(WEEKEVENTREG_90_08);
            SET_WEEKEVENTREG(WEEKEVENTREG_51_02);
        }
    }
}

void func_800F8970(void) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_27_40) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(9, 31))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_27_40);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_27_80) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(10, 3))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_27_80);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_28_01) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(10, 35))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_28_01);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_28_02) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(10, 53))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_28_02);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_28_04) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(11, 25))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_28_04);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN);
    }
}

void Environment_UpdatePostmanEvents(PlayState* play) {
    u16 temp_a2_2;
    u8 eventInfBits;
    u8 day = EVENTINF_GET_7_E0;

    if (day != (u8)((void)0, gSaveContext.save.day)) {
        day = (u8)((void)0, gSaveContext.save.day);

        EVENTINF_SET_7_E0(day, eventInfBits);

        SET_WEEKEVENTREG(WEEKEVENTREG_27_40);
        SET_WEEKEVENTREG(WEEKEVENTREG_27_80);
        SET_WEEKEVENTREG(WEEKEVENTREG_28_01);
        SET_WEEKEVENTREG(WEEKEVENTREG_28_02);
        SET_WEEKEVENTREG(WEEKEVENTREG_28_04);

        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN);
        func_800F88C4(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN);

        CLEAR_WEEKEVENTREG(WEEKEVENTREG_27_40);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_27_80);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_28_01);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_28_02);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_28_04);
    }

    if ((SCRIPT_TIME_NOW < SCRIPT_TIME(9, 0)) || (SCRIPT_TIME_NOW > SCRIPT_TIME(12, 0))) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_08);
    }

    func_800F8970();

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROOM_KEY) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_55_02)) {
        if (((void)0, gSaveContext.save.day) >= 2) {
            SET_WEEKEVENTREG(WEEKEVENTREG_55_02);
        } else if ((((void)0, gSaveContext.save.day) == 1) && (SCRIPT_TIME_NOW >= SCRIPT_TIME(16, 30))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_55_02);
        }
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_90_01)) {
        temp_a2_2 = CURRENT_TIME - D_801F4E78;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_89_40) && ((u16)SCRIPT_TIME_NOW >= (u16)SCRIPT_TIME(5, 0))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_90_01);
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_89_08) && (temp_a2_2 >= CLOCK_TIME(0, 23))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_89_40);
            D_801F4E78 = 0;
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_85_80) && (temp_a2_2 >= (CLOCK_TIME(1, 39) - 1))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_89_08);
            D_801F4E78 = CURRENT_TIME;
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_86_01) && (temp_a2_2 >= (CLOCK_TIME(1, 9) - 1))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_85_80);
        }
    }
}

void Environment_Update(PlayState* play, EnvironmentContext* envCtx, LightContext* lightCtx, PauseContext* pauseCtx,
                        MessageContext* msgCtx, GameOverContext* gameOverCtx, GraphicsContext* gfxCtx) {
    D_801BDBC0 = 0;
    D_801BDBC4 = 0;

    Environment_WipeRumbleRequests();

    if (pauseCtx->state == PAUSE_STATE_OFF) {
        Environment_UpdateSkyboxRotY(play);
        Environment_UpdateRain(play);
        Environment_UpdateTimeBasedSequence(play);
        Environment_UpdateNextDayTime();
        Environment_UpdateTime(play, envCtx, pauseCtx, msgCtx, gameOverCtx);
        Environment_UpdateSun(play);
        Environment_UpdateLights(play, envCtx, lightCtx);
        Environment_UpdatePostmanEvents(play);
    }
}

void Environment_DrawSun(PlayState* play) {
    if (!play->envCtx.sunDisabled) {
        OPEN_DISPS(play->state.gfxCtx);

        if ((play->envCtx.sunPos.y > -800.0f) || Environment_IsSceneUpsideDown(play)) {
            Matrix_Translate(play->view.eye.x + play->envCtx.sunPos.x, play->view.eye.y + play->envCtx.sunPos.y,
                             play->view.eye.z + play->envCtx.sunPos.z, MTXMODE_NEW);
            if (CURRENT_TIME < CLOCK_TIME(12, 0)) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, (u8)((u8)(sSunColor * 52.0f) + 203),
                                (u8)((u8)(sSunColor * 19.0f) + 181), (u8)sSunPrimAlpha);
                gDPSetEnvColor(POLY_OPA_DISP++, (u8)(-(u8)(sSunColor * 5.0f) + 175),
                               (u8)((u8)(sSunColor * 135.0f) + 120), (u8)((u8)(sSunColor * 100.0f) + 100),
                               sSunEnvAlpha);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, (u8)((u8)(sSunColor * 75.0f) + 180),
                                (u8)((u8)(sSunColor * 100.0f) + 100), (u8)sSunPrimAlpha);
                gDPSetEnvColor(POLY_OPA_DISP++, 180, (u8)(sSunColor * 255.0f), (u8)(sSunColor * 200.0f), sSunEnvAlpha);
            }
            Matrix_Scale(sSunScale, sSunScale, sSunScale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            Gfx_SetupDL54_Opa(play->state.gfxCtx);

            // #region 2S2H [Port] The sun texture was originally broken up into 3 pieces, but this causes
            // issues with extraction, so we've combined them into a single 64x64 texture, and this requires
            // that we render it ourself instead of using gSunDL.
            // gSPDisplayList(POLY_OPA_DISP++, gSunDL);
            static Vtx vertices[] = {
                VTX(-31, -31, 0, 0, 0, 255, 255, 255, 255),
                VTX(32, -31, 0, 2016, 0, 255, 255, 255, 255),
                VTX(-31, 32, 0, 0, 2016, 255, 255, 255, 255),
                VTX(32, 32, 0, 2016, 2016, 255, 255, 255, 255),
            };

            gSPMatrix(POLY_OPA_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gSun1Tex, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                   G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            gDPLoadMultiBlock_4b(POLY_OPA_DISP++, gSunEvening1Tex, 0x0100, 1, G_IM_FMT_I, 64, 64, 0,
                                 G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            gSPVertex(POLY_OPA_DISP++, vertices, 4, 0);
            gSP2Triangles(POLY_OPA_DISP++, 0, 1, 2, 0, 2, 1, 3, 0);
            // #endregion
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Environment_DrawSunLensFlare(PlayState* play, EnvironmentContext* envCtx, View* view, GraphicsContext* gfxCtx,
                                  Vec3f vec) {
    if ((play->envCtx.precipitation[PRECIP_RAIN_CUR] == 0) &&
        !(GET_ACTIVE_CAM(play)->stateFlags & CAM_STATE_UNDERWATER) && (play->skyboxId == SKYBOX_NORMAL_SKY)) {
        f32 v0 = Math_CosS(CURRENT_TIME - CLOCK_TIME(12, 0));

        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, vec, 370.0f, v0 * 120.0f, 0x190,
                                  true);
    }
}

typedef enum {
    /* 0 */ LENS_FLARE_CIRCLE0,
    /* 1 */ LENS_FLARE_CIRCLE1,
    /* 2 */ LENS_FLARE_RING
} LensFlareType;

static Color_RGB8 sLensFlareColors[] = {
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
static f32 sLensFlareScales[] = {
    23.0f, 12.0f, 7.0f, 5.0f, 3.0f, 10.0f, 6.0f, 2.0f, 3.0f, 1.0f,
};
static u32 sLensFlareAlphas[] = {
    50, 10, 25, 40, 70, 30, 50, 70, 50, 40,
};
static u32 sLensFlareTypes[] = {
    LENS_FLARE_RING,    LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
    LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1, LENS_FLARE_CIRCLE1,
};

void Environment_DrawLensFlare(PlayState* play, EnvironmentContext* envCtx, View* view, GraphicsContext* gfxCtx,
                               Vec3f pos, f32 scale, f32 colorIntensity, s16 glareStrength, u8 isSun) {
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
    s32 pad;
    f32 lensFlareAlphaScaleTarget;
    f32 alpha;
    f32 adjScale;
    f32 fogInfluence;
    Vec3f screenPos;
    u8 isOffScreen = false;
    f32 weight = 0.0f;
    f32 glareAlphaScale;
    Mtx* mtx;

    OPEN_DISPS(gfxCtx);

    dist = Math3D_Vec3f_DistXYZ(&pos, &view->eye) / 12.0f;

    // compute a unit vector in the look direction
    tempX = view->at.x - view->eye.x;
    tempY = view->at.y - view->eye.y;
    tempZ = view->at.z - view->eye.z;

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

    lensFlareAlphaScaleTarget = cosAngle * 3.5f;
    lensFlareAlphaScaleTarget = CLAMP_MAX(lensFlareAlphaScaleTarget, 1.0f);

    if (!isSun) {
        lensFlareAlphaScaleTarget = cosAngle;
    }

    if (cosAngle < 0.0f) {
        // don't draw lens flare
    } else {
        if (isSun) {
            Play_GetScreenPos(play, &pos, &screenPos);
            sSunDepthTestX = screenPos.x;
            sSunDepthTestY = screenPos.y - 5.0f;
            if (sSunDepthTestY < 0) {
                sSunDepthTestY = 0;
            }
            if ((sSunScreenDepth != GPACK_ZDZ(G_MAXFBZ, 0)) || (screenPos.x < 0.0f) || (screenPos.y < 0.0f) ||
                (screenPos.x > SCREEN_WIDTH) || (screenPos.y > SCREEN_HEIGHT)) {
                isOffScreen = true;
            }
        }

        for (i = 0; i < ARRAY_COUNT(sLensFlareTypes); i++) {
            Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);

            if (isSun) {
                weight = Environment_LerpWeight(60, 15, play->view.fovy);
            }

            Matrix_Translate(-posDirX * i * dist, -posDirY * i * dist, -posDirZ * i * dist, MTXMODE_APPLY);
            adjScale = sLensFlareScales[i] * cosAngle;

            if (isSun) {
                adjScale *= 0.001f * (scale + 630.0f * weight);
            } else {
                adjScale *= 0.0001f * scale * (2.0f * dist);
            }

            Matrix_Scale(adjScale, adjScale, adjScale, MTXMODE_APPLY);

            alpha = colorIntensity / 10.0f;
            alpha = CLAMP_MAX(alpha, 1.0f);
            alpha = alpha * sLensFlareAlphas[i];
            alpha = CLAMP_MIN(alpha, 0.0f);

            if (isSun) {
                fogInfluence = (ENV_FOGNEAR_MAX - play->lightCtx.fogNear) / 50.0f;

                fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

                alpha *= 1.0f - fogInfluence;
            }

            if (isOffScreen == false) {
                Math_SmoothStepToF(&envCtx->lensFlareAlphaScale, lensFlareAlphaScaleTarget, 0.5f, 0.05f, 0.001f);
            } else {
                Math_SmoothStepToF(&envCtx->lensFlareAlphaScale, 0.0f, 0.5f, 0.05f, 0.001f);
            }

            POLY_XLU_DISP = Gfx_SetupDL65_NoCD(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, sLensFlareColors[i].r, sLensFlareColors[i].g, sLensFlareColors[i].b,
                            alpha * envCtx->lensFlareAlphaScale);
            mtx = Matrix_Finalize(gfxCtx);
            if (mtx != NULL) {
                gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                  TEXEL0, 0, PRIMITIVE, 0);
                gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
                gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
                gSPMatrix(POLY_XLU_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);

                switch (sLensFlareTypes[i]) {
                    case LENS_FLARE_CIRCLE0:
                    case LENS_FLARE_CIRCLE1:
                        gSPDisplayList(POLY_XLU_DISP++, gLensFlareCircleDL);
                        break;

                    case LENS_FLARE_RING:
                        gSPDisplayList(POLY_XLU_DISP++, gLensFlareRingDL);
                        break;

                    default:
                        break;
                }
            }
        }

        glareAlphaScale = cosAngle - (1.5f - cosAngle);

        if (glareStrength != 0) {
            if (glareAlphaScale > 0.0f) {
                POLY_XLU_DISP = Gfx_SetupDL57(POLY_XLU_DISP);
                alpha = colorIntensity / 10.0f;
                alpha = CLAMP_MAX(alpha, 1.0f);
                alpha = alpha * glareStrength;
                alpha = CLAMP_MIN(alpha, 0.0f);

                fogInfluence = (ENV_FOGNEAR_MAX - play->lightCtx.fogNear) / 50.0f;

                fogInfluence = CLAMP_MAX(fogInfluence, 1.0f);

                alpha *= 1.0f - fogInfluence;

                gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
                gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

                //! FAKE:
                if (i) {}

                if (isOffScreen == false) {
                    Math_SmoothStepToF(&envCtx->glareAlpha, alpha * glareAlphaScale, 0.5f, 50.0f, 0.1f);
                } else {
                    Math_SmoothStepToF(&envCtx->glareAlpha, 0.0f, 0.5f, 50.0f, 0.1f);
                }

                weight = colorIntensity / 120.0f;
                weight = CLAMP_MIN(weight, 0.0f);

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, (u8)(weight * 75.0f) + 180, (u8)(weight * 155.0f) + 100,
                                (u8)envCtx->glareAlpha);
                gSPDisplayList(POLY_XLU_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));
            } else {
                envCtx->glareAlpha = 0.0f;
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}

f32 Environment_RandCentered(void) {
    return Rand_ZeroOne() - 0.5f;
}

void Environment_DrawRainImpl(PlayState* play, View* view, GraphicsContext* gfxCtx) {
    Player* player = GET_PLAYER(play);
    s32 i;
    u8 precip;
    Vec3f temp;
    f32 length;
    f32 precipScale;
    f32 windScale;
    f32 spF0;
    f32 spEC;
    f32 spE8;
    f32 spE4;
    f32 spE0;
    Vec3f spD4;
    u8 materialApplied;
    f32 temp_fs1;
    f32 temp_fs2;
    s16 pitch;
    s16 yaw;
    f32 scale;

    if (play->envCtx.precipitation[PRECIP_SOS_MAX] != 0) {
        precip = play->envCtx.precipitation[PRECIP_RAIN_CUR];
    } else {
        precipScale = func_80173B48(&play->state) / 3e7f;
        precipScale = CLAMP(precipScale, 0.0f, 1.0f);
        precip = play->envCtx.precipitation[PRECIP_RAIN_CUR] * precipScale;
        if (precip < 5) {
            precip = 5;
        }
    }

    OPEN_DISPS(gfxCtx);

    temp.x = view->at.x - view->eye.x;
    temp.y = view->at.y - view->eye.y;
    temp.z = view->at.z - view->eye.z;
    length = sqrtf(SQ(temp.x) + SQ(temp.y) + SQ(temp.z));

    temp_fs1 = temp.x / length;
    temp_fs2 = temp.z / length;

    spF0 = view->eye.x + (temp_fs1 * 50.0f);
    spEC = view->eye.y + ((temp.y / length) * 120.0f);
    spE8 = view->eye.z + (temp_fs2 * 50.0f);

    spE4 = view->eye.x + (temp_fs1 * 220.0f);
    spE0 = view->eye.z + (temp_fs2 * 220.0f);

    if ((u32)precip != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 25);
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
    }

    windScale = play->envCtx.windSpeed / 60.0f;
    windScale = CLAMP(windScale, 0.0f, 1.0f);

    spD4.x = play->envCtx.windDirection.x * windScale;
    spD4.y = play->envCtx.windDirection.y + 100.0f;
    spD4.z = play->envCtx.windDirection.z * windScale;

    pitch = 0x4000 - Math_Vec3f_Pitch(&gZeroVec3f, &spD4);
    yaw = Math_Vec3f_Yaw(&gZeroVec3f, &spD4) + 0x8000;

    for (i = 0; i < precip; i++) {
        Matrix_Translate(((Rand_ZeroOne() - 0.7f) * 100.0f) + spF0, ((Rand_ZeroOne() - 0.7f) * 100.0f) + spEC,
                         ((Rand_ZeroOne() - 0.7f) * 100.0f) + spE8, MTXMODE_NEW);
        gSPMatrix(POLY_XLU_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        Matrix_RotateYS(yaw + (s16)(i << 5), MTXMODE_APPLY);
        Matrix_RotateXS(pitch + (s16)(i << 5), MTXMODE_APPLY);
        Matrix_Scale(0.3f, 1.0f, 0.3f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gFallingRainDropDL);
    }

    if (player->actor.floorHeight < view->eye.y) {
        materialApplied = false;
        for (i = 0; i < precip; i++) {
            if (!materialApplied) {
                Gfx_SetupDL25_Xlu(gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 100);
                materialApplied++;
            }
            Matrix_Translate((Environment_RandCentered() * 220.0f) + spE4, player->actor.floorHeight + 2.0f,
                             (Environment_RandCentered() * 220.0f) + spE0, MTXMODE_NEW);
            scale = (Rand_ZeroOne() * 0.05f) + 0.05f;
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffShockwaveDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void Environment_DrawRain(PlayState* play, View* view, GraphicsContext* gfxCtx) {
    if (!(GET_ACTIVE_CAM(play)->stateFlags & CAM_STATE_UNDERWATER) &&
        (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0)) {
        if (play->envCtx.precipitation[PRECIP_SOS_MAX] != 0) {
            if (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0) {
                Environment_DrawRainImpl(play, view, gfxCtx);
            }
        } else if (!(GET_ACTIVE_CAM(play)->stateFlags & CAM_STATE_UNDERWATER)) {
            if ((Environment_GetStormState(play) != STORM_STATE_OFF) &&
                (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0)) {
                Environment_DrawRainImpl(play, view, gfxCtx);
            }
        }
    }
}

void Environment_ChangeLightSetting(PlayState* play, u8 lightSetting) {
    if (play->envCtx.lightMode == LIGHT_MODE_TIME) {
        if ((lightSetting == LIGHT_SETTING_MAX) || D_801F4F33) {
            if (D_801F4F30 != 0xFF) {
                play->envCtx.changeLightEnabled = true;
                play->envCtx.changeLightNextConfig = D_801F4F30;
                play->envCtx.changeDuration = 20;
                play->envCtx.changeLightTimer = play->envCtx.changeDuration;
                D_801F4F30 = 0xFF;
            }
        } else if (play->envCtx.changeLightNextConfig != lightSetting) {
            D_801F4F30 = play->envCtx.lightConfig;
            play->envCtx.changeLightEnabled = true;
            play->envCtx.changeLightNextConfig = lightSetting;
            play->envCtx.changeDuration = 20;
            play->envCtx.changeLightTimer = play->envCtx.changeDuration;
        }
    } else if ((play->envCtx.lightSetting != lightSetting) && (play->envCtx.lightBlend >= 1.0f) &&
               (play->envCtx.lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE)) {
        if (lightSetting >= LIGHT_SETTING_MAX) {
            lightSetting = 0;
        }

        play->envCtx.lightBlend = 0.0f;
        play->envCtx.prevLightSetting = play->envCtx.lightSetting;
        play->envCtx.lightSetting = lightSetting;
    }
}

void Environment_DrawSkyboxFilters(PlayState* play) {
    if ((((play->skyboxId != SKYBOX_NONE) && (play->lightCtx.fogNear < 980)) || (play->skyboxId >= SKYBOX_2)) &&
        ((play->skyboxId != SKYBOX_3) || (D_801F4E74 != 0.0f))) {
        f32 alpha;

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL57_Opa(play->state.gfxCtx);

        alpha = (1000 - play->lightCtx.fogNear) * 0.02f;

        if (play->skyboxId == SKYBOX_2) {
            alpha = 1.0f;
        }

        if (alpha > 1.0f) {
            alpha = 1.0f;
        }

        if (play->skyboxId != SKYBOX_3) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->lightCtx.fogColor[0], play->lightCtx.fogColor[1],
                            play->lightCtx.fogColor[2], 255.0f * alpha);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->lightCtx.fogColor[0] + 16, play->lightCtx.fogColor[1] + 16,
                            play->lightCtx.fogColor[2] + 16, 255.0f * D_801F4E74);
        }
        gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if (play->envCtx.customSkyboxFilter) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL57_Opa(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, play->envCtx.skyboxFilterColor[0], play->envCtx.skyboxFilterColor[1],
                        play->envCtx.skyboxFilterColor[2], play->envCtx.skyboxFilterColor[3]);
        gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Environment_DrawLightningFlash(PlayState* play, u8 red, u8 green, u8 blue, u8 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL57_Opa(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
    gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));

    CLOSE_DISPS(play->state.gfxCtx);
}

void Environment_UpdateLightningStrike(PlayState* play) {
    if (play->envCtx.lightningState != LIGHTNING_OFF) {
        switch (gLightningStrike.state) {
            case LIGHTNING_STRIKE_WAIT:
                // every frame theres a 10% chance of the timer advancing 10 units
                if (Rand_ZeroOne() < 0.1f) {
                    gLightningStrike.delayTimer += 10.0f;
                }

                gLightningStrike.delayTimer += Rand_ZeroOne();

                if (gLightningStrike.delayTimer > 500.0f) {
                    gLightningStrike.flashRed = 200;
                    gLightningStrike.flashGreen = 200;
                    gLightningStrike.flashBlue = 255;
                    gLightningStrike.flashAlphaTarget = 200;

                    gLightningStrike.delayTimer = 0.0f;
                    Environment_AddLightningBolts(
                        play, (u8)((Rand_ZeroOne() * (ARRAY_COUNT(sLightningBolts) - 0.1f)) + 1.0f));
                    sLightningFlashAlpha = 0;
                    gLightningStrike.state++;
                }
                break;

            case LIGHTNING_STRIKE_START:
                gLightningStrike.flashRed = 200;
                gLightningStrike.flashGreen = 200;
                gLightningStrike.flashBlue = 255;

                play->envCtx.adjLightSettings.ambientColor[0] += 80;
                play->envCtx.adjLightSettings.ambientColor[1] += 80;
                play->envCtx.adjLightSettings.ambientColor[2] += 100;

                sLightningFlashAlpha += 100;

                if (sLightningFlashAlpha >= gLightningStrike.flashAlphaTarget) {
                    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_0, 0);
                    gLightningStrike.state++;
                    gLightningStrike.flashAlphaTarget = 0;
                }
                break;

            case LIGHTNING_STRIKE_END:
                if (play->envCtx.adjLightSettings.ambientColor[0] > 0) {
                    play->envCtx.adjLightSettings.ambientColor[0] -= 10;
                    play->envCtx.adjLightSettings.ambientColor[1] -= 10;
                }

                if (play->envCtx.adjLightSettings.ambientColor[2] > 0) {
                    play->envCtx.adjLightSettings.ambientColor[2] -= 10;
                }

                sLightningFlashAlpha -= 10;

                if (sLightningFlashAlpha <= gLightningStrike.flashAlphaTarget) {
                    play->envCtx.adjLightSettings.ambientColor[0] = 0;
                    play->envCtx.adjLightSettings.ambientColor[1] = 0;
                    play->envCtx.adjLightSettings.ambientColor[2] = 0;

                    gLightningStrike.state = LIGHTNING_STRIKE_WAIT;

                    if (play->envCtx.lightningState == LIGHTNING_LAST) {
                        play->envCtx.lightningState = LIGHTNING_OFF;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (gLightningStrike.state != LIGHTNING_STRIKE_WAIT) {
        Environment_DrawLightningFlash(play, gLightningStrike.flashRed, gLightningStrike.flashGreen,
                                       gLightningStrike.flashBlue, sLightningFlashAlpha);
    }
}

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

static TexturePtr sLightningTextures[] = {
    gEffLightning1Tex, gEffLightning2Tex, gEffLightning3Tex, gEffLightning4Tex,
    gEffLightning5Tex, gEffLightning6Tex, gEffLightning7Tex, gEffLightning8Tex,
};

/**
 * Draw any active lightning bolt entries contained in `sLightningBolts`
 */
void Environment_DrawLightning(PlayState* play, s32 unused) {
    s16 i;
    f32 dx;
    f32 dz;
    f32 x;
    f32 z;
    s32 pad[2];

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(sLightningBolts); i++) {
        switch (sLightningBolts[i].state) {
            case LIGHTNING_BOLT_START:
                dx = play->view.at.x - play->view.eye.x;
                dz = play->view.at.z - play->view.eye.z;

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

            default:
                break;
        }

        if (sLightningBolts[i].state == LIGHTNING_BOLT_DRAW) {
            Matrix_Translate(sLightningBolts[i].pos.x + sLightningBolts[i].offset.x,
                             sLightningBolts[i].pos.y + sLightningBolts[i].offset.y,
                             sLightningBolts[i].pos.z + sLightningBolts[i].offset.z, MTXMODE_NEW);
            Matrix_RotateXFApply(DEG_TO_RAD(sLightningBolts[i].pitch));
            Matrix_RotateZF(DEG_TO_RAD(sLightningBolts[i].roll), MTXMODE_APPLY);
            Matrix_Scale(22.0f, 100.0f, 22.0f, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 128);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 128);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Lib_SegmentedToVirtual(sLightningTextures[sLightningBolts[i].textureIndex]));
            Gfx_SetupDL61_Xlu(play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffLightningDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Environment_PlaySceneSequence(PlayState* play) {
    u8 dayMinusOne = ((void)0, gSaveContext.save.day) - 1;

    if (dayMinusOne >= 3) {
        dayMinusOne = 0;
    }

    if (play->envCtx.timeSeqState != TIMESEQ_REQUEST) {
        play->envCtx.timeSeqState = TIMESEQ_DISABLED;
        if (((void)0, gSaveContext.forcedSeqId) != NA_BGM_GENERAL_SFX) {
            play->envCtx.timeSeqState = TIMESEQ_REQUEST;
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, ((void)0, gSaveContext.forcedSeqId) + SEQ_FLAG_ASYNC);
            gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
        } else if (!Environment_IsFinalHours(play) || func_800FE5D0(play) ||
                   (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_FINAL_HOURS)) {
            if (play->sceneSequences.seqId == NA_BGM_NO_MUSIC) {
                if (play->sceneSequences.ambienceId == AMBIENCE_ID_13) {
                    return;
                }
                if (play->sceneSequences.ambienceId != ((void)0, gSaveContext.ambienceId)) {
                    Audio_PlayAmbience(play->sceneSequences.ambienceId);
                }
            } else if (play->sceneSequences.ambienceId == AMBIENCE_ID_13) {
                if (play->sceneSequences.seqId != ((void)0, gSaveContext.seqId)) {
                    Audio_PlaySceneSequence(play->sceneSequences.seqId, dayMinusOne);
                }
            } else if ((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME <= CLOCK_TIME(17, 10))) {
                if (gSceneSeqState != SCENESEQ_DEFAULT) {
                    Audio_PlayMorningSceneSequence(play->sceneSequences.seqId, dayMinusOne);
                } else if ((CURRENT_TIME >= CLOCK_TIME(6, 1)) &&
                           (play->sceneSequences.seqId != ((void)0, gSaveContext.seqId))) {
                    Audio_PlaySceneSequence(play->sceneSequences.seqId, dayMinusOne);
                }
                play->envCtx.timeSeqState = TIMESEQ_FADE_DAY_BGM;
            } else {
                if (play->sceneSequences.ambienceId != ((void)0, gSaveContext.ambienceId)) {
                    Audio_PlayAmbience(play->sceneSequences.ambienceId);
                }
                if ((CURRENT_TIME > CLOCK_TIME(17, 10)) && (CURRENT_TIME < CLOCK_TIME(19, 0))) {
                    play->envCtx.timeSeqState = TIMESEQ_EARLY_NIGHT_CRITTERS;
                } else if ((CURRENT_TIME > CLOCK_TIME(19, 0)) || (CURRENT_TIME < CLOCK_TIME(5, 0))) {
                    play->envCtx.timeSeqState = TIMESEQ_NIGHT_CRITTERS;
                } else {
                    play->envCtx.timeSeqState = TIMESEQ_MORNING_CRITTERS;
                }
            }
        }
        Audio_SetEnvReverb(play->roomCtx.curRoom.echo);
        gSceneSeqState = SCENESEQ_DEFAULT;
    }
}

void Environment_UpdateTimeBasedSequence(PlayState* play) {
    s32 pad;

    //! FAKE:
    if (gSaveContext.sceneLayer) {}

    if ((play->csCtx.state == CS_STATE_IDLE) && !(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
        switch (play->envCtx.timeSeqState) {
            case TIMESEQ_DAY_BGM:
                break;

            case TIMESEQ_FADE_DAY_BGM:
                if (CURRENT_TIME > CLOCK_TIME(17, 10)) {
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 240);
                    play->envCtx.timeSeqState++;
                }
                break;

            case TIMESEQ_NIGHT_BEGIN_SFX:
                if (CURRENT_TIME >= CLOCK_TIME(18, 0)) {
                    play->envCtx.timeSeqState++;
                }
                break;

            case TIMESEQ_EARLY_NIGHT_CRITTERS:
                if (play->envCtx.precipitation[PRECIP_RAIN_CUR] < 9) {
                    Audio_PlayAmbience(play->sceneSequences.ambienceId);
                    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_CRITTER_0, 1, 1);
                }
                play->envCtx.timeSeqState++;
                break;

            case TIMESEQ_NIGHT_DELAY:
                if (CURRENT_TIME >= CLOCK_TIME(19, 0)) {
                    play->envCtx.timeSeqState++;
                }
                break;

            case TIMESEQ_NIGHT_CRITTERS:
                Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_CRITTER_0, 1, 0);
                Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_CRITTER_1 << 4 | AMBIENCE_CHANNEL_CRITTER_3, 1, 1);
                play->envCtx.timeSeqState++;
                break;

            case TIMESEQ_DAY_BEGIN_SFX:
                if ((CURRENT_TIME < CLOCK_TIME(19, 0)) && (CURRENT_TIME >= CLOCK_TIME(5, 0))) {
                    play->envCtx.timeSeqState++;
                }
                break;

            case TIMESEQ_MORNING_CRITTERS:
                Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_CRITTER_1 << 4 | AMBIENCE_CHANNEL_CRITTER_3, 1, 0);
                Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_CRITTER_4 << 4 | AMBIENCE_CHANNEL_CRITTER_5, 1, 1);
                play->envCtx.timeSeqState++;
                break;

            case TIMESEQ_DAY_DELAY:
                break;

            default:
                break;
        }
    }

    if ((play->envCtx.timeSeqState != TIMESEQ_REQUEST) && (((void)0, gSaveContext.save.day) == 3) &&
        (CURRENT_TIME < CLOCK_TIME(6, 0)) && !func_800FE5D0(play) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
        (play->transitionMode == TRANS_MODE_OFF) && (play->csCtx.state == CS_STATE_IDLE) &&
        ((play->sceneId != SCENE_00KEIKOKU) || (((void)0, gSaveContext.sceneLayer) != 1)) &&
        (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) &&
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_FINAL_HOURS) &&
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_SONG_OF_SOARING)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 0);
        Audio_PlaySceneSequence(NA_BGM_FINAL_HOURS, 3 - 1);
    }
}

void Environment_DrawCustomLensFlare(PlayState* play) {
    Vec3f pos;

    if (gCustomLensFlare1On) {
        pos.x = gCustomLensFlare1Pos.x;
        pos.y = gCustomLensFlare1Pos.y;
        pos.z = gCustomLensFlare1Pos.z;
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, pos, D_801F4E44, D_801F4E48,
                                  D_801F4E4C, false);
    }

    if (gCustomLensFlare2On) {
        pos.x = gCustomLensFlare2Pos.x;
        pos.y = gCustomLensFlare2Pos.y;
        pos.z = gCustomLensFlare2Pos.z;
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, pos, D_801F4E5C, D_801F4E60,
                                  D_801F4E64, false);
    }
}

void Environment_InitGameOverLights(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    sGameOverLightsIntensity = 0;

    Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, player->actor.world.pos.x - 10.0f,
                              player->actor.world.pos.y + 10.0f, player->actor.world.pos.z - 10.0f, 0, 0, 0, 255);
    sNGameOverLightNode = LightContext_InsertLight(play, &play->lightCtx, &sNGameOverLightInfo);

    Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, player->actor.world.pos.x + 10.0f,
                              player->actor.world.pos.y + 10.0f, player->actor.world.pos.z + 10.0f, 0, 0, 0, 255);

    sSGameOverLightNode = LightContext_InsertLight(play, &play->lightCtx, &sSGameOverLightInfo);
}

void Environment_FadeInGameOverLights(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;

    Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, player->actor.world.pos.x - 10.0f,
                              player->actor.world.pos.y + 10.0f, player->actor.world.pos.z - 10.0f,
                              sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
    Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, player->actor.world.pos.x + 10.0f,
                              player->actor.world.pos.y + 10.0f, player->actor.world.pos.z + 10.0f,
                              sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);

    if (sGameOverLightsIntensity < 254) {
        sGameOverLightsIntensity += 2;
    }

    if (Play_CamIsNotFixed(play)) {
        for (i = 0; i < 3; i++) {
            if (play->envCtx.adjLightSettings.ambientColor[i] > -255) {
                play->envCtx.adjLightSettings.ambientColor[i] -= 12;
                play->envCtx.adjLightSettings.light1Color[i] -= 12;
            }
            play->envCtx.adjLightSettings.fogColor[i] = -255;
        }

        if (play->envCtx.lightSettings.zFar + play->envCtx.adjLightSettings.zFar > 900) {
            play->envCtx.adjLightSettings.zFar -= 100;
        }

        if (play->envCtx.lightSettings.fogNear + play->envCtx.adjLightSettings.fogNear > 950) {
            play->envCtx.adjLightSettings.fogNear -= 10;
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
    s32 i;

    if (sGameOverLightsIntensity >= 3) {
        sGameOverLightsIntensity -= 3;
    } else {
        sGameOverLightsIntensity = 0;
    }

    if (sGameOverLightsIntensity == 1) {
        LightContext_RemoveLight(play, &play->lightCtx, sNGameOverLightNode);
        LightContext_RemoveLight(play, &play->lightCtx, sSGameOverLightNode);
    } else if (sGameOverLightsIntensity >= 2) {
        Lights_PointNoGlowSetInfo(&sNGameOverLightInfo, player->actor.world.pos.x - 10.0f,
                                  player->actor.world.pos.y + 10.0f, player->actor.world.pos.z - 10.0f,
                                  sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
        Lights_PointNoGlowSetInfo(&sSGameOverLightInfo, player->actor.world.pos.x + 10.0f,
                                  player->actor.world.pos.y + 10.0f, player->actor.world.pos.z + 10.0f,
                                  sGameOverLightsIntensity, sGameOverLightsIntensity, sGameOverLightsIntensity, 255);
    }

    if (Play_CamIsNotFixed(play)) {
        for (i = 0; i < 3; i++) {
            Math_SmoothStepToS(&play->envCtx.adjLightSettings.ambientColor[i], 0, 5, 12, 1);
            Math_SmoothStepToS(&play->envCtx.adjLightSettings.light1Color[i], 0, 5, 12, 1);
            play->envCtx.adjLightSettings.fogColor[i] = 0;
        }
        play->envCtx.adjLightSettings.zFar = 0;
        play->envCtx.adjLightSettings.fogNear = 0;
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

void Environment_UpdateRain(PlayState* play) {
    u8 max = MAX(play->envCtx.precipitation[PRECIP_RAIN_MAX], play->envCtx.precipitation[PRECIP_SOS_MAX]);

    if ((play->envCtx.precipitation[PRECIP_RAIN_CUR] != max) && ((play->state.frames % 8) == 0)) {
        if (play->envCtx.precipitation[PRECIP_RAIN_CUR] < max) {
            play->envCtx.precipitation[PRECIP_RAIN_CUR] += 2;
        } else {
            play->envCtx.precipitation[PRECIP_RAIN_CUR] -= 2;
        }
    }
}

void Environment_FillScreen(GraphicsContext* gfxCtx, u8 red, u8 green, u8 blue, u8 alpha, u8 drawFlags) {
    if (alpha != 0) {
        OPEN_DISPS(gfxCtx);

        if (drawFlags & FILL_SCREEN_OPA) {
            POLY_OPA_DISP = Gfx_SetupDL57(POLY_OPA_DISP);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, red, green, blue, alpha);
            gDPSetAlphaDither(POLY_OPA_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_OPA_DISP++, G_CD_DISABLE);

            gSPDisplayList(POLY_OPA_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));
        }

        if (drawFlags & FILL_SCREEN_XLU) {
            POLY_XLU_DISP = Gfx_SetupDL57(POLY_XLU_DISP);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, red, green, blue, alpha);

            if ((u32)alpha == 255) {
                gDPSetRenderMode(POLY_XLU_DISP++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
            }

            gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
            gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

            gSPDisplayList(POLY_XLU_DISP++, D_0E000000_TO_SEGMENTED(clearFillRect));
        }

        CLOSE_DISPS(gfxCtx);
    }
}

u8 sSandstormPrimColors[] = {
    255, 255, 255, //
    255, 255, 255, //
    255, 255, 255, //
    255, 255, 255, //
    210, 156, 85,  //
    255, 200, 100, //
    225, 160, 50,  //
    105, 90,  40,  //
};

u8 sSandstormEnvColors[] = {
    100, 100, 100, //
    100, 100, 100, //
    100, 100, 100, //
    100, 100, 100, //
    155, 106, 35,  //
    200, 150, 50,  //
    170, 110, 0,   //
    50,  40,  0,   //
};

void Environment_DrawSandstorm(PlayState* play, u8 sandstormState) {
    s32 primA1 = 0;
    s32 envA1 = 0;
    s32 primA = play->envCtx.sandstormPrimA;
    s32 envA = play->envCtx.sandstormEnvA;
    s32 step = 9;
    u16 sp96;
    u16 sp94;
    u16 sp92;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 index;
    f32 sp98;
    s32 pad;

    switch (sandstormState) {
        case SANDSTORM_ACTIVE:
            primA1 = play->state.frames % 128;
            if (primA1 > 128) {
                primA1 = 255 - primA1;
            }
            primA1 += 73;
            envA1 = 255;
            break;

        case SANDSTORM_FILL:
            primA1 = 255;
            envA1 = (play->envCtx.sandstormPrimA >= 255) ? 255 : 128;
            break;

        case SANDSTORM_UNFILL:
        case SANDSTORM_5:
            envA1 = 128;
            if (play->envCtx.sandstormEnvA > 128) {
                primA1 = 255;
            } else {
                primA1 = play->state.frames % 128;
                if (primA1 > 64) {
                    primA1 = 128 - primA1;
                }
                primA1 += 73;
            }
            if ((primA1 >= primA) && (primA1 != 255)) {
                play->envCtx.sandstormState++;
            }
            break;

        case SANDSTORM_DISSIPATE:
            primA1 = (play->envCtx.sandstormEnvA > 128) ? 255 : play->envCtx.sandstormEnvA >> 1;

            if (primA == 0) {
                play->envCtx.sandstormState = SANDSTORM_OFF;
            }
            break;

        case SANDSTORM_6:
        case SANDSTORM_8:
            primA1 = D_801F4E30;
            envA1 = D_801F4E30;
            step = 1;
            if (sandstormState == SANDSTORM_8) {
                step = 10;
            }
            break;

        case SANDSTORM_7:
        case SANDSTORM_9:
            step = 1;
            if (primA == 0) {
                play->envCtx.sandstormState = SANDSTORM_OFF;
            }
            if (sandstormState == SANDSTORM_9) {
                step = 10;
            }
            break;

        case SANDSTORM_A:
            step = 255;
            primA1 = D_801F4E30;
            envA1 = D_801F4E30;
            if (primA == 0) {
                play->envCtx.sandstormState = SANDSTORM_OFF;
            }
            break;

        case SANDSTORM_B:
            primA1 = play->state.frames & 0x7F;
            if (primA1 > 64) {
                primA1 = 128 - primA1;
            }
            primA1 += 73;
            envA1 = 128;
            break;

        case SANDSTORM_C:
            if (primA == 0) {
                play->envCtx.sandstormState = SANDSTORM_OFF;
            }
            break;

        case SANDSTORM_D:
            primA1 = D_801F4E30;
            envA1 = D_801F4E30;
            step = 10;
            break;

        default:
            break;
    }

    if (ABS_ALT(primA - primA1) < step) {
        primA = primA1;
    } else if (primA1 < primA) {
        primA -= step;
    } else {
        primA += step;
    }

    if (ABS_ALT(envA - envA1) < step) {
        envA = envA1;
    } else if (envA1 < envA) {
        envA -= step;
    } else {
        envA += step;
    }

    play->envCtx.sandstormPrimA = primA;
    play->envCtx.sandstormEnvA = envA;

    sp98 = (512.0f - (primA + envA)) * (3.0f / 128.0f);

    if (sp98 > 6.0f) {
        sp98 = 6.0f;
    }

    if (play->envCtx.sandstormPrimA != 0) {
        index = 0;
        if (sandstormState >= SANDSTORM_B) {
            index = 4 * 3;
        }

        if ((play->envCtx.lightMode != LIGHT_MODE_TIME) ||
            (play->envCtx.lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE)) {
            primColor.r = sSandstormPrimColors[index + 1 * 3 + 0];
            primColor.g = sSandstormPrimColors[index + 1 * 3 + 1];
            primColor.b = sSandstormPrimColors[index + 1 * 3 + 2];
            envColor.r = sSandstormEnvColors[index + 1 * 3 + 0];
            envColor.g = sSandstormEnvColors[index + 1 * 3 + 1];
            envColor.b = sSandstormEnvColors[index + 1 * 3 + 2];
        } else if (sSandstormColorIndex == sNextSandstormColorIndex) {
            primColor.r = sSandstormPrimColors[sSandstormColorIndex * 3 + index + 0];
            primColor.g = sSandstormPrimColors[sSandstormColorIndex * 3 + index + 1];
            primColor.b = sSandstormPrimColors[sSandstormColorIndex * 3 + index + 2];
            envColor.r = sSandstormEnvColors[sSandstormColorIndex * 3 + index + 0];
            envColor.g = sSandstormEnvColors[sSandstormColorIndex * 3 + index + 1];
            envColor.b = sSandstormEnvColors[sSandstormColorIndex * 3 + index + 2];
        } else {
            primColor.r =
                (s32)F32_LERP_ALT(sSandstormPrimColors[sSandstormColorIndex * 3 + index + 0],
                                  sSandstormPrimColors[sNextSandstormColorIndex * 3 + index + 0], sSandstormLerpScale);
            primColor.g =
                (s32)F32_LERP_ALT(sSandstormPrimColors[sSandstormColorIndex * 3 + index + 1],
                                  sSandstormPrimColors[sNextSandstormColorIndex * 3 + index + 1], sSandstormLerpScale);
            primColor.b =
                (s32)F32_LERP_ALT(sSandstormPrimColors[sSandstormColorIndex * 3 + index + 2],
                                  sSandstormPrimColors[sNextSandstormColorIndex * 3 + index + 2], sSandstormLerpScale);
            envColor.r =
                (s32)F32_LERP_ALT(sSandstormEnvColors[sSandstormColorIndex * 3 + index + 0],
                                  sSandstormEnvColors[sNextSandstormColorIndex * 3 + index + 0], sSandstormLerpScale);
            envColor.g =
                (s32)F32_LERP_ALT(sSandstormEnvColors[sSandstormColorIndex * 3 + index + 1],
                                  sSandstormEnvColors[sNextSandstormColorIndex * 3 + index + 1], sSandstormLerpScale);
            envColor.b =
                (s32)F32_LERP_ALT(sSandstormEnvColors[sSandstormColorIndex * 3 + index + 2],
                                  sSandstormEnvColors[sNextSandstormColorIndex * 3 + index + 2], sSandstormLerpScale);
        }

        envColor.r = ((envColor.r * sp98) + ((6.0f - sp98) * primColor.r)) * (1.0f / 6.0f);
        envColor.g = ((envColor.g * sp98) + ((6.0f - sp98) * primColor.g)) * (1.0f / 6.0f);
        envColor.b = ((envColor.b * sp98) + ((6.0f - sp98) * primColor.b)) * (1.0f / 6.0f);

        sp96 = (sSandstormScroll * (11.0f / 6.0f));
        sp94 = (sSandstormScroll * (9.0f / 6.0f));
        sp92 = (sSandstormScroll * (6.0f / 6.0f));

        OPEN_DISPS(play->state.gfxCtx);

        POLY_XLU_DISP = Gfx_SetupDL64(POLY_XLU_DISP);

        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColor.r, primColor.g, primColor.b, play->envCtx.sandstormPrimA);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor.r, envColor.g, envColor.b, play->envCtx.sandstormEnvA);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, (u32)sp96 % 4096, 0, 512, 32, 1,
                                      (u32)sp94 % 4096, 4095 - ((u32)sp92 % 4096), 256, 64, sp98, 0, sp98 * 1.5f,
                                      -sp98));
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        // #region 2S2H [Widescreen] Widescreen Sandstorm
        // gSPDisplayList(POLY_XLU_DISP++, gFieldSandstormDL); // Original Dlist call
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_CLD_SURF2);
        gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
        gDPLoadTextureBlock(POLY_XLU_DISP++, gFieldSandstorm1Tex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 6, 5, 1, G_TX_NOLOD);
        gDPLoadMultiBlock(POLY_XLU_DISP++, gFieldSandstorm2Tex, 0x0100, 1, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 32, 0,
                          G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 6, 5, 2, 1);
        gSPDisplayList(POLY_XLU_DISP++, 0x08000000 | 1);
        gSPWideTextureRectangle(POLY_XLU_DISP++, OTRGetRectDimensionFromLeftEdge(0) << 2, 0,
                                OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) << 2, SCREEN_HEIGHT << 2,
                                G_TX_RENDERTILE, 0, 0, 0x008C, -0x008C);
        // #endregion

        CLOSE_DISPS(play->state.gfxCtx);
    }

    sSandstormScroll += (s32)sp98;
}

s32 Environment_AdjustLights(PlayState* play, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp;
    s32 i;
    Player* player = GET_PLAYER(play);

    if (play->roomCtx.curRoom.type == ROOM_TYPE_BOSS) {
        return 0;
    }

    if (!Play_CamIsNotFixed(play)) {
        return 0;
    }
    if (play->unk_18880) {
        return 0;
    }
    if ((player != NULL) && (player->stateFlags1 & PLAYER_STATE1_2)) {
        return 0;
    }

    arg1 = CLAMP_MIN(arg1, 0.0f);
    arg1 = CLAMP_MAX(arg1, 1.0f);

    temp = arg1 - arg3;

    if (arg1 < arg3) {
        temp = 0.0f;
    }

    play->envCtx.adjLightSettings.fogNear = (arg2 - play->envCtx.lightSettings.fogNear) * temp;

    if (arg1 == 0.0f) {
        for (i = 0; i < 3; i++) {
            play->envCtx.adjLightSettings.fogColor[i] = 0;
        }
    } else {
        temp = arg1 * 5.0f;
        temp = CLAMP_MAX(temp, 1.0f);

        for (i = 0; i < 3; i++) {
            play->envCtx.adjLightSettings.fogColor[i] = -(f32)play->envCtx.lightSettings.fogColor[i] * temp;
        }
    }

    if (arg4 <= 0.0f) {
        return 1;
    }

    arg1 *= arg4;

    for (i = 0; i < 3; i++) {
        play->envCtx.adjLightSettings.ambientColor[i] = -(f32)play->envCtx.lightSettings.ambientColor[i] * arg1;
        play->envCtx.adjLightSettings.light1Color[i] = -(f32)play->envCtx.lightSettings.light1Color[i] * arg1;
    }

    return 1;
}

void Environment_LerpRGB8(u8* from, Color_RGB8* to, f32 lerp, s16* dst) {
    Color_RGB8 result;

    Color_RGB8_Lerp((Color_RGB8*)from, to, lerp, &result);

    dst[0] = result.r - from[0];
    dst[1] = result.g - from[1];
    dst[2] = result.b - from[2];
}

void Environment_LerpAmbientColor(PlayState* play, Color_RGB8* to, f32 lerp) {
    Environment_LerpRGB8(play->envCtx.lightSettings.ambientColor, to, lerp, play->envCtx.adjLightSettings.ambientColor);
}

void Environment_LerpDiffuseColor(PlayState* play, Color_RGB8* to, f32 lerp) {
    Environment_LerpRGB8(play->envCtx.lightSettings.light1Color, to, lerp, play->envCtx.adjLightSettings.light1Color);
    Environment_LerpRGB8(play->envCtx.lightSettings.light2Color, to, lerp, play->envCtx.adjLightSettings.light2Color);
}

void Environment_LerpFogColor(PlayState* play, Color_RGB8* to, f32 lerp) {
    Environment_LerpRGB8(play->envCtx.lightSettings.fogColor, to, lerp, play->envCtx.adjLightSettings.fogColor);
}

void Environment_LerpFog(PlayState* play, s16 fogNearTarget, s16 fogFarTarget, f32 lerp) {
    play->envCtx.adjLightSettings.fogNear = (fogNearTarget - play->envCtx.lightSettings.fogNear) * lerp;
    play->envCtx.adjLightSettings.zFar = (fogFarTarget - play->envCtx.lightSettings.zFar) * lerp;
}

// Repurposed from OoT to be more general
u32 Environment_GetEventDayCount(void) {
    return gSaveContext.save.eventDayCount;
}

// Repurposed from OoT to be more general
void Environment_ClearEventDayCount(void) {
    gSaveContext.save.eventDayCount = 0;
}

// Repurposed from OoT to be more general
u32 Environment_GetTotalDays(void) {
    return gSaveContext.save.day;
}

void Environment_ForcePlaySequence(u16 seqId) {
    gSaveContext.forcedSeqId = seqId;
}

s32 Environment_IsForcedSequenceDisabled(void) {
    s32 isDisabled = false;

    if (gSaveContext.forcedSeqId != NA_BGM_GENERAL_SFX) {
        isDisabled = true;
    }

    return isDisabled;
}

void Environment_PlayStormNatureAmbience(PlayState* play) {
    if (((play->sceneSequences.seqId != NA_BGM_NO_MUSIC) && (play->sceneSequences.ambienceId == AMBIENCE_ID_13)) ||
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_FINAL_HOURS)) {
        Audio_PlayAmbience(AMBIENCE_ID_07);
    } else if ((play->sceneSequences.seqId != NA_BGM_NO_MUSIC) && (play->sceneSequences.ambienceId != AMBIENCE_ID_13)) {
        if ((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME < CLOCK_TIME(18, 0))) {
            Audio_PlayAmbience(play->sceneSequences.ambienceId);
        }
    }

    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 1);
    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 1);
}

void Environment_StopStormNatureAmbience(PlayState* play) {
    if (((play->sceneSequences.seqId != NA_BGM_NO_MUSIC) && (play->sceneSequences.ambienceId == AMBIENCE_ID_13)) ||
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_FINAL_HOURS)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 20);
    } else if ((play->sceneSequences.seqId != NA_BGM_NO_MUSIC) && (play->sceneSequences.ambienceId != AMBIENCE_ID_13)) {
        if ((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME < CLOCK_TIME(18, 0))) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 20);
        }
    }

    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
    Audio_SetAmbienceChannelIO(AMBIENCE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
}

void Environment_WarpSongLeave(PlayState* play) {
    gWeatherMode = WEATHER_MODE_CLEAR;
    gSaveContext.save.cutsceneIndex = 0;
    gSaveContext.respawnFlag = -3;
    play->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_RETURN].entrance;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
}

void Environment_SetupSkyboxStars(PlayState* play) {
    f32 phi_f0;

    if ((play->envCtx.skybox1Index == 0) && (play->skyboxId == SKYBOX_NORMAL_SKY)) {
        if ((CURRENT_TIME >= CLOCK_TIME(21, 0)) || (CURRENT_TIME < CLOCK_TIME(2, 0))) {
            phi_f0 = 1.0f;
        } else if (CURRENT_TIME > CLOCK_TIME(19, 0)) {
            phi_f0 = 1.0f - ((CLOCK_TIME(21, 0) - CURRENT_TIME) * (1.0f / (CLOCK_TIME(2, 0) + 1)));
        } else if (CURRENT_TIME < CLOCK_TIME(3, 0)) {
            phi_f0 = (CLOCK_TIME(3, 0) - CURRENT_TIME) * (1.0f / (CLOCK_TIME(1, 0) + 1));
        } else {
            phi_f0 = 0.0f;
        }

        phi_f0 = (play->envCtx.skyboxConfig == SKYBOX_CONFIG_24) ? 1.0f : phi_f0;

        D_801F4F28 = phi_f0;
        sEnvSkyboxNumStars = gSkyboxNumStars;
    } else {
        D_801F4F28 = 0.0f;
        sEnvSkyboxNumStars = 0;
    }

    if ((sEnvSkyboxNumStars != 0) && (D_801F4F28 != 0.0f)) {
        OPEN_DISPS(play->state.gfxCtx);

        sSkyboxStarsDList = POLY_OPA_DISP;

        gSPNoOp(POLY_OPA_DISP++);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        sSkyboxStarsDList = NULL;
    }
}

void Environment_DrawSkyboxStar(Gfx** gfxP, f32 x, f32 y, s32 width, s32 height) {
    Gfx* gfx = *gfxP;
    // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3
    // left x coordinate turned into signed value to be used with wide texture rectangle
    s32 xl = x * 4.0f;
    u32 yl = y * 4.0f;
    u32 xd = width;
    u32 yd = height;

    gSPWideTextureRectangle(gfx++, xl, yl, xl + xd, yl + yd, 0, 0, 0, 0, 0);
    // #endregion

    *gfxP = gfx;
}

// 2S2H [Enhancement] Renders a star as a quad against the projection matrix by rotating it with the billboard matrix
void Ship_DrawSkyboxStarInterpolated(Gfx** gfxP, PlayState* play, Vec3f pos, u32 width, u32 height, s32 starNum) {
    static MtxF origBillboardMtxF;
    static Vtx starVtxData[] = {
        VTX(-1, -1, 0, 0, 0, 0, 0, 0, 255),
        VTX(1, -1, 0, 0, 0, 0, 0, 0, 255),
        VTX(-1, 1, 0, 0, 0, 0, 0, 0, 255),
        VTX(1, 1, 0, 0, 0, 0, 0, 0, 255),
    };

    Gfx* gfx = *gfxP;

    // One time logic before drawing the first star
    if (starNum == 0) {
        // play->billboardMtxF is overwitten by the skybox stars draw earlier,
        // so we need to derive the MtxF from the original Mtx
        Matrix_MtxToMtxF(play->billboardMtx, &origBillboardMtxF);

        gSPLoadGeometryMode(gfx++, G_CULL_BACK);
    }

    if (width >= 2) {
        // The following math is used to compute new position values for each star such that they are
        // each 5500 units from the view eye. This is necessary as some stars are further way than the zFar
        // of the loaded view projection, which caused those stars to be clipped away.
        // 5500 was choosen as this is roughly the largest distance that the sun is ever from the view eye.
        f32 eyeDist = Math3D_Vec3f_DistXYZ(&pos, &play->view.eye);
        f32 distRatio = 5500.0f / eyeDist;

        f32 x = play->view.eye.x + (pos.x - play->view.eye.x) * distRatio;
        f32 y = play->view.eye.y + (pos.y - play->view.eye.y) * distRatio;
        f32 z = play->view.eye.z + (pos.z - play->view.eye.z) * distRatio;

        // This scale mulitplier is set to draw the stars at a matching size as the texture rectangle
        // and made to cancel out field of view changes so that the stars always have the same
        // apparent size, preventing stars from appearing larger in first person mode or when looking through the
        // telescope.
        f32 scaleMultiplier = 3.0f / (60.0f / play->view.fovy);

        Matrix_Push();

        // Set the matrix so the star renders billboarded and scale to "stretch" it so that it matches
        // roughly how the texture rectangle would have looked
        Matrix_Translate(x, y, z, MTXMODE_NEW);
        Matrix_Scale(width * scaleMultiplier, height * scaleMultiplier, 1.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&origBillboardMtxF);

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);

        gSPVertex(gfx++, starVtxData, ARRAY_COUNT(starVtxData), 0);
        gSP2Triangles(gfx++, 0, 1, 2, 0, 1, 3, 2, 0);

        Matrix_Pop();
    }

    *gfxP = gfx;
}

void Environment_DrawSkyboxStarsImpl(PlayState* play, Gfx** gfxP) {
    static const Vec3s D_801DD880[] = {
        { 0x0384, 0x2328, 0xD508 }, { 0x09C4, 0x2328, 0xDA1C }, { 0x0E74, 0x22D8, 0xDA1C }, { 0x1450, 0x2468, 0xD8F0 },
        { 0x1C84, 0x28A0, 0xCBA8 }, { 0x1F40, 0x2134, 0xD8F0 }, { 0x1F40, 0x28A0, 0xDAE4 }, { 0xE4A8, 0x4A38, 0x4A38 },
        { 0xD058, 0x4C2C, 0x3A98 }, { 0xD8F0, 0x36B0, 0x47E0 }, { 0xD954, 0x3264, 0x3E1C }, { 0xD8F0, 0x3070, 0x37DC },
        { 0xD8F0, 0x1F40, 0x5208 }, { 0xD760, 0x1838, 0x27D8 }, { 0x0000, 0x4E20, 0x4A38 }, { 0x076C, 0x2328, 0xDCD8 },
    };
    // 2S2H [Port] Switched from Color_RGBA8_u32 to Color_RGBA8 to avoid endianenes struct union ordering issues
    static const Color_RGBA8 D_801DD8E0[] = {
        { 65, 164, 255, 255 },  { 131, 164, 230, 255 }, { 98, 205, 255, 255 }, { 82, 82, 255, 255 },
        { 123, 164, 164, 255 }, { 98, 205, 255, 255 },  { 98, 164, 230, 255 }, { 255, 90, 0, 255 },
    };
    // 2S2H [Port] This originally had `UNALIGNED` however we don't need that for PC and it was causing warnings in the
    // header file
    static const Color_RGBA8 D_801DD900[] = {
        { 64, 80, 112, 255 },   { 96, 96, 128, 255 },   { 128, 112, 144, 255 }, { 160, 128, 160, 255 },
        { 192, 144, 168, 255 }, { 224, 160, 176, 255 }, { 224, 160, 176, 255 }, { 104, 104, 136, 255 },
        { 136, 120, 152, 255 }, { 168, 136, 168, 255 }, { 200, 152, 184, 255 }, { 232, 168, 184, 255 },
        { 224, 176, 184, 255 }, { 240, 192, 192, 255 }, { 232, 184, 192, 255 }, { 248, 200, 192, 255 },
    };
    Vec3f pos;
    f32 temp;
    f32 imgY;
    f32 imgX;
    Gfx* gfx;
    s32 phi_v1;
    s32 negateY;
    f32 invScale;
    f32 temp_f20;
    Gfx* gfxTemp;
    f32 scale;
    s32 i;
    u32 randInt;
    u32 imgWidth;
    f32* imgXPtr;
    f32* imgYPtr;
    Vec3f* posPtr;
    s32 pad[2];
    f32(*viewProjectionMtxF)[4];

    gfx = *gfxP;
    negateY = Environment_IsSceneUpsideDown(play);

    Matrix_MtxToMtxF(play->view.viewingPtr, &play->billboardMtxF);
    Matrix_MtxToMtxF(&play->view.projection, &play->viewProjectionMtxF);
    SkinMatrix_MtxFMtxFMult(&play->viewProjectionMtxF, &play->billboardMtxF, &play->viewProjectionMtxF);

    phi_v1 = 0;

    gDPPipeSync(gfx++);
    gDPSetEnvColor(gfx++, 255, 255, 255, 255.0f * D_801F4F28);
    gDPSetCombineLERP(gfx++, PRIMITIVE, 0, ENVIRONMENT, 0, PRIMITIVE, 0, ENVIRONMENT, 0, PRIMITIVE, 0, ENVIRONMENT, 0,
                      PRIMITIVE, 0, ENVIRONMENT, 0);
    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_AA_XLU_LINE | G_RM_AA_XLU_LINE2);

    // 2S2H [Port] We need to set the render mode to XLU_SURF for alpha logic to be used in Fast3D
    // If Fast3D changes how it decides alpha in the future, we may be able to remove this line
    gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    randInt = ((u32)gSaveContext.save.saveInfo.playerData.playerName[0] << 0x18) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[1] << 0x14) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[2] << 0x10) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[3] << 0xC) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[4] << 8) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[5] << 4) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[6] << 0) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[7] >> 4) ^
              ((u32)gSaveContext.save.saveInfo.playerData.playerName[7] << 0x1C);

    //! FAKE:
    if (play->view.viewingPtr && play->view.viewingPtr && play->view.viewingPtr) {}

    for (i = 0; i < sEnvSkyboxNumStars; i++) {
        if (i < 16) {
            pos.x = play->view.eye.x + (s32)D_801DD880[i].x;
            pos.y = play->view.eye.y + (s32)D_801DD880[i].y;
            pos.z = play->view.eye.z + (s32)D_801DD880[i].z;
            imgWidth = 8;
        } else {
            f32 temp_f22;
            f32 temp_f4;
            f32 temp_f2;

            // temp_f4 = Rand_ZeroOne_Variable(&randInt);
            randInt = (randInt * RAND_MULTIPLIER) + RAND_INCREMENT;
            gRandFloat = (randInt >> 9) | 0x3F800000;
            temp = *((f32*)&gRandFloat);
            temp_f4 = temp - 1.0f;

            // temp_f20 = Rand_ZeroOne_Variable(&randInt);
            randInt = (randInt * RAND_MULTIPLIER) + RAND_INCREMENT;
            gRandFloat = (randInt >> 9) | 0x3F800000;
            temp_f20 = ((*((f32*)&gRandFloat) - 1.0f) + temp_f4) * 0.5f;

            // randInt = Rand_Next_Variable(&randInt);
            randInt = (randInt * RAND_MULTIPLIER) + RAND_INCREMENT;

            // Set random position
            pos.y = play->view.eye.y + (SQ(temp_f20) * SQ(128.0f)) - 1000.0f;
            pos.x = play->view.eye.x + (Math_SinS(randInt) * (1.2f - temp_f20) * SQ(128.0f));
            pos.z = play->view.eye.z + (Math_CosS(randInt) * (1.2f - temp_f20) * SQ(128.0f));

            // temp_f2 = Rand_ZeroOne_Variable(&randInt);
            randInt = (randInt * RAND_MULTIPLIER) + RAND_INCREMENT;
            gRandFloat = ((randInt >> 9) | 0x3F800000);
            temp_f2 = *((f32*)&gRandFloat) - 1.0f;

            // Set random width
            imgWidth = (u32)((SQ(temp_f2) * 8.0f) + 2.0f);
        }

        if (negateY) {
            pos.y = -pos.y;
        }

        if ((i < 15) || ((i == 15) && ((((void)0, gSaveContext.save.day) % 7) == 0))) {
            // 2S2H [Port] Switched from gDPSetColor to gDPSetPrimColor for Color_RGBA8 individual fields
            Color_RGBA8 color = D_801DD8E0[i % ARRAY_COUNTU(D_801DD8E0)];
            gDPSetPrimColor(gfx++, 0, 0, color.r, color.g, color.b, color.a);
        } else if (((i & 0x3F) == 0) || (i == 16)) {
            Color_RGBA8 color = D_801DD900[phi_v1 % ARRAY_COUNTU(D_801DD900)];
            gDPSetPrimColor(gfx++, 0, 0, color.r, color.g, color.b, color.a);
            phi_v1++;
        }

        posPtr = &pos;
        imgXPtr = &imgX;
        imgYPtr = &imgY;
        viewProjectionMtxF = play->viewProjectionMtxF.mf;

        // 2S2H [Enhancement] The stars are originally drawn as texture rectangles which cannot be interpolated,
        // so if interpolation is enabled we branch and render the stars through the projection matrix instead.
        if (Ship_GetInterpolationFPS() != 20) {
            Ship_DrawSkyboxStarInterpolated(&gfx, play, pos, imgWidth, 4, i);
            continue;
        }

        if (imgWidth >= 2) {
            // w component
            scale = pos.x * play->viewProjectionMtxF.mf[0][3] + pos.y * play->viewProjectionMtxF.mf[1][3] +
                    pos.z * play->viewProjectionMtxF.mf[2][3] + play->viewProjectionMtxF.mf[3][3];
            if (scale >= 1.0f) {
                invScale = 1.0f / scale;
                // x component
                imgX = (pos.x * viewProjectionMtxF[0][0] + pos.y * viewProjectionMtxF[1][0] +
                        pos.z * viewProjectionMtxF[2][0] + viewProjectionMtxF[3][0]) *
                       invScale;
                // y component
                imgY = (((pos.x * viewProjectionMtxF[0][1]) + (pos.y * viewProjectionMtxF[1][1]) +
                         (pos.z * viewProjectionMtxF[2][1])) +
                        viewProjectionMtxF[3][1]) *
                       invScale;
            }

            // #region 2S2H [Cosmetic] Account for different aspect ratios than 4:3
            // An adjusted bounds is calculated based on the new screen width to allow more stars to be rendered
            f32 adjustedXBounds = 1.0f;
            if (OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) > SCREEN_WIDTH) {
                adjustedXBounds =
                    (f32)(OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) - (SCREEN_WIDTH / 2)) / (SCREEN_WIDTH / 2);
            }

            if ((scale >= 1.0f) && (imgX > -adjustedXBounds) && (imgX < adjustedXBounds) && (imgY > -1.0f) &&
                (imgY < 1.0f)) {
                if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
                    imgX *= -1.0f;
                }
                // #endregion
                imgX = (imgX * (SCREEN_WIDTH / 2)) + (SCREEN_WIDTH / 2);
                imgY = (imgY * -(SCREEN_HEIGHT / 2)) + (SCREEN_HEIGHT / 2);

                gfxTemp = gfx;
                Environment_DrawSkyboxStar(&gfxTemp, imgX, imgY, imgWidth, 4);
                gfx = gfxTemp;
            }
            //! FAKE:
            if (viewProjectionMtxF) {}
        }
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

void Environment_Draw(PlayState* play) {
    Environment_SetupSkyboxStars(play);
    Environment_DrawSun(play);
    Environment_UpdateLightningStrike(play);
    Environment_DrawLightning(play, 0);
    Environment_DrawSkyboxFilters(play);
}

void Environment_DrawSkyboxStars(PlayState* play) {
    Gfx* nextOpa;
    Gfx* opa;

    if (sSkyboxStarsDList != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        opa = POLY_OPA_DISP;
        nextOpa = Graph_GfxPlusOne(opa);

        gSPDisplayList(sSkyboxStarsDList, nextOpa);

        Environment_DrawSkyboxStarsImpl(play, &nextOpa);

        gSPEndDisplayList(nextOpa++);

        Graph_BranchDlist(opa, nextOpa);

        POLY_OPA_DISP = nextOpa;
        sSkyboxStarsDList = NULL;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Environment_StopTime(void) {
    sEnvIsTimeStopped = true;
}

void Environment_StartTime(void) {
    sEnvIsTimeStopped = false;
}

u8 Environment_IsTimeStopped(void) {
    return sEnvIsTimeStopped;
}

u32 Environment_GetStormState(PlayState* play) {
    u32 stormState = play->envCtx.stormState;

    if ((play->sceneId == SCENE_OMOYA) && (play->roomCtx.curRoom.num == 0)) {
        stormState = ((gSaveContext.save.day >= 2) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS))
                         ? STORM_STATE_ON
                         : STORM_STATE_OFF;
    }

    switch (play->sceneId) {
        case SCENE_13HUBUKINOMITI:
        case SCENE_11GORONNOSATO:
        case SCENE_10YUKIYAMANOMURA:
        case SCENE_14YUKIDAMANOMITI:
        case SCENE_12HAKUGINMAE:
        case SCENE_17SETUGEN:
        case SCENE_GORONRACE:
            if (gSaveContext.sceneLayer == 0) {
                stormState = STORM_STATE_OFF;
            }
            break;

        case SCENE_10YUKIYAMANOMURA2:
            if (gSaveContext.sceneLayer == 1) {
                stormState = STORM_STATE_OFF;
            }
            break;

        default:
            break;
    }

    return stormState;
}

u8 Environment_IsFinalHours(PlayState* play) {
    u8 ret = false;

    if ((gSaveContext.save.day == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        ret = true;
    }

    return ret;
}

u8 func_800FE5D0(PlayState* play) {
    u8 ret = false;

    if (Entrance_GetSceneId(((void)0, gSaveContext.save.entrance)) < 0) {
        ret = true;
    }

    return ret;
}

u32 func_800FE610(PlayState* play) {
    return 0;
}

u16 Environment_GetTimeSpeed(PlayState* play) {
    u16 timeSpeed = 0;

    if (R_TIME_SPEED != 0) {
        timeSpeed = R_TIME_SPEED + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
    }

    return timeSpeed;
}

void Environment_SetTimeJump(f32 minutes) {
    sTimeJump = CLOCK_TIME_F(0, minutes);
}

u8 func_800FE6F8(PlayState* play, s16 timeAdvanceScaling, s16 nextTimeLimit) {
    u16 nextTime;
    u8 ret = false;

    if (R_TIME_SPEED != 0) {
        nextTime =
            CURRENT_TIME + (R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset)) * timeAdvanceScaling * 20;
        if (nextTime > nextTimeLimit) {
            ret = true;
        }
    }

    return ret;
}

u8 Environment_GetSandstormColorIndex(void) {
    return sSandstormColorIndex;
}

u8 Environment_GetNextSandstormColorIndex(void) {
    return sNextSandstormColorIndex;
}

f32 Environment_GetSandstormLerpScale(void) {
    return sSandstormLerpScale;
}

void Environment_LerpSandstormColors(Color_RGBA8* colorSrc, Color_RGBA8* colorDst) {
    f32 sandstormLerpScale = Environment_GetSandstormLerpScale();
    s32 pad;
    u32 sandstormColorIndex = Environment_GetSandstormColorIndex();
    u32 nextSandstormColorIndex = Environment_GetNextSandstormColorIndex();

    if (sandstormLerpScale <= 0.0f) {
        colorDst->r = colorSrc[sandstormColorIndex].r;
        colorDst->g = colorSrc[sandstormColorIndex].g;
        colorDst->b = colorSrc[sandstormColorIndex].b;
        colorDst->a = colorSrc[sandstormColorIndex].a;
    } else {
        colorDst->r =
            (s32)F32_LERP(colorSrc[sandstormColorIndex].r, colorSrc[nextSandstormColorIndex].r, sandstormLerpScale);
        colorDst->g =
            (s32)F32_LERP(colorSrc[sandstormColorIndex].g, colorSrc[nextSandstormColorIndex].g, sandstormLerpScale);
        colorDst->b =
            (s32)F32_LERP(colorSrc[sandstormColorIndex].b, colorSrc[nextSandstormColorIndex].b, sandstormLerpScale);
        colorDst->a =
            (s32)F32_LERP(colorSrc[sandstormColorIndex].a, colorSrc[nextSandstormColorIndex].a, sandstormLerpScale);
    }
}

// Related to soft soil
u8 func_800FE9B4(PlayState* play) {
    u8 ret = false;

    if ((play->envCtx.precipitation[PRECIP_SOS_MAX] == 60) && (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0)) {
        ret = true;
    }

    if ((play->envCtx.precipitation[PRECIP_RAIN_MAX] == 60) &&
        !(GET_ACTIVE_CAM(play)->stateFlags & CAM_STATE_UNDERWATER)) {
        if ((Environment_GetStormState(play) != STORM_STATE_OFF) &&
            (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 0)) {
            ret = true;
        }
    }

    return ret;
}

void func_800FEA50(PlayState* play) {
    s8 phi_v0;

    switch (gSaveContext.save.day) {
        default:
        case 0:
        case 1:
            phi_v0 = 0;
            break;

        case 2:
            phi_v0 = 3;
            break;

        case 3:
            phi_v0 = 4;
            break;
    }

    D_801F4F30 = phi_v0;
    D_801F4F33 = true;
}

void func_800FEAB0() {
    D_801F4F33 = false;
}

void Environment_JumpForwardInTime(void) {
    if (sTimeJump != 0) {
        gSaveContext.save.time = CURRENT_TIME + sTimeJump;
        sTimeJump = 0;
    }
}

void Environment_NewDay(EnvironmentContext* envCtx) {
    u8 dayOffset = 0;

    if (((void)0, gSaveContext.save.day) != 0) {
        dayOffset = ((void)0, gSaveContext.save.day) - 1;
    }

    envCtx->skyboxConfig = dayOffset + (sInitSkyboxConfig * 3);
    envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;

    if (sInitSkyboxConfig == SKYBOX_CONFIG_4) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_14;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_5) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_16;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_6) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_17;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_7) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_18 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_8) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_21 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_9) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_24;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    } else if (sInitSkyboxConfig == SKYBOX_CONFIG_10) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_25 + dayOffset;
        envCtx->changeSkyboxNextConfig = envCtx->skyboxConfig;
    }

    if (dayOffset >= 3) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_13;
        envCtx->changeSkyboxNextConfig = SKYBOX_CONFIG_13;
    }

    if (envCtx->skyboxConfig >= SKYBOX_CONFIG_MAX) {
        envCtx->skyboxConfig = SKYBOX_CONFIG_0;
        envCtx->changeSkyboxNextConfig = SKYBOX_CONFIG_0;
    }

    switch (((void)0, gSaveContext.save.day)) {
        default:
        case 0:
        case 1:
            envCtx->lightConfig = 0;
            envCtx->changeLightNextConfig = 0;
            break;

        case 2:
            envCtx->lightConfig = 3;
            envCtx->changeLightNextConfig = 3;
            break;

        case 3:
            envCtx->lightConfig = 4;
            envCtx->changeLightNextConfig = 4;
            break;
    }
}
