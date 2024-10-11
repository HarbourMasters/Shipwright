#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_BottomOfTheWell() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_BOTTOM_OF_THE_WELL_ENTRYWAY] = Region("Bottom of the Well Entryway", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MAIN_AREA,    {[]{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla() && logic->IsChild && (logic->CanAttack() || logic->CanUse(RG_NUTS));}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsMQ()      && logic->IsChild;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
  areaTable[RR_BOTTOM_OF_THE_WELL_MAIN_AREA] = Region("Bottom of the Well Main Region", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
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
  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER] = Region("Bottom of the Well MQ Perimeter", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //technically obsolete due to a wonder item fairy which only needs a projectile, but we don't have an event var for it yet
                  EventAccess(&logic->FairyPot,               {[]{return Here(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, []{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets();}}),
                  //It is possible to hit the water switch with a pot from RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE, however the hitbox for making it activate is very unintuitive
                  //You have to throw the pot from further back to hit the switch from the front instead of the top, trying to hit the "fingers" directly
                  //This unintuitiveness means it should be a trick. ZL is needed to get a clear path to carry the pot
                  EventAccess(&logic->LoweredWaterInsideBotw, {[]{return Here(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, []{return logic->CanJumpslash() || logic->CanUseProjectile();});}}),
  }, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY,            {[]{return logic->IsChild;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_WEST_ROOM_SWITCH, {[]{return Here(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, []{return logic->BlastOrSmash();}) && logic->CanPassEnemy(RE_BIG_SKULLTULA);}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM,      {[]{return (logic->LoweredWaterInsideBotw || logic->HasItem(RG_BRONZE_SCALE)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_LOCKED_CAGE,      {[]{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2) && logic->CanUseProjectile();}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_ROOM,   {[]{return logic->IsChild && logic->LoweredWaterInsideBotw;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE,           {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT,         {[]{return true;}}),
  });
  
  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_WEST_ROOM_SWITCH] = Region("Bottom of the Well MQ West Room Switch", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->OpenedWestRoomMQBotw, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return logic->BlastOrSmash() && (logic->CanPassEnemy(RE_BIG_SKULLTULA) || ctx->GetTrickOption(RT_BOTW_MQ_PITS));}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE,    {[]{return (bool)ctx->GetTrickOption(RT_BOTW_MQ_PITS);}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT,  {[]{return true;}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM] = Region("Bottom of the Well MQ Coffin Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER,  {[]{return (logic->LoweredWaterInsideBotw || logic->HasItem(RG_BRONZE_SCALE)) && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_LOCKED_CAGE] = Region("Bottom of the Well MQ Locked Cage", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->OpenedMiddleHoleMQBotw, {[]{return logic->HasExplosives();}}),
  }, {}, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_LOCKED_CAGE, {[]{return logic->IsChild && logic->SmallKeys(RR_BOTTOM_OF_THE_WELL, 2);}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_ROOM] = Region("Bottom of the Well MQ Dead Hand Room", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,              logic->CanKillEnemy(RE_DEAD_HAND)),
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, logic->HasExplosives() || (ctx->GetTrickOption(RT_BOTW_MQ_DEADHAND_KEY) && logic->CanUse(RG_BOOMERANG))),
  }, {
                  //Exits
                  //This assumes we spawned in dead hand's room, if whatever trick made this relevant instead puts us in the previous room, remove the kill Dead Hand check.
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER,  {[]{return logic->IsChild && logic->CanKillEnemy(RE_DEAD_HAND);}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE] = Region("Bottom of the Well MQ Middle", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,              true),
                  //This location technically involves an invisible platform, but it's intended to do lensless in vanilla and is clearly signposted by pots.
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, true),
                  //The enemies in this room are invisible and crowd around the player, being awkward to deal with blind unless you already know how.
                  //the right wall is safe, and can be followed to get behind the grave which you can then pull easily assuming you can tank invisible keese
                  //Using a deku nut however stuns everything easily. and if you have a melee weapon you can kill the skull through the grave then grab the drop
                  //though it can be hard to tell where the safe direct path to the grave is without lens.
                  //Also you get cheap shotted on entry sometimes.
                  //An MQ lens trick is recommended here, and a review of this room for OHKO logic what that is added is advised. 
                  //In the meantime I assume damage taken or the easy answer (nuts)
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,               logic->OpenedWestRoomMQBotw && (logic->TakeDamage() || logic->CanUse(RG_NUTS)) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SWITCH_PLATFORM, {[]{return logic->OpenedMiddleHoleMQBotw;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT,  {[]{return true;}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT] = Region("Bottom of the Well MQ Perimeter", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //behind invisible big skulltulas, but with navi spotting it's easy to avoid them, or at worst, take your way through as they do not block the path
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER, {[]{return true;}}),
  });

  areaTable[RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SWITCH_PLATFORM] = Region("Bottom of the Well MQ Perimeter", "Bottom of the Well", {RA_BOTTOM_OF_THE_WELL}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //Assumes RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT access
                  //it is technically possible to get the chest before you get screamed at without rolling, but hard enough to be a trick if that is the requirement for something to be logical
                  //With some kind of movement tech it's much easier, easy enough to be default logic, as the redeads don't lock on immediately in addition to the extra speed
                  //leaving with no requirements for now but up for discussion.
                  LOCATION(RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT,  {[]{return true;}}),
  });
  }
}
