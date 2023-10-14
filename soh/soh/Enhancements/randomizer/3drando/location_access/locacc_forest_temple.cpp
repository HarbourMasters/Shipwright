#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_ForestTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_FOREST_TEMPLE_ENTRYWAY] = Area("Forest Temple Entryway", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return Dungeon::ForestTemple.IsVanilla();}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_LOBBY,   {[]{return Dungeon::ForestTemple.IsMQ();}}),
                  Entrance(RR_SACRED_FOREST_MEADOW,     {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::ForestTemple.IsVanilla()) {
  areaTable[RR_FOREST_TEMPLE_FIRST_ROOM] = Area("Forest Temple First Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_FIRST_ROOM,    {[]{return (IsAdult && Bombs) || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG) || CanUse(RG_FAIRY_SLINGSHOT) || HasBombchus || CanUse(RG_DINS_FIRE) || (LogicForestFirstGS && (CanJumpslash || (IsChild && Bombs)));}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SOUTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_SOUTH_CORRIDOR] = Area("Forest Temple South Corridor", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOBBY,      {[]{return IsAdult || CanChildAttack || Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOBBY] = Area("Forest Temple Lobby", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleMeg, {[]{return ForestTempleMeg || (ForestTempleJoelle && ForestTempleBeth && ForestTempleAmy && CanUse(RG_FAIRY_BOW));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_GS_LOBBY, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_SOUTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NORTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return CanPlay(SongOfTime) || IsChild;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  Entrance(RR_FOREST_TEMPLE_WEST_CORRIDOR,     {[]{return SmallKeys(RR_FOREST_TEMPLE, 1, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_EAST_CORRIDOR,     {[]{return false;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_REGION,       {[]{return ForestTempleMeg;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NORTH_CORRIDOR] = Area("Forest Temple North Corridor", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,         {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_LOWER_STALFOS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_LOWER_STALFOS] = Area("Forest Temple Lower Stalfos", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, {[]{return IsAdult || KokiriSword;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NORTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Area("Forest Temple NW Outdoors Lower", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, {[]{return CanUse(RG_LONGSHOT) || Here(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, []{return HookshotOrBoomerang;});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return CanPlay(SongOfTime);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return false;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Area("Forest Temple NW Outdoors Upper", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER,    {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FLOORMASTER_ROOM,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Area("Forest Temple NE Outdoors Lower", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, {[]{return CanUse(RG_HOOKSHOT) || HasAccessTo(RR_FOREST_TEMPLE_FALLING_ROOM) || (HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER) && IsAdult && LogicForestOutdoorsLedge && HoverBoots);}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    {[]{return CanUse(RG_HOOKSHOT) || (LogicForestOutdoorEastGS && CanUse(RG_BOOMERANG)) || Here(RR_FOREST_TEMPLE_FALLING_ROOM, []{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_DINS_FIRE) || HasExplosives;});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,             {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return CanUse(RG_LONGSHOT) || (LogicForestVines && CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(RG_IRON_BOOTS) || HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return false;}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Area("Forest Temple NE Outdoors Upper", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,      {[]{return LogicForestDoorFrame && CanUse(RG_HOVER_BOOTS) && CanUse(RG_SCARECROW);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MAP_ROOM] = Area("Forest Temple Map Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MAP_CHEST, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_FAIRY_BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_FAIRY_BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_FAIRY_BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_SEWER] = Area("Forest Temple Sewer", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_WELL_CHEST, {[]{return HasAccessTo(RR_FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Area("Forest Temple Below Boss Key Chest", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return Here(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return HasExplosives || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_FAIRY_BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[RR_FOREST_TEMPLE_FLOORMASTER_ROOM] = Area("Forest Temple Floormaster Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, {[]{return IsAdult || CanChildDamage;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_WEST_CORRIDOR] = Area("Forest Temple West Corridor", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,           {[]{return SmallKeys(RR_FOREST_TEMPLE, 1, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Area("Forest Temple Block Push Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, {[]{return GoronBracelet && (CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_WEST_CORRIDOR,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NW_OUTDOORS_UPPER,        {[]{return CanUse(RG_HOVER_BOOTS) || (LogicForestOutsideBackdoor && CanJumpslash && GoronBracelet);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      {[]{return IsAdult && GoronBracelet && SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, {[]{return CanUse(RG_FAIRY_BOW) && GoronBracelet && SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Area("Forest Temple NW Corridor Twisted", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return SmallKeys(RR_FOREST_TEMPLE, 2);}}),
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,    {[]{return SmallKeys(RR_FOREST_TEMPLE, 3);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NW Corridor Straightened", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_RED_POE_ROOM] = Area("Forest Temple Red Poe Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoelle, {[]{return ForestTempleJoelle || CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_RED_POE_CHEST, {[]{return ForestTempleJoelle;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, {[]{return SmallKeys(RR_FOREST_TEMPLE, 3);}}),
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,       {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_UPPER_STALFOS] = Area("Forest Temple Upper Stalfos", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BOW_CHEST, {[]{return IsAdult || KokiriSword;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_RED_POE_ROOM,  {[]{return IsAdult || KokiriSword;}}),
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM, {[]{return IsAdult || KokiriSword;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BLUE_POE_ROOM] = Area("Forest Temple Blue Poe Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleBeth, {[]{return ForestTempleBeth || CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BLUE_POE_CHEST, {[]{return ForestTempleBeth;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_UPPER_STALFOS,            {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(RR_FOREST_TEMPLE, 4);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NE Corridor Straightened", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BLUE_POE_ROOM,   {[]{return SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(RR_FOREST_TEMPLE, 5);}}),
  });

  areaTable[RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Area("Forest Temple NE Corridor Twisted", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_FROZEN_EYE_ROOM] = Area("Forest Temple Frozen Eye Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(RR_FOREST_TEMPLE, 5);}}),
                  Entrance(RR_FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      {[]{return SmallKeys(RR_FOREST_TEMPLE, 5) && (CanUse(RG_FAIRY_BOW) || CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_FALLING_ROOM] = Area("Forest Temple Falling Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_GREEN_POE_ROOM] = Area("Forest Temple Green Poe Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmy, {[]{return ForestTempleAmy || CanUse(RG_FAIRY_BOW);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_FALLING_ROOM,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_EAST_CORRIDOR, {[]{return ForestTempleAmy;}}),
  });

  areaTable[RR_FOREST_TEMPLE_EAST_CORRIDOR] = Area("Forest Temple East Corridor", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,          {[]{return IsAdult || CanChildAttack || Nuts;}}),
                  Entrance(RR_FOREST_TEMPLE_GREEN_POE_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;}}),
  });

  areaTable[RR_FOREST_TEMPLE_BOSS_REGION] = Area("Forest Temple Boss Region", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_BASEMENT_CHEST, {[]{return true;}}),
                  LocationAccess(RC_FOREST_TEMPLE_GS_BASEMENT,    {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyForestTemple;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::ForestTemple.IsMQ()) {
  areaTable[RR_FOREST_TEMPLE_MQ_LOBBY] = Area("Forest Temple MQ Lobby", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, {[]{return IsAdult || Bombs || CanUse(RG_STICKS) || Nuts || CanUse(RG_BOOMERANG) || CanUse(RG_DINS_FIRE) || KokiriSword || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, {[]{return HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_CENTRAL_AREA, {[]{return SmallKeys(RR_FOREST_TEMPLE, 1) && (IsAdult || CanChildAttack || Nuts);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_CENTRAL_AREA] = Area("Forest Temple MQ Central Area", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,       {[]{return (CanPlay(SongOfTime) || IsChild) && (IsAdult || CanUse(RG_DINS_FIRE) || CanUse(RG_STICKS) || CanUse(RG_FAIRY_SLINGSHOT) || KokiriSword);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, {[]{return IsAdult || KokiriSword;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,        {[]{return (IsAdult && CanUse(RG_FAIRY_BOW)) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,        {[]{return (IsAdult && CanUse(RG_FAIRY_BOW)) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));}}), //This is as far as child can get
                  Entrance(RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE, {[]{return IsAdult && (GoronBracelet || (LogicForestMQBlockPuzzle && HasBombchus && IsAdult && CanUse(RG_HOOKSHOT)));}}),
                    //Trick: IsAdult && (GoronBracelet || (LogicForestMQBlockPuzzle && HasBombchus && IsAdult && CanUse(RG_HOOKSHOT)))
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      {[]{return (LogicForestMQHallwaySwitchJS && IsAdult && CanUse(RG_HOVER_BOOTS)) || (LogicForestMQHallwaySwitchBoomerang && CanUse(RG_BOOMERANG)) || (LogicForestMQHallwaySwitchHookshot && IsAdult && CanUse(RG_HOOKSHOT));}}),
                    //Trick (Hookshot trick not added to either n64 or oot3d rando as of yet, to enable in SoH needs uncommenting in randomizer_tricks.cpp): (LogicForestMQHallwaySwitchJS && IsAdult && CanUse(RG_HOVER_BOOTS)) || (LogicForestMQHallwaySwitchHookshot && IsAdult && CanUse(RG_HOOKSHOT)) 
                  Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION,        {[]{return ForestTempleJoAndBeth && ForestTempleAmyAndMeg;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE] = Area("Forest Temple MQ After Block Puzzle", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return SmallKeys(RR_FOREST_TEMPLE, 3);}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_BOW_REGION,    {[]{return SmallKeys(RR_FOREST_TEMPLE, 4);}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, {[]{return  SmallKeys(RR_FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJS && ((IsAdult && CanUse(RG_HOOKSHOT)) || (LogicForestOutsideBackdoor && (IsAdult || (IsChild && CanUse(RG_STICKS))))));}}),
                    //Trick (Doing the hallway switch jumpslash as child requires sticks and has been added above): SmallKeys(RR_FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJS && ((IsAdult && CanUse(RG_HOOKSHOT)) || LogicForestOutsideBackdoor))
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS,   {[]{return SmallKeys(RR_FOREST_TEMPLE, 2);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Area("Forest Temple MQ Outdoor Ledge", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NW_OUTDOORS, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NW_OUTDOORS] = Area("Forest Temple MQ NW Outdoors", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,         {[]{return (IsAdult && (CanUse(RG_IRON_BOOTS) || CanUse(RG_LONGSHOT) || (LogicForestMQWellSwim && CanUse(RG_HOOKSHOT)))) || ProgressiveScale >= 2;}}),
                    //Trick: (IsAdult && (CanUse(RG_IRON_BOOTS) || CanUse(RG_LONGSHOT) || (LogicForestMQWellSwim && CanUse(RG_HOOKSHOT)))) || ProgressiveScale >= 2
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(RG_FIRE_ARROWS);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS] = Area("Forest Temple MQ NE Outdoors", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_WELL_CHEST,                 {[]{return (IsAdult && CanUse(RG_FAIRY_BOW)) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, {[]{return HookshotOrBoomerang || (IsAdult && CanUse(RG_FIRE_ARROWS) && (CanPlay(SongOfTime) || (CanUse(RG_HOVER_BOOTS) && LogicForestDoorFrame)));}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_GS_WELL,                    {[]{return (IsAdult && ((CanUse(RG_IRON_BOOTS) && CanUse(RG_HOOKSHOT)) || CanUse(RG_FAIRY_BOW))) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(RG_HOOKSHOT) && (CanUse(RG_LONGSHOT) || CanUse(RG_HOVER_BOOTS) || CanPlay(SongOfTime));}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   {[]{return IsAdult && CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Area("Forest Temple MQ Outdoors Top Ledges", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,       {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return LogicForestOutdoorsLedge && IsAdult && CanUse(RG_HOVER_BOOTS);}}),
                    //Trick: LogicForestOutdoorsLedge && IsAdult && CanUse(RG_HOVER_BOOTS)
  });

  areaTable[RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Area("Forest Temple MQ NE Outdoors Ledge", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS,  {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return CanPlay(SongOfTime);}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOW_REGION] = Area("Forest Temple MQ Bow Region", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoAndBeth, {[]{return ForestTempleJoAndBeth || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BOW_CHEST,     {[]{return true;}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_MAP_CHEST,     {[]{return IsAdult && CanUse(RG_FAIRY_BOW);}}),
                  LocationAccess(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, {[]{return IsAdult && CanUse(RG_FAIRY_BOW);}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return SmallKeys(RR_FOREST_TEMPLE, 5) && ((IsAdult && CanUse(RG_FAIRY_BOW)) || CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_FALLING_ROOM] = Area("Forest Temple MQ Falling Room", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmyAndMeg, {[]{return ForestTempleAmyAndMeg || (IsAdult && CanUse(RG_FAIRY_BOW) && SmallKeys(RR_FOREST_TEMPLE, 6));}}),
  }, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return true;}}),
  });

  areaTable[RR_FOREST_TEMPLE_MQ_BOSS_REGION] = Area("Forest Temple MQ Boss Region", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST,   {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyForestTemple;}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_FOREST_TEMPLE_BOSS_ENTRYWAY] =
        Area("Forest Temple Boss Entryway", "Forest Temple", RHT_FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_FOREST_TEMPLE_BOSS_REGION, { [] { return Dungeon::ForestTemple.IsVanilla() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_MQ_BOSS_REGION, { [] { return Dungeon::ForestTemple.IsMQ() && false; } }),
                 Entrance(RR_FOREST_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_FOREST_TEMPLE_BOSS_ROOM] = Area(
        "Forest Temple Boss Room", "Forest Temple", RHT_NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&ForestTempleClear, { [] {
                return ForestTempleClear || ((CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) &&
                                             (CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT)));
            } }),
        },
        {
            // Locations
            LocationAccess(RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, { [] { return ForestTempleClear; } }),
            LocationAccess(RC_PHANTOM_GANON, { [] { return ForestTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_FOREST_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
        });
}
