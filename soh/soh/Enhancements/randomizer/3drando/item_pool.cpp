#include "item_pool.hpp"

#include "dungeon.hpp"
#include "fill.hpp"
#include "../static_data.h"
#include "../context.h"
#include "pool_functions.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "spoiler_log.hpp"
#include "z64item.h"
#include <spdlog/spdlog.h>


using namespace Settings;
using namespace Dungeon;

std::vector<RandomizerGet> ItemPool = {};
std::vector<RandomizerGet> PendingJunkPool = {};
std::vector<uint8_t> IceTrapModels = {};
const std::array<RandomizerGet, 9> dungeonRewards = {
  RG_KOKIRI_EMERALD,
  RG_GORON_RUBY,
  RG_ZORA_SAPPHIRE,
  RG_FOREST_MEDALLION,
  RG_FIRE_MEDALLION,
  RG_WATER_MEDALLION,
  RG_SPIRIT_MEDALLION,
  RG_SHADOW_MEDALLION,
  RG_LIGHT_MEDALLION,
};
const std::array<RandomizerGet, 16> JunkPoolItems = {
  RG_BOMBS_5,
  RG_BOMBS_10,
  RG_BOMBS_20,
  RG_DEKU_NUTS_5,
  RG_DEKU_STICK_1,
  RG_DEKU_SEEDS_30,
  RG_RECOVERY_HEART,
  RG_ARROWS_5,
  RG_ARROWS_10,
  RG_ARROWS_30,
  RG_BLUE_RUPEE,
  RG_RED_RUPEE,
  RG_PURPLE_RUPEE,
  RG_HUGE_RUPEE,
  RG_DEKU_NUTS_10,
  RG_ICE_TRAP,
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
  RG_PROGRESSIVE_HOOKSHOT,  //2 progressive hookshots
  RG_PROGRESSIVE_HOOKSHOT,
  RG_DEKU_SHIELD,
  RG_HYLIAN_SHIELD,
  RG_PROGRESSIVE_STRENGTH,  //3 progressive strength upgrades
  RG_PROGRESSIVE_STRENGTH,
  RG_PROGRESSIVE_STRENGTH,
  RG_PROGRESSIVE_SCALE,     //2 progressive scales
  RG_PROGRESSIVE_SCALE,
  RG_PROGRESSIVE_BOW,       //3 progressive Bows
  RG_PROGRESSIVE_BOW,
  RG_PROGRESSIVE_BOW,
  RG_PROGRESSIVE_SLINGSHOT, //3 progressive bullet bags
  RG_PROGRESSIVE_SLINGSHOT,
  RG_PROGRESSIVE_SLINGSHOT,
  RG_PROGRESSIVE_BOMB_BAG,   //3 progressive bomb bags
  RG_PROGRESSIVE_BOMB_BAG,
  RG_PROGRESSIVE_BOMB_BAG,
  RG_PROGRESSIVE_WALLET, //2 progressive wallets
  RG_PROGRESSIVE_WALLET,
  RG_PROGRESSIVE_MAGIC_METER,  //2 progressive magic meters
  RG_PROGRESSIVE_MAGIC_METER,
  RG_DOUBLE_DEFENSE,
  RG_PROGRESSIVE_STICK_UPGRADE, //2 stick upgrades
  RG_PROGRESSIVE_STICK_UPGRADE,
  RG_PROGRESSIVE_NUT_UPGRADE,   //2 nut upgrades
  RG_PROGRESSIVE_NUT_UPGRADE,
  RG_RECOVERY_HEART,  //6 recovery hearts
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_BOMBS_5, //2
  RG_BOMBS_5,
  RG_BOMBS_10,
  RG_BOMBS_20,
  RG_ARROWS_5,
  RG_ARROWS_10, //5
  RG_ARROWS_10,
  RG_ARROWS_10,
  RG_TREASURE_GAME_HEART,
};
const std::array<RandomizerGet, 43> easyItems = {
  RG_BIGGORON_SWORD,
  RG_KOKIRI_SWORD,
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
  RG_HEART_CONTAINER, //16 Heart Containers
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
  RG_PIECE_OF_HEART,   //3 heart pieces
  RG_PIECE_OF_HEART,
  RG_PIECE_OF_HEART,
};
const std::array<RandomizerGet, 43> normalItems = {
  RG_PIECE_OF_HEART,   //35 pieces of heart
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
  RG_HEART_CONTAINER, //8 heart containers
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
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_RECOVERY_HEART,
  RG_DEKU_SHIELD,
  RG_DEKU_STICK_1,
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
  RG_RECOVERY_HEART,
  RG_BOMBS_10,
  RG_HUGE_RUPEE,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_10,
  RG_DEKU_SHIELD,
  RG_HYLIAN_SHIELD,
};
const std::array<RandomizerGet, 4> GTG_Vanilla = {
  RG_ARROWS_30,
  RG_ARROWS_30,
  RG_ARROWS_30,
  RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 5> GTG_MQ = {
  RG_TREASURE_GAME_GREEN_RUPEE,
  RG_TREASURE_GAME_GREEN_RUPEE,
  RG_ARROWS_10,
  RG_GREEN_RUPEE,
  RG_PURPLE_RUPEE,
};
const std::array<RandomizerGet, 4> GC_Vanilla = {
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_ARROWS_30,
};
const std::array<RandomizerGet, 5> GC_MQ = {
  RG_ARROWS_10,
  RG_ARROWS_10,
  RG_BOMBS_5,
  RG_RED_RUPEE,
  RG_RECOVERY_HEART,
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
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
};
const std::array<RandomizerGet, 28> shopsanityRupees = {
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_RED_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_PURPLE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
  RG_HUGE_RUPEE,
  RG_PROGRESSIVE_WALLET,
};
const std::array<RandomizerGet, 19> dekuScrubItems = {
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_NUTS_5,
  RG_DEKU_STICK_1,
  RG_BOMBS_5,
  RG_BOMBS_5,
  RG_BOMBS_5,
  RG_BOMBS_5,
  RG_BOMBS_5,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_RECOVERY_HEART,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
  RG_BLUE_RUPEE,
};
const std::array<RandomizerGet, 12> songList = {
  RG_ZELDAS_LULLABY,
  RG_EPONAS_SONG,
  RG_SUNS_SONG,
  RG_SARIAS_SONG,
  RG_SONG_OF_TIME,
  RG_SONG_OF_STORMS,
  RG_MINUET_OF_FOREST,
  RG_PRELUDE_OF_LIGHT,
  RG_BOLERO_OF_FIRE,
  RG_SERENADE_OF_WATER,
  RG_NOCTURNE_OF_SHADOW,
  RG_REQUIEM_OF_SPIRIT,
};
const std::array<RandomizerGet, 10> tradeItems = {
  RG_POCKET_EGG,
  //RG_POCKET_CUCCO,
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

template <typename FromPool>
static void AddItemsToPool(std::vector<RandomizerGet>& toPool, const FromPool& fromPool) {
  AddElementsToPool(toPool, fromPool);
}

static void AddItemToMainPool(const RandomizerGet item, size_t count = 1) {
  ItemPool.insert(ItemPool.end(), count, item);
}

static void AddRandomBottle(std::vector<RandomizerGet>& bottlePool) {
  AddItemToMainPool(RandomElement(bottlePool, true));
}

RandomizerGet GetJunkItem() {
  if (IceTrapValue.Is(ICETRAPS_MAYHEM) || IceTrapValue.Is(ICETRAPS_ONSLAUGHT)) {
      return RG_ICE_TRAP;
  } else if (IceTrapValue.Is(ICETRAPS_EXTRA)) {
      return RandomElement(JunkPoolItems);
  }
  //Ice Trap is the last item in JunkPoolItems, so subtract 1 to never hit that index
  uint8_t idx = Random(0, JunkPoolItems.size() - 1);
  return JunkPoolItems[idx];
}

static RandomizerGet GetPendingJunkItem() {
  if (PendingJunkPool.empty()) {
    return GetJunkItem();
  }

  return RandomElement(PendingJunkPool, true);
}

//Replace junk items in the pool with pending junk
static void ReplaceMaxItem(const RandomizerGet itemToReplace, int max) {
  int itemCount = 0;
  for (size_t i = 0; i < ItemPool.size(); i++) {
    if (ItemPool[i] == itemToReplace) {
      if (itemCount >= max) {
        ItemPool[i] = GetJunkItem();
      }
      itemCount++;
    }
  }
}

void PlaceJunkInExcludedLocation(const RandomizerCheck il) {
  //place a non-advancement item in this location
  auto ctx = Rando::Context::GetInstance();
  for (size_t i = 0; i < ItemPool.size(); i++) {
    if (!Rando::StaticData::RetrieveItem(ItemPool[i]).IsAdvancement()) {
      ctx->PlaceItemInLocation(il, ItemPool[i]);
      ItemPool.erase(ItemPool.begin() + i);
      return;
    }
  }
  printf("ERROR: No Junk to Place!!!\n");
}

static void PlaceVanillaDekuScrubItems() {
    auto ctx = Rando::Context::GetInstance();
    ctx->PlaceItemInLocation(RC_ZR_DEKU_SCRUB_GROTTO_REAR,             RG_RED_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_ZR_DEKU_SCRUB_GROTTO_FRONT,            RG_GREEN_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_SFM_DEKU_SCRUB_GROTTO_REAR,            RG_RED_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_SFM_DEKU_SCRUB_GROTTO_FRONT,           RG_GREEN_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_LH_DEKU_SCRUB_GROTTO_LEFT,             RG_DEKU_NUTS_5, false, true);
    ctx->PlaceItemInLocation(RC_LH_DEKU_SCRUB_GROTTO_RIGHT,            RG_BOMBS_5, false, true);
    ctx->PlaceItemInLocation(RC_LH_DEKU_SCRUB_GROTTO_CENTER,           RG_DEKU_SEEDS_30, false, true);
    ctx->PlaceItemInLocation(RC_GV_DEKU_SCRUB_GROTTO_REAR,             RG_RED_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_GV_DEKU_SCRUB_GROTTO_FRONT,            RG_GREEN_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, RG_DEKU_NUTS_5, false, true);
    ctx->PlaceItemInLocation(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  RG_DEKU_STICK_1, false, true);
    ctx->PlaceItemInLocation(RC_LW_DEKU_SCRUB_GROTTO_REAR,             RG_DEKU_SEEDS_30, false, true);
    ctx->PlaceItemInLocation(RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,       RG_RED_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,      RG_GREEN_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_DMC_DEKU_SCRUB,                        RG_BOMBS_5, false, true);
    ctx->PlaceItemInLocation(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,            RG_DEKU_NUTS_5, false, true);
    ctx->PlaceItemInLocation(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,           RG_BOMBS_5, false, true);
    ctx->PlaceItemInLocation(RC_DMC_DEKU_SCRUB_GROTTO_CENTER,          RG_DEKU_SEEDS_30, false, true);
    ctx->PlaceItemInLocation(RC_GC_DEKU_SCRUB_GROTTO_LEFT,             RG_DEKU_NUTS_5, false, true);
    ctx->PlaceItemInLocation(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,            RG_BOMBS_5, false, true);
    ctx->PlaceItemInLocation(RC_GC_DEKU_SCRUB_GROTTO_CENTER,           RG_DEKU_SEEDS_30, false, true);
    ctx->PlaceItemInLocation(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,            RG_DEKU_NUTS_5, false, true);
    ctx->PlaceItemInLocation(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,           RG_BOMBS_5, false, true);
    ctx->PlaceItemInLocation(RC_LLR_DEKU_SCRUB_GROTTO_CENTER,          RG_DEKU_SEEDS_30, false, true);

    //Dungeon Scrubs
    if (DekuTree.IsMQ()) {
        ctx->PlaceItemInLocation(RC_DEKU_TREE_MQ_DEKU_SCRUB, RG_DEKU_SHIELD, false, true);
    }
    if (DodongosCavern.IsMQ()) {
        ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, RG_DEKU_STICK_1, false, true);
        ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, RG_DEKU_SEEDS_30, false, true);
        ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, RG_DEKU_SHIELD, false, true);
        ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RG_RED_POTION_REFILL,
                                 false, true);
    } else {
      ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,      RG_DEKU_NUTS_5, false, true);
      ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, RG_DEKU_STICK_1, false, true);
      ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,     RG_DEKU_SEEDS_30, false, true);
      ctx->PlaceItemInLocation(RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,                   RG_DEKU_SHIELD, false, true);
    }
    if (JabuJabusBelly.IsVanilla()) {
        ctx->PlaceItemInLocation(RC_JABU_JABUS_BELLY_DEKU_SCRUB, RG_DEKU_NUTS_5, false, true);
    }
    if (GanonsCastle.IsMQ()) {
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         RG_GREEN_POTION_REFILL, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  RG_BOMBS_5, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       RG_ARROWS_30, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, RG_RED_POTION_REFILL, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        RG_DEKU_NUTS_5, false, true);
    } else {
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  RG_BOMBS_5, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, RG_DEKU_SEEDS_30, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,        RG_RED_POTION_REFILL, false, true);
      ctx->PlaceItemInLocation(RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,         RG_GREEN_POTION_REFILL, false, true);
    }


}

static void PlaceVanillaMapsAndCompasses() {
  for (auto dungeon : dungeonList) {
    dungeon->PlaceVanillaMap();
    dungeon->PlaceVanillaCompass();
  }
}

static void PlaceVanillaSmallKeys() {
  for (auto dungeon : dungeonList) {
    dungeon->PlaceVanillaSmallKeys();
  }
}

static void PlaceVanillaBossKeys() {
  for (auto dungeon : dungeonList) {
    dungeon->PlaceVanillaBossKey();
  }
}

static void PlaceVanillaCowMilk() {
  auto ctx = Rando::Context::GetInstance();
  ctx->PlaceItemInLocation(RC_KF_LINKS_HOUSE_COW,    RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_HF_COW_GROTTO_COW,     RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_GV_COW,                RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_KAK_IMPAS_HOUSE_COW,   RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_DMT_COW_GROTTO_COW,    RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_LLR_STABLES_LEFT_COW,  RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_LLR_STABLES_RIGHT_COW, RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_LLR_TOWER_LEFT_COW,    RG_MILK, false, true);
  ctx->PlaceItemInLocation(RC_LLR_TOWER_RIGHT_COW,   RG_MILK, false, true);

  if (JabuJabusBelly.IsMQ()) {
    ctx->PlaceItemInLocation(RC_JABU_JABUS_BELLY_MQ_COW, RG_MILK, false, true);
  }
}

static void SetScarceItemPool() {
  ReplaceMaxItem(RG_PROGRESSIVE_BOMBCHUS, 3);
  ReplaceMaxItem(RG_BOMBCHU_5, 1);
  ReplaceMaxItem(RG_BOMBCHU_10, 2);
  ReplaceMaxItem(RG_BOMBCHU_20, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_MAGIC_METER, 1);
  ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_STICK_UPGRADE, 1);
  ReplaceMaxItem(RG_PROGRESSIVE_NUT_UPGRADE, 1);
  ReplaceMaxItem(RG_PROGRESSIVE_BOW, 2);
  ReplaceMaxItem(RG_PROGRESSIVE_SLINGSHOT, 2);
  ReplaceMaxItem(RG_PROGRESSIVE_BOMB_BAG, 2);
  ReplaceMaxItem(RG_HEART_CONTAINER, 0);
}

static void SetMinimalItemPool() {
  ReplaceMaxItem(RG_PROGRESSIVE_BOMBCHUS, 1);
  ReplaceMaxItem(RG_BOMBCHU_5, 1);
  ReplaceMaxItem(RG_BOMBCHU_10, 0);
  ReplaceMaxItem(RG_BOMBCHU_20, 0);
  ReplaceMaxItem(RG_NAYRUS_LOVE, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_MAGIC_METER, 1);
  ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_STICK_UPGRADE, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_NUT_UPGRADE, 0);
  ReplaceMaxItem(RG_PROGRESSIVE_BOW, 1);
  ReplaceMaxItem(RG_PROGRESSIVE_SLINGSHOT, 1);
  ReplaceMaxItem(RG_PROGRESSIVE_BOMB_BAG, 1);
  ReplaceMaxItem(RG_PIECE_OF_HEART, 0);
  // Need an extra heart container when starting with 1 heart to be able to reach 3 hearts
  ReplaceMaxItem(RG_HEART_CONTAINER, (StartingHearts.Value<uint8_t>() == 18)? 1 : 0);
}

void GenerateItemPool() {
  auto ctx = Rando::Context::GetInstance();
  ItemPool.clear();
  PendingJunkPool.clear();

  //Initialize ice trap models to always major items
  IceTrapModels = {
    GI_SHIELD_MIRROR,
    GI_BOOMERANG,
    GI_LENS,
    GI_HAMMER,
    GI_BOOTS_IRON,
    GI_BOOTS_HOVER,
    GI_STONE_OF_AGONY,
    GI_DINS_FIRE,
    GI_FARORES_WIND,
    GI_NAYRUS_LOVE,
    GI_ARROW_FIRE,
    GI_ARROW_ICE,
    GI_ARROW_LIGHT,
    0xB8, //Double defense
    GI_CLAIM_CHECK,
    0x80, //Progressive hookshot
    0x81, //Progressive strength
    0x82, //Progressive bomb bag
    0x83, //Progressive bow
    0x84, //Progressive slingshot
    0x85, //Progressive wallet
    0x86, //Progressive scale
    0x8A, //Progressive magic
  };
  //Check song shuffle and dungeon reward shuffle just for ice traps
  if (ShuffleSongs.Is(SONGSHUFFLE_ANYWHERE)) {
    //Push item ids for songs
    IceTrapModels.push_back(0xC1);
    IceTrapModels.push_back(0xC2);
    IceTrapModels.push_back(0xC3);
    IceTrapModels.push_back(0xC4);
    IceTrapModels.push_back(0xC5);
    IceTrapModels.push_back(0xC6);
    IceTrapModels.push_back(0xBB);
    IceTrapModels.push_back(0xBC);
    IceTrapModels.push_back(0xBD);
    IceTrapModels.push_back(0xBE);
    IceTrapModels.push_back(0xBF);
    IceTrapModels.push_back(0xC0);
  }
  if (ShuffleRewards.Is(REWARDSHUFFLE_ANYWHERE)) {
    //Push item ids for dungeon rewards
    IceTrapModels.push_back(0xCB);
    IceTrapModels.push_back(0xCC);
    IceTrapModels.push_back(0xCD);
    IceTrapModels.push_back(0xCE);
    IceTrapModels.push_back(0xCF);
    IceTrapModels.push_back(0xD0);
    IceTrapModels.push_back(0xD1);
    IceTrapModels.push_back(0xD2);
    IceTrapModels.push_back(0xD3);
  }

  if (TriforceHunt.Is(TRIFORCE_HUNT_ON)) {
    IceTrapModels.push_back(0xDF);
    AddItemToMainPool(RG_TRIFORCE_PIECE, Settings::TriforceHuntTotal.Value<uint8_t>());
    ctx->PlaceItemInLocation(RC_TRIFORCE_COMPLETED, RG_TRIFORCE); // Win condition
    ctx->PlaceItemInLocation(RC_GANON, GetJunkItem(), false, true);
  } else {
    ctx->PlaceItemInLocation(RC_GANON, RG_TRIFORCE); // Win condition
  }

  //Fixed item locations
  ctx->PlaceItemInLocation(RC_HC_ZELDAS_LETTER, RG_ZELDAS_LETTER);
  ctx->PlaceItemInLocation(RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS, RG_BOMBCHU_DROP);

  if (ShuffleKokiriSword) {
    AddItemToMainPool(RG_KOKIRI_SWORD);
    IceTrapModels.push_back(GI_SWORD_KOKIRI);
  } else {
    ctx->PlaceItemInLocation(RC_KF_KOKIRI_SWORD_CHEST, RG_KOKIRI_SWORD, false, true);
  }

  if (ShuffleWeirdEgg) {
    AddItemToMainPool(RG_WEIRD_EGG);
    IceTrapModels.push_back(GI_WEIRD_EGG);
  } else {
    ctx->PlaceItemInLocation(RC_HC_MALON_EGG, RG_WEIRD_EGG, false, true);
  }

  if (ShuffleOcarinas) {
    AddItemToMainPool(RG_PROGRESSIVE_OCARINA, 2);
    if (ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
      AddItemToPool(PendingJunkPool, RG_PROGRESSIVE_OCARINA);
    }
    IceTrapModels.push_back(0x8B); //Progressive ocarina
  } else {
    ctx->PlaceItemInLocation(RC_LW_GIFT_FROM_SARIA, RG_PROGRESSIVE_OCARINA, false, true);
    ctx->PlaceItemInLocation(RC_HF_OCARINA_OF_TIME_ITEM, RG_PROGRESSIVE_OCARINA, false, true);
  }

  if (ShuffleCows) {
    //9 total cow locations
    for (uint8_t i = 0; i < 9; i++) {
      AddItemToMainPool(GetJunkItem());
    }
    //extra location for Jabu MQ
    if (JabuJabusBelly.IsMQ()) {
      AddItemToMainPool(GetJunkItem());
    }
  } else {
    PlaceVanillaCowMilk();
  }

  if (ShuffleMagicBeans) {
    AddItemToMainPool(RG_MAGIC_BEAN_PACK);
    if (ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
      AddItemToPool(PendingJunkPool, RG_MAGIC_BEAN_PACK);
    }
    IceTrapModels.push_back(0xC9); //Magic bean pack
  } else {
    ctx->PlaceItemInLocation(RC_ZR_MAGIC_BEAN_SALESMAN, RG_MAGIC_BEAN, false, true);
  }

  if (ShuffleMerchants.IsNot(SHUFFLEMERCHANTS_OFF)) {
    if (!ProgressiveGoronSword) {
      AddItemToMainPool(RG_GIANTS_KNIFE);
    }
    if (BombchusInLogic) {
      AddItemToMainPool(RG_PROGRESSIVE_BOMBCHUS);
    } else {
      AddItemToMainPool(RG_BOMBCHU_10);
    }
  } else {
    ctx->PlaceItemInLocation(RC_KAK_GRANNYS_SHOP, RG_BLUE_POTION_REFILL, false, true);
    ctx->PlaceItemInLocation(RC_GC_MEDIGORON, RG_GIANTS_KNIFE, false, true);
    ctx->PlaceItemInLocation(RC_WASTELAND_BOMBCHU_SALESMAN, RG_BOMBCHU_10, false, true);
  }

  if (ShuffleFrogSongRupees) {
    AddItemToMainPool(RG_PURPLE_RUPEE, 5);
  } else {
    ctx->PlaceItemInLocation(RC_ZR_FROGS_ZELDAS_LULLABY, RG_PURPLE_RUPEE, false, true);
    ctx->PlaceItemInLocation(RC_ZR_FROGS_EPONAS_SONG, RG_PURPLE_RUPEE, false, true);
    ctx->PlaceItemInLocation(RC_ZR_FROGS_SARIAS_SONG, RG_PURPLE_RUPEE, false, true);
    ctx->PlaceItemInLocation(RC_ZR_FROGS_SUNS_SONG, RG_PURPLE_RUPEE, false, true);
    ctx->PlaceItemInLocation(RC_ZR_FROGS_SONG_OF_TIME, RG_PURPLE_RUPEE, false, true);
  }

  if (ShuffleAdultTradeQuest) {
    AddItemToMainPool(RG_POCKET_EGG);
    AddItemToMainPool(RG_COJIRO);
    AddItemToMainPool(RG_ODD_MUSHROOM);
    AddItemToMainPool(RG_ODD_POTION);
    AddItemToMainPool(RG_POACHERS_SAW);
    AddItemToMainPool(RG_BROKEN_SWORD);
    AddItemToMainPool(RG_PRESCRIPTION);
    AddItemToMainPool(RG_EYEBALL_FROG);
    AddItemToMainPool(RG_EYEDROPS);
  } else {
    ctx->PlaceItemInLocation(RC_KAK_TRADE_POCKET_CUCCO, RG_COJIRO, false, true);
    ctx->PlaceItemInLocation(RC_LW_TRADE_COJIRO, RG_ODD_MUSHROOM, false, true);
    ctx->PlaceItemInLocation(RC_KAK_TRADE_ODD_MUSHROOM, RG_ODD_POTION, false, true);
    ctx->PlaceItemInLocation(RC_LW_TRADE_ODD_POTION, RG_POACHERS_SAW, false, true);
    ctx->PlaceItemInLocation(RC_GV_TRADE_SAW, RG_BROKEN_SWORD, false, true);
    ctx->PlaceItemInLocation(RC_DMT_TRADE_BROKEN_SWORD, RG_PRESCRIPTION, false, true);
    ctx->PlaceItemInLocation(RC_ZD_TRADE_PRESCRIPTION, RG_EYEBALL_FROG, false, true);
    ctx->PlaceItemInLocation(RC_LH_TRADE_FROG, RG_EYEDROPS, false, true);
    ctx->PlaceItemInLocation(RC_DMT_TRADE_EYEDROPS, RG_CLAIM_CHECK, false, true);
  }
  AddItemToMainPool(RG_CLAIM_CHECK);

  if (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS)) {
    AddItemToMainPool(RG_TREASURE_GAME_SMALL_KEY, 6); // 6 individual keys
  } else if (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK)) {
    AddItemToMainPool(RG_TREASURE_GAME_SMALL_KEY); // 1 key which will behave as a pack of 6
  } else {
    ctx->PlaceItemInLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, RG_TREASURE_GAME_SMALL_KEY, false, true);
    ctx->PlaceItemInLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, RG_TREASURE_GAME_SMALL_KEY, false, true);
    ctx->PlaceItemInLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, RG_TREASURE_GAME_SMALL_KEY, false, true);
    ctx->PlaceItemInLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, RG_TREASURE_GAME_SMALL_KEY, false, true);
    ctx->PlaceItemInLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, RG_TREASURE_GAME_SMALL_KEY, false, true);
  };

  if (Tokensanity.Is(TOKENSANITY_OFF)) {
    for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, Category::cSkulltula)) {
      ctx->PlaceItemInLocation(loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
    }
  } else if (Tokensanity.Is(TOKENSANITY_DUNGEONS)) {
    for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, Category::cSkulltula)) {
      if (Rando::StaticData::GetLocation(loc)->IsOverworld()) {
          ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
      } else {
        AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN);
      }
    }
  } else if (Tokensanity.Is(TOKENSANITY_OVERWORLD)) {
    for (RandomizerCheck loc : ctx->GetLocations(ctx->allLocations, Category::cSkulltula)) {
      if (Rando::StaticData::GetLocation(loc)->IsDungeon()) {
          ctx->PlaceItemInLocation((RandomizerCheck)loc, RG_GOLD_SKULLTULA_TOKEN, false, true);
      } else {
        AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN);
      }
    }
  } else {
    AddItemToMainPool(RG_GOLD_SKULLTULA_TOKEN, 100);
  }

  if (Shuffle100GSReward) {
    if (Tokensanity.IsNot(TOKENSANITY_OFF) && ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
      AddItemToPool(PendingJunkPool, RG_GOLD_SKULLTULA_TOKEN, 10);
    }
    AddItemToMainPool(RG_HUGE_RUPEE);
  } else {
    ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_HUGE_RUPEE, false, true);
  }

  if (BombchusInLogic) {
    AddItemToMainPool(RG_PROGRESSIVE_BOMBCHUS, 5);
  } else {
    AddItemToMainPool(RG_BOMBCHU_5);
    AddItemToMainPool(RG_BOMBCHU_10, 3);
    AddItemToMainPool(RG_BOMBCHU_20);
  }

  //Ice Traps
  AddItemToMainPool(RG_ICE_TRAP);
  if (GerudoTrainingGrounds.IsVanilla()) {
    AddItemToMainPool(RG_ICE_TRAP);
  }
  if (GanonsCastle.IsVanilla()) {
    AddItemToMainPool(RG_ICE_TRAP, 4);
  }

  //Gerudo Fortress
  if (GerudoFortress.Is(GERUDOFORTRESS_OPEN)) {
    ctx->PlaceItemInLocation(RC_GF_NORTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
    ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
    ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
    ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
  } else if (GerudoKeys.IsNot(GERUDOKEYS_VANILLA)) {
    if (GerudoFortress.Is(GERUDOFORTRESS_FAST)) {
      AddItemToMainPool(RG_GERUDO_FORTRESS_SMALL_KEY);
      ctx->PlaceItemInLocation(RC_GF_NORTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
      ctx->PlaceItemInLocation(RC_GF_SOUTH_F1_CARPENTER, RG_RECOVERY_HEART, false, true);
      ctx->PlaceItemInLocation(RC_GF_SOUTH_F2_CARPENTER, RG_RECOVERY_HEART, false, true);
    } else {
      //Only add key ring if 4 Fortress keys necessary
      if (RingFortress) {
        AddItemToMainPool(RG_GERUDO_FORTRESS_KEY_RING);
        //Add junk to make up for missing keys
        for (uint8_t i = 0; i < 3; i++) {
          AddItemToMainPool(GetJunkItem());
        }
      } else {
        AddItemToMainPool(RG_GERUDO_FORTRESS_SMALL_KEY, 4);
      }
    }
    if (ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
      if (RingFortress && GerudoFortress.Is(GERUDOFORTRESS_NORMAL)) {
        AddItemToPool(PendingJunkPool, RG_GERUDO_FORTRESS_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_GERUDO_FORTRESS_SMALL_KEY);
      }
    }
  } else {
    if (GerudoFortress.Is(GERUDOFORTRESS_FAST)) {
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

  //Gerudo Membership Card
  if (ShuffleGerudoToken && GerudoFortress.IsNot(GERUDOFORTRESS_OPEN)) {
    AddItemToMainPool(RG_GERUDO_MEMBERSHIP_CARD);
    IceTrapModels.push_back(GI_GERUDO_CARD);
  } else if (ShuffleGerudoToken) {
    AddItemToPool(PendingJunkPool, RG_GERUDO_MEMBERSHIP_CARD);
    ctx->PlaceItemInLocation(RC_GF_GERUDO_MEMBERSHIP_CARD, RG_ICE_TRAP, false, true);
  } else {
    ctx->PlaceItemInLocation(RC_GF_GERUDO_MEMBERSHIP_CARD, RG_GERUDO_MEMBERSHIP_CARD, false, true);
  }

  //Keys

  //For key rings, need to add as many junk items as "missing" keys
  if (KeyRings.IsNot(KEYRINGS_OFF)) {
    uint8_t ringJunkAmt = 0;
    for (auto dungeon : dungeonList) {
      if (dungeon->HasKeyRing()) {
        ringJunkAmt += dungeon->GetSmallKeyCount() - 1;
      }
    }
    for (uint8_t i = 0; i < ringJunkAmt; i++) {
        AddItemToMainPool(GetJunkItem());
    }
  }

  if (ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
    if (ShuffleGerudoToken) {
      AddItemToPool(PendingJunkPool, RG_GERUDO_MEMBERSHIP_CARD);
    }

    //Plentiful small keys
    if (Keysanity.Is(KEYSANITY_ANYWHERE) || Keysanity.Is(KEYSANITY_ANY_DUNGEON) || Keysanity.Is(KEYSANITY_OVERWORLD)) {
      if (BottomOfTheWell.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_BOTTOM_OF_THE_WELL_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_BOTTOM_OF_THE_WELL_SMALL_KEY);
      }
      if (ForestTemple.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_SMALL_KEY);
      }
      if (FireTemple.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_SMALL_KEY);
      }
      if (WaterTemple.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_SMALL_KEY);
      }
      if (SpiritTemple.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_SMALL_KEY);
      }
      if (ShadowTemple.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_SMALL_KEY);
      }
      if (GerudoTrainingGrounds.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_GERUDO_TRAINING_GROUNDS_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY);
      }
      if (GanonsCastle.HasKeyRing()) {
        AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_KEY_RING);
      } else {
        AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_SMALL_KEY);
      } 
    }

    if (BossKeysanity.Is(BOSSKEYSANITY_ANYWHERE) || BossKeysanity.Is(BOSSKEYSANITY_ANY_DUNGEON) || BossKeysanity.Is(BOSSKEYSANITY_OVERWORLD)) {
      AddItemToPool(PendingJunkPool, RG_FOREST_TEMPLE_BOSS_KEY);
      AddItemToPool(PendingJunkPool, RG_FIRE_TEMPLE_BOSS_KEY);
      AddItemToPool(PendingJunkPool, RG_WATER_TEMPLE_BOSS_KEY);
      AddItemToPool(PendingJunkPool, RG_SPIRIT_TEMPLE_BOSS_KEY);
      AddItemToPool(PendingJunkPool, RG_SHADOW_TEMPLE_BOSS_KEY);
    }

    if (GanonsBossKey.Is(GANONSBOSSKEY_ANYWHERE) || GanonsBossKey.Is(GANONSBOSSKEY_ANY_DUNGEON) || GanonsBossKey.Is(GANONSBOSSKEY_OVERWORLD)) {
      AddItemToPool(PendingJunkPool, RG_GANONS_CASTLE_BOSS_KEY);
    }
  }

  //Shopsanity
  if (Settings::Shopsanity.Is(SHOPSANITY_OFF) || Settings::Shopsanity.Is(SHOPSANITY_ZERO)) {
    AddItemsToPool(ItemPool, normalRupees);
  } else { //Shopsanity 1-4, random
    AddItemsToPool(ItemPool, shopsanityRupees); //Shopsanity gets extra large rupees
  }

  //Scrubsanity
  if (Settings::Scrubsanity.IsNot(SCRUBSANITY_OFF)) {
    //Deku Tree
    if (DekuTree.IsMQ()) {
      AddItemToMainPool(RG_DEKU_SHIELD);
    }

    //Dodongos Cavern
    AddItemToMainPool(RG_DEKU_STICK_1);
    AddItemToMainPool(RG_DEKU_SHIELD);
    if (DodongosCavern.IsMQ()) {
      AddItemToMainPool(RG_RECOVERY_HEART);
    } else {
      AddItemToMainPool(RG_DEKU_NUTS_5);
    }

    //Jabu Jabus Belly
    if (JabuJabusBelly.IsVanilla()) {
      AddItemToMainPool(RG_DEKU_NUTS_5);
    }

    //Ganons Castle
    AddItemToMainPool(RG_BOMBS_5);
    AddItemToMainPool(RG_RECOVERY_HEART);
    AddItemToMainPool(RG_BLUE_RUPEE);
    if (GanonsCastle.IsMQ()) {
      AddItemToMainPool(RG_DEKU_NUTS_5);
    }

    //Overworld Scrubs
    AddItemsToPool(ItemPool, dekuScrubItems);

    //I'm not sure what this is for, but it was in ootr so I copied it
    for (uint8_t i = 0; i < 7; i++) {
      if (Random(0, 3)) {
        AddItemToMainPool(RG_ARROWS_30);
      } else {
        AddItemToMainPool(RG_DEKU_SEEDS_30);
      }
    }
  } else {
    PlaceVanillaDekuScrubItems();
  }

  AddItemsToPool(ItemPool, alwaysItems);
  AddItemsToPool(ItemPool, dungeonRewards);

  //Dungeon pools
  if (DekuTree.IsMQ()) {
    AddItemsToPool(ItemPool, DT_MQ);
  } else {
    AddItemsToPool(ItemPool, DT_Vanilla);
  }
  if (DodongosCavern.IsMQ()) {
    AddItemsToPool(ItemPool, DC_MQ);
  } else {
    AddItemsToPool(ItemPool, DC_Vanilla);
  }
  if (JabuJabusBelly.IsMQ()) {
    AddItemsToPool(ItemPool, JB_MQ);
  }
  if (ForestTemple.IsMQ()) {
    AddItemsToPool(ItemPool, FoT_MQ);
  } else {
    AddItemsToPool(ItemPool, FoT_Vanilla);
  }
  if (FireTemple.IsMQ()) {
    AddItemsToPool(ItemPool, FiT_MQ);
  } else {
    AddItemsToPool(ItemPool, FiT_Vanilla);
  }
  if (SpiritTemple.IsMQ()) {
    AddItemsToPool(ItemPool, SpT_MQ);
  } else {
    AddItemsToPool(ItemPool, SpT_Vanilla);
  }
  if (ShadowTemple.IsMQ()) {
    AddItemsToPool(ItemPool, ShT_MQ);
  } else {
    AddItemsToPool(ItemPool, ShT_Vanilla);
  }
  if (BottomOfTheWell.IsVanilla()) {
    AddItemsToPool(ItemPool, BW_Vanilla);
  }
  if (GerudoTrainingGrounds.IsMQ()) {
    AddItemsToPool(ItemPool, GTG_MQ);
  } else {
    AddItemsToPool(ItemPool, GTG_Vanilla);
  }
  if (GanonsCastle.IsMQ()) {
    AddItemsToPool(ItemPool, GC_MQ);
  } else {
    AddItemsToPool(ItemPool, GC_Vanilla);
  }

  uint8_t rutoBottles = 1;
  if (ZorasFountain.Is(ZORASFOUNTAIN_OPEN)) {
    rutoBottles = 0;
  }

  //Add 4 total bottles
  uint8_t bottleCount = 4;
  std::vector<RandomizerGet> bottles;
  bottles.assign(normalBottles.begin(), normalBottles.end());
  IceTrapModels.push_back(
      Rando::StaticData::RetrieveItem(RandomElement(bottles)).GetItemID()); // Get one random bottle type for ice traps
  for (uint8_t i = 0; i < bottleCount; i++) {
    if (i >= rutoBottles) {
      if ((i == bottleCount - 1) && ShuffleMerchants.IsNot(SHUFFLEMERCHANTS_OFF)) {
        AddItemToMainPool(RG_BOTTLE_WITH_BLUE_POTION);
      } else {
        AddRandomBottle(bottles);
      }
    } else {
      AddItemToMainPool(RG_RUTOS_LETTER);
    }
  }

  //add extra songs only if song shuffle is anywhere
  AddItemsToPool(ItemPool, songList);
  if (ShuffleSongs.Is(SONGSHUFFLE_ANYWHERE) && ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
    AddItemsToPool(PendingJunkPool, songList);
  }

  /*For item pool generation, dungeon items are either placed in their vanilla
  | location, or added to the pool now and filtered out later depending on when
  | they need to get placed or removed in fill.cpp. These items are kept in the
  | pool until removal because the filling algorithm needs to know all of the
  | advancement items that haven't been placed yet for placing higher priority
  | items like stones/medallions.*/

  if (MapsAndCompasses.Is(MAPSANDCOMPASSES_VANILLA)) {
    PlaceVanillaMapsAndCompasses();
  } else  {
    for (auto dungeon : dungeonList) {
      if (dungeon->GetMap() != RG_NONE) {
        AddItemToMainPool(dungeon->GetMap());
      }

      if (dungeon->GetCompass() != RG_NONE) {
        AddItemToMainPool(dungeon->GetCompass());
      }
    }
  }

  if (Keysanity.Is(KEYSANITY_VANILLA)) {
    PlaceVanillaSmallKeys();
  } else {
    for (auto dungeon : dungeonList) {
      if (dungeon->HasKeyRing() && Keysanity.IsNot(KEYSANITY_START_WITH)) {
        AddItemToMainPool(dungeon->GetKeyRing());
      } else {
        if (dungeon->GetSmallKeyCount() > 0) {
          AddItemToMainPool(dungeon->GetSmallKey(), dungeon->GetSmallKeyCount());
        }
      }
    }
  }

  if (BossKeysanity.Is(BOSSKEYSANITY_VANILLA)) {
    PlaceVanillaBossKeys();
  } else {
    AddItemToMainPool(RG_FOREST_TEMPLE_BOSS_KEY);
    AddItemToMainPool(RG_FIRE_TEMPLE_BOSS_KEY);
    AddItemToMainPool(RG_WATER_TEMPLE_BOSS_KEY);
    AddItemToMainPool(RG_SPIRIT_TEMPLE_BOSS_KEY);
    AddItemToMainPool(RG_SHADOW_TEMPLE_BOSS_KEY);
  }

  if (GanonsBossKey.Is(GANONSBOSSKEY_FINAL_GS_REWARD)) {
    ctx->PlaceItemInLocation(RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_GANONS_CASTLE_BOSS_KEY);
  } else if (GanonsBossKey.Value<uint8_t>() >= GANONSBOSSKEY_LACS_VANILLA && GanonsBossKey.IsNot(GANONSBOSSKEY_TRIFORCE_HUNT)) {
    ctx->PlaceItemInLocation(RC_TOT_LIGHT_ARROWS_CUTSCENE, RG_GANONS_CASTLE_BOSS_KEY);
  } else if (GanonsBossKey.Is(GANONSBOSSKEY_VANILLA)) {
    ctx->PlaceItemInLocation(RC_GANONS_TOWER_BOSS_KEY_CHEST, RG_GANONS_CASTLE_BOSS_KEY);
  } else {
    AddItemToMainPool(RG_GANONS_CASTLE_BOSS_KEY);
  }

  if (ItemPoolValue.Is(ITEMPOOL_PLENTIFUL)) {
    AddItemsToPool(ItemPool, easyItems);
  } else {
    AddItemsToPool(ItemPool, normalItems);
  }

  if (!ShuffleKokiriSword) {
    ReplaceMaxItem(RG_KOKIRI_SWORD, 0);
  }

  if (ProgressiveGoronSword) {
    ReplaceMaxItem(RG_BIGGORON_SWORD, 0);
    AddItemToMainPool(RG_PROGRESSIVE_GORONSWORD, 2);
    IceTrapModels.push_back(0xD4); // Progressive Goron Sword
  } else {
    IceTrapModels.push_back(GI_SWORD_BGS);
  }

  //Replace ice traps with junk from the pending junk pool if necessary
  if (IceTrapValue.Is(ICETRAPS_OFF)) {
    ReplaceMaxItem(RG_ICE_TRAP, 0);
  }
  //Replace all junk items with ice traps for onslaught mode
  else if (IceTrapValue.Is(ICETRAPS_ONSLAUGHT)) {
    for (uint8_t i = 0; i < JunkPoolItems.size() - 3; i++) { // -3 Omits Huge Rupees and Deku Nuts 10
      ReplaceMaxItem(JunkPoolItems[i], 0);
    }
  }

  if (ItemPoolValue.Is(ITEMPOOL_SCARCE)) {
    SetScarceItemPool();
  } else if (ItemPoolValue.Is(ITEMPOOL_MINIMAL)) {
    SetMinimalItemPool();
  } else if (RemoveDoubleDefense) {
    ReplaceMaxItem(RG_DOUBLE_DEFENSE, 0);
  }

  //this feels ugly and there's probably a better way, but
  //it replaces random junk with pending junk.
  bool junkSet;
  for (RandomizerGet pendingJunk : PendingJunkPool) {
    junkSet = false;
    for (RandomizerGet& item : ItemPool) {
      for (RandomizerGet junk : JunkPoolItems) {
        if (item == junk && item != RG_HUGE_RUPEE && item != RG_DEKU_NUTS_10) {
          item = pendingJunk;
          junkSet = true;
          break;
        }
      }
      if (junkSet) break;
    }
  }
  PendingJunkPool.clear();
}

void AddJunk() {
  SPDLOG_DEBUG("HAD TO PLACE EXTRA JUNK ");
  AddItemToMainPool(GetPendingJunkItem());
}
