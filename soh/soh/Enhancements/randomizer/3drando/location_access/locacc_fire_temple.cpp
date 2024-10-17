#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_FireTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_FIRE_TEMPLE_ENTRYWAY] = Region("Fire Temple Entryway", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return ctx->GetDungeon(FIRE_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER,   {[]{return ctx->GetDungeon(FIRE_TEMPLE)->IsMQ();}}),
                  Entrance(RR_DMC_CENTRAL_LOCAL,      {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(FIRE_TEMPLE)->IsVanilla()) {
  areaTable[RR_FIRE_TEMPLE_FIRST_ROOM] = Region("Fire Temple First Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, {[]{return logic->FireTimer() >= 24;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,   {[]{return Here(RR_FIRE_TEMPLE_FIRST_ROOM, []{return logic->CanUse(RG_MEGATON_HAMMER);}) && (logic->SmallKeys(RR_FIRE_TEMPLE, 8) || !logic->IsKeysanity);}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 2) && logic->FireTimer() >= 24;}}),
  });

  areaTable[RR_FIRE_TEMPLE_NEAR_BOSS_ROOM] = Region("Fire Temple Near Boss Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->HasItem(RG_FIRE_TEMPLE_BOSS_KEY) && ((logic->IsAdult && (ctx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP) || Here(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER, []{return logic->CanUse(RG_MEGATON_HAMMER);}))) || logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_ENEMIES] = Region("Fire Temple Loop Enemies", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8) || !logic->IsKeysanity;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES, {[]{return Here(RR_FIRE_TEMPLE_LOOP_ENEMIES, []{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_TILES] = Region("Fire Temple Loop Tiles", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_ENEMIES,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_FLARE_DANCER] = Region("Fire Temple Loop Flare Dancer", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, (logic->HasExplosives() || logic->CanUse(RG_MEGATON_HAMMER)) && logic->IsAdult),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_TILES,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return Here(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives() && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH] = Region("Fire Temple Loop Hammer Switch", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FireLoopSwitch, {[]{return logic->FireLoopSwitch || logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM,   {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_GORON_ROOM] = Region("Fire Temple Loop Goron Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BOSS_KEY_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LOOP_HAMMER_SWITCH, {[]{return logic->FireLoopSwitch;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_EXIT,          {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LOOP_EXIT] = Region("Fire Temple Loop Exit", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_LOOP_GORON_ROOM, {[]{return logic->FireLoopSwitch;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM] = Region("Fire Temple Big Lava Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRST_ROOM,                {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 2);}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES, {[]{return logic->IsAdult && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_FIRE_SOT));}}),
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON, {[]{return logic->IsAdult && logic->HasExplosives();}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,          {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_GORON] = Region("Fire Temple Big Lava Room North Goron", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_NORTH_TILES] = Region("Fire Temple Big Lava Room North Tiles", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //RANDOTODO check if child can reach
                  LOCATION(RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, (logic->IsAdult && logic->CanAttack()) || logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BIG_LAVA_ROOM_SOUTH_GORON] = Region("Fire Temple Big Lava Room South Goron", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM] = Region("Fire Temple Fire Pillar Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM, {[]{return logic->FireTimer() >= 56 && logic->SmallKeys(RR_FIRE_TEMPLE, 4);}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_ROOM] = Region("Fire Temple Shortcut Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;})),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_PILLAR_ROOM,   {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 4);}}),
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_SHORTCUT_CLIMB, []{return true;});}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || ctx->GetTrickOption(RT_FIRE_STRENGTH)) && (logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SHORTCUT_CLIMB] = Region("Fire Temple Shortcut Climb", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,      {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER] = Region("Fire Temple Boulder Maze Lower", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, true),
                  LOCATION(RC_FIRE_TEMPLE_GS_BOULDER_MAZE,          logic->HasExplosives() && (logic->IsAdult || logic->HookshotOrBoomerang())),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_ROOM,                {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,            {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5, 7);}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER,           {[]{return false;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER_SIDE_ROOM] = Region("Fire Temple Boulder Maze Lower Side Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM] = Region("Fire Temple East Central Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BIG_LAVA_ROOM,      {[]{return logic->TakeDamage();}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW);}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_WALL_CHASE] = Region("Fire Temple Fire Wall Chase", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM,  {[]{return logic->FireTimer() >= 24 && logic->SmallKeys(RR_FIRE_TEMPLE, 6, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_MAP_AREA,           {[]{return logic->IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return logic->FireTimer() >= 24 && logic->IsAdult;}}),
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,           {[]{return logic->FireTimer() >= 24 && logic->IsAdult && logic->SmallKeys(RR_FIRE_TEMPLE, 7);}}),
  });

  areaTable[RR_FIRE_TEMPLE_MAP_AREA] = Region("Fire Temple Map Region", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MAP_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER] = Region("Fire Temple Boulder Maze Upper", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SHORTCUT_CLIMB,     {[]{return logic->HasExplosives();}}),
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,     {[]{return logic->CanUse(RG_SCARECROW) || (ctx->GetTrickOption(RT_FIRE_SCARECROW) && logic->IsAdult && logic->CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_FIRE_TEMPLE_SCARECROW_ROOM] = Region("Fire Temple Scarecrow Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, logic->CanJumpslashExceptHammer() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE)),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_PEAK,          {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_EAST_PEAK] = Region("Fire Temple East Peak", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_SCARECROW_CHEST,  true),
                  LOCATION(RC_FIRE_TEMPLE_GS_SCARECROW_TOP, logic->CanUseProjectile()),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_SCARECROW_ROOM,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_EAST_CENTRAL_ROOM, {[]{return logic->TakeDamage();}}),
  });

  areaTable[RR_FIRE_TEMPLE_CORRIDOR] = Region("Fire Temple Corridor", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_WALL_CHASE, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 7);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_ROOM] = Region("Fire Temple Fire Maze Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,     {[]{return logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE,      {[]{return ctx->GetTrickOption(RT_FIRE_FLAME_MAZE) || false;}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_UPPER] = Region("Fire Temple Fire Maze Upper", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM,     {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_FIRE_MAZE_SIDE_ROOM] = Region("Fire Temple Fire Maze Side Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_COMPASS_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER] = Region("Fire Temple West Central Lower", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, Here(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, []{return (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER);})),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 8);}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME);}}),
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE,     {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER] = Region("Fire Temple West Central Upper", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,      {[]{return false;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FIRE_TEMPLE_LATE_FIRE_MAZE] = Region("Fire Temple Late Fire Maze", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_ROOM,     {[]{return false;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_LOWER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return logic->HasExplosives();}}),
  });

  areaTable[RR_FIRE_TEMPLE_UPPER_FLARE_DANCER] = Region("Fire Temple Upper Flare Dancer", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_LATE_FIRE_MAZE, {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives() && ((logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOOMERANG)));});}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,     {[]{return Here(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, []{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives() && ((logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) ||
                   logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)));});}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_CLIMB] = Region("Fire Temple West Climb", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_UPPER_FLARE_DANCER, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_PEAK,          {[]{return logic->CanUseProjectile();}}),
  });

  areaTable[RR_FIRE_TEMPLE_WEST_PEAK] = Region("Fire Temple West Peak", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_WEST_CENTRAL_UPPER, {[]{return logic->TakeDamage();}}),
                  Entrance(RR_FIRE_TEMPLE_WEST_CLIMB,         {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_HAMMER_RETURN_PATH] = Region("Fire Temple Hammer Return Path", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE] = Region("Fire Temple Above Fire Maze", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_HAMMER_RETURN_PATH, {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_FIRE_MAZE_UPPER,    {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(FIRE_TEMPLE)->IsMQ()) {
  areaTable[RR_FIRE_TEMPLE_MQ_LOWER] = Region("Fire Temple MQ Lower", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, logic->CanJumpslashExceptHammer() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOMB_BAG) || logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,     logic->IsAdult && (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_GORON_TUNIC)) && (((logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_FIRE_MQ_NEAR_BOSS) && logic->CanUse(RG_FAIRY_BOW))) && logic->HasFireSource()) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_DINS_FIRE) && ((ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO) && ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_QUADRUPLE) && ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OCTUPLE) && ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_SEXDECUPLE)) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT)))))),
                    //Trick: logic->IsAdult && (LogicFewerTunicRequirements || logic->CanUse(RG_GORON_TUNIC)) && (((logic->CanUse(RG_HOVER_BOOTS) || (LogicFireMQNearBoss && logic->CanUse(RG_FAIRY_BOW))) && logic->HasFireSource()) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_DINS_FIRE) && ((DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_QUADRUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OCTUPLE) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_SEXDECUPLE)) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT)))))
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY,        {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_MEGATON_HAMMER) && logic->HasItem(RG_FIRE_TEMPLE_BOSS_KEY) && ((logic->HasFireSource() && (ctx->GetTrickOption(RT_FIRE_BOSS_DOOR_JUMP) || logic->CanUse(RG_HOVER_BOOTS))) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 5) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD));}}),
                  Entrance(RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,     {[]{return logic->IsAdult && logic->FireTimer() >= 24 && logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_LOCKED_DOOR] = Region("Fire Temple MQ Lower Locked Door", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, logic->IsAdult && (logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_HOOKSHOT) || (logic->HasExplosives() && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG))))),
                  LOCATION(RC_FIRE_TEMPLE_MQ_MAP_CHEST,            logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
  }, {});

  areaTable[RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM] = Region("Fire Temple MQ Big Lava Room", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->HasFireSource() && (logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_FIRE_SOT)));}}),
                    //Trick: logic->HasFireSource() && (logic->CanUse(RG_FAIRY_BOW) || LogicFireMQBKChest) && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || LogicFireSongOfTime)
  }, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,                   logic->HasFireSource() && (logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_FIRE_MQ_BK_CHEST)) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)),
                    //Trick: logic->HasFireSource() && (logic->CanUse(RG_FAIRY_BOW) || LogicFireMQBKChest) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)
                  LOCATION(RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, logic->HasFireSource() && logic->HasExplosives() && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_FIRE_MQ_BLOCKED_CHEST))),
                    //Trick: logic->HasFireSource() && logic->HasExplosives() && logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || LogicFireMQBlockedChest)
                  LOCATION(RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,       true),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_LOWER_MAZE, {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(RR_FIRE_TEMPLE, 2) && (logic->HasFireSource() || (ctx->GetTrickOption(RT_FIRE_MQ_CLIMB) && logic->CanUse(RG_HOVER_BOOTS)));}}),
                    //Trick: logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->SmallKeys(RR_FIRE_TEMPLE, 2) && (logic->HasFireSource() || (LogicFireMQClimb && logic->CanUse(RG_HOVER_BOOTS)))
  });

  areaTable[RR_FIRE_TEMPLE_MQ_LOWER_MAZE] = Region("Fire Temple MQ Lower Maze", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,     logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, logic->HasExplosives() && (ctx->GetTrickOption(RT_FIRE_MQ_MAZE_SIDE_ROOM) || HasAccessTo(RR_FIRE_TEMPLE_MQ_UPPER_MAZE))),
                    //Trick: logic->HasExplosives() && (LogicFireMQMazeSideRoom || FIRE_TEMPLE_MQ_UPPER_MAZE.Adult())
   }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER_MAZE, {[]{return (logic->IsAdult && ((logic->HasExplosives() && logic->CanUse(RG_HOOKSHOT)) || (ctx->GetTrickOption(RT_FIRE_MQ_MAZE_HOVERS) && logic->CanUse(RG_HOVER_BOOTS)))) || ctx->GetTrickOption(RT_FIRE_MQ_MAZE_JUMP);}}),
                    //Trick: (logic->IsAdult && ((logic->HasExplosives() && logic->CanUse(RG_HOOKSHOT)) || (LogicFireMQMazeHovers && logic->CanUse(RG_HOVER_BOOTS)))) || LogicFireMQMazeJump
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER_MAZE] = Region("Fire Temple MQ Upper Maze", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (logic->IsAdult && (((logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT) && logic->HasExplosives()) || logic->CanUse(RG_LONGSHOT))));}}),
                  EventAccess(&logic->FairyPot,  {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3);}}),
  }, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, true),
                  LOCATION(RC_FIRE_TEMPLE_MQ_COMPASS_CHEST,             logic->HasExplosives()),
                  LOCATION(RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,          logic->IsAdult && ((logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_HOOKSHOT) && logic->HasExplosives()) || logic->CanUse(RG_LONGSHOT))),
  }, {
                  //Exits
                  Entrance(RR_FIRE_TEMPLE_MQ_UPPER, {[]{return logic->SmallKeys(RR_FIRE_TEMPLE, 3) && logic->IsAdult && ((logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_FIRE_ARROWS));}}),
  });

  areaTable[RR_FIRE_TEMPLE_MQ_UPPER] = Region("Fire Temple MQ Upper", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY,            ((logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)) && logic->CanUse(RG_HOOKSHOT)) || ctx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE)),
                    //Trick: (logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze
                  LOCATION(RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,               ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || ctx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE)) && logic->SmallKeys(RR_FIRE_TEMPLE, 4)),
                    //Trick: ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicFireMQFlameMaze) && logic->SmallKeys(RR_FIRE_TEMPLE, 4)
                  LOCATION(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_FIRE_MQ_FLAME_MAZE)),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || logic->CanUse(RG_HOVER_BOOTS) || LogicFireMQFlameMaze
                  LOCATION(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,    logic->HasExplosives()),
                  LOCATION(RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,     (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_FIRE_TEMPLE, 5)) || (ctx->GetTrickOption(RT_FIRE_MQ_ABOVE_MAZE_GS) && logic->IsAdult && logic->CanUse(RG_LONGSHOT))),
                    //Trick: (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_FIRE_TEMPLE, 5)) || (LogicFireMQAboveMazeGS && logic->IsAdult && logic->CanUse(RG_LONGSHOT))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_FIRE_TEMPLE_BOSS_ENTRYWAY] =
        Region("Fire Temple Boss Entryway", "Fire Temple", {RA_FIRE_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, { [] { return ctx->GetDungeon(FIRE_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_FIRE_TEMPLE_MQ_LOWER, { [] { return ctx->GetDungeon(FIRE_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_FIRE_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_FIRE_TEMPLE_BOSS_ROOM] =
        Region("Fire Temple Boss Room", "Fire Temple", {}, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&logic->FireTempleClear,
                             { [] { return logic->FireTempleClear || (logic->HasBossSoul(RG_VOLVAGIA_SOUL) && (logic->FireTimer() >= 64 && logic->CanUse(RG_MEGATON_HAMMER))); }}),
             },
             {
                 // Locations
                 LOCATION(RC_FIRE_TEMPLE_VOLVAGIA_HEART, logic->FireTempleClear),
                 LOCATION(RC_VOLVAGIA, logic->FireTempleClear),
             },
             {
                 // Exits
                 Entrance(RR_FIRE_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_DMC_CENTRAL_LOCAL, { [] { return logic->FireTempleClear; } }, false),
             });
}
