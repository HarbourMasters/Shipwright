#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

static constexpr int32_t CVAR_DISABLENAVICALLAUDIO_DEFAULT = 0;
#define CVAR_DISABLENAVICALLAUDIO_NAME CVAR_AUDIO("DisableNaviCallAudio")
#define CVAR_DISABLENAVICALLAUDIO_VALUE \
    CVarGetInteger(CVAR_DISABLENAVICALLAUDIO_NAME, CVAR_DISABLENAVICALLAUDIO_DEFAULT)

void RegisterDisableNaviCallAudio() {
    COND_VB_SHOULD(VB_PLAY_NAVI_CALL_SOUND, CVAR_DISABLENAVICALLAUDIO_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(RegisterDisableNaviCallAudio, { CVAR_DISABLENAVICALLAUDIO_NAME });
