#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_GerudoTrainingGrounds() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[GERUDO_TRAINING_GROUNDS_ENTRYWAY] = Area("Gerudo Training Grounds Entryway", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_LOBBY,    {[]{return Dungeon::GerudoTrainingGrounds.IsVanilla();}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, {[]{return Dungeon::GerudoTrainingGrounds.IsMQ();}}),
                  Entrance(GERUDO_FORTRESS,                  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::GerudoTrainingGrounds.IsVanilla()) {
  areaTable[GERUDO_TRAINING_GROUNDS_LOBBY] = Area("Gerudo Training Grounds Lobby", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_LOBBY_LEFT_CHEST,  {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_LOBBY_RIGHT_CHEST, {[]{return CanUse(BOW) || CanUse(SLINGSHOT);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_STALFOS_CHEST,     {[]{return IsAdult || KokiriSword;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_BEAMOS_CHEST,      {[]{return HasExplosives && (IsAdult || KokiriSword);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY,         {[]{return true;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM, {[]{return (IsAdult || KokiriSword) && (CanUse(HOOKSHOT) || LogicGtgWithoutHookshot);}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return Here(GERUDO_TRAINING_GROUNDS_LOBBY, []{return (IsAdult || KokiriSword) && HasExplosives;});}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,     {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE] = Area("Gerudo Training Grounds Central Maze", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HIDDEN_CEILING_CHEST,   {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 3) && (LogicLensGtg || CanUse(LENS_OF_TRUTH));},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && (LogicLensGtg || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_FIRST_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 4);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_SECOND_CHEST, {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 6);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 7);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_PATH_FINAL_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 9);},
                                                                      /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 9);},
                                                            /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds Central Maze Right", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_FREESTANDING_KEY,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,   {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_LAVA_ROOM] = Area("Gerudo Training Grounds Lava Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return CanUse(HOOKSHOT) && CanPlay(SongOfTime) && IronBoots && WaterTimer >= 24;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return CanPlay(SongOfTime) || IsChild;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,        {[]{return CanUse(LONGSHOT)  || (CanUse(HOVER_BOOTS) && CanUse(HOOKSHOT));}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM] = Area("Gerudo Training Grounds Hammer Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_CLEAR_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_SWITCH_CHEST, {[]{return CanUse(MEGATON_HAMMER) || (CanTakeDamage && LogicFlamingChests);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return CanUse(MEGATON_HAMMER) && Bow;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER] = Area("Gerudo Training Grounds Eye Statue Lower", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_EYE_STATUE_CHEST, {[]{return CanUse(BOW);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER] = Area("Gerudo Training Grounds Eye Statue Upper", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_NEAR_SCARECROW_CHEST, {[]{return CanUse(BOW);}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return true;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM] = Area("Gerudo Training Grounds Heavy Block Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_BEFORE_HEAVY_BLOCK_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER, {[]{return (LogicLensGtg || CanUse(LENS_OF_TRUTH)) && (CanUse(HOOKSHOT) || (LogicGtgFakeWall && CanUse(HOVER_BOOTS)));}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,   {[]{return (LogicLensGtg || CanUse(LENS_OF_TRUTH)) && (CanUse(HOOKSHOT) || (LogicGtgFakeWall && CanUse(HOVER_BOOTS))) && CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM] = Area("Gerudo Training Grounds Like Like Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_THIRD_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FOURTH_CHEST, {[]{return true;}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::GerudoTrainingGrounds.IsMQ()) {
  areaTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY] = Area("Gerudo Training Grounds MQ Lobby", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_LOBBY_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_LOBBY_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_HIDDEN_CEILING_CHEST,   {[]{return LogicLensGtgMQ || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 1);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY,      {[]{return true;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,  {[]{return Here(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return HasFireSource;});}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE, {[]{return Here(GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));});}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE] = Area("Gerudo Training Grounds MQ Right Side", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && CanUse(BOW));}}),
  }, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_DINOLFOS_CHEST, {[]{return IsAdult;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER, {[]{return (Bow || CanUse(LONGSHOT)) && CanUse(HOVER_BOOTS) && IsAdult;}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER] = Area("Gerudo Training Grounds MQ Underwater", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return HasFireSource && IsAdult && CanUse(IRON_BOOTS) && WaterTimer >= 24 && CanTakeDamage;}}),
  }, {});

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE] = Area("Gerudo Training Grounds MQ Left Side", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_FIRST_IRON_KNUCKLE_CHEST, {[]{return IsAdult || KokiriSword || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM, {[]{return IsAdult && CanUse(LONGSHOT);}}),
                    //Trick: (IsAdult && CanUse(LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && IsAdult && CanUse(HOOKSHOT))
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM] = Area("Gerudo Training Grounds MQ Stalfos Room", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_BEFORE_HEAVY_BLOCK_CHEST, {[]{return IsAdult;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_HEAVY_BLOCK_CHEST,        {[]{return CanUse(SILVER_GAUNTLETS);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS, {[]{return IsAdult && (LogicLensGtgMQ || CanUse(LENS_OF_TRUTH)) && BlueFire && CanPlay(SongOfTime);}}),
                    //Trick: IsAdult && (LogicLensGtgMQ || CanUse(LENS_OF_TRUTH)) && BlueFire && (CanPlay(SongOfTime) || (LogicGtgFakeWall && IsAdult && CanUse(HOVER_BOOTS)))
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS] = Area("Gerudo Training Grounds MQ Back Areas", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_EYE_STATUE_CHEST,          {[]{return Bow;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_SECOND_IRON_KNUCKLE_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_FLAME_CIRCLE_CHEST,        {[]{return CanUse(HOOKSHOT) || Bow || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT, {[]{return Hammer;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,         {[]{return CanUse(LONGSHOT);}}),
  });

  areaTable[GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds MQ Central Maze Right", "Gerudo Training Grounds", GERUDO_TRAINING_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(GERUDO_TRAINING_GROUNDS_MQ_ICE_ARROWS_CHEST,         {[]{return SmallKeys(GERUDO_TRAINING_GROUNDS, 3);}}),
  }, {
                  //Exits
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER,  {[]{return IsAdult && (CanUse(LONGSHOT) || (CanUse(HOOKSHOT) && Bow));}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,  {[]{return IsAdult && CanUse(HOOKSHOT);}}),
  });
  }
}
