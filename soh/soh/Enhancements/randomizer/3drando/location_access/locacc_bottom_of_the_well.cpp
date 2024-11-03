#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_BottomOfTheWell() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_BOTTOM_OF_THE_WELL_ENTRYWAY] = Area("Bottom of the Well Entryway", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && logic->IsChild && (logic->CanChildAttack || logic->CanUse(RG_NUTS));}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && logic->IsChild;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
  areaTable[RR_BOTTOM_OF_THE_WELL_MAIN_AREA] = Area("Bottom of the Well Main Area", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->StickPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  logic->HasExplosives),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,                randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,       randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,             (logic->Swim || logic->CanUse(RG_ZELDAS_LULLABY)) && logic->CanUse(RG_STICKS) || logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,          logic->CanUse(RG_ZELDAS_LULLABY) && (logic->KokiriSword || (logic->CanUse(RG_STICKS) && randoCtx->GetTrickOption(RT_BOTW_CHILD_DEADHAND)))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,              logic->CanUse(RG_ZELDAS_LULLABY) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MAP_CHEST,                    logic->HasExplosives || (((logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))) || logic->CanUse(RG_DINS_FIRE) || (logic->CanUse(RG_STICKS) && randoCtx->GetTrickOption(RT_BOTW_BASEMENT))) && logic->GoronBracelet)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,             logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,              logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,           logic->Boomerang && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,           logic->Boomerang && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,            logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (randoCtx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->Boomerang),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_1,              logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_2,              logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_3,              logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_1,          logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_2,          logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_POT_1,             logic->CanBreakPots && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_POT,               (logic->CanBreakPots && logic->CanUse(RG_ZELDAS_LULLABY)) || logic->Boomerang),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_1,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_2,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_3,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_4,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_5,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_6,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_7,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_8,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_9,               logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_10,              logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_11,              logic->CanBreakPots),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_12,              logic->CanBreakPots),
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
                  //EventAccess(&WallFairy, {[]{return WallFairy || logic->Slingshot;}}),
  }, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              logic->KokiriSword || (logic->CanUse(RG_STICKS) && randoCtx->GetTrickOption(RT_BOTW_CHILD_DEADHAND))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, logic->HasExplosives || (randoCtx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && logic->Boomerang)),
                    //Trick: logic->HasExplosives || (LogicBotWMQDeadHandKey && logic->Boomerang)
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                logic->CanChildAttack),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,             logic->CanChildAttack && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2)),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY,  {[]{return true;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) || (randoCtx->GetTrickOption(RT_BOTW_MQ_PITS) && logic->HasExplosives);}}),
                    //Trick: logic->CanUse(RG_ZELDAS_LULLABY) || (LogicBotWMQPits && logic->HasExplosives)
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Area("Bottom of the Well MQ Middle", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,                        true),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,              logic->HasExplosives && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, true),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               logic->CanChildAttack && (randoCtx->GetTrickOption(RT_BOTW_MQ_PITS) || logic->HasExplosives)),
                    //Trick: logic->CanChildAttack && (LogicBotWMQPits || logic->HasExplosives)
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });
  }
}
