#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_IceCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_BEGINNING,    {[]{return randoCtx->GetDungeon(ICE_CAVERN)->IsVanilla();}}),
                  Entrance(RR_ICE_CAVERN_MQ_BEGINNING, {[]{return randoCtx->GetDungeon(ICE_CAVERN)->IsMQ() && logic->CanUseProjectile;}}),
                  Entrance(RR_ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(ICE_CAVERN)->IsVanilla()) {
  areaTable[RR_ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MAIN,     {[]{return Here(RR_ICE_CAVERN_BEGINNING, []{return (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE);});}}),
  });

  areaTable[RR_ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess, {[]{return logic->BlueFireAccess || (logic->IsAdult && logic->HasBottle);}}),
                }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MAP_CHEST,               {[]{return logic->BlueFire && logic->IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_COMPASS_CHEST,           {[]{return logic->BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_IRON_BOOTS_CHEST,        {[]{return logic->BlueFire && (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_DINS_FIRE));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,                {[]{return logic->BlueFire && (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_DINS_FIRE)) && logic->IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_FREESTANDING_POH,        {[]{return logic->BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, {[]{return logic->HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,     {[]{return logic->BlueFire && logic->HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return logic->BlueFire && (logic->HookshotOrBoomerang || (randoCtx->GetTrickOption(RT_ICE_BLOCK_GS) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)));}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(ICE_CAVERN)->IsMQ()) {
  areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,          {[]{return logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_DINS_FIRE) || (logic->HasExplosives && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return logic->IsAdult && logic->BlueFire;}}),
                  Entrance(RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return logic->BlueFire;}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->BlueFireAccess || (logic->HasBottle && logic->CanJumpslash);}}),
  }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_MAP_CHEST, {[]{return logic->BlueFire && (logic->CanJumpslash || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives || logic->CanUseProjectile);}}),
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return logic->IsAdult && (logic->CanJumpslash || logic->CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,            {[]{return logic->IsAdult && (logic->CanJumpslash || logic->CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return logic->CanAdultAttack || logic->CanChildAttack;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return logic->CanUse(RG_SCARECROW) || (logic->HoverBoots && logic->CanUse(RG_LONGSHOT)) || (randoCtx->GetTrickOption(RT_ICE_MQ_SCARECROW) && logic->IsAdult);}}),
                    //Tricks: (logic->CanUse(RG_SCARECROW) || (logic->HoverBoots && logic->CanUse(RG_LONGSHOT)) || LogicIceMQScarecrow) && logic->IsAdult
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return logic->HasExplosives;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS);}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || LogicIceMQRedIceGS
  }, {});
  }
}
