#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"

extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_DEFAULT = 0;
#define CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_NAME CVAR_SETTING("A11yNoScreenFlashForFinishingBlow")
#define CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_VALUE \
    CVarGetInteger(CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_NAME, CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_DEFAULT)

static void RegisterA11yNoScreenFlashForFinishingBlow() {
    COND_VB_SHOULD(VB_FLASH_SCREEN_FOR_FINISHING_BLOW, CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_VALUE, {
        *should = false;
        gPlayState->envCtx.fillScreen = false; // Force screen fill disabled
    });
}

static RegisterShipInitFunc initFunc(RegisterA11yNoScreenFlashForFinishingBlow,
                                     { CVAR_A11YNOSCREENFLASHFORFINISHINGBLOW_NAME });
