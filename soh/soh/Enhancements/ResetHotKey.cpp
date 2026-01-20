#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "overlays/gamestates/ovl_file_choose/file_choose.h"
}

static constexpr int32_t CVAR_RESET_BTN_MASK_DEFAULT = BTN_CUSTOM_MODIFIER2;
#define CVAR_RESET_BTN_MASK_NAME "gSettings.ResetBtn"
#define CVAR_RESET_BTN_MASK_VALUE CVarGetInteger(CVAR_RESET_BTN_MASK_NAME, CVAR_RESET_BTN_MASK_DEFAULT)

static void OnGameStateMainStartResetHotkey() {
    const int32_t packed = CVarGetInteger("gSettings.ResetBtn", BTN_CUSTOM_MODIFIER2);

    const uint16_t mask = static_cast<uint16_t>(packed & 0xFFFF);

    if (mask != 0 && CHECK_BTN_ANY(gGameState->input[0].press.button, mask) &&
        CHECK_BTN_ALL(gGameState->input[0].cur.button, mask)) {

        auto consoleWin = std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"));

        if (consoleWin) {
            consoleWin->Dispatch("reset");
        }
    }
}

static void RegisterResetHotkey() {
    COND_HOOK(OnGameStateMainStart, true, OnGameStateMainStartResetHotkey);
}

static RegisterShipInitFunc initFuncResetHotkey(RegisterResetHotkey, { CVAR_RESET_BTN_MASK_NAME });
