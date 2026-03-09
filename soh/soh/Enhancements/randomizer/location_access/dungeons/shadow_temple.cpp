#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_ShadowTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_SHADOW_TEMPLE_ENTRYWAY] = Region("Shadow Temple Entryway", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEGINNING,    ctx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BEGINNING, ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ()),
        ENTRANCE(RR_GRAVEYARD_WARP_PAD_REGION,  true),
    });

#pragma region Vanilla

    areaTable[RR_SHADOW_TEMPLE_BEGINNING] = Region("Shadow Temple Beginning", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_ENTRYWAY,               (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_START, ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_FIRST_BEAMOS,           (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_POWER_BRACELET) && logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_SHADOW_TEMPLE_WHISPERING_WALLS_START] = Region("Shadow Temple Whispering Walls Start", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_NUT_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEGINNING,             true),
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE, ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_END,  ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    // shares RR_SHADOW_TEMPLE_WHISPERING_WALLS_START area with pots, but handles lens access for reaching door at start
    areaTable[RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE] = Region("Shadow Temple Whispering Walls Side", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_NUT_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_START,     ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE_ROOM, true),
    });

    areaTable[RR_SHADOW_TEMPLE_WHISPERING_WALLS_END] = Region("Shadow Temple Whispering Walls End", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_4, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_WHISPERING_WALLS_POT_5, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_NEAR_DEAD_HAND_POT_1,   logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_START, ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_DEAD_HAND,              true),
    });

    areaTable[RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE_ROOM] = Region("Shadow Temple Whispering Walls Side Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MAP_CHEST,       logic->CanKillEnemy(RE_REDEAD) && logic->CanKillEnemy(RE_KEESE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MAP_CHEST_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MAP_CHEST_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE, AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD) && logic->CanKillEnemy(RE_KEESE);})),
    });

    areaTable[RR_SHADOW_TEMPLE_DEAD_HAND] = Region("Shadow Temple Dead Hand", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, logic->CanKillEnemy(RE_DEAD_HAND) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_WHISPERING_WALLS_END, AnyAgeTime([]{return logic->CanKillEnemy(RE_DEAD_HAND);})),
    });

    areaTable[RR_SHADOW_TEMPLE_FIRST_BEAMOS] = Region("Shadow Temple First Beamos", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_BEAMOS_STORM_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEGINNING,            ctx->GetTrickOption(RT_VISIBLE_COLLISION) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_SHADOW_TEMPLE_COMPASS_ROOM,         true),
        ENTRANCE(RR_SHADOW_TEMPLE_SPINNING_BLADES,      true),
        ENTRANCE(RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B2, logic->HasExplosives() && logic->SmallKeys(SCENE_SHADOW_TEMPLE, 1)),
    });

    areaTable[RR_SHADOW_TEMPLE_COMPASS_ROOM] = Region("Shadow Temple Compass Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_COMPASS_CHEST, logic->CanKillEnemy(RE_GIBDO) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_FIRST_BEAMOS, AnyAgeTime([]{return logic->CanKillEnemy(RE_GIBDO);})),
    });

    areaTable[RR_SHADOW_TEMPLE_SPINNING_BLADES] = Region("Shadow Temple Spinning Blades", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, ((logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanGroundJump())) || logic->CanUse(RG_HOOKSHOT)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_FIRST_BEAMOS, true),
        ENTRANCE(RR_SHADOW_TEMPLE_DOCK,         logic->Get(LOGIC_SHADOW_SHORTCUT_BLOCK)),
    });

    areaTable[RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B2] = Region("Shadow Temple B2 to B3 Corridor B2", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_FIRST_BEAMOS,         logic->SmallKeys(SCENE_SHADOW_TEMPLE, 1)),
        ENTRANCE(RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B3, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B3] = Region("Shadow Temple B2 to B3 Corridor B3", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B2, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_HUGE_PIT,       logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        //bunnyhovers + lens lets you go from the very top of upper pit to the stationary invisible platform below quite easily
    });

    areaTable[RR_SHADOW_TEMPLE_UPPER_HUGE_PIT] = Region("Shadow Temple Upper Huge Pit", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_PIT_STORM_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B3,      logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_HUGE_PIT_DOOR_LEDGE, ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_HUGE_PIT,            logic->IsAdult || logic->CanJumpslash() || ctx->GetTrickOption(RT_SHADOW_MQ_HUGE_PIT)),
    });

    areaTable[RR_SHADOW_TEMPLE_UPPER_HUGE_PIT_DOOR_LEDGE] = Region("Shadow Temple Upper Huge Pit Door Ledge", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_HUGE_PIT,            ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPINNING_BLADES, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 2)),
    });

    areaTable[RR_SHADOW_TEMPLE_LOWER_HUGE_PIT] = Region("Shadow Temple Lower Huge Pit", SCENE_SHADOW_TEMPLE, {}, {},{
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_HUGE_PIT,            logic->IsAdult || logic->CanJumpslash()),
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_HUGE_PIT_DOOR_LEDGE, (ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_STONE_UMBRELLA,            true),
    });

    // See MQ for comments
    areaTable[RR_SHADOW_TEMPLE_STONE_UMBRELLA] = Region("Shadow Temple Stone Umbrella", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,     logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || (logic->IsAdult && ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->CanJumpslash())),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_POT_3,       logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_HUGE_PIT,       true),
        ENTRANCE(RR_SHADOW_TEMPLE_STONE_UMBRELLA_UPPER, ctx->GetTrickOption(RT_SHADOW_UMBRELLA_CLIP) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->TakeDamage()) || (logic->IsAdult && ((ctx->GetTrickOption(RT_SHADOW_UMBRELLA_HOVER) && logic->CanUse(RG_HOVER_BOOTS)) || logic->HasItem(RG_GORONS_BRACELET)))),
    });

    areaTable[RR_SHADOW_TEMPLE_STONE_UMBRELLA_UPPER] = Region("Shadow Temple Stone Umbrella Upper", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        //Assuming the known setup for RT_SHADOW_UMBRELLA_HOVER and RT_SHADOW_UMBRELLA_GS, probably possible without sword + shield.
        LOCATION(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,      ctx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanStandingShield() && logic->CanUse(RG_MASTER_SWORD)),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_POT_3,        logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_POT_4,        logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_STONE_UMBRELLA, true),
    });

    areaTable[RR_SHADOW_TEMPLE_LOWER_HUGE_PIT_DOOR_LEDGE] = Region("Shadow Temple Lower Huge Pit Door Ledge", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_HUGE_PIT,   (ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPIKES, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 2)),
    });

    areaTable[RR_SHADOW_TEMPLE_INVISIBLE_SPINNING_BLADES] = Region("Shadow Temple Invisible Spinning Blades", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   logic->CanKillEnemy(RE_LIKE_LIKE) && logic->CanKillEnemy(RE_KEESE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, logic->CanKillEnemy(RE_LIKE_LIKE) && logic->CanKillEnemy(RE_KEESE) && (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                AnyAgeTime([]{return logic->CanKillEnemy(RE_LIKE_LIKE) && logic->CanKillEnemy(RE_KEESE);}) && ((logic->IsAdult && logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)) || logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG))),
        //We cannot repeat the MQ invisible blades trick for these hearts as the like-like does not respawn if the room is cleared
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_LEFT_HEART,      (logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_RIGHT_HEART,     (logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_HUGE_PIT_DOOR_LEDGE, true),
    });

    areaTable[RR_SHADOW_TEMPLE_INVISIBLE_SPIKES] = Region("Shadow Temple Invisible Spikes", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST, logic->CanKillEnemy(RE_REDEAD) && (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->TakeDamage() || logic->CanUse(RG_GORON_TUNIC)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_HUGE_PIT_DOOR_LEDGE, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 2)),
        ENTRANCE(RR_SHADOW_TEMPLE_SKULL_JAR,                 (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))),
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPIKES_PLATFORM, (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && ((ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) ||
                                                             logic->CanUse(AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD) && (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH) || logic->TakeDamage() || logic->CanUse(RG_GORON_TUNIC));}) ? RG_HOOKSHOT : RG_LONGSHOT))),
    });

    areaTable[RR_SHADOW_TEMPLE_INVISIBLE_SPIKES_PLATFORM] = Region("Shadow Temple Invisible Spikes Platform", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPIKES,  true),
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_WIND_TUNNEL, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 3)),
    });

    areaTable[RR_SHADOW_TEMPLE_SKULL_JAR] = Region("Shadow Temple Skull Jar", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_FREESTANDING_KEY,    logic->CanUse(RG_BOMB_BAG) || logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_SHADOW_FREESTANDING_KEY) && logic->CanUse(RG_BOMBCHU_5))),
        LOCATION(RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT, logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPIKES, AnyAgeTime([]{return logic->CanKillEnemy(RE_KEESE);})),
    });

    areaTable[RR_SHADOW_TEMPLE_UPPER_WIND_TUNNEL] = Region("Shadow Temple Upper Wind Tunnel", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_INVISIBLE_SPIKES_PLATFORM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 3)),
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL,         (logic->CanUse(RG_HOVER_BOOTS) && logic->CanPassEnemy(RE_BIG_SKULLTULA)) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL] = Region("Shadow Temple Lower Wind Tunnel", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_UPPER_WIND_TUNNEL,     logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_WIND_TUNNEL_ALCOVE,    ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_WIND_TUNNEL_HINT_ROOM, true),
    });

    areaTable[RR_SHADOW_TEMPLE_WIND_TUNNEL_ALCOVE] = Region("Shadow Temple Wind Tunnel Alcove", SCENE_SHADOW_TEMPLE, {}, {}, {
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL, (ctx->GetTrickOption(RT_SHADOW_MQ_WINDY_WALKWAY)) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_SHADOW_TEMPLE_ROOM_TO_BOAT,      true),
    });

    areaTable[RR_SHADOW_TEMPLE_WIND_TUNNEL_HINT_ROOM] = Region("Shadow Temple Wind Tunnel Hint Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_WIND_HINT_CHEST,     logic->CanKillEnemy(RE_REDEAD) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_WIND_HINT_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL, logic->CanKillEnemy(RE_REDEAD)),
    });

    areaTable[RR_SHADOW_TEMPLE_ROOM_TO_BOAT] = Region("Shadow Temple Room to Boat", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  logic->CanKillEnemy(RE_GIBDO, ED_CLOSE, true, 2) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_POT_1,        logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_POT_2,        logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_WIND_TUNNEL_ALCOVE, AnyAgeTime([]{return logic->CanKillEnemy(RE_GIBDO, ED_CLOSE, true, 2);})),
        ENTRANCE(RR_SHADOW_TEMPLE_DOCK,               logic->SmallKeys(SCENE_SHADOW_TEMPLE, 4)),
    });

    areaTable[RR_SHADOW_TEMPLE_DOCK] = Region("Shadow Temple Dock", SCENE_SHADOW_TEMPLE, {
        //Event
        EVENT_ACCESS(LOGIC_SHADOW_SHORTCUT_BLOCK, logic->HasItem(RG_GORONS_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,          logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_SHADOW_TEMPLE_SCARECROW_NORTH_HEART, logic->ReachDistantScarecrow()),
        LOCATION(RC_SHADOW_TEMPLE_SCARECROW_SOUTH_HEART, logic->ReachDistantScarecrow()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_ROOM_TO_BOAT,    logic->SmallKeys(SCENE_SHADOW_TEMPLE, 4)),
        ENTRANCE(RR_SHADOW_TEMPLE_SPINNING_BLADES, logic->Get(LOGIC_SHADOW_SHORTCUT_BLOCK) && logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_SHADOW_TEMPLE_BEYOND_BOAT,     ((logic->IsAdult && ((logic->HasItem(RG_GORONS_BRACELET) && logic->HasItem(RG_CLIMB)) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))) || (ctx->GetTrickOption(RT_HOOKSHOT_LADDERS) && logic->CanUse(RG_HOOKSHOT))) && logic->CanUse(RG_ZELDAS_LULLABY)),
    });

    areaTable[RR_SHADOW_TEMPLE_BEYOND_BOAT] = Region("Shadow Temple Beyond Boat", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED, logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5))),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_AFTER_BOAT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_BOAT_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MAZE,         true),
        // a precise recoil hover back flip can get across too, but isn't a simple hover boost
        ENTRANCE(RR_SHADOW_TEMPLE_ACROSS_CHASM, logic->Get(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED) || logic->ReachDistantScarecrow()),
    });

    areaTable[RR_SHADOW_TEMPLE_ACROSS_CHASM] = Region("Shadow Temple Across Chasm", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED, logic->CanDetonateUprightBombFlower()),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_AFTER_BOAT_POT_3,             logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_BOAT_POT_4,             logic->CanBreakPots()),
        // don't actually need to use hookshot extension
        LOCATION(RC_SHADOW_TEMPLE_AFTER_SHIP_UPPER_LEFT_HEART,  ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME) ? logic->ReachScarecrow() : logic->ReachDistantScarecrow()),
        LOCATION(RC_SHADOW_TEMPLE_AFTER_SHIP_UPPER_RIGHT_HEART, ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME) ? logic->ReachScarecrow() : logic->ReachDistantScarecrow()),
        // can reach with logic->IsAdult && logic->ReachDistantScarecrow() && logic->CanJumpslash(), but precise enough to be trick
        LOCATION(RC_SHADOW_TEMPLE_AFTER_SHIP_LOWER_HEART,       (logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME)) || (logic->ReachDistantScarecrow() && logic->CanUse(RG_HOVER_BOOTS))),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEYOND_BOAT,   (logic->Get(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED) && logic->IsAdult) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_SHADOW_TEMPLE_PRE_BOSS_ROOM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 5)),
    });

    areaTable[RR_SHADOW_TEMPLE_MAZE] = Region("Shadow Temple Maze", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEYOND_BOAT,      true),
        ENTRANCE(RR_SHADOW_TEMPLE_X_CROSS,          true),
        ENTRANCE(RR_SHADOW_TEMPLE_THREE_SKULL_JARS, true),
        ENTRANCE(RR_SHADOW_TEMPLE_WOODEN_SPIKES,    true),
    });

    areaTable[RR_SHADOW_TEMPLE_X_CROSS] = Region("Shadow Temple X-Cross", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanKillEnemy(RE_FLOORMASTER) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_FLOORMASTER_POT_1,           logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_FLOORMASTER_POT_2,           logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MAZE, AnyAgeTime([]{return (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanKillEnemy(RE_FLOORMASTER);})),
    });

    areaTable[RR_SHADOW_TEMPLE_THREE_SKULL_JARS] = Region("Shadow Temple Three Skull Jars", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT, logic->HasItem(RG_GORONS_BRACELET) || logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_SHORT_JUMPSLASH)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MAZE, true),
    });

    areaTable[RR_SHADOW_TEMPLE_WOODEN_SPIKES] = Region("Shadow Temple Wooden Spikes", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,       logic->CanUse(RG_DINS_FIRE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,               logic->CanUse(RG_DINS_FIRE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_SPIKE_WALLS_POT_1,            logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MAZE, true),
    });

    areaTable[RR_SHADOW_TEMPLE_PRE_BOSS_ROOM] = Region("Shadow Temple Pre Boss Room", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BEYOND_BOAT, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 5)),
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_DOOR,   (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_SHADOW_TEMPLE_BOSS_DOOR] = Region("Shadow Temple Boss Door", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_PRE_BOSS_ROOM, (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_SHADOW_TEMPLE_MQ_BEGINNING] = Region("Shadow Temple MQ Beginning", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_ENTRYWAY,         (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM,  logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM] = Region("Shadow Temple MQ Spinner Room", SCENE_SHADOW_TEMPLE, {}, 
    {
        // Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_TRUTH_SPINNER_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_TRUTH_SPINNER_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_TRUTH_SPINNER_SMALL_CRATE_3, logic->CanBreakSmallCrates()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_TRUTH_SPINNER_SMALL_CRATE_4, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_ENTRYWAY,                  true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,           AnyAgeTime([]{return logic->HasItem(RG_POWER_BRACELET) && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)));}) && (logic->CanUse(RG_HOVER_BOOTS) || AnyAgeTime([]{return logic->CanUse(RG_FIRE_ARROWS);}) || (ctx->GetTrickOption(RT_SHADOW_MQ_GAP) && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslashExceptHammer()))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START, AnyAgeTime([]{return logic->HasExplosives();}) && logic->SmallKeys(SCENE_SHADOW_TEMPLE, 6) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START] = Region("Shadow Temple MQ Whispering Walls Start", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM,          true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE, ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_END,  (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME))),
    });

    // shares RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START area with pots, but handles lens access for reaching door at start
    areaTable[RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE] = Region("Shadow Temple MQ Whispering Walls Side", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START,        ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE_ROOM, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_END] = Region("Shadow Temple MQ Whispering Walls End", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START,     (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME))),
        //There's a shared flag tied to some glass here. eye target here and killing an enemy group later in the dungeon toggles. I'm building the logic as "intended", assuming the switch needs flipping
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_DEAD_HAND, logic->CanHitEyeTargets()),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE_ROOM] = Region("Shadow Temple MQ Whispering Walls Redeads", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,         logic->CanKillEnemy(RE_REDEAD) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_ENTRANCE_REDEAD_POT_1, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_ENTRANCE_REDEAD_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE, AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD);})),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_DEAD_HAND] = Region("Shadow Temple MQ Whispering Walls Dead Hand", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, logic->CanKillEnemy(RE_DEAD_HAND) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_END, AnyAgeTime([]{return logic->CanKillEnemy(RE_DEAD_HAND);})),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Region("Shadow Temple MQ First Beamos", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEAMOS_STORM_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM,           ctx->GetTrickOption(RT_VISIBLE_COLLISION) && (logic->CanUse(RG_HOVER_BOOTS) || logic->HasFireSource())),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_GIBDO_ROOM,          true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B2,   logic->HasExplosives() && logic->SmallKeys(SCENE_SHADOW_TEMPLE, 2)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_B2_GIBDO_ROOM] = Region("Shadow Temple MQ B2 Gibdo Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        //Doing this sets the shared flag for the glass in RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS, but doesn't seem to affect the chest
        LOCATION(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST, logic->CanKillEnemy(RE_GIBDO) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,  AnyAgeTime([]{return logic->CanKillEnemy(RE_GIBDO);})),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM] = Region("Shadow Temple MQ B2 Spinning Blade Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_MAP_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanGroundJump()))) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,  AnyAgeTime([]{return logic->CanKillEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)));})),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH] = Region("Shadow Temple MQ Shortcut Path", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_DOCK,                   logic->Get(LOGIC_SHADOW_SHORTCUT_BLOCK)),
        //WARNING if there's any way past here to ship without already reaching the other side the key logic in this dungeon becomes Quantum
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B2] = Region("Shadow Temple MQ B2 to B3 Corridor B2", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   logic->SmallKeys(SCENE_SHADOW_TEMPLE, 2)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B3] = Region("Shadow Temple MQ B2 to B3 Corridor B3", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B3, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,       true),
        //bunnyhovers + lens lets you go from the very top of upper pit to the stationary invisible platform below quite easily
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Region("Shadow Temple MQ Upper Huge Pit", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_MQ_PIT_STAIRS, logic->HasFireSource()),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_PIT_STORM_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT_DOOR_LEDGE, ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,            (logic->Get(LOGIC_SHADOW_MQ_PIT_STAIRS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))) || ctx->GetTrickOption(RT_SHADOW_MQ_HUGE_PIT)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT_DOOR_LEDGE] = Region("Shadow Temple MQ Upper Huge Pit Door Ledge", SCENE_SHADOW_TEMPLE, {}, {}, {
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,        ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM] = Region("Shadow Temple MQ Invisible Blades Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        //RT_SHADOW_MQ_INVISIBLE_BLADES does not work with NL as like-likes will not swallow you, likewise like-likes will not spit you with a fairy revive
        //you take half a heart base from a spit out, double check EffectiveHealth when damage logic gets reworked
        //Child is too small to get hit by the blades doesn't need the trick or lens for dodging them
        LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1)) &&
                                                                                                          (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) || logic->IsChild || logic->CanUse(RG_NAYRUS_LOVE) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1)) &&
                                                                                                          ((ctx->GetTrickOption(RT_LENS_SHADOW_MQ) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) || logic->IsChild || logic->CanUse(RG_NAYRUS_LOVE))) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_LEFT_HEART,      (logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1) || logic->CanUse(RG_BOOMERANG)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_RIGHT_HEART,     (logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1) || logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Region("Shadow Temple MQ Lower Huge Pit", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, logic->CanUse(RG_LONGSHOT) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B3,      logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,            logic->Get(LOGIC_SHADOW_MQ_PIT_STAIRS)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT_DOOR_LEDGE, logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_STONE_UMBRELLA_ROOM,       AnyAgeTime([]{return logic->CanJumpslash() || logic->HasExplosives() || logic->CanUse(RG_GIANTS_KNIFE) || (ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)));})),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT_DOOR_LEDGE] = Region("Shadow Temple MQ Upper Huge Pit Door Ledge", SCENE_SHADOW_TEMPLE, {}, {}, {
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,    logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM) || logic->CanUse(RG_LENS_OF_TRUTH)) && ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 3)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_STONE_UMBRELLA_ROOM] = Region("Shadow Temple MQ Stone Umbrella Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,     logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) || (logic->IsAdult && ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->CanJumpslash())),
        LOCATION(RC_SHADOW_TEMPLE_MQ_LOWER_UMBRELLA_WEST_POT,    logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_LOWER_UMBRELLA_EAST_POT,    logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_UPPER_UMBRELLA_SOUTH_POT,   logic->CanUse(RG_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,       AnyAgeTime([]{return ctx->GetTrickOption(RT_VISIBLE_COLLISION) || logic->CanHitSwitch();})),
        //Assuming the known setup for RT_SHADOW_UMBRELLA, probably possible without sword + shield
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_STONE_UMBRELLA, ctx->GetTrickOption(RT_SHADOW_UMBRELLA_CLIP) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->TakeDamage()) || (logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_SHADOW_UMBRELLA_HOVER) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanStandingShield() && logic->CanUse(RG_MASTER_SWORD))))),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_STONE_UMBRELLA] = Region("Shadow Temple MQ Upper Stone Umbrella", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        //Assuming the known setup for RT_SHADOW_UMBRELLA_HOVER and RT_SHADOW_UMBRELLA_GS, probably possible without sword + shield.
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      ctx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanStandingShield() && logic->CanUse(RG_MASTER_SWORD)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_UPPER_UMBRELLA_NORTH_POT,    logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_UPPER_UMBRELLA_SOUTH_POT,    logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_STONE_UMBRELLA_ROOM, true),
    });

    //while the spikes here are annoying, they don't really stop you doing anything, so I'll assume either lens trick, lens to see them, or taking damage from them. Not hovers though as a new player won't see the threat without lens to react properly
    areaTable[RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM] = Region("Shadow Temple MQ Floor Spikes Room", SCENE_SHADOW_TEMPLE, {
        //Events                                        //lens or trick is always required for hookshot targets. We handle it here to not complicate the RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_UPPER_DOOR logic
        EVENT_ACCESS(LOGIC_SHADOW_MQ_FLOOR_SPIKES_RUPEES, (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                          //Upper door side high rupee needs (hookshot and redead kill(as either age) for chest and adult) or longshot. hovers can cross from the left side with a backflip but that would be a trick
                                                          //East midair rupee needs (hookshot and(hover boots or jumpslash from the upper door platform)) or longshot.
                                                          //Combined these are longshot or (IsAdult && hookshot && (CanJumpslash || (Hover Boots && Here(CanKillRedeads))))
                                                          (logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->CanJumpslash() || (logic->CanUse(RG_HOVER_BOOTS) && AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD);}))))) &&
                                                          //1 rupee is in spikes, needs hovers or damage
                                                          (logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_GORON_TUNIC))),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, logic->CanKillEnemy(RE_REDEAD) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->TakeDamage() || logic->CanUse(RG_LENS_OF_TRUTH) || logic->CanUse(RG_GORON_TUNIC)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,        logic->SmallKeys(SCENE_SHADOW_TEMPLE, 3)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM,          logic->Get(LOGIC_SHADOW_MQ_FLOOR_SPIKES_RUPEES)),
        //We need to assume we can get here with or without the glass platforms
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_PLATFORM, ((logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->Get(LOGIC_SHADOW_MQ_FLOOR_SPIKES_RUPEES) || AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD);})))) && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS))) ||
                                                            ((ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump() && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_PLATFORM] = Region("Shadow Temple MQ Floor Spikes Platform", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_WIND_TUNNEL, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 4)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM] = Region("Shadow Temple MQ Stalfos Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);})),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_WIND_TUNNEL] = Region("Shadow Temple MQ Upper Wind Tunnel", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 4)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL, (logic->CanUse(RG_HOVER_BOOTS) && logic->CanPassEnemy(RE_BIG_SKULLTULA)) || logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL] = Region("Shadow Temple MQ Lower Wind Tunnel", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_UPPER_WIND_TUNNEL,  logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM,     true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL_ALCOVE, ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM] = Region("Shadow Temple MQ Wind Hint Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,     (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanPassEnemy(RE_REDEAD) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,   logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_WIND_HINT_SUN_FAIRY, logic->CanUse(RG_SUNS_SONG)),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL_ALCOVE] = Region("Shadow Temple MQ Wind Tunnel Alcove", SCENE_SHADOW_TEMPLE, {}, {}, {
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL, (ctx->GetTrickOption(RT_SHADOW_MQ_WINDY_WALKWAY)) || logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM,     true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM] = Region("Shadow Temple MQ B4 Gibdo Room", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_NUT_ACCESS, logic->CanBreakPots()),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  logic->CanKillEnemy(RE_GIBDO) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, logic->HasExplosives() && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           logic->HasExplosives()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEFORE_BOAT_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEFORE_BOAT_POT_2,       logic->CanBreakPots()),
    }, {
        //Exits
        //child can make it using the wind strat
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL_ALCOVE, true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_DOCK,               logic->SmallKeys(SCENE_SHADOW_TEMPLE, 5)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_DOCK] = Region("Shadow Temple MQ Dock", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_SHORTCUT_BLOCK, logic->HasItem(RG_GORONS_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_SCARECROW_NORTH_HEART, logic->ReachDistantScarecrow()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_SCARECROW_SOUTH_HEART, logic->ReachDistantScarecrow()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH, logic->Get(LOGIC_SHADOW_SHORTCUT_BLOCK) && logic->HasItem(RG_CLIMB)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 5)),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,   ((logic->IsAdult && ((logic->HasItem(RG_GORONS_BRACELET) && logic->HasItem(RG_CLIMB)) || ctx->GetTrickOption(RT_UNINTUITIVE_JUMPS))) || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_ZELDAS_LULLABY)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Region("Shadow Temple MQ Beyond Boat", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED, logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5))),
    }, {
        //Locations
        //It's a trick on N64 to kill this and drop down to collect this with normal weapons, as doing so without the statue being dropped voids you to before the boat
        //hilariously, you can hit this with a pot before you bring down statue, but there's no great way to reset it without crossing. the statue's collision is very inconvenient afterwards
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,         logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEFORE_CHASM_WEST_POT, logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_BEFORE_CHASM_EAST_POT, logic->CanBreakPots()),
    }, {
        //Exits
        // a precise recoil hover back flip can get across too, but isn't a simple hover boost
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM,   logic->Get(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED) || (logic->Get(LOGIC_SHADOW_MQ_SWITCH_ACROSS_CHASM) && logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, logic->Get(LOGIC_SHADOW_MQ_SWITCH_ACROSS_CHASM)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM] = Region("Shadow Temple MQ Across Chasm", SCENE_SHADOW_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED, logic->CanDetonateUprightBombFlower()),
        EVENT_ACCESS(LOGIC_SHADOW_MQ_EYE_SWITCH_ACROSS_CHASM, logic->CanHitEyeTargets() && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_HOOKSHOT_EXTENSION))),
        EVENT_ACCESS(LOGIC_SHADOW_MQ_SWITCH_ACROSS_CHASM,     logic->Get(LOGIC_SHADOW_MQ_EYE_SWITCH_ACROSS_CHASM) && logic->CanUse(RG_LONGSHOT)),
    }, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_CHASM_WEST_POT,         logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_CHASM_EAST_POT,         logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_SHIP_UPPER_LEFT_HEART,  logic->Get(LOGIC_SHADOW_MQ_EYE_SWITCH_ACROSS_CHASM) && logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_SHIP_UPPER_RIGHT_HEART, logic->Get(LOGIC_SHADOW_MQ_EYE_SWITCH_ACROSS_CHASM) && logic->CanUse(RG_LONGSHOT)),
        //There's invisible floor collision that makes aiming for the heart with rang harder than it should be, so it's a trick.
        LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_SHIP_LOWER_HEART,       logic->IsAdult),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,    (logic->Get(LOGIC_SHADOW_BRIDGE_BEYOND_BOAT_LOWERED) && logic->IsAdult) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_PRE_BOSS_ROOM,  true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_PRE_BOSS_ROOM] = Region("Shadow Temple MQ Pre Boss Room", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM, true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BOSS_DOOR,    logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_BOSS_DOOR] = Region("Shadow Temple MQ Boss Door", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        //you can drop onto this and the respawn is reasonable
        LOCATION(RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,  (logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW) || logic->CanUse(RG_MEGATON_HAMMER)) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_PRE_BOSS_ROOM, logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,    true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Region("Shadow Temple MQ Invisible Maze", SCENE_SHADOW_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,      true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_X_CROSS,          true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_THREE_SKULL_JARS, true),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM, logic->SmallKeys(SCENE_SHADOW_TEMPLE, 6)),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_X_CROSS] = Region("Shadow Temple MQ X-Cross", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        //don't use CanDetonateUprightBombFlower as blue fire logic would need to account for player having multiple bottles & taking damage multiple times
        LOCATION(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, (logic->CanUse(RG_LENS_OF_TRUTH) || ctx->GetTrickOption(RT_LENS_SHADOW_MQ_DEADHAND)) && logic->CanKillEnemy(RE_DEAD_HAND) && (logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_DEAD_HAND_POT_1,   logic->CanBreakPots()),
        LOCATION(RC_SHADOW_TEMPLE_MQ_DEAD_HAND_POT_2,   logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_THREE_SKULL_JARS] = Region("Shadow Temple MQ Three Skull Jars", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY, true),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, true),
    });

    areaTable[RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM] = Region("Shadow Temple MQ Spike Walls Room", SCENE_SHADOW_TEMPLE, {}, {
        //Locations
        LOCATION(RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, logic->CanUse(RG_DINS_FIRE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         logic->CanUse(RG_DINS_FIRE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_SHADOW_TEMPLE_MQ_SPIKE_BARICADE_POT,     logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,  logic->SmallKeys(SCENE_SHADOW_TEMPLE, 6) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_SHADOW_TEMPLE_BOSS_ENTRYWAY] = Region("Shadow Temple Boss Entryway", SCENE_SHADOW_TEMPLE, {}, {}, {
        // Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_DOOR,    ctx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && false),
        ENTRANCE(RR_SHADOW_TEMPLE_MQ_BOSS_DOOR, ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ() && false),
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_ROOM,    logic->HasItem(RG_SHADOW_TEMPLE_BOSS_KEY)),
    });

    areaTable[RR_SHADOW_TEMPLE_BOSS_ROOM] = Region("Shadow Temple Boss Room", SCENE_SHADOW_TEMPLE_BOSS, {
        // Events
        EVENT_ACCESS(LOGIC_SHADOW_TEMPLE_CLEAR, logic->CanKillEnemy(RE_BONGO_BONGO)),
    }, {
        // Locations
        LOCATION(RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, logic->Get(LOGIC_SHADOW_TEMPLE_CLEAR)),
        LOCATION(RC_BONGO_BONGO,                     logic->Get(LOGIC_SHADOW_TEMPLE_CLEAR)),
    }, {
        // Exits
        ENTRANCE(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, false),
        ENTRANCE(RR_GRAVEYARD_WARP_PAD_REGION,   logic->Get(LOGIC_SHADOW_TEMPLE_CLEAR), false),
    });

    // clang-format on
}
