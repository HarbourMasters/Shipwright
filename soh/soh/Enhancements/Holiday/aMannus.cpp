#include "Holiday.hpp"

extern "C" {
#include "variables.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#define AUTHOR "aMannus"
#define CVAR(v) "gHoliday." AUTHOR "." v

static void ConfigurationChanged() {
    if (gPlayState != NULL) {
        if (CVarGetInteger(CVAR("RocsFeather"), 0)) {
            INV_CONTENT(ITEM_NAYRUS_LOVE) = ITEM_ROCS_FEATHER;
        } else {
            if (gSaveContext.nayrusCollected) {
                INV_CONTENT(ITEM_NAYRUS_LOVE) = ITEM_NAYRUS_LOVE;
            } else {
                INV_CONTENT(ITEM_NAYRUS_LOVE) = ITEM_NONE;
            }
        }
    }
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);

    if (UIWidgets::EnhancementCheckbox("Roc's Feather", CVAR("RocsFeather"))) {
        ConfigurationChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    ConfigurationChanged();
    // #endregion
}

static Holiday holiday(DrawMenu, RegisterMod);
