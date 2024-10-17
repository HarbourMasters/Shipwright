#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_GerudoTrainingGrounds() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY] = Region("Gerudo Training Grounds Entryway", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LOBBY,    {[]{return ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsVanilla();}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, {[]{return ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsMQ();}}),
                  Entrance(RR_GERUDO_FORTRESS,                  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsVanilla()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_LOBBY] = Region("Gerudo Training Grounds Lobby", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,  logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,     logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,      logic->HasExplosives() && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,         {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM, {[]{return (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && (logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GTG_WITHOUT_HOOKSHOT));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_LOBBY, []{return (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->HasExplosives();});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE,     {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE] = Region("Gerudo Training Grounds Central Maze", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,   logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 3) && (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,  logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 4)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 6)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,  logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 7)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,  logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9)),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 9);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT] = Region("Gerudo Training Grounds Central Maze Right", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,    true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,         true),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,   {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM] = Region("Gerudo Training Grounds Lava Room", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_CENTRAL_MAZE_RIGHT, {[]{return logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM,        {[]{return logic->CanUse(RG_LONGSHOT)  || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM] = Region("Gerudo Training Grounds Hammer Room", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,  logic->CanAttack()),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, logic->CanUse(RG_MEGATON_HAMMER) || (logic->TakeDamage() && ctx->GetTrickOption(RT_FLAMING_CHESTS))),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_FAIRY_BOW);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LAVA_ROOM,        {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER] = Region("Gerudo Training Grounds Eye Statue Lower", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, logic->CanUse(RG_FAIRY_BOW)),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_HAMMER_ROOM, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER] = Region("Gerudo Training Grounds Eye Statue Upper", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, logic->CanUse(RG_FAIRY_BOW)),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_LOWER, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_ROOM] = Region("Gerudo Training Grounds Heavy Block Room", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, logic->CanJumpslashExceptHammer()),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_EYE_STATUE_UPPER, {[]{return (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_GTG_FAKE_WALL) && logic->CanUse(RG_HOVER_BOOTS)));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM,   {[]{return (ctx->GetTrickOption(RT_LENS_GTG) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_GTG_FAKE_WALL) && logic->CanUse(RG_HOVER_BOOTS))) && logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_LIKE_LIKE_ROOM] = Region("Gerudo Training Grounds Like Like Room", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,  logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,  logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, logic->CanJumpslashExceptHammer()),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(GERUDO_TRAINING_GROUNDS)->IsMQ()) {
  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY] = Region("Gerudo Training Grounds MQ Lobby", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,       true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,      true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,   ctx->GetTrickOption(RT_LENS_GTG_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,  true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST,  logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 1)),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY,      {[]{return true;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE,  {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return logic->HasFireSource();});}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE, {[]{return Here(RR_GERUDO_TRAINING_GROUNDS_MQ_LOBBY, []{return (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT));});}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE] = Region("Gerudo Training Grounds MQ Right Side", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW));}}),
  }, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER, {[]{return (logic->CanUse(RG_FAIRY_BOW) || (logic->CanUse(RG_LONGSHOT) && logic->HasFireSource())) && logic->CanUse(RG_HOVER_BOOTS) && logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD));}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER] = Region("Gerudo Training Grounds MQ Underwater", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, logic->HasFireSource() && logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24 && logic->TakeDamage()),
  }, {});

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_LEFT_SIDE] = Region("Gerudo Training Grounds MQ Left Side", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->HasExplosives()),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM, {[]{return (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || ctx->GetTrickOption(RT_GTG_MQ_WIHTOUT_HOOKSHOT) || (ctx->GetTrickOption(RT_GTG_MQ_WITH_HOOKSHOT) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT));}}),
                    //Trick: (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || LogicGtgMQWithoutHookshot || (LogicGtgMQWithHookshot && logic->IsAdult && logic->CanUse(RG_HOOKSHOT))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_STALFOS_ROOM] = Region("Gerudo Training Grounds MQ Stalfos Room", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->BlueFireAccess || logic->HasBottle();}}),
  }, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST,        logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS, {[]{return logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && (ctx->GetTrickOption(RT_LENS_GTG_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->BlueFire() && (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_GTG_FAKE_WALL) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)));}}),
                    //Trick: logic->IsAdult && (LogicLensGtgMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->BlueFire && (logic->CanUse(RG_SONG_OF_TIME) || (LogicGtgFakeWall && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_BACK_AREAS] = Region("Gerudo Training Grounds MQ Back Areas", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST,          logic->CanUse(RG_FAIRY_BOW)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,        logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->HasExplosives()),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,         {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GERUDO_TRAINING_GROUNDS_MQ_CENTRAL_MAZE_RIGHT] = Region("Gerudo Training Grounds MQ Central Maze Right", "Gerudo Training Grounds", {RA_GERUDO_TRAINING_GROUND}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST,    true),
                  LOCATION(RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST,         logic->SmallKeys(RR_GERUDO_TRAINING_GROUNDS, 3)),
  }, {
                  //Exits
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER,  {[]{return logic->IsAdult && (logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_MQ_RIGHT_SIDE,  {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}}),
  });
  }
}
