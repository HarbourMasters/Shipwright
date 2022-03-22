/*
 * File: z_en_okarina_effect.c
 * Overlay: ovl_En_Okarina_Effect
 * Description: Manages the storm created when playing Song of Storms
 */

#include "z_en_okarina_effect.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnOkarinaEffect_Init(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaEffect_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnOkarinaEffect_Update(Actor* thisx, GlobalContext* globalCtx);

void EnOkarinaEffect_TriggerStorm(EnOkarinaEffect* this, GlobalContext* globalCtx);
void EnOkarinaEffect_ManageStorm(EnOkarinaEffect* this, GlobalContext* globalCtx);

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

void EnOkarinaEffect_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    globalCtx->envCtx.unk_F2[0] = 0;
    if ((gWeatherMode != 4) && (gWeatherMode != 5) && (globalCtx->envCtx.gloomySkyMode == 1)) {
        globalCtx->envCtx.gloomySkyMode = 2; // end gloomy sky
        Environment_StopStormNatureAmbience(globalCtx);
    }
    globalCtx->envCtx.lightningMode = LIGHTNING_MODE_LAST;
}

void EnOkarinaEffect_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    osSyncPrintf("\n\n");
    // "Ocarina Storm Effect"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ オカリナあらし効果ビカビカビカ〜 ☆☆☆☆☆ \n" VT_RST);
    osSyncPrintf("\n\n");
    if (globalCtx->envCtx.unk_EE[1] != 0) {
        Actor_Kill(&this->actor);
    }
    EnOkarinaEffect_SetupAction(this, EnOkarinaEffect_TriggerStorm);
}

void EnOkarinaEffect_TriggerStorm(EnOkarinaEffect* this, GlobalContext* globalCtx) {
    this->timer = 400;                   // 20 seconds
    globalCtx->envCtx.unk_F2[0] = 20;    // rain intensity target
    globalCtx->envCtx.gloomySkyMode = 1; // start gloomy sky
    if ((gWeatherMode != 0) || globalCtx->envCtx.unk_17 != 0) {
        globalCtx->envCtx.unk_DE = 1;
    }
    globalCtx->envCtx.lightningMode = LIGHTNING_MODE_ON;
    Environment_PlayStormNatureAmbience(globalCtx);
    EnOkarinaEffect_SetupAction(this, EnOkarinaEffect_ManageStorm);
}

void EnOkarinaEffect_ManageStorm(EnOkarinaEffect* this, GlobalContext* globalCtx) {
    Flags_UnsetEnv(globalCtx, 5); // clear storms env flag
    if (((globalCtx->pauseCtx.state == 0) && (globalCtx->gameOverCtx.state == GAMEOVER_INACTIVE) &&
         (globalCtx->msgCtx.msgLength == 0) && (!FrameAdvance_IsEnabled(globalCtx)) &&
         ((globalCtx->transitionMode == 0) || (gSaveContext.gameMode != 0))) ||
        (this->timer >= 250)) {
        if (globalCtx->envCtx.indoors || globalCtx->envCtx.unk_1F != 1) {
            this->timer--;
        }
        osSyncPrintf("\nthis->timer=[%d]", this->timer);
        if (this->timer == 308) {
            osSyncPrintf("\n\n\n豆よ のびろ 指定\n\n\n"); // "Let's grow some beans"
            Flags_SetEnv(globalCtx, 5);                   // set storms env flag
        }
    }

    if (D_8011FB38 != 0) {
        this->timer = 0;
    }

    if (this->timer == 0) {
        globalCtx->envCtx.unk_F2[0] = 0;
        if (globalCtx->csCtx.state == CS_STATE_IDLE) {
            Environment_StopStormNatureAmbience(globalCtx);
        } else if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_NATURE_AMBIENCE) {
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_LIGHTNING, CHANNEL_IO_PORT_1, 0);
            Audio_SetNatureAmbienceChannelIO(NATURE_CHANNEL_RAIN, CHANNEL_IO_PORT_1, 0);
        }
        osSyncPrintf("\n\n\nE_wether_flg=[%d]", gWeatherMode);
        osSyncPrintf("\nrain_evt_trg=[%d]\n\n", globalCtx->envCtx.gloomySkyMode);
        if (gWeatherMode == 0 && (globalCtx->envCtx.gloomySkyMode == 1)) {
            globalCtx->envCtx.gloomySkyMode = 2; // end gloomy sky
        } else {
            globalCtx->envCtx.gloomySkyMode = 0;
            globalCtx->envCtx.unk_DE = 0;
        }
        globalCtx->envCtx.lightningMode = LIGHTNING_MODE_LAST;
        Actor_Kill(&this->actor);
    }
}

void EnOkarinaEffect_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnOkarinaEffect* this = (EnOkarinaEffect*)thisx;

    this->actionFunc(this, globalCtx);
    if (BREG(0) != 0) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0xFF, 0, 0xFF, 0xFF, 4, globalCtx->state.gfxCtx);
    }
}
