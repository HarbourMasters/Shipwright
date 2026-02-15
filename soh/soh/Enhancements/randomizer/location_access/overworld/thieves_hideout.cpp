#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

// clang-format off
// When Thieves' Hideout entrances are shuffled, getting caught by guards should behave like void outs to avoid logic headaches.
void RegionTable_Init_ThievesHideout() {
    areaTable[RR_TH_1_TORCH_CELL] = Region("Thieves Hideout 1 Torch Cell", SCENE_THIEVES_HIDEOUT, {
        //Events
        EVENT_ACCESS(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER, logic->CanKillEnemy(RE_GERUDO_WARRIOR) && logic->HasItem(RG_SPEAK_HYLIAN)),
        EVENT_ACCESS(LOGIC_TH_RESCUED_ALL_CARPENTERS,       logic->SmallKeys(SCENE_THIEVES_HIDEOUT, ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_NORMAL) ? 4 : 1) && logic->Get(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER)),
    }, {
        //Locations
        LOCATION(RC_TH_1_TORCH_CARPENTER,      logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_TH_1_TORCH_CELL_RIGHT_POT, logic->CanBreakPots()),
        LOCATION(RC_TH_1_TORCH_CELL_MID_POT,   logic->CanBreakPots()),
        LOCATION(RC_TH_1_TORCH_CELL_LEFT_POT,  logic->CanBreakPots()),
        LOCATION(RC_TH_1_TORCH_CELL_CRATE,     logic->CanBreakCrates()),
        LOCATION(RC_TH_FREED_CARPENTERS,       logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS)),
    }, {
        //Exits
        ENTRANCE(RR_GF_OUTSKIRTS,   true),
        ENTRANCE(RR_GF_NEAR_GROTTO, true),
    });

    areaTable[RR_TH_DOUBLE_CELL] = Region("Thieves Hideout Double Cell", SCENE_THIEVES_HIDEOUT, {
        //Events
        EVENT_ACCESS(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER, logic->CanKillEnemy(RE_GERUDO_WARRIOR) && logic->HasItem(RG_SPEAK_HYLIAN)),
        EVENT_ACCESS(LOGIC_TH_RESCUED_ALL_CARPENTERS,           logic->SmallKeys(SCENE_THIEVES_HIDEOUT, ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_NORMAL) ? 4 : 1) && logic->Get(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER)),
    }, {
        //Locations
        LOCATION(RC_TH_DOUBLE_CELL_CARPENTER,      logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_TH_NEAR_DOUBLE_CELL_RIGHT_POT, logic->CanBreakPots()),
        LOCATION(RC_TH_NEAR_DOUBLE_CELL_MID_POT,   logic->CanBreakPots()),
        LOCATION(RC_TH_NEAR_DOUBLE_CELL_LEFT_POT,  logic->CanBreakPots()),
        LOCATION(RC_TH_RIGHTMOST_JAILED_POT,       logic->CanBreakPots()),
        LOCATION(RC_TH_RIGHT_MIDDLE_JAILED_POT,    logic->CanBreakPots()),
        LOCATION(RC_TH_LEFT_MIDDLE_JAILED_POT,     logic->CanBreakPots()),
        LOCATION(RC_TH_LEFTMOST_JAILED_POT,        logic->CanBreakPots()),
        LOCATION(RC_TH_DOUBLE_CELL_LEFT_CRATE,     logic->CanBreakCrates()),
        LOCATION(RC_TH_DOUBLE_CELL_RIGHT_CRATE,    logic->CanBreakCrates()),
        LOCATION(RC_TH_FREED_CARPENTERS,           logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS)),
    }, {
        //Exits
        ENTRANCE(RR_GF_OUTSKIRTS,          true),
        ENTRANCE(RR_GF_ABOVE_GTG,          true),
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES, true),
    });

    areaTable[RR_TH_DEAD_END_CELL] = Region("Thieves Hideout Dead End Cell", SCENE_THIEVES_HIDEOUT, {
        //Events
        EVENT_ACCESS(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER, logic->CanKillEnemy(RE_GERUDO_WARRIOR) && logic->HasItem(RG_SPEAK_HYLIAN)),
        EVENT_ACCESS(LOGIC_TH_RESCUED_ALL_CARPENTERS,        logic->SmallKeys(SCENE_THIEVES_HIDEOUT, ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_NORMAL) ? 4 : 1) && logic->Get(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER)),
    }, {
        //Locations
        LOCATION(RC_TH_DEAD_END_CARPENTER,  logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_TH_DEAD_END_CELL_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_TH_FREED_CARPENTERS,    logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS)),
    }, {
        //Exits
        ENTRANCE(RR_GF_BELOW_GS, true),
    });

    areaTable[RR_TH_STEEP_SLOPE_CELL] = Region("Thieves Hideout Steep Slope Cell", SCENE_THIEVES_HIDEOUT, {
        //Events
        EVENT_ACCESS(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER, logic->CanKillEnemy(RE_GERUDO_WARRIOR) && logic->HasItem(RG_SPEAK_HYLIAN)),
        EVENT_ACCESS(LOGIC_TH_RESCUED_ALL_CARPENTERS,     logic->SmallKeys(SCENE_THIEVES_HIDEOUT, ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_NORMAL) ? 4 : 1) && logic->Get(LOGIC_TH_COULD_FREE_1_TORCH_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DOUBLE_CELL_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_DEAD_END_CARPENTER) && logic->Get(LOGIC_TH_COULD_FREE_SLOPE_CARPENTER)),
    }, {
        //Locations
        LOCATION(RC_TH_STEEP_SLOPE_CARPENTER, logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
        LOCATION(RC_TH_STEEP_SLOPE_RIGHT_POT, logic->CanBreakPots()),
        LOCATION(RC_TH_STEEP_SLOPE_LEFT_POT,  logic->CanBreakPots()),
        LOCATION(RC_TH_FREED_CARPENTERS,      logic->Get(LOGIC_TH_RESCUED_ALL_CARPENTERS)),
    }, {
        //Exits
        ENTRANCE(RR_GF_BOTTOM_OF_LOWER_VINES, true),
        ENTRANCE(RR_GF_NEAR_GROTTO,           true),
    });

    areaTable[RR_TH_KITCHEN_CORRIDOR] = Region("Thieves Hideout Kitchen Corridor", SCENE_THIEVES_HIDEOUT, {}, {
        //Locations
        LOCATION(RC_TH_NEAR_KITCHEN_LEFTMOST_CRATE,  logic->CanBreakCrates()),
        LOCATION(RC_TH_NEAR_KITCHEN_MID_LEFT_CRATE,  logic->CanBreakCrates()),
        LOCATION(RC_TH_NEAR_KITCHEN_MID_RIGHT_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_TH_NEAR_KITCHEN_RIGHTMOST_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GF_NEAR_GROTTO,  true),
        ENTRANCE(RR_GF_ABOVE_GTG,    true),
        ENTRANCE(RR_TH_KITCHEN_MAIN, logic->CanPassEnemy(RE_GERUDO_GUARD)),
    });

    areaTable[RR_TH_KITCHEN_MAIN] = Region("Thieves Hideout Kitchen Bottom", SCENE_THIEVES_HIDEOUT, {}, {
        //Locations
        LOCATION(RC_TH_KITCHEN_POT_1,     logic->CanBreakPots() && logic->CanPassEnemy(RE_GERUDO_GUARD)),
        LOCATION(RC_TH_KITCHEN_POT_2,     logic->CanBreakPots() && logic->CanPassEnemy(RE_GERUDO_GUARD)),
        LOCATION(RC_TH_KITCHEN_CRATE,     logic->CanBreakCrates() && logic->CanPassEnemy(RE_GERUDO_GUARD)),
        LOCATION(RC_TH_KITCHEN_SUN_FAIRY, logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_TH_KITCHEN_CORRIDOR,          logic->CanPassEnemy(RE_GERUDO_GUARD)),
        ENTRANCE(RR_TH_KITCHEN_BY_CORRIDOR,       logic->CanPassEnemy(RE_GERUDO_GUARD)),
        ENTRANCE(RR_TH_KITCHEN_OPPOSITE_CORRIDOR, logic->CanPassEnemy(RE_GERUDO_GUARD)),
    });

    areaTable[RR_TH_KITCHEN_BY_CORRIDOR] = Region("Thieves Hideout Kitchen Top By Corridor", SCENE_THIEVES_HIDEOUT, {}, {
        //Locations
        LOCATION(RC_TH_KITCHEN_POT_1, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_TH_KITCHEN_POT_2, logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_TH_KITCHEN_MAIN,              true),
        //hookshot to cross using rafters implied by logic->CanPassEnemy(RE_GERUDO_GUARD)
        ENTRANCE(RR_TH_KITCHEN_OPPOSITE_CORRIDOR, logic->CanPassEnemy(RE_GERUDO_GUARD) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GF_TOP_OF_LOWER_VINES,        true),
    });

    areaTable[RR_TH_KITCHEN_OPPOSITE_CORRIDOR] = Region("Thieves Hideout Kitchen Top Across From Corridor", SCENE_THIEVES_HIDEOUT, {}, {
        //Locations
        LOCATION(RC_TH_KITCHEN_POT_1, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_TH_KITCHEN_POT_2, logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_TH_KITCHEN_MAIN,        true),
        //hookshot to cross using rafters implied by logic->CanPassEnemy(RE_GERUDO_GUARD)
        ENTRANCE(RR_TH_KITCHEN_BY_CORRIDOR, logic->CanPassEnemy(RE_GERUDO_GUARD) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GF_NEAR_GS,             true),
    });

    areaTable[RR_TH_BREAK_ROOM] = Region("Thieves Hideout Break Room", SCENE_THIEVES_HIDEOUT, {}, {
        //Locations
        LOCATION(RC_TH_BREAK_ROOM_FRONT_POT,      (logic->CanPassEnemy(RE_BREAK_ROOM_GUARD) && logic->CanBreakPots()) ||
                                                  (logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_TH_BREAK_ROOM_BACK_POT,       (logic->CanPassEnemy(RE_BREAK_ROOM_GUARD) && logic->CanBreakPots()) ||
                                                  (logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_TH_BREAK_HALLWAY_OUTER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_TH_BREAK_HALLWAY_INNER_CRATE, logic->CanBreakCrates()),
        LOCATION(RC_TH_BREAK_ROOM_RIGHT_CRATE,    (logic->CanPassEnemy(RE_BREAK_ROOM_GUARD) && logic->CanBreakCrates()) || 
                                                   (logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->HasExplosives() && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_TH_BREAK_ROOM_LEFT_CRATE,     (logic->CanPassEnemy(RE_BREAK_ROOM_GUARD) && logic->CanBreakCrates()) || 
                                                   (logic->CanPassEnemy(RE_GERUDO_GUARD) && logic->HasExplosives() && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        ENTRANCE(RR_GF_BELOW_CHEST,         logic->CanPassEnemy(RE_GERUDO_GUARD)),
        //Implies logic->CanPassEnemy(RE_GERUDO_GUARD)
        ENTRANCE(RR_TH_BREAK_ROOM_CORRIDOR, logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_TH_BREAK_ROOM_CORRIDOR] = Region("Thieves Hideout Break Room", SCENE_THIEVES_HIDEOUT, {}, {}, {
        //Exits
        ENTRANCE(RR_TH_BREAK_ROOM, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GF_ABOVE_JAIL, true),
    });
}
// clang-format on