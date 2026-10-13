#include "ActorBehavior.h"
/*
 * This accounts for any interactions that check for the presence of the Zora Mask, which is synonymous with Mikau's
 * spirit being put to rest. Affected actors are EnZog (Mikau himself), EnSekihi (Mikau's grave), and EnTsn (the
 * fisherman, who has minor dialog about Mikau floating in the bay).
 */
void Rando::ActorBehavior::InitEnZogBehavior() {
    COND_VB_SHOULD(VB_CONSIDER_MIKAU_HEALED, IS_RANDO, {
        int shouldIfMikauIsGone = va_arg(args, int);
        // Check both eligible and cycleObtained flags to account for both cutscene skips and scene reloads
        if (shouldIfMikauIsGone) {
            *should = (RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].eligible ||
                       RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].cycleObtained);
        } else {
            // Only if Mikau is still present, i.e. kill his gravestone's actor
            *should = !(RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].eligible ||
                        RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].cycleObtained);
        }
    });
}