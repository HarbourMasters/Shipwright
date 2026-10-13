#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "variables.h"
}

// This interaction is skipped by the SkipLearningSongOfHealing and forced on for rando for now, this file simply
// handles queuing up the checks to be given.
void Rando::ActorBehavior::InitEnOsnBehavior() {
    COND_VB_SHOULD(VB_OSN_CONSIDER_ELIGIBLE_FOR_SONG_OF_HEALING, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OSN, IS_RANDO, { *should = false; });

    /*
     * When the player enters the Clock Tower Interior entrance 3, the Happy Mask Salesman actor will play one of two
     * cutscenes: learning the Song of Healing (csId 11), or the typical moon crash cycle reset (csId 13). In rando, the
     * former is never expected. Moon crashes in rando can inadvertently trigger the Song of Healing scene, so we'll
     * just quietly change to the intended cutscene.
     */
    COND_VB_SHOULD(VB_START_CUTSCENE, IS_RANDO, {
        s16* csId = va_arg(args, s16*);
        if (gPlayState->sceneId == SCENE_INSIDETOWER && *csId == 11) { // Song of Healing tutorial
            *csId = 13;                                                // Moon crash new cycle scene
        }
    });
}
