#include "ActorBehavior.h"

// This interaction is skipped by the SkipLearningElegyOfEmptiness and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnKnightBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_KNIGHT, IS_RANDO, {
        *should = false;

        RANDO_SAVE_CHECKS[RC_ANCIENT_CASTLE_OF_IKANA_BOSS].eligible = true;
    });
}
