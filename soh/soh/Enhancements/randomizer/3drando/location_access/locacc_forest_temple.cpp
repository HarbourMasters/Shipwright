#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_ForestTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_FOREST_TEMPLE_ENTRYWAY] = Area("Forest Temple Entryway", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return randoCtx->GetDungeon(FOREST_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,   {[]{return randoCtx->GetDungeon(FOREST_TEMPLE)->IsMQ();}}),
                  Entrance(RR_SACRED_FOREST_MEADOW,     {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(FOREST_TEMPLE)->IsVanilla()) {
  areaTable[RR_FOREST_TEMPLE_FIRST_ROOM] = Area("Forest Temple First Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    {[]{return (logic->IsAdult && logic->Bombs) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->HasBombchus || logic->CanUse(RG_DINS_FIRE) || (randoCtx->GetTrickOption(RT_FOREST_FIRST_GS) && (logic->CanJumpslash || (logic->IsChild && logic->Bombs)));}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SOUTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_SOUTH_CORRIDOR] = Area("Forest Temple South Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOBBY,      {[]{return logic->CanAdultAttack || logic->CanChildAttack || logic->Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOBBY] = Area("Forest Temple Lobby", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleMeg, {[]{return logic->ForestTempleMeg || (logic->ForestTempleJoelle && logic->ForestTempleBeth && logic->ForestTempleAmy && logic->CanUse(RG_FAIRY_BOW));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_GS_LOBBY, {[]{return logic->HookshotOrBoomerang;}}),
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

  areaTable[RR_FOREST_TEMPLE_NORTH_CORRIDOR] = Area("Forest Temple North Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,         {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOWER_STALFOS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Area("Forest Temple Lower Stalfos", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NORTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Area("Forest Temple NW Outdoors Lower", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, {[]{return logic->CanUse(RG_LONGSHOT) || Here(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, []{return logic->HookshotOrBoomerang;});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return logic->CanUse(RG_SONG_OF_TIME);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return false;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return logic->GoldScale || logic->CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Area("Forest Temple NW Outdoors Upper", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Area("Forest Temple NE Outdoors Lower", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, {[]{return logic->CanUse(RG_HOOKSHOT) || HasAccessTo(RR_FOREST_TEMPLE_FALLING_ROOM) || (HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER) && logic->IsAdult && randoCtx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->HoverBoots);}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    {[]{return logic->CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_FOREST_OUTDOORS_EAST_GS) && logic->CanUse(RG_BOOMERANG)) || Here(RR_FOREST_TEMPLE_FALLING_ROOM, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_DINS_FIRE) || logic->HasExplosives;});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_FOREST_VINES) && logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return logic->GoldScale || logic->CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Area("Forest Temple NE Outdoors Upper", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return randoCtx->GetTrickOption(RT_FOREST_DOORFRAME) && logic->CanJumpslash && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_SCARECROW);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Area("Forest Temple Map Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MAP_CHEST, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->Nuts || logic->HookshotOrBoomerang || logic->CanShield));});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->Nuts || logic->HookshotOrBoomerang || logic->CanShield));});}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return logic->HasExplosives || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->Nuts || logic->HookshotOrBoomerang || logic->CanShield));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_SEWER] = Area("Forest Temple Sewer", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_WELL_CHEST, {[]{return HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Area("Forest Temple Below Boss Key Chest", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return logic->HasExplosives || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_BOW) || ((logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT)) && (logic->Nuts || logic->HookshotOrBoomerang || logic->CanShield));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Area("Forest Temple Floormaster Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, {[]{return logic->CanAdultDamage || logic->CanChildDamage;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_WEST_CORRIDOR] = Area("Forest Temple West Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,           {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 1, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return logic->CanAdultAttack || logic->CanChildAttack || logic->Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Area("Forest Temple Block Push Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, {[]{return logic->GoronBracelet && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_WEST_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER,        {[]{return logic->CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && logic->CanJumpslash && logic->GoronBracelet);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      {[]{return logic->IsAdult && logic->GoronBracelet && logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, {[]{return logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)) && logic->GoronBracelet && logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Area("Forest Temple NW Corridor Twisted", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,    {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 3);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NW Corridor Straightened", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Area("Forest Temple Red Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleJoelle, {[]{return logic->ForestTempleJoelle || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_RED_POE_CHEST, {[]{return logic->ForestTempleJoelle;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 3);}}),
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,       {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Area("Forest Temple Upper Stalfos", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BOW_CHEST, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,  {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Area("Forest Temple Blue Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleBeth, {[]{return logic->ForestTempleBeth || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BLUE_POE_CHEST, {[]{return logic->ForestTempleBeth;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NE Corridor Straightened", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Area("Forest Temple NE Corridor Twisted", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_FROZEN_EYE_ROOM] = Area("Forest Temple Frozen Eye Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Area("Forest Temple Falling Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Area("Forest Temple Green Poe Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleAmy, {[]{return logic->ForestTempleAmy || logic->CanUse(RG_FAIRY_BOW);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_EAST_CORRIDOR, {[]{return logic->ForestTempleAmy;}}),
  });

  areaTable[RR_FOREST_TEMPLE_EAST_CORRIDOR] = Area("Forest Temple East Corridor", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,          {[]{return logic->CanAdultAttack || logic->CanChildAttack || logic->Nuts;}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM, {[]{return logic->CanAdultAttack || logic->CanChildAttack || logic->Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BOSS_REGION] = Area("Forest Temple Boss Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BASEMENT_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_BASEMENT,    {[]{return logic->HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->BossKeyForestTemple;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(FOREST_TEMPLE)->IsMQ()) {
  areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Area("Forest Temple MQ Lobby", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, {[]{return logic->CanJumpslash || logic->Bombs || logic->CanUse(RG_STICKS) || logic->Nuts || logic->HookshotOrBoomerang || logic->CanUse(RG_DINS_FIRE) || logic->KokiriSword || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, {[]{return logic->HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 1) && (logic->CanAdultAttack || logic->CanChildAttack || logic->Nuts);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_CENTRAL_AREA] = Area("Forest Temple MQ Central Area", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,       {[]{return (logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild) && (logic->CanJumpslash || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->KokiriSword);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,        {[]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,        {[]{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT);}}), //This is as far as child can get
                  Entrance(RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE, {[]{return logic->IsAdult && (logic->GoronBracelet || (randoCtx->GetTrickOption(RT_FOREST_MQ_BLOCK_PUZZLE) && logic->HasBombchus && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)));}}),
                    //Trick: logic->IsAdult && (logic->GoronBracelet || (LogicForestMQBlockPuzzle && logic->HasBombchus && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)))
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      {[]{return (randoCtx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (randoCtx->GetTrickOption(RT_FOREST_MQ_RANG_HALLWAY_SWITCH) && logic->CanUse(RG_BOOMERANG)) || (randoCtx->GetTrickOption(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT));}}),
                    //Trick (logic->Hookshot trick not added to either n64 or oot3d rando as of yet, to enable in SoH needs uncommenting in randomizer_tricks.cpp): (LogicForestMQHallwaySwitchJS && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (LogicForestMQHallwaySwitchHookshot && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) 
                  Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION,        {[]{return logic->ForestTempleJoAndBeth && logic->ForestTempleAmyAndMeg;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE] = Area("Forest Temple MQ After Block Puzzle", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 3);}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_BOW_REGION,    {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, {[]{return  logic->SmallKeys(RR_FOREST_TEMPLE, 3) || (randoCtx->GetTrickOption(RT_FOREST_MQ_JS_HALLWAY_SWITCH) && ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_FOREST_OUTSIDE_BACKDOOR) && (logic->IsAdult || (logic->IsChild && logic->CanUse(RG_STICKS))))));}}),
                    //Trick (Doing the hallway switch jumpslash as child requires sticks and has been added above): logic->SmallKeys(RR_FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJS && ((logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || LogicForestOutsideBackdoor))
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,   {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Area("Forest Temple MQ Outdoor Ledge", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, {[]{return logic->CanJumpslash;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NW_OUTDOORS] = Area("Forest Temple MQ NW Outdoors", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, {[]{return logic->CanAdultAttack || logic->CanChildAttack;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,         {[]{return (logic->IsAdult && (logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_FOREST_MQ_WELL_SWIM) && logic->CanUse(RG_HOOKSHOT)))) || logic->ProgressiveScale >= 2;}}),
                    //Trick: (logic->IsAdult && (logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_LONGSHOT) || (LogicForestMQWellSwim && logic->CanUse(RG_HOOKSHOT)))) || logic->ProgressiveScale >= 2
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS] = Area("Forest Temple MQ NE Outdoors", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 {[]{return (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, {[]{return logic->HookshotOrBoomerang || (logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS) && (logic->CanUse(RG_SONG_OF_TIME) || (logic->CanUse(RG_HOVER_BOOTS) && randoCtx->GetTrickOption(RT_FOREST_DOORFRAME))));}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_WELL,                    {[]{return (logic->IsAdult && ((logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_FAIRY_BOW))) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME));}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   {[]{return logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Area("Forest Temple MQ Outdoors Top Ledges", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return randoCtx->GetTrickOption(RT_FOREST_OUTDOORS_LEDGE) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS);}}),
                    //Trick: LogicForestOutdoorsLedge && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Area("Forest Temple MQ NE Outdoors Ledge", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return logic->CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOW_REGION] = Area("Forest Temple MQ Bow Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleJoAndBeth, {[]{return logic->ForestTempleJoAndBeth || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BOW_CHEST,     {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_MAP_CHEST,     {[]{return logic->IsAdult && logic->CanUse(RG_FAIRY_BOW);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, {[]{return logic->IsAdult && logic->CanUse(RG_FAIRY_BOW);}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return logic->SmallKeys(RR_FOREST_TEMPLE, 5) && ((logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || logic->CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Area("Forest Temple MQ Falling Room", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTempleAmyAndMeg, {[]{return logic->ForestTempleAmyAndMeg || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && logic->SmallKeys(RR_FOREST_TEMPLE, 6));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOSS_REGION] = Area("Forest Temple MQ Boss Region", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST,   {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->BossKeyForestTemple;}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] =
        Area("Forest Temple Boss Entryway", "Forest Temple", RA_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_FOREST_TEMPLE_BOSS_REGION, { [] { return randoCtx->GetDungeon(FOREST_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION, { [] { return randoCtx->GetDungeon(FOREST_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_FOREST_TEMPLE_BOSS_ROOM] = Area(
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
            LocationAccess(RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, { [] { return logic->ForestTempleClear; } }),
            LocationAccess(RC_PHANTOM_GANON, { [] { return logic->ForestTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_SACRED_FOREST_MEADOW, { [] { return logic->ForestTempleClear; } }),
        });
}
