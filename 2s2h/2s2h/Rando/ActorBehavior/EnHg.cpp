#include "ActorBehavior.h"

void Rando::ActorBehavior::InitEnHgBehavior() {
    COND_VB_SHOULD(VB_HAVE_HEALED_PAMELAS_FATHER, IS_RANDO,
                   { *should = RANDO_SAVE_CHECKS[RC_MUSIC_BOX_HOUSE_FATHER].cycleObtained; });
}