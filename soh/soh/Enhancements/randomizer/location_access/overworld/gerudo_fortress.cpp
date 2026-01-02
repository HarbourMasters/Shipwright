#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

// clang-format off
void RegionTable_Init_GerudoFortress() {
#pragma region Ground

    areaTable[RR_GF_OUTSKIRTS] = Region("Gerudo Fortress Outskirts", SCENE_GERUDOS_FORTRESS, {
        //Events
        EventAccess(LOGIC_GF_GATE_OPEN,  []{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}), //needs climb
    }, {
        //Locations
        LOCATION(RC_GF_OUTSKIRTS_NE_CRATE, (logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)) && logic->CanBreakCrates()),
        LOCATION(RC_GF_OUTSKIRTS_NW_CRATE, (logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)) && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GV_FORTRESS_SIDE, []{return true;}),
        Entrance(RR_TH_1_TORCH_CELL,  []{return true;}),
        Entrance(RR_GF_OUTSIDE_GATE,  []{return logic->Get(LOGIC_GF_GATE_OPEN);}),
        Entrance(RR_GF_NEAR_GROTTO,   []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_OUTSIDE_GTG,   []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        //You can talk to the guards to get yourself thrown in jail, so long as you have a hookshot to actually end up there
        Entrance(RR_GF_JAIL_WINDOW,   []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_GF_NEAR_GROTTO] = Region("GF Near Grotto", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_SOUTHMOST_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_MID_SOUTH_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_MID_NORTH_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_NORTHMOST_CENTER_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_TH_1_TORCH_CELL,       []{return true;}),
        Entrance(RR_TH_STEEP_SLOPE_CELL,   []{return true;}),
        Entrance(RR_TH_KITCHEN_CORRIDOR,   []{return true;}),
        //Jail
        Entrance(RR_GF_OUTSKIRTS,          []{return true;}),
        Entrance(RR_GF_JAIL_WINDOW,        []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_GF_OUTSIDE_GTG,        []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_TOP_OF_UPPER_VINES, []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GF_STORMS_GROTTO,      []{return logic->IsAdult && logic->CanOpenStormsGrotto();}),
    });

    areaTable[RR_GF_OUTSIDE_GTG] = Region("GF Outside GTG", SCENE_GERUDOS_FORTRESS, {
        //Events
        EventAccess(LOGIC_GTG_GATE_OPEN, []{return (logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->HasItem(RG_CHILD_WALLET));}),
    }, {}, {
        //Exits
        Entrance(RR_GF_TO_GTG,             []{return logic->Get(LOGIC_GTG_GATE_OPEN) && (logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES));}),
        //Jail
        Entrance(RR_GF_JAIL_WINDOW,        []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_GF_OUTSKIRTS,          []{return true;}),
        Entrance(RR_GF_NEAR_GROTTO,        []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        // RANDTODO: Add tricks for getting past the gerudo guarding the hba range
        Entrance(RR_GF_ABOVE_GTG,          []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        Entrance(RR_GF_TOP_OF_UPPER_VINES, []{return logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GF_HBA_RANGE,          []{return logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    });

    areaTable[RR_GF_TO_GTG] = Region("GF to GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        Entrance(RR_GERUDO_TRAINING_GROUND_ENTRYWAY, []{return true;}),
    });

    // Split out to handle adult being immediately captured without card
    areaTable[RR_GF_EXITING_GTG] = Region("GF Exiting GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        Entrance(RR_GF_OUTSIDE_GTG,                  []{return logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
        Entrance(RR_GF_JAIL_WINDOW,                  []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_GF_OUTSKIRTS,                    []{return true;}),
    });

#pragma endregion

#pragma region Rooftops

    areaTable[RR_GF_ABOVE_GTG] = Region("GF Above GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_TH_DOUBLE_CELL,           []{return true;}),
        Entrance(RR_TH_KITCHEN_CORRIDOR,      []{return true;}),
        //Jail
        Entrance(RR_GF_JAIL_WINDOW,           []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_GF_OUTSKIRTS,             []{return true;}),
        Entrance(RR_GF_NEAR_GROTTO,           []{return true;}),
        Entrance(RR_GF_OUTSIDE_GTG,           []{return logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD);}),
        // need to explicitly convert it into a bool
        Entrance(RR_GF_BOTTOM_OF_LOWER_VINES, []{return ctx->GetTrickOption(RT_GF_JUMP).Get() != 0 ;}),
    });

    areaTable[RR_GF_BOTTOM_OF_LOWER_VINES] = Region("GF Bottom of Lower Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_TH_STEEP_SLOPE_CELL,   []{return true;}),
        Entrance(RR_GF_NEAR_GROTTO,        []{return true;}),
        Entrance(RR_GF_TOP_OF_LOWER_VINES, []{return true /* logic->CanClimb() */;}),
        Entrance(RR_GF_ABOVE_GTG,          []{return true;}),
        Entrance(RR_GF_BELOW_GS,           []{return logic->IsAdult && logic->CanGroundJump();}),
    });

    areaTable[RR_GF_TOP_OF_LOWER_VINES] = Region("GF Top of Lower Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_TH_KITCHEN_BY_CORRIDOR,   []{return true;}),
        Entrance(RR_TH_DOUBLE_CELL,           []{return true;}),
        Entrance(RR_GF_ABOVE_GTG,             []{return true;}),
        Entrance(RR_GF_BOTTOM_OF_LOWER_VINES, []{return true;}),
        // need to explicitly convert it into a bool
        Entrance(RR_GF_BOTTOM_OF_UPPER_VINES, []{return logic->IsAdult && ctx->GetTrickOption(RT_GF_JUMP).Get();}),
    });

    areaTable[RR_GF_NEAR_GS] = Region("GF Near GS", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOMB_THROW) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_TH_KITCHEN_OPPOSITE_CORRIDOR, []{return true;}),
        Entrance(RR_GF_BOTTOM_OF_LOWER_VINES,     []{return true;}),
        Entrance(RR_GF_TOP_OF_LOWER_VINES,        []{return true;}),
        Entrance(RR_GF_SLOPED_ROOF,               []{return logic->IsAdult || logic->CanGroundJump();}),
        Entrance(RR_GF_LONG_ROOF,                 []{return logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_GF_NEAR_CHEST,                []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GF_BELOW_GS,                  []{return true;}),
    });

    areaTable[RR_GF_SLOPED_ROOF] = Region("GF Sloped Roof", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_GF_TOP_OF_LOWER_VINES,    []{return true;}),
        Entrance(RR_GF_NEAR_GS,               []{return true;}),
        Entrance(RR_GF_BOTTOM_OF_UPPER_VINES, []{return true;}),
        Entrance(RR_GF_TOP_OF_UPPER_VINES,    []{return logic->IsAdult && ctx->GetTrickOption(RT_GF_JUMP).Get();}),
    });

    areaTable[RR_GF_BOTTOM_OF_UPPER_VINES] = Region("GF Bottom of Upper Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_GF_OUTSIDE_GTG,        []{return true;}),
        Entrance(RR_GF_TOP_OF_LOWER_VINES, []{return true;}),
        Entrance(RR_GF_SLOPED_ROOF,        []{return logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_JUMP));}),
        Entrance(RR_GF_TOP_OF_UPPER_VINES, []{return true /* logic->CanClimb() */;}),
        Entrance(RR_GF_TO_GTG,             []{return logic->IsAdult && ctx->GetTrickOption(RT_GF_LEDGE_CLIP_INTO_GTG).Get();}),
    });

    areaTable[RR_GF_TOP_OF_UPPER_VINES] = Region("GF Top of Upper Vines", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        //if RR_GF_SLOPED_ROOF > RR_GF_TOP_OF_UPPER_VINES is ever made part of RT_GF_JUMP, climb is needed to get back up
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_GF_TOP_OF_LOWER_VINES,    []{return true;}),
        Entrance(RR_GF_SLOPED_ROOF,           []{return true;}),
        Entrance(RR_GF_BOTTOM_OF_UPPER_VINES, []{return true;}),
        Entrance(RR_GF_NEAR_CHEST,            []{return logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_SCARECROW) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_GF_NEAR_CHEST] = Region("GF Near Chest", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_CHEST,        true),
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_GF_NEAR_GS,     []{return true;}),
        Entrance(RR_GF_LONG_ROOF,   []{return true;}),
    });

    areaTable[RR_GF_LONG_ROOF] = Region("GF Long Roof", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_GF_BOTTOM_OF_LOWER_VINES, []{return true;}),
        Entrance(RR_GF_NEAR_GS,               []{return (logic->IsAdult && ctx->GetTrickOption(RT_GF_JUMP)) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_GF_BELOW_GS,              []{return true;}),
        Entrance(RR_GF_NEAR_CHEST,            []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GF_BELOW_CHEST,           []{return true;}),
    });

    areaTable[RR_GF_BELOW_GS] = Region("GF Below GS", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        Entrance(RR_TH_DEAD_END_CELL,         []{return true;}),
        Entrance(RR_GF_BOTTOM_OF_LOWER_VINES, []{return true;}),
    });

    areaTable[RR_GF_BELOW_CHEST] = Region("GF Below Chest", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        Entrance(RR_TH_BREAK_ROOM, []{return true;}),
        Entrance(RR_GF_OUTSKIRTS,  []{return true;}),
    });


#pragma endregion

    areaTable[RR_GF_ABOVE_JAIL] = Region("GF Above Jail", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_ABOVE_JAIL_CRATE, true),
    }, {
        //Exits
        //you don't take fall damage if you land on the rock with the flag on for some reason
        //there's a trick to reach RR_GF_LONG_ROOF but that's too intricate for GF_JUMP
        Entrance(RR_GF_OUTSKIRTS,           []{return ctx->GetTrickOption(RT_GF_JUMP).Get() != 0;}),
        Entrance(RR_GF_NEAR_CHEST,          []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GF_BELOW_CHEST,         []{return logic->TakeDamage();}),
        Entrance(RR_GF_JAIL_WINDOW,         []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_TH_BREAK_ROOM_CORRIDOR, []{return true;}),
    });

    areaTable[RR_GF_JAIL_WINDOW] = Region("GF Jail Window", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        //There's a trick where hovers backwalk into backflip gives access to RR_GF_LONG_ROOF from here
        Entrance(RR_GF_OUTSKIRTS,   []{return true;}),
        Entrance(RR_GF_BELOW_CHEST, []{return true;}),
    });

    areaTable[RR_GF_HBA_RANGE] = Region("GF HBA Range", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_HBA_1000_POINTS,          logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
        LOCATION(RC_GF_HBA_1500_POINTS,          logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
        LOCATION(RC_GF_HBA_RANGE_GS,             logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && logic->CanGetNightTimeGS()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_1,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_2,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_3,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_4,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_5,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_6,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_RANGE_CRATE_7,        logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_CANOPY_EAST_CRATE,    logic->CanBreakCrates()),
        LOCATION(RC_GF_HBA_CANOPY_WEST_CRATE,    logic->CanBreakCrates()),
        LOCATION(RC_GF_NORTH_TARGET_EAST_CRATE,  logic->CanBreakCrates()),
        LOCATION(RC_GF_NORTH_TARGET_WEST_CRATE,  logic->IsAdult || (logic->BlastOrSmash() || logic->HookshotOrBoomerang() || logic->CanUse(RG_HOVER_BOOTS))),
        //implies logic->CanBreakCrates()
        LOCATION(RC_GF_NORTH_TARGET_CHILD_CRATE, logic->IsChild && logic->BlastOrSmash()),
        LOCATION(RC_GF_SOUTH_TARGET_EAST_CRATE,  logic->CanBreakCrates()),
        LOCATION(RC_GF_SOUTH_TARGET_WEST_CRATE,  logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_GF_OUTSIDE_GTG, []{return logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    });

    areaTable[RR_GF_OUTSIDE_GATE] = Region("GF Outside Gate", SCENE_GERUDOS_FORTRESS, {
        //Events
        EventAccess(LOGIC_GF_GATE_OPEN, []{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}),
    }, {}, {
        //Exits
        Entrance(RR_GF_OUTSKIRTS,            []{return logic->Get(LOGIC_GF_GATE_OPEN);}),
        Entrance(RR_WASTELAND_NEAR_FORTRESS, []{return true;}),
    });

    areaTable[RR_GF_STORMS_GROTTO] = Region("GF Storms Grotto", SCENE_GROTTOS, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return true;}),
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
        Entrance(RR_GF_NEAR_GROTTO, []{return true;}),
    });
}
// clang-format on
