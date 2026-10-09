#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_En_Mnk/z_en_mnk.h"
void EnMnk_Monkey_WaitToRun(EnMnk* thisx, PlayState* play);
void EnMnk_Monkey_SetupRunAfterTalk(EnMnk* thisx, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBothersomeMonkey() {
    COND_VB_SHOULD(VB_MONKEY_WAIT_TO_TALK_AFTER_APPROACH, CVAR, {
        EnMnk* monkey = (EnMnk*)va_arg(args, EnMnk*);
        if (MONKEY_GET_TYPE(&monkey->picto.actor) == MONKEY_OUTSIDEWOODS) {
            *should = false;
            SET_EVENTINF(EVENTINF_25);
            SET_WEEKEVENTREG(WEEKEVENTREG_79_02);
            if (MONKEY_GET_SWITCH_FLAG(&monkey->picto.actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, MONKEY_GET_SWITCH_FLAG(&monkey->picto.actor));
            }
            monkey->actionFunc = EnMnk_Monkey_WaitToRun;
        } else if (MONKEY_GET_TYPE(&monkey->picto.actor) == MONKEY_OUTSIDECHAMBER) {
            *should = false;
            SET_WEEKEVENTREG(WEEKEVENTREG_08_02);
            if (MONKEY_GET_SWITCH_FLAG(&monkey->picto.actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, MONKEY_GET_SWITCH_FLAG(&monkey->picto.actor));
            }
            monkey->actionFunc = EnMnk_Monkey_SetupRunAfterTalk;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBothersomeMonkey, { CVAR_NAME });
