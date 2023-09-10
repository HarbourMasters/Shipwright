#include "starting_inventory.hpp"

#include "dungeon.hpp"
#include "item_list.hpp"
#include "settings.hpp"

using namespace Settings;
using namespace Dungeon;

std::vector<uint32_t> StartingInventory;
uint8_t AdditionalHeartContainers;

static void AddItemToInventory(uint32_t item, size_t count = 1) {
  StartingInventory.insert(StartingInventory.end(), count, item);
}

void GenerateStartingInventory() {
  StartingInventory.clear();

  if (MapsAndCompasses.Is(MAPSANDCOMPASSES_START_WITH)) {
    for (auto* dungeon : dungeonList) {
      if (dungeon->GetMap() != NONE) {
        AddItemToInventory(dungeon->GetMap());
      }

      if (dungeon->GetCompass() != NONE) {
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
      AddItemToInventory(SPIRIT_TEMPLE_SMALL_KEY, 3);
    }
  }

  if (BossKeysanity.Is(BOSSKEYSANITY_START_WITH)) {
    AddItemToInventory(FOREST_TEMPLE_BOSS_KEY);
    AddItemToInventory(FIRE_TEMPLE_BOSS_KEY);
    AddItemToInventory(WATER_TEMPLE_BOSS_KEY);
    AddItemToInventory(SPIRIT_TEMPLE_BOSS_KEY);
    AddItemToInventory(SHADOW_TEMPLE_BOSS_KEY);
  }

  if (GanonsBossKey.Is(GANONSBOSSKEY_START_WITH)) {
    AddItemToInventory(GANONS_CASTLE_BOSS_KEY);
  }

  if (GerudoFortress.Is(GERUDOFORTRESS_OPEN) && !ShuffleGerudoToken) {
    AddItemToInventory(GERUDO_MEMBERSHIP_CARD);
  }

  //Starting Inventory Menu
  //Values are associated so that the count of items matches the index of
  //the option selected. If None is selected, the value will be zero and
  //zero of the item will be added to the starting inventory.
  AddItemToInventory(PROGRESSIVE_STICK_UPGRADE, StartingStickCapacity.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_NUT_UPGRADE,   StartingNutCapacity.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_BOMB_BAG,      StartingBombBag.Value<uint8_t>());
  AddItemToInventory((BombchusInLogic ? PROGRESSIVE_BOMBCHUS : BOMBCHU_20), StartingBombchus.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_BOW,           StartingBow.Value<uint8_t>());
  AddItemToInventory(FIRE_ARROWS,               StartingFireArrows.Value<uint8_t>());
  AddItemToInventory(ICE_ARROWS,                StartingIceArrows.Value<uint8_t>());
  AddItemToInventory(LIGHT_ARROWS,              StartingLightArrows.Value<uint8_t>());
  AddItemToInventory(DINS_FIRE,                 StartingDinsFire.Value<uint8_t>());
  AddItemToInventory(FARORES_WIND,              StartingFaroresWind.Value<uint8_t>());
  AddItemToInventory(NAYRUS_LOVE,               StartingNayrusLove.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_SLINGSHOT,     StartingSlingshot.Value<uint8_t>());
  AddItemToInventory(BOOMERANG,                 StartingBoomerang.Value<uint8_t>());
  AddItemToInventory(LENS_OF_TRUTH,             StartingLensOfTruth.Value<uint8_t>());
  AddItemToInventory(MAGIC_BEAN_PACK,           StartingMagicBean.Value<uint8_t>());
  AddItemToInventory(MEGATON_HAMMER,            StartingMegatonHammer.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_HOOKSHOT,      StartingHookshot.Value<uint8_t>());
  AddItemToInventory(IRON_BOOTS,                StartingIronBoots.Value<uint8_t>());
  AddItemToInventory(HOVER_BOOTS,               StartingHoverBoots.Value<uint8_t>());
  //For starting bottles, we need to check if they are a big poe and add that if so
  // since a big poe bottle is not logically equivalent to an empty bottle.
  if (StartingBottle1.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle1.Value<uint8_t>()) {
    AddItemToInventory(EMPTY_BOTTLE, 1);
  }
  if (StartingBottle2.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle2.Value<uint8_t>()) {
    AddItemToInventory(EMPTY_BOTTLE, 1);
  }
  if (StartingBottle3.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle3.Value<uint8_t>()) {
    AddItemToInventory(EMPTY_BOTTLE, 1);
  }
  if (StartingBottle4.Value<uint8_t>() == STARTINGBOTTLE_BIG_POE) {
    AddItemToInventory(BOTTLE_WITH_BIG_POE, 1);
  } else if (StartingBottle4.Value<uint8_t>()) {
    AddItemToInventory(EMPTY_BOTTLE, 1);
  }
  AddItemToInventory(RUTOS_LETTER,              StartingRutoBottle.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_OCARINA,       StartingOcarina.Value<uint8_t>());
  AddItemToInventory(ZELDAS_LULLABY,            StartingZeldasLullaby.Value<uint8_t>());
  AddItemToInventory(EPONAS_SONG,               StartingEponasSong.Value<uint8_t>());
  AddItemToInventory(SARIAS_SONG,               StartingSariasSong.Value<uint8_t>());
  AddItemToInventory(SUNS_SONG,                 StartingSunsSong.Value<uint8_t>());
  AddItemToInventory(SONG_OF_TIME,              StartingSongOfTime.Value<uint8_t>());
  AddItemToInventory(SONG_OF_STORMS,            StartingSongOfStorms.Value<uint8_t>());
  AddItemToInventory(MINUET_OF_FOREST,          StartingMinuetOfForest.Value<uint8_t>());
  AddItemToInventory(BOLERO_OF_FIRE,            StartingBoleroOfFire.Value<uint8_t>());
  AddItemToInventory(SERENADE_OF_WATER,         StartingSerenadeOfWater.Value<uint8_t>());
  AddItemToInventory(REQUIEM_OF_SPIRIT,         StartingRequiemOfSpirit.Value<uint8_t>());
  AddItemToInventory(NOCTURNE_OF_SHADOW,        StartingNocturneOfShadow.Value<uint8_t>());
  AddItemToInventory(PRELUDE_OF_LIGHT,          StartingPreludeOfLight.Value<uint8_t>());
  AddItemToInventory(KOKIRI_SWORD,              StartingKokiriSword.Value<uint8_t>());
  if (ProgressiveGoronSword) {
    AddItemToInventory(PROGRESSIVE_GORONSWORD,  StartingBiggoronSword.Value<uint8_t>());
  } else {
    AddItemToInventory(GIANTS_KNIFE,            (StartingBiggoronSword.Is(STARTINGBGS_GIANTS_KNIFE)) ? 1 : 0);
    AddItemToInventory(BIGGORON_SWORD,          (StartingBiggoronSword.Is(STARTINGBGS_BIGGORON_SWORD)) ? 1 : 0);
  }
  AddItemToInventory(DEKU_SHIELD,               StartingDekuShield.Value<uint8_t>());
  AddItemToInventory(HYLIAN_SHIELD,             StartingHylianShield.Value<uint8_t>());
  AddItemToInventory(MIRROR_SHIELD,             StartingMirrorShield.Value<uint8_t>());
  AddItemToInventory(GORON_TUNIC,               StartingGoronTunic.Value<uint8_t>());
  AddItemToInventory(ZORA_TUNIC,                StartingZoraTunic.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_MAGIC_METER,   StartingMagicMeter.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_STRENGTH,      StartingStrength.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_SCALE,         StartingScale.Value<uint8_t>());
  AddItemToInventory(PROGRESSIVE_WALLET,        StartingWallet.Value<uint8_t>());
  AddItemToInventory(STONE_OF_AGONY,            StartingShardOfAgony.Value<uint8_t>());
  AddItemToInventory(DOUBLE_DEFENSE,            StartingDoubleDefense.Value<uint8_t>());
  AddItemToInventory(KOKIRI_EMERALD,            StartingKokiriEmerald.Value<uint8_t>());
  AddItemToInventory(GORON_RUBY,                StartingGoronRuby.Value<uint8_t>());
  AddItemToInventory(ZORA_SAPPHIRE,             StartingZoraSapphire.Value<uint8_t>());
  AddItemToInventory(FOREST_MEDALLION,          StartingForestMedallion.Value<uint8_t>());
  AddItemToInventory(FIRE_MEDALLION,            StartingFireMedallion.Value<uint8_t>());
  AddItemToInventory(WATER_MEDALLION,           StartingWaterMedallion.Value<uint8_t>());
  AddItemToInventory(SPIRIT_MEDALLION,          StartingSpiritMedallion.Value<uint8_t>());
  AddItemToInventory(SHADOW_MEDALLION,          StartingShadowMedallion.Value<uint8_t>());
  AddItemToInventory(LIGHT_MEDALLION,           StartingLightMedallion.Value<uint8_t>());
  AddItemToInventory(GOLD_SKULLTULA_TOKEN,      StartingSkulltulaToken.Value<uint8_t>());

  int8_t hearts = StartingHearts.Value<uint8_t>() - 2;
  AdditionalHeartContainers = 0;
  if (hearts < 0) {
    AddItemToInventory(PIECE_OF_HEART, 4);
    // Plentiful and minimal have less than 4 standard pieces of heart so also replace the winner heart
    if (ItemPoolValue.Value<uint8_t>() == 0 || ItemPoolValue.Value<uint8_t>() == 3) {
        AddItemToInventory(TREASURE_GAME_HEART);
    }

    AdditionalHeartContainers = 1 - hearts;
  } else if (hearts > 0) {
    // 16 containers in plentiful, 8 in balanced and 0 in the others
    uint8_t maxContainers = 8 * std::max(0, 2 - ItemPoolValue.Value<uint8_t>());

    if (hearts <= maxContainers) {
      AddItemToInventory(HEART_CONTAINER, hearts);
    } else {
      AddItemToInventory(HEART_CONTAINER, maxContainers);
      AddItemToInventory(PIECE_OF_HEART, (hearts - maxContainers) * 4);
    }

    if (hearts == 17) {
      AddItemToInventory(TREASURE_GAME_HEART);
    }
  }
}

bool StartingInventoryHasBottle() {
    uint32_t bottle = EMPTY_BOTTLE;
  return ElementInContainer(bottle, StartingInventory);
}

void ApplyStartingInventory() {
    for (uint32_t item : StartingInventory) {
    if (item == PIECE_OF_HEART || item == HEART_CONTAINER || item == TREASURE_GAME_HEART)
      continue;

    ItemTable(item).ApplyEffect();
  }
}
