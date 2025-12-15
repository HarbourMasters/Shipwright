#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_GanonsCastle() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_GANONS_CASTLE_ENTRYWAY] = Region("Ganon's Castle Entryway", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_LOBBY,               []{return ctx->GetDungeon(GANONS_CASTLE)->IsVanilla();}),
        Entrance(RR_GANONS_CASTLE_MQ_LOBBY,            []{return ctx->GetDungeon(GANONS_CASTLE)->IsMQ();}),
        Entrance(RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_GANONS_CASTLE_LOBBY] = Region("Ganon's Castle Lobby", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        Entrance(RR_GANONS_CASTLE_ENTRYWAY, []{return true;}),
        Entrance(RR_GANONS_CASTLE_MAIN,     []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MAIN] = Region("Ganon's Castle Main", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_SHEIK_HINT_GC, true),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_LOBBY,                       []{return true;}),
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,    []{return true;}),
        Entrance(RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM,        []{return true;}),
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM,  []{return true;}),
        Entrance(RR_GANONS_CASTLE_SHADOW_TRIAL_START,          []{return true;}),
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,    []{return true;}),
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM,     []{return logic->CanUse(RG_GOLDEN_GAUNTLETS);}),
        Entrance(RR_GANONS_TOWER_ENTRYWAY,                     []{return (logic->Get(LOGIC_FOREST_TRIAL_CLEAR) || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_FIRE_TRIAL_CLEAR)   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                         (logic->Get(LOGIC_WATER_TRIAL_CLEAR)  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                         (logic->Get(LOGIC_SHADOW_TRIAL_CLEAR) || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_SPIRIT_TRIAL_CLEAR) || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_LIGHT_TRIAL_CLEAR)  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped());}),
        Entrance(RR_GANONS_CASTLE_DEKU_SCRUBS,  []{return ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH);}),
    });

    areaTable[RR_GANONS_CASTLE_DEKU_SCRUBS] = Region("Ganon's Castle Deku Scrubs", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,        logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,         logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_1,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_2,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_3,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_4,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_5,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_6,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_7,          true),
        LOCATION(RC_GANONS_CASTLE_SCRUBS_FAIRY_8,          true),
    }, {});

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM] = Region("Ganon's Castle Forest Trial Wolfos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, logic->CanKillEnemy(RE_WOLFOS)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                     []{return true;}),
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM, []{return logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_DINS_FIRE));}),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle Forest Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {
        EventAccess(LOGIC_FOREST_TRIAL_SILVER_RUPEES, []{return logic->IsAdult || logic->CanUse(RG_HOOKSHOT);}),
    }, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,     []{return true;}),
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END] = Region("Ganon's Castle Forest Trial Beamos Room End", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM,            []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR, []{return logic->IsAdult || logic->CanGroundJump();}), // child can do unintuitive jump
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR] = Region("Ganon's Castle Forest Trial Beamos Room Final Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END, []{return true;}),
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM,      []{return logic->Get(LOGIC_FOREST_TRIAL_SILVER_RUPEES);}), // child needs to either ground jump or do unintuitive jump
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Forest Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FOREST_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM] = Region("Ganon's Castle Fire Trial Main Room", SCENE_INSIDE_GANONS_CASTLE, {
        // backwalking hoverboots with backflip reaches silver rupee without needing str3
        EventAccess(LOGIC_FIRE_TRIAL_SILVER_RUPEES, []{return logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_HEART, logic->CanUse(RG_GORON_TUNIC)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                     []{return true;}),
        Entrance(RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM_END, []{return logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM_END] = Region("Ganon's Castle Fire Trial Main Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM,  []{return logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM, []{return logic->Get(LOGIC_FIRE_TRIAL_SILVER_RUPEES);}),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Fire Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FIRE_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_FIRE_TRIAL_MAIN_ROOM_END, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM] = Region("Ganon's Castle Water Trial Blue Fire Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  true),
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, true),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                   []{return true;}),
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, []{return logic->BlueFire();}),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM] = Region("Ganon's Castle Water Trial Block Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS,              []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_WATER_TRIAL_RUSTED_SWITCH, []{return (logic->BlueFire() || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM, []{return true;}),
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END, []{return logic->IsAdult || logic->CanGroundJump();}),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END] = Region("Ganon's Castle Water Trial Block Room End", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, []{return true;}),
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM, []{return logic->Get(LOGIC_WATER_TRIAL_RUSTED_SWITCH);}),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Water Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_WATER_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_START] = Region("Ganon's Castle Shadow Trial Main Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,            logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_DINS_FIRE)))),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_1,                  logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_2,                  logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_1,                (logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_DINS_FIRE)))) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_2,                (logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_DINS_FIRE)))) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG))),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_3,                (logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_DINS_FIRE)))) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                    []{return true;}),
        Entrance(RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM, []{return logic->CanUse(RG_MEGATON_HAMMER) && ((logic->CanUse(RG_FIRE_ARROWS) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_DINS_FIRE) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))))));}),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Shadow Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SHADOW_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_3, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_SHADOW_TRIAL_START, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle Spirit Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_SUN_FAIRY,            logic->CanUse(RG_SUNS_SONG)),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_HEART,                true),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                       []{return true;}),
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH, []{return ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH] = Region("Ganon's Castle Spirit Trial Before Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, logic->CanJumpslashExceptHammer() || logic->HasExplosives()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,  []{return true;}),
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH, []{return logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)));}),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH] = Region("Ganon's Castle Spirit Trial After Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, logic->CanHitSwitch()),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH, []{return true;}),
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM,    []{return logic->CanUse(RG_FAIRY_BOW) && (logic->CanUse(RG_MIRROR_SHIELD) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)));}),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Spirit Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SPIRIT_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
        EventAccess(LOGIC_NUT_ACCESS,         []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM] = Region("Ganon's Castle Light Trial Chests Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       true),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanKillEnemy(RE_BIG_SKULLTULA) && logic->CanKillEnemy(RE_KEESE, ED_CLOSE, true, 3))),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MAIN,                      []{return true;}),
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 1);}),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM] = Region("Ganon's Castle Light Trial Triforce Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM,  []{return true;}),
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2);}),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM] = Region("Ganon's Castle Light Trial Boulder Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_POT_1, logic->CanBreakPots()),
    }, {
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM, []{return true;}),
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM,    []{return (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanGroundJump())) && logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2);}),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Light Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_LIGHT_TRIAL_CLEAR, []{return (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_POT_1, logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_POT_2, logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2);}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_GANONS_CASTLE_MQ_LOBBY] = Region("Ganon's Castle MQ Lobby", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_ENTRYWAY, []{return logic->CanPassEnemy(RE_GREEN_BUBBLE) || Here(RR_GANONS_CASTLE_MQ_LOBBY, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);});}),
        //Implies CanKillEnemy(RE_GREEN_BUBBLE)
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,  []{return Here(RR_GANONS_CASTLE_MQ_LOBBY, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_MAIN] = Region("Ganon's Castle MQ Main", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_SHEIK_HINT_MQ_GC,  true),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_LOBBY,                       []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,   []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM,        []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,     []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,    []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,   []{return Here(RR_GANONS_CASTLE_MQ_MAIN, []{return logic->CanUse(RG_GOLDEN_GAUNTLETS);});}),
        //RANDOTODO could we just set these events automatically based on the setting?
        Entrance(RR_GANONS_TOWER_ENTRYWAY,                        []{return (logic->Get(LOGIC_FOREST_TRIAL_CLEAR) || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_FIRE_TRIAL_CLEAR)   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                         (logic->Get(LOGIC_WATER_TRIAL_CLEAR)  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                         (logic->Get(LOGIC_SHADOW_TRIAL_CLEAR) || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_SPIRIT_TRIAL_CLEAR) || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                         (logic->Get(LOGIC_LIGHT_TRIAL_CLEAR)  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped());}),
        Entrance(RR_GANONS_CASTLE_MQ_DEKU_SCRUBS,                 []{return ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_DEKU_SCRUBS] = Region("Ganon's Castle MQ Deku Scrubs", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        logic->CanStunDeku()),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_1,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_2,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_3,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_4,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_5,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_6,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_7,          true),
        LOCATION(RC_GANONS_CASTLE_MQ_SCRUBS_FAIRY_8,          true),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Stalfos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                     []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, []{return Here(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);});}),
    });

    //If it is ever possible to warp into the RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM, this needs splitting up as the requirements to reach things from the other side are more complex
    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST, []{return logic->CanHitEyeTargets();})
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, logic->Get(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,       []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE] = Region("Ganon's Castle MQ Forest Trial Beamos Room End", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST, []{return logic->CanHitEyeTargets();})
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, logic->HasFireSource()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM,     []{return logic->Get(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST) && (logic->CanAvoidEnemy(RE_BEAMOS) || logic->CanKillEnemy(RE_ARMOS)) && logic->CanUse(RG_LONGSHOT);}),
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END, []{return logic->IsAdult || logic->CanGroundJump();}), // child can do unintuitive jump
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END] = Region("Ganon's Castle MQ Forest Trial Beamos Room Final Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE, []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM,         []{return logic->CanUse(RG_SONG_OF_TIME);}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Forest Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FOREST_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM] = Region("Ganon's Castle MQ Fire Trial Main Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                  []{return true;}),
                                                                                                                                    //2 checks, 1 for the rupees, 1 for actually making it, as the rupees are permanent but throwing a pillar is not
        Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM, []{return Here(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM, []{return logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS);}) && logic->CanUse(RG_GORON_TUNIC) && (logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_GOLDEN_GAUNTLETS) && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_GANON_MQ_FIRE_TRIAL) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)))));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Fire Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_FIRE_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
        //There's no way back across the lava without glitches
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FIRE_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_FIRE_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM] = Region("Ganon's Castle MQ Water Trial Geyser Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_BLUE_FIRE_ACCESS,  []{return logic->CanJumpslash() || logic->HasExplosives();}), // bow can also hit at right angle
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, logic->BlueFire()),
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_HEART, logic->BlueFire()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                   []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3) && Here(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM, []{return logic->BlueFire();});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM] = Region("Ganon's Castle MQ Water Trial Block Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_WATER_TRIAL_MQ_SILVER_RUPEES,             []{return logic->IsAdult && logic->BlueFire();}),
        EventAccess(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE, []{return logic->IsAdult && logic->BlueFire();}),
    }, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,    []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3);}),
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END, []{return logic->Get(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE) && (logic->IsAdult || logic->CanGroundJump());}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END] = Region("Ganon's Castle MQ Water Trial Block Room End", SCENE_INSIDE_GANONS_CASTLE, {
        EventAccess(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE, []{return logic->BlueFire();}),
    }, {}, {
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, []{return logic->Get(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE);}),
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM, []{return logic->Get(LOGIC_WATER_TRIAL_MQ_SILVER_RUPEES);}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Water Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_WATER_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, []{return Here(RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM, []{return logic->BlueFire();});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE] = Region("Ganon's Castle MQ Shadow Trial Starting Ledge", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SHADOW_TRIAL_FIRST_CHEST, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                        []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, []{return (logic->Get(LOGIC_SHADOW_TRIAL_FIRST_CHEST) && logic->CanUse(RG_HOOKSHOT)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Chest Platform", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SHADOW_TRIAL_FIRST_CHEST, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, logic->Get(LOGIC_SHADOW_TRIAL_FIRST_CHEST)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE,  []{return logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}),
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, []{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Moving Platform", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but tight, so would be a trick
        EventAccess(LOGIC_SHADOW_TRIAL_FIRST_CHEST, []{return logic->CanDetonateUprightBombFlower();}),
    }, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, []{return logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH,   []{return true;}),
    });


    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH] = Region("Ganon's Castle MQ Shadow Trial Beamos Torch", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, []{return ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}),
        //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but very tight, so would be a trick
        //The bow trick assumes RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE access, if you can somehow void warp directly here it will need handling properly
        //Hovers is possible as child but a bit tight, requires good rolls
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE,        []{return logic->HasFireSource() || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL) && logic->CanUse(RG_FAIRY_BOW));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE] = Region("Ganon's Castle MQ Shadow Trial Far Side", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  logic->CanHitEyeTargets()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH, []{return logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS);}),
        //Modelling the silver rupees properly will require a way to check temp flags in different regions.
        //It may be tempting to use a Here-like command for this but it could cause sphere skipping in playthroughs
        //So a system like event access which sets based on TimeAge would be preferable, as the application of these can be tracked and accounted for, unlike Here-like commands
        //For Now I am assuming the player has made it all the way from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, which logically means every rupee is available
        //with no extra requirements except the lens logic needed to reach the door, which also enables the beamos-platform rupee
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM,   []{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Shadow Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SHADOW_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE, []{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM] = Region("Ganon's Castle MQ Spirit Trial Chairs Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                       []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, []{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM, []{return (logic->CanHitEyeTargets() || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER);});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH] = Region("Ganon's Castle MQ Spirit Trial Before Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, logic->CanPassEnemy(RE_GREEN_BUBBLE)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,  []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, []{return logic->CanUse(RG_BOMBCHU_5);});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH] = Region("Ganon's Castle MQ Spirit Trial After Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        //better names for these would be nice.
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, []{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return logic->CanUse(RG_BOMBCHU_5);});}),
        //Sunlight arrows are bugged, should set a perm flag like mirror shield
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM,    []{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD));}) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Spirit Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_SPIRIT_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
        EventAccess(LOGIC_NUT_ACCESS,         []{return logic->CanBreakPots();}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return true;}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM] = Region("Ganon's Castle MQ Light Trial Dinolfos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_MAIN,                      []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM, []{return Here(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM, []{return logic->CanKillEnemy(RE_DINOLFOS) && logic->CanKillEnemy(RE_TORCH_SLUG);});}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM] = Region("Ganon's Castle MQ Light Trial Triforce Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,      []{return true;}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2);}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT] = Region("Ganon's Castle MQ Light Trial Boulder Room Front", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM,     []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2);}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, []{return logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL) || (logic->IsAdult && logic->CanGroundJump());}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK] = Region("Ganon's Castle MQ Light Trial Boulder Room Back", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LEFT_HEART,  true),
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_RIGHT_HEART, true),
    }, {
        //Exits
        //I got the trick going backwards, but only while taking damage. this isn't relevant anyway though
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, []{return logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL) || (logic->IsAdult && logic->CanGroundJump());}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,         []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3);}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FAKE_FINAL_ROOM] = Region("Ganon's Castle MQ Light Trial Fake Final Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3);}),
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,        []{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanJumpslash() || logic->CanUseProjectile());}),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Light Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EventAccess(LOGIC_LIGHT_TRIAL_CLEAR, []{return logic->CanUse(RG_LIGHT_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, []{return logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3);}),
    });

#pragma endregion

#pragma region Tower and Escape

    areaTable[RR_GANONS_TOWER_ENTRYWAY] = Region("Ganon's Tower Entryway", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        Entrance(RR_GANONS_CASTLE_LOBBY,   []{return ctx->GetDungeon(GANONS_CASTLE)->IsVanilla();}),
        Entrance(RR_GANONS_CASTLE_MQ_MAIN, []{return ctx->GetDungeon(GANONS_CASTLE)->IsMQ();}),
        Entrance(RR_GANONS_TOWER_STAIRS_1, []{return true;}),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_1] = Region("Ganon's Tower Stairs 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_ENTRYWAY, []{return true;}),
        Entrance(RR_GANONS_TOWER_FLOOR_1,  []{return true;}),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_1] = Region("Ganon's Tower Floor 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_STAIRS_1, []{return Here(RR_GANONS_TOWER_FLOOR_1, []{return logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2);});}),
        Entrance(RR_GANONS_TOWER_STAIRS_2, []{return Here(RR_GANONS_TOWER_FLOOR_1, []{return logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2);});}),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_2] = Region("Ganon's Tower Stairs 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_FLOOR_1, []{return true;}),
        Entrance(RR_GANONS_TOWER_FLOOR_2, []{return true;}),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_2] = Region("Ganon's Tower Floor 2", SCENE_GANONS_TOWER, {}, {
        //Locations
        LOCATION(RC_GANONS_TOWER_BOSS_KEY_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)),
    }, {
        //Exits
        Entrance(RR_GANONS_TOWER_STAIRS_2, []{return Here(RR_GANONS_TOWER_FLOOR_2, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);});}),
        Entrance(RR_GANONS_TOWER_STAIRS_3, []{return Here(RR_GANONS_TOWER_FLOOR_2, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);});}),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_3] = Region("Ganon's Tower Stairs 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_FLOOR_2, []{return true;}),
        Entrance(RR_GANONS_TOWER_FLOOR_3, []{return true;}),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_3] = Region("Ganon's Tower Floor 3", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_STAIRS_3, []{return Here(RR_GANONS_TOWER_FLOOR_3, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE, ED_CLOSE, true, 2);});}),
        Entrance(RR_GANONS_TOWER_STAIRS_4, []{return Here(RR_GANONS_TOWER_FLOOR_3, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE, ED_CLOSE, true, 2);});}),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_4] = Region("Ganon's Tower Stairs 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        Entrance(RR_GANONS_TOWER_FLOOR_3,               []{return true;}),
        Entrance(RR_GANONS_TOWER_BEFORE_GANONDORF_LAIR, []{return true;}),
    });

    areaTable[RR_GANONS_TOWER_BEFORE_GANONDORF_LAIR] = Region("Ganon's Tower Before Ganondorf's Lair", SCENE_GANONS_TOWER, {}, {
        // Locations
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_1,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_2,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_3,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_4,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_5,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_6,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_7,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_8,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_9,  logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_10, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_11, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_12, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_13, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_14, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_15, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_16, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_17, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_GANONS_TOWER_POT_18, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_GANONS_TOWER_FLOOR_3,        []{return Here(RR_GANONS_TOWER_BEFORE_GANONDORF_LAIR, []{return true;});}),
        Entrance(RR_GANONS_TOWER_GANONDORF_LAIR, []{return Here(RR_GANONS_TOWER_BEFORE_GANONDORF_LAIR, []{return logic->HasItem(RG_GANONS_CASTLE_BOSS_KEY);});}),
    });

    areaTable[RR_GANONS_TOWER_GANONDORF_LAIR] = Region("Ganondorf's Lair", SCENE_GANONDORF_BOSS, {}, {
        //Locations
        LOCATION(RC_GANONDORF_HINT, logic->HasBossSoul(RG_GANON_SOUL)),
    }, {
        //Exits
        Entrance(RR_GANONS_CASTLE_ESCAPE, []{return logic->CanKillEnemy(RE_GANONDORF);}),
    });

    areaTable[RR_GANONS_CASTLE_ESCAPE] = Region("Ganon's Castle Escape", SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR, {}, {
        //Locations
        //10 pots
        //RANDOTODO hook potsanity pots up to escape.
    }, {
        //Exits
        //temporary
        Entrance(RR_GANONS_CASTLE_GANON_ARENA, []{return true;}),
        //real logic once we figure out how to deal with castle escape skip
        //Entrance(RR_GANONS_CASTLE_GANON_ARENA, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true);}),
    });

    areaTable[RR_GANONS_CASTLE_GANON_ARENA] = Region("Ganon's Arena", SCENE_GANON_BOSS, {}, {
        //Locations
        LOCATION(RC_GANON, logic->CanKillEnemy(RE_GANON)),
    }, {});

#pragma endregion
    // clang-format on
}
