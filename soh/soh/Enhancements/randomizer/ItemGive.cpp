#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "z64save.h"
extern PlayState* gPlayState;
}

// These hooks are registered on boot rather than through the "IS_RANDO" path, because starting
// items are handed out by Randomizer_InitSaveFile -> SetStartingItems when the file is created,
// which happens long before OnLoadGame fires. Each body checks IS_RANDO itself.

// Swaps an item on the other age's C buttons, for items that upgrade in place
static void UpdateOtherAgeEquips(uint8_t fromItem, uint8_t toItem) {
    ItemEquips* otherAgeEquips = LINK_IS_CHILD ? &gSaveContext.adultEquips : &gSaveContext.childEquips;

    for (uint8_t i = 1; i < ARRAY_COUNT(otherAgeEquips->buttonItems); i++) {
        if (otherAgeEquips->buttonItems[i] == fromItem) {
            otherAgeEquips->buttonItems[i] = toItem;
            if (gPlayState != NULL) {
                Interface_LoadItemIcon1(gPlayState, i);
            }
        }
    }
}

static void RandomizerOnItemGive(GetItemEntry itemEntry) {
    // Items given through Randomizer_Item_Give set their own flags
    if (!IS_RANDO || itemEntry.modIndex != MOD_NONE) {
        return;
    }

    uint16_t item = itemEntry.itemId;

    switch (item) {
        case ITEM_WALLET_ADULT:
            if (RAND_GET_OPTION(RSK_FULL_WALLETS)) {
                Rupees_ChangeBy(200);
            }
            return;
        case ITEM_WALLET_GIANT:
            if (RAND_GET_OPTION(RSK_FULL_WALLETS)) {
                Rupees_ChangeBy(500);
            }
            return;
        // Accounting for an equip swapped hookshot as child
        case ITEM_LONGSHOT:
            UpdateOtherAgeEquips(ITEM_HOOKSHOT, ITEM_LONGSHOT);
            return;
        case ITEM_OCARINA_TIME:
            UpdateOtherAgeEquips(ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME);
            return;
        case ITEM_LETTER_RUTO:
            Flags_SetRandomizerInf(RAND_INF_OBTAINED_RUTOS_LETTER);
            return;
        // Handed to the trade sequence NPCs, not a trade item itself
        case ITEM_SOLD_OUT:
            return;
        case ITEM_LETTER_ZELDA:
            Flags_SetRandomizerInf(RAND_INF_ZELDAS_LETTER);
            // don't care about zelda's letter if it's already been shown to the guard
            if (!Flags_GetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD)) {
                Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_LETTER_ZELDA);
            }
            return;
        case ITEM_WEIRD_EGG:
            Flags_SetRandomizerInf(RAND_INF_WEIRD_EGG);
            break;
        default:
            break;
    }

    if (item >= ITEM_POCKET_EGG && item <= ITEM_CLAIM_CHECK) {
        Flags_SetRandomizerInf((RandomizerInf)(item - ITEM_POCKET_EGG + RAND_INF_ADULT_TRADES_HAS_POCKET_EGG));
    } else if (item >= ITEM_WEIRD_EGG && item <= ITEM_MASK_TRUTH) {
        Flags_SetRandomizerInf((RandomizerInf)(item - ITEM_WEIRD_EGG + RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG));
    }
}

static void RegisterRandomizerItemGive() {
    COND_HOOK(OnItemReceive, true, RandomizerOnItemGive);

    // Prevents getting sticks and nuts without their bag in case something got missed
    COND_VB_SHOULD(VB_ITEM_GIVE, true, {
        uint8_t item = va_arg(args, int);

        if (IS_RANDO) {
            if ((item == ITEM_STICK || item == ITEM_STICKS_5 || item == ITEM_STICKS_10) &&
                RAND_GET_OPTION(RSK_SHUFFLE_DEKU_STICK_BAG) && CUR_UPG_VALUE(UPG_STICKS) == 0) {
                *should = false;
            }

            if ((item == ITEM_NUT || item == ITEM_NUTS_5 || item == ITEM_NUTS_10) &&
                RAND_GET_OPTION(RSK_SHUFFLE_DEKU_NUT_BAG) && CUR_UPG_VALUE(UPG_NUTS) == 0) {
                *should = false;
            }
        }
    });

    // Bottle contents are never obtainable in rando, so a full bottle inventory must not fall
    // through to the inventory store, which would overwrite the first bottle
    COND_VB_SHOULD(VB_ITEM_GIVE_USE_INVENTORY_SLOT, true, {
        uint8_t item = va_arg(args, int);

        if (IS_RANDO && (((item >= ITEM_POTION_RED) && (item <= ITEM_POE)) || item == ITEM_MILK)) {
            *should = false;
        }
    });

    // Rando hands out equipment and magic through Randomizer_Item_Give, so nothing in these ranges
    // is ever obtainable through the vanilla path
    COND_VB_SHOULD(VB_ITEM_CHECK_OBTAINABILITY, true, {
        uint8_t item = va_arg(args, int);

        if (IS_RANDO && (item == ITEM_SINGLE_MAGIC || item == ITEM_DOUBLE_MAGIC || item == ITEM_DOUBLE_DEFENSE ||
                         ((item >= ITEM_SWORD_KOKIRI) && (item <= ITEM_SWORD_BGS)) ||
                         ((item >= ITEM_SHIELD_DEKU) && (item <= ITEM_SHIELD_MIRROR)) ||
                         ((item >= ITEM_TUNIC_KOKIRI) && (item <= ITEM_TUNIC_ZORA)) ||
                         ((item >= ITEM_BOOTS_KOKIRI) && (item <= ITEM_BOOTS_HOVER)))) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterRandomizerItemGive);
