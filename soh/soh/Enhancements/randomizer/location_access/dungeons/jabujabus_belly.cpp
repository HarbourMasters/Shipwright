#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_JabuJabusBelly() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_JABU_JABUS_BELLY_ENTRYWAY] = Region("Jabu Jabus Belly Entryway", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_BEGINNING,    []{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING, []{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ();}),
        Entrance(RR_ZORAS_FOUNTAIN,                []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_JABU_JABUS_BELLY_BEGINNING] = Region("Jabu Jabus Belly Beginning", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY,  []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_LIFT_ROOM, []{return logic->CanUseProjectile();}),
    });

    areaTable[RR_JABU_JABUS_BELLY_LIFT_ROOM] = Region("Jabu Jabus Belly Lift Room", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_DEKU_SCRUB,                  logic->HasItem(RG_BRONZE_SCALE) && (logic->IsChild || logic->HasItem(RG_SILVER_SCALE) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanUse(RG_IRON_BOOTS)) && logic->CanStunDeku() && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_JABU_JABUS_BELLY_PLATFORM_ROOM_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_PLATFORM_ROOM_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_BEGINNING,               []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_HOLES_ROOM,              []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM,          []{return logic->Get(LOGIC_JABU_LOWERED_PATH) || (ctx->GetTrickOption(RT_JABU_BOSS_HOVER) && logic->CanUse(RG_HOVER_BOOTS));}),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_ROOM] = Region("Jabu Jabus Belly Holes Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_LIFT_ROOM,              []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR,        []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE,          []{return logic->Get(LOGIC_JABU_NORTH_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_BASEMENT] = Region("Jabu Jabus Belly Holes Basement", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, logic->HookshotOrBoomerang()),
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_HOLES_ROOM,             []{return true /* logic->HasItem(RG_CLIMB) */;}),
        Entrance(RR_JABU_JABUS_BELLY_B1_JIGGLY,              []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE, []{return logic->IsAdult;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE] = Region("Jabu Jabus Belly Holes Lower Door Ledge", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_HOLES_BASEMENT,          []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_B1_JIGGLY] = Region("Jabu Jabus Belly B1 Cube", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS));}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_1, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_2, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_3, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_4, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_5, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_HOLES_BASEMENT, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH] = Region("Jabu Jabus Belly Water Switch Room North", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_RUTO_IN_1F, []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        //there's tricks for getting here with bunny-jumps or just side-hops
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_JABU_JABUS_BELLY_HOLES_BASEMENT,          []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH] = Region("Jabu Jabus Belly Water Switch Room South", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_JABU_JABUS_BELLY_LIFT_ROOM,               []{return logic->CanUseProjectile();}),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE] = Region("Jabu Jabus Belly Water Switch Room Ledge", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        //this is for climbing back and forth to use pots (or ruto) to kill skull...                                                            or killing the skull before climbing to grab the token
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->HasItem(RG_BRONZE_SCALE) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW)),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_3,       logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORKED_CORRIDOR] = Region("Jabu Jabus Belly Forked Corridor", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_HOLES_ROOM,         []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_FORK_WEST,          []{return logic->Get(LOGIC_JABU_RUTO_IN_1F);}),
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH,      []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_FORK_EAST,          []{return logic->Get(LOGIC_JABU_RUTO_IN_1F);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_WEST] = Region("Jabu Jabus Belly Fork West", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_WEST_TENTACLE, []{return logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MAP_CHEST, logic->Get(LOGIC_JABU_WEST_TENTACLE)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, []{return true;}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST] = Region("Jabu Jabus Belly To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_FORK_NORTH_WEST, []{return logic->Get(LOGIC_JABU_WEST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH_WEST] = Region("Jabu Jabus Belly Fork North West", SCENE_JABU_JABU, {}, {
        //Locations
        //ruto could theoretically clear this room, but it's hard because of the timer and she doesn't appear with you when you respawn after failing, which would force a savewarp
        LOCATION(RC_JABU_JABUS_BELLY_COMPASS_CHEST, logic->CanKillEnemy(RE_SHABOM, ED_CLOSE, false, 9)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_SHABOM, ED_CLOSE, false, 9);});}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH] = Region("Jabu Jabus Belly To Fork North", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_FORK_NORTH,      []{return logic->Get(LOGIC_JABU_EAST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH] = Region("Jabu Jabus Belly Fork North", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_NORTH_TENTACLE, []{return logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG);}),
    }, {}, {
        //Exits
        //implied logic->CanKillEnemy(RE_BARI)
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH, []{return logic->Get(LOGIC_JABU_NORTH_TENTACLE);}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST] = Region("Jabu Jabus Belly MQ To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_FORK_NORTH_EAST, []{return logic->Get(LOGIC_JABU_WEST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH_EAST] = Region("Jabu Jabus Belly Fork North East", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_EAST_TENTACLE, []{return logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG);}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST, []{return logic->Get(LOGIC_JABU_EAST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_EAST] = Region("Jabu Jabus Belly Fork East", SCENE_JABU_JABU, {}, {
        //Locations
        //We can kill the Stingers with ruto
        LOCATION(RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, logic->Get(LOGIC_JABU_RUTO_IN_1F) || logic->CanKillEnemy(RE_STINGER, ED_CLOSE, true, 4)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE] = Region("Jabu Jabus Belly Big Octo Ledge", SCENE_JABU_JABU, {}, {
        //Locations
        //Only adult can get the token without assistance
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)),
        //You can get the LOWER skull token from here as adult with hovers backwalk and a backflip, but it's trickworthy and not relevant unless you can beat tentacles without rang
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_HOLES_BASEMENT, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_BIGOCTO,        []{return logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_BIGOCTO] = Region("Jabu Jabus Belly Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE, []{return logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
        Entrance(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, []{return logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Region("Jabu Jabus Belly Above Big Octo", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_NUT_ACCESS,   []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_1, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_2, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        //when spawning above lift elevator is not raise. if elevator taken up access back down is irrelevant
        Entrance(RR_JABU_JABUS_BELLY_BIGOCTO,       []{return logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_JIGGLIES_ROOM] =  Region("Jabu Jabus Belly Cubes Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_LIFT_UPPER,    []{return logic->CanUse(RG_BOOMERANG) || (logic->IsAdult && logic->CanGroundJump());}),
    });

    areaTable[RR_JABU_JABUS_BELLY_LIFT_UPPER] = Region("Jabu Jabus Belly Lift Upper", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_LOWERED_PATH, []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_LIFT_ROOM,     []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly Near Boss Room", SCENE_JABU_JABU, {}, {
        //Locations
        //can also use ruto
        LOCATION(RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_LIFT_ROOM,     []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, []{return logic->CanUse(RG_BOOMERANG) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_EXPLOSIVES) && (logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_BOMB_BAG))));}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_JABU_JABUS_BELLY_MQ_BEGINNING] = Region("Jabu Jabus Belly MQ Beginning", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_NUT_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,             logic->BlastOrSmash()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_ENTRANCE_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_ENTRANCE_POT_2,        logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_GRASS_2,         logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY,     []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, []{return AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM] = Region("Jabu Jabus Belly MQ Lift Room", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_MQ_LIFT_ROOM_COW, []{return logic->CanUse(RG_FAIRY_SLINGSHOT);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,  true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_HEART_1,             true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_HEART_2,             true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_1,             logic->CanUse(RG_IRON_BOOTS)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_2,             logic->CanUse(RG_IRON_BOOTS)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_3,             logic->CanUse(RG_IRON_BOOTS)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING,                     []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE,             []{return logic->HasItem(RG_SILVER_SCALE) || (logic->HasItem(RG_BRONZE_SCALE) && ((logic->IsChild || logic->CanUse(RG_IRON_BOOTS) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))));}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,                    []{return logic->Get(LOGIC_JABU_MQ_HOLES_ROOM_DOOR);}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE,          []{return logic->Get(LOGIC_JABU_LOWERED_PATH) || logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && logic->Get(LOGIC_JABU_MQ_LIFT_ROOM_COW));}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER, []{return logic->Get(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE] = Region("Jabu Jabus Belly MQ Underwater Alcove", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_MQ_HOLES_ROOM_DOOR, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, logic->CanHitSwitch(ED_HOOKSHOT, true) || (ctx->GetTrickOption(RT_JABU_MQ_RANG_JUMP) && logic->CanUse(RG_BOOMERANG) && logic->HasItem(RG_BRONZE_SCALE))),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GEYSER_POT_1,  logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GEYSER_POT_2,  logic->CanBreakPots()),
        //Getting the ones closest to the ledge with rang may be a trick due to the awkward angle without blind shooting through the flesh
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_1,  logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_2,  logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_3,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, []{return logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM] = Region("Jabu Jabus Belly MQ Holes Room", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_MQ_FORKED_ROOM_DOOR, []{return (logic->HasExplosives() || ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION)) && logic->CanUse(RG_FAIRY_SLINGSHOT);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_PIT_GRASS_1, logic->CanCutShrubs() && logic->HasExplosives()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_PIT_GRASS_2, logic->CanCutShrubs() && logic->HasExplosives()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,              []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT,         []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM_PAST_JIGGLY, []{return logic->CanUse(RG_BOOMERANG);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM_PAST_JIGGLY] = Region("Jabu Jabus Belly MQ Holes Room Past Jiggly", SCENE_JABU_JABU, {}, {}, {
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,      []{return logic->CanUse(RG_BOOMERANG);}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_MQ_FORKED_ROOM_DOOR);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT] = Region("Jabu Jabus Belly MQ Holes Basement", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_1,             logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_2,             logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_3,             logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,           []{return true /* logic->HasItem(RG_CLIMB) */;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO,           []{return logic->Get(LOGIC_JABU_WEST_TENTACLE);}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM,    []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM, []{return logic->Get(LOGIC_JABU_NORTH_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM] = Region("Jabu Jabus Belly MQ Water Switch Room", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS, []{return logic->CanKillEnemy(RE_LIZALFOS);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,          logic->CanKillEnemy(RE_LIZALFOS)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,  (logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) || (ctx->GetTrickOption(RT_JABU_MQ_SOT_GS) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TIME_BLOCK_POT_1,         logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TIME_BLOCK_POT_2,         logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_BOOMERANG_GRASS, logic->CanCutShrubs()),
    }, {
        //Exits
        //without swim, jump from rang chest to the other side
        Entrance(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);});}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,                    []{return (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)) && AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER] = Region("Jabu Jabus Belly MQ Water Switch Room Past Geyser", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM, []{return logic->Get(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS);}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,         []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR] = Region("Jabu Jabus Belly MQ Forked Corridor", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,      []{return logic->CanUse(RG_BOOMERANG);}),
        //If some mode lets an age use sticks and not sling, and other use sling and not sticks, this needs changing
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST,       []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH,      []{return logic->BlastOrSmash();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST,    []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORK_EAST,          []{return AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanUse(RG_STICKS);}) || AnyAgeTime([]{return logic->HasFireSource();});}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST] = Region("Jabu Jabus Belly MQ To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORK_WEST,       []{return logic->Get(LOGIC_JABU_EAST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_WEST] = Region("Jabu Jabus Belly MQ Fork West", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_WEST_TENTACLE, []{return logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG);}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST, []{return true;}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST] = Region("Jabu Jabus Belly MQ To Fork North West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST, []{return logic->Get(LOGIC_JABU_EAST_TENTACLE);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST] = Region("Jabu Jabus Belly MQ Fork North West", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, logic->HasExplosives() && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH] = Region("Jabu Jabus Belly MQ To Fork North", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->BlastOrSmash();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH,      []{return logic->BlastOrSmash() /*&& str0*/;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH] = Region("Jabu Jabus Belly MQ Fork North", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_NORTH_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST] = Region("Jabu Jabus Belly MQ Fork North East", SCENE_JABU_JABU, {}, {
        //Locations
        //Implies CanKillEnemy(RE_LIKE_LIKE)
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIKE_LIKES_POT_1,             logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIKE_LIKES_POT_2,             logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_GRASS,      logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TRIPLE_HALLWAY_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TRIPLE_HALLWAY_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_EAST] = Region("Jabu Jabus Belly MQ Fork East", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_EAST_TENTACLE, []{return logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG);}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM] = Region("Jabu Jabus Belly MQ Invisible Keese Room", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM, //firstly, we can just use FAs to clear the web and then longshot the skull
                                                                   logic->CanUse(RG_FIRE_ARROWS) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) ||
                                                                   //Otherwise, we have to cross the gap and kill the skull.
                                                                   ((logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->IsAdult && logic->CanGroundJump())) &&
                                                                   //We can cheese the gap with hovers
                                                                    ((logic->CanUse(RG_HOVER_BOOTS) ||
                                                                   //Otherwise we have to kill the enemies to raise the platform. This persists so we can do it as the other age.
                                                                        AnyAgeTime([]{return (ctx->GetTrickOption(RT_LENS_JABU_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                                                                                              logic->CanKillEnemy(RE_STINGER, ED_BOOMERANG, false, 2, false, true) && 
                                                                                                                                              //we can hit the keese farthest from the water with irons and hookshot, but we won't be able to see it while doing so
                                                                                                                                              (logic->CanKillEnemy(RE_KEESE, ED_LONGSHOT, false) || (ctx->GetTrickOption(RT_LENS_JABU_MQ) && logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS)));}))
                                                                   //If we kill the enemies, we then need to cross the water using the platform. Note that adult cannot do so while swimming because MQ jank.
                                                                        && ((logic->IsChild && logic->HasItem(RG_BRONZE_SCALE)) || (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS)))))),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, []{return (logic->Get(LOGIC_JABU_NORTH_TENTACLE) || logic->TakeDamage()) && logic->HasItem(RG_BRONZE_SCALE);}),
    });

    // unlike other entrances behind tentacles, Link spawns behind the tentacle. Running into it throws him into main room still
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO] = Region("Jabu Jabus Belly MQ To Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT,  []{return logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_BIGOCTO,         []{return true;}),
    });

    // Need Ruto here
    areaTable[RR_JABU_JABUS_BELLY_MQ_BIGOCTO] = Region("Jabu Jabus Belly MQ Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO,    []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO] = Region("Jabu Jabus Belly MQ Above Big Octo", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_AFTER_BIG_OCTO_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_AFTER_BIG_OCTO_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_BIGOCTO,       []{return logic->TakeDamage() && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);});}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM] = Region("Jabu Jabus Belly MQ Cubes Room", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_COW,                    logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_GRASS,         logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_SMALL_CRATE_1, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_SMALL_CRATE_2, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO,  []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM, []{return logic->CanUse(RG_BOOMERANG) && logic->CanUse(RG_FAIRY_SLINGSHOT);}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM] = Region("Jabu Jabus Belly MQ Above Lift Room", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_JABU_LOWERED_PATH, []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,     []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE] = Region("Jabu Jabus Belly MQ Lift Room East Ledge", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST, logic->Get(LOGIC_JABU_MQ_LIFT_ROOM_COW)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,         []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM, []{return true;}),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly MQ To Near Boss Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE, []{return logic->Get(LOGIC_JABU_NORTH_TENTACLE) || logic->TakeDamage();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM,       []{return logic->Get(LOGIC_JABU_NORTH_TENTACLE);}),
    });


    areaTable[RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly MQ Near Boss Room", SCENE_JABU_JABU, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST,     logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,        logic->CanUse(RG_BOOMERANG) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM, []{return true;}),
        Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY,        []{return AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT);});}),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY] = Region("Jabu Jabus Belly Boss Entryway", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_BOSS_ROOM,      []{return true;}),
    });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_EXIT] = Region("Jabu Jabus Belly Boss Exit", SCENE_JABU_JABU, {}, {}, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM,    []{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla();}),
        Entrance(RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM, []{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ();}),
    });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_ROOM] = Region("Jabu Jabus Belly Boss Room", SCENE_JABU_JABU_BOSS, {
        //Events
        EventAccess(LOGIC_JABU_JABUS_BELLY_CLEAR, []{return logic->CanKillEnemy(RE_BARINADE);}),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_3, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_4, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_5, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_POT_6, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BARINADE_HEART, logic->Get(LOGIC_JABU_JABUS_BELLY_CLEAR)),
        LOCATION(RC_BARINADE,                        logic->Get(LOGIC_JABU_JABUS_BELLY_CLEAR)),
    }, {
        //Exits
        Entrance(RR_JABU_JABUS_BELLY_BOSS_EXIT, []{return false;}),
        Entrance(RR_ZORAS_FOUNTAIN,             []{return logic->Get(LOGIC_JABU_JABUS_BELLY_CLEAR);}, false),
    });

    // clang-format on
}
