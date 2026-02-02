#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_JabuJabusBelly() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_JABU_JABUS_BELLY_ENTRYWAY] = Region("Jabu Jabus Belly Entryway", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_BEGINNING,    ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_BEGINNING, ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ()),
        ENTRANCE(RR_ZORAS_FOUNTAIN,                true),
    });

#pragma region Vanilla

    areaTable[RR_JABU_JABUS_BELLY_BEGINNING] = Region("Jabu Jabus Belly Beginning", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_ENTRYWAY,  true),
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM, logic->CanUseProjectile()),
    });

    areaTable[RR_JABU_JABUS_BELLY_LIFT_ROOM] = Region("Jabu Jabus Belly Lift Room", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_DEKU_SCRUB,                  logic->HasItem(RG_BRONZE_SCALE) && (logic->IsChild || logic->HasItem(RG_SILVER_SCALE) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->CanUse(RG_IRON_BOOTS)) && logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_JABU_JABUS_BELLY_PLATFORM_ROOM_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_PLATFORM_ROOM_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_BEGINNING,               true),
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_ROOM,              true),
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM,          (logic->Get(LOGIC_JABU_LOWERED_PATH) || (ctx->GetTrickOption(RT_JABU_BOSS_HOVER) && logic->CanUse(RG_HOVER_BOOTS))) && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_ROOM] = Region("Jabu Jabus Belly Holes Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM,              true),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR,        true),
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE,          logic->Get(LOGIC_JABU_NORTH_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_BASEMENT] = Region("Jabu Jabus Belly Holes Basement", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, logic->HookshotOrBoomerang()),
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_ROOM,             logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_JABU_JABUS_BELLY_B1_JIGGLY,              true),
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE, logic->IsAdult),
    });

    areaTable[RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE] = Region("Jabu Jabus Belly Holes Lower Door Ledge", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_BASEMENT,          true),
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_B1_JIGGLY] = Region("Jabu Jabus Belly B1 Cube", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_1, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_2, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_3, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_4, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
        LOCATION(RC_JABU_JABUS_BELLY_TWO_OCTOROK_POT_5, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots() && ctx->GetTrickOption(RT_JABU_B1_CUBE_HOVER) && logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_BASEMENT, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH] = Region("Jabu Jabus Belly Water Switch Room North", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_RUTO_IN_1F, (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)) && logic->HasItem(RG_SPEAK_ZORA) && logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        //there's tricks for getting here with bunny-jumps or just side-hops
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE, (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS)) && logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_BASEMENT,          true),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH] = Region("Jabu Jabus Belly Water Switch Room South", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE, (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS)) && logic->HasItem(RG_CLIMB)),
        //Adult can barely reach it with BGS, but it's finnicky enough to be a trick, and too niche to be worth adding yet (only relevant in doorsanity)
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM,               logic->CanHitSwitch(ED_BOMB_THROW)),
    });

    areaTable[RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_LEDGE] = Region("Jabu Jabus Belly Water Switch Room Ledge", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        //this is for climbing back and forth to use pots (or ruto) to kill skull...                                                            
        LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, logic->HasItem(RG_POWER_BRACELET) && (logic->HasItem(RG_BRONZE_SCALE) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS))) ||
                                                           //or killing the skull before climbing to grab the token
                                                           logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW)),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_BASEMENT_POT_3,       logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, true),
        //Assumes RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH access
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM,               logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORKED_CORRIDOR] = Region("Jabu Jabus Belly Forked Corridor", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_ROOM,         true),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORK_WEST,          logic->Get(LOGIC_JABU_RUTO_IN_1F)),
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH,      true),
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORK_EAST,          logic->Get(LOGIC_JABU_RUTO_IN_1F)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_WEST] = Region("Jabu Jabus Belly Fork West", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_WEST_TENTACLE, logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG)),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MAP_CHEST, logic->Get(LOGIC_JABU_WEST_TENTACLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, true),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST] = Region("Jabu Jabus Belly To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORK_NORTH_WEST, logic->Get(LOGIC_JABU_WEST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH_WEST] = Region("Jabu Jabus Belly Fork North West", SCENE_JABU_JABU, {}, {
        //Locations
        //ruto could theoretically clear this room, but it's hard because of the timer and she doesn't appear with you when you respawn after failing, which would force a savewarp
        LOCATION(RC_JABU_JABUS_BELLY_COMPASS_CHEST, logic->CanKillEnemy(RE_SHABOM, ED_CLOSE, false, 9) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST, AnyAgeTime([]{return logic->CanKillEnemy(RE_SHABOM, ED_CLOSE, false, 9);})),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH] = Region("Jabu Jabus Belly To Fork North", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORK_NORTH,      logic->Get(LOGIC_JABU_EAST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH] = Region("Jabu Jabus Belly Fork North", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_NORTH_TENTACLE, logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG)),
    }, {}, {
        //Exits
        //implied logic->CanKillEnemy(RE_BARI)
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH, logic->Get(LOGIC_JABU_NORTH_TENTACLE)),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST] = Region("Jabu Jabus Belly MQ To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_FORK_NORTH_EAST, logic->Get(LOGIC_JABU_WEST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_NORTH_EAST] = Region("Jabu Jabus Belly Fork North East", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_EAST_TENTACLE, logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG)),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST, logic->Get(LOGIC_JABU_EAST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_FORK_EAST] = Region("Jabu Jabus Belly Fork East", SCENE_JABU_JABU, {}, {
        //Locations
        //We can kill the Stingers with ruto
        LOCATION(RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, (logic->Get(LOGIC_JABU_RUTO_IN_1F) || logic->CanKillEnemy(RE_STINGER, ED_CLOSE, true, 4)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE] = Region("Jabu Jabus Belly Big Octo Ledge", SCENE_JABU_JABU, {}, {
        //Locations
        //Only adult can get the token without assistance
        LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)),
        //You can get the LOWER skull token from here as adult with hovers backwalk and a backflip, but it's trickworthy and not relevant unless you can beat tentacles without rang
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_HOLES_BASEMENT, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_BIGOCTO,        logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_BIGOCTO] = Region("Jabu Jabus Belly Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE, logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);})),
        ENTRANCE(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, logic->Get(LOGIC_JABU_RUTO_IN_1F) && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Region("Jabu Jabus Belly Above Big Octo", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_1, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_2, logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_ABOVE_BIG_OCTO_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        //when spawning above lift elevator is not raise. if elevator taken up access back down is irrelevant
        ENTRANCE(RR_JABU_JABUS_BELLY_BIGOCTO,       logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_JIGGLIES_ROOM] =  Region("Jabu Jabus Belly Jigglies Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_UPPER,    logic->CanUse(RG_BOOMERANG) || (logic->IsAdult && logic->CanGroundJump())),
    });

    areaTable[RR_JABU_JABUS_BELLY_LIFT_UPPER] = Region("Jabu Jabus Belly Lift Upper", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_LOWERED_PATH, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM,     true),
        ENTRANCE(RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly Near Boss Room", SCENE_JABU_JABU, {}, {
        //Locations
        //can also use ruto
        LOCATION(RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW) && (logic->HasItem(RG_CLIMB) || logic->HookshotOrBoomerang())),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_LIFT_ROOM,     true),
        ENTRANCE(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, (logic->HasItem(RG_CLIMB) && logic->CanUse(RG_BOOMERANG)) ||
                                                    (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && (logic->CanUse(logic->HasItem(RG_CLIMB) ? RG_HOOKSHOT : RG_LONGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))) ||
                                                    (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_EXPLOSIVES) && (logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_BOMB_BAG) && logic->HasItem(RG_CLIMB))))),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_JABU_JABUS_BELLY_MQ_BEGINNING] = Region("Jabu Jabus Belly MQ Beginning", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_NUT_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,             logic->BlastOrSmash() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_ENTRANCE_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_ENTRANCE_POT_2,        logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_GRASS_1,         logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_GRASS_2,         logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_ENTRYWAY,     true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM] = Region("Jabu Jabus Belly MQ Lift Room", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_MQ_LIFT_ROOM_COW, logic->CanUse(RG_FAIRY_SLINGSHOT)),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_HEART_1,             true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_HEART_2,             true),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_1,             logic->CanUse(RG_IRON_BOOTS)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_2,             logic->CanUse(RG_IRON_BOOTS)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_3,             logic->CanUse(RG_IRON_BOOTS)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_BEGINNING,                     true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE,             logic->HasItem(RG_SILVER_SCALE) || (logic->HasItem(RG_BRONZE_SCALE) && ((logic->IsChild || logic->CanUse(RG_IRON_BOOTS) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))))),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,                    logic->Get(LOGIC_JABU_MQ_HOLES_ROOM_DOOR)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE,          logic->Get(LOGIC_JABU_LOWERED_PATH) || logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && logic->Get(LOGIC_JABU_MQ_LIFT_ROOM_COW))),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER, logic->Get(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE] = Region("Jabu Jabus Belly MQ Underwater Alcove", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_MQ_HOLES_ROOM_DOOR, true),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, (logic->CanHitSwitch(ED_HOOKSHOT, true) || (ctx->GetTrickOption(RT_JABU_MQ_RANG_JUMP) && logic->CanUse(RG_BOOMERANG) && logic->HasItem(RG_BRONZE_SCALE))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GEYSER_POT_1,  logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GEYSER_POT_2,  logic->CanBreakPots()),
        //Getting the ones closest to the ledge with rang may be a trick due to the awkward angle without blind shooting through the flesh
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_1,  logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_2,  logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIFT_RUPEE_3,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, logic->HasItem(RG_BRONZE_SCALE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM] = Region("Jabu Jabus Belly MQ Holes Room", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_MQ_FORKED_ROOM_DOOR, (logic->HasExplosives() || ctx->GetTrickOption(RT_DISTANT_BOULDER_COLLISION)) && logic->CanUse(RG_FAIRY_SLINGSHOT)),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_PIT_GRASS_1, logic->CanCutShrubs() && logic->HasExplosives()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_PIT_GRASS_2, logic->CanCutShrubs() && logic->HasExplosives()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,              true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT,         true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM_PAST_JIGGLY, logic->CanUse(RG_BOOMERANG)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM_PAST_JIGGLY] = Region("Jabu Jabus Belly MQ Holes Room Past Jiggly", SCENE_JABU_JABU, {}, {}, {
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,      logic->CanUse(RG_BOOMERANG)),
        //It is possible to open the door here without crossing the jiggly using RT_DISTANT_BOULDER and good timing
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_MQ_FORKED_ROOM_DOOR)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT] = Region("Jabu Jabus Belly MQ Holes Basement", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_1,             logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_2,             logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_GRASS_3,             logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,           logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO,           logic->Get(LOGIC_JABU_WEST_TENTACLE)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM,    logic->HasItem(RG_SPEAK_ZORA) && logic->HasItem(RG_POWER_BRACELET)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM, logic->Get(LOGIC_JABU_NORTH_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM] = Region("Jabu Jabus Belly MQ Water Switch Room", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS, logic->CanKillEnemy(RE_LIZALFOS)),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,            (logic->IsAdult || logic->HasItem(RG_CLIMB)) && logic->CanKillEnemy(RE_LIZALFOS) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,    (logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) || (ctx->GetTrickOption(RT_JABU_MQ_SOT_GS) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TIME_BLOCK_POT_1,           logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TIME_BLOCK_POT_2,           logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_BOOMERANG_GRASS,   logic->CanCutShrubs()),
    }, {
        //Exits
        //without swim, jump from rang chest to the other side
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER, AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,                    (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)) && AnyAgeTime([]{return logic->CanKillEnemy(RE_LIZALFOS);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER] = Region("Jabu Jabus Belly MQ Water Switch Room Past Geyser", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM, logic->Get(LOGIC_JABU_MQ_WATER_SWITCH_LIFT_ACCESS)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,         true),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR] = Region("Jabu Jabus Belly MQ Forked Corridor", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,         logic->CanUse(RG_BOOMERANG)),
        //If some mode lets an age use sticks and not sling, and other use sling and not sticks, this needs changing
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST,       true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH,      logic->BlastOrSmash() && logic->HasItem(RG_POWER_BRACELET)),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST,    true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORK_EAST,          AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanUse(RG_STICKS);}) || AnyAgeTime([]{return logic->HasFireSource();})),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST] = Region("Jabu Jabus Belly MQ To Fork West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORK_WEST,       logic->Get(LOGIC_JABU_EAST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_WEST] = Region("Jabu Jabus Belly MQ Fork West", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_WEST_TENTACLE, logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG)),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_WEST, true),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST] = Region("Jabu Jabus Belly MQ To Fork North West", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, logic->Get(LOGIC_JABU_EAST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST, logic->Get(LOGIC_JABU_EAST_TENTACLE)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST] = Region("Jabu Jabus Belly MQ Fork North West", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, logic->HasExplosives() && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH_WEST, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH] = Region("Jabu Jabus Belly MQ To Fork North", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, logic->BlastOrSmash()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORK_NORTH,      logic->BlastOrSmash() && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH] = Region("Jabu Jabus Belly MQ Fork North", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_NORTH_TENTACLE, logic->CanUse(RG_BOOMERANG)),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_FORK_NORTH, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST] = Region("Jabu Jabus Belly MQ Fork North East", SCENE_JABU_JABU, {}, {
        //Locations
        //Implies CanKillEnemy(RE_LIKE_LIKE)
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIKE_LIKES_POT_1,             logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_LIKE_LIKES_POT_2,             logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_GRASS,      logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TRIPLE_HALLWAY_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_TRIPLE_HALLWAY_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, true),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_FORK_EAST] = Region("Jabu Jabus Belly MQ Fork East", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_EAST_TENTACLE, logic->CanKillEnemy(RE_TENTACLE, ED_BOOMERANG)),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, true),
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
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT, (logic->Get(LOGIC_JABU_NORTH_TENTACLE) || logic->TakeDamage()) && logic->HasItem(RG_BRONZE_SCALE)),
    });

    // unlike other entrances behind tentacles, Link spawns behind the tentacle. Running into it throws him into main room still
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO] = Region("Jabu Jabus Belly MQ To Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_HOLES_BASEMENT,  logic->Get(LOGIC_JABU_WEST_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_BIGOCTO,         true),
    });

    // Need Ruto here
    areaTable[RR_JABU_JABUS_BELLY_MQ_BIGOCTO] = Region("Jabu Jabus Belly MQ Big Octo", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_BIGOCTO,    AnyAgeTime([]{return logic->HasItem(RG_SPEAK_ZORA) && logic->HasItem(RG_POWER_BRACELET) && logic->CanKillEnemy(RE_BIG_OCTO);})),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO, AnyAgeTime([]{return logic->HasItem(RG_SPEAK_ZORA) && logic->HasItem(RG_POWER_BRACELET) && logic->CanKillEnemy(RE_BIG_OCTO);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO] = Region("Jabu Jabus Belly MQ Above Big Octo", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_AFTER_BIG_OCTO_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_AFTER_BIG_OCTO_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_BIGOCTO,       logic->TakeDamage() && AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_OCTO);})),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM] = Region("Jabu Jabus Belly MQ Cubes Room", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_COW,                    logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_GRASS,         logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_SMALL_CRATE_1, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanBreakSmallCrates()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_JIGGLIES_SMALL_CRATE_2, logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO,  true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM, logic->CanUse(RG_BOOMERANG) && logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM] = Region("Jabu Jabus Belly MQ Above Lift Room", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_LOWERED_PATH, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,     true),
    });

    areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE] = Region("Jabu Jabus Belly MQ Lift Room East Ledge", SCENE_JABU_JABU, {}, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST, logic->Get(LOGIC_JABU_MQ_LIFT_ROOM_COW) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,         true),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM, true),
    });

    // this handles spawning in tentacle
    areaTable[RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly MQ To Near Boss Room", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE, logic->Get(LOGIC_JABU_NORTH_TENTACLE) || logic->TakeDamage()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM,       logic->Get(LOGIC_JABU_NORTH_TENTACLE)),
    });


    areaTable[RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly MQ Near Boss Room", SCENE_JABU_JABU, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST,     logic->CanUse(RG_FAIRY_SLINGSHOT)),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,        (logic->HasItem(RG_CLIMB) && (logic->CanUse(RG_BOOMERANG) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && logic->CanUse(RG_HOOKSHOT)))) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && logic->CanUse(RG_LONGSHOT))),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_GRASS_1, logic->CanCutShrubs()),
        LOCATION(RC_JABU_JABUS_BELLY_MQ_BEFORE_BOSS_GRASS_2, logic->CanCutShrubs()),
    }, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_TO_NEAR_BOSS_ROOM, true),
        ENTRANCE(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY,        AnyAgeTime([]{return logic->CanUse(RG_FAIRY_SLINGSHOT);})),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY] = Region("Jabu Jabus Belly Boss Entryway", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_BOSS_ROOM,      true),
    });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_EXIT] = Region("Jabu Jabus Belly Boss Exit", SCENE_JABU_JABU, {}, {}, {
        //Exits
        ENTRANCE(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM,    ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla()),
        ENTRANCE(RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM, ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ()),
    });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_ROOM] = Region("Jabu Jabus Belly Boss Room", SCENE_JABU_JABU_BOSS, {
        //Events
        EVENT_ACCESS(LOGIC_JABU_JABUS_BELLY_CLEAR, logic->CanKillEnemy(RE_BARINADE)),
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
        ENTRANCE(RR_JABU_JABUS_BELLY_BOSS_EXIT, false),
        ENTRANCE(RR_ZORAS_FOUNTAIN,             logic->Get(LOGIC_JABU_JABUS_BELLY_CLEAR), false),
    });

    // clang-format on
}
