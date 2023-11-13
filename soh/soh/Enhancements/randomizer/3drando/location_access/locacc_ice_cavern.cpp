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
  areaTable[ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_BEGINNING,    {[]{return Dungeon::IceCavern.IsVanilla();}}),
                  Entrance(ICE_CAVERN_MQ_BEGINNING, {[]{return Dungeon::IceCavern.IsMQ();}}),
                  Entrance(ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::IceCavern.IsVanilla()) {
  areaTable[ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(ICE_CAVERN_MAIN,     {[]{return Here(ICE_CAVERN_BEGINNING, []{return (CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(HOOKSHOT) || CanUse(DINS_FIRE);});}}),
  });

  areaTable[ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess, {[]{return BlueFireAccess || (IsAdult && HasBottle);}}),
                }, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MAP_CHEST,               {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(ICE_CAVERN_COMPASS_CHEST,           {[]{return BlueFire;}}),
                  LocationAccess(ICE_CAVERN_IRON_BOOTS_CHEST,        {[]{return BlueFire && (CanJumpslash || CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(MEGATON_HAMMER) || CanUse(DINS_FIRE));}}),
                  LocationAccess(SHEIK_IN_ICE_CAVERN,                {[]{return BlueFire && (CanJumpslash || CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(MEGATON_HAMMER) || CanUse(DINS_FIRE)) && IsAdult;}}),
                  LocationAccess(ICE_CAVERN_FREESTANDING_POH,        {[]{return BlueFire;}}),
                  LocationAccess(ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, {[]{return HookshotOrBoomerang;}}),
                  LocationAccess(ICE_CAVERN_GS_HEART_PIECE_ROOM,     {[]{return BlueFire && HookshotOrBoomerang;}}),
                  LocationAccess(ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return BlueFire && (HookshotOrBoomerang || (LogicIceBlockGS && IsAdult && CanUse(HOVER_BOOTS)));}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::IceCavern.IsMQ()) {
  areaTable[ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(ICE_CAVERN_MQ_MAP_ROOM,          {[]{return CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || CanUse(DINS_FIRE) || (HasExplosives && (CanUse(KOKIRI_SWORD) || CanUse(STICKS) || CanUse(SLINGSHOT) || CanUse(BOW)));}}),
                  Entrance(ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return IsAdult && BlueFire;}}),
                  Entrance(ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return BlueFire;}}),
  });

  areaTable[ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || (HasBottle && CanJumpslash);}}),
  }, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_MAP_CHEST, {[]{return BlueFire && (CanJumpslash || HasExplosives || CanUseProjectile);}}),
  }, {});

  areaTable[ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(SHEIK_IN_ICE_CAVERN,            {[]{return IsAdult && (CanJumpslash || CanUse(RG_MEGATON_HAMMER));}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return CanAdultAttack || CanChildAttack;}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return CanUse(SCARECROW) || (HoverBoots && CanUse(LONGSHOT)) || (LogicIceMQScarecrow && IsAdult);}}),
                    //Tricks: (CanUse(SCARECROW) || (HoverBoots && CanUse(LONGSHOT)) || LogicIceMQScarecrow) && IsAdult
  }, {});

  areaTable[ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return HasExplosives;}}),
                  LocationAccess(ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return CanPlay(SongOfTime) || LogicIceMQRedIceGS;}}),
                    //Trick: CanPlay(SongOfTime) || LogicIceMQRedIceGS
  }, {});
  }
}
