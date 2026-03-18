#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

// clang-format off
void RegionTable_Init_GerudoFortress() {
#pragma region Ground

    areaTable[RR_GF_OUTSKIRTS] = Region("Gerudo Fortress Outskirts", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_OUTSKIRTS_NE_CRATE, (logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)) && logic->CanBreakCrates()),
        LOCATION(RC_GF_OUTSKIRTS_NW_CRATE, (logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)) && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GV_FORTRESS_SIDE, true),
        ENTRANCE(RR_TH_1_TORCH_CELL,  true),
        ENTRANCE(RR_GF_OUTSIDE_GATE,  logic->Get(LOGIC_GF_GATE_OPEN)),
        ENTRANCE(RR_GF_NEAR_GROTTO,   logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        ENTRANCE(RR_GF_OUTSIDE_GTG,   logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        //You can talk to the guards to get yourself thrown in jail, so long as you have a hookshot to actually end up there
        ENTRANCE(RR_GF_TOWER,         (logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD)) && logic->CanClimbHighLadder()),
        ENTRANCE(RR_GF_JAIL_WINDOW,   logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_GF_NEAR_GROTTO] = Region("GF Near Grotto", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_SOUTHMOST_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_MID_SOUTH_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_MID_NORTH_CENTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_GF_NORTHMOST_CENTER_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_TH_1_TORCH_CELL,       true),
        ENTRANCE(RR_TH_STEEP_SLOPE_CELL,   true),
        ENTRANCE(RR_TH_KITCHEN_CORRIDOR,   true),
        //Jail
        ENTRANCE(RR_GF_OUTSKIRTS,          true),
        ENTRANCE(RR_GF_JAIL_WINDOW,        logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_OUTSIDE_GTG,        logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        ENTRANCE(RR_GF_TOP_OF_UPPER_VINES, logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GF_STORMS_GROTTO,      logic->IsAdult && logic->CanOpenStormsGrotto()),
    });

    areaTable[RR_GF_OUTSIDE_GTG] = Region("GF Outside GTG", SCENE_GERUDOS_FORTRESS, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_GATE_OPEN, logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_GERUDO)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GF_TO_GTG,             logic->Get(LOGIC_GTG_GATE_OPEN) && (logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES))),
        //Jail
        ENTRANCE(RR_GF_JAIL_WINDOW,        logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_OUTSKIRTS,          true),
        ENTRANCE(RR_GF_NEAR_GROTTO,        logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        // RANDTODO: Add tricks for getting past the gerudo guarding the hba range
        ENTRANCE(RR_GF_ABOVE_GTG,          logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        ENTRANCE(RR_GF_TOP_OF_UPPER_VINES, logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GF_HBA_RANGE,          logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD)),
    });

    areaTable[RR_GF_TO_GTG] = Region("GF to GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_ENTRYWAY, true),
    });

    // Split out to handle adult being immediately captured without card
    areaTable[RR_GF_EXITING_GTG] = Region("GF Exiting GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        ENTRANCE(RR_GF_OUTSIDE_GTG,                  logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD)),
        ENTRANCE(RR_GF_JAIL_WINDOW,                  logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_OUTSKIRTS,                    true),
    });

#pragma endregion

#pragma region Rooftops

    areaTable[RR_GF_ABOVE_GTG] = Region("GF Above GTG", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_TH_DOUBLE_CELL,           true),
        ENTRANCE(RR_TH_KITCHEN_CORRIDOR,      true),
        //Jail
        ENTRANCE(RR_GF_JAIL_WINDOW,           logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_OUTSKIRTS,             true),
        ENTRANCE(RR_GF_NEAR_GROTTO,           true),
        ENTRANCE(RR_GF_OUTSIDE_GTG,           logic->IsChild || logic->CanPassEnemy(RE_GERUDO_GUARD)),
        // need to explicitly convert it into a bool
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES, ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS).Get() != 0 ),
    });

    areaTable[RR_GF_BOTTOM_OF_LOWER_VINES] = Region("GF Bottom of Lower Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_TH_STEEP_SLOPE_CELL,   true),
        ENTRANCE(RR_GF_NEAR_GROTTO,        true),
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_ABOVE_GTG,          true),
        ENTRANCE(RR_GF_BELOW_GS,           logic->IsAdult && logic->CanGroundJump()),
    });

    areaTable[RR_GF_TOP_OF_LOWER_VINES] = Region("GF Top of Lower Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_TH_KITCHEN_BY_CORRIDOR,   true),
        ENTRANCE(RR_TH_DOUBLE_CELL,           true),
        ENTRANCE(RR_GF_ABOVE_GTG,             true),
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES, true),
        // need to explicitly convert it into a bool
        ENTRANCE(RR_GF_BOTTOM_OF_UPPER_VINES, logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS).Get()),
    });

    areaTable[RR_GF_NEAR_GS] = Region("GF Near GS", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOMB_THROW) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        ENTRANCE(RR_TH_KITCHEN_OPPOSITE_CORRIDOR, true),
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES,     true),
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES,        true),
        ENTRANCE(RR_GF_SLOPED_ROOF,               logic->IsAdult || logic->CanGroundJump()),
        ENTRANCE(RR_GF_LONG_ROOF,                 logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GF_NEAR_CHEST,                logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GF_BELOW_GS,                  true),
    });

    areaTable[RR_GF_SLOPED_ROOF] = Region("GF Sloped Roof", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES,    true),
        ENTRANCE(RR_GF_NEAR_GS,               true),
        ENTRANCE(RR_GF_BOTTOM_OF_UPPER_VINES, true),
        ENTRANCE(RR_GF_TOP_OF_UPPER_VINES,    logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS).Get()),
    });

    areaTable[RR_GF_BOTTOM_OF_UPPER_VINES] = Region("GF Bottom of Upper Vines", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_GF_OUTSIDE_GTG,        true),
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES, true),
        ENTRANCE(RR_GF_SLOPED_ROOF,        logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))),
        ENTRANCE(RR_GF_TOP_OF_UPPER_VINES, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_TO_GTG,             logic->IsAdult && ctx->GetTrickOption(RT_GF_LEDGE_CLIP_INTO_GTG).Get()),
    });

    areaTable[RR_GF_TOP_OF_UPPER_VINES] = Region("GF Top of Upper Vines", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        //if RR_GF_SLOPED_ROOF > RR_GF_TOP_OF_UPPER_VINES is ever made part of RT_UNINTUITIVE_JUMPS, climb is needed to get back up
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetNightTimeGS() && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))),
    }, {
        //Exits
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES,    true),
        ENTRANCE(RR_GF_SLOPED_ROOF,           true),
        ENTRANCE(RR_GF_BOTTOM_OF_UPPER_VINES, true),
        ENTRANCE(RR_GF_NEAR_CHEST,            logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->ReachScarecrow()) || logic->CanUse(RG_LONGSHOT)),
    });

    areaTable[RR_GF_NEAR_CHEST] = Region("GF Near Chest", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_CHEST,        logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        ENTRANCE(RR_GF_NEAR_GS,     true),
        ENTRANCE(RR_GF_LONG_ROOF,   true),
    });

    areaTable[RR_GF_LONG_ROOF] = Region("GF Long Roof", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES, true),
        ENTRANCE(RR_GF_NEAR_GS,               (logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GF_BELOW_GS,              true),
        ENTRANCE(RR_GF_NEAR_CHEST,            logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GF_BELOW_CHEST,           true),
    });

    areaTable[RR_GF_BELOW_GS] = Region("GF Below GS", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_GS_TOP_FLOOR, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) && logic->CanGetNightTimeGS()),
    }, {
        //Exits
        ENTRANCE(RR_TH_DEAD_END_CELL,         true),
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES, true),
    });

    areaTable[RR_GF_BELOW_CHEST] = Region("GF Below Chest", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_TH_BREAK_ROOM, true),
        ENTRANCE(RR_GF_OUTSKIRTS,  true),
    });


#pragma endregion

//Assumes IsChild || HasItem(RG_GERUDO_MEMBERSHIP_CARD) is checked on entry
    areaTable[RR_GF_TOWER] = Region("Gerudo Fortress Tower", SCENE_GERUDOS_FORTRESS, {
        //Events
        EVENT_ACCESS(LOGIC_GF_GATE_OPEN, logic->IsAdult && logic->HasItem(RG_SPEAK_GERUDO)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GF_OUTSKIRTS,     true),
        ENTRANCE(RR_GF_OUTSIDE_GATE,  (logic->IsAdult && logic->CanGroundJump()) || ctx->GetTrickOption(RT_GF_WASTELAND_GATE_SIDEHOP_SKIP)),
    });

    areaTable[RR_GF_ABOVE_JAIL] = Region("GF Above Jail", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_ABOVE_JAIL_CRATE, true),
    }, {
        //Exits
        //you don't take fall damage if you land on the rock with the flag on for some reason
        //there's a trick to reach RR_GF_LONG_ROOF
        ENTRANCE(RR_GF_OUTSKIRTS,           ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS).Get() != 0),
        ENTRANCE(RR_GF_NEAR_CHEST,          logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GF_BELOW_CHEST,         logic->TakeDamage()),
        ENTRANCE(RR_GF_JAIL_WINDOW,         logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_TH_BREAK_ROOM_CORRIDOR, true),
        ENTRANCE(RR_GF_TOWER,               ctx->GetTrickOption(RT_GF_ADULT_SKIP_WASTELAND_GATE) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash() && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD)),
        ENTRANCE(RR_GF_OUTSIDE_GATE,        ctx->GetTrickOption(RT_GF_ADULT_SKIP_WASTELAND_GATE) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash()),
    });

    areaTable[RR_GF_JAIL_WINDOW] = Region("GF Jail Window", SCENE_GERUDOS_FORTRESS, {}, {}, {
        //Exits
        ENTRANCE(RR_GF_LONG_ROOF,   ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER)), // can also get it with hovers backwalk into backflip
        ENTRANCE(RR_GF_OUTSKIRTS,   true),
        ENTRANCE(RR_GF_BELOW_CHEST, true),
        ENTRANCE(RR_GF_ABOVE_JAIL,  ctx->GetTrickOption(RT_HOOKSHOT_CLIP) && logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_GF_HBA_RANGE] = Region("GF HBA Range", SCENE_GERUDOS_FORTRESS, {}, {
        //Locations
        LOCATION(RC_GF_HBA_1000_POINTS,          logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_GERUDO) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->SummonEpona() && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
        LOCATION(RC_GF_HBA_1500_POINTS,          logic->IsAdult && logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_GERUDO) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->SummonEpona() && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
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
        ENTRANCE(RR_GF_OUTSIDE_GTG, logic->IsChild || logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD)),
    });

    areaTable[RR_GF_OUTSIDE_GATE] = Region("GF Outside Gate", SCENE_GERUDOS_FORTRESS, {
        //Events
        EVENT_ACCESS(LOGIC_GF_GATE_OPEN, logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->HasItem(RG_SPEAK_GERUDO)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GF_OUTSKIRTS,            logic->Get(LOGIC_GF_GATE_OPEN)),
        ENTRANCE(RR_WASTELAND_NEAR_FORTRESS, true),
    });

    areaTable[RR_GF_STORMS_GROTTO] = Region("GF Storms Grotto", SCENE_GROTTOS, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, true),
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
        ENTRANCE(RR_GF_NEAR_GROTTO, true),
    });
}
// clang-format on
