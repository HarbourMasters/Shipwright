#include "location_access.h"

#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/context.h"
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

    return conditionsMet && CanBuy(calculatingAvailableChecks);
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

bool LocationAccess::CanBuy(bool calculatingAvailableChecks) const {
    const auto& loc = Rando::StaticData::GetLocation(location);
    const auto& itemLoc = OTRGlobals::Instance->gRandoContext->GetItemLocation(location);

    if (loc->GetRCType() == RCTYPE_SHOP || loc->GetRCType() == RCTYPE_SCRUB || loc->GetRCType() == RCTYPE_MERCHANT) {
        // Checks should only be identified while playing
        if (calculatingAvailableChecks && itemLoc->GetCheckStatus() != RCSHOW_IDENTIFIED) {
            return CanBuyAnother(GetMinimumPrice(loc));
        } else {
            return CanBuyAnother(itemLoc->GetPrice());
        }
    }

    return true;
}

bool CanBuyAnother(RandomizerCheck rc) {
    return CanBuyAnother(ctx->GetItemLocation(rc)->GetPrice());
}

bool CanBuyAnother(uint16_t price) {
    if (price > 500) {
        return logic->HasItem(RG_TYCOON_WALLET);
    } else if (price > 200) {
        return logic->HasItem(RG_GIANT_WALLET);
    } else if (price > 99) {
        return logic->HasItem(RG_ADULT_WALLET);
    } else if (price > 0) {
        return logic->HasItem(RG_CHILD_WALLET);
    }
    return true;
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

bool Region::CanPlantBeanCheck() const {
    return Rando::Context::GetInstance()->GetLogic()->GetAmmo(ITEM_BEAN) > 0 && BothAgesCheck();
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

/*
 * This logic covers checks that exist in the shared areas of MQ spirit from a glitchless standpoint.
 * This room has Quantum logic that I am currently handling with this function, however this is NOT suitable for
 glitch logic as it relies on specific ages
 * In this chunk there are 3 possibilities for passing a check, but first I have to talk about parallel universes.

 * In MQ Spirit key logic, we mostly care about 2 possibilities for how the player can spend keys, creating 2
 Parralel universes
 * In the first universe, the player did not enter spirit as adult until after climbing as child, thus child spends
 keys linearly, only needing 2 to reach statue room.
 * In the second universe, the player went in as adult, possibly out of logic, and started wasting the keys to lock
 child out.
 * These Universes converge when the player has 7 keys (meaning adult can no longer lock child out) and adult is
 known to be able to reach Statue room. This creates "Certain Access", which is tracked seperatly for each age.
 * Child Certain Access is simple, if we have 7 keys and child access, it's Certain Access.
 * Adult Certain Access is also simple, adult is not key locked, so if they make it to a location, it's Certain
 Access.
 * Things get complicated when we handle the overlap of the 2 universes,
 * though an important detail is that if we have Certain Access as either age, we don't need to checked the overlap
 because overlap logic is strictly stricter than either Certain Access.

 * In order to track the first universe, the logic allows technical child access with the minimum number of keys,
 and then checks in this function for if we have 7 keys to determine if that is Certain or not.
 * This is for technical reasons, as areas with no access at all will simply not be checked.
 * Normally we would need to do similar shenanigans to track the second universe, however adult must have go through
 statue room to waste keys,
 * so can go back there and get new keys for Child to use if they do, and the navigation logic for shared MQ spirit
 from Statue Room is very simple for Adult.
 * Additionally, we don't need to know if adult can actually reach spirit temple or climb to statue room, because if
 the player can't do that, then universe 2 can't happen anyway,
 * and if the player does so out of logic, they can do it again, as the only consumable used sets a permanent flag.

 * The Adult Navigation logic is as such:
 * - Broken Wall room is 6 key locked, because if the player tries to spend 6 keys in a way that would block adults
 access, they would have to give child access instead.
 * - The child side hammer switch for the time travelling chest is 7 key locked for adult
 * - Reaching gauntlets hand is 7 key locked
 * - Going back into big block room is complex, but the only check there is child only so not a concern
 * - Everything else is possible with basic adult movement, or is impossible for child to reach glitchlessly
 * Anything 7 key locked does not need to be checked as shared, as all child access is Certain and because of this
 workaround we don't need to fake Adult access, meaning that is also Certain.
 * All of this combined means that when checking if adult can reach a location in universe 2, we only have to ask if
 it is a 6 key locked location or not.

 * Knowing all of this this, we can confirm things are logical in 3 different ways:
 * - If we have Adult Access, we know it is Certain Access, so they can get checks alone.
 * - If we have 7 keys, child has Certain Access as we know they cannot be locked out, so can get checks alone,
 otherwise we check the logical overlap
 * - If Child and Adult can get the check (ignoring actual adult access to the location), and the location is either
 not 6 key locked or we have 6 keys, we can get the check with the overlap
 */
bool Region::MQSpiritShared(ConditionFn condition, bool IsBrokenWall, bool anyAge) {
    // if we have Certain Access as child, we can check anyAge and if true, resolve a condition with Here as if
    // adult is here it's also Certain Access
    if (logic->SmallKeys(SCENE_SPIRIT_TEMPLE, 7)) {
        if (anyAge) {
            return Here(condition);
        }
        return condition();
        // else, if we are here as adult, we have Certain Access from that and don't need special handling for
        // checking adult
    } else if (Adult() && logic->IsAdult) {
        return condition();
        // if we do not have Certain Access, we need to check the overlap by seeing if we are both here as child and
        // meet the adult universe's access condition. We only need to do it as child, as only child access matters
        // for this check, as adult access is assumed based on keys
    } else if (Child() && logic->IsChild && (!IsBrokenWall || logic->SmallKeys(SCENE_SPIRIT_TEMPLE, 6))) {
        // store current age variables
        bool pastAdult = logic->IsAdult;
        bool pastChild = logic->IsChild;

        // First check if the check is possible as child
        logic->IsChild = true;
        logic->IsAdult = false;
        bool result = condition();
        // If so, check again as adult. both have to be true for result to be true
        if (result) {
            logic->IsChild = false;
            logic->IsAdult = true;
            result = condition();
        }

        // set back age variables
        logic->IsChild = pastChild;
        logic->IsAdult = pastAdult;
        return result;
    }
    return false;
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

bool Here(const RandomizerRegion region, ConditionFn condition) {
    return areaTable[region].Here(condition);
}

bool MQSpiritSharedStatueRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge) {
    return areaTable[region].MQSpiritShared(condition, false, anyAge);
}

bool MQSpiritSharedBrokenWallRoom(const RandomizerRegion region, ConditionFn condition, bool anyAge) {
    return areaTable[region].MQSpiritShared(condition, true, anyAge);
}

bool BeanPlanted(const RandomizerRegion region) {
    // swchFlag found using the Actor Viewer to get the Obj_Bean parameters & 0x3F
    // not tested with multiple OTRs, but can be automated similarly to GetDungeonSmallKeyDoors
    SceneID sceneID;
    uint8_t swchFlag;
    switch (region) {
        case RR_ZORAS_RIVER:
            sceneID = SceneID::SCENE_ZORAS_RIVER;
            swchFlag = 3;
            break;
        case RR_THE_GRAVEYARD:
            sceneID = SceneID::SCENE_GRAVEYARD;
            swchFlag = 3;
            break;
        case RR_KOKIRI_FOREST:
            sceneID = SceneID::SCENE_KOKIRI_FOREST;
            swchFlag = 9;
            break;
        case RR_THE_LOST_WOODS:
            sceneID = SceneID::SCENE_LOST_WOODS;
            swchFlag = 4;
            break;
        case RR_LW_BEYOND_MIDO:
            sceneID = SceneID::SCENE_LOST_WOODS;
            swchFlag = 18;
            break;
        case RR_DEATH_MOUNTAIN_TRAIL:
            sceneID = SceneID::SCENE_DEATH_MOUNTAIN_TRAIL;
            swchFlag = 6;
            break;
        case RR_LAKE_HYLIA:
            sceneID = SceneID::SCENE_LAKE_HYLIA;
            swchFlag = 1;
            break;
        case RR_GERUDO_VALLEY:
            sceneID = SceneID::SCENE_GERUDO_VALLEY;
            swchFlag = 3;
            break;
        case RR_DMC_CENTRAL_LOCAL:
            sceneID = SceneID::SCENE_DEATH_MOUNTAIN_CRATER;
            swchFlag = 3;
            break;
        case RR_DESERT_COLOSSUS:
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
        swch = Rando::Context::GetInstance()->GetLogic()->GetSaveContext()->sceneFlags[sceneID].swch;
    } else {
        swch = 0;
    }

    return swch >> swchFlag & 1;
}

bool CanPlantBean(const RandomizerRegion region) {
    return areaTable[region].CanPlantBeanCheck() || BeanPlanted(region);
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

bool HasAccessTo(const RandomizerRegion region) {
    return areaTable[region].HasAccess();
}

Rando::Context* ctx;
std::shared_ptr<Rando::Logic> logic;

void RegionTable_Init() {
    using namespace Rando;
    ctx = Context::GetInstance().get();
    logic = ctx->GetLogic(); // RANDOTODO do not hardcode, instead allow accepting a Logic class somehow
    grottoEvents = {
        EventAccess(&logic->GossipStoneFairy, [] { return logic->CallGossipFairy(); }),
        EventAccess(&logic->ButterflyFairy, [] { return logic->CanUse(RG_STICKS); }),
        EventAccess(&logic->BugShrub, [] { return logic->CanCutShrubs(); }),
        EventAccess(&logic->LoneFish, [] { return true; }),
    };
    // Clear the array from any previous playthrough attempts. This is important so that
    // locations which appear in both MQ and Vanilla dungeons don't get set in both areas.
    areaTable.fill(Region("Invalid Region", SCENE_ID_MAX, {}, {}, {}));

    // clang-format off
    areaTable[RR_ROOT] = Region("Root", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {
        //Events
        EventAccess(&logic->KakarikoVillageGateOpen,        []{return ctx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN);}),
        EventAccess(&logic->THCouldFree1TorchCarpenter,     []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE);}),
        EventAccess(&logic->THCouldFreeDoubleCellCarpenter, []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(&logic->TH_CouldFreeDeadEndCarpenter,   []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(&logic->THCouldRescueSlopeCarpenter,    []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST);}),
        EventAccess(&logic->THRescuedAllCarpenters,         []{return ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE);}),EventAccess(&logic->FreedEpona,              []{return (bool)ctx->GetOption(RSK_SKIP_EPONA_RACE);}),
    }, {
        //Locations
        LOCATION(RC_LINKS_POCKET,       true),
        LOCATION(RC_TRIFORCE_COMPLETED, logic->GetSaveContext()->ship.quest.data.randomizer.triforcePiecesCollected >= ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).Get() + 1;),
        LOCATION(RC_SARIA_SONG_HINT,    logic->CanUse(RG_SARIAS_SONG)),
        LOCATION(RC_SONG_FROM_IMPA,     (bool)ctx->GetOption(RSK_SKIP_CHILD_ZELDA)),
        LOCATION(RC_TOT_MASTER_SWORD,   (bool)ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_ADULT)),
    }, {
        //Exits
        Entrance(RR_ROOT_EXITS, []{return true;}),
    });

    areaTable[RR_ROOT_EXITS] = Region("Root Exits", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_CHILD_SPAWN,             []{return logic->IsChild;}),
        Entrance(RR_ADULT_SPAWN,             []{return logic->IsAdult;}),
        Entrance(RR_MINUET_OF_FOREST_WARP,   []{return logic->CanUse(RG_MINUET_OF_FOREST);}),
        Entrance(RR_BOLERO_OF_FIRE_WARP,     []{return logic->CanUse(RG_BOLERO_OF_FIRE);}),
        Entrance(RR_SERENADE_OF_WATER_WARP,  []{return logic->CanUse(RG_SERENADE_OF_WATER);}),
        Entrance(RR_NOCTURNE_OF_SHADOW_WARP, []{return logic->CanUse(RG_NOCTURNE_OF_SHADOW);}),
        Entrance(RR_REQUIEM_OF_SPIRIT_WARP,  []{return logic->CanUse(RG_REQUIEM_OF_SPIRIT);}),
        Entrance(RR_PRELUDE_OF_LIGHT_WARP,   []{return logic->CanUse(RG_PRELUDE_OF_LIGHT);}),
    });

    areaTable[RR_CHILD_SPAWN] = Region("Child Spawn", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_KF_LINKS_HOUSE, []{return true;}),
    });

    areaTable[RR_ADULT_SPAWN] = Region("Adult Spawn", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });

    areaTable[RR_MINUET_OF_FOREST_WARP] = Region("Minuet of Forest Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_SACRED_FOREST_MEADOW, []{return true;}),
    });

    areaTable[RR_BOLERO_OF_FIRE_WARP] = Region("Bolero of Fire Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_DMC_CENTRAL_LOCAL, []{return true;}),
    });

    areaTable[RR_SERENADE_OF_WATER_WARP] = Region("Serenade of Water Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_LAKE_HYLIA, []{return true;}),
    });

    areaTable[RR_REQUIEM_OF_SPIRIT_WARP] = Region("Requiem of Spirit Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_DESERT_COLOSSUS, []{return true;}),
    });

    areaTable[RR_NOCTURNE_OF_SHADOW_WARP] = Region("Nocturne of Shadow Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_GRAVEYARD_WARP_PAD_REGION, []{return true;}),
    });

    areaTable[RR_PRELUDE_OF_LIGHT_WARP] = Region("Prelude of Light Warp", SCENE_ID_MAX, TIME_DOESNT_PASS, {RA_LINKS_POCKET}, {}, {}, {
        //Exits
        Entrance(RR_TEMPLE_OF_TIME, []{return true;}),
    });

    // clang-format on

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
