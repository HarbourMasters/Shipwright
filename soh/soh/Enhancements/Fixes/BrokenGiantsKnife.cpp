#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"

extern "C" PlayState* gPlayState;

static constexpr int32_t CVAR_BGS_FIX_DEFAULT = 0;
#define CVAR_BGS_FIX_NAME CVAR_ENHANCEMENT("FixBrokenGiantsKnife")
#define CVAR_BGS_FIX_VALUE CVarGetInteger(CVAR_BGS_FIX_NAME, CVAR_BGS_FIX_DEFAULT)

void OnReceiveBrokenGiantsKnife(GetItemEntry itemEntry) {
    if (itemEntry.itemId != ITEM_SWORD_BGS) {
        return;
    }

    // Flag wasn't reset because Kokiri or Master Sword was missing, so we need to bypass those checks
    int32_t bypassEquipmentChecks = (1 << EQUIP_INV_SWORD_KOKIRI) | (1 << EQUIP_INV_SWORD_MASTER);

    int32_t allSwordsInEquipment = bypassEquipmentChecks | ALL_EQUIP_VALUE(EQUIP_TYPE_SWORD);
    int32_t allSwordFlags = (1 << EQUIP_INV_SWORD_KOKIRI) | (1 << EQUIP_INV_SWORD_MASTER) |
                            (1 << EQUIP_INV_SWORD_BIGGORON) | (1 << EQUIP_INV_SWORD_BROKENGIANTKNIFE);

    if (allSwordsInEquipment != allSwordFlags) {
        return;
    }

    gSaveContext.inventory.equipment ^= OWNED_EQUIP_FLAG_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);

    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KNIFE) {
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
        if (gPlayState != NULL) {
            Interface_LoadItemIcon1(gPlayState, 0);
        }
    }
}

void RegisterBrokenGiantsKnifeFix() {
    // If enhancement is off, flag should be handled exclusively by vanilla behaviour
    COND_HOOK(OnItemReceive, CVAR_BGS_FIX_VALUE || IS_RANDO, OnReceiveBrokenGiantsKnife);
}

static RegisterShipInitFunc initFunc(RegisterBrokenGiantsKnifeFix, { CVAR_BGS_FIX_NAME, "IS_RANDO" });
