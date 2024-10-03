#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_ForestTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_FOREST_TEMPLE_ENTRYWAY] = Region("Forest Temple Entryway", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,   {[]{return ctx->GetDungeon(FOREST_TEMPLE)->IsMQ();}}),
                  Entrance(RR_SACRED_FOREST_MEADOW,     {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla()) {
  areaTable[RR_FOREST_TEMPLE_FIRST_ROOM] = Region("Forest Temple First Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, true),
                  LOCATION(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    (logic->IsAdult && logic->CanUse(RG_BOMB_BAG)) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOMBCHU_5) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_FOREST_FIRST_GS) && (logic->CanJumpslash() || (logic->IsChild && logic->CanUse(RG_BOMB_BAG))))),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SOUTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_SOUTH_CORRIDOR] = Region("Forest Temple South Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOBBY,      {[]{return logic->CanAttack() || logic->CanUse(RG_NUTS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOBBY] = Region("Forest Temple Lobby", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleMeg, {[]{return logic->ForestTempleMeg || (logic->ForestTempleJoelle && logic->ForestTempleBeth && logic->ForestTempleAmy && logic->CanUse(RG_FAIRY_BOW));}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_GS_LOBBY, logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_SOUTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NORTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  Entrance(RR_FOREST_TEMPLE_WEST_CORRIDOR,     {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 1, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_EAST_CORRIDOR,     {[]{return false;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_REGION,       {[]{return logic->ForestTempleMeg;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NORTH_CORRIDOR] = Region("Forest Temple North Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,         {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOWER_STALFOS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Region("Forest Temple Lower Stalfos", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER)),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NORTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Region("Forest Temple NW Outdoors Lower", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, logic->CanUse(RG_LONGSHOT) || Here(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, []{return logic->HookshotOrBoomerang();})),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return logic->CanUse(RG_SONG_OF_TIME);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return false;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Region("Forest Temple NW Outdoors Upper", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Region("Forest Temple NE Outdoors Lower", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, logic->CanUse(RG_HOOKSHOT) || HasAccessTo(RR_FOREST_TEMPLE_FALLING_ROOM) || (HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER) && logic->IsAdult && ctx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->CanUse(RG_HOVER_BOOTS))),
                  LOCATION(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_FOREST_OUTDOORS_EAST_GS) && logic->CanUse(RG_BOOMERANG)) || Here(RR_FOREST_TEMPLE_FALLING_ROOM, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_DINS_FIRE) || logic->HasExplosives();})),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Region("Forest Temple NE Outdoors Upper", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return ctx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanJumpslash() && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_SCARECROW);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Region("Forest Temple Map Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MAP_CHEST, Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives() || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->CanUse(RG_NUTS) || logic->HookshotOrBoomerang() || logic->CanShield()));})),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives() || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->CanUse(RG_NUTS) || logic->HookshotOrBoomerang() || logic->CanShield()));});}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives() || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->CanUse(RG_NUTS) || logic->HookshotOrBoomerang() || logic->CanShield()));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_SEWER] = Region("Forest Temple Sewer", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_WELL_CHEST, HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER)),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Region("Forest Temple Below Boss Key Chest", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return logic->HasExplosives() || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->CanUse(RG_NUTS) || logic->HookshotOrBoomerang() || logic->CanShield()));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Region("Forest Temple Floormaster Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, logic->CanDamage()),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_WEST_CORRIDOR] = Region("Forest Temple West Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,           {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 1, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return logic->CanAttack() || logic->CanUse(RG_NUTS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Region("Forest Temple Block Push Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_WEST_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER,        {[]{return logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && logic->CanJumpslash() && logic->HasItem(RG_GORONS_BRACELET));}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      {[]{return logic->IsAdult && logic->HasItem(RG_GORONS_BRACELET) && logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, {[]{return logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)) && logic->HasItem(RG_GORONS_BRACELET) && logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Region("Forest Temple NW Corridor Twisted", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,    {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 3);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Region("Forest Temple NW Corridor Straightened", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Region("Forest Temple Red Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleJoelle, {[]{return logic->ForestTempleJoelle || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_RED_POE_CHEST, logic->ForestTempleJoelle),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 3);}}),
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,       {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Region("Forest Temple Upper Stalfos", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_BOW_CHEST, logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER)),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,  {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Region("Forest Temple Blue Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleBeth, {[]{return logic->ForestTempleBeth || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_BLUE_POE_CHEST, logic->ForestTempleBeth),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Region("Forest Temple NE Corridor Straightened", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Region("Forest Temple NE Corridor Twisted", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_FROZEN_EYE_ROOM] = Region("Forest Temple Frozen Eye Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Region("Forest Temple Falling Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Region("Forest Temple Green Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleAmy, {[]{return logic->ForestTempleAmy || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_EAST_CORRIDOR, {[]{return logic->ForestTempleAmy;}}),
  });

  areaTable[RR_FOREST_TEMPLE_EAST_CORRIDOR] = Region("Forest Temple East Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,          {[]{return logic->CanAttack() || logic->CanUse(RG_NUTS);}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM, {[]{return logic->CanAttack() || logic->CanUse(RG_NUTS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_BOSS_REGION] = Region("Forest Temple Boss Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_BASEMENT_CHEST, true),
                  LOCATION(RC_FOREST_TEMPLE_GS_BASEMENT,    logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->HasItem(RG_FOREST_TEMPLE_BOSS_KEY);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(FOREST_TEMPLE)->IsMQ()) {
  areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Region("Forest Temple MQ Lobby", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, logic->CanJumpslash() || logic->CanUse(RG_BOMB_BAG) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_NUTS) || logic->HookshotOrBoomerang() || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOVER_BOOTS)),
                  LOCATION(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, logic->HookshotOrBoomerang()),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 1) && (logic->CanAttack() || logic->CanUse(RG_NUTS));}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_CENTRAL_AREA] = Region("Forest Temple MQ Central Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,       (logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild) && (logic->CanJumpslash() || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_KOKIRI_SWORD))),
                  LOCATION(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER)),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,        {[]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,        {[]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}}), //This is as far as child can get
                  Entrance(RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE, {[]{return logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_FOREST_MQ_BLOCK_PUZZLE) && logic->CanUse(RG_BOMBCHU_5) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)));}}),
                    //Trick: logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || (LogicForestMQBlockPuzzle && logic->CanUse(RG_BOMBCHU_5) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)))
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      {[]{return (ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (ctx->GetTrickOption(RT_FOREST_MQ_RANG_HALLWAY_SWITCH) && logic->CanUse(RG_BOOMERANG)) || (ctx->GetTrickOption(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT));}}),
                    //Trick (logic->Hookshot trick not added to either n64 or oot3d rando as of yet, to enable in SoH needs uncommenting in randomizer_tricks.cpp): (LogicForestMQHallwaySwitchJS && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (LogicForestMQHallwaySwitchHookshot && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) 
                  Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION,        {[]{return logic->ForestTempleJoAndBeth && logic->ForestTempleAmyAndMeg;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE] = Region("Forest Temple MQ After Block Puzzle", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, logic->SmallKeys(RR_FOREST_TEMPLE, 3)),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_BOW_REGION,    {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, {[]{return  logic->SmallKeys(RR_FOREST_TEMPLE, 3) || (ctx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || (ctx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && (logic->IsAdult || (logic->IsChild && logic->CanUse(RG_STICKS))))));}}),
                    //Trick (Doing the hallway switch jumpslash as child requires sticks and has been added above): logic->SmallKeys(RR_FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJS && ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicForestOutsideBackdoor))
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,   {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Region("Forest Temple MQ Outdoor Ledge", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, logic->CanJumpslash()),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NW_OUTDOORS] = Region("Forest Temple MQ NW Outdoors", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, logic->CanAttack()),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,         {[]{return (logic->IsAdult && (logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_FOREST_MQ_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT)))) || logic->HasItem(RG_GOLDEN_SCALE);}}),
                    //Trick: (logic->IsAdult && (logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (LogicForestMQWellSwim && logic->CanUse(RG_HOOKSHOT)))) || logic->ProgressiveScale >= 2
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS] = Region("Forest Temple MQ NE Outdoors", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE));}}),
  }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT))),
                  LOCATION(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, logic->HookshotOrBoomerang() || (logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS) && (logic->CanUse(RG_SONG_OF_TIME) || (logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_FOREST_DOORFRAME))))),
                  LOCATION(RC_FOREST_TEMPLE_MQ_GS_WELL,                    (logic->IsAdult && ((logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_FAIRY_BOW))) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT))),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME));}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   {[]{return logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Region("Forest Temple MQ Outdoors Top Ledges", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return ctx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS);}}),
                    //Trick: LogicForestOutdoorsLedge && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Region("Forest Temple MQ NE Outdoors Ledge", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return logic->CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOW_REGION] = Region("Forest Temple MQ Bow Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleJoAndBeth, {[]{return logic->ForestTempleJoAndBeth || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_BOW_CHEST,     logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER)),
                  LOCATION(RC_FOREST_TEMPLE_MQ_MAP_CHEST,     logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)),
                  LOCATION(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5) && ((logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || logic->CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Region("Forest Temple MQ Falling Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleAmyAndMeg, {[]{return logic->ForestTempleAmyAndMeg || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && logic->SmallKeys(RR_FOREST_TEMPLE, 6));}}),
  }, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOSS_REGION] = Region("Forest Temple MQ Boss Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST,   true),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->HasItem(RG_FOREST_TEMPLE_BOSS_KEY);}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] =
        Region("Forest Temple Boss Entryway", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_FOREST_TEMPLE_BOSS_REGION, { [] { return ctx->GetDungeon(FOREST_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION, { [] { return ctx->GetDungeon(FOREST_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_FOREST_TEMPLE_BOSS_ROOM] = Region(
        "Forest Temple Boss Room", "Forest Temple", RA_NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&logic->ForestTempleClear, { [] {
                return logic->ForestTempleClear || (logic->HasBossSoul(RG_PHANTOM_GANON_SOUL) && ((logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) &&
                                             (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))));
            } }),
        },
        {
            // Locations
            LOCATION(RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, logic->ForestTempleClear),
            LOCATION(RC_PHANTOM_GANON, logic->ForestTempleClear),
        },
        {
            // Exits
            Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_SACRED_FOREST_MEADOW, { [] { return logic->ForestTempleClear; } }),
        });
}
