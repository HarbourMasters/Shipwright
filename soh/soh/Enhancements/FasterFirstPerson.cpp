#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_FASTER_FIRST_PERSON_NAME CVAR_ENHANCEMENT("FasterFirstPerson")
#define CVAR_FASTER_FIRST_PERSON_VALUE CVarGetInteger(CVAR_FASTER_FIRST_PERSON_NAME, 0)

void RegisterFasterFirstPerson() {
    COND_VB_SHOULD(VB_FAST_FIRST_PERSON_TRANSITION, CVAR_FASTER_FIRST_PERSON_VALUE, { *should = true; });
}

static RegisterShipInitFunc initFunc(RegisterFasterFirstPerson, { CVAR_FASTER_FIRST_PERSON_NAME });