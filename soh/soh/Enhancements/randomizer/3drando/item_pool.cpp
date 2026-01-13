#include "item_pool.hpp"

#include "../dungeon.h"
#include "fill.hpp"
#include "../static_data.h"
#include "../SeedContext.h"
#include "pool_functions.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "z64item.h"
#include <spdlog/spdlog.h>

std::vector<RandomizerGet> itemPool = {};
std::vector<RandomizerGet> lesserPool = {};
std::vector<RandomizerGet> plentifulPool = {};
std::vector<RandomizerGet> junkPool = {};
const std::array<RandomizerGet, 13> JunkPoolItems = {
    RG_BOMBS_5,  RG_BOMBS_10,  RG_BOMBS_20,  RG_DEKU_NUTS_5, RG_DEKU_STICK_1, RG_DEKU_SEEDS_30, RG_RECOVERY_HEART,
    RG_ARROWS_5, RG_ARROWS_10, RG_ARROWS_30, RG_BLUE_RUPEE,  RG_RED_RUPEE,    RG_DEKU_NUTS_10,
};
// RANDOTODO should probably check the same thing as check matches contents at some point
const std::map<RandomizerGet, std::vector<RandomizerGet>*> poolForItem = {
    { RG_BOMBS_5, &junkPool },         { RG_BOMBS_10, &junkPool },     { RG_BOMBS_20, &junkPool },
    { RG_DEKU_NUTS_5, &junkPool },     { RG_DEKU_STICK_1, &junkPool }, { RG_DEKU_SEEDS_30, &junkPool },
    { RG_RECOVERY_HEART, &junkPool },  { RG_ARROWS_5, &junkPool },     { RG_ARROWS_10, &junkPool },
    { RG_ARROWS_30, &junkPool },       { RG_GREEN_RUPEE, &junkPool },  { RG_BLUE_RUPEE, &junkPool },
    { RG_RED_RUPEE, &junkPool },       { RG_DEKU_NUTS_10, &junkPool }, { RG_TREASURE_GAME_GREEN_RUPEE, &junkPool },
    { RG_PURPLE_RUPEE, &lesserPool },  { RG_HUGE_RUPEE, &lesserPool }, { RG_DEKU_SHIELD, &lesserPool },
    { RG_HYLIAN_SHIELD, &lesserPool }, { RG_BOMBCHU_5, &lesserPool },  { RG_BOMBCHU_10, &lesserPool },
    { RG_BOMBCHU_20, &lesserPool }
};

void AddItemToPool(RandomizerGet item, int plentifulCount, size_t balancedCount, size_t scarceCount = 1,
                   size_t minimalCount = 1, bool iceTrapModel = true) {
    int count = balancedCount;
    switch (ctx->GetOption(RSK_ITEM_POOL).Get()) {
        case RO_ITEM_POOL_SCARCE:
            count = scarceCount;
            break;
        case RO_ITEM_POOL_MINIMAL:
            count = minimalCount;
            break;
        default:
            break;
    }
    if (!poolForItem.contains(item)) {
        itemPool.insert(itemPool.end(), count, item);
        if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            plentifulPool.insert(plentifulPool.end(), plentifulCount - count, item);
        }
        if (iceTrapModel && count > 0 && item != RG_ICE_TRAP) {
            ctx->possibleIceTrapModels.insert(item);
        }
    } else {
        poolForItem.at(item)->insert(poolForItem.at(item)->end(), count, item);
    }
}

void AddFixedItemToPool(RandomizerGet item, int count = 1, bool iceTrapModel = true) {
    if (!poolForItem.contains(item)) {
        itemPool.insert(itemPool.end(), count, item);
        if (iceTrapModel && count > 0 && item != RG_ICE_TRAP) {
            ctx->possibleIceTrapModels.insert(item);
        }
    } else {
        poolForItem.at(item)->insert(poolForItem.at(item)->end(), count, item);
    }
}

RandomizerGet GetJunkItem() {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_ICE_TRAP_PERCENT).IsNot(0) &&
        (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Is(100) || Random(0, 100) < ctx->GetOption(RSK_ICE_TRAP_PERCENT).Get())) {
        return RG_ICE_TRAP;
    }
    return RandomElement(JunkPoolItems);
}

// Replace junk items in the pool with pending junk
static void ReplaceMaxItem(const RandomizerGet itemToReplace, int max) {
    int itemCount = 0;
    for (size_t i = 0; i < itemPool.size(); i++) {
        if (itemPool[i] == itemToReplace) {
            if (itemCount >= max) {
                itemPool[i] = RG_NONE;
            }
            itemCount++;
        }
    }
}

static void PlaceVanillaMapsAndCompasses() {
    auto ctx = Rando::Context::GetInstance();
    for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
        dungeon->PlaceVanillaMap();
        dungeon->PlaceVanillaCompass();
    }
}

static void PlaceVanillaSmallKeys() {
    auto ctx = Rando::Context::GetInstance();
    for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
        dungeon->PlaceVanillaSmallKeys();
    }
}

static void PlaceVanillaBossKeys() {
    auto ctx = Rando::Context::GetInstance();
    for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
        dungeon->PlaceVanillaBossKey();
    }
}

static void PlaceItemsForType(RandomizerCheckType rctype, bool overworldActive = true, bool dungeonActive = true) {
    if (!(overworldActive || dungeonActive)) {
        return;
    }
    for (RandomizerCheck rc : ctx->GetLocations(ctx->allLocations, rctype)) {
        auto loc = Rando::StaticData::GetLocation(rc);

        // If item is in the overworld and shuffled, add its item to the pool
        if (loc->IsOverworld()) {
            if (overworldActive) {
                AddFixedItemToPool(loc->GetVanillaItem(), 1, false);
            }
        } else {
            if (dungeonActive) {
                // If the same in MQ and vanilla, add.
                RandomizerCheckQuest currentQuest = loc->GetQuest();
                if (currentQuest == RCQUEST_BOTH) {
                    AddFixedItemToPool(loc->GetVanillaItem(), 1, false);
                } else {
                    // Check if current item's dungeon is vanilla or MQ, and only add if quest corresponds to it.
                    SceneID itemScene = loc->GetScene();

                    if (itemScene >= SCENE_DEKU_TREE && itemScene <= SCENE_GERUDO_TRAINING_GROUND) {
                        bool isMQ = ctx->GetDungeon(itemScene)->IsMQ();

                        if ((isMQ && currentQuest == RCQUEST_MQ) || (!isMQ && currentQuest == RCQUEST_VANILLA)) {
                            AddFixedItemToPool(loc->GetVanillaItem(), 1, false);
                        }
                    }
                }
            }
        }
    }
}

void GenerateItemPool() {
    // RANDOTODO proper removal of items not in pool or logically relevant instead of dummy checks.
    auto ctx = Rando::Context::GetInstance();
    itemPool.clear();
    junkPool.clear();
    plentifulPool.clear();
    lesserPool.clear();
    int reservedSlots = 0;

    // clang-format off
    AddItemToPool(RG_BOOMERANG, 2, 1, 1, 1);
    AddItemToPool(RG_LENS_OF_TRUTH, 2, 1, 1, 1);
    AddItemToPool(RG_MEGATON_HAMMER, 2, 1, 1, 1);
    AddItemToPool(RG_IRON_BOOTS, 2, 1, 1, 1);
    AddItemToPool(RG_GORON_TUNIC, 2, 1, 1, 1);
    AddItemToPool(RG_ZORA_TUNIC, 2, 1, 1, 1);
    AddItemToPool(RG_HOVER_BOOTS, 2, 1, 1, 1);
    AddItemToPool(RG_MIRROR_SHIELD, 2, 1, 1, 1);
    AddItemToPool(RG_STONE_OF_AGONY, 2, 1, 1, 1);
    AddItemToPool(RG_FIRE_ARROWS, 2, 1, 1, 1);
    AddItemToPool(RG_ICE_ARROWS, 2, 1, 1, 1);
    AddItemToPool(RG_LIGHT_ARROWS, 2, 1, 1, 1);
    AddItemToPool(RG_DINS_FIRE, 2, 1, 1, 1);
    AddItemToPool(RG_NAYRUS_LOVE, 2, 1, 0, 0);
    AddItemToPool(RG_GREG_RUPEE, 1, 1, 1, 1);
    AddItemToPool(RG_PROGRESSIVE_HOOKSHOT, 2, 2, 2, 2);
    AddItemToPool(RG_HYLIAN_SHIELD, 1, 1, 1, 1);
    AddItemToPool(RG_PROGRESSIVE_STRENGTH, 4, 3, 3, 3);
    AddItemToPool(RG_DOUBLE_DEFENSE, 2, 1, 0, 0);
    AddItemToPool(RG_BIGGORON_SWORD, 2, 1, 1, 0);
    bool isScrubs = ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL);
    AddFixedItemToPool(RG_DEKU_SHIELD, isScrubs ? 1 : 2);
    AddFixedItemToPool(RG_RECOVERY_HEART, isScrubs ? 6 : 11);
    AddFixedItemToPool(RG_BOMBS_5, isScrubs ? 2 : 8);
    AddFixedItemToPool(RG_DEKU_STICK_1, isScrubs ? 0 : 2);
    AddFixedItemToPool(RG_BOMBS_10, 1);
    AddFixedItemToPool(RG_BOMBS_20, 1);
    AddFixedItemToPool(RG_ARROWS_5, 1);
    AddFixedItemToPool(RG_ARROWS_10, 3);

    if (isScrubs) {
        AddFixedItemToPool(RG_DEKU_NUTS_5, ctx->GetDungeon(Rando::JABU_JABUS_BELLY)->IsVanilla() ? 5 : 6);
        // Scrubs which sell seeds or arrows sell it based on age, this randomly assigns them
        for (uint8_t i = 0; i < 7; i++) {
            if (Random(0, 2)) {
                AddFixedItemToPool(RG_ARROWS_30);
            } else {
                AddFixedItemToPool(RG_DEKU_SEEDS_30);
            }
        }
    }

    int infiniteProgressive = ctx->GetOption(RSK_INFINITE_UPGRADES).Is(RO_INF_UPGRADES_PROGRESSIVE) ? 1 : 0;
    AddItemToPool(RG_PROGRESSIVE_BOW, 4 + infiniteProgressive, 
                                      3 + infiniteProgressive, 
                                      2 + infiniteProgressive,
                                      1 + infiniteProgressive);
    AddItemToPool(RG_PROGRESSIVE_SLINGSHOT, 4 + infiniteProgressive, 
                                            3 + infiniteProgressive, 
                                            2 + infiniteProgressive,
                                            1 + infiniteProgressive);
    AddItemToPool(RG_PROGRESSIVE_BOMB_BAG,  4 + infiniteProgressive, 
                                            3 + infiniteProgressive, 
                                            2 + infiniteProgressive,
                                            1 + infiniteProgressive);
    AddItemToPool(RG_PROGRESSIVE_MAGIC_METER, 3 + infiniteProgressive, 
                                              2 + infiniteProgressive, 
                                              1 + infiniteProgressive,
                                              1 + infiniteProgressive);
    //clang-format on

    int extraWallets =(ctx->GetOption(RSK_SHUFFLE_CHILD_WALLET) ? 1 : 0) + (ctx->GetOption(RSK_INCLUDE_TYCOON_WALLET) ? 1 : 0);
    AddItemToPool(RG_PROGRESSIVE_WALLET, 3 + infiniteProgressive + extraWallets, 
                                         2 + infiniteProgressive + extraWallets,
                                         2 + infiniteProgressive + extraWallets,
                                         2 + infiniteProgressive + extraWallets);

    int stickShuffle = ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG) ? 1 : 0;
    AddItemToPool(RG_PROGRESSIVE_STICK_UPGRADE, 3 + infiniteProgressive + stickShuffle,
                                                2 + infiniteProgressive + stickShuffle,
                                                1 + infiniteProgressive + stickShuffle,
                                                0 + infiniteProgressive + stickShuffle);

    int nutShuffle = ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG) ? 1 : 0;
    AddItemToPool(RG_PROGRESSIVE_NUT_UPGRADE, 3 + infiniteProgressive + nutShuffle,
                                              2 + infiniteProgressive + nutShuffle,
                                              1 + infiniteProgressive + nutShuffle,
                                              0 + infiniteProgressive + nutShuffle);

    if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_SINGLE)) {
        AddItemToPool(RG_PROGRESSIVE_BOMBCHU_BAG, 6, 5, 3, 1);
    } else if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_PROGRESSIVE)) {
        AddItemToPool(RG_PROGRESSIVE_BOMBCHU_BAG,  4 + infiniteProgressive, 
                                                   3 + infiniteProgressive, 
                                                   2 + infiniteProgressive,
                                                   1 + infiniteProgressive);
    } else {
        AddItemToPool(RG_BOMBCHU_20, 2, 1, 0, 0);
        AddItemToPool(RG_BOMBCHU_10, 3, 3, 2, 0);
        AddItemToPool(RG_BOMBCHU_5, 1, 1, 1, 1);
    }

    // add extra songs only if song shuffle is anywhere
    if (ctx->GetOption(RSK_SHUFFLE_SONGS).IsNot(RO_SONG_SHUFFLE_OFF)) {
        bool songAnywhere = ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_ANYWHERE);
        if (!ctx->GetOption(RSK_STARTING_ZELDAS_LULLABY).Get()) {
            AddItemToPool(RG_ZELDAS_LULLABY, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_EPONAS_SONG).Get()) {
            AddItemToPool(RG_EPONAS_SONG, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_SARIAS_SONG).Get()) {
            AddItemToPool(RG_SARIAS_SONG, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_SUNS_SONG).Get()) {
            AddItemToPool(RG_SUNS_SONG, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_SONG_OF_TIME).Get()) {
            AddItemToPool(RG_SONG_OF_TIME, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_SONG_OF_STORMS).Get()) {
            AddItemToPool(RG_SONG_OF_STORMS, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_MINUET_OF_FOREST).Get()) {
            AddItemToPool(RG_MINUET_OF_FOREST, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_BOLERO_OF_FIRE).Get()) {
            AddItemToPool(RG_BOLERO_OF_FIRE, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_SERENADE_OF_WATER).Get()) {
            AddItemToPool(RG_SERENADE_OF_WATER, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_REQUIEM_OF_SPIRIT).Get()) {
            AddItemToPool(RG_REQUIEM_OF_SPIRIT, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_NOCTURNE_OF_SHADOW).Get()) {
            AddItemToPool(RG_NOCTURNE_OF_SHADOW, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
        if (!ctx->GetOption(RSK_STARTING_PRELUDE_OF_LIGHT).Get()) {
            AddItemToPool(RG_PRELUDE_OF_LIGHT, songAnywhere ? 2 : 1, 1, 1, 1, songAnywhere);
        }
    } else {
        ctx->PlaceItemInLocation(RC_SHEIK_IN_FOREST, RG_MINUET_OF_FOREST, false, true);
        ctx->PlaceItemInLocation(RC_SHEIK_IN_CRATER, RG_BOLERO_OF_FIRE, false, true);
        ctx->PlaceItemInLocation(RC_SHEIK_IN_ICE_CAVERN, RG_SERENADE_OF_WATER, false, true);
        ctx->PlaceItemInLocation(RC_SHEIK_AT_COLOSSUS, RG_REQUIEM_OF_SPIRIT, false, true);
        ctx->PlaceItemInLocation(RC_SHEIK_IN_KAKARIKO, RG_NOCTURNE_OF_SHADOW, false, true);
        ctx->PlaceItemInLocation(RC_SHEIK_AT_TEMPLE, RG_PRELUDE_OF_LIGHT, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_IMPA, RG_ZELDAS_LULLABY, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_MALON, RG_EPONAS_SONG, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_SARIA, RG_SARIAS_SONG, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_ROYAL_FAMILYS_TOMB, RG_SUNS_SONG, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_OCARINA_OF_TIME, RG_SONG_OF_TIME, false, true);
        ctx->PlaceItemInLocation(RC_SONG_FROM_WINDMILL, RG_SONG_OF_STORMS, false, true);
    }

    bool rewardIceTraps = ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Get() >= RO_DUNGEON_REWARDS_ANY_DUNGEON;
    AddFixedItemToPool(RG_KOKIRI_EMERALD, 1, rewardIceTraps);
    AddFixedItemToPool(RG_GORON_RUBY, 1, rewardIceTraps);
    AddFixedItemToPool(RG_ZORA_SAPPHIRE, 1, rewardIceTraps);
    AddFixedItemToPool(RG_FOREST_MEDALLION, 1, rewardIceTraps);
    AddFixedItemToPool(RG_FIRE_MEDALLION, 1, rewardIceTraps);
    AddFixedItemToPool(RG_WATER_MEDALLION, 1, rewardIceTraps);
    AddFixedItemToPool(RG_SPIRIT_MEDALLION, 1, rewardIceTraps);
    AddFixedItemToPool(RG_SHADOW_MEDALLION, 1, rewardIceTraps);
    AddFixedItemToPool(RG_LIGHT_MEDALLION, 1, rewardIceTraps);

    if (ctx->GetOption(RSK_TRIFORCE_HUNT).IsNot(RO_TRIFORCE_HUNT_OFF)) {
        AddFixedItemToPool(RG_TRIFORCE_PIECE, ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_TOTAL).Get() + 1, false);

        switch (ctx->GetOption(RSK_TRIFORCE_HUNT).Get()) {
            case RO_TRIFORCE_HUNT_OFF:
                break;
            case RO_TRIFORCE_HUNT_WIN:
                ctx->PlaceItemInLocation(RC_TRIFORCE_COMPLETED, RG_TRIFORCE); // Win condition
                ctx->PlaceItemInLocation(RC_GANON, RG_BLUE_RUPEE, false, true);
                break;
            case RO_TRIFORCE_HUNT_GBK:
                ctx->PlaceItemInLocation(RC_TRIFORCE_COMPLETED, RG_GANONS_CASTLE_BOSS_KEY);
                ctx->PlaceItemInLocation(RC_GANON, RG_TRIFORCE); // Win condition
                break;
        }
    } else {
        ctx->PlaceItemInLocation(RC_GANON, RG_TRIFORCE); // Win condition
    }

    // Fixed item locations
    ctx->PlaceItemInLocation(RC_HC_ZELDAS_LETTER, RG_ZELDAS_LETTER);

    if (!ctx->GetOption(RSK_STARTING_KOKIRI_SWORD)) {
        if (ctx->GetOption(RSK_SHUFFLE_KOKIRI_SWORD)) {
            AddItemToPool(RG_KOKIRI_SWORD, 2, 1, 1, 1);
        } else {
            ctx->PlaceItemInLocation(RC_KF_KOKIRI_SWORD_CHEST, RG_KOKIRI_SWORD, false, true);
        }
    }

    if (!ctx->GetOption(RSK_STARTING_MASTER_SWORD)) {
        if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
            AddItemToPool(RG_MASTER_SWORD, 2, 1, 1, 1);
        } else {
            ctx->PlaceItemInLocation(RC_TOT_MASTER_SWORD, RG_MASTER_SWORD, false, true);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_WEIRD_EGG)) {
        AddItemToPool(RG_WEIRD_EGG, 2, 1, 1, 1);
    } else {
        ctx->PlaceItemInLocation(RC_HC_MALON_EGG, RG_WEIRD_EGG, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_OCARINA)) {
        if (ctx->GetOption(RSK_STARTING_OCARINA).IsNot(RO_STARTING_OCARINA_TIME)) {
            int baseOcarinas = ctx->GetOption(RSK_STARTING_OCARINA).Is(RO_STARTING_OCARINA_OFF) ? 2 : 1;
            AddItemToPool(RG_PROGRESSIVE_OCARINA, baseOcarinas + 1, baseOcarinas, baseOcarinas, baseOcarinas);
        }
    } else {
        if (ctx->GetOption(RSK_STARTING_OCARINA).Is(RO_STARTING_OCARINA_OFF)) {
            ctx->PlaceItemInLocation(RC_LW_GIFT_FROM_SARIA, RG_PROGRESSIVE_OCARINA, false, true);
            ctx->PlaceItemInLocation(RC_HF_OCARINA_OF_TIME_ITEM, RG_PROGRESSIVE_OCARINA, false, true);
        } else {
            if (ctx->GetOption(RSK_STARTING_OCARINA).IsNot(RO_STARTING_OCARINA_TIME)) {
                ctx->PlaceItemInLocation(RC_HF_OCARINA_OF_TIME_ITEM, RG_PROGRESSIVE_OCARINA, false, true);
            }
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_OCARINA_BUTTONS)) {
        AddItemToPool(RG_OCARINA_A_BUTTON, 2, 1, 1, 1);
        AddItemToPool(RG_OCARINA_C_UP_BUTTON, 2, 1, 1, 1);
        AddItemToPool(RG_OCARINA_C_DOWN_BUTTON, 2, 1, 1, 1);
        AddItemToPool(RG_OCARINA_C_LEFT_BUTTON, 2, 1, 1, 1);
        AddItemToPool(RG_OCARINA_C_RIGHT_BUTTON, 2, 1, 1, 1);
    }

    if (ctx->GetOption(RSK_SKELETON_KEY)) {
        AddFixedItemToPool(RG_SKELETON_KEY, 1);
    }

    if (ctx->GetOption(RSK_ROCS_FEATHER)) {
        AddItemToPool(RG_ROCS_FEATHER, 2, 1, 1, 1);
    }

    int bronzeScale = ctx->GetOption(RSK_SHUFFLE_SWIM) ? 1 : 0;
    AddItemToPool(RG_PROGRESSIVE_SCALE, 3 + bronzeScale, 2 + bronzeScale, 2 + bronzeScale, 2 + bronzeScale);

    if (ctx->GetOption(RSK_SHUFFLE_BEEHIVES)) {
        PlaceItemsForType(RCTYPE_BEEHIVE, true, true);
    }

    // Shuffle Pots
    bool overworldPotsActive = ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_OVERWORLD) ||
                               ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_ALL);
    bool dungeonPotsActive = ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_DUNGEONS) ||
                             ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_ALL);
    PlaceItemsForType(RCTYPE_POT, overworldPotsActive, dungeonPotsActive);

    // Shuffle Trees
    bool treesActive = (bool)ctx->GetOption(RSK_SHUFFLE_TREES);
    PlaceItemsForType(RCTYPE_TREE, treesActive, false);
    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC)) {
        PlaceItemsForType(RCTYPE_NLTREE, treesActive, false);
    }

    // Shuffle Bushes
    bool bushesActive = (bool)ctx->GetOption(RSK_SHUFFLE_BUSHES);
    PlaceItemsForType(RCTYPE_BUSH, bushesActive, false);

    // Shuffle Crates
    bool overworldCratesActive = ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_OVERWORLD) ||
                                 ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_ALL);
    bool dungeonCratesActive = ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_DUNGEONS) ||
                               ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_ALL);
    PlaceItemsForType(RCTYPE_CRATE, overworldCratesActive, dungeonCratesActive);
    PlaceItemsForType(RCTYPE_NLCRATE, ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC) && overworldCratesActive,
                      ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC) && dungeonCratesActive);
    PlaceItemsForType(RCTYPE_SMALL_CRATE, overworldCratesActive, dungeonCratesActive);

    if (ctx->GetOption(RSK_FISHSANITY).Is(RO_FISHSANITY_HYRULE_LOACH)) {
        AddFixedItemToPool(RG_PURPLE_RUPEE, 1);
    } else {
        ctx->PlaceItemInLocation(RC_LH_HYRULE_LOACH, RG_PURPLE_RUPEE, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_FISHING_POLE)) {
        AddItemToPool(RG_FISHING_POLE, 2, 1, 1, 1);
    }
    // if beans unshuffled, put on bean guy, otherwise if not starting with beans, add to pool
    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).IsNot(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) &&
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).IsNot(RO_SHUFFLE_MERCHANTS_ALL)) {
        ctx->PlaceItemInLocation(RC_ZR_MAGIC_BEAN_SALESMAN, RG_MAGIC_BEAN, false, true);
    } else if (!ctx->GetOption(RSK_STARTING_BEANS)) {
        AddItemToPool(RG_MAGIC_BEAN_PACK, 2, 1, 1, 1);
    }

    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)) {
        if (/*!ProgressiveGoronSword TODO: Implement Progressive Goron Sword*/ true) {
            AddFixedItemToPool(RG_GIANTS_KNIFE, 1);
        }
        if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_SINGLE)) {
            AddFixedItemToPool(RG_PROGRESSIVE_BOMBCHU_BAG, 1);
        } else if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_NONE)) {
            AddFixedItemToPool(RG_BOMBCHU_10, 1);
        }
    } else {
        ctx->PlaceItemInLocation(RC_KAK_GRANNYS_SHOP, RG_BLUE_POTION_REFILL, false, true);
        ctx->PlaceItemInLocation(RC_GC_MEDIGORON, RG_GIANTS_KNIFE, false, true);
        ctx->PlaceItemInLocation(RC_WASTELAND_BOMBCHU_SALESMAN, RG_BOMBCHU_10, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_FROG_SONG_RUPEES)) {
        AddFixedItemToPool(RG_PURPLE_RUPEE, 5);
    } else {
        ctx->PlaceItemInLocation(RC_ZR_FROGS_ZELDAS_LULLABY, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_EPONAS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SARIAS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SUNS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SONG_OF_TIME, RG_PURPLE_RUPEE, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE)) {
        AddItemToPool(RG_POCKET_EGG, 2, 1, 1, 1);
        AddItemToPool(RG_COJIRO, 2, 1, 1, 1);
        AddItemToPool(RG_ODD_MUSHROOM, 2, 1, 1, 1);
        AddItemToPool(RG_ODD_POTION, 2, 1, 1, 1);
        AddItemToPool(RG_POACHERS_SAW, 2, 1, 1, 1);
        AddItemToPool(RG_BROKEN_SWORD, 2, 1, 1, 1);
        AddItemToPool(RG_PRESCRIPTION, 2, 1, 1, 1);
        AddItemToPool(RG_EYEBALL_FROG, 2, 1, 1, 1);
        AddItemToPool(RG_EYEDROPS, 2, 1, 1, 1);
    }
    AddItemToPool(RG_CLAIM_CHECK, 2, 1, 1, 1);

    if (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS)) {
        AddItemToPool(RG_TREASURE_GAME_SMALL_KEY, 7, 6, 6, 6);
    } else if (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK)) {
        AddItemToPool(RG_TREASURE_GAME_KEY_RING, 2, 1, 1, 1);
    }

    int tokensToAdd = 0;
    if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OFF)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            ctx->PlaceItemInLocation(loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
        }
    } else if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_DUNGEONS)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            if (Rando::StaticData::GetLocation(loc)->IsOverworld()) {
                ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
            } else {
                tokensToAdd++;
            }
        }
    } else if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OVERWORLD)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            if (Rando::StaticData::GetLocation(loc)->IsDungeon()) {
                ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
            } else {
                tokensToAdd++;
            }
        }
    } else {
        tokensToAdd = 100;
    }

    if (ctx->GetOption(RSK_SHUFFLE_100_GS_REWARD)) {
        AddFixedItemToPool(RG_HUGE_RUPEE, 1);
    } else {
        ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_HUGE_RUPEE, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_BEAN_SOULS)) {
        ctx->possibleIceTrapModels.insert(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL); // ice traps reroll this into a random bean soul
        AddItemToPool(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_DESERT_COLOSSUS_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_GERUDO_VALLEY_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_GRAVEYARD_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_KOKIRI_FOREST_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_LAKE_HYLIA_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_LOST_WOODS_BRIDGE_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_LOST_WOODS_BEAN_SOUL, 2, 1, 1, 1, false);
        AddItemToPool(RG_ZORAS_RIVER_BEAN_SOUL, 2, 1, 1, 1, false);
    }

    if (ctx->GetOption(RSK_SHUFFLE_TOKENS).IsNot(RO_TOKENSANITY_OFF) &&
        ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
        tokensToAdd += 10;
    }

    if (ctx->GetOption(RSK_STARTING_SKULLTULA_TOKEN).Get() < tokensToAdd) {
        AddFixedItemToPool(RG_GOLD_SKULLTULA_TOKEN, tokensToAdd - ctx->GetOption(RSK_STARTING_SKULLTULA_TOKEN).Get());
    }

    if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS)) {
        AddItemToPool(RG_GOHMA_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_KING_DODONGO_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_BARINADE_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_PHANTOM_GANON_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_VOLVAGIA_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_MORPHA_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_BONGO_BONGO_SOUL, 2, 1, 1, 1);
        AddItemToPool(RG_TWINROVA_SOUL, 2, 1, 1, 1);
        if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON)) {
            AddItemToPool(RG_GANON_SOUL, 2, 1, 1, 1);
        }
    }

    // Gerudo Fortress
    if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE)) {
        ctx->PlaceItemInLocation(RC_TH_1_TORCH_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_TH_DEAD_END_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_TH_DOUBLE_CELL_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_TH_STEEP_SLOPE_CARPENTER, RG_RECOVERY_HEART, false, true);

    } else if (ctx->GetOption(RSK_GERUDO_KEYS).IsNot(RO_GERUDO_KEYS_VANILLA)) {
        if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)) {
            AddItemToPool(RG_GERUDO_FORTRESS_SMALL_KEY, 2, 1, 1, 1);
            ctx->PlaceItemInLocation(RC_TH_DEAD_END_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_TH_DOUBLE_CELL_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_TH_STEEP_SLOPE_CARPENTER, RG_RECOVERY_HEART, false, true);
        } else {
            // Only add key ring if 4 Fortress keys necessary
            if (ctx->GetOption(RSK_KEYRINGS_GERUDO_FORTRESS) && ctx->GetOption(RSK_KEYRINGS)) {
                AddItemToPool(RG_GERUDO_FORTRESS_KEY_RING, 2, 1, 1, 1);
            } else {
                AddItemToPool(RG_GERUDO_FORTRESS_SMALL_KEY, 5, 4, 4, 4);
            }
        }

    } else {
        if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)) {
            ctx->PlaceItemInLocation(RC_TH_1_TORCH_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_TH_DEAD_END_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_TH_DOUBLE_CELL_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_TH_STEEP_SLOPE_CARPENTER, RG_RECOVERY_HEART, false, true);
        } else {
            ctx->PlaceItemInLocation(RC_TH_1_TORCH_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_TH_DEAD_END_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_TH_DOUBLE_CELL_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_TH_STEEP_SLOPE_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
        }
    }

    // Gerudo Membership Card
    if (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
        AddItemToPool(RG_GERUDO_MEMBERSHIP_CARD, 2, 1, 1, 1);
        if (ctx->GetOption(RSK_GERUDO_FORTRESS).IsNot(RO_GF_CARPENTERS_FREE)) {
            ctx->PlaceItemInLocation(RC_TH_FREED_CARPENTERS, RG_BLUE_RUPEE, false, true);
        }
    } else {
        ctx->PlaceItemInLocation(RC_TH_FREED_CARPENTERS, RG_GERUDO_MEMBERSHIP_CARD, false, true);
    }

    // Keys
    if (ctx->GetOption(RSK_LOCK_OVERWORLD_DOORS)) {
        // only 1 is added to the ice trap pool, to avoid the pool being filled with them.
        // a random one is chosen in CreateItemOverrides
        AddItemToPool(RG_GUARD_HOUSE_KEY, 2, 1, 1, 1);
        AddItemToPool(RG_MARKET_BAZAAR_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_MARKET_POTION_SHOP_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_MASK_SHOP_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_MARKET_SHOOTING_GALLERY_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_BOMBCHU_BOWLING_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_TREASURE_CHEST_GAME_BUILDING_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_BOMBCHU_SHOP_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_RICHARDS_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_ALLEY_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_KAK_BAZAAR_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_KAK_POTION_SHOP_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_BOSS_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_GRANNYS_POTION_SHOP_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_SKULLTULA_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_IMPAS_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_WINDMILL_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_KAK_SHOOTING_GALLERY_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_DAMPES_HUT_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_TALONS_HOUSE_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_STABLES_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_BACK_TOWER_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_HYLIA_LAB_KEY, 2, 1, 1, 1, false);
        AddItemToPool(RG_FISHING_HOLE_KEY, 2, 1, 1, 1, false);
    }

    if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaSmallKeys();
    } else if (ctx->GetOption(RSK_KEYSANITY).IsNot(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->HasKeyRing()) {
                AddItemToPool(dungeon->GetKeyRing(), 2, 1, 1, 1);
            } else if (dungeon->GetSmallKeyCount() > 0) {
                int smallKeys = dungeon->GetSmallKeyCount();
                AddItemToPool(dungeon->GetSmallKey(), smallKeys + 1, smallKeys, smallKeys, smallKeys);
            }
        }
    }

    if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaBossKeys();
    } else if (ctx->GetOption(RSK_BOSS_KEYSANITY).IsNot(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        AddItemToPool(RG_FOREST_TEMPLE_BOSS_KEY, 2, 1, 1, 1);
        AddItemToPool(RG_FIRE_TEMPLE_BOSS_KEY, 2, 1, 1, 1);
        AddItemToPool(RG_WATER_TEMPLE_BOSS_KEY, 2, 1, 1, 1);
        AddItemToPool(RG_SPIRIT_TEMPLE_BOSS_KEY, 2, 1, 1, 1);
        AddItemToPool(RG_SHADOW_TEMPLE_BOSS_KEY, 2, 1, 1, 1);
    }

    // Don't add GBK to the pool at all for Triforce Hunt or if we start with it.
    if (!(ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH) || ctx->GetOption(RSK_TRIFORCE_HUNT))) {
        if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
            ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_GANONS_CASTLE_BOSS_KEY);
        } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Get() >= RO_GANON_BOSS_KEY_LACS_VANILLA) {
            ctx->PlaceItemInLocation(RC_TOT_LIGHT_ARROWS_CUTSCENE, RG_GANONS_CASTLE_BOSS_KEY);
        } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
            ctx->PlaceItemInLocation(RC_GANONS_TOWER_BOSS_KEY_CHEST, RG_GANONS_CASTLE_BOSS_KEY);
        } else {
            AddItemToPool(RG_GANONS_CASTLE_BOSS_KEY, 2, 1, 1, 1);
        }
    }

    // Shopsanity
    if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_OFF) ||
        (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_SPECIFIC_COUNT) &&
         ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_ZERO_ITEMS))) {
        AddFixedItemToPool(RG_BLUE_RUPEE, 13);
        AddFixedItemToPool(RG_RED_RUPEE, 5);
        AddFixedItemToPool(RG_PURPLE_RUPEE, 7);
        AddFixedItemToPool(RG_HUGE_RUPEE, 3);
    } else {
        // Shopsanity gets extra large rupees
        AddFixedItemToPool(RG_BLUE_RUPEE, 2);
        AddFixedItemToPool(RG_RED_RUPEE, 10);
        AddFixedItemToPool(RG_PURPLE_RUPEE, 10);
        AddFixedItemToPool(RG_HUGE_RUPEE, 6);
    }

    bool overworldFreeStandingActive = ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_OVERWORLD) ||
                                       ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_ALL);
    bool dungeonFreeStandingActive = ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_DUNGEONS) ||
                                     ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_ALL);
    PlaceItemsForType(RCTYPE_FREESTANDING, overworldFreeStandingActive, dungeonFreeStandingActive);

    // Dungeon pools
    if (ctx->GetDungeon(Rando::DEKU_TREE)->IsMQ()) {
        AddFixedItemToPool(RG_PURPLE_RUPEE);
        if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
            AddFixedItemToPool(RG_DEKU_SHIELD, 3);
        } else {
            AddFixedItemToPool(RG_DEKU_SHIELD, 2);
        }
    } else {
        AddFixedItemToPool(RG_RECOVERY_HEART, 2);
    }
    if (ctx->GetDungeon(Rando::DODONGOS_CAVERN)->IsMQ()) {
        AddFixedItemToPool(RG_HYLIAN_SHIELD);
        AddFixedItemToPool(RG_BLUE_RUPEE);
        if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
            AddFixedItemToPool(RG_RECOVERY_HEART);
        }
    } else {
        AddFixedItemToPool(RG_RED_RUPEE);
        if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
            AddFixedItemToPool(RG_DEKU_NUTS_5);
        }
    }
    if (ctx->GetDungeon(Rando::JABU_JABUS_BELLY)->IsMQ()) {
        AddFixedItemToPool(RG_DEKU_NUTS_5, 4);
        AddFixedItemToPool(RG_RECOVERY_HEART);
        AddFixedItemToPool(RG_DEKU_STICK_1);
        AddFixedItemToPool(RG_DEKU_SHIELD);
    }
    if (ctx->GetDungeon(Rando::FOREST_TEMPLE)->IsMQ()) {
        AddFixedItemToPool(RG_ARROWS_5);
    } else {
        AddFixedItemToPool(RG_RECOVERY_HEART);
        AddFixedItemToPool(RG_ARROWS_10);
        AddFixedItemToPool(RG_ARROWS_30);
    }
    if (ctx->GetDungeon(Rando::FIRE_TEMPLE)->IsMQ()) {
        AddFixedItemToPool(RG_HYLIAN_SHIELD);
        AddFixedItemToPool(RG_BOMBS_20);
    } else {
        AddFixedItemToPool(RG_HUGE_RUPEE);
    }
    if (ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->IsMQ()) {
        AddFixedItemToPool(RG_PURPLE_RUPEE, 2);
        AddFixedItemToPool(RG_ARROWS_30);
    } else {
        AddFixedItemToPool(RG_DEKU_SHIELD, 2);
        AddFixedItemToPool(RG_BOMBS_20);
        AddFixedItemToPool(RG_RECOVERY_HEART, 2);
    }
    if (ctx->GetDungeon(Rando::SHADOW_TEMPLE)->IsMQ()) {
        AddFixedItemToPool(RG_ARROWS_5, 2);
        AddFixedItemToPool(RG_RED_RUPEE);
    } else {
        AddFixedItemToPool(RG_ARROWS_30);
    }
    if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
        AddFixedItemToPool(RG_DEKU_NUTS_5);
        AddFixedItemToPool(RG_DEKU_NUTS_10);
        AddFixedItemToPool(RG_RECOVERY_HEART);
        AddFixedItemToPool(RG_BOMBS_10);
        AddFixedItemToPool(RG_DEKU_SHIELD);
        AddFixedItemToPool(RG_HYLIAN_SHIELD);
        AddFixedItemToPool(RG_HUGE_RUPEE);
    }
    if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsMQ()) {
        AddFixedItemToPool(RG_TREASURE_GAME_GREEN_RUPEE, 2);
        AddFixedItemToPool(RG_ARROWS_10);
        AddFixedItemToPool(RG_GREEN_RUPEE);
        AddFixedItemToPool(RG_PURPLE_RUPEE);
    } else {
        AddFixedItemToPool(RG_HUGE_RUPEE);
        AddFixedItemToPool(RG_ARROWS_30, 3);
    }
    if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsMQ()) {
        AddFixedItemToPool(RG_ARROWS_10, 2);
        AddFixedItemToPool(RG_BOMBS_5);
        AddFixedItemToPool(RG_RED_RUPEE);
        AddFixedItemToPool(RG_RECOVERY_HEART);
        if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
            AddFixedItemToPool(RG_DEKU_NUTS_5);
        }
    } else {
        AddFixedItemToPool(RG_BLUE_RUPEE, 3);
        AddFixedItemToPool(RG_ARROWS_30);
    }

    // Add 4 total bottles
    uint8_t bottleCount = 4;
    if (ctx->GetOption(RSK_ZORAS_FOUNTAIN).IsNot(RO_ZF_OPEN)) {
        AddFixedItemToPool(RG_RUTOS_LETTER);
        bottleCount--;
    }
    if ((ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
         ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
        AddFixedItemToPool(RG_BOTTLE_WITH_BLUE_POTION);
        bottleCount--;
    }

    ctx->possibleIceTrapModels.insert(RG_EMPTY_BOTTLE); // ice traps reroll this into a random normal bottle
    for (uint8_t i = 0; i < bottleCount; i++) {
        AddFixedItemToPool(RandomElement(Rando::StaticData::normalBottles), 1, false);
    }

    /*For item pool generation, dungeon items are either placed in their vanilla
    | location, or added to the pool now and filtered out later depending on when
    | they need to get placed or removed in fill.cpp. These items are kept in the
    | pool until removal because the filling algorithm needs to know all of the
    | advancement items that haven't been placed yet for placing higher priority
    | items like stones/medallions.*/

    if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaMapsAndCompasses();
    } else if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).IsNot(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->GetMap() != RG_NONE) {
                AddFixedItemToPool(dungeon->GetMap(), false);
            }

            if (dungeon->GetCompass() != RG_NONE) {
                AddFixedItemToPool(dungeon->GetCompass(), false);
            }
        }
    }

    int maxHearts = 20;
    switch (ctx->GetOption(RSK_ITEM_POOL).Get()) {
        case RO_ITEM_POOL_PLENTIFUL:
        case RO_ITEM_POOL_BALANCED:
            break;
        case RO_ITEM_POOL_SCARCE:
            maxHearts = 12;
            break;
        case RO_ITEM_POOL_MINIMAL:
            maxHearts = 3;
            break;
    }

    int startingHearts = ctx->GetOption(RSK_STARTING_HEARTS).Get() + 1;
    if (startingHearts < maxHearts) {
        AddFixedItemToPool(RG_TREASURE_GAME_HEART, 1, false);
        AddFixedItemToPool(RG_PIECE_OF_HEART, 3, false);
        startingHearts++;
        if (startingHearts < maxHearts) {
            switch (ctx->GetOption(RSK_ITEM_POOL).Get()) {
                case RO_ITEM_POOL_PLENTIFUL:
                case RO_ITEM_POOL_MINIMAL:
                    AddFixedItemToPool(RG_HEART_CONTAINER, maxHearts - startingHearts, false);
                    break;
                case RO_ITEM_POOL_BALANCED: {
                    int heartsToPlace = maxHearts - startingHearts;
                    int halfHearts = maxHearts >> 2;
                    AddFixedItemToPool(RG_HEART_CONTAINER, heartsToPlace - halfHearts, false);
                    AddFixedItemToPool(RG_PIECE_OF_HEART, halfHearts * 4, false);
                    break;
                }
                case RO_ITEM_POOL_SCARCE:
                    AddFixedItemToPool(RG_PIECE_OF_HEART, (maxHearts - startingHearts) * 4, false);
                    break;
            }
        }
    }

    std::erase(junkPool, RG_NONE);
    std::erase(itemPool, RG_NONE);
    std::erase(lesserPool, RG_NONE);
    std::erase(plentifulPool, RG_NONE);

    size_t locCount = ctx->CountEmptyLocations(false);
    assert(itemPool.size() <= locCount);
    int iceTrapstoAdd = 0;
    if (itemPool.size() + plentifulPool.size() < locCount) {
        itemPool.insert(itemPool.end(), plentifulPool.begin(), plentifulPool.end());
        // Fixed Ice Traps
        if (ctx->GetOption(RSK_BASE_ICE_TRAPS)) {
            iceTrapstoAdd++;
            if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsVanilla()) {
                iceTrapstoAdd++;
            }
            if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsVanilla()) {
                iceTrapstoAdd += 4;
            }
        }
        iceTrapstoAdd += ctx->GetOption(RSK_ADDITIONAL_ICE_TRAPS).Get();
        AddFixedItemToPool(RG_ICE_TRAP,
                          itemPool.size() + iceTrapstoAdd < locCount ? iceTrapstoAdd : locCount - itemPool.size(), false);
        if (itemPool.size() + lesserPool.size() < locCount) {
            itemPool.insert(itemPool.end(), lesserPool.begin(), lesserPool.end());
        } else {
            while (itemPool.size() < locCount) {
                itemPool.insert(itemPool.end(), RandomElement(lesserPool, true));
            }
        }
    } else {
        while (itemPool.size() < locCount) {
            itemPool.insert(itemPool.end(), RandomElement(plentifulPool, true));
        }
    }

    size_t junkToAdd = locCount - itemPool.size();
    iceTrapstoAdd = 0;
    if (junkToAdd > 0) {
        if (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Is(100)) {
            iceTrapstoAdd = junkToAdd;
        } else if (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Get() >= 0) {
            for (int count = 0; count < junkToAdd; count++) {
                if (Random(0, 101) < ctx->GetOption(RSK_ICE_TRAP_PERCENT).Get()) {
                    iceTrapstoAdd++;
                }
            }
        }
        AddFixedItemToPool(RG_ICE_TRAP, iceTrapstoAdd, false);
        junkToAdd -= iceTrapstoAdd;
        if (junkToAdd > junkPool.size()) {
            itemPool.insert(itemPool.end(), junkPool.begin(), junkPool.end());
            while (itemPool.size() < locCount) {
                itemPool.insert(itemPool.end(), RandomElement(JunkPoolItems));
            }
        } else {
            while (itemPool.size() < locCount) {
                itemPool.insert(itemPool.end(), RandomElement(junkPool, true));
            }
        }
    }

    assert(itemPool.size() == locCount);
}
