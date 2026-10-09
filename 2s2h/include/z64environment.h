#ifndef Z64ENVIRONMENT_H
#define Z64ENVIRONMENT_H

#include "PR/ultratypes.h"
#include "PR/os_message.h"
#include "color.h"
#include "z64dma.h"
#include "z64light.h"
#include "z64math.h"
#include "unk.h"

struct GameOverContext;
struct GraphicsContext;
struct LightContext;
struct MessageContext;
struct PauseContext;
struct PlayState;
struct SkyboxContext;
struct View;

#define FILL_SCREEN_OPA (1 << 0)
#define FILL_SCREEN_XLU (1 << 1)

#define NEXT_TIME_NONE 0xFFFF
#define NEXT_TIME_DAY CLOCK_TIME(12, 0)
#define NEXT_TIME_NIGHT CLOCK_TIME(0, 0)
#define NEXT_TIME_DAY_SET 0xFFFE
#define NEXT_TIME_NIGHT_SET 0xFFFD

#define LIGHT_SETTING_MAX 31
#define LIGHT_SETTING_OVERRIDE_NONE 0xFF

// This mode disables the updating of lights in both light modes.
// With this mode enabled, the only way lights can change is via the adjustment arrays.
// This mode is not used in the original game.
#define LIGHT_SETTING_OVERRIDE_FULL_CONTROL 0xFE

#define LIGHT_BLENDRATE_OVERRIDE_NONE 0xFFFF
#define LIGHT_BLEND_OVERRIDE_NONE 0
#define LIGHT_BLEND_OVERRIDE_ON 1

// This mode disables the light system's automatic blending between
// light settings for `LIGHT_MODE_SETTINGS` (or using a light setting override).
// This is a bit of a hack used only by bosses in the original game.
#define LIGHT_BLEND_OVERRIDE_FULL_CONTROL 2

typedef enum {
    /* 0 */ LIGHT_MODE_TIME, // environment lights use `lightConfig` and change based on time of day
    /* 1 */ LIGHT_MODE_SETTINGS // environment lights use `lightSetting`
} LightMode;

typedef enum {
    /* 0 */ LIGHTNING_OFF, // no lightning
    /* 1 */ LIGHTNING_ON, // request ligtning strikes at random intervals
    /* 2 */ LIGHTNING_LAST // request one lightning strike before turning off
} LightningState;

typedef enum {
    /* 0 */ LIGHTNING_STRIKE_WAIT, // wait between lightning strikes. request bolts when timer hits 0
    /* 1 */ LIGHTNING_STRIKE_START, // fade in the flash. note: bolts are requested in the previous state
    /* 2 */ LIGHTNING_STRIKE_END // fade out the flash and go back to wait
} LightningStrikeState;

typedef enum {
    /* 0 */ WEATHER_MODE_CLEAR,
    /* 1 */ WEATHER_MODE_RAIN,
    /* 2 */ WEATHER_MODE_2,
    /* 3 */ WEATHER_MODE_SNOW,
    /* 4 */ WEATHER_MODE_4,
    /* 5 */ WEATHER_MODE_5
} WeatherMode;

typedef enum {
    /* 0 */ CHANGE_SKYBOX_INACTIVE,
    /* 1 */ CHANGE_SKYBOX_REQUESTED,
    /* 2 */ CHANGE_SKYBOX_WAIT,
    /* 3 */ CHANGE_SKYBOX_ACTIVE
} ChangeSkyboxState;

typedef enum {
    /*  0 */ SKYBOX_DMA_INACTIVE,
    /*  1 */ SKYBOX_DMA_TEXTURE1_START,
    /* 11 */ SKYBOX_DMA_TEXTURE2_START = 11
} SkyboxDmaState;

typedef enum SkyboxConfig {
    /* 0x00 */ SKYBOX_CONFIG_0,
    /* 0x01 */ SKYBOX_CONFIG_1,
    /* 0x02 */ SKYBOX_CONFIG_2,
    /* 0x03 */ SKYBOX_CONFIG_3,
    /* 0x04 */ SKYBOX_CONFIG_4,
    /* 0x05 */ SKYBOX_CONFIG_5,
    /* 0x06 */ SKYBOX_CONFIG_6,
    /* 0x07 */ SKYBOX_CONFIG_7,
    /* 0x08 */ SKYBOX_CONFIG_8,
    /* 0x09 */ SKYBOX_CONFIG_9,
    /* 0x0A */ SKYBOX_CONFIG_10,
    /* 0x0B */ SKYBOX_CONFIG_11,
    /* 0x0C */ SKYBOX_CONFIG_12,
    /* 0x0D */ SKYBOX_CONFIG_13,
    /* 0x0E */ SKYBOX_CONFIG_14,
    /* 0x0F */ SKYBOX_CONFIG_15,
    /* 0x10 */ SKYBOX_CONFIG_16,
    /* 0x11 */ SKYBOX_CONFIG_17,
    /* 0x12 */ SKYBOX_CONFIG_18,
    /* 0x13 */ SKYBOX_CONFIG_19,
    /* 0x14 */ SKYBOX_CONFIG_20,
    /* 0x15 */ SKYBOX_CONFIG_21,
    /* 0x16 */ SKYBOX_CONFIG_22,
    /* 0x17 */ SKYBOX_CONFIG_23,
    /* 0x18 */ SKYBOX_CONFIG_24,
    /* 0x19 */ SKYBOX_CONFIG_25,
    /* 0x1A */ SKYBOX_CONFIG_26,
    /* 0x1B */ SKYBOX_CONFIG_27,
    /* 0x1C */ SKYBOX_CONFIG_MAX
} SkyboxConfig;

//! @TODO: Verify from OoT (may not be the same)
typedef enum {
    /* 0 */ PRECIP_RAIN_MAX, // max number of raindrops that can draw; uses this or SOS_MAX, whichever is larger
    /* 1 */ PRECIP_RAIN_CUR, // current number of rain drops being drawn on screen
    /* 2 */ PRECIP_SNOW_CUR, // current number of snowflakes being drawn on screen
    /* 3 */ PRECIP_SNOW_MAX, // max number of snowflakes that can draw
    /* 4 */ PRECIP_SOS_MAX, // max number of rain drops requested from song of storms specifically
    /* 5 */ PRECIP_MAX
} PrecipitationData;

typedef enum {
    /* 0 */ STORM_REQUEST_NONE,
    /* 1 */ STORM_REQUEST_START,
    /* 2 */ STORM_REQUEST_STOP
} StormRequest;

typedef enum {
    /* 0 */ STORM_STATE_OFF,
    /* 1 */ STORM_STATE_ON
} StormState;

typedef enum {
    /* 0x00 */ TIMESEQ_DAY_BGM,
    /* 0x01 */ TIMESEQ_FADE_DAY_BGM,
    /* 0x02 */ TIMESEQ_NIGHT_BEGIN_SFX,
    /* 0x03 */ TIMESEQ_EARLY_NIGHT_CRITTERS,
    /* 0x04 */ TIMESEQ_NIGHT_DELAY,
    /* 0x05 */ TIMESEQ_NIGHT_CRITTERS,
    /* 0x06 */ TIMESEQ_DAY_BEGIN_SFX,
    /* 0x07 */ TIMESEQ_MORNING_CRITTERS,
    /* 0x08 */ TIMESEQ_DAY_DELAY,
    /* 0xFE */ TIMESEQ_REQUEST = 0xFE,
    /* 0xFF */ TIMESEQ_DISABLED
} TimeBasedSeqState;

#define SCENESEQ_DEFAULT 0
#define SCENESEQ_MORNING 0xFE

typedef enum {
    /* 0x0 */ SANDSTORM_OFF,
    /* 0x1 */ SANDSTORM_FILL,
    /* 0x2 */ SANDSTORM_UNFILL,
    /* 0x3 */ SANDSTORM_ACTIVE,
    /* 0x4 */ SANDSTORM_DISSIPATE,
    /* 0x5 */ SANDSTORM_5,
    /* 0x6 */ SANDSTORM_6,
    /* 0x7 */ SANDSTORM_7,
    /* 0x8 */ SANDSTORM_8,
    /* 0x9 */ SANDSTORM_9,
    /* 0xA */ SANDSTORM_A,
    /* 0xB */ SANDSTORM_B,
    /* 0xC */ SANDSTORM_C,
    /* 0xD */ SANDSTORM_D
} SandstormState;

typedef struct LightningStrike {
    /* 0x00 */ u8 state;
    /* 0x01 */ u8 flashRed;
    /* 0x02 */ u8 flashGreen;
    /* 0x03 */ u8 flashBlue;
    /* 0x04 */ u8 flashAlphaTarget;
    /* 0x08 */ f32 delayTimer;
} LightningStrike; // size = 0xC

#define ENV_FOGNEAR_MAX 996
#define ENV_ZFAR_MAX 15000

typedef struct {
    /* 0x00 */ u8 ambientColor[3];
    /* 0x03 */ s8 light1Dir[3];
    /* 0x06 */ u8 light1Color[3];
    /* 0x09 */ s8 light2Dir[3];
    /* 0x0C */ u8 light2Color[3];
    /* 0x0F */ u8 fogColor[3];
    /* 0x12 */ s16 fogNear; // ranges from 0-1000 (0: starts immediately, 1000: no fog), but is clamped to ENV_FOGNEAR_MAX
    /* 0x14 */ s16 zFar; // Max depth (render distance) of the view as a whole. fogFar will always match zFar
} CurrentEnvLightSettings; // size = 0x16

// `EnvLightSettings` is very similar to `CurrentEnvLightSettings` with one key difference.
// The light settings data in the scene packs blend rate information with the fog near value.
// The blendRate determines how fast the current light settings fade to the next one 
// (under LIGHT_MODE_SETTINGS, otherwise unused). 

// Get blend rate from `EnvLightSettings.blendRateAndFogNear` in 0-255 range
#define ENV_LIGHT_SETTINGS_BLEND_RATE_U8(blendRateAndFogNear) (((blendRateAndFogNear) >> 10) * 4)
#define ENV_LIGHT_SETTINGS_FOG_NEAR(blendRateAndFogNear) ((blendRateAndFogNear) & 0x3FF)

typedef struct {
    /* 0x00 */ u8 ambientColor[3];
    /* 0x03 */ s8 light1Dir[3];
    /* 0x06 */ u8 light1Color[3];
    /* 0x09 */ s8 light2Dir[3];
    /* 0x0C */ u8 light2Color[3];
    /* 0x0F */ u8 fogColor[3];
    /* 0x12 */ s16 blendRateAndFogNear; 
    /* 0x14 */ s16 zFar;
} EnvLightSettings; // size = 0x16

// ZAPD compatibility typedefs
// TODO: Remove when ZAPD adds support for them
typedef CurrentEnvLightSettings LightSettings;

typedef struct EnvironmentContext {
    /* 0x00 */ u16 unk_0;
    /* 0x02 */ u16 sceneTimeSpeed;
    /* 0x04 */ Vec3f sunPos;
    /* 0x10 */ u8 skybox1Index;
    /* 0x11 */ u8 skybox2Index;
    /* 0x12 */ u8 unk_12;
    /* 0x13 */ u8 skyboxBlend;
    /* 0x14 */ u8 unk_14;
    /* 0x15 */ u8 skyboxDisabled;
    /* 0x16 */ u8 sunDisabled;
    /* 0x17 */ u8 skyboxConfig;
    /* 0x18 */ u8 changeSkyboxNextConfig;
    /* 0x19 */ u8 changeSkyboxState;
    /* 0x1A */ u16 changeSkyboxTimer;
    /* 0x1C */ u16 unk_1C;
    /* 0x1E */ u8 lightMode;
    /* 0x1F */ u8 lightConfig;
    /* 0x20 */ u8 changeLightNextConfig;
    /* 0x21 */ u8 changeLightEnabled;
    /* 0x22 */ u16 changeLightTimer;
    /* 0x24 */ u16 changeDuration;
    /* 0x26 */ u8 unk_26;
    /* 0x28 */ LightInfo dirLight1; // sun 1
    /* 0x36 */ LightInfo dirLight2; // sun 2
    /* 0x44 */ s8 skyboxDmaState;
    /* 0x48 */ DmaRequest dmaRequest;
    /* 0x68 */ OSMesgQueue loadQueue;
    /* 0x80 */ OSMesg loadMsg[1];
    /* 0x84 */ f32 glareAlpha;
    /* 0x88 */ f32 lensFlareAlphaScale;
    /* 0x8C */ AdjLightSettings adjLightSettings;
    /* 0xA8 */ f32 unk_A8;
    /* 0xAC */ Vec3s windDirection;
    /* 0xB4 */ f32 windSpeed;
    /* 0xB8 */ u8 numLightSettings;
    /* 0xBC */ EnvLightSettings* lightSettingsList; // list of light settings from the scene file
    /* 0xC0 */ u8 lightBlendEnabled; // only used with `LIGHT_MODE_SETTINGS` or on override
    /* 0xC1 */ u8 lightSetting; // only used with `LIGHT_MODE_SETTINGS` or on override
    /* 0xC2 */ u8 prevLightSetting;
    /* 0xC3 */ u8 lightSettingOverride;
    /* 0xC4 */ CurrentEnvLightSettings lightSettings; // settings for the currently "live" lights
    /* 0xDA */ u16 lightBlendRateOverride;
    /* 0xDC */ f32 lightBlend;
    /* 0xE0 */ u8 lightBlendOverride;
    /* 0xE1 */ u8 stormRequest;
    /* 0xE2 */ u8 stormState;
    /* 0xE3 */ u8 lightningState; // modified by unused func in EnWeatherTag
    /* 0xE4 */ u8 timeSeqState;
    /* 0xE5 */ u8 fillScreen;
    /* 0xE6 */ u8 screenFillColor[4];
    /* 0xEA */ u8 sandstormState;
    /* 0xEB */ u8 sandstormPrimA;
    /* 0xEC */ u8 sandstormEnvA;
    /* 0xED */ u8 customSkyboxFilter;
    /* 0xEE */ u8 skyboxFilterColor[4];
    /* 0xF2 */ u8 precipitation[PRECIP_MAX];
    /* 0xF7 */ UNK_TYPE1 unk_F7[9];
} EnvironmentContext; // size = 0x100

s32 Environment_ZBufValToFixedPoint(s32 zBufferVal);
void Environment_Init(struct PlayState* play2, EnvironmentContext* envCtx, s32 arg2);
u8 Environment_SmoothStepToU8(u8* pvalue, u8 target, u8 scale, u8 step, u8 minStep);
f32 Environment_LerpWeight(u16 max, u16 min, u16 val);
f32 Environment_LerpWeightAccelDecel(u16 endFrame, u16 startFrame, u16 curFrame, u16 accelDuration, u16 decelDuration);
void Environment_UpdateSkybox(u8 skyboxId, EnvironmentContext* envCtx, struct SkyboxContext* skyboxCtx);
void Environment_EnableUnderwaterLights(struct PlayState* play, s32 waterLightsIndex);
void Environment_DisableUnderwaterLights(struct PlayState* play);
void Environment_Update(struct PlayState* play, EnvironmentContext* envCtx, struct LightContext* lightCtx,
                        struct PauseContext* pauseCtx, struct MessageContext* msgCtx, struct GameOverContext* gameOverCtx,
                        struct GraphicsContext* gfxCtx);
void Environment_DrawSunLensFlare(struct PlayState* play, EnvironmentContext* envCtx, struct View* view, struct GraphicsContext* gfxCtx, Vec3f vec);
void Environment_DrawLensFlare(struct PlayState* play, EnvironmentContext* envCtx, struct View* view, struct GraphicsContext* gfxCtx, Vec3f pos, f32 scale, f32 colorIntensity, s16 glareStrength, u8 isSun);
void Environment_DrawRain(struct PlayState* play, struct View* view, struct GraphicsContext* gfxCtx);
void Environment_ChangeLightSetting(struct PlayState* play, u8 lightSetting);
void Environment_AddLightningBolts(struct PlayState* play, u8 num);
void Environment_PlaySceneSequence(struct PlayState* play);
void Environment_DrawCustomLensFlare(struct PlayState* play);
void Environment_InitGameOverLights(struct PlayState* play);
void Environment_FadeInGameOverLights(struct PlayState* play);
void Environment_FadeOutGameOverLights(struct PlayState* play);
void Environment_FillScreen(struct GraphicsContext* gfxCtx, u8 red, u8 green, u8 blue, u8 alpha, u8 drawFlags);
void Environment_DrawSandstorm(struct PlayState* play, u8 sandstormState);
s32 Environment_AdjustLights(struct PlayState* play, f32 arg1, f32 arg2, f32 arg3, f32 arg4);
void Environment_LerpAmbientColor(struct PlayState* play, Color_RGB8* to, f32 lerp);
void Environment_LerpDiffuseColor(struct PlayState* play, Color_RGB8* to, f32 lerp);
void Environment_LerpFogColor(struct PlayState* play, Color_RGB8* to, f32 lerp);
void Environment_LerpFog(struct PlayState* play, s16 fogNearTarget, s16 fogFarTarget, f32 lerp);
void Environment_ForcePlaySequence(u16 seqId);
s32 Environment_IsForcedSequenceDisabled(void);
void Environment_PlayStormNatureAmbience(struct PlayState* play);
void Environment_StopStormNatureAmbience(struct PlayState* play);
void Environment_Draw(struct PlayState* play);
void Environment_DrawSkyboxStars(struct PlayState* play);
void Environment_StopTime(void);
void Environment_StartTime(void);
u8 Environment_IsTimeStopped(void);
u32 Environment_GetStormState(struct PlayState* play);
u8 Environment_IsFinalHours(struct PlayState* play);
u16 Environment_GetTimeSpeed(struct PlayState* play);
void Environment_SetTimeJump(f32 minutes);
u8 func_800FE6F8(struct PlayState* play, s16 timeAdvanceScaling, s16 nextTimeLimit);
void Environment_LerpSandstormColors(Color_RGBA8* colorSrc, Color_RGBA8* colorDst);
u8 func_800FE9B4(struct PlayState* play);
void func_800FEA50(struct PlayState* play);
void func_800FEAB0(void);
void Environment_NewDay(EnvironmentContext* envCtx);

// Data
extern u8 gWeatherMode;
extern u8 gLightConfigAfterUnderwater;
extern u8 gInterruptSongOfStorms;
extern u8 gSceneSeqState;

// Bss
extern u8 D_801F4E30;

extern u8 gCustomLensFlare1On;
extern Vec3f gCustomLensFlare1Pos;
extern f32 D_801F4E44;
extern f32 D_801F4E48;
extern s16 D_801F4E4C;

extern u8 gCustomLensFlare2On;
extern Vec3f gCustomLensFlare2Pos;
extern f32 D_801F4E5C;
extern f32 D_801F4E60;
extern s16 D_801F4E64;

extern struct LightningStrike gLightningStrike;
extern f32 D_801F4E74;
extern u16 D_801F4E78;
extern u16 gSkyboxNumStars;

#endif
