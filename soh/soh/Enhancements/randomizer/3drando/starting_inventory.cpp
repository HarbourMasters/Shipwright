#include "starting_inventory.hpp"

#include "dungeon.hpp"
#include "settings.hpp"
#include "soh/Enhancements/randomizer/static_data.h"

using namespace Settings;
using namespace Dungeon;

std::vector<RandomizerGet> StartingInventory;
uint8_t AdditionalHeartContainers;

static void AddItemToInventory(RandomizerGet item, size_t count = 1) {
  StartingInventory.insert(StartingInventory.end(), count, item);
}

void GenerateStartingInventory() {
  StartingInventory.clear();

  if (MapsAndCompasses.Is(MAPSANDCOMPASSES_START_WITH)) {
    for (auto* dungeon : dungeonList) {
      if (dungeon->GetMap() != RG_NONE) {
        AddItemToInventory(dungeon->GetMap());
      }

      if (dungeon->GetCompass() != RG_NONE) {
        AddItemToInventory(dungeon->GetCompass());
      }
    }
  }

  if (Keysanity.Is(KEYSANITY_START_WITH)) {
    for (auto* dungeon : dungeonList) {
      if (dungeon->GetSmallKeyCount() > 0) {
        AddItemToInventory(dungeon->GetSmallKey(), dungeon->GetSmallKeyCount());
      }
    }
  } else if (Keysanity.Is(KEYSANITY_VANILLA)) {
    // Logic cannot handle vanilla key layout in some dungeons
    // this is because vanilla expects the dungeon major item to be
    // locked behind the keys, which is not always true in rando.
    // We can resolve this by starting with some extra keys
    // - OoT Randomizer
    if (SpiritTemple.IsMQ()) {
      AddItemToInventory(RG_SPIRIT_TEMPLE_SMALL_KEY, 3);
    }
  }

  if (BossKeysanity.Is(BOSSKEYSANITY_START_WITH)) {
    AddItemToInventory(RG_FOREST_TEMPLE_BOSS_KEY);
    AddItemToInventory(RG_FIRE_TEMPLE_BOSS_KEY);
    AddItemToInventory(RG_WATER_TEMPLE_BOSS_KEY);
    AddItemToInventory(RG_SPIRIT_TEMPLE_BOSS_KEY);
    AddItemToInventory(RG_SHADOW_TEMPLE_BOSS_KEY);
  }

  // Add Ganon's Boss key with Triforce Hunt so the game thinks it's obtainable from the start.
  // During save init, the boss key isn't actually given and it's instead given when completing the triforce.
  if (GanonsBossKey.Is(GANONSBOSSKEY_START_WITH) || GanonsBossKey.Is(GANONSBOSSKEY_TRIFORCE_HUNT)) {
    AddItemToInventory(RG_GANONS_CASTLE_BOSS_KEY);
  }

  if (GerudoFortress.Is(GERUDOFORTRESS_OPEN) && !ShuffleGerudoToken) {
    AddItemToInventory(RG_GERUDO_MEMBERSHIP_CARD);
  }

  //Starting Inventory Menu
  //Values are associated so that the count of items matches the index of
  //the option selected. If None is selected, the value will be zero and
  //zero of the item will be added to the starting inventory.
  AddItemToInventory(RG_PROGRESSIVE_STICK_UPGRADE, StartingStickCapacity.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_NUT_UPGRADE, StartingNutCapacity.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_BOMB_BAG, StartingBombBag.Value<uint8_t>());
  AddItemToInventory((BombchusInLogic ? RG_PROGRESSIVE_BOMBCHUS : RG_BOMBCHU_20), StartingBombchus.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_BOW, StartingBow.Value<uint8_t>());
  AddItemToInventory(RG_FIRE_ARROWS, StartingFireArrows.Value<uint8_t>());
  AddItemToInventory(RG_ICE_ARROWS, StartingIceArrows.Value<uint8_t>());
  AddItemToInventory(RG_LIGHT_ARROWS, StartingLightArrows.Value<uint8_t>());
  AddItemToInventory(RG_DINS_FIRE, StartingDinsFire.Value<uint8_t>());
  AddItemToInventory(RG_FARORES_WIND, StartingFaroresWind.Value<uint8_t>());
  AddItemToInventory(RG_NAYRUS_LOVE, StartingNayrusLove.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_SLINGSHOT, StartingSlingshot.Value<uint8_t>());
  AddItemToInventory(RG_BOOMERANG, StartingBoomerang.Value<uint8_t>());
  AddItemToInventory(RG_LENS_OF_TRUTH, StartingLensOfTruth.Value<uint8_t>());
  AddItemToInventory(RG_MAGIC_BEAN_PACK, StartingMagicBean.Value<uint8_t>());
  AddItemToInventory(RG_MEGATON_HAMMER, StartingMegatonHammer.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_HOOKSHOT, StartingHookshot.Value<uint8_t>());
  AddItemToInventory(RG_IRON_BOOTS, StartingIronBoots.Value<uint8_t>());
  AddItemToInventory(RG_HOVER_BOOTS, StartingHoverBoots.Value<uint8_t>());
  //For starting bottles, we need to check if they are a big poe and add that if so
  // since a big poe bottle is not logically equivalent to an empty bottle.
  if (StartingBottle1.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle1.Value<uint8_t>()) {
    AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  }
  if (StartingBottle2.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle2.Value<uint8_t>()) {
    AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  }
  if (StartingBottle3.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle3.Value<uint8_t>()) {
    AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  }
  if (StartingBottle4.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(RG_BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle4.Value<uint8_t>()) {
    AddItemToInventory(RG_EMPTY_BOTTLE, 1);
  }
  AddItemToInventory(RG_RUTOS_LETTER,              StartingRutoBottle.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_OCARINA,       StartingOcarina.Value<uint8_t>());
  AddItemToInventory(RG_ZELDAS_LULLABY,            StartingZeldasLullaby.Value<uint8_t>());
  AddItemToInventory(RG_EPONAS_SONG,               StartingEponasSong.Value<uint8_t>());
  AddItemToInventory(RG_SARIAS_SONG,               StartingSariasSong.Value<uint8_t>());
  AddItemToInventory(RG_SUNS_SONG,                 StartingSunsSong.Value<uint8_t>());
  AddItemToInventory(RG_SONG_OF_TIME,              StartingSongOfTime.Value<uint8_t>());
  AddItemToInventory(RG_SONG_OF_STORMS,            StartingSongOfStorms.Value<uint8_t>());
  AddItemToInventory(RG_MINUET_OF_FOREST,          StartingMinuetOfForest.Value<uint8_t>());
  AddItemToInventory(RG_BOLERO_OF_FIRE,            StartingBoleroOfFire.Value<uint8_t>());
  AddItemToInventory(RG_SERENADE_OF_WATER,         StartingSerenadeOfWater.Value<uint8_t>());
  AddItemToInventory(RG_REQUIEM_OF_SPIRIT,         StartingRequiemOfSpirit.Value<uint8_t>());
  AddItemToInventory(RG_NOCTURNE_OF_SHADOW,        StartingNocturneOfShadow.Value<uint8_t>());
  AddItemToInventory(RG_PRELUDE_OF_LIGHT,          StartingPreludeOfLight.Value<uint8_t>());
  AddItemToInventory(RG_KOKIRI_SWORD,              StartingKokiriSword.Value<uint8_t>());
  if (ProgressiveGoronSword) {
    AddItemToInventory(RG_PROGRESSIVE_GORONSWORD, StartingBiggoronSword.Value<uint8_t>());
  } else {
    AddItemToInventory(RG_GIANTS_KNIFE, (StartingBiggoronSword.Is(STARTINGBGS_GIANTS_KNIFE)) ? 1 : 0);
    AddItemToInventory(RG_BIGGORON_SWORD, (StartingBiggoronSword.Is(STARTINGBGS_BIGGORON_SWORD)) ? 1 : 0);
  }
  AddItemToInventory(RG_DEKU_SHIELD, StartingDekuShield.Value<uint8_t>());
  AddItemToInventory(RG_HYLIAN_SHIELD,             StartingHylianShield.Value<uint8_t>());
  AddItemToInventory(RG_MIRROR_SHIELD,             StartingMirrorShield.Value<uint8_t>());
  AddItemToInventory(RG_GORON_TUNIC,               StartingGoronTunic.Value<uint8_t>());
  AddItemToInventory(RG_ZORA_TUNIC,                StartingZoraTunic.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_MAGIC_METER,   StartingMagicMeter.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_STRENGTH,      StartingStrength.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_SCALE,         StartingScale.Value<uint8_t>());
  AddItemToInventory(RG_PROGRESSIVE_WALLET,        StartingWallet.Value<uint8_t>());
  AddItemToInventory(RG_STONE_OF_AGONY,            StartingShardOfAgony.Value<uint8_t>());
  AddItemToInventory(RG_DOUBLE_DEFENSE,            StartingDoubleDefense.Value<uint8_t>());
  AddItemToInventory(RG_KOKIRI_EMERALD,            StartingKokiriEmerald.Value<uint8_t>());
  AddItemToInventory(RG_GORON_RUBY,                StartingGoronRuby.Value<uint8_t>());
  AddItemToInventory(RG_ZORA_SAPPHIRE,             StartingZoraSapphire.Value<uint8_t>());
  AddItemToInventory(RG_FOREST_MEDALLION,          StartingForestMedallion.Value<uint8_t>());
  AddItemToInventory(RG_FIRE_MEDALLION,            StartingFireMedallion.Value<uint8_t>());
  AddItemToInventory(RG_WATER_MEDALLION,           StartingWaterMedallion.Value<uint8_t>());
  AddItemToInventory(RG_SPIRIT_MEDALLION,          StartingSpiritMedallion.Value<uint8_t>());
  AddItemToInventory(RG_SHADOW_MEDALLION,          StartingShadowMedallion.Value<uint8_t>());
  AddItemToInventory(RG_LIGHT_MEDALLION,           StartingLightMedallion.Value<uint8_t>());
  AddItemToInventory(RG_GOLD_SKULLTULA_TOKEN,      StartingSkulltulaToken.Value<uint8_t>());

  int8_t hearts = StartingHearts.Value<uint8_t>() - 2;
  AdditionalHeartContainers = 0;
  if (hearts < 0) {
    AddItemToInventory(RG_PIECE_OF_HEART, 4);
    // Plentiful and minimal have less than 4 standard pieces of heart so also replace the winner heart
    if (ItemPoolValue.Value<uint8_t>() == 0 || ItemPoolValue.Value<uint8_t>() == 3) {
      AddItemToInventory(RG_TREASURE_GAME_HEART);
    }

    AdditionalHeartContainers = 1 - hearts;
  } else if (hearts > 0) {
    // 16 containers in plentiful, 8 in balanced and 0 in the others
    uint8_t maxContainers = 8 * std::max(0, 2 - ItemPoolValue.Value<uint8_t>());

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
