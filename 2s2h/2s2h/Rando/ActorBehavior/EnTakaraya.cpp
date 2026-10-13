#include "ActorBehavior.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Takaraya/z_en_takaraya.h"
void func_80ADF608(EnTakaraya* enTakaraya, PlayState* play);
void func_80ADF520(EnTakaraya* enTakaraya, PlayState* play);
}

#define ENTAKARAYA_RC (enTakaraya->actor.home.rot.x)

void func_80ADF520_modified(EnTakaraya* enTakaraya, PlayState* play) {
    SkelAnime_Update(&enTakaraya->skelAnime);
    if (!Play_InCsMode(play)) {
        if (Flags_GetTreasure(play, enTakaraya->actor.params)) {
            Flags_SetSwitch(play, enTakaraya->formSwitchFlag);
            play->actorCtx.sceneFlags.chest &= ~TAKARAYA_GET_TREASURE_FLAG(&enTakaraya->actor);
        } else if (RANDO_SAVE_CHECKS[ENTAKARAYA_RC].cycleObtained) {
            enTakaraya->timer = 10;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_6;
            func_80ADF608(enTakaraya, play);
        } else if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == TIMER_STATE_OFF) {
            enTakaraya->timer = 50;
            Message_StartTextbox(play, 0x77D, &enTakaraya->actor);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            func_80ADF608(enTakaraya, play);
        }
    }
}

void Rando::ActorBehavior::InitEnTakarayaBehavior() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_TAKARAYA, IS_RANDO, [](Actor* actor) {
        EnTakaraya* enTakaraya = (EnTakaraya*)actor;
        if (enTakaraya->actionFunc == func_80ADF520) {
            Actor* box =
                Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_BOX, ACTORCAT_CHEST, 99999.9f);
            if (box == nullptr) {
                return;
            }
            if (box->home.rot.x >= RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_DEKU &&
                box->home.rot.x <= RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_ZORA) {
                ENTAKARAYA_RC = box->home.rot.x;
                enTakaraya->actionFunc = func_80ADF520_modified;
            }
        }
    });
}
