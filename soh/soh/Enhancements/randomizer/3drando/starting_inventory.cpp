#include "starting_inventory.hpp"

#include "../dungeon.h"
#include "../SeedContext.h"
#include "../logic.h"
#include "pool_functions.hpp"
#include "soh/Enhancements/randomizer/static_data.h"

std::vector<RandomizerGet> StartingInventory;
uint8_t AdditionalHeartContainers;

static void AddItemToInventory(RandomizerGet item, size_t count = 1) {
    StartingInventory.insert(StartingInventory.end(), count, item);
}

void GenerateStartingInventory() {
    auto ctx = Rando::Context::GetInstance();
    StartingInventory.clear();

    if (ctx->GetOption(RSK_SHUFFLE_MAPANDCOMPASS).Is(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        for (auto* dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->GetMap() != RG_NONE) {
                AddItemToInventory(dungeon->GetMap());
            }
            if (dungeon->GetCompass() != RG_NONE) {
                AddItemToInventory(dungeon->GetCompass());
            }
        }
    }

    if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        for (auto* dungeon : ctx->GetDungeons()->GetDungeonList()) {
            if (dungeon->GetSmallKeyCount() > 0) {
                AddItemToInventory(dungeon->GetSmallKey(), dungeon->GetSmallKeyCount());
            }
        }
    } else if (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA)) {
        // Logic cannot handle vanilla key layout in some dungeons
        // this is because vanilla expects the dungeon major item to be
        // locked behind the keys, which is not always true in rando.
        // We can resolve this by starting with some extra keys
        // - OoT Randomizer
        if (ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->IsMQ()) {
            AddItemToInventory(RG_SPIRIT_TEMPLE_SMALL_KEY, 3);
        }
    }

    if (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_STARTWITH)) {
        AddItemToInventory(RG_FOREST_TEMPLE_BOSS_KEY);
        AddItemToInventory(RG_FIRE_TEMPLE_BOSS_KEY);
        AddItemToInventory(RG_WATER_TEMPLE_BOSS_KEY);
        AddItemToInventory(RG_SPIRIT_TEMPLE_BOSS_KEY);
        AddItemToInventory(RG_SHADOW_TEMPLE_BOSS_KEY);
    }

    // Add Ganon's Boss key with Triforce Hunt's Win setting so the game thinks it's obtainable from the start.
    // During save init, the boss key isn't actually given and it's instead given when completing the triforce.
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH) ||
        ctx->GetOption(RSK_TRIFORCE_HUNT).Is(RO_TRIFORCE_HUNT_WIN)) {
        AddItemToInventory(RG_GANONS_CASTLE_BOSS_KEY);
    }

    if (ctx->GetOption(RSK_STARTING_GERUDO_CARD) || (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) &&
                                                     !ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD))) {
        AddItemToInventory(RG_GERUDO_MEMBERSHIP_CARD);
    }

    // Starting Inventory Menu
    // Values are associated so that the count of items matches the index of
    // the option selected. If None is selected, the value will be zero and
    // zero of the item will be added to the starting inventory.
    //  TODO: Uncomment when these options are implemented.
    //  AddItemToInventory(RG_PROGRESSIVE_STICK_UPGRADE, StartingStickCapacity.Value<uint8_t>());
    //  AddItemToInventory(RG_PROGRESSIVE_NUT_UPGRADE, StartingNutCapacity.Value<uint8_t>());
    AddItemToInventory(RG_PROGRESSIVE_BOMB_BAG, ctx->GetOption(RSK_STARTING_BOMB_BAG).Get());
    AddItemToInventory(RG_PROGRESSIVE_BOW, ctx->GetOption(RSK_STARTING_BOW).Get());
    AddItemToInventory(RG_PROGRESSIVE_SLINGSHOT, ctx->GetOption(RSK_STARTING_SLINGSHOT).Get());
    AddItemToInventory(RG_BOOMERANG, ctx->GetOption(RSK_STARTING_BOOMERANG) ? 1 : 0);
    AddItemToInventory(RG_LENS_OF_TRUTH, ctx->GetOption(RSK_STARTING_LENS_OF_TRUTH) ? 1 : 0);
    AddItemToInventory(RG_PROGRESSIVE_HOOKSHOT, ctx->GetOption(RSK_STARTING_HOOKSHOT).Get());
    // Bombchu bags only exist when the bombchu bag setting enables them; a single bag is one item,
    // progressive bags are added per starting tier.
    if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_SINGLE)) {
        AddItemToInventory(RG_PROGRESSIVE_BOMBCHU_BAG, ctx->GetOption(RSK_STARTING_BOMBCHU_BAG).Get() ? 1 : 0);
    } else if (ctx->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_PROGRESSIVE)) {
        AddItemToInventory(RG_PROGRESSIVE_BOMBCHU_BAG, ctx->GetOption(RSK_STARTING_BOMBCHU_BAG).Get());
    }
    AddItemToInventory(RG_MEGATON_HAMMER, ctx->GetOption(RSK_STARTING_MEGATON_HAMMER) ? 1 : 0);
    AddItemToInventory(RG_IRON_BOOTS, ctx->GetOption(RSK_STARTING_IRON_BOOTS) ? 1 : 0);
    AddItemToInventory(RG_HOVER_BOOTS, ctx->GetOption(RSK_STARTING_HOVER_BOOTS) ? 1 : 0);
    AddItemToInventory(RG_DINS_FIRE, ctx->GetOption(RSK_STARTING_DINS_FIRE) ? 1 : 0);
    AddItemToInventory(RG_FARORES_WIND, ctx->GetOption(RSK_STARTING_FARORES_WIND) ? 1 : 0);
    AddItemToInventory(RG_NAYRUS_LOVE, ctx->GetOption(RSK_STARTING_NAYRUS_LOVE) ? 1 : 0);
    AddItemToInventory(RG_FIRE_ARROWS, ctx->GetOption(RSK_STARTING_FIRE_ARROWS) ? 1 : 0);
    AddItemToInventory(RG_ICE_ARROWS, ctx->GetOption(RSK_STARTING_ICE_ARROWS) ? 1 : 0);
    AddItemToInventory(RG_LIGHT_ARROWS, ctx->GetOption(RSK_STARTING_LIGHT_ARROWS) ? 1 : 0);
    AddItemToInventory(RG_HYLIAN_SHIELD, ctx->GetOption(RSK_STARTING_HYLIAN_SHIELD) ? 1 : 0);
    AddItemToInventory(RG_MIRROR_SHIELD, ctx->GetOption(RSK_STARTING_MIRROR_SHIELD) ? 1 : 0);
    AddItemToInventory(RG_GORON_TUNIC, ctx->GetOption(RSK_STARTING_GORON_TUNIC) ? 1 : 0);
    AddItemToInventory(RG_ZORA_TUNIC, ctx->GetOption(RSK_STARTING_ZORA_TUNIC) ? 1 : 0);
    AddItemToInventory(RG_STONE_OF_AGONY, ctx->GetOption(RSK_STARTING_STONE_OF_AGONY) ? 1 : 0);
    // A big poe bottle is not logically equivalent to an empty bottle, so it's a distinct item.
    for (RandomizerSettingKey bottleKey :
         { RSK_STARTING_BOTTLE_1, RSK_STARTING_BOTTLE_2, RSK_STARTING_BOTTLE_3, RSK_STARTING_BOTTLE_4 }) {
        uint8_t bottle = ctx->GetOption(bottleKey).Get();
        if (bottle == RO_STARTING_BOTTLE_BIG_POE) {
            AddItemToInventory(RG_BOTTLE_WITH_BIG_POE);
        } else if (bottle == RO_STARTING_BOTTLE_RUTOS_LETTER) {
            AddItemToInventory(RG_RUTOS_LETTER);
        } else if (bottle != RO_STARTING_BOTTLE_OFF) {
            AddItemToInventory(RG_EMPTY_BOTTLE);
        }
    }
    // The weird egg only exists as an item when it's shuffled; vanilla gives it through the cutscene.
    if (ctx->GetOption(RSK_SHUFFLE_WEIRD_EGG)) {
        AddItemToInventory(RG_WEIRD_EGG, ctx->GetOption(RSK_STARTING_WEIRD_EGG) ? 1 : 0);
    }
    AddItemToInventory(RG_BUNNY_HOOD, ctx->GetOption(RSK_STARTING_BUNNY_HOOD) ? 1 : 0);
    AddItemToInventory(RG_CLAIM_CHECK, ctx->GetOption(RSK_STARTING_CLAIM_CHECK) ? 1 : 0);
    AddItemToInventory(RG_PROGRESSIVE_OCARINA, ctx->GetOption(RSK_STARTING_OCARINA).Get());
    AddItemToInventory(RG_ZELDAS_LULLABY, ctx->GetOption(RSK_STARTING_ZELDAS_LULLABY) ? 1 : 0);
    AddItemToInventory(RG_EPONAS_SONG, ctx->GetOption(RSK_STARTING_EPONAS_SONG) ? 1 : 0);
    AddItemToInventory(RG_SARIAS_SONG, ctx->GetOption(RSK_STARTING_SARIAS_SONG) ? 1 : 0);
    AddItemToInventory(RG_SUNS_SONG, ctx->GetOption(RSK_STARTING_SUNS_SONG) ? 1 : 0);
    AddItemToInventory(RG_SONG_OF_TIME, ctx->GetOption(RSK_STARTING_SONG_OF_TIME) ? 1 : 0);
    AddItemToInventory(RG_SONG_OF_STORMS, ctx->GetOption(RSK_STARTING_SONG_OF_STORMS) ? 1 : 0);
    AddItemToInventory(RG_MINUET_OF_FOREST, ctx->GetOption(RSK_STARTING_MINUET_OF_FOREST) ? 1 : 0);
    AddItemToInventory(RG_BOLERO_OF_FIRE, ctx->GetOption(RSK_STARTING_BOLERO_OF_FIRE) ? 1 : 0);
    AddItemToInventory(RG_SERENADE_OF_WATER, ctx->GetOption(RSK_STARTING_SERENADE_OF_WATER) ? 1 : 0);
    AddItemToInventory(RG_REQUIEM_OF_SPIRIT, ctx->GetOption(RSK_STARTING_REQUIEM_OF_SPIRIT) ? 1 : 0);
    AddItemToInventory(RG_NOCTURNE_OF_SHADOW, ctx->GetOption(RSK_STARTING_NOCTURNE_OF_SHADOW) ? 1 : 0);
    AddItemToInventory(RG_PRELUDE_OF_LIGHT, ctx->GetOption(RSK_STARTING_PRELUDE_OF_LIGHT) ? 1 : 0);
    AddItemToInventory(RG_KOKIRI_SWORD, ctx->GetOption(RSK_STARTING_KOKIRI_SWORD) ? 1 : 0);
    AddItemToInventory(RG_MAGIC_BEAN_PACK, ctx->GetOption(RSK_STARTING_BEANS) ? 1 : 0);
    AddItemToInventory(RG_GIANTS_KNIFE,
                       ctx->GetOption(RSK_STARTING_BIGGORON_SWORD).Is(RO_STARTING_BGS_GIANTS_KNIFE) ? 1 : 0);
    AddItemToInventory(RG_BIGGORON_SWORD,
                       ctx->GetOption(RSK_STARTING_BIGGORON_SWORD).Is(RO_STARTING_BGS_BIGGORON_SWORD) ? 1 : 0);
    AddItemToInventory(RG_MASTER_SWORD, ctx->GetOption(RSK_STARTING_MASTER_SWORD) ? 1 : 0);
    AddItemToInventory(RG_DEKU_SHIELD, ctx->GetOption(RSK_STARTING_DEKU_SHIELD) ? 1 : 0);
    AddItemToInventory(RG_PROGRESSIVE_MAGIC_METER, ctx->GetOption(RSK_STARTING_MAGIC_METER).Get());
    AddItemToInventory(RG_PROGRESSIVE_STRENGTH, ctx->GetOption(RSK_STARTING_STRENGTH).Get());
    AddItemToInventory(RG_PROGRESSIVE_SCALE, ctx->GetOption(RSK_STARTING_SCALE).Get());
    AddItemToInventory(RG_PROGRESSIVE_WALLET, ctx->GetOption(RSK_STARTING_WALLET).Get());
    // AddItemToInventory(RG_DOUBLE_DEFENSE,            StartingDoubleDefense.Value<uint8_t>());
    // AddItemToInventory(RG_KOKIRI_EMERALD,            StartingKokiriEmerald.Value<uint8_t>());
    // AddItemToInventory(RG_GORON_RUBY,                StartingGoronRuby.Value<uint8_t>());
    // AddItemToInventory(RG_ZORA_SAPPHIRE,             StartingZoraSapphire.Value<uint8_t>());
    // AddItemToInventory(RG_FOREST_MEDALLION,          StartingForestMedallion.Value<uint8_t>());
    // AddItemToInventory(RG_FIRE_MEDALLION,            StartingFireMedallion.Value<uint8_t>());
    // AddItemToInventory(RG_WATER_MEDALLION,           StartingWaterMedallion.Value<uint8_t>());
    // AddItemToInventory(RG_SPIRIT_MEDALLION,          StartingSpiritMedallion.Value<uint8_t>());
    // AddItemToInventory(RG_SHADOW_MEDALLION,          StartingShadowMedallion.Value<uint8_t>());
    // AddItemToInventory(RG_LIGHT_MEDALLION,           StartingLightMedallion.Value<uint8_t>());
    AddItemToInventory(RG_GOLD_SKULLTULA_TOKEN, ctx->GetOption(RSK_STARTING_SKULLTULA_TOKEN).Get());
}

bool StartingInventoryHasBottle() {
    RandomizerGet bottle = RG_EMPTY_BOTTLE;
    return ElementInContainer(bottle, StartingInventory);
}

void ApplyStartingInventory() {
    for (RandomizerGet item : StartingInventory) {
        if (item == RG_PIECE_OF_HEART || item == RG_HEART_CONTAINER || item == RG_TREASURE_GAME_HEART)
            continue;

        Rando::StaticData::RetrieveItem(item).ApplyEffect();
    }
}
