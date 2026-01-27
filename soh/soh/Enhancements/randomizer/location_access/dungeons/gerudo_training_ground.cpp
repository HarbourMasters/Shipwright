#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_GerudoTrainingGround() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_GERUDO_TRAINING_GROUND_ENTRYWAY] = Region("Gerudo Training Ground Entryway", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LOBBY,    ctx->GetDungeon(GERUDO_TRAINING_GROUND)->IsVanilla()),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY, ctx->GetDungeon(GERUDO_TRAINING_GROUND)->IsMQ()),
        ENTRANCE(RR_GF_EXITING_GTG,                  true),
    });

#pragma region Vanilla

    areaTable[RR_GERUDO_TRAINING_GROUND_LOBBY] = Region("Gerudo Training Ground Lobby", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,      logic->CanHitEyeTargets() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST,     logic->CanHitEyeTargets() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_ENTRANCE_STORMS_FAIRY, logic->CanUse(RG_SONG_OF_STORMS)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_ENTRYWAY,         true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_SAND_ROOM,        true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_DINALFOS,         true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE,     true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_SAND_ROOM] = Region("Gerudo Training Ground Sand Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LOBBY,        true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true);})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM] = Region("Gerudo Training Ground Boulder Room", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_SAND_ROOM,        true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM, AnyAgeTime([]{return logic->CanUse(logic->IsAdult ? RG_HOOKSHOT : RG_LONGSHOT) || ctx->GetTrickOption(RT_GTG_WITHOUT_HOOKSHOT);})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE] = Region("Gerudo Training Ground Central Maze", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,   logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 3) && (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || logic->HasItem(RG_CLIMB)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,  logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 4) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 6) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,  logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 7) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,  logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 9) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LOBBY,              true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE_RIGHT, logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 9)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE_RIGHT] = Region("Gerudo Training Ground Central Maze Right", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,         true),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM,             true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM_UPPER_LEDGE, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE,          logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 9)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM] = Region("Gerudo Training Ground Heavy Block Room", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_PUSHED_HEAVY_BLOCK, logic->CanUse(RG_SILVER_GAUNTLETS)),
    }, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, logic->CanKillEnemy(RE_WOLFOS, ED_CLOSE, true, 4, true) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM_UPPER, (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_GTG_FAKE_WALL) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (logic->IsAdult && logic->CanGroundJump()))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_BEHIND_HEAVY_BLOCK,     logic->Get(LOGIC_GTG_PUSHED_HEAVY_BLOCK)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM,           true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM_UPPER] = Region("Gerudo Training Ground Heavy Block Room Upper", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_UNLOCKED_DOOR_BEHIND_HEAVY_BLOCK, true),
    }, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM, ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_BEHIND_HEAVY_BLOCK] = Region("Gerudo Training Ground Behind Heavy Block", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM, logic->Get(LOGIC_GTG_PUSHED_HEAVY_BLOCK)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LIKE_LIKE_ROOM,   logic->Get(LOGIC_GTG_UNLOCKED_DOOR_BEHIND_HEAVY_BLOCK)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_LIKE_LIKE_ROOM] = Region("Gerudo Training Ground Like Like Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,  logic->CanKillEnemy(RE_LIKE_LIKE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, logic->CanKillEnemy(RE_LIKE_LIKE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,  (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanPassEnemy(RE_LIKE_LIKE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_BEHIND_HEAVY_BLOCK, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER] = Region("Gerudo Training Ground Eye Statue Upper", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_EYE_STATUE_LOWER,       true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM_UPPER, true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_ABOVE_MAZE,             logic->Get(LOGIC_GTG_CLEARED_EYE_STATUE)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_ABOVE_MAZE] = Region("Gerudo Training Ground Above Eye", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_EYE_STATUE_LOWER] = Region("Gerudo Training Ground Eye Statue Lower", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_CLEARED_EYE_STATUE, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, logic->Get(LOGIC_GTG_CLEARED_EYE_STATUE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM] = Region("Gerudo Training Ground Hammer Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,  logic->CanAttack() && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, (logic->CanUse(RG_MEGATON_HAMMER) || (logic->TakeDamage() && ctx->GetTrickOption(RT_FLAMING_CHESTS))) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_EYE_STATUE_LOWER, logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_FAIRY_BOW)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM,        true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_LAVA_ROOM] = Region("Gerudo Training Ground Lava Room", SCENE_GERUDO_TRAINING_GROUND, {
        EVENT_ACCESS(LOGIC_GTG_PLATFORM_SILVER_RUPEES, logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_DINALFOS,              true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE_RIGHT,    logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM_UPPER_LEDGE, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild || (logic->IsAdult && ctx->GetTrickOption(RT_GTG_LAVA_JUMP)) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->CanUse(RG_BOMB_BAG) && logic->TakeDamage())))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_UNDERWATER,            logic->Get(LOGIC_GTG_PLATFORM_SILVER_RUPEES)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_LAVA_ROOM_UPPER_LEDGE] = Region("Gerudo Training Ground Lava Room", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM,   logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild || (logic->IsAdult && ctx->GetTrickOption(RT_GTG_LAVA_JUMP)) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->CanUse(RG_BOMB_BAG) && logic->TakeDamage())),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_UNDERWATER] = Region("Gerudo Training Ground Underwater", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && logic->WaterTimer() >= 24 && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_DINALFOS] = Region("Gerudo Training Dinalfos", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,       logic->CanKillEnemy(RE_BEAMOS) && logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2, true) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_BEAMOS_SOUTH_HEART, true),
        LOCATION(RC_GERUDO_TRAINING_GROUND_BEAMOS_EAST_HEART,  true),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LOBBY,     true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_LAVA_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_BEAMOS) && logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2, true);})),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_LOBBY] = Region("Gerudo Training Ground MQ Lobby", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,       logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,      logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_POT_1,       logic->CanBreakPots()),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_POT_2,       logic->CanBreakPots()),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_POT_1,      logic->CanBreakPots()),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_POT_2,      logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_ENTRYWAY,            true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_BY_LOBBY,    true),
        //It's possible to use the torch in hidden room of maze with flame storage to light these
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM,        AnyAgeTime([]{return logic->HasFireSource();})),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM,    AnyAgeTime([]{return logic->CanHitEyeTargets();})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_MAZE_BY_LOBBY] = Region("Gerudo Training Ground MQ Maze By Lobby", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,  logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,   (ctx->GetTrickOption(RT_LENS_GTG_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,           true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_FIRST_LOCK, logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 1)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_MAZE_FIRST_LOCK] = Region("Gerudo Training Ground MQ Maze First Lock", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,       logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 1)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_CENTER, logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 3)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_MAZE_CENTER] = Region("Gerudo Training Ground MQ Center", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_MQ_MAZE_SWITCH, logic->CanUse(RG_MEGATON_HAMMER)),
    }, 
    {   //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_CRATE, logic->CanBreakCrates()),
    },
    {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_FIRST_LOCK, logic->SmallKeys(SCENE_GERUDO_TRAINING_GROUND, 3)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM] = Region("Gerudo Training Ground MQ Sand Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,        true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM, AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE);})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM] = Region("Gerudo Training Ground MQ Left Side", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM,    true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM, AnyAgeTime([]{return logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_GTG_MQ_WITHOUT_HOOKSHOT) || (ctx->GetTrickOption(RT_GTG_MQ_WITH_HOOKSHOT) && logic->IsAdult && logic->CanJumpslash() && logic->CanUse(RG_HOOKSHOT));})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM] = Region("Gerudo Training Ground MQ Stalfos Room", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_BLUE_FIRE_ACCESS,                     true),
        EVENT_ACCESS(LOGIC_GTG_UNLOCKED_DOOR_BEHIND_HEAVY_BLOCK, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true);})),
        EVENT_ACCESS(LOGIC_GTG_PUSHED_HEAVY_BLOCK,               logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanAvoidEnemy(RE_STALFOS, true, 2)),
    }, {
        //Locations
        //implies logic->CanKillEnemy(RE_BIG_SKULLTULA)
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM,      true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_BEHIND_BLOCK,      logic->Get(LOGIC_GTG_PUSHED_HEAVY_BLOCK)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE, logic->IsAdult && AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2, true);}) && (ctx->GetTrickOption(RT_LENS_GTG_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->BlueFire() && (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_GTG_FAKE_WALL) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (logic->IsAdult && logic->CanGroundJump()))),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_BEHIND_BLOCK] = Region("Gerudo Training Ground MQ Behind Block", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM,      logic->Get(LOGIC_GTG_PUSHED_HEAVY_BLOCK)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_ROOM_BEHIND_BLOCK, logic->Get(LOGIC_GTG_UNLOCKED_DOOR_BEHIND_HEAVY_BLOCK)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_ROOM_BEHIND_BLOCK] = Region("Gerudo Training Ground MQ Room Behind Block", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        //implies logic->CanKillEnemy(RE_SPIKE)
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST, logic->CanKillEnemy(RE_FREEZARD) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_BEHIND_BLOCK, true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE] = Region("Gerudo Training Ground MQ Statue Room Ledge", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM,      true),
        //implies dropping down to hit the switch. Using swords, especially master, is a bit awkward, may be trick worthy, but is only relevant with other tricks
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAGENTA_FIRE_ROOM, AnyAgeTime([]{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG);})),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM,       true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_MAGENTA_FIRE_ROOM] = Region("Gerudo Training Ground MQ Magenta Fire Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST, logic->Get(LOGIC_GTG_MQ_MAZE_SWITCH) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE,  true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM] = Region("Gerudo Training Ground MQ Statue Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST, logic->CanUse(RG_FAIRY_BOW) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE, logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM,   true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM] = Region("Gerudo Training Ground MQ Torch Slug Room", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        //implies logic->CanKillEnemy(RE_TORCH_SLUG)
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,        logic->CanHitSwitch(ED_BOMB_THROW) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM,  AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE);})),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_SWITCH_LEDGE, AnyAgeTime([]{return logic->CanKillEnemy(RE_IRON_KNUCKLE);})),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_SWITCH_LEDGE] = Region("Gerudo Training Ground MQ Switch Ledge", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_MQ_RIGHT_SIDE_SWITCH,   logic->CanUse(RG_MEGATON_HAMMER)),
        EVENT_ACCESS(LOGIC_GTG_PLATFORM_SILVER_RUPEES, logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_HOVER_BOOTS)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS,  logic->CanUse(RG_FIRE_ARROWS)),
        //the fire bubble here is a jerk if you are aiming for the nearest hook platform, you have to aim to the right hand side with hook to dodge it
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH, logic->CanUse(RG_LONGSHOT) || (logic->Get(LOGIC_GTG_PLATFORM_SILVER_RUPEES) && logic->CanUse(RG_HOOKSHOT)) || ((logic->CanUse(RG_FIRE_ARROWS) && logic->Get(LOGIC_GTG_PLATFORM_SILVER_RUPEES)) && logic->CanUse(RG_HOVER_BOOTS))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT,            logic->Get(LOGIC_GTG_MQ_RIGHT_SIDE_SWITCH) && logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM,       true),
    });

    //this region exists to place silver rupee items on later, normally it's all on fire and cannot be stood on without access from another area
    //This covers the 2 platforms that can be jumped to directly from RR_GERUDO_TRAINING_GROUND_MQ_SWITCH_LEDGE
    //the unshuffled rupee collection is handled by the event GTGPlatformSilverRupees
    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS] = Region("Gerudo Training Ground MQ Ledge Side Platforms", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        //This is merely to extend this region's logic if you have hovers
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_FURTHEST_PLATFORM, logic->CanUse(RG_HOVER_BOOTS)),
    });

    //this region exists to place silver rupee items on later, normally it's all on fire and cannot be stood on without access from another area
    //This covers the platform that needs hover boots or the spawned targets to reach from any starting point other than RR_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT
    //the unshuffled rupee collection is handled by the event GTGPlatformSilverRupees
    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_FURTHEST_PLATFORM] = Region("Gerudo Training Ground MQ Furthest Platform", SCENE_GERUDO_TRAINING_GROUND, {}, {}, {
        //Exits
        //This is merely to extend this region's logic if you have hovers
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS, logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH] = Region("Gerudo Training Ground MQ Platforms Unlit Torch", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_PLATFORM_SILVER_RUPEES, logic->HasFireSource() && logic->CanUse(RG_HOVER_BOOTS)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_UNDERWATER,           logic->Get(LOGIC_GTG_PLATFORM_SILVER_RUPEES)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS, logic->HasFireSource() && logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS, logic->HasFireSource() || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT,           logic->Get(LOGIC_GTG_MQ_RIGHT_SIDE_SWITCH) && (logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->HasFireSource()))),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS] = Region("Gerudo Training Ground Torch Side Platforms", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        //this torch shot is possible as child but tight and obtuse enough to be a trick
        EVENT_ACCESS(LOGIC_GTG_PLATFORM_SILVER_RUPEES, ((logic->CanUse(RG_FAIRY_BOW) && logic->IsAdult) || logic->CanUse(RG_FIRE_ARROWS)) && logic->CanUse(RG_HOVER_BOOTS)),
    }, {}, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS,  ((logic->CanUse(RG_FAIRY_BOW) && logic->IsAdult) || logic->CanUse(RG_FIRE_ARROWS)) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_GTG_LAVA_JUMP)) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->CanUse(RG_BOMB_BAG) && logic->TakeDamage()))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH, (logic->CanUse(RG_FAIRY_BOW) && logic->IsAdult) || logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT,            logic->Get(LOGIC_GTG_MQ_RIGHT_SIDE_SWITCH) && ((logic->CanUse(RG_FAIRY_BOW) && logic->IsAdult) || logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM,         true),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_UNDERWATER] = Region("Gerudo Training Ground MQ Underwater", SCENE_GERUDO_TRAINING_GROUND, {}, {
        //Locations
        //it is possible to snipe the stingers with bow or sling before dropping in, or just get really lucky, and avoid needing to take damage, but that might be trick worthy
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, (logic->HasFireSource() && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24 && logic->HasItem(RG_BRONZE_SCALE) && logic->TakeDamage()) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH, true),
    });


    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT] = Region("Gerudo Training Ground MQ Maze Right", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_GTG_PLATFORM_SILVER_RUPEES, logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_HOVER_BOOTS)),
    }, {
        //Locations
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST,    logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,                 true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS,  logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH, logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(logic->Get(LOGIC_GTG_PLATFORM_SILVER_RUPEES) ? RG_HOOKSHOT : RG_LONGSHOT) || (ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->Get(LOGIC_GTG_MQ_RIGHT_SIDE_SWITCH) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER))),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LEDGE_SIDE_PLATFORMS,  logic->CanUse(RG_FIRE_ARROWS)),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_FURTHEST_PLATFORM,     logic->CanUse(RG_FIRE_ARROWS)),
    });

    areaTable[RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM] = Region("Gerudo Training Ground MQ Dinolfos Room", SCENE_GERUDO_TRAINING_GROUND, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS, logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        //implies logic->CanKillEnemy(RE_LIZALFOS and logic->CanKillEnemy(RE_DODONGO)
        //is logic->CanKillEnemy(RE_DINOLFOS, ED_CLOSE, true, 2, true) && logic->CanKillEnemy(RE_ARMOS, ED_CLOSE, true, 1, true) broken down to exclude sticks, as it takes too many to clear the room
        //Proper enemy kill room ammo logic is needed to handle this room
        //some combinations may be impossible without taking damage, keep an eye out for issues here
        LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT)))) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,                true),
        ENTRANCE(RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS, AnyAgeTime([]{return logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT)));})),
    });

#pragma endregion
    // clang-format on
}
