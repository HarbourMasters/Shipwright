#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

void DoRBA(uint8_t itemOnCRight, uint8_t itemToPutInBottle) {
    if (itemOnCRight >= ITEM_STICK && itemOnCRight <= ITEM_POTION_BLUE) {
        gSaveContext.inventory.items[itemOnCRight] = itemToPutInBottle;
    } else if (itemOnCRight >= ITEM_FAIRY && itemOnCRight <= ITEM_MASK_BUNNY) {
        gSaveContext.inventory.ammo[itemOnCRight - ITEM_FAIRY] = itemToPutInBottle;
    } else if (itemOnCRight == ITEM_MASK_GORON) {
        gSaveContext.inventory.equipment = (itemToPutInBottle << 8) | (gSaveContext.inventory.equipment & 0x00FF);
    } else if (itemOnCRight == ITEM_MASK_ZORA) {
        gSaveContext.inventory.equipment = itemToPutInBottle | (gSaveContext.inventory.equipment & 0xFF00);
    }
    // ITEM_MASK_GERUDO and ITEM_MASK_TRUTH land in padding bytes
    else if (itemOnCRight == ITEM_SOLD_OUT) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 24) | (gSaveContext.inventory.upgrades & 0x00FFFFFF);
    } else if (itemOnCRight == ITEM_POCKET_EGG) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 16) | (gSaveContext.inventory.upgrades & 0xFF00FFFF);
    } else if (itemOnCRight == ITEM_POCKET_CUCCO) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 8) | (gSaveContext.inventory.upgrades & 0xFFFF00FF);
    } else if (itemOnCRight == ITEM_COJIRO) {
        gSaveContext.inventory.upgrades = itemToPutInBottle | (gSaveContext.inventory.upgrades & 0xFFFFFF00);
    } else if (itemOnCRight == ITEM_ODD_MUSHROOM) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 24) | (gSaveContext.inventory.questItems & 0x00FFFFFF);
    } else if (itemOnCRight == ITEM_ODD_POTION) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 16) | (gSaveContext.inventory.questItems & 0xFF00FFFF);
    } else if (itemOnCRight == ITEM_SAW) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 8) | (gSaveContext.inventory.questItems & 0xFFFF00FF);
    } else if (itemOnCRight == ITEM_SWORD_BROKEN) {
        gSaveContext.inventory.questItems = itemToPutInBottle | (gSaveContext.inventory.questItems & 0xFFFFFF00);
    } else if (itemOnCRight >= ITEM_PRESCRIPTION && itemOnCRight <= ITEM_BULLET_BAG_30) {
        gSaveContext.inventory.dungeonItems[itemOnCRight - ITEM_PRESCRIPTION] = itemToPutInBottle;
    } else if (itemOnCRight >= ITEM_BULLET_BAG_40 && itemOnCRight <= ITEM_SWORD_KNIFE) {
        gSaveContext.inventory.dungeonKeys[itemOnCRight - ITEM_BULLET_BAG_40] = itemToPutInBottle;
    } else if (itemOnCRight == ITEM_SONG_BOLERO) {
        gSaveContext.inventory.defenseHearts = itemToPutInBottle;
    } else if (itemOnCRight == ITEM_SONG_SERENADE) {
        gSaveContext.inventory.gsTokens = (itemToPutInBottle << 8) | (gSaveContext.inventory.gsTokens & 0x00FF);
    } else if (itemOnCRight == ITEM_SONG_REQUIEM) {
        gSaveContext.inventory.gsTokens = itemToPutInBottle | (gSaveContext.inventory.gsTokens & 0xFF00);
    }
    // ITEM_SONG_NOCTURNE and ITEM_SONG_PRELUDE land in padding bytes
    else if (itemOnCRight >= ITEM_SONG_LULLABY) {
        // The rest of the items fall into the saved scene flags. Let's calculate the scene and which field it sets
        u32 offset = itemOnCRight - ITEM_SONG_LULLABY;
        u32 scene = offset / sizeof(SavedSceneFlags);
        switch (offset % sizeof(SavedSceneFlags)) {
        case 0:
            gSaveContext.sceneFlags[scene].chest = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].chest & 0x00FFFFFF);
            break;
        case 1:
            gSaveContext.sceneFlags[scene].chest = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].chest & 0xFF00FFFF);
            break;
        case 2:
            gSaveContext.sceneFlags[scene].chest = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].chest & 0xFFFF00FF);
            break;
        case 3:
            gSaveContext.sceneFlags[scene].chest = itemToPutInBottle | (gSaveContext.sceneFlags[scene].chest & 0xFFFFFF00);
            break;
        case 4:
            gSaveContext.sceneFlags[scene].swch = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].swch & 0x00FFFFFF);
            break;
        case 5:
            gSaveContext.sceneFlags[scene].swch = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].swch & 0xFF00FFFF);
            break;
        case 6:
            gSaveContext.sceneFlags[scene].swch = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].swch & 0xFFFF00FF);
            break;
        case 7:
            gSaveContext.sceneFlags[scene].swch = itemToPutInBottle | (gSaveContext.sceneFlags[scene].swch & 0xFFFFFF00);
            break;
        case 8:
            gSaveContext.sceneFlags[scene].clear = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].clear & 0x00FFFFFF);
            break;
        case 9:
            gSaveContext.sceneFlags[scene].clear = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].clear & 0xFF00FFFF);
            break;
        case 10:
            gSaveContext.sceneFlags[scene].clear = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].clear & 0xFFFF00FF);
            break;
        case 11:
            gSaveContext.sceneFlags[scene].clear = itemToPutInBottle | (gSaveContext.sceneFlags[scene].clear & 0xFFFFFF00);
            break;
        case 12:
            gSaveContext.sceneFlags[scene].collect = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].collect & 0x00FFFFFF);
            break;
        case 13:
            gSaveContext.sceneFlags[scene].collect = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].collect & 0xFF00FFFF);
            break;
        case 14:
            gSaveContext.sceneFlags[scene].collect = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].collect & 0xFFFF00FF);
            break;
        case 15:
            gSaveContext.sceneFlags[scene].collect = itemToPutInBottle | (gSaveContext.sceneFlags[scene].collect & 0xFFFFFF00);
            break;
        case 16:
            gSaveContext.sceneFlags[scene].unk = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].unk & 0x00FFFFFF);
            break;
        case 17:
            gSaveContext.sceneFlags[scene].unk = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].unk & 0xFF00FFFF);
            break;
        case 18:
            gSaveContext.sceneFlags[scene].unk = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].unk & 0xFFFF00FF);
            break;
        case 19:
            gSaveContext.sceneFlags[scene].unk = itemToPutInBottle | (gSaveContext.sceneFlags[scene].unk & 0xFFFFFF00);
            break;
        case 20:
            gSaveContext.sceneFlags[scene].rooms = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].rooms & 0x00FFFFFF);
            break;
        case 21:
            gSaveContext.sceneFlags[scene].rooms = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].rooms & 0xFF00FFFF);
            break;
        case 22:
            gSaveContext.sceneFlags[scene].rooms = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].rooms & 0xFFFF00FF);
            break;
        case 23:
            gSaveContext.sceneFlags[scene].rooms = itemToPutInBottle | (gSaveContext.sceneFlags[scene].rooms & 0xFFFFFF00);
            break;
        case 24:
            gSaveContext.sceneFlags[scene].floors = (itemToPutInBottle << 24) | (gSaveContext.sceneFlags[scene].floors & 0x00FFFFFF);
            break;
        case 25:
            gSaveContext.sceneFlags[scene].floors = (itemToPutInBottle << 16) | (gSaveContext.sceneFlags[scene].floors & 0xFF00FFFF);
            break;
        case 26:
            gSaveContext.sceneFlags[scene].floors = (itemToPutInBottle << 8) | (gSaveContext.sceneFlags[scene].floors & 0xFFFF00FF);
            break;
        case 27:
            gSaveContext.sceneFlags[scene].floors = itemToPutInBottle | (gSaveContext.sceneFlags[scene].floors & 0xFFFFFF00);
            break;
        }
    }
}

void RegisterBottleAdventure() {
    REGISTER_VB_SHOULD(VB_UPDATE_BOTTLE_ITEM, {
        // vanilla condition is button != 0, meaning we use the vanilla
        // logic to update the bottle item when the bottle isn't on b
        // 
        // if that's the case (should is true), we don't want to perform RBA
        // so we early return
        if (*should) {
            return;
        }

        auto itemToPutInBottle = static_cast<uint8_t>(va_arg(args, int32_t));
        auto itemOnCRight = gSaveContext.equips.buttonItems[3];

        DoRBA(itemOnCRight, itemToPutInBottle);
    });
}

static RegisterShipInitFunc initFunc(RegisterBottleAdventure);
