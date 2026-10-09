#include "MiscBehavior.h"

extern "C" {
#include <variables.h>
#include <functions.h>
}

SaveContext saveContextCopy;

// TODO: This should be moved somewhere else
bool IsRenewable(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_BOTTLE_MILK:
        case RI_MILK_REFILL:
        case RI_BOTTLE_GOLD_DUST:
        case RI_GOLD_DUST_REFILL:
        case RI_BOTTLE_CHATEAU_ROMANI:
        case RI_CHATEAU_ROMANI_REFILL:
        case RI_RED_POTION_REFILL:
        case RI_BLUE_POTION_REFILL:
        case RI_GREEN_POTION_REFILL:
        case RI_MAGIC_BEAN:
            return true;
        default:
            return false;
    }
}

void Rando::MiscBehavior::BeforeEndOfCycleSave() {
    memcpy(&saveContextCopy, &gSaveContext, sizeof(SaveContext));
}

void Rando::MiscBehavior::AfterEndOfCycleSave() {
    for (int i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.inventory.dungeonItems[i] = saveContextCopy.save.saveInfo.inventory.dungeonItems[i];
        gSaveContext.save.saveInfo.inventory.dungeonKeys[i] =
            saveContextCopy.save.shipSaveInfo.rando.foundDungeonKeys[i];
        gSaveContext.save.saveInfo.inventory.strayFairies[i] = saveContextCopy.save.saveInfo.inventory.strayFairies[i];
    }

    if (saveContextCopy.save.saveInfo.weekEventReg[((WEEKEVENTREG_08_80) >> 8)] & ((WEEKEVENTREG_08_80)&0xFF)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
    }

    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_GOLD_SKULLTULAS]) {
        gSaveContext.save.saveInfo.skullTokenCount = saveContextCopy.save.saveInfo.skullTokenCount;
    }

    // Persist found frogs in frog shuffle. TODO: Make this optional
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_FROGS]) {
        if (saveContextCopy.save.saveInfo.weekEventReg[32] & 0x40) {
            SET_WEEKEVENTREG(WEEKEVENTREG_32_40);
        }
        if (saveContextCopy.save.saveInfo.weekEventReg[32] & 0x80) {
            SET_WEEKEVENTREG(WEEKEVENTREG_32_80);
        }
        if (saveContextCopy.save.saveInfo.weekEventReg[33] & 0x01) {
            SET_WEEKEVENTREG(WEEKEVENTREG_33_01);
        }
        if (saveContextCopy.save.saveInfo.weekEventReg[33] & 0x02) {
            SET_WEEKEVENTREG(WEEKEVENTREG_33_02);
        }
    }

    // For now, we're just going to always persist these slots. We may do something smarter here later if this causes
    // any issues.
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_DEED] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_DEED];
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_KEY_MAMA];
    gSaveContext.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE] =
        saveContextCopy.save.saveInfo.inventory.items[SLOT_TRADE_COUPLE];

    // Unset any flags used for checks, whether or not they get the item or junk is determined on our end instead.
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        RANDO_SAVE_CHECKS[randoCheckId].cycleObtained = false;

        if (randoCheckId == RC_CLOCK_TOWN_WEST_BANK_ADULTS_WALLET || randoCheckId == RC_CLOCK_TOWN_WEST_BANK_INTEREST ||
            randoCheckId == RC_CLOCK_TOWN_WEST_BANK_PIECE_OF_HEART) {
            continue;
        }
        switch (randoStaticCheck.flagType) {
            case FLAG_WEEK_EVENT_REG:
                // Clear the flag without triggering hook
                WEEKEVENTREG((randoStaticCheck.flag) >> 8) =
                    GET_WEEKEVENTREG((randoStaticCheck.flag) >> 8) & (u8) ~((randoStaticCheck.flag) & 0xFF);
                break;
            case FLAG_CYCL_SCENE_SWITCH:
                // Clear the flag without triggering hook
                if (gPlayState->sceneId == randoStaticCheck.sceneId) {
                    gPlayState->actorCtx.sceneFlags.switches[(randoStaticCheck.flag & ~0x1F) >> 5] &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                }
                if ((randoStaticCheck.flag & ~0x1F) >> 5 == 0) {
                    gSaveContext.save.saveInfo.permanentSceneFlags[randoStaticCheck.sceneId].switch0 &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                    gSaveContext.cycleSceneFlags[randoStaticCheck.sceneId].switch0 &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                } else if ((randoStaticCheck.flag & ~0x1F) >> 5 == 1) {
                    gSaveContext.save.saveInfo.permanentSceneFlags[randoStaticCheck.sceneId].switch1 &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                    gSaveContext.cycleSceneFlags[randoStaticCheck.sceneId].switch1 &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                }
                break;
            case FLAG_CYCL_SCENE_COLLECTIBLE:
                // Clear the flag without triggering hook
                if (gPlayState->sceneId == randoStaticCheck.sceneId) {
                    gPlayState->actorCtx.sceneFlags.collectible[(randoStaticCheck.flag & ~0x1F) >> 5] &=
                        ~(1 << (randoStaticCheck.flag & 0x1F));
                }
                gSaveContext.save.saveInfo.permanentSceneFlags[randoStaticCheck.sceneId].collectible &=
                    ~(1 << (randoStaticCheck.flag & 0x1F));
                gSaveContext.cycleSceneFlags[randoStaticCheck.sceneId].collectible &=
                    ~(1 << (randoStaticCheck.flag & 0x1F));
                break;
        }
    }

    // Re-grant initial items, in the future we may only do this if it's a refill
    if (IsRenewable(RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].randoItemId)) {
        RANDO_SAVE_CHECKS[RC_STARTING_ITEM_DEKU_MASK].eligible = true;
    }
    if (IsRenewable(RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].randoItemId)) {
        RANDO_SAVE_CHECKS[RC_STARTING_ITEM_SONG_OF_HEALING].eligible = true;
    }

    // Override a cycle reset from trying to restore your sword if you don't have one
    u8 curSword = (saveContextCopy.save.saveInfo.equips.equipment & gEquipMasks[EQUIP_TYPE_SWORD]) >>
                  gEquipShifts[EQUIP_TYPE_SWORD];
    u8 stolen1 = ((saveContextCopy.save.saveInfo.stolenItems & 0xFF000000) >> 0x18);
    u8 stolen2 = ((saveContextCopy.save.saveInfo.stolenItems & 0x00FF0000) >> 0x10);

    // Check for sword not being stolen. Rando doesn't turn sword in to the smithy, so don't need to check that
    if (curSword == EQUIP_VALUE_SWORD_NONE && !((stolen1 >= ITEM_SWORD_KOKIRI && stolen1 <= ITEM_SWORD_GILDED) ||
                                                (stolen2 >= ITEM_SWORD_KOKIRI && stolen2 <= ITEM_SWORD_GILDED))) {
        SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_NONE;
    }
}
