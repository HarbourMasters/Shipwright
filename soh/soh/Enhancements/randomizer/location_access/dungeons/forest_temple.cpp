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
        Entrance(RR_FOREST_TEMPLE_TREES,      []{return ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla();}),
        Entrance(RR_FOREST_TEMPLE_MQ_TREES,   []{return ctx->GetDungeon(FOREST_TEMPLE)->IsMQ();}),
        Entrance(RR_SACRED_FOREST_MEADOW,     []{return true;}),
    });

#pragma region Vanilla

    areaTable[RR_FOREST_TEMPLE_TREES] = Region("Forest Temple Trees", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, true),
        LOCATION(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    (logic->IsAdult && logic->CanUse(RG_BOMB_BAG)) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOMBCHU_5) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_FOREST_FIRST_GS) && (logic->CanJumpslashExceptHammer() || (logic->IsChild && logic->CanUse(RG_BOMB_BAG))))),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_ENTRYWAY,                []{return true;}),
        Entrance(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER] = Region("Forest Temple Overgrown Hallway Lower", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_TREES,                   []{return true;}),
        Entrance(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
    });

    areaTable[RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER] = Region("Forest Temple Overgrown Hallway Upper", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_FOREST_TEMPLE_LOBBY,                   []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_LOBBY] = Region("Forest Temple Lobby", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_MEG, []{return logic->Get(LOGIC_FOREST_JOELLE) && logic->Get(LOGIC_FOREST_BETH) && logic->Get(LOGIC_FOREST_AMY) && logic->CanKillEnemy(RE_MEG);}),
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
        Entrance(RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NORTH_HALLWAY,           []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER,      []{return logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,      []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}),
        Entrance(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY,     []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 1);}),
        Entrance(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY,    []{return false;}),
        Entrance(RR_FOREST_TEMPLE_BASEMENT,                []{return logic->Get(LOGIC_FOREST_MEG);}),
        Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,           []{return false;}),
    });

    areaTable[RR_FOREST_TEMPLE_NORTH_HALLWAY] = Region("Forest Temple North Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOBBY,         []{return true;}),
        Entrance(RR_FOREST_TEMPLE_LOWER_STALFOS, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Region("Forest Temple Lower Stalfos", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS,                         []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH)),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_LOWER_STALFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NORTH_HALLWAY, []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH);}),
    });

    areaTable[RR_FOREST_TEMPLE_NW_COURTYARD_LOWER] = Region("Forest Temple NW Courtyard Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_STICK_ACCESS, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(LOGIC_NUT_ACCESS,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->CanUse(RG_LONGSHOT)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_FOREST_COURTYARD_HEARTS_BOOMERANG)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOBBY,              []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, []{return logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && (logic->CanUse(RG_MEGATON_HAMMER) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()))) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->IsAdult && logic->CanGroundJump()));}),
        Entrance(RR_FOREST_TEMPLE_MAP_ROOM,           []{return true;}),
        Entrance(RR_FOREST_TEMPLE_SEWER,              []{return logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS);}),
        Entrance(RR_FOREST_TEMPLE_DRAINED_SEWER,      []{return logic->Get(LOGIC_FOREST_DRAINED_WELL);}),
        Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,      []{return false;}),
    });

    areaTable[RR_FOREST_TEMPLE_NW_COURTYARD_UPPER] = Region("Forest Temple NW Courtyard Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_STICK_ACCESS, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(LOGIC_NUT_ACCESS,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->HookshotOrBoomerang()),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_RIGHT_HEART,     true),
        LOCATION(RC_FOREST_TEMPLE_COURTYARD_LEFT_HEART,      true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER,               []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST,             []{return true;}),
        Entrance(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,                 []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_LOWER] = Region("Forest Temple NE Courtyard Lower", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_STICK_ACCESS, []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(LOGIC_NUT_ACCESS,   []{return logic->CanGetDekuBabaNuts();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_FOREST_COURTYARD_EAST_GS) && logic->CanUse(RG_BOOMERANG))),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOBBY,               []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_UPPER,  []{return logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, []{return logic->CanUse(RG_HOOKSHOT);}),
        Entrance(RR_FOREST_TEMPLE_SEWER,               []{return (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16;}),
        Entrance(RR_FOREST_TEMPLE_DRAINED_SEWER,       []{return logic->Get(LOGIC_FOREST_DRAINED_WELL);}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_UPPER] = Region("Forest Temple NE Courtyard Upper", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_STICK_ACCESS,               []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(LOGIC_NUT_ACCESS,                 []{return logic->CanGetDekuBabaNuts();}),
        EventAccess(LOGIC_FOREST_DRAINED_WELL,        []{return true;}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,     []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND,    []{return logic->IsAdult && ctx->GetTrickOption(RT_FOREST_COURTYARD_LEDGE) && logic->CanUse(RG_HOVER_BOOTS);}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_DOORFRAME, []{return logic->CanHammerRecoilHover() || ((ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash()));}),
        Entrance(RR_FOREST_TEMPLE_MAP_ROOM,               []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_DOORFRAME] = Region("Forest Temple NE Courtyard Doorframe", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_SUMMON_NE_SCARECROW, []{return logic->ScarecrowsSong();}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,  []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, []{return logic->CanHammerRecoilHover();}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND] = Region("Forest Temple NE Courtyard Island", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,           []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE, []{return logic->Get(LOGIC_FOREST_SUMMON_NE_SCARECROW) && logic->CanUse(RG_HOOKSHOT);}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE] = Region("Forest Temple NE Courtyard Scarecrow Ledge", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_SUMMON_NE_SCARECROW, []{return logic->ScarecrowsSong();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_DINS_FIRE) || logic->HasExplosives()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_ISLAND, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,        []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Region("Forest Temple Map Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MAP_CHEST, logic->CanKillEnemy(RE_BLUE_BUBBLE)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);});}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_UPPER, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);});}),
    });

    areaTable[RR_FOREST_TEMPLE_SEWER] = Region("Forest Temple Sewer", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, []{return logic->HasItem(RG_BRONZE_SCALE);}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER, []{return logic->HasItem(RG_BRONZE_SCALE);}),
    });

    areaTable[RR_FOREST_TEMPLE_DRAINED_SEWER] = Region("Forest Temple Drained Well", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_WELL_CHEST,      true),
        LOCATION(RC_FOREST_TEMPLE_WELL_WEST_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_WELL_EAST_HEART, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_LOWER, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Region("Forest Temple Below Boss Key Chest", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_BLUE_BUBBLE);});}),
    });

    areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Region("Forest Temple Floormaster Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, logic->CanDamage()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY] = Region("Forest Temple West Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOBBY,                       []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 1);}),
        Entrance(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
    });

    areaTable[RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT] = Region("Forest Temple West Hallway Doormat", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY,   []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM] = Region("Forest Temple Lower Block Push Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT,      []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM,           []{return logic->HasItem(RG_GORONS_BRACELET);}),
        Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, []{return logic->CanUse(RG_HOVER_BOOTS);}),
    });

    areaTable[RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM] = Region("Forest Temple Middle Block Push Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, logic->CanHitEyeTargets()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM,            []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, []{return ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && logic->CanJumpslashExceptHammer();}),
        Entrance(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM,            []{return logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET);}),
    });

    areaTable[RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM] = Region("Forest Temple Upper Block Push Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MIDDLE_BLOCK_PUSH_ROOM,   []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED,      []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 2);}),
        Entrance(RR_FOREST_TEMPLE_NW_HALLWAY_STRAIGHTENED, []{return logic->CanHitEyeTargets() && logic->SmallKeys(SCENE_FOREST_TEMPLE, 2);}),
    });

    areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE] = Region("Forest Temple Block Push Room Courtyard Alcove", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOWER_BLOCK_PUSH_ROOM, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,    []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED] = Region("Forest Temple NW Hallway Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 2);}),
        Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,          []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 3);}),
    });

    areaTable[RR_FOREST_TEMPLE_NW_HALLWAY_STRAIGHTENED] = Region("Forest Temple NW Hallway Straightened", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST,  []{return true;}),
        Entrance(RR_FOREST_TEMPLE_UPPER_BLOCK_PUSH_ROOM, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 2);}),
    });

    areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Region("Forest Temple Red Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_JOELLE, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_RED_POE_CHEST, logic->Get(LOGIC_FOREST_JOELLE)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 3);}),
        Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,       []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Region("Forest Temple Upper Stalfos", SCENE_FOREST_TEMPLE, {
        //Events
        //technically happens in RR_FOREST_TEMPLE_LOWER_STALFOS, but the way this room blocks the hole means it cannot be logical to do anything else there.
        EventAccess(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BOW_CHEST,           logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_UPPER_STALFOS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,  []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3);}),
        Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM, []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3);}),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Region("Forest Temple Blue Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_BETH, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_CHEST, logic->Get(LOGIC_FOREST_BETH)),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_BLUE_POE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,            []{return true;}),
        Entrance(RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 4);}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED] = Region("Forest Temple NE Hallway Straightened", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 4);}),
        Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 5);}),
    });

    areaTable[RR_FOREST_TEMPLE_NE_HALLWAY_TWISTED] = Region("Forest Temple NE Hallway Twisted", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 5);}),
        Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,    []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_FROZEN_EYE_ROOM] = Region("Forest Temple Frozen Eye Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FROZEN_EYE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_FROZEN_EYE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 5);}),
        Entrance(RR_FOREST_TEMPLE_NE_HALLWAY_TWISTED,      []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE));}),
    });

    areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Region("Forest Temple Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM,               []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Region("Forest Temple Green Poe Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_AMY, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,                 []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT, []{return logic->Get(LOGIC_FOREST_AMY);}),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT] = Region("Forest Temple Blue Doormat Hallway Doormat", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM,       []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
    });

    areaTable[RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY] = Region("Forest Temple Blue Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_FOREST_TEMPLE_LOBBY,                        []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_BASEMENT] = Region("Forest Temple Basement", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_OPEN_BOSS_HALLWAY, []{return true;}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_BASEMENT_CHEST, true),
        LOCATION(RC_FOREST_TEMPLE_GS_BASEMENT,    logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_LOBBY,         []{return true;}),
        Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, []{return logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY);}),
    });

#pragma endregion

#pragma region MQ

    areaTable[RR_FOREST_TEMPLE_MQ_TREES] = Region("Forest Temple MQ Trees", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA, ED_SHORT_JUMPSLASH, false) || logic->CanUse(RG_HOVER_BOOTS)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_ENTRYWAY,                   []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER] = Region("Forest Temple MQ Overgrown Hallway Lower", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_TREES,                []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER] = Region("Forest Temple MQ Overgrown Hallway Upper", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, logic->HookshotOrBoomerang()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,                   []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 1);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Region("Forest Temple MQ Lobby", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_MEG, []{return logic->Get(LOGIC_FOREST_JOELLE) && logic->Get(LOGIC_FOREST_BETH) && logic->Get(LOGIC_FOREST_AMY) && logic->CanKillEnemy(RE_MEG);}),
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
        Entrance(RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER,  []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 1);}),
        Entrance(RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY,        []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY,  []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, []{return false;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,         []{return logic->CanHitEyeTargets();}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,         []{return logic->CanHitEyeTargets();}),
        //implies the other 3 poes
        Entrance(RR_FOREST_TEMPLE_MQ_BASEMENT,             []{return logic->Get(LOGIC_FOREST_MEG);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY] = Region("Forest Temple MQ North Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,       []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM, []{return logic->IsChild || logic->CanUse(RG_SONG_OF_TIME);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM] = Region("Forest Temple MQ Wolfos Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FAIRY_ACCESS,                         []{return logic->CanBreakPots();}),
        EventAccess(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, []{return logic->CanKillEnemy(RE_WOLFOS);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY, []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME));}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY] = Region("Forest Temple MQ Red Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,              []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);});}),
        Entrance(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);});}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE] = Region("Forest Temple MQ Lower Block Puzzle", SCENE_FOREST_TEMPLE, {
        //longshot is capable of hitting the switch, but some invisible collision makes the shot harder than you would think, so it may be trickworthy
        EventAccess(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, []{return (ctx->GetTrickOption(RT_FOREST_MQ_BLOCK_PUZZLE) && logic->CanUse(RG_BOMBCHU_5));}),
        //It is barely possible to get this as child with master + hovers, but it's tight without bunny speed
        EventAccess(LOGIC_FOREST_CAN_TWIST_HALLWAY,     []{return (ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && logic->CanUse(RG_HOVER_BOOTS) && (logic->IsAdult && logic->CanJumpslash()) || (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_MASTER_SWORD)))) || (ctx->GetTrickOption(RT_FOREST_MQ_RANG_HALLWAY_SWITCH) && logic->CanUse(RG_BOOMERANG)) || (ctx->GetTrickOption(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH) && logic->CanUse(RG_HOOKSHOT));}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_STALFOS);});}),
        Entrance(RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE, []{return logic->HasItem(RG_GORONS_BRACELET) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT));}),
        Entrance(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,        []{return logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE] = Region("Forest Temple MQ Middle Block Puzzle", SCENE_FOREST_TEMPLE, {
        //longshot is capable of hitting the switch, but some invisible collision makes the shot more annoying than you would think, so it may be trickworthy
        EventAccess(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS, []{return (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_LONGSHOT));}),
        EventAccess(LOGIC_FOREST_CAN_TWIST_HALLWAY,     []{return ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && (logic->IsAdult && logic->CanJumpslash()) || (logic->CanUse(RG_HOVER_BOOTS) && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD)));}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, []{return (logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET)) || (logic->Get(LOGIC_FOREST_MQ_BLOCK_ROOM_TARGETS) && logic->CanUse(RG_HOOKSHOT));}),
        //Hammer cannot recoil from here, but can make the jump forwards with a hammer jumpslash as adult
        Entrance(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,       []{return logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && (logic->CanJumpslashExceptHammer() || (logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER))));}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE] = Region("Forest Temple MQ After Block Puzzle", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_MIDDLE_BLOCK_PUZZLE, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY,    []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 3);}),
        Entrance(RR_FOREST_TEMPLE_MQ_JOELLE_ROOM,         []{return logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY) && logic->SmallKeys(SCENE_FOREST_TEMPLE, 4);}),
        //!QUANTUM LOGIC!
        //As there is no way in default logic to reach the other possible key use without going through RR_FOREST_TEMPLE_MQ_NW_COURTYARD, this is logically safe for now
        //Breaks if there's any other way to RR_FOREST_TEMPLE_MQ_FALLING_ROOM than going through the eye targets in RR_FOREST_TEMPLE_MQ_LOBBY
        //Requires a bow/sling ammo source once ammo logic is done, to avoid edge cases.
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,       []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 2) && AnyAgeTime([]{return logic->CanKillEnemy(RE_FLOORMASTER);});}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_STRAIGHT_HALLWAY] = Region("Forest Temple MQ Straight Hallway", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, logic->SmallKeys(SCENE_FOREST_TEMPLE, 3)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM] = Region("Forest Temple MQ Floormaster Room", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_FLOORMASTER);});}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE] = Region("Forest Temple MQ Indoor Ledge", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_CAN_TWIST_HALLWAY, []{return logic->CanHitSwitch();}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, []{return logic->Get(LOGIC_FOREST_CAN_TWIST_HALLWAY);}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE] = Region("Forest Temple MQ Outdoor Ledge", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_RIGHT_HEART,  true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_MIDDLE_HEART, true),
        LOCATION(RC_FOREST_TEMPLE_MQ_COURTYARD_LEFT_HEART,   true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,     []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,     []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_REDEAD_ROOM,      []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_REDEAD_ROOM] = Region("Forest Temple MQ Redead Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, logic->CanKillEnemy(RE_REDEAD)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, []{return AnyAgeTime([]{return logic->CanKillEnemy(RE_REDEAD);});}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD] = Region("Forest Temple MQ NW Courtyard", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_MQ_BURNED_WEB, []{return logic->CanUse(RG_FIRE_ARROWS);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
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
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,              []{return logic->Get(LOGIC_FOREST_DRAINED_WELL) || ((((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT))) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16);}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE, []{return logic->Get(LOGIC_FOREST_MQ_BURNED_WEB) && (logic->CanKillEnemy(RE_BIG_SKULLTULA, ED_LONGSHOT) || logic->CanUse(RG_BOMBCHU_5));}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE,        []{return logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives()) || (ctx->GetTrickOption(RT_GROUND_JUMP_HARD) && logic->IsAdult && logic->CanGroundJump()));}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE] = Region("Forest Temple MQ NW Courtyard Upper Alcove", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_MQ_BURNED_WEB, []{return logic->HasFireSource();}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,       []{return logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage();}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, []{return ctx->GetTrickOption(RT_HOVER_BOOST_SIMPLE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_MEGATON_HAMMER);}),
        Entrance(RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,      []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE] = Region("Forest Temple MQ North Passage", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_MQ_BURNED_WEB, []{return logic->HasFireSourceWithTorch();}),
    }, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD_UPPER_ALCOVE, []{return logic->Get(LOGIC_FOREST_MQ_BURNED_WEB);}),
        Entrance(RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES,      []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD] = Region("Forest Temple MQ NE Courtyard", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_STICK_ACCESS,        []{return logic->CanGetDekuBabaSticks();}),
        EventAccess(LOGIC_NUT_ACCESS,          []{return logic->CanGetDekuBabaNuts();}),
        EventAccess(LOGIC_FOREST_DRAINED_WELL, []{return logic->CanHitEyeTargets();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanOpenUnderwaterChest() && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
        //implies logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                    logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_WEST_HEART,            logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_MIDDLE_HEART,          logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
        LOCATION(RC_FOREST_TEMPLE_MQ_WELL_EAST_HEART,            logic->Get(LOGIC_FOREST_DRAINED_WELL) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,                []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NW_COURTYARD,         []{return (((logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT)) && logic->HasItem(RG_BRONZE_SCALE)) || logic->HasItem(RG_GOLDEN_SCALE)) && logic->WaterTimer() >= 16;}),
        Entrance(RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES, []{return logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && ((logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || logic->CanUse(RG_SONG_OF_TIME)));}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND,  []{return logic->CanUse(RG_LONGSHOT);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_DOORFRAME] = Region("Forest Temple MQ NE Courtyard Doorframe", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        //Actually killing the skull from the doorframe with melee is annoying. Hammer swing hits low enough unaided, other swords need to crouch stab but the spot is precise based on range. kokiri sword doesn't reach at all for adult.
        LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOMB_THROW) || logic->CanUse(RG_MEGATON_HAMMER) ||
                                                                 (logic->CanStandingShield() && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD) || (logic->IsChild && logic->CanUse(RG_KOKIRI_SWORD))))),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,        []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND, []{return logic->CanHammerRecoilHover();}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES] = Region("Forest Temple MQ Courtyard Top Ledges", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,          []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,           []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_DOORFRAME, []{return logic->CanHammerRecoilHover() || 
                                                                       ((ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash())) ||
                                                                       (logic->IsChild && (ctx->GetTrickOption(RT_FOREST_MQ_CHILD_DOORFRAME) || logic->CanMiddairGroundJump()));}),
        //N64 logic doesn't check damage but I always take some so I'm adding it
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND,    []{return ctx->GetTrickOption(RT_FOREST_COURTYARD_LEDGE) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash() && logic->TakeDamage();}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND] = Region("Forest Temple MQ NE Courtyard Island", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, true),
    }, {
        //Exits
        //Skipping swim here is non-trival, needs a roll-jump. If a swim lock is added it's probably wise to copy deku baba events here
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD,                    []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND, []{return logic->CanUse(RG_SONG_OF_TIME);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND] = Region("Forest Temple MQ NE Courtyard Ledge Above Island", SCENE_FOREST_TEMPLE, {}, {}, {
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_ISLAND, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,        []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_JOELLE_ROOM] = Region("Forest Temple MQ Joelle Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_JOELLE, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_MAP_CHEST, logic->Get(LOGIC_FOREST_JOELLE)),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_UPPER_BLOCK_PUZZLE, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 4);}),
        Entrance(RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,     []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM] = Region("Forest Temple MQ 3 Stalfos Room", SCENE_FOREST_TEMPLE, {
        //Events
        //technically happens in RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM, but the way this room blocks the hole means it cannot be logical to do anything else there.
        EventAccess(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH, []{return logic->CanKillEnemy(RE_WOLFOS);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BOW_CHEST,           logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3)),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_UPPER_STALFOS_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_JOELLE_ROOM, []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3);}),
        Entrance(RR_FOREST_TEMPLE_MQ_BETH_ROOM,   []{return logic->Get(LOGIC_FOREST_CLEAR_BETWEEN_JOELLE_AND_BETH) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BETH_ROOM] = Region("Forest Temple MQ Beth Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_BETH, []{return logic->CanUse(RG_FAIRY_BOW);}),
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
        Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,    []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 5) && AnyAgeTime([]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);});}),
        Entrance(RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM, []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 6);}),
        Entrance(RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,  []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_TORCH_SHOT_ROOM] = Region("Forest Temple MQ Torch Shot Room", SCENE_FOREST_TEMPLE, {}, {
        // Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_1, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_2, logic->CanBreakSmallCrates()),
        LOCATION(RC_FOREST_TEMPLE_MQ_FROZEN_EYE_SWITCH_SMALL_CRATE_3, logic->CanBreakSmallCrates()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE);}),
        Entrance(RR_FOREST_TEMPLE_MQ_BETH_ROOM,    []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 6);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Region("Forest Temple MQ Falling Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_NE_COURTYARD_LEDGE_ABOVE_ISLAND, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_AMY_ROOM,                        []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 6);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_AMY_ROOM] = Region("Forest Temple MQ Amy Room", SCENE_FOREST_TEMPLE, {
        //Events
        EventAccess(LOGIC_FOREST_AMY, []{return logic->CanUse(RG_FAIRY_BOW);}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_GREEN_POE_POT_2, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, []{return logic->Get(LOGIC_FOREST_AMY);}),
        Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM,         []{return logic->SmallKeys(SCENE_FOREST_TEMPLE, 6);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY] = Region("Forest Temple MQ Blue Doormat Hallway", SCENE_FOREST_TEMPLE, {}, {}, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_AMY_ROOM, []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,    []{return true;}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BASEMENT] = Region("Forest Temple MQ Basement", SCENE_FOREST_TEMPLE, {
        //Events
        //Implies CanHitSwitch()
        EventAccess(LOGIC_FOREST_OPEN_BOSS_HALLWAY, []{return logic->CanHitEyeTargets();}),
    }, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, true),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,             []{return true;}),
        Entrance(RR_FOREST_TEMPLE_MQ_BASEMENT_POT_ROOM, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->TakeDamage();}),
        Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,        []{return logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY);}),
    });

    areaTable[RR_FOREST_TEMPLE_MQ_BASEMENT_POT_ROOM] = Region("Forest Temple MQ Basement Pot Room", SCENE_FOREST_TEMPLE, {}, {
        //Locations
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_1, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_2, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_3, logic->CanBreakPots()),
        LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_POT_4, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_FOREST_TEMPLE_MQ_BASEMENT, []{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}),
    });

#pragma endregion

    // Boss Room
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] = Region("Forest Temple Boss Entryway", SCENE_FOREST_TEMPLE, {}, {}, {
        // Exits
        Entrance(RR_FOREST_TEMPLE_BASEMENT,    []{return ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla() && logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY);}),
        Entrance(RR_FOREST_TEMPLE_MQ_BASEMENT, []{return ctx->GetDungeon(FOREST_TEMPLE)->IsMQ() && logic->Get(LOGIC_FOREST_OPEN_BOSS_HALLWAY);}),
        Entrance(RR_FOREST_TEMPLE_BOSS_ROOM,   []{return logic->HasItem(RG_FOREST_TEMPLE_BOSS_KEY);}),
    });

    areaTable[RR_FOREST_TEMPLE_BOSS_ROOM] = Region("Forest Temple Boss Room", SCENE_FOREST_TEMPLE_BOSS, {
        // Events
        EventAccess(LOGIC_FOREST_TEMPLE_CLEAR, []{return logic->CanKillEnemy(RE_PHANTOM_GANON);}),
    }, {
        // Locations
        LOCATION(RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, logic->Get(LOGIC_FOREST_TEMPLE_CLEAR)),
        LOCATION(RC_PHANTOM_GANON,                     logic->Get(LOGIC_FOREST_TEMPLE_CLEAR)),
    }, {
        // Exits
        Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, []{return false;}),
        Entrance(RR_SACRED_FOREST_MEADOW,        []{return logic->Get(LOGIC_FOREST_TEMPLE_CLEAR);}, false),
    });

    // clang-format on
}
