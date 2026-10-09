#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Jg/z_en_jg.h"
}

void Rando::ActorBehavior::InitEnJgBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_JG, IS_RANDO, {
        // Do not grant vanilla Lullaby Intro item so that we can override it with rando item
        *should = false;
    });

    COND_VB_SHOULD(VB_JG_THINK_YOU_KNOW_LULLABY, IS_RANDO, {
        // Always consider lullaby known so we don't go into the cutscene to learn it
        *should = true;

        if (!RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].cycleObtained) {
            RANDO_SAVE_CHECKS[RC_PATH_TO_GORON_VILLAGE_LULLABY_INTRO].eligible = true;
        }
    });
}
