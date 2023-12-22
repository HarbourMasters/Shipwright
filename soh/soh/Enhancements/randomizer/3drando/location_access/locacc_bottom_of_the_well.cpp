#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_BottomOfTheWell() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_BOTTOM_OF_THE_WELL_ENTRYWAY] = Area("Bottom of the Well Entryway", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && IsChild && (CanChildAttack || Nuts);}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && IsChild;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
  areaTable[RR_BOTTOM_OF_THE_WELL_MAIN_AREA] = Area("Bottom of the Well Main Area", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&StickPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   {[]{return randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  {[]{return HasExplosives;}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, {[]{return randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,                {[]{return randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,       {[]{return randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     {[]{return (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,             {[]{return Sticks || CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,          {[]{return CanUse(RG_ZELDAS_LULLABY) && (KokiriSword || (Sticks && randoCtx->GetTrickOption(RT_BOTW_CHILD_DEADHAND)));}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,              {[]{return CanUse(RG_ZELDAS_LULLABY) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       {[]{return CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        {[]{return CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MAP_CHEST,                    {[]{return HasExplosives || (((SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH))) || CanUse(RG_DINS_FIRE) || (Sticks && randoCtx->GetTrickOption(RT_BOTW_BASEMENT))) && GoronBracelet);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,             {[]{return SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,              {[]{return SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,           {[]{return Boomerang && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH)) && SmallKeys(RR_BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,           {[]{return Boomerang && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH)) && SmallKeys(RR_BOTTOM_OF_THE_WELL, 3);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,            {[]{return SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || CanUse(RG_LENS_OF_TRUTH)) && Boomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()) {
  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER] = Area("Bottom of the Well MQ Perimeter", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || Slingshot;}}),
  }, {
                  //Locations
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              {[]{return KokiriSword || (Sticks && randoCtx->GetTrickOption(RT_BOTW_CHILD_DEADHAND));}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, {[]{return HasExplosives || (randoCtx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && Boomerang);}}),
                    //Trick: HasExplosives || (LogicBotWMQDeadHandKey && Boomerang)
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                {[]{return CanChildAttack;}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,             {[]{return CanChildAttack && SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}}),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY,  {[]{return true;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE, {[]{return CanUse(RG_ZELDAS_LULLABY) || (randoCtx->GetTrickOption(RT_BOTW_MQ_PITS) && HasExplosives);}}),
                    //Trick: CanUse(RG_ZELDAS_LULLABY) || (LogicBotWMQPits && HasExplosives)
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Area("Bottom of the Well MQ Middle", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,                        {[]{return true;}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,              {[]{return HasExplosives && SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, {[]{return true;}}),
                  LocationAccess(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               {[]{return CanChildAttack && (randoCtx->GetTrickOption(RT_BOTW_MQ_PITS) || HasExplosives);}}),
                    //Trick: CanChildAttack && (LogicBotWMQPits || HasExplosives)
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });
  }
}
