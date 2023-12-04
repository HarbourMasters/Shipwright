/*
 * File: z_en_okarina_effect.c
 * Overlay: ovl_En_Okarina_Effect
 * Description: Manages the storm created when playing Song of Storms
 */

#include "z_en_okarina_effect.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnOkarinaEffect_Init(Actor* thisx, PlayState* play);
void EnOkarinaEffect_Destroy(Actor* thisx, PlayState* play);
void EnOkarinaEffect_Update(Actor* thisx, PlayState* play);

void EnOkarinaEffect_TriggerStorm(EnOkarinaEffect* this, PlayState* play);
void EnOkarinaEffect_ManageStorm(EnOkarinaEffect* this, PlayState* play);

const ActorInit En_Okarina_Effect_InitVars = {
    ACTOR_EN_OKARINA_EFFECT,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnOkarinaEffect),
    (ActorFunc)EnOkarinaEffect_Init,
    (ActorFunc)EnOkarinaEffect_Destroy,
    (ActorFunc)EnOkarinaEffect_Update,
    NULL,
    NULL,
};

void EnOkarinaEffect_SetupAction(EnOkarinaEffect* this, EnOkarinaEffectActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnOkarinaEffect_Destroy(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    play->envCtx.unk_F2[0] = 0;
    if ((gWeatherMode != 4) && (gWeatherMode != 5) && (play->envCtx.gloomySkyMode == 1)) {
        play->envCtx.gloomySkyMode = 2; // end gloomy sky
        Environment_StopStormNatureAmbience(play);
    }
    play->envCtx.lightningMode = LIGHTNING_MODE_LAST;
}

void EnOkarinaEffect_Init(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    osSyncPrintf("\n\n");
    // "Ocarina Storm Effect"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ オカリナあらし効果ビカビカビカ〜 ☆☆☆☆☆ \n" VT_RST);
    osSyncPrintf("\n\n");
    if (play->envCtx.unk_EE[1] != 0) {
        Actor_Kill(&this->actor);
    }
    EnOkarinaEffect_SetupAction(this, EnOkarinaEffect_TriggerStorm);
}

void EnOkarinaEffect_TriggerStorm(EnOkarinaEffect* this, PlayState* play) {
    this->timer = 400;                   // 20 seconds
    play->envCtx.unk_F2[0] = 20;    // rain intensity target
    play->envCtx.gloomySkyMode = 1; // start gloomy sky
    if ((gWeatherMode != 0) || play->envCtx.unk_17 != 0) {
        play->envCtx.unk_DE = 1;
    }
    play->envCtx.lightningMode = LIGHTNING_MODE_ON;
    Environment_PlayStormNatureAmbience(play);
    EnOkarinaEffect_SetupAction(this, EnOkarinaEffect_ManageStorm);
}

void EnOkarinaEffect_ManageStorm(EnOkarinaEffect* this, PlayState* play) {
    Flags_UnsetEnv(play, 5); // clear storms env flag
    if (((play->pauseCtx.state == 0) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
         (play->msgCtx.msgLength == 0) && (!FrameAdvance_IsEnabled(play)) &&
         ((play->transitionMode == TRANS_MODE_OFF) || (gSaveContext.gameMode != 0))) ||
        (this->timer >= 250)) {
        if (play->envCtx.indoors || play->envCtx.unk_1F != 1) {
            this->timer--;
        }
        osSyncPrintf("\nthis->timer=[%d]", this->timer);
        if (this->timer == 308) {
            osSyncPrintf("\n\n\n豆よ のびろ 指定\n\n\n"); // "Let's grow some beans"
            Flags_SetEnv(play, 5);                   // set storms env flag
        }
    }

    if (D_8011FB38 != 0) {
        this->timer = 0;
    }

    if (this->timer == 0) {
        play->envCtx.unk_F2[0] = 0;
        if (play->csCtx.state == CS_STATE_IDLE) {
            Environment_StopStormNatureAmbience(play);
        } else if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
        }
        osSyncPrintf("\n\n\nE_wether_flg=[%d]", gWeatherMode);
        osSyncPrintf("\nrain_evt_trg=[%d]\n\n", play->envCtx.gloomySkyMode);
        if (gWeatherMode == 0 && (play->envCtx.gloomySkyMode == 1)) {
            play->envCtx.gloomySkyMode = 2; // end gloomy sky
        } else {
            play->envCtx.gloomySkyMode = 0;
            play->envCtx.unk_DE = 0;
        }
        play->envCtx.lightningMode = LIGHTNING_MODE_LAST;
        Actor_Kill(&this->actor);
    }
}

void EnOkarinaEffect_Update(Actor* thisx, PlayState* play) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    this->actionFunc(this, play);
    if (BREG(0) != 0) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0xFF, 0, 0xFF, 0xFF, 4, play->state.gfxCtx);
    }
}
