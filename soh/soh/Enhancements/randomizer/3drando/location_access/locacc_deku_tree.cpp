#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_DekuTree() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_DEKU_TREE_ENTRYWAY] = Region("Deku Tree Entryway", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,          {[]{return ctx->GetDungeon(DEKU_TREE)->IsVanilla();}}),
                  Entrance(RR_DEKU_TREE_MQ_1F, {[]{return ctx->GetDungeon(DEKU_TREE)->IsMQ();}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE,     {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(DEKU_TREE)->IsVanilla()) {
  areaTable[RR_DEKU_TREE_LOBBY] = Region("Deku Tree Lobby", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}}),
                }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MAP_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM,    {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_COMPASS_ROOM,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,    {[]{return Here(RR_DEKU_TREE_LOBBY, []{return logic->CanAttack() || logic->CanUse(RG_NUTS);});}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return false;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_2F_MIDDLE_ROOM] = Region("Deku Tree 2F Middle Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,         {[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);});}}),
                  Entrance(RR_DEKU_TREE_SLINGSHOT_ROOM,{[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return logic->CanReflectNuts() || logic->CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_DEKU_TREE_SLINGSHOT_ROOM] = Region("Deku Tree Slingshot Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_SLINGSHOT_CHEST,           true),
                  LOCATION(RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_DEKU_TREE_COMPASS_ROOM] = Region("Deku Tree Compass Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}}),
                }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_COMPASS_CHEST,           true),
                  LOCATION(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, true),
                  LOCATION(RC_DEKU_TREE_GS_COMPASS_ROOM,         logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,     {[]{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_LOWER] = Region("Deku Tree Basement Lower", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();},
                                    /*Glitched*/[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_BASEMENT_CHEST,    true),
                  LOCATION(RC_DEKU_TREE_GS_BASEMENT_GATE,  logic->CanJumpslashExceptHammer() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_DEKU_TREE_GS_BASEMENT_VINES, logic->CanUseProjectile() || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS) && logic->CanJumpslashExceptHammer())),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,               {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return Here(RR_DEKU_TREE_BASEMENT_LOWER, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return logic->IsAdult || ctx->GetTrickOption(RT_DEKU_B1_SKIP) || HasAccessTo(RR_DEKU_TREE_BASEMENT_UPPER);}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_SCRUB_ROOM] = Region("Deku Tree Basement Scrub Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,            {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree Basement Water Room Front", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, {[]{return logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG);}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree Basement Water Room Back", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, {[]{return logic->HasItem(RG_BRONZE_SCALE) || ctx->GetTrickOption(RT_DEKU_B1_BACKFLIP_OVER_SPIKED_LOG);}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM,       {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_TORCH_ROOM] = Region("Deku Tree Basement Torch Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY,      {[]{return Here(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_LOBBY] = Region("Deku Tree Basement Back Lobby", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->DekuBabaSticks || (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_BOOMERANG));}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->DekuBabaNuts   || (Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) &&
                                                                            (logic->CanJumpslash() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE)));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_ROOM,  {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) &&
                                                                     Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);}) && logic->IsChild;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_ROOM] = Region("Deku Tree Basement Back Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_UPPER] = Region("Deku Tree Basement Upper", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return logic->IsChild;}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return Here(RR_DEKU_TREE_BASEMENT_UPPER, []{return logic->HasFireSourceWithTorch() || (ctx->GetTrickOption(RT_DEKU_B1_BOW_WEBS) && logic->IsAdult && logic->CanUse(RG_FAIRY_BOW));});}}),
  });

  areaTable[RR_DEKU_TREE_OUTSIDE_BOSS_ROOM] = Region("Deku Tree Outside Boss Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,  {[]{return (logic->HasItem(RG_BRONZE_SCALE) || Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return logic->CanUse(RG_IRON_BOOTS);})) && Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return logic->CanReflectNuts();});}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(DEKU_TREE)->IsMQ()) {
  areaTable[RR_DEKU_TREE_MQ_1F] = Region("Deku Tree MQ 1F", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks,    {[]{return logic->CanKillEnemy(RE_WITHERED_DEKU_BABA);}}),
                  EventAccess(&logic->BrokeDeku1FWeb,    {[]{return logic->HasFireSource();}}),
  }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,    {[]{return true;}}),
                  //may need canAvoid logic with enemy shuffle
                  Entrance(RR_DEKU_TREE_MQ_2F,       {[]{return true;}}),
                  //Swim is not required because you can jump with enough momentum to hit land. 
                  //You even avoid fall damage if you hit the shallow water, though it's obscure knowledge so may be a trick
                  //if it is, then we need a landing room with (IsAdult || HasItem(RG_BRONZE_SCALE) || TakeDamage() || that trick) to reach basement
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT, {[]{return logic->BrokeDeku1FWeb;}}),
                  //is it possible to recoil from here to the ledge with a trick?
  });

  areaTable[RR_DEKU_TREE_MQ_2F] = Region("Deku Tree MQ 2F", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_MAP_CHEST,                 true),
                  LOCATION(RC_DEKU_TREE_MQ_GS_LOBBY,                  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_1F,              {[]{return true;}}),
                  //Will need canAvoid logic with enemy shuffle
                  Entrance(RR_DEKU_TREE_MQ_3F,              {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, {[]{return Here(RR_DEKU_TREE_MQ_2F, []{return logic->HasFireSource();});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_3F] = Region("Deku Tree MQ 3F", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks,    {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,      {[]{return logic->CanGetDekuBabaNuts();}}),
                  EventAccess(&logic->BrokeDeku1FWeb,    {[]{return true;}}),
  }, {
                  //Locations
                  //Implies CanKillEnemy(RE_GOHMA_LARVA)
                  LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,           logic->CanKillEnemy(RE_DEKU_BABA)),
                  LOCATION(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, logic->HasFireSourceWithTorch() || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW))),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_2F,              {[]{return true;}}),
                  //Assumes RR_DEKU_TREE_MQ_2F access
                  Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, {[]{return Here(RR_DEKU_TREE_MQ_3F, []{return logic->CanUse(RG_STICKS) || logic->CanUse(RG_FAIRY_BOW);});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT,        {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_EYE_TARGET_ROOM] = Region("Deku Tree MQ Eye Target Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM, {[]{return Here(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, []{return logic->CanHitEyeTargets();});}}),
                  Entrance(RR_DEKU_TREE_MQ_2F,           {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_COMPASS_ROOM] = Region("Deku Tree MQ Compass Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_COMPASS_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_EYE_TARGET_ROOM,    {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_PAST_BOULDER_VINES, {[]{return Here(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return logic->CanUse(RG_BOMBCHU_5) ||
                                                                               (logic->CanUse(RG_BOMB_BAG) && (logic->CanUse(RG_SONG_OF_TIME) || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS))) ||
                                                                               (logic->CanUse(RG_MEGATON_HAMMER) && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS)));});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_PAST_BOULDER_VINES] = Region("Deku Tree MQ Past Boulder Vines", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT)),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM, {[]{return logic->BlastOrSmash();}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT] = Region("Deku Tree MQ Basement", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}}),
  }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_BASEMENT_CHEST, logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW)),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_1F,                        {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,   {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanHitEyeTargets();});}}),
                  //includes RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM Access, other fire sources clear directly from there
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanHitEyeTargets();}) && logic->ClearedMQDekuSERoom &&
                                                                                 Here(RR_DEKU_TREE_MQ_BASEMENT, []{return logic->CanUse(RG_STICKS);});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,            {[]{return ctx->GetTrickOption(RT_DEKU_B1_SKIP) || logic->PushedDekuBasementBlock || logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM] = Region("Deku Tree MQ Southeast Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //Implies CanKillEnemy(RE_GOHMA_LARVA)
                  EventAccess(&logic->ClearedMQDekuSERoom, {[]{return logic->CanKillEnemy(RE_MAD_SCRUB);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return logic->HasFireSource();}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT,                  {[]{return logic->ClearedMQDekuSERoom;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Region("Deku Tree MQ Basement Water Room Front", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //It's possible to get this with bow if you have move while in first person and one-point skips on, noticeably harder and jankier as child, but that's a trick
                  EventAccess(&logic->MQDekuWaterRoomTorches, {[]{return logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_STICKS) && (ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield())));}}),
  }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) ||
                                                                                logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Region("Deku Tree MQ Basement Water Room Back", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks,         {[]{return logic->CanKillEnemy(RE_WITHERED_DEKU_BABA);}}),
                  EventAccess(&logic->MQDekuWaterRoomTorches, {[]{return logic->HasFireSource();}}),
  }, {
                  //Locations
                  //it blocks the chest while stunned unless you stun it from afar while it's slightly off the ground
                  LOCATION(RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, logic->CanUse(RG_SONG_OF_TIME) && logic->CanPassEnemy(RE_BIG_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,   {[]{return logic->MQDekuWaterRoomTorches && logic->CanPassEnemy(RE_BIG_SKULLTULA, logic->CanUse(RG_SONG_OF_TIME) ? ED_CLOSE : ED_SHORT_JUMPSLASH);}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return ctx->GetTrickOption(RT_DEKU_MQ_LOG) || (logic->IsChild && logic->CanShield()) ||
                                                                                 logic->CanUse(RG_LONGSHOT) || logic->HasItem(RG_BRONZE_SCALE) ||
                                                                                 (logic->CanUse(RG_IRON_BOOTS) && (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)));}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM] = Region("Deku Tree MQ Basement Southwest Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //both imply CanKillEnemy(RE_GOHMA_LARVA)
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,      {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, []{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, []{return logic->CanKillEnemy(RE_MAD_SCRUB) && logic->CanKillEnemy(RE_KEESE);});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM] = Region("Deku Tree MQ Basement Grave Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  EventAccess(&logic->DekuBabaNuts,   {[]{return logic->CanGetDekuBabaNuts();}})
  }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT))),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,           {[]{return logic->IsChild && Here(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,  {[]{return true;}}),
                  //Using a bow to get past here as adult is a bit precise on standing position but simple, doing as as child requires a side-hop with the bow out to shoot through the torch and may be trick worthy
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,       {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, []{return logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Region("Deku Tree MQ Basement Back Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,   logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT)),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_LEDGE] = Region("Deku Tree MQ Basement Ledge", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->PushedDekuBasementBlock, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_DEKU_TREE_MQ_DEKU_SCRUB, logic->CanStunDeku()),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, {[]{return logic->IsChild;}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT,            {[]{return true;}}),
                  //If strength 0 is shuffled, add hovers or block push to the stick check
                  //recoiling to skip swim is possible, but would be a trick
                  Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM,  {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, []{return logic->HasFireSource() || (/*logic->PushedDekuBasementBlock && */logic->CanUse(RG_STICKS));})
                                                                          && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
  });

    areaTable[RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM] =
            Region("Deku Tree MQ Outside Boss Room", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, {[]{return logic->HasItem(RG_BRONZE_SCALE);}}),
                     Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     {[]{return Here(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, []{return logic->CanReflectNuts();});}}),
                 });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_DEKU_TREE_BOSS_ENTRYWAY] =
        Region("Deku Tree Boss Entryway", "Deku Tree", {RA_DEKU_TREE}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, { [] { return ctx->GetDungeon(DEKU_TREE)->IsVanilla(); } }),
                 Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, { [] { return ctx->GetDungeon(DEKU_TREE)->IsMQ(); } }),
                 Entrance(RR_DEKU_TREE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_DEKU_TREE_BOSS_ROOM] =
        Region("Deku Tree Boss Room", "Deku Tree", {}, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&logic->DekuTreeClear, { [] {
                                                  return logic->DekuTreeClear || (logic->HasBossSoul(RG_GOHMA_SOUL) && 
                                                         (logic->CanJumpslashExceptHammer() && (logic->CanUse(RG_NUTS) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) ||
                                                                           logic->HookshotOrBoomerang())));
                                              }}),
             },
             {
                 // Locations
                 LOCATION(RC_QUEEN_GOHMA, logic->DekuTreeClear),
                 LOCATION(RC_DEKU_TREE_QUEEN_GOHMA_HEART, logic->DekuTreeClear),
             },
             {
                 // Exits
                 Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, { [] { return true; } }),
                 Entrance(RR_KF_OUTSIDE_DEKU_TREE, { [] { return logic->DekuTreeClear; } }, false),
             });
}
