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
  areaTable[FOREST_TEMPLE_ENTRYWAY] = Area("Forest Temple Entryway", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FIRST_ROOM, {[]{return Dungeon::ForestTemple.IsVanilla();}}),
                  Entrance(FOREST_TEMPLE_MQ_LOBBY,   {[]{return Dungeon::ForestTemple.IsMQ();}}),
                  Entrance(SACRED_FOREST_MEADOW,     {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::ForestTemple.IsVanilla()) {
  areaTable[FOREST_TEMPLE_FIRST_ROOM] = Area("Forest Temple First Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FIRST_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_GS_FIRST_ROOM,    {[]{return (IsAdult && Bombs) || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(BOOMERANG) || CanUse(SLINGSHOT) || HasBombchus || CanUse(DINS_FIRE);},
                                                      /*Glitched*/[]{return (Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY,       {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_SOUTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_SOUTH_CORRIDOR] = Area("Forest Temple South Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FIRST_ROOM, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_LOBBY,      {[]{return IsAdult || CanChildAttack || Nuts;},
                                          /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_LOBBY] = Area("Forest Temple Lobby", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleMeg, {[]{return ForestTempleMeg || (ForestTempleJoelle && ForestTempleBeth && ForestTempleAmy && CanUse(BOW));}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_GS_LOBBY, {[]{return HookshotOrBoomerang;},
                                              /*Glitched*/[]{return IsAdult && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_SOUTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NORTH_CORRIDOR,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return CanPlay(SongOfTime) || IsChild;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::NOVICE) || (SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                                       ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  Entrance(FOREST_TEMPLE_WEST_CORRIDOR,     {[]{return SmallKeys(FOREST_TEMPLE, 1, 5);}}),
                  Entrance(FOREST_TEMPLE_EAST_CORRIDOR,     {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(FOREST_TEMPLE_BOSS_REGION,       {[]{return ForestTempleMeg;}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM,         {[]{return false;},
                                                 /*Glitched*/[]{return IsAdult && (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)) && GlitchForestBKSkip;}}),
  });

  areaTable[FOREST_TEMPLE_NORTH_CORRIDOR] = Area("Forest Temple North Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,         {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_LOWER_STALFOS, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_LOWER_STALFOS] = Area("Forest Temple Lower Stalfos", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FIRST_STALFOS_CHEST, {[]{return IsAdult || KokiriSword;},
                                                         /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NORTH_CORRIDOR, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_NW_OUTDOORS_LOWER] = Area("Forest Temple NW Outdoors Lower", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, {[]{return CanUse(LONGSHOT) || Here(FOREST_TEMPLE_NW_OUTDOORS_UPPER, []{return HookshotOrBoomerang;});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,             {[]{return CanPlay(SongOfTime);},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::NOVICE) || (SongOfTime && (CanDoGlitch(GlitchType::DungeonBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                                       ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (IsAdult && CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::NOVICE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(IRON_BOOTS) || HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM,         {[]{return false;},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FOREST_TEMPLE_NW_OUTDOORS_UPPER] = Area("Forest Temple NW Outdoors Upper", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER,    {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_FLOORMASTER_ROOM,     {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_NE_OUTDOORS_LOWER] = Area("Forest Temple NE Outdoors Lower", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, {[]{return CanUse(HOOKSHOT) || HasAccessTo(FOREST_TEMPLE_FALLING_ROOM);}}),
                  LocationAccess(FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,    {[]{return CanUse(HOOKSHOT) || (LogicForestOutdoorEastGS && CanUse(BOOMERANG)) || Here(FOREST_TEMPLE_FALLING_ROOM, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(DINS_FIRE) || HasExplosives;});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,             {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return CanUse(LONGSHOT);}}),
                  Entrance(FOREST_TEMPLE_SEWER,             {[]{return GoldScale || CanUse(IRON_BOOTS) || HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,      {[]{return false;},
                                                 /*Glitched*/[]{return Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[FOREST_TEMPLE_NE_OUTDOORS_UPPER] = Area("Forest Temple NE Outdoors Upper", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MAP_ROOM,          {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,      {[]{return LogicForestDoorFrame && CanUse(HOVER_BOOTS) && CanUse(SCARECROW);},
                                                 /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::EXPERT);}}),
  });

  areaTable[FOREST_TEMPLE_MAP_ROOM] = Area("Forest Temple Map Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MAP_CHEST, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_UPPER, {[]{return Here(FOREST_TEMPLE_MAP_ROOM, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[FOREST_TEMPLE_SEWER] = Area("Forest Temple Sewer", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_WELL_CHEST, {[]{return HasAccessTo(FOREST_TEMPLE_NE_OUTDOORS_UPPER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST] = Area("Forest Temple Below Boss Key Chest", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return Here(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, []{return HasExplosives || CanUse(MEGATON_HAMMER) || CanUse(BOW) || ((IsAdult || Sticks || KokiriSword || Slingshot) && (Nuts || HookshotOrBoomerang || CanShield));});}}),
  });

  areaTable[FOREST_TEMPLE_FLOORMASTER_ROOM] = Area("Forest Temple Floormaster Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FLOORMASTER_CHEST, {[]{return IsAdult || CanChildDamage;},
                                                       /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_WEST_CORRIDOR] = Area("Forest Temple West Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,           {[]{return SmallKeys(FOREST_TEMPLE, 1, 5);}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;},
                                               /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BLOCK_PUSH_ROOM] = Area("Forest Temple Block Push Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_EYE_SWITCH_CHEST, {[]{return GoronBracelet && (CanUse(BOW) || CanUse(SLINGSHOT));},
                                                      /*Glitched*/[]{return IsAdult && (CanUse(BOW) || CanUse(SLINGSHOT)) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_WEST_CORRIDOR,            {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NW_OUTDOORS_UPPER,        {[]{return CanUse(HOVER_BOOTS) || (LogicForestOutsideBackdoor && IsAdult && GoronBracelet);},
                                                        /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_TWISTED,      {[]{return IsAdult && GoronBracelet && SmallKeys(FOREST_TEMPLE, 2);},
                                                        /*Glitched*/[]{return ((IsAdult && (Bow || Hookshot || CanUse(SLINGSHOT)) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) ||
                                                                               (Bombs && GoronBracelet && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE))) && SmallKeys(FOREST_TEMPLE, 2);}}),
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED, {[]{return CanUse(BOW) && GoronBracelet && SmallKeys(FOREST_TEMPLE, 2);},
                                                        /*Glitched*/[]{return ((IsAdult && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) ||
                                                                               (IsChild && Bombs && GoronBracelet && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE))) && (CanUse(BOW) || CanUse(SLINGSHOT)) && SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_NW_CORRIDOR_TWISTED] = Area("Forest Temple NW Corridor Twisted", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
                  Entrance(FOREST_TEMPLE_RED_POE_ROOM,    {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
  });

  areaTable[FOREST_TEMPLE_NW_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NW Corridor Straightened", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BLOCK_PUSH_ROOM,      {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_RED_POE_ROOM] = Area("Forest Temple Red Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoelle, {[]{return ForestTempleJoelle || CanUse(BOW);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_RED_POE_CHEST, {[]{return ForestTempleJoelle;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NW_CORRIDOR_TWISTED, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
                  Entrance(FOREST_TEMPLE_UPPER_STALFOS,       {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_UPPER_STALFOS] = Area("Forest Temple Upper Stalfos", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BOW_CHEST, {[]{return IsAdult || KokiriSword;},
                                               /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_RED_POE_ROOM,  {[]{return IsAdult || KokiriSword;},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FOREST_TEMPLE_BLUE_POE_ROOM, {[]{return IsAdult || KokiriSword;},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BLUE_POE_ROOM] = Area("Forest Temple Blue Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleBeth, {[]{return ForestTempleBeth || CanUse(BOW);}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BLUE_POE_CHEST, {[]{return ForestTempleBeth;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_UPPER_STALFOS,            {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
  });

  areaTable[FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED] = Area("Forest Temple NE Corridor Straightened", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_BLUE_POE_ROOM,   {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
                  Entrance(FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
  });

  areaTable[FOREST_TEMPLE_NE_CORRIDOR_TWISTED] = Area("Forest Temple NE Corridor Twisted", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FROZEN_EYE_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,    {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_FROZEN_EYE_ROOM] = Area("Forest Temple Frozen Eye Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, {[]{return SmallKeys(FOREST_TEMPLE, 5);}}),
                  Entrance(FOREST_TEMPLE_NE_CORRIDOR_TWISTED,      {[]{return SmallKeys(FOREST_TEMPLE, 5) && (CanUse(BOW) || CanUse(DINS_FIRE));}}),
  });

  areaTable[FOREST_TEMPLE_FALLING_ROOM] = Area("Forest Temple Falling Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_NE_OUTDOORS_LOWER, {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_GREEN_POE_ROOM,    {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_GREEN_POE_ROOM] = Area("Forest Temple Green Poe Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmy, {[]{return ForestTempleAmy || CanUse(BOW);}}),
                }, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_FALLING_ROOM,  {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_EAST_CORRIDOR, {[]{return ForestTempleAmy;}}),
  });

  areaTable[FOREST_TEMPLE_EAST_CORRIDOR] = Area("Forest Temple East Corridor", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,          {[]{return IsAdult || CanChildAttack || Nuts;},
                                              /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(FOREST_TEMPLE_GREEN_POE_ROOM, {[]{return IsAdult || CanChildAttack || Nuts;},
                                              /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
  });

  areaTable[FOREST_TEMPLE_BOSS_REGION] = Area("Forest Temple Boss Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_BASEMENT_CHEST, {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_GS_BASEMENT,    {[]{return HookshotOrBoomerang;},
                                                    /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_BOSS_ROOM, {[]{return BossKeyForestTemple;}}),
  });

  areaTable[FOREST_TEMPLE_BOSS_ROOM] = Area("Forest Temple Boss Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleClear, {[]{return ForestTempleClear || ((IsAdult || KokiriSword) && (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)));}}),
                }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_PHANTOM_GANON_HEART, {[]{return ForestTempleClear;}}),
                  LocationAccess(PHANTOM_GANON,                     {[]{return ForestTempleClear;}}),
                }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY, {[]{return ForestTempleClear;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::ForestTemple.IsMQ()) {
  areaTable[FOREST_TEMPLE_MQ_LOBBY] = Area("Forest Temple MQ Lobby", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, {[]{return IsAdult || Bombs || CanUse(STICKS) || Nuts || CanUse(BOOMERANG) || CanUse(DINS_FIRE) || KokiriSword || CanUse(SLINGSHOT);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, {[]{return HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_CENTRAL_AREA, {[]{return SmallKeys(FOREST_TEMPLE, 1) && (IsAdult || CanChildAttack || Nuts);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_CENTRAL_AREA] = Area("Forest Temple MQ Central Area", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_WOLFOS_CHEST,       {[]{return (CanPlay(SongOfTime) || IsChild) && (IsAdult || CanUse(DINS_FIRE) || CanUse(STICKS) || CanUse(SLINGSHOT) || KokiriSword);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, {[]{return IsAdult || KokiriSword;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}), //This is as far as child can get
                  Entrance(FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE, {[]{return IsAdult && GoronBracelet;}}),
                    //Trick: IsAdult && (GoronBracelet || (LogicForestMQBlockPuzzle && HasBombchus && IsAdult && CanUse(HOOKSHOT)))
                  Entrance(FOREST_TEMPLE_MQ_OUTDOOR_LEDGE,      {[]{return false;}}),
                    //Trick: (LogicForestMQHallwaySwitchJumpslash && IsAdult && CanUse(HOVER_BOOTS)) || (LogicForestMQHallwaySwitchHookshot && IsAdult && CanUse(HOOKSHOT))
                  Entrance(FOREST_TEMPLE_MQ_BOSS_REGION,        {[]{return ForestTempleJoAndBeth && ForestTempleAmyAndMeg;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_AFTER_BLOCK_PUZZLE] = Area("Forest Temple MQ After Block Puzzle", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_BOW_REGION,    {[]{return SmallKeys(FOREST_TEMPLE, 4);}}),
                  Entrance(FOREST_TEMPLE_MQ_OUTDOOR_LEDGE, {[]{return SmallKeys(FOREST_TEMPLE, 3);}}),
                    //Trick: SmallKeys(FOREST_TEMPLE, 3) || (LogicForestMQHallwaySwitchJumpslash && ((IsAdult && CanUse(HOOKSHOT)) || LogicForestOutsideBackdoor))
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS,   {[]{return SmallKeys(FOREST_TEMPLE, 2);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_OUTDOOR_LEDGE] = Area("Forest Temple MQ Outdoor Ledge", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_REDEAD_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NW_OUTDOORS, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_NW_OUTDOORS] = Area("Forest Temple MQ NW Outdoors", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,         {[]{return (IsAdult && (CanUse(IRON_BOOTS) || CanUse(LONGSHOT))) || ProgressiveScale >= 2;}}),
                    //Trick: (IsAdult && (CanUse(IRON_BOOTS) || CanUse(LONGSHOT) || (LogicForestMQWellSwim && CanUse(HOOKSHOT)))) || ProgressiveScale >= 2
                  Entrance(FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(FIRE_ARROWS);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_NE_OUTDOORS] = Area("Forest Temple MQ NE Outdoors", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_WELL_CHEST,                 {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, {[]{return HookshotOrBoomerang || (IsAdult && CanUse(FIRE_ARROWS) && (CanPlay(SongOfTime) || (CanUse(HOVER_BOOTS) && LogicForestDoorFrame)));}}),
                  LocationAccess(FOREST_TEMPLE_MQ_GS_WELL,                    {[]{return (IsAdult && ((CanUse(IRON_BOOTS) && CanUse(HOOKSHOT)) || CanUse(BOW))) || (IsChild && CanUse(SLINGSHOT));}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES, {[]{return IsAdult && CanUse(HOOKSHOT) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS) || CanPlay(SongOfTime));}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE,   {[]{return IsAdult && CanUse(LONGSHOT);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_OUTDOORS_TOP_LEDGES] = Area("Forest Temple MQ Outdoors Top Ledges", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,       {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return false;}}),
                    //Trick: LogicForestOutdoorsLedge && IsAdult && CanUse(HOVER_BOOTS)
  });

  areaTable[FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE] = Area("Forest Temple MQ NE Outdoors Ledge", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS,  {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return CanPlay(SongOfTime);}}),
  });

  areaTable[FOREST_TEMPLE_MQ_BOW_REGION] = Area("Forest Temple MQ Bow Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleJoAndBeth, {[]{return ForestTempleJoAndBeth || (IsAdult && CanUse(BOW));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BOW_CHEST,     {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_MQ_MAP_CHEST,     {[]{return IsAdult && CanUse(BOW);}}),
                  LocationAccess(FOREST_TEMPLE_MQ_COMPASS_CHEST, {[]{return IsAdult && CanUse(BOW);}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_FALLING_ROOM, {[]{return SmallKeys(FOREST_TEMPLE, 5) && ((IsAdult && CanUse(BOW)) || CanUse(DINS_FIRE));}}),
  });

  areaTable[FOREST_TEMPLE_MQ_FALLING_ROOM] = Area("Forest Temple MQ Falling Room", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleAmyAndMeg, {[]{return ForestTempleAmyAndMeg || (IsAdult && CanUse(BOW) && SmallKeys(FOREST_TEMPLE, 6));}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(FOREST_TEMPLE_MQ_NE_OUTDOORS_LEDGE, {[]{return true;}}),
  });

  areaTable[FOREST_TEMPLE_MQ_BOSS_REGION] = Area("Forest Temple MQ Boss Region", "Forest Temple", FOREST_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTempleClear, {[]{return ForestTempleClear || BossKeyForestTemple;}}),
  }, {
                  //Locations
                  LocationAccess(FOREST_TEMPLE_MQ_BASEMENT_CHEST,   {[]{return true;}}),
                  LocationAccess(FOREST_TEMPLE_PHANTOM_GANON_HEART, {[]{return BossKeyForestTemple;}}),
                  LocationAccess(PHANTOM_GANON,                     {[]{return BossKeyForestTemple;}}),
  }, {});
  }
}
