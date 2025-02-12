#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"

extern "C" SaveContext gSaveContext;

#define CVAR_DEKU_NUT_UPGRADE_FIX_NAME CVAR_ENHANCEMENT("DekuNutUpgradeFix")
#define CVAR_DEKU_NUT_UPGRADE_FIX_DEFAULT 0
#define CVAR_DEKU_NUT_UPGRADE_FIX_VALUE \
    IS_RANDO || CVarGetInteger(CVAR_DEKU_NUT_UPGRADE_FIX_NAME, CVAR_DEKU_NUT_UPGRADE_FIX_DEFAULT)

void DekuNutUpgradeFixAtForestStage(bool* should) {
    s32 expectedNutUpgrades = (INV_CONTENT(ITEM_NUT) == ITEM_NUT ? 1 : 0) +
                              (Flags_GetInfTable(INFTABLE_BOUGHT_NUT_UPGRADE) ? 1 : 0) +
                              (Flags_GetItemGetInf(ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE) ? 1 : 0);
    s32 actualNutUpgrades = CUR_UPG_VALUE(UPG_NUTS);

    if (expectedNutUpgrades != actualNutUpgrades) {
        Flags_UnsetItemGetInf(ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE);
        *should = true;
    }
}

void DekuNutUpgradeSetByPoachersSaw(bool* should) {
    *should = false;
}

void RegisterDekuNutUpgradeFix() {
    COND_VB_SHOULD(VB_POACHERS_SAW_SET_DEKU_NUT_UPGRADE_FLAG, CVAR_DEKU_NUT_UPGRADE_FIX_VALUE,
                   { DekuNutUpgradeSetByPoachersSaw(should); });
    COND_VB_SHOULD(VB_DEKU_SCRUBS_REACT_TO_MASK_OF_TRUTH, CVAR_DEKU_NUT_UPGRADE_FIX_VALUE,
                   { DekuNutUpgradeFixAtForestStage(should); });
}

static RegisterShipInitFunc initFunc(RegisterDekuNutUpgradeFix, { CVAR_DEKU_NUT_UPGRADE_FIX_NAME, "IS_RANDO" });
