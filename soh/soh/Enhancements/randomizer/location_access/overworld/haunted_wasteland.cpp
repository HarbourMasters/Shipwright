#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_HauntedWasteland() {
    // clang-format off
    areaTable[RR_WASTELAND_NEAR_FORTRESS] = Region("Wasteland Near Fortress", SCENE_HAUNTED_WASTELAND, {},
    {
        //Locations
        LOCATION(RC_HW_BEFORE_QUICKSAND_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_GF_OUTSIDE_GATE,   true),
        ENTRANCE(RR_HAUNTED_WASTELAND, logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING)),
    });

    areaTable[RR_HAUNTED_WASTELAND] = Region("Haunted Wasteland", SCENE_HAUNTED_WASTELAND, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,    logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,      logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_CARPET_MERCHANT, logic->HasItem(RG_ADULT_WALLET) && GetCheckPrice(RC_WASTELAND_BOMBCHU_SALESMAN) <= GetWalletCapacity() && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Locations
        LOCATION(RC_WASTELAND_CHEST,            logic->HasFireSource() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_WASTELAND_BOMBCHU_SALESMAN, (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS)) && logic->HasItem(RG_SPEAK_HYLIAN) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_WASTELAND_GS,               logic->HookshotOrBoomerang() || (logic->IsAdult && ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->CanJumpslash())), // need to jumpslash immediately with two handed weapons
        LOCATION(RC_WASTELAND_NEAR_GS_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_3,    logic->CanBreakPots()),
        LOCATION(RC_WASTELAND_NEAR_GS_POT_4,    logic->CanBreakPots()),
        LOCATION(RC_HW_AFTER_QUICKSAND_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_HW_AFTER_QUICKSAND_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_HW_AFTER_QUICKSAND_CRATE_3, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_WASTELAND_NEAR_COLOSSUS, ctx->GetTrickOption(RT_LENS_HW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_WASTELAND_NEAR_FORTRESS, logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING)),
    });

    areaTable[RR_WASTELAND_NEAR_COLOSSUS] = Region("Wasteland Near Colossus", SCENE_HAUNTED_WASTELAND, {}, {
        //Locations
        LOCATION(RC_HW_NEAR_COLOSSUS_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DESERT_COLOSSUS,   true),
        ENTRANCE(RR_HAUNTED_WASTELAND, ctx->GetTrickOption(RT_HW_REVERSE) || false),
    });

    // clang-format on
}
