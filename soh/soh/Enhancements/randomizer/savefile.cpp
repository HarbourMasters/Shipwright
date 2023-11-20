#include "savefile.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"

uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
uint8_t Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
}

void StartingItemGive(GetItemEntry getItemEntry) {
    if (getItemEntry.modIndex == MOD_NONE) {
        if (getItemEntry.getItemId == GI_SWORD_BGS) {
            gSaveContext.bgsFlag = true;
        }
        Item_Give(NULL, getItemEntry.itemId);
    } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
        if (getItemEntry.getItemId == RG_ICE_TRAP) {
            gSaveContext.pendingIceTrapCount++;
        } else {
            Randomizer_Item_Give(NULL, getItemEntry);
        }
    }
}

// RANDOTODO replace most of these GiveLink functions with calls to
// Item_Give in z_parameter, we'll need to update Item_Give to ensure
// nothing breaks when calling it without a valid play first
void GiveLinkRupees(int numOfRupees) {
    int maxRupeeCount;
    if (CUR_UPG_VALUE(UPG_WALLET) == 0) {
        maxRupeeCount = 99;
    } else if (CUR_UPG_VALUE(UPG_WALLET) == 1) {
        maxRupeeCount = 200;
    } else if (CUR_UPG_VALUE(UPG_WALLET) == 2) {
        maxRupeeCount = 500;
    }

    int newRupeeCount = gSaveContext.rupees;
    newRupeeCount += numOfRupees;

    if (newRupeeCount > maxRupeeCount) {
        gSaveContext.rupees = maxRupeeCount;
    } else {
        gSaveContext.rupees = newRupeeCount;
    }
}

void GiveLinkDekuSticks(int howManySticks) {
    int maxStickCount;
    if (CUR_UPG_VALUE(UPG_STICKS) == 0) {
        INV_CONTENT(ITEM_STICK) = ITEM_STICK;
        Inventory_ChangeUpgrade(UPG_STICKS, 1);
        maxStickCount = 10;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 1) {
        maxStickCount = 10;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 2) {
        maxStickCount = 20;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 3) {
        maxStickCount = 30;
    }

    if ((AMMO(ITEM_STICK) + howManySticks) > maxStickCount) {
        AMMO(ITEM_STICK) = maxStickCount;
    } else {
        AMMO(ITEM_STICK) += howManySticks;
    }
}

void GiveLinkDekuNuts(int howManyNuts) {
    int maxNutCount;
    if (CUR_UPG_VALUE(UPG_NUTS) == 0) {
        INV_CONTENT(ITEM_NUT) = ITEM_NUT;
        Inventory_ChangeUpgrade(UPG_NUTS, 1);
        maxNutCount = 20;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 1) {
        maxNutCount = 20;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 2) {
        maxNutCount = 30;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 3) {
        maxNutCount = 40;
    }

    if ((AMMO(ITEM_NUT) + howManyNuts) > maxNutCount) {
        AMMO(ITEM_NUT) = maxNutCount;
    } else {
        AMMO(ITEM_NUT) += howManyNuts;
    }
}

void GiveLinksPocketItem() {
    if (Randomizer_GetSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LINKS_POCKET, (GetItemID)RG_NONE);
        StartingItemGive(getItemEntry);
    }
}

void SetStartingItems() {
    if (Randomizer_GetSettingValue(RSK_STARTING_KOKIRI_SWORD))
        Item_Give(NULL, ITEM_SWORD_KOKIRI);
    if (Randomizer_GetSettingValue(RSK_STARTING_DEKU_SHIELD))
        Item_Give(NULL, ITEM_SHIELD_DEKU);

    // Songs
    if (Randomizer_GetSettingValue(RSK_STARTING_ZELDAS_LULLABY))
        Item_Give(NULL, ITEM_SONG_LULLABY);
    if (Randomizer_GetSettingValue(RSK_STARTING_EPONAS_SONG))
        Item_Give(NULL, ITEM_SONG_EPONA);
    if (Randomizer_GetSettingValue(RSK_STARTING_SARIAS_SONG))
        Item_Give(NULL, ITEM_SONG_SARIA);
    if (Randomizer_GetSettingValue(RSK_STARTING_SUNS_SONG))
        Item_Give(NULL, ITEM_SONG_SUN);
    if (Randomizer_GetSettingValue(RSK_STARTING_SONG_OF_TIME))
        Item_Give(NULL, ITEM_SONG_TIME);
    if (Randomizer_GetSettingValue(RSK_STARTING_SONG_OF_STORMS))
        Item_Give(NULL, ITEM_SONG_STORMS);
    if (Randomizer_GetSettingValue(RSK_STARTING_MINUET_OF_FOREST))
        Item_Give(NULL, ITEM_SONG_MINUET);
    if (Randomizer_GetSettingValue(RSK_STARTING_BOLERO_OF_FIRE))
        Item_Give(NULL, ITEM_SONG_BOLERO);
    if (Randomizer_GetSettingValue(RSK_STARTING_SERENADE_OF_WATER))
        Item_Give(NULL, ITEM_SONG_SERENADE);
    if (Randomizer_GetSettingValue(RSK_STARTING_REQUIEM_OF_SPIRIT))
        Item_Give(NULL, ITEM_SONG_REQUIEM);
    if (Randomizer_GetSettingValue(RSK_STARTING_NOCTURNE_OF_SHADOW))
        Item_Give(NULL, ITEM_SONG_NOCTURNE);
    if (Randomizer_GetSettingValue(RSK_STARTING_PRELUDE_OF_LIGHT))
        Item_Give(NULL, ITEM_SONG_PRELUDE);

    if (Randomizer_GetSettingValue(RSK_STARTING_SKULLTULA_TOKEN)) {
        gSaveContext.inventory.questItems |= gBitFlags[QUEST_SKULL_TOKEN];
        gSaveContext.inventory.gsTokens = Randomizer_GetSettingValue(RSK_STARTING_SKULLTULA_TOKEN);
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_OCARINA)) {
        INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_CONSUMABLES)) {
        GiveLinkDekuSticks(10);
        GiveLinkDekuNuts(20);
    }

    if (Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
        GiveLinkRupees(9001);
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        uint32_t mapBitMask = 1 << 1;
        uint32_t compassBitMask = 1 << 2;
        uint32_t startingDungeonItemsBitMask = mapBitMask | compassBitMask;
        for (int scene = SCENE_DEKU_TREE; scene <= SCENE_ICE_CAVERN; scene++) {
            gSaveContext.inventory.dungeonItems[scene] |= startingDungeonItemsBitMask;
        }
    }

    if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;         // Forest
        gSaveContext.sohStats.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;          // Forest
        gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;            // Fire
        gSaveContext.sohStats.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;             // Fire
        gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;         // Water
        gSaveContext.sohStats.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;          // Water
        gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;      // Spirit
        gSaveContext.sohStats.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;       // Spirit
        gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;        // Shadow
        gSaveContext.sohStats.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;         // Shadow
        gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX; // BotW
        gSaveContext.sohStats.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;  // BotW
        gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;  // GTG
        gSaveContext.sohStats.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX;   // GTG
        gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;      // Ganon
        gSaveContext.sohStats.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;       // Ganon
    } else if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA) {
        // Logic cannot handle vanilla key layout in some dungeons
        // this is because vanilla expects the dungeon major item to be
        // locked behind the keys, which is not always true in rando.
        // We can resolve this by starting with some extra keys
        if (ResourceMgr_IsSceneMasterQuest(SCENE_SPIRIT_TEMPLE)) {
            // MQ Spirit needs 3 keys
            gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = 3;
            gSaveContext.sohStats.dungeonKeys[SCENE_SPIRIT_TEMPLE] = 3;
        }
    }

    if (Randomizer_GetSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        gSaveContext.inventory.dungeonItems[SCENE_FOREST_TEMPLE] |= 1;    // Forest
        gSaveContext.inventory.dungeonItems[SCENE_FIRE_TEMPLE] |= 1;     // Fire
        gSaveContext.inventory.dungeonItems[SCENE_WATER_TEMPLE] |= 1;   // Water
        gSaveContext.inventory.dungeonItems[SCENE_SPIRIT_TEMPLE] |= 1; // Spirit
        gSaveContext.inventory.dungeonItems[SCENE_SHADOW_TEMPLE] |= 1;   // Shadow
    }

    if (Randomizer_GetSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_STARTWITH) {
        gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER] |= 1;
    }
}

extern "C" void Randomizer_InitSaveFile() {
    // Sets all rando flags to false
    for (s32 i = 0; i < ARRAY_COUNT(gSaveContext.randomizerInf); i++) {
        gSaveContext.randomizerInf[i] = 0;
    }

    gSaveContext.cutsceneIndex = 0; // no intro cutscene
    // Starts pending ice traps out at 0 before potentially incrementing them down the line.
    gSaveContext.pendingIceTrapCount = 0;

    // Set Cutscene flags and texts to skip them
    Flags_SetInfTable(INFTABLE_GREETED_BY_SARIA);
    Flags_SetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO);
    Flags_SetEventChkInf(EVENTCHKINF_MET_DEKU_TREE);
    Flags_SetEventChkInf(EVENTCHKINF_DEKU_TREE_OPENED_MOUTH);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_KAEPORA_IN_LAKE_HYLIA);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_MASTER_SWORD_CHAMBER);
    Flags_SetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL);
    Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
    Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
    Flags_SetEventChkInf(EVENTCHKINF_RENTED_HORSE_FROM_INGO);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_POE_COLLECTOR_IN_RUINED_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_NABOORU_IN_SPIRIT_TEMPLE);

    Flags_SetInfTable(INFTABLE_MET_CHILD_MALON_AT_CASTLE_OR_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_CASTLE_OR_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_INGO_AT_RANCH_BEFORE_TALON_RETURNS);
    Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH);
    Flags_SetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
    Flags_SetInfTable(INFTABLE_CHILD_MALON_SAID_EPONA_WAS_AFRAID_OF_YOU);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_INGO_ONCE_AS_ADULT);

    // Ruto already met in jabu and spawns down the hole immediately
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_MEET_RUTO);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_TALK_FIRST_TIME);
    Flags_SetInfTable(INFTABLE_RUTO_IN_JJ_WANTS_TO_BE_TOSSED_TO_SAPPHIRE);

    // Skip cutscenes before Nabooru fight
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_NABOORU_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_NABOORU_ORDERED_TO_FIGHT_BY_TWINROVA);

    // Skip boss cutscenes
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_GOHMA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_KING_DODONGO_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_MORPHA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_TWINROVA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE);
    Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE);

    // Entered areas
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_HYRULE_FIELD);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEATH_MOUNTAIN_TRAIL);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_KAKARIKO_VILLAGE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_ZORAS_DOMAIN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_HYRULE_CASTLE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GORON_CITY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_TEMPLE_OF_TIME);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEKU_TREE);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DODONGOS_CAVERN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_LAKE_HYLIA);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GERUDO_VALLEY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GERUDOS_FORTRESS);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_LON_LON_RANCH);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_JABU_JABUS_BELLY);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GRAVEYARD);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_ZORAS_FOUNTAIN);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DESERT_COLOSSUS);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_DEATH_MOUNTAIN_CRATER);
    Flags_SetEventChkInf(EVENTCHKINF_ENTERED_GANONS_CASTLE_EXTERIOR);
    Flags_SetInfTable(INFTABLE_ENTERED_HYRULE_CASTLE);

    // skip the z target talk instructions by the kokiri shop
    gSaveContext.sceneFlags[SCENE_KOKIRI_FOREST].swch |= (1 << 0x1F);

    // Go away ruto (water temple first cutscene)
    gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x10);

    // no more kaepora
    gSaveContext.sceneFlags[SCENE_HYRULE_FIELD].swch |= (1 << 0xC);  // hyrule field kaepora outside kokiri forest
    gSaveContext.sceneFlags[SCENE_HYRULE_FIELD].swch |= (1 << 0xB);  // hyrule field kaepora outside lake hylia
    gSaveContext.sceneFlags[SCENE_LOST_WOODS].swch |= (1 << 0x7);  // lost woods kaepora pre-saria
    gSaveContext.sceneFlags[SCENE_LOST_WOODS].swch |= (1 << 0x8);  // lost woods kaepora post-saria
    gSaveContext.sceneFlags[SCENE_DESERT_COLOSSUS].swch |= (1 << 0x1F); // desert colossus kaepora
    gSaveContext.sceneFlags[SCENE_HYRULE_CASTLE].swch |= (1 << 0x5);  // hyrule castle kaepora

    if (!Randomizer_GetSettingValue(RSK_ENABLE_GLITCH_CUTSCENES)) {
        Flags_SetInfTable(INFTABLE_SPOKE_TO_DARUNIA_IN_FIRE_TEMPLE); // Darunia in Fire Temple
    }

    // Give Link's pocket item
    GiveLinksPocketItem();

    // shuffle adult trade quest
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE)) {
        gSaveContext.adultTradeItems = 0;
    }

    int startingAge = Randomizer_GetSettingValue(RSK_STARTING_AGE);
    switch (startingAge) {
        case RO_AGE_ADULT: // Adult
            gSaveContext.linkAge = LINK_AGE_ADULT;
            gSaveContext.entranceIndex = ENTR_TEMPLE_OF_TIME_7;
            gSaveContext.savedSceneNum = SCENE_LON_LON_RANCH; // Set scene num manually to ToT
            break;
        case RO_AGE_CHILD: // Child
            gSaveContext.linkAge = LINK_AGE_CHILD;
            gSaveContext.savedSceneNum = -1;
            break;
        default:
            break;
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS)) {
        // Override the spawn entrance so entrance rando can take control,
        // and to prevent remember save location from breaking inital spawn
        gSaveContext.entranceIndex = -1;
    }

    // If any trials aren't required, set them as completed
    for (u16 i = RAND_INF_TRIALS_DONE_LIGHT_TRIAL; i <= RAND_INF_TRIALS_DONE_SHADOW_TRIAL; i++) {
        if (!OTRGlobals::Instance->gRandomizer->IsTrialRequired((RandomizerInf)i)) {
            Flags_SetRandomizerInf((RandomizerInf)i);
        }
    }

    if (Randomizer_GetSettingValue(RSK_SKIP_CHILD_ZELDA)) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_SONG_FROM_IMPA, (GetItemID)RG_ZELDAS_LULLABY);
        StartingItemGive(getItemEntry);

        // malon/talon back at ranch
        Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_CASTLE);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);

        // Set "Got Zelda's Letter" flag. Also ensures Saria is back at SFM.
        Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER);

        // Got item from impa
        Flags_SetEventChkInf(EVENTCHKINF_LEARNED_ZELDAS_LULLABY);

        gSaveContext.sceneFlags[SCENE_HYRULE_CASTLE].swch |= (1 << 0x4); // move milk crates in hyrule castle to moat

        // set this at the end to ensure we always start with the letter
        // this is for the off chance we got the weird egg from impa (which should never happen)
        INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_LETTER_ZELDA;
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_MASTER_SWORD) && startingAge == RO_AGE_ADULT) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_TOT_MASTER_SWORD, GI_NONE);
        StartingItemGive(getItemEntry);
        Flags_SetRandomizerInf(RAND_INF_TOT_MASTER_SWORD);
    }

    HIGH_SCORE(HS_POE_POINTS) = 1000 - (100 * Randomizer_GetSettingValue(RSK_BIG_POE_COUNT));

    if (Randomizer_GetSettingValue(RSK_SKIP_EPONA_RACE)) {
        Flags_SetEventChkInf(EVENTCHKINF_EPONA_OBTAINED);
    }

    // Open lowest Vanilla Fire Temple locked door (to prevent key logic lockouts)
    // Not done on keysanity since this lockout is a non issue when Fire keys can be found outside the temple
    u8 keysanity = Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                   Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                   Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
    if (!ResourceMgr_IsSceneMasterQuest(SCENE_FIRE_TEMPLE) && !keysanity) {
        gSaveContext.sceneFlags[SCENE_FIRE_TEMPLE].swch |= (1 << 0x17);
    }

    // Opens locked Water Temple door in vanilla to prevent softlocks
    // West door on the middle level that leads to the water raising thing
    // Happens in 3DS rando and N64 rando as well
    if (!ResourceMgr_IsSceneMasterQuest(SCENE_WATER_TEMPLE)) {
        gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x15);
    }

    int openForest = Randomizer_GetSettingValue(RSK_FOREST);
    switch (openForest) {
        case RO_FOREST_OPEN:
            Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
            // Fallthrough
        case RO_FOREST_CLOSED_DEKU:
            Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
            break;
    }

    int doorOfTime = Randomizer_GetSettingValue(RSK_DOOR_OF_TIME);
    switch (doorOfTime) {
        case RO_DOOROFTIME_OPEN:
            Flags_SetEventChkInf(EVENTCHKINF_OPENED_THE_DOOR_OF_TIME);
            break;
    }

    if (Randomizer_GetSettingValue(RSK_KAK_GATE) == RO_KAK_GATE_OPEN) {
        Flags_SetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD);
    }

    if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_FAST ||
        Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_OPEN) {
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(1));
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(2));
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(3));
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x02); // heard yells and unlocked doors
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x03);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x04);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x06);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x07);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x08);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x10);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x12);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x13);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0A); // picked up keys
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0E);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0F);
    }

    if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_OPEN) {
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(0));
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x01); // heard yell and unlocked door
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x05);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x11);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0C); // picked up key

        if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
            Item_Give(NULL, ITEM_GERUDO_CARD);
        }
    }

    // complete mask quest
    if (Randomizer_GetSettingValue(RSK_COMPLETE_MASK_QUEST)) {
        Flags_SetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK);
        Flags_SetEventChkInf(EVENTCHKINF_PAID_BACK_BUNNY_HOOD_FEE);
        gSaveContext.itemGetInf[3] |= 0x100;  // Sold Keaton Mask
        gSaveContext.itemGetInf[3] |= 0x200;  // Sold Skull Mask
        gSaveContext.itemGetInf[3] |= 0x400;  // Sold Spooky Mask
        gSaveContext.itemGetInf[3] |= 0x800;  // bunny hood related
        gSaveContext.itemGetInf[3] |= 0x8000; // Obtained Mask of Truth
    }

    // Reset triforce pieces collected
    gSaveContext.triforcePiecesCollected = 0;

    SetStartingItems();
}
