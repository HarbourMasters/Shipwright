#include "Rando/Rando.h"
#include "Rando/ActorBehavior/Souls.h"
#include "Rando/MiscBehavior/MiscBehavior.h"
#include "Rando/MiscBehavior/ClockShuffle.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

void Rando::GiveItem(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_CLOCK_TOWN_STRAY_FAIRY:
            SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
            break;
        case RI_WOODFALL_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]++;
            break;
        case RI_SNOWHEAD_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]++;
            break;
        case RI_GREAT_BAY_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]++;
            break;
        case RI_STONE_TOWER_STRAY_FAIRY:
            gSaveContext.save.saveInfo.inventory.strayFairies[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]++;
            break;
        case RI_GREAT_SPIN_ATTACK:
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
            break;
        case RI_DOUBLE_DEFENSE:
            gSaveContext.save.saveInfo.playerData.doubleDefense = true;
            gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
            break;
        case RI_SINGLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            SET_WEEKEVENTREG(WEEKEVENTREG_12_80);
            break;
        case RI_DOUBLE_MAGIC:
            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
            gSaveContext.save.saveInfo.playerData.magic = gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
            SET_WEEKEVENTREG(WEEKEVENTREG_12_80);
            break;
        // Don't love this because it doesn't hit GameInteractor_ExecuteOnItemGive()
        // but not sure how else to account for collecting outside of dungeon
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_MAP:
        case RI_WOODFALL_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE);
            break;
        case RI_WOODFALL_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE]++;
            }
            break;
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_MAP:
        case RI_SNOWHEAD_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE);
            break;
        case RI_SNOWHEAD_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE]++;
            }
            break;
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_MAP:
        case RI_GREAT_BAY_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE);
            break;
        case RI_GREAT_BAY_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE]++;
            }
            break;
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_MAP:
        case RI_STONE_TOWER_COMPASS:
            SET_DUNGEON_ITEM(Rando::StaticData::Items[randoItemId].itemId - ITEM_KEY_BOSS,
                             DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE);
            break;
        case RI_STONE_TOWER_SMALL_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) < 0) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE] = 1;
            } else {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE)++;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE]++;
            }
            break;
        // Grants the max small keys for every dungeon at once (Woodfall 1, Snowhead 3, Great Bay 1, Stone Tower 4)
        case RI_SKELETON_KEY:
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) < 1) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE] = 1;
            }
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) < 3) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE) = 3;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE] = 3;
            }
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) < 1) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE) = 1;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE] = 1;
            }
            if (DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) < 4) {
                DUNGEON_KEY_COUNT(DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE) = 4;
                gSaveContext.save.shipSaveInfo.rando.foundDungeonKeys[DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE] = 4;
            }
            break;
        case RI_TRIFORCE_PIECE:
        case RI_TRIFORCE_PIECE_PREVIOUS:
            gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces++;
            if (gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces ==
                RANDO_SAVE_OPTIONS[RO_TRIFORCE_PIECES_REQUIRED]) {
                // Blocks the ability to beat the game through killing Majora until all Triforce Pieces are found.
                if (!Flags_GetRandoInf(RANDO_INF_OBTAINED_SOUL_OF_BOSS_MAJORA)) {
                    Rando::GiveItem(RI_SOUL_BOSS_MAJORA);
                }
                GameInteractor_ExecuteOnGameCompletion();
                GameInteractor::Instance->events.emplace_back(
                    GIEventTransition{ .entrance = ENTRANCE(TERMINA_FIELD, 0),
                                       .cutsceneIndex = 0xFFF7,
                                       .transitionTrigger = TRANS_TRIGGER_START,
                                       .transitionType = TRANS_TYPE_FADE_BLACK });
            }
            break;
        // Technically these should never be used, but leaving them here just in case
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
        case RI_PROGRESSIVE_LULLABY:
        case RI_PROGRESSIVE_SWORD:
        case RI_PROGRESSIVE_WALLET:
            Rando::GiveItem(Rando::ConvertItem(randoItemId));
            break;
        case RI_BOMB_BAG_20:
        case RI_BOMB_BAG_30:
        case RI_BOMB_BAG_40:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMB) = AMMO(ITEM_BOMBCHU) = CUR_CAPACITY(UPG_BOMB_BAG);
            break;
        case RI_WALLET_ADULT:
        case RI_WALLET_GIANT:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            // Fill Rupees to max, this may be opt-in later
            // Use remaining space rather than full capacity to avoid excess in the accumulator.
            gSaveContext.rupeeAccumulator = CUR_CAPACITY(UPG_WALLET) - gSaveContext.save.saveInfo.playerData.rupees;
            break;
        case RI_WALLET_TYCOON:
            Inventory_ChangeUpgrade(UPG_WALLET, 3);
            gSaveContext.rupeeAccumulator = CUR_CAPACITY(UPG_WALLET) - gSaveContext.save.saveInfo.playerData.rupees;
            break;
        case RI_GS_TOKEN_SWAMP:
            // Set QUEST_QUIVER to match bug mentioned in z_parameter.c
            SET_QUEST_ITEM(QUEST_QUIVER);
            Inventory_IncrementSkullTokenCount(SCENE_KINSTA1);
            break;
        case RI_GS_TOKEN_OCEAN:
            SET_QUEST_ITEM(QUEST_QUIVER);
            Inventory_IncrementSkullTokenCount(SCENE_KINDAN2);
            break;
        case RI_MOONS_TEAR:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_MOONS_TEAR);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_LAND:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_LAND);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_SWAMP:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_SWAMP);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_MOUNTAIN:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_MOUNTAIN);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_DEED_OCEAN:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_DEED_OCEAN);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_ROOM_KEY:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_LETTER_TO_MAMA:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_MAMA);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_LETTER_TO_KAFEI:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_LETTER_TO_KAFEI);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_PENDANT_OF_MEMORIES:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_POWDER_KEG:
            Flags_SetWeekEventReg(WEEKEVENTREG_HAS_POWDERKEG_PRIVILEGES);
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_SWORD_GILDED:
        case RI_SWORD_KOKIRI:
        case RI_SWORD_RAZOR:
            if (STOLEN_ITEM_1 == ITEM_SWORD_KOKIRI || STOLEN_ITEM_1 == ITEM_SWORD_RAZOR) {
                SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
            }
            if (STOLEN_ITEM_2 == ITEM_SWORD_KOKIRI || STOLEN_ITEM_2 == ITEM_SWORD_RAZOR) {
                SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);
            }

            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_TINGLE_MAP_CLOCK_TOWN:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_CLOCK_TOWN);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN);
            break;
        case RI_TINGLE_MAP_WOODFALL:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_WOODFALL);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL);
            break;
        case RI_TINGLE_MAP_SNOWHEAD:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_SNOWHEAD);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD);
            break;
        case RI_TINGLE_MAP_ROMANI_RANCH:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_ROMANI_RANCH);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH);
            break;
        case RI_TINGLE_MAP_GREAT_BAY:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_GREAT_BAY);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY);
            break;
        case RI_TINGLE_MAP_STONE_TOWER:
            Inventory_SetWorldMapCloudVisibility(TINGLE_MAP_STONE_TOWER);
            SET_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER);
            break;
        case RI_OWL_CLOCK_TOWN_SOUTH:
            Sram_ActivateOwl(OWL_WARP_CLOCK_TOWN);
            break;
        case RI_OWL_GREAT_BAY_COAST:
            Sram_ActivateOwl(OWL_WARP_GREAT_BAY_COAST);
            break;
        case RI_OWL_IKANA_CANYON:
            Sram_ActivateOwl(OWL_WARP_IKANA_CANYON);
            break;
        case RI_OWL_MILK_ROAD:
            Sram_ActivateOwl(OWL_WARP_MILK_ROAD);
            break;
        case RI_OWL_MOUNTAIN_VILLAGE:
            Sram_ActivateOwl(OWL_WARP_MOUNTAIN_VILLAGE);
            break;
        case RI_OWL_SNOWHEAD:
            Sram_ActivateOwl(OWL_WARP_SNOWHEAD);
            break;
        case RI_OWL_SOUTHERN_SWAMP:
            Sram_ActivateOwl(OWL_WARP_SOUTHERN_SWAMP);
            break;
        case RI_OWL_STONE_TOWER:
            Sram_ActivateOwl(OWL_WARP_STONE_TOWER);
            break;
        case RI_OWL_WOODFALL:
            Sram_ActivateOwl(OWL_WARP_WOODFALL);
            break;
        case RI_OWL_ZORA_CAPE:
            Sram_ActivateOwl(OWL_WARP_ZORA_CAPE);
            break;
        case RI_TIME_DAY_1:
        case RI_TIME_NIGHT_1:
        case RI_TIME_DAY_2:
        case RI_TIME_NIGHT_2:
        case RI_TIME_DAY_3:
        case RI_TIME_NIGHT_3: {
            int index = Rando::ClockItems::GetHalfDayIndexFromClockItem(randoItemId);
            if (index != Rando::ClockItems::INVALID) {
                Flags_SetRandoInf(static_cast<RandoInf>(RANDO_INF_OBTAINED_CLOCK_DAY_1 + index));
            }
            break;
        }
        case RI_TIME_PROGRESSIVE: {
            // Convert to actual half-day per mode
            RandoItemId concrete = Rando::ConvertItem(RI_TIME_PROGRESSIVE);
            if (concrete != RI_JUNK) {
                Rando::GiveItem(concrete);
            }
            break;
        }
        case RI_HEART_CONTAINER:
        case RI_HEART_PIECE:
            gSaveContext.healthAccumulator = gSaveContext.save.saveInfo.playerData.healthCapacity + 0x10;
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_BOTTLE_RED_POTION:
            // ITEM_LONGSHOT will give a Red Potion bottle on the first available bottle slot
            // ITEM_POTION_RED will put a Red Potion bottle on the first bottle slot
            Item_Give(gPlayState, ITEM_LONGSHOT);
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
            Flags_SetRandoInf(SOUL_RI_TO_RANDO_INF(randoItemId));
            break;
        case RI_FROG_BLUE:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_01);
            break;
        case RI_FROG_CYAN:
            SET_WEEKEVENTREG(WEEKEVENTREG_32_40);
            break;
        case RI_FROG_PINK:
            SET_WEEKEVENTREG(WEEKEVENTREG_32_80);
            break;
        case RI_FROG_WHITE:
            SET_WEEKEVENTREG(WEEKEVENTREG_33_02);
            break;
        case RI_ABILITY_SWIM:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_SWIM);
            break;
        case RI_TRAP:
            Rando::MiscBehavior::OfferTrapItem();
            break;
        case RI_OCARINA_BUTTON_A:
        case RI_OCARINA_BUTTON_C_DOWN:
        case RI_OCARINA_BUTTON_C_LEFT:
        case RI_OCARINA_BUTTON_C_RIGHT:
        case RI_OCARINA_BUTTON_C_UP:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_OCARINA_BUTTON_A + (randoItemId - RI_OCARINA_BUTTON_A));
            break;
        case RI_SONG_DOUBLE_TIME:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_SONG_DOUBLE_TIME);
            break;
        case RI_SONG_INVERTED_TIME:
            Flags_SetRandoInf(RANDO_INF_OBTAINED_SONG_INVERTED_TIME);
            break;
        case RI_SONG_SARIA:
            gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable++;
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
        case RI_JUNK:
        case RI_NONE:
            break;
        default:
            Item_Give(gPlayState, Rando::StaticData::Items[randoItemId].itemId);
            break;
    }
}
