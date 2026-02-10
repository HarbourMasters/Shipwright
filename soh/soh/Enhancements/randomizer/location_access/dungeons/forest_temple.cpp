#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

// Poe sisters: Joelle is red, Beth is blue, Amy is green, Meg is purple

void RegionTable_Init_ForestTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_FOREST_TEMPLE_ENTRYWAY] = Region("Forest Temple Entryway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_TREES,      ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_TREES,   ctx->GetDungeon(FOREST_TEMPLE)->IsMQ()),
        ENTRANCE(RR_SACRED_FOREST_MEADOW,     true),
    });

#pragma region Vanilla

    areaTable[RR_FOREST_TEMPLE_TREES] = Region("Forest Temple Trees", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)) && ((logic->IsAdult && logic->CanUse(RG_BOMB_BAG)) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOMBCHU_5) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_FOREST_FIRST_GS) && (logic->CanJumpslashExceptHammer() || (logic->IsChild && logic->CanUse(RG_BOMB_BAG)))))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_ENTRYWAY,                true),
        ENTRANCE(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER, true),
    });

    areaTable[RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER] = Region("Forest Temple Overgrown Hallway Lower", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_TREES,                   true),
        ENTRANCE(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER] = Region("Forest Temple Overgrown Hallway Upper", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,                   true),
    });

    areaTable[RR_FOREST_TEMPLE_LOBBY] = Region("Forest Temple Lobby", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MEG, logic->Get(LOGIC_FOREST_JOELLE) && logic->Get(LOGIC_FOREST_BETH) && logic->Get(LOGIC_FOREST_AMY) && logic->CanKillEnemy(RE_MEG)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LOBBY,    logic->HookshotOrBoomerang()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_4, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_5, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOBBY_POT_6, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER, true),
        ENTRANCE(RR_FOREST_TEMPLE_NORTH_HALLWAY,           true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER,      logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,      logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY,     logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY,    false),
        ENTRANCE(RR_FOREST_TEMPLE_BASEMENT,                logic->Get(LOGIC_FOREST_MEG)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,           false),
    });

    areaTable[RR_FOREST_TEMPLE_NORTH_HALLWAY] = Region("Forest Temple North Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,         true),
        ENTRANCE(RR_FOREST_TEMPLE_LOWER_STALFOS, true),
    });

    areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Region("Forest Temple Lower Stalfos", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,                         logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NORTH_HALLWAY, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH)),
    });

    areaTable[RR_FOREST_TEMPLE_NW_COURTYARD_LOWER] = Region("Forest Temple NW Courtyard Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,                     true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,        (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives())) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->IsAdult && logic->CanGroundJump()))),
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER_ALCOVE, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_SEWER,                     logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_DRAINED_SEWER,             logic->Get(LOGIC_FOREST_DRAINED_WELL)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,             false),
    });

    areaTable[RR_FOREST_TEMPLE_NW_COURTYARD_UPPER_ALCOVE] = Region("Forest Temple NW Courtyard Upper Alcove", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, true),
        ENTRANCE(RR_FOREST_TEMPLE_MAP_ROOM,           true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER)),
    });

    areaTable[RR_FOREST_TEMPLE_NW_COURTYARD_UPPER] = Region("Forest Temple NW Courtyard Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->HookshotOrBoomerang()),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     true),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER,               true),
        ENTRANCE(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST,             true),
        ENTRANCE(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,                 true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, true),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_LOWER] = Region("Forest Temple NE Courtyard Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_FOREST_COURTYARD_EAST_GS) && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,               true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_UPPER,  logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_CLIMB))),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_SEWER,               (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16),
        ENTRANCE(RR_FOREST_TEMPLE_DRAINED_SEWER,       logic->Get(LOGIC_FOREST_DRAINED_WELL)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_UPPER] = Region("Forest Temple NE Courtyard Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,               logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,                 logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_FOREST_DRAINED_WELL,        true),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,     true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND,    logic->IsAdult && ctx->GetTrickOption(RT_FOREST_COURTYARD_LEDGE) && logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_DOORFRAME, logic->CanHammerRecoilHover() || ((ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash()))),
        ENTRANCE(RR_FOREST_TEMPLE_MAP_ROOM,               true),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_DOORFRAME] = Region("Forest Temple NE Courtyard Doorframe", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_SUMMON_NE_SCARECROW, logic->ScarecrowsSong()),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,  true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, logic->CanHammerRecoilHover()),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND] = Region("Forest Temple NE Courtyard Island", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,           true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE, logic->Get(LOGIC_FOREST_SUMMON_NE_SCARECROW) && logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE] = Region("Forest Temple NE Courtyard Scarecrow Ledge", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_SUMMON_NE_SCARECROW, logic->ScarecrowsSong()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_DINS_FIRE) || logic->HasExplosives()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, true),
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,        true),
    });

    areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Region("Forest Temple Map Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MAP_CHEST, logic->CanKillEnemy(RE_BLUE_BUBBLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_UPPER, AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
    });

    areaTable[RR_FOREST_TEMPLE_SEWER] = Region("Forest Temple Sewer", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER, logic->HasItem(RG_BRONZE_SCALE)),
    });

    areaTable[RR_FOREST_TEMPLE_DRAINED_SEWER] = Region("Forest Temple Drained Well", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER, logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Region("Forest Temple Below Boss Key Chest", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
    });

    areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Region("Forest Temple Floormaster Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, logic->CanDamage() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, true),
    });

    areaTable[RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY] = Region("Forest Temple West Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,                       logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
        ENTRANCE(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT] = Region("Forest Temple West Hallway Doormat", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_FLOOR,    true),
    });

    areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_FLOOR] = Region("Forest Temple Lower Block Push Floor", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT,      true),
        ENTRANCE(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM,            logic->HasItem(RG_CLIMB) || (logic->IsAdult && logic->CanGroundJump())),
    });

    areaTable[RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM] = Region("Forest Temple Lower Block Push Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_FLOOR,                 true),
        ENTRANCE(RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM,           logic->HasItem(RG_CLIMB) && logic->HasItem(RG_GORONS_BRACELET)),
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM,            logic->IsAdult && logic->CanGroundJump() && logic->HasItem(RG_GORONS_BRACELET) && logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM] = Region("Forest Temple Middle Block Push Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, logic->CanHitEyeTargets() && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM,            true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && logic->CanJumpslashExceptHammer()),
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM,            logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET)),
    });

    areaTable[RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM] = Region("Forest Temple Upper Block Push Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM, true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_TOP,    logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_TOP] = Region("Forest Temple Block Push Room Top", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM,   true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED,      logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
        ENTRANCE(RR_FOREST_TEMPLE_NW_HALLWAY_STRAIGHTENED, logic->CanHitEyeTargets() && logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
    });

    areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE] = Region("Forest Temple Block Push Room Courtyard Alcove", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM, true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,    true),
    });

    areaTable[RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED] = Region("Forest Temple NW Hallway Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_TOP, logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
        ENTRANCE(RR_FOREST_TEMPLE_RED_POE_ROOM,        logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
    });

    areaTable[RR_FOREST_TEMPLE_NW_HALLWAY_STRAIGHTENED] = Region("Forest Temple NW Hallway Straightened", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_TOP,  logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
    });

    areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Region("Forest Temple Red Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_JOELLE, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RED_POE_CHEST, logic->Get(LOGIC_FOREST_JOELLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_STALFOS,       true),
    });

    areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Region("Forest Temple Upper Stalfos", SCENE_FOREST_TEMPLE, {
        //Events
        //technically happens in RR_FOREST_TEMPLE_LOWER_STALFOS, but the way this room blocks the hole means it cannot be logical to do anything else there.
        EVENT_ACCESS(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOW_CHEST,           logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_RED_POE_ROOM,  logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_POE_ROOM, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Region("Forest Temple Blue Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_BETH, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_CHEST, logic->Get(LOGIC_FOREST_BETH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_STALFOS,            true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED] = Region("Forest Temple NE Hallway Straightened", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
        ENTRANCE(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 5)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_HALLWAY_TWISTED] = Region("Forest Temple NE Hallway Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 5)),
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,    true),
    });

    areaTable[RR_FOREST_TEMPLE_FROZEN_EYE_ROOM] = Region("Forest Temple Frozen Eye Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FROZEN_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_FROZEN_EYE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 5)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_HALLWAY_TWISTED,      logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE))),
    });

    areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Region("Forest Temple Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE, true),
        ENTRANCE(RR_FOREST_TEMPLE_GREEN_POE_ROOM,               true),
    });

    areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Region("Forest Temple Green Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_AMY, logic->CanUse(RG_FAIRY_BOW) && logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,                 true),
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT, logic->Get(LOGIC_FOREST_AMY)),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT] = Region("Forest Temple Blue Doormat Hallway Doormat", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_GREEN_POE_ROOM,       true),
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY] = Region("Forest Temple Blue Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,                        true),
    });

    areaTable[RR_FOREST_TEMPLE_BASEMENT] = Region("Forest Temple Basement", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_OPEN_BOSS_HALLWAY, logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BASEMENT_CHEST, logic->HasItem(RG_POWER_BRACELET) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_GS_BASEMENT,    logic->HasItem(RG_POWER_BRACELET) && logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,         true),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY)),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_FOREST_TEMPLE_MQ_TREES] = Region("Forest Temple MQ Trees", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)) && (logic->CanPassEnemy(RE_BIG_SKULLTULA, ED_SHORT_JUMPSLASH, false) || logic->CanUse(RG_HOVER_BOOTS)) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_ENTRYWAY,                   true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER] = Region("Forest Temple MQ Overgrown Hallway Lower", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_TREES,                true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER] = Region("Forest Temple MQ Overgrown Hallway Upper", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,                   logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Region("Forest Temple MQ Lobby", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MEG, logic->Get(LOGIC_FOREST_JOELLE) && logic->Get(LOGIC_FOREST_BETH) && logic->Get(LOGIC_FOREST_AMY) && logic->CanKillEnemy(RE_MEG)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_4, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_5, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_LOBBY_POT_6, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER,  logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY,        true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY,  true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, false),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,         logic->CanHitEyeTargets()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,         logic->CanHitEyeTargets()),
        //implies the other 3 poes
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT,             logic->Get(LOGIC_FOREST_MEG)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY] = Region("Forest Temple MQ North Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,       true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM, logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM] = Region("Forest Temple MQ Wolfos Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_ACCESS,                         logic->CanBreakPots()),
        EVENT_ACCESS(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, logic->CanKillEnemy(RE_WOLFOS)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY] = Region("Forest Temple MQ Red Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,              AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);})),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BLOCK_PUZZLE_FLOOR, AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BLOCK_PUZZLE_FLOOR] = Region("Forest Temple MQ Block Puzzle Floor", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, (ctx->GetTrickOption(RT_FOREST_MQ_BLOCK_PUZZLE) && logic->CanUse(RG_BOMBCHU_5))),
        //It is barely possible to get this as child with master + hovers, but it's tight without bunny speed
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY,     (ctx->GetTrickOption(RT_FOREST_MQ_RANG_HALLWAY_SWITCH) && logic->CanUse(RG_BOOMERANG)) || (ctx->GetTrickOption(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH) && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE,  ((logic->HasItem(RG_CLIMB) || (logic->IsAdult && logic->CanGroundJump())) && 
                                                           (logic->HasItem(RG_GORONS_BRACELET) || logic->CanUse(RG_HOVER_BOOTS))) || 
                                                          (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,        logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->CanUse(RG_HOOKSHOT)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE] = Region("Forest Temple MQ Lower Block Puzzle", SCENE_FOREST_TEMPLE, {
        //Events
        //It is barely possible to get this as child with master + hovers, but it's tight without bunny speed
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY,     (ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && logic->CanUse(RG_HOVER_BOOTS) && 
                                                          (logic->IsAdult && logic->CanJumpslash()) || 
                                                          (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_MASTER_SWORD))))),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BLOCK_PUZZLE_FLOOR,  true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE, (logic->HasItem(RG_GORONS_BRACELET) && (logic->HasItem(RG_CLIMB) || (logic->IsAdult && logic->CanGroundJump()))) || 
                                                           logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,        logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->CanUse(RG_HOVER_BOOTS)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE] = Region("Forest Temple MQ Middle Block Puzzle", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_LONGSHOT))),
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY,     ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && 
                                                         (logic->IsAdult && logic->CanJumpslash()) || 
                                                          (logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD)))),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, (logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET)) || 
                                                         (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT))),
        //Hammer cannot recoil from here, but can make the jump forwards with a hammer jumpslash as adult
        ENTRANCE(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,       logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->CanUse(RG_HOVER_BOOTS) || 
                                                         (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && (logic->CanJumpslashExceptHammer() || (logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER))))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE] = Region("Forest Temple MQ Upper Block Puzzle", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, logic->CanHitSwitch()),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BLOCK_PUZZLE_TOP,    logic->HasItem(RG_CLIMB)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BLOCK_PUZZLE_TOP] = Region("Forest Temple MQ Block Puzzle Top", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY,   logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_JOELLE_ROOM,        logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
        //!QUANTUM LOGIC!
        //As there is no way in default logic to reach the other possible key use without going through RR_FOREST_TEMPLE_MQ_NW_COURTYARD, this is logically safe for now
        //Breaks if there's any other way to RR_FOREST_TEMPLE_MQ_FALLING_ROOM than going through the eye targets in RR_FOREST_TEMPLE_MQ_LOBBY
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,       logic->SmallKeys(SCENE_FOREST_TEMPLE, 2) && AnyAgeTime([]{return logic->CanKillEnemy(RE_FLOORMASTER);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY] = Region("Forest Temple MQ Straight Hallway", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM] = Region("Forest Temple MQ Floormaster Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, AnyAgeTime([]{return logic->CanKillEnemy(RE_FLOORMASTER);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE] = Region("Forest Temple MQ Indoor Ledge", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY, logic->CanHitSwitch()),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE] = Region("Forest Temple MQ Outdoor Ledge", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_RIGHT_HEART,  true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_MIDDLE_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_LEFT_HEART,   true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,            true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,            true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_WELL_LEDGE, logic->CanUse(RG_HOVER_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_REDEAD_ROOM,             true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM,        true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_REDEAD_ROOM] = Region("Forest Temple MQ Redead Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, logic->CanKillEnemy(RE_REDEAD) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD] = Region("Forest Temple MQ NW Courtyard", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BURNED_WEB, logic->CanUse(RG_FIRE_ARROWS)),
    }, {
        //Locations
        //the well checks are considered from both areas instead of being a region because the draining is a temp flag and the skull (as well as the chest with hook glitch) has different breath timers from each side
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                   (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_RIGHT_HEART,     logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_MIDDLE_HEART,    logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_LEFT_HEART,      logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_WEST_HEART,           logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_MIDDLE_HEART,         logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_EAST_HEART,           logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_WELL_LEDGE,   logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,              logic->HasItem(RG_CLIMB) && (logic->Get(LOGIC_FOREST_DRAINED_WELL) || ((((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16))),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE, (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_LONGSHOT)) && logic->Get(LOGIC_FOREST_MQ_BURNED_WEB) && (logic->CanKillEnemy(RE_BIG_SKULLTULA, ED_LONGSHOT) || logic->CanUse(RG_BOMBCHU_5))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD_WELL_LEDGE] = Region("Forest Temple MQ NW Courtyard Well Ledge", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)), // rang trick could get this from below
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,       true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->IsAdult && logic->CanGroundJump()))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE] = Region("Forest Temple MQ NW Courtyard Upper Alcove", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BURNED_WEB, logic->HasFireSource()),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,       logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,      true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE] = Region("Forest Temple MQ North Passage", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_MQ_BURNED_WEB, logic->HasFireSourceWithTorch()),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE, logic->Get(LOGIC_FOREST_MQ_BURNED_WEB)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES,      true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD] = Region("Forest Temple MQ NE Courtyard", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_STICK_ACCESS,        logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_NUT_ACCESS,          logic->CanGetDekuBabaNuts()),
        EVENT_ACCESS(LOGIC_FOREST_DRAINED_WELL, logic->CanHitEyeTargets()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 (logic->Get(LOGIC_FOREST_DRAINED_WELL) && logic->HasItem(RG_OPEN_CHEST)) || (logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        //implies logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                    logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_WEST_HEART,            logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_MIDDLE_HEART,          logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_EAST_HEART,            logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,                true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,         (logic->HasItem(RG_CLIMB) || logic->CanUse(RG_HOOKSHOT)) && (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT)) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES, logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_CLIMB)) || (logic->CanUse(RG_HOOKSHOT) && ((logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->HasItem(RG_CLIMB)) || logic->CanUse(RG_SONG_OF_TIME)))),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND,  logic->CanUse(RG_LONGSHOT)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_DOORFRAME] = Region("Forest Temple MQ NE Courtyard Doorframe", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        //Actually killing the skull from the doorframe with melee is annoying. Hammer swing hits low enough unaided, other swords need to crouch stab but the spot is precise based on range. kokiri sword doesn't reach at all for adult.
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW) || logic->CanUse(RG_MEGATON_HAMMER) ||
                                                                 (logic->CanStandingShield() && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD) || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD))))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,        true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND, logic->CanHammerRecoilHover()),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES] = Region("Forest Temple MQ Courtyard Top Ledges", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,          true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,           true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_DOORFRAME, logic->CanHammerRecoilHover() || 
                                                             ((ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash())) ||
                                                             (logic->IsChild && (ctx->GetTrickOption(RT_FOREST_MQ_CHILD_DOORFRAME) || logic->CanMiddairGroundJump()))),
        //N64 logic doesn't check damage but I always take some so I'm adding it
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND,    ctx->GetTrickOption(RT_FOREST_COURTYARD_LEDGE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash() && logic->TakeDamage()),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND] = Region("Forest Temple MQ NE Courtyard Island", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        //Skipping swim here is non-trival, needs a roll-jump. If a swim lock is added it's probably wise to copy deku baba events here
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,                    true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND, logic->CanUse(RG_SONG_OF_TIME)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND] = Region("Forest Temple MQ NE Courtyard Ledge Above Island", SCENE_FOREST_TEMPLE, {}, {}, {
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,        true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_JOELLE_ROOM] = Region("Forest Temple MQ Joelle Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_JOELLE, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_MAP_CHEST, logic->Get(LOGIC_FOREST_JOELLE) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,     true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM] = Region("Forest Temple MQ 3 Stalfos Room", SCENE_FOREST_TEMPLE, {
        //Events
        //technically happens in RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM, but the way this room blocks the hole means it cannot be logical to do anything else there.
        EVENT_ACCESS(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, logic->CanKillEnemy(RE_WOLFOS)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOW_CHEST,           logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_JOELLE_ROOM, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BETH_ROOM,   logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BETH_ROOM] = Region("Forest Temple MQ Beth Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_BETH, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST,  logic->Get(LOGIC_FOREST_BETH) && logic->HasItem(RG_OPEN_CHEST)),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        //!QUANTUM LOGIC!
        //This key logic assumes that you can get to falling room either by spending the 5th key here, or by wasting a key in falling room itself.
        //While being the 5th key makes this simpler in theory, if a different age can waste the key compared to reaching this room it breaks
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,    logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && AnyAgeTime([]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);})),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,  true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM] = Region("Forest Temple MQ Torch Shot Room", SCENE_FOREST_TEMPLE, {}, {
        // Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_3, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, logic->CanUse(logic->HasItem(RG_POWER_BRACELET) ? RG_FAIRY_BOW : RG_FIRE_ARROWS) || logic->CanUse(RG_DINS_FIRE)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BETH_ROOM,    logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Region("Forest Temple MQ Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_AMY_ROOM,                        logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_AMY_ROOM] = Region("Forest Temple MQ Amy Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_AMY, logic->CanUse(RG_FAIRY_BOW) && logic->HasItem(RG_POWER_BRACELET)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, logic->Get(LOGIC_FOREST_AMY)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,         logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY] = Region("Forest Temple MQ Blue Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_AMY_ROOM, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,    true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BASEMENT] = Region("Forest Temple MQ Basement", SCENE_FOREST_TEMPLE, {
        //Events
        //Implies CanHitSwitch()
        EVENT_ACCESS(LOGIC_FOREST_OPEN_BOSS_HALLWAY, logic->HasItem(RG_POWER_BRACELET) && logic->CanHitEyeTargets()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, logic->HasItem(RG_POWER_BRACELET) && logic->HasItem(RG_OPEN_CHEST)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,             true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT_POT_ROOM,  logic->HasItem(RG_POWER_BRACELET) && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage())),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,        logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BASEMENT_POT_ROOM] = Region("Forest Temple MQ Basement Pot Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] = Region("Forest Temple Boss Entryway", SCENE_FOREST_TEMPLE, {}, {}, {
        // Exits
        ENTRANCE(RR_FOREST_TEMPLE_BASEMENT,    ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla() && logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT, ctx->GetDungeon(FOREST_TEMPLE)->IsMQ() && logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ROOM,   logic->HasItem(RG_FOREST_TEMPLE_BOSS_KEY)),
    });

    areaTable[RR_FOREST_TEMPLE_BOSS_ROOM] = Region("Forest Temple Boss Room", SCENE_FOREST_TEMPLE_BOSS, {
        // Events
        EVENT_ACCESS(LOGIC_FOREST_TEMPLE_CLEAR, logic->CanKillEnemy(RE_PHANTOM_GANON)),
    }, {
        // Locations
        LOCATION(RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, logic->Get(LOGIC_FOREST_TEMPLE_CLEAR)),
        LOCATION(RC_PHANTOM_GANON,                     logic->Get(LOGIC_FOREST_TEMPLE_CLEAR)),
    }, {
        // Exits
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, false),
        ENTRANCE(RR_SACRED_FOREST_MEADOW,        logic->Get(LOGIC_FOREST_TEMPLE_CLEAR), false),
    });

    // clang-format on
}
