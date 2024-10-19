#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_DodongosCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_DODONGOS_CAVERN_ENTRYWAY] = Region("Dodongos Cavern Entryway", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BEGINNING,    {[]{return ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla();}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_BEGINNING, {[]{return ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ();}}),
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL,         {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla()) {
  areaTable[RR_DODONGOS_CAVERN_BEGINNING] = Region("Dodongos Cavern Beginning", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,    {[]{return Here(RR_DODONGOS_CAVERN_BEGINNING, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOBBY] = Region("Dodongos Cavern Lobby", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return (Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls();}) || logic->HasItem(RG_GORONS_BRACELET)) && logic->CallGossipFairy();}}),
                }, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MAP_CHEST,        Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
                  LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, logic->CanStunDeku() || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,     Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BEGINNING,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOBBY_SWITCH, {[]{return logic->IsAdult;}}),
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR,  {[]{return Here(RR_DODONGOS_CAVERN_LOBBY, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return HasAccessTo(RR_DODONGOS_CAVERN_LOBBY_SWITCH);}}),
                  Entrance(RR_DODONGOS_CAVERN_FAR_BRIDGE,   {[]{return HasAccessTo(RR_DODONGOS_CAVERN_FAR_BRIDGE);}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_AREA,    {[]{return Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->HasExplosives();});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,{[]{return false;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOBBY_SWITCH] = Region("Dodongos Cavern Lobby Switch", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SE_CORRIDOR] = Region("Dodongos Cavern SE Corridor", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_GS_SCARECROW, logic->CanUse(RG_SCARECROW) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (ctx->GetTrickOption(RT_DC_SCARECROW_GS) && (logic->CanAttack()))),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,               {[]{return true;}}),
                  //Shield seems to be in logic to drop a pot on thier head as they hit you to blow up the wall
                  Entrance(RR_DODONGOS_CAVERN_SE_ROOM,             {[]{return Here(RR_DODONGOS_CAVERN_SE_CORRIDOR, []{return logic->BlastOrSmash() || logic->CanAttack() || (logic->TakeDamage() && logic->CanShield());});}}),
                  Entrance(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SE_ROOM] = Region("Dodongos Cavern SE Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Region("Dodongos Cavern Near Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOWER_LIZALFOS] = Region("Dodongos Cavern Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) ||
                    logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives();});}}),
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM,        {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) || 
                    logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives();});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_DODONGO_ROOM] = Region("Dodongos Cavern Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY_SWITCH,      {[]{return logic->HasFireSourceWithTorch();}}),
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_DODONGO_ROOM, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Region("Dodongos Cavern Near Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, logic->CanStunDeku()),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_STAIRS_LOWER] = Region("Dodongos Cavern Stairs Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_UPPER, {[]{return logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_DODONGOS_CAVERN_COMPASS_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_STAIRS_LOWER, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_STAIRS_UPPER] = Region("Dodongos Cavern Stairs Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->HookshotOrBoomerang();}) || logic->CanUse(RG_LONGSHOT)),
                  LOCATION(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  logic->IsAdult || logic->CanAttack() || (HasAccessTo(RR_DODONGOS_CAVERN_STAIRS_LOWER) && logic->CanUse(RG_LONGSHOT) && ctx->GetTrickOption(RT_DC_VINES_GS))),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_ARMOS_ROOM,   {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_COMPASS_ROOM] = Region("Dodongos Cavern Compass Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_COMPASS_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET);}}),
  });

  areaTable[RR_DODONGOS_CAVERN_ARMOS_ROOM] = Region("Dodongos Cavern Armos Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_UPPER,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Region("Dodongos Cavern Bomb Room Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_2F_SIDE_ROOM,         {[]{return Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return logic->BlastOrSmash() || (ctx->GetTrickOption(RT_DC_SCRUB_ROOM) && logic->HasItem(RG_GORONS_BRACELET));});}}),
                  Entrance(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER,      {[]{return (logic->IsAdult && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_DODONGOS_CAVERN_2F_SIDE_ROOM] = Region("Dodongos Cavern 2F Side Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  logic->CanStunDeku()),
                  LOCATION(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, logic->CanStunDeku()),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Region("Dodongos Cavern First Slingshot Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP);}}),
  });

  areaTable[RR_DODONGOS_CAVERN_UPPER_LIZALFOS] = Region("Dodongos Cavern Upper Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) || 
                    logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives();});}}),
                  Entrance(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) || 
                    logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->HasExplosives();});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Region("Dodongos Cavern Second Slingshot Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_DC_SLINGSHOT_SKIP);}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Region("Dodongos Cavern Bomb Room Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,       {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_FAR_BRIDGE,            {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_FAR_BRIDGE] = Region("Dodongos Cavern Far Bridge", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return logic->BlastOrSmash();})),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,           {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOSS_AREA] = Region("Dodongos Cavern Boss Region", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BACK_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_BOSS_AREA, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BACK_ROOM] = Region("Dodongos Cavern Back Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_GS_BACK_ROOM, logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOSS_AREA, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ()) {
  areaTable[RR_DODONGOS_CAVERN_MQ_BEGINNING] = Region("Dodongos Cavern MQ Beginning", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOBBY, {[]{return Here(RR_DODONGOS_CAVERN_MQ_BEGINNING, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LOBBY] = Region("Dodongos Cavern MQ Lobby", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return (Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->CanBreakMudWalls();}) || logic->HasItem(RG_GORONS_BRACELET)) && logic->CallGossipFairy();}}),
  }, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_MAP_CHEST,                 logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,     logic->CanStunDeku()),
                  LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,    logic->CanStunDeku()),
                  LOCATION(RC_DODONGOS_CAVERN_GOSSIP_STONE,                 Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET);})),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE,     {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,          {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->BlastOrSmash() || logic->HasItem(RG_GORONS_BRACELET);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,      {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return logic->CanBreakMudWalls();}) ||
                                                                                   Here(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, []{return logic->HasItem(RG_GORONS_BRACELET) && logic->TakeDamage();});}}), //strength 1 and bunny speed works too
                  Entrance(RR_DODONGOS_CAVERN_MQ_POES_ROOM,             {[]{return logic->IsAdult;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,          {[]{return Here(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE, []{return logic->HasExplosives() || (logic->ClearMQDCUpperLobbyRocks && logic->HasItem(RG_GORONS_BRACELET) &&
                                                                                                                                                                    ((logic->IsAdult && ctx->GetTrickOption(RT_DC_MQ_ADULT_EYES)) ||
                                                                                                                                                                    (logic->IsChild && ctx->GetTrickOption(RT_DC_MQ_CHILD_EYES))));});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE] = Region("Dodongos Cavern MQ Mouth Side Bridge", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ClearMQDCUpperLobbyRocks, {[]{return logic->BlastOrSmash() || logic->CanUse(RG_DINS_FIRE);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOBBY,                 {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER,    {[]{return logic->ClearMQDCUpperLobbyRocks;}}),
                  //Bunny hood jump + jumpslash can also make it directly from the raising platform
                  Entrance(RR_DODONGOS_CAVERN_MQ_POES_ROOM,             {[]{return logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DC_MQ_CHILD_BOMBS) && logic->CanJumpslashExceptHammer() && logic->TakeDamage());}}), //RANDOTODO is this possible with equip swapped hammer?
                  //it is possible to use bunny hood speed, hovers and a jumpslash to go between here and the other bridge (included with TORCH_ROOM_LOWER), but this would be a trick
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER] = Region("Dodongos Cavern MQ Stairs Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&logic->CanClimbDCStairs, {[]{return logic->HasExplosives || logic->CanUse(RG_DINS_FIRE) || (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW));}}),
  }, {}, {
                  //Exits
                  //This is possible with sticks and shield, igniting a first flower by "touch" then very quickly crouch stabbing in a way that cuts the corner to light the 3rd bomb on the other side, but that's a trick
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER,         {[]{return Here(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, []{return logic->HasExplosives() || logic->CanUse(RG_DINS_FIRE) ||
                                                                                       (ctx->GetTrickOption(RT_DC_STAIRCASE) && logic->CanUse(RG_FAIRY_BOW));});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL, {[]{return Here(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, []{return logic->CanBreakMudWalls();});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL] = Region("Dodongos Cavern MQ Stairs Past Mud Wall", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks, {[]{return logic->CanGetDekuBabaSticks();}}),
                  //EventAccess(&logic->CanClimbDCStairs, {[]{return logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_STICKS));}}),
  }, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, logic->CanUse(RG_SONG_OF_TIME) && logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, {[]{return logic->HasItem(RG_GORONS_BRACELET) && (logic->CanUse(RG_STICKS));}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER] = Region("Dodongos Cavern MQ Stairs Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,       logic->CanStunDeku()),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,               {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS] = Region("Dodongos Cavern MQ Past Big Skulltulas", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_UPPER, {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, {[]{return logic->TakeDamage();}}),
                  //If some case comes up where you can directly (void?)warp here without going through Dodongo room or climbing up from below, 
                  //the commented out logic is to handle going down and reclimbing to get silver rupees. A new eventVar will need decalring to handle this.
                  /*(logic->CanPassEnemy(RE_BIG_SKULLTULA) || CanUse(RG_HOVER_BOOTS)) && logic->CanClimbDCStairs;*/
                  Entrance(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, {[]{return true;}}),//if we add BONKO or other crate logic, logic for silver rupees goes here
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM] = Region("Dodongos Cavern MQ Dodongo Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST, logic->CanKillEnemy(RE_DODONGO) || logic->HasItem(RG_GORONS_BRACELET)),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER,         {[]{return Here(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, []{return logic->CanKillEnemy(RE_DODONGO) || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER] = Region("Dodongos Cavern MQ Torch Puzzle Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ClearMQDCUpperLobbyRocks, {[]{return (((logic->IsAdult /*or bunny hood jump*/) && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_STICKS);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOBBY,              {[]{return logic->TakeDamage();}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM,       {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM,        {[]{return logic->HasFireSourceWithTorch();}}),//torch checks here need strength 0 with sticks when that is implemented
                  Entrance(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM,     {[]{return Here(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, []{return logic->HasFireSourceWithTorch();});}}), //Includes an implied CanPass(RE_BIG_SKULLTULA)
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, {[]{return ((logic->IsAdult /*or bunny hood jump*/) && ctx->GetTrickOption(RT_DC_JUMP)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     {[]{return logic->CanUse(RG_STICKS) && logic->HasItem(RG_GORONS_BRACELET);}}), //Implies access to RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM from here
 });

  areaTable[RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM] = Region("Dodongos Cavern MQ Torch Puzzle Lower", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     {[]{return (logic->HasFireSource() && logic->HasItem(RG_GORONS_BRACELET)) || logic->CanBreakMudWalls();}}), //Requires stregnth 0, If you can somehow warp into this room, add logic->CanPassEnemy(RE_BIG_SKULLTULA)
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM] = Region("Dodongos Cavern MQ Larvae Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST, true), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,    true), //implied logic->CanKillEnemy(RE_GOLD_SKULTULLA) based on entry reqs. Add crate logic when BONKO is added
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, {[]{return true;}}), //implied logic->CanKillEnemy(RE_GOHMA_LARVA) based on entry reqs with a trick to kill with nuts
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS] = Region("Dodongos Cavern MQ Before Upper Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,   logic->BlastOrSmash()), //Implied CanGetEnemyDrop(RE_GOLD_SKULLTULA)
  }, {
                  //Exits
                  //Falling down gets you stuck with nothing there, not a useful exit for logic
                  Entrance(RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM] = Region("Dodongos Cavern MQ Before Upper Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,     {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER, {[]{return logic->IsAdult || (Here(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, []{return logic->BlastOrSmash() || (logic->CanAttack() && logic->HasItem(RG_GORONS_BRACELET));}));}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_UPPER] = Region("Dodongos Cavern MQ Torch Puzzle Upper", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ClearMQDCUpperLobbyRocks, {[]{return logic->CanDetonateUprightBombFlower() || logic->CanUse(RG_MEGATON_HAMMER);}}),
  }, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_MOUTH_SIDE_BRIDGE,  {[]{return logic->ClearMQDCUpperLobbyRocks;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM,     {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Region("Dodongos Cavern MQ Lower Right Side", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, (logic->CanBreakMudWalls() || logic->HasItem(RG_GORONS_BRACELET)) && logic->CanStunDeku()),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);}) && logic->CanHitEyeTargets();}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS] = Region("Dodongos Cavern MQ Lower Lizalfos", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //When you add wonder item logic for behind the lavafall, use Here(RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS) to account for jumping down instead of an exit
                  //because the doors are sealed when entering from the top and you can't spawn the lower lizalfos
  }, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_POES_ROOM,        {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, []{return logic->CanKillEnemy(RE_LIZALFOS);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_POES_ROOM] = Region("Dodongos Cavern MQ Poes Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, true), //If you can get to the locked part of POES_ROOM without a way to open it or passing the chest, this will need it's own room 
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);}) && //could be a seperate room if it gets busy
                                                                  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT, true))), //Implies you can avoid/kill the enemies with what you use on the skull, if this assumption is broken, add
                                                                //&& (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);}) || (logic->CanAvoidEnemy(RE_FIRE_KEESE) && logic->CanAvoidEnemy(RE_MAD_SCRUB)))
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  {[]{return Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM,    {[]{return Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanDetonateBombFlowers() || logic->HasItem(RG_GORONS_BRACELET);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM] = Region("Dodongos Cavern Mad Scrub Room", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT, true))), //Implies you can avoid/kill the enemies with what you use on the skull, if this assumption is broken, add
                                                                //&& (Here(RR_DODONGOS_CAVERN_MQ_POES_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);}) || (logic->CanAvoidEnemy(RE_FIRE_KEESE) && logic->CanAvoidEnemy(RE_MAD_SCRUB)))
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_POES_ROOM,  {[]{return Here(RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM, []{return logic->CanKillEnemy(RE_FIRE_KEESE) && logic->CanKillEnemy(RE_MAD_SCRUB);});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH] = Region("Dodongos Cavern MQ Behind Mouth", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOBBY,               {[]{return true;}}),
                  //using pots to get past the fire is in default logic. if stregnth 0 gets added, this will need to be:
                  //stregnth 0 || explosives, or projectiles if str0 isn't needed to pull graves (it's a narrow shot though, may be trick worthy)
                  Entrance(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE,   {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE] = Region("Dodongos Cavern MQ Back Behind Fire", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, true), //pulling the grave isn't required, as you can open the chest through it
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,  {[]{return logic->CanAttack();}}),
                  //There's a trick N64 rolls into the child eyes trick for using armos blow up the bomb flowers when dieing, which would be killing an armos
                  Entrance(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE,   {[]{return Here(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, []{return logic->CanDetonateBombFlowers();}) ||
                                                                           Here(RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, []{return logic->CanAttack();});}}), 
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE] = Region("Dodongos Cavern MQ BossArea", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA) || logic->HasItem(RG_GORONS_BRACELET) || //even if you somehow warp to BACK_BEHIND_FIRE, if you can kill the skull at range, you can get to BEHIND_MOUTH
                                                               Here(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, []{return (logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_RANG_OR_HOOKSHOT)) ||
                                                                                                                  (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) /* || bunny jumps*/);})),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,       {[]{return true;}}), //if strength 0 prevents grave pulls, add it here
  });

  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_DODONGOS_CAVERN_BOSS_ENTRYWAY] =
        Region("Dodongos Cavern Boss Entryway", "Dodongos Cavern", {RA_DODONGOS_CAVERN}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_DODONGOS_CAVERN_BOSS_AREA, { [] { return ctx->GetDungeon(DODONGOS_CAVERN)->IsVanilla(); } }),
                 Entrance(RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, { [] { return ctx->GetDungeon(DODONGOS_CAVERN)->IsMQ(); } }),
                 Entrance(RR_DODONGOS_CAVERN_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_DODONGOS_CAVERN_BOSS_ROOM] =
        Region("Dodongos Cavern Boss Room", "Dodongos Cavern", {}, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&logic->DodongosCavernClear,
                             { [] {
                                  return logic->DodongosCavernClear || (logic->HasBossSoul(RG_KING_DODONGO_SOUL) &&
                                         (Here(RR_DODONGOS_CAVERN_BOSS_ROOM,
                                               [] { return logic->HasExplosives() || (logic->CanUse(RG_MEGATON_HAMMER) && ctx->GetTrickOption(RT_DC_HAMMER_FLOOR)); }) &&
                                          (logic->CanUse(RG_BOMB_BAG) || logic->HasItem(RG_GORONS_BRACELET)) && logic->CanJumpslashExceptHammer())); /*todo add chu kill to tricks*/
                              }}),
             },
             {
                 // Locations
                 LOCATION(RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, true),
                 LOCATION(RC_DODONGOS_CAVERN_KING_DODONGO_HEART, logic->DodongosCavernClear),
                 LOCATION(RC_KING_DODONGO, logic->DodongosCavernClear),
             },
             {
                 // Exits
                 Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, { [] { return true; } }),
                 Entrance(RR_DEATH_MOUNTAIN_TRAIL, { [] { return logic->DodongosCavernClear; } }, false),
             });
}
