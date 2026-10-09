#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "Rando/MiscBehavior/ClockShuffle.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/ShipInit.hpp"
#include <cassert>

// Copied from z_player.c, we could instead move this to a header file, idk
typedef struct GetItemEntry {
    /* 0x0 */ u8 itemId;
    /* 0x1 */ u8 field; // various bit-packed data
    /* 0x2 */ s8 gid;   // defines the draw id and chest opening animation
    /* 0x3 */ u8 textId;
    /* 0x4 */ u16 objectId;
} GetItemEntry; // size = 0x6
#define GIFIELD_20 (1 << 5)
#define GIFIELD_40 (1 << 6)

extern "C" {
#include "variables.h"
#include "functions.h"

extern GetItemEntry sGetItemTable[GI_MAX - 1];
}

#define CAN_OWL_WARP(owlId) ((gSaveContext.save.saveInfo.playerData.owlActivationFlags >> owlId) & 1)

// ALRIGHT BUCKLE UP. This file is for converting items to their "obtainable" form. This is used for various reasons,
// and you don't always want to convert items, depending on the context. For example, if you're rendering ammo in a shop
// for an item you don't have yet, you don't want to convert it, because you want to show the player what they're
// missing. But if you're rendering a Mask they already have, you want to convert it, because they can't get it again.
//
// Progressive Items:
// - If an RC was provided, and the player has already obtained the check, the item is converted to a junk item.
// - If the player has obtained the highest level of the item, the item is converted to junk item.
//
// Keys, Fairies and Skulltulas:
// - If the settings deem this item should be persistent (NOT IMPLEMENTED YET), and the player has already obtained the
// check, the item is converted to a junk item.
//
// Ammo:
// - If the player does not have the associated item to use the ammo, the item is converted to a junk item.
//
// Refills:
// - If the player has no empty bottles, the item is converted to a junk item.
//
// Heart Pieces/Containers/One-Time Items:
// - If the player has already obtained the check, the item is converted to a junk item.
//
// Everything Else:
// - We check if the item has an associated GetItemEntry, and if it does, we check if vanilla would allow the player to
// obtain it. If not, we convert it to a junk item.
//
// Junk Items:
// - The list of junk items is defined below. Every 1.5 seconds we roll a random junk item, based on the RC provided and
// the current obtainability status of all junk items. The user can also opt out of the timed cycling.

static std::vector<RandoItemId> junkItems = {
    // Rupees
    RI_RUPEE_GREEN,
    RI_RUPEE_BLUE,
    RI_RUPEE_RED,
    RI_RUPEE_PURPLE,
    // Ammo
    RI_ARROWS_10,
    RI_BOMBCHU_5,
    RI_BOMBS_5,
    RI_DEKU_NUTS_5,
    RI_DEKU_STICKS_5,
    RI_MAGIC_JAR_SMALL,
    // Refill - Disabling for now, maybe temporarily, maybe permanently
    // RI_RED_POTION_REFILL,
    // RI_GREEN_POTION_REFILL,
    // RI_BLUE_POTION_REFILL,
    // RI_MILK_REFILL,
    // Misc
    RI_RECOVERY_HEART,
    RI_NONE,
};

static std::vector<RandoItemId> obtainableJunkItems;
static std::vector<RandoItemId> obtainableTrapItems;
static std::vector<RandoItemId> allTrapItems;

void RefreshObtainableJunkItems() {
    obtainableJunkItems.clear();

    for (RandoItemId junkItem : junkItems) {
        if (Rando::IsItemObtainable(junkItem)) {
            switch (junkItem) {
                case RI_ARROWS_10:
                    if (AMMO(ITEM_BOW) < CUR_CAPACITY(UPG_QUIVER)) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                case RI_BOMBCHU_5:
                    if (AMMO(ITEM_BOMBCHU) < CUR_CAPACITY(UPG_BOMB_BAG)) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                case RI_BOMBS_5:
                    if (AMMO(ITEM_BOMB) < CUR_CAPACITY(UPG_BOMB_BAG)) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                case RI_DEKU_NUTS_5:
                    if (AMMO(ITEM_DEKU_NUT) < CUR_CAPACITY(UPG_DEKU_NUTS)) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                case RI_DEKU_STICKS_5:
                    if (AMMO(ITEM_DEKU_STICK) < CUR_CAPACITY(UPG_DEKU_STICKS)) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                case RI_MAGIC_JAR_SMALL:
                    if (gSaveContext.save.saveInfo.playerData.magic < gSaveContext.magicCapacity) {
                        obtainableJunkItems.push_back(junkItem);
                    }
                    break;
                default:
                    obtainableJunkItems.push_back(junkItem);
                    break;
            }
        }
    }
}

void RefreshObtainableTrapItems() {
    obtainableTrapItems.clear();

    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled && !saveCheck.obtained &&
            (Rando::StaticData::Items[saveCheck.randoItemId].randoItemType == RITYPE_MAJOR ||
             Rando::StaticData::Items[saveCheck.randoItemId].randoItemType == RITYPE_MASK)) {
            obtainableTrapItems.push_back(saveCheck.randoItemId);
        }
    }
}

static RegisterShipInitFunc refreshInitFunc(
    []() {
        COND_HOOK(OnSceneInit, IS_RANDO, [](s8 sceneId, s8 spawnNum) {
            RefreshObtainableJunkItems();
            RefreshObtainableTrapItems();
        });

        allTrapItems.clear();

        if (IS_RANDO) {
            for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
                RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                if (saveCheck.shuffled &&
                    (Rando::StaticData::Items[saveCheck.randoItemId].randoItemType == RITYPE_MAJOR ||
                     Rando::StaticData::Items[saveCheck.randoItemId].randoItemType == RITYPE_MASK)) {
                    allTrapItems.push_back(saveCheck.randoItemId);
                }
            }
        }
    },
    { "IS_RANDO" });

RandoItemId Rando::CurrentJunkItem(RandoCheckId randoCheckId) {
    if (CVarGetInteger("gRando.JunkItems", 0) == 0) {
        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + randoCheckId +
                         (gPlayState->gameplayFrames / 30));
        return obtainableJunkItems[Ship_Random(0, obtainableJunkItems.size())];
    } else {
        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + randoCheckId);
        return obtainableJunkItems[Ship_Random(0, obtainableJunkItems.size())];
    }
}

RandoItemId Rando::CurrentTrapItem(RandoCheckId randoCheckId) {
    if (CVarGetInteger("gRando.TrapItems", 0) == 0) {
        if (obtainableTrapItems.size() == 0) {
            return RI_RUPEE_SILVER;
        }

        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + randoCheckId);

        return obtainableTrapItems[Ship_Random(0, obtainableTrapItems.size())];
    } else {
        if (allTrapItems.size() == 0) {
            return RI_RUPEE_SILVER;
        }

        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + randoCheckId);

        return allTrapItems[Ship_Random(0, allTrapItems.size())];
    }
}

bool Rando::IsItemObtainable(RandoItemId randoItemId, RandoCheckId randoCheckId) {
    bool hasObtainedCheck = false;
    if (randoCheckId != RC_UNKNOWN) {
        hasObtainedCheck = RANDO_SAVE_CHECKS[randoCheckId].obtained;
    }

    u8 vanillaCantObtain = false;
    if (Rando::StaticData::Items[randoItemId].itemId != ITEM_NONE &&
        Rando::StaticData::Items[randoItemId].getItemId != GI_NONE) {
        GetItemEntry* giEntry = &sGetItemTable[Rando::StaticData::Items[randoItemId].getItemId - 1];
        vanillaCantObtain =
            ((Item_CheckObtainability(giEntry->itemId) != ITEM_NONE) && (giEntry->field & GIFIELD_20)) ||
            ((Item_CheckObtainability(giEntry->itemId) == ITEM_NONE) && (giEntry->field & GIFIELD_40));
    }

    switch (randoItemId) {
        case RI_UNKNOWN:
            return false;
        case RI_PROGRESSIVE_WALLET:
            if (hasObtainedCheck) {
                return false;
            } else if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_TYCOON_WALLET] == RO_GENERIC_YES) {
                if (CUR_UPG_VALUE(UPG_WALLET) >= 3) {
                    return false;
                }
            } else if (CUR_UPG_VALUE(UPG_WALLET) >= 2) {
                return false;
            }
            return true;
        case RI_WALLET_ADULT:
            if (CUR_UPG_VALUE(UPG_WALLET) >= 1) {
                return false;
            }
            break;
        case RI_WALLET_GIANT:
            if (CUR_UPG_VALUE(UPG_WALLET) >= 2) {
                return false;
            }
            break;
        case RI_WALLET_TYCOON:
            if (CUR_UPG_VALUE(UPG_WALLET) >= 3) {
                return false;
            }
            break;
        case RI_PROGRESSIVE_SWORD:
            if (hasObtainedCheck) {
                return false;
            } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_GILDED ||
                       (STOLEN_ITEM_1 >= ITEM_SWORD_GILDED) || (STOLEN_ITEM_2 >= ITEM_SWORD_GILDED)) {
                return false;
            }
            return true;
        case RI_SWORD_KOKIRI:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI ||
                (STOLEN_ITEM_1 >= ITEM_SWORD_KOKIRI) || (STOLEN_ITEM_2 >= ITEM_SWORD_KOKIRI)) {
                return false;
            }
            break;
        case RI_SWORD_RAZOR:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_RAZOR ||
                (STOLEN_ITEM_1 >= ITEM_SWORD_RAZOR) || (STOLEN_ITEM_2 >= ITEM_SWORD_RAZOR)) {
                return false;
            }
            break;
        case RI_SWORD_GILDED:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_GILDED ||
                (STOLEN_ITEM_1 >= ITEM_SWORD_GILDED) || (STOLEN_ITEM_2 >= ITEM_SWORD_GILDED)) {
                return false;
            }
            break;
        case RI_PROGRESSIVE_BOMB_BAG:
            if (hasObtainedCheck) {
                return false;
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                return false;
            }
            return true;
        case RI_BOMB_BAG_20:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 1) {
                return false;
            }
            break;
        case RI_BOMB_BAG_30:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 2) {
                return false;
            }
            break;
        case RI_BOMB_BAG_40:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                return false;
            }
            break;
        case RI_PROGRESSIVE_BOW:
            if (hasObtainedCheck) {
                return false;
            } else if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                return false;
            }
            return true;
        case RI_BOW:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 1) {
                return false;
            }
            break;
        case RI_QUIVER_40:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 2) {
                return false;
            }
            break;
        case RI_QUIVER_50:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                return false;
            }
            break;
        case RI_PROGRESSIVE_LULLABY:
            if (hasObtainedCheck) {
                return false;
            } else if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO) && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                return false;
            }
            return true;
        case RI_PROGRESSIVE_MAGIC:
            if (hasObtainedCheck) {
                return false;
            } else if (gSaveContext.save.saveInfo.playerData.isMagicAcquired &&
                       gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                return false;
            }
            return true;
        case RI_DOUBLE_MAGIC:
            return !gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired;
        case RI_SINGLE_MAGIC:
            return !gSaveContext.save.saveInfo.playerData.isMagicAcquired;
        case RI_MAGIC_JAR_SMALL:
        case RI_MAGIC_JAR_BIG:
            return gSaveContext.save.saveInfo.playerData.isMagicAcquired;
        // TODO: These should be based on a persistent setting
        case RI_WOODFALL_SMALL_KEY:
        case RI_SNOWHEAD_SMALL_KEY:
        case RI_GREAT_BAY_SMALL_KEY:
        case RI_STONE_TOWER_SMALL_KEY:
        case RI_WOODFALL_STRAY_FAIRY:
        case RI_SNOWHEAD_STRAY_FAIRY:
        case RI_GREAT_BAY_STRAY_FAIRY:
        case RI_STONE_TOWER_STRAY_FAIRY:
        case RI_GS_TOKEN_SWAMP:
        case RI_GS_TOKEN_OCEAN:
        case RI_TRIFORCE_PIECE:
            if (hasObtainedCheck) {
                return false;
            }
            return true;
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80);
        case RI_HEART_PIECE:
        case RI_HEART_CONTAINER:
            if (hasObtainedCheck) {
                return false;
            }
            break;
        case RI_GOLD_DUST_REFILL:
        case RI_MILK_REFILL:
        case RI_CHATEAU_ROMANI_REFILL:
        case RI_FAIRY_REFILL:
        case RI_RED_POTION_REFILL:
        case RI_BLUE_POTION_REFILL:
        case RI_GREEN_POTION_REFILL:
            if (!Inventory_HasEmptyBottle()) {
                return false;
            }
            break;
        case RI_ARROWS_10:
        case RI_ARROWS_30:
        case RI_ARROWS_50:
            if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                return false;
            }
            break;
        case RI_BOMBCHU:
        case RI_BOMBCHU_5:
        case RI_BOMBCHU_10:
        case RI_BOMBS_5:
        case RI_BOMBS_10:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                return false;
            }
            break;
        case RI_SHIELD_HERO:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
                return false;
            }
            break;
        case RI_SHIELD_MIRROR:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_MIRROR) {
                return false;
            }
            break;
        case RI_BOTTLE_EMPTY:
        case RI_BOTTLE_CHATEAU_ROMANI:
        case RI_BOTTLE_MILK:
        case RI_BOTTLE_GOLD_DUST:
        case RI_BOTTLE_RED_POTION:
            if (hasObtainedCheck) {
                return false;
            }

            for (s32 slot = SLOT_BOTTLE_1; slot <= SLOT_BOTTLE_6; slot++) {
                if (gSaveContext.save.saveInfo.inventory.items[slot] == ITEM_NONE) {
                    return true;
                }
            }
            return false;
        case RI_MOONS_TEAR:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR) && INV_CONTENT(ITEM_MOONS_TEAR) != ITEM_NONE);
        case RI_DEED_LAND:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_LAND) && INV_CONTENT(ITEM_DEED_LAND) != ITEM_NONE);
        case RI_DEED_SWAMP:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP) && INV_CONTENT(ITEM_DEED_SWAMP) != ITEM_NONE);
        case RI_DEED_MOUNTAIN:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN) &&
                     INV_CONTENT(ITEM_DEED_MOUNTAIN) != ITEM_NONE);
        case RI_DEED_OCEAN:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN) && INV_CONTENT(ITEM_DEED_OCEAN) != ITEM_NONE);
        case RI_ROOM_KEY:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY) && INV_CONTENT(ITEM_ROOM_KEY) != ITEM_NONE);
        case RI_LETTER_TO_MAMA:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA) &&
                     INV_CONTENT(ITEM_LETTER_MAMA) != ITEM_NONE);
        case RI_LETTER_TO_KAFEI:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI) &&
                     INV_CONTENT(ITEM_LETTER_TO_KAFEI) != ITEM_NONE);
        case RI_PENDANT_OF_MEMORIES:
            return !(Flags_GetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES) &&
                     INV_CONTENT(ITEM_PENDANT_OF_MEMORIES) != ITEM_NONE);
        case RI_DOUBLE_DEFENSE:
            return !gSaveContext.save.saveInfo.playerData.doubleDefense;
        case RI_GREAT_SPIN_ATTACK:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
        case RI_WOODFALL_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
        case RI_WOODFALL_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
        case RI_WOODFALL_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
        case RI_SNOWHEAD_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
        case RI_SNOWHEAD_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
        case RI_SNOWHEAD_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
        case RI_GREAT_BAY_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
        case RI_GREAT_BAY_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
        case RI_GREAT_BAY_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
        case RI_STONE_TOWER_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
        case RI_STONE_TOWER_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
        case RI_STONE_TOWER_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
        case RI_OWL_CLOCK_TOWN_SOUTH:
            return !CAN_OWL_WARP(OWL_WARP_CLOCK_TOWN);
        case RI_OWL_GREAT_BAY_COAST:
            return !CAN_OWL_WARP(OWL_WARP_GREAT_BAY_COAST);
        case RI_OWL_IKANA_CANYON:
            return !CAN_OWL_WARP(OWL_WARP_IKANA_CANYON);
        case RI_OWL_MILK_ROAD:
            return !CAN_OWL_WARP(OWL_WARP_MILK_ROAD);
        case RI_OWL_MOUNTAIN_VILLAGE:
            return !CAN_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE);
        case RI_OWL_SNOWHEAD:
            return !CAN_OWL_WARP(OWL_WARP_SNOWHEAD);
        case RI_OWL_SOUTHERN_SWAMP:
            return !CAN_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP);
        case RI_OWL_STONE_TOWER:
            return !CAN_OWL_WARP(OWL_WARP_STONE_TOWER);
        case RI_OWL_WOODFALL:
            return !CAN_OWL_WARP(OWL_WARP_WOODFALL);
        case RI_OWL_ZORA_CAPE:
            return !CAN_OWL_WARP(OWL_WARP_ZORA_CAPE);
        // These items are technically fine to receive again because they don't do anything, but we'll convert them to
        // ensure it's clear to the player something didn't go wrong.
        // Quest Items
        case RI_BOMBERS_NOTEBOOK:
            return !CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK);
        case RI_REMAINS_ODOLWA:
            return !CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA);
        case RI_REMAINS_GOHT:
            return !CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT);
        case RI_REMAINS_GYORG:
            return !CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG);
        case RI_REMAINS_TWINMOLD:
            return !CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD);
        case RI_SONG_DOUBLE_TIME:
            return !Flags_GetRandoInf(RANDO_INF_OBTAINED_SONG_DOUBLE_TIME);
        case RI_SONG_NOVA:
            return !CHECK_QUEST_ITEM(QUEST_SONG_BOSSA_NOVA);
        case RI_SONG_ELEGY:
            return !CHECK_QUEST_ITEM(QUEST_SONG_ELEGY);
        case RI_SONG_EPONA:
            return !CHECK_QUEST_ITEM(QUEST_SONG_EPONA);
        case RI_SONG_HEALING:
            return !CHECK_QUEST_ITEM(QUEST_SONG_HEALING);
        case RI_SONG_INVERTED_TIME:
            return !Flags_GetRandoInf(RANDO_INF_OBTAINED_SONG_INVERTED_TIME);
        case RI_SONG_LULLABY_INTRO:
            return !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
        case RI_SONG_LULLABY:
            return !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY);
        case RI_SONG_OATH:
            return !CHECK_QUEST_ITEM(QUEST_SONG_OATH);
        case RI_SONG_SARIA:
            if (hasObtainedCheck) {
                return false;
            }
            return true;
        case RI_SONG_SOARING:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SOARING);
        case RI_SONG_SONATA:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SONATA);
        case RI_SONG_STORMS:
            return !CHECK_QUEST_ITEM(QUEST_SONG_STORMS);
        case RI_SONG_SUN:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SUN);
        case RI_SONG_TIME:
            return !CHECK_QUEST_ITEM(QUEST_SONG_TIME);
        case RI_TINGLE_MAP_CLOCK_TOWN:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
        case RI_TINGLE_MAP_WOODFALL:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
        case RI_TINGLE_MAP_GREAT_BAY:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
        case RI_TINGLE_MAP_ROMANI_RANCH:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
        case RI_TINGLE_MAP_SNOWHEAD:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
        case RI_TINGLE_MAP_STONE_TOWER:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
        case RI_SOUL_BOSS_GOHT:
        case RI_SOUL_BOSS_GYORG:
        case RI_SOUL_BOSS_MAJORA:
        case RI_SOUL_BOSS_ODOLWA:
        case RI_SOUL_BOSS_TWINMOLD:
        case RI_SOUL_ENEMY_ALIEN:
        case RI_SOUL_ENEMY_ARMOS:
        case RI_SOUL_ENEMY_BAD_BAT:
        case RI_SOUL_ENEMY_BEAMOS:
        case RI_SOUL_ENEMY_BOE:
        case RI_SOUL_ENEMY_BUBBLE:
        case RI_SOUL_ENEMY_CAPTAIN_KEETA:
        case RI_SOUL_ENEMY_CHUCHU:
        case RI_SOUL_ENEMY_DEATH_ARMOS:
        case RI_SOUL_ENEMY_DEEP_PYTHON:
        case RI_SOUL_ENEMY_DEKU_BABA:
        case RI_SOUL_ENEMY_DEXIHAND:
        case RI_SOUL_ENEMY_DINOLFOS:
        case RI_SOUL_ENEMY_DODONGO:
        case RI_SOUL_ENEMY_DRAGONFLY:
        case RI_SOUL_ENEMY_EENO:
        case RI_SOUL_ENEMY_EYEGORE:
        case RI_SOUL_ENEMY_FREEZARD:
        case RI_SOUL_ENEMY_GARO:
        case RI_SOUL_ENEMY_GEKKO:
        case RI_SOUL_ENEMY_GIANT_BEE:
        case RI_SOUL_ENEMY_GOMESS:
        case RI_SOUL_ENEMY_GUAY:
        case RI_SOUL_ENEMY_HIPLOOP:
        case RI_SOUL_ENEMY_IGOS_DU_IKANA:
        case RI_SOUL_ENEMY_IRON_KNUCKLE:
        case RI_SOUL_ENEMY_KEESE:
        case RI_SOUL_ENEMY_LEEVER:
        case RI_SOUL_ENEMY_LIKE_LIKE:
        case RI_SOUL_ENEMY_MAD_SCRUB:
        case RI_SOUL_ENEMY_NEJIRON:
        case RI_SOUL_ENEMY_OCTOROK:
        case RI_SOUL_ENEMY_PEAHAT:
        case RI_SOUL_ENEMY_PIRATE:
        case RI_SOUL_ENEMY_POE:
        case RI_SOUL_ENEMY_REDEAD:
        case RI_SOUL_ENEMY_SHELLBLADE:
        case RI_SOUL_ENEMY_SKULLFISH:
        case RI_SOUL_ENEMY_SKULLTULA:
        case RI_SOUL_ENEMY_SNAPPER:
        case RI_SOUL_ENEMY_STALCHILD:
        case RI_SOUL_ENEMY_TAKKURI:
        case RI_SOUL_ENEMY_TEKTITE:
        case RI_SOUL_ENEMY_WALLMASTER:
        case RI_SOUL_ENEMY_WART:
        case RI_SOUL_ENEMY_WIZROBE:
        case RI_SOUL_ENEMY_WOLFOS:
            return !Flags_GetRandoInf(SOUL_RI_TO_RANDO_INF(randoItemId));
        case RI_ABILITY_SWIM:
            return !Flags_GetRandoInf(RANDO_INF_OBTAINED_SWIM);
        case RI_FROG_BLUE:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_33_01);
        case RI_FROG_CYAN:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_32_40);
        case RI_FROG_PINK:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_32_80);
        case RI_FROG_WHITE:
            return !CHECK_WEEKEVENTREG(WEEKEVENTREG_33_02);
        case RI_TIME_DAY_1:
        case RI_TIME_NIGHT_1:
        case RI_TIME_DAY_2:
        case RI_TIME_NIGHT_2:
        case RI_TIME_DAY_3:
        case RI_TIME_NIGHT_3:
            return !Flags_GetRandoInf(RANDO_INF_OBTAINED_CLOCK_DAY_1 +
                                      Rando::ClockItems::GetHalfDayIndexFromClockItem(randoItemId));
        case RI_TIME_PROGRESSIVE:
            if (hasObtainedCheck) {
                return false;
            } else if (Rando::ClockItems::GetAllOwnedHalfDaysMask() == 0x3F) { // Have all 6 half days
                return false;
            }
            return true;
        case RI_OCARINA_BUTTON_A:
        case RI_OCARINA_BUTTON_C_DOWN:
        case RI_OCARINA_BUTTON_C_LEFT:
        case RI_OCARINA_BUTTON_C_RIGHT:
        case RI_OCARINA_BUTTON_C_UP:
            return !Flags_GetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A + (randoItemId - RI_OCARINA_BUTTON_A));
        // These items are technically fine to receive again because they don't do anything, but we'll convert them to
        // ensure it's clear to the player something didn't go wrong. We just simply check the inventory state
        // Masks
        case RI_MASK_ALL_NIGHT:
        case RI_MASK_BLAST:
        case RI_MASK_BREMEN:
        case RI_MASK_BUNNY:
        case RI_MASK_CAPTAIN:
        case RI_MASK_CIRCUS_LEADER:
        case RI_MASK_COUPLE:
        case RI_MASK_DEKU:
        case RI_MASK_DON_GERO:
        case RI_MASK_FIERCE_DEITY:
        case RI_MASK_GARO:
        case RI_MASK_GIANT:
        case RI_MASK_GIBDO:
        case RI_MASK_GORON:
        case RI_MASK_GREAT_FAIRY:
        case RI_MASK_KAFEIS_MASK:
        case RI_MASK_KAMARO:
        case RI_MASK_KEATON:
        case RI_MASK_POSTMAN:
        case RI_MASK_ROMANI:
        case RI_MASK_SCENTS:
        case RI_MASK_STONE:
        case RI_MASK_TRUTH:
        case RI_MASK_ZORA: {
            ItemId itemId = StaticData::Items[randoItemId].itemId;
            return INV_CONTENT(itemId) != itemId;
        }
        default:
            break;
    }

    if (vanillaCantObtain) {
        return false;
    }

    return true;
}

RandoItemId Rando::ConvertItem(RandoItemId randoItemId, RandoCheckId randoCheckId) {
    if (IsItemObtainable(randoItemId, randoCheckId)) {
        switch (randoItemId) {
            case RI_TIME_PROGRESSIVE: {
                // Choose the next clock according to mode and current owned half-days
                int mode = RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE_PROGRESSIVE];

                if (mode == RO_CLOCK_SHUFFLE_RANDOM) {
                    // Random mode should never have progressive items
                    return RI_JUNK;
                }

                // Build list in target order
                RandoItemId ascending[] = { RI_TIME_DAY_1,   RI_TIME_NIGHT_1, RI_TIME_DAY_2,
                                            RI_TIME_NIGHT_2, RI_TIME_DAY_3,   RI_TIME_NIGHT_3 };
                RandoItemId descending[] = { RI_TIME_NIGHT_3, RI_TIME_DAY_3,   RI_TIME_NIGHT_2,
                                             RI_TIME_DAY_2,   RI_TIME_NIGHT_1, RI_TIME_DAY_1 };
                RandoItemId* order = (mode == RO_CLOCK_SHUFFLE_DESCENDING) ? descending : ascending;
                for (int i = 0; i < 6; ++i) {
                    int halfIndex = Rando::ClockItems::GetHalfDayIndexFromClockItem(order[i]);
                    if (halfIndex >= 0 &&
                        !Flags_GetRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + halfIndex))) {
                        return order[i];
                    }
                }
                // All owned; degrade to junk
                return RI_JUNK;
            }
            case RI_PROGRESSIVE_BOMB_BAG:
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                    return RI_BOMB_BAG_20;
                } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 1) {
                    return RI_BOMB_BAG_30;
                } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 2) {
                    return RI_BOMB_BAG_40;
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            case RI_PROGRESSIVE_BOW:
                if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                    return RI_BOW;
                } else if (CUR_UPG_VALUE(UPG_QUIVER) == 1) {
                    return RI_QUIVER_40;
                } else if (CUR_UPG_VALUE(UPG_QUIVER) == 2) {
                    return RI_QUIVER_50;
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            case RI_PROGRESSIVE_LULLABY:
                if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
                    return RI_SONG_LULLABY_INTRO;
                } else {
                    if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                        return RI_SONG_LULLABY;
                    }
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            case RI_PROGRESSIVE_MAGIC:
                if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                    return RI_SINGLE_MAGIC;
                } else if (!gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                    return RI_DOUBLE_MAGIC;
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            case RI_PROGRESSIVE_WALLET:
                if (CUR_UPG_VALUE(UPG_WALLET) == 0) {
                    return RI_WALLET_ADULT;
                } else if (CUR_UPG_VALUE(UPG_WALLET) == 1) {
                    return RI_WALLET_GIANT;
                } else if (CUR_UPG_VALUE(UPG_WALLET) == 2) {
                    return RI_WALLET_TYCOON;
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            case RI_PROGRESSIVE_SWORD:
                if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE &&
                    (STOLEN_ITEM_1 < ITEM_SWORD_KOKIRI) && (STOLEN_ITEM_2 < ITEM_SWORD_KOKIRI)) {
                    return RI_SWORD_KOKIRI;
                } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_KOKIRI ||
                           (STOLEN_ITEM_1 == ITEM_SWORD_KOKIRI) || (STOLEN_ITEM_2 == ITEM_SWORD_KOKIRI)) {
                    return RI_SWORD_RAZOR;
                } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_RAZOR ||
                           (STOLEN_ITEM_1 == ITEM_SWORD_RAZOR) || (STOLEN_ITEM_2 == ITEM_SWORD_RAZOR)) {
                    return RI_SWORD_GILDED;
                }
                // Shouldn't happen, just in case
                assert(false);
                return RI_JUNK;
            default:
                break;
        }

        return randoItemId;
    } else {
        switch (randoItemId) {
            case RI_BOTTLE_GOLD_DUST:
                if (Inventory_HasEmptyBottle()) {
                    return RI_GOLD_DUST_REFILL;
                }
                break;
            case RI_BOTTLE_MILK:
                if (Inventory_HasEmptyBottle()) {
                    return RI_MILK_REFILL;
                }
                break;
            case RI_BOTTLE_CHATEAU_ROMANI:
                if (Inventory_HasEmptyBottle()) {
                    return RI_CHATEAU_ROMANI_REFILL;
                }
                break;
            case RI_BOTTLE_RED_POTION:
                if (Inventory_HasEmptyBottle()) {
                    return RI_RED_POTION_REFILL;
                }
            default:
                break;
        }

        return RI_JUNK;
    }
}
