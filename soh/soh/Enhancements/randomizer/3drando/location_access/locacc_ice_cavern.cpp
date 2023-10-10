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
  areaTable[ICE_CAVERN_ENTRYWAY] = Area("Ice Cavern Entryway", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_BEGINNING,    {[]{return Dungeon::IceCavern.IsVanilla();}}),
                  Entrance(ICE_CAVERN_MQ_BEGINNING, {[]{return Dungeon::IceCavern.IsMQ();}}),
                  Entrance(ZORAS_FOUNTAIN,          {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::IceCavern.IsVanilla()) {
  areaTable[ICE_CAVERN_BEGINNING] = Area("Ice Cavern Beginning", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(ICE_CAVERN_MAIN,     {[]{return Here(ICE_CAVERN_BEGINNING, []{return IsAdult || HasExplosives || CanUse(DINS_FIRE);});}}),
  });

  areaTable[ICE_CAVERN_MAIN] = Area("Ice Cavern", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess, {[]{return BlueFireAccess || (IsAdult && HasBottle);}}),
                }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MAP_CHEST,               {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_COMPASS_CHEST,           {[]{return BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_IRON_BOOTS_CHEST,        {[]{return BlueFire && (IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE));}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,                {[]{return BlueFire && IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_FREESTANDING_POH,        {[]{return BlueFire;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, {[]{return HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,     {[]{return BlueFire && HookshotOrBoomerang;}}),
                  LocationAccess(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,      {[]{return BlueFire && (HookshotOrBoomerang || (LogicIceBlockGS && IsAdult && CanUse(HOVER_BOOTS)));}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::IceCavern.IsMQ()) {
  areaTable[ICE_CAVERN_MQ_BEGINNING] = Area("Ice Cavern MQ Beginning", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(ICE_CAVERN_ENTRYWAY,             {[]{return true;}}),
                  Entrance(ICE_CAVERN_MQ_MAP_ROOM,          {[]{return IsAdult || CanUse(DINS_FIRE) || (HasExplosives && (CanUse(STICKS) || CanUse(SLINGSHOT) || KokiriSword));}}),
                  Entrance(ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return IsAdult && BlueFire;}}),
                  Entrance(ICE_CAVERN_MQ_IRON_BOOTS_REGION, {[]{return BlueFire;}}),
  });

  areaTable[ICE_CAVERN_MQ_MAP_ROOM] = Area("Ice Cavern MQ Map Room", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_MAP_CHEST, {[]{return BlueFire && (IsAdult || CanUse(STICKS) || KokiriSword || CanUseProjectile);}}),
  }, {});

  areaTable[ICE_CAVERN_MQ_IRON_BOOTS_REGION] = Area("Ice Cavern MQ Iron Boots Region", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, {[]{return IsAdult;}}),
                  LocationAccess(RC_SHEIK_IN_ICE_CAVERN,            {[]{return IsAdult;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     {[]{return IsAdult || CanUseProjectile;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_SCARECROW,     {[]{return (CanUse(SCARECROW) || (HoverBoots && CanUse(LONGSHOT)) || LogicIceMQScarecrow) && IsAdult;}}),
                    //Tricks: (CanUse(SCARECROW) || (HoverBoots && CanUse(LONGSHOT)) || LogicIceMQScarecrow) && IsAdult
  }, {});

  areaTable[ICE_CAVERN_MQ_COMPASS_ROOM] = Area("Ice Cavern MQ Compass Room", "Ice Cavern", RHT_ICE_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_FREESTANDING_POH, {[]{return HasExplosives;}}),
                  LocationAccess(RC_ICE_CAVERN_MQ_GS_RED_ICE,       {[]{return CanPlay(SongOfTime) || LogicIceMQRedIceGS;}}),
                    //Trick: CanPlay(SongOfTime) || LogicIceMQRedIceGS
  }, {});
  }
}
