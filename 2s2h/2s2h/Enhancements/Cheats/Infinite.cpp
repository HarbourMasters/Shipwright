#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

static RegisterShipInitFunc healthInitFunc(
    []() {
        COND_HOOK(OnGameStateUpdate, CVarGetInteger("gCheats.InfiniteHealth", 0), []() {
            gSaveContext.save.saveInfo.playerData.health = gSaveContext.save.saveInfo.playerData.healthCapacity;
        });
    },
    { "gCheats.InfiniteHealth" });

static RegisterShipInitFunc magicInitFunc(
    []() {
        COND_HOOK(OnGameStateUpdate, CVarGetInteger("gCheats.InfiniteMagic", 0), []() {
            uint8_t magicLevel = gSaveContext.save.saveInfo.playerData.magicLevel;
            if (magicLevel == 1) {
                gSaveContext.save.saveInfo.playerData.magic = MAGIC_NORMAL_METER;
            } else if (magicLevel == 2) {
                gSaveContext.save.saveInfo.playerData.magic = MAGIC_DOUBLE_METER;
            }
        });
    },
    { "gCheats.InfiniteMagic" });

static RegisterShipInitFunc rupeesInitFunc(
    []() {
        COND_HOOK(OnGameStateUpdate, CVarGetInteger("gCheats.InfiniteRupees", 0),
                  []() { gSaveContext.save.saveInfo.playerData.rupees = CUR_CAPACITY(UPG_WALLET); });
    },
    { "gCheats.InfiniteRupees" });

static RegisterShipInitFunc consumeablesInitFunc(
    []() {
        COND_HOOK(OnGameStateUpdate, CVarGetInteger("gCheats.InfiniteConsumables", 0), []() {
            if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
                AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            }

            if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
                AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            }

            if (INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU) {
                AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
            }

            if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
                AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
            }

            if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
                AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
            }

            if (INV_CONTENT(ITEM_MAGIC_BEANS) == ITEM_MAGIC_BEANS) {
                AMMO(ITEM_MAGIC_BEANS) = 20;
            }

            if (INV_CONTENT(ITEM_POWDER_KEG) == ITEM_POWDER_KEG) {
                AMMO(ITEM_POWDER_KEG) = CVarGetInteger("gEnhancements.Items.ExtraPowderKegs", 0) ? 3 : 1;
            }
        });
    },
    { "gCheats.InfiniteConsumables" });
