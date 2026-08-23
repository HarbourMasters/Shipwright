#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

/* Notes for calculating timers:
    - RT_DMC_BOLERO_JUMP takes about 16 seconds to perform.
    - RT_DMC_BOULDER_SKIP assumes a setup that takes 14 seconds to fully execute
    - Spawning and killing the bean skull takes around 10 seconds, as does getting the bean sprout,
      as does getting the rupees on the near plank platform but as it's child only does not stack with
   RT_DMC_BOLERO_JUMP or RT_DMC_BOULDER_SKIP
    - All child only checks must be possible with 3 hearts at the default damage multiplier, or minimal item pool
   breaks.
*/
void RegionTable_Init_DeathMountainCrater() {
    // clang-format off
    // The ledge you arrive on from the summit, and the top of the ladder down into the crater
    areaTable[RR_DMC_UPPER] = Region("DMC Upper", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_CRATE,        ROUTE(true, Cost().Heat(0))),
        ENTRANCE_ROUTES(RR_DMC_ROCK_GROTTO,  ROUTE(true, Cost().Heat(8))),
        ENTRANCE_ROUTES(RR_DMC_CRACKED_WALL, ROUTE(true, Cost().Heat(16))),
        ENTRANCE_ROUTES(RR_DMC_SCRUB,        ROUTE(true, Cost().Heat(16))),
        //Over the crater to the warp pad with the distant scarecrow
        ENTRANCE_ROUTES(RR_DMC_PAD,          ROUTE(logic->IsAdult && ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->ReachDistantScarecrow(), logic->HitCost().Heat(32))),
    });

    areaTable[RR_DMC_CRATE] = Region("DMC Crate", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_GS_CRATE, logic->IsChild && logic->CanKillEnemy(RE_GOLD_SKULLTULA) && logic->CanBreakCrates()),
        LOCATION(RC_DMC_CRATE,    logic->IsChild && logic->CanBreakCrates()),
    }, {
        //Exits
        ENTRANCE(RR_DEATH_MOUNTAIN_SUMMIT, true),
        ENTRANCE_ROUTES(RR_DMC_UPPER, ROUTE(true, Cost().Heat(0))),
    });

    areaTable[RR_DMC_ROCK_GROTTO] = Region("DMC Rock Grotto", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        //You can also walk off the edge at a shallow angle to not grab the wall, then drift to land in the alcove.
        LOCATION_ROUTES(RC_DMC_WALL_FREESTANDING_POH, ROUTE(logic->HasItem(RG_CLIMB) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->CanJumpslash()), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_1, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_2, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_3, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_4, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_5, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_6, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_7, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_CIRCLE_ROCK_8, ROUTE(logic->CanBreakRocks(), Cost().Heat(8))),
        //Boulders 1 and 2 are a bit separate, but are in 8 seconds from upper entry and closer or the same distance
        //from all ways to reach upper grotto otherwise, so it works
        LOCATION_ROUTES(RC_DMC_BOULDER_1,     ROUTE(logic->BlastOrSmash(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_BOULDER_2,     ROUTE(logic->BlastOrSmash(), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_BOULDER_3,     ROUTE(logic->BlastOrSmash(), Cost().Heat(8))),
    }, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_UPPER,        ROUTE(true, Cost().Heat(8))),
        ENTRANCE_ROUTES(RR_DMC_UPPER_GROTTO, ROUTE(AnyAgeTime([]{return logic->BlastOrSmash();}), Cost().Heat(8))),
        //Jumping down onto the distant platform, which is a one way trip
        ENTRANCE_ROUTES(RR_DMC_FAR_PLATFORM, ROUTE(ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS), logic->HitCost().Heat(16))),
    });

    areaTable[RR_DMC_CRACKED_WALL] = Region("DMC Cracked Wall", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        FAIRY_REFILL(logic->HasExplosives() && logic->CallGossipFairyExceptSuns()),
    }, {
        //Locations
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY,     logic->CallGossipFairyExceptSuns() && logic->HasExplosives()),
        LOCATION(RC_DMC_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS) && logic->HasExplosives()),
        LOCATION(RC_DMC_GOSSIP_ROCK_1,          logic->IsChild),
        LOCATION(RC_DMC_GOSSIP_ROCK_2,          logic->IsChild),
        LOCATION(RC_DMC_GOSSIP_STONE,           logic->HasExplosives()),
    }, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_UPPER, ROUTE(true, Cost().Heat(16))),
    });

    areaTable[RR_DMC_SCRUB] = Region("DMC Scrub", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_DMC_BOULDER, logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && ctx->GetTrickOption(RT_DMC_BOULDER_JS)),
    }, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB, logic->IsChild && logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
    }, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_UPPER,   ROUTE(logic->CanClimbLadder(), Cost().Heat(16))),
        //Past the boulder onto the bridge, implied hookshot use to cross
        ENTRANCE_ROUTES(RR_DMC_BLOCKED, ROUTE(logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && (logic->Get(LOGIC_DMC_BOULDER) || ctx->GetTrickOption(RT_DMC_BOULDER_SKIP))), Cost().Heat(8))),
    });

    areaTable[RR_DMC_BLOCKED] = Region("DMC Blocked Exit", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        //All boulders here are only a perm flag for adult.
        EVENT_ACCESS(LOGIC_DMC_BOULDER, logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
    }, {}, {
        //Exits
        ENTRANCE(RR_DMC_GREAT_FAIRY_FOUNTAIN, logic->CanUse(RG_MEGATON_HAMMER)),
        //Dropping back down past the boulder
        ENTRANCE_ROUTES(RR_DMC_SCRUB, ROUTE(true, Cost().Heat(8))),
        ENTRANCE_ROUTES(RR_DMC_POTS,  ROUTE(true, Cost().Heat(8))),
    });

    areaTable[RR_DMC_POTS] = Region("DMC Pots", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        // Locations
        LOCATION(RC_DMC_NEAR_GC_POT_1,           logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_2,           logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_3,           logic->CanBreakPots()),
        LOCATION(RC_DMC_NEAR_GC_POT_4,           logic->CanBreakPots()),
        LOCATION(RC_DMC_BRONZE_BOULDER_1,        logic->CanUse(RG_MEGATON_HAMMER)),
        LOCATION(RC_DMC_BRONZE_BOULDER_2,        logic->CanUse(RG_MEGATON_HAMMER)),
        LOCATION(RC_DMC_BRONZE_BOULDER_3,        logic->CanUse(RG_MEGATON_HAMMER)),
        LOCATION(RC_DMC_BRONZE_BOULDER_SHORTCUT, logic->CanUse(RG_MEGATON_HAMMER)),
        LOCATION(RC_DMC_BRIDGE_EXIT_ARROW_SIGN,  logic->CanRead()),
        //Hovering out over the lava to the two hearts, which needs the longshot to get back up to the wall one
        LOCATION_ROUTES(RC_DMC_VOLCANO_FREESTANDING_POH, ROUTE(ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_WALL_FREESTANDING_POH,    ROUTE(ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT) &&
                                                           (logic->HasItem(RG_CLIMB) || (ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS) && logic->CanJumpslash())), Cost().Heat(24))),
    }, {
        //Exits
        ENTRANCE(RR_GC_DARUNIAS_CHAMBER, true),
        //Hovering out over the lava and pulling up to the rim with the longshot
        ENTRANCE_ROUTES(RR_DMC_ROCK_GROTTO, ROUTE(ctx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT), Cost().Heat(24))),
        ENTRANCE_ROUTES(RR_DMC_BLOCKED,    ROUTE(true, Cost().Heat(8))),
        ENTRANCE_ROUTES(RR_DMC_POT_GROTTO, ROUTE(true, Cost().Heat(8))),
        ENTRANCE_ROUTES(RR_DMC_PAD,        ROUTE(logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT) ||
                                              (logic->IsAdult && logic->CanShield() && ctx->GetTrickOption(RT_DMC_BOLERO_JUMP) && logic->CanUse(RG_POWER_BRACELET)), Cost().Heat(16))),
    });

    areaTable[RR_DMC_POT_GROTTO] = Region("DMC Pot Grotto", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        //Perm flag only as Adult
        ENTRANCE(RR_DMC_SCRUB_GROTTO, logic->CanUse(RG_MEGATON_HAMMER)),
        ENTRANCE_ROUTES(RR_DMC_POTS, ROUTE(true, Cost().Heat(8))),
    });

    areaTable[RR_DMC_PAD] = Region("DMC Pad", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_CENTRAL,      ROUTE(true, Cost().Heat(16))),
        ENTRANCE_ROUTES(RR_DMC_POTS,         ROUTE((logic->CanUse(RG_HOVER_BOOTS) && (logic->IsAdult || logic->HasItem(RG_CLIMB))) || logic->CanUse(RG_HOOKSHOT), Cost().Heat(16))),
        ENTRANCE_ROUTES(RR_DMC_TEMPLE,       ROUTE(logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF), Cost().Heat(16))),
        ENTRANCE_ROUTES(RR_DMC_FAR_PLATFORM, ROUTE(logic->IsAdult && logic->ReachDistantScarecrow(), Cost().Heat(16))),
    });

    areaTable[RR_DMC_CENTRAL] = Region("DMC Central", SCENE_DEATH_MOUNTAIN_CRATER, {
        //Events
        EVENT_ACCESS(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN, CanPlantBean(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL)),
        FAIRY_REFILL(logic->IsChild && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Locations
        LOCATION(RC_SHEIK_IN_CRATER,                    logic->IsAdult),
        LOCATION(RC_DMC_GS_BEAN_PATCH,                  logic->CanSpawnSoilSkull(RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) && logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
        LOCATION(RC_DMC_NEAR_PLATFORM_RED_RUPEE,        logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_RED_RUPEE,      logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_1,   logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_2,   logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_3,   logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_4,   logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_5,   logic->IsChild),
        LOCATION(RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_6,   logic->IsChild),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_1,            logic->IsChild && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_2,            logic->IsChild && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_BEAN_SPROUT_FAIRY_3,            logic->IsChild && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_ROCK_BY_FIRE_TEMPLE_1,          logic->IsAdult),
        LOCATION(RC_DMC_ROCK_BY_FIRE_TEMPLE_2,          logic->IsAdult),
        LOCATION(RC_DMC_ROCK_BY_FIRE_TEMPLE_3,          logic->IsAdult),
        LOCATION(RC_DMC_ROCK_BY_FIRE_TEMPLE_4,          logic->IsAdult),
        LOCATION(RC_DMC_ROCK_BY_FIRE_TEMPLE_5,          logic->IsAdult),
        // RANDOTODO: A number of tricks to reach this: sidehop jumpslash or hookshot + jumpslash from bridge platform, chu+shield damage boost
        LOCATION(RC_DMC_WONDER_BENEATH_BRIDGE_PLATFORM, logic->IsAdult && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS))),
        //Riding the bean over the crater, dropping off at either heart on the way
        LOCATION_ROUTES(RC_DMC_VOLCANO_FREESTANDING_POH, ROUTE(logic->IsAdult && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN), Cost().Heat(8))),
        LOCATION_ROUTES(RC_DMC_WALL_FREESTANDING_POH,    ROUTE(logic->IsAdult && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN), Cost().Heat(8))),
    }, {
        //Exits
        ENTRANCE_ROUTES(RR_DMC_PAD, ROUTE(true, Cost().Heat(16))),
        //The bean ride ends up on the rim by the rock circle
        ENTRANCE_ROUTES(RR_DMC_ROCK_GROTTO, ROUTE(logic->IsAdult && logic->BeanPlanted(LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN), Cost().Heat(8))),
    });

    areaTable[RR_DMC_FAR_PLATFORM] = Region("DMC Far Platform", SCENE_DEATH_MOUNTAIN_CRATER, {}, {
        //Locations
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_1, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_2, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_3, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_4, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_5, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_6, logic->IsAdult),
        LOCATION(RC_DMC_DISTANT_PLATFORM_RED_RUPEE,     logic->IsAdult),
    }, {});

    areaTable[RR_DMC_TEMPLE] = Region("DMC Temple Exit", SCENE_DEATH_MOUNTAIN_CRATER, {}, {}, {
        //Exits
        ENTRANCE(RR_FIRE_TEMPLE_ENTRYWAY, true),
        //Climbing back up to the warp pad
        ENTRANCE_ROUTES(RR_DMC_PAD, ROUTE(logic->HasItem(RG_CLIMB) && (logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)), Cost().Heat(32))),
    });

    areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Region("DMC Great Fairy Fountain", SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, {}, {
        //Locations
        LOCATION(RC_DMC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_BLOCKED, true),
    });

    areaTable[RR_DMC_UPPER_GROTTO] = Region("DMC Upper Grotto", SCENE_GROTTOS, grottoEvents, {
        //Locations
        LOCATION(RC_DMC_UPPER_GROTTO_CHEST,                   logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_DMC_UPPER_GROTTO_FISH,                    logic->HasBottle()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY,      logic->CallGossipFairy()),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG,  logic->CanUse(RG_SONG_OF_STORMS)),
        LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE,            true),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT,            logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT,           logic->CanBreakLowerBeehives()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_1,                 logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_2,                 logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_3,                 logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_GROTTO_GRASS_4,                 logic->CanCutShrubs()),
        LOCATION(RC_DMC_UPPER_BOULDER_GROTTO_BUTTERFLY_FAIRY, logic->CanUse(RG_STICKS)),
    }, {
        //Exits
        ENTRANCE(RR_DMC_ROCK_GROTTO, true),
    });

    areaTable[RR_DMC_SCRUB_GROTTO] = Region("DMC Scrub Grotto", SCENE_GROTTOS, {}, {
        //Locations
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku() && logic->HasItem(RG_SPEAK_DEKU) && GetCheckPrice() <= GetWalletCapacity()),
        LOCATION(RC_DMC_HAMMER_GROTTO_BEEHIVE,    logic->CanBreakUpperBeehives()),
    }, {
        //Exits
        ENTRANCE(RR_DMC_POT_GROTTO, true),
    });

    // clang-format on
}
