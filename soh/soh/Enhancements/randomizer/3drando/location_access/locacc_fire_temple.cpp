#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_FireTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_FIRE_TEMPLE_ENTRYWAY] = Area("Fire Temple Entryway", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return randoCtx->GetDungeon(FIRE_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER,   {[]{return randoCtx->GetDungeon(FIRE_TEMPLE)->IsMQ();}}),
                  Entrance(RR_DMC_CENTRAL_LOCAL,      {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(FIRE_TEMPLE)->IsVanilla()) {
  areaTable[RR_FIRE_TEMPLE_FIRST_ROOM] = Area("Fire Temple First Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, {[]{return FireTimer >= 24;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,   {[]{return Here(RR_FIRE_TEMPLE_FIRST_ROOM, []{return CanUse(RG_MEGATON_HAMMER);}) && (SmallKeys(RR_FIRE_TEMPLE, 8) || !IsKeysanity);}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,  {[]{return SmallKeys(RR_FIRE_TEMPLE, 2) && FireTimer >= 24;}}),
  });

  areaTable[RR_FIRE_TEMPLE_NEAR_BOSS_ROOM] = Area("Fire Temple Near Boss Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyFireTemple && ((IsAdult && randoCtx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP)) || CanUse(RG_HOVER_BOOTS) || Here(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER, []{return CanUse(RG_MEGATON_HAMMER);}));}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_ENEMIES] = Area("Fire Temple Loop Enemies", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return SmallKeys(RR_FIRE_TEMPLE, 8) || !IsKeysanity;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES, {[]{return Here(RR_FIRE_TEMPLE_LOOP_ENEMIES, []{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_TILES] = Area("Fire Temple Loop Tiles", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_FLARE_DANCER] = Area("Fire Temple Loop Flare Dancer", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, {[]{return (HasExplosives || CanUse(RG_MEGATON_HAMMER)) && IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return Here(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, []{return CanUse(RG_MEGATON_HAMMER) || CanUse(RG_HOOKSHOT) || (HasExplosives && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) ||
                   CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH] = Area("Fire Temple Loop Hammer Switch", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireLoopSwitch, {[]{return FireLoopSwitch || CanUse(RG_MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM,   {[]{return FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_GORON_ROOM] = Area("Fire Temple Loop Goron Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return FireLoopSwitch;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,          {[]{return FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_EXIT] = Area("Fire Temple Loop Exit", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM, {[]{return FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM] = Area("Fire Temple Big Lava Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,                {[]{return SmallKeys(RR_FIRE_TEMPLE, 2);}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES, {[]{return IsAdult && (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_FIRE_SOT));}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON, {[]{return IsAdult && HasExplosives;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,          {[]{return SmallKeys(RR_FIRE_TEMPLE, 3);}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON] = Area("Fire Temple Big Lava Room North Goron", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES] = Area("Fire Temple Big Lava Room North Tiles", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, {[]{return CanAdultAttack || HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON] = Area("Fire Temple Big Lava Room South Goron", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM] = Area("Fire Temple Fire Pillar Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return SmallKeys(RR_FIRE_TEMPLE, 3);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM, {[]{return FireTimer >= 56 && SmallKeys(RR_FIRE_TEMPLE, 4);}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_ROOM] = Area("Fire Temple Shortcut Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, {[]{return Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,   {[]{return SmallKeys(RR_FIRE_TEMPLE, 4);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return IsAdult && (GoronBracelet || randoCtx->GetTrickOption(RT_FIRE_STRENGTH)) && (HasExplosives || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_SLINGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_CLIMB] = Area("Fire Temple Shortcut Climb", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Area("Fire Temple Boulder Maze Lower", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_GS_BOULDER_MAZE,          {[]{return HasExplosives && (IsAdult || HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,                {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,            {[]{return SmallKeys(RR_FIRE_TEMPLE, 5, 7);}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER,           {[]{return false;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM] = Area("Fire Temple Boulder Maze Lower Side Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM] = Area("Fire Temple East Central Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,      {[]{return CanTakeDamage;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return SmallKeys(RR_FIRE_TEMPLE, 5, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW);}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_WALL_CHASE] = Area("Fire Temple Fire Wall Chase", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,  {[]{return FireTimer >= 24 && SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return FireTimer >= 24 && IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,           {[]{return FireTimer >= 24 && IsAdult && SmallKeys(RR_FIRE_TEMPLE, 7);}}),
  });

  areaTable[RR_FIRE_TEMPLE_MAP_AREA] = Area("Fire Temple Map Area", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER] = Area("Fire Temple Boulder Maze Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return HasExplosives;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,     {[]{return CanUse(RG_SCARECROW) || (randoCtx->GetTrickOption(RT_FIRE_SCARECROW) && IsAdult && CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SCARECROW_ROOM] = Area("Fire Temple Scarecrow Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, {[]{return CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG) || HasExplosives || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_DINS_FIRE);}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_PEAK,          {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_EAST_PEAK] = Area("Fire Temple East Peak", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_SCARECROW_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_GS_SCARECROW_TOP, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return CanTakeDamage;}}),
  });

  areaTable[RR_FIRE_TEMPLE_CORRIDOR] = Area("Fire Temple Corridor", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE, {[]{return SmallKeys(RR_FIRE_TEMPLE, 7);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_ROOM] = Area("Fire Temple Fire Maze Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,     {[]{return CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER,  {[]{return SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE,      {[]{return randoCtx->GetTrickOption(RT_FIRE_FLAME_MAZE) || false;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_UPPER] = Area("Fire Temple Fire Maze Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,     {[]{return CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM] = Area("Fire Temple Fire Maze Side Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER] = Area("Fire Temple West Central Lower", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, {[]{return Here(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_RUSTED_SWITCHES)) && CanUse(RG_MEGATON_HAMMER);});}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return IsAdult && CanUse(RG_SONG_OF_TIME);}}),
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE,     {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER] = Area("Fire Temple West Central Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,      {[]{return false;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LATE_FIRE_MAZE] = Area("Fire Temple Late Fire Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return false;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return HasExplosives;}}),
  });

  areaTable[RR_FIRE_TEMPLE_UPPER_FLARE_DANCER] = Area("Fire Temple Upper Flare Dancer", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE, {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return CanUse(RG_MEGATON_HAMMER) || CanUse(RG_HOOKSHOT) || (HasExplosives && ((CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) ||
                   CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_BOOMERANG)));});}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return CanUse(RG_MEGATON_HAMMER) || CanUse(RG_HOOKSHOT) || (HasExplosives && ((CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) ||
                   CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CLIMB] = Area("Fire Temple West Climb", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_PEAK,          {[]{return CanUseProjectile;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_PEAK] = Area("Fire Temple West Peak", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return CanTakeDamage;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_HAMMER_RETURN_PATH] = Area("Fire Temple Hammer Return Path", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE, {[]{return CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Area("Fire Temple Above Fire Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return CanUse(RG_MEGATON_HAMMER);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(FIRE_TEMPLE)->IsMQ()) {
  areaTable[RR_FIRE_TEMPLE_MQ_LOWER] = Area("Fire Temple MQ Lower", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, {[]{return CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || Bombs || CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,     {[]{return IsAdult && (randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || CanUse(RG_GORON_TUNIC)) && (((CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_FIRE_MQ_NEAR_BOSS) && CanUse(RG_FAIRY_BOW))) && HasFireSource) || (CanUse(RG_HOOKSHOT) && CanUse(RG_FIRE_ARROWS) || (CanUse(RG_DINS_FIRE) && ((randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_QUADRUPLE) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OCTUPLE) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_SEXDECUPLE)) || CanUse(RG_GORON_TUNIC) || CanUse(RG_FAIRY_BOW) || CanUse(RG_LONGSHOT)))));}}),
                    //Trick: IsAdult && (LogicFewerTunicRequirements || CanUse(RG_GORON_TUNIC)) && (((CanUse(RG_HOVER_BOOTS) || (LogicFireMQNearBoss && CanUse(RG_FAIRY_BOW))) && HasFireSource) || (CanUse(RG_HOOKSHOT) && CanUse(RG_FIRE_ARROWS) || (CanUse(RG_DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || CanUse(RG_GORON_TUNIC) || CanUse(RG_FAIRY_BOW) || CanUse(RG_LONGSHOT)))))
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,        {[]{return IsAdult && CanUse(RG_GORON_TUNIC) && CanUse(RG_MEGATON_HAMMER) && BossKeyFireTemple && ((HasFireSource && (randoCtx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP) || HoverBoots)) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR, {[]{return SmallKeys(RR_FIRE_TEMPLE, 5) && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,     {[]{return IsAdult && FireTimer >= 24 && CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR] = Area("Fire Temple MQ Lower Locked Door", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, {[]{return IsAdult && (CanUse(RG_MEGATON_HAMMER) || CanUse(RG_HOOKSHOT) || (HasExplosives && (CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG))));}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MAP_CHEST,            {[]{return IsAdult && CanUse(RG_MEGATON_HAMMER);}}),
  }, {});

  areaTable[RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Area("Fire Temple MQ Big Lava Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (HasFireSource && (Bow || randoCtx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && IsAdult && (CanUse(RG_HOOKSHOT) || randoCtx->GetTrickOption(RT_FIRE_SOT)));}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && (CanUse(RG_HOOKSHOT) || LogicFireSongOfTime)
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,                   {[]{return HasFireSource && (Bow || randoCtx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && IsAdult && CanUse(RG_HOOKSHOT);}}),
                    //Trick: HasFireSource && (Bow || LogicFireMQBKChest) && IsAdult && CanUse(RG_HOOKSHOT)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return HasFireSource && HasExplosives && IsAdult && (CanUse(RG_HOOKSHOT) || randoCtx->GetTrickOption(RT_FIRE_MQ_BLOCKED_CHEST));}}),
                    //Trick: HasFireSource && HasExplosives && IsAdult && (CanUse(RG_HOOKSHOT) || LogicFireMQBlockedChest)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_MAZE, {[]{return IsAdult && CanUse(RG_GORON_TUNIC) && SmallKeys(RR_FIRE_TEMPLE, 2) && (HasFireSource || (randoCtx->GetTrickOption(RT_FIRE_MQ_CLIMB) && HoverBoots));}}),
                    //Trick: IsAdult && CanUse(RG_GORON_TUNIC) && SmallKeys(RR_FIRE_TEMPLE, 2) && (HasFireSource || (LogicFireMQClimb && HoverBoots))
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_MAZE] = Area("Fire Temple MQ Lower Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,     {[]{return CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, {[]{return HasExplosives && (randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_SIDE_ROOM) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER_MAZE));}}),
                    //Trick: HasExplosives && (LogicFireMQMazeSideRoom || FIRE_TEMPLE_MQ_UPPER_MAZE.Adult())
   }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER_MAZE, {[]{return (IsAdult && ((HasExplosives && CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_HOVERS) && CanUse(RG_HOVER_BOOTS)))) || randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_JUMP);}}),
                    //Trick: (IsAdult && ((HasExplosives && CanUse(RG_HOOKSHOT)) || (LogicFireMQMazeHovers && CanUse(RG_HOVER_BOOTS)))) || LogicFireMQMazeJump
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER_MAZE] = Area("Fire Temple MQ Upper Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && (((CanUse(RG_SONG_OF_TIME) && CanUse(RG_HOOKSHOT) && HasExplosives) || CanUse(RG_LONGSHOT))));}}),
                  EventAccess(&FairyPot,  {[]{return SmallKeys(RR_FIRE_TEMPLE, 3);}}),
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_COMPASS_CHEST,             {[]{return HasExplosives;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,          {[]{return IsAdult && ((CanUse(RG_SONG_OF_TIME) && CanUse(RG_HOOKSHOT) && HasExplosives) || CanUse(RG_LONGSHOT));}}),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER, {[]{return SmallKeys(RR_FIRE_TEMPLE, 3) && IsAdult && ((CanUse(RG_FAIRY_BOW) && CanUse(RG_HOOKSHOT)) || CanUse(RG_FIRE_ARROWS));}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER] = Area("Fire Temple MQ Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY,            {[]{return ((CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG)) && CanUse(RG_HOOKSHOT)) || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE);}}),
                    //Trick: (IsAdult && CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze
                  LocationAccess(RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,               {[]{return ((IsAdult && CanUse(RG_HOOKSHOT)) || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE)) && SmallKeys(RR_FIRE_TEMPLE, 4);}}),
                    //Trick: ((IsAdult && CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze) && SmallKeys(RR_FIRE_TEMPLE, 4)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, {[]{return CanUse(RG_SONG_OF_TIME) || HoverBoots || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE);}}),
                    //Trick: CanUse(RG_SONG_OF_TIME) || HoverBoots || LogicFireMQFlameMaze
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,    {[]{return HasExplosives;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,     {[]{return (IsAdult && CanUse(RG_HOOKSHOT) && SmallKeys(RR_FIRE_TEMPLE, 5)) || (randoCtx->GetTrickOption(RT_FIRE_MQ_ABOVE_MAZE_GS) && IsAdult && CanUse(RG_LONGSHOT));}}),
                    //Trick: (IsAdult && CanUse(RG_HOOKSHOT) && SmallKeys(RR_FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && IsAdult && CanUse(RG_LONGSHOT))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_FIRE_TEMPLE_BOSS_ENTRYWAY] =
        Area("Fire Temple Boss Entryway", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, { [] { return randoCtx->GetDungeon(FIRE_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_FIRE_TEMPLE_MQ_LOWER, { [] { return randoCtx->GetDungeon(FIRE_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_FIRE_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_FIRE_TEMPLE_BOSS_ROOM] =
        Area("Fire Temple Boss Room", "Fire Temple", RA_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&FireTempleClear,
                             { [] { return FireTempleClear || (HasBossSoul(RG_VOLVAGIA_SOUL) && (FireTimer >= 64 && CanUse(RG_MEGATON_HAMMER))); }}),
             },
             {
                 // Locations
                 LocationAccess(RC_FIRE_TEMPLE_VOLVAGIA_HEART, { [] { return FireTempleClear; } }),
                 LocationAccess(RC_VOLVAGIA, { [] { return FireTempleClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
             });
}
