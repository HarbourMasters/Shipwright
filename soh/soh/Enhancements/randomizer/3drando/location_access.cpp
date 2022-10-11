#include "location_access.hpp"

#include "dungeon.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "item_pool.hpp"
#include "logic.hpp"
#include "settings.hpp"
#include "spoiler_log.hpp"
#include "trial.hpp"
#include "entrance.hpp"

#include <fstream>
#include <iostream>

using namespace Logic;
using namespace Settings;

//generic grotto event list
std::vector<EventAccess> grottoEvents = {
  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(STICKS));}}),
  EventAccess(&BugShrub,         {[]{return CanCutShrubs;}}),
  EventAccess(&LoneFish,         {[]{return true;}}),
};

//set the logic to be a specific age and time of day and see if the condition still holds
bool LocationAccess::CheckConditionAtAgeTime(bool& age, bool& time) const {

  IsChild = false;
  IsAdult = false;
  AtDay   = false;
  AtNight = false;

  time = true;
  age = true;

  UpdateHelpers();
  return GetConditionsMet();
}

bool LocationAccess::ConditionsMet() const {

  Area* parentRegion = AreaTable(Location(location)->GetParentRegionKey());
  bool conditionsMet = false;

  if ((parentRegion->childDay   && CheckConditionAtAgeTime(IsChild, AtDay))   ||
      (parentRegion->childNight && CheckConditionAtAgeTime(IsChild, AtNight)) ||
      (parentRegion->adultDay   && CheckConditionAtAgeTime(IsAdult, AtDay))   ||
      (parentRegion->adultNight && CheckConditionAtAgeTime(IsAdult, AtNight))) {
        conditionsMet = true;
  }

  return conditionsMet && CanBuy();
}

bool LocationAccess::CanBuy() const {
  //Not a shop location, don't need to check if buyable
  if (!(Location(location)->IsCategory(Category::cShop))) {
    return true;
  }

  //Check if wallet is large enough to buy item
  bool SufficientWallet = true;
  if (Location(location)->GetPrice() > 500) {
    SufficientWallet = ProgressiveWallet >= 3;
  } else if (Location(location)->GetPrice() > 200) {
    SufficientWallet = ProgressiveWallet >= 2;
  } else if (Location(location)->GetPrice() > 99) {
    SufficientWallet = ProgressiveWallet >= 1;
  }

  bool OtherCondition = true;
  uint32_t placed = Location(location)->GetPlacedItemKey();
  //Need bottle to buy bottle items, only logically relevant bottle items included here
  if (placed == BUY_BLUE_FIRE || placed == BUY_BOTTLE_BUG || placed == BUY_FISH || placed == BUY_FAIRYS_SPIRIT) {
    OtherCondition = HasBottle;
  }
  //If bombchus in logic, need to have found chus to buy; if not just need bomb bag
  else if (placed == BUY_BOMBCHU_10 || placed == BUY_BOMBCHU_20) {
    OtherCondition = (!BombchusInLogic && Bombs) || (BombchusInLogic && FoundBombchus);
  }

  return SufficientWallet && OtherCondition;
}

Area::Area() = default;
Area::Area(std::string regionName_, std::string scene_, uint32_t hintKey_,
         bool timePass_,
         std::vector<EventAccess> events_,
         std::vector<LocationAccess> locations_,
         std::list<Entrance> exits_)
  : regionName(std::move(regionName_)),
    scene(std::move(scene_)),
    hintKey(hintKey_),
    timePass(timePass_),
    events(std::move(events_)),
    locations(std::move(locations_)),
    exits(std::move(exits_)) {}

Area::~Area() = default;

bool Area::UpdateEvents(SearchMode mode) {

  if (timePass && mode != SearchMode::TimePassAccess) {
    if (Child()) {
      childDay = true;
      childNight = true;
      AreaTable(ROOT)->childDay = true;
      AreaTable(ROOT)->childNight = true;
    }
    if (Adult()) {
      adultDay = true;
      adultNight = true;
      AreaTable(ROOT)->adultDay = true;
      AreaTable(ROOT)->adultNight = true;
    }
  }

  bool eventsUpdated =  false;

  for (EventAccess& event : events) {
    //If the event has already happened, there's no reason to check it
    if (event.GetEvent()) {
      continue;
    }

    if ((childDay   && event.CheckConditionAtAgeTime(IsChild, AtDay))    ||
        (childNight && event.CheckConditionAtAgeTime(IsChild, AtNight))  ||
        (adultDay   && event.CheckConditionAtAgeTime(IsAdult, AtDay))    ||
        (adultNight && event.CheckConditionAtAgeTime(IsAdult, AtNight))) {
          event.EventOccurred();
          eventsUpdated = true;
    }
  }
  return eventsUpdated;
}

void Area::AddExit(uint32_t parentKey, uint32_t newExitKey, ConditionFn condition) {
  Entrance newExit = Entrance(newExitKey, {condition});
  newExit.SetParentRegion(parentKey);
  exits.push_front(newExit);
}

//The exit will be completely removed from this area
void Area::RemoveExit(Entrance* exitToRemove) {
  exits.remove_if([exitToRemove](const auto exit){return &exit == exitToRemove;});
}

void Area::SetAsPrimary(uint32_t exitToBePrimary) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToBePrimary) {
      exit.SetAsPrimary();
      return;
    }
  }
}

Entrance* Area::GetExit(uint32_t exitToReturn) {
  for (auto& exit : exits) {
    if (exit.Getuint32_t() == exitToReturn) {
      return &exit;
    }
  }
  auto message = "ERROR: EXIT " + AreaTable(exitToReturn)->regionName + " DOES NOT EXIST IN " + this->regionName;
  CitraPrint(message);
  return nullptr;
}

bool Area::CanPlantBeanCheck() const {
  return (Logic::MagicBean || Logic::MagicBeanPack) && BothAgesCheck();
}

bool Area::AllAccountedFor() const {
  for (const EventAccess& event : events) {
    if (!event.GetEvent()) {
      return false;
    }
  }

  for (const LocationAccess& loc : locations) {
    if (!(Location(loc.GetLocation())->IsAddedToPool())) {
      return false;
    }
  }

  for (const auto& exit : exits) {
    if (!exit.GetConnectedRegion()->AllAccess()) {
      return false;
    }
  }

  return AllAccess();
}

bool Area::CheckAllAccess(const uint32_t exitKey) {
  if (!AllAccess()) {
    return false;
  }

  for (Entrance& exit : exits) {
    if (exit.Getuint32_t() == exitKey) {
      return exit.CheckConditionAtAgeTime(Logic::IsChild, Logic::AtDay)   &&
             exit.CheckConditionAtAgeTime(Logic::IsChild, Logic::AtNight) &&
             exit.CheckConditionAtAgeTime(Logic::IsAdult, Logic::AtDay)   &&
             exit.CheckConditionAtAgeTime(Logic::IsAdult, Logic::AtNight);
    }
  }
  return false;
}

void Area::ResetVariables() {
  childDay = false;
  childNight = false;
  adultDay = false;
  adultNight = false;
  addedToPool = false;
  for (auto& exit : exits) {
    exit.RemoveFromPool();
  }
}

std::array<Area, KEY_ENUM_MAX> areaTable;

bool Here(const uint32_t area, ConditionFn condition) {
  return areaTable[area].HereCheck(condition);
}

bool CanPlantBean(const uint32_t area) {
  return areaTable[area].CanPlantBeanCheck();
}

bool BothAges(const uint32_t area) {
  return areaTable[area].BothAgesCheck();
}

bool ChildCanAccess(const uint32_t area) {
  return areaTable[area].Child();
}

bool AdultCanAccess(const uint32_t area) {
  return areaTable[area].Adult();
}

bool HasAccessTo(const uint32_t area) {
  return areaTable[area].HasAccess();
}



void AreaTable_Init() {
  //Clear the array from any previous playthrough attempts. This is important so that
  //locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
  areaTable.fill(Area("Invalid Area", "Invalid Area", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {}));

                       //name, scene, hint text,                       events, locations, exits
  areaTable[ROOT] = Area("Root", "", LINKS_POCKET, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LINKS_POCKET, {[]{return true;}})
                }, {
                  //Exits
                  Entrance(ROOT_EXITS, {[]{return true;}})
  });

  areaTable[ROOT_EXITS] = Area("Root Exits", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KF_LINKS_HOUSE,            {[]{return IsChild;}}),
                  Entrance(TEMPLE_OF_TIME,            {[]{return (CanPlay(PreludeOfLight)   && CanLeaveForest) || IsAdult;},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && PreludeOfLight   && CanLeaveForest;}}),
                  Entrance(SACRED_FOREST_MEADOW,      {[]{return CanPlay(MinuetOfForest);},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && MinuetOfForest;}}),
                  Entrance(DMC_CENTRAL_LOCAL,         {[]{return CanPlay(BoleroOfFire)     && CanLeaveForest;},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && BoleroOfFire     && CanLeaveForest;}}),
                  Entrance(LAKE_HYLIA,                {[]{return CanPlay(SerenadeOfWater)  && CanLeaveForest;},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && SerenadeOfWater  && CanLeaveForest;}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION, {[]{return CanPlay(NocturneOfShadow) && CanLeaveForest;},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && NocturneOfShadow && CanLeaveForest;}}),
                  Entrance(DESERT_COLOSSUS,           {[]{return CanPlay(RequiemOfSpirit)  && CanLeaveForest;},
                                           /*Glitched*/[]{return (((IsChild && (ChildCanAccess(KOKIRI_FOREST) || ChildCanAccess(HYRULE_FIELD) || ChildCanAccess(LAKE_HYLIA))) || (IsAdult && (AdultCanAccess(KOKIRI_FOREST) || AdultCanAccess(HYRULE_FIELD) || AdultCanAccess(LAKE_HYLIA)))) && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) ||
                                                                 ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (IsAdult || KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) && RequiemOfSpirit  && CanLeaveForest;}}),
  });

  // Overworld
  AreaTable_Init_LostWoods();
  AreaTable_Init_HyruleField();
  AreaTable_Init_CastleTown();
  AreaTable_Init_Kakariko();
  AreaTable_Init_DeathMountain();
  AreaTable_Init_ZorasDomain();
  AreaTable_Init_GerudoValley();
  // Dungeons
  AreaTable_Init_DekuTree();
  AreaTable_Init_DodongosCavern();
  AreaTable_Init_JabuJabusBelly();
  AreaTable_Init_ForestTemple();
  AreaTable_Init_FireTemple();
  AreaTable_Init_WaterTemple();
  AreaTable_Init_SpiritTemple();
  AreaTable_Init_ShadowTemple();
  AreaTable_Init_BottomOfTheWell();
  AreaTable_Init_IceCavern();
  AreaTable_Init_GerudoTrainingGrounds();
  AreaTable_Init_GanonsCastle();

  //Set parent regions
  for (uint32_t i = ROOT; i <= GANONS_CASTLE; i++) {
    for (LocationAccess& locPair : areaTable[i].locations) {
      uint32_t location = locPair.GetLocation();
      Location(location)->SetParentRegion(i);
    }
    for (Entrance& exit : areaTable[i].exits) {
      exit.SetParentRegion(i);
      exit.SetName();
      exit.GetConnectedRegion()->entrances.push_front(&exit);
    }
  }
  /*
  //Events
}, {
  //Locations
}, {
  //Exits
*/
}

namespace Areas {

  static std::array<const uint32_t, 414> allAreas = {
    ROOT,
    ROOT_EXITS,

    KOKIRI_FOREST,
    KF_LINKS_HOUSE,
    KF_MIDOS_HOUSE,
    KF_SARIAS_HOUSE,
    KF_HOUSE_OF_TWINS,
    KF_KNOW_IT_ALL_HOUSE,
    KF_KOKIRI_SHOP,
    KF_STORMS_GROTTO,
    KF_OUTSIDE_DEKU_TREE,
    DEKU_TREE_ENTRYWAY,

    THE_LOST_WOODS,
    LW_BRIDGE_FROM_FOREST,
    LW_BRIDGE,
    LW_FOREST_EXIT,
    LW_BEYOND_MIDO,
    LW_NEAR_SHORTCUTS_GROTTO,
    DEKU_THEATER,
    LW_SCRUBS_GROTTO,
    SFM_ENTRYWAY,
    SACRED_FOREST_MEADOW,
    SFM_WOLFOS_GROTTO,
    SFM_FAIRY_GROTTO,
    SFM_STORMS_GROTTO,
    FOREST_TEMPLE_ENTRYWAY,

    KAKARIKO_VILLAGE,
    KAK_CARPENTER_BOSS_HOUSE,
    KAK_HOUSE_OF_SKULLTULA,
    KAK_IMPAS_HOUSE,
    KAK_IMPAS_LEDGE,
    KAK_IMPAS_HOUSE_BACK,
    KAK_IMPAS_HOUSE_NEAR_COW,
    KAK_WINDMILL,
    KAK_BAZAAR,
    KAK_SHOOTING_GALLERY,
    KAK_POTION_SHOP_FRONT,
    KAK_POTION_SHOP_BACK,
    KAK_ROOFTOP,
    KAK_BEHIND_GATE,
    KAK_BACKYARD,
    KAK_ODD_POTION_BUILDING,
    KAK_REDEAD_GROTTO,
    KAK_OPEN_GROTTO,
    BOTTOM_OF_THE_WELL_ENTRYWAY,

    THE_GRAVEYARD,
    GRAVEYARD_DAMPES_GRAVE,
    GRAVEYARD_DAMPES_HOUSE,
    GRAVEYARD_SHIELD_GRAVE,
    GRAVEYARD_COMPOSERS_GRAVE,
    GRAVEYARD_HEART_PIECE_GRAVE,
    GRAVEYARD_WARP_PAD_REGION,
    SHADOW_TEMPLE_ENTRYWAY,

    DEATH_MOUNTAIN_TRAIL,
    DEATH_MOUNTAIN_SUMMIT,
    DMT_OWL_FLIGHT,
    DMT_GREAT_FAIRY_FOUNTAIN,
    DMT_COW_GROTTO,
    DMT_STORMS_GROTTO,
    DODONGOS_CAVERN_ENTRYWAY,

    DMC_UPPER_LOCAL,
    DMC_CENTRAL_LOCAL,
    DMC_LOWER_LOCAL,
    DMC_LOWER_NEARBY,
    DMC_UPPER_NEARBY,
    DMC_CENTRAL_NEARBY,
    DMC_LADDER_AREA_NEARBY,
    DMC_UPPER_GROTTO,
    DMC_HAMMER_GROTTO,
    DMC_GREAT_FAIRY_FOUNTAIN,
    FIRE_TEMPLE_ENTRYWAY,

    GORON_CITY,
    GC_WOODS_WARP,
    GC_DARUNIAS_CHAMBER,
    GC_GROTTO_PLATFORM,
    GC_SHOP,
    GC_GROTTO,

    ZR_FRONT,
    ZORAS_RIVER,
    ZR_BEHIND_WATERFALL,
    ZR_OPEN_GROTTO,
    ZR_FAIRY_GROTTO,
    ZR_STORMS_GROTTO,
    ZORAS_DOMAIN,
    ZD_BEHIND_KING_ZORA,
    ZD_SHOP,
    ZD_STORMS_GROTTO,
    ZORAS_FOUNTAIN,
    ZF_GREAT_FAIRY_FOUNTAIN,
    JABU_JABUS_BELLY_ENTRYWAY,
    ICE_CAVERN_ENTRYWAY,

    HYRULE_FIELD,
    HF_SOUTHEAST_GROTTO,
    HF_OPEN_GROTTO,
    HF_INSIDE_FENCE_GROTTO,
    HF_COW_GROTTO,
    HF_NEAR_MARKET_GROTTO,
    HF_FAIRY_GROTTO,
    HF_NEAR_KAK_GROTTO,
    HF_TEKTITE_GROTTO,

    LON_LON_RANCH,
    LLR_TALONS_HOUSE,
    LLR_STABLES,
    LLR_TOWER,
    LLR_GROTTO,

    LAKE_HYLIA,
    LH_OWL_FLIGHT,
    LH_LAB,
    LH_FISHING_ISLAND,
    LH_FISHING_HOLE,
    LH_GROTTO,
    WATER_TEMPLE_ENTRYWAY,

    GERUDO_VALLEY,
    GV_UPPER_STREAM,
    GV_LOWER_STREAM,
    GV_GROTTO_LEDGE,
    GV_CRATE_LEDGE,
    GV_OCTOROK_GROTTO,
    GV_FORTRESS_SIDE,
    GV_CARPENTER_TENT,
    GV_STORMS_GROTTO,
    GERUDO_FORTRESS,
    GF_OUTSIDE_GATE,
    GF_STORMS_GROTTO,
    GERUDO_TRAINING_GROUNDS_ENTRYWAY,

    WASTELAND_NEAR_FORTRESS,
    HAUNTED_WASTELAND,
    WASTELAND_NEAR_COLOSSUS,
    DESERT_COLOSSUS,
    COLOSSUS_GREAT_FAIRY_FOUNTAIN,
    COLOSSUS_GROTTO,
    SPIRIT_TEMPLE_ENTRYWAY,

    MARKET_ENTRANCE,
    THE_MARKET,
    MARKET_GUARD_HOUSE,
    MARKET_BAZAAR,
    MARKET_MASK_SHOP,
    MARKET_SHOOTING_GALLERY,
    MARKET_BOMBCHU_BOWLING,
    MARKET_TREASURE_CHEST_GAME,
    MARKET_POTION_SHOP,
    MARKET_BACK_ALLEY,
    MARKET_BOMBCHU_SHOP,
    MARKET_DOG_LADY_HOUSE,
    MARKET_MAN_IN_GREEN_HOUSE,
    TOT_ENTRANCE,
    TEMPLE_OF_TIME,
    TOT_BEYOND_DOOR_OF_TIME,

    CASTLE_GROUNDS,
    HYRULE_CASTLE_GROUNDS,
    HC_GARDEN,
    HC_GREAT_FAIRY_FOUNTAIN,
    HC_STORMS_GROTTO,
    GANONS_CASTLE_GROUNDS,
    OGC_GREAT_FAIRY_FOUNTAIN,
    GANONS_CASTLE_ENTRYWAY,

    DEKU_TREE_LOBBY,
    DEKU_TREE_2F_MIDDLE_ROOM,
    DEKU_TREE_SLINGSHOT_ROOM,
    DEKU_TREE_COMPASS_ROOM,
    DEKU_TREE_BASEMENT_LOWER,
    DEKU_TREE_BASEMENT_SCRUB_ROOM,
    DEKU_TREE_BASEMENT_WATER_ROOM,
    DEKU_TREE_BASEMENT_TORCH_ROOM,
    DEKU_TREE_BASEMENT_BACK_LOBBY,
    DEKU_TREE_BASEMENT_BACK_ROOM,
    DEKU_TREE_BASEMENT_UPPER,
    DEKU_TREE_OUTSIDE_BOSS_ROOM,
    DEKU_TREE_BOSS_ROOM,

    DODONGOS_CAVERN_BEGINNING,
    DODONGOS_CAVERN_LOBBY,
    DODONGOS_CAVERN_LOBBY_SWITCH,
    DODONGOS_CAVERN_SE_CORRIDOR,
    DODONGOS_CAVERN_SE_ROOM,
    DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS,
    DODONGOS_CAVERN_LOWER_LIZALFOS,
    DODONGOS_CAVERN_DODONGO_ROOM,
    DODONGOS_CAVERN_NEAR_DODONGO_ROOM,
    DODONGOS_CAVERN_STAIRS_LOWER,
    DODONGOS_CAVERN_STAIRS_UPPER,
    DODONGOS_CAVERN_COMPASS_ROOM,
    DODONGOS_CAVERN_ARMOS_ROOM,
    DODONGOS_CAVERN_BOMB_ROOM_LOWER,
    DODONGOS_CAVERN_2F_SIDE_ROOM,
    DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,
    DODONGOS_CAVERN_UPPER_LIZALFOS,
    DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM,
    DODONGOS_CAVERN_BOMB_ROOM_UPPER,
    DODONGOS_CAVERN_FAR_BRIDGE,
    DODONGOS_CAVERN_BOSS_AREA,
    DODONGOS_CAVERN_BACK_ROOM,
    DODONGOS_CAVERN_BOSS_ROOM,

    JABU_JABUS_BELLY_BEGINNING,
    JABU_JABUS_BELLY_LIFT_MIDDLE,
    JABU_JABUS_BELLY_MAIN_UPPER,
    JABU_JABUS_BELLY_MAIN_LOWER,
    JABU_JABUS_BELLY_SHABOMB_CORRIDOR,
    JABU_JABUS_BELLY_LOWER_SIDE_ROOM,
    JABU_JABUS_BELLY_LIFT_LOWER,
    JABU_JABUS_BELLY_FORKED_CORRIDOR,
    JABU_JABUS_BELLY_BOOMERANG_ROOM,
    JABU_JABUS_BELLY_MAP_ROOM,
    JABU_JABUS_BELLY_COMPASS_ROOM,
    JABU_JABUS_BELLY_BLUE_TENTACLE,
    JABU_JABUS_BELLY_GREEN_TENTACLE,
    JABU_JABUS_BELLY_BIGOCTO_ROOM,
    JABU_JABUS_BELLY_ABOVE_BIGOCTO,
    JABU_JABUS_BELLY_LIFT_UPPER,
    JABU_JABUS_BELLY_NEAR_BOSS_ROOM,
    JABU_JABUS_BELLY_BOSS_ROOM,

    FOREST_TEMPLE_FIRST_ROOM,
    FOREST_TEMPLE_SOUTH_CORRIDOR,
    FOREST_TEMPLE_LOBBY,
    FOREST_TEMPLE_NORTH_CORRIDOR,
    FOREST_TEMPLE_LOWER_STALFOS,
    FOREST_TEMPLE_NW_OUTDOORS_LOWER,
    FOREST_TEMPLE_NW_OUTDOORS_UPPER,
    FOREST_TEMPLE_NE_OUTDOORS_LOWER,
    FOREST_TEMPLE_NE_OUTDOORS_UPPER,
    FOREST_TEMPLE_MAP_ROOM,
    FOREST_TEMPLE_SEWER,
    FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST,
    FOREST_TEMPLE_FLOORMASTER_ROOM,
    FOREST_TEMPLE_WEST_CORRIDOR,
    FOREST_TEMPLE_BLOCK_PUSH_ROOM,
    FOREST_TEMPLE_NW_CORRIDOR_TWISTED,
    FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED,
    FOREST_TEMPLE_RED_POE_ROOM,
    FOREST_TEMPLE_UPPER_STALFOS,
    FOREST_TEMPLE_BLUE_POE_ROOM,
    FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED,
    FOREST_TEMPLE_NE_CORRIDOR_TWISTED,
    FOREST_TEMPLE_FROZEN_EYE_ROOM,
    FOREST_TEMPLE_FALLING_ROOM,
    FOREST_TEMPLE_GREEN_POE_ROOM,
    FOREST_TEMPLE_EAST_CORRIDOR,
    FOREST_TEMPLE_BOSS_REGION,
    FOREST_TEMPLE_BOSS_ROOM,

    FIRE_TEMPLE_FIRST_ROOM,
    FIRE_TEMPLE_NEAR_BOSS_ROOM,
    FIRE_TEMPLE_BOSS_ROOM,
    FIRE_TEMPLE_LOOP_ENEMIES,
    FIRE_TEMPLE_LOOP_TILES,
    FIRE_TEMPLE_LOOP_FLARE_DANCER,
    FIRE_TEMPLE_LOOP_HAMMER_SWITCH,
    FIRE_TEMPLE_LOOP_GORON_ROOM,
    FIRE_TEMPLE_LOOP_EXIT,
    FIRE_TEMPLE_BIG_LAVA_ROOM,
    FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON,
    FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES,
    FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON,
    FIRE_TEMPLE_FIRE_PILLAR_ROOM,
    FIRE_TEMPLE_SHORTCUT_ROOM,
    FIRE_TEMPLE_SHORTCUT_CLIMB,
    FIRE_TEMPLE_BOULDER_MAZE_LOWER,
    FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM,
    FIRE_TEMPLE_EAST_CENTRAL_ROOM,
    FIRE_TEMPLE_FIRE_WALL_CHASE,
    FIRE_TEMPLE_MAP_AREA,
    FIRE_TEMPLE_BOULDER_MAZE_UPPER,
    FIRE_TEMPLE_SCARECROW_ROOM,
    FIRE_TEMPLE_EAST_PEAK,
    FIRE_TEMPLE_CORRIDOR,
    FIRE_TEMPLE_FIRE_MAZE_ROOM,
    FIRE_TEMPLE_FIRE_MAZE_UPPER,
    FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM,
    FIRE_TEMPLE_WEST_CENTRAL_LOWER,
    FIRE_TEMPLE_WEST_CENTRAL_UPPER,
    FIRE_TEMPLE_LATE_FIRE_MAZE,
    FIRE_TEMPLE_UPPER_FLARE_DANCER,
    FIRE_TEMPLE_WEST_CLIMB,
    FIRE_TEMPLE_WEST_PEAK,
    FIRE_TEMPLE_HAMMER_RETURN_PATH,
    FIRE_TEMPLE_ABOVE_FIRE_MAZE,

    WATER_TEMPLE_LOBBY,
    WATER_TEMPLE_EAST_LOWER,
    WATER_TEMPLE_MAP_ROOM,
    WATER_TEMPLE_CRACKED_WALL,
    WATER_TEMPLE_TORCH_ROOM,
    WATER_TEMPLE_NORTH_LOWER,
    WATER_TEMPLE_BOULDERS_LOWER,
    WATER_TEMPLE_BLOCK_ROOM,
    WATER_TEMPLE_JETS_ROOM,
    WATER_TEMPLE_BOULDERS_UPPER,
    WATER_TEMPLE_BOSS_KEY_ROOM,
    WATER_TEMPLE_SOUTH_LOWER,
    WATER_TEMPLE_WEST_LOWER,
    WATER_TEMPLE_DRAGON_ROOM,
    WATER_TEMPLE_CENTRAL_PILLAR_LOWER,
    WATER_TEMPLE_CENTRAL_PILLAR_UPPER,
    WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT,
    WATER_TEMPLE_EAST_MIDDLE,
    WATER_TEMPLE_WEST_MIDDLE,
    WATER_TEMPLE_HIGH_WATER,
    WATER_TEMPLE_BLOCK_CORRIDOR,
    WATER_TEMPLE_FALLING_PLATFORM_ROOM,
    WATER_TEMPLE_DRAGON_PILLARS_ROOM,
    WATER_TEMPLE_DARK_LINK_ROOM,
    WATER_TEMPLE_LONGSHOT_ROOM,
    WATER_TEMPLE_RIVER,
    WATER_TEMPLE_PRE_BOSS_ROOM,
    WATER_TEMPLE_BOSS_ROOM,

    SPIRIT_TEMPLE_LOBBY,
    SPIRIT_TEMPLE_CHILD,
    SPIRIT_TEMPLE_CHILD_CLIMB,
    SPIRIT_TEMPLE_EARLY_ADULT,
    SPIRIT_TEMPLE_CENTRAL_CHAMBER,
    SPIRIT_TEMPLE_OUTDOOR_HANDS,
    SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR,
    SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR,
    SHADOW_TEMPLE_BEGINNING,
    SHADOW_TEMPLE_FIRST_BEAMOS,
    SHADOW_TEMPLE_HUGE_PIT,
    SHADOW_TEMPLE_WIND_TUNNEL,
    SHADOW_TEMPLE_BEYOND_BOAT,
    BOTTOM_OF_THE_WELL,
    BOTTOM_OF_THE_WELL_MAIN_AREA,
    ICE_CAVERN_BEGINNING,
    ICE_CAVERN_MAIN,
    GERUDO_TRAINING_GROUNDS_LOBBY,
    GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,
    GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT,
    GERUDO_TRAINING_GROUNDS_LAVA_ROOM,
    GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,
    GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER,
    GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER,
    GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM,
    GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,
    GANONS_CASTLE_LOBBY,
    GANONS_CASTLE_DEKU_SCRUBS,
    GANONS_CASTLE_FOREST_TRIAL,
    GANONS_CASTLE_FIRE_TRIAL,
    GANONS_CASTLE_WATER_TRIAL,
    GANONS_CASTLE_SHADOW_TRIAL,
    GANONS_CASTLE_SPIRIT_TRIAL,
    GANONS_CASTLE_LIGHT_TRIAL,
    GANONS_CASTLE_TOWER,
    DEKU_TREE_MQ_LOBBY,
    DEKU_TREE_MQ_COMPASS_ROOM,
    DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT,
    DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK,
    DEKU_TREE_MQ_BASEMENT_BACK_ROOM,
    DEKU_TREE_MQ_BASEMENT_LEDGE,
    DODONGOS_CAVERN_MQ_BEGINNING,
    DODONGOS_CAVERN_MQ_LOBBY,
    DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,
    DODONGOS_CAVERN_MQ_BOMB_BAG_AREA,
    DODONGOS_CAVERN_MQ_BOSS_AREA,
    JABU_JABUS_BELLY_MQ_BEGINNING,
    JABU_JABUS_BELLY_MQ_MAIN,
    JABU_JABUS_BELLY_MQ_DEPTHS,
    JABU_JABUS_BELLY_MQ_BOSS_AREA,
    FOREST_TEMPLE_MQ_LOBBY,
    FOREST_TEMPLE_MQ_CENTRAL_AREA,
    FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE,
    FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,
    FOREST_TEMPLE_MQ_NW_OUTDOORS,
    FOREST_TEMPLE_MQ_NE_OUTDOORS,
    FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES,
    FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,
    FOREST_TEMPLE_MQ_BOW_REGION,
    FOREST_TEMPLE_MQ_FALLING_ROOM,
    FOREST_TEMPLE_MQ_BOSS_REGION,
    FIRE_TEMPLE_MQ_LOWER,
    FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR,
    FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
    FIRE_TEMPLE_MQ_LOWER_MAZE,
    FIRE_TEMPLE_MQ_UPPER_MAZE,
    FIRE_TEMPLE_MQ_UPPER,
    FIRE_TEMPLE_MQ_BOSS_ROOM,
    WATER_TEMPLE_MQ_LOBBY,
    WATER_TEMPLE_MQ_DIVE,
    WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS,
    WATER_TEMPLE_MQ_DARK_LINK_REGION,
    WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS,
    SPIRIT_TEMPLE_MQ_LOBBY,
    SPIRIT_TEMPLE_MQ_CHILD,
    SPIRIT_TEMPLE_MQ_ADULT,
    SPIRIT_TEMPLE_MQ_SHARED,
    SPIRIT_TEMPLE_MQ_LOWER_ADULT,
    SPIRIT_TEMPLE_MQ_BOSS_AREA,
    SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND,
    SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND,
    SHADOW_TEMPLE_MQ_ENTRYWAY,
    SHADOW_TEMPLE_MQ_BEGINNING,
    SHADOW_TEMPLE_MQ_DEAD_HAND_AREA,
    SHADOW_TEMPLE_MQ_FIRST_BEAMOS,
    SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,
    SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,
    SHADOW_TEMPLE_MQ_WIND_TUNNEL,
    SHADOW_TEMPLE_MQ_BEYOND_BOAT,
    SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,
    BOTTOM_OF_THE_WELL_MQ,
    BOTTOM_OF_THE_WELL_MQ_PERIMETER,
    BOTTOM_OF_THE_WELL_MQ_MIDDLE,
    ICE_CAVERN_MQ_BEGINNING,
    ICE_CAVERN_MQ_MAP_ROOM,
    ICE_CAVERN_MQ_IRON_BOOTS_REGION,
    ICE_CAVERN_MQ_COMPASS_ROOM,
    GERUDO_TRAINING_GROUNDS_MQ_LOBBY,
    GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,
    GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER,
    GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,
    GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM,
    GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS,
    GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT,
    GANONS_CASTLE_MQ_LOBBY,
    GANONS_CASTLE_MQ_DEKU_SCRUBS,
    GANONS_CASTLE_MQ_FOREST_TRIAL,
    GANONS_CASTLE_MQ_FIRE_TRIAL,
    GANONS_CASTLE_MQ_WATER_TRIAL,
    GANONS_CASTLE_MQ_SHADOW_TRIAL,
    GANONS_CASTLE_MQ_SPIRIT_TRIAL,
    GANONS_CASTLE_MQ_LIGHT_TRIAL,
  };

  void AccessReset() {
      for (const uint32_t area : allAreas) {
      AreaTable(area)->ResetVariables();
    }

    if(Settings::HasNightStart) {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childNight = true;
        } else {
          AreaTable(ROOT)->adultNight = true;
        }
      } else {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childDay = true;
        } else {
          AreaTable(ROOT)->adultDay = true;
        }
    }
  }

  //Reset exits and clear items from locations
  void ResetAllLocations() {
      for (const uint32_t area : allAreas) {
      AreaTable(area)->ResetVariables();
      //Erase item from every location in this exit
      for (LocationAccess& locPair : AreaTable(area)->locations) {
          uint32_t location = locPair.GetLocation();
          Location(location)->ResetVariables();
      }
    }

    if(Settings::HasNightStart) {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childNight = true;
        } else {
          AreaTable(ROOT)->adultNight = true;
        }
      } else {
        if(Settings::ResolvedStartingAge == AGE_CHILD) {
          AreaTable(ROOT)->childDay = true;
        } else {
          AreaTable(ROOT)->adultDay = true;
        }
    }
  }

  bool HasTimePassAccess(uint8_t age) {
      for (const uint32_t areaKey : allAreas) {
      auto area = AreaTable(areaKey);
      if (area->timePass && ((age == AGE_CHILD && area->Child()) || (age == AGE_ADULT && area->Adult()))) {
        return true;
      }
    }
    return false;
  }

  // Will dump a file which can be turned into a visual graph using graphviz
  // https://graphviz.org/download/
  // Use command: dot -Tsvg <filename> -o world.svg
  // Then open in a browser and CTRL + F to find the area of interest
  void DumpWorldGraph(std::string str) {
    std::ofstream worldGraph;
    worldGraph.open (str + ".dot");
    worldGraph << "digraph {\n\tcenter=true;\n";

    for (const uint32_t areaKey : allAreas) {
      auto area = AreaTable(areaKey);
      for (auto exit : area->exits) {
        if (exit.GetConnectedRegion()->regionName != "Invalid Area") {
          std::string parent = exit.GetParentRegion()->regionName;
          if (area->childDay) {
            parent += " CD";
          }
          if (area->childNight) {
            parent += " CN";
          }
          if (area->adultDay) {
            parent += " AD";
          }
          if (area->adultNight) {
            parent += " AN";
          }
          Area* connected = exit.GetConnectedRegion();
          auto connectedStr = connected->regionName;
          if (connected->childDay) {
            connectedStr += " CD";
          }
          if (connected->childNight) {
            connectedStr += " CN";
          }
          if (connected->adultDay) {
            connectedStr += " AD";
          }
          if (connected->adultNight) {
            connectedStr += " AN";
          }
          worldGraph << "\t\"" + parent + "\"[shape=\"plain\"];\n";
          worldGraph << "\t\"" + connectedStr + "\"[shape=\"plain\"];\n";
          worldGraph << "\t\"" + parent + "\" -> \"" + connectedStr + "\"\n";
        }
      }
    }
    worldGraph << "}";
    worldGraph.close();
  }

} //namespace Areas

Area* AreaTable(const uint32_t areaKey) {
  if (areaKey > KEY_ENUM_MAX) {
    printf("\x1b[1;1HERROR: AREAKEY TOO BIG");
  }
  return &(areaTable[areaKey]);
}

//Retrieve all the shuffable entrances of a specific type
std::vector<Entrance*> GetShuffleableEntrances(EntranceType type, bool onlyPrimary /*= true*/) {
  std::vector<Entrance*> entrancesToShuffle = {};
    for (uint32_t area : Areas::allAreas) {
    for (auto& exit: AreaTable(area)->exits) {
      if ((exit.GetType() == type || type == EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) && exit.GetType() != EntranceType::None) {
        entrancesToShuffle.push_back(&exit);
      }
    }
  }
  return entrancesToShuffle;
}
