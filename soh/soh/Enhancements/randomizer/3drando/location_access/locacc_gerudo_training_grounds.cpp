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
  areaTable[RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY] = Area("Gerudo Training Grounds Entryway", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LOBBY,    {[]{return Dungeon::GerudoTrainingGrounds.IsVanilla();}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, {[]{return Dungeon::GerudoTrainingGrounds.IsMQ();}}),
                  Entrance(RR_GERUDO_FORTRESS,                  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::GerudoTrainingGrounds.IsVanilla()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_LOBBY] = Area("Gerudo Training Grounds Lobby", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,  {[]{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, {[]{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,     {[]{return IsAdult || KokiriSword;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,      {[]{return HasExplosives && (IsAdult || KokiriSword);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,         {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM, {[]{return (IsAdult || KokiriSword) && (CanUse(RG_HOOKSHOT) || LogicGtgWithoutHookshot);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_LOBBY, []{return (IsAdult || KokiriSword) && HasExplosives;});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,     {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE] = Area("Gerudo Training Grounds Central Maze", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,   {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 3) && (LogicLensGtg || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 4);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 6);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 7);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds Central Maze Right", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,   {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM] = Area("Gerudo Training Grounds Lava Room", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return CanUse(RG_HOOKSHOT) && CanPlay(SongOfTime) && IronBoots && WaterTimer >= 24;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return CanPlay(SongOfTime) || IsChild;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,        {[]{return CanUse(RG_LONGSHOT)  || (CanUse(RG_HOVER_BOOTS) && CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM] = Area("Gerudo Training Grounds Hammer Room", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, {[]{return CanUse(RG_MEGATON_HAMMER) || (CanTakeDamage && LogicFlamingChests);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return CanUse(RG_MEGATON_HAMMER) && Bow;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER] = Area("Gerudo Training Grounds Eye Statue Lower", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, {[]{return CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER] = Area("Gerudo Training Grounds Eye Statue Upper", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, {[]{return CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM] = Area("Gerudo Training Grounds Heavy Block Room", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER, {[]{return (LogicLensGtg || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOOKSHOT) || (LogicGtgFakeWall && CanUse(RG_HOVER_BOOTS)));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,   {[]{return (LogicLensGtg || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOOKSHOT) || (LogicGtgFakeWall && CanUse(RG_HOVER_BOOTS))) && CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM] = Area("Gerudo Training Grounds Like Like Room", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, {[]{return true;}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::GerudoTrainingGrounds.IsMQ()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY] = Area("Gerudo Training Grounds MQ Lobby", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,   {[]{return LogicLensGtgMQ || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 1);}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,      {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,  {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return HasFireSource;});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE, {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return (IsAdult && CanUse(RG_FAIRY_BOW)) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));});}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE] = Area("Gerudo Training Grounds MQ Right Side", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, {[]{return IsAdult;}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER, {[]{return (Bow || CanUse(RG_LONGSHOT)) && CanUse(RG_HOVER_BOOTS) && IsAdult;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER] = Area("Gerudo Training Grounds MQ Underwater", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return HasFireSource && IsAdult && CanUse(RG_IRON_BOOTS) && WaterTimer >= 24 && CanTakeDamage;}}),
  }, {});

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE] = Area("Gerudo Training Grounds MQ Left Side", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, {[]{return IsAdult || KokiriSword || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM, {[]{return (IsAdult && CanUse(RG_LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && IsAdult && CanUse(RG_HOOKSHOT));}}),
                    //Trick: (IsAdult && CanUse(RG_LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && IsAdult && CanUse(RG_HOOKSHOT))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM] = Area("Gerudo Training Grounds MQ Stalfos Room", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, {[]{return IsAdult;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST,        {[]{return CanUse(RG_SILVER_GAUNTLETS);}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS, {[]{return IsAdult && (LogicLensGtgMQ || CanUse(RG_LENS_OF_TRUTH)) && BlueFire && (CanPlay(SongOfTime) || (LogicGtgFakeWall && IsAdult && CanUse(RG_HOVER_BOOTS)));}}),
                    //Trick: IsAdult && (LogicLensGtgMQ || CanUse(RG_LENS_OF_TRUTH)) && BlueFire && (CanPlay(SongOfTime) || (LogicGtgFakeWall && IsAdult && CanUse(RG_HOVER_BOOTS)))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS] = Area("Gerudo Training Grounds MQ Back Areas", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST,          {[]{return Bow;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,        {[]{return CanUse(RG_HOOKSHOT) || Bow || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT, {[]{return Hammer;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,         {[]{return CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds MQ Central Maze Right", "Gerudo Training Grounds", RHT_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST,         {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 3);}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER,  {[]{return IsAdult && (CanUse(RG_LONGSHOT) || (CanUse(RG_HOOKSHOT) && Bow));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,  {[]{return IsAdult && CanUse(RG_HOOKSHOT);}}),
  });
  }
}
