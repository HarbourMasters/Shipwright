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
  areaTable[RR_ICE_CAVERN_MQ_BEGINNING] = Region("Ice Cavern MQ Beginning", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_ENTRYWAY, {[]{return true;}}),
                  //It is in logic to use a pot to hit the toggle switch here.
                  Entrance(RR_ICE_CAVERN_MQ_HUB,   {[]{return true;}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_HUB] = Region("Ice Cavern MQ Hub", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  //the switch for the glass blocking the entrance is linked to the switch that controls the glass around the skulltulla in RR_ICE_CAVERN_MQ_SCARECROW_ROOM
                  //if you clear the ice, you can hit it with a pot from here.
                  Entrance(RR_ICE_CAVERN_BEGINNING,            {[]{return logic->BlueFire();}}),
                  Entrance(RR_ICE_CAVERN_MQ_MAP_ROOM,          {[]{return Here(RR_ICE_CAVERN_MQ_BEGINNING, []{return logic->CanKillEnemy(RE_WHITE_WOLFOS) && logic->CanKillEnemy(RE_FREEZARD);});}}),
                  Entrance(RR_ICE_CAVERN_MQ_COMPASS_ROOM,      {[]{return logic->IsAdult && logic->BlueFire();}}),
                  Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM, {[]{return logic->BlueFire();}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_MAP_ROOM] = Region("Ice Cavern MQ Map Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //Child can fit between the stalagmites on the left hand side
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->IsChild || logic->CanJumpslash() || logic->HasExplosives();}}),
  }, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_MAP_CHEST, logic->BlueFire() && Here(RR_ICE_CAVERN_MQ_MAP_ROOM, []{return logic->CanHitSwitch();})),
  }, {});

  areaTable[RR_ICE_CAVERN_MQ_SCARECROW_ROOM] = Region("Ice Cavern MQ Scarecrow Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,     (logic->BlueFire() && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) || (logic->IsAdult && logic->CanHitSwitch(ED_LONG_JUMPSLASH))),
                  LOCATION(RC_ICE_CAVERN_MQ_GS_SCARECROW,     logic->CanUse(RG_SCARECROW) || (logic->IsAdult && (logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_ICE_MQ_SCARECROW)))),
  }, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_MQ_HUB,          {[]{return logic->BlueFire();}}),
                  //Assumes RR_ICE_CAVERN_MQ_HUB access for a pot if using blue fire
                  Entrance(RR_ICE_CAVERN_MQ_STALFOS_ROOM, {[]{return logic->IsAdult && logic->BlueFire();}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_STALFOS_ROOM] = Region("Ice Cavern MQ Stalfos Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, logic->CanKillEnemy(RE_STALFOS)),
                  LOCATION(RC_SHEIK_IN_ICE_CAVERN,            logic->CanKillEnemy(RE_STALFOS)),
  }, {
                  //Exits
                  Entrance(RR_ICE_CAVERN_MQ_SCARECROW_ROOM, {[]{return logic->BlueFire() && Here(RR_ICE_CAVERN_MQ_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}}),
                  Entrance(RR_ICE_CAVERN_MQ_BEGINNING,      {[]{return logic->CanUse(RG_IRON_BOOTS) && Here(RR_ICE_CAVERN_MQ_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}}),
  });

  areaTable[RR_ICE_CAVERN_MQ_COMPASS_ROOM] = Region("Ice Cavern MQ Compass Room", "Ice Cavern", {RA_ICE_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_ICE_CAVERN_MQ_COMPASS_CHEST,    true),
                  //It is possible for child with master, BGS or sticks, or adult with BGS, to hit this switch through the ice with a crouchstab, but it's precise and unintuitive for a trick
                  LOCATION(RC_ICE_CAVERN_MQ_FREESTANDING_POH, logic->HasExplosives()),
                  //doing RT_ICE_MQ_RED_ICE_GS as child is untested, as I could not perform the trick reliably even as adult
                  LOCATION(RC_ICE_CAVERN_MQ_GS_RED_ICE,       (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && logic->CanUse(RG_ICE_ARROWS)) || 
                                                              (logic->CanUse(RG_BOTTLE_WITH_BLUE_FIRE) && (logic->CanUse(RG_SONG_OF_TIME) || (logic->IsAdult && ctx->GetTrickOption(RT_ICE_MQ_RED_ICE_GS))) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA))),
  }, {});
  }
}
