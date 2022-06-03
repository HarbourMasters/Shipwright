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

bool DAY_NIGHT_CYCLE    = true;
bool NO_DAY_NIGHT_CYCLE = false;

//generic grotto event list
static std::vector<EventAccess> grottoEvents = {
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
  else if (placed == BUY_BOMBCHU_5 || placed == BUY_BOMBCHU_10 || placed == BUY_BOMBCHU_20) {
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

static std::array<Area, KEY_ENUM_MAX> areaTable;

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

  areaTable[KOKIRI_FOREST] = Area("Kokiri Forest", "Kokiri Forest", KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy,           {[]{return BeanPlantFairy   || (CanPlantBean(KOKIRI_FOREST) && CanPlay(SongOfStorms));}}),
                  EventAccess(&GossipStoneFairy,         {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(KF_KOKIRI_SWORD_CHEST,   {[]{return IsChild;}}),
                  LocationAccess(KF_GS_KNOW_IT_ALL_HOUSE, {[]{return IsChild && CanChildAttack && AtNight && (HasNightStart || CanLeaveForest || CanPlay(SunsSong)) && CanGetNightTimeGS;}}),
                  LocationAccess(KF_GS_BEAN_PATCH,        {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(KF_GS_HOUSE_OF_TWINS,    {[]{return IsAdult && AtNight && HookshotOrBoomerang && CanGetNightTimeGS;},
                                               /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && (CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                  LocationAccess(KF_GOSSIP_STONE,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KF_LINKS_HOUSE,        {[]{return true;}}),
                  Entrance(KF_MIDOS_HOUSE,        {[]{return true;}}),
                  Entrance(KF_SARIAS_HOUSE,       {[]{return true;}}),
                  Entrance(KF_HOUSE_OF_TWINS,     {[]{return true;}}),
                  Entrance(KF_KNOW_IT_ALL_HOUSE,  {[]{return true;}}),
                  Entrance(KF_KOKIRI_SHOP,        {[]{return true;}}),
                  Entrance(KF_OUTSIDE_DEKU_TREE,  {[]{return IsAdult || OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield;},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(THE_LOST_WOODS,        {[]{return true;}}),
                  Entrance(LW_BRIDGE_FROM_FOREST, {[]{return IsAdult || OpenForest.IsNot(OPENFOREST_CLOSED) || DekuTreeClear;},
                                       /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || GlitchWWTEscape);}}),
                  Entrance(KF_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;},
                                       /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[KF_OUTSIDE_DEKU_TREE] = Area("KF Outside Deku Tree", "Kokiri Forest", KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,           {[]{return DekuBabaSticks || ((IsAdult && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF)) || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,             {[]{return DekuBabaNuts   || ((IsAdult && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF)) || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(KF_DEKU_TREE_GOSSIP_STONE_LEFT,  {[]{return true;}}),
                  LocationAccess(KF_DEKU_TREE_GOSSIP_STONE_RIGHT, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY, {[]{return IsChild || (ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF) && (OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield));},
                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(KOKIRI_FOREST,      {[]{return IsAdult || OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield;},
                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[KF_LINKS_HOUSE] = Area("KF Link's House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_LINKS_HOUSE_COW, {[]{return IsAdult && CanPlay(EponasSong) && LinksCow;},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT))) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && Bombs && IsAdult && EponasSong && LinksCow;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[KF_MIDOS_HOUSE] = Area("KF Mido's House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_MIDOS_TOP_LEFT_CHEST,     {[]{return true;}}),
                  LocationAccess(KF_MIDOS_TOP_RIGHT_CHEST,    {[]{return true;}}),
                  LocationAccess(KF_MIDOS_BOTTOM_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(KF_MIDOS_BOTTOM_RIGHT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_SARIAS_HOUSE] = Area("KF Saria's House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_HOUSE_OF_TWINS] = Area("KF House of Twins", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_KNOW_IT_ALL_HOUSE] = Area("KF Know It All House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_KOKIRI_SHOP] = Area("KF Kokiri Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_STORMS_GROTTO] = Area("KF Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(KF_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(KF_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[LW_FOREST_EXIT] = Area("LW Forest Exit", "Lost Woods", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[THE_LOST_WOODS] = Area("Lost Woods", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&OddMushroomAccess, {[]{return OddMushroomAccess || (IsAdult && (CojiroAccess || Cojiro));}}),
                  EventAccess(&PoachersSawAccess, {[]{return PoachersSawAccess || (IsAdult && OddPoulticeAccess);}}),
                  EventAccess(&GossipStoneFairy,  {[]{return GossipStoneFairy  || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&BeanPlantFairy,    {[]{return BeanPlantFairy    || CanPlay(SongOfStorms);}}),
                  EventAccess(&BugShrub,          {[]{return IsChild && CanCutShrubs;},
                                       /*Glitched*/[]{return IsChild && Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                }, {
                  //Locations
                  LocationAccess(LW_SKULL_KID,                 {[]{return IsChild && CanPlay(SariasSong);},
                                                    /*Glitched*/[]{return IsChild && (Fish || Bugs) && SariasSong && CanShield && (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                  LocationAccess(LW_TRADE_COJIRO,              {[]{return IsAdult && Cojiro;}}),
                  LocationAccess(LW_TRADE_ODD_POULTICE,        {[]{return IsAdult && OddPoultice && Cojiro;}}),
                  LocationAccess(LW_OCARINA_MEMORY_GAME,       {[]{return IsChild && Ocarina;},
                                                    /*Glitched*/[]{return IsChild && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Fish || Bugs) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                  LocationAccess(LW_TARGET_IN_WOODS,           {[]{return IsChild && CanUse(SLINGSHOT);}}),
                  LocationAccess(LW_DEKU_SCRUB_NEAR_BRIDGE,    {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_GS_BEAN_PATCH_NEAR_BRIDGE, {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(LW_GOSSIP_STONE,              {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(LW_FOREST_EXIT,           {[]{return true;}}),
                  Entrance(GC_WOODS_WARP,            {[]{return true;}}),
                  Entrance(LW_BRIDGE,                {[]{return CanLeaveForest && ((IsAdult && CanPlantBean(THE_LOST_WOODS)) || CanUse(HOVER_BOOTS) || CanUse(LONGSHOT));},
                                          /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && HasBombchus) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(ZORAS_RIVER,              {[]{return CanLeaveForest && (CanDive || CanUse(IRON_BOOTS));},
                                          /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::ADVANCED) ||
                                                                ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                                ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))));}}),
                  Entrance(LW_BEYOND_MIDO,           {[]{return IsChild || CanPlay(SariasSong);},
                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) ||
                                                                CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::ADVANCED) && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE) || HoverBoots)) ||
                                                                ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SariasSong);}}),
                  Entrance(LW_NEAR_SHORTCUTS_GROTTO, {[]{return Here(THE_LOST_WOODS, []{return CanBlastOrSmash;});},
                                          /*Glitched*/[]{return Here(THE_LOST_WOODS, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
  });

  areaTable[LW_BEYOND_MIDO] = Area("LW Beyond Mido", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || CanUse(STICKS);}}),
                }, {
                  //Locations
                  LocationAccess(LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_GS_ABOVE_THEATER,                   {[]{return IsAdult && AtNight && (CanPlantBean(LW_BEYOND_MIDO) || (LogicLostWoodsGSBean && CanUse(HOOKSHOT) && (CanUse(LONGSHOT) || CanUse(BOW) || CanUse(SLINGSHOT) || HasBombchus || CanUse(DINS_FIRE)))) && CanGetNightTimeGS;},
                                                             /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(LW_GS_BEAN_PATCH_NEAR_THEATER,         {[]{return CanPlantBugs && (CanChildAttack || (Scrubsanity.Is(SCRUBSANITY_OFF) && DekuShield));}}),
                }, {
                  //Exits
                  Entrance(LW_FOREST_EXIT,   {[]{return true;}}),
                  Entrance(THE_LOST_WOODS,   {[]{return IsChild || CanPlay(SariasSong);},
                                  /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                        ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SariasSong);}}),
                  Entrance(SFM_ENTRYWAY,     {[]{return true;}}),
                  Entrance(DEKU_THEATER,     {[]{return true;}}),
                  Entrance(LW_SCRUBS_GROTTO, {[]{return Here(LW_BEYOND_MIDO, []{return CanBlastOrSmash;});},
                                  /*Glitched*/[]{return Here(LW_BEYOND_MIDO, []{return IsChild && CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
  });

  areaTable[LW_NEAR_SHORTCUTS_GROTTO] = Area("LW Near Shortcuts Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(LW_NEAR_SHORTCUTS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[DEKU_THEATER] = Area("Deku Theater", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_THEATER_SKULL_MASK,    {[]{return IsChild && SkullMask;}}),
                  LocationAccess(DEKU_THEATER_MASK_OF_TRUTH, {[]{return IsChild && MaskOfTruth;}}),
                }, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[LW_SCRUBS_GROTTO] = Area("LW Scrubs Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LW_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(LW_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[SFM_ENTRYWAY] = Area("SFM Entryway", "Sacred Forest Meadow", SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO,       {[]{return true;}}),
                  Entrance(SACRED_FOREST_MEADOW, {[]{return IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  Entrance(SFM_WOLFOS_GROTTO,    {[]{return CanOpenBombGrotto;}}),
  });

  areaTable[SACRED_FOREST_MEADOW] = Area("Sacred Forest Meadow", "Sacred Forest Meadow", SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(SONG_FROM_SARIA,             {[]{return IsChild && ZeldasLetter;}}),
                  LocationAccess(SHEIK_IN_FOREST,             {[]{return IsAdult;}}),
                  LocationAccess(SFM_GS,                      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(SFM_MAZE_GOSSIP_STONE_LOWER, {[]{return true;}}),
                  LocationAccess(SFM_MAZE_GOSSIP_STONE_UPPER, {[]{return true;}}),
                  LocationAccess(SFM_SARIA_GOSSIP_STONE,      {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(SFM_ENTRYWAY,           {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_ENTRYWAY, {[]{return CanUse(HOOKSHOT);},
                                        /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(SFM_FAIRY_GROTTO,       {[]{return true;}}),
                  Entrance(SFM_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;},
                                        /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[SFM_FAIRY_GROTTO] = Area("SFM Fairy Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[SFM_WOLFOS_GROTTO] = Area("SFM Wolfos Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SFM_WOLFOS_GROTTO_CHEST, {[]{return IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(SFM_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[SFM_STORMS_GROTTO] = Area("SFM Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SFM_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(SFM_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[LW_BRIDGE_FROM_FOREST] = Area("LW Bridge From Forest", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LW_GIFT_FROM_SARIA, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(LW_BRIDGE, {[]{return true;}}),
  });

  areaTable[LW_BRIDGE] = Area("LW Bridge", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST,  {[]{return true;}}),
                  Entrance(HYRULE_FIELD,   {[]{return true;}}),
                  Entrance(THE_LOST_WOODS, {[]{return CanUse(LONGSHOT);},
                                /*Glitched*/[]{return (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[HYRULE_FIELD] = Area("Hyrule Field", "Hyrule Field", HYRULE_FIELD, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BigPoeKill, {[]{return CanUse(BOW) && CanRideEpona && HasBottle;}}),
                }, {
                  //Locations
                  LocationAccess(HF_OCARINA_OF_TIME_ITEM,   {[]{return IsChild && HasAllStones;}}),
                  LocationAccess(SONG_FROM_OCARINA_OF_TIME, {[]{return IsChild && HasAllStones;}}),
                }, {
                  //Exits
                  Entrance(LW_BRIDGE,              {[]{return true;}}),
                  Entrance(LAKE_HYLIA,             {[]{return true;}}),
                  Entrance(GERUDO_VALLEY,          {[]{return true;}}),
                  Entrance(MARKET_ENTRANCE,        {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,       {[]{return true;}}),
                  Entrance(ZR_FRONT,               {[]{return true;}}),
                  Entrance(LON_LON_RANCH,          {[]{return true;}}),
                  Entrance(HF_SOUTHEAST_GROTTO,    {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(HF_OPEN_GROTTO,         {[]{return true;}}),
                  Entrance(HF_INSIDE_FENCE_GROTTO, {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HF_COW_GROTTO,          {[]{return (CanUse(MEGATON_HAMMER) || IsChild) && CanOpenBombGrotto;},
                                        /*Glitched*/[]{return (CanUse(STICKS) && (ShardOfAgony || LogicGrottosWithoutAgony) && (IsChild || (CanOpenBombGrotto && (CanTakeDamageTwice || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE));}}),
                  Entrance(HF_NEAR_MARKET_GROTTO,  {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(HF_FAIRY_GROTTO,        {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && ((IsChild && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT)) || (IsAdult && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)));});}}),
                  Entrance(HF_NEAR_KAK_GROTTO,     {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HF_TEKTITE_GROTTO,      {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[HF_SOUTHEAST_GROTTO] = Area("HF Southeast Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_SOUTHEAST_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_SOUTHEAST_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_OPEN_GROTTO] = Area("HF Open Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_INSIDE_FENCE_GROTTO] = Area("HF Inside Fence Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_DEKU_SCRUB_GROTTO, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_COW_GROTTO] = Area("HF Cow Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_GS_COW_GROTTO,           {[]{return HasFireSource && HookshotOrBoomerang;},
                                                  /*Glitched*/[]{return (CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) && HookshotOrBoomerang;}}),
                  LocationAccess(HF_COW_GROTTO_COW,          {[]{return HasFireSource && CanPlay(EponasSong);},
                                                  /*Glitched*/[]{return ((CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) || HasFireSource) &&
                                                                        (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) || Ocarina) && EponasSong;}}),
                  LocationAccess(HF_COW_GROTTO_GOSSIP_STONE, {[]{return HasFireSource;},
                                                  /*Glitched*/[]{return CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_NEAR_MARKET_GROTTO] = Area("HF Near Market Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_NEAR_MARKET_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_NEAR_MARKET_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_FAIRY_GROTTO] = Area("HF Fairy Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_NEAR_KAK_GROTTO] = Area("HF Near Kak Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_GS_NEAR_KAK_GROTTO, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_TEKTITE_GROTTO] = Area("HF Tektite Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_TEKTITE_GROTTO_FREESTANDING_POH, {[]{return ProgressiveScale >= 2 || CanUse(IRON_BOOTS);}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[LAKE_HYLIA] = Area("Lake Hylia", "Lake Hylia", LAKE_HYLIA, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(LAKE_HYLIA) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || (IsChild && CanCutShrubs);}}),
                  EventAccess(&ChildScarecrow,   {[]{return ChildScarecrow   || (IsChild && Ocarina);},
                                      /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE))) && IsChild;}}),
                  EventAccess(&AdultScarecrow,   {[]{return AdultScarecrow   || (IsAdult && Ocarina);},
                                      /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE))) && IsAdult;}}),
                }, {
                  //Locations
                  LocationAccess(LH_UNDERWATER_ITEM,        {[]{return IsChild && CanDive;}}),
                  LocationAccess(LH_SUN,                    {[]{return IsAdult && WaterTempleClear && CanUse(BOW);}}),
                  LocationAccess(LH_FREESTANDING_POH,       {[]{return IsAdult && (CanUse(SCARECROW) || CanPlantBean(LAKE_HYLIA));},
                                                 /*Glitched*/[]{return (IsAdult && CanUse(HOOKSHOT) && ScarecrowSong && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(LH_GS_BEAN_PATCH,          {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(LH_GS_LAB_WALL,            {[]{return IsChild && (HookshotOrBoomerang || (LogicLabWallGS && (Sticks || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)))) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_GS_SMALL_ISLAND,        {[]{return IsChild && CanChildAttack && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_GS_TREE,                {[]{return IsAdult && CanUse(LONGSHOT) && AtNight && CanGetNightTimeGS;},
                                                 /*Glitched*/[]{return (CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE)) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_LAB_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(LH_GOSSIP_STONE_SOUTHEAST, {[]{return true;}}),
                  LocationAccess(LH_GOSSIP_STONE_SOUTHWEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,          {[]{return true;}}),
                  Entrance(ZORAS_DOMAIN,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));},
                                       /*Glitched*/[]{return IsChild && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::INTERMEDIATE) ||
                                                             (Bugs && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                             ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))));}}),
                  Entrance(LH_OWL_FLIGHT,         {[]{return IsChild;}}),
                  Entrance(LH_FISHING_ISLAND,     {[]{return IsChild || CanUse(SCARECROW) || CanPlantBean(LAKE_HYLIA) || WaterTempleClear;},
                                       /*Glitched*/[]{return (CanUse(HOOKSHOT) && ScarecrowSong && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(LH_LAB,                {[]{return true;}}),
                  Entrance(WATER_TEMPLE_ENTRYWAY, {[]{return CanUse(HOOKSHOT) && (CanUse(IRON_BOOTS) || (IsAdult && CanUse(LONGSHOT) && ProgressiveScale >= 2));},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::ADVANCED);}}),
                  Entrance(LH_GROTTO,             {[]{return true;}}),
  });

  areaTable[LH_FISHING_ISLAND] = Area("LH Fishing Island", "Lake Hylia", LAKE_HYLIA, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LAKE_HYLIA,      {[]{return true;}}),
                  Entrance(LH_FISHING_HOLE, {[]{return true;}}),
  });

  areaTable[LH_OWL_FLIGHT] = Area("LH Owl Flight", "Lake Hylia", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[LH_LAB] = Area("LH Lab", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&EyedropsAccess, {[]{return EyedropsAccess || (IsAdult && (EyeballFrogAccess || (EyeballFrog && DisableTradeRevert)));}}),
                }, {
                  //Locations
                  LocationAccess(LH_LAB_DIVE,     {[]{return ProgressiveScale >= 2 || (LogicLabDiving && CanUse(IRON_BOOTS) && CanUse(HOOKSHOT));},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                             (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) && CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE));}}),
                  LocationAccess(LH_TRADE_FROG,   {[]{return IsAdult && EyeballFrog;}}),
                  LocationAccess(LH_GS_LAB_CRATE, {[]{return CanUse(IRON_BOOTS) && CanUse(HOOKSHOT);},
                                       /*Glitched*/[]{return (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && HasBombchus && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)) &&
                                                             (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                             (CanUse(FARORES_WIND) && CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || ProgressiveScale >= 2 || CanUse(IRON_BOOTS))) ||
                                                             (CanUse(IRON_BOOTS) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE));}}),
                }, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[LH_FISHING_HOLE] = Area("LH Fishing Hole", "", NONE, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LH_CHILD_FISHING, {[]{return IsChild;}}),
                  LocationAccess(LH_ADULT_FISHING, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(LH_FISHING_ISLAND, {[]{return true;}}),
  });

  areaTable[LH_GROTTO] = Area("LH Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[GERUDO_VALLEY] = Area("Gerudo Valley", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BugRock, {[]{return BugRock || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(GV_GS_SMALL_BRIDGE, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                          /*Glitched*/[]{return IsChild && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,      {[]{return true;}}),
                  Entrance(GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(GV_CRATE_LEDGE,    {[]{return IsChild || CanUse(LONGSHOT);}}),
                  Entrance(GV_GROTTO_LEDGE,   {[]{return true;}}),
                  Entrance(GV_FORTRESS_SIDE,  {[]{return (IsAdult && (CanRideEpona || CanUse(LONGSHOT) || GerudoFortress.Is(GERUDOFORTRESS_OPEN) || CarpenterRescue)) || (IsChild && CanUse(HOOKSHOT));},
                                   /*Glitched*/[]{return (HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE)) || (IsChild && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::HERO)) || (IsAdult && (CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED) ||
                                                         (CanUse(HOVER_BOOTS) && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE) || (CanSurviveDamage && (Bombs || HasBombchus) && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::NOVICE)) || (Bombs && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED))))));}}),
  });

  areaTable[GV_UPPER_STREAM] = Area("GV Upper Stream", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(GV_UPPER_STREAM) && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(GV_WATERFALL_FREESTANDING_POH, {[]{return true;}}),
                  LocationAccess(GV_GS_BEAN_PATCH,              {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(GV_COW,                        {[]{return IsChild && CanPlay(EponasSong);},
                                                     /*Glitched*/[]{return IsChild && EponasSong && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                  LocationAccess(GV_GOSSIP_STONE,               {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[GV_LOWER_STREAM] = Area("GV Lower Stream", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[GV_GROTTO_LEDGE] = Area("GV Grotto Ledge", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_LOWER_STREAM,   {[]{return true;}}),
                  Entrance(GV_OCTOROK_GROTTO, {[]{return CanUse(SILVER_GAUNTLETS);}}),
                  Entrance(GV_CRATE_LEDGE,    {[]{return CanUse(LONGSHOT);}}),
  });

  areaTable[GV_CRATE_LEDGE] = Area("GV Crate Ledge", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GV_CRATE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[GV_FORTRESS_SIDE] = Area("GV Fortress Side", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BrokenSwordAccess, {[]{return IsAdult && (PoachersSawAccess || PoachersSaw);}}),
                }, {
                  //Locations
                  LocationAccess(GV_CHEST,          {[]{return IsAdult && (CanUse(MEGATON_HAMMER) || LogicGVHammerChest);},
                                         /*Glitched*/[]{return IsAdult && (CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE) && (CanTakeDamageTwice || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED))));}}),
                  LocationAccess(GV_TRADE_SAW,      {[]{return IsAdult && PoachersSaw;}}),
                  LocationAccess(GV_GS_BEHIND_TENT, {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                         /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && IsAdult && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(GV_GS_PILLAR,      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_FORTRESS,   {[]{return true;}}),
                  Entrance(GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(GERUDO_VALLEY,     {[]{return IsChild || CanRideEpona || CanUse(LONGSHOT) || GerudoFortress.Is(GERUDOFORTRESS_OPEN) || CarpenterRescue;},
                                   /*Glitched*/[]{return (HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED) ||
                                                         (CanUse(HOVER_BOOTS) && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE) || (CanSurviveDamage && (Bombs || HasBombchus) && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::NOVICE)) || (Bombs && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(GV_CARPENTER_TENT, {[]{return IsAdult;},
                                   /*Glitched*/[]{return GlitchGVTentAsChild.Value<bool>();}}),
                  Entrance(GV_STORMS_GROTTO,  {[]{return IsAdult && CanOpenStormGrotto;},
                                   /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[GV_CARPENTER_TENT] = Area("GV Carpenter Tent", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[GV_OCTOROK_GROTTO] = Area("GV Octorok Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_GROTTO_LEDGE, {[]{return true;}}),
  });

  areaTable[GV_STORMS_GROTTO] = Area("GV Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GV_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(GV_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[GERUDO_FORTRESS] = Area("Gerudo Fortress", "Gerudo Fortress", GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CarpenterRescue, {[]{return CanFinishGerudoFortress;}}),
                  EventAccess(&GF_GateOpen,     {[]{return IsAdult && GerudoToken;}}),
                  EventAccess(&GtG_GateOpen,    {[]{return GtG_GateOpen || (IsAdult && GerudoToken);}}),
                }, {
                  //Locations
                  LocationAccess(GF_CHEST,              {[]{return CanUse(HOVER_BOOTS) || (IsAdult && CanUse(SCARECROW)) || CanUse(LONGSHOT);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE))) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen);}}),
                  LocationAccess(GF_HBA_1000_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(GF_HBA_1500_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(GF_NORTH_F1_CARPENTER, {[]{return  IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GF_NORTH_F2_CARPENTER, {[]{return (IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen);}}),
                  LocationAccess(GF_SOUTH_F1_CARPENTER, {[]{return  IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GF_SOUTH_F2_CARPENTER, {[]{return  IsAdult || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GF_GERUDO_TOKEN,       {[]{return CanFinishGerudoFortress;}}),
                  LocationAccess(GF_GS_ARCHERY_RANGE,   {[]{return IsAdult && HookshotOrBoomerang && GerudoToken && AtNight && CanGetNightTimeGS;},
                                             /*Glitched*/[]{return IsAdult && HookshotOrBoomerang && GlitchGFGuardSneak && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(GF_GS_TOP_FLOOR,       {[]{return IsAdult && AtNight && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen) && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE,                 {[]{return true;}}),
                  Entrance(GF_OUTSIDE_GATE,                  {[]{return GF_GateOpen;},
                                                  /*Glitched*/[]{return (IsChild && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::ADVANCED)) || ((IsChild || ((CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(BOOMERANG)) && GlitchGFGuardSneak)) && ((CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::NOVICE)) ||
                                                                        CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::INTERMEDIATE)) && (CanUse(STICKS) || (BiggoronSword && IsAdult) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)));}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY, {[]{return GtG_GateOpen && (IsAdult || ShuffleDungeonEntrances);},
                                                  /*Glitched*/[]{return (CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE) && (HoverBoots || CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::ADVANCED))) || ((IsChild || ((CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(BOOMERANG)) && GlitchGFGuardSneak)) && ((CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) &&
                                                                        CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::INTERMEDIATE)) && (CanUse(STICKS) || (BiggoronSword && IsAdult) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)));}}),
                  Entrance(GF_STORMS_GROTTO,                 {[]{return IsAdult && CanOpenStormGrotto;},
                                                  /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[GF_OUTSIDE_GATE] = Area("GF Outside Gate", "Gerudo Fortress", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GF_GateOpen, {[]{return IsAdult && GerudoToken && (ShuffleGerudoToken || ShuffleOverworldEntrances /*|| ShuffleSpecialIndoorEntrances*/);}}),
                }, {}, {
                  //Exits
                  Entrance(GERUDO_FORTRESS,         {[]{return (IsAdult && (Hookshot || !ShuffleOverworldEntrances)) || GF_GateOpen;}}),
                  Entrance(WASTELAND_NEAR_FORTRESS, {[]{return true;}}),
  });

  areaTable[GF_STORMS_GROTTO] = Area("GF Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(GERUDO_FORTRESS, {[]{return true;}}),
  });

  areaTable[WASTELAND_NEAR_FORTRESS] = Area("Wasteland Near Fortress", "Haunted Wasteland", HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GF_OUTSIDE_GATE,   {[]{return true;}}),
                  Entrance(HAUNTED_WASTELAND, {[]{return CanUse(HOVER_BOOTS) || CanUse(LONGSHOT);},
                                   /*Glitched*/[]{return ((Bombs || HasBombchus) && CanSurviveDamage && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::NOVICE)) || (CanUse(MEGATON_HAMMER) && CanShield && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::INTERMEDIATE)) ||
                                                         (Bombs && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) || GlitchItemlessWasteland;}}),
  });

  areaTable[HAUNTED_WASTELAND] = Area("Haunted Wasteland", "Haunted Wasteland", HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(WASTELAND_CHEST,            {[]{return HasFireSource;},
                                                  /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(WASTELAND_BOMBCHU_SALESMAN, {[]{return AdultsWallet && (IsAdult || Sticks || KokiriSword);},
                                                  /*Glitched*/[]{return AdultsWallet && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(WASTELAND_GS,               {[]{return HookshotOrBoomerang;},
                                                  /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(WASTELAND_NEAR_COLOSSUS, {[]{return LogicLensWasteland || CanUse(LENS_OF_TRUTH);}}),
                  Entrance(WASTELAND_NEAR_FORTRESS, {[]{return CanUse(HOVER_BOOTS) || CanUse(LONGSHOT);},
                                         /*Glitched*/[]{return ((Bombs || HasBombchus) && CanSurviveDamage && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::NOVICE)) || (CanUse(MEGATON_HAMMER) && CanShield && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::INTERMEDIATE)) ||
                                                               (Bombs && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED)) || (HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) || GlitchItemlessWasteland;}}),
  });

  areaTable[WASTELAND_NEAR_COLOSSUS] = Area("Wasteland Near Colossus", "Haunted Wasteland", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DESERT_COLOSSUS,   {[]{return true;}}),
                  Entrance(HAUNTED_WASTELAND, {[]{return LogicReverseWasteland || false;}}),
  });

  areaTable[DESERT_COLOSSUS] = Area("Desert Colossus", "Desert Colossus", DESERT_COLOSSUS, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPond, {[]{return FairyPond || CanPlay(SongOfStorms);},
                               /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms;}}),
                  EventAccess(&BugRock,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(COLOSSUS_FREESTANDING_POH, {[]{return IsAdult && CanPlantBean(DESERT_COLOSSUS);},
                                                 /*Glitched*/[]{return (HoverBoots && CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED)) || (((IsChild && (ChildCanAccess(SPIRIT_TEMPLE_OUTDOOR_HANDS) || ChildCanAccess(SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND) || ChildCanAccess(SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND))) ||
                                                                       (IsAdult && (AdultCanAccess(SPIRIT_TEMPLE_OUTDOOR_HANDS) || AdultCanAccess(SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND) || AdultCanAccess(SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND)))) && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::ADVANCED) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE)));}}),
                  LocationAccess(SHEIK_AT_COLOSSUS,         {[]{return true;}}),
                  LocationAccess(COLOSSUS_GS_BEAN_PATCH,    {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(COLOSSUS_GS_TREE,          {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(COLOSSUS_GS_HILL,          {[]{return IsAdult && AtNight && (CanPlantBean(DESERT_COLOSSUS) || CanUse(LONGSHOT) || (LogicColossusGS && CanUse(HOOKSHOT))) && CanGetNightTimeGS;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::EXPERT) && CanShield && IsAdult && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(COLOSSUS_GOSSIP_STONE,     {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(COLOSSUS_GREAT_FAIRY_FOUNTAIN, {[]{return HasExplosives;}}),
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(WASTELAND_NEAR_COLOSSUS,       {[]{return true;}}),
                  Entrance(COLOSSUS_GROTTO,               {[]{return CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[COLOSSUS_GREAT_FAIRY_FOUNTAIN] = Area("Colossus Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(COLOSSUS_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                                   /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[COLOSSUS_GROTTO] = Area("Colossus Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(COLOSSUS_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[MARKET_ENTRANCE] = Area("Market Entrance", "Market Entrance", THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD,       {[]{return IsAdult || AtDay;}}),
                  Entrance(THE_MARKET,         {[]{return true;}}),
                  Entrance(MARKET_GUARD_HOUSE, {[]{return true;}}),
  });

  areaTable[THE_MARKET] = Area("Market", "Market", THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(MARKET_ENTRANCE,            {[]{return true;}}),
                  Entrance(TOT_ENTRANCE,               {[]{return true;}}),
                  Entrance(CASTLE_GROUNDS,             {[]{return true;}}),
                  Entrance(MARKET_BAZAAR,              {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_MASK_SHOP,           {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_SHOOTING_GALLERY,    {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_BOMBCHU_BOWLING,     {[]{return IsChild;}}),
                  Entrance(MARKET_TREASURE_CHEST_GAME, {[]{return IsChild && AtNight;}}),
                  Entrance(MARKET_POTION_SHOP,         {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_BACK_ALLEY,          {[]{return IsChild;}}),
  });

  areaTable[MARKET_BACK_ALLEY] = Area("Market Back Alley", "Market", THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_MARKET,                 {[]{return true;}}),
                  Entrance(MARKET_BOMBCHU_SHOP,        {[]{return AtNight;}}),
                  Entrance(MARKET_DOG_LADY_HOUSE,      {[]{return true;}}),
                  Entrance(MARKET_MAN_IN_GREEN_HOUSE,  {[]{return AtNight;}}),
  });

  areaTable[TOT_ENTRANCE] = Area("ToT Entrance", "ToT Entrance", THE_MARKET, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(TOT_GOSSIP_STONE_LEFT,         {[]{return true;}}),
                  LocationAccess(TOT_GOSSIP_STONE_LEFT_CENTER,  {[]{return true;}}),
                  LocationAccess(TOT_GOSSIP_STONE_RIGHT_CENTER, {[]{return true;}}),
                  LocationAccess(TOT_GOSSIP_STONE_RIGHT,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET,  {[]{return true;}}),
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[TEMPLE_OF_TIME] = Area("Temple of Time", "", TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(TOT_LIGHT_ARROWS_CUTSCENE, {[]{return IsAdult && CanTriggerLACS;}}),
                }, {
                  //Exits
                  Entrance(TOT_ENTRANCE,            {[]{return true;}}),
                  Entrance(TOT_BEYOND_DOOR_OF_TIME, {[]{return OpenDoorOfTime.Is(OPENDOOROFTIME_OPEN) || (CanPlay(SongOfTime) && (OpenDoorOfTime.Is(OPENDOOROFTIME_CLOSED) || (HasAllStones && OcarinaOfTime)));},
                                         /*Glitched*/[]{return SongOfTime && OpenDoorOfTime.Is(OPENDOOROFTIME_CLOSED) && (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::ADVANCED) ||
                                                               ((Bugs || Fish) && Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE)));}}),
  });

  areaTable[TOT_BEYOND_DOOR_OF_TIME] = Area("Beyond Door of Time", "", TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&TimeTravel, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(SHEIK_AT_TEMPLE, {[]{return ForestMedallion && IsAdult;}}),
                }, {
                  //Exits
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[CASTLE_GROUNDS] = Area("Castle Grounds", "Castle Grounds", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_MARKET,            {[]{return true;}}),
                  Entrance(HYRULE_CASTLE_GROUNDS, {[]{return IsChild;}}),
                  Entrance(GANONS_CASTLE_GROUNDS, {[]{return IsAdult;}}),
  });

  areaTable[HYRULE_CASTLE_GROUNDS] = Area("Hyrule Castle Grounds", "Castle Grounds", HYRULE_CASTLE, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugRock,          {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(HC_MALON_EGG,              {[]{return true;}}),
                  LocationAccess(HC_GS_TREE,                {[]{return CanChildAttack;}}),
                  LocationAccess(HC_MALON_GOSSIP_STONE,     {[]{return true;}}),
                  LocationAccess(HC_ROCK_WALL_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS,          {[]{return true;}}),
                  Entrance(HC_GARDEN,               {[]{return WeirdEgg || !ShuffleWeirdEgg;}}),
                  Entrance(HC_GREAT_FAIRY_FOUNTAIN, {[]{return CanBlastOrSmash;},
                                         /*Glitched*/[]{return Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HC_STORMS_GROTTO,        {[]{return CanOpenStormGrotto;},
                                         /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[HC_GARDEN] = Area("HC Garden", "Castle Grounds", HYRULE_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                }, {
                  //Locations
                  LocationAccess(HC_ZELDAS_LETTER, {[]{return true;}}),
                  LocationAccess(SONG_FROM_IMPA,   {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[HC_GREAT_FAIRY_FOUNTAIN] = Area("HC Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[HC_STORMS_GROTTO] = Area("HC Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,           {[]{return NutPot           || CanBlastOrSmash;}}),
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (CanBlastOrSmash && CanSummonGossipFairy);}}),
                  EventAccess(&WanderingBugs,    {[]{return WanderingBugs    || CanBlastOrSmash;}}),
                }, {
                  //Locations
                  LocationAccess(HC_GS_STORMS_GROTTO,           {[]{return CanBlastOrSmash && HookshotOrBoomerang;},
                                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(HC_STORMS_GROTTO_GOSSIP_STONE, {[]{return CanBlastOrSmash;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[GANONS_CASTLE_GROUNDS] = Area("Ganon's Castle Grounds", "Castle Grounds", OUTSIDE_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations                                   //the terrain was lowered such that you can't get this GS with a simple sword slash
                  LocationAccess(OGC_GS, {[]{return HasExplosives || (IsAdult && (LogicOutsideGanonsGS || Bow || HookshotOrBoomerang || CanUse(DINS_FIRE)));}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS,           {[]{return AtNight;}}),
                  Entrance(OGC_GREAT_FAIRY_FOUNTAIN, {[]{return CanUse(GOLDEN_GAUNTLETS) && AtNight;}}),
                  Entrance(GANONS_CASTLE_ENTRYWAY,   {[]{return CanBuildRainbowBridge;},
                                          /*Glitched*/[]{return (HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::ADVANCED) || (HoverBoots && CanShield && Bombs && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::EXPERT)) || (HoverBoots && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::ADVANCED));}}),
  });

  areaTable[OGC_GREAT_FAIRY_FOUNTAIN] = Area("OGC Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(OGC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                              /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[MARKET_GUARD_HOUSE] = Area("Market Guard House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_10_BIG_POES,    {[]{return IsAdult && BigPoeKill;}}),
                  LocationAccess(MARKET_GS_GUARD_HOUSE, {[]{return IsChild;}}),
                }, {
                  //Exits
                  Entrance(MARKET_ENTRANCE, {[]{return true;}}),
  });

  areaTable[MARKET_BAZAAR] = Area("Market Bazaar", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_BAZAAR_ITEM_1, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_2, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_3, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_4, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_5, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_6, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_7, {[]{return true;}}),
                  LocationAccess(MARKET_BAZAAR_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_MASK_SHOP] = Area("Market Mask Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SkullMask,   {[]{return SkullMask   || (ZeldasLetter && (CompleteMaskQuest ||  ChildCanAccess(KAKARIKO_VILLAGE)));}}),
                  EventAccess(&MaskOfTruth, {[]{return MaskOfTruth || (SkullMask && (CompleteMaskQuest || (ChildCanAccess(THE_LOST_WOODS) && CanPlay(SariasSong) && AreaTable(THE_GRAVEYARD)->childDay && ChildCanAccess(HYRULE_FIELD) && HasAllStones)));}}),
                }, {}, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_SHOOTING_GALLERY] = Area("Market Shooting Gallery", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_SHOOTING_GALLERY_REWARD, {[]{return IsChild;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_BOMBCHU_BOWLING] = Area("Market Bombchu Bowling", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,  {[]{return CanPlayBowling;}}),
                  LocationAccess(MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, {[]{return CanPlayBowling;}}),
                  LocationAccess(MARKET_BOMBCHU_BOWLING_BOMBCHUS,     {[]{return CanPlayBowling;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_POTION_SHOP] = Area("Market Potion Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_POTION_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(MARKET_POTION_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_TREASURE_CHEST_GAME] = Area("Market Treasure Chest Game", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_REWARD, {[]{return (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 6)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1));},
                                                         /*Glitched*/[]{return !ShuffleChestMinigame && (CanUse(FARORES_WIND) && (HasBottle || WeirdEgg) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE));}}),
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_ITEM_1, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_ITEM_2, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 2)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_ITEM_3, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 3)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_ITEM_4, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 4)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(MARKET_TREASURE_CHEST_GAME_ITEM_5, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 5)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_BOMBCHU_SHOP] = Area("Market Bombchu Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(MARKET_BOMBCHU_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[MARKET_DOG_LADY_HOUSE] = Area("Market Dog Lady House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(MARKET_LOST_DOG, {[]{return IsChild && AtNight;}}),
                }, {
                  //Exits
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[MARKET_MAN_IN_GREEN_HOUSE] = Area("Market Man in Green House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[KAKARIKO_VILLAGE] = Area("Kakariko Village", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CojiroAccess,            {[]{return CojiroAccess || (IsAdult && WakeUpAdultTalon);}}),
                  EventAccess(&BugRock,                 {[]{return true;}}),
                  EventAccess(&KakarikoVillageGateOpen, {[]{return KakarikoVillageGateOpen || (IsChild && (ZeldasLetter || OpenKakariko.Is(OPENKAKARIKO_OPEN)));}}),
                }, {
                  //Locations
                  LocationAccess(SHEIK_IN_KAKARIKO,               {[]{return IsAdult && ForestMedallion && FireMedallion && WaterMedallion;}}),
                  LocationAccess(KAK_ANJU_AS_CHILD,               {[]{return IsChild && AtDay;}}),
                  LocationAccess(KAK_ANJU_AS_ADULT,               {[]{return IsAdult && AtDay;}}),
                  LocationAccess(KAK_TRADE_POCKET_CUCCO,          {[]{return IsAdult && AtDay && PocketEgg && WakeUpAdultTalon;}}),
                  LocationAccess(KAK_GS_HOUSE_UNDER_CONSTRUCTION, {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_SKULLTULA_HOUSE,          {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_GUARDS_HOUSE,             {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_TREE,                     {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_WATCHTOWER,               {[]{return IsChild && (Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT)) && AtNight && CanGetNightTimeGS;},
                                                       /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && (CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE) || (Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE)));}}),
                  LocationAccess(KAK_GS_ABOVE_IMPAS_HOUSE,        {[]{return IsAdult && CanUse(HOOKSHOT) && AtNight && CanGetNightTimeGS;},
                                                       /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && ((HoverBoots && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,                {[]{return true;}}),
                  Entrance(KAK_CARPENTER_BOSS_HOUSE,    {[]{return true;}}),
                  Entrance(KAK_HOUSE_OF_SKULLTULA,      {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE,             {[]{return true;}}),
                  Entrance(KAK_WINDMILL,                {[]{return true;}}),
                  Entrance(KAK_BAZAAR,                  {[]{return IsAdult && AtDay;},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE);}}),
                  Entrance(KAK_SHOOTING_GALLERY,        {[]{return IsAdult && AtDay;}}),
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return DrainWell && (IsChild || ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF));},
                                             /*Glitched*/[]{return (IsChild && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || (AtNight && (CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                                   (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || WeirdEgg))) && ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) ||
                                                                   (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))))))) || (IsAdult && CanUse(LONGSHOT) && AtDay && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE));}}),
                  Entrance(KAK_POTION_SHOP_FRONT,       {[]{return AtDay || IsChild;}}),
                  Entrance(KAK_REDEAD_GROTTO,           {[]{return CanOpenBombGrotto;}}),
                  Entrance(KAK_IMPAS_LEDGE,             {[]{return (IsChild && AtDay) || CanUse(HOOKSHOT);},
                                             /*Glitched*/[]{return IsAdult && ((HoverBoots && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(KAK_ROOFTOP,                 {[]{return CanUse(HOOKSHOT) || (LogicManOnRoof && (IsAdult || AtDay || Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT)));},
                                             /*Glitched*/[]{return LogicManOnRoof && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                  Entrance(THE_GRAVEYARD,               {[]{return true;}}),
                  Entrance(KAK_BEHIND_GATE,             {[]{return IsAdult || (KakarikoVillageGateOpen);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[KAK_IMPAS_LEDGE] = Area("Kak Impas Ledge", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_BACK, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
  });

  areaTable[KAK_ROOFTOP] = Area("Kak Rooftop", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_MAN_ON_ROOF, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[KAK_BACKYARD] = Area("Kak Backyard", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,          {[]{return true;}}),
                  Entrance(KAK_OPEN_GROTTO,           {[]{return true;}}),
                  Entrance(KAK_ODD_POULTICE_BUILDING, {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_BACK,      {[]{return IsAdult && AtDay;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[KAK_CARPENTER_BOSS_HOUSE] = Area("Kak Carpenter Boss House", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WakeUpAdultTalon, {[]{return WakeUpAdultTalon || (IsAdult && PocketEgg);}}),
                }, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_HOUSE_OF_SKULLTULA] = Area("Kak House of Skulltula", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_10_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 10;}}),
                  LocationAccess(KAK_20_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 20;}}),
                  LocationAccess(KAK_30_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 30;}}),
                  LocationAccess(KAK_40_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 40;}}),
                  LocationAccess(KAK_50_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 50;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE] = Area("Kak Impas House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,         {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_BACK] = Area("Kak Impas House Back", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_IMPAS_HOUSE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE,          {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_NEAR_COW] = Area("Kak Impas House Near Cow", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_IMPAS_HOUSE_COW, {[]{return CanPlay(EponasSong);},
                                           /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED))) && EponasSong;}}),
  }, {});

  areaTable[KAK_WINDMILL] = Area("Kak Windmill", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DrainWell, {[]{return DrainWell || (IsChild && CanPlay(SongOfStorms));},
                               /*Glitched*/[]{return IsChild && SongOfStorms && (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::ADVANCED) || ((Fish || Bugs) && CanShield && ((Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) &&
                                                     CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Fish || Bugs) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                }, {
                  //Locations
                  LocationAccess(KAK_WINDMILL_FREESTANDING_POH, {[]{return CanUse(BOOMERANG) || DampesWindmillAccess || (IsAdult && CanUse(HOOKSHOT) && LogicWindmillPoHHookshot);},
                                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(SONG_FROM_WINDMILL,            {[]{return IsAdult && Ocarina;},
                                                     /*Glitched*/[]{return IsAdult && (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::EXPERT) || ((Fish || Bugs) && CanShield && ((Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) || (DampesWindmillAccess || (IsAdult && CanUse(HOOKSHOT) && LogicWindmillPoHHookshot) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE))) &&
                                                                           CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Fish || Bugs) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_BAZAAR] = Area("Kak Bazaar", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_BAZAAR_ITEM_1, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_2, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_3, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_4, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_5, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_6, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_7, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_SHOOTING_GALLERY] = Area("Kak Shooting Gallery", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_SHOOTING_GALLERY_REWARD, {[]{return IsAdult && Bow;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_POTION_SHOP_FRONT] = Area("Kak Potion Shop Front", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_POTION_SHOP_ITEM_1, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_2, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_3, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_4, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_5, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_6, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_7, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_8, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
                  Entrance(KAK_POTION_SHOP_BACK, {[]{return IsAdult;}}),
  });

  areaTable[KAK_POTION_SHOP_BACK] = Area("Kak Potion Shop Back", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_BACKYARD,          {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_FRONT, {[]{return true;}}),
  });

  areaTable[KAK_ODD_POULTICE_BUILDING] = Area("Kak Granny's Potion Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&OddPoulticeAccess, {[]{return OddPoulticeAccess || (IsAdult && (OddMushroomAccess || (OddMushroom && DisableTradeRevert)));}}),
                }, {
                  LocationAccess(KAK_TRADE_ODD_MUSHROOM, {[]{return IsAdult && OddMushroom;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[KAK_REDEAD_GROTTO] = Area("Kak Redead Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_REDEAD_GROTTO_CHEST, {[]{return IsAdult || (Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_OPEN_GROTTO] = Area("Kak Open Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(KAK_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(KAK_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[THE_GRAVEYARD] = Area("The Graveyard", "The Graveyard", THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || (CanUse(STICKS) && AtDay);}}),
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(THE_GRAVEYARD) && CanPlay(SongOfStorms));}}),
                  EventAccess(&BugRock,        {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_FREESTANDING_POH,        {[]{return (IsAdult && CanPlantBean(THE_GRAVEYARD)) || CanUse(LONGSHOT) || (LogicGraveyardPoH && CanUse(BOOMERANG));},
                                                         /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, {[]{return IsChild && AtNight;}}), //TODO: This needs to change
                  LocationAccess(GRAVEYARD_GS_WALL,                 {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                                         /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(GRAVEYARD_GS_BEAN_PATCH,           {[]{return CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(GRAVEYARD_SHIELD_GRAVE,       {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_COMPOSERS_GRAVE,    {[]{return CanPlay(ZeldasLullaby);},
                                              /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanShield && (IsChild || AdultCanAccess(GRAVEYARD_WARP_PAD_REGION) || (IsAdult && (CanPlantBean(THE_GRAVEYARD) || CanUse(LONGSHOT))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2)))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                    ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                  Entrance(GRAVEYARD_HEART_PIECE_GRAVE,  {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_DAMPES_GRAVE,       {[]{return IsAdult;}}),
                  Entrance(GRAVEYARD_DAMPES_HOUSE,       {[]{return IsAdult || AtDampeTime;}}), //TODO: This needs to be handled
                  Entrance(KAKARIKO_VILLAGE,             {[]{return true;}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION,    {[]{return false;},
                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[GRAVEYARD_SHIELD_GRAVE] = Area("Graveyard Shield Grave", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_SHIELD_GRAVE_CHEST, {[]{return true;}}),
                  //Free Fairies
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_HEART_PIECE_GRAVE] = Area("Graveyard Heart Piece Grave", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_HEART_PIECE_GRAVE_CHEST, {[]{return CanPlay(SunsSong);},
                                                         /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanShield && (Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SunsSong;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_COMPOSERS_GRAVE] = Area("Graveyard Composers Grave", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_COMPOSERS_GRAVE_CHEST, {[]{return HasFireSource;},
                                                       /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(SONG_FROM_COMPOSERS_GRAVE,       {[]{return IsAdult || (Slingshot || Boomerang || Sticks || HasExplosives || KokiriSword || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(BOW) || CanUse(HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_DAMPES_GRAVE] = Area("Graveyard Dampes Grave", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,               {[]{return true;}}),
                  EventAccess(&DampesWindmillAccess, {[]{return DampesWindmillAccess || (IsAdult && CanPlay(SongOfTime));},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfTime;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_HOOKSHOT_CHEST,              {[]{return true;}}),
                  LocationAccess(GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, {[]{return IsAdult || LogicChildDampeRacePoH;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
                  Entrance(KAK_WINDMILL,  {[]{return IsAdult && CanPlay(SongOfTime);},
                               /*Glitched*/[]{return (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                     ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfTime;}}),
  });

  areaTable[GRAVEYARD_DAMPES_HOUSE] = Area("Graveyard Dampes House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_WARP_PAD_REGION] = Area("Graveyard Warp Pad Region", "Graveyard", THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD,             {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,    {[]{return CanUse(DINS_FIRE) || (LogicShadowFireArrowEntry && IsAdult && CanUse(FIRE_ARROWS));},
                                           /*Glitched*/[]{return (CanUse(STICKS) && (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE) || CanUse(FIRE_ARROWS))) || (CanTakeDamage && (
                                                                 CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE)));}}),
  });

  areaTable[KAK_BEHIND_GATE] = Area("Kak Behind Gate", "Kakariko Village", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,     {[]{return IsAdult || LogicVisibleCollision || KakarikoVillageGateOpen || OpenKakariko.Is(OPENKAKARIKO_OPEN);}}),
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[DEATH_MOUNTAIN_TRAIL] = Area("Death Mountain", "Death Mountain", DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(DEATH_MOUNTAIN_TRAIL) && CanPlay(SongOfStorms) && (HasExplosives || GoronBracelet));}}),
                }, {
                  //Locations
                  LocationAccess(DMT_CHEST,                    {[]{return CanBlastOrSmash || (LogicDMTBombable && IsChild && GoronBracelet);},
                                                    /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  LocationAccess(DMT_FREESTANDING_POH,         {[]{return CanTakeDamage || CanUse(HOVER_BOOTS) || (IsAdult && CanPlantBean(DEATH_MOUNTAIN_TRAIL) && (HasExplosives || GoronBracelet));}}),
                  LocationAccess(DMT_GS_BEAN_PATCH,            {[]{return CanPlantBugs && (HasExplosives || GoronBracelet || (LogicDMTSoilGS && (CanTakeDamage || CanUse(HOVER_BOOTS)) && CanUse(BOOMERANG)));}}),
                  LocationAccess(DMT_GS_NEAR_KAK,              {[]{return CanBlastOrSmash;},
                                                    /*Glitched*/[]{return (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}}),
                  LocationAccess(DMT_GS_ABOVE_DODONGOS_CAVERN, {[]{return IsAdult && AtNight && CanUse(MEGATON_HAMMER) && CanGetNightTimeGS;},
                                                    /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && CanUse(STICKS) && ((CanTakeDamageTwice && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}}),
                }, {
                  //Exits
                  Entrance(KAK_BEHIND_GATE,          {[]{return true;}}),
                  Entrance(GORON_CITY,               {[]{return true;}}),
                  Entrance(DEATH_MOUNTAIN_SUMMIT,    {[]{return Here(DEATH_MOUNTAIN_TRAIL, []{return CanBlastOrSmash;}) || (IsAdult && ((CanPlantBean(DEATH_MOUNTAIN_TRAIL) && GoronBracelet) || (HoverBoots && LogicDMTSummitHover)));},
                                          /*Glitched*/[]{return IsAdult && Here(DEATH_MOUNTAIN_TRAIL, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return HasExplosives || GoronBracelet || IsAdult;}}),
                  Entrance(DMT_STORMS_GROTTO,        {[]{return CanOpenStormGrotto;},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[DEATH_MOUNTAIN_SUMMIT] = Area("Death Mountain Summit", "Death Mountain", DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&PrescriptionAccess, {[]{return PrescriptionAccess || (IsAdult && (BrokenSwordAccess || BrokenSword));}}),
                  EventAccess(&GossipStoneFairy,   {[]{return GossipStoneFairy   || CanSummonGossipFairy;}}),
                  EventAccess(&BugRock,            {[]{return BugRock            || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(DMT_TRADE_BROKEN_SWORD,    {[]{return IsAdult && BrokenSword;}}),
                  LocationAccess(DMT_TRADE_EYEDROPS,        {[]{return IsAdult && Eyedrops;}}),
                  LocationAccess(DMT_TRADE_CLAIM_CHECK,     {[]{return IsAdult && ClaimCheck;}}),
                  LocationAccess(DMT_GS_FALLING_ROCKS_PATH, {[]{return IsAdult && AtNight && CanUse(MEGATON_HAMMER) && CanGetNightTimeGS;}}),
                  LocationAccess(DMT_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL,     {[]{return true;}}),
                  Entrance(DMC_UPPER_LOCAL,          {[]{return true;}}),
                  Entrance(DMT_OWL_FLIGHT,           {[]{return IsChild;}}),
                  Entrance(DMT_COW_GROTTO,           {[]{return Here(DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});},
                                          /*Glitched*/[]{return Here(DEATH_MOUNTAIN_SUMMIT, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(DMT_GREAT_FAIRY_FOUNTAIN, {[]{return Here(DEATH_MOUNTAIN_SUMMIT, []{return CanBlastOrSmash;});},
                                          /*Glitched*/[]{return ((KokiriSword || Sticks || IsAdult) && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::ADVANCED)) || (IsChild && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::EXPERT));}}),
  });

  areaTable[DMT_OWL_FLIGHT] = Area("DMT Owl Flight", "Death Mountain", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE, {[]{return true;}}),
  });

  areaTable[DMT_COW_GROTTO] = Area("DMT Cow Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMT_COW_GROTTO_COW, {[]{return CanPlay(EponasSong);},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && EponasSong;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),

  });

  areaTable[DMT_STORMS_GROTTO] = Area("DMT Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(DMT_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(DMT_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[DMT_GREAT_FAIRY_FOUNTAIN] = Area("DMT Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMT_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                              /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                    ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
  });

  areaTable[GORON_CITY] = Area("Goron City", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy,          {[]{return GossipStoneFairy          || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&StickPot,                  {[]{return StickPot                  || IsChild;}}),
                  EventAccess(&BugRock,                   {[]{return BugRock                   || (CanBlastOrSmash || CanUse(SILVER_GAUNTLETS));}}),
                  EventAccess(&GoronCityChildFire,        {[]{return GoronCityChildFire        || (IsChild && CanUse(DINS_FIRE));},
                                               /*Glitched*/[]{return IsChild && CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE);}}),
                  EventAccess(&GCWoodsWarpOpen,           {[]{return GCWoodsWarpOpen           || (CanBlastOrSmash || CanUse(DINS_FIRE) || CanUse(BOW) || GoronBracelet || GoronCityChildFire);},
                                               /*Glitched*/[]{return (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}}),
                  EventAccess(&GCDaruniasDoorOpenChild,   {[]{return GCDaruniasDoorOpenChild   || (IsChild && CanPlay(ZeldasLullaby));},
                                               /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                     ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLullaby;}}),
                  EventAccess(&StopGCRollingGoronAsAdult, {[]{return StopGCRollingGoronAsAdult || (IsAdult && (GoronBracelet || HasExplosives || Bow || (LogicLinkGoronDins && CanUse(DINS_FIRE))));}}),
                }, {
                  //Locations
                  LocationAccess(GC_MAZE_LEFT_CHEST,        {[]{return CanUse(MEGATON_HAMMER) || CanUse(SILVER_GAUNTLETS) || (LogicGoronCityLeftMost && HasExplosives && CanUse(HOVER_BOOTS));},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GC_MAZE_CENTER_CHEST,      {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_MAZE_RIGHT_CHEST,       {[]{return CanBlastOrSmash  || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_POT_FREESTANDING_POH,   {[]{return IsChild && GoronCityChildFire && (Bombs || (GoronBracelet && LogicGoronCityPotWithStrength) || (HasBombchus && LogicGoronCityPot));}}),
                  LocationAccess(GC_ROLLING_GORON_AS_CHILD, {[]{return IsChild && (HasExplosives || (GoronBracelet && LogicChildRollingWithStrength));}}),
                  LocationAccess(GC_ROLLING_GORON_AS_ADULT, {[]{return StopGCRollingGoronAsAdult;}}),
                  LocationAccess(GC_GS_BOULDER_MAZE,        {[]{return IsChild && CanBlastOrSmash;}}),
                  LocationAccess(GC_GS_CENTER_PLATFORM,     {[]{return IsAdult;}}),
                  LocationAccess(GC_MEDIGORON,              {[]{return IsAdult && AdultsWallet && (CanBlastOrSmash || GoronBracelet);}}),
                  LocationAccess(GC_MAZE_GOSSIP_STONE,      {[]{return CanBlastOrSmash || CanUse(SILVER_GAUNTLETS);}}),
                  LocationAccess(GC_MEDIGORON_GOSSIP_STONE, {[]{return CanBlastOrSmash || GoronBracelet;}}),
                }, {
                  //Exits
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
                  Entrance(GC_WOODS_WARP,        {[]{return GCWoodsWarpOpen;}}),
                  Entrance(GC_SHOP,              {[]{return (IsAdult && StopGCRollingGoronAsAdult) || (IsChild && (CanBlastOrSmash || GoronBracelet || GoronCityChildFire || CanUse(BOW)));},
                                      /*Glitched*/[]{return IsChild && Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(GC_DARUNIAS_CHAMBER,  {[]{return (IsAdult && StopGCRollingGoronAsAdult) || GCDaruniasDoorOpenChild;}}),
                  Entrance(GC_GROTTO,            {[]{return IsAdult && ((CanPlay(SongOfTime) && ((EffectiveHealth > 2) || CanUse(GORON_TUNIC) || CanUse(LONGSHOT) || CanUse(NAYRUS_LOVE))) || (EffectiveHealth > 1 && CanUse(GORON_TUNIC) && CanUse(HOOKSHOT)) || (CanUse(NAYRUS_LOVE) && CanUse(HOOKSHOT)));},
                                      /*Glitched*/[]{return (HasBombchus && ((IsChild && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE))) || (IsChild && CanUse(LONGSHOT));}}),
  });

  areaTable[GC_WOODS_WARP] = Area("GC Woods Warp", "Goron City", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GCWoodsWarpOpen, {[]{return GCWoodsWarpOpen || (CanBlastOrSmash || CanUse(DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(GORON_CITY,     {[]{return CanLeaveForest && GCWoodsWarpOpen;}}),
                  Entrance(THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[GC_DARUNIAS_CHAMBER] = Area("GC Darunias Chamber", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GoronCityChildFire, {[]{return GoronCityChildFire || (IsChild && CanUse(STICKS));}}),
                }, {
                  //Locations
                  LocationAccess(GC_DARUNIAS_JOY, {[]{return IsChild && CanPlay(SariasSong);},
                                       /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && ((Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) || GCDaruniasDoorOpenChild) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                             ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SariasSong;}}),
                }, {
                  //Exits
                  Entrance(GORON_CITY,      {[]{return true;}}),
                  Entrance(DMC_LOWER_LOCAL, {[]{return IsAdult;},
                                 /*Glitched*/[]{return IsChild && GCDaruniasDoorOpenChild && (KokiriSword || Sticks) && GlitchOccamsStatue;}}),
  });

  areaTable[GC_GROTTO_PLATFORM] = Area("GC Grotto Platform", "Goron City", GORON_CITY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GC_GROTTO,  {[]{return true;}}),
                  Entrance(GORON_CITY, {[]{return EffectiveHealth > 2 || CanUse(GORON_TUNIC) || CanUse(NAYRUS_LOVE) || ((IsChild || CanPlay(SongOfTime)) && CanUse(LONGSHOT));}}),
  });

  areaTable[GC_SHOP] = Area("GC Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GC_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(GC_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GORON_CITY, {[]{return true;}}),
  });

  areaTable[GC_GROTTO] = Area("GC Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(GC_GROTTO_PLATFORM, {[]{return true;}}),
  });

  areaTable[DMC_UPPER_NEARBY] = Area("DMC Upper Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_UPPER_LOCAL,       {[]{return FireTimer >= 48;}}),
                  Entrance(DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
                  Entrance(DMC_UPPER_GROTTO,      {[]{return Here(DMC_UPPER_NEARBY, []{return CanBlastOrSmash && (FireTimer >= 8 || Hearts >= 3);});},
                                       /*Glitched*/[]{return Here(DMC_UPPER_NEARBY, []{return CanUse(STICKS) && FireTimer >= 48 && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}})
  });

  areaTable[DMC_UPPER_LOCAL] = Area("DMC Upper Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (HasExplosives && CanSummonGossipFairyWithoutSuns && (FireTimer >= 16 || Hearts >= 3));}}),
                }, {
                  //Locations
                  LocationAccess(DMC_WALL_FREESTANDING_POH, {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  LocationAccess(DMC_GS_CRATE,              {[]{return (FireTimer >= 8 || Hearts >= 3) && IsChild && CanChildAttack;}}),
                  LocationAccess(DMC_GOSSIP_STONE,          {[]{return HasExplosives && (FireTimer >= 16 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_NEARBY,         {[]{return true;}}),
                  Entrance(DMC_LADDER_AREA_NEARBY,   {[]{return FireTimer >= 16 || Hearts >= 3;}}),
                  Entrance(DMC_CENTRAL_NEARBY,       {[]{return IsAdult && CanUse(GORON_TUNIC) && CanUse(DISTANT_SCARECROW) && ((EffectiveHealth > 2) || (Fairy && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || CanUse(NAYRUS_LOVE));},
                                          /*Glitched*/[]{return FireTimer >= 24 && CanTakeDamage && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
                  Entrance(DMC_LOWER_NEARBY,         {[]{return false;},
                                          /*Glitched*/[]{return FireTimer >= 24 && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[DMC_LADDER_AREA_NEARBY] = Area("DMC Ladder Area Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_DEKU_SCRUB, {[]{return IsChild && CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_NEARBY, {[]{return Hearts >= 3;}}),
                  Entrance(DMC_LOWER_NEARBY, {[]{return Hearts >= 3 && (CanUse(HOVER_BOOTS) || (LogicCraterUpperToLower && IsAdult && CanUse(MEGATON_HAMMER)));}}),
  });

  areaTable[DMC_LOWER_NEARBY] = Area("DMC Lower Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL,          {[]{return FireTimer >= 48;}}),
                  Entrance(GC_DARUNIAS_CHAMBER,      {[]{return true;}}),
                  Entrance(DMC_GREAT_FAIRY_FOUNTAIN, {[]{return CanUse(MEGATON_HAMMER);},
                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || (FireTimer >= 48 && CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE) && (CanTakeDamageTwice || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED))) ||
                                                                (Bombs && CanShield && FireTimer >= 48 && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::EXPERT)) || Here(DMC_UPPER_LOCAL, []{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(DMC_HAMMER_GROTTO,        {[]{return IsAdult && CanUse(MEGATON_HAMMER);},
                                          /*Glitched*/[]{return IsAdult && FireTimer >= 48 && CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE) && (CanTakeDamageTwice || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}}),
  });

  areaTable[DMC_LOWER_LOCAL] = Area("DMC Lower Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DMC_LOWER_NEARBY,       {[]{return true;}}),
                  Entrance(DMC_LADDER_AREA_NEARBY, {[]{return true;}}),
                  Entrance(DMC_CENTRAL_NEARBY,     {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT)) && (FireTimer >= 8 || Hearts >= 3);},
                                        /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) && (FireTimer >= 8 || Hearts >= 3);}}),
                  Entrance(DMC_CENTRAL_LOCAL,      {[]{return (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT)) && FireTimer >= 24;}}),
  });

  areaTable[DMC_CENTRAL_NEARBY] = Area("DMC Central Nearby", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_VOLCANO_FREESTANDING_POH, {[]{return IsAdult && Hearts >= 3 && (CanPlantBean(DMC_CENTRAL_LOCAL) || (LogicCraterBeanPoHWithHovers && HoverBoots));},
                                                    /*Glitched*/[]{return Here(DMC_LOWER_LOCAL, []{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) && FireTimer >= 24;});}}),
                  LocationAccess(SHEIK_IN_CRATER,              {[]{return IsAdult && (FireTimer >= 8 || Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(DMC_CENTRAL_LOCAL, {[]{return FireTimer >= 48;}}),
  });

  areaTable[DMC_CENTRAL_LOCAL] = Area("DMC Central Local", "Death Mountain Crater", DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(DMC_CENTRAL_LOCAL) && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(DMC_GS_BEAN_PATCH, {[]{return (FireTimer >= 8 || Hearts >= 3) && CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(DMC_CENTRAL_NEARBY,   {[]{return true;}}),
                  Entrance(DMC_LOWER_NEARBY,     {[]{return (IsAdult && CanPlantBean(DMC_CENTRAL_LOCAL)) || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT);},
                                      /*Glitched*/[]{return IsChild && Hearts >= 3 && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(DMC_UPPER_NEARBY,     {[]{return IsAdult && CanPlantBean(DMC_CENTRAL_LOCAL);}}),
                  Entrance(FIRE_TEMPLE_ENTRYWAY, {[]{return (IsChild && Hearts >= 3 && ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF)) || (IsAdult && FireTimer >= 24);},
                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) && FireTimer >= 48;}}),
  });

  areaTable[DMC_GREAT_FAIRY_FOUNTAIN] = Area("DMC Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                              /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanTakeDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                    ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[DMC_UPPER_GROTTO] = Area("DMC Upper Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(DMC_UPPER_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(DMC_UPPER_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DMC_UPPER_LOCAL, {[]{return true;}}),
  });

  areaTable[DMC_HAMMER_GROTTO] = Area("DMC Hammer Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(DMC_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[ZR_FRONT] = Area("ZR Front", "Zora River", ZORAS_RIVER, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZR_GS_TREE, {[]{return IsChild && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return IsAdult || CanBlastOrSmash;},
                              /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[ZORAS_RIVER] = Area("Zora River", "Zora River", ZORAS_RIVER, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(ZORAS_RIVER) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(ZR_MAGIC_BEAN_SALESMAN,               {[]{return IsChild;}}),
                  LocationAccess(ZR_FROGS_OCARINA_GAME,                {[]{return IsChild && CanPlay(ZeldasLullaby) && CanPlay(SariasSong) && CanPlay(SunsSong) && CanPlay(EponasSong) && CanPlay(SongOfTime) && CanPlay(SongOfStorms);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLullaby && SariasSong && SunsSong && EponasSong && SongOfTime && SongOfStorms;}}),
                  LocationAccess(ZR_FROGS_IN_THE_RAIN,                 {[]{return IsChild && CanPlay(SongOfStorms);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SongOfStorms;}}),
                  LocationAccess(ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverLower);}}),
                  LocationAccess(ZR_NEAR_DOMAIN_FREESTANDING_POH,      {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverUpper);}}),
                  LocationAccess(ZR_GS_LADDER,                         {[]{return IsChild && AtNight && CanChildAttack && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_GS_NEAR_RAISED_GROTTOS,            {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_GS_ABOVE_BRIDGE,                   {[]{return IsAdult && CanUse(HOOKSHOT) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_NEAR_GROTTOS_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(ZR_NEAR_DOMAIN_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_FRONT,            {[]{return true;}}),
                  Entrance(ZR_OPEN_GROTTO,      {[]{return true;}}),
                  Entrance(ZR_FAIRY_GROTTO,     {[]{return Here(ZORAS_RIVER, []{return CanBlastOrSmash;});},
                                     /*Glitched*/[]{return Here(ZORAS_RIVER, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(THE_LOST_WOODS,      {[]{return CanDive || CanUse(IRON_BOOTS);},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                           ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(ZR_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;},
                                     /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                           ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return CanPlay(ZeldasLullaby);},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED) || ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                           ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby);}}),
  });

  areaTable[ZR_BEHIND_WATERFALL] = Area("ZR Behind Waterfall", "Zora River", NONE, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return true;}}),
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZR_OPEN_GROTTO] = Area("ZR Open Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(ZR_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(ZR_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_FAIRY_GROTTO] = Area("ZR Fairy Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Event
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_STORMS_GROTTO] = Area("ZR Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZR_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(ZR_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZORAS_DOMAIN] = Area("Zoras Domain", "Zoras Domain", ZORAS_DOMAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&EyeballFrogAccess, {[]{return EyeballFrogAccess || (IsAdult && KingZoraThawed && (Eyedrops || EyeballFrog || Prescription || PrescriptionAccess));}}),
                  EventAccess(&GossipStoneFairy,  {[]{return GossipStoneFairy  || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&NutPot,            {[]{return true;}}),
                  EventAccess(&StickPot,          {[]{return StickPot          || IsChild;}}),
                  EventAccess(&FishGroup,         {[]{return FishGroup         || IsChild;}}),
                  EventAccess(&KingZoraThawed,    {[]{return KingZoraThawed    || (IsAdult     && BlueFire);}}),
                  EventAccess(&DeliverLetter,     {[]{return DeliverLetter     || (RutosLetter && IsChild && ZorasFountain.IsNot(ZORASFOUNTAIN_OPEN));}}),
                }, {
                  //Locations
                  LocationAccess(ZD_DIVING_MINIGAME,     {[]{return IsChild;}}),
                  LocationAccess(ZD_CHEST,               {[]{return IsChild && CanUse(STICKS);}}),
                  LocationAccess(ZD_KING_ZORA_THAWED,    {[]{return KingZoraThawed;}}),
                  LocationAccess(ZD_TRADE_PRESCRIPTION,  {[]{return KingZoraThawed && Prescription;}}),
                  LocationAccess(ZD_GS_FROZEN_WATERFALL, {[]{return IsAdult && AtNight && (HookshotOrBoomerang || CanUse(SLINGSHOT) || Bow || MagicMeter) && CanGetNightTimeGS;}}),
                  LocationAccess(ZD_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return true;}}),
                  Entrance(LAKE_HYLIA,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));},
                                     /*Glitched*/[]{return (IsAdult && GlitchZDOoBJumpSlash) || (IsChild && CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || WeirdEgg))) &&
                                                           ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(ZD_BEHIND_KING_ZORA, {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult);},
                                     /*Glitched*/[]{return ((IsChild || CanUse(IRON_BOOTS)) && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::NOVICE) ||
                                                           CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(ZD_SHOP,             {[]{return IsChild || BlueFire;},
                                     /*Glitched*/[]{return GlitchZDOoBJumpSlash.Value<bool>();}}),
                  Entrance(ZD_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;},
                                     /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                           ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[ZD_BEHIND_KING_ZORA] = Area("ZD Behind King Zora", "Zoras Domain", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN,   {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult);},
                                /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::NOVICE) || (CanUse(LONGSHOT) && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE)) || (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[ZD_SHOP] = Area("ZD Shop", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZD_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZD_STORMS_GROTTO] = Area("ZD Storms Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZORAS_FOUNTAIN] = Area("Zoras Fountain", "Zoras Fountain", ZORAS_FOUNTAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(STICKS) && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(ZF_ICEBERG_FREESTANDING_POH, {[]{return IsAdult;}}),
                  LocationAccess(ZF_BOTTOM_FREESTANDING_POH,  {[]{return IsAdult && IronBoots && WaterTimer >= 24;}}),
                  LocationAccess(ZF_GS_TREE,                  {[]{return IsChild;}}),
                  LocationAccess(ZF_GS_ABOVE_THE_LOG,         {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                                   /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(ZF_GS_HIDDEN_CAVE,           {[]{return CanUse(SILVER_GAUNTLETS) && CanBlastOrSmash && HookshotOrBoomerang && IsAdult && AtNight && CanGetNightTimeGS;},
                                                   /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::INTERMEDIATE) && IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZF_FAIRY_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(ZF_JABU_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZD_BEHIND_KING_ZORA,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY, {[]{return (IsChild && Fish);},
                                           /*Glitched*/[]{return (IsChild && CanUse(STICKS) && GlitchJabuStickRecoil) || (IsAdult && GlitchJabuAdult);}}),
                  Entrance(ICE_CAVERN_ENTRYWAY,       {[]{return IsAdult;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(ZF_GREAT_FAIRY_FOUNTAIN,   {[]{return HasExplosives || (CanUse(SILVER_GAUNTLETS) && Hammer && LogicZFGreatFairy);},
                                           /*Glitched*/[]{return IsChild && (KokiriSword || Sticks) && CanShield && (CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::ADVANCED) || (CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::INTERMEDIATE)));}}),
  });

  areaTable[ZF_GREAT_FAIRY_FOUNTAIN] = Area("ZF Great Fairy Fountain", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZF_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                   ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[LON_LON_RANCH] = Area("Lon Lon Ranch", "Lon Lon Ranch", LON_LON_RANCH, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&Epona,    {[]{return Epona    || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                  EventAccess(&LinksCow, {[]{return LinksCow || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(SONG_FROM_MALON,     {[]{return IsChild && ZeldasLetter && Ocarina && AtDay;},
                                           /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                 ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLetter && AtDay;}}),
                  LocationAccess(LLR_GS_TREE,         {[]{return IsChild;}}),
                  LocationAccess(LLR_GS_RAIN_SHED,    {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LLR_GS_HOUSE_WINDOW, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LLR_GS_BACK_WALL,    {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && IsChild && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,     {[]{return true;}}),
                  Entrance(LLR_TALONS_HOUSE, {[]{return true;}}),
                  Entrance(LLR_STABLES,      {[]{return true;}}),
                  Entrance(LLR_TOWER,        {[]{return true;}}),
                  Entrance(LLR_GROTTO,       {[]{return IsChild;}}),
  });

  areaTable[LLR_TALONS_HOUSE] = Area("LLR Talons House", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_TALONS_CHICKENS, {[]{return IsChild && AtDay && ZeldasLetter;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_STABLES] = Area("LLR Stables", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_STABLES_LEFT_COW,  {[]{return CanPlay(EponasSong);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                  LocationAccess(LLR_STABLES_RIGHT_COW, {[]{return CanPlay(EponasSong);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_TOWER] = Area("LLR Tower", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_FREESTANDING_POH, {[]{return IsChild;}}),
                  LocationAccess(LLR_TOWER_LEFT_COW,   {[]{return CanPlay(EponasSong);},
                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                  LocationAccess(LLR_TOWER_RIGHT_COW,  {[]{return CanPlay(EponasSong);},
                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_GROTTO] = Area("LLR Grotto", "", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  /*--------------------------
  |    VANILLA/MQ DECIDERS   |
  ---------------------------*/
  areaTable[DEKU_TREE_ENTRYWAY] = Area("Deku Tree Entryway", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,      {[]{return Dungeon::DekuTree.IsVanilla();}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,   {[]{return Dungeon::DekuTree.IsMQ();}}),
                  Entrance(KF_OUTSIDE_DEKU_TREE, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_ENTRYWAY] = Area("Dodongos Cavern Entryway", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BEGINNING,    {[]{return Dungeon::DodongosCavern.IsVanilla();}}),
                  Entrance(DODONGOS_CAVERN_MQ_BEGINNING, {[]{return Dungeon::DodongosCavern.IsMQ();}}),
                  Entrance(DEATH_MOUNTAIN_TRAIL,         {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_ENTRYWAY] = Area("Jabu Jabus Belly Entryway", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_BEGINNING,    {[]{return Dungeon::JabuJabusBelly.IsVanilla();}}),
                  Entrance(JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return Dungeon::JabuJabusBelly.IsMQ();}}),
                  Entrance(ZORAS_FOUNTAIN,                {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_ENTRYWAY] = Area("Forest Temple Entryway", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FIRST_ROOM, {[]{return Dungeon::ForestTemple.IsVanilla();}}),
                  Entrance(FOREST_TEMPLE_MQ_LOBBY,   {[]{return Dungeon::ForestTemple.IsMQ();}}),
                  Entrance(SACRED_FOREST_MEADOW,     {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_ENTRYWAY] = Area("Fire Temple Entryway", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return Dungeon::FireTemple.IsVanilla();}}),
                  Entrance(FIRE_TEMPLE_MQ_LOWER,   {[]{return Dungeon::FireTemple.IsMQ();}}),
                  Entrance(DMC_CENTRAL_LOCAL,      {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_ENTRYWAY] = Area("Water Temple Entryway", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,    {[]{return Dungeon::WaterTemple.IsVanilla();}}),
                  Entrance(WATER_TEMPLE_MQ_LOBBY, {[]{return Dungeon::WaterTemple.IsMQ();}}),
                  Entrance(LAKE_HYLIA,            {[]{return true;}}),
  });

  areaTable[SPIRIT_TEMPLE_ENTRYWAY] = Area("Spirit Temple Entryway", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_LOBBY,    {[]{return Dungeon::SpiritTemple.IsVanilla();}}),
                  Entrance(SPIRIT_TEMPLE_MQ_LOBBY, {[]{return Dungeon::SpiritTemple.IsMQ();}}),
                  Entrance(DESERT_COLOSSUS,        {[]{return true;}}),
  });

  areaTable[SHADOW_TEMPLE_ENTRYWAY] = Area("Shadow Temple Entryway", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_BEGINNING,    {[]{return Dungeon::ShadowTemple.IsVanilla() && (LogicLensShadow || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));},
                                            /*Glitched*/[]{return Dungeon::ShadowTemple.IsVanilla() && (LogicLensShadow || CanUse(LENS_OF_TRUTH)) && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(SHADOW_TEMPLE_MQ_BEGINNING, {[]{return Dungeon::ShadowTemple.IsMQ()    && (LogicLensShadowMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));},
                                            /*Glitched*/[]{return Dungeon::ShadowTemple.IsMQ()    && (LogicLensShadowMQ || CanUse(LENS_OF_TRUTH)) && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION,  {[]{return true;}}),
  });

  areaTable[BOTTOM_OF_THE_WELL_ENTRYWAY] = Area("Bottom of the Well Entryway", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return Dungeon::BottomOfTheWell.IsVanilla() && IsChild && (CanChildAttack || Nuts);},
                                                 /*Glitched*/[]{return Dungeon::BottomOfTheWell.IsVanilla() && IsChild && CanUse(MEGATON_HAMMER);}}),
                  Entrance(BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return Dungeon::BottomOfTheWell.IsMQ()      && IsChild;},
                                                 /*Glitched*/[]{return Dungeon::BottomOfTheWell.IsMQ()      && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE) && Longshot;}}),
                  Entrance(KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  areaTable[ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_BEGINNING,    {[]{return Dungeon::IceCavern.IsVanilla();}}),
                  Entrance(ICE_CAVERN_MQ_BEGINNING, {[]{return Dungeon::IceCavern.IsMQ();}}),
                  Entrance(ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_ENTRYWAY] = Area("Gerudo Training Grounds Entryway", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_LOBBY,    {[]{return Dungeon::GerudoTrainingGrounds.IsVanilla();}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, {[]{return Dungeon::GerudoTrainingGrounds.IsMQ();}}),
                  Entrance(GERUDO_FORTRESS,                  {[]{return true;}}),
  });

  areaTable[GANONS_CASTLE_ENTRYWAY] = Area("Ganon's Castle Entryway", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_LOBBY,    {[]{return Dungeon::GanonsCastle.IsVanilla();}}),
                  Entrance(GANONS_CASTLE_MQ_LOBBY, {[]{return Dungeon::GanonsCastle.IsMQ();}}),
                  Entrance(GANONS_CASTLE_GROUNDS,  {[]{return true;}}),
  });

  /*--------------------------
  |    VANILLA DUNGEONS      |
  ---------------------------*/
  if (Dungeon::DekuTree.IsVanilla()) {
  areaTable[DEKU_TREE_LOBBY] = Area("Deku Tree Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(DEKU_TREE_2F_MIDDLE_ROOM,    {[]{return true;}}),
                  Entrance(DEKU_TREE_COMPASS_ROOM,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_LOWER,    {[]{return Here(DEKU_TREE_LOBBY, []{return IsAdult || CanChildAttack || Nuts;});},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return false;},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
                  Entrance(DEKU_TREE_BOSS_ROOM,         {[]{return false;},
                                             /*Glitched*/[]{return IsChild && CanUse(KOKIRI_SWORD) && Nuts && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::EXPERT);}}),
  });

  areaTable[DEKU_TREE_2F_MIDDLE_ROOM] = Area("Deku Tree 2F Middle Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,         {[]{return Here(DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(MEGATON_HAMMER);});}}),
                  Entrance(DEKU_TREE_SLINGSHOT_ROOM,{[]{return Here(DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(MEGATON_HAMMER);});}}),
  });

  areaTable[DEKU_TREE_SLINGSHOT_ROOM] = Area("Deku Tree Slingshot Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_SLINGSHOT_CHEST,           {[]{return true;}}),
                  LocationAccess(DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_2F_MIDDLE_ROOM, {[]{return CanUse(SLINGSHOT) || CanUse(HOVER_BOOTS);},
                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[DEKU_TREE_COMPASS_ROOM] = Area("Deku Tree Compass Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_COMPASS_CHEST,           {[]{return true;}}),
                  LocationAccess(DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, {[]{return true;}}),
                  LocationAccess(DEKU_TREE_GS_COMPASS_ROOM,         {[]{return IsAdult || CanChildAttack;},
                                                         /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,     {[]{return HasFireSourceWithTorch || CanUse(BOW);}}),
                  Entrance(DEKU_TREE_BOSS_ROOM, {[]{return false;},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[DEKU_TREE_BASEMENT_LOWER] = Area("Deku Tree Basement Lower", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_BASEMENT_CHEST,    {[]{return true;}}),
                  LocationAccess(DEKU_TREE_GS_BASEMENT_GATE,  {[]{return IsAdult || CanChildAttack;},
                                                   /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  LocationAccess(DEKU_TREE_GS_BASEMENT_VINES, {[]{return CanUseProjectile || CanUse(DINS_FIRE) || (LogicDekuBasementGS && (IsAdult || Sticks || KokiriSword));},
                                                   /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,               {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return Here(DEKU_TREE_BASEMENT_LOWER, []{return HasFireSourceWithTorch || CanUse(BOW);});}}),
                  Entrance(DEKU_TREE_BASEMENT_UPPER,      {[]{return IsAdult || LogicDekuB1Skip || HasAccessTo(DEKU_TREE_BASEMENT_UPPER);},
                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return false;},
                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[DEKU_TREE_BASEMENT_SCRUB_ROOM] = Area("Deku Tree Basement Scrub Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return Here(DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return CanUse(SLINGSHOT) || CanUse(BOW);});}}),
  });

  areaTable[DEKU_TREE_BASEMENT_WATER_ROOM] = Area("Deku Tree Basement Water Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_TORCH_ROOM] = Area("Deku Tree Basement Torch Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return Here(DEKU_TREE_BASEMENT_TORCH_ROOM, []{return HasFireSourceWithTorch || CanUse(BOW);});}}),
  });

  areaTable[DEKU_TREE_BASEMENT_BACK_LOBBY] = Area("Deku Tree Basement Back Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(BOW);}) &&
                                                                            (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE)));},
                                    /*Glitched*/[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(BOW);}) && CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_ROOM,  {[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) &&
                                                                     Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return CanBlastOrSmash;});},
                                               /*Glitched*/[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) &&
                                                                     Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT));});}}),
                  Entrance(DEKU_TREE_BASEMENT_UPPER,      {[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) && IsChild;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_BACK_ROOM] = Area("Deku Tree Basement Back Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_GS_BASEMENT_BACK_ROOM, {[]{return HookshotOrBoomerang;},
                                                       /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_UPPER] = Area("Deku Tree Basement Upper", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return Here(DEKU_TREE_BASEMENT_UPPER, []{return HasFireSourceWithTorch || (LogicDekuB1WebsWithBow && IsAdult && CanUse(BOW));});}}),
  });

  areaTable[DEKU_TREE_OUTSIDE_BOSS_ROOM] = Area("Deku Tree Outside Boss Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_UPPER, {[]{return true;}}),
                  Entrance(DEKU_TREE_BOSS_ROOM,      {[]{return Here(DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return HasShield;});}}),
  });

  areaTable[DEKU_TREE_BOSS_ROOM] = Area("Deku Tree Boss Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuTreeClear, {[]{return DekuTreeClear || ((IsAdult || KokiriSword || Sticks) && (Nuts || CanUse(SLINGSHOT) || CanUse(BOW) || HookshotOrBoomerang));},
                                   /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Locations
                  LocationAccess(QUEEN_GOHMA,                 {[]{return DekuTreeClear;}}),
                  LocationAccess(DEKU_TREE_QUEEN_GOHMA_HEART, {[]{return DekuTreeClear;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_ENTRYWAY,          {[]{return DekuTreeClear;}}),
  });
  }

  if (Dungeon::DodongosCavern.IsVanilla()) {
  areaTable[DODONGOS_CAVERN_BEGINNING] = Area("Dodongos Cavern Beginning", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOBBY,    {[]{return Here(DODONGOS_CAVERN_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});},
                                          /*Glitched*/[]{return Here(DODONGOS_CAVERN_BEGINNING, []{return GlitchBlueFireWall && BlueFire;});}}),
  });

  areaTable[DODONGOS_CAVERN_LOBBY] = Area("Dodongos Cavern Lobby", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (CanSummonGossipFairy && Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;}));},
                                      /*Glitched*/[]{return CanSummonGossipFairy && Here(DODONGOS_CAVERN_LOBBY, []{return GlitchBlueFireWall && BlueFire;});}}),
                }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MAP_CHEST,        {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});},
                                                        /*Glitched*/[]{return Here(DODONGOS_CAVERN_LOBBY, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}) || CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, {[]{return CanStunDeku || GoronBracelet;}}),
                  LocationAccess(DODONGOS_CAVERN_GOSSIP_STONE,     {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});},
                                                        /*Glitched*/[]{return Here(DODONGOS_CAVERN_LOBBY, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));});}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BEGINNING,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOBBY_SWITCH, {[]{return IsAdult;},
                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) || (HasExplosives && (CanUse(SLINGSHOT) || CanUse(HOOKSHOT) || CanUse(BOW)) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) || (CanShield && GoronBracelet && GlitchModernHalfie) || ((KokiriSword || Sticks || CanUse(MEGATON_HAMMER)) && (Bombs || GoronBracelet) && Fairy && GlitchClassicHalfie);}}),
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR,  {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});},
                                              /*Glitched*/[]{return Here(DODONGOS_CAVERN_LOBBY, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));});}}),
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return HasAccessTo(DODONGOS_CAVERN_LOBBY_SWITCH);}}),
                  Entrance(DODONGOS_CAVERN_FAR_BRIDGE,   {[]{return HasAccessTo(DODONGOS_CAVERN_FAR_BRIDGE);},
                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(DODONGOS_CAVERN_BOSS_AREA,    {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return HasExplosives;});}}),
                  Entrance(DODONGOS_CAVERN_BOSS_ROOM,    {[]{return false;},
                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[DODONGOS_CAVERN_LOBBY_SWITCH] = Area("Dodongos Cavern Lobby Switch", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_SE_CORRIDOR] = Area("Dodongos Cavern SE Corridor", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_SCARECROW, {[]{return CanUse(SCARECROW) || (IsAdult && CanUse(LONGSHOT)) || (LogicDCScarecrowGS && (IsAdult || CanChildAttack));},
                                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (CanUse(LONGSHOT) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,               {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_SE_ROOM,             {[]{return Here(DODONGOS_CAVERN_SE_CORRIDOR, []{return CanBlastOrSmash || IsAdult || CanChildAttack || (CanTakeDamage && CanShield);});},
                                                     /*Glitched*/[]{return Here(DODONGOS_CAVERN_SE_CORRIDOR, []{return (GlitchBlueFireWall && BlueFire);});}}),
                  Entrance(DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_SE_ROOM] = Area("Dodongos Cavern SE Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return IsAdult || CanChildAttack;},
                                                                        /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Area("Dodongos Cavern Near Lower Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_LOWER_LIZALFOS] = Area("Dodongos Cavern Lower Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});},
                                                     /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM,        {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});},
                                                     /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[DODONGOS_CAVERN_DODONGO_ROOM] = Area("Dodongos Cavern Dodongo Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY_SWITCH,      {[]{return HasFireSourceWithTorch;}}),
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_NEAR_DODONGO_ROOM, {[]{return Here(DODONGOS_CAVERN_DODONGO_ROOM, []{return CanBlastOrSmash || GoronBracelet;});},
                                                   /*Glitched*/[]{return Here(DODONGOS_CAVERN_DODONGO_ROOM, []{return GlitchBlueFireWall && BlueFire;});}}),
  });

  areaTable[DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Area("Dodongos Cavern Near Dodongo Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_STAIRS_LOWER] = Area("Dodongos Cavern Stairs Lower", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_STAIRS_UPPER, {[]{return HasExplosives || GoronBracelet || CanUse(DINS_FIRE) || (LogicDCStaircase && CanUse(BOW));}}),
                  Entrance(DODONGOS_CAVERN_COMPASS_ROOM, {[]{return Here(DODONGOS_CAVERN_STAIRS_LOWER, []{return CanBlastOrSmash || GoronBracelet;});},
                                              /*Glitched*/[]{return Here(DODONGOS_CAVERN_STAIRS_LOWER, []{return GlitchBlueFireWall && BlueFire;});}}),
  });

  areaTable[DODONGOS_CAVERN_STAIRS_UPPER] = Area("Dodongos Cavern Stairs Upper", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return HookshotOrBoomerang;}) || CanUse(LONGSHOT);},
                                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE) && HookshotOrBoomerang);}}),
                  LocationAccess(DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_ARMOS_ROOM,   {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_COMPASS_ROOM] = Area("Dodongos Cavern Compass Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return IsAdult || HasExplosives || GoronBracelet;},
                                              /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[DODONGOS_CAVERN_ARMOS_ROOM] = Area("Dodongos Cavern Armos Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_UPPER,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Area("Dodongos Cavern Bomb Room Lower", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_2F_SIDE_ROOM,         {[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash;});},
                                                      /*Glitched*/[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));});}}),
                  Entrance(DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, {[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash || GoronBracelet;});},
                                                      /*Glitched*/[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));});}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER,      {[]{return (IsAdult && LogicDCJump) || CanUse(HOVER_BOOTS) || (IsAdult && CanUse(LONGSHOT));},
                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) ||
                                                                            (IsAdult && (CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(SLINGSHOT)) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED));}}),
  });

  areaTable[DODONGOS_CAVERN_2F_SIDE_ROOM] = Area("Dodongos Cavern 2F Side Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Area("Dodongos Cavern First Slingshot Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return CanUse(SLINGSHOT) || CanUse(BOW) || LogicDCSlingshotSkip;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (IsAdult && CanUse(HOOKSHOT) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED));}}),
  });

  areaTable[DODONGOS_CAVERN_UPPER_LIZALFOS] = Area("Dodongos Cavern Upper Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});},
                                                       /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});},
                                                       /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Area("Dodongos Cavern Second Slingshot Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return CanUse(SLINGSHOT) || CanUse(BOW) || LogicDCSlingshotSkip;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (IsAdult && CanUse(HOOKSHOT) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED));}}),
  });

  areaTable[DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Area("Dodongos Cavern Bomb Room Upper", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_BOMB_BAG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER,       {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_FAR_BRIDGE,            {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_FAR_BRIDGE] = Area("Dodongos Cavern Far Bridge", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return CanBlastOrSmash;});},
                                                           /*Glitched*/[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));}) ||
                                                                                 CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,           {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BOSS_AREA] = Area("Dodongos Cavern Boss Area", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BACK_ROOM, {[]{return Here(DODONGOS_CAVERN_BOSS_AREA, []{return CanBlastOrSmash;});},
                                           /*Glitched*/[]{return Here(DODONGOS_CAVERN_BOSS_AREA, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED));});}}),
                  Entrance(DODONGOS_CAVERN_BOSS_ROOM, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BACK_ROOM] = Area("Dodongos Cavern Back Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_BACK_ROOM, {[]{return IsAdult || CanChildAttack;},
                                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOSS_AREA, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BOSS_ROOM] = Area("Dodongos Cavern Boss Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DodongosCavernClear, {[]{return DodongosCavernClear || (Here(DODONGOS_CAVERN_BOSS_ROOM, []{return HasExplosives || (CanUse(MEGATON_HAMMER) && CanShield);}) && (Bombs || GoronBracelet) && (IsAdult || Sticks || KokiriSword));},
                                         /*Glitched*/[]{return Here(DODONGOS_CAVERN_BOSS_ROOM, []{return HasExplosives || (CanUse(MEGATON_HAMMER) && CanShield) || (GlitchBlueFireWall && BlueFire);}) && (HasExplosives || GoronBracelet) && (IsAdult || Sticks || KokiriSword || CanUse(MEGATON_HAMMER));}}),
                }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_BOSS_ROOM_CHEST,    {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_KING_DODONGO_HEART, {[]{return DodongosCavernClear;}}),
                  LocationAccess(KING_DODONGO,                       {[]{return DodongosCavernClear;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOSS_AREA, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_ENTRYWAY,  {[]{return DodongosCavernClear;}}),
  });
  }

  if (Dungeon::JabuJabusBelly.IsVanilla()) {
  areaTable[JABU_JABUS_BELLY_BEGINNING] = Area("Jabu Jabus Belly Beginning", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY,    {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return CanUseProjectile;},
                                              /*Glitched*/[]{return CanUse(BOOMERANG) || CanUse(BOW) || CanUse(HOOKSHOT) || (CanUse(STICKS) && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT)) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_MIDDLE] = Area("Jabu Jabus Belly Lift Middle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_BEGINNING,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_NEAR_BOSS_ROOM, {[]{return HasAccessTo(JABU_JABUS_BELLY_LIFT_UPPER) || (LogicJabuBossGSAdult && IsAdult && CanUse(HOVER_BOOTS));},
                                                 /*Glitched*/[]{return (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)) &&
                                                                       GlitchJabuSwitch && (Fish || Bugs || CanUse(FARORES_WIND) || (IsAdult && ClaimCheck));}}),
  });

  areaTable[JABU_JABUS_BELLY_MAIN_UPPER] = Area("Jabu Jabus Belly Main Upper", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BIGOCTO_ROOM,    {[]{return Here(JABU_JABUS_BELLY_GREEN_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_MAIN_LOWER] = Area("Jabu Jabus Belly Main Lower", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, {[]{return HookshotOrBoomerang;},
                                                                /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, {[]{return HookshotOrBoomerang;},
                                                                /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LOWER_SIDE_ROOM,  {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_SHABOMB_CORRIDOR] = Area("Jabu Jabus Belly Shabomb Corridor", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER, {[]{return CanUseProjectile;},
                                             /*Glitched*/[]{return CanUse(BOOMERANG) || CanUse(BOW) || CanUse(HOOKSHOT) || (CanUse(STICKS) && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT));}}),
  });

  areaTable[JABU_JABUS_BELLY_LOWER_SIDE_ROOM] = Area("Jabu Jabus Belly Lower Side Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (CanUse(BOOMERANG) || CanUse(HOVER_BOOTS));},
                              /*Glitched*/[]{return (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
                }, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_LOWER] = Area("Jabu Jabus Belly Lift Lower", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_DEKU_SCRUB, {[]{return (IsChild || CanDive || LogicJabuScrubJumpDive || CanUse(IRON_BOOTS)) && CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE,      {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_FORKED_CORRIDOR] = Area("Jabu Jabus Belly Forked Corridor", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BOOMERANG_ROOM, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAP_ROOM,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_COMPASS_ROOM,   {[]{return Here(JABU_JABUS_BELLY_MAP_ROOM,      []{return CanUse(BOOMERANG);});}}),
                  Entrance(JABU_JABUS_BELLY_BLUE_TENTACLE,  {[]{return Here(JABU_JABUS_BELLY_MAP_ROOM,      []{return CanUse(BOOMERANG);});}}),
                  Entrance(JABU_JABUS_BELLY_GREEN_TENTACLE, {[]{return Here(JABU_JABUS_BELLY_BLUE_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_BOOMERANG_ROOM] = Area("Jabu Jabus Belly Boomerang Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_BOOMERANG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_MAP_ROOM] = Area("Jabu Jabus Belly Map Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MAP_CHEST, {[]{return CanUse(BOOMERANG);}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_COMPASS_ROOM] = Area("Jabu Jabus Belly Compass Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_COMPASS_CHEST, {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_BLUE_TENTACLE] = Area("Jabu Jabus Belly Blue Tentacle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(JABU_JABUS_BELLY_BLUE_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_GREEN_TENTACLE] = Area("Jabu Jabus Belly Green Tentacle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(JABU_JABUS_BELLY_GREEN_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_BIGOCTO_ROOM] = Area("Jabu Jabus Belly Bigocto Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER,    {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_ABOVE_BIGOCTO, {[]{return Here(JABU_JABUS_BELLY_BIGOCTO_ROOM, []{return (CanUse(BOOMERANG) || Nuts) && (CanUse(KOKIRI_SWORD) || CanUse(STICKS));});}}),
  });

  areaTable[JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Area("Jabu Jabus Belly Above Bigocto", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_UPPER, {[]{return CanUse(BOOMERANG);},
                                             /*Glitched*/[]{return HasBombchus && CanShield && IsAdult && (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_UPPER] = Area("Jabu Jabus Belly Lift Upper", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER,  {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Area("Jabu Jabus Belly Near Boss Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_NEAR_BOSS, {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BOSS_ROOM,   {[]{return CanUse(BOOMERANG);},
                                              /*Glitched*/[]{return (CanUse(HOVER_BOOTS) && (CanUse(BOW) || CanUse(SLINGSHOT))) || CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) ||
                                                                    (CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) || (Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[JABU_JABUS_BELLY_BOSS_ROOM] = Area("Jabu Jabus Belly Boss Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&JabuJabusBellyClear, {[]{return JabuJabusBellyClear || CanUse(BOOMERANG);}}),
                }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_BARINADE_HEART, {[]{return JabuJabusBellyClear;}}),
                  LocationAccess(BARINADE,                        {[]{return JabuJabusBellyClear;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_NEAR_BOSS_ROOM, {[]{return JabuJabusBellyClear;}}),
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY,       {[]{return JabuJabusBellyClear;}}),
  });
  }

  if (Dungeon::ForestTemple.IsVanilla()) {
  areaTable[FOREST_TEMPLE_FIRST_ROOM] = Area("Forest Temple First Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FIRST_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_GS_FIRST_ROOM,    {[]{return (IsAdult && Bombs) || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(BOOMERANG) || CanUse(SLINGSHOT) || HasBombchus || CanUse(DINS_FIRE);},
                                                      /*Glitched*/[]{return (Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_SOUTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_SOUTH_CORRIDOR] = Area("Forest Temple South Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_LOBBY,      {[]{return IsAdult || CanChildAttack || Nuts;},
                                          /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_LOBBY] = Area("Forest Temple Lobby", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleMeg, {[]{return ForestTempleMeg || (ForestTempleJoelle && ForestTempleBeth && ForestTempleAmy && CanUse(BOW));}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_GS_LOBBY, {[]{return HookshotOrBoomerang;},
                                              /*Glitched*/[]{return IsAdult && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_SOUTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NORTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return CanPlay(SongOfTime) || IsChild;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::NOVICE) || (SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                                       ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  Entrance(FOREST_TEMPLE_WEST_CORRIDOR,     {[]{return SmallKeys(FOREST_TEMPLE, 1, 5);}}),
                  Entrance(FOREST_TEMPLE_EAST_CORRIDOR,     {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(FOREST_TEMPLE_BOSS_REGION,       {[]{return ForestTempleMeg;}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM,         {[]{return false;},
                                                 /*Glitched*/[]{return IsAdult && (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)) && GlitchForestBKSkip;}}),
  });

  areaTable[FOREST_TEMPLE_NORTH_CORRIDOR] = Area("Forest Temple North Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,         {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_LOWER_STALFOS, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_LOWER_STALFOS] = Area("Forest Temple Lower Stalfos", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FIRST_STALFOS_CHEST, {[]{return IsAdult || KokiriSword;},
                                                         /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NORTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Area("Forest Temple NW Outdoors Lower", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, {[]{return CanUse(LONGSHOT) || Here(FOREST_TEMPLE_NW_OUTDOORS_UPPER, []{return HookshotOrBoomerang;});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,             {[]{return CanPlay(SongOfTime);},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::NOVICE) || (SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                                       ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (IsAdult && CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::NOVICE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(IRON_BOOTS) || HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM,         {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Area("Forest Temple NW Outdoors Upper", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_FLOORMASTER_ROOM,     {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Area("Forest Temple NE Outdoors Lower", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, {[]{return CanUse(HOOKSHOT) || HasAccessTo(FOREST_TEMPLE_FALLING_ROOM);}}),
                  LocationAccess(FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    {[]{return CanUse(HOOKSHOT) || (LogicForestOutdoorEastGS && CanUse(BOOMERANG)) || Here(FOREST_TEMPLE_FALLING_ROOM, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(DINS_FIRE) || HasExplosives;});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,             {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return CanUse(LONGSHOT);}}),
                  Entrance(FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(IRON_BOOTS) || HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,      {[]{return false;},
                                                 /*Glitched*/[]{return Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Area("Forest Temple NE Outdoors Upper", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,      {[]{return LogicForestDoorFrame && CanUse(HOVER_BOOTS) && CanUse(SCARECROW);},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::EXPERT);}}),
  });

  areaTable[FOREST_TEMPLE_MAP_ROOM] = Area("Forest Temple Map Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MAP_CHEST, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[FOREST_TEMPLE_SEWER] = Area("Forest Temple Sewer", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_WELL_CHEST, {[]{return HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Area("Forest Temple Below Boss Key Chest", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return Here(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[FOREST_TEMPLE_FLOORMASTER_ROOM] = Area("Forest Temple Floormaster Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FLOORMASTER_CHEST, {[]{return IsAdult || CanChildDamage;},
                                                       /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_WEST_CORRIDOR] = Area("Forest Temple West Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,           {[]{return SmallKeys(FOREST_TEMPLE, 1, 5);}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;},
                                               /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Area("Forest Temple Block Push Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_EYE_SWITCH_CHEST, {[]{return GoronBracelet && (CanUse(BOW) || CanUse(SLINGSHOT));},
                                                      /*Glitched*/[]{return IsAdult && (CanUse(BOW) || CanUse(SLINGSHOT)) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_WEST_CORRIDOR,            {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER,        {[]{return CanUse(HOVER_BOOTS) || (LogicForestOutsideBackdoor && IsAdult && GoronBracelet);},
                                                        /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      {[]{return IsAdult && GoronBracelet && SmallKeys(FOREST_TEMPLE, 2);},
                                                        /*Glitched*/[]{return ((IsAdult && (Bow || Hookshot || CanUse(SLINGSHOT)) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) ||
                                                                               (Bombs && GoronBracelet && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE))) && SmallKeys(FOREST_TEMPLE, 2);}}),
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, {[]{return CanUse(BOW) && GoronBracelet && SmallKeys(FOREST_TEMPLE, 2);},
                                                        /*Glitched*/[]{return ((IsAdult && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) ||
                                                                               (IsChild && Bombs && GoronBracelet && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE))) && (CanUse(BOW) || CanUse(SLINGSHOT)) && SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Area("Forest Temple NW Corridor Twisted", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
                  Entrance(FOREST_TEMPLE_RED_POE_ROOM,    {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
  });

  areaTable[FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NW Corridor Straightened", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_RED_POE_ROOM] = Area("Forest Temple Red Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoelle, {[]{return ForestTempleJoelle || CanUse(BOW);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_RED_POE_CHEST, {[]{return ForestTempleJoelle;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_TWISTED, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
                  Entrance(FOREST_TEMPLE_UPPER_STALFOS,       {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_UPPER_STALFOS] = Area("Forest Temple Upper Stalfos", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BOW_CHEST, {[]{return IsAdult || KokiriSword;},
                                               /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_RED_POE_ROOM,  {[]{return IsAdult || KokiriSword;},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FOREST_TEMPLE_BLUE_POE_ROOM, {[]{return IsAdult || KokiriSword;},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BLUE_POE_ROOM] = Area("Forest Temple Blue Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleBeth, {[]{return ForestTempleBeth || CanUse(BOW);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BLUE_POE_CHEST, {[]{return ForestTempleBeth;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_UPPER_STALFOS,            {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
  });

  areaTable[FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NE Corridor Straightened", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BLUE_POE_ROOM,   {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
                  Entrance(FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
  });

  areaTable[FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Area("Forest Temple NE Corridor Twisted", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,    {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_FROZEN_EYE_ROOM] = Area("Forest Temple Frozen Eye Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      {[]{return SmallKeys(FOREST_TEMPLE, 5) && (CanUse(BOW) || CanUse(DINS_FIRE));}}),
  });

  areaTable[FOREST_TEMPLE_FALLING_ROOM] = Area("Forest Temple Falling Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_GREEN_POE_ROOM,    {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_GREEN_POE_ROOM] = Area("Forest Temple Green Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmy, {[]{return ForestTempleAmy || CanUse(BOW);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,  {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_EAST_CORRIDOR, {[]{return ForestTempleAmy;}}),
  });

  areaTable[FOREST_TEMPLE_EAST_CORRIDOR] = Area("Forest Temple East Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,          {[]{return IsAdult || CanChildAttack || Nuts;},
                                              /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FOREST_TEMPLE_GREEN_POE_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;},
                                              /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BOSS_REGION] = Area("Forest Temple Boss Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BASEMENT_CHEST, {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_GS_BASEMENT,    {[]{return HookshotOrBoomerang;},
                                                    /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM, {[]{return BossKeyForestTemple;}}),
  });

  areaTable[FOREST_TEMPLE_BOSS_ROOM] = Area("Forest Temple Boss Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleClear, {[]{return ForestTempleClear || ((IsAdult || KokiriSword) && (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)));}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_PHANTOM_GANON_HEART, {[]{return ForestTempleClear;}}),
                  LocationAccess(PHANTOM_GANON,                     {[]{return ForestTempleClear;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY, {[]{return ForestTempleClear;}}),
  });
  }

  if (Dungeon::FireTemple.IsVanilla()) {
  areaTable[FIRE_TEMPLE_FIRST_ROOM] = Area("Fire Temple First Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_NEAR_BOSS_ROOM, {[]{return FireTimer >= 24;}}),
                  Entrance(FIRE_TEMPLE_LOOP_ENEMIES,   {[]{return Here(FIRE_TEMPLE_FIRST_ROOM, []{return CanUse(MEGATON_HAMMER);}) && (SmallKeys(FIRE_TEMPLE, 8) || !IsKeysanity);},
                                            /*Glitched*/[]{return ((IsAdult && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::EXPERT)) ||
                                                                  (GlitchFireGrunzClip && Bombs && IsAdult && CanUse(HOVER_BOOTS) && CanSurviveDamage)) && (SmallKeys(FIRE_TEMPLE, 8) || !IsKeysanity);}}),
                  Entrance(FIRE_TEMPLE_LOOP_EXIT,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM,  {[]{return SmallKeys(FIRE_TEMPLE, 2) && FireTimer >= 24;}}),
  });

  areaTable[FIRE_TEMPLE_NEAR_BOSS_ROOM] = Area("Fire Temple Near Boss Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_NEAR_BOSS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOSS_ROOM,  {[]{return BossKeyFireTemple && ((IsAdult && LogicFireBossDoorJump) || CanUse(HOVER_BOOTS) || Here(FIRE_TEMPLE_FIRE_MAZE_UPPER, []{return CanUse(MEGATON_HAMMER);}));},
                                        /*Glitched*/[]{return BossKeyFireTemple && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) ||
                                                              (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)));}}),
  });

  areaTable[FIRE_TEMPLE_BOSS_ROOM] = Area("Fire Temple Boss Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTempleClear, {[]{return FireTempleClear || (FireTimer >= 64 && CanUse(MEGATON_HAMMER));},
                                     /*Glitched*/[]{return FireTimer >= 48 && ((CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) ||
                                                           CanUse(MEGATON_HAMMER)) && Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_VOLVAGIA_HEART, {[]{return FireTempleClear;}}),
                  LocationAccess(VOLVAGIA,                   {[]{return FireTempleClear;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ENTRYWAY, {[]{return FireTempleClear;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_ENEMIES] = Area("Fire Temple Loop Enemies", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return SmallKeys(FIRE_TEMPLE, 8) || !IsKeysanity;}}),
                  Entrance(FIRE_TEMPLE_LOOP_TILES, {[]{return Here(FIRE_TEMPLE_LOOP_ENEMIES, []{return IsAdult || KokiriSword;});},
                                        /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_TILES] = Area("Fire Temple Loop Tiles", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_BOSS_KEY_LOOP, {[]{return IsAdult || CanChildAttack;},
                                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_ENEMIES,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_FLARE_DANCER] = Area("Fire Temple Loop Flare Dancer", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_FLARE_DANCER_CHEST, {[]{return (HasExplosives || CanUse(MEGATON_HAMMER)) && IsAdult;},
                                                      /*Glitched*/[]{return (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) ||
                                                                            (CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED) && HasBombchus && CanShield && (Slingshot || CanUse(BOW) || CanUse(HOOKSHOT)));}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_TILES,         {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return Here(FIRE_TEMPLE_LOOP_FLARE_DANCER, []{return (HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT)) && (IsAdult || KokiriSword || Slingshot || Boomerang);});}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_HAMMER_SWITCH] = Area("Fire Temple Loop Hammer Switch", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireLoopSwitch, {[]{return FireLoopSwitch || CanUse(MEGATON_HAMMER);},
                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE) && Bombs && CanTakeDamage && CanUse(STICKS);}}),
                }, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_GORON_ROOM,   {[]{return FireLoopSwitch;},
                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_GORON_ROOM] = Area("Fire Temple Loop Goron Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return FireLoopSwitch;}}),
                  Entrance(FIRE_TEMPLE_LOOP_EXIT,          {[]{return FireLoopSwitch;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_EXIT] = Area("Fire Temple Loop Exit", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_GORON_ROOM, {[]{return FireLoopSwitch;},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM] = Area("Fire Temple Big Lava Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM,                {[]{return SmallKeys(FIRE_TEMPLE, 2);}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES, {[]{return IsAdult && (CanPlay(SongOfTime) || LogicFireSongOfTime);},
                                                       /*Glitched*/[]{return FireTimer >= 48 && ((CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) ||
                                                                             (IsAdult && (SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) || (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && (Bugs || Fish) && CanShield)))));}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON, {[]{return IsAdult && HasExplosives;},
                                                       /*Glitched*/[]{return FireTimer >= 48 && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(FIRE_TEMPLE_FIRE_PILLAR_ROOM,          {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON] = Area("Fire Temple Big Lava Room North Goron", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES] = Area("Fire Temple Big Lava Room North Tiles", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON] = Area("Fire Temple Big Lava Room South Goron", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_PILLAR_ROOM] = Area("Fire Temple Fire Pillar Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM, {[]{return FireTimer >= 56 && SmallKeys(FIRE_TEMPLE, 4);}}),
  });

  areaTable[FIRE_TEMPLE_SHORTCUT_ROOM] = Area("Fire Temple Shortcut Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, {[]{return Here(FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});},
                                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_PILLAR_ROOM,   {[]{return SmallKeys(FIRE_TEMPLE, 4);}}),
                  Entrance(FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return Here(FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});},
                                                /*Glitched*/[]{return (GoronBracelet || LogicFireStrength) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return (GoronBracelet || (IsAdult && LogicFireStrength)) && (HasExplosives || (IsAdult && (CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(SLINGSHOT))));},
                                                /*Glitched*/[]{return (GoronBracelet || (IsAdult && LogicFireStrength)) && (CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE) ||
                                                                      (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanUse(STICKS)));}}),
  });

  areaTable[FIRE_TEMPLE_SHORTCUT_CLIMB] = Area("Fire Temple Shortcut Climb", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Area("Fire Temple Boulder Maze Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_GS_BOULDER_MAZE,          {[]{return HasExplosives && (IsAdult || Boomerang || CanUse(HOOKSHOT));},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE) && Bombs) ||
                                                                                  Here(FIRE_TEMPLE_BOULDER_MAZE_UPPER, []{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);});}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM,                {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM,            {[]{return SmallKeys(FIRE_TEMPLE, 5, 7);}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER,           {[]{return false;},
                                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE) && Bombs;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM] = Area("Fire Temple Boulder Maze Lower Side Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_EAST_CENTRAL_ROOM] = Area("Fire Temple East Central Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM,      {[]{return CanTakeDamage;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return SmallKeys(FIRE_TEMPLE, 5, 8);}}),
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return SmallKeys(FIRE_TEMPLE, 6, 8);}}),
                  Entrance(FIRE_TEMPLE_MAP_AREA,           {[]{return CanUse(SLINGSHOT) || CanUse(BOW);}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_WALL_CHASE] = Area("Fire Temple Fire Wall Chase", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM,  {[]{return FireTimer >= 24 && SmallKeys(FIRE_TEMPLE, 6, 8);}}),
                  Entrance(FIRE_TEMPLE_MAP_AREA,           {[]{return IsAdult;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return FireTimer >= 24 && IsAdult;},
                                                /*Glitched*/[]{return FireTimer >= 32 && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(FIRE_TEMPLE_CORRIDOR,           {[]{return FireTimer >= 24 && IsAdult && SmallKeys(FIRE_TEMPLE, 7);},
                                                /*Glitched*/[]{return FireTimer >= 32 && SmallKeys(FIRE_TEMPLE, 7) && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && Bombs && HasBombchus && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FIRE_TEMPLE_MAP_AREA] = Area("Fire Temple Map Area", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_UPPER] = Area("Fire Temple Boulder Maze Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return HasExplosives;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_SCARECROW_ROOM,     {[]{return CanUse(SCARECROW) || (LogicFireScarecrow && IsAdult && CanUse(LONGSHOT));}}),
  });

  areaTable[FIRE_TEMPLE_SCARECROW_ROOM] = Area("Fire Temple Scarecrow Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_SCARECROW_CLIMB, {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_PEAK,          {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_EAST_PEAK] = Area("Fire Temple East Peak", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_SCARECROW_CHEST,  {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_GS_SCARECROW_TOP, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SCARECROW_ROOM,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return CanTakeDamage;}}),
  });

  areaTable[FIRE_TEMPLE_CORRIDOR] = Area("Fire Temple Corridor", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE, {[]{return SmallKeys(FIRE_TEMPLE, 7);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,  {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_ROOM] = Area("Fire Temple Fire Maze Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_CORRIDOR,            {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,     {[]{return CanUse(HOVER_BOOTS);},
                                                 /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER,  {[]{return SmallKeys(FIRE_TEMPLE, 8);}}),
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE,      {[]{return LogicFireFlameMaze || false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE) || (CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED) && Bombs && CanShield);}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_UPPER] = Area("Fire Temple Fire Maze Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_NEAR_BOSS_ROOM,     {[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanUse(MEGATON_HAMMER);},
                                                /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM] = Area("Fire Temple Fire Maze Side Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CENTRAL_LOWER] = Area("Fire Temple West Central Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_HIGHEST_GORON_CHEST, {[]{return Here(FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return CanPlay(SongOfTime) && CanUse(MEGATON_HAMMER);});},
                                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE) || Here(FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return (IsAdult || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) ||
                                                                             (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE))) &&
                                                                             (((SongOfTime && (((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::NOVICE))) ||
                                                                             CanPlay(SongOfTime)) && ((CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE)) || CanUse(MEGATON_HAMMER)));});}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return SmallKeys(FIRE_TEMPLE, 8);}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return IsAdult && CanPlay(SongOfTime);},
                                                /*Glitched*/[]{return (IsAdult && SongOfTime && (((Bugs || Fish) && CanShield && (CanSurviveDamage || (CanTakeDamage && NumBottles >= 2)) && Bombs && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                      (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::NOVICE)))) || (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE,     {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CENTRAL_UPPER] = Area("Fire Temple West Central Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOSS_ROOM,          {[]{return false;},
                                                /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_LATE_FIRE_MAZE] = Area("Fire Temple Late Fire Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return false;},
                                                /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (Bombs && CanShield && CanDoGlitch(GlitchType::SuperSlide, GlitchDifficulty::ADVANCED));}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return HasExplosives;}}),
  });

  areaTable[FIRE_TEMPLE_UPPER_FLARE_DANCER] = Area("Fire Temple Upper Flare Dancer", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE, {[]{return Here(FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return (HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT)) && (IsAdult || KokiriSword || Slingshot || Boomerang);});}}),
                  Entrance(FIRE_TEMPLE_WEST_CLIMB,     {[]{return Here(FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return (HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT)) && (IsAdult || KokiriSword || Slingshot || Boomerang);});}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CLIMB] = Area("Fire Temple West Climb", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_PEAK,          {[]{return CanUseProjectile;},
                                                /*Glitched*/[]{return CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[FIRE_TEMPLE_WEST_PEAK] = Area("Fire Temple West Peak", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MEGATON_HAMMER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanTakeDamage;}}),
                  Entrance(FIRE_TEMPLE_WEST_CLIMB,         {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_HAMMER_RETURN_PATH] = Area("Fire Temple Hammer Return Path", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ABOVE_FIRE_MAZE, {[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Area("Fire Temple Above Fire Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return CanUse(MEGATON_HAMMER);}}),
  });
  }

  if (Dungeon::WaterTemple.IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[WATER_TEMPLE_LOBBY] = Area("Water Temple Lobby", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(WATER_TEMPLE_EAST_LOWER,            {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && (CanUse(IRON_BOOTS) || (CanUse(LONGSHOT) && LogicWaterTempleTorchLongshot)));},
                                                    /*Glitched*/[]{return ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) &&
                                                                          ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(WATER_TEMPLE_NORTH_LOWER,           {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && CanUse(IRON_BOOTS));}}),
                  Entrance(WATER_TEMPLE_SOUTH_LOWER,           {[]{return WaterTempleLow && HasExplosives && (CanDive || CanUse(IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC));},
                                                    /*Glitched*/[]{return CanUse(IRON_BOOTS) && (WaterTempleMiddle || WaterTempleHigh) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && AdultCanAccess(WATER_TEMPLE_WEST_LOWER) && CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(WATER_TEMPLE_WEST_LOWER,            {[]{return WaterTempleLow && GoronBracelet && (IsChild || CanDive || CanUse(IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC));}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return WaterTempleLow && SmallKeys(WATER_TEMPLE, 5);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (WaterTempleLow || WaterTempleMiddle) && (HasFireSourceWithTorch || CanUse(BOW));}}),
                  Entrance(WATER_TEMPLE_EAST_MIDDLE,           {[]{return (WaterTempleLow || WaterTempleMiddle || (CanUse(IRON_BOOTS) && WaterTimer >= 16)) && CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_WEST_MIDDLE,           {[]{return WaterTempleMiddle;},
                                                    /*Glitched*/[]{return WaterTempleLow && (CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(WATER_TEMPLE_HIGH_WATER,            {[]{return IsAdult && (CanUse(HOVER_BOOTS) || (LogicWaterTempleUpperBoost && Bombs && CanTakeDamage));},
                                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (WaterTempleLow || WaterTempleMiddle) && (CanUse(SLINGSHOT) || CanUse(BOW)) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS) || (LogicWaterCentralBow && (IsAdult || WaterTempleMiddle)));},
                                                    /*Glitched*/[]{return (WaterTempleLow || WaterTempleMiddle) && IsAdult && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return WaterTempleHigh && SmallKeys(WATER_TEMPLE, 4);},
                                                    /*Glitched*/[]{return SmallKeys(WATER_TEMPLE, 4) && (CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE));}}),
                  Entrance(WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return WaterTempleHigh && CanUse(LONGSHOT);},
                                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::NOVICE) ||
                                                                          (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
  });

  areaTable[WATER_TEMPLE_EAST_LOWER] = Area("Water Temple East Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleLow, {[]{return WaterTempleLow || CanPlay(ZeldasLullaby);},
                                    /*Glitched*/[]{return ZeldasLullaby && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield &&
                                                          ((Bombs && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)))));}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,        {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && CanUse(IRON_BOOTS));},
                                           /*Glitched*/[]{return ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) &&
                                                                 ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(WATER_TEMPLE_MAP_ROOM,     {[]{return WaterTempleHigh;}}),
                  Entrance(WATER_TEMPLE_CRACKED_WALL, {[]{return WaterTempleMiddle || (WaterTempleHigh && WaterTempleLow && ((CanUse(HOVER_BOOTS) && LogicWaterCrackedWallHovers) || LogicWaterCrackedWallNothing));},
                                           /*Glitched*/[]{return WaterTempleHigh && WaterTempleLow && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  Entrance(WATER_TEMPLE_TORCH_ROOM,   {[]{return WaterTempleLow && (HasFireSourceWithTorch || CanUse(BOW));}}),
  });

  areaTable[WATER_TEMPLE_MAP_ROOM] = Area("Water Temple Map Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MAP_CHEST, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_CRACKED_WALL] = Area("Water Temple Cracked Wall", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CRACKED_WALL_CHEST, {[]{return HasExplosives;},
                                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_TORCH_ROOM] = Area("Water Temple Torch Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_TORCHES_CHEST, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_NORTH_LOWER] = Area("Water Temple North Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,          {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return (CanUse(LONGSHOT) || (LogicWaterBossKeyRegion && CanUse(HOVER_BOOTS))) && SmallKeys(WATER_TEMPLE, 4);},
                                             /*Glitched*/[]{return ((Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) ||
                                                                   CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE)) && SmallKeys(WATER_TEMPLE, 4);}}),
  });

  areaTable[WATER_TEMPLE_BOULDERS_LOWER] = Area("Water Temple Boulders Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, {[]{return CanUse(LONGSHOT) || Here(WATER_TEMPLE_BOULDERS_UPPER, []{return (IsAdult && HookshotOrBoomerang) || (CanUse(IRON_BOOTS) && CanUse(HOOKSHOT));});}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_NORTH_LOWER,    {[]{return SmallKeys(WATER_TEMPLE, 4);}}),
                  Entrance(WATER_TEMPLE_BLOCK_ROOM,     {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return (IsAdult && (CanUse(HOVER_BOOTS) || LogicWaterNorthBasementLedgeJump)) || (CanUse(HOVER_BOOTS) && CanUse(IRON_BOOTS));}}),
  });

  areaTable[WATER_TEMPLE_BLOCK_ROOM] = Area("Water Temple Block Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return (GoronBracelet && HasExplosives) || CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_JETS_ROOM,      {[]{return (GoronBracelet && HasExplosives) || (CanUse(HOOKSHOT) && CanUse(HOVER_BOOTS));},
                                             /*Glitched*/[]{return CanUse(HOOKSHOT) && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[WATER_TEMPLE_JETS_ROOM] = Area("Water Temple Jets Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BLOCK_ROOM,     {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_BOULDERS_UPPER] = Area("Water Temple Boulders Upper", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return true;}}),
                  Entrance(WATER_TEMPLE_JETS_ROOM,      {[]{return IsAdult;}}),
                  Entrance(WATER_TEMPLE_BOSS_KEY_ROOM,  {[]{return (CanUse(IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive)) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_BOSS_KEY_ROOM] = Area("Water Temple Boss Key Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return (CanUse(IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive) || IsChild || CanDive) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_SOUTH_LOWER] = Area("Water Temple South Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_BEHIND_GATE, {[]{return CanUse(HOOKSHOT) || (IsAdult && CanUse(HOVER_BOOTS));},
                                                   /*Glitched*/[]{return (CanUse(BOOMERANG) && CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE)) ||
                                                                         (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_WEST_LOWER] = Area("Water Temple West Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,       {[]{return CanUse(HOOKSHOT) && CanUse(IRON_BOOTS) && GoronBracelet;},
                                          /*Glitched*/[]{return CanUse(IRON_BOOTS) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(WATER_TEMPLE_DRAGON_ROOM, {[]{return IsAdult || CanChildAttack;}}),
  });

  areaTable[WATER_TEMPLE_DRAGON_ROOM] = Area("Water Temple Dragon Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_DRAGON_CHEST, {[]{return (CanUse(HOOKSHOT) && CanUse(IRON_BOOTS)) || (IsAdult && LogicWaterDragonAdult && HasBombchus && (CanDive || CanUse(IRON_BOOTS))) ||
                                                                       Here(WATER_TEMPLE_RIVER, []{return IsAdult && CanUse(BOW) && ((LogicWaterDragonAdult && (CanDive || CanUse(IRON_BOOTS))) || LogicWaterDragonJumpDive);});},
                                                 /*Glitched*/[]{return Bombs && ((IsAdult && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::ADVANCED)) || (CanUse(IRON_BOOTS) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_WEST_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_LOWER] = Area("Water Temple Central Pillar Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,                   {[]{return SmallKeys(WATER_TEMPLE, 5);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_UPPER,    {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return WaterTempleMiddle && CanUse(IRON_BOOTS) && WaterTimer >= 40;},
                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::NOVICE) && CanUse(IRON_BOOTS) && WaterTimer >= 40;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Area("Water Temple Central Pillar Upper", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleMiddle, {[]{return WaterTempleMiddle || CanPlay(ZeldasLullaby);},
                                       /*Glitched*/[]{return ZeldasLullaby && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield &&
                                                             (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)))));}}),
                }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_CENTRAL_PILLAR, {[]{return CanUse(LONGSHOT) || (LogicWaterCentralGSFW && WaterTempleHigh && CanUse(FARORES_WIND) && HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,                {[]{return true;}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT] = Area("Water Temple Central Pillar Basement", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CENTRAL_PILLAR_CHEST, {[]{return CanUse(HOOKSHOT) && CanUse(IRON_BOOTS) && WaterTimer >= 40;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return CanUse(IRON_BOOTS) && WaterTimer >= 16;}}),
  });

  areaTable[WATER_TEMPLE_EAST_MIDDLE] = Area("Water Temple East Middle", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_COMPASS_CHEST, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_WEST_MIDDLE] = Area("Water Temple West Middle", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,      {[]{return true;}}),
                  Entrance(WATER_TEMPLE_HIGH_WATER, {[]{return CanUseProjectile;}}),
  });

  areaTable[WATER_TEMPLE_HIGH_WATER] = Area("Water Temple High Water", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleHigh, {[]{return WaterTempleHigh || CanPlay(ZeldasLullaby);},
                                     /*Glitched*/[]{return ZeldasLullaby && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield &&
                                                           (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)))));}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_BLOCK_CORRIDOR] = Area("Water Temple Block Corridor", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, {[]{return GoronBracelet && (WaterTempleLow || WaterTempleMiddle);},
                                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::INTERMEDIATE) && (WaterTempleLow || WaterTempleMiddle);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_FALLING_PLATFORM_ROOM] = Area("Water Temple Falling Platform Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, {[]{return CanUse(LONGSHOT) || (LogicWaterFallingPlatformGS && IsAdult && HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,               {[]{return CanUse(HOOKSHOT) && SmallKeys(WATER_TEMPLE, 4);}}),
                  Entrance(WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(HOOKSHOT) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_DRAGON_PILLARS_ROOM] = Area("Water Temple Dragon Pillars Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return CanUseProjectile;}}),
                  Entrance(WATER_TEMPLE_DARK_LINK_ROOM,        {[]{return CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_DARK_LINK_ROOM] = Area("Water Temple Dark Link Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  Entrance(WATER_TEMPLE_LONGSHOT_ROOM,       {[]{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
  });

  areaTable[WATER_TEMPLE_LONGSHOT_ROOM] = Area("Water Temple Longshot Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_LONGSHOT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_DARK_LINK_ROOM, {[]{return true;}}),
                  Entrance(WATER_TEMPLE_RIVER,          {[]{return IsChild || CanPlay(SongOfTime);},
                                             /*Glitched*/[]{return SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield &&
                                                                   (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)))));}}),
  });

  areaTable[WATER_TEMPLE_RIVER] = Area("Water Temple River", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_RIVER_CHEST, {[]{return (CanUse(SLINGSHOT) || CanUse(BOW)) && (IsAdult || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
                  LocationAccess(WATER_TEMPLE_GS_RIVER,    {[]{return (CanUse(IRON_BOOTS) && CanUse(HOOKSHOT)) || (LogicWaterRiverGS && CanUse(LONGSHOT));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_DRAGON_ROOM, {[]{return (CanUse(SLINGSHOT) || CanUse(BOW)) && (IsAdult || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
  });

  areaTable[WATER_TEMPLE_PRE_BOSS_ROOM] = Area("Water Temple Pre Boss Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOSS_ROOM, {[]{return BossKeyWaterTemple;}}),
  });

  areaTable[WATER_TEMPLE_BOSS_ROOM] = Area("Water Temple Boss Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleClear, {[]{return WaterTempleClear || (CanUse(HOOKSHOT) && (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)));}}),
                }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MORPHA_HEART, {[]{return WaterTempleClear;}}),
                  LocationAccess(MORPHA,                    {[]{return WaterTempleClear;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_ENTRYWAY, {[]{return WaterTempleClear;}}),
  });
  }

  if (Dungeon::SpiritTemple.IsVanilla()) {
  areaTable[SPIRIT_TEMPLE_LOBBY] = Area("Spirit Temple Lobby", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY,    {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_CHILD,       {[]{return IsChild;}}),
                  Entrance(SPIRIT_TEMPLE_EARLY_ADULT, {[]{return CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[SPIRIT_TEMPLE_CHILD] = Area("Child Spirit Temple", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutCrate, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,        {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                  LocationAccess(SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot))) && (Sticks || CanUse(DINS_FIRE));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_METAL_FENCE,            {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CHILD_CLIMB, {[]{return SmallKeys(SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[SPIRIT_TEMPLE_CHILD_CLIMB] = Area("Child Spirit Temple Climb", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    {[]{return HasProjectile(HasProjectileAge::Both) || CanUse(DINS_FIRE) ||
                                                                                      (CanTakeDamage && (Sticks || KokiriSword || HasProjectile(HasProjectileAge::Child))) ||
                                                                                        (IsChild && SmallKeys(SPIRIT_TEMPLE, 5) && HasProjectile(HasProjectileAge::Child)) ||
                                                                                          ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && (HasProjectile(HasProjectileAge::Adult) || CanTakeDamage));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return HasExplosives;}}),
  });

  areaTable[SPIRIT_TEMPLE_EARLY_ADULT] = Area("Early Adult Spirit Temple", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_COMPASS_CHEST,            {[]{return CanUse(HOOKSHOT) && CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,  {[]{return Bow || Hookshot || HasBombchus || (Bombs && LogicSpiritLowerAdultSwitch);}}),
                  LocationAccess(SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  {[]{return SmallKeys(SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, {[]{return SmallKeys(SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_BOULDER_ROOM,          {[]{return CanPlay(SongOfTime) && (Bow || Hookshot || HasBombchus || (Bombs && LogicSpiritLowerAdultSwitch));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return SmallKeys(SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[SPIRIT_TEMPLE_CENTRAL_CHAMBER] = Area("Spirit Temple Central Chamber", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MAP_CHEST,                    {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&

                                                                                                (CanUse(DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || LogicSpiritMapChest) && Bow && Sticks))) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(STICKS)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(FIRE_ARROWS) || (LogicSpiritMapChest && Bow)) &&
                                                                                                CanUse(SILVER_GAUNTLETS));}}),
                  LocationAccess(SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,         {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&
                                                                                                (CanUse(DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || LogicSpiritSunChest) && Bow && Sticks))) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(STICKS)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(FIRE_ARROWS) || (LogicSpiritSunChest && Bow)) &&
                                                                                                CanUse(SILVER_GAUNTLETS));}}),
                  LocationAccess(SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,       {[]{return SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,  {[]{return SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && CanPlay(ZeldasLullaby) && (Hookshot || HoverBoots);}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, {[]{return (HasExplosives && Boomerang && Hookshot) ||
                                                                                            (CanUse(BOOMERANG) && SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives) ||
                                                                                            (Hookshot && CanUse(SILVER_GAUNTLETS) &&
                                                                                                (SmallKeys(SPIRIT_TEMPLE, 3) ||
                                                                                                    (SmallKeys(SPIRIT_TEMPLE, 2) && Boomerang && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_LOBBY,                     {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&
                                                                                                LogicSpiritLobbyGS && Boomerang && (Hookshot || HoverBoots)) ||
                                                                                            (LogicSpiritLobbyGS && SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives && CanUse(BOOMERANG)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && (Hookshot || HoverBoots));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_OUTDOOR_HANDS,              {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR, {[]{return SmallKeys(SPIRIT_TEMPLE, 4) && CanUse(SILVER_GAUNTLETS);}}),
                  Entrance(SPIRIT_TEMPLE_CHILD_CLIMB,                {[]{return true;}}),
  });

  areaTable[SPIRIT_TEMPLE_OUTDOOR_HANDS] = Area("Spirit Temple Outdoor Hands", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return (SmallKeys(SPIRIT_TEMPLE, 3) && Longshot && HasExplosives) || SmallKeys(SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,    {[]{return SmallKeys(SPIRIT_TEMPLE, 4) && CanUse(SILVER_GAUNTLETS) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return (IsChild && SmallKeys(SPIRIT_TEMPLE, 5)) || (CanUse(SILVER_GAUNTLETS) && ((SmallKeys(SPIRIT_TEMPLE, 3) && HasExplosives) || SmallKeys(SPIRIT_TEMPLE, 5)));}}),
  });

  areaTable[SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Central Locked Door", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,         {[]{return MirrorShield && HasExplosives;}}),
                  LocationAccess(SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  {[]{return (LogicLensSpirit || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, {[]{return (LogicLensSpirit || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR, {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && (LogicSpiritWall || CanUse(LONGSHOT) || HasBombchus || ((Bombs || Nuts || CanUse(DINS_FIRE)) && (Bow || CanUse(HOOKSHOT) || Hammer)));}}),
  });

  areaTable[SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Final Locked Door", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTempleClear, {[]{return SpiritTempleClear || (MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_BOSS_KEY_CHEST, {[]{return CanPlay(ZeldasLullaby) && ((CanTakeDamage && LogicFlamingChests) || (Bow && Hookshot));}}),
                  LocationAccess(SPIRIT_TEMPLE_TOPMOST_CHEST,  {[]{return MirrorShield;}}),
                  LocationAccess(SPIRIT_TEMPLE_TWINROVA_HEART, {[]{return MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple;}}),
                  LocationAccess(TWINROVA,                     {[]{return MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple;}}),
  }, {});
  }

  if (Dungeon::ShadowTemple.IsVanilla()) {
  areaTable[SHADOW_TEMPLE_BEGINNING] = Area("Shadow Temple Beginning", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MAP_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_HOVER_BOOTS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_FIRST_BEAMOS, {[]{return HoverBoots;},
                                            /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[SHADOW_TEMPLE_FIRST_BEAMOS] = Area("Shadow Temple First Beamos", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}), //This fairy pot is only on 3DS
                }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_COMPASS_CHEST,            {[]{return IsAdult || KokiriSword || Sticks;}}),
                  LocationAccess(SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, {[]{return CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT);}}),
                  LocationAccess(SHADOW_TEMPLE_GS_NEAR_SHIP,             {[]{return false;},
                                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && Longshot;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_HUGE_PIT,    {[]{return HasExplosives && IsAdult && SmallKeys(SHADOW_TEMPLE, 1, 2);}}),
                  Entrance(SHADOW_TEMPLE_BEYOND_BOAT, {[]{return false;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && Longshot && CanPlay(ZeldasLullaby);}}),
  });

  areaTable[SHADOW_TEMPLE_HUGE_PIT] = Area("Shadow Temple Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,       {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,       {[]{return LogicShadowUmbrella || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,      {[]{return LogicShadowUmbrella || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,           {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_FREESTANDING_KEY,                 {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot && (Bombs || GoronBracelet || (LogicShadowFreestandingKey && HasBombchus));}}),
                  LocationAccess(SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,           {[]{return Hookshot;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,              {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_WIND_TUNNEL, {[]{return (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot && SmallKeys(SHADOW_TEMPLE, 3, 4);}}),
  });

  areaTable[SHADOW_TEMPLE_WIND_TUNNEL] = Area("Shadow Temple Wind Tunnel", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_NEAR_SHIP,            {[]{return CanUse(LONGSHOT) && SmallKeys(SHADOW_TEMPLE, 4, 5);}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_BEYOND_BOAT, {[]{return CanPlay(ZeldasLullaby) && SmallKeys(SHADOW_TEMPLE, 4, 5);}}),
  });

  areaTable[SHADOW_TEMPLE_BEYOND_BOAT] = Area("Shadow Temple Beyond Boat", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTempleClear, {[]{return ShadowTempleClear || (SmallKeys(SHADOW_TEMPLE, 5) && BossKeyShadowTemple && (Bow || CanUse(DISTANT_SCARECROW) || (LogicShadowStatue && HasBombchus)));}}),
  }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,      {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(SHADOW_TEMPLE_BOSS_KEY_CHEST,              {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_BONGO_BONGO_HEART,           {[]{return SmallKeys(SHADOW_TEMPLE, 5) && BossKeyShadowTemple && (Bow || CanUse(DISTANT_SCARECROW) || (LogicShadowStatue && HasBombchus));}}),
                  LocationAccess(BONGO_BONGO,                               {[]{return SmallKeys(SHADOW_TEMPLE, 5) && BossKeyShadowTemple && (Bow || CanUse(DISTANT_SCARECROW) || (LogicShadowStatue && HasBombchus));}}),
                  LocationAccess(SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,         {[]{return true;}}),
  }, {});
  }

  if (Dungeon::BottomOfTheWell.IsVanilla()) {
  areaTable[BOTTOM_OF_THE_WELL_MAIN_AREA] = Area("Bottom of the Well Main Area", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&StickPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  {[]{return HasExplosives;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_COMPASS_CHEST,                {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,       {[]{return LogicLensBotw || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     {[]{return (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FREESTANDING_KEY,             {[]{return Sticks || CanUse(DINS_FIRE);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,          {[]{return CanPlay(ZeldasLullaby) && (KokiriSword || (Sticks && LogicChildDeadhand));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,              {[]{return CanPlay(ZeldasLullaby) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MAP_CHEST,                    {[]{return HasExplosives || (((SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH))) || CanUse(DINS_FIRE)) && GoronBracelet);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,             {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,              {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,           {[]{return Boomerang && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && SmallKeys(BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,           {[]{return Boomerang && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && SmallKeys(BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,            {[]{return SmallKeys(BOTTOM_OF_THE_WELL, 3) && (LogicLensBotw || CanUse(LENS_OF_TRUTH)) && Boomerang;}}),
                }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return true;}}),
  });
  }

  if (Dungeon::IceCavern.IsVanilla()) {
  areaTable[ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(ICE_CAVERN_MAIN,     {[]{return Here(ICE_CAVERN_BEGINNING, []{return IsAdult || HasExplosives || CanUse(DINS_FIRE);});}}),
  });

  areaTable[ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess, {[]{return BlueFireAccess || (IsAdult && HasBottle);}}),
                }, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MAP_CHEST,               {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(ICE_CAVERN_COMPASS_CHEST,           {[]{return BlueFire;}}),
                  LocationAccess(ICE_CAVERN_IRON_BOOTS_CHEST,        {[]{return BlueFire && (IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE));}}),
                  LocationAccess(SHEIK_IN_ICE_CAVERN,                {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(ICE_CAVERN_FREESTANDING_POH,        {[]{return BlueFire;}}),
                  LocationAccess(ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, {[]{return HookshotOrBoomerang;}}),
                  LocationAccess(ICE_CAVERN_GS_HEART_PIECE_ROOM,     {[]{return BlueFire && HookshotOrBoomerang;}}),
                  LocationAccess(ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return BlueFire && HookshotOrBoomerang;}}),
  }, {});
  }

  if (Dungeon::GerudoTrainingGrounds.IsVanilla()) {
  areaTable[GERUDO_TRAINING_GROUNDS_LOBBY] = Area("Gerudo Training Grounds Lobby", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_LOBBY_LEFT_CHEST,  {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_LOBBY_RIGHT_CHEST, {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_STALFOS_CHEST,     {[]{return IsAdult || KokiriSword;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_BEAMOS_CHEST,      {[]{return HasExplosives && (IsAdult || KokiriSword);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY,         {[]{return true;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM, {[]{return (IsAdult || KokiriSword) && (CanUse(HOOKSHOT) || LogicGtgWithoutHookshot);}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return Here(GERUDO_TRAINING_GROUNDS_LOBBY, []{return (IsAdult || KokiriSword) && HasExplosives;});}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,     {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE] = Area("Gerudo Training Grounds Central Maze", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HIDDEN_CEILING_CHEST,   {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 3) && (LogicLensGtg || CanUse(LENS_OF_TRUTH));},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && (LogicLensGtg || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_FIRST_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 4);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_SECOND_CHEST, {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 6);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 7);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_FINAL_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 9);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 9);},
                                                            /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds Central Maze Right", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_FREESTANDING_KEY,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,   {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_LAVA_ROOM] = Area("Gerudo Training Grounds Lava Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return CanUse(HOOKSHOT) && CanPlay(SongOfTime) && IronBoots && WaterTimer >= 24;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return CanPlay(SongOfTime) || IsChild;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,        {[]{return CanUse(LONGSHOT)  || (CanUse(HOVER_BOOTS) && CanUse(HOOKSHOT));}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM] = Area("Gerudo Training Grounds Hammer Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_CLEAR_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_SWITCH_CHEST, {[]{return CanUse(MEGATON_HAMMER) || (CanTakeDamage && LogicFlamingChests);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return CanUse(MEGATON_HAMMER) && Bow;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER] = Area("Gerudo Training Grounds Eye Statue Lower", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_EYE_STATUE_CHEST, {[]{return CanUse(BOW);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER] = Area("Gerudo Training Grounds Eye Statue Upper", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_NEAR_SCARECROW_CHEST, {[]{return CanUse(BOW);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM] = Area("Gerudo Training Grounds Heavy Block Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_BEFORE_HEAVY_BLOCK_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER, {[]{return (LogicLensGtg || CanUse(LENS_OF_TRUTH)) && (CanUse(HOOKSHOT) || (LogicGtgFakeWall && CanUse(HOVER_BOOTS)));}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,   {[]{return (LogicLensGtg || CanUse(LENS_OF_TRUTH)) && (CanUse(HOOKSHOT) || (LogicGtgFakeWall && CanUse(HOVER_BOOTS))) && CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM] = Area("Gerudo Training Grounds Like Like Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_THIRD_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FOURTH_CHEST, {[]{return true;}}),
  }, {});
  }

  if (Dungeon::GanonsCastle.IsVanilla()) {
  areaTable[GANONS_CASTLE_LOBBY] = Area("Ganon's Castle Lobby", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(GANONS_CASTLE_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(GANONS_CASTLE_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(GANONS_CASTLE_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_LIGHT_TRIAL,  {[]{return CanUse(GOLDEN_GAUNTLETS);}}),
                  Entrance(GANONS_CASTLE_TOWER,        {[]{return (ForestTrialClear || Trial::ForestTrial.IsSkipped()) &&
                                                                         (FireTrialClear   || Trial::FireTrial.IsSkipped())   &&
                                                                         (WaterTrialClear  || Trial::WaterTrial.IsSkipped())  &&
                                                                         (ShadowTrialClear || Trial::ShadowTrial.IsSkipped()) &&
                                                                         (SpiritTrialClear || Trial::SpiritTrial.IsSkipped()) &&
                                                                         (LightTrialClear  || Trial::LightTrial.IsSkipped());}}),
                  Entrance(GANONS_CASTLE_DEKU_SCRUBS,  {[]{return LogicLensCastle || CanUse(LENS_OF_TRUTH);}}),
  });

  areaTable[GANONS_CASTLE_DEKU_SCRUBS] = Area("Ganon's Castle Deku Scrubs", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
  }, {});

  areaTable[GANONS_CASTLE_FOREST_TRIAL] = Area("Ganon's Castle Forest Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return CanUse(LIGHT_ARROWS) && (FireArrows || DinsFire);}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_FOREST_TRIAL_CHEST, {[]{return IsAdult || CanChildDamage;}}),
  }, {});

  areaTable[GANONS_CASTLE_FIRE_TRIAL] = Area("Ganon's Castle Fire Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && CanUse(LONGSHOT);}}),
  }, {}, {});

  areaTable[GANONS_CASTLE_WATER_TRIAL] = Area("Ganon's Castle Water Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
                  EventAccess(&FairyPot,        {[]{return FairyPot || BlueFire;}}),
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && Hammer && CanUse(LIGHT_ARROWS);}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, {[]{return true;}}),
  }, {});

  areaTable[GANONS_CASTLE_SHADOW_TRIAL] = Area("Ganon's Castle Shadow Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return CanUse(LIGHT_ARROWS) && Hammer && ((FireArrows && (LogicLensCastle || CanUse(LENS_OF_TRUTH))) || (CanUse(LONGSHOT) && (CanUse(HOVER_BOOTS) || (DinsFire && (LogicLensCastle || CanUse(LENS_OF_TRUTH))))));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,            {[]{return CanUse(FIRE_ARROWS) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || CanPlay(SongOfTime) || IsChild;}}),
                  LocationAccess(GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, {[]{return CanUse(FIRE_ARROWS) || (CanUse(LONGSHOT) && (CanUse(HOVER_BOOTS) || CanUse(DINS_FIRE)));}}),
  }, {});

  areaTable[GANONS_CASTLE_SPIRIT_TRIAL] = Area("Ganon's Castle Spirit Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,           {[]{return NutPot || ((LogicSpiritTrialHookshot || CanUse(HOOKSHOT)) && HasBombchus && Bow && MirrorShield && IsAdult);}}),
                  EventAccess(&SpiritTrialClear, {[]{return CanUse(LIGHT_ARROWS)  && MirrorShield && HasBombchus && (LogicSpiritTrialHookshot || CanUse(HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, {[]{return LogicSpiritTrialHookshot || CanUse(HOOKSHOT);}}),
                  LocationAccess(GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      {[]{return (LogicSpiritTrialHookshot || CanUse(HOOKSHOT)) && HasBombchus && (LogicLensCastle || CanUse(LENS_OF_TRUTH));}}),
  }, {});

  areaTable[GANONS_CASTLE_LIGHT_TRIAL] = Area("Ganon's Castle Light Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return CanUse(LIGHT_ARROWS) && CanUse(HOOKSHOT) && SmallKeys(GANONS_CASTLE, 2) && (LogicLensCastle || CanUse(LENS_OF_TRUTH));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, {[]{return LogicLensCastle || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,           {[]{return CanPlay(ZeldasLullaby) && SmallKeys(GANONS_CASTLE, 1);}}),
  }, {});
  }

  areaTable[GANONS_CASTLE_TOWER] = Area("Ganon's Castle Tower", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GANONS_TOWER_BOSS_KEY_CHEST, {[]{return true;}}),
                  LocationAccess(GANONDORF_HINT,              {[]{return BossKeyGanonsCastle;}}),
                  LocationAccess(GANON,                       {[]{return BossKeyGanonsCastle && CanUse(LIGHT_ARROWS);}}),
  }, {});

  /*---------------------------
  |   MASTER QUEST DUNGEONS   |
  ---------------------------*/
  if (Dungeon::DekuTree.IsMQ()) {
  areaTable[DEKU_TREE_MQ_LOBBY] = Area("Deku Tree MQ Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(DEKU_TREE_MQ_SLINGSHOT_CHEST,           {[]{return IsAdult || CanChildAttack;}}),
                  LocationAccess(DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));}}),
                  LocationAccess(DEKU_TREE_MQ_BASEMENT_CHEST,            {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));}}),
                  LocationAccess(DEKU_TREE_MQ_GS_LOBBY,                  {[]{return IsAdult || CanChildAttack;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY,                     {[]{return true;}}),
                  Entrance(DEKU_TREE_MQ_COMPASS_ROOM,              {[]{return Here(DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW));}) &&
                                                                               Here(DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW));}) &&
                                                                               Here(DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch;});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_LEDGE,            {[]{return LogicDekuB1Skip || Here(DEKU_TREE_MQ_LOBBY, []{return IsAdult;});}}),
  });

  areaTable[DEKU_TREE_MQ_COMPASS_ROOM] = Area("Deku Tree MQ Compass Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_COMPASS_CHEST,   {[]{return true;}}),
                  LocationAccess(DEKU_TREE_MQ_GS_COMPASS_ROOM, {[]{return HookshotOrBoomerang &&
                                                                               Here(DEKU_TREE_MQ_COMPASS_ROOM, []{return HasBombchus ||
                                                                               (Bombs && (CanPlay(SongOfTime) || IsAdult)) ||
                                                                               (IsAdult && CanUse(MEGATON_HAMMER) && (CanPlay(SongOfTime) /*|| LogicDekuMQCompassGS*/));});}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_LOBBY, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return /*LogicDekuMQLog || */ (IsChild && (DekuShield || HylianShield)) ||
                                                                             (IsAdult && (CanUse(LONGSHOT) || (CanUse(HOOKSHOT) && CanUse(IRON_BOOTS))));}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,                    {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, {[]{return CanPlay(SongOfTime);}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_BACK_ROOM,        {[]{return Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return (IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) ||
                                                                               Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return IsAdult && CanUse(FIRE_ARROWS);});}) &&
                                                                                 Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return IsAdult || KokiriSword || CanUseProjectile || (Nuts && Sticks);});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Area("Deku Tree MQ Basement Back Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, {[]{return (IsAdult && CanUse(LONGSHOT)) || (CanPlay(SongOfTime) && HookshotOrBoomerang);}}),
                  LocationAccess(DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,   {[]{return HasFireSourceWithTorch && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_LEDGE,           {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return (IsChild && CanUse(KOKIRI_SWORD)) || CanUseProjectile || (Nuts && (IsChild && CanUse(STICKS)));}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_LEDGE] = Area("Deku Tree MQ Basement Ledge", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuTreeClear, {[]{return DekuTreeClear || (Here(DEKU_TREE_MQ_BASEMENT_LEDGE, []{return HasFireSourceWithTorch;}) &&
                                                                           Here(DEKU_TREE_MQ_BASEMENT_LEDGE, []{return HasShield;})  &&
                                                                           (IsAdult || KokiriSword || Sticks) && (Nuts || CanUse(SLINGSHOT) || CanUse(BOW) || HookshotOrBoomerang));}}),
  }, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_DEKU_SCRUB,     {[]{return CanStunDeku;}}),
                  LocationAccess(DEKU_TREE_QUEEN_GOHMA_HEART, {[]{return HasFireSourceWithTorch && HasShield && (IsAdult || KokiriSword || Sticks) && (Nuts || CanUse(SLINGSHOT) || CanUse(BOW) || HookshotOrBoomerang);}}),
                  LocationAccess(QUEEN_GOHMA,                 {[]{return HasFireSourceWithTorch && HasShield && (IsAdult || KokiriSword || Sticks) && (Nuts || CanUse(SLINGSHOT) || CanUse(BOW) || HookshotOrBoomerang);}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_BACK_ROOM, {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,              {[]{return true;}}),
  });
  }

  if (Dungeon::DodongosCavern.IsMQ()) {
  areaTable[DODONGOS_CAVERN_MQ_BEGINNING] = Area("Dodongos Cavern MQ Beginning", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_MQ_LOBBY, {[]{return Here(DODONGOS_CAVERN_MQ_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_LOBBY] = Area("Dodongos Cavern MQ Lobby", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,   {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_MAP_CHEST,                  {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_COMPASS_CHEST,              {[]{return IsAdult || CanChildAttack || Nuts;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,          {[]{return (IsChild && CanUse(STICKS)) || HasFireSource;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,    {[]{return CanBlastOrSmash || (IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) || (IsAdult && (LogicDCJump || HoverBoots || Hookshot));}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, {[]{return CanPlay(SongOfTime) && (CanChildAttack || IsAdult);}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,             {[]{return (IsChild && CanUse(STICKS)) || HasFireSource;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,           {[]{return CanBlastOrSmash;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,      {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,     {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,       {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_GOSSIP_STONE,                  {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  {[]{return Here(DODONGOS_CAVERN_MQ_LOBBY, []{return CanBlastOrSmash || (((IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE)) && CanTakeDamage);});}}),
                  Entrance(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA,     {[]{return IsAdult || (Here(DODONGOS_CAVERN_MQ_LOBBY, []{return IsAdult;}) && HasExplosives);}}),
                    //Trick: IsAdult || HasExplosives || (LogicDCMQChildBombs && (KokiriSword || Sticks) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  Entrance(DODONGOS_CAVERN_MQ_BOSS_AREA,         {[]{return HasExplosives;}}),
                    //Trick: HasExplosives || (LogicDCMQEyes && GoronBracelet && (IsAdult || LogicDCMQChildBack) && ((IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) || (IsAdult && (LogicDCJump || Hammer || HoverBoots || Hookshot))))
  });

  areaTable[DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Area("Dodongos Cavern MQ Lower Right Side", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return CanStunDeku;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, {[]{return (Here(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, []{return IsAdult && CanUse(BOW);}) || GoronBracelet ||
                                                                                CanUse(DINS_FIRE) || HasExplosives) &&
                                                                                IsChild && CanUse(SLINGSHOT);}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_BOMB_BAG_AREA] = Area("Dodongos Cavern MQ Bomb Bag Area", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  {[]{return (Here(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, []{return IsAdult && CanUse(BOW);}) ||  GoronBracelet || CanUse(DINS_FIRE) || HasExplosives) && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_BOSS_AREA] = Area("Dodongos Cavern MQ BossArea", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot,            {[]{return true;}}),
                  EventAccess(&DodongosCavernClear, {[]{return DodongosCavernClear || (CanBlastOrSmash && (Bombs || GoronBracelet) && (IsAdult || Sticks || KokiriSword));}}),
  }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_BOSS_ROOM_CHEST,      {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_KING_DODONGO_HEART,   {[]{return CanBlastOrSmash && (Bombs || GoronBracelet) && (IsAdult || Sticks || KokiriSword);}}),
                  LocationAccess(KING_DODONGO,                         {[]{return CanBlastOrSmash && (Bombs || GoronBracelet) && (IsAdult || Sticks || KokiriSword);}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_BACK_AREA,      {[]{return true;}}),
  }, {});
  }

  if (Dungeon::JabuJabusBelly.IsMQ()) {
  areaTable[JABU_JABUS_BELLY_MQ_BEGINNING] = Area("Jabu Jabus Belly MQ Beginning", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_MAP_CHEST,             {[]{return CanBlastOrSmash;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, {[]{return IsChild && CanUse(SLINGSHOT);}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN,  {[]{return Here(JABU_JABUS_BELLY_MQ_BEGINNING, []{return IsChild && CanUse(SLINGSHOT);});}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_MAIN] = Area("Jabu Jabus Belly MQ Main", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,      {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,      {[]{return (IsAdult && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT))) || ChildCanAccess(JABU_JABUS_BELLY_MQ_BOSS_AREA);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_COMPASS_CHEST,                {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,   {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,              {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,      {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || (LogicJabuMQSoTGS && IsChild && CanUse(BOOMERANG))
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_DEPTHS,    {[]{return HasExplosives && IsChild && CanUse(BOOMERANG);}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_DEPTHS] = Area("Jabu Jabus Belly MQ Depths", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,          {[]{return Sticks || CanUse(DINS_FIRE);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,    {[]{return (LogicLensJabuMQ || CanUse(LENS_OF_TRUTH)) || Here(JABU_JABUS_BELLY_MQ_MAIN, []{return IsAdult && CanUse(HOVER_BOOTS) && CanUse(HOOKSHOT);});}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_BOSS_AREA, {[]{return Sticks || (CanUse(DINS_FIRE) && KokiriSword);}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_BOSS_AREA] = Area("Jabu Jabus Belly MQ Boss Area", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot,            {[]{return true;}}),
                  EventAccess(&JabuJabusBellyClear, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_COW,             {[]{return CanPlay(EponasSong);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_BARINADE_HEART,     {[]{return true;}}),
                  LocationAccess(BARINADE,                            {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,    {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN, {[]{return true;}}),
  });
  }

  if (Dungeon::ForestTemple.IsMQ()) {
  areaTable[FOREST_TEMPLE_MQ_LOBBY] = Area("Forest Temple MQ Lobby", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, {[]{return IsAdult || Bombs || CanUse(STICKS) || Nuts || CanUse(BOOMERANG) || CanUse(DINS_FIRE) || KokiriSword || CanUse(SLINGSHOT);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, {[]{return HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_CENTRAL_AREA, {[]{return SmallKeys(FOREST_TEMPLE, 1) && (IsAdult || CanChildAttack || Nuts);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_CENTRAL_AREA] = Area("Forest Temple MQ Central Area", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_WOLFOS_CHEST,       {[]{return (CanPlay(SongOfTime) || IsChild) && (IsAdult || CanUse(DINS_FIRE) || CanUse(STICKS) || CanUse(SLINGSHOT) || KokiriSword);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, {[]{return IsAdult || KokiriSword;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}), //This is as far as child can get
                  Entrance(FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE, {[]{return IsAdult && GoronBracelet;}}),
                    //Trick: IsAdult && (GoronBracelet || (LogicForestMQBlockPuzzle && HasBombchus && IsAdult && CanUse(HOOKSHOT)))
                  Entrance(FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      {[]{return false;}}),
                    //Trick: (LogicForestMQHallwaySwitchJumpslash && IsAdult && CanUse(HOVER_BOOTS)) || (LogicForestMQHallwaySwitchHookshot && IsAdult && CanUse(HOOKSHOT))
                  Entrance(FOREST_TEMPLE_MQ_BOSS_REGION,        {[]{return ForestTempleJoAndBeth && ForestTempleAmyAndMeg;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE] = Area("Forest Temple MQ After Block Puzzle", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_BOW_REGION,    {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
                  Entrance(FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
                    //Trick: SmallKeys(FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJumpslash && ((IsAdult && CanUse(HOOKSHOT)) || LogicForestOutsideBackdoor))
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS,   {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Area("Forest Temple MQ Outdoor Ledge", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_REDEAD_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_NW_OUTDOORS] = Area("Forest Temple MQ NW Outdoors", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,         {[]{return (IsAdult && (CanUse(IRON_BOOTS) || CanUse(LONGSHOT))) || ProgressiveScale >= 2;}}),
                    //Trick: (IsAdult && (CanUse(IRON_BOOTS) || CanUse(LONGSHOT) || (LogicForestMQWellSwim && CanUse(HOOKSHOT)))) || ProgressiveScale >= 2
                  Entrance(FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(FIRE_ARROWS);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_NE_OUTDOORS] = Area("Forest Temple MQ NE Outdoors", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_WELL_CHEST,                 {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, {[]{return HookshotOrBoomerang || (IsAdult && CanUse(FIRE_ARROWS) && (CanPlay(SongOfTime) || (CanUse(HOVER_BOOTS) && LogicForestDoorFrame)));}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_WELL,                    {[]{return (IsAdult && ((CanUse(IRON_BOOTS) && CanUse(HOOKSHOT)) || CanUse(BOW))) || (IsChild && CanUse(SLINGSHOT));}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(HOOKSHOT) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS) || CanPlay(SongOfTime));}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   {[]{return IsAdult && CanUse(LONGSHOT);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Area("Forest Temple MQ Outdoors Top Ledges", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,       {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return false;}}),
                    //Trick: LogicForestOutdoorsLedge && IsAdult && CanUse(HOVER_BOOTS)
  });

  areaTable[FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Area("Forest Temple MQ NE Outdoors Ledge", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,  {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return CanPlay(SongOfTime);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_BOW_REGION] = Area("Forest Temple MQ Bow Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoAndBeth, {[]{return ForestTempleJoAndBeth || (IsAdult && CanUse(BOW));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BOW_CHEST,     {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_MQ_MAP_CHEST,     {[]{return IsAdult && CanUse(BOW);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_COMPASS_CHEST, {[]{return IsAdult && CanUse(BOW);}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5) && ((IsAdult && CanUse(BOW)) || CanUse(DINS_FIRE));}}),
  });

  areaTable[FOREST_TEMPLE_MQ_FALLING_ROOM] = Area("Forest Temple MQ Falling Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmyAndMeg, {[]{return ForestTempleAmyAndMeg || (IsAdult && CanUse(BOW) && SmallKeys(FOREST_TEMPLE, 6));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_BOSS_REGION] = Area("Forest Temple MQ Boss Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleClear, {[]{return ForestTempleClear || BossKeyForestTemple;}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BASEMENT_CHEST,   {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_PHANTOM_GANON_HEART, {[]{return BossKeyForestTemple;}}),
                  LocationAccess(PHANTOM_GANON,                     {[]{return BossKeyForestTemple;}}),
  }, {});
  }

  if (Dungeon::FireTemple.IsMQ()) {
  areaTable[FIRE_TEMPLE_MQ_LOWER] = Area("Fire Temple MQ Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, {[]{return IsAdult || KokiriSword || Sticks || Slingshot || Bombs || CanUse(DINS_FIRE);}}),
                  LocationAccess(FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,     {[]{return IsAdult && FireTimer >= 24
                                                                             && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT))
                                                                             && (CanUse(FIRE_ARROWS) || (CanUse(DINS_FIRE) &&
                                                                                                            ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE))
                                                                                                           || CanUse(GORON_TUNIC)
                                                                                                           || CanUse(BOW)
                                                                                                           || CanUse(LONGSHOT))));}}),
                    //Trick: IsAdult && (LogicFewerTunicRequirements || CanUse(GORON_TUNIC)) && (((CanUse(HOVER_BOOTS) || (LogicFireMQNearBoss && CanUse(BOW))) && HasFireSource) || (CanUse(HOOKSHOT) && CanUse(FIRE_ARROWS) || (CanUse(DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || CanUse(GORON_TUNIC) || CanUse(BOW) || CanUse(LONGSHOT)))))
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ENTRYWAY,             {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_MQ_BOSS_ROOM,         {[]{return IsAdult && CanUse(GORON_TUNIC) && CanUse(MEGATON_HAMMER) && BossKeyFireTemple && ((HasFireSource && (LogicFireBossDoorJump || HoverBoots)) || HasAccessTo(FIRE_TEMPLE_MQ_UPPER));}}),
                  Entrance(FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR, {[]{return SmallKeys(FIRE_TEMPLE, 5) && (IsAdult || KokiriSword);}}),
                  Entrance(FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,     {[]{return IsAdult && FireTimer >= 24 && CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR] = Area("Fire Temple MQ Lower Locked Door", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, {[]{return IsAdult && (HasExplosives || Hammer || Hookshot);}}),
                  LocationAccess(FIRE_TEMPLE_MQ_MAP_CHEST,            {[]{return IsAdult && CanUse(MEGATON_HAMMER);}}),
  }, {});

  areaTable[FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Area("Fire Temple MQ Big Lava Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (HasFireSource && Bow && IsAdult && (CanUse(HOOKSHOT) || LogicFireSongOfTime));}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && (CanUse(HOOKSHOT) || LogicFireSongOfTime)
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,                   {[]{return HasFireSource && Bow && IsAdult && CanUse(HOOKSHOT);}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && CanUse(HOOKSHOT)
                  LocationAccess(FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return HasFireSource && HasExplosives && IsAdult && CanUse(HOOKSHOT);}}),
                    //Trick: HasFireSource && HasExplosives && IsAdult && (CanUse(HOOKSHOT) || LogicFireMQBlockedChest)
                  LocationAccess(FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_LOWER_MAZE, {[]{return IsAdult && CanUse(GORON_TUNIC) && SmallKeys(FIRE_TEMPLE, 2) && HasFireSource;}}),
                    //Trick: IsAdult && CanUse(GORON_TUNIC) && SmallKeys(FIRE_TEMPLE, 2) && (HasFireSource || (LogicFireMQClimb && HoverBoots))
  });

  areaTable[FIRE_TEMPLE_MQ_LOWER_MAZE] = Area("Fire Temple MQ Lower Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,     {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, {[]{return HasExplosives && HasAccessTo(FIRE_TEMPLE_MQ_UPPER_MAZE);}}),
                    //Trick: HasExplosives && (LogicFireMQMazeSideRoom || FIRE_TEMPLE_MQ_UPPER_MAZE.Adult())
   }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_UPPER_MAZE, {[]{return HasExplosives && IsAdult && CanUse(HOOKSHOT);}}),
                    //Trick: (IsAdult && ((HasExplosives && CanUse(HOOKSHOT)) || (LogicFireMQMazeHovers && CanUse(HOVER_BOOTS)))) || LogicFireMQMazeJump
  });

  areaTable[FIRE_TEMPLE_MQ_UPPER_MAZE] = Area("Fire Temple MQ Upper Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && (((CanPlay(SongOfTime) && CanUse(HOOKSHOT) && HasExplosives) || CanUse(LONGSHOT))));}}),
                  EventAccess(&FairyPot,  {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_COMPASS_CHEST,             {[]{return HasExplosives;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,          {[]{return IsAdult && ((CanPlay(SongOfTime) && CanUse(HOOKSHOT) && HasExplosives) || CanUse(LONGSHOT));}}),
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_UPPER, {[]{return SmallKeys(FIRE_TEMPLE, 3) && IsAdult && ((CanUse(BOW) && CanUse(HOOKSHOT)) || CanUse(FIRE_ARROWS));}}),
  });

  areaTable[FIRE_TEMPLE_MQ_UPPER] = Area("Fire Temple MQ Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_FREESTANDING_KEY,            {[]{return IsAdult && CanUse(HOOKSHOT);}}),
                    //Trick: (IsAdult && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze
                  LocationAccess(FIRE_TEMPLE_MQ_CHEST_ON_FIRE,               {[]{return IsAdult && CanUse(HOOKSHOT) && SmallKeys(FIRE_TEMPLE, 4);}}),
                    //Trick: ((IsAdult && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze) && SmallKeys(FIRE_TEMPLE, 4)
                  LocationAccess(FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, {[]{return CanPlay(SongOfTime) || HoverBoots;}}),
                    //Trick: CanPlay(SongOfTime) || HoverBoots || LogicFireMQFlameMaze
                  LocationAccess(FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,    {[]{return HasExplosives;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,     {[]{return IsAdult && CanUse(HOOKSHOT) && SmallKeys(FIRE_TEMPLE, 5);}}),
                    //Trick: (IsAdult && CanUse(HOOKSHOT) && SmallKeys(FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && IsAdult && CanUse(LONGSHOT))
  }, {});

  areaTable[FIRE_TEMPLE_MQ_BOSS_ROOM] = Area("Fire Temple MQ Boss Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTempleClear, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_VOLVAGIA_HEART, {[]{return true;}}),
                  LocationAccess(VOLVAGIA,                   {[]{return true;}}),
  }, {});
  }

  if (Dungeon::WaterTemple.IsMQ()) {
  areaTable[WATER_TEMPLE_MQ_LOBBY] = Area("Water Temple MQ Lobby", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleClear, {[]{return BossKeyWaterTemple && IsAdult && CanUse(LONGSHOT);}}),
  }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MORPHA_HEART, {[]{return BossKeyWaterTemple && IsAdult && CanUse(LONGSHOT);}}),
                  LocationAccess(MORPHA,                    {[]{return BossKeyWaterTemple && IsAdult && CanUse(LONGSHOT);}}),
  }, {
                  //Exits
                  Entrance(WATER_TEMPLE_ENTRYWAY,            {[]{return true;}}),
                  Entrance(WATER_TEMPLE_MQ_DIVE,             {[]{return IsAdult && WaterTimer >= 24 && CanUse(IRON_BOOTS);}}),
                  Entrance(WATER_TEMPLE_MQ_DARK_LINK_REGION, {[]{return SmallKeys(WATER_TEMPLE, 1) && IsAdult && CanUse(LONGSHOT);}}),
  });

  areaTable[WATER_TEMPLE_MQ_DIVE] = Area("Water Temple MQ Dive", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_MAP_CHEST,            {[]{return HasFireSource && IsAdult && CanUse(HOOKSHOT);}}),
                  LocationAccess(WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, {[]{return IsAdult && CanUse(ZORA_TUNIC) && CanUse(HOOKSHOT) && (CanUse(DINS_FIRE) && CanPlay(SongOfTime));}}),
                    //Trick: IsAdult && CanUse(ZORA_TUNIC) && CanUse(HOOKSHOT) && ((LogicWaterMQCentralPillar && CanUse(FIRE_ARROWS)) || (CanUse(DINS_FIRE) && CanPlay(SongOfTime)))
  }, {
                  //Exits
                  Entrance(WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS, {[]{return CanPlay(ZeldasLullaby);}}),
  });

  areaTable[WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS] = Area("Water Temple MQ Lowered Water Levels", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_COMPASS_CHEST,                {[]{return (IsAdult && CanUse(BOW)) || CanUse(DINS_FIRE) || Here(WATER_TEMPLE_MQ_LOBBY, []{return IsChild && CanUse(STICKS) && HasExplosives;});}}),
                  LocationAccess(WATER_TEMPLE_MQ_LONGSHOT_CHEST,               {[]{return IsAdult && CanUse(HOOKSHOT);}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,          {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, {[]{return IsAdult && CanUse(LONGSHOT);}}),
  }, {});

  areaTable[WATER_TEMPLE_MQ_DARK_LINK_REGION] = Area("Water Temple MQ Dark Link Region", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return IsAdult && WaterTimer >= 24 && CanUse(DINS_FIRE) && (LogicWaterDragonJumpDive || CanDive || CanUse(IRON_BOOTS));}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_RIVER,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS, {[]{return IsAdult && WaterTimer >= 24 && CanUse(DINS_FIRE) && CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS] = Area("Water Temple MQ Basement Gated Areas", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_FREESTANDING_KEY,         {[]{return HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump;}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,     {[]{return CanUse(FIRE_ARROWS) && (HoverBoots || CanUse(SCARECROW));}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, {[]{return SmallKeys(WATER_TEMPLE, 2) && (HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump);}}),
                    //Trick: LogicWaterMQLockedGS || (SmallKeys(WATER_TEMPLE, 2) && (HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump))
  }, {});
  }

  if (Dungeon::SpiritTemple.IsMQ()) {
  areaTable[SPIRIT_TEMPLE_MQ_LOBBY] = Area("Spirit Temple MQ Lobby", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,  {[]{return Here(SPIRIT_TEMPLE_MQ_LOBBY, []{return CanBlastOrSmash;}) && ((IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW)));}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, {[]{return HasBombchus || (IsAdult && (CanUse(BOW) || CanUse(HOOKSHOT))) || (IsChild && (CanUse(SLINGSHOT) || CanUse(BOOMERANG)));}}),
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_CHILD, {[]{return IsChild;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_ADULT, {[]{return HasBombchus && IsAdult && CanUse(LONGSHOT) && CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_CHILD] = Area("Spirit Temple MQ Child", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (KokiriSword && HasBombchus && Slingshot);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,  {[]{return Here(SPIRIT_TEMPLE_MQ_ADULT, []{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer;});}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,       {[]{return KokiriSword && HasBombchus && Slingshot && CanUse(DINS_FIRE);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_MAP_CHEST,                  {[]{return KokiriSword || Bombs;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, {[]{return HasBombchus && SmallKeys(SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(DINS_FIRE) || (Here(SPIRIT_TEMPLE_MQ_ADULT, []{return IsAdult && CanUse(FIRE_ARROWS);})));}}),
                    //Trick: HasBombchus && SmallKeys(SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(DINS_FIRE) || (SPIRIT_TEMPLE_MQ_ADULT.Adult() && IsAdult && (CanUse(FIRE_ARROWS) || (LogicSpiritMQFrozenEye && CanUse(BOW) && CanPlay(SongOfTime)))))
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_SHARED, {[]{return HasBombchus && SmallKeys(SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_ADULT] = Area("Spirit Temple MQ Adult", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,     {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, {[]{return (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,           {[]{return SmallKeys(SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,          {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,              {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime) && MirrorShield;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_LOWER_ADULT,        {[]{return MirrorShield && IsAdult && CanUse(FIRE_ARROWS);}}),
                    //Trick: MirrorShield && IsAdult && (CanUse(FIRE_ARROWS) || (LogicSpiritMQLowerAdult && CanUse(DINS_FIRE) && Bow))
                  Entrance(SPIRIT_TEMPLE_MQ_SHARED,             {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_BOSS_AREA,          {[]{return SmallKeys(SPIRIT_TEMPLE, 6) && CanPlay(ZeldasLullaby) && Hammer;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND, {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH));}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_SHARED] = Area("Spirit Temple MQ Shared", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, {[]{return SmallKeys(SPIRIT_TEMPLE, 6);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_COMPASS_CHEST,           {[]{return (IsChild && CanUse(SLINGSHOT) && SmallKeys(SPIRIT_TEMPLE, 7)) || (IsAdult && CanUse(BOW)) || (Bow && Slingshot);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,    {[]{return CanPlay(SongOfTime) || IsAdult;}}),
                    //Trick: CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,       {[]{return IsAdult;}}),
                    //Trick: (LogicSpiritMQSunBlockGS && Boomerange && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT)) || IsAdult
   }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)));}}),
                    //Trick: (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)))
                  Entrance(DESERT_COLOSSUS,                        {[]{return (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)) && IsAdult);}}),
                    //Trick: (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)) && IsAdult)
  });

  areaTable[SPIRIT_TEMPLE_MQ_LOWER_ADULT] = Area("Spirit Temple MQ Lower Adult", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,          {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,        {[]{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, {[]{return Hammer;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,             {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,           {[]{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_BOSS_AREA] = Area("Spirit Temple MQ Boss Area", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTempleClear, {[]{return SpiritTempleClear || (MirrorShield && BossKeySpiritTemple);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, {[]{return LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(SPIRIT_TEMPLE_TWINROVA_HEART,                   {[]{return MirrorShield && BossKeySpiritTemple;}}),
                  LocationAccess(TWINROVA,                                       {[]{return MirrorShield && BossKeySpiritTemple;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND] = Area("Spirit Temple MQ Mirror Shield Hand", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, {[]{return true;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND] = Area("Spirit Temple MQ Silver Gauntlets Hand", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return true;}}),
  }, {});
  }

  if (Dungeon::ShadowTemple.IsMQ()) {
  areaTable[SHADOW_TEMPLE_MQ_BEGINNING] = Area("Shadow Temple MQ Beginning", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return IsAdult && (CanUse(FIRE_ARROWS) || CanUse(HOVER_BOOTS));}}),
                    //Trick: IsAdult && (CanUse(FIRE_ARROWS) || HoverBoots || (LogicShadowMQGap && CanUse(LONGSHOT)))
                  Entrance(SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return HasExplosives && SmallKeys(SHADOW_TEMPLE, 6);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Area("Shadow Temple MQ Dead Hand Area", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_COMPASS_CHEST,     {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, {[]{return CanPlay(SongOfTime) && IsAdult && CanUse(BOW);}}),
  }, {});

  areaTable[SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Area("Shadow Temple MQ First Beamos", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,        {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return HasExplosives && SmallKeys(SHADOW_TEMPLE, 2);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Area("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return HasFireSource;}}),
                    //Trick: HasFireSource || LogicShadowMQHugePit
  });

  areaTable[SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Area("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,  {[]{return IsAdult && CanUse(LONGSHOT);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,      {[]{return HoverBoots && SmallKeys(SHADOW_TEMPLE, 3) && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,          {[]{return HoverBoots && SmallKeys(SHADOW_TEMPLE, 3) && Hookshot && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      {[]{return Hookshot;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return HoverBoots && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH)) && Hookshot && SmallKeys(SHADOW_TEMPLE, 4);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Area("Shadow Temple MQ Wind Tunnel", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,       {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_BEYOND_BOAT, {[]{return CanPlay(ZeldasLullaby) && SmallKeys(SHADOW_TEMPLE, 5);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Area("Shadow Temple MQ Beyond Boat", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTempleClear, {[]{return ShadowTempleClear || ((Bow || (LogicShadowStatue && HasBombchus)) && BossKeyShadowTemple);}}),
  }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_BONGO_BONGO_HEART, {[]{return (Bow || (LogicShadowStatue && HasBombchus)) && BossKeyShadowTemple;}}),
                  LocationAccess(BONGO_BONGO,                     {[]{return (Bow || (LogicShadowStatue && HasBombchus)) && BossKeyShadowTemple;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,   {[]{return Bow || (LogicShadowStatue && HasBombchus);}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return Bow && CanPlay(SongOfTime) && IsAdult && CanUse(LONGSHOT);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Area("Shadow Temple MQ Invisible Maze", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, {[]{return CanUse(DINS_FIRE) && SmallKeys(SHADOW_TEMPLE, 6);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         {[]{return CanUse(DINS_FIRE) && SmallKeys(SHADOW_TEMPLE, 6);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,      {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FREESTANDING_KEY,       {[]{return true;}}),
  }, {});
  }

  if (Dungeon::BottomOfTheWell.IsMQ()) {
  areaTable[BOTTOM_OF_THE_WELL_MQ_PERIMETER] = Area("Bottom of the Well MQ Perimeter", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || Slingshot;}}),
  }, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              {[]{return KokiriSword || (Sticks && LogicChildDeadhand);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, {[]{return HasExplosives;}}),
                    //Trick: HasExplosives || (LogicBotWMQDeadHandKey && Boomerang)
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                {[]{return CanChildAttack;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,             {[]{return CanChildAttack && SmallKeys(BOTTOM_OF_THE_WELL, 2);}}),
  }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY,  {[]{return true;}}),
                  Entrance(BOTTOM_OF_THE_WELL_MQ_MIDDLE, {[]{return CanPlay(ZeldasLullaby);}}),
                    //Trick: CanPlay(ZeldasLullaby) || (LogicBotWMQPits && HasExplosives)
  });

  areaTable[BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Area("Bottom of the Well MQ Middle", "Bottom of the Well", BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,                        {[]{return true;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,              {[]{return HasExplosives && SmallKeys(BOTTOM_OF_THE_WELL, 2);}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, {[]{return true;}}),
                  LocationAccess(BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               {[]{return CanChildAttack && HasExplosives;}}),
                    //Trick: CanChildAttack && (LogicBotWMQPits || HasExplosives)
  }, {
                  //Exits
                  Entrance(BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });
  }

  if (Dungeon::IceCavern.IsMQ()) {
  areaTable[ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(ICE_CAVERN_MQ_MAP_ROOM,          {[]{return IsAdult || CanUse(DINS_FIRE) || (HasExplosives && (CanUse(STICKS) || CanUse(SLINGSHOT) || KokiriSword));}}),
                  Entrance(ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return IsAdult && BlueFire;}}),
                  Entrance(ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return BlueFire;}}),
  });

  areaTable[ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_MAP_CHEST, {[]{return BlueFire && (IsAdult || CanUse(STICKS) || KokiriSword || CanUseProjectile);}}),
  }, {});

  areaTable[ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return IsAdult;}}),
                  LocationAccess(SHEIK_IN_ICE_CAVERN,            {[]{return IsAdult;}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return IsAdult || CanUseProjectile;}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return IsAdult && (CanUse(SCARECROW) || (CanUse(HOVER_BOOTS) && CanUse(LONGSHOT)));}}),
                    //Tricks: (CanUse(SCARECROW) || (HoverBoots && CanUse(LONGSHOT)) || LogicIceMQScarecrow) && IsAdult
  }, {});

  areaTable[ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return HasExplosives;}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || LogicIceMQRedIceGS
  }, {});
  }

  if (Dungeon::GerudoTrainingGrounds.IsMQ()) {
  areaTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY] = Area("Gerudo Training Grounds MQ Lobby", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_LOBBY_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_LOBBY_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_HIDDEN_CEILING_CHEST,   {[]{return LogicLensGtgMQ || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 1);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY,      {[]{return true;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,  {[]{return Here(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return HasFireSource;});}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE, {[]{return Here(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));});}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE] = Area("Gerudo Training Grounds MQ Right Side", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && CanUse(BOW));}}),
  }, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_DINOLFOS_CHEST, {[]{return IsAdult;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER, {[]{return (Bow || CanUse(LONGSHOT)) && CanUse(HOVER_BOOTS) && IsAdult;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER] = Area("Gerudo Training Grounds MQ Underwater", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return HasFireSource && IsAdult && CanUse(IRON_BOOTS) && WaterTimer >= 24 && CanTakeDamage;}}),
  }, {});

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE] = Area("Gerudo Training Grounds MQ Left Side", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_FIRST_IRON_KNUCKLE_CHEST, {[]{return IsAdult || KokiriSword || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM, {[]{return IsAdult && CanUse(LONGSHOT);}}),
                    //Trick: (IsAdult && CanUse(LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && IsAdult && CanUse(HOOKSHOT))
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM] = Area("Gerudo Training Grounds MQ Stalfos Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_BEFORE_HEAVY_BLOCK_CHEST, {[]{return IsAdult;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_HEAVY_BLOCK_CHEST,        {[]{return CanUse(SILVER_GAUNTLETS);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS, {[]{return IsAdult && (LogicLensGtgMQ || CanUse(LENS_OF_TRUTH)) && BlueFire && CanPlay(SongOfTime);}}),
                    //Trick: IsAdult && (LogicLensGtgMQ || CanUse(LENS_OF_TRUTH)) && BlueFire && (CanPlay(SongOfTime) || (LogicGtgFakeWall && IsAdult && CanUse(HOVER_BOOTS)))
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS] = Area("Gerudo Training Grounds MQ Back Areas", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_EYE_STATUE_CHEST,          {[]{return Bow;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_SECOND_IRON_KNUCKLE_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_FLAME_CIRCLE_CHEST,        {[]{return CanUse(HOOKSHOT) || Bow || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT, {[]{return Hammer;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,         {[]{return CanUse(LONGSHOT);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds MQ Central Maze Right", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_ICE_ARROWS_CHEST,         {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 3);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER,  {[]{return IsAdult && (CanUse(LONGSHOT) || (CanUse(HOOKSHOT) && Bow));}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,  {[]{return IsAdult && CanUse(HOOKSHOT);}}),
  });
  }

  if (Dungeon::GanonsCastle.IsMQ()) {
  areaTable[GANONS_CASTLE_MQ_LOBBY] = Area("Ganon's Castle MQ Lobby", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_ENTRYWAY,        {[]{return (IsAdult || (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword))));}}),
                  Entrance(GANONS_CASTLE_MQ_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_LIGHT_TRIAL,  {[]{return CanUse(GOLDEN_GAUNTLETS);}}),
                  Entrance(GANONS_CASTLE_TOWER,           {[]{return (ForestTrialClear || Trial::ForestTrial.IsSkipped()) &&
                                                                            (FireTrialClear   || Trial::FireTrial.IsSkipped())   &&
                                                                            (WaterTrialClear  || Trial::WaterTrial.IsSkipped())  &&
                                                                            (ShadowTrialClear || Trial::ShadowTrial.IsSkipped()) &&
                                                                            (SpiritTrialClear || Trial::SpiritTrial.IsSkipped()) &&
                                                                            (LightTrialClear  || Trial::LightTrial.IsSkipped());}}),
                  Entrance(GANONS_CASTLE_MQ_DEKU_SCRUBS,  {[]{return LogicLensCastleMQ || CanUse(LENS_OF_TRUTH);}}),
  });

  areaTable[GANONS_CASTLE_MQ_DEKU_SCRUBS] = Area("Ganon's Castle MQ Deku Scrubs", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_FOREST_TRIAL] = Area("Ganon's Castle MQ Forest Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && CanPlay(SongOfTime);}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, {[]{return HasFireSource;}}),
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,        {[]{return HookshotOrBoomerang;}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_FIRE_TRIAL] = Area("Ganon's Castle MQ Fire Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS));}}),
                    //Trick: CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && (CanUse(LONGSHOT) || HoverBoots || (LogicFireTrialMQ && CanUse(HOOKSHOT)))
  }, {}, {});

  areaTable[GANONS_CASTLE_MQ_WATER_TRIAL] = Area("Ganon's Castle MQ Water Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3);}}),
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, {[]{return BlueFire && (IsAdult || CanUse(STICKS) || KokiriSword || CanUseProjectile);}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_SHADOW_TRIAL] = Area("Ganon's Castle MQ Shadow Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || (CanUse(HOOKSHOT) && HasFireSource));}}),
                    //Trick: IsAdult && CanUse(LIGHT_ARROWS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, {[]{return IsAdult && ((Bow && (CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS))) || (CanUse(HOVER_BOOTS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HasExplosives || GoronBracelet || CanUse(DINS_FIRE))));}}),
                  LocationAccess(GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  {[]{return IsAdult && Bow && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || (CanUse(HOOKSHOT) && HasFireSource));}}),
                    //Trick: IsAdult && Bow && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {});

  areaTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL] = Area("Ganon's Castle MQ Spirit Castle", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && Hammer && HasBombchus && FireArrows && MirrorShield;}}),
                  EventAccess(&NutPot,           {[]{return NutPot || (Hammer && HasBombchus && IsAdult && CanUse(FIRE_ARROWS) && MirrorShield);}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,             {[]{return IsAdult && Bow && Hammer;}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         {[]{return IsAdult && Bow && Hammer && HasBombchus && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_LIGHT_TRIAL] = Area("Ganon's Castle MQ Light Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && CanUse(HOOKSHOT);}}),
                    //Trick: IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (Hookshot || LogicLightTrialMQ)
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, {[]{return CanPlay(ZeldasLullaby);}}),
  }, {});
  }

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
    KAK_ODD_POULTICE_BUILDING,
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
