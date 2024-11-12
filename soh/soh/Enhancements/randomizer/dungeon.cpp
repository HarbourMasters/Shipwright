#include "dungeon.h"

#include "3drando/pool_functions.hpp"
#include "static_data.h"
#include "context.h"

namespace Rando {
DungeonInfo::DungeonInfo(std::string name_, const RandomizerHintTextKey hintKey_, const RandomizerGet map_,
    const RandomizerGet compass_, const RandomizerGet smallKey_, const RandomizerGet keyRing_,
    const RandomizerGet bossKey_, RandomizerArea area_, const uint8_t vanillaKeyCount_, const uint8_t mqKeyCount_,
    const RandomizerSettingKey mqSetting_,
    std::vector<RandomizerCheck> vanillaLocations_, std::vector<RandomizerCheck> mqLocations_,
    std::vector<RandomizerCheck> sharedLocations_, std::vector<RandomizerCheck> bossRoomLocations_)
    : name(std::move(name_)), hintKey(hintKey_), map(map_), compass(compass_), smallKey(smallKey_), keyRing(keyRing_),
      bossKey(bossKey_), area(area_), vanillaKeyCount(vanillaKeyCount_), mqKeyCount(mqKeyCount_),
      mqSetting(mqSetting_),
      vanillaLocations(std::move(vanillaLocations_)), mqLocations(std::move(mqLocations_)),
      sharedLocations(std::move(sharedLocations_)), bossRoomLocations(std::move(bossRoomLocations_)) {
}
DungeonInfo::DungeonInfo() : hintKey(RHT_NONE), map(RG_NONE), compass(RG_NONE), smallKey(RG_NONE), keyRing(RG_NONE),
    bossKey(RG_NONE) {}
DungeonInfo::~DungeonInfo() = default;

const std::string& DungeonInfo::GetName() const {
    return name;
}

void DungeonInfo::SetMQ() {
    masterQuest = true;
}

void DungeonInfo::ClearMQ() {
    masterQuest = false;
}

bool DungeonInfo::IsMQ() const {
    return masterQuest;
}

void DungeonInfo::SetKeyRing() {
    hasKeyRing = true;
}

void DungeonInfo::ClearKeyRing() {
    hasKeyRing = false;
}

bool DungeonInfo::HasKeyRing() const {
    return hasKeyRing;
}

bool DungeonInfo::IsVanilla() const {
    return !masterQuest;
}

uint8_t DungeonInfo::GetSmallKeyCount() const {
    return masterQuest ? mqKeyCount : vanillaKeyCount;
}

RandomizerHintTextKey DungeonInfo::GetHintKey() const {
    return hintKey;
}

RandomizerArea DungeonInfo::GetArea() const {
    return area;
}

RandomizerGet DungeonInfo::GetSmallKey() const {
    return smallKey;
}

RandomizerGet DungeonInfo::GetKeyRing() const {
    return keyRing;
}

RandomizerGet DungeonInfo::GetMap() const {
    return map;
}

RandomizerGet DungeonInfo::GetCompass() const {
    return compass;
}

RandomizerGet DungeonInfo::GetBossKey() const {
    return bossKey;
}

RandomizerSettingKey DungeonInfo::GetMQSetting() const {
    return mqSetting;
}

void DungeonInfo::SetDungeonKnown(bool known) {
    isDungeonModeKnown = known;
}

void DungeonInfo::PlaceVanillaMap() const {
    if (map == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto mapLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_MAP;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(mapLocation, map);
}

void DungeonInfo::PlaceVanillaCompass() const {
    if (compass == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto compassLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_COMPASS;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(compassLocation, compass);
}

void DungeonInfo::PlaceVanillaBossKey() const {
    if (bossKey == RG_NONE || bossKey == RG_GANONS_CASTLE_BOSS_KEY) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto bossKeyLocation = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_BOSS_KEY;
    })[0];
    Context::GetInstance()->PlaceItemInLocation(bossKeyLocation, bossKey);
}

void DungeonInfo::PlaceVanillaSmallKeys() const {
    if (smallKey == RG_NONE) {
        return;
    }

    auto dungeonLocations = GetDungeonLocations();
    const auto smallKeyLocations = FilterFromPool(dungeonLocations, [](const RandomizerCheck loc) {
        return StaticData::GetLocation(loc)->GetRCType() == RCTYPE_SMALL_KEY;
    });
    for (const auto location : smallKeyLocations) {
        Context::GetInstance()->PlaceItemInLocation(location, smallKey);
    }
}

// Gets the chosen dungeon locations for a playthrough (so either MQ or Vanilla)
std::vector<RandomizerCheck> DungeonInfo::GetDungeonLocations() const {
    auto locations = masterQuest ? mqLocations : vanillaLocations;
    AddElementsToPool(locations, sharedLocations);
    AddElementsToPool(locations, bossRoomLocations);
    return locations;
}

// Gets all dungeon locations (MQ + Vanilla)
std::vector<RandomizerCheck> DungeonInfo::GetEveryLocation() const {
    auto locations = vanillaLocations;
    AddElementsToPool(locations, mqLocations);
    AddElementsToPool(locations, sharedLocations);
    AddElementsToPool(locations, bossRoomLocations);
    return locations;
}

Dungeons::Dungeons() {
    dungeonList[DEKU_TREE] =
        DungeonInfo("Deku Tree", RHT_DEKU_TREE, RG_DEKU_TREE_MAP, RG_DEKU_TREE_COMPASS, RG_NONE, RG_NONE, RG_NONE, RA_DEKU_TREE, 0, 0, RSK_MQ_DEKU_TREE,
                    {
                        // Vanilla Locations
                        RC_DEKU_TREE_MAP_CHEST,
                        RC_DEKU_TREE_COMPASS_CHEST,
                        RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST,
                        RC_DEKU_TREE_BASEMENT_CHEST,
                        RC_DEKU_TREE_SLINGSHOT_CHEST,
                        RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST,
                        RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM,
                        RC_DEKU_TREE_GS_BASEMENT_GATE,
                        RC_DEKU_TREE_GS_BASEMENT_VINES,
                        RC_DEKU_TREE_GS_COMPASS_ROOM,
                    },
                    {
                        // MQ Locations
                        RC_DEKU_TREE_MQ_MAP_CHEST,
                        RC_DEKU_TREE_MQ_COMPASS_CHEST,
                        RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,
                        RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST,
                        RC_DEKU_TREE_MQ_BASEMENT_CHEST,
                        RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST,
                        RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST,
                        RC_DEKU_TREE_MQ_DEKU_SCRUB,
                        RC_DEKU_TREE_MQ_GS_LOBBY,
                        RC_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES,
                        RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM,
                        RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,
                    },
                    {},
                    {
                        // Boss Room Locations
                        RC_DEKU_TREE_QUEEN_GOHMA_HEART,
                        RC_QUEEN_GOHMA,
                    });
    dungeonList[DODONGOS_CAVERN] = DungeonInfo("Dodongo's Cavern", RHT_DODONGOS_CAVERN, RG_DODONGOS_CAVERN_MAP,
                                               RG_DODONGOS_CAVERN_COMPASS, RG_NONE, RG_NONE, RG_NONE, RA_DODONGOS_CAVERN, 0, 0, RSK_MQ_DODONGOS_CAVERN,
                                               {
                                                   // Vanilla Locations
                                                   RC_DODONGOS_CAVERN_MAP_CHEST,
                                                   RC_DODONGOS_CAVERN_COMPASS_CHEST,
                                                   RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST,
                                                   RC_DODONGOS_CAVERN_BOMB_BAG_CHEST,
                                                   RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST,
                                                   RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,
                                                   RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
                                                   RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
                                                   RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
                                                   RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
                                                   RC_DODONGOS_CAVERN_GS_SCARECROW,
                                                   RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
                                                   RC_DODONGOS_CAVERN_GS_BACK_ROOM,
                                                   RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
                                               },
                                               {
                                                   // MQ Locations
                                                   RC_DODONGOS_CAVERN_MQ_MAP_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST,
                                                   RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,
                                                   RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,
                                                   RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,
                                                   RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
                                                   RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,
                                                   RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM,
                                                   RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,
                                                   RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,
                                                   RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,
                                               },
                                               {},
                                               {
                                                   // Boss Room Locations
                                                   RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST,
                                                   RC_DODONGOS_CAVERN_KING_DODONGO_HEART,
                                                   RC_KING_DODONGO,
                                               });
    dungeonList[JABU_JABUS_BELLY] = DungeonInfo("Jabu Jabu's Belly", RHT_JABU_JABUS_BELLY, RG_JABU_JABUS_BELLY_MAP,
                                                RG_JABU_JABUS_BELLY_COMPASS, RG_NONE, RG_NONE, RG_NONE, RA_JABU_JABUS_BELLY, 0, 0, RSK_MQ_JABU_JABU,
                                                {
                                                    // Vanilla Locations
                                                    RC_JABU_JABUS_BELLY_MAP_CHEST,
                                                    RC_JABU_JABUS_BELLY_COMPASS_CHEST,
                                                    RC_JABU_JABUS_BELLY_BOOMERANG_CHEST,
                                                    RC_JABU_JABUS_BELLY_DEKU_SCRUB,
                                                    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
                                                    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
                                                    RC_JABU_JABUS_BELLY_GS_NEAR_BOSS,
                                                    RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
                                                },
                                                {
                                                    // MQ Locations
                                                    RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,
                                                    RC_JABU_JABUS_BELLY_MQ_COW,
                                                    RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,
                                                    RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,
                                                    RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,
                                                    RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,
                                                },
                                                {},
                                                {
                                                    // Boss Room Locations
                                                    RC_JABU_JABUS_BELLY_BARINADE_HEART,
                                                    RC_BARINADE,
                                                });
    dungeonList[FOREST_TEMPLE] =
        DungeonInfo("Forest Temple", RHT_FOREST_TEMPLE, RG_FOREST_TEMPLE_MAP, RG_FOREST_TEMPLE_COMPASS,
                    RG_FOREST_TEMPLE_SMALL_KEY, RG_FOREST_TEMPLE_KEY_RING, RG_FOREST_TEMPLE_BOSS_KEY, RA_FOREST_TEMPLE, 5, 6, RSK_MQ_FOREST_TEMPLE,
                    {
                        // Vanilla Locations
                        RC_FOREST_TEMPLE_FIRST_ROOM_CHEST,
                        RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST,
                        RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
                        RC_FOREST_TEMPLE_MAP_CHEST,
                        RC_FOREST_TEMPLE_WELL_CHEST,
                        RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,
                        RC_FOREST_TEMPLE_EYE_SWITCH_CHEST,
                        RC_FOREST_TEMPLE_BOSS_KEY_CHEST,
                        RC_FOREST_TEMPLE_FLOORMASTER_CHEST,
                        RC_FOREST_TEMPLE_BOW_CHEST,
                        RC_FOREST_TEMPLE_RED_POE_CHEST,
                        RC_FOREST_TEMPLE_BLUE_POE_CHEST,
                        RC_FOREST_TEMPLE_BASEMENT_CHEST,
                        RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
                        RC_FOREST_TEMPLE_GS_FIRST_ROOM,
                        RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
                        RC_FOREST_TEMPLE_GS_LOBBY,
                        RC_FOREST_TEMPLE_GS_BASEMENT,
                    },
                    {
                        // MQ Locations
                        RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST,
                        RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,
                        RC_FOREST_TEMPLE_MQ_BOW_CHEST,
                        RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST,
                        RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST,
                        RC_FOREST_TEMPLE_MQ_WELL_CHEST,
                        RC_FOREST_TEMPLE_MQ_MAP_CHEST,
                        RC_FOREST_TEMPLE_MQ_COMPASS_CHEST,
                        RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST,
                        RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST,
                        RC_FOREST_TEMPLE_MQ_REDEAD_CHEST,
                        RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY,
                        RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM,
                        RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD,
                        RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD,
                        RC_FOREST_TEMPLE_MQ_GS_WELL,
                    },
                    {},
                    {
                        // Boss Room Locations
                        RC_FOREST_TEMPLE_PHANTOM_GANON_HEART,
                        RC_PHANTOM_GANON,
                    });
    dungeonList[FIRE_TEMPLE] =
        DungeonInfo("Fire Temple", RHT_FIRE_TEMPLE, RG_FIRE_TEMPLE_MAP, RG_FIRE_TEMPLE_COMPASS,
                    RG_FIRE_TEMPLE_SMALL_KEY, RG_FIRE_TEMPLE_KEY_RING, RG_FIRE_TEMPLE_BOSS_KEY, RA_FIRE_TEMPLE, 8, 5, RSK_MQ_FIRE_TEMPLE,
                    {
                        // Vanilla Locations
                        RC_FIRE_TEMPLE_NEAR_BOSS_CHEST,
                        RC_FIRE_TEMPLE_FLARE_DANCER_CHEST,
                        RC_FIRE_TEMPLE_BOSS_KEY_CHEST,
                        RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
                        RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
                        RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST,
                        RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST,
                        RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
                        RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST,
                        RC_FIRE_TEMPLE_SCARECROW_CHEST,
                        RC_FIRE_TEMPLE_MAP_CHEST,
                        RC_FIRE_TEMPLE_COMPASS_CHEST,
                        RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST,
                        RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST,
                        RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
                        RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
                        RC_FIRE_TEMPLE_GS_BOULDER_MAZE,
                        RC_FIRE_TEMPLE_GS_SCARECROW_TOP,
                        RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB,
                    },
                    {
                        // MQ Locations
                        RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,
                        RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST,
                        RC_FIRE_TEMPLE_MQ_COMPASS_CHEST,
                        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,
                        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST,
                        RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,
                        RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST,
                        RC_FIRE_TEMPLE_MQ_MAP_CHEST,
                        RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
                        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST,
                        RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY,
                        RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_MAZE,
                        RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,
                        RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,
                        RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM,
                        RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,
                    },
                    {},
                    {
                        // Boos Room Locations
                        RC_FIRE_TEMPLE_VOLVAGIA_HEART,
                        RC_VOLVAGIA,
                    });
    dungeonList[WATER_TEMPLE] =
        DungeonInfo("Water Temple", RHT_WATER_TEMPLE, RG_WATER_TEMPLE_MAP, RG_WATER_TEMPLE_COMPASS,
                    RG_WATER_TEMPLE_SMALL_KEY, RG_WATER_TEMPLE_KEY_RING, RG_WATER_TEMPLE_BOSS_KEY, RA_WATER_TEMPLE, 6, 2, RSK_MQ_WATER_TEMPLE,
                    {
                        // Vanilla Locations
                        RC_WATER_TEMPLE_MAP_CHEST,
                        RC_WATER_TEMPLE_COMPASS_CHEST,
                        RC_WATER_TEMPLE_TORCHES_CHEST,
                        RC_WATER_TEMPLE_DRAGON_CHEST,
                        RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
                        RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
                        RC_WATER_TEMPLE_CRACKED_WALL_CHEST,
                        RC_WATER_TEMPLE_BOSS_KEY_CHEST,
                        RC_WATER_TEMPLE_LONGSHOT_CHEST,
                        RC_WATER_TEMPLE_RIVER_CHEST,
                        RC_WATER_TEMPLE_GS_BEHIND_GATE,
                        RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
                        RC_WATER_TEMPLE_GS_CENTRAL_PILLAR,
                        RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
                        RC_WATER_TEMPLE_GS_RIVER,
                    },
                    {
                        // MQ Locations
                        RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST,
                        RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST,
                        RC_WATER_TEMPLE_MQ_COMPASS_CHEST,
                        RC_WATER_TEMPLE_MQ_MAP_CHEST,
                        RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,
                        RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH,
                        RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA,
                        RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,
                        RC_WATER_TEMPLE_MQ_GS_RIVER,
                        RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,
                    },
                    {},
                    {
                        // Boss Room Locations
                        RC_WATER_TEMPLE_MORPHA_HEART,
                        RC_MORPHA,
                    });
    dungeonList[SPIRIT_TEMPLE] =
        DungeonInfo("Spirit Temple", RHT_SPIRIT_TEMPLE, RG_SPIRIT_TEMPLE_MAP, RG_SPIRIT_TEMPLE_COMPASS,
                    RG_SPIRIT_TEMPLE_SMALL_KEY, RG_SPIRIT_TEMPLE_KEY_RING, RG_SPIRIT_TEMPLE_BOSS_KEY, RA_SPIRIT_TEMPLE, 5, 7, RSK_MQ_SPIRIT_TEMPLE,
                    {
                        // Vanilla Locations
                        RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,
                        RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
                        RC_SPIRIT_TEMPLE_COMPASS_CHEST,
                        RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
                        RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,
                        RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST,
                        RC_SPIRIT_TEMPLE_MAP_CHEST,
                        RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST,
                        RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,
                        RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
                        RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
                        RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,
                        RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
                        RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,
                        RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST,
                        RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST,
                        RC_SPIRIT_TEMPLE_TOPMOST_CHEST,
                        RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
                        RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,
                        RC_SPIRIT_TEMPLE_GS_LOBBY,
                        RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,
                        RC_SPIRIT_TEMPLE_GS_METAL_FENCE,
                    },
                    {
                        // MQ Locations
                        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_MAP_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST,
                        RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,
                        RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,
                        RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,
                        RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,
                        RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,
                    },
                    {
                        // Shared Locations
                        RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
                        RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,
                    },
                    {
                        // Boss Room Locations
                        RC_SPIRIT_TEMPLE_TWINROVA_HEART,
                        RC_TWINROVA,
                    });
    dungeonList[SHADOW_TEMPLE] =
        DungeonInfo("Shadow Temple", RHT_SHADOW_TEMPLE, RG_SHADOW_TEMPLE_MAP, RG_SHADOW_TEMPLE_COMPASS,
                    RG_SHADOW_TEMPLE_SMALL_KEY, RG_SHADOW_TEMPLE_KEY_RING, RG_SHADOW_TEMPLE_BOSS_KEY, RA_SHADOW_TEMPLE, 5, 6, RSK_MQ_SHADOW_TEMPLE,
                    {
                        // Vanilla Locations
                        RC_SHADOW_TEMPLE_MAP_CHEST,
                        RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST,
                        RC_SHADOW_TEMPLE_COMPASS_CHEST,
                        RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST,
                        RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,
                        RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST,
                        RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,
                        RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,
                        RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,
                        RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,
                        RC_SHADOW_TEMPLE_WIND_HINT_CHEST,
                        RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,
                        RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
                        RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,
                        RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,
                        RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
                        RC_SHADOW_TEMPLE_FREESTANDING_KEY,
                        RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
                        RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
                        RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
                        RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
                        RC_SHADOW_TEMPLE_GS_NEAR_SHIP,
                    },
                    {
                        // MQ Locations
                        RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,
                        RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST,
                        RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,
                        RC_SHADOW_TEMPLE_MQ_MAP_CHEST,
                        RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,
                        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST,
                        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,
                        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,
                        RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,
                        RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST,
                        RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,
                        RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST,
                        RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,
                        RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,
                        RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,
                        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST,
                        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,
                        RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST,
                        RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,
                        RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,
                        RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,
                        RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,
                        RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,
                        RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,
                    },
                    {},
                    {
                        // Boss Room Locations
                        RC_SHADOW_TEMPLE_BONGO_BONGO_HEART,
                        RC_BONGO_BONGO,
                    });
    dungeonList[BOTTOM_OF_THE_WELL] = DungeonInfo(
        "Bottom of the Well", RHT_BOTTOM_OF_THE_WELL, RG_BOTTOM_OF_THE_WELL_MAP, RG_BOTTOM_OF_THE_WELL_COMPASS,
        RG_BOTTOM_OF_THE_WELL_SMALL_KEY, RG_BOTTOM_OF_THE_WELL_KEY_RING, RG_NONE, RA_BOTTOM_OF_THE_WELL, 3, 2, RSK_MQ_BOTTOM_OF_THE_WELL,
        {
            // Vanilla Locations
            RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
            RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,
            RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
            RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,
            RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,
            RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,
            RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
            RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,
            RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,
            RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,
            RC_BOTTOM_OF_THE_WELL_MAP_CHEST,
            RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,
            RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,
            RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
            RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
            RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
            RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
        },
        {
            // MQ Locations
            RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,
            RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,
            RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,
            RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY,
            RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY,
            RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,
            RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,
            RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,
        },
        {}, {});
    dungeonList[ICE_CAVERN] = DungeonInfo("Ice Cavern", RHT_ICE_CAVERN, RG_ICE_CAVERN_MAP, RG_ICE_CAVERN_COMPASS,
                                          RG_NONE, RG_NONE, RG_NONE, RA_ICE_CAVERN, 0, 0, RSK_MQ_ICE_CAVERN,
                                          {
                                              // Vanilla Locations
                                              RC_ICE_CAVERN_MAP_CHEST,
                                              RC_ICE_CAVERN_COMPASS_CHEST,
                                              RC_ICE_CAVERN_IRON_BOOTS_CHEST,
                                              RC_ICE_CAVERN_FREESTANDING_POH,
                                              RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,
                                              RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
                                              RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,
                                          },
                                          {
                                              // MQ Locations
                                              RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST,
                                              RC_ICE_CAVERN_MQ_COMPASS_CHEST,
                                              RC_ICE_CAVERN_MQ_MAP_CHEST,
                                              RC_ICE_CAVERN_MQ_FREESTANDING_POH,
                                              RC_ICE_CAVERN_MQ_GS_SCARECROW,
                                              RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,
                                              RC_ICE_CAVERN_MQ_GS_RED_ICE,
                                          },
                                          {
                                              // Shared Locations
                                              RC_SHEIK_IN_ICE_CAVERN,
                                          },
                                          {});
    dungeonList[GERUDO_TRAINING_GROUNDS] =
        DungeonInfo("Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, RG_NONE, RG_NONE,
                    RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, RG_GERUDO_TRAINING_GROUNDS_KEY_RING, RG_NONE, RA_GERUDO_TRAINING_GROUND, 9, 3, RSK_MQ_GTG,
                    {
                        // Vanilla Locations
                        RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,
                        RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST,
                        RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,
                        RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST,
                        RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST,
                        RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST,
                        RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,
                    },
                    {
                        // MQ Locations
                        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST,
                        RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST,
                    },
                    {}, {});
    dungeonList[GANONS_CASTLE] =
        DungeonInfo("Ganon's Castle", RHT_GANONS_CASTLE, RG_NONE, RG_NONE, RG_GANONS_CASTLE_SMALL_KEY,
                    RG_GANONS_CASTLE_KEY_RING, RG_GANONS_CASTLE_BOSS_KEY, RA_GANONS_CASTLE, 2, 3, RSK_MQ_GANONS_CASTLE,
                    {
                        // Vanilla Locations
                        RC_GANONS_CASTLE_FOREST_TRIAL_CHEST,
                        RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,
                        RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST,
                        RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,
                        RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST,
                        RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST,
                        RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
                        RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
                        RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,
                        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,
                        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,
                        RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,
                    },
                    {
                        // MQ Locations
                        RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST,
                        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,
                        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST,
                        RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST,
                        RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST,
                        RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,
                        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,
                        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,
                        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,
                        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,
                        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,
                        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,
                    },
                    {
                        // Shared Locations
                        RC_GANONS_TOWER_BOSS_KEY_CHEST,
                        RC_GANON,
                    },
                    {});
}

Dungeons::~Dungeons() = default;

DungeonInfo* Dungeons::GetDungeon(const DungeonKey key) {
    return &dungeonList[key];
}

DungeonInfo* Dungeons::GetDungeonFromScene(const uint16_t scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
            return &dungeonList[DEKU_TREE];
        case SCENE_DODONGOS_CAVERN:
            return &dungeonList[DODONGOS_CAVERN];
        case SCENE_JABU_JABU:
            return &dungeonList[JABU_JABUS_BELLY];
        case SCENE_FOREST_TEMPLE:
            return &dungeonList[FOREST_TEMPLE];
        case SCENE_FIRE_TEMPLE:
            return &dungeonList[FIRE_TEMPLE];
        case SCENE_WATER_TEMPLE:
            return &dungeonList[WATER_TEMPLE];
        case SCENE_SPIRIT_TEMPLE:
            return &dungeonList[SPIRIT_TEMPLE];
        case SCENE_SHADOW_TEMPLE:
            return &dungeonList[SHADOW_TEMPLE];
        case SCENE_BOTTOM_OF_THE_WELL:
            return &dungeonList[BOTTOM_OF_THE_WELL];
        case SCENE_ICE_CAVERN:
            return &dungeonList[ICE_CAVERN];
        case SCENE_GERUDO_TRAINING_GROUND:
            return &dungeonList[GERUDO_TRAINING_GROUNDS];
        case SCENE_INSIDE_GANONS_CASTLE:
            return &dungeonList[GANONS_CASTLE];
        default:
            return nullptr;
    }
}

size_t Dungeons::CountMQ() {
    size_t count = 0;
    for (DungeonInfo& dungeon : dungeonList) {
        if (dungeon.IsMQ()) {
            count++;
        }
    }
    return count;
}

void Dungeons::ClearAllMQ() {
    for (DungeonInfo& dungeon : dungeonList) {
        dungeon.ClearMQ();
    }
}

std::array<DungeonInfo*, 12> Dungeons::GetDungeonList() {
    std::array<DungeonInfo*, 12> dungeonList_{};
    for (size_t i = 0; i < dungeonList.size(); i++) {
        dungeonList_[i] = &dungeonList[i];
    }
    return dungeonList_;
}

size_t Dungeons::GetDungeonListSize() const {
    return dungeonList.size();
}
void Dungeons::ParseJson(nlohmann::json spoilerFileJson) {
    nlohmann::json mqDungeonsJson = spoilerFileJson["masterQuestDungeons"];
    for (auto it = mqDungeonsJson.begin(); it != mqDungeonsJson.end(); ++it) {
        std::string dungeonName = it.value().get<std::string>();
        for (auto& dungeon : dungeonList) {
            if (dungeon.GetName() == dungeonName) {
                dungeon.SetMQ();
            } else {
                dungeon.ClearMQ();
            }
        }
    }
}
} // namespace Rando