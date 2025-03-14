#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_GerudoFortress() {
#pragma region Ground

    areaTable[RR_GF_GROUND_BOTTOM] = Region("Gerudo Fortress", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GF_GateOpen,  []{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    }, {}, {
        //Exits
        Entrance(RR_GV_FORTRESS_SIDE, []{return true;}),
        Entrance(RR_GF_OUTSIDE_GATE,  []{return logic->GF_GateOpen;}),
        // RANDTODO: Add tricks for getting past the gerudo guarding the hba range
        Entrance(RR_GF_GROUND_RED,    []{return true;}),
        Entrance(RR_GF_GROUND_GREY,   []{return true;}),
    });

    areaTable[RR_GF_GROUND_RED] = Region("GF Ground Red", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_NORTH_F1_CARPENTER_AREA, []{return true;}),
        Entrance(RR_GF_GROUND_GREY,             []{return true;}),
        Entrance(RR_GF_GROUND_BOTTOM,           []{return true;}),
    });

    areaTable[RR_GF_GROUND_GREY] = Region("GF Ground Grey", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_GROUND_RED,    []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_GROUND_ORANGE, []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_GROUND_SALMON, []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_GROUND_BOTTOM, []{return true;}),
    });

    areaTable[RR_GF_GROUND_ORANGE] = Region("GF Ground Orange", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_KITCHEN_BOTTOM,          []{return true;}),
        Entrance(RR_TH_NORTH_F1_CARPENTER_AREA, []{return true;}),
        Entrance(RR_TH_SOUTH_F1_CARPENTER_AREA, []{return true;}),
        Entrance(RR_GF_STORMS_GROTTO,           []{return logic->IsAdult && logic->CanOpenStormsGrotto();}),
        Entrance(RR_GF_GROUND_GREY,             []{return true;}),
    });

    areaTable[RR_GF_GROUND_SALMON] = Region("GF Ground Orange", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GtG_GateOpen, []{return (logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->HasItem(RG_CHILD_WALLET));}),
    }, {}, {
        //Exits
        Entrance(RR_GERUDO_TRAINING_GROUND_ENTRYWAY, []{return logic->GtG_GateOpen && (logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES));}),
        Entrance(RR_GF_GROUND_GREY,                  []{return true;}),
        Entrance(RR_GF_HBA_RANGE,                    []{return logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
        Entrance(RR_GF_GROUND_BOTTOM,                []{return true;}),
        Entrance(RR_GF_ROOFTOP_WHITE,                []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
    });

#pragma endregion

#pragma region Rooftops

    areaTable[RR_GF_ROOFTOP_YELLOW] = Region("GF Rooftop Yellow", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_BREAK_ROOM, []{return true;}),
        Entrance(RR_GF_GROUND_RED, []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_LIMEGREEN] = Region("GF Rooftop Limegreen", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_ROOFTOP_YELLOW, []{return true;}),
        Entrance(RR_GF_GROUND_RED,     []{return true;}),
        Entrance(RR_GF_GROUND_GREY,    []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_WHITE] = Region("GF Rooftop White", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_KITCHEN_BOTTOM,          []{return true;}),
        // need to explicitly convert it into a bool
        Entrance(RR_GF_ROOFTOP_LIME,            []{return ctx->GetTrickOption(RT_GF_JUMP).Get() != 0;}),
        Entrance(RR_TH_SOUTH_F2_CARPENTER_AREA, []{return true;}),
        Entrance(RR_GF_GROUND_GREY,             []{return true;}),
        Entrance(RR_GF_GROUND_ORANGE,           []{return true;}),
        Entrance(RR_GF_GROUND_SALMON,           []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_LIME] = Region("GF Rooftop Lime", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_SOUTH_F1_CARPENTER_AREA, []{return true;}),
        Entrance(RR_GF_ROOFTOP_CYAN,            []{return true /* logic->CanClimb() */;}),
        Entrance(RR_GF_ROOFTOP_WHITE,           []{return true;}),
        Entrance(RR_GF_GROUND_GREY,             []{return true;}),
        Entrance(RR_GF_GROUND_ORANGE,           []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_CYAN] = Region("GF Rooftop Cyan", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_TH_KITCHEN_TOP,             []{return true;}),
        Entrance(RR_TH_SOUTH_F2_CARPENTER_AREA, []{return true;}),
        // need to explicitly convert it into a bool
        Entrance(RR_GF_ROOFTOP_PURPLE,          []{return ctx->GetTrickOption(RT_GF_JUMP).Get() != 0;}),
        Entrance(RR_GF_GROUND_ORANGE,           []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_PURPLE] = Region("GF Rooftop Purple", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_ROOFTOP_BLUE,  []{return true /* logic->CanClimb() */;}),
        Entrance(RR_GF_ROOFTOP_CYAN,  []{return true;}),
        Entrance(RR_GF_GROUND_SALMON, []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_MAGENTA] = Region("GF Rooftop Magenta", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_TH_NORTH_F2_CARPENTER_AREA, []{return true;}),
        Entrance(RR_GF_ROOFTOP_LIME,            []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_GREEN] = Region("GF Rooftop Green", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOMB_THROW) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_TH_KITCHEN_TOP,       []{return true;}),
        Entrance(RR_GF_ROOFTOP_LIME,      []{return true;}),
        Entrance(RR_GF_ROOFTOP_MAGENTA,   []{return true;}),
        Entrance(RR_GF_ROOFTOP_LIMEGREEN, []{return logic->CanUse(RG_HOVER_BOOTS) /* || bunny hood jump */;}),
        Entrance(RR_GF_ROOFTOP_VIOLET,    []{return logic->IsAdult;}),
    });

    areaTable[RR_GF_ROOFTOP_VIOLET] = Region("GF Rooftop Violet", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_GF_ROOFTOP_BLUE,  []{return logic->IsAdult;}),
        Entrance(RR_GF_ROOFTOP_CYAN,  []{return true;}),
        Entrance(RR_GF_ROOFTOP_GREEN, []{return true;}),
        Entrance(RR_GF_ROOFTOP_LIME,  []{return true;}),
    });

    areaTable[RR_GF_ROOFTOP_BLUE] = Region("GF Rooftop Blue", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH) /* && logic->CanClimb() (to get back up) */ && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_GF_ROOFTOP_CYAN,      []{return true;}),
        Entrance(RR_GF_ROOFTOP_PURPLE,    []{return true;}),
        Entrance(RR_GF_ROOFTOP_GREEN,     []{return true;}),
        Entrance(RR_GF_ROOFTOP_MAGENTA,   []{return true;}),
        Entrance(RR_GF_ROOFTOP_VIOLET,    []{return true;}),
        Entrance(RR_GF_ROOFTOP_TURQUOISE, []{return logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_SCARECROW) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_GF_ROOFTOP_TURQUOISE] = Region("GF Rooftop Turquoise", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_CHEST,        true),
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONG_JUMPSLASH) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_GF_ROOFTOP_GREEN,     []{return true;}),
        Entrance(RR_GF_ROOFTOP_MAGENTA,   []{return true;}),
        Entrance(RR_GF_ROOFTOP_LIMEGREEN, []{return true;}),
        Entrance(RR_GF_ROOFTOP_YELLOW,    []{return true;}),
    });

#pragma endregion

    areaTable[RR_GF_TOP_LEDGE] = Region("GF Top of Fortress", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        // 1 crate
    }, {
        //Exits
        Entrance(RR_GF_GROUND_RED, []{return logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    });

    areaTable[RR_GF_HBA_RANGE] = Region("GF HBA Range", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_GF_HBA_1000_POINTS,  logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
        LOCATION(RC_GF_HBA_1500_POINTS,  logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
        LOCATION(RC_GF_GS_ARCHERY_RANGE, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_GF_GROUND_SALMON, []{return logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    });

    areaTable[RR_GF_OUTSIDE_GATE] = Region("GF Outside Gate", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->GF_GateOpen, []{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) /*|| ShuffleSpecialIndoorEntrances*/);}),
    }, {}, {
        //Exits
        Entrance(RR_GF_GROUND_BOTTOM,        []{return (logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || !ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES))) || logic->GF_GateOpen;}),
        Entrance(RR_WASTELAND_NEAR_FORTRESS, []{return true;}),
    });

    areaTable[RR_GF_STORMS_GROTTO] = Region("GF Storms Grotto", "GF Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FreeFairies, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_1, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_2, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_3, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_4, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_5, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_6, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_7, true),
        LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_8, true),
    }, {
        //Exits
        Entrance(RR_GF_GROUND_ORANGE, []{return true;}),
    });
}