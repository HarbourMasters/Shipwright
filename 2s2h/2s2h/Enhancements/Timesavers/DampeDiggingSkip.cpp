#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Tk/z_en_tk.h"
#include "overlays/actors/ovl_En_Bigpo/z_en_bigpo.h"

void func_80AEDBEC(EnTk* thisx, PlayState* play);
void func_80AEE6B8(EnTk* thisx, PlayState* play);
void EnBigpo_FireCounting(EnBigpo* thisx, PlayState* play);
void EnBigpo_RevealedFireIdle(EnBigpo* thisx, PlayState* play);
void EnBigpo_SetupSpawnCutscene(EnBigpo* thisx);
}

#define CVAR_NAME "gEnhancements.Timesavers.DampeDiggingSkip"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterDampeDiggingSkip() {
    // Updates Dampe to think 3 fires are revealed upon digging, which then sets his cutscene state.
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_TK, CVAR, [](Actor* actor) {
        EnTk* dampe = (EnTk*)actor;
        if (dampe->actionFunc != func_80AEE6B8) {
            return;
        }

        if (dampe->unk_2CA & 0x20) {
            dampe->unk_2E4 = 3;
        }
    });

    // Sets the summoned Big Poe to appear after one fire is revealed
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_BIGPO, CVAR, [](Actor* actor) {
        EnBigpo* bigPo = (EnBigpo*)actor;
        // Look for the real big poe when it is counting the fires
        if (bigPo->actor.params != BIG_POE_TYPE_SUMMONED || bigPo->actionFunc != EnBigpo_FireCounting) {
            return;
        }

        for (EnBigpo* firePo = (EnBigpo*)bigPo->actor.child; firePo != NULL; firePo = (EnBigpo*)firePo->actor.child) {
            if ((firePo->actor.params == BIG_POE_TYPE_REVEALED_FIRE) &&
                (firePo->actionFunc == EnBigpo_RevealedFireIdle)) {
                EnBigpo_SetupSpawnCutscene(bigPo);

                // Update Dampe if the enhancement was toggled after digging
                EnTk* dampe = (EnTk*)SubS_FindActor(gPlayState, NULL, ACTORCAT_NPC, ACTOR_EN_TK);
                if (dampe != NULL && dampe->actor.params >= 0) {
                    func_80AEDBEC(dampe, gPlayState);
                }
                break;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDampeDiggingSkip, { CVAR_NAME });
