#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_GanonsCastle() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_GANONS_CASTLE_ENTRYWAY] = Region("Ganon's Castle Entryway", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LOBBY,               ctx->GetDungeon(GANONS_CASTLE)->IsVanilla()),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LOBBY,            ctx->GetDungeon(GANONS_CASTLE)->IsMQ()),
        ENTRANCE(RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, true),
    });

#pragma region Vanilla

    areaTable[RR_GANONS_CASTLE_LOBBY] = Region("Ganon's Castle Lobby", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        ENTRANCE(RR_GANONS_CASTLE_ENTRYWAY, true),
        ENTRANCE(RR_GANONS_CASTLE_MAIN,     true),
    });

    areaTable[RR_GANONS_CASTLE_MAIN] = Region("Ganon's Castle Main", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_SHEIK_HINT_GC, logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LOBBY,                       true),
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,    !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_FOREST_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_FROM_OPEN,        !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_FIRE_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM,  !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_WATER_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_START,          !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_SHADOW_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,    !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_SPIRIT_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM,     (!ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_LIGHT_MEDALLION)) &&
                                                               logic->CanUse(RG_GOLDEN_GAUNTLETS)),
        ENTRANCE(RR_GANONS_TOWER_ENTRYWAY,                     (logic->Get(LOGIC_FOREST_TRIAL_CLEAR) || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                               (logic->Get(LOGIC_FIRE_TRIAL_CLEAR)   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                               (logic->Get(LOGIC_WATER_TRIAL_CLEAR)  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                               (logic->Get(LOGIC_SHADOW_TRIAL_CLEAR) || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                               (logic->Get(LOGIC_SPIRIT_TRIAL_CLEAR) || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                               (logic->Get(LOGIC_LIGHT_TRIAL_CLEAR)  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped())),
        ENTRANCE(RR_GANONS_CASTLE_DEKU_SCRUBS,  ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_GANONS_CASTLE_DEKU_SCRUBS] = Region("Ganon's Castle Deku Scrubs", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,        logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,         logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
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
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, logic->CanKillEnemy(RE_WOLFOS) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN,                     true),
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM, logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_DINS_FIRE))),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle Forest Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {
        EVENT_ACCESS(LOGIC_FOREST_TRIAL_SILVER_RUPEES, logic->IsAdult || logic->CanUse(RG_HOOKSHOT)), // child can get these by voiding after switch
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,     true),
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END, true),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END] = Region("Ganon's Castle Forest Trial Beamos Room End", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM,            logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR, logic->IsAdult || logic->CanGroundJump() || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR] = Region("Ganon's Castle Forest Trial Beamos Room Final Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_END, true),
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM,      logic->Get(LOGIC_FOREST_TRIAL_SILVER_RUPEES)),
    });

    areaTable[RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Forest Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR, true),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_OPEN_DOOR] = Region("Ganon's Castle Fire Trial Open Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN, true),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_FROM_OPEN] = Region("Ganon's Castle Fire Trial From Open Door", SCENE_INSIDE_GANONS_CASTLE, {
        // backwalking hoverboots with backflip reaches silver rupee without needing str3
        EVENT_ACCESS(LOGIC_FIRE_TRIAL_SILVER_RUPEES, logic->FireTimer() >= 48 && logic->CanUse(RG_GOLDEN_GAUNTLETS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_HEART, logic->FireTimer() >= 16),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_OPEN_DOOR,   true),
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_BARRED_DOOR, logic->CanUse(RG_LONGSHOT) && logic->FireTimer() >= 16),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_FROM_BARRED] = Region("Ganon's Castle Fire Trial From Barred Door", SCENE_INSIDE_GANONS_CASTLE, {
        // backwalking hoverboots with backflip reaches silver rupee without needing str3
        EVENT_ACCESS(LOGIC_FIRE_TRIAL_SILVER_RUPEES, logic->FireTimer() >= 56 && logic->CanUse(RG_GOLDEN_GAUNTLETS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_HEART, logic->FireTimer() >= 16),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_OPEN_DOOR,   logic->CanUse(RG_LONGSHOT) && logic->FireTimer() >= 24),
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_BARRED_DOOR, true),
    });    
    
    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_BARRED_DOOR] = Region("Ganon's Castle Fire Trial Barred Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM, logic->Get(LOGIC_FIRE_TRIAL_SILVER_RUPEES)),
    });

    areaTable[RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Fire Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_FIRE_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_FIRE_TRIAL_FROM_BARRED, true),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM] = Region("Ganon's Castle Water Trial Blue Fire Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS, logic->CanClearStalagmite() || ctx->GetTrickOption(RT_ICE_STALAGMITE_CLIP)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN,                   true),
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, logic->BlueFire()),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM] = Region("Ganon's Castle Water Trial Block Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,              logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_WATER_TRIAL_RUSTED_SWITCH, logic->IsAdult && (logic->HasItem(RG_POWER_BRACELET) || (logic->CanMiddairGroundJump())) && 
                                                      (logic->BlueFire() || ctx->GetTrickOption(RT_VISIBLE_COLLISION)) && 
                                                      logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM, true),
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END, logic->IsAdult || (logic->HasItem(RG_POWER_BRACELET) && logic->CanUse(RG_HOVER_BOOTS)) || logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP)),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END] = Region("Ganon's Castle Water Trial Block Room End", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, true),
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM, logic->Get(LOGIC_WATER_TRIAL_RUSTED_SWITCH)),
    });

    areaTable[RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Water Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_WATER_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END, true),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_START] = Region("Ganon's Castle Shadow Trial Start", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST, (logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN,                       true),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_POTS_PLATFORM,  logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT)),
        // shortcut for longshot to torch, dins, longshot to like like, run to chest platform
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_CHEST_PLATFORM, logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_LONGSHOT)),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_POTS_PLATFORM] = Region("Ganon's Castle Shadow Pots Platform", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_START,          logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_CHEST_PLATFORM, logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS) || (logic->Get(LOGIC_SHADOW_TRIAL_LOWER_SWITCH) && logic->CanUse(RG_HOOKSHOT))),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_CHEST_PLATFORM] = Region("Ganon's Castle Shadow Chest Platform", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_RUSTED_SWITCH, (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_MEGATON_HAMMER)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, logic->Get(LOGIC_SHADOW_TRIAL_LOWER_SWITCH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_1,                ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_2,                ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_3,                ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_POTS_PLATFORM, logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_LOWER_SWITCH,  logic->CanUse(RG_FIRE_ARROWS) || ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_END,           ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_LOWER_SWITCH] = Region("Ganon's Castle Shadow Trial Lower Switch", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_LOWER_SWITCH, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_CHEST_PLATFORM, logic->CanUse(RG_FIRE_ARROWS) || (logic->Get(LOGIC_SHADOW_TRIAL_LOWER_SWITCH) && logic->CanUse(RG_LONGSHOT))),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_END] = Region("Ganon's Castle Shadow Trial End", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_2, logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_HEART_3, logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_CHEST_PLATFORM, (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) || (logic->CanUse(RG_HOVER_BOOTS) && logic->HasFireSource()) || (logic->Get(LOGIC_SHADOW_TRIAL_LOWER_SWITCH) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_POTS_PLATFORM,  logic->CanUse(RG_LONGSHOT) && logic->CanUse(RG_DINS_FIRE) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_LOWER_SWITCH,   logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM,     logic->Get(LOGIC_SHADOW_TRIAL_RUSTED_SWITCH)),
    });

    areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Shadow Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_3, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SHADOW_TRIAL_END, true),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle Spirit Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_HEART,     true),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN,                       true),
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH, ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH] = Region("Ganon's Castle Spirit Trial Before Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, (logic->CanJumpslash() || logic->HasExplosives() || logic->CanUse(RG_GIANTS_KNIFE)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,  true),
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH, logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)))),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH] = Region("Ganon's Castle Spirit Trial After Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, logic->CanHitSwitch() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH, true),
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM,    logic->CanUse(RG_FAIRY_BOW) && (logic->CanUse(RG_MIRROR_SHIELD) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
    });

    areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Spirit Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SPIRIT_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,         logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH, true),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM] = Region("Ganon's Castle Light Trial Chests Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanKillEnemy(RE_BIG_SKULLTULA) && logic->CanKillEnemy(RE_KEESE, ED_CLOSE, true, 3)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MAIN,                      true),
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 1)),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM] = Region("Ganon's Castle Light Trial Triforce Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM,  true),
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2)),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM] = Region("Ganon's Castle Light Trial Boulder Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_POT_1, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2)),
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM,    logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanGroundJump())),
    });

    areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle Light Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_LIGHT_TRIAL_CLEAR, (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_POT_1, logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
        LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_POT_2, logic->CanBreakPots() && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_GANONS_CASTLE_MQ_LOBBY] = Region("Ganon's Castle MQ Lobby", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_ENTRYWAY, logic->CanPassEnemy(RE_GREEN_BUBBLE) || AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);})),
        //Implies CanKillEnemy(RE_GREEN_BUBBLE)
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,  AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);})),
    });

    areaTable[RR_GANONS_CASTLE_MQ_MAIN] = Region("Ganon's Castle MQ Main", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_SHEIK_HINT_MQ_GC, logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_LOBBY,                       true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,   !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_FOREST_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_OPEN_DOOR,        !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_FIRE_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,     !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_WATER_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_SHADOW_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,    !ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) || logic->HasItem(RG_SPIRIT_MEDALLION)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,   AnyAgeTime([]{return (!ctx->GetOption(RSK_MEDALLION_LOCKED_TRIALS) ||
                                                                  logic->HasItem(RG_LIGHT_MEDALLION)) && logic->CanUse(RG_GOLDEN_GAUNTLETS);})),
        //RANDOTODO could we just set these events automatically based on the setting?
        ENTRANCE(RR_GANONS_TOWER_ENTRYWAY,                        (logic->Get(LOGIC_FOREST_TRIAL_CLEAR) || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                  (logic->Get(LOGIC_FIRE_TRIAL_CLEAR)   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                  (logic->Get(LOGIC_WATER_TRIAL_CLEAR)  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                  (logic->Get(LOGIC_SHADOW_TRIAL_CLEAR) || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                  (logic->Get(LOGIC_SPIRIT_TRIAL_CLEAR) || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                  (logic->Get(LOGIC_LIGHT_TRIAL_CLEAR)  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped())),
        ENTRANCE(RR_GANONS_CASTLE_MQ_DEKU_SCRUBS,                 ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_DEKU_SCRUBS] = Region("Ganon's Castle MQ Deku Scrubs", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, true),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
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
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Stalfos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,                     true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);})),
    });

    //If it is ever possible to warp into the RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM, this needs splitting up as the requirements to reach things from the other side are more complex
    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Beamos Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST, logic->CanHitEyeTargets()),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, logic->Get(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,       true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE] = Region("Ganon's Castle MQ Forest Trial Beamos Room End", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST, logic->CanHitEyeTargets()),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, logic->HasFireSource() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM,     logic->Get(LOGIC_FOREST_TRIAL_MQ_SPAWN_BEAMOS_CHEST) && (logic->CanAvoidEnemy(RE_BEAMOS) || logic->CanKillEnemy(RE_ARMOS)) && logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END, logic->IsAdult || logic->CanGroundJump() || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END] = Region("Ganon's Castle MQ Forest Trial Beamos Room Final Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_MIDDLE, true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM,         logic->CanUse(RG_SONG_OF_TIME)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Forest Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_OPEN_DOOR] = Region("Ganon's Castle MQ Fire Trial Open Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN, true)
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FROM_OPEN] = Region("Ganon's Castle MQ Fire Trial From Open Door", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_TRIAL_SILVER_RUPEES, logic->FireTimer() >= 72 && logic->CanUse(RG_GOLDEN_GAUNTLETS);),
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_OPEN_DOOR,   true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_BARRED_DOOR, logic->FireTimer() >= 32 && (logic->CanUse(RG_LONGSHOT) || 
                                                                                          (logic->CanUse(RG_GOLDEN_GAUNTLETS) && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_GANON_MQ_FIRE_TRIAL) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)))))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FROM_BARRED] = Region("Ganon's Castle MQ Fire Trial From Barred Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_BARRED_DOOR, true)
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_BARRED_DOOR] = Region("Ganon's Castle MQ Fire Trial Barred Door", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM, logic->Get(LOGIC_FIRE_TRIAL_SILVER_RUPEES)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Fire Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FIRE_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
        //There's no way back across the lava without glitches
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_FIRE_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_FIRE_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_OPEN_DOOR, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM] = Region("Ganon's Castle MQ Water Trial Geyser Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS,  logic->CanJumpslash() || logic->HasExplosives()), // bow can also hit at right angle
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, logic->BlueFire() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_HEART, logic->BlueFire()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,                   true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3) && AnyAgeTime([]{return logic->BlueFire();})),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM] = Region("Ganon's Castle MQ Water Trial Block Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_WATER_TRIAL_MQ_SILVER_RUPEES,             logic->IsAdult && (logic->HasItem(RG_POWER_BRACELET) || logic->CanMiddairGroundJump()) && logic->BlueFire()),
        EVENT_ACCESS(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE, (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && logic->CanUse(RG_ICE_ARROWS)) || (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS)) && logic->HasItem(RG_POWER_BRACELET) && logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,    logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END, logic->Get(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE) && (logic->IsAdult || (logic->CanUse(RG_HOVER_BOOTS) && logic->HasItem(RG_POWER_BRACELET)) || logic->CanGroundJump() || ctx->GetTrickOption(RT_SLIDE_JUMP))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END] = Region("Ganon's Castle MQ Water Trial Block Room End", SCENE_INSIDE_GANONS_CASTLE, {
        EVENT_ACCESS(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE, logic->BlueFire()),
    }, {}, {
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, logic->Get(LOGIC_WATER_TRIAL_MQ_MELTED_FINAL_DOOR_RED_ICE)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM, logic->Get(LOGIC_WATER_TRIAL_MQ_SILVER_RUPEES)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Water Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_WATER_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE] = Region("Ganon's Castle MQ Shadow Trial Starting Ledge", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_FIRST_CHEST, logic->CanUse(RG_FAIRY_BOW)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,                        true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, (logic->Get(LOGIC_SHADOW_TRIAL_FIRST_CHEST) && logic->CanUse(RG_HOOKSHOT)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Chest Platform", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_FIRST_CHEST, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, logic->Get(LOGIC_SHADOW_TRIAL_FIRST_CHEST) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE,  logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Moving Platform", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but tight, so would be a trick
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_FIRST_CHEST, logic->CanDetonateUprightBombFlower()),
    }, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH,   true),
    });


    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH] = Region("Ganon's Castle MQ Shadow Trial Beamos Torch", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but very tight, so would be a trick
        //The bow trick assumes RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE access, if you can somehow void warp directly here it will need handling properly
        //Hovers is possible as child but a bit tight, requires good rolls
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE,        logic->HasFireSource() || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL) && logic->CanUse(RG_FAIRY_BOW))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE] = Region("Ganon's Castle MQ Shadow Trial Far Side", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  logic->CanHitEyeTargets() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH, logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS)),
        //Modelling the silver rupees properly will require a way to check temp flags in different regions.
        //It may be tempting to use a Here-like command for this but it could cause sphere skipping in playthroughs
        //So a system like event access which sets based on TimeAge would be preferable, as the application of these can be tracked and accounted for, unlike Here-like commands
        //For Now I am assuming the player has made it all the way from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, which logically means every rupee is available
        //with no extra requirements except the lens logic needed to reach the door, which also enables the beamos-platform rupee
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM,   (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Shadow Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE, (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM] = Region("Ganon's Castle MQ Spirit Trial Chairs Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,                       true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, AnyAgeTime([]{return (logic->CanHitEyeTargets() || ctx->GetTrickOption(RT_VISIBLE_COLLISION)) && logic->CanUse(RG_MEGATON_HAMMER);})),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH] = Region("Ganon's Castle MQ Spirit Trial Before Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, logic->CanPassEnemy(RE_GREEN_BUBBLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,  true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, AnyAgeTime([]{return logic->CanUse(RG_BOMBCHU_5);})),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH] = Region("Ganon's Castle MQ Spirit Trial After Switch", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        //better names for these would be nice.
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, AnyAgeTime([]{return logic->CanUse(RG_BOMBCHU_5);})),
        //Sunlight arrows are bugged, should set a perm flag like mirror shield
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM,    AnyAgeTime([]{return (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD));}) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
    });

    areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Spirit Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_SPIRIT_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,         logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, true),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM] = Region("Ganon's Castle MQ Light Trial Dinolfos Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN,                      true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_DINOLFOS) && logic->CanKillEnemy(RE_TORCH_SLUG);})),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM] = Region("Ganon's Castle MQ Light Trial Triforce Room", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,      true),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT] = Region("Ganon's Castle MQ Light Trial Boulder Room Front", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM,     logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 2)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL) || (logic->IsAdult && logic->CanGroundJump())),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK] = Region("Ganon's Castle MQ Light Trial Boulder Room Back", SCENE_INSIDE_GANONS_CASTLE, {}, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LEFT_HEART,  true),
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_RIGHT_HEART, true),
    }, {
        //Exits
        //I got the trick going backwards, but only while taking damage. this isn't relevant anyway though
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL) || (logic->IsAdult && logic->CanGroundJump())),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,         logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3)),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FAKE_FINAL_ROOM] = Region("Ganon's Castle MQ Light Trial Fake Final Room", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3)),
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,        (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanJumpslash() || logic->CanUseProjectile())),
    });

    areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Light Trial Final Room", SCENE_INSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_LIGHT_TRIAL_CLEAR, logic->CanUse(RG_LIGHT_ARROWS)),
    }, {
        //Locations
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_POT_1, logic->CanBreakPots()),
        LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, logic->SmallKeys(SCENE_INSIDE_GANONS_CASTLE, 3)),
    });

#pragma endregion

#pragma region Tower and Escape

    areaTable[RR_GANONS_TOWER_ENTRYWAY] = Region("Ganon's Tower Entryway", SCENE_INSIDE_GANONS_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_LOBBY,   ctx->GetDungeon(GANONS_CASTLE)->IsVanilla()),
        ENTRANCE(RR_GANONS_CASTLE_MQ_MAIN, ctx->GetDungeon(GANONS_CASTLE)->IsMQ()),
        ENTRANCE(RR_GANONS_TOWER_STAIRS_1, true),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_1] = Region("Ganon's Tower Stairs 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_ENTRYWAY, true),
        ENTRANCE(RR_GANONS_TOWER_FLOOR_1,  true),
        // for imaginary blue warp
        ENTRANCE(RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, false),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_1] = Region("Ganon's Tower Floor 1", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_STAIRS_1, AnyAgeTime([]{return logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_GANONS_TOWER_STAIRS_2, AnyAgeTime([]{return logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_2] = Region("Ganon's Tower Stairs 2", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_FLOOR_1, true),
        ENTRANCE(RR_GANONS_TOWER_FLOOR_2, true),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_2] = Region("Ganon's Tower Floor 2", SCENE_GANONS_TOWER, {}, {
        //Locations
        LOCATION(RC_GANONS_TOWER_BOSS_KEY_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_STAIRS_2, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_GANONS_TOWER_STAIRS_3, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_3] = Region("Ganon's Tower Stairs 3", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_FLOOR_2, true),
        ENTRANCE(RR_GANONS_TOWER_FLOOR_3, true),
    });

    areaTable[RR_GANONS_TOWER_FLOOR_3] = Region("Ganon's Tower Floor 3", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_STAIRS_3, AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_GANONS_TOWER_STAIRS_4, AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_GANONS_TOWER_STAIRS_4] = Region("Ganon's Tower Stairs 4", SCENE_GANONS_TOWER, {}, {}, {
        //Exits
        ENTRANCE(RR_GANONS_TOWER_FLOOR_3,               true),
        ENTRANCE(RR_GANONS_TOWER_BEFORE_GANONDORF_LAIR, true),
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
        ENTRANCE(RR_GANONS_TOWER_FLOOR_3,        AnyAgeTime([]{return true;})),
        ENTRANCE(RR_GANONS_TOWER_GANONDORF_LAIR, AnyAgeTime([]{return logic->HasItem(RG_GANONS_CASTLE_BOSS_KEY);})),
    });

    areaTable[RR_GANONS_TOWER_GANONDORF_LAIR] = Region("Ganondorf's Lair", SCENE_GANONDORF_BOSS, {}, {
        //Locations
        LOCATION(RC_GANONDORF_HINT, logic->HasBossSoul(RG_GANON_SOUL)),
    }, {
        //Exits
        ENTRANCE(RR_GANONS_CASTLE_ESCAPE,               logic->CanKillEnemy(RE_GANONDORF)),
    });

    areaTable[RR_GANONS_CASTLE_ESCAPE] = Region("Ganon's Castle Escape", SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR, {}, {
        //Locations
        //10 pots
        //RANDOTODO hook potsanity pots up to escape.
    }, {
        //Exits
        //temporary
        ENTRANCE(RR_GANONS_CASTLE_GANON_ARENA, true),
        //real logic once we figure out how to deal with castle escape skip
        //ENTRANCE(RR_GANONS_CASTLE_GANON_ARENA, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true)),
    });

    areaTable[RR_GANONS_CASTLE_GANON_ARENA] = Region("Ganon's Arena", SCENE_GANON_BOSS, {}, {
        //Locations
        LOCATION(RC_GANON, logic->CanKillEnemy(RE_GANON)),
    }, {});

#pragma endregion
    // clang-format on
}
