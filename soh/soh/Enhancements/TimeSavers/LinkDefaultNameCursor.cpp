#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/gamestates/ovl_file_choose/file_choose.h"
}

#define CVAR_LINK_DEFAULT_NAME CVAR_ENHANCEMENT("LinkDefaultName")

static void SetDefaultNameCursor(void* gameState) {
    auto* fileChoose = static_cast<FileChooseContext*>(gameState);
    if (fileChoose->newFileNameCharCount == 0) {
        return;
    }

    fileChoose->kbdX = FS_KBD_BTN_END;
    fileChoose->kbdY = 5;
    fileChoose->kbdButton = FS_KBD_BTN_END;
}

static void RegisterLinkDefaultNameCursor() {
    COND_HOOK(OnFileChooseNameEntry, CVarGetInteger(CVAR_LINK_DEFAULT_NAME, 0), SetDefaultNameCursor);
}

static RegisterShipInitFunc initFunc(RegisterLinkDefaultNameCursor, { CVAR_LINK_DEFAULT_NAME });
