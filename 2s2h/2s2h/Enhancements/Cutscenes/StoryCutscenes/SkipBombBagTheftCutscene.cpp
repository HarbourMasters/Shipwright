#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Suttari/z_en_suttari.h"
void EnSuttari_TriggerTransition(PlayState* play, u16 entrance);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBombBagTheftCutscene() {
    COND_VB_SHOULD(VB_SAKON_TAKE_DAMAGE, CVAR, {
        EnSuttari* enSuttari = va_arg(args, EnSuttari*);
        if (enSuttari->actor.colChkInfo.damageEffect == 0xF && enSuttari->scheduleResult == 4) {
            // scheduleResult becomes 4 when Sakon steals the bomb bag. damageEffect is 0xF if the player stopped Sakon
            // without killing him. In this scenario, skip ahead to the transition instead of waiting for him flee. N
            // need to alter the scenario where the player kills Sakon, as the Bomb Shop Lady will already trigger the
            // transition upon his death.
            enSuttari->actor.speed = 0.0f;
            SET_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG);
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
            enSuttari->flags2 |= 4 | 8;
            EnSuttari_TriggerTransition(gPlayState, ENTRANCE(NORTH_CLOCK_TOWN, 7));
        } else {
            // Avoid cutscene
            *should = false;
        }
    });
    /*
     * Upon the second bomb theft textbox, Sakon fires off a move route that updates even before the textbox closes.
     * This means it is possible for him to escape before the textbox is dismissed. This happens even in vanilla, so
     * note that this is not a bug introduced by this skip.
     */
    COND_VB_SHOULD(VB_QUEUE_CUTSCENE, CVAR, {
        /*
         * Scene 18 in North Clock Town is the bomb bag theft. Preventing the cutscene from starting is not sufficient;
         * Sakon's actor will lock Link in place if this cutscene is queued. Preventing the queue instead allows the
         * player to roam freely as the theft takes place.
         */
        if (gPlayState->sceneId == SCENE_BACKTOWN) {
            s16* csId = va_arg(args, s16*);
            if (*csId == 18) {
                *should = false;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBombBagTheftCutscene, { CVAR_NAME });
