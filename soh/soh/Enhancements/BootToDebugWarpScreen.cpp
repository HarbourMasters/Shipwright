#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "functions.h"

extern "C" {
#include "z64.h"
#include "overlays/gamestates/ovl_file_choose/file_choose.h"
}

static constexpr int32_t CVAR_DEBUG_ENABLED_DEFAULT = 0;
#define CVAR_DEBUG_ENABLED_NAME CVAR_DEVELOPER_TOOLS("DebugEnabled")
#define CVAR_DEBUG_ENABLED_VALUE CVarGetInteger(CVAR_DEBUG_ENABLED_NAME, CVAR_DEBUG_ENABLED_DEFAULT)

static constexpr int32_t CVAR_BOOT_TO_DEBUG_WARP_SCREEN_DEFAULT = 0;
#define CVAR_BOOT_TO_DEBUG_WARP_SCREEN_NAME CVAR_DEVELOPER_TOOLS("BootToDebugWarpScreen")
#define CVAR_BOOT_TO_DEBUG_WARP_SCREEN_VALUE \
    CVarGetInteger(CVAR_BOOT_TO_DEBUG_WARP_SCREEN_NAME, CVAR_BOOT_TO_DEBUG_WARP_SCREEN_DEFAULT)

void OnFileChooseMainBootToDebugWarpScreen(void* gameState) {
    FileChooseContext* fileChooseContext = (FileChooseContext*)gameState;
    fileChooseContext->buttonIndex = 0xFF;
    fileChooseContext->menuMode = FS_MENU_MODE_SELECT;
    fileChooseContext->selectMode = SM_LOAD_GAME;
}

void RegisterBootToDebugWarpScreen() {
    COND_HOOK(OnFileChooseMain, CVAR_DEBUG_ENABLED_VALUE && CVAR_BOOT_TO_DEBUG_WARP_SCREEN_VALUE,
              OnFileChooseMainBootToDebugWarpScreen);
}

static RegisterShipInitFunc initFunc_BootToDebugWarpScreen(RegisterBootToDebugWarpScreen,
                                                           { CVAR_DEBUG_ENABLED_NAME,
                                                             CVAR_BOOT_TO_DEBUG_WARP_SCREEN_NAME });
