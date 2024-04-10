#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

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
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, {[]{return logic->FireTimer >= 24;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,   {[]{return Here(RR_FIRE_TEMPLE_FIRST_ROOM, []{return logic->CanUse(RG_MEGATON_HAMMER);}) && (logic->SmallKeys(RR_FIRE_TEMPLE, 8) || !logic->IsKeysanity);}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 2) && logic->FireTimer >= 24;}}),
  });

  areaTable[RR_FIRE_TEMPLE_NEAR_BOSS_ROOM] = Area("Fire Temple Near Boss Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->BossKeyFireTemple && ((logic->IsAdult && (randoCtx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP) || Here(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER, []{return logic->CanUse(RG_MEGATON_HAMMER);}))) || logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_ENEMIES] = Area("Fire Temple Loop Enemies", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8) || !logic->IsKeysanity;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES, {[]{return Here(RR_FIRE_TEMPLE_LOOP_ENEMIES, []{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_TILES] = Area("Fire Temple Loop Tiles", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, {[]{return logic->CanAdultAttack || logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_FLARE_DANCER] = Area("Fire Temple Loop Flare Dancer", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, {[]{return (logic->HasExplosives || logic->CanUse(RG_MEGATON_HAMMER)) && logic->IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return Here(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH] = Area("Fire Temple Loop Hammer Switch", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FireLoopSwitch, {[]{return logic->FireLoopSwitch || logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM,   {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_GORON_ROOM] = Area("Fire Temple Loop Goron Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return logic->FireLoopSwitch;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,          {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_EXIT] = Area("Fire Temple Loop Exit", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM, {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM] = Area("Fire Temple Big Lava Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,                {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 2);}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES, {[]{return logic->IsAdult && (logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_FIRE_SOT));}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON, {[]{return logic->IsAdult && logic->HasExplosives;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,          {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
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
                  LocationAccess(RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, {[]{return logic->CanAdultAttack || logic->HookshotOrBoomerang;}}),
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
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM, {[]{return logic->FireTimer >= 56 && logic->SmallKeys(RR_FIRE_TEMPLE, 4);}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_ROOM] = Area("Fire Temple Shortcut Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, {[]{return Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,   {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 4);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return logic->IsAdult && (logic->GoronBracelet || randoCtx->GetTrickOption(RT_FIRE_STRENGTH)) && (logic->HasExplosives || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_CLIMB] = Area("Fire Temple Shortcut Climb", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Area("Fire Temple Boulder Maze Lower", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_GS_BOULDER_MAZE,          {[]{return logic->HasExplosives && (logic->IsAdult || logic->HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,                {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,            {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5, 7);}}),
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
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,      {[]{return logic->CanTakeDamage;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW);}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_WALL_CHASE] = Area("Fire Temple Fire Wall Chase", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,  {[]{return logic->FireTimer >= 24 && logic->SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return logic->IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return logic->FireTimer >= 24 && logic->IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,           {[]{return logic->FireTimer >= 24 && logic->IsAdult && logic->SmallKeys(RR_FIRE_TEMPLE, 7);}}),
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
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return logic->HasExplosives;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,     {[]{return logic->CanUse(RG_SCARECROW) || (randoCtx->GetTrickOption(RT_FIRE_SCARECROW) && logic->IsAdult && logic->CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SCARECROW_ROOM] = Area("Fire Temple Scarecrow Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, {[]{return logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasExplosives || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE);}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_PEAK,          {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_EAST_PEAK] = Area("Fire Temple East Peak", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_SCARECROW_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_GS_SCARECROW_TOP, {[]{return logic->CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return logic->CanTakeDamage;}}),
  });

  areaTable[RR_FIRE_TEMPLE_CORRIDOR] = Area("Fire Temple Corridor", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 7);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_ROOM] = Area("Fire Temple Fire Maze Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,     {[]{return logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE,      {[]{return randoCtx->GetTrickOption(RT_FIRE_FLAME_MAZE) || false;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_UPPER] = Area("Fire Temple Fire Maze Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,     {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
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
                  LocationAccess(RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, {[]{return Here(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return (logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER);});}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME);}}),
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
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return logic->HasExplosives;}}),
  });

  areaTable[RR_FIRE_TEMPLE_UPPER_FLARE_DANCER] = Area("Fire Temple Upper Flare Dancer", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE, {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives && ((logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOOMERANG)));});}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives && ((logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CLIMB] = Area("Fire Temple West Climb", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_PEAK,          {[]{return logic->CanUseProjectile;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_PEAK] = Area("Fire Temple West Peak", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->CanTakeDamage;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_HAMMER_RETURN_PATH] = Area("Fire Temple Hammer Return Path", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Area("Fire Temple Above Fire Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(FIRE_TEMPLE)->IsMQ()) {
  areaTable[RR_FIRE_TEMPLE_MQ_LOWER] = Area("Fire Temple MQ Lower", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, {[]{return logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->Bombs || logic->CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,     {[]{return logic->IsAdult && (randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_GORON_TUNIC)) && (((logic->CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_FIRE_MQ_NEAR_BOSS) && logic->CanUse(RG_FAIRY_BOW))) && logic->HasFireSource) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_DINS_FIRE) && ((randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_QUADRUPLE) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OCTUPLE) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_SEXDECUPLE)) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT)))));}}),
                    //Trick: logic->IsAdult && (LogicFewerTunicRequirements || logic->CanUse(RG_GORON_TUNIC)) && (((logic->CanUse(RG_HOVER_BOOTS) || (LogicFireMQNearBoss && logic->CanUse(RG_FAIRY_BOW))) && logic->HasFireSource) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT)))))
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,        {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_MEGATON_HAMMER) && logic->BossKeyFireTemple && ((logic->HasFireSource && (randoCtx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP) || logic->HoverBoots)) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,     {[]{return logic->IsAdult && logic->FireTimer >= 24 && logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR] = Area("Fire Temple MQ Lower Locked Door", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, {[]{return logic->IsAdult && (logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG))));}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_MAP_CHEST,            {[]{return logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER);}}),
  }, {});

  areaTable[RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Area("Fire Temple MQ Big Lava Room", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->HasFireSource && (logic->Bow || randoCtx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || randoCtx->GetTrickOption(RT_FIRE_SOT)));}}),
                    //Trick: logic->HasFireSource && (logic->Bow || LogicFireMQBKChest) && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || LogicFireSongOfTime)
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,                   {[]{return logic->HasFireSource && (logic->Bow || randoCtx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}}),
                    //Trick: logic->HasFireSource && (logic->Bow || LogicFireMQBKChest) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, {[]{return logic->HasFireSource && logic->HasExplosives && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || randoCtx->GetTrickOption(RT_FIRE_MQ_BLOCKED_CHEST));}}),
                    //Trick: logic->HasFireSource && logic->HasExplosives && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || LogicFireMQBlockedChest)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_MAZE, {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(RR_FIRE_TEMPLE, 2) && (logic->HasFireSource || (randoCtx->GetTrickOption(RT_FIRE_MQ_CLIMB) && logic->HoverBoots));}}),
                    //Trick: logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(RR_FIRE_TEMPLE, 2) && (logic->HasFireSource || (LogicFireMQClimb && logic->HoverBoots))
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_MAZE] = Area("Fire Temple MQ Lower Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,     {[]{return logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, {[]{return logic->HasExplosives && (randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_SIDE_ROOM) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER_MAZE));}}),
                    //Trick: logic->HasExplosives && (LogicFireMQMazeSideRoom || FIRE_TEMPLE_MQ_UPPER_MAZE.Adult())
   }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER_MAZE, {[]{return (logic->IsAdult && ((logic->HasExplosives && logic->CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_HOVERS) && logic->CanUse(RG_HOVER_BOOTS)))) || randoCtx->GetTrickOption(RT_FIRE_MQ_MAZE_JUMP);}}),
                    //Trick: (logic->IsAdult && ((logic->HasExplosives && logic->CanUse(RG_HOOKSHOT)) || (LogicFireMQMazeHovers && logic->CanUse(RG_HOVER_BOOTS)))) || LogicFireMQMazeJump
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER_MAZE] = Area("Fire Temple MQ Upper Maze", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (logic->IsAdult && (((logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT) && logic->HasExplosives) || logic->CanUse(RG_LONGSHOT))));}}),
                  EventAccess(&logic->FairyPot,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
  }, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_COMPASS_CHEST,             {[]{return logic->HasExplosives;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,          {[]{return logic->IsAdult && ((logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT) && logic->HasExplosives) || logic->CanUse(RG_LONGSHOT));}}),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3) && logic->IsAdult && ((logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_FIRE_ARROWS));}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER] = Area("Fire Temple MQ Upper", "Fire Temple", RA_FIRE_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY,            {[]{return ((logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)) && logic->CanUse(RG_HOOKSHOT)) || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE);}}),
                    //Trick: (logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze
                  LocationAccess(RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,               {[]{return ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE)) && logic->SmallKeys(RR_FIRE_TEMPLE, 4);}}),
                    //Trick: ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze) && logic->SmallKeys(RR_FIRE_TEMPLE, 4)
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, {[]{return logic->CanUse(RG_SONG_OF_TIME) || logic->HoverBoots || randoCtx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE);}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || logic->HoverBoots || LogicFireMQFlameMaze
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,    {[]{return logic->HasExplosives;}}),
                  LocationAccess(RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,     {[]{return (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_FIRE_TEMPLE, 5)) || (randoCtx->GetTrickOption(RT_FIRE_MQ_ABOVE_MAZE_GS) && logic->IsAdult && logic->CanUse(RG_LONGSHOT));}}),
                    //Trick: (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && logic->IsAdult && logic->CanUse(RG_LONGSHOT))
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
                 EventAccess(&logic->FireTempleClear,
                             { [] { return logic->FireTempleClear || (logic->HasBossSoul(RG_VOLVAGIA_SOUL) && (logic->FireTimer >= 64 && logic->CanUse(RG_MEGATON_HAMMER))); }}),
             },
             {
                 // Locations
                 LocationAccess(RC_FIRE_TEMPLE_VOLVAGIA_HEART, { [] { return logic->FireTempleClear; } }),
                 LocationAccess(RC_VOLVAGIA, { [] { return logic->FireTempleClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_DMC_CENTRAL_LOCAL, { [] { return logic->FireTempleClear; } }),
             });
}
