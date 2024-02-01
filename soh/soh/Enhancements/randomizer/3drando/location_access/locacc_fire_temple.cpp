#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_FireTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[FIRE_TEMPLE_ENTRYWAY] = Area("Fire Temple Entryway", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return Dungeon::FireTemple.IsVanilla();}}),
                  Entrance(FIRE_TEMPLE_MQ_LOWER,   {[]{return Dungeon::FireTemple.IsMQ();}}),
                  Entrance(DMC_CENTRAL_LOCAL,      {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::FireTemple.IsVanilla()) {
  areaTable[FIRE_TEMPLE_FIRST_ROOM] = Area("Fire Temple First Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_NEAR_BOSS_ROOM, {[]{return FireTimer >= 24;}}),
                  Entrance(FIRE_TEMPLE_LOOP_ENEMIES,   {[]{return Here(FIRE_TEMPLE_FIRST_ROOM, []{return CanUse(MEGATON_HAMMER);}) && (SmallKeys(FIRE_TEMPLE, 8) || !IsKeysanity);}}),
                  Entrance(FIRE_TEMPLE_LOOP_EXIT,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM,  {[]{return SmallKeys(FIRE_TEMPLE, 2) && FireTimer >= 24;}}),
  });

  areaTable[FIRE_TEMPLE_NEAR_BOSS_ROOM] = Area("Fire Temple Near Boss Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_NEAR_BOSS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyFireTemple && ((IsAdult && (LogicFireBossDoorJump || Here(FIRE_TEMPLE_FIRE_MAZE_UPPER, []{return CanUse(MEGATON_HAMMER);}))) || CanUse(HOVER_BOOTS));}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_ENEMIES] = Area("Fire Temple Loop Enemies", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM, {[]{return SmallKeys(FIRE_TEMPLE, 8) || !IsKeysanity;}}),
                  Entrance(FIRE_TEMPLE_LOOP_TILES, {[]{return Here(FIRE_TEMPLE_LOOP_ENEMIES, []{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER);});}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_TILES] = Area("Fire Temple Loop Tiles", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_BOSS_KEY_LOOP, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_ENEMIES,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_FLARE_DANCER] = Area("Fire Temple Loop Flare Dancer", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_FLARE_DANCER_CHEST, {[]{return (HasExplosives || CanUse(MEGATON_HAMMER)) && IsAdult;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_TILES,         {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return Here(FIRE_TEMPLE_LOOP_FLARE_DANCER, []{return CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT) || (HasExplosives && (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) ||
                   CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(BOOMERANG)));});}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_HAMMER_SWITCH] = Area("Fire Temple Loop Hammer Switch", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireLoopSwitch, {[]{return FireLoopSwitch || CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_GORON_ROOM,   {[]{return FireLoopSwitch;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_GORON_ROOM] = Area("Fire Temple Loop Goron Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return FireLoopSwitch;}}),
                  Entrance(FIRE_TEMPLE_LOOP_EXIT,          {[]{return FireLoopSwitch;}}),
  });

  areaTable[FIRE_TEMPLE_LOOP_EXIT] = Area("Fire Temple Loop Exit", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_LOOP_GORON_ROOM, {[]{return FireLoopSwitch;}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM] = Area("Fire Temple Big Lava Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRST_ROOM,                {[]{return SmallKeys(FIRE_TEMPLE, 2);}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES, {[]{return IsAdult && (CanPlay(SongOfTime) || LogicFireSongOfTime);}}),
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON, {[]{return IsAdult && HasExplosives;}}),
                  Entrance(FIRE_TEMPLE_FIRE_PILLAR_ROOM,          {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON] = Area("Fire Temple Big Lava Room North Goron", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES] = Area("Fire Temple Big Lava Room North Tiles", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, {[]{return CanAdultAttack || HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON] = Area("Fire Temple Big Lava Room South Goron", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_PILLAR_ROOM] = Area("Fire Temple Fire Pillar Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM, {[]{return FireTimer >= 56 && SmallKeys(FIRE_TEMPLE, 4);}}),
  });

  areaTable[FIRE_TEMPLE_SHORTCUT_ROOM] = Area("Fire Temple Shortcut Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, {[]{return Here(FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_PILLAR_ROOM,   {[]{return SmallKeys(FIRE_TEMPLE, 4);}}),
                  Entrance(FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return Here(FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return IsAdult && (GoronBracelet || LogicFireStrength) && (HasExplosives || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(SLINGSHOT));}}),
  });

  areaTable[FIRE_TEMPLE_SHORTCUT_CLIMB] = Area("Fire Temple Shortcut Climb", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM,      {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Area("Fire Temple Boulder Maze Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_GS_BOULDER_MAZE,          {[]{return HasExplosives && (IsAdult || HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_ROOM,                {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM,            {[]{return SmallKeys(FIRE_TEMPLE, 5, 7);}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER,           {[]{return false;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM] = Area("Fire Temple Boulder Maze Lower Side Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_EAST_CENTRAL_ROOM] = Area("Fire Temple East Central Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BIG_LAVA_ROOM,      {[]{return CanTakeDamage;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return SmallKeys(FIRE_TEMPLE, 5, 8);}}),
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return SmallKeys(FIRE_TEMPLE, 6, 8);}}),
                  Entrance(FIRE_TEMPLE_MAP_AREA,           {[]{return CanUse(SLINGSHOT) || CanUse(BOW);}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_WALL_CHASE] = Area("Fire Temple Fire Wall Chase", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM,  {[]{return FireTimer >= 24 && SmallKeys(FIRE_TEMPLE, 6, 8);}}),
                  Entrance(FIRE_TEMPLE_MAP_AREA,           {[]{return IsAdult;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return FireTimer >= 24 && IsAdult;}}),
                  Entrance(FIRE_TEMPLE_CORRIDOR,           {[]{return FireTimer >= 24 && IsAdult && SmallKeys(FIRE_TEMPLE, 7);}}),
  });

  areaTable[FIRE_TEMPLE_MAP_AREA] = Area("Fire Temple Map Area", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_BOULDER_MAZE_UPPER] = Area("Fire Temple Boulder Maze Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return HasExplosives;}}),
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_SCARECROW_ROOM,     {[]{return CanUse(SCARECROW) || (LogicFireScarecrow && IsAdult && CanUse(LONGSHOT));}}),
  });

  areaTable[FIRE_TEMPLE_SCARECROW_ROOM] = Area("Fire Temple Scarecrow Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_GS_SCARECROW_CLIMB, {[]{return CanJumpslash || CanUse(SLINGSHOT) || CanUse(BOOMERANG) || HasExplosives || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(DINS_FIRE);}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_PEAK,          {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_EAST_PEAK] = Area("Fire Temple East Peak", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_SCARECROW_CHEST,  {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_GS_SCARECROW_TOP, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_SCARECROW_ROOM,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return CanTakeDamage;}}),
  });

  areaTable[FIRE_TEMPLE_CORRIDOR] = Area("Fire Temple Corridor", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_WALL_CHASE, {[]{return SmallKeys(FIRE_TEMPLE, 7);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,  {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_ROOM] = Area("Fire Temple Fire Maze Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_CORRIDOR,            {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,     {[]{return CanUse(HOVER_BOOTS);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER,  {[]{return SmallKeys(FIRE_TEMPLE, 8);}}),
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE,      {[]{return LogicFireFlameMaze || false;}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_UPPER] = Area("Fire Temple Fire Maze Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_NEAR_BOSS_ROOM,     {[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM] = Area("Fire Temple Fire Maze Side Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CENTRAL_LOWER] = Area("Fire Temple West Central Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_HIGHEST_GORON_CHEST, {[]{return Here(FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return (CanPlay(SongOfTime) || LogicRustedSwitches) && CanUse(MEGATON_HAMMER);});}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return SmallKeys(FIRE_TEMPLE, 8);}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return IsAdult && CanPlay(SongOfTime);}}),
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE,     {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CENTRAL_UPPER] = Area("Fire Temple West Central Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_BOSS_ENTRYWAY,      {[]{return false;}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
  });

  areaTable[FIRE_TEMPLE_LATE_FIRE_MAZE] = Area("Fire Temple Late Fire Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return false;}}),
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return HasExplosives;}}),
  });

  areaTable[FIRE_TEMPLE_UPPER_FLARE_DANCER] = Area("Fire Temple Upper Flare Dancer", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_LATE_FIRE_MAZE, {[]{return Here(FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT) || (HasExplosives && ((CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) ||
                   CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(BOOMERANG)));});}}),
                  Entrance(FIRE_TEMPLE_WEST_CLIMB,     {[]{return Here(FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT) || (HasExplosives && ((CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) ||
                   CanUse(SLINGSHOT) || CanUse(BOOMERANG)));});}}),
  });

  areaTable[FIRE_TEMPLE_WEST_CLIMB] = Area("Fire Temple West Climb", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_WEST_PEAK,          {[]{return CanUseProjectile;}}),
  });

  areaTable[FIRE_TEMPLE_WEST_PEAK] = Area("Fire Temple West Peak", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MEGATON_HAMMER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanTakeDamage;}}),
                  Entrance(FIRE_TEMPLE_WEST_CLIMB,         {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_HAMMER_RETURN_PATH] = Area("Fire Temple Hammer Return Path", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ABOVE_FIRE_MAZE, {[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Area("Fire Temple Above Fire Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return CanUse(MEGATON_HAMMER);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::FireTemple.IsMQ()) {
  areaTable[FIRE_TEMPLE_MQ_LOWER] = Area("Fire Temple MQ Lower", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, {[]{return CanJumpslash || CanUse(SLINGSHOT) || CanUse(BOW) || Bombs || CanUse(DINS_FIRE);}}),
                  LocationAccess(FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,     {[]{return IsAdult && (LogicFewerTunicRequirements || CanUse(GORON_TUNIC)) && (((CanUse(HOVER_BOOTS) || (LogicFireMQNearBoss && CanUse(BOW))) && HasFireSource) || (CanUse(HOOKSHOT) && CanUse(FIRE_ARROWS) || (CanUse(DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || CanUse(GORON_TUNIC) || CanUse(BOW) || CanUse(LONGSHOT)))));}}),
                    //Trick: IsAdult && (LogicFewerTunicRequirements || CanUse(GORON_TUNIC)) && (((CanUse(HOVER_BOOTS) || (LogicFireMQNearBoss && CanUse(BOW))) && HasFireSource) || (CanUse(HOOKSHOT) && CanUse(FIRE_ARROWS) || (CanUse(DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || CanUse(GORON_TUNIC) || CanUse(BOW) || CanUse(LONGSHOT)))))
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_ENTRYWAY,             {[]{return true;}}),
                  Entrance(FIRE_TEMPLE_BOSS_ENTRYWAY,        {[]{return IsAdult && CanUse(GORON_TUNIC) && CanUse(MEGATON_HAMMER) && BossKeyFireTemple && ((HasFireSource && (LogicFireBossDoorJump || HoverBoots)) || HasAccessTo(FIRE_TEMPLE_MQ_UPPER));}}),
                  Entrance(FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR, {[]{return SmallKeys(FIRE_TEMPLE, 5) && (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD));}}),
                  Entrance(FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,     {[]{return IsAdult && FireTimer >= 24 && CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR] = Area("Fire Temple MQ Lower Locked Door", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, {[]{return IsAdult && (CanUse(MEGATON_HAMMER) || CanUse(HOOKSHOT) || (HasExplosives && (CanUse(MASTER_SWORD) || CanUse(KOKIRI_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(BOOMERANG))));}}),
                  LocationAccess(FIRE_TEMPLE_MQ_MAP_CHEST,            {[]{return IsAdult && CanUse(MEGATON_HAMMER);}}),
  }, {});

  areaTable[FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Area("Fire Temple MQ Big Lava Room", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && (CanUse(HOOKSHOT) || LogicFireSongOfTime));}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && (CanUse(HOOKSHOT) || LogicFireSongOfTime)
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,                   {[]{return HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && CanUse(HOOKSHOT);}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && CanUse(HOOKSHOT)
                  LocationAccess(FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return HasFireSource && HasExplosives && IsAdult && (CanUse(HOOKSHOT) || LogicFireMQBlockedChest);}}),
                    //Trick: HasFireSource && HasExplosives && IsAdult && (CanUse(HOOKSHOT) || LogicFireMQBlockedChest)
                  LocationAccess(FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_LOWER_MAZE, {[]{return IsAdult && CanUse(GORON_TUNIC) && SmallKeys(FIRE_TEMPLE, 2) && (HasFireSource || (LogicFireMQClimb && HoverBoots));}}),
                    //Trick: IsAdult && CanUse(GORON_TUNIC) && SmallKeys(FIRE_TEMPLE, 2) && (HasFireSource || (LogicFireMQClimb && HoverBoots))
  });

  areaTable[FIRE_TEMPLE_MQ_LOWER_MAZE] = Area("Fire Temple MQ Lower Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,     {[]{return CanUse(MASTER_SWORD) || CanUse(KOKIRI_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, {[]{return HasExplosives && (LogicFireMQMazeSideRoom || HasAccessTo(FIRE_TEMPLE_MQ_UPPER_MAZE));}}),
                    //Trick: HasExplosives && (LogicFireMQMazeSideRoom || FIRE_TEMPLE_MQ_UPPER_MAZE.Adult())
   }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_UPPER_MAZE, {[]{return (IsAdult && ((HasExplosives && CanUse(HOOKSHOT)) || (LogicFireMQMazeHovers && CanUse(HOVER_BOOTS)))) || LogicFireMQMazeJump;}}),
                    //Trick: (IsAdult && ((HasExplosives && CanUse(HOOKSHOT)) || (LogicFireMQMazeHovers && CanUse(HOVER_BOOTS)))) || LogicFireMQMazeJump
  });

  areaTable[FIRE_TEMPLE_MQ_UPPER_MAZE] = Area("Fire Temple MQ Upper Maze", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && (((CanPlay(SongOfTime) && CanUse(HOOKSHOT) && HasExplosives) || CanUse(LONGSHOT))));}}),
                  EventAccess(&FairyPot,  {[]{return SmallKeys(FIRE_TEMPLE, 3);}}),
  }, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, {[]{return true;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_COMPASS_CHEST,             {[]{return HasExplosives;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,          {[]{return IsAdult && ((CanPlay(SongOfTime) && CanUse(HOOKSHOT) && HasExplosives) || CanUse(LONGSHOT));}}),
  }, {
                  //Exits
                  Entrance(FIRE_TEMPLE_MQ_UPPER, {[]{return SmallKeys(FIRE_TEMPLE, 3) && IsAdult && ((CanUse(BOW) && CanUse(HOOKSHOT)) || CanUse(FIRE_ARROWS));}}),
  });

  areaTable[FIRE_TEMPLE_MQ_UPPER] = Area("Fire Temple MQ Upper", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FIRE_TEMPLE_MQ_FREESTANDING_KEY,            {[]{return ((CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(SLINGSHOT) || CanUse(BOOMERANG)) && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze;}}),
                    //Trick: (IsAdult && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze
                  LocationAccess(FIRE_TEMPLE_MQ_CHEST_ON_FIRE,               {[]{return ((IsAdult && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze) && SmallKeys(FIRE_TEMPLE, 4);}}),
                    //Trick: ((IsAdult && CanUse(HOOKSHOT)) || LogicFireMQFlameMaze) && SmallKeys(FIRE_TEMPLE, 4)
                  LocationAccess(FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, {[]{return CanPlay(SongOfTime) || HoverBoots || LogicFireMQFlameMaze;}}),
                    //Trick: CanPlay(SongOfTime) || HoverBoots || LogicFireMQFlameMaze
                  LocationAccess(FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,    {[]{return HasExplosives;}}),
                  LocationAccess(FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,     {[]{return (IsAdult && CanUse(HOOKSHOT) && SmallKeys(FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && IsAdult && CanUse(LONGSHOT));}}),
                    //Trick: (IsAdult && CanUse(HOOKSHOT) && SmallKeys(FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && IsAdult && CanUse(LONGSHOT))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[FIRE_TEMPLE_BOSS_ENTRYWAY] =
        Area("Fire Temple Boss Entryway", "Fire Temple", FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(FIRE_TEMPLE_NEAR_BOSS_ROOM, { [] { return Dungeon::FireTemple.IsVanilla() && false; } }),
                 Entrance(FIRE_TEMPLE_MQ_LOWER, { [] { return Dungeon::FireTemple.IsMQ() && false; } }),
                 Entrance(FIRE_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[FIRE_TEMPLE_BOSS_ROOM] =
        Area("Fire Temple Boss Room", "Fire Temple", NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&FireTempleClear,
                             { [] { return FireTempleClear || (FireTimer >= 64 && CanUse(MEGATON_HAMMER)); }}),
             },
             {
                 // Locations
                 LocationAccess(FIRE_TEMPLE_VOLVAGIA_HEART, { [] { return FireTempleClear; } }),
                 LocationAccess(VOLVAGIA, { [] { return FireTempleClear; } }),
             },
             {
                 // Exits
                 Entrance(FIRE_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
             });
}
