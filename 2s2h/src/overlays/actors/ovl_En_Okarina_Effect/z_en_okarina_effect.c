/*
 * File: z_en_okarina_effect.c
 * Overlay: ovl_En_Okarina_Effect
 * Description: Manages the storm created when playing Song of Storms
 */

#include "z_en_okarina_effect.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnOkarinaEffect_Init(Actor* thisx, PlayState* play);
void EnOkarinaEffect_Destroy(Actor* thisx, PlayState* play);
void EnOkarinaEffect_Update(Actor* thisx, PlayState* play);

void func_8096B104(EnOkarinaEffect* this, PlayState* play);
void func_8096B174(EnOkarinaEffect* this, PlayState* play);
void func_8096B1FC(EnOkarinaEffect* this, PlayState* play);

ActorProfile En_Okarina_Effect_Profile = {
    /**/ ACTOR_EN_OKARINA_EFFECT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnOkarinaEffect),
    /**/ EnOkarinaEffect_Init,
    /**/ EnOkarinaEffect_Destroy,
    /**/ EnOkarinaEffect_Update,
    /**/ NULL,
};

void EnOkarinaEffect_SetupAction(EnOkarinaEffect* this, EnOkarinaEffectActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnOkarinaEffect_Destroy(Actor* thisx, PlayState* play) {
}

void EnOkarinaEffect_Init(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    if (play->envCtx.precipitation[PRECIP_RAIN_CUR] != 0) {
        Actor_Kill(&this->actor);
    }
    EnOkarinaEffect_SetupAction(this, func_8096B104);
}

void func_8096B104(EnOkarinaEffect* this, PlayState* play) {
    this->timer = 80;
    play->envCtx.precipitation[PRECIP_SOS_MAX] = 60;
    gLightningStrike.delayTimer = 501.0f;
    play->envCtx.lightningState = LIGHTNING_LAST;
    Environment_PlayStormNatureAmbience(play);
    EnOkarinaEffect_SetupAction(this, func_8096B174);
}

void func_8096B174(EnOkarinaEffect* this, PlayState* play) {
    DECR(this->timer);

    if ((play->pauseCtx.state == PAUSE_STATE_OFF) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
        (play->msgCtx.msgLength == 0) && !FrameAdvance_IsEnabled(play) && (this->timer == 0)) {
        EnOkarinaEffect_SetupAction(this, func_8096B1FC);
    }
}

void func_8096B1FC(EnOkarinaEffect* this, PlayState* play) {
    if (play->envCtx.precipitation[PRECIP_SOS_MAX] != 0) {
        if ((play->state.frames & 3) == 0) {
            play->envCtx.precipitation[PRECIP_SOS_MAX]--;
            if (play->envCtx.precipitation[PRECIP_SOS_MAX] == 8) {
                Environment_StopStormNatureAmbience(play);
            }
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnOkarinaEffect_Update(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    this->actionFunc(this, play);
}
