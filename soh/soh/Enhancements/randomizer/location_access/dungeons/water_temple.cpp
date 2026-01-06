#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_WaterTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_WATER_TEMPLE_ENTRYWAY] = Region("Water Temple Entryway", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_ENTRANCE_LEDGE,    []{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsVanilla();}),
        Entrance(RR_WATER_TEMPLE_MQ_ENTRANCE_LEDGE, []{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsMQ();}),
        Entrance(RR_LH_FROM_WATER_TEMPLE,           []{return true;}),
    });

#pragma region Vanilla

    //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
    areaTable[RR_WATER_TEMPLE_ENTRANCE_LEDGE] = Region("Water Temple Entrance Ledge", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_MIDDLE, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_ENTRYWAY,      []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A,  []{return (logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,  []{return logic->WaterLevel(WL_HIGH) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_LM, []{return logic->CanUse(RG_HOVER_BOOTS) && logic->WaterLevel(WL_LOW_OR_MID);}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM, []{return logic->WaterLevel(WL_LOW_OR_MID);}),
    });

    //This region covers simply existing in the area around the central pillar without being on a specific platform, either swimming or walking on the lakebed
    //Entry should only include being in the correct area, taking any possible fall damage, and floating up to the surface of WL_HIGH if coming from below
    //This area then leads to others based on level and worst-case water timers for follow-up exits from the water's surface
    //remember that any solution that works for any level doesn't need to be given a level, even if that solution is overkill for a lower level
    //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
    areaTable[RR_WATER_TEMPLE_MAIN] = Region("Water Temple Main", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_PUSHED_1F_BLOCK, []{return logic->WaterLevel(WL_LOW) && logic->HasItem(RG_GORONS_BRACELET);}),
        EventAccess(LOGIC_WATER_COULD_MIDDLE,    []{return (logic->CanUse(RG_LONGSHOT) && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW))) ||
                                                           (logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(SCENE_WATER_TEMPLE, 5));}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_ENTRANCE_LEDGE, []{return logic->HasItem(RG_BRONZE_SCALE) && logic->WaterLevel(WL_HIGH);}),
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_1F,  []{return (logic->WaterTimer() >= 24 && logic->CanUse(RG_IRON_BOOTS)) ||
                                                           (logic->WaterLevel(WL_MID) && logic->HasItem(RG_GOLDEN_SCALE) && logic->WaterTimer() >= 16) ||
                                                           logic->WaterLevel(WL_LOW) ||
                                                           (logic->CanUse(RG_LONGSHOT) && ctx->GetTrickOption(RT_WATER_LONGSHOT_TORCH) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A,   []{return logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,   []{return logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_H,   []{return logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_IRON_BOOTS) && ((logic->WaterTimer() >= 8 && logic->HasItem(RG_BRONZE_SCALE)) || (logic->WaterTimer() >= 40 && logic->CanUse(RG_LONGSHOT)));}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM,  []{return logic->WaterLevel(WL_LOW_OR_MID) && ((logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                           ((logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40)) && logic->CanUse(RG_LONGSHOT)));}),
        Entrance(RR_WATER_TEMPLE_PILLAR_1F,      []{return logic->WaterLevel(WL_LOW) && logic->SmallKeys(SCENE_WATER_TEMPLE, 5);}),
        Entrance(RR_WATER_TEMPLE_SPIKE_MOAT,     []{return ((logic->WaterLevel(WL_LOW) && logic->HasItem(RG_BRONZE_SCALE)) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && (logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_BRONZE_SCALE))));}),
        Entrance(RR_WATER_TEMPLE_BLOCK_U_BEND,   []{return logic->Get(LOGIC_WATER_PUSHED_1F_BLOCK) &&
                                                          ((logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 16) ||
                                                            (logic->WaterLevel(WL_LOW) && logic->HasItem(RG_SILVER_SCALE)));}),
        Entrance(RR_WATER_TEMPLE_NEAR_CAGE,      []{return AnyAgeTime([]{return logic->WaterLevel(WL_LOW) && logic->HasExplosives();}) &&
                                                           (logic->WaterLevel(WL_LOW) && logic->HasItem(RG_SILVER_SCALE) ||
                                                            (logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 16));}),
    });

    areaTable[RR_WATER_TEMPLE_3F_CENTRAL_A] = Region("Water Temple 3F Central Any Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_1, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots(ED_BOOMERANG, false, true) && logic->HasItem(RG_GOLDEN_SCALE))),
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_2, logic->CanUse(RG_BOOMERANG) || (logic->CanBreakPots(ED_BOOMERANG, false, true) && logic->HasItem(RG_GOLDEN_SCALE))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,                []{return true;}),
        Entrance(RR_WATER_TEMPLE_ENTRANCE_LEDGE,      []{return logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_HIGH_EMBLEM,         []{return logic->Water3FCentralToHighEmblem();}),
        Entrance(RR_WATER_TEMPLE_JET_CHEST_ROOM,      []{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}),
        Entrance(RR_WATER_TEMPLE_RISING_TARGET_LEDGE, []{return ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()));}),
    });

    //assumes checking for WL_HIGH on entry
    areaTable[RR_WATER_TEMPLE_3F_CENTRAL_H] = Region("Water Temple 3F Central High Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A,        []{return true;}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_H,        []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_OUTSIDE_WATERFALL,   []{return logic->IsAdult ||logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_3F_H,     []{return logic->CanUse(RG_HOVER_BOOTS) || logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_RISING_TARGET_LEDGE, []{return logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_IRON_BOOTS_LEDGE_GRAB) && logic->IsAdult && logic->HasItem(RG_BRONZE_SCALE) && logic->CanUse(RG_IRON_BOOTS));}),
    });

    //assumes checking for WL_LOW_OR_MID on entry
    areaTable[RR_WATER_TEMPLE_3F_CENTRAL_LM] = Region("Water Temple 3F Central Low Or Mid Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A,        []{return true;}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM,       []{return true;}),
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP,          []{return ctx->GetTrickOption(RT_WATER_CENTRAL_BOW) && logic->CanHitEyeTargets();}),
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_3F_LM,    []{return logic->CanUse(RG_HOVER_BOOTS);}),
        //assumes RR_WATER_TEMPLE_HIGH_EMBLEM and RR_WATER_TEMPLE_3F_CENTRAL_LM access
        Entrance(RR_WATER_TEMPLE_PILLAR_H,            []{return ctx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) && logic->CanUse(RG_DINS_FIRE) && logic->Water3FCentralToHighEmblem();}),
        });

    //Assumes checking for iron boots and WL_HIGH on entry
    areaTable[RR_WATER_TEMPLE_2F_CENTRAL_H] = Region("Water Temple 2F Central High Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_1, logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_2, logic->CanUse(RG_HOOKSHOT)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,           []{return true;}),
        //Implies CanAvoid(RE_STINGERS)
        Entrance(RR_WATER_TEMPLE_JET_CHEST_ROOM, []{return (logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_WATER_IRON_BOOTS_LEDGE_GRAB) && logic->HasItem(RG_BRONZE_SCALE) && logic->TakeDamage())) && logic->WaterTimer() >= 48;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,   []{return logic->CanUse(RG_LONGSHOT) && logic->WaterTimer() >= 40;}),
    });

    //assumes checking for WL_LOW_OR_MID on entry
    areaTable[RR_WATER_TEMPLE_2F_CENTRAL_LM] = Region("Water Temple 2F Central Low Or Mid Water", SCENE_WATER_TEMPLE, {
        //Events
        //Assumes RR_WATER_TEMPLE_JET_LIFT and RR_WATER_TEMPLE_HIGH_EMBLEM access
        EventAccess(LOGIC_WATER_COULD_HIGH_FROM_MID, []{return logic->WaterLevel(WL_MID) &&
                                                               (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->HasItem(RG_BRONZE_SCALE)) &&
                                                               logic->CanHitSwitch(ED_BOMB_THROW);})
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,                []{return true;}),
        //Implies CanAvoid(RE_STINGERS)
        Entrance(RR_WATER_TEMPLE_JET_CHEST_ROOM,      []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F, []{return logic->WaterLevel(WL_MID) &&
                                                           (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->HasItem(RG_BRONZE_SCALE));}),
        //Child can jumpslash with the lit stick to light the torch
        Entrance(RR_WATER_TEMPLE_PILLAR_2F,           []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}),
        //assumes RR_WATER_TEMPLE_3F_CENTRAL_LM and RR_WATER_TEMPLE_HIGH_EMBLEM access
        Entrance(RR_WATER_TEMPLE_PILLAR_H,            []{return ctx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) &&
                                                           (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)) &&
                                                           logic->CanUse(RG_LONGSHOT) &&
                                                           logic->Water3FCentralToHighEmblem();}),
        //hookshot + jumpslash can make it up through the platform slit as adult, is easier with WL_MID but still possible on WL_LOW, but it's a trick
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_LM,       []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP,          []{return logic->CanHitEyeTargets() && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS));}),
    });

    areaTable[RR_WATER_TEMPLE_SIDE_TOWER_1F] = Region("Water Temple Side Tower 1F", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_LOW_FROM_HIGH, []{return logic->HasItem(RG_BRONZE_SCALE);}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_TORCH_POT_1, (logic->WaterLevel(WL_LOW) && logic->CanBreakPots()) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_WATER_TEMPLE_TORCH_POT_2, (logic->WaterLevel(WL_LOW) && logic->CanBreakPots()) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_LOW_EMBLEM,    []{return logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_2F, []{return logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_TORCH_ROOM,    []{return logic->WaterLevel(WL_LOW) && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}),
    });

    areaTable[RR_WATER_TEMPLE_SIDE_TOWER_2F] = Region("Water Temple Side Tower 2F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,         []{return (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->WaterLevel(WL_LOW);}),
        Entrance(RR_WATER_TEMPLE_CRACKED_WALL, []{return logic->WaterLevel(WL_LOW_OR_MID) && logic->HasExplosives();}),
    });

    areaTable[RR_WATER_TEMPLE_LOW_EMBLEM] = Region("Water Temple Low Emblem", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_LOW,       []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        EventAccess(LOGIC_WATER_COULD_LOW, []{return true;}),
    }, {}, {
        //Exits
        //trick: with WL_HIGH, chu cracked wall from here, then open underwater chest
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return logic->WaterLevel(WL_LOW) || (logic->WaterTimer() >= 24 && logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_2F, []{return logic->WaterLevel(WL_LOW_OR_MID) && ((logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_WATER_CRACKED_WALL_HOVERS)) || ctx->GetTrickOption(RT_WATER_CRACKED_WALL)) ||
                                                          logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_4_SPIKES_ROOM, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_4_SPIKES_ROOM] = Region("Water Temple 4 Spikes Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MAP_CHEST, AnyAgeTime([]{return logic->CanKillEnemy(RE_SPIKE, ED_CLOSE, true, 4);})),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_1F, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_SPIKE, ED_CLOSE, true, 4);});}),
    });

    areaTable[RR_WATER_TEMPLE_CRACKED_WALL] = Region("Water Temple Cracked Wall", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_CRACKED_WALL_CHEST, logic->WaterLevel(WL_LOW_OR_MID) || logic->CanOpenUnderwaterChest()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_2F, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_TORCH_ROOM] = Region("Water Temple Torch Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_TORCHES_CHEST, AnyAgeTime([]{return (logic->WaterLevel(WL_LOW) && logic->CanKillEnemy(RE_SHELL_BLADE, ED_CLOSE, true, 3)) ||
                                                                                           logic->CanOpenUnderwaterChest();})),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_SIDE_TOWER_1F, []{return ((logic->WaterLevel(WL_LOW) || logic->CanUse(RG_IRON_BOOTS)) &&
                                                          AnyAgeTime([]{return (logic->WaterLevel(WL_LOW) && logic->CanKillEnemy(RE_SHELL_BLADE, ED_CLOSE, true, 3)) ||
                                                                                                     logic->CanUse(RG_HOOKSHOT);}));}),
    });

    areaTable[RR_WATER_TEMPLE_SPIKE_MOAT] = Region("Water Temple Spike Moat", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,              []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_BEHIND_SPIKE_MOAT, []{return (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS)));}),
    });

    areaTable[RR_WATER_TEMPLE_BEHIND_SPIKE_MOAT] = Region("Water Temple Behind Spike Moat", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        //You can backflip over the spikes and into the water. You can equip irons to get over while skipping swim, but it leaves you stuck in the pond
        Entrance(RR_WATER_TEMPLE_SPIKE_MOAT,     []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_BOULDERS_SOUTH, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 4);}),
    });

    areaTable[RR_WATER_TEMPLE_BOULDERS_SOUTH] = Region("Water Temple Boulders South", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, logic->CanUse(RG_LONGSHOT)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BEHIND_SPIKE_MOAT, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 4);}),
        Entrance(RR_WATER_TEMPLE_BOULDERS_NORTH,    []{return logic->HasItem(RG_BRONZE_SCALE) || (ctx->GetTrickOption(RT_WATER_INVISIBLE_HOOKSHOT_TARGET) && logic->CanUse(logic->IsAdult ? RG_HOOKSHOT : RG_LONGSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_BOULDER_CANAL,     []{return (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP))) ||
                                                           // A midair ground jump gets child onto the ledge, but they can't reasonably do anything without irons, and this may need TakeDamage due to the boulders
                                                           // swim can be skipped by boots changing during the ledge climb
                                                           (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanGroundJump() && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
    });

    areaTable[RR_WATER_TEMPLE_BOULDERS_NORTH] = Region("Water Temple Boulders North", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BOULDERS_SOUTH,    []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_TARGET, []{return true;}),
    });

    //The water rising in this room is permanent, but there is no way to do so without breaking the walls without glitches
    //because of this the logic for this room must either check for if you can raise the water, or be possible regardless of the water
    areaTable[RR_WATER_TEMPLE_BLOCK_ROOM_TARGET] = Region("Water Temple Block Room Target", SCENE_WATER_TEMPLE, {}, {
        //Locations
        //from this door, we can get the pots swimless with irons if we have to
        //using bombchus from this side is trickworthy due to awkward terrain
        LOCATION(RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_1, logic->CanBreakPots(ED_HOOKSHOT, false, true) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_2, logic->CanBreakPots(ED_HOOKSHOT, false, true) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,        []{return true;}),
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS, []{return logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_WATER_TEMPLE_BLOCK_ROOM] = Region("Water Temple Block Room", SCENE_WATER_TEMPLE, {
        //Events
        //Implies CanAvoid(RE_STINGER)
        //the full logic for the puzzle, as it is cut down here for optimisation
        //EventAccess(LOGIC_WATER_PUSHED_B1_BLOCK, []{return logic->HasItem(RG_GORONS_BRACELET) && logic->HasExplosives() &&
        //                                                   (logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS));}),
    }, {
        //Locations                                           //we can always get the pots by shooting them from afar and diving for the item...
        LOCATION(RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_1, (logic->CanBreakPots(ED_LONGSHOT, true, true) && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                              //or we can use irons and hookshot the pot, hookshoting to the target door if needed to cross the gap
                                                              (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 8)),
        LOCATION(RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_2, (logic->CanBreakPots(ED_LONGSHOT, true, true) && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                              (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 8)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_TARGET, []{return AnyAgeTime([]{return logic->HasItem(RG_GORONS_BRACELET) && logic->HasExplosives();}) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS, []{return AnyAgeTime([]{return logic->HasItem(RG_GORONS_BRACELET) && logic->HasExplosives();}) && logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS] = Region("Water Temple Block Room Stairs", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,        []{return true;}),
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_TARGET, []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_WATER_TEMPLE_3_JETS_SWITCH] = Region("Water Temple 3 Jets Room", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS, []{return true;}),
        Entrance(RR_WATER_TEMPLE_3_JETS_NO_SWITCH,  []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_3_JETS_NO_SWITCH] = Region("Water Temple 3 Jets Room No Switch", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_3_JETS_SWITCH,   []{return logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanStandingShield());}),
        Entrance(RR_WATER_TEMPLE_CANAL_ALCOVE,    []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_CANAL_ALCOVE] = Region("Water Temple Canal Alcove", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) ||
                                                         //child can also do a hovers backwalk backflip to reach the token after killing the skull
                                                         ((logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) &&
                                                         //killing with bombchu from here is hard due to the terrain, but adult can do it much easier from the river so it's only relevant for child
                                                         logic->CanUse(RG_HOVER_BOOTS) && logic->CanKillEnemy(RE_GOLD_SKULLTULA, logic->HasItem(RG_BRONZE_SCALE) && logic->IsAdult ? ED_SHORT_JUMPSLASH : ED_BOOMERANG))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_3_JETS_SWITCH, []{return true;}),
        Entrance(RR_WATER_TEMPLE_BOULDER_CANAL, []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_BEHIND_CANAL,  []{return logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_BOULDER_CANAL] = Region("Water Temple Boulder Canal", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, (logic->IsAdult && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        //making the jump as adult without jumpslash is possible, but hard enough to be a trick
        Entrance(RR_WATER_TEMPLE_BOULDERS_SOUTH, []{return logic->HasItem(RG_BRONZE_SCALE) || (logic->IsAdult && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS)));}),
        Entrance(RR_WATER_TEMPLE_BOULDERS_NORTH, []{return logic->HasItem(RG_BRONZE_SCALE) || (ctx->GetTrickOption(RT_WATER_INVISIBLE_HOOKSHOT_TARGET) && logic->CanUse(logic->IsAdult ? RG_HOOKSHOT : RG_LONGSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_CANAL_ALCOVE,   []{return logic->IsAdult;}),
        Entrance(RR_WATER_TEMPLE_BEHIND_CANAL,   []{return logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 8;}),
    });

    areaTable[RR_WATER_TEMPLE_BEHIND_CANAL] = Region("Water Temple Behind Canal", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BOULDER_CANAL, []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_BOSS_KEY_ROOM, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 5);}),
    });

    areaTable[RR_WATER_TEMPLE_BOSS_KEY_ROOM] = Region("Water Temple Boss Key Room", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_BOSS_KEY_CHEST, true),
        LOCATION(RC_WATER_TEMPLE_BOSS_KEY_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_BOSS_KEY_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BEHIND_CANAL, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 5);}),
    });

    areaTable[RR_WATER_TEMPLE_NEAR_CAGE_STEPS] = Region("Water Temple Near Cage Steps", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,      []{return logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 16;}),
        Entrance(RR_WATER_TEMPLE_NEAR_CAGE, []{return logic->CanUse(RG_HOOKSHOT) ||
                                                      (logic->CanUse(RG_HOVER_BOOTS) && (logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())));}),
    });

    areaTable[RR_WATER_TEMPLE_NEAR_CAGE] = Region("Water Temple Near Cage", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_NEAR_CAGE_STEPS, []{return true;}),
        //Chus can hit the switch, but not bombs, and it's a kinda narrow timing so probably a trick
        Entrance(RR_WATER_TEMPLE_GS_CAGE,         []{return logic->CanJumpslash() || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)));}),
    });

    areaTable[RR_WATER_TEMPLE_GS_CAGE] = Region("Water Temple GS Cage", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_BEHIND_GATE,    logic->IsAdult || logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) &&logic->CanGroundJump() && logic->CanJumpslash())),
        LOCATION(RC_WATER_TEMPLE_BEHIND_GATE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_BEHIND_GATE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_BEHIND_GATE_POT_3, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_BEHIND_GATE_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        //The room is too small to hit the switch with pots
        Entrance(RR_WATER_TEMPLE_NEAR_CAGE, []{return logic->CanHitSwitch();}),
    });

    areaTable[RR_WATER_TEMPLE_BLOCK_U_BEND] = Region("Water Temple Block U-Bend", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,                []{return logic->Get(LOGIC_WATER_PUSHED_1F_BLOCK) && ((logic->CanUse(RG_IRON_BOOTS) && ((logic->CanUse(RG_HOOKSHOT) && logic->WaterLevel(WL_LOW)) || logic->HasItem(RG_BRONZE_SCALE))) ||
                                                                (logic->WaterLevel(WL_HIGH_OR_MID) && logic->CanUse(RG_SILVER_SCALE))) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_OUTSIDE_DRAGON_ROOM, []{return logic->CanHitSwitch();}),
    });

    areaTable[RR_WATER_TEMPLE_OUTSIDE_DRAGON_ROOM] = Region("Water Temple Outside Dragon Room", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_U_BEND, []{return logic->CanHitSwitch(ED_BOOMERANG) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_DRAGON_ROOM,  []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_DRAGON_ROOM] = Region("Water Temple Dragon Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_DRAGON_CHEST, logic->HasItem(RG_BRONZE_SCALE) &&
                                               ((logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS)) ||
                                                (((logic->IsAdult && ctx->GetTrickOption(RT_WATER_ADULT_DRAGON)) || (logic->IsChild && ctx->GetTrickOption(RT_WATER_CHILD_DRAGON))) &&
                                                 logic->CanHitSwitch(ED_BOOMERANG, true) && (logic->HasItem(RG_SILVER_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8))))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_OUTSIDE_DRAGON_ROOM, []{return true;}),
        Entrance(RR_WATER_TEMPLE_ABOVE_DRAGON,        []{return false;}),
    });

    areaTable[RR_WATER_TEMPLE_PILLAR_1F] = Region("Water Temple Pillar 1F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,      []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 5) && logic->WaterLevel(WL_LOW);}),
        Entrance(RR_WATER_TEMPLE_PILLAR_2F, []{return logic->CanUse(RG_HOOKSHOT) || (logic->WaterLevel(WL_MID) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_PILLAR_B1, []{return logic->WaterLevel(WL_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40;}),
        Entrance(RR_WATER_TEMPLE_PILLAR_H,  []{return ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && (logic->CanUse(RG_IRON_BOOTS) || logic->HasItem(RG_BRONZE_SCALE));}),
    });

    areaTable[RR_WATER_TEMPLE_PILLAR_2F] = Region("Water Temple Pillar 2F", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_MIDDLE,       []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        EventAccess(LOGIC_WATER_COULD_MIDDLE, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, logic->CanUse(RG_LONGSHOT);),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM, []{return true;}),
        Entrance(RR_WATER_TEMPLE_PILLAR_1F,     []{return logic->WaterLevel(WL_LOW) || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_PILLAR_H,      []{return ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && (logic->CanUse(RG_IRON_BOOTS) || logic->HasItem(RG_BRONZE_SCALE));}),
    });

    areaTable[RR_WATER_TEMPLE_PILLAR_H] = Region("Water Temple Pillar High Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, logic->HasItem(RG_BRONZE_SCALE) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM, []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_PILLAR_B1,     []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40;}),
    });

    //Assuming tunic and irons was checked on entry
    areaTable[RR_WATER_TEMPLE_PILLAR_B1] = Region("Water Temple Pillar B1", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_BRONZE_SCALE)),
    }, {
        //Exits
        //Can't know water level, so we'll just assume any possibility and skip to MAIN
        Entrance(RR_WATER_TEMPLE_MAIN, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_WATER_TEMPLE_JET_CHEST_ROOM] = Region("Water Temple Jet Chest Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_COMPASS_CHEST,      logic->CanUseProjectile()),
        LOCATION(RC_WATER_TEMPLE_NEAR_COMPASS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_NEAR_COMPASS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_NEAR_COMPASS_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN, []{return (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->WaterLevel(WL_LOW_OR_MID);}),
    });

    areaTable[RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F] = Region("Water Temple Outside Jet Lift 2F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM, []{return logic->WaterLevel(WL_MID) && (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_JET_LIFT_2F,   []{return logic->WaterLevel(WL_MID);}),
    });

    areaTable[RR_WATER_TEMPLE_JET_LIFT_2F] = Region("Water Temple Jet Lift 2F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F, []{return logic->WaterLevel(WL_LOW_OR_MID) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_JET_LIFT_3F,         []{return logic->CanHitSwitch(ED_BOMB_THROW);}),
    });

    areaTable[RR_WATER_TEMPLE_JET_LIFT_3F] = Region("Water Temple Jet Lift 3F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_JET_LIFT_2F, []{return true;}),
        Entrance(RR_WATER_TEMPLE_HIGH_EMBLEM, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_HIGH_EMBLEM] = Region("Water Temple High Emblem", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_HIGH,       []{return true;}),
        EventAccess(LOGIC_WATER_SCARECROW,  []{return logic->ScarecrowsSong();}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,         []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A, []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H, []{return logic->WaterLevel(WL_HIGH);}),
    });

    //This region represents being able to be in "front" of the block while it's in it's half-pushed state
    //Currently assumes WL_LOW_OR_MID as there's no way to reach it on WL_HIGH in logic, this will need splitting if one is added
    areaTable[RR_WATER_TEMPLE_BLOCK_LOOP] = Region("Water Temple Block Loop", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_BACK,  []{return logic->HasItem(RG_GORONS_BRACELET) && logic->WaterLevel(WL_LOW_OR_MID);}),
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_3F_LM, []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    //Currently assumes WL_LOW_OR_MID as there's no way to reach it on WL_HIGH in logic, this will need splitting if one is added
    areaTable[RR_WATER_TEMPLE_BLOCK_LOOP_BACK] = Region("Water Temple Block Loop Back", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, true),
        LOCATION(RC_WATER_TEMPLE_CENTRAL_BOW_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_CENTRAL_BOW_POT_2,        logic->CanBreakPots()),
    }, {});

    //This region is not able to finish pushing the block out of the way to reach the chest
    areaTable[RR_WATER_TEMPLE_BLOCK_LOOP_3F_A] = Region("Water Temple Block Loop 3F Any Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_1, logic->CanUse(RG_BOMBCHU_5) && (logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_2, logic->CanUse(RG_BOMBCHU_5) && (logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,         []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A, []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
    });

    //This region is not able to finish pushing the block out of the way to reach the chest
    //assumes checking WL_HIGH on entry
    areaTable[RR_WATER_TEMPLE_BLOCK_LOOP_3F_H] = Region("Water Temple Block Loop 3F High Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_3F_A, []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,    []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
    });

    //This region is not able to finish pushing the block out of the way to reach the chest
    //Assumes checking WL_LOW_OR_MID on entry
    areaTable[RR_WATER_TEMPLE_BLOCK_LOOP_3F_LM] = Region("Water Temple Block Loop 3F Low or Mid Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_BLOCK_LOOP_3F_A, []{return true;}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_LM,   []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_LM,   []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_OUTSIDE_WATERFALL] = Region("Water Temple Outside Waterfall", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MAIN,                []{return true;}),
        Entrance(RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F, []{return logic->WaterLevel(WL_MID);}),
        Entrance(RR_WATER_TEMPLE_WATERFALL,           []{return logic->WaterLevel(WL_HIGH) && logic->SmallKeys(SCENE_WATER_TEMPLE, 4);}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,        []{return logic->WaterLevel(WL_HIGH) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_2F_CENTRAL_H,        []{return logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS);}),
    });

    //This is specifically the initial entrance alcove and platform directly underneath
    areaTable[RR_WATER_TEMPLE_WATERFALL] = Region("Water Temple Waterfall", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_LONGSHOT) ||
                                                           (ctx->GetTrickOption(RT_WATER_RANG_FALLING_PLATFORM_GS) && logic->CanUse(RG_BOOMERANG)) ||
                                                           (ctx->GetTrickOption(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_OUTSIDE_WATERFALL, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 4);}),
        Entrance(RR_WATER_TEMPLE_WATERFALL_TOP,     []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_WATERFALL_TOP] = Region("Water Temple Waterfall Top", SCENE_WATER_TEMPLE, {}, {}, {
        Entrance(RR_WATER_TEMPLE_WATERFALL,     []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_TOGGLE_SWITCH, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 5);}),
    });

    areaTable[RR_WATER_TEMPLE_TOGGLE_SWITCH] = Region("Water Temple Toggle Switch", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_WATERFALL_TOP,    []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 5);}),
        //you can reach the spikes with hovers + CanHitSwitch(ED_BOMB_THROW) but can't cross them
        Entrance(RR_WATER_TEMPLE_LIKE_LIKE_SPIKES, []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    //specifically the area past the spikes
    areaTable[RR_WATER_TEMPLE_LIKE_LIKE_SPIKES] = Region("Water Temple Like Like Spikes", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_LIKE_LIKE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_LIKE_LIKE_POT_2, logic->CanBreakPots()),
    }, {
        //swim can be skipped here as adult if you have hookshot and a melee weapon, as you can use the side of the snouts of the pillars to just barely reach the next platform,
        //and get a glitchy ledge grab by jumping from it while it is raised for the final one, which can be escaped with a jumpslash.
        //this is a trick separate from glitch ledge grabs however as the snouts are very wonky collision to climb on.
        Entrance(RR_WATER_TEMPLE_TOGGLE_SWITCH,  []{return (logic->HasItem(RG_BRONZE_SCALE) && logic->CanHitSwitch(ED_BOMB_THROW)) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_DARK_LINK_ROOM] = Region("Water Temple Dark Link Room", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_LIKE_LIKE_SPIKES, []{return logic->CanKillEnemy(RE_DARK_LINK);}),
        Entrance(RR_WATER_TEMPLE_SOT_PIT_ROOM,     []{return logic->CanKillEnemy(RE_DARK_LINK);}),
    });

    areaTable[RR_WATER_TEMPLE_SOT_PIT_ROOM] = Region("Water Temple Song Of Time Pit Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_LONGSHOT_CHEST, true),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM, []{return true;}),
        Entrance(RR_WATER_TEMPLE_RIVER,          []{return logic->IsChild || logic->CanUse(RG_SONG_OF_TIME);}),
    });

    areaTable[RR_WATER_TEMPLE_RIVER] = Region("Water Temple River", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_RIVER,      (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 16)),
        LOCATION(RC_WATER_TEMPLE_RIVER_HEART_1, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16) || logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_WATER_TEMPLE_RIVER_HEART_2, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16) || logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_WATER_TEMPLE_RIVER_HEART_3, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16) || logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_WATER_TEMPLE_RIVER_HEART_4, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16) || logic->HasItem(RG_BRONZE_SCALE)),
    }, {
        //Exits
        //it is possible with persistence for adult to make it past the final whirlpool with only irons and climb to the pots,
        //but the collision around there is very janky and loves making you slide back down.
        //combined with the drowning time pressure this is a trick.
        //For best results walk to the right of the seam pointing to the pots, then move left to try and cross it at the very top.
        Entrance(RR_WATER_TEMPLE_RIVER_POTS, []{return (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 32 && logic->CanUse(RG_LONGSHOT)) || logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_RIVER_POTS] = Region("Water Temple River Pots", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_GS_RIVER,      ctx->GetTrickOption(RT_WATER_RIVER_GS) && logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_WATER_TEMPLE_RIVER_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_RIVER_POT_2,   logic->CanBreakPots()),
    }, {
        //Exits
        //Child can enter the water without jumping to have just enough time to make it with just swim, but it's a trick
        Entrance(RR_WATER_TEMPLE_ABOVE_DRAGON, []{return logic->CanHitEyeTargets() && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_WATER_TEMPLE_RIVER,        []{return logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_ABOVE_DRAGON] = Region("Water Temple Above Dragon", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_RIVER_CHEST,   true),
        LOCATION(RC_WATER_TEMPLE_DRAGON_CHEST,  logic->IsAdult && logic->CanHitSwitch(ED_LONGSHOT) && logic->HasItem(RG_BRONZE_SCALE) &&
                                                ((ctx->GetTrickOption(RT_WATER_ADULT_DRAGON) && (logic->HasItem(RG_SILVER_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8))) ||
                                                 ctx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash());}),
    });

    areaTable[RR_WATER_TEMPLE_RISING_TARGET_LEDGE] = Region("Water Temple Rising Target Ledge", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS,       []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_WATER_COULD_MIDDLE, []{return logic->WaterRisingTargetTo3FCentral() && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}),
    }, {
        // Locations
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_1_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MAIN_LEVEL_1_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        //As you cannot change the water level from here, we must be able to make the drop on any water level
        //Bronze scale does this alone thanks to the nearby pond cancelling fall damage
        //otherwise we need irons for high level and taking damage for low
        Entrance(RR_WATER_TEMPLE_MAIN,          []{return logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->TakeDamage());}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_A,  []{return logic->WaterRisingTargetTo3FCentral();}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_H,  []{return logic->WaterRisingTargetTo3FCentral() && logic->WaterLevel(WL_HIGH);}),
        Entrance(RR_WATER_TEMPLE_3F_CENTRAL_LM, []{return logic->WaterRisingTargetTo3FCentral() && logic->WaterLevel(WL_LOW_OR_MID);}),
        //Assumes RR_WATER_TEMPLE_3F_CENTRAL, RR_WATER_TEMPLE_HIGH_EMBLEM and RR_WATER_TEMPLE_2F_CENTRAL access
        Entrance(RR_WATER_TEMPLE_PILLAR_H,      []{return ctx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) && logic->CanUse(RG_FIRE_ARROWS) && logic->WaterRisingTargetTo3FCentral();}),
        Entrance(RR_WATER_TEMPLE_TRAPPED_SLOPE, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_TRAPPED_SLOPE] = Region("Water Temple Trapped Slope", SCENE_WATER_TEMPLE, {}, {}, {
        Entrance(RR_WATER_TEMPLE_RISING_TARGET_LEDGE, []{return true;}),
        Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,       []{return true;}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_WATER_TEMPLE_MQ_ENTRANCE_LEDGE] = Region("Water Temple MQ Entrance Ledge", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_MIDDLE, []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_ENTRYWAY,         []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,          []{return true;}),
        //If we are not on WL_HIGH, we reach RR_WATER_TEMPLE_MQ_3F_MAIN with hookshot via 2F, otherwise we can reach the platform
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,  []{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_H,  []{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && logic->WaterLevel(WL_HIGH);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_LM, []{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && logic->WaterLevel(WL_LOW_OR_MID);}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM, []{return logic->WaterLevel(WL_LOW_OR_MID);}),
    });

    //This region covers simply existing in the area around the central pillar without being on a specific platform, either swimming or walking on the lakebed
    //Entry should only include being in the correct area, taking any possible fall damage, and floating up to the surface of WL_HIGH if coming from below
    //This area then leads to others based on level and worst-case water timers for follow-up exits from the water's surface
    //remember that any solution that works for any level doesn't need to be given a level, even if that solution is overkill for a lower level
    areaTable[RR_WATER_TEMPLE_MQ_MAIN] = Region("Water Temple MQ Main", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_MIDDLE, []{return logic->CanUse(RG_HOOKSHOT);}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_ENTRANCE_LEDGE,      []{return logic->HasItem(RG_BRONZE_SCALE) && logic->WaterLevel(WL_HIGH);}),
        Entrance(RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F,       []{return (logic->WaterTimer() >= 24 && logic->CanUse(RG_IRON_BOOTS)) || (logic->WaterLevel(WL_LOW_OR_MID) && logic->HasItem(RG_GOLDEN_SCALE) && logic->WaterTimer() >= 16) || logic->WaterLevel(WL_LOW);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,        []{return logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_H,        []{return logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE);}),
        //First water timer uses the hook to go from the top of center to storage room/central pillar as coming from the bottom
        //Second water timer is simply diving down and entering the door as fast as possible from the surface
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_A,        []{return ((logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && (logic->WaterLevel(WL_LOW_OR_MID) || logic->WaterTimer() >= 16))) && logic->CanUse(RG_LONGSHOT)) ||
                                                                   ((logic->WaterLevel(WL_MID) || (logic->WaterLevel(WL_HIGH_OR_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)) && logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_H,        []{return logic->WaterLevel(WL_HIGH) &&
                                                                   ((logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_LONGSHOT)) ||
                                                                    (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->HasItem(RG_BRONZE_SCALE)));}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM,       []{return ((logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterLevel(WL_LOW_OR_MID))) && logic->CanUse(RG_LONGSHOT)) ||
                                                                   (logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_1F,           []{return logic->WaterLevel(WL_LOW);}),
        //A special entry as we can't set it to high after entering at a lower height
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_H,            []{return logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT));}),
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_A,     []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_LM,    []{return logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH,      []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && (logic->WaterLevel(WL_LOW) || ((logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24) && logic->HasItem(RG_BRONZE_SCALE)));}),
        Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM, []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && ((logic->WaterLevel(WL_LOW) && logic->HasItem(RG_SILVER_SCALE)) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 &&  (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT))));}),
        //Adult needs to jump in instead of dive for swim access, but you just hold forward. RT_WATER_BK_REGION Isn't relevant unless the Dark Link loop can be done without longshot with other tricks
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM,   []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && ((logic->WaterLevel(WL_LOW) && logic->HasItem(RG_BRONZE_SCALE)) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_BRONZE_SCALE))) && (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS)));}),
    });

    //This region specifically covers the topmost platform around central pillar
    areaTable[RR_WATER_TEMPLE_MQ_3F_CENTRAL_A] = Region("Water Temple MQ 3F Central Any Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,                []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_ENTRANCE_LEDGE,      []{return logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_A,        []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE, []{return ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()));}),
        //this swimless jump with irons may be a trick as you have to put irons on quite late.
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_A,     []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}),
        //Jumping across is possible but a trick due to the janky ledge
        Entrance(RR_WATER_TEMPLE_MQ_HIGH_EMBLEM,         []{return logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}),
    });

    //This region specifically covers the topmost platform around central pillar
    //Assumes WL_HIGH is checked on entry
    areaTable[RR_WATER_TEMPLE_MQ_3F_CENTRAL_H] = Region("Water Temple MQ 3F Central High Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_H,        []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE, []{return logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_IRON_BOOTS_LEDGE_GRAB) && logic->IsAdult && logic->HasItem(RG_BRONZE_SCALE) && logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_MQ_OUTSIDE_WATERFALL,   []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}),
    });

    //This region specifically covers the topmost platform around central pillar
    areaTable[RR_WATER_TEMPLE_MQ_3F_CENTRAL_LM] = Region("Water Temple MQ 3F Central Low or Mid Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM,    []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_LM, []{return true;}),
    });

    //This region specifically covers walking on the lower platform around central pillar.
    //This is underwater when WL_HIGH and irons are assumed if this is the case
    areaTable[RR_WATER_TEMPLE_MQ_2F_CENTRAL_A] = Region("Water Temple MQ 2F Central Any Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,                  []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,          []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_STORAGE_ROOM,          []{return logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_WATER_IRON_BOOTS_LEDGE_GRAB) && logic->HasItem(RG_BRONZE_SCALE));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_2F_CENTRAL_H] = Region("Water Temple MQ 2F Central High Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_A, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_H,     []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM] = Region("Water Temple MQ 2F Central Low or Mid Water", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_A,     []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_2F,        []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_2F, []{return (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_LM, []{return logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_HIGH_EMBLEM] = Region("Water Temple MQ High Emblem", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_SCARECROW, []{return logic->ScarecrowsSong();}),
        EventAccess(LOGIC_WATER_HIGH,      []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,         []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE] = Region("Water Temple MQ Rising Target Ledge", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        //As you cannot change the water level from here, we must be able to make the drop on any water level
        //Bronze scale does this alone thanks to the nearby pond cancelling fall damage
        //otherwise we need irons for high level and taking damage for low
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,           []{return logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->TakeDamage());}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,   []{return logic->WaterRisingTargetTo3FCentral();}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_H,   []{return logic->WaterRisingTargetTo3FCentral() && logic->WaterLevel(WL_HIGH);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_LM,  []{return logic->WaterRisingTargetTo3FCentral() && logic->WaterLevel(WL_LOW_OR_MID);}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR_RAMP, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BOSS_DOOR_RAMP] = Region("Water Temple MQ Boss Door Ramp", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR,           []{return logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_ICE_ARROWS) || logic->CanUse(RG_NAYRUS_LOVE);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BOSS_DOOR] = Region("Water Temple MQ Boss Door", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR_RAMP, []{return logic->CanUse(RG_ICE_ARROWS) || logic->TakeDamage();}),
        Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,     []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F] = Region("Water Temple MQ Side Tower 1F", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_LOW_FROM_HIGH, []{return logic->HasItem(RG_BRONZE_SCALE);}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_LOWER_TORCHES_POT_1, (logic->WaterLevel(WL_LOW) && logic->CanBreakPots()) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 16)),
        LOCATION(RC_WATER_TEMPLE_MQ_LOWER_TORCHES_POT_2, (logic->WaterLevel(WL_LOW) && logic->CanBreakPots()) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 16)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,              []{return logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16);}),
        Entrance(RR_WATER_TEMPLE_MQ_TOWER_TARGET_ROOM, []{return logic->WaterLevel(WL_LOW) && (logic->CanUse(RG_FAIRY_BOW) || logic->HasFireSourceWithTorch());}),
        Entrance(RR_WATER_TEMPLE_MQ_SIDE_TOWER_2F,     []{return (logic->WaterLevel(WL_MID) && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                                 (logic->Get(LOGIC_WATER_MQ_SIDE_TOWER_TARGETS) && logic->CanUse(RG_LONGSHOT));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_SIDE_TOWER_2F] = Region("Water Temple MQ Side Tower 2F", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, logic->CanUse(RG_HOOKSHOT) && (logic->WaterLevel(WL_MID) || logic->CanOpenUnderwaterChest())),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F, []{return logic->WaterLevel(WL_LOW) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_LOW_EMBLEM,    []{return (logic->WaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                             //SoT can be skipped with longshot, but it's precise to get an angle you can ledgegrab with so a trick
                                                             (logic->Get(LOGIC_WATER_MQ_SIDE_TOWER_TARGETS) && logic->WaterLevel(WL_LOW_OR_MID) && logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SONG_OF_TIME));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_LOW_EMBLEM] = Region("Water Temple MQ Low Emblem", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_LOW, []{return true;}),
        EventAccess(LOGIC_WATER_LOW,       []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_3_STALFOS_ROOM, []{return logic->WaterLevel(WL_HIGH) && logic->HasFireSource();}),
        Entrance(RR_WATER_TEMPLE_MQ_SIDE_TOWER_2F,  []{return logic->WaterLevel(WL_LOW) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_STICKS));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_3_STALFOS_ROOM] = Region("Water Temple MQ 3 Stalfos Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_MAP_CHEST, logic->CanUse(RG_HOOKSHOT)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_LOW_EMBLEM, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 4);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_TOWER_TARGET_ROOM] = Region("Water Temple MQ Tower Target Room", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_MQ_SIDE_TOWER_TARGETS, []{return logic->CanKillEnemy(RE_LIZALFOS) && logic->CanKillEnemy(RE_SPIKE);}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_COMPASS_CHEST, logic->Get(LOGIC_WATER_MQ_SIDE_TOWER_TARGETS)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F, []{return true;}),
    });

    //This area assumes we entered through the lower door, so water is low or mid
    areaTable[RR_WATER_TEMPLE_MQ_PILLAR_1F] = Region("Water Temple MQ Central Pillar 1F", SCENE_WATER_TEMPLE, {
        //Events
        //This is harder than the other possibilities as you have to move between shots on top of the extra range
        EventAccess(LOGIC_WATER_MQ_B1_SWITCH, []{return ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS);}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,      []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_H,  []{return logic->WaterLevel(WL_HIGH) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_2F, []{return logic->CanUse(RG_HOOKSHOT) || (logic->WaterLevel(WL_MID) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->HasItem(RG_BRONZE_SCALE));}),
        //if the gate is open, you sink straight in, so you can't climb up this way in logic without swimming
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_B1, []{return logic->Get(LOGIC_WATER_MQ_B1_OPENED_PILLAR) && logic->WaterLevel(WL_HIGH_OR_MID) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}),
    });

    //If we enter here in WL_HIGH, go to RR_WATER_TEMPLE_MQ_PILLAR_H instead, Assumes WL_LOW_OR_MID
    areaTable[RR_WATER_TEMPLE_MQ_PILLAR_2F] = Region("Water Temple MQ Central Pillar 2F", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_COULD_MIDDLE,         []{return true;}),
        EventAccess(LOGIC_WATER_MIDDLE,               []{return logic->CanUse(RG_ZELDAS_LULLABY);}),
        //It's possible to do this even on low water, but more awkward. I'm not sure if it's even possible for it to be relevant though.
        EventAccess(LOGIC_WATER_MQ_B1_OPENED_PILLAR,  []{return ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS);}),
        EventAccess(LOGIC_WATER_MQ_PILLAR_SOT_BLOCK,  []{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SONG_OF_TIME);}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_1, logic->CanBreakCrates() && logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_2, logic->CanBreakCrates() && logic->CanUse(RG_LONGSHOT)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_H,      []{return logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_FARORES_WIND) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_B1,     []{return logic->Get(LOGIC_WATER_MQ_B1_OPENED_PILLAR) && logic->WaterLevel(WL_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_PILLAR_H] = Region("Water Temple MQ Central Pillar High", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_MQ_B1_OPENED_PILLAR, []{return ((logic->Get(LOGIC_WATER_MQ_PILLAR_SOT_BLOCK) && logic->CanUse(RG_DINS_FIRE)) ||
                                                                (ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS))) &&
                                                               (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslash()));}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_1, logic->CanBreakCrates() && logic->HasItem(RG_BRONZE_SCALE)),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_2, logic->CanBreakCrates() && logic->HasItem(RG_BRONZE_SCALE)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_B1, []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}),
    });

    //Assuming tunic and irons was checked on entry
    areaTable[RR_WATER_TEMPLE_MQ_PILLAR_B1] = Region("Water Temple MQ Central Pillar B1", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_1,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_2,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_3,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_4,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_5,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_6,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_7,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_8,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_9,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_10, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_11, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_12, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_13, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_14, logic->CanBreakCrates()),
    }, {
        //Exits
        //Can't know water level, so we'll just assume any possibility and skip to MAIN
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,            []{return logic->Get(LOGIC_WATER_MQ_B1_OPENED_PILLAR) && logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE);}),
        //Child needs to release irons for height to push down the larger "peg", however they can push the lower one down by climbing and then hit the switch through the larger peg, but it's a trick
        Entrance(RR_WATER_TEMPLE_MQ_PILLAR_B1_FINAL, []{return ((logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_BRONZE_SCALE)));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_PILLAR_B1_FINAL] = Region("Water Temple MQ Central Pillar B1 Final", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST,          logic->CanUse(RG_HOOKSHOT)),
    }, {});

    areaTable[RR_WATER_TEMPLE_MQ_STORAGE_ROOM] = Region("Water Temple MQ Storage Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_1,         logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_2,         logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_3,         logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_1,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_2,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_3,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_4,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_5,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_6,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_7,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_3, logic->CanBreakSmallCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_4, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN, []{return logic->WaterLevel(WL_LOW_OR_MID) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_OUTSIDE_HIDDEN_SWITCH_2F] = Region("Water Temple MQ Outside Hidden Switch 2F", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,             []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM,    []{return logic->WaterLevel(WL_MID) && (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_2F, []{return logic->WaterLevel(WL_MID);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_2F] = Region("Water Temple MQ Hidden Switch 2F", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_1,             logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_2,             logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_3,             logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_1,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_2,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_3,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_4,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_5,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_6,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_SMALL_CRATE, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,             []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_3F, []{return logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_3F] = Region("Water Temple MQ Hidden Switch 3F", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_CRATE_1,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_CRATE_2,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_SMALL_CRATE, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_HIDDEN_SWITCH_2F, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_HIGH_EMBLEM,      []{return true;}),
    });

    //Assumes Iron Boots are checked on entry if the water is high
    areaTable[RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_A] = Region("Water Temple MQ Lizalfos Loop Any Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_WEST_POT,     logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SOUTH_POT,    logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SE_POT,       logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_1,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_2,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_3,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_5, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A, []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    //Assumes WL_LOW_OR_MID is checked on entry
    areaTable[RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_LM] = Region("Water Temple MQ Lizalfos Loop Low or Mid Water", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_WEST_POT,     logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SOUTH_POT,    logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SE_POT,       logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_1,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_2,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_3,      logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_5, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_LIZALFOS_CAGE, []{return logic->CanUse(RG_DINS_FIRE);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,  []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_LM, []{return logic->CanUse(RG_HOOKSHOT);}),
    });

    //Assumes WL_LOW_OR_MID is checked on entry
    areaTable[RR_WATER_TEMPLE_MQ_LIZALFOS_CAGE] = Region("Water Temple MQ Lizalfos Cage", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,           logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_CAGE_SOUTH_POT,       logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_CAGE_NORTH_POT,       logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_GATE_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_GATE_CRATE_2, logic->CanBreakCrates()),
    }, {});

    areaTable[RR_WATER_TEMPLE_MQ_OUTSIDE_WATERFALL] = Region("Water Temple Outside Waterfall", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,                     []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_OUTSIDE_HIDDEN_SWITCH_2F, []{return logic->WaterLevel(WL_MID);}),
        Entrance(RR_WATER_TEMPLE_MQ_WATERFALL,                []{return logic->WaterLevel(WL_HIGH) && logic->SmallKeys(SCENE_WATER_TEMPLE, 1);}),
        Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL_H,             []{return logic->WaterLevel(WL_HIGH) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL_H,             []{return logic->WaterLevel(WL_HIGH) && logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_WATERFALL] = Region("Water Temple Waterfall", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_OUTSIDE_WATERFALL, []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 1);}),
        Entrance(RR_WATER_TEMPLE_MQ_WATERFALL_TOP,     []{return logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_WATERFALL_TOP] = Region("Water Temple Waterfall Top", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_WATERFALL,   []{return logic->CanUse(RG_LONGSHOT) && logic->CanHitSwitch(ED_FAR);}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT] = Region("Water Temple MQ Stalfos Pit", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_WATERFALL_TOP,     []{return logic->Get(LOGIC_WATER_MQ_STALFOS_PIT);}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_LOWER, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS,  []{return (logic->Get(LOGIC_WATER_MQ_STALFOS_PIT) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, []{return logic->Get(LOGIC_WATER_MQ_STALFOS_PIT) && logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT_LOWER] = Region("Water Temple MQ Stalfos Pit Lower", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_MQ_STALFOS_PIT, []{return ((logic->IsAdult && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3, false, true)) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->CanKillEnemy(RE_STALFOS, ED_BOMB_THROW, true, 3, false, true)));}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT,       []{return logic->CanUse(RG_HOOKSHOT) && (logic->IsAdult || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8));}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS,  []{return (logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || (logic->CanUse(RG_HOOKSHOT) && (logic->IsAdult || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) && (logic->CanUse(RG_HOVER_BOOTS) || logic->Get(LOGIC_WATER_MQ_STALFOS_PIT)));}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, []{return logic->Get(LOGIC_WATER_MQ_STALFOS_PIT) && (logic->IsAdult || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) && logic->CanUse(RG_HOOKSHOT);}),
    });

    //also includes the suns fairy in the middle
    areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS] = Region("Water Temple MQ Stalfos Pit Pots", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_NUT_ACCESS,   []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_STALFOS_PIT_SOUTH_POT,     logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STALFOS_PIT_MIDDLE_POT,    logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STALFOS_PIT_NORTH_POT,     logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_DARK_LINK_PILAR_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT,       []{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_LOWER, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, []{return logic->Get(LOGIC_WATER_MQ_STALFOS_PIT) && logic->CanUse(RG_HOOKSHOT);}),
    });

    //specifically the area past the spikes
    areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER] = Region("Water Temple MQ Stalfos Pit Upper", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_BEFORE_DARK_LINK_POT_1,     logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_BEFORE_DARK_LINK_POT_2,     logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_DARK_LINK_LEFT_STORM_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_WATER_TEMPLE_MQ_DARK_LINK_RIGHT_SUN_FAIRY,  logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_LOWER, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS,  []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM,    []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM] = Region("Water Temple MQ Dark Link Room", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_DARK_LINK);});}),
        Entrance(RR_WATER_TEMPLE_MQ_GATED_PIT,         []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_DARK_LINK);});}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_GATED_PIT] = Region("Water Temple MQ Gated Pit", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_AFTER_DARK_LINK_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_AFTER_DARK_LINK_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM,  []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_RIVER_SKULL,     []{return logic->CanUse(RG_HOOKSHOT) && (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->CanUse(RG_LONGSHOT));}),
    });

    //if we can use hookshot, we are standing on the targets, otherwise assume we're in the water
    areaTable[RR_WATER_TEMPLE_MQ_RIVER_SKULL] = Region("Water Temple MQ River Skull", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_RIVER, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_RIVER_POTS] = Region("Water Temple MQ River Pots", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_RIVER_POT_1, logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_RIVER_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_RIVER_SKULL,        []{return logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT));}),
        //You don't need to swim for this if you put irons on in midair and hold forward while aiming for the tunnel with a tight angle, but if you miss you have to void unless you have a hook. It's only relevant with glitches anyway
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL, []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE, []{return logic->HasItem(RG_SILVER_SCALE) || (logic->IsAdult && logic->HasItem(RG_BRONZE_SCALE) && ctx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE));}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash());}),
    });

    //This region assumes Iron boots to access
    areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL] = Region("Water Temple MQ Dragon Room Tunnel", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_4, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS,         []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE, []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE] = Region("Water Temple MQ Dragon Room Alcove", SCENE_WATER_TEMPLE, {
        //Events
        EventAccess(LOGIC_WATER_MQ_DRAGON_TORCHES, []{return logic->HasFireSource();}),
    }, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_CRATE_1,        logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_CRATE_2,        logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_1,  logic->CanBreakSmallCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_2,  logic->CanBreakSmallCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_3,  logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL, []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   []{return logic->HasItem(RG_SILVER_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR] = Region("Water Temple MQ Dragon Room Door", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR_CRATE_2, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS,            []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL,    []{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE,    []{return logic->HasItem(RG_SILVER_SCALE);}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH,  []{return logic->Get(LOGIC_WATER_MQ_DRAGON_TORCHES);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH] = Region("Water Temple MQ Boss Key Room Switch", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_BOSS_KEY_POT,        logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_BK_ROOM_UPPER_CRATE, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,    []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT,   []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST, []{return logic->CanHitSwitch() && AnyAgeTime([]{return logic->CanUse(RG_DINS_FIRE);});}),
    });

    //this exists for the crates in preparation for clips through the grate
    areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT] = Region("Water Temple MQ Boss Key Room Pit", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_4, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH, []{return logic->CanHitSwitch(ED_BOOMERANG);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST] = Region("Water Temple MQ Boss Key Room Chest", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, true),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH, []{return logic->CanHitSwitch(ED_BOMB_THROW) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT,    []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH,       []{return logic->HasItem(RG_SILVER_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && (logic->HasItem(RG_BRONZE_SCALE) || (logic->WaterTimer() >= 24 && logic->CanUse(RG_LONGSHOT))));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH] = Region("Water Temple MQ B1 Gate Switch", SCENE_WATER_TEMPLE, {
        //Events
        //If the water is low, the switch is underwater and needs irons to press, otherwise, the water is too low to climb up and you need irons to hookshot a target
        //If a glitch clips through the gate on low, have it logically press the switch and let entrance logic enter
        EventAccess(LOGIC_WATER_MQ_B1_SWITCH, []{return logic->CanUse(RG_IRON_BOOTS);}),
    }, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,                []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && (logic->WaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16));}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST, []{return logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && (logic->WaterLevel(WL_LOW) || logic->WaterTimer() >= 24);})
    });

    areaTable[RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM] = Region("Water Temple MQ Triangle Torch Room", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_1, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()) ||
                                                                         (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_2, (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()) ||
                                                                         (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_3, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_4, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_5, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_6, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,                    []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && ((logic->WaterLevel(WL_LOW) && logic->HasItem(RG_GOLDEN_SCALE)) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40 && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT))));}),
        Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_TOP_STEP, []{return logic->IsAdult || logic->CanGroundJump() || logic->CanUse(RG_HOOKSHOT);})
    });

    areaTable[RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_TOP_STEP] = Region("Water Temple MQ Triangle Torch Top Step", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_3, (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_4, (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_5, (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_6, (logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM, []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE, []{return logic->CanUse(RG_FIRE_ARROWS) &&
                                                                   (((logic->IsAdult || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump())) && logic->CanUse(RG_HOVER_BOOTS)) ||
                                                                    (logic->CanUse(RG_LONGSHOT) && AnyAgeTime([]{return logic->ScarecrowsSong();})));})
    });

    areaTable[RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE] = Region("Water Temple MQ Triangle Torch Cage", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,           logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_1,                logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_2,                logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_3,                logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_4,                logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_2, logic->CanBreakCrates()),
    }, {});

    areaTable[RR_WATER_TEMPLE_MQ_SPIKE_MOAT] = Region("Water Temple MQ Spike Moat", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_MAIN,              []{return logic->Get(LOGIC_WATER_MQ_B1_SWITCH) && ((logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24 && logic->CanUse(RG_HOOKSHOT)) || logic->HasItem(RG_BRONZE_SCALE));}),
        Entrance(RR_WATER_TEMPLE_MQ_BEHIND_SPIKE_MOAT, []{return (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS)));}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BEHIND_SPIKE_MOAT] = Region("Water Temple MQ Behind Spike Moat", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        //You can backflip over the spikes and into the water.
        Entrance(RR_WATER_TEMPLE_MQ_SPIKE_MOAT,        []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS));}),
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM] = Region("Water Temple MQ Crate Vortex Room", SCENE_WATER_TEMPLE, {},
    {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_FRONT_CRATE_1,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_FRONT_CRATE_2,     logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_1, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_2, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_3, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_4, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_5, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_6, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_BEHIND_SPIKE_MOAT, []{return true;}),
        //Child can use the crate to get the height to make it with hovers, but it's annoyingly tight so would be a trick
        Entrance(RR_WATER_TEMPLE_MQ_SCARECROW_CANAL,   []{return ((logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP))) ||
                                                                  (AnyAgeTime([]{return logic->ScarecrowsSong();}) && logic->CanUse(RG_HOOKSHOT))) &&
                                                                 (logic->IsAdult || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8));}),
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_CAGE, []{return ctx->GetTrickOption(RT_WATER_INVISIBLE_HOOKSHOT_TARGET) && logic->CanUse(logic->IsAdult ? RG_HOOKSHOT : RG_LONGSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_SCARECROW_CANAL] = Region("Water Temple MQ Scarecrow Canal", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        //making the jump as adult without jumpslash is possible, but hard enough to be a trick
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM, []{return logic->HasItem(RG_BRONZE_SCALE) ||
                                                                 (logic->IsAdult && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS))) ||
                                                                 (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_WATER_TEMPLE_MQ_CANAL_ALCOVE,      []{return logic->IsAdult;}),
        Entrance(RR_WATER_TEMPLE_MQ_BEHIND_CANAL,      []{return logic->CanUse(RG_IRON_BOOTS) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT)) && logic->WaterTimer() >= 8;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_CANAL_ALCOVE] = Region("Water Temple MQ Canal Alcove", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_3_JETS_ROOM,       []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 2);}),
        Entrance(RR_WATER_TEMPLE_MQ_SCARECROW_CANAL,   []{return logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}),
        Entrance(RR_WATER_TEMPLE_MQ_BEHIND_CANAL,      []{return logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_BEHIND_CANAL] = Region("Water Temple MQ Behind Canal", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_SCARECROW_CANAL,   []{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->WaterTimer() >= 8;}),
        Entrance(RR_WATER_TEMPLE_MQ_FREESTANDING_ROOM, []{return true;}),
    });

    areaTable[RR_WATER_TEMPLE_MQ_FREESTANDING_ROOM] = Region("Water Temple MQ Freestanding Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,       logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_POT_2,   logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_3, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_4, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_5, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_BEHIND_CANAL, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);});})
    });

    areaTable[RR_WATER_TEMPLE_MQ_3_JETS_ROOM] = Region("Water Temple MQ 3 Jets Room", SCENE_WATER_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_CANAL_ALCOVE,      []{return logic->SmallKeys(SCENE_WATER_TEMPLE, 2) && logic->CanHitSwitch(ED_BOOMERANG);}),
        Entrance(RR_WATER_TEMPLE_MQ_DODONGO_ROOM,      []{return logic->CanHitSwitch() && logic->HasFireSource();})
    });

    areaTable[RR_WATER_TEMPLE_MQ_DODONGO_ROOM] = Region("Water Temple MQ Dodongo Room", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_MINI_DODONGO_POT_1,         logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_MINI_DODONGO_POT_2,         logic->CanBreakPots()),
        LOCATION(RC_WATER_TEMPLE_MQ_DODONGO_ROOM_UPPER_CRATE,   logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DODONGO_ROOM_HALL_CRATE,    logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_1, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_2, logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_3, logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_3_JETS_ROOM,       []{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && AnyAgeTime([]{return logic->CanKillEnemy(RE_DODONGO, ED_CLOSE, true, 5);});}),
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_CAGE, []{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && AnyAgeTime([]{return logic->CanKillEnemy(RE_DODONGO, ED_CLOSE, true, 5);});})
    });

    areaTable[RR_WATER_TEMPLE_MQ_CRATE_VORTEX_CAGE] = Region("Water Temple MQ Crate Vortex Cage", SCENE_WATER_TEMPLE, {}, {
        //Locations
        LOCATION(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA,       logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA) && logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_1,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_2,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_3,  logic->CanBreakCrates()),
        LOCATION(RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_4,  logic->CanBreakCrates()),
    }, {
        //Exits
        Entrance(RR_WATER_TEMPLE_MQ_DODONGO_ROOM,      []{return true;}),
        Entrance(RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM, []{return logic->CanUse(RG_LONGSHOT) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}), // stand on crate to aim
    });

#pragma endregion

    // Boss Room
    areaTable[RR_WATER_TEMPLE_BOSS_ENTRYWAY] = Region("Water Temple Boss Entryway", SCENE_WATER_TEMPLE, {}, {}, {
        // Exits
        Entrance(RR_WATER_TEMPLE_TRAPPED_SLOPE, []{return ctx->GetDungeon(WATER_TEMPLE)->IsVanilla() && false;}),
        Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR,  []{return ctx->GetDungeon(WATER_TEMPLE)->IsMQ() && false;}),
        Entrance(RR_WATER_TEMPLE_BOSS_ROOM,     []{return logic->HasItem(RG_WATER_TEMPLE_BOSS_KEY);}),
    });

    areaTable[RR_WATER_TEMPLE_BOSS_ROOM] = Region("Water Temple Boss Room", SCENE_WATER_TEMPLE_BOSS, {
        // Events
        EventAccess(LOGIC_WATER_TEMPLE_CLEAR, []{return logic->CanKillEnemy(RE_MORPHA);}),
    }, {
        // Locations
        LOCATION(RC_WATER_TEMPLE_MORPHA_HEART, logic->Get(LOGIC_WATER_TEMPLE_CLEAR)),
        LOCATION(RC_MORPHA,                    logic->Get(LOGIC_WATER_TEMPLE_CLEAR)),
    }, {
        // Exits
        Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, []{return false;}),
        Entrance(RR_LAKE_HYLIA,                 []{return logic->Get(LOGIC_WATER_TEMPLE_CLEAR);}, false),
    });

    // clang-format on
}
