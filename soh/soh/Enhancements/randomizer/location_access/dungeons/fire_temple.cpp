#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_FireTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_FIRE_TEMPLE_ENTRYWAY] = Region("Fire Temple Entryway", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FOYER,          ctx->GetDungeon(FIRE_TEMPLE)->IsVanilla()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_LOWER, ctx->GetDungeon(FIRE_TEMPLE)->IsMQ()),
        ENTRANCE(RR_DMC_TEMPLE_EXIT,            true),
    });

#pragma region Vanilla

    areaTable[RR_FIRE_TEMPLE_FOYER] = Region("Fire Temple Foyer", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ENTRYWAY,             true),
        ENTRANCE(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,       true),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM,    AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);}) && (logic->SmallKeys(SCENE_FIRE_TEMPLE, 8) || !logic->IsFireLoopLocked())),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_CAGE_FOYER_SIDE, true),
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,        logic->SmallKeys(SCENE_FIRE_TEMPLE, 2) && logic->FireTimer() >= 24),
    });

    areaTable[RR_FIRE_TEMPLE_NEAR_BOSS_ROOM] = Region("Fire Temple Near Boss Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, logic->FireTimer() >= 16 && logic->HasItem(RG_OPEN_CHEST)),
        //It's plausible to get the pots with rang from the larger square platform, but it's a blind shot that likely needs a setup
        //and I've only been able to get the nearest 2, regardless it's a trick and probably a specific one like GY crate freestanding with rang
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FOYER,           (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS)) && (logic->FireTimer() >= 16 || (logic->Get(LOGIC_FIRE_HIT_PLATFORM) && logic->FireTimer() >= 8))),
        ENTRANCE(RR_FIRE_TEMPLE_NEAR_BOSS_UPPER, logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && logic->FireTimer() >= 16),
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,   logic->FireTimer() >= 16 && (logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->Get(LOGIC_FIRE_HIT_PLATFORM))))),
    });

    //This region assumes tunic logic is handled on entry.
    areaTable[RR_FIRE_TEMPLE_NEAR_BOSS_UPPER] = Region("Fire Temple Near Boss Upper", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits should be handled if there's any way here directly
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM] = Region("Fire Temple Loop Hexagon Room", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FOYER,            logic->SmallKeys(SCENE_FIRE_TEMPLE, 8) || !logic->IsFireLoopLocked()),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_TORCH_SLUG) && logic->CanKillEnemy(RE_FIRE_KEESE);})),
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM] = Region("Fire Temple Loop 5 Tile Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, true),
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_FLARE_DANCER] = Region("Fire Temple Loop Flare Dancer", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);}) && (logic->IsAdult || logic->CanGroundJump() || logic->CanUse(RG_HOOKSHOT)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_CAGE_SWITCH, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_CAGE_SWITCH] = Region("Fire Temple Loop Cage Switch", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_LOOP_SWITCH, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, true),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_GORON_CAGE,   logic->Get(LOGIC_FIRE_LOOP_SWITCH)),
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_GORON_CAGE] = Region("Fire Temple Loop Goron Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BOSS_KEY_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_CAGE_SWITCH,     logic->Get(LOGIC_FIRE_LOOP_SWITCH)),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_CAGE_FOYER_SIDE, logic->Get(LOGIC_FIRE_LOOP_SWITCH)),
    });

    areaTable[RR_FIRE_TEMPLE_LOOP_CAGE_FOYER_SIDE] = Region("Fire Temple Cage Foyer Side", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FOYER,           true),
        ENTRANCE(RR_FIRE_TEMPLE_LOOP_GORON_CAGE, logic->Get(LOGIC_FIRE_LOOP_SWITCH)),
    });

    areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM] = Region("Fire Temple Big Lava Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_POT_1, logic->CanBreakPots() && logic->FireTimer() >= 32),
        LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_POT_2, logic->CanBreakPots() && logic->FireTimer() >= 32),
        LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_POT_3, logic->CanBreakPots() && logic->FireTimer() >= 32),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FOYER,                logic->SmallKeys(SCENE_FIRE_TEMPLE, 2) && logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_1F_CURVED_CAGE,       logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_8_TILE_ROOM,          logic->IsAdult && logic->FireTimer() >= 32 && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_FIRE_SOT))),
        ENTRANCE(RR_FIRE_TEMPLE_STRAIGHTFORWARD_CAGE, (logic->IsAdult && logic->HasExplosives() && logic->FireTimer() >= 32) || (logic->CanGroundJump() && logic->FireTimer() >= 40)),
        // Fewer tunic requirements ends here
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_1F,       logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(SCENE_FIRE_TEMPLE, 3)),
    });

    areaTable[RR_FIRE_TEMPLE_1F_CURVED_CAGE] = Region("Fire Temple 1F Curved Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_8_TILE_ROOM] = Region("Fire Temple 8 Tile Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //it's also possible to use the like like trick to grab this as child, but there's no generic version of that yet
        LOCATION(RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, logic->IsAdult ? ED_CLOSE : ED_BOOMERANG) ||
                                                      logic->CanGroundJumpJumpSlash()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_STRAIGHTFORWARD_CAGE] = Region("Fire Temple Straightforward Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_LAVA_GEYSER_1F] = Region("Fire Temple Lava Geyser 1F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,     logic->SmallKeys(SCENE_FIRE_TEMPLE, 3)),
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_GRATE, logic->FireTimer() >= 40 && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_TORCH, logic->CanUse(RG_LONGSHOT) && logic->FireTimer() >= 40),
    });

    areaTable[RR_FIRE_TEMPLE_LAVA_GEYSER_GRATE] = Region("Fire Temple Lava Geyser Grate", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_FIRE_PILLAR_LEFT_HEART,  true),
        LOCATION(RC_FIRE_TEMPLE_FIRE_PILLAR_RIGHT_HEART, true),
        LOCATION(RC_FIRE_TEMPLE_FIRE_PILLAR_BACK_HEART,  true),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_1F, logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_2F, logic->FireTimer() >= 48 && logic->HasItem(RG_POWER_BRACELET)),
    });

    //you can get the hearts with an indirect boomerang from here, but it's a trick
    areaTable[RR_FIRE_TEMPLE_LAVA_GEYSER_TORCH] = Region("Fire Temple Lava Geyser Torch", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_1F,    true),
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_GRATE, logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_FIRE_TEMPLE_LAVA_GEYSER_2F] = Region("Fire Temple Lava Geyser 2F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_TORCH, logic->TakeDamage() && logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_SHORTCUT_ROOM,     logic->SmallKeys(SCENE_FIRE_TEMPLE, 4)),
    });

    areaTable[RR_FIRE_TEMPLE_SHORTCUT_ROOM] = Region("Fire Temple Shortcut Room", SCENE_FIRE_TEMPLE, {}, {
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_LAVA_GEYSER_1F,     logic->SmallKeys(SCENE_FIRE_TEMPLE, 4)),
        ENTRANCE(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     logic->Get(LOGIC_FIRE_OPENED_UPPER_SHORTCUT)),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, logic->IsAdult && logic->HasItem(RG_CLIMB) && ((logic->HasItem(RG_GORONS_BRACELET) || ctx->GetTrickOption(RT_FIRE_STRENGTH)) || logic->CanGroundJump()) && logic->CanHitSwitch(ED_BOMB_THROW)),
    });

    areaTable[RR_FIRE_TEMPLE_SHORTCUT_CLIMB] = Region("Fire Temple Shortcut Climb", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_OPENED_UPPER_SHORTCUT, true),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_SHORTCUT_ROOM,      true),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Region("Fire Temple Boulder Maze Lower", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_GS_BOULDER_MAZE,          logic->HasExplosives() && (logic->IsAdult || logic->HookshotOrBoomerang() || logic->CanGroundJumpJumpSlash())),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_SHORTCUT_ROOM,      true),
        ENTRANCE(RR_FIRE_TEMPLE_3F_CURVED_CAGE,     true),
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_PATH_ROOM,   logic->SmallKeys(SCENE_FIRE_TEMPLE, 5)),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, false),
    });

    areaTable[RR_FIRE_TEMPLE_3F_CURVED_CAGE] = Region("Fire Temple 3F Curved Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, true),
    });

    areaTable[RR_FIRE_TEMPLE_NARROW_PATH_ROOM] = Region("Fire Temple Narrow Path Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_EAST_CENTRAL_LEFT_HEART,   true),
        LOCATION(RC_FIRE_TEMPLE_EAST_CENTRAL_RIGHT_HEART,  true),
        LOCATION(RC_FIRE_TEMPLE_EAST_CENTRAL_MIDDLE_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,      logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, logic->SmallKeys(SCENE_FIRE_TEMPLE, 5)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    logic->SmallKeys(SCENE_FIRE_TEMPLE, 6)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CAGE,     logic->CanHitEyeTargets()),
    });

    areaTable[RR_FIRE_TEMPLE_FIRE_WALL_CHASE] = Region("Fire Temple Fire Wall Chase", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_FIRE_WALL_EAST_HEART, logic->FireTimer() >= 24 && (logic->IsAdult || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_FIRE_TEMPLE_FIRE_WALL_WEST_HEART, logic->FireTimer() >= 24 && (logic->IsAdult || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_FIRE_TEMPLE_FIRE_WALL_EXIT_HEART, logic->FireTimer() >= 16),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_PATH_ROOM,   logic->FireTimer() >= 24 && logic->SmallKeys(SCENE_FIRE_TEMPLE, 6)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CAGE,     logic->FireTimer() >= 16 && logic->IsAdult),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, logic->FireTimer() >= 24 && logic->IsAdult),
        ENTRANCE(RR_FIRE_TEMPLE_CORRIDOR,           logic->FireTimer() >= 16 && logic->IsAdult && logic->SmallKeys(SCENE_FIRE_TEMPLE, 7)),
    });

    //firetimer for entering this area from RR_FIRE_TEMPLE_FIRE_WALL_CHASE is handled there
    areaTable[RR_FIRE_TEMPLE_FIRE_WALL_CAGE] = Region("Fire Temple Fire Wall Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MAP_CHEST, logic->FireTimer() >= 8 && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_PATH_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,  false),
    });

    areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER] = Region("Fire Temple Boulder Maze Upper", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     logic->HasExplosives()),
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, true),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    true),
        ENTRANCE(RR_FIRE_TEMPLE_GS_CLIMB_4F,        logic->ReachScarecrow() || (ctx->GetTrickOption(RT_FIRE_SCARECROW) && logic->IsAdult && logic->CanUse(RG_LONGSHOT))),
    });

    areaTable[RR_FIRE_TEMPLE_GS_CLIMB_4F] = Region("Fire Temple GS Climb 4F", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, true),
        ENTRANCE(RR_FIRE_TEMPLE_GS_CLIMB_5F,        logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FIRE_TEMPLE_GS_CLIMB_5F] = Region("Fire Temple GS Climb 5F", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, logic->CanKillEnemy(RE_GOLD_SKULLTULA, logic->HasItem(RG_CLIMB) ? ED_SHORT_JUMPSLASH : ED_BOMB_THROW)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_GS_CLIMB_4F,    true),
        ENTRANCE(RR_FIRE_TEMPLE_5F_RUINS,       true),
    });

    //RANDOTODO find a better name
    areaTable[RR_FIRE_TEMPLE_5F_RUINS] = Region("Fire Temple 5F Ruins", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_SCARECROW_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_GS_SCARECROW_TOP, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_GS_CLIMB_4F,      true),
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_PATH_ROOM, logic->TakeDamage()),
    });

    areaTable[RR_FIRE_TEMPLE_CORRIDOR] = Region("Fire Temple Corridor", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_WALL_CHASE, logic->SmallKeys(SCENE_FIRE_TEMPLE, 7)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,  true),
    });

    areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_MAIN] = Region("Fire Temple Fire Maze Main", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_LEFT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_LEFT_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_LEFT_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_LEFT_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        //Accounting for either air-drifting to the platform you want and taking fall damage or landing on the platform and jumping off
        ENTRANCE(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,      logic->Get(LOGIC_FIRE_HIT_PLATFORM) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->TakeDamage())),
        ENTRANCE(RR_FIRE_TEMPLE_CORRIDOR,            true),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS, logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && (logic->Get(LOGIC_FIRE_HIT_ABOVE_MAZE_PLATFORM) || logic->CanGroundJump()))),
        ENTRANCE(RR_FIRE_TEMPLE_CAGELESS_CHEST_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_LOWER,      logic->SmallKeys(SCENE_FIRE_TEMPLE, 8)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_SWITCH,    (bool)ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
    });

    areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS] = Region("Fire Temple Fire Maze Platforms", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_HIT_PLATFORM,    logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,      true),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR, logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_FIRE_TEMPLE_CAGELESS_CHEST_ROOM] = Region("Fire Temple Cageless Chest Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_COMPASS_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN, true),
    });

    areaTable[RR_FIRE_TEMPLE_SOT_CAGE_LOWER] = Region("Fire Temple Sot Cage Lower", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,      logic->SmallKeys(SCENE_FIRE_TEMPLE, 8)),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR, logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_SWITCH,     logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_SWITCH,    true),
    });

    areaTable[RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR] = Region("Fire Temple Sot Cage Upper Door", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,       false),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_SWITCH,     logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS, true),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_LOWER,      true),
    });

    areaTable[RR_FIRE_TEMPLE_SOT_CAGE_SWITCH] = Region("Fire Temple Sot Cage Switch", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_VISIBLE_COLLISION)) && logic->CanUse(RG_MEGATON_HAMMER) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,       false),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR, logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_LOWER,      true),
    });

    areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_SWITCH] = Region("Fire Temple Fire Maze Switch", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,      (ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS) && logic->TakeDamage()) ||
                                                     (logic->IsAdult && logic->CanGroundJump() && ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS)))),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_LOWER,      true),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_PAST_WALL, true),
    });

    areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_PAST_WALL] = Region("Fire Temple Fire Maze Past Wall", SCENE_FIRE_TEMPLE, {}, {
        // Locations
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_RIGHT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_RIGHT_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_RIGHT_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_FLAME_MAZE_RIGHT_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_3F_FLARE_DANCER, logic->HasExplosives()),
    });

    areaTable[RR_FIRE_TEMPLE_3F_FLARE_DANCER] = Region("Fire Temple 3F Flare Dancer", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_PAST_WALL,   AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
        ENTRANCE(RR_FIRE_TEMPLE_ABOVE_3F_FLARE_DANCER, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
    });

    areaTable[RR_FIRE_TEMPLE_ABOVE_3F_FLARE_DANCER] = Region("Fire Temple Above 3F Flare Dancer", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_3F_FLARE_DANCER, true),
        ENTRANCE(RR_FIRE_TEMPLE_SWITCH_CLIMB,    true),
    });

    areaTable[RR_FIRE_TEMPLE_SWITCH_CLIMB] = Region("Fire Temple Switch Climb", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ABOVE_3F_FLARE_DANCER, true),
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_STAIRS,         logic->CanHitSwitch(ED_BOMB_THROW) && logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FIRE_TEMPLE_NARROW_STAIRS] = Region("Fire Temple Narrow Stairs", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR, logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_SOT_CAGE_SWITCH,     logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_SWITCH_CLIMB,        true),
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_STAIRS_4F,    AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);})),
    });

    areaTable[RR_FIRE_TEMPLE_NARROW_STAIRS_4F] = Region("Fire Temple Narrow Stairs 4F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_TOP_OF_COLLAPSING_STAIRS, AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);})),
        //this return path is blocked by a hammer peg that is a perm flag, if a way up is ever added, LOGIC_FIRE_MQ_HIT_SCARECROW_ROOM_PLATFORM should be reworked
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_STAIRS,            false),
    });

    areaTable[RR_FIRE_TEMPLE_TOP_OF_COLLAPSING_STAIRS] = Region("Fire Temple Top of Collapsing Stairs", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_HIT_STAIRS,             logic->CanUse(RG_MEGATON_HAMMER)),
        EVENT_ACCESS(LOGIC_FIRE_CHILD_AT_TOP_OF_STAIRS, logic->IsChild),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_AFTER_HAMMER_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_FIRE_TEMPLE_AFTER_HAMMER_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_NARROW_STAIRS_4F,          true),
        ENTRANCE(RR_FIRE_TEMPLE_BASE_OF_COLLAPSING_STAIRS, logic->Get(LOGIC_FIRE_HIT_STAIRS)),
    });

    areaTable[RR_FIRE_TEMPLE_BASE_OF_COLLAPSING_STAIRS] = Region("Fire Temple Base of Collapsing Stairs", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_TOP_OF_COLLAPSING_STAIRS, logic->Get(LOGIC_FIRE_HIT_STAIRS) && logic->IsAdult),
        // this is here to maintain 1:1 door entrances between regions
        ENTRANCE(RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE,          logic->Get(LOGIC_FIRE_HIT_STAIRS) && (logic->IsAdult || logic->Get(LOGIC_FIRE_CHILD_AT_TOP_OF_STAIRS)) && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Region("Fire Temple Above Fire Maze", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_HIT_ABOVE_MAZE_PLATFORM, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_BASE_OF_COLLAPSING_STAIRS, true),
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,            logic->Get(LOGIC_FIRE_HIT_ABOVE_MAZE_PLATFORM)),
        //it's possible to land directly on the upper platform as child and even avoid fall damage, but it's not intuitive (you have to ledge grab, drop down and then air drift with enough momentum to roll)
        ENTRANCE(RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS,       logic->Get(LOGIC_FIRE_HIT_ABOVE_MAZE_PLATFORM) && logic->CanJumpslash() && logic->TakeDamage()),
    });

#pragma endregion

#pragma region MQ

    //potentially dangerous temp flag on the first room's torches, should be made permanent if possible
    areaTable[RR_FIRE_TEMPLE_MQ_FOYER_LOWER] = Region("Fire Temple MQ Foyer Lower", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_ENTRANCE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_ENTRANCE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ENTRYWAY,                true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_CAGE_FOYER_SIDE, true),
        //child can easilly pass the flame wall with a well timed sidehop, but that's a generic version of RT_FIRE_FLAME_MAZE
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_UPPER,          logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM,    logic->SmallKeys(SCENE_FIRE_TEMPLE, 5)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FOYER_UPPER] = Region("Fire Temple MQ Foyer Upper", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_LOWER,    true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM, logic->HasFireSource()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,  AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);})),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_CAGE_FOYER_SIDE] = Region("Fire Temple MQ Loop Cage Foyer Side", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, logic->CanKillEnemy(RE_LIKE_LIKE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_LOWER,     AnyAgeTime([]{return logic->CanKillEnemy(RE_LIKE_LIKE);})),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_GORON_CAGE, logic->Get(LOGIC_FIRE_OPENED_LOWEST_GORON_CAGE)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM] = Region("Fire Temple MQ Loop Hexagon Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LOOP_STALFOS_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_LOWER,      true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM] = Region("Fire Temple MQ Loop 5 Tile Room", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LOOP_KNUCKLE_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_4, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_5, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_6, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_7, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_BEFORE_MINI_BOSS_POT_8, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER, AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE);})),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER] = Region("Fire Temple MQ Loop Flare Dancer", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, (logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || logic->CanGroundJump()) && AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);}) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_CAGE_SWITCH, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_CAGE_SWITCH] = Region("Fire Temple MQ Loop Cage Switch", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_OPENED_LOWEST_GORON_CAGE, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_GORON_CAGE,   logic->Get(LOGIC_FIRE_OPENED_LOWEST_GORON_CAGE)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOOP_GORON_CAGE] = Region("Fire Temple MQ Loop Goron Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_MAP_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_CAGE_SWITCH,     logic->Get(LOGIC_FIRE_OPENED_LOWEST_GORON_CAGE)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOOP_CAGE_FOYER_SIDE, logic->Get(LOGIC_FIRE_OPENED_LOWEST_GORON_CAGE)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM] = Region("Fire Temple MQ Near Boss Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //If we're using the south torch as the initial torch, or using FAs, we either have to cross to the north to remove the crate, or use a trick to ignore it
        LOCATION(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,      logic->FireTimer() >= 24 && ctx->GetTrickOption(RT_FIRE_MQ_NEAR_BOSS) && (logic->CanUse(RG_FIRE_ARROWS) || (logic->IsAdult && logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_FAIRY_BOW))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_1, logic->FireTimer() >= 24 && logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_2, logic->FireTimer() >= 24 && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_UPPER,      (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS)) && (logic->FireTimer() >= 16 || (logic->Get(LOGIC_FIRE_HIT_PLATFORM) && logic->FireTimer() >= 8))),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_TARGET, logic->FireTimer() >= 32 && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,       logic->FireTimer() >= 16 && (logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) || logic->Get(LOGIC_FIRE_HIT_PLATFORM))))),
    });

    //This region assumes tunic logic is handled on entry.
    areaTable[RR_FIRE_TEMPLE_MQ_NEAR_BOSS_TARGET] = Region("Fire Temple MQ Near Boss Target", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //With Dins, we first light them with dins and then either use a bow shot or to cross back over to light the other torch.
        //Only adult is tall enough to use a bow shot without climbing to UPPER
        //Logical ways to cross are hovers, longshot or running through the lava and then climbing back up as adult (child can't reach the ledge).
        //The Damage logic here is for jumping down and running across the lava to get in dins range of the south torch
        //Fairies cannot be used for this as it is time sensetive, and NL is only useful with sticks as it disables other magic while in use, so it's tunic or raw damage taking ability.
        //testing tells me you take 3 ticks of lava damage, which is 12 internal damage or 3/4 of a heart at x1 damage multiplier, performing this run
        //logic->EffectiveHealth() works in half hearts for whatever reason, meaning this needs a deeper refactor to be perfect, but it should be good enough for now
        LOCATION(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,      logic->CanUse(RG_DINS_FIRE) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_GORON_TUNIC) || logic->EffectiveHealth() >= 2 || (logic->CanUse(RG_NAYRUS_LOVE) && logic->CanUse(RG_STICKS))))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_POT_2,   logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_5, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_6, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM,       true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM_UPPER, logic->IsAdult || logic->CanUse(RG_HOOKSHOT)),
    });

    //This region assumes tunic logic is handled on entry.
    areaTable[RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM_UPPER] = Region("Fire Temple MQ Near Boss Room Upper", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //If we have FAs, we can just remove the crate and use those to light the torches.
        //otherwise, with Dins, we first light them with dins and then use a bow shot 
        LOCATION(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,      (logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_FAIRY_BOW))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_OUTSIDE_BOSS_CRATE_4, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_TARGET, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Region("Fire Temple MQ Big Lava Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //I'm currently assuming the oversight version of RT_FIRE_MQ_BK_CHEST for the fire timer logic
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_ROOM_NORTH_POT, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_ROOM_HIGH_POT,  logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_ROOM_SOUTH_POT, logic->FireTimer() >= 40 && (logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_FIRE_MQ_BLOCKED_CHEST)) && logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FOYER_UPPER,           logic->FireTimer() >= 32),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_GS_GORON_CAGE,         logic->FireTimer() >= 32),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_BLOCKED_DOOR, logic->FireTimer() >= 48 && (logic->CanUse(RG_HOOKSHOT) || ((logic->IsAdult || logic->CanGroundJump()) && ctx->GetTrickOption(RT_FIRE_MQ_BLOCKED_CHEST)))),
        // Fewer tunic requirements ends here
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F,        logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(SCENE_FIRE_TEMPLE, 2)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TORCH_FIREWALL_ROOM,   logic->HasFireSource() && ((logic->CanUse(RG_FAIRY_BOW) && logic->FireTimer() >= 32) || (ctx->GetTrickOption(RT_FIRE_MQ_BK_CHEST) && logic->FireTimer() >= 56)) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && ctx->GetTrickOption(RT_FIRE_SOT)))),
    });

    //Tunic timers from other doors are handled on entry from RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM
    //Specifically the tiled platform, not the ground underneath
    areaTable[RR_FIRE_TEMPLE_MQ_BIG_LAVA_BLOCKED_DOOR] = Region("Fire Temple MQ Big Lava Blocked Door", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_ROOM_SOUTH_POT, logic->CanBreakPots() && logic->FireTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TORCH_LOCKED_CAGE, logic->HasExplosives()),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_GS_GORON_CAGE] = Region("Fire Temple MQ GS Goron Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //implies CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_TORCH_LOCKED_CAGE] = Region("Fire Temple MQ Torch Locked Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, logic->HasFireSource() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_TORCH_FIREWALL_ROOM] = Region("Fire Temple MQ Torch Firewall Room", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanUse(RG_HOOKSHOT)),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,   logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_POT_1, logic->HookshotOrBoomerang()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_POT_2, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F] = Region("Fire Temple MQ Lava Geyser 1F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,       true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_GRATE,   logic->FireTimer() >= 40 && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT))),
        //this technically only reaches torch pillar, but the heart pillar can be reached from there with longshot
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_PILLARS, logic->FireTimer() >= 40 && logic->CanUse(RG_LONGSHOT)),
    });
    
    //tunic logic handled on entry
    areaTable[RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_GRATE] = Region("Fire Temple MQ Lava Geyser Grate", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_PILLAR_LEFT_HEART,  true),
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_PILLAR_RIGHT_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F,      true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_PILLARS, true),
    });

    //tunic logic handled on entry
    areaTable[RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_PILLARS] = Region("Fire Temple MQ Lava Geyser Pillars", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //rang can get the other 2 hearts from here but it's an awkward shot so a trick
        //similarly rang from 1f can get this lower heart
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_PILLAR_LOWER_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F,    true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_GRATE, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_2F,    true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_2F] = Region("Fire Temple MQ Lava Geyser 2F", SCENE_FIRE_TEMPLE, {}, {
    }, {
        //Exits
        //the block lift does some janky stuff that makes leave sideways while on it difficult, so you can't avoid fall damage if you want to land on pillars.
        //you can still move "backwards" into the door though and with a roll jump onto the grate, which avoids damage
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F,      logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_PILLARS, logic->TakeDamage() && logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER] = Region("Fire Temple MQ Shortcut Room Lower", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_MID, (logic->HasFireSource() && (logic->IsAdult || (logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_CLIMB)))) || 
                                                       (ctx->GetTrickOption(RT_FIRE_MQ_CLIMB) && logic->CanUse(RG_HOVER_BOOTS) && logic->HasItem(RG_CLIMB))),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_2F,    true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_CAGE,     logic->Get(LOGIC_FIRE_OPENED_UPPER_SHORTCUT)),
    });

    //specifically the foot of the final grate, where the lizalfos spawns
    areaTable[RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_MID] = Region("Fire Temple MQ Shortcut Room Middle", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_3F,    logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_3F] = Region("Fire Temple MQ Shortcut Room 3F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_MID,   true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE] = Region("Fire Temple MQ Lower Lizalfos Maze", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER, true),
        //Explosives can also reach this room. Chus is relatively simple, they need to detonate on the first horizontal bar up from the floor while horizontally near the switch, but bombs are much harder
        ENTRANCE(RR_FIRE_TEMPLE_MQ_MAZE_CRATE_CAGE,     AnyAgeTime([]{return logic->CanJumpslash();})),
        //it's possible to make the RT_FIRE_MQ_MAZE_HOVERS as child using bunny hood jumps, but not adult as adult bonks
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, (logic->HasExplosives() || ctx->GetTrickOption(RT_VISIBLE_COLLISION)) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR,    logic->HasExplosives() && ctx->GetTrickOption(RT_FIRE_MQ_MAZE_SIDE_ROOM)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,    false),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR] = Region("Fire Temple MQ Maze Switch Door", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        //This exists to join upper/lower access in a way compatible with door shuffle
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_3F_CURVED_CAGE,      true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_3F_CURVED_CAGE] = Region("Fire Temple MQ 3F Curved Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_MAZE_CRATE_CAGE] = Region("Fire Temple MQ Maze Crate Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,   logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CRATE_3, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, true),
        //it's possible to make the RT_FIRE_MQ_MAZE_HOVERS as child using bunny hood jumps, but not adult as adult bonks
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, logic->IsAdult && ((ctx->GetTrickOption(RT_FIRE_MQ_MAZE_HOVERS) && logic->CanUse(RG_HOVER_BOOTS)) || ctx->GetTrickOption(RT_FIRE_MQ_MAZE_JUMP))),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE] = Region("Fire Temple MQ Upper Lizalfos Maze", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, true),
        //this cage is much more lenient than the lower cage as the switch is close to the front. sling, rang and bow all hit the switch easily, though might be too unintuitive for default logic
        //This shouldn't come up in most cases anyway as most methods to get here need either a melee weapon or explosives
        ENTRANCE(RR_FIRE_TEMPLE_MQ_MAZE_BOX_CAGE,       AnyAgeTime([]{return logic->CanJumpslash() || logic->HasExplosives();})),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_CLIMB,      logic->HasExplosives()),
        //Implies RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE access
        ENTRANCE(RR_FIRE_TEMPLE_MQ_ABOVE_MAZE,          logic->HasExplosives() && logic->CanUse(RG_MEGATON_HAMMER) && (logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SONG_OF_TIME)))),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,     logic->SmallKeys(SCENE_FIRE_TEMPLE, 3) && logic->CanUse(RG_GORON_TUNIC)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_MAZE_BOX_CAGE] = Region("Fire Temple MQ Maze Box Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST,         logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CRATE_1,       logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CRATE_2,       logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CRATE_3,       logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, true),
        //Assumes maze access
        ENTRANCE(RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR,    logic->HasExplosives() && logic->HasItem(RG_POWER_BRACELET)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_ABOVE_MAZE] = Region("Fire Temple MQ Above Maze", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB,    true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_SHORTCUT_CLIMB] = Region("Fire Temple MQ Shortcut Climb", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_OPENED_UPPER_SHORTCUT, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_5, logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SHORTCUT_CRATE_6, logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_CAGE,       logic->Get(LOGIC_FIRE_OPENED_UPPER_SHORTCUT)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_SHORTCUT_CAGE] = Region("Fire Temple MQ Shortcut Cage", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_COMPASS_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_CLIMB,      logic->Get(LOGIC_FIRE_OPENED_UPPER_SHORTCUT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER, logic->Get(LOGIC_FIRE_OPENED_UPPER_SHORTCUT)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB] = Region("Fire Temple MQ Torch Slug Climb", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanUse(RG_HOOKSHOT)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_ABOVE_MAZE,    true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BURNING_BLOCK, logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_BURNING_BLOCK] = Region("Fire Temple MQ Burning Block", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        //There's definitely ways to do this hammerless, but with one points on it's a trick
        //you can just hook through the block while next to it to kill the skull and get the token
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE, logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_HOOKSHOT) && (logic->HasItem(RG_POWER_BRACELET) || ctx->GetTrickOption(RT_VISIBLE_COLLISION))),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM, logic->TakeDamage()),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM] = Region("Fire Temple MQ Narrow Path Room", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_ABOVE_LAVA_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_ABOVE_LAVA_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_ABOVE_LAVA_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE,  true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,      false),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_CAGE, false),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,        logic->TakeDamage()),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM] = Region("Fire Temple MQ High Torch Room", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_MQ_HIGH_TORCH_LIT, (logic->CanUse(RG_FIRE_ARROWS) && logic->FireTimer() >= 24)),
    }, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_FLAME_WALL_POT_1,         logic->CanBreakPots() && logic->FireTimer() >= 24),
        LOCATION(RC_FIRE_TEMPLE_MQ_FLAME_WALL_POT_2,         logic->CanBreakPots() && logic->FireTimer() >= 24),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_CRATE_1,       logic->CanBreakCrates() && logic->FireTimer() >= 24),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_CRATE_3,       logic->CanBreakCrates() && logic->FireTimer() >= 24),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_CRATE_4,       logic->CanBreakCrates() && logic->FireTimer() >= 16),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_SMALL_CRATE_2, logic->CanBreakSmallCrates() && logic->FireTimer() >= 24),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_SMALL_CRATE_3, logic->CanBreakSmallCrates() && logic->FireTimer() >= 16),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_ABOVE_CAGE, (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)) && logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE,        logic->SmallKeys(SCENE_FIRE_TEMPLE, 3) && logic->FireTimer() >= 24),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,           logic->FireTimer() >= 24),
        //Child has issues navigating the higher points of this room without an equip swapped hookshot
        ENTRANCE(RR_FIRE_TEMPLE_MQ_CORRIDOR,                   logic->Get(LOGIC_FIRE_MQ_HIGH_TORCH_LIT) && (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)) && logic->FireTimer() >= 16),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_BARRED_DOOR] = Region("Fire Temple MQ High Torch Barred Door", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_CRATE_2,       logic->CanBreakCrates() && logic->FireTimer() >= 8),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_SMALL_CRATE_1, logic->CanBreakSmallCrates() && logic->FireTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_CORRIDOR,        logic->Get(LOGIC_FIRE_MQ_HIGH_TORCH_LIT)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_ABOVE_CAGE] = Region("Fire Temple MQ High Torch Room Above Cage", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_MQ_HIGH_TORCH_LIT, ((logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_POWER_BRACELET)) && logic->FireTimer() >= 48)),
    }, {
        //Locations
        //Tunic logic for these checks is handled on entry
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_CRATE_5,       logic->CanBreakCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_SMALL_CRATE_4, logic->CanBreakSmallCrates()),
        LOCATION(RC_FIRE_TEMPLE_MQ_LAVA_TORCH_SMALL_CRATE_5, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,      true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_CAGE, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_CAGE] = Region("Fire Temple MQ High Torch Room Cage", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_ABOVE_CAGE,  logic->CanUse(RG_HOOKSHOT) && logic->FireTimer() >= 8),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,            true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_CORRIDOR] = Region("Fire Temple Corridor", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_BARRED_DOOR, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,              true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN] = Region("Fire Temple MQ Fire Maze Main", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER, logic->HasExplosives()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SOUTH_FIRE_MAZE_WEST_POT, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_SOUTH_FIRE_MAZE_EAST_POT, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM,      logic->Get(LOGIC_FIRE_HIT_PLATFORM)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_CORRIDOR,            true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS, logic->IsAdult || logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_HOVER_BOOTS)),
        //Hover boots get there via the platforms
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE,    (bool)ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,  true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS] = Region("Fire Temple MQ Fire Maze Platforms", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_HIT_PLATFORM, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,          true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE,        logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR, true),
        //This one might be a bit too hard for base logic, but is only relevent in doorsanity or with RT_FIRE_MQ_MAZE_HOVERS
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH,        logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOVER_BOOTS) && (logic->TakeDamage() || logic->CanJumpslash())),
    });

   areaTable[RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR] = Region("Fire Temple MQ 2 Fire Walls Upper Door", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_SWITCH, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,  false),
    });

   areaTable[RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_SWITCH] = Region("Fire Temple MQ 2 Fire Walls Switch", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_MQ_OPENED_FIRE_MAZE_DOOR, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,      false),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER] = Region("Fire Temple MQ 2 Fire Walls Lower", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER, logic->HasExplosives() && (logic->IsAdult || logic->CanGroundJump())),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,   true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH, logic->Get(LOGIC_FIRE_MQ_OPENED_FIRE_MAZE_DOOR)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE] = Region("Fire Temple MQ Fire Maze Middle", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_PAST_FIRE_MAZE_SOUTH_POT, logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_MAZE_NORTHMOST_POT,  logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_MAZE_NORTHWEST_POT,  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_GS_LIZALFOS_ROOM, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,   logic->IsAdult || ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH, (bool)ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH] = Region("Fire Temple MQ Fire Maze Switch", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PAST_WALL, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE,    (bool)ctx->GetTrickOption(RT_FIRE_SKIP_FLAME_WALLS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,  true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_GS_LIZALFOS_ROOM] = Region("Fire Temple MQ GS Lizalfos Room", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PAST_WALL] = Region("Fire Temple MQ Fire Maze Past Wall", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_PAST_FIRE_MAZE_SOUTH_POT, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_PAST_FIRE_MAZE_NORTH_POT, logic->CanBreakPots()),
        LOCATION(RC_FIRE_TEMPLE_MQ_FIRE_MAZE_NORTHWEST_POT,  logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER, true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER] = Region("Fire Temple MQ 3F Flare Dancer", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY, logic->CanKillEnemy(RE_FLARE_DANCER)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PAST_WALL,   AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_ABOVE_3F_FLARE_DANCER, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLARE_DANCER);})),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_ABOVE_3F_FLARE_DANCER] = Region("Fire Temple MQ Above 3F Flare Dancer", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER, true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB,    true),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB] = Region("Fire Temple MQ Locked Climb", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER,    true),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_ROOM, logic->SmallKeys(SCENE_FIRE_TEMPLE, 4) && logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_ROOM] = Region("Fire Temple MQ Narrow Stairs Room", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_MQ_HIT_SCARECROW_ROOM_PLATFORM, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        //This requires nothing in N64 logic, but is tight enough to need rollspam with the one-point on which is stricter than I would normally consider in logic
        //Child basically needs the scarecrow or a bunny hood though due to a worse ledge grab.
        LOCATION(RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE, (logic->IsAdult || logic->ReachScarecrow()) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        //The dropdown here is unusual in that it hits 1 of 3 locations: RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER, RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR, and RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_SWITCH
        //Using this dropdown is in N64 logic elsewhere, but not here, probably because it requires good foreknowlege to determine where to land
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,      logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR, logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_SWITCH,     logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER,         logic->SmallKeys(SCENE_FIRE_TEMPLE, 4)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_4F,        logic->Get(LOGIC_FIRE_MQ_HIT_SCARECROW_ROOM_PLATFORM)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_4F] = Region("Fire Temple MQ Narrow Stairs 4F", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_ROOM,       logic->Get(LOGIC_FIRE_MQ_HIT_SCARECROW_ROOM_PLATFORM) && logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TOP_OF_COLLAPSING_STAIRS, logic->SmallKeys(SCENE_FIRE_TEMPLE, 5)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_TOP_OF_COLLAPSING_STAIRS] = Region("Fire Temple MQ Top of Collapsing Stairs", SCENE_FIRE_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_HIT_STAIRS, logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BASE_OF_COLLAPSING_STAIRS, logic->Get(LOGIC_FIRE_HIT_STAIRS)),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_4F,          logic->SmallKeys(SCENE_FIRE_TEMPLE, 5)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_BASE_OF_COLLAPSING_STAIRS] = Region("Fire Temple MQ Base of Collapsing Stairs", SCENE_FIRE_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_TOP_OF_COLLAPSING_STAIRS, logic->Get(LOGIC_FIRE_HIT_STAIRS) && logic->IsAdult),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_ABOVE_FIRE_MAZE,          logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_FIRE_TEMPLE_MQ_ABOVE_FIRE_MAZE] = Region("Fire Temple MQ Above Fire Maze", SCENE_FIRE_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_MAZE, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,            AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);})),
        //it's possible to land directly on the upper platform as child and even avoid fall damage, but it's not intuitive (you have to ledge grab, drop down and then air drift with enough momentum to roll)
        ENTRANCE(RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS,       AnyAgeTime([]{return logic->CanUse(RG_MEGATON_HAMMER);}) && logic->CanJumpslash() && logic->TakeDamage()),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_BASE_OF_COLLAPSING_STAIRS, false),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_FIRE_TEMPLE_BOSS_ENTRYWAY] = Region("Fire Temple Boss Entryway", SCENE_FIRE_TEMPLE, {}, {}, {
        // Exits
        ENTRANCE(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,    ctx->GetDungeon(FIRE_TEMPLE)->IsVanilla() && false),
        ENTRANCE(RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM, ctx->GetDungeon(FIRE_TEMPLE)->IsMQ() && false),
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ROOM,         logic->HasItem(RG_FIRE_TEMPLE_BOSS_KEY)),
    });

    areaTable[RR_FIRE_TEMPLE_BOSS_ROOM] = Region("Fire Temple Boss Room", SCENE_FIRE_TEMPLE_BOSS, {
        // Events
        EVENT_ACCESS(LOGIC_FIRE_TEMPLE_CLEAR, logic->FireTimer() >= 64 && logic->CanKillEnemy(RE_VOLVAGIA)),
    }, {
        // Locations
        LOCATION(RC_FIRE_TEMPLE_VOLVAGIA_HEART, logic->Get(LOGIC_FIRE_TEMPLE_CLEAR)),
        LOCATION(RC_VOLVAGIA,                   logic->Get(LOGIC_FIRE_TEMPLE_CLEAR)),
    }, {
        // Exits
        ENTRANCE(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, false),
        ENTRANCE(RR_DMC_PAD_ENTRY,             logic->Get(LOGIC_FIRE_TEMPLE_CLEAR), false),
    });

    // clang-format on
}
