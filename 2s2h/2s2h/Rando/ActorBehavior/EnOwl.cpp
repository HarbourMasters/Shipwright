#include "ActorBehavior.h"
#include "2s2h/Rando/Logic/Logic.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
}

void Rando::ActorBehavior::InitEnOwlBehavior() {
    /*
     * Vanilla behavior is to kill the owl when the Lens of Truth is obtained; it is assumed that magic is already
     * obtained and therefore the Lens is usable to show the invisible platforms without the owl's help. In rando
     * logic, Lens may be obtained without magic, so we use this hook to only kill this owl if both Lens and magic are
     * acquired.
     */
    COND_VB_SHOULD(VB_KILL_GORON_VILLAGE_OWL, IS_RANDO, { *should = HAS_ITEM(ITEM_LENS_OF_TRUTH) && HAS_MAGIC; });
}
