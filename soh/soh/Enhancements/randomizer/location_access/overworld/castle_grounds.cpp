#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_CastleGrounds() {
    // clang-format off
    //With multi-area support {RA_CASTLE_GROUNDS} is not strictly required anymore, as any interior here could inherit both
    //{RA_HYRULE_CASTLE} and {RA_OUTSIDE_GANONS_CASTLE}, but a setting to merge the latter 2 into the former may be preferred
    //
    //Temporarily uses SCENE_OUTSIDE_GANONS_CASTLE to avoid self connection between ages
    areaTable[RR_CASTLE_GROUNDS] = Region("Castle Grounds", SCENE_OUTSIDE_GANONS_CASTLE, TIME_DOESNT_PASS, {RA_CASTLE_GROUNDS}, {}, {}, {
        //Exits
        ENTRANCE(RR_THE_MARKET,            true),
        ENTRANCE(RR_HC_GATE,               logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS, logic->IsAdult),
    });

    areaTable[RR_CASTLE_GROUNDS_FROM_GREAT_FAIRY] = Region("Castle Grounds From Great Fairy", SCENE_OUTSIDE_GANONS_CASTLE, TIME_DOESNT_PASS, {RA_CASTLE_GROUNDS}, {}, {}, {
        ENTRANCE(RR_HC_PAST_GATE,          logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS, logic->IsAdult),
    });

    areaTable[RR_HC_GATE] = Region("Hyrule Castle Gate", SCENE_HYRULE_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy() || logic->CanUse(RG_STICKS)),
        EVENT_ACCESS(LOGIC_BUG_ACCESS,   logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_HC_MALON_EGG,      logic->HasItem(RG_SPEAK_HYLIAN)),
        LOCATION(RC_HC_GS_TREE,        logic->CanBonkTrees() && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_CLOSE)),
        LOCATION(RC_HC_SKULLTULA_TREE, logic->CanBonkTrees()),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS, true),
        ENTRANCE(RR_HC_ABOVE_VINE,  logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_HC_PAST_GATE,   logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_SPEAK_HYLIAN)),
    });

    areaTable[RR_HC_ABOVE_VINE] = Region("Hyrule Castle Above Vine", SCENE_HYRULE_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_HC_MALON_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HC_MALON_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HC_MALON_GOSSIP_STONE,           true),
    }, {
        //Exits
        ENTRANCE(RR_HC_GATE,      true),
        ENTRANCE(RR_HC_PAST_GATE, true),
    });

    areaTable[RR_HC_PAST_GATE] = Region("Hyrule Castle Past Gate", SCENE_HYRULE_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CanUse(RG_STICKS)),
    }, {
        //Locations
        LOCATION(RC_HC_NEAR_GUARDS_TREE_1, logic->CanBonkTrees()),
        LOCATION(RC_HC_NEAR_GUARDS_TREE_2, logic->CanBonkTrees()),
        LOCATION(RC_HC_NEAR_GUARDS_TREE_3, logic->CanBonkTrees()),
        LOCATION(RC_HC_NEAR_GUARDS_TREE_4, logic->CanBonkTrees()),
        LOCATION(RC_HC_NEAR_GUARDS_TREE_5, logic->CanBonkTrees()),
        LOCATION(RC_HC_NEAR_GUARDS_TREE_6, logic->CanBonkTrees()),
        LOCATION(RC_HC_NL_TREE_1,          false),
        LOCATION(RC_HC_NL_TREE_2,          false),
    }, {
        //Exits
        ENTRANCE(RR_HC_GATE,                  true),
        ENTRANCE(RR_HC_ABOVE_VINE,            logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_HC_ABOVE_CLIMBABLE_ROCKS, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_HC_GREAT_FAIRY_FOUNTAIN,  logic->BlastOrSmash() && logic->CanUse(RG_CRAWL)),
    });

    areaTable[RR_HC_ABOVE_CLIMBABLE_ROCKS] = Region("Hyrule Castle Above Climbable Rocks", SCENE_HYRULE_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
    }, {
        //Locations
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE,           true),
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_HC_PAST_GATE, true),
        ENTRANCE(RR_HC_MOAT,      true),
    });

    areaTable[RR_HC_MOAT] = Region("Hyrule Castle Grounds", SCENE_HYRULE_CASTLE, {}, {
        //Locations
        LOCATION(RC_HC_GRASS_1,     logic->CanCutShrubs()),
        LOCATION(RC_HC_GRASS_2,     logic->CanCutShrubs()),
        LOCATION(RC_HC_GROTTO_TREE, logic->CanBonkTrees()),
    }, {
        //Exits
        ENTRANCE(RR_HC_GATE,          true),
        ENTRANCE(RR_HC_STORMS_GROTTO, logic->CanOpenStormsGrotto()),
        ENTRANCE(RR_HC_GARDEN,        (logic->CanUse(RG_WEIRD_EGG) && logic->HasItem(RG_POWER_BRACELET) && logic->HasItem(RG_SPEAK_HYLIAN)) || 
                                      (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->TakeDamage() && logic->HasExplosives() && logic->CanJumpslash())),
    });

    areaTable[RR_HC_DRAIN_LEDGE] = Region("Hyrule Castle Drain Ledge", SCENE_HYRULE_CASTLE, {}, {}, {
        //Exits
        ENTRANCE(RR_HC_MOAT,   true),
        ENTRANCE(RR_HC_GARDEN, logic->CanUse(RG_CRAWL)),
    });

    areaTable[RR_HC_GARDEN] = Region("HC Garden", SCENE_CASTLE_COURTYARD_ZELDA, {}, {
        //Locations
        LOCATION(RC_HC_ZELDAS_LETTER, logic->HasItem(RG_SPEAK_HYLIAN)),
        LOCATION(RC_SONG_FROM_IMPA,   logic->HasItem(RG_SPEAK_HYLIAN)),
    }, {
        //Exits
        ENTRANCE(RR_HC_DRAIN_LEDGE, true), // if this ever gets shuffled leaving garden area should come out crawlspace
    });

    areaTable[RR_HC_GREAT_FAIRY_FOUNTAIN] = Region("HC Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, {}, {
        //Locations
        LOCATION(RC_HC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS_FROM_GREAT_FAIRY, true),
    });

    areaTable[RR_HC_STORMS_GROTTO] = Region("HC Storms Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_HC_GS_STORMS_GROTTO, logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_HC_STORMS_GS)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS_FROM_GROTTO,    true),
        ENTRANCE(RR_HC_STORMS_GROTTO_BEHIND_WALLS, logic->CanBreakMudWalls()),
    });

    areaTable[RR_HC_STORMS_GROTTO_BEHIND_WALLS] = Region("HC Storms Grotto Behind Walls", SCENE_GROTTOS, {
        //Events
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->CallGossipFairy()),
        EVENT_ACCESS(LOGIC_BUG_ACCESS,   true), // wandering bugs
    }, {
        //Locations
        LOCATION(RC_HC_GS_STORMS_GROTTO,                     logic->HookshotOrBoomerang()),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE,           true),
        LOCATION(RC_HC_STORMS_GROTTO_POT_1,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_2,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_3,                  logic->CanBreakPots()),
        LOCATION(RC_HC_STORMS_GROTTO_POT_4,                  logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_HC_STORMS_GROTTO, true),
    });

    areaTable[RR_CASTLE_GROUNDS_FROM_GROTTO] = Region("Castle Grounds From Grotto", SCENE_OUTSIDE_GANONS_CASTLE, TIME_DOESNT_PASS, {RA_CASTLE_GROUNDS}, {}, {}, {
        ENTRANCE(RR_HC_MOAT,               logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS, logic->IsAdult),
    });

    areaTable[RR_GANONS_CASTLE_GROUNDS] = Region("Ganon's Castle Grounds", SCENE_OUTSIDE_GANONS_CASTLE, {
        //Events
        EVENT_ACCESS(LOGIC_BUILD_RAINBOW_BRIDGE, logic->CanBuildRainbowBridge()),
    }, {
        //Locations
        LOCATION(RC_OGC_GS, logic->CanJumpslashExceptHammer() || logic->CanUseProjectile() || (logic->CanShield() && logic->CanUse(RG_MEGATON_HAMMER)) || logic->CanUse(RG_DINS_FIRE)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS,           logic->AtNight),
        ENTRANCE(RR_OGC_GREAT_FAIRY_FOUNTAIN, logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->AtNight),
        ENTRANCE(RR_GANONS_CASTLE_LEDGE,      logic->Get(LOGIC_BUILD_RAINBOW_BRIDGE)),
    });

    areaTable[RR_OGC_GREAT_FAIRY_FOUNTAIN] = Region("OGC Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, {}, {
        //Locations
        LOCATION(RC_OGC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_CASTLE_GROUNDS_FROM_GREAT_FAIRY, true),
    });

    areaTable[RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE] = Region("Castle Grounds From Ganon's Castle", SCENE_OUTSIDE_GANONS_CASTLE, {}, {}, {
        // Exits
        ENTRANCE(RR_HC_DRAIN_LEDGE,      logic->IsChild),
        ENTRANCE(RR_GANONS_CASTLE_LEDGE, logic->IsAdult),
    });

    areaTable[RR_GANONS_CASTLE_LEDGE] = Region("Ganon's Castle Ledge", SCENE_OUTSIDE_GANONS_CASTLE, {}, {}, {
        // Exits
        ENTRANCE(RR_GANONS_CASTLE_GROUNDS,  logic->Get(LOGIC_BUILD_RAINBOW_BRIDGE)),
        ENTRANCE(RR_GANONS_CASTLE_ENTRYWAY, logic->IsAdult),
    });

    // clang-format on
}
