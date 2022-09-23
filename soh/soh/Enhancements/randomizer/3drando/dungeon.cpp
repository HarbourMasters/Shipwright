#include "dungeon.hpp"

#include "category.hpp"
#include "item_location.hpp"
#include "pool_functions.hpp"
#include "keys.hpp"

namespace Dungeon {

DungeonInfo::DungeonInfo(std::string name_, uint32_t map_, uint32_t compass_, uint32_t smallKey_, uint32_t keyRing_,
                         uint32_t bossKey_, uint8_t vanillaKeyCount_, uint8_t mqKeyCount_,
                         std::vector<uint32_t> vanillaLocations_,
                         std::vector<uint32_t> mqLocations_,
                         std::vector<uint32_t> sharedLocations_)
  : name(std::move(name_)),
    map(map_),
    compass(compass_),
    smallKey(smallKey_),
    keyRing(keyRing_),
    bossKey(bossKey_),
    vanillaKeyCount(vanillaKeyCount_),
    mqKeyCount(mqKeyCount_),
    vanillaLocations(std::move(vanillaLocations_)),
    mqLocations(std::move(mqLocations_)),
    sharedLocations(std::move(sharedLocations_)) {}

DungeonInfo::~DungeonInfo() = default;

uint32_t DungeonInfo::GetSmallKey() const {
  return smallKey;
}

uint32_t DungeonInfo::GetKeyRing() const {
  return keyRing;
}

uint32_t DungeonInfo::GetMap() const {
  return map;
}

uint32_t DungeonInfo::GetCompass() const {
  return compass;
}

uint32_t DungeonInfo::GetBossKey() const {
  return bossKey;
}

void DungeonInfo::PlaceVanillaMap() {
  if (map == NONE) {
    return;
  }

  auto dungeonLocations = GetDungeonLocations();
  auto mapLocation = FilterFromPool(dungeonLocations, [](const uint32_t loc){ return Location(loc)->IsCategory(Category::cVanillaMap); })[0];
  PlaceItemInLocation(mapLocation, map);
}

void DungeonInfo::PlaceVanillaCompass() {
  if (compass == NONE) {
    return;
  }

  auto dungeonLocations = GetDungeonLocations();
  auto compassLocation = FilterFromPool(dungeonLocations, [](const uint32_t loc){ return Location(loc)->IsCategory(Category::cVanillaCompass); })[0];
  PlaceItemInLocation(compassLocation, compass);
}

void DungeonInfo::PlaceVanillaBossKey() {
  if (bossKey == NONE || bossKey == GANONS_CASTLE_BOSS_KEY) {
    return;
  }

  auto dungeonLocations = GetDungeonLocations();
  auto bossKeyLocation = FilterFromPool(dungeonLocations, [](const uint32_t loc){ return Location(loc)->IsCategory(Category::cVanillaBossKey); })[0];
  PlaceItemInLocation(bossKeyLocation, bossKey);
}

void DungeonInfo::PlaceVanillaSmallKeys() {
  if (smallKey == NONE) {
    return;
  }

  auto dungeonLocations = GetDungeonLocations();
  auto smallKeyLocations = FilterFromPool(dungeonLocations, [](const uint32_t loc){ return Location(loc)->IsCategory(Category::cVanillaSmallKey); });
  for (auto location : smallKeyLocations) {
      PlaceItemInLocation(location, smallKey);
  }
}

//Gets the chosen dungeon locations for a playthrough (so either MQ or Vanilla)
std::vector<uint32_t> DungeonInfo::GetDungeonLocations() const {
  auto locations = masterQuest ? mqLocations : vanillaLocations;
  AddElementsToPool(locations, sharedLocations);
  return locations;
}

//Gets all dungeon locations (MQ + Vanilla)
std::vector<uint32_t> DungeonInfo::GetEveryLocation() const {
  auto locations = vanillaLocations;
  AddElementsToPool(locations, mqLocations);
  AddElementsToPool(locations, sharedLocations);
  return locations;
}

  DungeonInfo DekuTree = DungeonInfo("Deku Tree", DEKU_TREE_MAP, DEKU_TREE_COMPASS, NONE, NONE, NONE, 0, 0, {
                            //Vanilla Locations
                            DEKU_TREE_MAP_CHEST,
                            DEKU_TREE_COMPASS_CHEST,
                            DEKU_TREE_COMPASS_ROOM_SIDE_CHEST,
                            DEKU_TREE_BASEMENT_CHEST,
                            DEKU_TREE_SLINGSHOT_CHEST,
                            DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST,
                            DEKU_TREE_GS_BASEMENT_BACK_ROOM,
                            DEKU_TREE_GS_BASEMENT_GATE,
                            DEKU_TREE_GS_BASEMENT_VINES,
                            DEKU_TREE_GS_COMPASS_ROOM,
                          }, {
                            //MQ Locations
                            DEKU_TREE_MQ_MAP_CHEST,
                            DEKU_TREE_MQ_COMPASS_CHEST,
                            DEKU_TREE_MQ_SLINGSHOT_CHEST,
                            DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST,
                            DEKU_TREE_MQ_BASEMENT_CHEST,
                            DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST,
                            DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST,
                            DEKU_TREE_MQ_DEKU_SCRUB,
                            DEKU_TREE_MQ_GS_LOBBY,
                            DEKU_TREE_MQ_GS_COMPASS_ROOM,
                            DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM,
                            DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,
                          }, {
                            //Shared Locations
                            DEKU_TREE_QUEEN_GOHMA_HEART,
                            QUEEN_GOHMA,
                          });

  DungeonInfo DodongosCavern = DungeonInfo("Dodongo's Cavern", DODONGOS_CAVERN_MAP, DODONGOS_CAVERN_COMPASS, NONE, NONE, NONE, 0, 0, {
                            //Vanilla Locations
                            DODONGOS_CAVERN_MAP_CHEST,
                            DODONGOS_CAVERN_COMPASS_CHEST,
                            DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST,
                            DODONGOS_CAVERN_BOMB_BAG_CHEST,
                            DODONGOS_CAVERN_END_OF_BRIDGE_CHEST,
                            DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,
                            DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
                            DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
                            DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
                            DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
                            DODONGOS_CAVERN_GS_SCARECROW,
                            DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
                            DODONGOS_CAVERN_GS_BACK_ROOM,
                            DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
                          }, {
                            //MQ Locations
                            DODONGOS_CAVERN_MQ_MAP_CHEST,
                            DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST,
                            DODONGOS_CAVERN_MQ_COMPASS_CHEST,
                            DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,
                            DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,
                            DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST,
                            DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,
                            DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,
                            DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,
                            DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
                            DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,
                            DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM,
                            DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,
                            DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,
                            DODONGOS_CAVERN_MQ_GS_BACK_AREA,
                          }, {
                            //Shared Locations
                            DODONGOS_CAVERN_BOSS_ROOM_CHEST,
                            DODONGOS_CAVERN_KING_DODONGO_HEART,
                            KING_DODONGO,
                          });

  DungeonInfo JabuJabusBelly = DungeonInfo("Jabu Jabu's Belly", JABU_JABUS_BELLY_MAP, JABU_JABUS_BELLY_COMPASS, NONE, NONE, NONE, 0, 0, {
                            //Vanilla Locations
                            JABU_JABUS_BELLY_MAP_CHEST,
                            JABU_JABUS_BELLY_COMPASS_CHEST,
                            JABU_JABUS_BELLY_BOOMERANG_CHEST,
                            JABU_JABUS_BELLY_DEKU_SCRUB,
                            JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
                            JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
                            JABU_JABUS_BELLY_GS_NEAR_BOSS,
                            JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
                          }, {
                            //MQ Locations
                            JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST,
                            JABU_JABUS_BELLY_MQ_MAP_CHEST,
                            JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,
                            JABU_JABUS_BELLY_MQ_COMPASS_CHEST,
                            JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,
                            JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST,
                            JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,
                            JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST,
                            JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST,
                            JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,
                            JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,
                            JABU_JABUS_BELLY_MQ_COW,
                            JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,
                            JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,
                            JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,
                            JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,
                          }, {
                            //Shared Locations
                            JABU_JABUS_BELLY_BARINADE_HEART,
                            BARINADE,
                          });

  DungeonInfo ForestTemple = DungeonInfo("Forest Temple", FOREST_TEMPLE_MAP, FOREST_TEMPLE_COMPASS, FOREST_TEMPLE_SMALL_KEY, FOREST_TEMPLE_KEY_RING, FOREST_TEMPLE_BOSS_KEY, 5, 6, {
                            //Vanilla Locations
                            FOREST_TEMPLE_FIRST_ROOM_CHEST,
                            FOREST_TEMPLE_FIRST_STALFOS_CHEST,
                            FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
                            FOREST_TEMPLE_MAP_CHEST,
                            FOREST_TEMPLE_WELL_CHEST,
                            FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,
                            FOREST_TEMPLE_EYE_SWITCH_CHEST,
                            FOREST_TEMPLE_BOSS_KEY_CHEST,
                            FOREST_TEMPLE_FLOORMASTER_CHEST,
                            FOREST_TEMPLE_BOW_CHEST,
                            FOREST_TEMPLE_RED_POE_CHEST,
                            FOREST_TEMPLE_BLUE_POE_CHEST,
                            FOREST_TEMPLE_BASEMENT_CHEST,
                            FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
                            FOREST_TEMPLE_GS_FIRST_ROOM,
                            FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
                            FOREST_TEMPLE_GS_LOBBY,
                            FOREST_TEMPLE_GS_BASEMENT,
                          }, {
                            //MQ Locations
                            FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST,
                            FOREST_TEMPLE_MQ_WOLFOS_CHEST,
                            FOREST_TEMPLE_MQ_BOW_CHEST,
                            FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST,
                            FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST,
                            FOREST_TEMPLE_MQ_WELL_CHEST,
                            FOREST_TEMPLE_MQ_MAP_CHEST,
                            FOREST_TEMPLE_MQ_COMPASS_CHEST,
                            FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST,
                            FOREST_TEMPLE_MQ_BASEMENT_CHEST,
                            FOREST_TEMPLE_MQ_REDEAD_CHEST,
                            FOREST_TEMPLE_MQ_BOSS_KEY_CHEST,
                            FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY,
                            FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM,
                            FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD,
                            FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD,
                            FOREST_TEMPLE_MQ_GS_WELL,
                          }, {
                            //Shared Locations
                            FOREST_TEMPLE_PHANTOM_GANON_HEART,
                            PHANTOM_GANON,
                          });

  DungeonInfo FireTemple = DungeonInfo("Fire Temple", FIRE_TEMPLE_MAP, FIRE_TEMPLE_COMPASS, FIRE_TEMPLE_SMALL_KEY, FIRE_TEMPLE_KEY_RING, FIRE_TEMPLE_BOSS_KEY, 8, 5, {
                            //Vanilla Locations
                            FIRE_TEMPLE_NEAR_BOSS_CHEST,
                            FIRE_TEMPLE_FLARE_DANCER_CHEST,
                            FIRE_TEMPLE_BOSS_KEY_CHEST,
                            FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
                            FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
                            FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST,
                            FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST,
                            FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
                            FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST,
                            FIRE_TEMPLE_SCARECROW_CHEST,
                            FIRE_TEMPLE_MAP_CHEST,
                            FIRE_TEMPLE_COMPASS_CHEST,
                            FIRE_TEMPLE_HIGHEST_GORON_CHEST,
                            FIRE_TEMPLE_MEGATON_HAMMER_CHEST,
                            FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
                            FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
                            FIRE_TEMPLE_GS_BOULDER_MAZE,
                            FIRE_TEMPLE_GS_SCARECROW_TOP,
                            FIRE_TEMPLE_GS_SCARECROW_CLIMB,
                          }, {
                            //MQ Locations
                            FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,
                            FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST,
                            FIRE_TEMPLE_MQ_COMPASS_CHEST,
                            FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,
                            FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST,
                            FIRE_TEMPLE_MQ_CHEST_ON_FIRE,
                            FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST,
                            FIRE_TEMPLE_MQ_MAP_CHEST,
                            FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,
                            FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
                            FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST,
                            FIRE_TEMPLE_MQ_FREESTANDING_KEY,
                            FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,
                            FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,
                            FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,
                            FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM,
                            FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,
                          }, {
                            //Shared Locations
                            FIRE_TEMPLE_VOLVAGIA_HEART,
                            VOLVAGIA,
                          });

  DungeonInfo WaterTemple = DungeonInfo("Water Temple", WATER_TEMPLE_MAP, WATER_TEMPLE_COMPASS, WATER_TEMPLE_SMALL_KEY, WATER_TEMPLE_KEY_RING, WATER_TEMPLE_BOSS_KEY, 6, 2, {
                            //Vanilla Locations
                            WATER_TEMPLE_MAP_CHEST,
                            WATER_TEMPLE_COMPASS_CHEST,
                            WATER_TEMPLE_TORCHES_CHEST,
                            WATER_TEMPLE_DRAGON_CHEST,
                            WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
                            WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
                            WATER_TEMPLE_CRACKED_WALL_CHEST,
                            WATER_TEMPLE_BOSS_KEY_CHEST,
                            WATER_TEMPLE_LONGSHOT_CHEST,
                            WATER_TEMPLE_RIVER_CHEST,
                            WATER_TEMPLE_GS_BEHIND_GATE,
                            WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
                            WATER_TEMPLE_GS_CENTRAL_PILLAR,
                            WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
                            WATER_TEMPLE_GS_RIVER,
                          }, {
                            //MQ Locations
                            WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST,
                            WATER_TEMPLE_MQ_BOSS_KEY_CHEST,
                            WATER_TEMPLE_MQ_LONGSHOT_CHEST,
                            WATER_TEMPLE_MQ_COMPASS_CHEST,
                            WATER_TEMPLE_MQ_MAP_CHEST,
                            WATER_TEMPLE_MQ_FREESTANDING_KEY,
                            WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH,
                            WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA,
                            WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,
                            WATER_TEMPLE_MQ_GS_RIVER,
                            WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,
                          }, {
                            //Shared Locations
                            WATER_TEMPLE_MORPHA_HEART,
                            MORPHA,
                          });

  DungeonInfo SpiritTemple = DungeonInfo("Spirit Temple", SPIRIT_TEMPLE_MAP, SPIRIT_TEMPLE_COMPASS, SPIRIT_TEMPLE_SMALL_KEY, SPIRIT_TEMPLE_KEY_RING, SPIRIT_TEMPLE_BOSS_KEY, 5, 7, {
                            //Vanilla Locations
                            SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,
                            SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
                            SPIRIT_TEMPLE_COMPASS_CHEST,
                            SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
                            SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,
                            SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST,
                            SPIRIT_TEMPLE_MAP_CHEST,
                            SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST,
                            SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,
                            SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
                            SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
                            SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,
                            SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
                            SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,
                            SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST,
                            SPIRIT_TEMPLE_BOSS_KEY_CHEST,
                            SPIRIT_TEMPLE_TOPMOST_CHEST,
                            SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
                            SPIRIT_TEMPLE_GS_BOULDER_ROOM,
                            SPIRIT_TEMPLE_GS_LOBBY,
                            SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,
                            SPIRIT_TEMPLE_GS_METAL_FENCE,
                          }, {
                            //MQ Locations
                            SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST,
                            SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST,
                            SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST,
                            SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,
                            SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,
                            SPIRIT_TEMPLE_MQ_MAP_CHEST,
                            SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,
                            SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST,
                            SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,
                            SPIRIT_TEMPLE_MQ_COMPASS_CHEST,
                            SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,
                            SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST,
                            SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST,
                            SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,
                            SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,
                            SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,
                            SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,
                            SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,
                            SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,
                            SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST,
                            SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,
                            SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,
                            SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,
                            SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,
                            SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,
                          }, {
                            //Shared Locations
                            SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
                            SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,
                            SPIRIT_TEMPLE_TWINROVA_HEART,
                            TWINROVA,
                          });

  DungeonInfo ShadowTemple = DungeonInfo("Shadow Temple", SHADOW_TEMPLE_MAP, SHADOW_TEMPLE_COMPASS, SHADOW_TEMPLE_SMALL_KEY, SHADOW_TEMPLE_KEY_RING, SHADOW_TEMPLE_BOSS_KEY, 5, 6, {
                            //Vanilla Locations
                            SHADOW_TEMPLE_MAP_CHEST,
                            SHADOW_TEMPLE_HOVER_BOOTS_CHEST,
                            SHADOW_TEMPLE_COMPASS_CHEST,
                            SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST,
                            SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,
                            SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST,
                            SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,
                            SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,
                            SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,
                            SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,
                            SHADOW_TEMPLE_WIND_HINT_CHEST,
                            SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,
                            SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
                            SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,
                            SHADOW_TEMPLE_BOSS_KEY_CHEST,
                            SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
                            SHADOW_TEMPLE_FREESTANDING_KEY,
                            SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
                            SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
                            SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
                            SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
                            SHADOW_TEMPLE_GS_NEAR_SHIP,
                          }, {
                            //MQ Locations
                            SHADOW_TEMPLE_MQ_COMPASS_CHEST,
                            SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST,
                            SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,
                            SHADOW_TEMPLE_MQ_MAP_CHEST,
                            SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,
                            SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST,
                            SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,
                            SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,
                            SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,
                            SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,
                            SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST,
                            SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,
                            SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST,
                            SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,
                            SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,
                            SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,
                            SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST,
                            SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,
                            SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST,
                            SHADOW_TEMPLE_MQ_FREESTANDING_KEY,
                            SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,
                            SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,
                            SHADOW_TEMPLE_MQ_GS_AFTER_WIND,
                            SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,
                            SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,
                          }, {
                            //Shared Locations
                            SHADOW_TEMPLE_BONGO_BONGO_HEART,
                            BONGO_BONGO,
                          });

  DungeonInfo BottomOfTheWell = DungeonInfo("Bottom of the Well", BOTTOM_OF_THE_WELL_MAP, BOTTOM_OF_THE_WELL_COMPASS, BOTTOM_OF_THE_WELL_SMALL_KEY, BOTTOM_OF_THE_WELL_KEY_RING, NONE, 3, 2, {
                            //Vanilla Locations
                            BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
                            BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,
                            BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
                            BOTTOM_OF_THE_WELL_COMPASS_CHEST,
                            BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,
                            BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,
                            BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
                            BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,
                            BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,
                            BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,
                            BOTTOM_OF_THE_WELL_MAP_CHEST,
                            BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,
                            BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,
                            BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
                            BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
                            BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
                            BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
                          }, {
                            //MQ Locations
                            BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,
                            BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,
                            BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,
                            BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY,
                            BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY,
                            BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,
                            BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,
                            BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,
                          }, {});

  DungeonInfo IceCavern = DungeonInfo("Ice Cavern", ICE_CAVERN_MAP, ICE_CAVERN_COMPASS, NONE, NONE, NONE, 0, 0, {
                            //Vanilla Locations
                            ICE_CAVERN_MAP_CHEST,
                            ICE_CAVERN_COMPASS_CHEST,
                            ICE_CAVERN_IRON_BOOTS_CHEST,
                            ICE_CAVERN_FREESTANDING_POH,
                            ICE_CAVERN_GS_PUSH_BLOCK_ROOM,
                            ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
                            ICE_CAVERN_GS_HEART_PIECE_ROOM,
                          }, {
                            //MQ Locations
                            ICE_CAVERN_MQ_IRON_BOOTS_CHEST,
                            ICE_CAVERN_MQ_COMPASS_CHEST,
                            ICE_CAVERN_MQ_MAP_CHEST,
                            ICE_CAVERN_MQ_FREESTANDING_POH,
                            ICE_CAVERN_MQ_GS_SCARECROW,
                            ICE_CAVERN_MQ_GS_ICE_BLOCK,
                            ICE_CAVERN_MQ_GS_RED_ICE,
                          }, {
                            //Shared Locations
                            SHEIK_IN_ICE_CAVERN,
                          });

  DungeonInfo GerudoTrainingGrounds = DungeonInfo("Gerudo Training Grounds", NONE, NONE, GERUDO_TRAINING_GROUNDS_SMALL_KEY, GERUDO_TRAINING_GROUNDS_KEY_RING, NONE, 9, 3, {
                            //Vanilla Locations
                            GERUDO_TRAINING_GROUNDS_LOBBY_LEFT_CHEST,
                            GERUDO_TRAINING_GROUNDS_LOBBY_RIGHT_CHEST,
                            GERUDO_TRAINING_GROUNDS_STALFOS_CHEST,
                            GERUDO_TRAINING_GROUNDS_BEAMOS_CHEST,
                            GERUDO_TRAINING_GROUNDS_HIDDEN_CEILING_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_PATH_FIRST_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_PATH_SECOND_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_PATH_THIRD_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_PATH_FINAL_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_CENTRAL_CHEST,
                            GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_SIDE_CHEST,
                            GERUDO_TRAINING_GROUNDS_UNDERWATER_SILVER_RUPEE_CHEST,
                            GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_CLEAR_CHEST,
                            GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_SWITCH_CHEST,
                            GERUDO_TRAINING_GROUNDS_EYE_STATUE_CHEST,
                            GERUDO_TRAINING_GROUNDS_NEAR_SCARECROW_CHEST,
                            GERUDO_TRAINING_GROUNDS_BEFORE_HEAVY_BLOCK_CHEST,
                            GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FIRST_CHEST,
                            GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_SECOND_CHEST,
                            GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_THIRD_CHEST,
                            GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FOURTH_CHEST,
                            GERUDO_TRAINING_GROUNDS_FREESTANDING_KEY,
                          }, {
                            //MQ Locations
                            GERUDO_TRAINING_GROUNDS_MQ_LOBBY_RIGHT_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_LOBBY_LEFT_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_FIRST_IRON_KNUCKLE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_BEFORE_HEAVY_BLOCK_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_EYE_STATUE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_FLAME_CIRCLE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_SECOND_IRON_KNUCKLE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_DINOLFOS_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_ICE_ARROWS_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_CENTRAL_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_FIRST_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_SIDE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_THIRD_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_SECOND_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_HIDDEN_CEILING_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER_SILVER_RUPEE_CHEST,
                            GERUDO_TRAINING_GROUNDS_MQ_HEAVY_BLOCK_CHEST,
                          }, {});

  DungeonInfo GanonsCastle = DungeonInfo("Ganon's Castle", NONE, NONE, GANONS_CASTLE_SMALL_KEY, GANONS_CASTLE_KEY_RING, GANONS_CASTLE_BOSS_KEY, 2, 3, {
                            //Vanilla Locations
                            GANONS_CASTLE_FOREST_TRIAL_CHEST,
                            GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,
                            GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST,
                            GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,
                            GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST,
                            GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST,
                            GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
                            GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
                            GANONS_CASTLE_DEKU_SCRUB_LEFT,
                            GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,
                            GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,
                            GANONS_CASTLE_DEKU_SCRUB_RIGHT,
                          }, {
                            //MQ Locations
                            GANONS_CASTLE_MQ_WATER_TRIAL_CHEST,
                            GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,
                            GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST,
                            GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST,
                            GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST,
                            GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,
                            GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,
                            GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,
                            GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,
                            GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,
                            GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,
                            GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,
                            GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,
                          }, {
                            //Shared Locations
                            GANONS_TOWER_BOSS_KEY_CHEST,
                            GANON,
                          });

  const DungeonArray dungeonList = {
    &DekuTree,
    &DodongosCavern,
    &JabuJabusBelly,
    &ForestTemple,
    &FireTemple,
    &WaterTemple,
    &SpiritTemple,
    &ShadowTemple,
    &BottomOfTheWell,
    &IceCavern,
    &GerudoTrainingGrounds,
    &GanonsCastle,
  };

} //namespace Dungeon
