#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_IceCavern() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_ICE_CAVERN_ENTRYWAY] = Region("Ice Cavern Entryway", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        Entrance(RR_ICE_CAVERN_BEGINNING,    []{return ctx->GetDungeon(ICE_CAVERN)->IsVanilla();}),
        Entrance(RR_ICE_CAVERN_MQ_BEGINNING, []{return ctx->GetDungeon(ICE_CAVERN)->IsMQ() && logic->CanUseProjectile();}),
        Entrance(RR_ZF_LEDGE,                []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_ICE_CAVERN_BEGINNING] = Region("Ice Cavern Beginning", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_ENTRANCE_STORMS_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_ICE_CAVERN_LOBBY_RUPEE,           logic->BlueFire()), // can get with rang trick
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_ENTRYWAY,        []{return true;}),
        Entrance(RR_ICE_CAVERN_HUB,             []{return Here(RR_ICE_CAVERN_BEGINNING, []{return logic->CanKillEnemy(RE_FREEZARD, ED_CLOSE, true, 4);});}),
        Entrance(RR_ICE_CAVERN_ABOVE_BEGINNING, []{return false;}),
    });

    areaTable[RR_ICE_CAVERN_HUB] = Region("Ice Cavern Hub", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, logic->HookshotOrBoomerang()),
        LOCATION(RC_ICE_CAVERN_HALL_POT_1,              logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_HALL_POT_2,              logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_2,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_SPINNING_BLADE_POT_3,    logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_BEGINNING,    []{return true;}),
        Entrance(RR_ICE_CAVERN_MAP_ROOM,     []{return (logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) && logic->CanClearStalagmite();}),
        Entrance(RR_ICE_CAVERN_COMPASS_ROOM, []{return Here(RR_ICE_CAVERN_HUB, []{return logic->BlueFire();});}),
        Entrance(RR_ICE_CAVERN_BLOCK_ROOM,   []{return Here(RR_ICE_CAVERN_HUB, []{return logic->BlueFire();}) && logic->CanClearStalagmite();}),
    });

    areaTable[RR_ICE_CAVERN_MAP_ROOM] = Region("Ice Cavern Map Room", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MAP_CHEST,             logic->BlueFire()),
        // very easy to break pot through ice
        LOCATION(RC_ICE_CAVERN_FROZEN_POT_1,          (logic->CanBreakPots() && logic->BlueFire()) || logic->HasExplosives() ||
                                                      (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && ((logic->CanStandingShield() && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER))) ||
                                                      (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_LEFT_HEART,   true),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_MIDDLE_HEART, true),
        LOCATION(RC_ICE_CAVERN_MAP_ROOM_RIGHT_HEART,  true),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_HUB, []{return true;}),
    });

    areaTable[RR_ICE_CAVERN_COMPASS_ROOM] = Region("Ice Cavern Map Room", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_COMPASS_CHEST,       logic->CanClearStalagmite() && logic->BlueFire()),
        LOCATION(RC_ICE_CAVERN_FREESTANDING_POH,    logic->CanClearStalagmite() && logic->BlueFire()), // can skip blue fire with rang trick
        LOCATION(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_HUB, []{return true;}),
    });

    // heavily relies on str0
    areaTable[RR_ICE_CAVERN_BLOCK_ROOM] = Region("Ice Cavern Block Room", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return true;}),
    }, {
        //Locations
        // trick involves backflip, could be merged into general trick
        LOCATION(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,    logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_ICE_BLOCK_GS) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH))),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_1, logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_2, logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_3, logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_HUB,               []{return logic->CanClearStalagmite();}),
        Entrance(RR_ICE_CAVERN_BEFORE_FINAL_ROOM, []{return Here(RR_ICE_CAVERN_BLOCK_ROOM, []{return logic->BlueFire();});}),
    });

    // this represents being past the red ice barricade, not just past the silver rupee door
    areaTable[RR_ICE_CAVERN_BEFORE_FINAL_ROOM] = Region("Ice Cavern Before Final Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_NEAR_END_POT_1, logic->CanBreakPots() && logic->BlueFire()),
        LOCATION(RC_ICE_CAVERN_NEAR_END_POT_2, logic->CanBreakPots() && logic->BlueFire()),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_BLOCK_ROOM, []{return Here(RR_ICE_CAVERN_BEFORE_FINAL_ROOM, []{return logic->BlueFire();});}),
        Entrance(RR_ICE_CAVERN_FINAL_ROOM, []{return true;}),
    });

    areaTable[RR_ICE_CAVERN_FINAL_ROOM] = Region("Ice Cavern Final Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_IRON_BOOTS_CHEST, Here(RR_ICE_CAVERN_FINAL_ROOM, []{return logic->CanKillEnemy(RE_WOLFOS);})),
        LOCATION(RC_SHEIK_IN_ICE_CAVERN,         Here(RR_ICE_CAVERN_FINAL_ROOM, []{return logic->CanKillEnemy(RE_WOLFOS);})), // rando enables this for child
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_BEFORE_FINAL_ROOM,     []{return Here(RR_ICE_CAVERN_FINAL_ROOM, []{return logic->CanKillEnemy(RE_WOLFOS);});}),
        Entrance(RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER, []{return Here(RR_ICE_CAVERN_FINAL_ROOM, []{return logic->CanKillEnemy(RE_WOLFOS);}) && logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER] = Region("Ice Cavern Final Room Underwater", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        Entrance(RR_ICE_CAVERN_FINAL_ROOM,      []{return logic->CanUse(RG_BRONZE_SCALE);}),
        Entrance(RR_ICE_CAVERN_ABOVE_BEGINNING, []{return logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_ICE_CAVERN_ABOVE_BEGINNING] = Region("Ice Cavern Above Beginning", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        Entrance(RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER, []{return logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_ICE_CAVERN_BEGINNING,             []{return true;}),
    });
#pragma endregion

#pragma region MQ

    areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Region("Ice Cavern MQ Beginning", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_ENTRANCE_POT, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_ENTRYWAY,           []{return true;}),
        //It is in logic to use a pot to hit the toggle switch here.
        Entrance(RR_ICE_CAVERN_MQ_HUB,             []{return true;}),
        Entrance(RR_ICE_CAVERN_MQ_ABOVE_BEGINNING, []{return false;}),
    });

    areaTable[RR_ICE_CAVERN_MQ_HUB] = Region("Ice Cavern MQ Hub", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_2, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_1,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_2,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_3,  logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_4,  logic->CanBreakPots()),
    }, {
        //Exits
        //the switch for the glass blocking the entrance is linked to the switch that controls the glass around the skulltulla in RR_ICE_CAVERN_MQ_SCARECROW_ROOM
        //if you clear the ice, you can hit it with a pot from here.
        Entrance(RR_ICE_CAVERN_MQ_BEGINNING,      []{return logic->BlueFire();}),
        Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,       []{return Here(RR_ICE_CAVERN_MQ_HUB, []{return logic->CanKillEnemy(RE_WHITE_WOLFOS) && logic->CanKillEnemy(RE_FREEZARD);});}),
        Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,   []{return (logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) && logic->BlueFire();}),
        Entrance(RR_ICE_CAVERN_MQ_SCARECROW_ROOM, []{return logic->BlueFire();}),
    });

    areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Region("Ice Cavern MQ Map Room", SCENE_ICE_CAVERN, {
        //Events
        //Child can fit between the stalagmites on the left hand side
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return logic->IsChild || logic->CanClearStalagmite();}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_MAP_CHEST, logic->BlueFire() && Here(RR_ICE_CAVERN_MQ_MAP_ROOM, []{return logic->CanHitSwitch();})),
    }, {});

    areaTable[RR_ICE_CAVERN_MQ_SCARECROW_ROOM] = Region("Ice Cavern MQ Scarecrow Room", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return logic->IsAdult && logic->CanGroundJump();}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK, (logic->BlueFire() && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) || (logic->IsAdult && logic->CanHitSwitch(ED_LONG_JUMPSLASH))),
        LOCATION(RC_ICE_CAVERN_MQ_GS_SCARECROW, logic->CanUse(RG_SCARECROW) || (logic->IsAdult && (logic->CanUse(RG_LONGSHOT) || logic->CanGroundJump() || ctx->GetTrickOption(RT_ICE_MQ_SCARECROW)))),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_MQ_HUB,           []{return logic->BlueFire();}),
        //Assumes RR_ICE_CAVERN_MQ_HUB access for a pot if using blue fire
        Entrance(RR_ICE_CAVERN_MQ_WEST_CORRIDOR, []{return logic->IsAdult && logic->BlueFire();}),
    });

    areaTable[RR_ICE_CAVERN_MQ_WEST_CORRIDOR] = Region("Ice Cavern MQ West Corridor", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_1, logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_MQ_SCARECROW_ROOM, []{return logic->BlueFire();}),
        Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM,   []{return true;}),
    });

    areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Region("Ice Cavern MQ Compass Room", SCENE_ICE_CAVERN, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    true),
        //It is possible for child with master, BGS or sticks, or adult with BGS, to hit this switch through the ice with a crouchstab, but it's precise and unintuitive for a trick
        LOCATION(RC_ICE_CAVERN_MQ_FREESTANDING_POH, logic->HasExplosives()), // can get with rang trick
        //doing RT_ICE_MQ_RED_ICE_GS as child is untested, as I could not perform the trick reliably even as adult
        LOCATION(RC_ICE_CAVERN_MQ_GS_RED_ICE,       (logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE) && (logic->CanUse(RG_SONG_OF_TIME) || (logic->IsAdult && ctx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS))) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) ||
                                                    (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && logic->CanUse(RG_ICE_ARROWS)) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_POT_1,    logic->CanBreakPots()),
        LOCATION(RC_ICE_CAVERN_MQ_COMPASS_POT_2,    logic->CanBreakPots()),
    }, {});

    areaTable[RR_ICE_CAVERN_MQ_STALFOS_ROOM] = Region("Ice Cavern MQ Stalfos Room", SCENE_ICE_CAVERN, {}, {
        //Locations
        LOCATION(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, logic->CanKillEnemy(RE_STALFOS)),
        LOCATION(RC_SHEIK_IN_ICE_CAVERN,            logic->CanKillEnemy(RE_STALFOS)), // rando enables this for child
    }, {
        //Exits
        Entrance(RR_ICE_CAVERN_MQ_WEST_CORRIDOR,           []{return Here(RR_ICE_CAVERN_MQ_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}),
        Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER, []{return logic->CanUse(RG_IRON_BOOTS) && Here(RR_ICE_CAVERN_MQ_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}),
    });

    areaTable[RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER] = Region("Ice Cavern MQ Stalfos Room Underwater", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM,    []{return logic->CanUse(RG_BRONZE_SCALE);}),
        Entrance(RR_ICE_CAVERN_MQ_ABOVE_BEGINNING, []{return logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_ICE_CAVERN_MQ_ABOVE_BEGINNING] = Region("Ice Cavern MQ Above Beginning", SCENE_ICE_CAVERN, {}, {}, {
        //Exits
        Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER, []{return logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_ICE_CAVERN_MQ_BEGINNING,               []{return true;}),
    });

#pragma endregion
    // clang-format on
}
