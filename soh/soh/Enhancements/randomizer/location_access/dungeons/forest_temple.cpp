#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/dungeon.h"

using namespace Rando;

void RegionTable_Init_ForestTemple() {
    // clang-format off
    // Vanilla/MQ Decider
    areaTable[RR_FOREST_TEMPLE_ENTRYWAY] = Region("Forest Temple Entryway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_FIRST_ROOM, ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOBBY,   ctx->GetDungeon(FOREST_TEMPLE)->IsMQ()),
        ENTRANCE(RR_SACRED_FOREST_MEADOW,     true),
    });

#pragma region Vanilla

    areaTable[RR_FOREST_TEMPLE_FIRST_ROOM] = Region("Forest Temple First Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, true),
        LOCATION(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    (logic->IsAdult && logic->CanUse(RG_BOMB_BAG)) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOMBCHU_5) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_FOREST_FIRST_GS) && (logic->CanJumpslashExceptHammer() || (logic->IsChild && logic->CanUse(RG_BOMB_BAG))))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_ENTRYWAY,       true),
        ENTRANCE(RR_FOREST_TEMPLE_SOUTH_CORRIDOR, true),
    });

    areaTable[RR_FOREST_TEMPLE_SOUTH_CORRIDOR] = Region("Forest Temple South Corridor", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_FIRST_ROOM, true),
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,      logic->CanPassEnemy(RE_BIG_SKULLTULA)),
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
        ENTRANCE(RR_FOREST_TEMPLE_SOUTH_CORRIDOR,    true),
        ENTRANCE(RR_FOREST_TEMPLE_NORTH_CORRIDOR,    true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild),
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)),
        ENTRANCE(RR_FOREST_TEMPLE_WEST_CORRIDOR,     logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
        ENTRANCE(RR_FOREST_TEMPLE_EAST_CORRIDOR,     false),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_REGION,       logic->Get(LOGIC_FOREST_MEG)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     false),
    });

    areaTable[RR_FOREST_TEMPLE_NORTH_CORRIDOR] = Region("Forest Temple North Corridor", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,         true),
        ENTRANCE(RR_FOREST_TEMPLE_LOWER_STALFOS, true),
    });

    areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Region("Forest Temple Lower Stalfos", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_POT, true),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NORTH_CORRIDOR, true),
    });

    areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Region("Forest Temple NW Outdoors Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_BABA_STICKS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_DEKU_BABA_NUTS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_OUTDOORS_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_OUTDOORS_HEARTS_BOOMERANG)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,             logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->CanGroundJump()))),
        ENTRANCE(RR_FOREST_TEMPLE_MAP_ROOM,          true),
        ENTRANCE(RR_FOREST_TEMPLE_SEWER,             logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     false),
    });

    areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Region("Forest Temple NW Outdoors Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_BABA_STICKS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_DEKU_BABA_NUTS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->HookshotOrBoomerang()),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     true),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER,    true),
        ENTRANCE(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, true),
        ENTRANCE(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,     true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      true),
    });

    areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Region("Forest Temple NE Outdoors Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_BABA_STICKS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_DEKU_BABA_NUTS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, logic->CanUse(RG_HOOKSHOT)),
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_FOREST_OUTDOORS_EAST_GS) && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,             true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_FOREST_TEMPLE_SEWER,             logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,      false),
    });

    areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Region("Forest Temple NE Outdoors Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_BABA_STICKS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_DEKU_BABA_NUTS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, logic->IsAdult && ctx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->CanUse(RG_HOVER_BOOTS)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, true),
        ENTRANCE(RR_FOREST_TEMPLE_MAP_ROOM,          true),
        ENTRANCE(RR_FOREST_TEMPLE_DRAINED_SEWER,     true),
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,      ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanJumpslashExceptHammer() && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_SCARECROW)),
    });

    areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Region("Forest Temple Map Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MAP_CHEST, logic->CanKillEnemy(RE_BLUE_BUBBLE)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
    });

    areaTable[RR_FOREST_TEMPLE_SEWER] = Region("Forest Temple Sewer", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, logic->HasItem(RG_BRONZE_SCALE)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, logic->HasItem(RG_BRONZE_SCALE)),
    });

    areaTable[RR_FOREST_TEMPLE_DRAINED_SEWER] = Region("Forest Temple Drained Well", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      true),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, true),
    });

    areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Region("Forest Temple Below Boss Key Chest", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, Here(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return logic->CanKillEnemy(RE_BLUE_BUBBLE);})),
    });

    areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Region("Forest Temple Floormaster Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, logic->CanDamage()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, true),
    });

    areaTable[RR_FOREST_TEMPLE_WEST_CORRIDOR] = Region("Forest Temple West Corridor", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,           logic->SmallKeys(SCENE_FOREST_TEMPLE, 1)),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, logic->CanAttack() || logic->CanUse(RG_NUTS)),
    });

    areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Region("Forest Temple Block Push Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_WEST_CORRIDOR,            true),
        ENTRANCE(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER,        logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && logic->CanJumpslashExceptHammer() && logic->HasItem(RG_GORONS_BRACELET))),
        ENTRANCE(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET) && logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
        ENTRANCE(RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)) && logic->HasItem(RG_GORONS_BRACELET) && logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
    });

    areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Region("Forest Temple NW Corridor Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
        ENTRANCE(RR_FOREST_TEMPLE_RED_POE_ROOM,    logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
    });

    areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Region("Forest Temple NW Corridor Straightened", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, true),
        ENTRANCE(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      logic->SmallKeys(SCENE_FOREST_TEMPLE, 2)),
    });

    areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Region("Forest Temple Red Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_JOELLE, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RED_POE_CHEST, logic->Get(LOGIC_FOREST_JOELLE)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_STALFOS,       true),
    });

    areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Region("Forest Temple Upper Stalfos", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOW_CHEST,           logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_RED_POE_ROOM,  logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_POE_ROOM, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Region("Forest Temple Blue Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_BETH, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_CHEST, logic->Get(LOGIC_FOREST_BETH)),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_UPPER_STALFOS,            true),
        ENTRANCE(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Region("Forest Temple NE Corridor Straightened", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
        ENTRANCE(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 5)),
    });

    areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Region("Forest Temple NE Corridor Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
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
        ENTRANCE(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, logic->SmallKeys(SCENE_FOREST_TEMPLE, 5)),
        ENTRANCE(RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE))),
    });

    areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Region("Forest Temple Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_DINS_FIRE) || logic->HasExplosives()),
        LOCATION(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,    true),
        LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, true),
        ENTRANCE(RR_FOREST_TEMPLE_GREEN_POE_ROOM,    true),
    });

    areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Region("Forest Temple Green Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_AMY, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_FALLING_ROOM,  true),
        ENTRANCE(RR_FOREST_TEMPLE_EAST_CORRIDOR, logic->Get(LOGIC_FOREST_AMY)),
    });

    areaTable[RR_FOREST_TEMPLE_EAST_CORRIDOR] = Region("Forest Temple East Corridor", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,          logic->CanAttack() || logic->CanUse(RG_NUTS)),
        ENTRANCE(RR_FOREST_TEMPLE_GREEN_POE_ROOM, logic->CanAttack() || logic->CanUse(RG_NUTS)),
    });

    areaTable[RR_FOREST_TEMPLE_BOSS_REGION] = Region("Forest Temple Boss Region", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BASEMENT_CHEST, true),
        LOCATION(RC_FOREST_TEMPLE_GS_BASEMENT,    logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_LOBBY,         true),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, true),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Region("Forest Temple MQ Lobby", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA, ED_SHORT_JUMPSLASH, false) || logic->CanUse(RG_HOVER_BOOTS)),
        //Implies CanPassEnemy(RE_BIG_SKULLTULA)
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_ENTRYWAY,        true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, logic->SmallKeys(SCENE_FOREST_TEMPLE, 1) && logic->CanPassEnemy(RE_BIG_SKULLTULA)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_CENTRAL_AREA] = Region("Forest Temple MQ Central Region", SCENE_FOREST_TEMPLE, {
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
        ENTRANCE(RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM,        logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,        logic->CanHitEyeTargets()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,        logic->CanHitEyeTargets()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, Here(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, []{return logic->CanKillEnemy(RE_STALFOS);})),
        //implies the other 3 poes
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT,           logic->Get(LOGIC_FOREST_MEG)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM] = Region("Forest Temple MQ Wolfos Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FAIRY_POT,                            true),
        EVENT_ACCESS(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, logic->CanKillEnemy(RE_WOLFOS)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE] = Region("Forest Temple MQ Lower Block Puzzle", SCENE_FOREST_TEMPLE, {
        //longshot is capable of hitting the switch, but some invisible collision makes the shot harder than you would think, so it may be trickworthy
        EVENT_ACCESS(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, (ctx->GetTrickOption(RT_FOREST_MQ_BLOCK_PUZZLE) && logic->CanUse(RG_BOMBCHU_5))),
        //It is barely possible to get this as child with master + hovers, but it's tight without bunny speed
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY,     (ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && logic->CanUse(RG_HOVER_BOOTS) && (logic->IsAdult && logic->CanJumpslash()) || (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_MASTER_SWORD)))) || (ctx->GetTrickOption(RT_FOREST_MQ_RANG_HALLWAY_SWITCH) && logic->CanUse(RG_BOOMERANG)) || (ctx->GetTrickOption(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH) && logic->CanUse(RG_HOOKSHOT))),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA,        Here(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, []{return logic->CanKillEnemy(RE_STALFOS);})),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE, logic->HasItem(RG_GORONS_BRACELET) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT))),
        //Assumes RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE access
        ENTRANCE(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE,  (logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET)) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT))),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,       logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE] = Region("Forest Temple MQ Middle Block Puzzle", SCENE_FOREST_TEMPLE, {
        //longshot is capable of hitting the switch, but some invisible collision makes the shot more annoying than you would think, so it may be trickworthy
        EVENT_ACCESS(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_LONGSHOT))),
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY,     ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && (logic->IsAdult && logic->CanJumpslash()) || (logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD)))),
    }, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, (logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET)) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT))),
        //Hammer cannot recoil from here, but can make the jump forwards with a hammer jumpslash as adult
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && (logic->CanJumpslashExceptHammer() || (logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER))))),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE] = Region("Forest Temple MQ After Block Puzzle", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_JOELLE_ROOM,      logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->SmallKeys(SCENE_FOREST_TEMPLE, 4)),
        //!QUANTUM LOGIC!
        //As there is no way in default logic to reach the other possible key use without going through RR_FOREST_TEMPLE_MQ_NW_OUTDOORS, this is logically safe for now
        //Breaks if there's any other way to RR_FOREST_TEMPLE_MQ_FALLING_ROOM than going through the eye targets in RR_FOREST_TEMPLE_MQ_CENTRAL_AREA
        //Requires a bow/sling ammo source once ammo logic is done, to avoid edge cases.
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,      logic->SmallKeys(SCENE_FOREST_TEMPLE, 2) && Here(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, []{return logic->CanKillEnemy(RE_FLOORMASTER);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY] = Region("Forest Temple MQ Straight Hallway", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM] = Region("Forest Temple MQ Floormaster Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, Here(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, []{return logic->CanKillEnemy(RE_FLOORMASTER);})),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Region("Forest Temple MQ Outdoor Ledge", SCENE_FOREST_TEMPLE, {
        EVENT_ACCESS(LOGIC_FOREST_CAN_TWIST_HALLWAY, logic->CanHitSwitch()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST,           logic->CanKillEnemy(RE_REDEAD)),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_RIGHT_HEART,  true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_MIDDLE_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_LEFT_HEART,   true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_OUTDOORS] = Region("Forest Temple MQ NW Outdoors", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        //the well checks are considered from both areas instead of being a region because the draining is a temp flag and the skull (as well as the chest with hook glitch) has different breath timers from each side
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                   (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_RIGHT_HEART,     logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_OUTDOORS_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_MIDDLE_HEART,    logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_OUTDOORS_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_LEFT_HEART,      logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_OUTDOORS_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_WEST_HEART,           logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_MIDDLE_HEART,         logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_EAST_HEART,           logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,         (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_MQ_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, logic->CanUse(RG_FIRE_ARROWS)),
    });

    //The well only coniders the eye target here because the eye target is a temp flag, making it unwieldy to use as an EventAccess to make it it's own room
    areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS] = Region("Forest Temple MQ NE Outdoors", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_DEKU_BABA_STICKS, logic->CanGetDekuBabaSticks()),
        EVENT_ACCESS(LOGIC_DEKU_BABA_NUTS,   logic->CanGetDekuBabaNuts()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 logic->CanHitEyeTargets() || (logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
        //implies logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                    logic->CanHitEyeTargets() || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_WEST_HEART,            (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->CanHitEyeTargets()),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_MIDDLE_HEART,          (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->CanHitEyeTargets()),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_EAST_HEART,            (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->CanHitEyeTargets()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,         (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT)) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && ((logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || logic->CanUse(RG_SONG_OF_TIME)))),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   logic->CanUse(RG_LONGSHOT)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Region("Forest Temple MQ Outdoors Top Ledges", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, true),
        //Actually killing the skull from the doorframe with melee is annoying. Hammer swing hits low enough unaided, other swords need to crouch stab but the spot is precise based on range. kokiri sword doesn't reach at all for adult.
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD,          ((logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME)) || (logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_FOREST_DOORFRAME))) && logic->CanJumpslash() && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_FAIRY_BOW) || logic->HookshotOrBoomerang() || (logic->CanStandingShield() && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD) || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD)))))),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,       logic->HasFireSourceWithTorch()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,       true),
        //N64 logic doesn't check damage but I always take some so I'm adding it
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, ctx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash() && logic->TakeDamage()),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Region("Forest Temple MQ NE Outdoors Ledge", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, true),
    }, {
        //Exits
        //Skipping swim here is non-trival, needs a roll-jump. If a swim lock is added it's probably wise to copy deku baba events here
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,  true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, logic->CanUse(RG_SONG_OF_TIME)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_JOELLE_ROOM] = Region("Forest Temple MQ Joelle room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_JOELLE, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_MAP_CHEST, logic->Get(LOGIC_FOREST_JOELLE)),
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
        LOCATION(RC_FOREST_TEMPLE_MQ_BOW_CHEST,           logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
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
        LOCATION(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST,  logic->Get(LOGIC_FOREST_BETH)),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BLUE_POE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        //!QUANTUM LOGIC!
        //This key logic assumes that you can get to falling room either by spending the 5th key here, or by wasting a key in falling room itself.
        //While being the 5th key makes this simpler in theory, if a different age can waste the key compared to reaching this room it breaks
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,    logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && Here(RR_FOREST_TEMPLE_MQ_BETH_ROOM, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);})),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM, logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,  true),
    });

    //This room exists to show the actual map layout, and for when the crates get added to logic
    areaTable[RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM] = Region("Forest Temple MQ Torch Shot Room", SCENE_FOREST_TEMPLE, {}, {
        // Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_3, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BETH_ROOM,    logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Region("Forest Temple MQ Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, true),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_AMY_ROOM,          logic->SmallKeys(SCENE_FOREST_TEMPLE, 6)),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_AMY_ROOM] = Region("Forest Temple MQ Amy Room", SCENE_FOREST_TEMPLE, {
        //Events
        EVENT_ACCESS(LOGIC_FOREST_AMY, logic->CanUse(RG_FAIRY_BOW)),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, logic->Get(LOGIC_FOREST_AMY)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, true),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BASEMENT] = Region("Forest Temple MQ Basement", SCENE_FOREST_TEMPLE, {
        //Events
        //Implies CanHitSwitch()
        EVENT_ACCESS(LOGIC_FOREST_OPEN_BOSS_CORRIDOR, logic->CanHitEyeTargets()),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, true),
    }, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA,      logic->Get(LOGIC_FOREST_MEG)),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT_POT_ROOM, logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage()),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BOSS_REGION,       logic->Get(LOGIC_FOREST_OPEN_BOSS_CORRIDOR)),
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

    areaTable[RR_FOREST_TEMPLE_MQ_BOSS_REGION] = Region("Forest Temple MQ Boss Region", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BASEMENT,   logic->Get(LOGIC_FOREST_OPEN_BOSS_CORRIDOR)),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, true),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] = Region("Forest Temple Boss Entryway", SCENE_FOREST_TEMPLE, {}, {}, {
        // Exits
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_REGION,    ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla() && false),
        ENTRANCE(RR_FOREST_TEMPLE_MQ_BOSS_REGION, ctx->GetDungeon(FOREST_TEMPLE)->IsMQ() && false),
        ENTRANCE(RR_FOREST_TEMPLE_BOSS_ROOM,      logic->HasItem(RG_FOREST_TEMPLE_BOSS_KEY)),
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
