#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_IceCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_BEGINNING,    {[]{return Dungeon::IceCavern.IsVanilla();}}),
                  Entrance(RR_ICE_CAVERN_MQ_BEGINNING, {[]{return Dungeon::IceCavern.IsMQ() && CanUseProjectile;}}),
                  Entrance(RR_ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::IceCavern.IsVanilla()) {
  areaTable[RR_ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MAIN,     {[]{return Here(RR_ICE_CAVERN_BEGINNING, []{return (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_HOOKSHOT) || CanUse(RG_DINS_FIRE);});}}),
  });

  areaTable[RR_ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
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
                  LocationAccess(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return BlueFire && (HookshotOrBoomerang || (LogicIceBlockGS && IsAdult && CanUse(RG_HOVER_BOOTS)));}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::IceCavern.IsMQ()) {
  areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,          {[]{return CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_DINS_FIRE) || (HasExplosives && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_STICKS) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return IsAdult && BlueFire;}}),
                  Entrance(RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return BlueFire;}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || (HasBottle && CanJumpslash);}}),
  }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_MAP_CHEST, {[]{return BlueFire && (CanJumpslash || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUseProjectile);}}),
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,            {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return CanAdultAttack || CanChildAttack;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return CanUse(RG_SCARECROW) || (HoverBoots && CanUse(RG_LONGSHOT)) || (LogicIceMQScarecrow && IsAdult);}}),
                    //Tricks: (CanUse(RG_SCARECROW) || (HoverBoots && CanUse(RG_LONGSHOT)) || LogicIceMQScarecrow) && IsAdult
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return HasExplosives;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return CanPlay(SongOfTime) || LogicIceMQRedIceGS;}}),
                    //Trick: CanPlay(SongOfTime) || LogicIceMQRedIceGS
  }, {});
  }
}
