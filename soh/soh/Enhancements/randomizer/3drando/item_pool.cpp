#include "item_pool.hpp"

#include "../dungeon.h"
#include "fill.hpp"
#include "../static_data.h"
#include "../context.h"
#include "pool_functions.hpp"
#include "random.hpp"
#include "spoiler_log.hpp"
#include "z64item.h"
#include <spdlog/spdlog.h>

std::vector<RandomizerGet> ItemPool = {};
std::vector<RandomizerGet> PendingJunkPool = {};
const std::array<RandomizerGet, 9> dungeonRewards = {
    RG_KOKIRI_EMERALD,  RG_GORON_RUBY,       RG_ZORA_SAPPHIRE,    RG_FOREST_MEDALLION, RG_FIRE_MEDALLION,
    RG_WATER_MEDALLION, RG_SPIRIT_MEDALLION, RG_SHADOW_MEDALLION, RG_LIGHT_MEDALLION,
};
const std::array<RandomizerGet, 16> JunkPoolItems = {
    RG_BOMBS_5,        RG_BOMBS_10,   RG_BOMBS_20,     RG_DEKU_NUTS_5, RG_DEKU_STICK_1, RG_DEKU_SEEDS_30,
    RG_RECOVERY_HEART, RG_ARROWS_5,   RG_ARROWS_10,    RG_ARROWS_30,   RG_BLUE_RUPEE,   RG_RED_RUPEE,
    RG_PURPLE_RUPEE,   RG_HUGE_RUPEE, RG_DEKU_NUTS_10, RG_ICE_TRAP,
};
const std::array<RandomizerGet, 59> alwaysItems = {
    RG_BIGGORON_SWORD,
    RG_BOOMERANG,
    RG_LENS_OF_TRUTH,
    RG_MEGATON_HAMMER,
    RG_IRON_BOOTS,
    RG_GORON_TUNIC,
    RG_ZORA_TUNIC,
    RG_HOVER_BOOTS,
    RG_MIRROR_SHIELD,
    RG_STONE_OF_AGONY,
    RG_FIRE_ARROWS,
    RG_ICE_ARROWS,
    RG_LIGHT_ARROWS,
    RG_DINS_FIRE,
    RG_FARORES_WIND,
    RG_NAYRUS_LOVE,
    RG_GREG_RUPEE,
    RG_PROGRESSIVE_HOOKSHOT, // 2 progressive hookshots
    RG_PROGRESSIVE_HOOKSHOT,
    RG_DEKU_SHIELD,
    RG_HYLIAN_SHIELD,
    RG_PROGRESSIVE_STRENGTH, // 3 progressive strength upgrades
    RG_PROGRESSIVE_STRENGTH,
    RG_PROGRESSIVE_STRENGTH,
    RG_PROGRESSIVE_SCALE, // 2 progressive scales
    RG_PROGRESSIVE_SCALE,
    RG_PROGRESSIVE_BOW, // 3 progressive Bows
    RG_PROGRESSIVE_BOW,
    RG_PROGRESSIVE_BOW,
    RG_PROGRESSIVE_SLINGSHOT, // 3 progressive bullet bags
    RG_PROGRESSIVE_SLINGSHOT,
    RG_PROGRESSIVE_SLINGSHOT,
    RG_PROGRESSIVE_BOMB_BAG, // 3 progressive bomb bags
    RG_PROGRESSIVE_BOMB_BAG,
    RG_PROGRESSIVE_BOMB_BAG,
    RG_PROGRESSIVE_WALLET, // 2 progressive wallets
    RG_PROGRESSIVE_WALLET,
    RG_PROGRESSIVE_MAGIC_METER, // 2 progressive magic meters
    RG_PROGRESSIVE_MAGIC_METER,
    RG_DOUBLE_DEFENSE,
    RG_PROGRESSIVE_STICK_UPGRADE, // 2 stick upgrades
    RG_PROGRESSIVE_STICK_UPGRADE,
    RG_PROGRESSIVE_NUT_UPGRADE, // 2 nut upgrades
    RG_PROGRESSIVE_NUT_UPGRADE,
    RG_RECOVERY_HEART, // 6 recovery hearts
    RG_RECOVERY_HEART,
    RG_RECOVERY_HEART,
    RG_RECOVERY_HEART,
    RG_RECOVERY_HEART,
    RG_RECOVERY_HEART,
    RG_BOMBS_5, // 2
    RG_BOMBS_5,
    RG_BOMBS_10,
    RG_BOMBS_20,
    RG_ARROWS_5,
    RG_ARROWS_10, // 5
    RG_ARROWS_10,
    RG_ARROWS_10,
    RG_TREASURE_GAME_HEART,
};
const std::array<RandomizerGet, 44> easyItems = {
    RG_BIGGORON_SWORD,
    RG_KOKIRI_SWORD,
    RG_MASTER_SWORD,
    RG_BOOMERANG,
    RG_LENS_OF_TRUTH,
    RG_MEGATON_HAMMER,
    RG_IRON_BOOTS,
    RG_GORON_TUNIC,
    RG_ZORA_TUNIC,
    RG_HOVER_BOOTS,
    RG_MIRROR_SHIELD,
    RG_FIRE_ARROWS,
    RG_LIGHT_ARROWS,
    RG_DINS_FIRE,
    RG_PROGRESSIVE_HOOKSHOT,
    RG_PROGRESSIVE_STRENGTH,
    RG_PROGRESSIVE_SCALE,
    RG_PROGRESSIVE_WALLET,
    RG_PROGRESSIVE_MAGIC_METER,
    RG_PROGRESSIVE_STICK_UPGRADE,
    RG_PROGRESSIVE_NUT_UPGRADE,
    RG_PROGRESSIVE_BOW,
    RG_PROGRESSIVE_SLINGSHOT,
    RG_PROGRESSIVE_BOMB_BAG,
    RG_DOUBLE_DEFENSE,
    RG_HEART_CONTAINER, // 16 Heart Containers
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_PIECE_OF_HEART, // 3 heart pieces
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
};
const std::array<RandomizerGet, 43> normalItems = {
    // 35 pieces of heart
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    RG_PIECE_OF_HEART,
    // 8 heart containers
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
    RG_HEART_CONTAINER,
};
const std::array<RandomizerGet, 2> DT_Vanilla = {
    RG_RECOVERY_HEART,
    RG_RECOVERY_HEART,
};
const std::array<RandomizerGet, 3> DT_MQ = {
    RG_DEKU_SHIELD,
    RG_DEKU_SHIELD,
    RG_PURPLE_RUPEE,
};
const std::array<RandomizerGet, 1> DC_Vanilla = {
    RG_RED_RUPEE,
};
const std::array<RandomizerGet, 2> DC_MQ = {
    RG_HYLIAN_SHIELD,
    RG_BLUE_RUPEE,
};
const std::array<RandomizerGet, 7> JB_MQ = {
    RG_DEKU_NUTS_5, RG_DEKU_NUTS_5, RG_DEKU_NUTS_5, RG_DEKU_NUTS_5, RG_RECOVERY_HEART, RG_DEKU_SHIELD, RG_DEKU_STICK_1,
};
const std::array<RandomizerGet, 3> FoT_Vanilla = {
    RG_RECOVERY_HEART,
    RG_ARROWS_10,
    RG_ARROWS_30,
};
const std::array<RandomizerGet, 1> FoT_MQ = {
    RG_ARROWS_5,
};
const std::array<RandomizerGet, 1> FiT_Vanilla = {
    RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 2> FiT_MQ = {
    RG_BOMBS_20,
    RG_HYLIAN_SHIELD,
};
const std::array<RandomizerGet, 4> SpT_Vanilla = {
    RG_DEKU_SHIELD,
    RG_DEKU_SHIELD,
    RG_RECOVERY_HEART,
    RG_BOMBS_20,
};
const std::array<RandomizerGet, 3> SpT_MQ = {
    RG_PURPLE_RUPEE,
    RG_PURPLE_RUPEE,
    RG_ARROWS_30,
};
const std::array<RandomizerGet, 1> ShT_Vanilla = {
    RG_ARROWS_30,
};
const std::array<RandomizerGet, 3> ShT_MQ = {
    RG_ARROWS_5,
    RG_ARROWS_5,
    RG_RED_RUPEE,
};
const std::array<RandomizerGet, 7> BW_Vanilla = {
    RG_RECOVERY_HEART, RG_BOMBS_10, RG_HUGE_RUPEE, RG_DEKU_NUTS_5, RG_DEKU_NUTS_10, RG_DEKU_SHIELD, RG_HYLIAN_SHIELD,
};
const std::array<RandomizerGet, 4> GTG_Vanilla = {
    RG_ARROWS_30,
    RG_ARROWS_30,
    RG_ARROWS_30,
    RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 5> GTG_MQ = {
    RG_TREASURE_GAME_GREEN_RUPEE, RG_TREASURE_GAME_GREEN_RUPEE, RG_ARROWS_10, RG_GREEN_RUPEE, RG_PURPLE_RUPEE,
};
const std::array<RandomizerGet, 4> GC_Vanilla = {
    RG_BLUE_RUPEE,
    RG_BLUE_RUPEE,
    RG_BLUE_RUPEE,
    RG_ARROWS_30,
};
const std::array<RandomizerGet, 5> GC_MQ = {
    RG_ARROWS_10, RG_ARROWS_10, RG_BOMBS_5, RG_RED_RUPEE, RG_RECOVERY_HEART,
};
const std::array<RandomizerGet, 11> normalBottles = {
    RG_EMPTY_BOTTLE,
    RG_BOTTLE_WITH_MILK,
    RG_BOTTLE_WITH_RED_POTION,
    RG_BOTTLE_WITH_GREEN_POTION,
    RG_BOTTLE_WITH_BLUE_POTION,
    RG_BOTTLE_WITH_FAIRY,
    RG_BOTTLE_WITH_FISH,
    RG_BOTTLE_WITH_BUGS,
    RG_BOTTLE_WITH_POE,
    RG_BOTTLE_WITH_BIG_POE,
    RG_BOTTLE_WITH_BLUE_FIRE,
};
const std::array<RandomizerGet, 28> normalRupees = {
    RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,
    RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_BLUE_RUPEE,
    RG_BLUE_RUPEE,   RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,
    RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE,
    RG_PURPLE_RUPEE, RG_HUGE_RUPEE,   RG_HUGE_RUPEE,   RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 28> shopsanityRupees = {
    RG_BLUE_RUPEE,   RG_BLUE_RUPEE,   RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,
    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,    RG_RED_RUPEE,
    RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE,
    RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_PURPLE_RUPEE, RG_HUGE_RUPEE,   RG_HUGE_RUPEE,
    RG_HUGE_RUPEE,   RG_HUGE_RUPEE,   RG_HUGE_RUPEE,   RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 19> dekuScrubItems = {
    RG_DEKU_NUTS_5,  RG_DEKU_NUTS_5,    RG_DEKU_NUTS_5,    RG_DEKU_NUTS_5,    RG_DEKU_NUTS_5,
    RG_DEKU_STICK_1, RG_BOMBS_5,        RG_BOMBS_5,        RG_BOMBS_5,        RG_BOMBS_5,
    RG_BOMBS_5,      RG_RECOVERY_HEART, RG_RECOVERY_HEART, RG_RECOVERY_HEART, RG_RECOVERY_HEART,
    RG_BLUE_RUPEE,   RG_BLUE_RUPEE,     RG_BLUE_RUPEE,     RG_BLUE_RUPEE,
};
const std::array<RandomizerGet, 12> songList = {
    RG_ZELDAS_LULLABY, RG_EPONAS_SONG,       RG_SUNS_SONG,          RG_SARIAS_SONG,
    RG_SONG_OF_TIME,   RG_SONG_OF_STORMS,    RG_MINUET_OF_FOREST,   RG_PRELUDE_OF_LIGHT,
    RG_BOLERO_OF_FIRE, RG_SERENADE_OF_WATER, RG_NOCTURNE_OF_SHADOW, RG_REQUIEM_OF_SPIRIT,
};
const std::array<RandomizerGet, 10> tradeItems = {
    RG_POCKET_EGG,
    // RG_POCKET_CUCCO,
    RG_COJIRO,
    RG_ODD_MUSHROOM,
    RG_POACHERS_SAW,
    RG_BROKEN_SWORD,
    RG_PRESCRIPTION,
    RG_EYEBALL_FROG,
    RG_EYEDROPS,
    RG_CLAIM_CHECK,
};

void AddItemToPool(std::vector<RandomizerGet>& pool, RandomizerGet item, size_t count /*= 1*/) {
    pool.insert(pool.end(), count, item);
}

template <typename FromPool> static void AddItemsToPool(std::vector<RandomizerGet>& toPool, const FromPool& fromPool) {
    AddElementsToPool(toPool, fromPool);
}

static void AddItemToMainPool(const RandomizerGet item, size_t count = 1) {
    ItemPool.insert(ItemPool.end(), count, item);
}

static void AddRandomBottle(std::vector<RandomizerGet>& bottlePool) {
    AddItemToMainPool(RandomElement(bottlePool, true));
}

RandomizerGet GetJunkItem() {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_ICE_TRAPS).Is(RO_ICE_TRAPS_MAYHEM) ||
        ctx->GetOption(RSK_ICE_TRAPS).Is(RO_ICE_TRAPS_ONSLAUGHT)) {
        return RG_ICE_TRAP;
    } else if (ctx->GetOption(RSK_ICE_TRAPS).Is(RO_ICE_TRAPS_EXTRA)) {
        return RandomElement(JunkPoolItems);
    }
    // Ice Trap is the last item in JunkPoolItems, so subtract 1 to never hit that index
    uint8_t idx = Random(0, static_cast<uint32_t>(JunkPoolItems.size()) - 1);
    return JunkPoolItems[idx];
}

static RandomizerGet GetPendingJunkItem() {
    if (PendingJunkPool.empty()) {
        return GetJunkItem();
    }

    return RandomElement(PendingJunkPool, true);
}

// Replace junk items in the pool with pending junk
static void ReplaceMaxItem(const RandomizerGet itemToReplace, int max) {
    int itemCount = 0;
    for (size_t i = 0; i < ItemPool.size(); i++) {
        if (ItemPool[i] == itemToReplace) {
            if (itemCount >= max) {
                ItemPool[i] = RG_NONE;
            }
            itemCount++;
        }
    }
}

void PlaceJunkInExcludedLocation(const RandomizerCheck il) {
    // place a non-advancement item in this location
    auto ctx = Rando::Context::GetInstance();
    for (size_t i = 0; i < ItemPool.size(); i++) {
        if (!Rando::StaticData::RetrieveItem(ItemPool[i]).IsAdvancement()) {
            ctx->PlaceItemInLocation(il, ItemPool[i]);
            ItemPool.erase(ItemPool.begin() + i);
            return;
        }
    }
    SPDLOG_ERROR("ERROR: No Junk to Place!!!");
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

static void PlaceItemsForType(RandomizerCheckType rctype, bool overworldActive, bool dungeonActive) {
    if (!(overworldActive || dungeonActive)) {
        return;
    }
    for (RandomizerCheck rc : ctx->GetLocations(ctx->allLocations, rctype)) {
        auto loc = Rando::StaticData::GetLocation(rc);

        // If item is in the overworld and shuffled, add its item to the pool
        if (loc->IsOverworld()) {
            if (overworldActive) {
                AddItemToMainPool(loc->GetVanillaItem());
            }
        } else {
            if (dungeonActive) {
                // If the same in MQ and vanilla, add.
                RandomizerCheckQuest currentQuest = loc->GetQuest();
                if (currentQuest == RCQUEST_BOTH) {
                    AddItemToMainPool(loc->GetVanillaItem());
                } else {
                    // Check if current item's dungeon is vanilla or MQ, and only add if quest corresponds to it.
                    SceneID itemScene = loc->GetScene();

                    if (itemScene >= SCENE_DEKU_TREE && itemScene <= SCENE_GERUDO_TRAINING_GROUND) {
                        bool isMQ = ctx->GetDungeon(itemScene)->IsMQ();

                        if ((isMQ && currentQuest == RCQUEST_MQ) || (!isMQ && currentQuest == RCQUEST_VANILLA)) {
                            AddItemToMainPool(loc->GetVanillaItem());
                        }
                    }
                }
            }
        }
    }
}

static void SetScarceItemPool() {
    ReplaceMaxItem(RG_PROGRESSIVE_BOMBCHUS, 3);
    ReplaceMaxItem(RG_BOMBCHU_5, 1);
    ReplaceMaxItem(RG_BOMBCHU_10, 2);
    ReplaceMaxItem(RG_BOMBCHU_20, 0);
    ReplaceMaxItem(RG_PROGRESSIVE_MAGIC_METER, 1);
    ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
    ReplaceMaxItem(RG_PROGRESSIVE_STICK_UPGRADE, ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG) ? 2 : 1);
    ReplaceMaxItem(RG_PROGRESSIVE_NUT_UPGRADE, ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG) ? 2 : 1);
    ReplaceMaxItem(RG_PROGRESSIVE_BOW, 2);
    ReplaceMaxItem(RG_PROGRESSIVE_SLINGSHOT, 2);
    ReplaceMaxItem(RG_PROGRESSIVE_BOMB_BAG, 2);
    ReplaceMaxItem(RG_HEART_CONTAINER, 0);
}

static void SetMinimalItemPool() {
    auto ctx = Rando::Context::GetInstance();
    ReplaceMaxItem(RG_PROGRESSIVE_BOMBCHUS, 1);
    ReplaceMaxItem(RG_BOMBCHU_5, 1);
    ReplaceMaxItem(RG_BOMBCHU_10, 0);
    ReplaceMaxItem(RG_BOMBCHU_20, 0);
    ReplaceMaxItem(RG_NAYRUS_LOVE, 0);
    ReplaceMaxItem(RG_PROGRESSIVE_MAGIC_METER, 1);
    ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
    ReplaceMaxItem(RG_PROGRESSIVE_STICK_UPGRADE, ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG) ? 1 : 0);
    ReplaceMaxItem(RG_PROGRESSIVE_NUT_UPGRADE, ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG) ? 1 : 0);
    ReplaceMaxItem(RG_PROGRESSIVE_BOW, 1);
    ReplaceMaxItem(RG_PROGRESSIVE_SLINGSHOT, 1);
    ReplaceMaxItem(RG_PROGRESSIVE_BOMB_BAG, 1);
    ReplaceMaxItem(RG_PIECE_OF_HEART, 0);
    // Need an extra heart container when starting with 1 heart to be able to reach 3 hearts
    ReplaceMaxItem(RG_HEART_CONTAINER, (ctx->GetOption(RSK_STARTING_HEARTS).Get() == 18) ? 1 : 0);
}

void GenerateItemPool() {
    // RANDOTODO proper removal of items not in pool or logically relevant instead of dummy checks.
    auto ctx = Rando::Context::GetInstance();
    ItemPool.clear();
    PendingJunkPool.clear();

    // Initialize ice trap models to always major items
    ctx->possibleIceTrapModels = {
        RG_MIRROR_SHIELD,
        RG_BOOMERANG,
        RG_LENS_OF_TRUTH,
        RG_MEGATON_HAMMER,
        RG_IRON_BOOTS,
        RG_HOVER_BOOTS,
        RG_STONE_OF_AGONY,
        RG_DINS_FIRE,
        RG_FARORES_WIND,
        RG_NAYRUS_LOVE,
        RG_FIRE_ARROWS,
        RG_ICE_ARROWS,
        RG_LIGHT_ARROWS,
        RG_DOUBLE_DEFENSE,
        RG_CLAIM_CHECK,
        RG_PROGRESSIVE_HOOKSHOT,
        RG_PROGRESSIVE_STRENGTH,
        RG_PROGRESSIVE_BOMB_BAG,
        RG_PROGRESSIVE_BOW,
        RG_PROGRESSIVE_SLINGSHOT,
        RG_PROGRESSIVE_WALLET,
        RG_PROGRESSIVE_SCALE,
        RG_PROGRESSIVE_MAGIC_METER,
    };
    // Check song shuffle and dungeon reward shuffle just for ice traps
    if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_ANYWHERE)) {
        // Push item ids for songs
        ctx->possibleIceTrapModels.push_back(RG_ZELDAS_LULLABY);
        ctx->possibleIceTrapModels.push_back(RG_EPONAS_SONG);
        ctx->possibleIceTrapModels.push_back(RG_SARIAS_SONG);
        ctx->possibleIceTrapModels.push_back(RG_SUNS_SONG);
        ctx->possibleIceTrapModels.push_back(RG_SONG_OF_TIME);
        ctx->possibleIceTrapModels.push_back(RG_SONG_OF_STORMS);
        ctx->possibleIceTrapModels.push_back(RG_MINUET_OF_FOREST);
        ctx->possibleIceTrapModels.push_back(RG_BOLERO_OF_FIRE);
        ctx->possibleIceTrapModels.push_back(RG_SERENADE_OF_WATER);
        ctx->possibleIceTrapModels.push_back(RG_REQUIEM_OF_SPIRIT);
        ctx->possibleIceTrapModels.push_back(RG_NOCTURNE_OF_SHADOW);
        ctx->possibleIceTrapModels.push_back(RG_PRELUDE_OF_LIGHT);
    }
    if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_ANYWHERE)) {
        // Push item ids for dungeon rewards
        ctx->possibleIceTrapModels.push_back(RG_KOKIRI_EMERALD);
        ctx->possibleIceTrapModels.push_back(RG_GORON_RUBY);
        ctx->possibleIceTrapModels.push_back(RG_ZORA_SAPPHIRE);
        ctx->possibleIceTrapModels.push_back(RG_FOREST_MEDALLION);
        ctx->possibleIceTrapModels.push_back(RG_FIRE_MEDALLION);
        ctx->possibleIceTrapModels.push_back(RG_WATER_MEDALLION);
        ctx->possibleIceTrapModels.push_back(RG_SPIRIT_MEDALLION);
        ctx->possibleIceTrapModels.push_back(RG_SHADOW_MEDALLION);
        ctx->possibleIceTrapModels.push_back(RG_LIGHT_MEDALLION);
    }

    if (ctx->GetOption(RSK_TRIFORCE_HUNT)) {
        ctx->possibleIceTrapModels.push_back(RG_TRIFORCE_PIECE);
        AddItemToMainPool(RG_TRIFORCE_PIECE, (ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_TOTAL).Get() + 1));
        ctx->PlaceItemInLocation(RC_TRIFORCE_COMPLETED, RG_TRIFORCE); // Win condition
        ctx->PlaceItemInLocation(RC_GANON, GetJunkItem(), false, true);
    } else {
        ctx->PlaceItemInLocation(RC_GANON, RG_TRIFORCE); // Win condition
    }

    // Fixed item locations
    ctx->PlaceItemInLocation(RC_HC_ZELDAS_LETTER, RG_ZELDAS_LETTER);

    if (ctx->GetOption(RSK_SHUFFLE_KOKIRI_SWORD)) {
        AddItemToMainPool(RG_KOKIRI_SWORD);
        ctx->possibleIceTrapModels.push_back(RG_KOKIRI_SWORD);
    } else {
        if (!ctx->GetOption(RSK_STARTING_KOKIRI_SWORD)) {
            ctx->PlaceItemInLocation(RC_KF_KOKIRI_SWORD_CHEST, RG_KOKIRI_SWORD, false, true);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
        AddItemToMainPool(RG_MASTER_SWORD);
        ctx->possibleIceTrapModels.push_back(RG_MASTER_SWORD);
    } else {
        if (!ctx->GetOption(RSK_STARTING_MASTER_SWORD)) {
            ctx->PlaceItemInLocation(RC_TOT_MASTER_SWORD, RG_MASTER_SWORD, false, true);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_WEIRD_EGG)) {
        AddItemToMainPool(RG_WEIRD_EGG);
        ctx->possibleIceTrapModels.push_back(RG_WEIRD_EGG);
    } else {
        ctx->PlaceItemInLocation(RC_HC_MALON_EGG, RG_WEIRD_EGG, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_OCARINA)) {
        AddItemToMainPool(RG_PROGRESSIVE_OCARINA, 2);
        if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            AddItemToPool(PendingJunkPool, RG_PROGRESSIVE_OCARINA);
        }
        ctx->possibleIceTrapModels.push_back(RG_PROGRESSIVE_OCARINA);
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
        AddItemToMainPool(RG_OCARINA_A_BUTTON);
        AddItemToMainPool(RG_OCARINA_C_UP_BUTTON);
        AddItemToMainPool(RG_OCARINA_C_DOWN_BUTTON);
        AddItemToMainPool(RG_OCARINA_C_LEFT_BUTTON);
        AddItemToMainPool(RG_OCARINA_C_RIGHT_BUTTON);

        ctx->possibleIceTrapModels.push_back(RG_OCARINA_A_BUTTON);
        ctx->possibleIceTrapModels.push_back(RG_OCARINA_C_UP_BUTTON);
        ctx->possibleIceTrapModels.push_back(RG_OCARINA_C_DOWN_BUTTON);
        ctx->possibleIceTrapModels.push_back(RG_OCARINA_C_LEFT_BUTTON);
        ctx->possibleIceTrapModels.push_back(RG_OCARINA_C_RIGHT_BUTTON);
    }

    if (ctx->GetOption(RSK_SKELETON_KEY)) {
        AddItemToMainPool(RG_SKELETON_KEY);
    }

    if (ctx->GetOption(RSK_SHUFFLE_SWIM)) {
        AddItemToMainPool(RG_PROGRESSIVE_SCALE);
    }

    if (ctx->GetOption(RSK_SHUFFLE_BEEHIVES)) {
        // 32 total beehive locations
        AddItemToPool(PendingJunkPool, RG_RED_RUPEE, 23);
        AddItemToPool(PendingJunkPool, RG_BLUE_RUPEE, 9);
    }

    // Shuffle Pots
    bool overworldPotsActive = ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_OVERWORLD) ||
                               ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_ALL);
    bool dungeonPotsActive = ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_DUNGEONS) ||
                             ctx->GetOption(RSK_SHUFFLE_POTS).Is(RO_SHUFFLE_POTS_ALL);
    PlaceItemsForType(RCTYPE_POT, overworldPotsActive, dungeonPotsActive);

    // Shuffle Crates
    bool overworldCratesActive = ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_OVERWORLD) ||
                                 ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_ALL);
    bool overworldNLCratesActive = ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_NO_LOGIC) &&
                                   (ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_OVERWORLD) ||
                                    ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_ALL));
    bool dungeonCratesActive = ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_DUNGEONS) ||
                               ctx->GetOption(RSK_SHUFFLE_CRATES).Is(RO_SHUFFLE_CRATES_ALL);
    PlaceItemsForType(RCTYPE_CRATE, overworldCratesActive, dungeonCratesActive);
    PlaceItemsForType(RCTYPE_NLCRATE, overworldNLCratesActive, dungeonCratesActive);
    PlaceItemsForType(RCTYPE_SMALL_CRATE, overworldCratesActive, dungeonCratesActive);

    auto fsMode = ctx->GetOption(RSK_FISHSANITY);
    if (fsMode.IsNot(RO_FISHSANITY_OFF)) {
        if (fsMode.Is(RO_FISHSANITY_POND) || fsMode.Is(RO_FISHSANITY_BOTH)) {
            // 17 max child pond fish
            uint8_t pondCt = ctx->GetOption(RSK_FISHSANITY_POND_COUNT).Get();
            for (uint8_t i = 0; i < pondCt; i++) {
                AddItemToMainPool(GetJunkItem());
            }

            if (ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)) {
                // 16 max adult pond fish, have to reduce to 16 if every fish is enabled
                if (pondCt > 16)
                    pondCt = 16;
                for (uint8_t i = 0; i < pondCt; i++) {
                    AddItemToMainPool(GetJunkItem());
                }
            }
        }
        // 9 grotto fish, 5 zora's domain fish
        if (fsMode.Is(RO_FISHSANITY_OVERWORLD) || fsMode.Is(RO_FISHSANITY_BOTH)) {
            for (uint8_t i = 0; i < Rando::StaticData::GetOverworldFishLocations().size(); i++)
                AddItemToMainPool(GetJunkItem());
        }

        if (fsMode.Is(RO_FISHSANITY_HYRULE_LOACH)) {
            AddItemToMainPool(RG_PURPLE_RUPEE);
        } else {
            ctx->PlaceItemInLocation(RC_LH_HYRULE_LOACH, RG_PURPLE_RUPEE, false, true);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_FISHING_POLE)) {
        AddItemToMainPool(RG_FISHING_POLE);
        ctx->possibleIceTrapModels.push_back(RG_FISHING_POLE);
    }

    if (ctx->GetOption(RSK_INFINITE_UPGRADES).Is(RO_INF_UPGRADES_PROGRESSIVE)) {
        AddItemToMainPool(RG_PROGRESSIVE_BOMB_BAG);
        AddItemToMainPool(RG_PROGRESSIVE_BOW);
        AddItemToMainPool(RG_PROGRESSIVE_NUT_UPGRADE);
        AddItemToMainPool(RG_PROGRESSIVE_SLINGSHOT);
        AddItemToMainPool(RG_PROGRESSIVE_STICK_UPGRADE);
        AddItemToMainPool(RG_PROGRESSIVE_MAGIC_METER);
        AddItemToMainPool(RG_PROGRESSIVE_WALLET);
    }

    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) ||
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)) {
        AddItemToMainPool(RG_MAGIC_BEAN_PACK);
        if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            AddItemToPool(PendingJunkPool, RG_MAGIC_BEAN_PACK);
        }
        ctx->possibleIceTrapModels.push_back(RG_MAGIC_BEAN_PACK);
    } else {
        ctx->PlaceItemInLocation(RC_ZR_MAGIC_BEAN_SALESMAN, RG_MAGIC_BEAN, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
        ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL)) {
        if (/*!ProgressiveGoronSword TODO: Implement Progressive Goron Sword*/ true) {
            AddItemToMainPool(RG_GIANTS_KNIFE);
        }
        if (ctx->GetOption(RSK_BOMBCHU_BAG)) {
            AddItemToMainPool(RG_PROGRESSIVE_BOMBCHUS);
        } else {
            AddItemToMainPool(RG_BOMBCHU_10);
        }
    } else {
        ctx->PlaceItemInLocation(RC_KAK_GRANNYS_SHOP, RG_BLUE_POTION_REFILL, false, true);
        ctx->PlaceItemInLocation(RC_GC_MEDIGORON, RG_GIANTS_KNIFE, false, true);
        ctx->PlaceItemInLocation(RC_WASTELAND_BOMBCHU_SALESMAN, RG_BOMBCHU_10, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_FROG_SONG_RUPEES)) {
        AddItemToMainPool(RG_PURPLE_RUPEE, 5);
    } else {
        ctx->PlaceItemInLocation(RC_ZR_FROGS_ZELDAS_LULLABY, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_EPONAS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SARIAS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SUNS_SONG, RG_PURPLE_RUPEE, false, true);
        ctx->PlaceItemInLocation(RC_ZR_FROGS_SONG_OF_TIME, RG_PURPLE_RUPEE, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE)) {
        AddItemToMainPool(RG_POCKET_EGG);
        AddItemToMainPool(RG_COJIRO);
        AddItemToMainPool(RG_ODD_MUSHROOM);
        AddItemToMainPool(RG_ODD_POTION);
        AddItemToMainPool(RG_POACHERS_SAW);
        AddItemToMainPool(RG_BROKEN_SWORD);
        AddItemToMainPool(RG_PRESCRIPTION);
        AddItemToMainPool(RG_EYEBALL_FROG);
        AddItemToMainPool(RG_EYEDROPS);
    }
    AddItemToMainPool(RG_CLAIM_CHECK);

    if (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS)) {
        AddItemToMainPool(RG_TREASURE_GAME_SMALL_KEY, 6); // 6 individual keys
    } else if (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK)) {
        AddItemToMainPool(RG_TREASURE_GAME_SMALL_KEY); // 1 key which will behave as a pack of 6
    }

    if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OFF)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            ctx->PlaceItemInLocation(loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
        }
    } else if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_DUNGEONS)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            if (Rando::StaticData::GetLocation(loc)->IsOverworld()) {
                ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
            } else {
                AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN);
            }
        }
    } else if (ctx->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OVERWORLD)) {
        for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, RCTYPE_SKULL_TOKEN)) {
            if (Rando::StaticData::GetLocation(loc)->IsDungeon()) {
                ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
            } else {
                AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN);
            }
        }
    } else {
        AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN, 100);
    }

    if (ctx->GetOption(RSK_SHUFFLE_100_GS_REWARD)) {
        if (ctx->GetOption(RSK_SHUFFLE_TOKENS).IsNot(RO_TOKENSANITY_OFF) &&
            ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            AddItemToPool(PendingJunkPool, RG_GOLD_SKULLTULA_TOKEN, 10);
        }
        AddItemToMainPool(RG_HUGE_RUPEE);
    } else {
        ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_HUGE_RUPEE, false, true);
    }

    if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS)) {
        AddItemToMainPool(RG_GOHMA_SOUL);
        AddItemToMainPool(RG_KING_DODONGO_SOUL);
        AddItemToMainPool(RG_BARINADE_SOUL);
        AddItemToMainPool(RG_PHANTOM_GANON_SOUL);
        AddItemToMainPool(RG_VOLVAGIA_SOUL);
        AddItemToMainPool(RG_MORPHA_SOUL);
        AddItemToMainPool(RG_BONGO_BONGO_SOUL);
        AddItemToMainPool(RG_TWINROVA_SOUL);

        ctx->possibleIceTrapModels.push_back(RG_GOHMA_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_KING_DODONGO_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_BARINADE_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_PHANTOM_GANON_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_VOLVAGIA_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_MORPHA_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_BONGO_BONGO_SOUL);
        ctx->possibleIceTrapModels.push_back(RG_TWINROVA_SOUL);
        if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON)) {
            AddItemToMainPool(RG_GANON_SOUL);
            ctx->possibleIceTrapModels.push_back(RG_GANON_SOUL);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_CHILD_WALLET)) {
        AddItemToMainPool(RG_PROGRESSIVE_WALLET);
    }

    if (ctx->GetOption(RSK_INCLUDE_TYCOON_WALLET)) {
        AddItemToMainPool(RG_PROGRESSIVE_WALLET);
    }

    if (ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG)) {
        AddItemToMainPool(RG_PROGRESSIVE_STICK_UPGRADE);
    }

    if (ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG)) {
        AddItemToMainPool(RG_PROGRESSIVE_NUT_UPGRADE);
    }

    if (ctx->GetOption(RSK_BOMBCHU_BAG)) {
        AddItemToMainPool(RG_PROGRESSIVE_BOMBCHUS, 5);
    } else {
        AddItemToMainPool(RG_BOMBCHU_5);
        AddItemToMainPool(RG_BOMBCHU_10, 3);
        AddItemToMainPool(RG_BOMBCHU_20);
    }

    // Ice Traps
    AddItemToMainPool(RG_ICE_TRAP);
    if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsVanilla()) {
        AddItemToMainPool(RG_ICE_TRAP);
    }
    if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsVanilla()) {
        AddItemToMainPool(RG_ICE_TRAP, 4);
    }

    // Gerudo Fortress
    if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE)) {
        ctx->PlaceItemInLocation(RC_GF_NORTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
        ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
    } else if (ctx->GetOption(RSK_GERUDO_KEYS).IsNot(RO_GERUDO_KEYS_VANILLA)) {
        if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)) {
            AddItemToMainPool(RG_GERUDO_FORTRESS_SMALL_KEY);
            ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
        } else {
            // Only add key ring if 4 Fortress keys necessary
            if (ctx->GetOption(RSK_KEYRINGS_GERUDO_FORTRESS) && ctx->GetOption(RSK_KEYRINGS)) {
                AddItemToMainPool(RG_GERUDO_FORTRESS_KEY_RING);
                // Add junk to make up for missing keys
                for (uint8_t i = 0; i < 3; i++) {
                    AddItemToMainPool(GetJunkItem());
                }
            } else {
                AddItemToMainPool(RG_GERUDO_FORTRESS_SMALL_KEY, 4);
            }
        }
        if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            if (ctx->GetOption(RSK_KEYRINGS_GERUDO_FORTRESS) &&
                ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_NORMAL) && ctx->GetOption(RSK_KEYRINGS)) {
                AddItemToPool(PendingJunkPool, RG_GERUDO_FORTRESS_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_GERUDO_FORTRESS_SMALL_KEY);
            }
        }
    } else {
        if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)) {
            ctx->PlaceItemInLocation(RC_GF_NORTH_F1_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
        } else {
            ctx->PlaceItemInLocation(RC_GF_NORTH_F1_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
            ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_GERUDO_FORTRESS_SMALL_KEY, false, true);
        }
    }

    // Gerudo Membership Card
    if (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) &&
        ctx->GetOption(RSK_GERUDO_FORTRESS).IsNot(RO_GF_CARPENTERS_FREE)) {
        AddItemToMainPool(RG_GERUDO_MEMBERSHIP_CARD);
        ctx->possibleIceTrapModels.push_back(RG_GERUDO_MEMBERSHIP_CARD);
    } else if (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
        AddItemToPool(PendingJunkPool, RG_GERUDO_MEMBERSHIP_CARD);
        ctx->PlaceItemInLocation(RC_GF_GERUDO_MEMBERSHIP_CARD, RG_ICE_TRAP, false, true);
    } else {
        ctx->PlaceItemInLocation(RC_GF_GERUDO_MEMBERSHIP_CARD, RG_GERUDO_MEMBERSHIP_CARD, false, true);
    }

    // Keys

    // For key rings, need to add as many junk items as "missing" keys
    if (ctx->GetOption(RSK_KEYRINGS).IsNot(RO_KEYRINGS_OFF)) {
        uint8_t ringJunkAmt = 0;
        for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->HasKeyRing()) {
                ringJunkAmt += dungeon->GetSmallKeyCount() - 1;
            }
        }
        for (uint8_t i = 0; i < ringJunkAmt; i++) {
            AddItemToMainPool(GetJunkItem());
        }
    }

    if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
        if (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
            AddItemToPool(PendingJunkPool, RG_GERUDO_MEMBERSHIP_CARD);
        }

        if (ctx->GetOption(RSK_SHUFFLE_FISHING_POLE)) {
            AddItemToPool(PendingJunkPool, RG_FISHING_POLE);
        }

        // Plentiful small keys
        if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANYWHERE) ||
            ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON) ||
            ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OVERWORLD)) {
            if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_BOTTOM_OF_THE_WELL_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_BOTTOM_OF_THE_WELL_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::FOREST_TEMPLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::FIRE_TEMPLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::WATER_TEMPLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::SHADOW_TEMPLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_GERUDO_TRAINING_GROUND_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_GERUDO_TRAINING_GROUND_SMALL_KEY);
            }
            if (ctx->GetDungeon(Rando::GANONS_CASTLE)->HasKeyRing()) {
                AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_KEY_RING);
            } else {
                AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_SMALL_KEY);
            }
        }

        if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANYWHERE) ||
            ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANY_DUNGEON) ||
            ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OVERWORLD)) {
            AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_BOSS_KEY);
            AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_BOSS_KEY);
            AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_BOSS_KEY);
            AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_BOSS_KEY);
            AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_BOSS_KEY);
        }

        if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANYWHERE) ||
            ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_ANY_DUNGEON) ||
            ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OVERWORLD)) {
            AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_BOSS_KEY);
        }
    }

    if (ctx->GetOption(RSK_LOCK_OVERWORLD_DOORS)) {
        AddItemToPool(ItemPool, RG_GUARD_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_MARKET_BAZAAR_KEY);
        AddItemToPool(ItemPool, RG_MARKET_POTION_SHOP_KEY);
        AddItemToPool(ItemPool, RG_MASK_SHOP_KEY);
        AddItemToPool(ItemPool, RG_MARKET_SHOOTING_GALLERY_KEY);
        AddItemToPool(ItemPool, RG_BOMBCHU_BOWLING_KEY);
        AddItemToPool(ItemPool, RG_TREASURE_CHEST_GAME_BUILDING_KEY);
        AddItemToPool(ItemPool, RG_BOMBCHU_SHOP_KEY);
        AddItemToPool(ItemPool, RG_RICHARDS_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_ALLEY_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_KAK_BAZAAR_KEY);
        AddItemToPool(ItemPool, RG_KAK_POTION_SHOP_KEY);
        AddItemToPool(ItemPool, RG_BOSS_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_GRANNYS_POTION_SHOP_KEY);
        AddItemToPool(ItemPool, RG_SKULLTULA_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_IMPAS_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_WINDMILL_KEY);
        AddItemToPool(ItemPool, RG_KAK_SHOOTING_GALLERY_KEY);
        AddItemToPool(ItemPool, RG_DAMPES_HUT_KEY);
        AddItemToPool(ItemPool, RG_TALONS_HOUSE_KEY);
        AddItemToPool(ItemPool, RG_STABLES_KEY);
        AddItemToPool(ItemPool, RG_BACK_TOWER_KEY);
        AddItemToPool(ItemPool, RG_HYLIA_LAB_KEY);
        AddItemToPool(ItemPool, RG_FISHING_HOLE_KEY);
    }

    // Shopsanity
    if (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_OFF) ||
        (ctx->GetOption(RSK_SHOPSANITY).Is(RO_SHOPSANITY_SPECIFIC_COUNT) &&
         ctx->GetOption(RSK_SHOPSANITY_COUNT).Is(RO_SHOPSANITY_COUNT_ZERO_ITEMS))) {
        AddItemsToPool(ItemPool, normalRupees);
    } else {
        AddItemsToPool(ItemPool, shopsanityRupees); // Shopsanity gets extra large rupees
    }

    // Shuffle Fairies
    if (ctx->GetOption(RSK_SHUFFLE_FAIRIES)) {
        for (auto rc : Rando::StaticData::GetOverworldFairyLocations()) {
            AddItemToMainPool(GetJunkItem());
        }
        // 8 extra for Ganon's Castle + 2 Dodongo's Cavern Gossip Stone + 3 Shadow Temple
        int extra = 13;
        extra += ctx->GetDungeon(Rando::FIRE_TEMPLE)->IsVanilla() ? 0 : 2;
        extra += ctx->GetDungeon(Rando::WATER_TEMPLE)->IsVanilla() ? 0 : 3;
        extra += ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->IsVanilla() ? 2 : 1;
        extra += ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() ? 1 : 2;
        extra += ctx->GetDungeon(Rando::ICE_CAVERN)->IsVanilla() ? 1 : 0;
        extra += ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsVanilla() ? 1 : 0;
        extra += ctx->GetDungeon(Rando::GANONS_CASTLE)->IsVanilla() ? 1 : 0;
        for (int i = 0; i < extra; i++) {
            AddItemToMainPool(GetJunkItem());
        }
    }

    // Scrubsanity
    if (ctx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_ALL)) {
        // Deku Tree
        if (ctx->GetDungeon(Rando::DEKU_TREE)->IsMQ()) {
            AddItemToMainPool(RG_DEKU_SHIELD);
        }

        // Dodongos Cavern
        AddItemToMainPool(RG_DEKU_STICK_1);
        AddItemToMainPool(RG_DEKU_SHIELD);
        if (ctx->GetDungeon(Rando::DODONGOS_CAVERN)->IsMQ()) {
            AddItemToMainPool(RG_RECOVERY_HEART);
        } else {
            AddItemToMainPool(RG_DEKU_NUTS_5);
        }

        // Jabu Jabus Belly
        if (ctx->GetDungeon(Rando::JABU_JABUS_BELLY)->IsVanilla()) {
            AddItemToMainPool(RG_DEKU_NUTS_5);
        }

        // Ganons Castle
        AddItemToMainPool(RG_BOMBS_5);
        AddItemToMainPool(RG_RECOVERY_HEART);
        AddItemToMainPool(RG_BLUE_RUPEE);
        if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsMQ()) {
            AddItemToMainPool(RG_DEKU_NUTS_5);
        }

        // Overworld Scrubs
        AddItemsToPool(ItemPool, dekuScrubItems);

        // Scrubs which sell seeds or arrows sell it based on age, this randomly assigns them
        for (uint8_t i = 0; i < 7; i++) {
            if (Random(0, 3)) {
                AddItemToMainPool(RG_ARROWS_30);
            } else {
                AddItemToMainPool(RG_DEKU_SEEDS_30);
            }
        }
    }

    bool overworldFreeStandingActive = ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_OVERWORLD) ||
                                       ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_ALL);
    bool dungeonFreeStandingActive = ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_DUNGEONS) ||
                                     ctx->GetOption(RSK_SHUFFLE_FREESTANDING).Is(RO_SHUFFLE_FREESTANDING_ALL);
    PlaceItemsForType(RCTYPE_FREESTANDING, overworldFreeStandingActive, dungeonFreeStandingActive);

    AddItemsToPool(ItemPool, alwaysItems);
    AddItemsToPool(ItemPool, dungeonRewards);

    // Dungeon pools
    if (ctx->GetDungeon(Rando::DEKU_TREE)->IsMQ()) {
        AddItemsToPool(ItemPool, DT_MQ);
    } else {
        AddItemsToPool(ItemPool, DT_Vanilla);
    }
    if (ctx->GetDungeon(Rando::DODONGOS_CAVERN)->IsMQ()) {
        AddItemsToPool(ItemPool, DC_MQ);
    } else {
        AddItemsToPool(ItemPool, DC_Vanilla);
    }
    if (ctx->GetDungeon(Rando::JABU_JABUS_BELLY)->IsMQ()) {
        AddItemsToPool(ItemPool, JB_MQ);
    }
    if (ctx->GetDungeon(Rando::FOREST_TEMPLE)->IsMQ()) {
        AddItemsToPool(ItemPool, FoT_MQ);
    } else {
        AddItemsToPool(ItemPool, FoT_Vanilla);
    }
    if (ctx->GetDungeon(Rando::FIRE_TEMPLE)->IsMQ()) {
        AddItemsToPool(ItemPool, FiT_MQ);
    } else {
        AddItemsToPool(ItemPool, FiT_Vanilla);
    }
    if (ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->IsMQ()) {
        AddItemsToPool(ItemPool, SpT_MQ);
    } else {
        AddItemsToPool(ItemPool, SpT_Vanilla);
    }
    if (ctx->GetDungeon(Rando::SHADOW_TEMPLE)->IsMQ()) {
        AddItemsToPool(ItemPool, ShT_MQ);
    } else {
        AddItemsToPool(ItemPool, ShT_Vanilla);
    }
    if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
        AddItemsToPool(ItemPool, BW_Vanilla);
    }
    if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsMQ()) {
        AddItemsToPool(ItemPool, GTG_MQ);
    } else {
        AddItemsToPool(ItemPool, GTG_Vanilla);
    }
    if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsMQ()) {
        AddItemsToPool(ItemPool, GC_MQ);
    } else {
        AddItemsToPool(ItemPool, GC_Vanilla);
    }

    uint8_t rutoBottles = 1;
    if (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN)) {
        rutoBottles = 0;
    }

    // Add 4 total bottles
    uint8_t bottleCount = 4;
    std::vector<RandomizerGet> bottles;
    bottles.assign(normalBottles.begin(), normalBottles.end());
    ctx->possibleIceTrapModels.push_back(Rando::StaticData::RetrieveItem(RandomElement(bottles))
                                             .GetRandomizerGet()); // Get one random bottle type for ice traps
    for (uint8_t i = 0; i < bottleCount; i++) {
        if (i >= rutoBottles) {
            if ((i == bottleCount - 1) &&
                (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
                 ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                AddItemToMainPool(RG_BOTTLE_WITH_BLUE_POTION);
            } else {
                AddRandomBottle(bottles);
            }
        } else {
            AddItemToMainPool(RG_RUTOS_LETTER);
        }
    }

    if (ctx->GetOption(RSK_SHUFFLE_SONGS).IsNot(RO_SONG_SHUFFLE_OFF)) {
        AddItemsToPool(ItemPool, songList);
        // add extra songs only if song shuffle is anywhere
        if (ctx->GetOption(RSK_SHUFFLE_SONGS).Is(RO_SONG_SHUFFLE_ANYWHERE) &&
            ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
            AddItemsToPool(PendingJunkPool, songList);
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

    /*For item pool generation, dungeon items are either placed in their vanilla
    | location, or added to the pool now and filtered out later depending on when
    | they need to get placed or removed in fill.cpp. These items are kept in the
    | pool until removal because the filling algorithm needs to know all of the
    | advancement items that haven't been placed yet for placing higher priority
    | items like stones/medallions.*/

    if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaMapsAndCompasses();
    } else {
        for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->GetMap() != RG_NONE) {
                AddItemToMainPool(dungeon->GetMap());
            }

            if (dungeon->GetCompass() != RG_NONE) {
                AddItemToMainPool(dungeon->GetCompass());
            }
        }
    }

    if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaSmallKeys();
    } else {
        for (auto dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->HasKeyRing() && ctx->GetOption(RSK_KEYSANITY).IsNot(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
                AddItemToMainPool(dungeon->GetKeyRing());
            } else if (dungeon->GetSmallKeyCount() > 0) {
                AddItemToMainPool(dungeon->GetSmallKey(), dungeon->GetSmallKeyCount());
            }
        }
    }

    if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        PlaceVanillaBossKeys();
    } else {
        AddItemToMainPool(RG_FOREST_TEMPLE_BOSS_KEY);
        AddItemToMainPool(RG_FIRE_TEMPLE_BOSS_KEY);
        AddItemToMainPool(RG_WATER_TEMPLE_BOSS_KEY);
        AddItemToMainPool(RG_SPIRIT_TEMPLE_BOSS_KEY);
        AddItemToMainPool(RG_SHADOW_TEMPLE_BOSS_KEY);
    }

    if (!ctx->GetOption(RSK_TRIFORCE_HUNT)) { // Don't add GBK to the pool at all for Triforce Hunt.
        if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_KAK_TOKENS)) {
            ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_GANONS_CASTLE_BOSS_KEY);
        } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Get() >= RO_GANON_BOSS_KEY_LACS_VANILLA) {
            ctx->PlaceItemInLocation(RC_TOT_LIGHT_ARROWS_CUTSCENE, RG_GANONS_CASTLE_BOSS_KEY);
        } else if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA)) {
            ctx->PlaceItemInLocation(RC_GANONS_TOWER_BOSS_KEY_CHEST, RG_GANONS_CASTLE_BOSS_KEY);
        } else {
            AddItemToMainPool(RG_GANONS_CASTLE_BOSS_KEY);
        }
    }

    if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_PLENTIFUL)) {
        AddItemsToPool(ItemPool, easyItems);
    } else {
        AddItemsToPool(ItemPool, normalItems);
    }

    if (!ctx->GetOption(RSK_SHUFFLE_KOKIRI_SWORD)) {
        ReplaceMaxItem(RG_KOKIRI_SWORD, 0);
    }

    if (!ctx->GetOption(RSK_SHUFFLE_MASTER_SWORD)) {
        ReplaceMaxItem(RG_MASTER_SWORD, 0);
    }

    if (/*ProgressiveGoronSword TODO: Implement Setting*/ false) {
        ReplaceMaxItem(RG_BIGGORON_SWORD, 0);
        AddItemToMainPool(RG_PROGRESSIVE_GORONSWORD, 2);
        ctx->possibleIceTrapModels.push_back(RG_PROGRESSIVE_GORONSWORD);
    } else {
        ctx->possibleIceTrapModels.push_back(RG_BIGGORON_SWORD);
    }

    // Replace ice traps with junk from the pending junk pool if necessary
    if (ctx->GetOption(RSK_ICE_TRAPS).Is(RO_ICE_TRAPS_OFF)) {
        ReplaceMaxItem(RG_ICE_TRAP, 0);
    }
    // Replace all junk items with ice traps for onslaught mode
    else if (ctx->GetOption(RSK_ICE_TRAPS).Is(RO_ICE_TRAPS_ONSLAUGHT)) {
        PendingJunkPool.clear();
        for (uint8_t i = 0; i < JunkPoolItems.size() - 3; i++) { // -3 Omits Huge Rupees and Deku Nuts 10
            ReplaceMaxItem(JunkPoolItems[i], 0);
        }
    }

    if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_SCARCE)) {
        SetScarceItemPool();
    } else if (ctx->GetOption(RSK_ITEM_POOL).Is(RO_ITEM_POOL_MINIMAL)) {
        SetMinimalItemPool();
    } else if (/*RemoveDoubleDefense TODO: Implement setting*/ false) {
        ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
    }

    std::erase(ItemPool, RG_NONE);

    if (ItemPool.size() < ctx->allLocations.size()) {
        Shuffle(PendingJunkPool);
        size_t junkNeeded = std::min(PendingJunkPool.size(), ctx->allLocations.size() - ItemPool.size());
        ItemPool.insert(ItemPool.end(), PendingJunkPool.begin(), PendingJunkPool.begin() + junkNeeded);
        PendingJunkPool.erase(PendingJunkPool.begin(), PendingJunkPool.begin() + junkNeeded);
    } else if (ItemPool.size() > ctx->allLocations.size()) {
        // RANDOTODO: all junk should be put in PendingJunkPool so this is never needed
        size_t remove = ItemPool.size() - ctx->allLocations.size();
        for (size_t i = 0; remove > 0 && i < ItemPool.size(); i++) {
            for (size_t j = 0; j < JunkPoolItems.size(); j++) {
                if (ItemPool[i] == JunkPoolItems[j]) {
                    ItemPool[i] = RG_NONE;
                    remove--;
                    break;
                }
            }
        }
        std::erase(ItemPool, RG_NONE);
    }

    // RANDOTODO: Ideally this should be checking for equality, but that is not currently the case and has never been
    // the case, and isn't even currently the case in the 3drando repo we inherited this from years ago, so it may
    // be a large undertaking to fix.
    assert(ItemPool.size() <= ctx->allLocations.size() || !"Item Pool larger than Location Pool");
}
