#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/gamestates/ovl_file_choose/z_file_select.h"

extern FileSelectState* gFileSelectState;
}

#define CVAR_NAME "gEnhancements.Saving.FileSlot3"
#define CVAR CVarGetInteger(CVAR_NAME, true)

void RegisterFileSlot3() {
    static int lastValue = -1;
    int value = CVAR;
    bool changed = (lastValue != -1) && (lastValue != value);
    lastValue = value;

    // When we're adding the third file slot we need to refresh the entire file select state. We're only wanting
    // to do this when the value changes, as this hook gets run when a preset is applied, and the user might
    // have been in the middle of a file creation when they applied the preset.
    if (changed && gFileSelectState != NULL) {
        STOP_GAMESTATE(&gFileSelectState->state);
        SET_NEXT_GAMESTATE(&gFileSelectState->state, FileSelect_Init, sizeof(FileSelectState));
    }
}

static RegisterShipInitFunc initFunc(RegisterFileSlot3, { CVAR_NAME });
