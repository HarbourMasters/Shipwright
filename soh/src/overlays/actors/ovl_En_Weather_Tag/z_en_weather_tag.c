/*
 * File: z_en_weather_tag.c
 * Overlay: ovl_En_Weather_Tag
 * Description: Proximity Activated Weather Effects
 */

#include "z_en_weather_tag.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void EnWeatherTag_Init(Actor* thisx, PlayState* play);
void EnWeatherTag_Destroy(Actor* thisx, PlayState* play);
void EnWeatherTag_Update(Actor* thisx, PlayState* play);

void EnWeatherTag_DisabledCloudyHyruleMarket(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledCloudyHyruleMarket(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledCloudyLonLonRanch(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledCloudyLonLonRanch(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledCloudySnow(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledCloudySnow(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledRainLakeHylia(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledRainLakeHylia(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledCloudyDeathMountain(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledCloudyDeathMountain(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledCloudyRainThunderKakariko(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledCloudyRainThunderKakariko(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_SetSandstormIntensity(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_DisabledRainThunder(EnWeatherTag* this, PlayState* play);
void EnWeatherTag_EnabledRainThunder(EnWeatherTag* this, PlayState* play);

#define WEATHER_TAG_RANGE100(x) ((x >> 8) * 100.0f)

const ActorInit En_Weather_Tag_InitVars = {
    ACTOR_EN_WEATHER_TAG,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWeatherTag),
    (ActorFunc)EnWeatherTag_Init,
    (ActorFunc)EnWeatherTag_Destroy,
    (ActorFunc)EnWeatherTag_Update,
    NULL,
    NULL,
};

void EnWeatherTag_SetupAction(EnWeatherTag* this, EnWeatherTagActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnWeatherTag_Destroy(Actor* thisx, PlayState* play) {
}

void EnWeatherTag_Init(Actor* thisx, PlayState* play) {
    EnWeatherTag* this = (EnWeatherTag*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_0;

    switch (this->actor.params & 0xF) {
        case EN_WEATHER_TAG_TYPE_CLOUDY_MARKET:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ (;o;) About ☆☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ （;o;) くらいよー ☆☆☆☆☆ \n" VT_RST);
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyHyruleMarket);
            break;
        case EN_WEATHER_TAG_TYPE_CLOUDY_LON_LON_RANCH:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Cloudy (._.) Ah Melancholy ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり (._.) あーあ 憂鬱 ☆☆☆☆☆ \n" VT_RST);
            if (Flags_GetEventChkInf(0x18)) {
                Actor_Kill(&this->actor);
            }
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyLonLonRanch);
            break;
        case EN_WEATHER_TAG_TYPE_SNOW_ZORAS_DOMAIN:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Yukigafuru You won't come (._.) ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ ゆきがふるー あなたはこないー (._.) ☆☆☆☆☆ \n" VT_RST);

            if (gSaveContext.eventChkInf[4] & 0x400) {
                Actor_Kill(&this->actor);
            }
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudySnow);
            break;
        case EN_WEATHER_TAG_TYPE_RAIN_LAKE_HYLIA:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Wow wa wa na wa saa ki ha (^o^) ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ わわわわー なーがーさーきーはー (^o^) ☆☆☆☆☆ \n" VT_RST);

            if (gSaveContext.eventChkInf[4] & 0x400) {
                Actor_Kill(&this->actor);
            }
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledRainLakeHylia);
            break;
        case EN_WEATHER_TAG_TYPE_CLOUDY_DEATH_MOUNTAIN:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Cloudy (._.) Ah Melancholy ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり (._.) あーあ 憂鬱 ☆☆☆☆☆ \n" VT_RST);
            if (gSaveContext.eventChkInf[4] & 0x200) {
                Actor_Kill(&this->actor);
            }
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyDeathMountain);
            break;
        case EN_WEATHER_TAG_TYPE_THUNDERSTORM_KAKARIKO:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Cloudy Rain Thunder (;O;) Uo Melancholy ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ くもり雨雷 (;O;) うおお 憂鬱 ☆☆☆☆☆ \n" VT_RST);

            if (!(gSaveContext.eventChkInf[4] & 0x100) || !(gSaveContext.eventChkInf[4] & 0x200) ||
                !(gSaveContext.eventChkInf[4] & 0x400) || CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                Actor_Kill(&this->actor);
            }
            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyRainThunderKakariko);
            break;
        case EN_WEATHER_TAG_TYPE_SANDSTORM_INTENSITY:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ The desert becomes thicker ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 砂漠が濃くなります ☆☆☆☆☆ \n" VT_RST);
            EnWeatherTag_SetupAction(this, EnWeatherTag_SetSandstormIntensity);
            break;
        case EN_WEATHER_TAG_TYPE_THUNDERSTORM_GRAVEYARD:
            osSyncPrintf("\n\n");
            // "☆☆☆☆☆ Wow wa wa na wa saa ki ha (^o^) ☆☆☆☆☆"
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ わわわわー なーがーさーきーはー (^o^) ☆☆☆☆☆ \n" VT_RST);

            EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledRainThunder);
            break;
    }
}

u8 WeatherTag_CheckEnableWeatherEffect(EnWeatherTag* this, PlayState* play, u8 arg2, u8 arg3, u8 arg4, u8 arg5,
                                       u16 arg6, u8 weatherMode) {
    s32 pad;
    u8 ret = false;
    Player* player = GET_PLAYER(play);

    if (Actor_WorldDistXZToActor(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        if ((play->envCtx.indoors != 0) || !gSkyboxBlendingEnabled ||
            (play->skyboxId != SKYBOX_NORMAL_SKY && play->envCtx.unk_1F == play->envCtx.unk_20)) {
            D_8011FB38 = 1;
            if (play->envCtx.gloomySkyMode == 0 &&
                (play->envCtx.indoors != 0 || (play->envCtx.unk_1F != 1 && play->envCtx.unk_21 == 0))) {
                D_8011FB38 = 0;
                if (gWeatherMode != weatherMode) {
                    gWeatherMode = weatherMode;
                    if (play->envCtx.gloomySkyMode == 0) {
                        play->envCtx.unk_19 = 1;
                        play->envCtx.unk_17 = arg2;
                        play->envCtx.unk_18 = arg3;
                        play->envCtx.unk_1A = arg6;
                        play->envCtx.unk_21 = 1;
                        play->envCtx.unk_1F = arg4;
                        play->envCtx.unk_20 = arg5;
                        D_8011FB34 = arg5;
                        play->envCtx.unk_24 = arg6;
                        play->envCtx.unk_22 = play->envCtx.unk_24;
                    }
                }
                ret = true;
            }
        } else {
            if (gTimeIncrement != 0) {
                gSaveContext.dayTime += 0x14;
            }
        }
    }

    return ret;
}

u8 WeatherTag_CheckRestoreWeather(EnWeatherTag* this, PlayState* play, u8 arg2, u8 arg3, u8 arg4, u8 arg5,
                                  u16 arg6) {
    s32 pad;
    u8 ret = false;
    Player* player = GET_PLAYER(play);

    if ((WEATHER_TAG_RANGE100(this->actor.params) + 100.0f) < Actor_WorldDistXZToActor(&player->actor, &this->actor)) {
        if (play->envCtx.indoors != 0 || !gSkyboxBlendingEnabled ||
            (play->skyboxId != SKYBOX_NORMAL_SKY && play->envCtx.unk_1F == play->envCtx.unk_20)) {
            D_8011FB38 = 1;
            if ((play->envCtx.gloomySkyMode == 0) &&
                (play->envCtx.indoors != 0 || (play->envCtx.unk_1F != 1 && play->envCtx.unk_21 == 0))) {
                D_8011FB38 = 0;
                gWeatherMode = 0;
                play->envCtx.unk_19 = 1;
                play->envCtx.unk_17 = arg2;
                play->envCtx.unk_18 = arg3;
                play->envCtx.unk_1A = arg6;
                play->envCtx.unk_21 = 1;
                play->envCtx.unk_1F = arg4;
                play->envCtx.unk_20 = arg5;
                D_8011FB34 = arg5;
                play->envCtx.unk_24 = arg6;
                play->envCtx.unk_22 = play->envCtx.unk_24;

                ret = true;
            }
        } else if (gTimeIncrement != 0) {
            gSaveContext.dayTime += 0x14;
        }
    }
    return ret;
}

void EnWeatherTag_DisabledCloudyHyruleMarket(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 3, 60, 1)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledCloudyHyruleMarket);
    }
}

void EnWeatherTag_EnabledCloudyHyruleMarket(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 3, 0, 60)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyHyruleMarket);
    }
}

void EnWeatherTag_DisabledCloudyLonLonRanch(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 2, 100, 2)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledCloudyLonLonRanch);
    }
}

void EnWeatherTag_EnabledCloudyLonLonRanch(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 2, 0, 100)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyLonLonRanch);
    }
}

void EnWeatherTag_DisabledCloudyDeathMountain(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 2, 60, 2)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledCloudyDeathMountain);
    }
}

void EnWeatherTag_EnabledCloudyDeathMountain(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 2, 0, 60)) {
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyLonLonRanch);
    }
}

void EnWeatherTag_DisabledCloudySnow(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 2, 60, 3)) {
        play->envCtx.unk_EE[3] = 64;
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledCloudySnow);
    }
}

void EnWeatherTag_EnabledCloudySnow(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 2, 0, 60)) {
        play->envCtx.unk_EE[3] = 0;
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudySnow);
    }
}

void EnWeatherTag_DisabledRainLakeHylia(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 2, 100, 4)) {
        Environment_PlayStormNatureAmbience(play);
        play->envCtx.unk_EE[0] = 25;
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledRainLakeHylia);
    }
}

void EnWeatherTag_EnabledRainLakeHylia(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 2, 0, 100)) {
        Environment_StopStormNatureAmbience(play);
        play->envCtx.unk_EE[0] = 0;
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledRainLakeHylia);
    }
}

void EnWeatherTag_DisabledCloudyRainThunderKakariko(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckEnableWeatherEffect(this, play, 0, 1, 0, 4, 100, 5)) {
        Environment_PlayStormNatureAmbience(play);
        play->envCtx.lightningMode = LIGHTNING_MODE_ON;
        play->envCtx.unk_EE[0] = 30;
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledCloudyRainThunderKakariko);
    }
}

void EnWeatherTag_EnabledCloudyRainThunderKakariko(EnWeatherTag* this, PlayState* play) {
    if (WeatherTag_CheckRestoreWeather(this, play, 1, 0, 4, 0, 100)) {
        Environment_StopStormNatureAmbience(play);
        play->envCtx.lightningMode = LIGHTNING_MODE_LAST;
        play->envCtx.unk_EE[0] = 0;
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledCloudyRainThunderKakariko);
    }
}

void EnWeatherTag_SetSandstormIntensity(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_WorldDistXZToActor(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        Math_SmoothStepToS(&play->envCtx.adjFogNear, -0x50, 1, 2, 1);
        Math_SmoothStepToS(&play->envCtx.adjFogFar, -0x7D0, 1, 50, 1);
    } else {
        Math_SmoothStepToS(&play->envCtx.adjFogNear, 0, 1, 1, 1);
        Math_SmoothStepToS(&play->envCtx.adjFogFar, 0, 1, 25, 1);
    }
}

void EnWeatherTag_DisabledRainThunder(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_WorldDistXZToActor(&player->actor, &this->actor) < WEATHER_TAG_RANGE100(this->actor.params)) {
        Environment_PlayStormNatureAmbience(play);
        play->envCtx.lightningMode = LIGHTNING_MODE_ON;
        play->envCtx.unk_EE[0] = 25;
        EnWeatherTag_SetupAction(this, EnWeatherTag_EnabledRainThunder);
    }
}

void EnWeatherTag_EnabledRainThunder(EnWeatherTag* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((WEATHER_TAG_RANGE100(this->actor.params) + 10.0f) < Actor_WorldDistXZToActor(&player->actor, &this->actor)) {
        Environment_StopStormNatureAmbience(play);
        play->envCtx.lightningMode = LIGHTNING_MODE_LAST;
        play->envCtx.unk_EE[0] = 0;
        play->envCtx.unk_EE[1] = 10;
        EnWeatherTag_SetupAction(this, EnWeatherTag_DisabledRainThunder);
    }
}

void EnWeatherTag_Update(Actor* thisx, PlayState* play) {
    EnWeatherTag* this = (EnWeatherTag*)thisx;

    this->actionFunc(this, play);
    if (BREG(0) != 0) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 255, 255, 4, play->state.gfxCtx);
    }
}
