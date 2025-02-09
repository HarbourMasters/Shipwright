#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

// gSaveContext.inventory.items
void HandleBAInventoryItems() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.items[itemOnCRight];
}

void HandleRBAInventoryItems(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.inventory.items[itemOnCRight] = itemToPutInBottle;
}

// gSaveContext.inventory.ammo
void HandleBAInventoryAmmo() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.ammo[itemOnCRight - ITEM_FAIRY];
}

void HandleRBAInventoryAmmo(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.inventory.ammo[itemOnCRight - ITEM_FAIRY] = itemToPutInBottle;
}

// gSaveContext.inventory.equipment
void HandleBAInventoryEquipment() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_MASK_GORON) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.equipment >> 8) & 0xFF;
    } else if (itemOnCRight == ITEM_MASK_ZORA) {
        gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.equipment & 0xFF;
    }
}

void HandleRBAInventoryEquipment(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_MASK_GORON) {
        gSaveContext.inventory.equipment = (itemToPutInBottle << 8) | (gSaveContext.inventory.equipment & 0x00FF);
    } else if (itemOnCRight == ITEM_MASK_ZORA) {
        gSaveContext.inventory.equipment = itemToPutInBottle | (gSaveContext.inventory.equipment & 0xFF00);
    }
}

// gSaveContext.inventory.upgrades
void HandleBAInventoryUpgrades() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_SOLD_OUT) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.upgrades >> 24) & 0xFF;
    } else if (itemOnCRight == ITEM_POCKET_EGG) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.upgrades >> 16) & 0xFF;
    } else if (itemOnCRight == ITEM_POCKET_CUCCO) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.upgrades >> 8) & 0xFF;
    } else if (itemOnCRight == ITEM_COJIRO) {
        gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.upgrades & 0xFF;
    }
}

void HandleRBAInventoryUpgrades(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_SOLD_OUT) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 24) | (gSaveContext.inventory.upgrades & 0x00FFFFFF);
    } else if (itemOnCRight == ITEM_POCKET_EGG) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 16) | (gSaveContext.inventory.upgrades & 0xFF00FFFF);
    } else if (itemOnCRight == ITEM_POCKET_CUCCO) {
        gSaveContext.inventory.upgrades = (itemToPutInBottle << 8) | (gSaveContext.inventory.upgrades & 0xFFFF00FF);
    } else if (itemOnCRight == ITEM_COJIRO) {
        gSaveContext.inventory.upgrades = itemToPutInBottle | (gSaveContext.inventory.upgrades & 0xFFFFFF00);
    }
}

// gSaveContext.inventory.questItems
void HandleBAInventoryQuestItems() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_ODD_MUSHROOM) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.questItems >> 24) & 0xFF;
    } else if (itemOnCRight == ITEM_ODD_POTION) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.questItems >> 16) & 0xFF;
    } else if (itemOnCRight == ITEM_SAW) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.questItems >> 8) & 0xFF;
    } else if (itemOnCRight == ITEM_SWORD_BROKEN) {
        gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.questItems & 0xFF;
    }
}

void HandleRBAInventoryQuestItems(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];
    
    if (itemOnCRight == ITEM_ODD_MUSHROOM) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 24) | (gSaveContext.inventory.questItems & 0x00FFFFFF);
    } else if (itemOnCRight == ITEM_ODD_POTION) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 16) | (gSaveContext.inventory.questItems & 0xFF00FFFF);
    } else if (itemOnCRight == ITEM_SAW) {
        gSaveContext.inventory.questItems = (itemToPutInBottle << 8) | (gSaveContext.inventory.questItems & 0xFFFF00FF);
    } else if (itemOnCRight == ITEM_SWORD_BROKEN) {
        gSaveContext.inventory.questItems = itemToPutInBottle | (gSaveContext.inventory.questItems & 0xFFFFFF00);
    }
}

// gSaveContext.inventory.dungeonItems
void HandleBAInventoryDungeonItems() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.dungeonItems[itemOnCRight - ITEM_PRESCRIPTION];
}

void HandleRBAInventoryDungeonItems(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];
    
    gSaveContext.inventory.dungeonItems[itemOnCRight - ITEM_PRESCRIPTION] = itemToPutInBottle;
}

// gSaveContext.inventory.dungeonKeys
void HandleBAInventoryDungeonKeys() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.dungeonKeys[itemOnCRight - ITEM_BULLET_BAG_40];
}

void HandleRBAInventoryDungeonKeys(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];
    
    gSaveContext.inventory.dungeonKeys[itemOnCRight - ITEM_BULLET_BAG_40] = itemToPutInBottle;
}

// gSaveContext.inventory.defenseHearts
void HandleBAInventoryDefenseHearts() {
    gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.defenseHearts;
}

void HandleRBAInventoryDefenseHearts(uint8_t itemToPutInBottle) {
    gSaveContext.inventory.defenseHearts = itemToPutInBottle;
}

// gSaveContext.inventory.gsTokens
void HandleBAInventoryGSTokens() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_SONG_SERENADE) {
        gSaveContext.equips.buttonItems[0] = (gSaveContext.inventory.gsTokens >> 8) & 0xFF;
    } else if (itemOnCRight == ITEM_SONG_REQUIEM) {
        gSaveContext.equips.buttonItems[0] = gSaveContext.inventory.gsTokens & 0xFF;
    }
}

void HandleRBAInventoryGSTokens(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight == ITEM_SONG_SERENADE) {
        gSaveContext.inventory.gsTokens = (itemToPutInBottle << 8) | (gSaveContext.inventory.gsTokens & 0x00FF);
    } else if (itemOnCRight == ITEM_SONG_REQUIEM) {
        gSaveContext.inventory.gsTokens = itemToPutInBottle | (gSaveContext.inventory.gsTokens & 0xFF00);
    }
}

// gSaveContext.sceneFlags
void HandleBASceneFlags() {
    // The rest of the items fall into the saved scene flags. Let's calculate the scene and which field it pulls from
    u32 offset = gSaveContext.equips.buttonItems[3] - ITEM_SONG_LULLABY;
    u32 scene = offset / sizeof(SavedSceneFlags);
    switch (offset % sizeof(SavedSceneFlags)) {
    case 0:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].chest >> 24) & 0xFF;
        break;
    case 1:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].chest >> 16) & 0xFF;
        break;
    case 2:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].chest >> 8) & 0xFF;
        break;
    case 3:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].chest & 0xFF;
        break;
    case 4:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].swch >> 24) & 0xFF;
        break;
    case 5:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].swch >> 16) & 0xFF;
        break;
    case 6:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].swch >> 8) & 0xFF;
        break;
    case 7:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].swch & 0xFF;
        break;
    case 8:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].clear >> 24) & 0xFF;
        break;
    case 9:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].clear >> 16) & 0xFF;
        break;
    case 10:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].clear >> 8) & 0xFF;
        break;
    case 11:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].clear & 0xFF;
        break;
    case 12:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].collect >> 24) & 0xFF;
        break;
    case 13:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].collect >> 16) & 0xFF;
        break;
    case 14:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].collect >> 8) & 0xFF;
        break;
    case 15:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].collect & 0xFF;
        break;
    case 16:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].unk >> 24) & 0xFF;
        break;
    case 17:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].unk >> 16) & 0xFF;
        break;
    case 18:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].unk >> 8) & 0xFF;
        break;
    case 19:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].unk & 0xFF;
        break;
    case 20:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].rooms >> 24) & 0xFF;
        break;
    case 21:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].rooms >> 16) & 0xFF;
        break;
    case 22:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].rooms >> 8) & 0xFF;
        break;
    case 23:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].rooms & 0xFF;
        break;
    case 24:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].floors >> 24) & 0xFF;
        break;
    case 25:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].floors >> 16) & 0xFF;
        break;
    case 26:
        gSaveContext.equips.buttonItems[0] = (gSaveContext.sceneFlags[scene].floors >> 8) & 0xFF;
        break;
    case 27:
        gSaveContext.equips.buttonItems[0] = gSaveContext.sceneFlags[scene].floors & 0xFF;
        break;
    }
}

void HandleRBASceneFlags(uint8_t itemToPutInBottle) {
    // The rest of the items fall into the saved scene flags. Let's calculate the scene and which field it sets
    u32 offset = gSaveContext.equips.buttonItems[3] - ITEM_SONG_LULLABY;
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

// padding bytes
void HandleBAPaddingBytes() {
    // Reading from padding bytes is not implemented
    gSaveContext.equips.buttonItems[0] = 0;
}

void HandleRBAPaddingBytes() {
    // Writing to padding bytes is not implemented
}

// Bottle Adventure
void DoBA() {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight >= ITEM_STICK && itemOnCRight <= ITEM_POTION_BLUE) {
        HandleBAInventoryItems();
    } else if (itemOnCRight >= ITEM_FAIRY && itemOnCRight <= ITEM_MASK_BUNNY) {
        HandleBAInventoryAmmo();
    } else if (itemOnCRight == ITEM_MASK_GORON || itemOnCRight == ITEM_MASK_ZORA) {
        HandleBAInventoryEquipment();
    } else if (itemOnCRight == ITEM_MASK_GERUDO || itemOnCRight == ITEM_MASK_TRUTH) {
        HandleBAPaddingBytes();
    } else if (itemOnCRight >= ITEM_SOLD_OUT && itemOnCRight <= ITEM_COJIRO) {
        HandleBAInventoryUpgrades();
    } else if (itemOnCRight >= ITEM_ODD_MUSHROOM && itemOnCRight <= ITEM_SWORD_BROKEN) {
        HandleBAInventoryQuestItems();
    } else if (itemOnCRight >= ITEM_PRESCRIPTION && itemOnCRight <= ITEM_BULLET_BAG_30) {
        HandleBAInventoryDungeonItems();
    } else if (itemOnCRight >= ITEM_BULLET_BAG_40 && itemOnCRight <= ITEM_SWORD_KNIFE) {
        HandleBAInventoryDungeonKeys();
    } else if (itemOnCRight == ITEM_SONG_BOLERO) {
        HandleBAInventoryDefenseHearts();
    } else if (itemOnCRight == ITEM_SONG_SERENADE || itemOnCRight == ITEM_SONG_REQUIEM) {
        HandleBAInventoryGSTokens();
    } else if (itemOnCRight == ITEM_SONG_NOCTURNE || itemOnCRight == ITEM_SONG_PRELUDE) {
        HandleBAPaddingBytes();
    } else if (itemOnCRight >= ITEM_SONG_LULLABY) {
        HandleBASceneFlags();
    }
}

// Reverse Bottle Adventure
void DoRBA(uint8_t itemToPutInBottle) {
    auto itemOnCRight = gSaveContext.equips.buttonItems[3];

    if (itemOnCRight >= ITEM_STICK && itemOnCRight <= ITEM_POTION_BLUE) {
        HandleRBAInventoryItems(itemToPutInBottle);
    } else if (itemOnCRight >= ITEM_FAIRY && itemOnCRight <= ITEM_MASK_BUNNY) {
        HandleRBAInventoryAmmo(itemToPutInBottle);
    } else if (itemOnCRight == ITEM_MASK_GORON || itemOnCRight == ITEM_MASK_ZORA) {
        HandleRBAInventoryEquipment(itemToPutInBottle);
    } else if (itemOnCRight == ITEM_MASK_GERUDO || itemOnCRight == ITEM_MASK_TRUTH) {
        HandleRBAPaddingBytes();
    } else if (itemOnCRight >= ITEM_SOLD_OUT && itemOnCRight <= ITEM_COJIRO) {
        HandleRBAInventoryUpgrades(itemToPutInBottle);
    } else if (itemOnCRight >= ITEM_ODD_MUSHROOM && itemOnCRight <= ITEM_SWORD_BROKEN) {
        HandleRBAInventoryQuestItems(itemToPutInBottle);
    } else if (itemOnCRight >= ITEM_PRESCRIPTION && itemOnCRight <= ITEM_BULLET_BAG_30) {
        HandleRBAInventoryDungeonItems(itemToPutInBottle);
    } else if (itemOnCRight >= ITEM_BULLET_BAG_40 && itemOnCRight <= ITEM_SWORD_KNIFE) {
        HandleRBAInventoryDungeonKeys(itemToPutInBottle);
    } else if (itemOnCRight == ITEM_SONG_BOLERO) {
        HandleRBAInventoryDefenseHearts(itemToPutInBottle);
    } else if (itemOnCRight == ITEM_SONG_SERENADE || itemOnCRight == ITEM_SONG_REQUIEM) {
        HandleRBAInventoryGSTokens(itemToPutInBottle);
    } else if (itemOnCRight == ITEM_SONG_NOCTURNE || itemOnCRight == ITEM_SONG_PRELUDE) {
        HandleRBAPaddingBytes();
    } else if (itemOnCRight >= ITEM_SONG_LULLABY) {
        HandleRBASceneFlags(itemToPutInBottle);
    }
}

void RegisterBottleAdventure() {
    REGISTER_VB_SHOULD(VB_SET_BUTTON_ITEM_FROM_C_BUTTON_SLOT, {
        // if we aren't dealing with the b button, early return
        auto button = static_cast<int16_t>(va_arg(args, int32_t));
        if (button != 0) {
            return;
        }

        *should = false;
        DoBA();
    });

    REGISTER_VB_SHOULD(VB_UPDATE_BOTTLE_ITEM, {
        // if we aren't dealing with a bottle on b, early return
        auto buttonBottleIsOn = static_cast<uint8_t>(va_arg(args, int32_t));
        if (buttonBottleIsOn != 0) {
            return;
        }

        *should = false;

        auto itemToPutInBottle = static_cast<uint8_t>(va_arg(args, int32_t));
        DoRBA(itemToPutInBottle);
    });
}

static RegisterShipInitFunc initFunc(RegisterBottleAdventure);
