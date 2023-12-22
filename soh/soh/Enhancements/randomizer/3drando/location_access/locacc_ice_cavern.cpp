#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_IceCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_BEGINNING,    {[]{return randoCtx->GetDungeon(ICE_CAVERN)->IsVanilla();}}),
                  Entrance(RR_ICE_CAVERN_MQ_BEGINNING, {[]{return randoCtx->GetDungeon(ICE_CAVERN)->IsMQ();}}),
                  Entrance(RR_ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(ICE_CAVERN)->IsVanilla()) {
  areaTable[RR_ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MAIN,     {[]{return Here(RR_ICE_CAVERN_BEGINNING, []{return (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_HOOKSHOT) || CanUse(RG_DINS_FIRE);});}}),
  });

  areaTable[RR_ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess, {[]{return BlueFireAccess || (IsAdult && HasBottle);}}),
                }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MAP_CHEST,               {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_COMPASS_CHEST,           {[]{return BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_IRON_BOOTS_CHEST,        {[]{return BlueFire && (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_DINS_FIRE));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,                {[]{return BlueFire && (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_DINS_FIRE)) && IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_FREESTANDING_POH,        {[]{return BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, {[]{return HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,     {[]{return BlueFire && HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return BlueFire && (HookshotOrBoomerang || (randoCtx->GetTrickOption(RT_ICE_BLOCK_GS) && IsAdult && CanUse(RG_HOVER_BOOTS)));}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(ICE_CAVERN)->IsMQ()) {
  areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,          {[]{return CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_DINS_FIRE) || (HasExplosives && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_STICKS) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return IsAdult && BlueFire;}}),
                  Entrance(RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return BlueFire;}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || (HasBottle && CanJumpslash);}}),
  }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_MAP_CHEST, {[]{return BlueFire && (CanJumpslash || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUseProjectile);}}),
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,            {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return CanAdultAttack || CanChildAttack;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return CanUse(RG_SCARECROW) || (HoverBoots && CanUse(RG_LONGSHOT)) || (randoCtx->GetTrickOption(RT_ICE_MQ_SCARECROW) && IsAdult);}}),
                    //Tricks: (CanUse(RG_SCARECROW) || (HoverBoots && CanUse(RG_LONGSHOT)) || LogicIceMQScarecrow) && IsAdult
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", RA_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return HasExplosives;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS);}}),
                    //Trick: CanUse(RG_SONG_OF_TIME) || LogicIceMQRedIceGS
  }, {});
  }
}
