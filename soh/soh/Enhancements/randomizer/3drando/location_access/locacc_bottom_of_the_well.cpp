#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_BottomOfTheWell() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_BOTTOM_OF_THE_WELL_ENTRYWAY] = Region("Bottom of the Well Entryway", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && logic->IsChild && (logic->CanAttack() || logic->CanUse(RG_NUTS));}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && logic->IsChild;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
  areaTable[RR_BOTTOM_OF_THE_WELL_MAIN_AREA] = Region("Bottom of the Well Main Region", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->StickPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,   ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,  logic->HasExplosives()),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,                ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,       ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,     (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives()),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,             (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_ZELDAS_LULLABY)) && logic->CanUse(RG_STICKS) || logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,          logic->CanUse(RG_ZELDAS_LULLABY) && (logic->CanUse(RG_KOKIRI_SWORD) || (logic->CanUse(RG_STICKS) && ctx->GetTrickOption(RT_BOTW_CHILD_DEADHAND)))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,              logic->CanUse(RG_ZELDAS_LULLABY) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,       logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,        logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MAP_CHEST,                    logic->HasExplosives() || (((logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))) || logic->CanUse(RG_DINS_FIRE) || (logic->CanUse(RG_STICKS) && ctx->GetTrickOption(RT_BOTW_BASEMENT))) && logic->HasItem(RG_GORONS_BRACELET))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,             logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,              logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,           logic->CanUse(RG_BOOMERANG) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,           logic->CanUse(RG_BOOMERANG) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,            logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 3) && (ctx->GetTrickOption(RT_LENS_BOTW) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_BOOMERANG)),
                }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()) {
    //RANDOTODO double check the CanAttacks when rewriting, i assumed CanChildAttack was the only one called because of the initial crawlspace
  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER] = Region("Bottom of the Well MQ Perimeter", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&WallFairy, {[]{return WallFairy || logic->Slingshot;}}),
  }, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              logic->CanUse(RG_KOKIRI_SWORD) || (logic->CanUse(RG_STICKS) && ctx->GetTrickOption(RT_BOTW_CHILD_DEADHAND))),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, logic->HasExplosives() || (ctx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && logic->CanUse(RG_BOOMERANG))),
                    //Trick: logic->HasExplosives() || (LogicBotWMQDeadHandKey && logic->CanUse(RG_BOOMERANG))
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,                logic->CanAttack()),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,             logic->CanAttack() && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2)),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY,  {[]{return true;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) || (ctx->GetTrickOption(RT_BOTW_MQ_PITS) && logic->HasExplosives());}}),
                    //Trick: logic->CanUse(RG_ZELDAS_LULLABY) || (LogicBotWMQPits && logic->HasExplosives())
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Region("Bottom of the Well MQ Middle", "Bottom of the Well", RA_BOTTOM_OF_THE_WELL, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,                        true),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,              logic->HasExplosives() && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, true),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               logic->CanAttack() && (ctx->GetTrickOption(RT_BOTW_MQ_PITS) || logic->HasExplosives())),
                    //Trick: logic->CanChildAttack && (LogicBotWMQPits || logic->HasExplosives())
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });
  }
}
