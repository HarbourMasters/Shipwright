#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.h"
}

static constexpr int32_t CVAR_CUCCOS_TO_RETURN_DEFAULT = 7;
#define CVAR_CUCCOS_TO_RETURN_NAME CVAR_ENHANCEMENT("CuccosToReturn")
#define CVAR_CUCCOS_TO_RETURN_VALUE CVarGetInteger(CVAR_CUCCOS_TO_RETURN_NAME, CVAR_CUCCOS_TO_RETURN_DEFAULT)
#define CVAR_CUCCOS_TO_RETURN_SET (CVAR_CUCCOS_TO_RETURN_VALUE != CVAR_CUCCOS_TO_RETURN_DEFAULT)

static void RegisterCuccosToReturn() {
    COND_VB_SHOULD(VB_SET_CUCCO_COUNT, CVAR_CUCCOS_TO_RETURN_SET, {
        EnNiwLady* enNiwLady = va_arg(args, EnNiwLady*);
        // Override starting Cucco count using setting value
        enNiwLady->cuccosInPen = 7 - CVAR_CUCCOS_TO_RETURN_VALUE;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterCuccosToReturn, { CVAR_CUCCOS_TO_RETURN_NAME });
