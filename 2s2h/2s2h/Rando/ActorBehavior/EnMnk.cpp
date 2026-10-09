#include "ActorBehavior.h"

// This interaction is skipped by the SkipLearningSonataOfAwakening and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnMnkBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_MNK, IS_RANDO, {
        *should = false;

        RANDO_SAVE_CHECKS[RC_DEKU_KINGS_CHAMBER_MONKEY].eligible = true;
    });
}
