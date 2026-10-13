#include "ActorBehavior.h"

// This accounts for both EnGg and EnGg2, both of which act as Darmani's ghost.
void Rando::ActorBehavior::InitEnGgBehavior() {
    COND_VB_SHOULD(VB_CONSIDER_DARMANI_HEALED, IS_RANDO,
                   { *should = RANDO_SAVE_CHECKS[RC_GORON_GRAVEYARD_DARMANI].cycleObtained; });
}