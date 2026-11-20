#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/gamestates/ovl_file_choose/file_choose.h"
}

#define CVAR_LINK_DEFAULT_NAME CVAR_ENHANCEMENT("LinkDefaultName")

static void RegisterLinkDefaultNameCursor() {
    COND_VB_SHOULD(VB_FILE_SELECT_NAME_ENTRY_READY, CVarGetInteger(CVAR_LINK_DEFAULT_NAME, 0), {
        auto* fileChoose = va_arg(args, FileChooseContext*);
        if (fileChoose->newFileNameCharCount > 0) {
            fileChoose->kbdX = FS_KBD_BTN_END;
            fileChoose->kbdY = 5;
            fileChoose->kbdButton = FS_KBD_BTN_END;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLinkDefaultNameCursor, { CVAR_LINK_DEFAULT_NAME });
