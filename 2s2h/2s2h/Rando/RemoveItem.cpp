#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "Rando/MiscBehavior/ClockShuffle.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

#define CLEAR_OWL_WARP(owlId) (gSaveContext.save.saveInfo.playerData.owlActivationFlags &= ~(1 << owlId))

void Rando::RemoveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]--;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]--;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]--;
            break;
        case RI_GREAT_SPIN_ATTACK:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = false;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 0;
            break;
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = false;
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = 0;
            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_12_80);
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = false;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            break;
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_MAP:
        case RI_WOODFALL_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]--;
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_MAP:
        case RI_SNOWHEAD_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]--;
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_MAP:
        case RI_GREAT_BAY_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]--;
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_MAP:
        case RI_STONE_TOWER_COMPASS:
            REMOVE_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                                DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE)--;
            gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]--;
            break;
        case RI_PROGRESSIVE_MAGIC:
            if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                RemoveItem(RI_DOUBLE_MAGIC);
            } else if (gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                RemoveItem(RI_SINGLE_MAGIC);
            }
            break;
        case RI_BOW:
            Inventory_ChangeUpgrade(UPG_QUIVER, 0);
            Inventory_DeleteItem(ITEM_BOW, SLOT(ITEM_BOW));
            break;
        case RI_QUIVER_40:
            Inventory_ChangeUpgrade(UPG_QUIVER, 1);
            break;
        case RI_QUIVER_50:
            Inventory_ChangeUpgrade(UPG_QUIVER, 2);
            break;
        case RI_PROGRESSIVE_BOW:
            if (CUR_UPG_VALUE(UPG_QUIVER) >= 3) {
                RemoveItem(RI_QUIVER_50);
            } else if (CUR_UPG_VALUE(UPG_QUIVER) >= 2) {
                RemoveItem(RI_QUIVER_40);
            } else if (CUR_UPG_VALUE(UPG_QUIVER) >= 1) {
                RemoveItem(RI_BOW);
            }
            break;
        case RI_BOMB_BAG_20:
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 0);
            Inventory_DeleteItem(ITEM_BOMB, SLOT(ITEM_BOMB));
            Inventory_DeleteItem(ITEM_BOMBCHU, SLOT(ITEM_BOMBCHU));
            break;
        case RI_BOMB_BAG_30:
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 1);
            break;
        case RI_BOMB_BAG_40:
            Inventory_ChangeUpgrade(UPG_BOMB_BAG, 2);
            break;
        case RI_PROGRESSIVE_BOMB_BAG:
            if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 3) {
                RemoveItem(RI_BOMB_BAG_40);
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 2) {
                RemoveItem(RI_BOMB_BAG_30);
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) >= 1) {
                RemoveItem(RI_BOMB_BAG_20);
            }
            break;
        case RI_WALLET_ADULT:
            Inventory_ChangeUpgrade(UPG_WALLET, 0);
            break;
        case RI_WALLET_GIANT:
            Inventory_ChangeUpgrade(UPG_WALLET, 1);
            break;
        case RI_WALLET_TYCOON:
            Inventory_ChangeUpgrade(UPG_WALLET, 2);
            break;
        case RI_PROGRESSIVE_WALLET:
            if (CUR_UPG_VALUE(UPG_WALLET) >= 3) {
                RemoveItem(RI_WALLET_TYCOON);
            } else if (CUR_UPG_VALUE(UPG_WALLET) >= 2) {
                RemoveItem(RI_WALLET_GIANT);
            } else if (CUR_UPG_VALUE(UPG_WALLET) >= 1) {
                RemoveItem(RI_WALLET_ADULT);
            }
            break;
        case RI_SWORD_KOKIRI:
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_NONE;
            if (gPlayState != NULL) {
                Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
            }
            break;
        case RI_SWORD_RAZOR:
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI;
            if (gPlayState != NULL) {
                Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
            }
            break;
        case RI_SWORD_GILDED:
            SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_RAZOR);
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_RAZOR;
            if (gPlayState != NULL) {
                Interface_LoadItemIconImpl(gPlayState, EQUIP_SLOT_B);
            }
            break;
        case RI_PROGRESSIVE_SWORD:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_GILDED) {
                RemoveItem(RI_SWORD_GILDED);
            } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_RAZOR) {
                RemoveItem(RI_SWORD_RAZOR);
            } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) >= EQUIP_VALUE_SWORD_KOKIRI) {
                RemoveItem(RI_SWORD_KOKIRI);
            }
            break;
        case RI_PROGRESSIVE_LULLABY:
            if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                RemoveItem(RI_SONG_LULLABY);
            } else if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
                RemoveItem(RI_SONG_LULLABY_INTRO);
            }
            break;
        case RI_SHIELD_MIRROR:
            SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_HERO);
            if (gPlayState != NULL) {
                Player_SetEquipmentData(gPlayState, GET_PLAYER(gPlayState));
            }
            break;
        case RI_SHIELD_HERO:
            if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_HERO) {
                SET_EQUIP_VALUE(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_NONE);
            }
            if (gPlayState != NULL) {
                Player_SetEquipmentData(gPlayState, GET_PLAYER(gPlayState));
            }
            break;
        case RI_GS_TOKEN_SWAMP: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINSTA1);
            skullTokenCount--;
            gSaveContext.save.saveInfo.skullTokenCount =
                ((int)(skullTokenCount & 0xFFFF) << 0x10) | (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF);
            break;
        }
        case RI_GS_TOKEN_OCEAN: {
            int skullTokenCount = Inventory_GetSkullTokenCount(SCENE_KINDAN2);
            skullTokenCount--;
            gSaveContext.save.saveInfo.skullTokenCount =
                (gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF0000) | (skullTokenCount & 0xFFFF);
            break;
        }
        case RI_MOONS_TEAR:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
            break;
        case RI_DEED_LAND:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
            break;
        case RI_DEED_SWAMP:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
            break;
        case RI_DEED_MOUNTAIN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
            break;
        case RI_DEED_OCEAN:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
            break;
        case RI_ROOM_KEY:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
            break;
        case RI_LETTER_TO_MAMA:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
            break;
        case RI_LETTER_TO_KAFEI:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
            break;
        case RI_PENDANT_OF_MEMORIES:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
            break;
        case RI_OWL_CLOCK_TOWN_SOUTH:
            CLEAR_OWL_WARP(OWL_WARP_CLOCK_TOWN);
            break;
        case RI_OWL_GREAT_BAY_COAST:
            CLEAR_OWL_WARP(OWL_WARP_GREAT_BAY_COAST);
            break;
        case RI_OWL_IKANA_CANYON:
            CLEAR_OWL_WARP(OWL_WARP_IKANA_CANYON);
            break;
        case RI_OWL_MILK_ROAD:
            CLEAR_OWL_WARP(OWL_WARP_MILK_ROAD);
            break;
        case RI_OWL_MOUNTAIN_VILLAGE:
            CLEAR_OWL_WARP(OWL_WARP_MOUNTAIN_VILLAGE);
            break;
        case RI_OWL_SNOWHEAD:
            CLEAR_OWL_WARP(OWL_WARP_SNOWHEAD);
            break;
        case RI_OWL_SOUTHERN_SWAMP:
            CLEAR_OWL_WARP(OWL_WARP_SOUTHERN_SWAMP);
            break;
        case RI_OWL_STONE_TOWER:
            CLEAR_OWL_WARP(OWL_WARP_STONE_TOWER);
            break;
        case RI_OWL_WOODFALL:
            CLEAR_OWL_WARP(OWL_WARP_WOODFALL);
            break;
        case RI_OWL_ZORA_CAPE:
            CLEAR_OWL_WARP(OWL_WARP_ZORA_CAPE);
            break;
        case RI_TINGLE_MAP_CLOCK_TOWN:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
            break;
        case RI_TINGLE_MAP_WOODFALL:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
            break;
        case RI_TINGLE_MAP_SNOWHEAD:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
            break;
        case RI_TINGLE_MAP_ROMANI_RANCH:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
            break;
        case RI_TINGLE_MAP_GREAT_BAY:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
            break;
        case RI_TINGLE_MAP_STONE_TOWER:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
            break;
        case RI_TIME_DAY_1:
        case RI_TIME_NIGHT_1:
        case RI_TIME_DAY_2:
        case RI_TIME_NIGHT_2:
        case RI_TIME_DAY_3:
        case RI_TIME_NIGHT_3: {
            int index = Rando::ClockItems::GetHalfDayIndexFromClockItem(randoItemId);
            if (index != Rando::ClockItems::INVALID) {
                Flags_ClearRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + index));
            }
            break;
        }
        case RI_TIME_PROGRESSIVE: {
            // Remove most recently earned half-day per current mode
            const bool descending = (RANDO_SAVE_OPTIONS[RO_CLOCK_SHUFFLE_PROGRESSIVE] == RO_CLOCK_SHUFFLE_DESCENDING);
            // For ascending mode, remove the latest (fromEnd=true)
            // For descending mode, remove the earliest (fromEnd=false)
            int toRemove = Rando::ClockItems::FindOwnedHalfDay(!descending);
            if (toRemove >= 0) {
                Flags_ClearRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + toRemove));
            }
            break;
        }
        case RI_HEART_CONTAINER:
            gSaveContext.save.saveInfo.playerData.healthCapacity -= 0x10;
            gSaveContext.save.saveInfo.playerData.health =
                MIN(gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
            break;
        case RI_HEART_PIECE:
            if (GET_QUEST_HEART_PIECE_COUNT == 0) {
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                INCREMENT_QUEST_HEART_PIECE_COUNT;
                gSaveContext.save.saveInfo.playerData.healthCapacity -= 0x10;
                gSaveContext.save.saveInfo.playerData.health = MIN(
                    gSaveContext.save.saveInfo.playerData.health, gSaveContext.save.saveInfo.playerData.healthCapacity);
            } else {
                DECREMENT_QUEST_HEART_PIECE_COUNT;
            }
            break;
        case RI_BOMBERS_NOTEBOOK:
            REMOVE_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK);
            break;
        case RI_SONG_DOUBLE_TIME:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_SONG_DOUBLE_TIME);
            break;
        case RI_SONG_ELEGY:
            REMOVE_QUEST_ITEM(QUEST_SONG_ELEGY);
            break;
        case RI_SONG_EPONA:
            REMOVE_QUEST_ITEM(QUEST_SONG_EPONA);
            break;
        case RI_SONG_HEALING:
            REMOVE_QUEST_ITEM(QUEST_SONG_HEALING);
            break;
        case RI_SONG_INVERTED_TIME:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_SONG_INVERTED_TIME);
            break;
        case RI_SONG_LULLABY_INTRO:
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO);
            break;
        case RI_SONG_LULLABY:
            REMOVE_QUEST_ITEM(QUEST_SONG_LULLABY);
            break;
        case RI_SONG_NOVA:
            REMOVE_QUEST_ITEM(QUEST_SONG_BOSSA_NOVA);
            break;
        case RI_SONG_OATH:
            REMOVE_QUEST_ITEM(QUEST_SONG_OATH);
            break;
        case RI_SONG_SARIA:
            gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable =
                MAX(gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable - 1, 0);
            if (gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable == 0) {
                REMOVE_QUEST_ITEM(QUEST_SONG_SARIA);
            }
            break;
        case RI_SONG_SOARING:
            REMOVE_QUEST_ITEM(QUEST_SONG_SOARING);
            break;
        case RI_SONG_SONATA:
            REMOVE_QUEST_ITEM(QUEST_SONG_SONATA);
            break;
        case RI_SONG_STORMS:
            REMOVE_QUEST_ITEM(QUEST_SONG_STORMS);
            break;
        case RI_SONG_SUN:
            REMOVE_QUEST_ITEM(QUEST_SONG_SUN);
            break;
        case RI_SONG_TIME:
            REMOVE_QUEST_ITEM(QUEST_SONG_TIME);
            break;
        case RI_REMAINS_GOHT:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_GOHT);
            break;
        case RI_REMAINS_GYORG:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_GYORG);
            break;
        case RI_REMAINS_ODOLWA:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_ODOLWA);
            break;
        case RI_REMAINS_TWINMOLD:
            REMOVE_QUEST_ITEM(QUEST_REMAINS_TWINMOLD);
            break;
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
            Flags_ClearRandoInf(SOUL_RI_TO_RANDO_INF(randoItemId));
            break;
        case RI_FROG_BLUE:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_33_01);
            break;
        case RI_FROG_CYAN:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_32_40);
            break;
        case RI_FROG_PINK:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_32_80);
            break;
        case RI_FROG_WHITE:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_33_02);
            break;
        case RI_OCARINA_BUTTON_A:
        case RI_OCARINA_BUTTON_C_DOWN:
        case RI_OCARINA_BUTTON_C_LEFT:
        case RI_OCARINA_BUTTON_C_RIGHT:
        case RI_OCARINA_BUTTON_C_UP:
            Flags_ClearRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A + (randoItemId - RI_OCARINA_BUTTON_A));
            break;
        // Ignore Ammo
        case RI_BOMBCHU:
        case RI_DEKU_STICK:
        case RI_DEKU_NUT:
        case RI_MILK_REFILL:
        case RI_RED_POTION_REFILL:
        case RI_GREEN_POTION_REFILL:
        case RI_BLUE_POTION_REFILL:
        case RI_FAIRY_REFILL:
        case RI_GOLD_DUST_REFILL:
            break;
        default:
            if (Rando::StaticData::Items[randoItemId].itemId < 77) {
                Inventory_DeleteItem(Rando::StaticData::Items[randoItemId].itemId,
                                     SLOT(Rando::StaticData::Items[randoItemId].itemId));
            }
            break;
    }
}
