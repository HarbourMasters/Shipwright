#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_IceCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_ICE_CAVERN_ENTRYWAY] = Region("Ice Cavern Entryway", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_BEGINNING,    {[]{return ctx->GetDungeon(ICE_CAVERN)->IsVanilla();}}),
                  Entrance(RR_ICE_CAVERN_MQ_BEGINNING, {[]{return ctx->GetDungeon(ICE_CAVERN)->IsMQ() && logic->CanUseProjectile();}}),
                  Entrance(RR_ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(ICE_CAVERN)->IsVanilla()) {
  areaTable[RR_ICE_CAVERN_BEGINNING] = Region("Ice Cavern Beginning", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MAIN,     {[]{return Here(RR_ICE_CAVERN_BEGINNING, []{return (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE);});}}),
  });

  areaTable[RR_ICE_CAVERN_MAIN] = Region("Ice Cavern", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess, {[]{return logic->BlueFireAccess || (logic->IsAdult && logic->HasBottle());}}),
                }, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MAP_CHEST,               logic->BlueFire() && logic->IsAdult),
                  LOCATION(RC_ICE_CAVERN_COMPASS_CHEST,           logic->BlueFire()),
                  LOCATION(RC_ICE_CAVERN_IRON_BOOTS_CHEST,        logic->BlueFire() && (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE))),
                  LOCATION(RC_SHEIK_IN_ICE_CAVERN,                logic->BlueFire() && (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE)) && logic->IsAdult),
                  LOCATION(RC_ICE_CAVERN_FREESTANDING_POH,        logic->BlueFire()),
                  LOCATION(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, logic->HookshotOrBoomerang()),
                  LOCATION(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,     logic->BlueFire() && logic->HookshotOrBoomerang()),
                  LOCATION(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      logic->BlueFire() && (logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_ICE_BLOCK_GS) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(ICE_CAVERN)->IsMQ()) {
  areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Region("Ice Cavern MQ Beginning", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,          {[]{return logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_DINS_FIRE) || (logic->HasExplosives() && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return logic->IsAdult && logic->BlueFire();}}),
                  Entrance(RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return logic->BlueFire();}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Region("Ice Cavern MQ Map Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->BlueFireAccess || (logic->HasBottle() && logic->CanJumpslashExceptHammer());}}),
  }, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_MAP_CHEST, logic->BlueFire() && (logic->CanJumpslash() || logic->HasExplosives() || logic->CanUseProjectile())),
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Region("Ice Cavern MQ Iron Boots Region", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, logic->IsAdult && (logic->CanJumpslash())),
                  LOCATION(RC_SHEIK_IN_ICE_CAVERN,            logic->IsAdult && (logic->CanJumpslash())),
                  LOCATION(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     logic->CanAttack()),
                  LOCATION(RC_ICE_CAVERN_MQ_GS_SCARECROW,     logic->CanUse(RG_SCARECROW) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) || (ctx->GetTrickOption(RT_ICE_MQ_SCARECROW) && logic->IsAdult)),
                    //Tricks: (logic->CanUse(RG_SCARECROW) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_LONGSHOT)) || LogicIceMQScarecrow) && logic->IsAdult
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Region("Ice Cavern MQ Compass Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    true),
                  LOCATION(RC_ICE_CAVERN_MQ_FREESTANDING_POH, logic->HasExplosives()),
                  LOCATION(RC_ICE_CAVERN_MQ_GS_RED_ICE,       logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS)),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || LogicIceMQRedIceGS
  }, {});
  }
}
