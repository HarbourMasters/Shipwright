#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_LonLonRanch() {
    // clang-format off
    areaTable[RR_LON_LON_RANCH] = Region("Lon Lon Ranch", SCENE_LON_LON_RANCH, {
        //Events
        EVENT_ACCESS(LOGIC_FREED_EPONA, logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay),
        EVENT_ACCESS(LOGIC_LINKS_COW,   logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay),
    }, {
        //Locations
        LOCATION(RC_SONG_FROM_MALON,     logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER) && logic->HasItem(RG_FAIRY_OCARINA) && logic->AtDay),
        LOCATION(RC_LLR_GS_TREE,         logic->IsChild && logic->CanBonkTrees() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_LLR_GS_RAIN_SHED,    logic->IsChild && logic->CanGetNightTimeGS() && (logic->HasItem(RG_POWER_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA))),
        LOCATION(RC_LLR_GS_HOUSE_WINDOW, logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_LLR_GS_BACK_WALL,    logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
        LOCATION(RC_LLR_FRONT_POT_1,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_2,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_3,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_FRONT_POT_4,     logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_1, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_2, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_RAIN_SHED_POT_3, logic->IsChild && logic->CanBreakPots()),
        LOCATION(RC_LLR_NEAR_TREE_CRATE, logic->IsChild && logic->CanBreakCrates()),
        LOCATION(RC_LLR_TREE,            logic->IsChild && logic->CanBonkTrees()),
    }, {
        //Exits
        ENTRANCE(RR_HYRULE_FIELD,     true),
        ENTRANCE(RR_LLR_TALONS_HOUSE, logic->CanOpenOverworldDoor(RG_TALONS_HOUSE_KEY)),
        ENTRANCE(RR_LLR_STABLES,      logic->CanOpenOverworldDoor(RG_STABLES_KEY)),
        ENTRANCE(RR_LLR_TOWER,        logic->CanOpenOverworldDoor(RG_BACK_TOWER_KEY)),
        ENTRANCE(RR_LLR_GROTTO,       logic->IsChild),
    });

    areaTable[RR_LLR_TALONS_HOUSE] = Region("LLR Talons House", SCENE_LON_LON_BUILDINGS, {}, {
        //Locations
        LOCATION(RC_LLR_TALONS_CHICKENS,    logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN) && logic->IsChild && logic->AtDay && logic->HasItem(RG_ZELDAS_LETTER) && logic->HasItem(RG_POWER_BRACELET)),
        LOCATION(RC_LLR_TALONS_HOUSE_POT_1, logic->HasItem(RG_POWER_BRACELET) || logic->CanUseSword()), // TODO: CanBreakPots() restricted
        LOCATION(RC_LLR_TALONS_HOUSE_POT_2, logic->HasItem(RG_POWER_BRACELET) || logic->CanUseSword()), // TODO: CanBreakPots() restricted
        LOCATION(RC_LLR_TALONS_HOUSE_POT_3, logic->HasItem(RG_POWER_BRACELET) || logic->CanUseSword()), // TODO: CanBreakPots() restricted
    }, {
        //Exits
        ENTRANCE(RR_LON_LON_RANCH, true),
    });

    areaTable[RR_LLR_STABLES] = Region("LLR Stables", SCENE_STABLE, {}, {
        //Locations
        LOCATION(RC_LLR_STABLES_LEFT_COW,  logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_LLR_STABLES_RIGHT_COW, logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_LON_LON_RANCH, true),
    });

    areaTable[RR_LLR_TOWER] = Region("LLR Tower", SCENE_LON_LON_BUILDINGS, {}, {
        //Locations
        LOCATION(RC_LLR_FREESTANDING_POH, logic->IsChild && logic->HasItem(RG_POWER_BRACELET) && logic->HasItem(RG_CRAWL)),
        LOCATION(RC_LLR_TOWER_LEFT_COW,   logic->CanUse(RG_EPONAS_SONG)),
        LOCATION(RC_LLR_TOWER_RIGHT_COW,  logic->CanUse(RG_EPONAS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_LON_LON_RANCH, true),
    });

    areaTable[RR_LLR_GROTTO] = Region("LLR Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_LLR_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_LON_LON_RANCH, true),
    });

    // clang-format on
}
