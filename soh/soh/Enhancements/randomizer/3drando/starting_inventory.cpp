#include "starting_inventory.hpp"

#include "../dungeon.h"
#include "../context.h"
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

  // Add Ganon's Boss key with Triforce Hunt so the game thinks it's obtainable from the start.
  // During save init, the boss key isn't actually given and it's instead given when completing the triforce.
  if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_STARTWITH) ||
      ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_TRIFORCE_HUNT)) {
      AddItemToInventory(RG_GANONS_CASTLE_BOSS_KEY);
  }

  if (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN) && !ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
    AddItemToInventory(RG_GERUDO_MEMBERSHIP_CARD);
  }

  //Starting Inventory Menu
  //Values are associated so that the count of items matches the index of
  //the option selected. If None is selected, the value will be zero and
  //zero of the item will be added to the starting inventory.
  // TODO: Uncomment when these options are implemented.
  // AddItemToInventory(RG_PROGRESSIVE_STICK_UPGRADE, StartingStickCapacity.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_NUT_UPGRADE, StartingNutCapacity.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_BOMB_BAG, StartingBombBag.Value<uint8_t>());
  // AddItemToInventory((BombchusInLogic ? RG_PROGRESSIVE_BOMBCHUS : RG_BOMBCHU_20), StartingBombchus.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_BOW, StartingBow.Value<uint8_t>());
  // AddItemToInventory(RG_FIRE_ARROWS, StartingFireArrows.Value<uint8_t>());
  // AddItemToInventory(RG_ICE_ARROWS, StartingIceArrows.Value<uint8_t>());
  // AddItemToInventory(RG_LIGHT_ARROWS, StartingLightArrows.Value<uint8_t>());
  // AddItemToInventory(RG_DINS_FIRE, StartingDinsFire.Value<uint8_t>());
  // AddItemToInventory(RG_FARORES_WIND, StartingFaroresWind.Value<uint8_t>());
  // AddItemToInventory(RG_NAYRUS_LOVE, StartingNayrusLove.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_SLINGSHOT, StartingSlingshot.Value<uint8_t>());
  // AddItemToInventory(RG_BOOMERANG, StartingBoomerang.Value<uint8_t>());
  // AddItemToInventory(RG_LENS_OF_TRUTH, StartingLensOfTruth.Value<uint8_t>());
  // AddItemToInventory(RG_MAGIC_BEAN_PACK, StartingMagicBean.Value<uint8_t>());
  // AddItemToInventory(RG_MEGATON_HAMMER, StartingMegatonHammer.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_HOOKSHOT, StartingHookshot.Value<uint8_t>());
  // AddItemToInventory(RG_IRON_BOOTS, StartingIronBoots.Value<uint8_t>());
  // AddItemToInventory(RG_HOVER_BOOTS, StartingHoverBoots.Value<uint8_t>());
  //For starting bottles, we need to check if they are a big poe and add that if so
  // since a big poe bottle is not logically equivalent to an empty bottle.
  // if (StartingBottle1.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
  //   AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  // } else if (StartingBottle1.Value<uint8_t>()) {
  //   AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  // }
  // if (StartingBottle2.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
  //   AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  // } else if (StartingBottle2.Value<uint8_t>()) {
  //   AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  // }
  // if (StartingBottle3.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
  //   AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  // } else if (StartingBottle3.Value<uint8_t>()) {
  //   AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  // }
  // if (StartingBottle4.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
  //   AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  // } else if (StartingBottle4.Value<uint8_t>()) {
  //   AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  // }
  // AddItemToInventory(RG_RUTOS_LETTER,              StartingRutoBottle.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_OCARINA,       ctx->GetOption(RSK_STARTING_OCARINA).Value<uint8_t>());
  AddItemToInventory(RG_ZELDAS_LULLABY,            ctx->GetOption(RSK_STARTING_ZELDAS_LULLABY) ? 1 : 0);
  AddItemToInventory(RG_EPONAS_SONG,               ctx->GetOption(RSK_STARTING_EPONAS_SONG) ? 1 : 0);
  AddItemToInventory(RG_SARIAS_SONG,               ctx->GetOption(RSK_STARTING_SARIAS_SONG) ? 1 : 0);
  AddItemToInventory(RG_SUNS_SONG,                 ctx->GetOption(RSK_STARTING_SUNS_SONG) ? 1 : 0);
  AddItemToInventory(RG_SONG_OF_TIME,              ctx->GetOption(RSK_STARTING_SONG_OF_TIME) ? 1 : 0);
  AddItemToInventory(RG_SONG_OF_STORMS,            ctx->GetOption(RSK_STARTING_SONG_OF_STORMS) ? 1 : 0);
  AddItemToInventory(RG_MINUET_OF_FOREST,          ctx->GetOption(RSK_STARTING_MINUET_OF_FOREST) ? 1 : 0);
  AddItemToInventory(RG_BOLERO_OF_FIRE,            ctx->GetOption(RSK_STARTING_BOLERO_OF_FIRE) ? 1 : 0);
  AddItemToInventory(RG_SERENADE_OF_WATER,         ctx->GetOption(RSK_STARTING_SERENADE_OF_WATER) ? 1 : 0);
  AddItemToInventory(RG_REQUIEM_OF_SPIRIT,         ctx->GetOption(RSK_STARTING_REQUIEM_OF_SPIRIT) ? 1 : 0);
  AddItemToInventory(RG_NOCTURNE_OF_SHADOW,        ctx->GetOption(RSK_STARTING_NOCTURNE_OF_SHADOW) ? 1 : 0);
  AddItemToInventory(RG_PRELUDE_OF_LIGHT,          ctx->GetOption(RSK_STARTING_PRELUDE_OF_LIGHT) ? 1 : 0);
  AddItemToInventory(RG_KOKIRI_SWORD,              ctx->GetOption(RSK_STARTING_KOKIRI_SWORD) ? 1 : 0);
  // if (ProgressiveGoronSword) {
  //   AddItemToInventory(RG_PROGRESSIVE_GORONSWORD, StartingBiggoronSword.Value<uint8_t>());
  // } else {
  //   AddItemToInventory(RG_GIANTS_KNIFE, (StartingBiggoronSword.Is(STARTINGBGS_GIANTS_KNIFE)) ? 1 : 0);
  //   AddItemToInventory(RG_BIGGORON_SWORD, (StartingBiggoronSword.Is(STARTINGBGS_BIGGORON_SWORD)) ? 1 : 0);
  // }
  AddItemToInventory(RG_MASTER_SWORD,              ctx->GetOption(RSK_STARTING_MASTER_SWORD) ? 1 : 0);
  AddItemToInventory(RG_DEKU_SHIELD,               ctx->GetOption(RSK_STARTING_DEKU_SHIELD) ? 1 : 0);
  // AddItemToInventory(RG_HYLIAN_SHIELD,             StartingHylianShield.Value<uint8_t>());
  // AddItemToInventory(RG_MIRROR_SHIELD,             StartingMirrorShield.Value<uint8_t>());
  // AddItemToInventory(RG_GORON_TUNIC,               StartingGoronTunic.Value<uint8_t>());
  // AddItemToInventory(RG_ZORA_TUNIC,                StartingZoraTunic.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_MAGIC_METER,   StartingMagicMeter.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_STRENGTH,      StartingStrength.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_SCALE,         StartingScale.Value<uint8_t>());
  // AddItemToInventory(RG_PROGRESSIVE_WALLET,        StartingWallet.Value<uint8_t>());
  // AddItemToInventory(RG_STONE_OF_AGONY,            StartingShardOfAgony.Value<uint8_t>());
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
  AddItemToInventory(RG_GOLD_SKULLTULA_TOKEN,      ctx->GetOption(RSK_STARTING_SKULLTULA_TOKEN).Value<uint8_t>());

  int8_t hearts = ctx->GetOption(RSK_STARTING_HEARTS).Value<uint8_t>() - 2;
  AdditionalHeartContainers = 0;
  if (hearts < 0) {
    AddItemToInventory(RG_PIECE_OF_HEART, 4);
    // Plentiful and minimal have less than 4 standard pieces of heart so also replace the winner heart
    if (ctx->GetOption(RSK_ITEM_POOL).Value<uint8_t>() == 0 || ctx->GetOption(RSK_ITEM_POOL).Value<uint8_t>() == 3) {
      AddItemToInventory(RG_TREASURE_GAME_HEART);
    }

    AdditionalHeartContainers = 1 - hearts;
  } else if (hearts > 0) {
    // 16 containers in plentiful, 8 in balanced and 0 in the others
    uint8_t maxContainers = 8 * std::max(0, 2 - ctx->GetOption(RSK_ITEM_POOL).Value<uint8_t>());

    if (hearts <= maxContainers) {
      AddItemToInventory(RG_HEART_CONTAINER, hearts);
    } else {
      AddItemToInventory(RG_HEART_CONTAINER, maxContainers);
      AddItemToInventory(RG_PIECE_OF_HEART, (hearts - maxContainers) * 4);
    }

    if (hearts == 17) {
      AddItemToInventory(RG_TREASURE_GAME_HEART);
    }
  }
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
