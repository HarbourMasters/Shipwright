#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_GerudoTrainingGrounds() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY] = Area("Gerudo Training Grounds Entryway", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LOBBY,    {[]{return randoCtx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsVanilla();}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, {[]{return randoCtx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsMQ();}}),
                  Entrance(RR_GERUDO_FORTRESS,                  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsVanilla()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_LOBBY] = Area("Gerudo Training Grounds Lobby", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,  {[]{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, {[]{return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,     {[]{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,      {[]{return HasExplosives && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,         {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM, {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (CanUse(RG_HOOKSHOT) || randoCtx->GetTrickOption(RT_GTG_WITHOUT_HOOKSHOT));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_LOBBY, []{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && HasExplosives;});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,     {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE] = Area("Gerudo Training Grounds Central Maze", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,   {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 3) && (randoCtx->GetTrickOption(RT_LENS_GTG) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 4);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 6);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 7);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds Central Maze Right", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,   {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM] = Area("Gerudo Training Grounds Lava Room", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return CanUse(RG_HOOKSHOT) && CanUse(RG_SONG_OF_TIME) && IronBoots && WaterTimer >= 24;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return CanUse(RG_SONG_OF_TIME) || IsChild;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,        {[]{return CanUse(RG_LONGSHOT)  || (CanUse(RG_HOVER_BOOTS) && CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM] = Area("Gerudo Training Grounds Hammer Room", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,  {[]{return CanAdultAttack || CanChildAttack;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, {[]{return CanUse(RG_MEGATON_HAMMER) || (CanTakeDamage && randoCtx->GetTrickOption(RT_FLAMING_CHESTS));}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return CanUse(RG_MEGATON_HAMMER) && CanUse(RG_FAIRY_BOW);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER] = Area("Gerudo Training Grounds Eye Statue Lower", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, {[]{return CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER] = Area("Gerudo Training Grounds Eye Statue Upper", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, {[]{return CanUse(RG_FAIRY_BOW);}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM] = Area("Gerudo Training Grounds Heavy Block Room", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, {[]{return CanJumpslash;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER, {[]{return (randoCtx->GetTrickOption(RT_LENS_GTG) || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_GTG_FAKE_WALL) && CanUse(RG_HOVER_BOOTS)));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,   {[]{return (randoCtx->GetTrickOption(RT_LENS_GTG) || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_GTG_FAKE_WALL) && CanUse(RG_HOVER_BOOTS))) && CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM] = Area("Gerudo Training Grounds Like Like Room", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,  {[]{return CanJumpslash;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, {[]{return CanJumpslash;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,  {[]{return CanJumpslash;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, {[]{return CanJumpslash;}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsMQ()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY] = Area("Gerudo Training Grounds MQ Lobby", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,   {[]{return randoCtx->GetTrickOption(RT_LENS_GTG_MQ) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, {[]{return true;}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST,  {[]{return SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 1);}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,      {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,  {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return HasFireSource;});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE, {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return (IsAdult && CanUse(RG_FAIRY_BOW)) || (IsChild && CanUse(RG_FAIRY_SLINGSHOT));});}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE] = Area("Gerudo Training Grounds MQ Right Side", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, {[]{return IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER, {[]{return (Bow || (CanUse(RG_LONGSHOT) && HasFireSource)) && CanUse(RG_HOVER_BOOTS) && IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER] = Area("Gerudo Training Grounds MQ Underwater", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, {[]{return HasFireSource && IsAdult && CanUse(RG_IRON_BOOTS) && WaterTimer >= 24 && CanTakeDamage;}}),
  }, {});

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE] = Area("Gerudo Training Grounds MQ Left Side", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, {[]{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM, {[]{return (IsAdult && CanUse(RG_LONGSHOT)) || randoCtx->GetTrickOption(RT_GTG_MQ_WIHTOUT_HOOKSHOT) || (randoCtx->GetTrickOption(RT_GTG_MQ_WITH_HOOKSHOT) && IsAdult && CanUse(RG_HOOKSHOT));}}),
                    //Trick: (IsAdult && CanUse(RG_LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && IsAdult && CanUse(RG_HOOKSHOT))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM] = Area("Gerudo Training Grounds MQ Stalfos Room", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, {[]{return IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST,        {[]{return CanUse(RG_SILVER_GAUNTLETS) && (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS, {[]{return IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (randoCtx->GetTrickOption(RT_LENS_GTG_MQ) || CanUse(RG_LENS_OF_TRUTH)) && BlueFire && (CanUse(RG_SONG_OF_TIME) || (randoCtx->GetTrickOption(RT_GTG_FAKE_WALL) && IsAdult && CanUse(RG_HOVER_BOOTS)));}}),
                    //Trick: IsAdult && (LogicLensGtgMQ || CanUse(RG_LENS_OF_TRUTH)) && BlueFire && (CanUse(RG_SONG_OF_TIME) || (LogicGtgFakeWall && IsAdult && CanUse(RG_HOVER_BOOTS)))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS] = Area("Gerudo Training Grounds MQ Back Areas", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST,          {[]{return CanUse(RG_FAIRY_BOW);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, {[]{return CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,        {[]{return CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW) || HasExplosives;}}),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT, {[]{return CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,         {[]{return CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT] = Area("Gerudo Training Grounds MQ Central Maze Right", "Gerudo Training Grounds", RA_GERUDO_TRAINING_GROUND, NO_DAY_NIGHT_CYCLE, {}, {
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
