#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_JabuJabusBelly() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_JABU_JABUS_BELLY_ENTRYWAY] = Region("Jabu Jabus Belly Entryway", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_BEGINNING,    {[]{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla();}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ();}}),
                  Entrance(RR_ZORAS_FOUNTAIN,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla()) {
  areaTable[RR_JABU_JABUS_BELLY_BEGINNING] = Region("Jabu Jabus Belly Beginning", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY,    {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return logic->CanUseProjectile();}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_MIDDLE] = Region("Jabu Jabus Belly Lift Middle", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_BEGINNING,      {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM, {[]{return HasAccessTo(RR_JABU_JABUS_BELLY_LIFT_UPPER) || (ctx->GetTrickOption(RT_JABU_BOSS_HOVER) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAIN_UPPER] = Region("Jabu Jabus Belly Main Upper", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER,      {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BIGOCTO_ROOM,    {[]{return Here(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAIN_LOWER] = Region("Jabu Jabus Belly Main Lower", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, logic->HookshotOrBoomerang()),
                  LOCATION(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LOWER_SIDE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR] = Region("Jabu Jabus Belly Shabomb Corridor", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, true),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER, {[]{return logic->HasItem(RG_BRONZE_SCALE);}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER, {[]{return logic->HasItem(RG_BRONZE_SCALE) && logic->CanUseProjectile();}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LOWER_SIDE_ROOM] = Region("Jabu Jabus Belly Lower Side Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_HOVER_BOOTS));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_LOWER] = Region("Jabu Jabus Belly Lift Lower", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_DEKU_SCRUB, logic->HasItem(RG_BRONZE_SCALE) && (logic->IsChild || logic->HasItem(RG_SILVER_SCALE) || ctx->GetTrickOption(RT_JABU_ALCOVE_JUMP_DIVE) || logic->CanUse(RG_IRON_BOOTS)) && logic->CanStunDeku()),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE,      {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_FORKED_CORRIDOR] = Region("Jabu Jabus Belly Forked Corridor", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOOMERANG_ROOM, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAP_ROOM,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_COMPASS_ROOM,   {[]{return Here(RR_JABU_JABUS_BELLY_MAP_ROOM,      []{return logic->CanUse(RG_BOOMERANG);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_BLUE_TENTACLE,  {[]{return Here(RR_JABU_JABUS_BELLY_MAP_ROOM,      []{return logic->CanUse(RG_BOOMERANG);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, {[]{return Here(RR_JABU_JABUS_BELLY_BLUE_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BOOMERANG_ROOM] = Region("Jabu Jabus Belly Boomerang Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAP_ROOM] = Region("Jabu Jabus Belly Map Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MAP_CHEST, logic->CanUse(RG_BOOMERANG)),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_COMPASS_ROOM] = Region("Jabu Jabus Belly Compass Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_COMPASS_CHEST, logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BLUE_TENTACLE] = Region("Jabu Jabus Belly Blue Tentacle", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(RR_JABU_JABUS_BELLY_BLUE_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_GREEN_TENTACLE] = Region("Jabu Jabus Belly Green Tentacle", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BIGOCTO_ROOM] = Region("Jabu Jabus Belly Bigocto Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER,    {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, {[]{return Here(RR_JABU_JABUS_BELLY_BIGOCTO_ROOM, []{return (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_NUTS)) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_STICKS));});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Region("Jabu Jabus Belly Above Bigocto", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_UPPER, {[]{return logic->CanUse(RG_BOOMERANG);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_UPPER] = Region("Jabu Jabus Belly Lift Upper", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER,  {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Region("Jabu Jabus Belly Near Boss Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, {[]{return logic->CanUse(RG_BOOMERANG) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && ((logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW))) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)))) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_EXPLOSIVES) && (logic->CanUse(RG_BOMBCHU_5) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_BOMB_BAG))));}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ()) {
  areaTable[RR_JABU_JABUS_BELLY_MQ_BEGINNING] = Region("Jabu Jabus Belly MQ Beginning", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,             logic->BlastOrSmash()),
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,  {[]{return Here(RR_JABU_JABUS_BELLY_MQ_BEGINNING, []{return logic->CanUse(RG_FAIRY_SLINGSHOT);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM] = Region("Jabu Jabus Belly MQ Lift Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQJabuLiftRoomCow, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,  true),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING,            {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE,    {[]{return logic->HasItem(RG_SILVER_SCALE) || (logic->HasItem(RG_BRONZE_SCALE) && ((logic->IsChild || logic->CanUse(RG_IRON_BOOTS) || ctx->GetTrickOption(RT_JABU_ALCOVE_JUMP_DIVE))));}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,           {[]{return logic->MQJabuHolesRoomDoor;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE, {[]{return logic->LoweredJabuPath || logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && logic->MQJabuLiftRoomCow);}}),
                  //If opening RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM by lowering the geyser as 1 age is to let the other through is relevant, it needs an eventAccess
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_UNDERWATER_ALCOVE] = Region("Jabu Jabus Belly MQ Underwater Alcove", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQJabuHolesRoomDoor, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, logic->CanHitSwitch(ED_HOOKSHOT, true) || (ctx->GetTrickOption(RT_JABU_MQ_RANG_JUMP) && logic->CanUse(RG_BOOMERANG) && logic->HasItem(RG_BRONZE_SCALE))),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, {[]{return logic->HasItem(RG_BRONZE_SCALE);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM] = Region("Jabu Jabus Belly MQ Holes Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    logic->CanUse(RG_FAIRY_SLINGSHOT)),
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING,             {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,       {[]{return logic->CanUse(RG_BOOMERANG) && logic->HasExplosives() && Here(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, []{return logic->CanUse(RG_FAIRY_SLINGSHOT);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM,  {[]{return logic->JabuNorthTentacle;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_PAST_OCTO,             {[]{return logic->JabuWestTentacle && Here(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, []{return logic->CanKillEnemy(RE_BIG_OCTO);}) && logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM] = Region("Jabu Jabus Belly MQ Water Switch Room", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,  true),
                                                                               //Implies logic->CanKillEnemy(RE_LIKE_LIKE) && logic->CanKillEnemy(RE_STINGER). Without swim, jump from the song of time block to the vines.
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,             logic->CanKillEnemy(RE_LIZALFOS)),
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,     (logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)) || (ctx->GetTrickOption(RT_JABU_MQ_SOT_GS) && logic->CanUse(RG_BOOMERANG))),
  }, {
                  //Exits
                  //without swim, jump from rang chest to the other side
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING,  {[]{return Here(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, {[]{return (logic->IsAdult || logic->HasItem(RG_BRONZE_SCALE)) && Here(RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
  });

  //Includes Like Like room
  areaTable[RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR] = Region("Jabu Jabus Belly MQ Forked Corridor", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->JabuNorthTentacle, {[]{return Here(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->BlastOrSmash();}) && logic->CanUse(RG_BOOMERANG);}}),
  }, {
                  //Locations
                  //Implies CanKillEnemy(RE_LIKE_LIKE)
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,          {[]{return logic->CanUse(RG_BOOMERANG);}}),
                  //If some mode lets an age use sticks and not sling, and other use sling and not sticks, this needs changing
                  Entrance(RR_JABU_JABUS_BELLY_MQ_WEST_FORKED_ROOMS,   {[]{return Here(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->CanUse(RG_BOOMERANG);}) &&
                                                                                  (Here(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanUse(RG_STICKS);}) ||
                                                                                  Here(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, []{return logic->HasFireSource();}));}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_WEST_FORKED_ROOMS] = Region("Jabu Jabus Belly MQ West Forked Rooms", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->JabuWestTentacle, {[]{return logic->CanUse(RG_BOOMERANG);}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, Here(RR_JABU_JABUS_BELLY_MQ_WEST_FORKED_ROOMS, []{return logic->HasExplosives();}) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM] = Region("Jabu Jabus Belly MQ Depths", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,  //firstly, we can just use FAs to clear the web and then longshot the skull
                                                                              logic->CanUse(RG_FIRE_ARROWS) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_LONGSHOT) ||
                                                                              //Otherwise, we we have to cross the gap and kill the skull.
                                                                              (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) && 
                                                                              //We can cheese the gap with hovers
                                                                               ((logic->CanUse(RG_HOVER_BOOTS) ||
                                                                              //Otherwise we have to kill the enemies to raise the platform. This persists so we can do it as the other age.
                                                                                 Here(RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM, []{return (ctx->GetTrickOption(RT_LENS_JABU_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                                                                                          logic->CanKillEnemy(RE_STINGER, ED_BOOMERANG, false, 2, false, true) && 
                                                                                                                                          //we can hit the keese farthest from the water with irons and hookshot, but we won't be able to see it while doing so
                                                                                                                                          (logic->CanKillEnemy(RE_KEESE, ED_LONGSHOT, false) || (ctx->GetTrickOption(RT_LENS_JABU_MQ) && logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS)));}))
                                                                              //If we kill the enemies, we then need to cross the water using the platform. Note that adult cannot do so while swimming because MQ jank.
                                                                                 && ((logic->IsChild && logic->HasItem(RG_BRONZE_SCALE)) || (logic->IsAdult && logic->CanUse(RG_IRON_BOOTS)))))),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, {[]{return (logic->JabuNorthTentacle || logic->TakeDamage()) && logic->HasItem(RG_BRONZE_SCALE);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_PAST_OCTO] = Region("Jabu Jabus Belly MQ Past Octo", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //if a hover up to the path is added, this will want it's own room
                  EventAccess(&logic->LoweredJabuPath, {[]{return logic->CanUse(RG_BOOMERANG) && logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_COW, logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_FAIRY_SLINGSHOT)),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,  {[]{return logic->CanUse(RG_BOOMERANG) && logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  //you take both fall damage and tentacle damage, unless the tentacle is down. need better damage logic
                  Entrance(RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, {[]{return logic->TakeDamage() && Here(RR_JABU_JABUS_BELLY_MQ_PAST_OCTO, []{return logic->CanKillEnemy(RE_BIG_OCTO);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE] = Region("Jabu Jabus Belly MQ Lift Room East Ledge", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,  logic->MQJabuLiftRoomCow),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_EAST_ROOM, {[]{return logic->JabuNorthTentacle;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_EAST_ROOM] = Region("Jabu Jabus Belly MQ Boss Region", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, logic->CanUse(RG_FAIRY_SLINGSHOT)),
                  LOCATION(RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,    logic->CanUse(RG_BOOMERANG) || (ctx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && logic->CanUse(RG_HOOKSHOT))),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY,           {[]{return Here(RR_JABU_JABUS_BELLY_MQ_EAST_ROOM, []{return logic->CanUse(RG_FAIRY_SLINGSHOT);});}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY] =
        Region("Jabu Jabus Belly Boss Entryway", "Jabu Jabus Belly", {RA_JABU_JABUS_BELLY}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM, { [] { return ctx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla(); } }),
                 Entrance(RR_JABU_JABUS_BELLY_MQ_EAST_ROOM, { [] { return ctx->GetDungeon(JABU_JABUS_BELLY)->IsMQ(); } }),
                 Entrance(RR_JABU_JABUS_BELLY_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_ROOM] =
        Region("Jabu Jabus Belly Boss Room", "Jabu Jabus Belly", {}, NO_DAY_NIGHT_CYCLE,
             {
                 // Events //todo: add pot kill trick
                 EventAccess(&logic->JabuJabusBellyClear,
                             { [] { return logic->JabuJabusBellyClear || (logic->HasBossSoul(RG_BARINADE_SOUL) && (logic->CanUse(RG_BOOMERANG) && logic->CanJumpslashExceptHammer())); } }),
             },
             {
                 // Locations
                 LOCATION(RC_JABU_JABUS_BELLY_BARINADE_HEART, logic->JabuJabusBellyClear),
                 LOCATION(RC_BARINADE, logic->JabuJabusBellyClear),
             },
             {
                 // Exits
                 Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_ZORAS_FOUNTAIN, { [] { return logic->JabuJabusBellyClear; } }, false),
             });
}
