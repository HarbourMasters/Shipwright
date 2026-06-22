#include "savefile.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/logic.h"
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"

uint8_t Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
}

// RANDOTODO: Replace most of these GiveLink functions with calls to
// Item_Give in z_parameter, we'll need to update Item_Give to ensure
// nothing breaks when calling it without a valid play first.
void GiveLinkRupees(int numOfRupees) {
    int maxRupeeCount = 0;
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

static uint16_t rupeeCounts[] = {
    1,   // ITEM_RUPEE_GREEN
    5,   // ITEM_RUPEE_BLUE
    20,  // ITEM_RUPEE_RED
    50,  // ITEM_RUPEE_PURPLE
    200, // ITEM_RUPEE_GOLD
};

void StartingItemGive(GetItemEntry getItemEntry, RandomizerCheck randomizerCheck) {
    if (randomizerCheck != RC_MAX) {
        OTRGlobals::Instance->gRandoContext->GetItemLocation(randomizerCheck)->SetCheckStatus(RCSHOW_SAVED);
    }
    if (getItemEntry.modIndex == MOD_NONE) {
        if (getItemEntry.itemId >= ITEM_RUPEE_GREEN && getItemEntry.itemId <= ITEM_RUPEE_GOLD) {
            GiveLinkRupees(rupeeCounts[getItemEntry.itemId - ITEM_RUPEE_GREEN]);
        } else {
            if (getItemEntry.getItemId == GI_SWORD_BGS) {
                gSaveContext.bgsFlag = true;
            }
            Item_Give(NULL, static_cast<uint8_t>(getItemEntry.itemId));
        }
    } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
        if (getItemEntry.getItemId == RG_ICE_TRAP) {
            gSaveContext.ship.pendingIceTrapCount++;
        } else {
            Randomizer_Item_Give(NULL, getItemEntry);
        }
    }
}

void GiveLinkDekuSticks(int howManySticks) {
    int maxStickCount = 0;
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
    int maxNutCount = 0;
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
        StartingItemGive(getItemEntry, RC_LINKS_POCKET);
        // If we re-add the above, we'll get the item on save creation, now it's given on first load.
        Flags_SetRandomizerInf(RAND_INF_LINKS_POCKET);
    }
}

void SetStartingItems() {
    int startingAge = OTRGlobals::Instance->gRandoContext->GetOption(RSK_SELECTED_STARTING_AGE).Get();
    if (Randomizer_GetSettingValue(RSK_STARTING_KOKIRI_SWORD))
        Item_Give(NULL, ITEM_SWORD_KOKIRI);
    if (Randomizer_GetSettingValue(RSK_STARTING_DEKU_SHIELD))
        Item_Give(NULL, ITEM_SHIELD_DEKU);
    if (Randomizer_GetSettingValue(RSK_STARTING_HYLIAN_SHIELD))
        Item_Give(NULL, ITEM_SHIELD_HYLIAN);
    if (Randomizer_GetSettingValue(RSK_STARTING_MIRROR_SHIELD))
        Item_Give(NULL, ITEM_SHIELD_MIRROR);
    if (Randomizer_GetSettingValue(RSK_STARTING_GORON_TUNIC))
        Item_Give(NULL, ITEM_TUNIC_GORON);
    if (Randomizer_GetSettingValue(RSK_STARTING_ZORA_TUNIC))
        Item_Give(NULL, ITEM_TUNIC_ZORA);
    if (Randomizer_GetSettingValue(RSK_STARTING_IRON_BOOTS))
        Item_Give(NULL, ITEM_BOOTS_IRON);
    if (Randomizer_GetSettingValue(RSK_STARTING_HOVER_BOOTS))
        Item_Give(NULL, ITEM_BOOTS_HOVER);
    if (Randomizer_GetSettingValue(RSK_STARTING_MEGATON_HAMMER))
        Item_Give(NULL, ITEM_HAMMER);
    if (Randomizer_GetSettingValue(RSK_STARTING_BOOMERANG))
        Item_Give(NULL, ITEM_BOOMERANG);
    if (Randomizer_GetSettingValue(RSK_STARTING_LENS_OF_TRUTH))
        Item_Give(NULL, ITEM_LENS);
    if (Randomizer_GetSettingValue(RSK_STARTING_DINS_FIRE))
        Item_Give(NULL, ITEM_DINS_FIRE);
    if (Randomizer_GetSettingValue(RSK_STARTING_FARORES_WIND))
        Item_Give(NULL, ITEM_FARORES_WIND);
    if (Randomizer_GetSettingValue(RSK_STARTING_NAYRUS_LOVE))
        Item_Give(NULL, ITEM_NAYRUS_LOVE);
    if (Randomizer_GetSettingValue(RSK_STARTING_FIRE_ARROWS))
        Item_Give(NULL, ITEM_ARROW_FIRE);
    if (Randomizer_GetSettingValue(RSK_STARTING_ICE_ARROWS))
        Item_Give(NULL, ITEM_ARROW_ICE);
    if (Randomizer_GetSettingValue(RSK_STARTING_LIGHT_ARROWS))
        Item_Give(NULL, ITEM_ARROW_LIGHT);
    if (Randomizer_GetSettingValue(RSK_STARTING_STONE_OF_AGONY))
        Item_Give(NULL, ITEM_STONE_OF_AGONY);

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

    if ((Randomizer_GetSettingValue(RSK_STARTING_HEARTS) + 1) != 3) {
        gSaveContext.healthCapacity = (Randomizer_GetSettingValue(RSK_STARTING_HEARTS) + 1) * 16;
        gSaveContext.health = gSaveContext.healthCapacity;
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_OCARINA)) {
        INV_CONTENT(ITEM_OCARINA_FAIRY) = Randomizer_GetSettingValue(RSK_STARTING_OCARINA) == RO_STARTING_OCARINA_FAIRY
                                              ? ITEM_OCARINA_FAIRY
                                              : ITEM_OCARINA_TIME;
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_STICKS) && !Randomizer_GetSettingValue(RSK_SHUFFLE_DEKU_STICK_BAG)) {
        GiveLinkDekuSticks(10);
    }
    if (Randomizer_GetSettingValue(RSK_STARTING_NUTS) && !Randomizer_GetSettingValue(RSK_SHUFFLE_DEKU_NUT_BAG)) {
        GiveLinkDekuNuts(20);
    }
    if (Randomizer_GetSettingValue(RSK_STARTING_MASTER_SWORD)) {
        if (startingAge == RO_AGE_ADULT) {
            Item_Give(NULL, ITEM_SWORD_MASTER);
        } else {
            gSaveContext.inventory.equipment |= 1 << 1;
        }
    }

    // Tiered/progressive starting items. The upgrade items are given cumulatively where Item_Give
    // only sets the base inventory content on the first tier.
    switch (Randomizer_GetSettingValue(RSK_STARTING_HOOKSHOT)) {
        case 2:
            Item_Give(NULL, ITEM_LONGSHOT);
            break;
        case 1:
            Item_Give(NULL, ITEM_HOOKSHOT);
            break;
    }

    uint8_t startBow = Randomizer_GetSettingValue(RSK_STARTING_BOW);
    if (startBow >= 1)
        Item_Give(NULL, ITEM_QUIVER_30);
    if (startBow >= 2)
        Item_Give(NULL, ITEM_QUIVER_40);
    if (startBow >= 3)
        Item_Give(NULL, ITEM_QUIVER_50);

    uint8_t startSlingshot = Randomizer_GetSettingValue(RSK_STARTING_SLINGSHOT);
    if (startSlingshot >= 1)
        Item_Give(NULL, ITEM_SLINGSHOT);
    if (startSlingshot >= 2)
        Item_Give(NULL, ITEM_BULLET_BAG_40);
    if (startSlingshot >= 3)
        Item_Give(NULL, ITEM_BULLET_BAG_50);

    uint8_t startBombBag = Randomizer_GetSettingValue(RSK_STARTING_BOMB_BAG);
    if (startBombBag >= 1)
        Item_Give(NULL, ITEM_BOMB_BAG_20);
    if (startBombBag >= 2)
        Item_Give(NULL, ITEM_BOMB_BAG_30);
    if (startBombBag >= 3)
        Item_Give(NULL, ITEM_BOMB_BAG_40);

    switch (Randomizer_GetSettingValue(RSK_STARTING_STRENGTH)) {
        case 3:
            Item_Give(NULL, ITEM_GAUNTLETS_GOLD);
            break;
        case 2:
            Item_Give(NULL, ITEM_GAUNTLETS_SILVER);
            break;
        case 1:
            Item_Give(NULL, ITEM_BRACELET);
            break;
    }

    switch (Randomizer_GetSettingValue(RSK_STARTING_SCALE)) {
        case 2:
            Item_Give(NULL, ITEM_SCALE_GOLDEN);
            break;
        case 1:
            Item_Give(NULL, ITEM_SCALE_SILVER);
            break;
    }

    switch (Randomizer_GetSettingValue(RSK_STARTING_WALLET)) {
        case 2:
            Item_Give(NULL, ITEM_WALLET_GIANT);
            break;
        case 1:
            Item_Give(NULL, ITEM_WALLET_ADULT);
            break;
    }

    uint8_t startMagic = Randomizer_GetSettingValue(RSK_STARTING_MAGIC_METER);
    if (startMagic > 0) {
        gSaveContext.isMagicAcquired = true;
        gSaveContext.isDoubleMagicAcquired = startMagic >= 2;
        gSaveContext.magicLevel = startMagic;
        gSaveContext.magicCapacity = startMagic * MAGIC_NORMAL_METER;
        gSaveContext.magic = static_cast<s8>(gSaveContext.magicCapacity);
    }

    uint8_t startBombchu = Randomizer_GetSettingValue(RSK_STARTING_BOMBCHU_BAG);
    if (startBombchu > 0) {
        uint8_t bombchuMode = Randomizer_GetSettingValue(RSK_BOMBCHU_BAG);
        if (bombchuMode == RO_BOMBCHU_BAG_SINGLE) {
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) = 20;
        } else if (bombchuMode == RO_BOMBCHU_BAG_PROGRESSIVE) {
            static const uint8_t bombchuCapacities[] = { 0, 20, 30, 50 };
            gSaveContext.ship.quest.data.randomizer.bombchuUpgradeLevel = startBombchu;
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) = bombchuCapacities[startBombchu];
        }
    }

    // Big poe bottles first: Item_Give for a bottled content fills the first empty-bottle
    // slot, so each poe is paired with the bottle given right before it. Ruto's Letter fills
    // an empty inventory slot on its own. The remaining plain empty bottles follow.
    uint8_t emptyBottles = 0;
    for (RandomizerSettingKey bottleKey :
         { RSK_STARTING_BOTTLE_1, RSK_STARTING_BOTTLE_2, RSK_STARTING_BOTTLE_3, RSK_STARTING_BOTTLE_4 }) {
        uint8_t bottle = Randomizer_GetSettingValue(bottleKey);
        switch (bottle) {
            case RO_STARTING_BOTTLE_OFF:
                break;
            case RO_STARTING_BOTTLE_EMPTY:
                emptyBottles++;
                break;
            case RO_STARTING_BOTTLE_BIG_POE:
                Item_Give(NULL, ITEM_BOTTLE);
                Item_Give(NULL, ITEM_BIG_POE);
                break;
            case RO_STARTING_BOTTLE_RUTOS_LETTER:
                Item_Give(NULL, ITEM_LETTER_RUTO);
                break;
            default:
                SPDLOG_ERROR("[SetStartingItems] Unhandled value for bottleKey {}: {}", (int)bottleKey, bottle);
                assert(false);
                break;
        }
    }
    for (uint8_t i = 0; i < emptyBottles; i++) {
        Item_Give(NULL, ITEM_BOTTLE);
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_WEIRD_EGG) && Randomizer_GetSettingValue(RSK_SHUFFLE_WEIRD_EGG)) {
        Item_Give(NULL, ITEM_WEIRD_EGG);
    }
    if (Randomizer_GetSettingValue(RSK_STARTING_CLAIM_CHECK)) {
        Item_Give(NULL, ITEM_CLAIM_CHECK);
    }
    if (Randomizer_GetSettingValue(RSK_STARTING_GERUDO_CARD)) {
        Item_Give(NULL, ITEM_GERUDO_CARD);
    }

    if (Randomizer_GetSettingValue(RSK_STARTING_BUNNY_HOOD)) {
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_BUNNY);
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_NONE) {
            INV_CONTENT(ITEM_TRADE_CHILD) = ITEM_MASK_BUNNY;
        }
    }

    // Giant's Knife and Biggoron's Sword share an item slot, bgsFlag marks unbreakable
    switch (Randomizer_GetSettingValue(RSK_STARTING_BIGGORON_SWORD)) {
        case RO_STARTING_BGS_BIGGORON_SWORD:
            gSaveContext.bgsFlag = true;
            [[fallthrough]];
        case RO_STARTING_BGS_GIANTS_KNIFE:
            Item_Give(NULL, ITEM_SWORD_BGS);
            break;
    }

    if (Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
        GiveLinkRupees(9001);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_MAPANDCOMPASS) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        uint32_t mapBitMask = 1 << 1;
        uint32_t compassBitMask = 1 << 2;
        uint32_t startingDungeonItemsBitMask = mapBitMask | compassBitMask;
        for (int scene = SCENE_DEKU_TREE; scene <= SCENE_ICE_CAVERN; scene++) {
            gSaveContext.inventory.dungeonItems[scene] |= startingDungeonItemsBitMask;
        }
    }

    if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;            // Forest
        gSaveContext.ship.stats.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;           // Forest
        gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;                // Fire
        gSaveContext.ship.stats.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;               // Fire
        gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;              // Water
        gSaveContext.ship.stats.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;             // Water
        gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;            // Spirit
        gSaveContext.ship.stats.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;           // Spirit
        gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;            // Shadow
        gSaveContext.ship.stats.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;           // Shadow
        gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;  // BotW
        gSaveContext.ship.stats.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX; // BotW
        gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUND_SMALL_KEY_MAX;  // GTG
        gSaveContext.ship.stats.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUND_SMALL_KEY_MAX; // GTG
        gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;  // Ganon
        gSaveContext.ship.stats.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX; // Ganon
    } else if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA) {
        // Logic cannot handle vanilla key layout in some dungeons
        // this is because vanilla expects the dungeon major item to be
        // locked behind the keys, which is not always true in rando.
        // We can resolve this by starting with some extra keys.
        if (ResourceMgr_IsSceneMasterQuest(SCENE_SPIRIT_TEMPLE)) {
            // MQ Spirit needs 3 keys.
            gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = 3;
            gSaveContext.ship.stats.dungeonKeys[SCENE_SPIRIT_TEMPLE] = 3;
        }
    }

    if (Randomizer_GetSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
        gSaveContext.inventory.dungeonItems[SCENE_FOREST_TEMPLE] |= 1; // Forest
        gSaveContext.inventory.dungeonItems[SCENE_FIRE_TEMPLE] |= 1;   // Fire
        gSaveContext.inventory.dungeonItems[SCENE_WATER_TEMPLE] |= 1;  // Water
        gSaveContext.inventory.dungeonItems[SCENE_SPIRIT_TEMPLE] |= 1; // Spirit
        gSaveContext.inventory.dungeonItems[SCENE_SHADOW_TEMPLE] |= 1; // Shadow
    }

    if (Randomizer_GetSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_STARTWITH) {
        gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER] |= 1;
    }
}

extern "C" void Randomizer_InitSaveFile() {
    auto ctx = Rando::Context::GetInstance();
    ctx->GetLogic()->SetSaveContext(&gSaveContext);

    // Starts pending ice traps out at 0 before potentially incrementing them down the line.
    gSaveContext.ship.pendingIceTrapCount = 0;

    // Reset triforce pieces collected.
    gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected = 0;

    // Reset Bombchu Bag Upgrade
    gSaveContext.ship.quest.data.randomizer.bombchuUpgradeLevel = 0;

    SetStartingItems();

    // Set Cutscene flags and texts to skip them.
    Flags_SetEventChkInf(EVENTCHKINF_FIRST_SPOKE_TO_MIDO);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_KAEPORA_IN_LAKE_HYLIA);
    Flags_SetEventChkInf(EVENTCHKINF_SHEIK_SPAWNED_AT_MASTER_SWORD_PEDESTAL);
    Flags_SetEventChkInf(EVENTCHKINF_RENTED_HORSE_FROM_INGO);
    Flags_SetInfTable(INFTABLE_SPOKE_TO_POE_COLLECTOR_IN_RUINED_MARKET);
    Flags_SetEventChkInf(EVENTCHKINF_WATCHED_GANONS_CASTLE_COLLAPSE_CAUGHT_BY_GERUDO);

    if (Randomizer_GetSettingValue(RSK_FOREST) == RO_CLOSED_FOREST_OFF) {
        Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
        Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_MIDO_AFTER_DEKU_TREES_DEATH);
    }

    // Go away Ruto (Water Temple first cutscene).
    gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x10);

    if (Randomizer_GetSettingValue(RSK_STARTING_BEANS)) {
        INV_CONTENT(ITEM_BEAN) = ITEM_BEAN;
        if (Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_BEANS_ONLY &&
            Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_ALL) {
            BEANS_BOUGHT = 10;
        }
        if (Randomizer_GetSettingValue(RSK_SKIP_PLANTING_BEANS)) {
            AMMO(ITEM_BEAN) = 0;
            gSaveContext.sceneFlags[SCENE_DEATH_MOUNTAIN_CRATER].swch |= (1 << 3);
            gSaveContext.sceneFlags[SCENE_DEATH_MOUNTAIN_TRAIL].swch |= (1 << 6);
            gSaveContext.sceneFlags[SCENE_DESERT_COLOSSUS].swch |= (1 << 24);
            gSaveContext.sceneFlags[SCENE_GERUDO_VALLEY].swch |= (1 << 3);
            gSaveContext.sceneFlags[SCENE_GRAVEYARD].swch |= (1 << 3);
            gSaveContext.sceneFlags[SCENE_KOKIRI_FOREST].swch |= (1 << 9);
            gSaveContext.sceneFlags[SCENE_LAKE_HYLIA].swch |= (1 << 1);
            gSaveContext.sceneFlags[SCENE_LOST_WOODS].swch |= (1 << 4) | (1 << 18);
            gSaveContext.sceneFlags[SCENE_ZORAS_RIVER].swch |= (1 << 3);
        } else {
            AMMO(ITEM_BEAN) = 10;
        }
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_BEAN_SOULS) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_DEATH_MOUNTAIN_CRATER_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_DESERT_COLOSSUS_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_GERUDO_VALLEY_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_GRAVEYARD_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_KOKIRI_FOREST_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_LAKE_HYLIA_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_LOST_WOODS_BRIDGE_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_LOST_WOODS_BEAN_SOUL);
        Flags_SetRandomizerInf(RAND_INF_ZORAS_RIVER_BEAN_SOUL);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OCARINA_BUTTONS) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_HAS_OCARINA_A);
        Flags_SetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT);
        Flags_SetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT);
        Flags_SetRandomizerInf(RAND_INF_HAS_OCARINA_C_UP);
        Flags_SetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_SWIM) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_CAN_SWIM);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_GRAB) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_CAN_GRAB);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_CLIMB) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_CAN_CLIMB);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_CRAWL) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_CAN_CRAWL);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_SPEAK) == RO_GENERIC_OFF) {
        Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_NABOORU_IN_SPIRIT_TEMPLE);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_DEKU);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_GERUDO);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_GORON);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_HYLIAN);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_KOKIRI);
        Flags_SetRandomizerInf(RAND_INF_CAN_SPEAK_ZORA);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OPEN_CHEST) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_CAN_OPEN_CHEST);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_CHILD_WALLET) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_HAS_WALLET);
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_FISHING_POLE) == RO_GENERIC_OFF) {
        Flags_SetRandomizerInf(RAND_INF_FISHING_POLE_FOUND);
    }

    // Give Link's pocket item
    GiveLinksPocketItem();

    // Remove One Time Scrubs with Scrubsanity off
    if (Randomizer_GetSettingValue(RSK_SHUFFLE_SCRUBS) == RO_SCRUBS_OFF) {
        Flags_SetItemGetInf(ITEMGETINF_DEKU_SCRUB_HEART_PIECE);
        Flags_SetInfTable(INFTABLE_BOUGHT_STICK_UPGRADE);
        Flags_SetInfTable(INFTABLE_BOUGHT_NUT_UPGRADE);
    }

    int startingAge = OTRGlobals::Instance->gRandoContext->GetOption(RSK_SELECTED_STARTING_AGE).Get();
    gSaveContext.savedSceneNum = -1;
    switch (startingAge) {
        case RO_AGE_ADULT: // Adult
            gSaveContext.linkAge = LINK_AGE_ADULT;
            gSaveContext.entranceIndex = ENTR_TEMPLE_OF_TIME_WARP_PAD;
            gSaveContext.cutsceneIndex = 0;
            break;
        case RO_AGE_CHILD: // Child
            gSaveContext.linkAge = LINK_AGE_CHILD;
            break;
        default:
            break;
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS)) {
        // Override the spawn entrance so entrance rando can take control,
        // and to prevent remember save location from breaking initial spawn.
        gSaveContext.entranceIndex = -1;
    }

    for (auto trialFlag : { EVENTCHKINF_COMPLETED_LIGHT_TRIAL, EVENTCHKINF_COMPLETED_FOREST_TRIAL,
                            EVENTCHKINF_COMPLETED_FIRE_TRIAL, EVENTCHKINF_COMPLETED_WATER_TRIAL,
                            EVENTCHKINF_COMPLETED_SPIRIT_TRIAL, EVENTCHKINF_COMPLETED_SHADOW_TRIAL }) {
        if (!OTRGlobals::Instance->gRandomizer->IsTrialRequired(trialFlag)) {
            Flags_SetEventChkInf(trialFlag);
        }
    }

    if (Randomizer_GetSettingValue(RSK_SKIP_CHILD_ZELDA)) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_SONG_FROM_IMPA, (GetItemID)RG_ZELDAS_LULLABY);
        StartingItemGive(getItemEntry, RC_SONG_FROM_IMPA);
        getItemEntry = Randomizer_GetItemFromKnownCheck(RC_HC_MALON_EGG, (GetItemID)RG_WEIRD_EGG);
        StartingItemGive(getItemEntry, RC_HC_ZELDAS_LETTER);
        getItemEntry = Randomizer_GetItemFromKnownCheck(RC_HC_ZELDAS_LETTER, (GetItemID)RG_ZELDAS_LETTER);
        StartingItemGive(getItemEntry, RC_HC_MALON_EGG);

        // Malon/Talon back at ranch.
        Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG);
        Flags_SetRandomizerInf(RAND_INF_WEIRD_EGG);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_WOKEN_IN_CASTLE);
        Flags_SetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);

        // Set "Got Zelda's Letter" flag. Also ensures Saria is back at SFM.
        Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER);
        Flags_SetRandomizerInf(RAND_INF_ZELDAS_LETTER);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_LETTER_ZELDA);

        // Got item from Impa.
        Flags_SetEventChkInf(EVENTCHKINF_LEARNED_ZELDAS_LULLABY);

        gSaveContext.sceneFlags[SCENE_HYRULE_CASTLE].swch |= (1 << 0x4); // Move milk crates in Hyrule Castle to moat.

        // Set this at the end to ensure we always start with the letter.
        // This is for the off chance, we got the Weird Egg from Impa (which should never happen).
        INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_LETTER_ZELDA;
    }

    if (Randomizer_GetSettingValue(RSK_SHUFFLE_MASTER_SWORD) && startingAge == RO_AGE_ADULT) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_TOT_MASTER_SWORD, GI_NONE);
        StartingItemGive(getItemEntry, RC_TOT_MASTER_SWORD);
        Flags_SetRandomizerInf(RAND_INF_TOT_MASTER_SWORD);
    }

    HIGH_SCORE(HS_POE_POINTS) = 1000 - (100 * Randomizer_GetSettingValue(RSK_BIG_POE_COUNT));

    // Open lowest Vanilla Fire Temple locked door (to prevent key logic lockouts).
    // Not done on Keysanity since this lockout is a non-issue when Fire Keys can be found outside the temple.
    u8 keysanity = Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                   Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                   Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
    if (!ResourceMgr_IsSceneMasterQuest(SCENE_FIRE_TEMPLE) && !keysanity) {
        gSaveContext.sceneFlags[SCENE_FIRE_TEMPLE].swch |= (1 << 0x17);
    }

    // Opens locked Water Temple door in vanilla to prevent softlocks.
    // West door on the middle level that leads to the water raising thing.
    // Happens in 3DS rando and N64 rando as well.
    if (!ResourceMgr_IsSceneMasterQuest(SCENE_WATER_TEMPLE)) {
        gSaveContext.sceneFlags[SCENE_WATER_TEMPLE].swch |= (1 << 0x15);
    }

    int doorOfTime = Randomizer_GetSettingValue(RSK_DOOR_OF_TIME);
    switch (doorOfTime) {
        case RO_DOOROFTIME_OPEN:
            Flags_SetEventChkInf(EVENTCHKINF_OPENED_THE_DOOR_OF_TIME);
            break;
    }

    if (Randomizer_GetSettingValue(RSK_KAK_GATE) == RO_KAK_GATE_OPEN) {
        Flags_SetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD);
        Flags_UnsetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_LETTER_ZELDA);
    }

    if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_CARPENTERS_FAST ||
        Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_CARPENTERS_FREE) {
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

    if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_CARPENTERS_FREE) {
        Flags_SetEventChkInf(EVENTCHKINF_CARPENTERS_FREE(0));
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x01); // heard yell and unlocked door
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x05);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].swch |= (1 << 0x11);
        gSaveContext.sceneFlags[SCENE_THIEVES_HIDEOUT].collect |= (1 << 0x0C); // picked up key

        Flags_SetRandomizerInf(RAND_INF_TH_ITEM_FROM_LEADER_OF_FORTRESS);
        if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
            Item_Give(NULL, ITEM_GERUDO_CARD);
        }
    }

    // complete mask quest
    if (Randomizer_GetSettingValue(RSK_MASK_QUEST) == RO_MASK_QUEST_COMPLETED) {
        Flags_SetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK);
        Flags_SetEventChkInf(EVENTCHKINF_PAID_BACK_BUNNY_HOOD_FEE);

        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_KEATON);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_SKULL);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_SPOOKY);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_BUNNY);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_GORON);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_ZORA);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_GERUDO);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_MASK_TRUTH);

        gSaveContext.itemGetInf[3] |= 0x100;  // Sold Keaton Mask
        gSaveContext.itemGetInf[3] |= 0x200;  // Sold Skull Mask
        gSaveContext.itemGetInf[3] |= 0x400;  // Sold Spooky Mask
        gSaveContext.itemGetInf[3] |= 0x800;  // Bunny Hood related
        gSaveContext.itemGetInf[3] |= 0x8000; // Obtained Mask of Truth
    }
}
