#include "location_access.h"

#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/3drando/item_pool.hpp"
#include "soh/Enhancements/randomizer/3drando/spoiler_log.hpp"
#include "soh/Enhancements/randomizer/trial.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/debugger/performanceTimer.h"

#include <fstream>
#include <soh/OTRGlobals.h>

#include "3drando/shops.hpp"
extern "C" {
extern PlayState* gPlayState;
}

// generic grotto event list
std::vector<EventAccess> grottoEvents;

bool EventAccess::CheckConditionAtAgeTime(bool& age, bool& time) {
    logic->IsChild = false;
    logic->IsAdult = false;
    logic->AtDay = false;
    logic->AtNight = false;

    time = true;
    age = true;

    return ConditionsMet();
}

// set the logic to be a specific age and time of day and see if the condition still holds
bool LocationAccess::CheckConditionAtAgeTime(bool& age, bool& time) const {
    logic->IsChild = false;
    logic->IsAdult = false;
    logic->AtDay = false;
    logic->AtNight = false;

    time = true;
    age = true;

    return GetConditionsMet();
}

bool LocationAccess::ConditionsMet(Region* parentRegion, bool calculatingAvailableChecks) const {
    // WARNING enterance validation can run this after resetting the access for sphere 0 validation
    // When refactoring ToD access, either fix the above or do not assume that we
    // have any access at all just because this is being run
    bool conditionsMet = false;

    if ((parentRegion->childDay && CheckConditionAtAgeTime(logic->IsChild, logic->AtDay)) ||
        (parentRegion->childNight && CheckConditionAtAgeTime(logic->IsChild, logic->AtNight)) ||
        (parentRegion->adultDay && CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay)) ||
        (parentRegion->adultNight && CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight))) {
        conditionsMet = true;
    }

    return conditionsMet;
}

static uint16_t GetMinimumPrice(const Rando::Location* loc) {
    extern PriceSettingsStruct shopsanityPrices;
    extern PriceSettingsStruct scrubPrices;
    extern PriceSettingsStruct merchantPrices;
    PriceSettingsStruct priceSettings = loc->GetRCType() == RCTYPE_SHOP    ? shopsanityPrices
                                        : loc->GetRCType() == RCTYPE_SCRUB ? scrubPrices
                                                                           : merchantPrices;

    auto ctx = Rando::Context::GetInstance();
    switch (ctx->GetOption(priceSettings.main).Get()) {
        case RO_PRICE_VANILLA:
            return loc->GetVanillaPrice();
        case RO_PRICE_CHEAP_BALANCED:
            return 0;
        case RO_PRICE_BALANCED:
            return 0;
        case RO_PRICE_FIXED:
            return ctx->GetOption(priceSettings.fixedPrice).Get() * 5;
        case RO_PRICE_RANGE: {
            uint16_t range1 = ctx->GetOption(priceSettings.range1).Get() * 5;
            uint16_t range2 = ctx->GetOption(priceSettings.range1).Get() * 5;
            return range1 < range2 ? range1 : range2;
        }
        case RO_PRICE_SET_BY_WALLET: {
            if (ctx->GetOption(priceSettings.noWallet).Get()) {
                return 0;
            } else if (ctx->GetOption(priceSettings.childWallet).Get()) {
                return 1;
            } else if (ctx->GetOption(priceSettings.adultWallet).Get()) {
                return 100;
            } else if (ctx->GetOption(priceSettings.giantWallet).Get()) {
                return 201;
            } else {
                return 501;
            }
        }
        default:
            return 0;
    }
}

uint16_t GetCheckPrice(RandomizerCheck check /* = RC_UNKNOWN_CHECK */) {
    RandomizerCheck rc = check != RC_UNKNOWN_CHECK ? check : logic->CurrentCheckKey;
    assert(rc != RC_UNKNOWN_CHECK);
    const auto& loc = Rando::StaticData::GetLocation(rc);
    assert(loc->GetRCType() == RCTYPE_SHOP || loc->GetRCType() == RCTYPE_SCRUB || loc->GetRCType() == RCTYPE_MERCHANT);
    const auto& itemLoc = OTRGlobals::Instance->gRandoContext->GetItemLocation(rc);

    // Checks should only be identified while playing
    if (logic->CalculatingAvailableChecks && itemLoc->GetCheckStatus() != RCSHOW_IDENTIFIED) {
        return GetMinimumPrice(loc);
    }

    return itemLoc->GetPrice();
}

uint16_t GetWalletCapacity() {
    if (logic->HasItem(RG_TYCOON_WALLET)) {
        return 999;
    } else if (logic->HasItem(RG_GIANT_WALLET)) {
        return 500;
    } else if (logic->HasItem(RG_ADULT_WALLET)) {
        return 200;
    } else if (logic->HasItem(RG_CHILD_WALLET)) {
        return 99;
    }
    return 0;
}

std::set<RandomizerArea> CalculateAreas(SceneID scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
            return { RA_DEKU_TREE };
        case SCENE_DODONGOS_CAVERN:
            return { RA_DODONGOS_CAVERN };
        case SCENE_JABU_JABU:
            return { RA_JABU_JABUS_BELLY };
        case SCENE_FOREST_TEMPLE:
            return { RA_FOREST_TEMPLE };
        case SCENE_FIRE_TEMPLE:
            return { RA_FIRE_TEMPLE };
        case SCENE_WATER_TEMPLE:
            return { RA_WATER_TEMPLE };
        case SCENE_SPIRIT_TEMPLE:
            return { RA_SPIRIT_TEMPLE };
        case SCENE_SHADOW_TEMPLE:
            return { RA_SHADOW_TEMPLE };
        case SCENE_BOTTOM_OF_THE_WELL:
            return { RA_BOTTOM_OF_THE_WELL };
        case SCENE_ICE_CAVERN:
            return { RA_ICE_CAVERN };
        case SCENE_INSIDE_GANONS_CASTLE:
            return { RA_GANONS_CASTLE };
        case SCENE_GERUDO_TRAINING_GROUND:
            return { RA_GERUDO_TRAINING_GROUND };
        case SCENE_THIEVES_HIDEOUT:
        case SCENE_GERUDOS_FORTRESS:
            return { RA_GERUDO_FORTRESS };
        case SCENE_MARKET_ENTRANCE_DAY:
        case SCENE_MARKET_ENTRANCE_NIGHT:
        case SCENE_MARKET_ENTRANCE_RUINS:
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
            return { RA_THE_MARKET };
        case SCENE_TEMPLE_OF_TIME:
            return { RA_TEMPLE_OF_TIME };
        case SCENE_HYRULE_FIELD:
            return { RA_HYRULE_FIELD };
        case SCENE_KAKARIKO_VILLAGE:
            return { RA_KAKARIKO_VILLAGE };
        case SCENE_GRAVEYARD:
            return { RA_THE_GRAVEYARD };
        case SCENE_ZORAS_RIVER:
            return { RA_ZORAS_RIVER };
        case SCENE_KOKIRI_FOREST:
            return { RA_KOKIRI_FOREST };
        case SCENE_SACRED_FOREST_MEADOW:
            return { RA_SACRED_FOREST_MEADOW };
        case SCENE_LAKE_HYLIA:
            return { RA_LAKE_HYLIA };
        case SCENE_ZORAS_DOMAIN:
            return { RA_ZORAS_DOMAIN };
        case SCENE_ZORAS_FOUNTAIN:
            return { RA_ZORAS_FOUNTAIN };
        case SCENE_GERUDO_VALLEY:
            return { RA_GERUDO_VALLEY };
        case SCENE_LOST_WOODS:
            return { RA_THE_LOST_WOODS };
        case SCENE_DESERT_COLOSSUS:
            return { RA_DESERT_COLOSSUS };
        case SCENE_HAUNTED_WASTELAND:
            return { RA_HAUNTED_WASTELAND };
        case SCENE_HYRULE_CASTLE:
            return { RA_HYRULE_CASTLE };
        case SCENE_DEATH_MOUNTAIN_TRAIL:
            return { RA_DEATH_MOUNTAIN_TRAIL };
        case SCENE_DEATH_MOUNTAIN_CRATER:
            return { RA_DEATH_MOUNTAIN_CRATER };
        case SCENE_GORON_CITY:
            return { RA_GORON_CITY };
        case SCENE_LON_LON_RANCH:
            return { RA_LON_LON_RANCH };
        case SCENE_OUTSIDE_GANONS_CASTLE:
            return { RA_OUTSIDE_GANONS_CASTLE };
        case SCENE_TREASURE_BOX_SHOP:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
        case SCENE_GANONS_TOWER:
        case SCENE_GANONDORF_BOSS:
        case SCENE_KNOW_IT_ALL_BROS_HOUSE:
        case SCENE_TWINS_HOUSE:
        case SCENE_MIDOS_HOUSE:
        case SCENE_SARIAS_HOUSE:
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
        case SCENE_BACK_ALLEY_HOUSE:
        case SCENE_BAZAAR:
        case SCENE_KOKIRI_SHOP:
        case SCENE_GORON_SHOP:
        case SCENE_ZORA_SHOP:
        case SCENE_POTION_SHOP_KAKARIKO:
        case SCENE_POTION_SHOP_MARKET:
        case SCENE_BOMBCHU_SHOP:
        case SCENE_HAPPY_MASK_SHOP:
        case SCENE_LINKS_HOUSE:
        case SCENE_DOG_LADY_HOUSE:
        case SCENE_STABLE:
        case SCENE_IMPAS_HOUSE:
        case SCENE_LAKESIDE_LABORATORY:
        case SCENE_CARPENTERS_TENT:
        case SCENE_GRAVEKEEPERS_HUT:
        case SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC:
        case SCENE_FAIRYS_FOUNTAIN:
        case SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS:
        case SCENE_GROTTOS:
        case SCENE_REDEAD_GRAVE:
        case SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN:
        case SCENE_ROYAL_FAMILYS_TOMB:
        case SCENE_SHOOTING_GALLERY:
        case SCENE_CASTLE_COURTYARD_GUARDS_DAY:
        case SCENE_CASTLE_COURTYARD_GUARDS_NIGHT:
        case SCENE_WINDMILL_AND_DAMPES_GRAVE:
        case SCENE_FISHING_POND:
        case SCENE_CASTLE_COURTYARD_ZELDA:
        case SCENE_BOMBCHU_BOWLING_ALLEY:
        case SCENE_LON_LON_BUILDINGS:
        case SCENE_MARKET_GUARD_HOUSE:
        case SCENE_POTION_SHOP_GRANNY:
        case SCENE_HOUSE_OF_SKULLTULA:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
        case SCENE_GANON_BOSS:
        case SCENE_ID_MAX:
            return {};
        case SCENE_CHAMBER_OF_THE_SAGES:
        case SCENE_CUTSCENE_MAP:
        case SCENE_TEST01:
        case SCENE_BESITU:
        case SCENE_DEPTH_TEST:
        case SCENE_SYOTES:
        case SCENE_SYOTES2:
        case SCENE_SUTARU:
        case SCENE_HAIRAL_NIWA2:
        case SCENE_SASATEST:
        case SCENE_TESTROOM:
        default:
            assert(false);
            return {};
    }
}

bool GetTimePassFromScene(SceneID scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_GANONS_TOWER:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_THIEVES_HIDEOUT:
        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_TREASURE_BOX_SHOP:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
        case SCENE_GANONDORF_BOSS:
        case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
        case SCENE_MARKET_ENTRANCE_DAY:
        case SCENE_MARKET_ENTRANCE_NIGHT:
        case SCENE_MARKET_ENTRANCE_RUINS:
        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
        case SCENE_KNOW_IT_ALL_BROS_HOUSE:
        case SCENE_TWINS_HOUSE:
        case SCENE_MIDOS_HOUSE:
        case SCENE_SARIAS_HOUSE:
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
        case SCENE_BACK_ALLEY_HOUSE:
        case SCENE_BAZAAR:
        case SCENE_KOKIRI_SHOP:
        case SCENE_GORON_SHOP:
        case SCENE_ZORA_SHOP:
        case SCENE_POTION_SHOP_KAKARIKO:
        case SCENE_POTION_SHOP_MARKET:
        case SCENE_BOMBCHU_SHOP:
        case SCENE_HAPPY_MASK_SHOP:
        case SCENE_LINKS_HOUSE:
        case SCENE_DOG_LADY_HOUSE:
        case SCENE_STABLE:
        case SCENE_IMPAS_HOUSE:
        case SCENE_LAKESIDE_LABORATORY:
        case SCENE_CARPENTERS_TENT:
        case SCENE_GRAVEKEEPERS_HUT:
        case SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC:
        case SCENE_FAIRYS_FOUNTAIN:
        case SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS:
        case SCENE_GROTTOS:
        case SCENE_REDEAD_GRAVE:
        case SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN:
        case SCENE_ROYAL_FAMILYS_TOMB:
        case SCENE_SHOOTING_GALLERY:
        case SCENE_TEMPLE_OF_TIME:
        case SCENE_CHAMBER_OF_THE_SAGES:
        case SCENE_CASTLE_COURTYARD_GUARDS_DAY:
        case SCENE_CASTLE_COURTYARD_GUARDS_NIGHT:
        case SCENE_CUTSCENE_MAP:
        case SCENE_WINDMILL_AND_DAMPES_GRAVE:
        case SCENE_CASTLE_COURTYARD_ZELDA:
        case SCENE_BOMBCHU_BOWLING_ALLEY:
        case SCENE_LON_LON_BUILDINGS:
        case SCENE_MARKET_GUARD_HOUSE:
        case SCENE_POTION_SHOP_GRANNY:
        case SCENE_GANON_BOSS:
        case SCENE_HOUSE_OF_SKULLTULA:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LOST_WOODS:
        case SCENE_GORON_CITY:
        case SCENE_OUTSIDE_GANONS_CASTLE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_ID_MAX:
            return false;

        // Time does pass in the fishing pond but it's
        // extremely slow (more than 2 IRL seconds per in-game minute)
        // maybe in the future there could be a trick to count it
        case SCENE_FISHING_POND:
            return false;

        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_ZORAS_RIVER:
        case SCENE_LAKE_HYLIA:
        case SCENE_GERUDO_VALLEY:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_LON_LON_RANCH:
            return true;

        case SCENE_TEST01:
        case SCENE_BESITU:
        case SCENE_DEPTH_TEST:
        case SCENE_SYOTES:
        case SCENE_SYOTES2:
        case SCENE_SUTARU:
        case SCENE_HAIRAL_NIWA2:
        case SCENE_SASATEST:
        case SCENE_TESTROOM:
        default:
            assert(false);
            return false;
    }
}

Region::Region() = default;
Region::Region(std::string regionName_, SceneID scene_, bool timePass_, std::set<RandomizerArea> areas,
               std::vector<EventAccess> events_, std::vector<LocationAccess> locations_,
               std::list<Rando::Entrance> exits_)
    : regionName(std::move(regionName_)), scene(scene_), timePass(timePass_), areas(areas), events(std::move(events_)),
      locations(std::move(locations_)), exits(std::move(exits_)) {
}
Region::Region(std::string regionName_, SceneID scene_, std::vector<EventAccess> events_,
               std::vector<LocationAccess> locations_, std::list<Rando::Entrance> exits_)
    : regionName(std::move(regionName_)), scene(scene_), timePass(GetTimePassFromScene(scene_)),
      areas(CalculateAreas(scene_)), events(std::move(events_)), locations(std::move(locations_)),
      exits(std::move(exits_)) {
}

Region::~Region() = default;

bool Region::TimePass() {
    return timePass;
}

void Region::ApplyTimePass() {
    if (TimePass()) {
        StartPerformanceTimer(PT_TOD_ACCESS);
        if (Child()) {
            childDay = true;
            childNight = true;
            RegionTable(RR_ROOT)->childDay = true;
            RegionTable(RR_ROOT)->childNight = true;
        }
        if (Adult()) {
            adultDay = true;
            adultNight = true;
            RegionTable(RR_ROOT)->adultDay = true;
            RegionTable(RR_ROOT)->adultNight = true;
        }
        StopPerformanceTimer(PT_TOD_ACCESS);
    }
}

bool Region::UpdateEvents() {
    bool eventsUpdated = false;
    StartPerformanceTimer(PT_EVENT_ACCESS);
    for (EventAccess& event : events) {
        // If the event has already happened, there's no reason to check it
        if (event.GetEvent()) {
            continue;
        }

        if ((childDay && event.CheckConditionAtAgeTime(logic->IsChild, logic->AtDay)) ||
            (childNight && event.CheckConditionAtAgeTime(logic->IsChild, logic->AtNight)) ||
            (adultDay && event.CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay)) ||
            (adultNight && event.CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight))) {
            event.EventOccurred();
            eventsUpdated = true;
        }
    }
    StopPerformanceTimer(PT_EVENT_ACCESS);
    return eventsUpdated;
}

void Region::AddExit(RandomizerRegion parentKey, RandomizerRegion newExitKey, ConditionFn condition) {
    Rando::Entrance newExit = Rando::Entrance(newExitKey, condition);
    newExit.SetParentRegion(parentKey);
    exits.push_front(newExit);
}

// The exit will be completely removed from this region
void Region::RemoveExit(Rando::Entrance* exitToRemove) {
    exits.remove_if([exitToRemove](const auto exit) { return &exit == exitToRemove; });
}

void Region::SetAsPrimary(RandomizerRegion exitToBePrimary) {
    for (auto& exit : exits) {
        if (exit.Getuint32_t() == exitToBePrimary) {
            exit.SetAsPrimary();
            return;
        }
    }
}

Rando::Entrance* Region::GetExit(RandomizerRegion exitToReturn) {
    for (auto& exit : exits) {
        if (exit.Getuint32_t() == exitToReturn) {
            return &exit;
        }
    }

    LUSLOG_ERROR("ERROR: EXIT \"%s\" DOES NOT EXIST IN \"%s\"", RegionTable(exitToReturn)->regionName.c_str(),
                 this->regionName.c_str());
    assert(false);
    return nullptr;
}

bool Region::CanPlantBeanCheck(RandomizerGet bean) const {
    auto ctx = Rando::Context::GetInstance();
    auto logic = ctx->GetLogic();
    return logic->HasItem(bean) && logic->GetAmmo(ITEM_BEAN) > 0 &&
           (ctx->GetOption(RSK_SKIP_PLANTING_BEANS) || BothAgesCheck());
}

bool Region::AllAccountedFor() const {
    for (const EventAccess& event : events) {
        if (!event.GetEvent()) {
            return false;
        }
    }

    for (const LocationAccess& loc : locations) {
        if (!(Rando::Context::GetInstance()->GetItemLocation(loc.GetLocation())->IsAddedToPool())) {
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

bool Region::CheckAllAccess(const RandomizerRegion exitKey) {
    if (!AllAccess()) {
        return false;
    }

    for (Rando::Entrance& exit : exits) {
        if (exit.GetConnectedRegionKey() == exitKey) {
            return exit.CheckConditionAtAgeTime(logic->IsChild, logic->AtDay) &&
                   exit.CheckConditionAtAgeTime(logic->IsChild, logic->AtNight) &&
                   exit.CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay) &&
                   exit.CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight);
        }
    }
    return false;
}

void Region::ResetVariables() {
    childDay = false;
    childNight = false;
    adultDay = false;
    adultNight = false;
    addedToPool = false;
    for (auto& exit : exits) {
        exit.RemoveFromPool();
    }
}

void Region::printAgeTimeAccess() {
    auto message = "Child Day:   " + std::to_string(childDay) +
                   "\t"
                   "Child Night: " +
                   std::to_string(childNight) +
                   "\t"
                   "Adult Day:   " +
                   std::to_string(adultDay) +
                   "\t"
                   "Adult Night: " +
                   std::to_string(adultNight);
}

std::array<Region, RR_MAX> areaTable;

/*
    * This logic covers checks that exist in the shared areas of Spirit
    * This code will fail if any glitch allows Adult to go in the Child spirit door first or vice versa as it relies on
    specific ages

    * In order to pass a check, we must either determine that Access is certain,
    or that it is always possible to get a check somehow.

    * But first I have to talk about parallel universes.

    * In the first universe, the player enters spirit as child, and spends as many keys as they can to lock adult out
    * In the second, they enter as adult and spend as many keys as they can to lock child out.

    * Additionally, if it is possible to enter spirit in reverse, there are 2 more universes:
    * In the third universe, adult enters in reverse, and wastes all the keys so noone can enter through the front
    * In the forth, child manages to do the same, and lock people out of the front
    * However all access from the boss door in Statue Room and adjacent areas is Certain, so this is not usually
   relevant

    * While other universes exist, such as both ages entering in reverse or
    child using their key, getting stuck, then coming back to do the dungeon as adult, these
    are all sub-possibilities of these 4 universes

    * As we do not know which universe we are in until the player chooses one in-game,
    we must be able to collect the check in all universes

    * When an Age can no longer be kept out in any universe, that age is said to have Certain Access to a
   region
    * If both ages have potential access to a region with a certain number of keys, but there is no Certain Access,
    * then a check is only in logic if all possible universes can collect the check independently

    * The universes converge when the player has all the keys, giving both ages Certain Access everywhere.

    * We must check for these universes manually as we set access vairables to true with minimum keys for
    * technical reasons as otherwise the logic code will never run

    * The 1st and 3rd column list how many keys are needed for each age to have Certain Access from the front
    * the 2nd and 4th column list how many keys are needed for each age to have Certain Access from the boss door
    * Sometimes, we may check for a higher number of keys in the condition, this happens in cases where the number of
   keys
    * for Certain Access depends on a certain condition, the listed number is the lowest possible to make sure the
   condition is checked.

    * The 1st condition is the combined conditions needed to move from the 1F child lock to the area being checks
    * the 2nd condition is the same for adult 1F lock, and the 3rd is the access from the boss door.
*/

// clang-format off
std::map<RandomizerRegion, SpiritLogicData> Region::spiritLogicData = {
    //Vanilla Child uses ExplosiveKeyLogic here because they need to exist for shared adult checks 
    {RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_1F,       {5, 0, 3, 0, []{return true;},                                                                                          []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)*/;}}},
    {RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_2F,       {5, 0, 3, 0, []{return true/*logic->CanClimbHigh()*/;},                                                                 []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)*/;}}},
    {RR_SPIRIT_TEMPLE_2F_MIRROR_ROOM,        {5, 0, 3, 0, []{return logic->CanUse(RG_HOOKSHOT) && logic->SpiritSunOnFloorToStatue();},                               []{return true/*logic->CanClimbHigh()*/;},                                                                       []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}},
    {RR_SPIRIT_TEMPLE_STATUE_ROOM_CHILD,     {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh()*/;},                                 []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)*/;}}},
    {RR_SPIRIT_TEMPLE_INNER_WEST_HAND,       {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh()*/;},                                 []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)*/;}}},
    {RR_SPIRIT_TEMPLE_GS_LEDGE,              {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic() && logic->SpiritWestToSkull()/* && logic->CanClimbHigh()*/;},   []{return logic->SpiritExplosiveKeyLogic() && logic->SpiritWestToSkull()/* && logic->CanClimbHigh() && str0*/;}, []{return logic->SpiritWestToSkull()/* && (logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS))*/;}}},
    {RR_SPIRIT_TEMPLE_STATUE_ROOM,           {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic();},                                                              []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true;}}},
    //Assumes SpiritSunBlockSouthLedge() for all access
    {RR_SPIRIT_TEMPLE_SUN_BLOCK_CHEST_LEDGE, {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                         []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*((logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)) && str0) || (logic->CanKillEnemy(RE_BEAMOS) && logic->CanUse(RG_LONGSHOT))*/;}}},
    {RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS,      {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                         []{return logic->SpiritExplosiveKeyLogic()/* && logic->CanClimbHigh() && str0*/;},                               []{return true/*((logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)) && str0) || (logic->CanKillEnemy(RE_BEAMOS) && logic->CanUse(RG_LONGSHOT))*/;}}},
    {RR_SPIRIT_TEMPLE_OUTER_RIGHT_HAND,      {5, 5, 3, 3, []{return logic->OuterWestHandLogic();},                                                                   []{return logic->OuterWestHandLogic();},                                                                         []{return logic->OuterWestHandLogic();}}},
    {RR_SPIRIT_TEMPLE_STATUE_ROOM_ADULT,     {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic() && logic->CanUse(RG_HOOKSHOT)/* && logic->CanClimbHigh()*/;},   []{return true/*logic->CanClimbHigh() && str0*/;},                                                               []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}},
    {RR_SPIRIT_TEMPLE_INNER_LEFT_HAND,       {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic() && logic->CanUse(RG_HOOKSHOT)/* && logic->CanClimbHigh()*/;},   []{return true/*logic->CanClimbHigh() && str0*/;},                                                               []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}},
    {RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH,       {5, 0, 3, 0, []{return logic->SpiritExplosiveKeyLogic() && logic->CanUse(RG_HOOKSHOT) && logic->SpiritEastToSwitch();}, []{return logic->SpiritEastToSwitch()/* && logic->CanClimbHigh() && str0*/;},                                    []{return logic->SpiritEastToSwitch() && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}}},
    //MQ                                                              /*&& logic->CanClimbHigh()*/
    {RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE,    {7, 6, 7, 7, []{return logic->StatueRoomMQKeyLogic();},                                                                 []{return logic->SmallKeys(SCENE_SPIRIT_TEMPLE, 6) && logic->CanHitSwitch()/* && logic->Climb*/;},               []{return logic->StatueRoomMQKeyLogic() && logic->CanHitSwitch()/* && (logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS))*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_SUN_ON_FLOOR,       {7, 6, 7, 7, []{return logic->StatueRoomMQKeyLogic() && logic->CanHitSwitch()/* && logic->CanClimbHigh()*/;},           []{return logic->SmallKeys(SCENE_SPIRIT_TEMPLE, 6)/* && logic->Climb*/;},                                        []{return logic->StatueRoomMQKeyLogic()/* && (logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS))*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_CHILD,  {7, 0, 0, 0, []{return logic->CanHitSwitch()/* && logic->CanClimbHigh()*/;},                                            []{return true/*logic->Climb*/;},                                                                                []{return true/*logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS)*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_POT_LEDGE,          {7, 0, 0, 0, []{return logic->CanHitSwitch() && logic->MQSpiritWestToPots()/* && logic->CanClimbHigh()*/;},             []{return logic->MQSpiritWestToPots()/* && logic->Climb*/;},                                                     []{return /*logic->CanUse(RG_HOVER_BOOTS) || (logic->CanClimb() && */logic->MQSpiritWestToPots()/*)*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_INNER_RIGHT_HAND,   {7, 0, 0, 0, []{return logic->CanHitSwitch() && logic->MQSpiritWestToPots()/* && logic->CanClimbHigh()*/;},             []{return logic->MQSpiritWestToPots()/* && logic->Climb*/;},                                                     []{return /*logic->CanUse(RG_HOVER_BOOTS) || (logic->CanClimb() && */logic->MQSpiritWestToPots()/*)*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,        {7, 0, 0, 0, []{return logic->CanHitSwitch()/* && logic->CanClimbHigh()*/;},                                            []{return true;},                                                                                                []{return true;}}},
    {RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM,     {7, 0, 0, 0, []{return logic->CanHitSwitch() && logic->MQSpiritStatueToSunBlock()/* && logic->CanClimbHigh()*/;},       []{return logic->MQSpiritStatueToSunBlock()/* && logic->Climb*/;},                                               []{return logic->MQSpiritStatueToSunBlock()/* && (logic->CanClimb() || logic->CanUse(RG_HOVER_BOOTS))*/;}}},
    {RR_SPIRIT_TEMPLE_MQ_OUTER_RIGHT_HAND,   {7, 7, 4, 4, []{return logic->CanHitSwitch() && logic->OuterWestHandMQLogic()/* && logic->CanClimbHigh() && str0*/;},   []{return logic->OuterWestHandMQLogic();},                                                                       []{return logic->OuterWestHandMQLogic();}}},
    {RR_SPIRIT_TEMPLE_MQ_BIG_BLOCKS_DOOR,    {7, 0, 0, 0, []{return logic->CanHitSwitch() && /* logic->CanClimbHigh() &&*/
                                                                    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCKS_DOOR].AnyAgeTime([]{return logic->MQSpiritStatueSouthDoor();});}, []{return true;},                                                                                      []{return areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCKS_DOOR].AnyAgeTime([]{return logic->MQSpiritStatueSouthDoor();});}}},
};
// clang-format on

bool SpiritCertainAccess(RandomizerRegion region) {
    SpiritLogicData& curRegionData = Region::spiritLogicData[region];
    if (logic->IsChild) {
        uint8_t keys = curRegionData.childKeys;
        uint8_t revKeys = curRegionData.childRevKeys;
        bool knownFrontAccess = logic->Get(LOGIC_FORWARDS_SPIRIT_CHILD) || !logic->IsReverseAccessPossible();
        // If we have enough keys that an age cannot be kept out, we have Certain Access
        // otherwise if we have entered in reverse and can reach from the face, we have Certain Access
        return ((knownFrontAccess && curRegionData.childAccess()) && logic->SmallKeys(SCENE_SPIRIT_TEMPLE, keys)) ||
               ((logic->Get(LOGIC_REVERSE_SPIRIT_CHILD) && curRegionData.reverseAccess()) &&
                logic->SmallKeys(SCENE_SPIRIT_TEMPLE, revKeys)) ||
               (curRegionData.childAccess() && curRegionData.reverseAccess() &&
                logic->SmallKeys(SCENE_SPIRIT_TEMPLE, keys > revKeys ? keys : revKeys));
    } else {
        uint8_t keys = curRegionData.adultKeys;
        uint8_t revKeys = curRegionData.adultRevKeys;
        bool knownFrontAccess = logic->Get(LOGIC_FORWARDS_SPIRIT_ADULT) || !logic->IsReverseAccessPossible();
        // If we have enough keys that an age cannot be kept out, we have Certain Access
        // otherwise if we have entered in reverse and can reach from the face, we have Certain Access
        return ((knownFrontAccess && curRegionData.adultAccess()) && logic->SmallKeys(SCENE_SPIRIT_TEMPLE, keys)) ||
               ((logic->Get(LOGIC_FORWARDS_SPIRIT_ADULT) && curRegionData.reverseAccess()) &&
                logic->SmallKeys(SCENE_SPIRIT_TEMPLE, revKeys)) ||
               (curRegionData.adultAccess() && curRegionData.reverseAccess() &&
                logic->SmallKeys(SCENE_SPIRIT_TEMPLE, keys > revKeys ? keys : revKeys));
    }
}

/*
    Spirit Shared can take up to 3 regions, this is because checks can exist in many regions at the same time
    and the logic needs to be able to check the access logic from those regions to check the other universes properly.

    anyAge is equivalent to a self referencing Here, used for events and any check where that is relevent.
*/

bool SpiritShared(RandomizerRegion region, ConditionFn condition, bool anyAge, RandomizerRegion otherRegion,
                  ConditionFn otherCondition, RandomizerRegion thirdRegion, ConditionFn thirdCondition) {
    SpiritLogicData& curRegionData = Region::spiritLogicData[region];
    bool result = false;

    // store current age variables
    bool pastAdult = logic->IsAdult;
    bool pastChild = logic->IsChild;

    logic->IsChild = true;
    logic->IsAdult = false;

    bool ChildCertainAccess = SpiritCertainAccess(region);

    // Switch back to adult to check adult access
    logic->IsChild = false;
    logic->IsAdult = true;

    bool AdultCertainAccess = SpiritCertainAccess(region);
    // If we are AnyAge and have any CertainAccess, then we can check those ages
    // we don't need to check ambiguity here as if this fails, then 1 of the ages has failed
    if (anyAge && (ChildCertainAccess || AdultCertainAccess)) {
        // set age access to the Certain Access
        logic->IsChild = ChildCertainAccess;
        logic->IsAdult = AdultCertainAccess;

        // check condition as well as having at least child or adult access
        result = condition();

        // otherwise, we have to check the current age and...
    } else if (areaTable[region].Child() && pastChild) {
        // Switch to Child
        logic->IsChild = true;
        logic->IsAdult = false;

        result = condition();
        // If we have Certain Access, we just run the condition.
        // Otherwise, if we have the keys to know either age can reach, we need to see if we could reach as Adult
        // and if needed, in reverse
        if (!ChildCertainAccess && result) {
            // Switch to Adult
            logic->IsChild = false;
            logic->IsAdult = true;

            // If Adult can get there and get the check, we can get the check in logic
            // If reverse spirit is also possible, we need to make sure Adult can get it via reverse entry too
            result = (curRegionData.adultAccess() &&
                      (!logic->IsReverseAccessPossible() || curRegionData.reverseAccess) && condition()) ||
                     (otherRegion != RR_NONE &&
                      (Region::spiritLogicData[otherRegion].adultAccess() &&
                       (!logic->IsReverseAccessPossible() || Region::spiritLogicData[otherRegion].reverseAccess()) &&
                       otherCondition())) ||
                     (thirdRegion != RR_NONE &&
                      (Region::spiritLogicData[thirdRegion].adultAccess() &&
                       (!logic->IsReverseAccessPossible() || Region::spiritLogicData[thirdRegion].reverseAccess()) &&
                       thirdCondition()));
        }
    } else if (areaTable[region].Adult() && pastAdult) {
        result = condition();
        // if we have enough keys to have Certain Access, we just run the condition
        // Alternatively, if we have entered in reverse and can reach from the face, we have Certain Access
        // Otherwise, if we have the keys to know either age can reach, we need to see if we could reach as Child
        // and if needed, in reverse
        if (!AdultCertainAccess && result) {
            // Switch to Child
            logic->IsChild = true;
            logic->IsAdult = false;

            // If Child can get there and get the check, we can get the check in logic
            // If reverse spirit is also possible, we need to make sure Child can get it via reverse entry too
            result = (curRegionData.childAccess() &&
                      (!logic->IsReverseAccessPossible() || curRegionData.reverseAccess()) && condition()) ||
                     (otherRegion != RR_NONE &&
                      (Region::spiritLogicData[otherRegion].childAccess() &&
                       (!logic->IsReverseAccessPossible() || Region::spiritLogicData[otherRegion].reverseAccess()) &&
                       otherCondition())) ||
                     (thirdRegion != RR_NONE &&
                      (Region::spiritLogicData[thirdRegion].childAccess() &&
                       (!logic->IsReverseAccessPossible() || Region::spiritLogicData[thirdRegion].reverseAccess()) &&
                       thirdCondition()));
        }
    }
    // set back age variables
    logic->IsChild = pastChild;
    logic->IsAdult = pastAdult;
    return result;
}

bool AnyAgeTime(ConditionFn condition) {
    assert(logic->CurrentRegionKey != RR_NONE);
    return areaTable[logic->CurrentRegionKey].AnyAgeTime(condition);
}

bool BeanPlanted(const RandomizerGet bean) {
    auto logic = Rando::Context::GetInstance()->GetLogic();
    // flag irrelevant if plant won't spawn
    if (!logic->HasItem(bean)) {
        return false;
    } else if (ctx->GetOption(RSK_SKIP_PLANTING_BEANS) && ctx->GetOption(RSK_STARTING_BEANS)) {
        return true;
    }

    // swchFlag found using the Actor Viewer to get the Obj_Bean parameters & 0x3F
    // not tested with multiple OTRs, but can be automated similarly to GetDungeonSmallKeyDoors
    SceneID sceneID;
    uint8_t swchFlag;
    switch (bean) {
        case RG_ZORAS_RIVER_BEAN_SOUL:
            sceneID = SceneID::SCENE_ZORAS_RIVER;
            swchFlag = 3;
            break;
        case RG_GRAVEYARD_BEAN_SOUL:
            sceneID = SceneID::SCENE_GRAVEYARD;
            swchFlag = 3;
            break;
        case RG_KOKIRI_FOREST_BEAN_SOUL:
            sceneID = SceneID::SCENE_KOKIRI_FOREST;
            swchFlag = 9;
            break;
        case RG_LOST_WOODS_BRIDGE_BEAN_SOUL:
            sceneID = SceneID::SCENE_LOST_WOODS;
            swchFlag = 4;
            break;
        case RG_LOST_WOODS_BEAN_SOUL:
            sceneID = SceneID::SCENE_LOST_WOODS;
            swchFlag = 18;
            break;
        case RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL:
            sceneID = SceneID::SCENE_DEATH_MOUNTAIN_TRAIL;
            swchFlag = 6;
            break;
        case RG_LAKE_HYLIA_BEAN_SOUL:
            sceneID = SceneID::SCENE_LAKE_HYLIA;
            swchFlag = 1;
            break;
        case RG_GERUDO_VALLEY_BEAN_SOUL:
            sceneID = SceneID::SCENE_GERUDO_VALLEY;
            swchFlag = 3;
            break;
        case RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL:
            sceneID = SceneID::SCENE_DEATH_MOUNTAIN_CRATER;
            swchFlag = 3;
            break;
        case RG_DESERT_COLOSSUS_BEAN_SOUL:
            sceneID = SceneID::SCENE_DESERT_COLOSSUS;
            swchFlag = 24;
            break;
        default:
            sceneID = SCENE_ID_MAX;
            swchFlag = 0;
            assert(false);
            break;
    }

    // Get the swch value for the scene
    uint32_t swch;
    if (gPlayState != nullptr && gPlayState->sceneNum == sceneID) {
        swch = gPlayState->actorCtx.flags.swch;
    } else if (sceneID != SCENE_ID_MAX) {
        swch = logic->GetSaveContext()->sceneFlags[sceneID].swch;
    } else {
        swch = 0;
    }

    return swch >> swchFlag & 1;
}

bool CanPlantBean(const RandomizerRegion region, const RandomizerGet bean) {
    return areaTable[region].CanPlantBeanCheck(bean) || BeanPlanted(bean);
}

bool BothAges(const RandomizerRegion region) {
    return areaTable[region].BothAgesCheck();
}

bool ChildCanAccess(const RandomizerRegion region) {
    return areaTable[region].Child();
}

bool AdultCanAccess(const RandomizerRegion region) {
    return areaTable[region].Adult();
}

Rando::Context* ctx;
std::shared_ptr<Rando::Logic> logic;

void RegionTable_Init() {
    using namespace Rando;
    ctx = Context::GetInstance().get();
    logic = ctx->GetLogic(); // RANDOTODO do not hardcode, instead allow accepting a Logic class somehow
    grottoEvents = {
        EventAccess(LOGIC_FAIRY_ACCESS, [] { return logic->CallGossipFairy() || logic->CanUse(RG_STICKS); }),
        EventAccess(LOGIC_BUG_ACCESS, [] { return logic->CanCutShrubs(); }),
        EventAccess(LOGIC_FISH_ACCESS, [] { return true; }),
    };
    // Clear the array from any previous playthrough attempts. This is important so that
    // locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
    areaTable.fill(Region("Invalid Region", SCENE_ID_MAX, {}, {}, {}));

    RegionTable_Init_Root();
    // Overworld
    RegionTable_Init_KokiriForest();
    RegionTable_Init_LostWoods();
    RegionTable_Init_SacredForestMeadow();
    RegionTable_Init_HyruleField();
    RegionTable_Init_LakeHylia();
    RegionTable_Init_LonLonRanch();
    RegionTable_Init_Market();
    RegionTable_Init_TempleOfTime();
    RegionTable_Init_CastleGrounds();
    RegionTable_Init_Kakariko();
    RegionTable_Init_Graveyard();
    RegionTable_Init_DeathMountainTrail();
    RegionTable_Init_GoronCity();
    RegionTable_Init_DeathMountainCrater();
    RegionTable_Init_ZoraRiver();
    RegionTable_Init_ZorasDomain();
    RegionTable_Init_ZorasFountain();
    RegionTable_Init_GerudoValley();
    RegionTable_Init_GerudoFortress();
    RegionTable_Init_ThievesHideout();
    RegionTable_Init_HauntedWasteland();
    RegionTable_Init_DesertColossus();
    // Dungeons
    RegionTable_Init_DekuTree();
    RegionTable_Init_DodongosCavern();
    RegionTable_Init_JabuJabusBelly();
    RegionTable_Init_ForestTemple();
    RegionTable_Init_FireTemple();
    RegionTable_Init_WaterTemple();
    RegionTable_Init_SpiritTemple();
    RegionTable_Init_ShadowTemple();
    RegionTable_Init_BottomOfTheWell();
    RegionTable_Init_IceCavern();
    RegionTable_Init_GerudoTrainingGround();
    RegionTable_Init_GanonsCastle();

    // Set parent regions
    for (uint32_t i = RR_ROOT; i < RR_MAX; i++) {
        areaTable[i].randomizerRegionKey = (RandomizerRegion)i;
        for (LocationAccess& locPair : areaTable[i].locations) {
            RandomizerCheck location = locPair.GetLocation();
            Rando::Context::GetInstance()->GetItemLocation(location)->SetParentRegion((RandomizerRegion)i);
        }
        for (Entrance& exit : areaTable[i].exits) {
            exit.SetParentRegion((RandomizerRegion)i);
            exit.SetName();
            exit.GetConnectedRegion()->entrances.push_front(&exit);
        }
    }
}

void ReplaceFirstInString(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
    size_t pos = s.find(toReplace);
    if (pos == std::string::npos) {
        return;
    }
    s.replace(pos, toReplace.length(), replaceWith);
}

void ReplaceAllInString(std::string& s, std::string const& toReplace, std::string const& replaceWith) {
    std::string buf;
    size_t pos = 0;
    size_t prevPos;

    buf.reserve(s.size());

    while (true) {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == std::string::npos) {
            break;
        }
        buf.append(s, prevPos, pos - prevPos);
        buf += replaceWith;
        pos += toReplace.size();
    }

    buf.append(s, prevPos, s.size() - prevPos);
    s.swap(buf);
}

std::string CleanCheckConditionString(std::string condition) {
    ReplaceAllInString(condition, "logic->", "");
    ReplaceAllInString(condition, "ctx->", "");
    ReplaceAllInString(condition, ".Get()", "");
    ReplaceAllInString(condition, "GetSaveContext()->", "");
    return condition;
}

namespace Regions {
auto GetAllRegions() {
    static const size_t regionCount = RR_MAX - (RR_NONE + 1);

    static std::array<RandomizerRegion, regionCount> allRegions = {};

    static bool initialized = false;
    if (!initialized) {
        for (size_t i = 0; i < regionCount; i++) {
            allRegions[i] = (RandomizerRegion)((RR_NONE + 1) + i);
        }
        initialized = true;
    }

    return allRegions;
}

void AccessReset() {
    auto ctx = Rando::Context::GetInstance();
    for (const RandomizerRegion region : GetAllRegions()) {
        RegionTable(region)->ResetVariables();
    }

    if (/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_CHILD)) {
            RegionTable(RR_ROOT)->childNight = true;
        } else {
            RegionTable(RR_ROOT)->adultNight = true;
        }
    } else {
        if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_CHILD)) {
            RegionTable(RR_ROOT)->childDay = true;
        } else {
            RegionTable(RR_ROOT)->adultDay = true;
        }
    }
}

// Reset exits and clear items from locations
void ResetAllLocations() {
    auto ctx = Rando::Context::GetInstance();
    for (const RandomizerRegion region : GetAllRegions()) {
        RegionTable(region)->ResetVariables();
        // Erase item from every location in this exit
        for (LocationAccess& locPair : RegionTable(region)->locations) {
            RandomizerCheck location = locPair.GetLocation();
            Rando::Context::GetInstance()->GetItemLocation(location)->ResetVariables();
        }
    }

    if (/*Settings::HasNightStart TODO:: Randomize Starting Time*/ false) {
        if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_CHILD)) {
            RegionTable(RR_ROOT)->childNight = true;
        } else {
            RegionTable(RR_ROOT)->adultNight = true;
        }
    } else {
        if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_CHILD)) {
            RegionTable(RR_ROOT)->childDay = true;
        } else {
            RegionTable(RR_ROOT)->adultDay = true;
        }
    }
}

bool HasTimePassAccess(uint8_t age) {
    for (const RandomizerRegion regionKey : GetAllRegions()) {
        auto region = RegionTable(regionKey);
        if (region->TimePass() &&
            ((age == RO_AGE_CHILD && region->Child()) || (age == RO_AGE_ADULT && region->Adult()))) {
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
    worldGraph.open(str + ".dot");
    worldGraph << "digraph {\n\tcenter=true;\n";

    for (const RandomizerRegion regionKey : GetAllRegions()) {
        auto region = RegionTable(regionKey);
        for (auto exit : region->exits) {
            if (exit.GetConnectedRegion()->regionName != "Invalid Region") {
                std::string parent = exit.GetParentRegion()->regionName;
                if (region->childDay) {
                    parent += " CD";
                }
                if (region->childNight) {
                    parent += " CN";
                }
                if (region->adultDay) {
                    parent += " AD";
                }
                if (region->adultNight) {
                    parent += " AN";
                }
                Region* connected = exit.GetConnectedRegion();
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
} // namespace Regions

Region* RegionTable(const RandomizerRegion regionKey) {
    if (regionKey > RR_MAX) {
        printf("\x1b[1;1HERROR: AREAKEY TOO BIG");
    }
    return &(areaTable[regionKey]);
}

// Retrieve all the shuffable entrances of a specific type
std::vector<Rando::Entrance*> GetShuffleableEntrances(Rando::EntranceType type, bool onlyPrimary /*= true*/) {
    std::vector<Rando::Entrance*> entrancesToShuffle = {};
    for (RandomizerRegion region : Regions::GetAllRegions()) {
        for (auto& exit : RegionTable(region)->exits) {
            if ((exit.GetType() == type || type == Rando::EntranceType::All) && (exit.IsPrimary() || !onlyPrimary) &&
                exit.GetType() != Rando::EntranceType::None) {
                entrancesToShuffle.push_back(&exit);
            }
        }
    }
    return entrancesToShuffle;
}

Rando::Entrance* GetEntrance(RandomizerRegion source, RandomizerRegion destination) {
    for (auto& exit : RegionTable(source)->exits) {
        if (exit.GetOriginalConnectedRegionKey() == destination) {
            return &exit;
        }
    }

    return nullptr;
}
