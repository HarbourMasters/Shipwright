#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_SpiritTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_SPIRIT_TEMPLE_ENTRYWAY] = Region("Spirit Temple Entryway", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_LOBBY,            []{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla();}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,         []{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ();}),
        Entrance(RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE, []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_SPIRIT_TEMPLE_LOBBY] = Region("Spirit Temple Lobby", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_LOBBY_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_LOBBY_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_1F_WEST,  []{return logic->IsChild/*CanUse(RG_CRAWL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_1F_EAST,  []{return logic->CanUse(RG_SILVER_GAUNTLETS);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_1F_WEST] = Region("Spirit Temple 1F West", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->NutCrate, []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_LOBBY,               []{return logic->IsChild/*CanUse(RG_CRAWL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_1F_BOXES,            []{return logic->IsChild/*CanUse(RG_CRAWL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_SOUTH, []{return Here(RR_SPIRIT_TEMPLE_1F_WEST, []{return logic->CanKillEnemy(RE_ARMOS) && logic->CanKillEnemy(RE_KEESE);});}),
        Entrance(RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_SOUTH,  []{return Here(RR_SPIRIT_TEMPLE_1F_WEST, []{return logic->CanKillEnemy(RE_ARMOS) && logic->CanKillEnemy(RE_KEESE);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_SOUTH] = Region("Spirit Temple Switch Bridge South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //RANDOTODO a version of CanHitSwitch that takes WallOrFloor
        EventAccess(&logic->SpiritChildSwitchBridge, []{return logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_SPIRIT_CHILD_CHU));}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_WEST,             []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_NORTH, []{return (logic->SpiritChildSwitchBridge && logic->CanPassEnemy(RE_GREEN_BUBBLE, ED_CLOSE, false)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_NORTH] = Region("Spirit Temple Switch Bridge North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->SpiritChildSwitchBridge, []{return logic->CanHitSwitch();}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_SOUTH, []{return logic->CanUse(RG_HOVER_BOOTS) || (logic->SpiritChildSwitchBridge && logic->CanPassEnemy(RE_GREEN_BUBBLE, ED_CLOSE, false));}),
        Entrance(RR_SPIRIT_TEMPLE_1F_ANUBIS,           []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_1F_ANUBIS] = Region("Spirit Temple 1F Anubis", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_ANUBIS_POT_1, true),
        LOCATION(RC_SPIRIT_TEMPLE_ANUBIS_POT_2, true),
        LOCATION(RC_SPIRIT_TEMPLE_ANUBIS_POT_3, true),
        LOCATION(RC_SPIRIT_TEMPLE_ANUBIS_POT_4, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_NORTH, []{return Here(RR_SPIRIT_TEMPLE_1F_ANUBIS, []{return logic->CanHitSwitch() || logic->CanKillEnemy(RE_ANUBIS);});}),
        Entrance(RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_NORTH,  []{return Here(RR_SPIRIT_TEMPLE_1F_ANUBIS, []{return logic->CanHitSwitch() || logic->CanKillEnemy(RE_ANUBIS);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_NORTH] = Region("Spirit Temple Rupee Bridge North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->SpiritRupeeBridge, []{return true/*CanClimb()*/;}),
    }, {
        //Locations
        // these assume SpiritRupeeBridge, silver rupee shuffle & shuffle climb will want to adjust
        LOCATION(RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, logic->HasFireSourceWithTorch()),
        // possible to collect without lowering fence, should be a trick
        LOCATION(RC_SPIRIT_TEMPLE_GS_METAL_FENCE,            logic->SpiritRupeeBridge && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_SOUTH, []{return logic->SpiritRupeeBridge;}),
        Entrance(RR_SPIRIT_TEMPLE_1F_ANUBIS,          []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_SOUTH] = Region("Spirit Temple Rupee Bridge South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_GS_METAL_FENCE, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_WEST,            []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_NORTH, []{return logic->SpiritRupeeBridge;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_1F_BOXES] = Region("Child Spirit Temple Before Climb", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_BEFORE_CHILD_CLIMB_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_SPIRIT_TEMPLE_BEFORE_CHILD_CLIMB_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_WEST,     []{return logic->IsChild/*CanUse(RG_CRAWL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_BROKEN_WALL, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_WEST_CLIMB_BASE] = Region("Spirit Temple Child Climb Base", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM, SpiritShared(RR_SPIRIT_TEMPLE_WEST_CLIMB_BASE, []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG);})),
        LOCATION(RC_SPIRIT_TEMPLE_CHILD_CLIMB_POT_1,    SpiritShared(RR_SPIRIT_TEMPLE_WEST_CLIMB_BASE, []{return logic->CanBreakPots();})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_BOXES,    []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}),
        Entrance(RR_SPIRIT_TEMPLE_BROKEN_WALL, []{return true/*CanClimbHigh()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BROKEN_WALL] = Region("Spirit Temple Broken Wall", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_BROKEN_WALL, []{return logic->CanHitSwitch(ED_BOMB_THROW);})),
        LOCATION(RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  SpiritShared(RR_SPIRIT_TEMPLE_BROKEN_WALL, []{return logic->CanHitSwitch(ED_BOMB_THROW);})),
        LOCATION(RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    SpiritShared(RR_SPIRIT_TEMPLE_BROKEN_WALL, []{return logic->CanKillEnemy(RE_GOLD_SKULLTULA, logic->TakeDamage() ? ED_SHORT_JUMPSLASH : ED_BOMB_THROW);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_WEST_CLIMB_BASE, []{return true;}),
        ///*CanClimbHigh() &&*/ (HasExplosives() || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)))
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,     []{return logic->SpiritBrokenWallToStatue();}),
    });

    areaTable[RR_SPIRIT_TEMPLE_1F_EAST] = Region("Adult Spirit Temple Lobby", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_LOBBY,           []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_SAND_PIT,        []{return Here(RR_SPIRIT_TEMPLE_1F_EAST, []{return logic->CanHitSwitch(logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH) ? ED_BOMB_THROW : ED_BOOMERANG);});}),
        Entrance(RR_SPIRIT_TEMPLE_BOULDERS,        []{return Here(RR_SPIRIT_TEMPLE_1F_EAST, []{return logic->CanHitSwitch(logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH) ? ED_BOMB_THROW : ED_BOOMERANG);});}),
        Entrance(RR_SPIRIT_TEMPLE_EAST_CLIMB_BASE, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SAND_PIT] = Region("Spirit Temple Sand Pit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_COMPASS_CHEST, logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_EAST, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_ABOVE_BOULDERS] = Region("Spirit Temple Above Boulders", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //Jump slash is possible as child, but pretty tight. Jumpslash as late as you can
        //A damage boost off the boulder is also possible, but you need 
        EventAccess(&logic->SpiritBouldersSilvers, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanJumpslash() || logic->CanUse(RG_LONGSHOT);}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_EAST,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_BOULDERS, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BOULDERS] = Region("Spirit Temple Boulders", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM, logic->CanUse(RG_SONG_OF_TIME) && logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_ABOVE_BOULDERS, []{return true/*CanClimb()*/;}),
        Entrance(RR_SPIRIT_TEMPLE_PAST_BOULDERS,  []{return logic->SpiritBouldersSilvers;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_PAST_BOULDERS] = Region("Spirit Temple Past Boulders", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST, true),
        LOCATION(RC_SPIRIT_TEMPLE_BOULDER_ROOM_SUN_FAIRY,  logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BOULDERS, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_EAST_CLIMB_BASE] = Region("Spirit Temple East Climb Base", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_1F_EAST,   []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}),
        Entrance(RR_SPIRIT_TEMPLE_2F_MIRROR, []{return true/*CanClimbHigh()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_2F_MIRROR] = Region("Spirit Temple 2F Mirror", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  SpiritShared(RR_SPIRIT_TEMPLE_2F_MIRROR, []{return true/*str0 || SunlightArrows()*/;})),
        LOCATION(RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_2F_MIRROR, []{return true/*str0 || SunlightArrows()*/;})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_EAST_CLIMB_BASE,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return true/*str0 || SunlightArrows()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST] = Region("Spirit Temple Statue Rooom West", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Assumes RR_SPIRIT_TEMPLE_STATUE_ROOM access
        LOCATION(RC_SPIRIT_TEMPLE_MAP_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->HasFireSourceWithTorch() || (ctx->GetTrickOption(RT_SPIRIT_MAP_CHEST) && logic->CanUse(RG_FAIRY_BOW));}, false,
                                                          RR_SPIRIT_TEMPLE_STATUE_ROOM,      []{return logic->HasFireSource();})),
        LOCATION(RC_SPIRIT_TEMPLE_GS_LOBBY,  SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT);}, false,
                                                          RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE) ? ED_BOOMERANG : ED_HOOKSHOT);},
                                                          RR_SPIRIT_TEMPLE_GS_LEDGE,         []{return logic->CanKillEnemy(RE_GOLD_SKULLTULA);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BROKEN_WALL,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_WEST_HAND, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_GS_LEDGE,        []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SCARECROW);}),
        // RT_SPIRIT_PLATFORM_HOOKSHOT is currently disabled
        Entrance(RR_SPIRIT_TEMPLE_PLATFORM,        []{return logic->SpiritPlatformLowered && 
                                                             (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_SPIRIT_PLATFORM_HOOKSHOT) && logic->CanUse(RG_HOOKSHOT)));}),
        Entrance(RR_SPIRIT_TEMPLE_EMPTY_STAIRS,    []{return true/*str0*/;}),
        //!QUANTUM LOGIC!
        //When child enters spirit in reverse, has 4 keys, and dungeon entrance shuffle is off, 
        //Child cannot lock themselves out of desert colossus access as if they save the west hand lock for last
        //they will be able to exit the dungeon through the intended entrance and vice versa
        //for needing to open the west hand lock to block the intended child route
        Entrance(RR_DESERT_COLOSSUS,               []{return ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && logic->ReverseSpiritChild && /*str0 &&*/
                                                             logic->IsChild/*CanUse(RG_CRAWL)*/ && logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanKillEnemy(RE_IRON_KNUCKLE);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_INNER_WEST_HAND] = Region("Spirit Temple Inner West Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //You can hit the skull with normal hookshot from a specific spot on the forearm, which is in logic on N64 and 3DS, pretty questionable though
        LOCATION(RC_SPIRIT_TEMPLE_GS_LOBBY, SpiritShared(RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE) ? ED_BOOMERANG : ED_HOOKSHOT);}, false,
                                                         RR_SPIRIT_TEMPLE_GS_LEDGE,         []{return logic->CanKillEnemy(RE_GOLD_SKULLTULA);},
                                                         RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_GS_LEDGE,         []{return logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP);})
    });

    areaTable[RR_SPIRIT_TEMPLE_GS_LEDGE] = Region("Spirit Temple GS ledge", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_GS_LOBBY, SpiritShared(RR_SPIRIT_TEMPLE_GS_LEDGE,         []{return logic->CanKillEnemy(RE_GOLD_SKULLTULA);}, false,
                                                         RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE) ? ED_BOOMERANG : ED_HOOKSHOT);},
                                                         RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return true;})
    });

    areaTable[RR_SPIRIT_TEMPLE_STATUE_ROOM] = Region("Spirit Temple Statue Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MAP_CHEST,             SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM,      []{return logic->HasFireSource();}, false,
                                                                      RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return logic->HasFireSourceWithTorch() || (ctx->GetTrickOption(RT_SPIRIT_MAP_CHEST) && logic->CanUse(RG_FAIRY_BOW));})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_1, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_2, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_3, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_4, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_5, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_CENTRAL_CHAMBER_POT_6, SpiritShared(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return logic->CanBreakPots();})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return true/*CanClimb()*/;}),
        // RT_SPIRIT_PLATFORM_HOOKSHOT is currently disabled
        Entrance(RR_SPIRIT_TEMPLE_PLATFORM,         []{return logic->SpiritPlatformLowered && 
                                                              (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_SPIRIT_PLATFORM_HOOKSHOT) && logic->CanUse(RG_HOOKSHOT)));}),
        //explicit adult check here is a precaution against possible child logic leaking, child with a hookshot can do this
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_SHORTCUT,         []{return logic->SpiritStatueRoomSouthDoor;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_EMPTY_STAIRS] = Region("Spirit Temple Empty Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,    []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM] = Region("Spirit Temple Sun Block Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_EMPTY_STAIRS,          []{return true/*str0 || SunlightArrows*/;}),
        //The blocks can be used to get onto this ledge itemless
        //true/*str0 || IsAdult || CanKillEnemy(RE_BEAMOS) || BunnyHovers() ||
        //((HasFireSource() || (SpiritSunBlockTorch && (logic->CanUse(STICKS) || (ctx->GetTrickOption(RT_SPIRIT_SUN_CHEST) && logic->CanUse(RG_FAIRY_BOW))))) && CanUse(RG_HOOKSHOT))*/
        Entrance(RR_SPIRIT_TEMPLE_SUN_BLOCK_SOUTH_LEDGE, []{return logic->SpiritSunBlockSouthLedge();}),
        Entrance(RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS,      []{return true/*str0 || SunlightArrows*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SUN_BLOCK_SOUTH_LEDGE] = Region("Spirit Temple Sun Block South ledge", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //Assumes RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM access
        EventAccess(&logic->SpiritSunBlockTorch, []{return SpiritShared(RR_SPIRIT_TEMPLE_SUN_BLOCK_SOUTH_LEDGE, []{return true;}, true);}),
    }, {
        //Locations
        //Spawning the chest to get here is accounted for in movement logic, so we only need to confirm it can be spawned here
        LOCATION(RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_SUN_BLOCK_SOUTH_LEDGE, 
                                                                     []{return logic->HasFireSource() || 
                                                                               (logic->SpiritSunBlockTorch && //Possible without str0 by timing a shot during a backflip from the ledge, but it's probably a separate trick
                                                                                (logic->CanUse(RG_STICKS) || (ctx->GetTrickOption(RT_SPIRIT_SUN_CHEST) && logic->CanUse(RG_FAIRY_BOW)/* && str0*/)));})), 
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS] = Region("Spirit Temple Skulltula Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, SpiritShared(RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, []{return logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG);})),
        LOCATION(RC_SPIRIT_TEMPLE_AFTER_SUN_BLOCK_POT_1,        SpiritShared(RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_AFTER_SUN_BLOCK_POT_2,        SpiritShared(RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, []{return logic->CanBreakPots();})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_WEST_THRONE,    []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_WEST_THRONE] = Region("Spirit Temple West Throne", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3);}),
        Entrance(RR_SPIRIT_TEMPLE_WEST_HAND_EXIT,   []{return Here(RR_SPIRIT_TEMPLE_WEST_THRONE, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_WEST_HAND_EXIT] = Region("Spirit Temple West Hand Exit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_WEST_THRONE,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_OUTER_WEST_HAND, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_OUTER_WEST_HAND] = Region("Spirit Temple West Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_OUTER_WEST_HAND, []{return true;})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_WEST_HAND_EXIT, []{return true;}),
        Entrance(RR_DESERT_COLOSSUS,              []{return SpiritCertainAccess(RR_SPIRIT_TEMPLE_OUTER_WEST_HAND);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST] = Region("Spirit Temple Statue Room East", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_2F_MIRROR,       []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_EAST_HAND, []{return true;}),
        //(IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP)) || CanUse(RG_HOVER_BOOTS) || (CanUse(RG_ZELDAS_LULLABY) && CanUse(RG_HOOKSHOT));
        Entrance(RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH, []{return logic->SpiritEastToSwitch();}),
        Entrance(RR_SPIRIT_TEMPLE_POT_STAIRS,      []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4);}),
        //!QUANTUM LOGIC!
        //With 3 keys, you cannot lock adult out of leaving spirit onto the hands and jumping down, as you would have to 
        //open the west hand door and then adult could climb through sun block room to jump down from there
        //This requires that adult can complete both routes
        //Implies CanKillEnemy(RE_IRON_KNUCKLE)
        Entrance(RR_DESERT_COLOSSUS,               []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanKillEnemy(RE_BEAMOS)/* && CanClimb() && str0*/;}),
        //!QUANTUM LOGIC!
        //A variant of the above, if dungeon entrance randomiser is off, Adult entered spirit in reverse, and they have str 2 to get there from the front,
        //it is always possible for them to reach 1F_EAST with only 2 keys. This is because you can only waste 1 key (on the first child side lock)
        //before you either allow you to climb down through 2F mirror room, or give yourself access to a hand to jump down from.
        //If for whatever reason you can reach east hand but not west hand, this becomes possible with 3 keys instead.
        //If for whatever reason you can reach west hand but not east hand, this becomes possible with 4 keys instead.
        Entrance(RR_SPIRIT_TEMPLE_1F_EAST,         []{return ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) &&
                                                             logic->CanUse(RG_SILVER_GAUNTLETS) && 
                                                             ((logic->CanKillEnemy(RE_BEAMOS) && logic->SmallKeys(RR_SPIRIT_TEMPLE, /*CanClimb() && str0 ?*/ 2 /*: 3*/)) ||
                                                              (/*CanClimb() && str0 && */logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 4)));}),
        //RT_SPIRIT_PLATFORM_HOOKSHOT is currently disabled
        Entrance(RR_SPIRIT_TEMPLE_PLATFORM,        []{return logic->SpiritPlatformLowered && 
                                                             (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_SPIRIT_PLATFORM_HOOKSHOT) && logic->CanUse(RG_HOOKSHOT)));}),
    });

    areaTable[RR_SPIRIT_TEMPLE_INNER_EAST_HAND] = Region("Spirit Temple Inner East Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Assumes RR_SPIRIT_TEMPLE_INNER_WEST_HAND access via RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST and RR_SPIRIT_TEMPLE_STATUE_ROOM.
        LOCATION(RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_INNER_EAST_HAND, []{return logic->CanUse(RG_ZELDAS_LULLABY)/* && CanClimb()*/;})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return logic->CanUse(RG_HOVER_BOOTS);}),
        //Assumes RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST access via RR_SPIRIT_TEMPLE_STATUE_ROOM
        Entrance(RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH,  []{return (logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP)) ||
                                                              (logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanUse(RG_HOOKSHOT));}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH] = Region("Spirit Temple Shortcut Switch", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->SpiritStatueRoomSouthDoor, []{return SpiritShared(RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH, []{return logic->CanUse(RG_MEGATON_HAMMER);});}),
    }, {
        //Locations
        //Assumes RR_SPIRIT_TEMPLE_INNER_EAST_HAND access
        LOCATION(RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_SHORTCUT_SWITCH, []{return logic->CanUse(RG_ZELDAS_LULLABY);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_EAST_HAND,  []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_SHORTCUT] = Region("Spirit Temple Shortcut", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //If child can ever use silver gauntlets, there needs to be an event here to account for child entering in reverse
        //opening the way for adult entering via the front.
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_LOBBY,       []{return logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanUse(RG_MEGATON_HAMMER);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_POT_STAIRS] = Region("Spirit Temple Pot Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        LOCATION(RC_SPIRIT_TEMPLE_BEAMOS_HALL_POT_1, logic->CanBreakPots()),
    }, {
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4);}),
        Entrance(RR_SPIRIT_TEMPLE_BEAMOS_PITS,      []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BEAMOS_PITS] = Region("Spirit Temple Beamos Pits", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        //Implies killing the anubis with the fire ring, doing so itemless requires voiding out, which can lock hardcore + OHKO seeds
        Entrance(RR_SPIRIT_TEMPLE_POT_STAIRS,    []{return logic->CanKillEnemy(RE_BEAMOS);}),
        Entrance(RR_SPIRIT_TEMPLE_FOUR_ARMOS,    []{return logic->CanKillEnemy(RE_BEAMOS);}),
        Entrance(RR_SPIRIT_TEMPLE_BIG_WALL_BASE, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_FOUR_ARMOS] = Region("Spirit Temple Four Armos", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST, (logic->CanUse(RG_MIRROR_SHIELD) || logic->SunlightArrows()) && logic->HasExplosives()),
        LOCATION(RC_SPIRIT_TEMPLE_ARMOS_ROOM_SUN_FAIRY,  logic->HasExplosives() && logic->CanUse(RG_SUNS_SONG)),
    }, {
        Entrance(RR_SPIRIT_TEMPLE_BEAMOS_PITS,          []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_FOUR_ARMOS_SIDE_ROOM, []{return logic->CanUse(RG_MIRROR_SHIELD) || logic->SunlightArrows();}),
        Entrance(RR_SPIRIT_TEMPLE_CHEST_STAIRS,         []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_FOUR_ARMOS_SIDE_ROOM] = Region("Spirit Temple Four Armos Side Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_FOUR_ARMOS, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_CHEST_STAIRS] = Region("Spirit Temple Chest Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  ctx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)),
        LOCATION(RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_FOUR_ARMOS,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_EAST_THRONE, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_EAST_THRONE] = Region("Spirit Temple East Throne", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_CHEST_STAIRS,   []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_EAST_HAND_EXIT, []{return Here(RR_SPIRIT_TEMPLE_EAST_THRONE, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_EAST_HAND_EXIT] = Region("Spirit Temple East hand Exit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_EAST_THRONE,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_OUTER_EAST_HAND, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_OUTER_EAST_HAND] = Region("Spirit Temple Outer East Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_EAST_HAND_EXIT,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_OUTER_WEST_HAND, []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_DESERT_COLOSSUS,               []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BIG_WALL_BASE] = Region("Spirit Temple Big Wall Base", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BEAMOS_PITS,    []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_BIG_WALL_UPPER, []{return (ctx->GetTrickOption(RT_SPIRIT_WALL) || 
                                                             (logic->CanAvoidEnemy(RE_BEAMOS, true, 2) && logic->CanPassEnemy(RE_WALLTULA, ED_BOOMERANG)))
                                                            /*&& CanClimbHigh()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BIG_WALL_UPPER] = Region("Spirit Temple Big Wall Upper", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Grabbing these with rang is possible, but requires a blind shot aimed high
        LOCATION(RC_SPIRIT_TEMPLE_ADULT_CLIMB_LEFT_HEART,  logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_SPIRIT_TEMPLE_ADULT_CLIMB_RIGHT_HEART, logic->CanUse(RG_HOOKSHOT)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BIG_WALL_BASE, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_4F_CENTRAL,    []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_4F_CENTRAL] = Region("Spirit Temple 4F Central", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BIG_WALL_UPPER,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_FAKE_DOORS_ROOM, []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        Entrance(RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_FAKE_DOORS_ROOM] = Region("Spirit Temple Fake Doors Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, (logic->TakeDamage() && ctx->GetTrickOption(RT_FLAMING_CHESTS)) || 
                                                  (Here(RR_SPIRIT_TEMPLE_FAKE_DOORS_ROOM, []{return logic->CanHitEyeTargets() && logic->CanAvoidEnemy(RE_TORCH_SLUG, true, 4);}) 
                                                   && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_4F_CENTRAL, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM] = Region("Spirit Temple Big Mirror Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->Spirit4FSwitch,        []{return logic->CanJumpslash() || logic->HasExplosives();}),
        //Needs the mirror in the cave to be a perm flag and event for doorsanity
        EventAccess(&logic->SpiritPlatformLowered, []{return (logic->SpiritPushed4FMirrors && logic->CanUse(RG_MIRROR_SHIELD)) || logic->SunlightArrows();}),

    }, {}, {
        Entrance(RR_SPIRIT_TEMPLE_4F_CENTRAL,      []{return true;}),
        //Rang can hit the switch on the way back but that's a trick
        Entrance(RR_SPIRIT_TEMPLE_BIG_MIRROR_CAVE, []{return logic->Spirit4FSwitch;}),
        //Assumes RR_SPIRIT_TEMPLE_BIG_MIRROR_CAVE access
        Entrance(RR_SPIRIT_TEMPLE_PLATFORM,        []{return logic->SpiritPlatformLowered;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BIG_MIRROR_CAVE] = Region("Spirit Temple Big Mirror Cave", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->Spirit4FSwitch,        []{return logic->HasExplosives();}),
        //WARNING: not currently a permanent flag, needs to be one for doorsanity
        EventAccess(&logic->SpiritPushed4FMirrors, []{return logic->HasExplosives()/*&& str0*/;}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_TOPMOST_CHEST, (logic->IsAdult && logic->CanUse(RG_MIRROR_SHIELD)) || logic->SunlightArrows()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM, []{return true;}),
    });

    //Assumes a SpiritPlatformLowered check on entry
    areaTable[RR_SPIRIT_TEMPLE_PLATFORM] = Region("Spirit Temple Lowered Platform", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_WEST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM_EAST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_EAST_HAND,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_STATUE_HEAD,      []{return logic->SpiritPushed4FMirrors && logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_STATUE_HEAD] = Region("Spirit Temple Statue Head", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //WARNING these events are not glitchproofed and assume you need all keys to reach from the front
        EventAccess(&logic->ReverseSpiritChild, []{return logic->IsChild;}),
        EventAccess(&logic->ReverseSpiritAdult, []{return logic->IsAdult;}),
    }, {}, {
        // Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_WEST_HAND,  []{return logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_INNER_EAST_HAND,  []{return logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_PLATFORM,         []{return logic->SpiritPlatformLowered && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY,    []{return logic->HasItem(RG_SPIRIT_TEMPLE_BOSS_KEY);}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_SPIRIT_TEMPLE_MQ_LOBBY] = Region("Spirit Temple MQ Lobby", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST,  true),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,   Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST,  logic->CanHitSwitch(ED_BOOMERANG)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, logic->Spirit1FSilverRupees),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_POT_1,             logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_POT_2,             logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_POT_3,             logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_POT_4,             logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,                []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,              []{return logic->IsChild/*logic->CanUse(RG_CRAWL) && logic->HasSoul(RG_NABOORU_SOUL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST,              []{return false;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH, []{return logic->CanUse(RG_LONGSHOT) && logic->CanUse(RG_BOMBCHU_5);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_1F_WEST] = Region("Spirit Temple MQ 1F West", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //not technically a rusted switch, but a boulder through a wall, but is part of the same trick on N64
        EventAccess(&logic->MQSpiritCrawlBoulder, []{return logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && logic->CanUse(RG_MEGATON_HAMMER));}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, logic->MQSpiritTimeTravelChest),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_SLUGMA_POT,          logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_LEFT_HEART,          logic->CanHitEyeTargets()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_RIGHT_HEART,         logic->CanHitEyeTargets()),
    }, {
        //Exits
        //Nabooru's legs are technically visible one way collision here, but I'm not sure if this counts
        Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,                 []{return logic->IsChild/*logic->CanUse(RG_CRAWL)*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_SOUTH,   []{return Here(RR_SPIRIT_TEMPLE_MQ_1F_WEST, []{return logic->CanKillEnemy(RE_TORCH_SLUG);});}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH,        []{return Here(RR_SPIRIT_TEMPLE_MQ_1F_WEST, []{return logic->CanKillEnemy(RE_TORCH_SLUG);});}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH, []{return logic->IsChild/*logic->CanUse(RG_CRAWL)*/ && logic->MQSpiritCrawlBoulder;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_SOUTH] = Region("Spirit Temple MQ 1F Gibdo Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->MQSpiritGibdosCleared, []{return logic->CanUse(RG_BOMBCHU_5) && logic->CanHitEyeTargets() && logic->CanKillEnemy(RE_GIBDO, ED_CLOSE, true, 3);}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,             []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH, []{return logic->CanUse(RG_BOMBCHU_5) && logic->CanHitEyeTargets()/* && str0*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH] = Region("Spirit Temple MQ Gibdo Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_GIBDO_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_GIBDO_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM,    []{return logic->MQSpiritGibdosCleared;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM] = Region("Spirit Temple MQ Turntable Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->FairyPot, []{return Here(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}),
    }, {
        //Locations
        //implies logic->CanBreakPots()
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_STALFOS_POT_1, logic->CanUse(RG_BOOMERANG) || logic->CanKillEnemy(RE_STALFOS)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_STALFOS_POT_2, logic->CanUse(RG_BOOMERANG) || logic->CanKillEnemy(RE_STALFOS)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_STALFOS_POT_3, logic->CanUse(RG_BOOMERANG) || logic->CanKillEnemy(RE_STALFOS)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_STALFOS_POT_4, logic->CanUse(RG_BOOMERANG) || logic->CanKillEnemy(RE_STALFOS)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH,      []{return Here(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH] = Region("Spirit Temple MQ Map Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->MQSpiritMapRoomEnemies, []{return /*(*/logic->CanKillEnemy(RE_ANUBIS) && logic->CanKillEnemy(RE_KEESE)/*) && (str0 || CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT)) ||
                                                                logic->CanKillEnemy(RE_ANUBIS, ED_BOOMERANG) && logic->CanKillEnemy(RE_KEESE, ED_FAR)*/;}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,  logic->MQSpiritMapRoomEnemies),
    }, {
        //Exits
        //Stalfos room blocks you in with fire until you kill the stalfos, which won't spawn from behind the fire
        Entrance(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM, []{return false;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH, []{return true/*str0 || CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT)*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH] = Region("Spirit Temple MQ Map Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //You can lure the keese over by aggroing them with dins if you use it as close to the torch keese as possible, but it's a trick as it's not intuitive and basically never comes up
        EventAccess(&logic->MQSpiritMapRoomEnemies, []{return logic->CanKillEnemy(RE_ANUBIS) && logic->CanKillEnemy(RE_KEESE, ED_BOOMERANG);}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, true),
    }, {
        //Exits
        //The bridge is a temp flag, so not a way to cross south to north in logic
        Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH, []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,        []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH] = Region("Spirit Temple MQ West 1F Rusted Switch", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->MQSpiritTimeTravelChest, []{return logic->CanUse(RG_MEGATON_HAMMER);}),
        EventAccess(&logic->MQSpiritCrawlBoulder,    []{return logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && logic->CanUse(RG_MEGATON_HAMMER));}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,         []{return logic->IsChild/*logic->CanUse(RG_CRAWL)*/ && logic->MQSpiritCrawlBoulder;}),
        //This tracks possible child access, if adult has not entered STATUE_ROOM. Certain Child Access is checked for separately as 7 Keys
        Entrance(RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE] = Region("Spirit Temple MQ Under Like Like", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_LIKE_LIKE_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, []{return logic->CanBreakPots();})),
    }, {
        //Exits
        //This covers adult and reverse access only, as child going forwards arrives here from the other side of this door
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM,      []{return logic->CanHitSwitch()/* && CanClimbHigh()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM] = Region("Spirit Temple MQ Broken Wall Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Implies CanKillEnemy(RE_LIKE_LIKE)
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM, []{return logic->CanKillEnemy(RE_BEAMOS);})),
        //Sunlights only temp spawn this chest, which is unintuitive/a bug.
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM, []{return (logic->HasExplosives() || logic->SunlightArrows()) && logic->CanUse(RG_HOOKSHOT);})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, []{return logic->CanHitSwitch();}),
        //This exit only governs child forwards access, adult and reverse access starts on the other side so never checks this
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,     []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 2);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST] = Region("Spirit Temple MQ Statue Room West", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_SMALL_CRATE, SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST, []{return logic->CanBreakSmallCrates();})),
    }, {
        //Exits
        //!QUANTUM LOGIC!
        //If we entered in reverse and dungeon entrances are off, we only need 6 keys, access to Gauntlets Hand and the ability to crawl to reach colossus
        //This is because with 6 keys it becomes impossible to avoid opening either the west hand lock or the first child side lock
        //and either direction lets child reach colossus. CanHitSwitch and CanKillEnemy(RE_IRON_KNUCKLE) is implied.
        //Logic can then allow child back into spirit, putting 1F west in logic with only 6 keys without forwards entry
        Entrance(RR_DESERT_COLOSSUS,                      []{return logic->IsChild/*CanUse(RG_CRAWL)*/ && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) &&
                                                                    logic->ReverseSpiritChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 6) && /*(str0 || SunlightArrows())&&*/
                                                                    (logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && logic->CanUse(RG_MEGATON_HAMMER)));}),
        //!QUANTUM LOGIC!
        //If we have 6 keys and Child reverse spirit entry, we can gurantee broken wall room access for Child
        //as long as we can hit a switch and because Adult cannot reach the initial child lock without 
        //first opening the Statue Room to Broken Wall Room lock. The details of this are handled in SpiritShared.
        //if adult can ever cross crawlspaces this becomes more complicated.
        Entrance(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM,    []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_POT_LEDGE,           []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND,     []{return logic->IsAdult || logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS);}),
        //(IsAdult || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || CanUse(RG_SONG_OF_TIME)) && str0
        Entrance(RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS, []{return logic->MQSpiritStatueToSunBlock();}),
        //explicit adult check here is a precaution against possible child logic leaking, child with a hookshot can do this
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,    []{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_POT_LEDGE] = Region("Spirit Temple MQ Pot Ledge", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //This event does not need handling in SpiritShared as it only affects navigation, adult access here is always Certain, and Child has no way through that Adult does not.
        EventAccess(&logic->SpiritStatueRoomSouthDoor, []{return ctx->GetTrickOption(RT_SPIRIT_MQ_FROZEN_EYE) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME);}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_3F_EAST_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_POT_LEDGE       , []{return logic->CanBreakPots();}, false,
                                                                      RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND , []{return logic->CanUse(RG_BOOMERANG);})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_3F_WEST_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_POT_LEDGE       , []{return logic->CanBreakPots();}, false,
                                                                      RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND , []{return ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE) && logic->CanUse(RG_BOOMERANG);})),
    }, {
        //Exits
        //This is pretty tight to reach the SoT block, but you can just go via the hand...
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND,  []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND] = Region("Spirit Temple MQ Inner West Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_3F_EAST_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND, []{return logic->CanUse(RG_BOOMERANG);}, false,
                                                                      RR_SPIRIT_TEMPLE_MQ_POT_LEDGE,       []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_3F_WEST_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND, []{return logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE);}, false,
                                                                      RR_SPIRIT_TEMPLE_MQ_POT_LEDGE,       []{return logic->CanBreakPots();})),
    }, {
        //Exits
        //Land on the SoT block
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_POT_LEDGE,        []{return logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP);}),
        // RT_SPIRIT_PLATFORM_HOOKSHOT is currently disabled
        Entrance(RR_SPIRIT_TEMPLE_MQ_PLATFORM,         []{return logic->SpiritPlatformLowered && 
                                                                 (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_SPIRIT_PLATFORM_HOOKSHOT) && logic->CanUse(RG_HOOKSHOT)));}),
    });
    
    areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM] = Region("Spirit Temple MQ Statue Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //This event does not need handling in SpiritShared as it only affects navigation, Adult access here is always Certain, and Child has no way through that adult does not.
        EventAccess(&logic->SpiritStatueRoomSouthDoor, []{return logic->HasFireSource();}),
        //Assuming all higher areas filter down to here for this despite there being many good angles to use FAs
        EventAccess(&logic->MQSpiritStatueRoomTorches, []{return logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_MQ_LOWER_ADULT) && logic->CanUse(RG_DINS_FIRE));}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,             SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanHitEyeTargets();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_2F_CENTER_EAST_POT, SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_2F_WEST_POT,        SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_2F_EASTMOST_POT,    SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_CRATE_1,            SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanBreakCrates();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_CRATE_2,            SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanBreakCrates();})),
        //The drop sometimes flies off the block when the crate is blown up, but not always, so I added a rang requirement
        //A trick to reload for it is plausible
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_SMALL_CRATE,        SpiritShared(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return (logic->CanUse(RG_SONG_OF_TIME) && logic->CanBreakSmallCrates()) ||
                                                                                                                        (logic->CanUse(RG_BOOMERANG) && logic->HasExplosives());})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST,     []{return true/*logic->CanClimb()*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH, []{return logic->SpiritStatueRoomSouthDoor;}),
        //explicit adult check here is a precaution against possible Child logic leaking, Child with a hookshot can do this
        //It's possible to climb the legs of the statue to line up a longshot hit, but you can just go via West
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,     []{return logic->IsAdult && logic->CanUse(RG_SCARECROW);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS] = Region("Spirit Temple MQ Flamethrower Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,    []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM] = Region("Spirit Temple MQ Sun Block Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //We don't need Shared here because If we are checking as child, universe 2 adult access needs nothing so it always passes, and if we are checking as adult, it is Certain Access
        LOCATION(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return true/*str0*/;})),
        // RT_SPIRIT_MQ_SUN_BLOCK_GS should probably be expanded to cover all ground based methods when str0 is added, as it can be hit with longshot because the skull hitbox is larger than the model
        LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,    SpiritShared(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return (logic->CanUse(RG_HOOKSHOT)/* && (str0 || SunlightArrows())*/) || 
                                                                                                                      (ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_GS) && logic->CanUse(RG_BOOMERANG));})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCKS_POT_1,     SpiritShared(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return logic->CanBreakPots();})),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCKS_POT_2,     SpiritShared(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return logic->CanBreakPots();})),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS, []{return true/*str0 || SunlightArrows()*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS   , []{return true/*str0 || SunlightArrows()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS] = Region("Spirit Temple MQ Skulltula Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_THRONE,    []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_WEST_THRONE] = Region("Spirit Temple MQ West Throne", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_HAND_EXIT,   []{return Here(RR_SPIRIT_TEMPLE_MQ_WEST_THRONE, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_WEST_HAND_EXIT] = Region("Spirit Temple MQ West Hand Exit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_THRONE    , []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND] = Region("Spirit Temple MQ Outer West Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND, []{return true;})),
    }, {
        //Exits
        //If it is ever relevent for 1 age to spawn the mirror shield chest for the other can longshot across, it needs an eventAccess
        Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_HAND_EXIT, []{return true;}),
        Entrance(RR_DESERT_COLOSSUS,                 []{return SpiritCertainAccess(RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH] = Region("Spirit Temple MQ Block Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,                []{return true;}),
        //The block here is unusual in that it is a permanent flag, but reset anyway as child. This is because there's a check that would be blocked off by pushing them otherwise
        //It may be worth considering making this always temp in future so adult doesn't have the same issue
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH, []{return logic->IsChild ? logic->CanUse(RG_SILVER_GAUNTLETS) : Here(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH, []{return logic->CanUse(RG_SILVER_GAUNTLETS);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH] = Region("Spirit Temple MQ Block Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //Hard child locked because pushing the block is a permanent flag that blocks the eye target and cannot be undone
        //but it resets as Child as a rando QoL feature
        LOCATION(RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, SpiritShared(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH, []{return logic->IsChild && logic->CanHitEyeTargets();})),
    }, {
        //Exits
        //if going to RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH from here is ever relevant, there needs to be an event to handle the block
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST] = Region("Spirit Temple MQ Statue Room East", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,           []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_EAST_HAND,       []{return logic->IsAdult || logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_CHEST_LEDGE,           []{return logic->CanUse(RG_HOVER_BOOTS) || 
                                                                      ((ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_LONGSHOT));}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F,    []{return logic->MQSpiritStatueRoomTorches;}),
        //!QUANTUM LOGIC!
        //We only need 4 keys and the ability to reach both hands for adult to logically be able to drop down onto Desert Colossus
        //This is because there are only 3 keys that can be wasted without opening up either this lock to East hand, or the West Hand lock through Sun Block Room
        //and both directions allow you to drop onto colossus
        //logic->CanKillEnemy(RE_FLOORMASTER) is implied
        Entrance(RR_DESERT_COLOSSUS,                        []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) &&
                                                                      logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME) &&
                                                                      logic->CanJumpslash() && /*(str0 || SunlightArrows) &&*/
                                                                      (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                      logic->CanKillEnemy(RE_IRON_KNUCKLE) &&
                                                                      logic->CanUse(RG_HOOKSHOT);}),
        //!QUANTUM LOGIC!
        //Continuing from above, if we also have a longshot, we can go from the East hand to the West hand, meaning we always have access to East Hand
        /* 
                                                                       logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME) &&
                                                                       logic->CanJumpslash() && /*(str0 || SunlightArrows) &&
                                                                       (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                       logic->CanKillEnemy(RE_IRON_KNUCKLE) &&
                                                                       logic->CanUse(RG_LONGSHOT) */
        Entrance(RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND,        []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->MQSpirit4KeyWestHand();}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_LOWER, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}),
        // RT_SPIRIT_PLATFORM_HOOKSHOT is currently disabled
        Entrance(RR_SPIRIT_TEMPLE_MQ_PLATFORM,               []{return logic->SpiritPlatformLowered && 
                                                                       (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_SPIRIT_PLATFORM_HOOKSHOT) && logic->CanUse(RG_HOOKSHOT)));}),
        });

    areaTable[RR_SPIRIT_TEMPLE_MQ_INNER_EAST_HAND] = Region("Spirit Temple MQ Inner East Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanBreakCrates()),
        }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_CHEST_LEDGE,      []{return ((logic->IsAdult || ctx->GetTrickOption(RT_SPIRIT_WEST_LEDGE)) && (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOOKSHOT))
                                                                 || (logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP));}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_CHEST_LEDGE] = Region("Spirit Temple MQ Chest Ledge", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST, []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_EAST_HAND,  []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F] = Region("Spirit Temple MQ Three Suns Room 2F", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //implies logic->CanKillEnemy(RE_WALLMASTER). If we have lights, we can kill stalfos and wallmasters with bow
        EventAccess(&logic->MQSpirit3SunsEnemies, []{return (logic->CanUse(RG_MIRROR_SHIELD) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)) || logic->SunlightArrows();}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,   []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F, []{return logic->MQSpirit3SunsEnemies;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F] = Region("Spirit Temple MQ Three Suns Room 1F", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        //It is possible to clip through here with longshot by aiming high enough
        Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F, []{return logic->MQSpirit3SunsEnemies/* && CanClimbHigh()*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST,            []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_1F_EAST] = Region("Spirit Temple MQ 1F East", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //Assumes RR_SPIRIT_TEMPLE_MQ_LOBBY access
        EventAccess(&logic->Spirit1FSilverRupees, []{return logic->CanUse(RG_MEGATON_HAMMER);}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_EARLY_ADULT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_EARLY_ADULT_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,               []{return logic->CanUse(RG_MEGATON_HAMMER);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_SAND_PIT,            []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SAND_PIT] = Region("Spirit Temple MQ Sand Pit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, logic->CanKillEnemy(RE_PURPLE_LEEVER) && logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,    logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST, []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER] = Region("Spirit Temple MQ Symphony Room Upper", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->MQSpiritSymphonyRoomDoor, []{return logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_EPONAS_SONG) &&
                                                                logic->CanUse(RG_SUNS_SONG) && logic->CanUse(RG_SONG_OF_STORMS) && logic->CanUse(RG_ZELDAS_LULLABY);}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST,             []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}),
        //Implies CanPassEnemy(RE_MOBLIN_CHIEF)
        Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_LOWER, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_LOWER] = Region("Spirit Temple MQ Symphony Room Lower", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER, []{return true/*CanClimb()*/;}),
        //Implies CanPassEnemy(RE_MOBLIN_CHIEF)
        Entrance(RR_SPIRIT_TEMPLE_MQ_AFTER_SYMPHONY_ROOM, []{return logic->MQSpiritSymphonyRoomDoor;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_AFTER_SYMPHONY_ROOM] = Region("Spirit Temple MQ After Symphony Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,    logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_LOWER] = Region("Spirit Temple MQ Fire Wall Stairs Lower", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,       []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_UPPER, []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_UPPER] = Region("Spirit Temple MQ Fire Wall Stairs Upper", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_LOWER, []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,            []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS] = Region("Spirit Temple MQ Beamos Pits", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,  logic->CanKillEnemy(RE_BEAMOS)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BEAMOS_SMALL_CRATE, logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_UPPER, []{return logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME)/* && str0*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,           []{return logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME)/* && str0*/;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL_LOWER,         []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM] = Region("Spirit Temple MQ SoT Sun Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,      true),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_DINALFOS_ROOM_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,        []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS, []{return logic->CanJumpslash();}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM,      []{return Here(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, []{return (logic->IsAdult || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanUse(RG_MIRROR_SHIELD);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS] = Region("Spirit Temple MQ Floormaster Stairs", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_THRONE,  []{return (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && Here(RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS, []{return logic->CanKillEnemy(RE_FLOORMASTER);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_EAST_THRONE] = Region("Spirit Temple MQ East Throne", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_HAND_EXIT,     []{return Here(RR_SPIRIT_TEMPLE_MQ_EAST_THRONE, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE);});}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_EAST_HAND_EXIT] = Region("Spirit Temple MQ East Hand Exit", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_THRONE,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_OUTER_EAST_HAND, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_OUTER_EAST_HAND] = Region("Spirit Temple MQ Outer East Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_OUTER_WEST_HAND, []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_THRONE,     []{return true;}),
        Entrance(RR_DESERT_COLOSSUS,                  []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM] = Region("Spirit Temple MQ 3F Gibdo Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST, true),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_WALL_LOWER] = Region("Spirit Temple MQ Big Wall Lower", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_LONG_CLIMB_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_LONG_CLIMB_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,    []{return true;}),
        //technically we only need to avoid them, but the sheer height and the moving walls makes getting to the top after only stunning them very difficult/impossible
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL_UPPER, []{return /*(*/logic->CanKillEnemy(RE_KEESE)/*|| CanUse(RG_SKULL_MASK)) && CanClimbHigh()*/;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_WALL_UPPER] = Region("Spirit Temple MQ Big Wall Upper", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //Getting some of these with just climbing downwards is theoretically possible but definitly a trick
        EventAccess(&logic->MQSpiritBigWallSilvers, []{return /*(*/logic->CanKillEnemy(RE_KEESE)/*|| CanUse(RG_SKULL_MASK)) && CanClimbHigh()*/;}),
    }, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL_LOWER, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,     []{return logic->MQSpiritBigWallSilvers;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL] = Region("Spirit Temple MQ 4F Central", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BEFORE_MIRROR_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BEFORE_MIRROR_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL_UPPER,    []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_NINE_THRONES_ROOM, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM,   []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_NINE_THRONES_ROOM] = Region("Spirit Temple MQ Nine Thrones Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        //These skulls rely on the iron knuckle existing without a trick to shoot through the chairs
        LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL, []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM] = Region("Spirit Temple MQ Big Mirror Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //Needs the mirror in the cave to be a perm flag and event for doorsanity
        EventAccess(&logic->SpiritPlatformLowered, []{return (logic->SpiritPushed4FMirrors && logic->CanUse(RG_MIRROR_SHIELD)) || logic->SunlightArrows();}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_POT_2,   logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_POT_3,   logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_POT_4,   logic->CanBreakPots()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CRATE_3, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE, []{return Here(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM, []{return logic->CanUse(RG_MEGATON_HAMMER);});}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_PLATFORM,        []{return logic->SpiritPlatformLowered;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE] = Region("Spirit Temple MQ Big Mirror Cave", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //WARNING: Not currently a permanent flag, needs to be one for doorsanity
        EventAccess(&logic->SpiritPushed4FMirrors, []{return ((logic->IsAdult && logic->CanUse(RG_MIRROR_SHIELD)) || logic->SunlightArrows())/* && str0*/;}),
    }, {
        //Locations
        LOCATION(RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
    }, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM, []{return true;}),
    });

    //Assumes SpiritPlatformLowered is checked on entry
    areaTable[RR_SPIRIT_TEMPLE_MQ_PLATFORM] = Region("Spirit Temple MQ Platform", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_HEAD,      []{return logic->SpiritPushed4FMirrors && logic->CanUse(RG_MIRROR_SHIELD) && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_WEST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND,  []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,      []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST, []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_EAST_HAND,  []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_HEAD] = Region("Spirit Temple MQ Statue Head", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
        //Events
        //WARNING these events are not glitchproofed and assume you need all keys to reach from the front
        EventAccess(&logic->ReverseSpiritChild, []{return logic->IsChild;}),
        EventAccess(&logic->ReverseSpiritAdult, []{return logic->IsAdult;}),
    }, {}, {
        // Exits
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,     []{return true;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_WEST_HAND, []{return logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_INNER_EAST_HAND, []{return logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_PLATFORM,        []{return logic->SpiritPlatformLowered && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY,      []{return logic->HasItem(RG_SPIRIT_TEMPLE_BOSS_KEY);}),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY] = Region("Spirit Temple Boss Entryway", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        // Exits
        Entrance(RR_SPIRIT_TEMPLE_STATUE_HEAD,    []{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla() && false;}),
        Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_HEAD, []{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ() && false;}),
        Entrance(RR_SPIRIT_TEMPLE_BOSS_ROOM,      []{return true;}),
    });

    areaTable[RR_SPIRIT_TEMPLE_BOSS_ROOM] = Region("Spirit Temple Boss Room", "Spirit Temple", {}, NO_DAY_NIGHT_CYCLE, {
        // Events
        EventAccess(&logic->SpiritTempleClear, []{return logic->SpiritTempleClear || logic->CanKillEnemy(RE_TWINROVA);}),
    }, {
        // Locations
        LOCATION(RC_SPIRIT_TEMPLE_TWINROVA_HEART, logic->SpiritTempleClear),
        LOCATION(RC_TWINROVA,                     logic->SpiritTempleClear),
    }, {
        // Exits
        Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, []{return false;}),
        Entrance(RR_DESERT_COLOSSUS,             []{return logic->SpiritTempleClear;}, false),
    });

    // clang-format on
}
