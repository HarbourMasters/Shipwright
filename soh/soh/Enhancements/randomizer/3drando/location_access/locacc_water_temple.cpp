#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_WaterTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_WATER_TEMPLE_ENTRYWAY] = Region("Water Temple Entryway", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,         {[]{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL, {[]{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsMQ();}}),
                  Entrance(RR_LAKE_HYLIA,                 {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(WATER_TEMPLE)->IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[RR_WATER_TEMPLE_LOBBY] = Region("Water Temple Lobby", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER,            {[]{return logic->CanWaterTempleLowFromHigh || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && (logic->CanUse(RG_IRON_BOOTS) || (logic->CanUse(RG_LONGSHOT) && ctx->GetTrickOption(RT_WATER_LONGSHOT_TORCH))));}}),
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,           {[]{return logic->CanWaterTempleLowFromHigh || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_SOUTH_LOWER,           {[]{return logic->CanWaterTempleLowFromHigh && logic->HasExplosives() && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)) && (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER,            {[]{return logic->CanWaterTempleLowFromHigh && logic->HasItem(RG_GORONS_BRACELET) && (logic->IsChild || logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)) && (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return logic->CanWaterTempleLowFromHigh && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (logic->CanWaterTempleLowFromHigh || logic->CanWaterTempleMiddle) && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_WATER_TEMPLE_EAST_MIDDLE,           {[]{return (logic->CanWaterTempleLowFromHigh || logic->CanWaterTempleMiddle || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16)) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_WEST_MIDDLE,           {[]{return logic->CanWaterTempleMiddle;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER,            {[]{return logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DAMAGE_BOOST) && logic->CanUse(RG_BOMB_BAG) && logic->TakeDamage()));}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (logic->CanWaterTempleLowFromHigh || logic->CanWaterTempleMiddle) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_WATER_CENTRAL_BOW) && (logic->IsAdult || logic->CanWaterTempleMiddle)));}}),
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return logic->CanWaterTempleHigh && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return logic->CanWaterTempleHigh && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_LOWER] = Region("Water Temple East Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CanWaterTempleLowFromHigh, {[]{return logic->CanWaterTempleLowFromHigh || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,        {[]{return logic->CanWaterTempleLowFromHigh || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_MAP_ROOM,     {[]{return logic->CanWaterTempleHigh;}}),
                  Entrance(RR_WATER_TEMPLE_CRACKED_WALL, {[]{return logic->CanWaterTempleMiddle || (logic->CanWaterTempleHigh && logic->CanWaterTempleLowFromHigh && ((logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_WATER_CRACKED_WALL_HOVERS)) || ctx->GetTrickOption(RT_WATER_CRACKED_WALL)));}}),
                  Entrance(RR_WATER_TEMPLE_TORCH_ROOM,   {[]{return logic->CanWaterTempleLowFromHigh && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}}),
  });

  areaTable[RR_WATER_TEMPLE_MAP_ROOM] = Region("Water Temple Map Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MAP_CHEST, (logic->CanUse(RG_MAGIC_SINGLE) && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT)),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (logic->CanUse(RG_MAGIC_SINGLE) && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_CRACKED_WALL] = Region("Water Temple Cracked Wall", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_CRACKED_WALL_CHEST, logic->HasExplosives()),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_TORCH_ROOM] = Region("Water Temple Torch Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_TORCHES_CHEST, (logic->CanUse(RG_MAGIC_SINGLE) && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT)),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (logic->CanUse(RG_MAGIC_SINGLE) && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_NORTH_LOWER] = Region("Water Temple North Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,          {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS))) && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_LOWER] = Region("Water Temple Boulders Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, logic->CanUse(RG_LONGSHOT) || Here(RR_WATER_TEMPLE_BOULDERS_UPPER, []{return (logic->IsAdult && logic->HookshotOrBoomerang()) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT));})),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,    {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP))) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_IRON_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_ROOM] = Region("Water Temple Block Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (logic->HasItem(RG_GORONS_BRACELET) && logic->HasExplosives()) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return (logic->HasItem(RG_GORONS_BRACELET) && logic->HasExplosives()) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_JETS_ROOM] = Region("Water Temple Jets Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_UPPER] = Region("Water Temple Boulders Upper", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return logic->IsAdult;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_KEY_ROOM,  {[]{return (logic->CanUse(RG_IRON_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_WATER_BK_JUMP_DIVE))) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOSS_KEY_ROOM] = Region("Water Temple Boss Key Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_BOSS_KEY_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (logic->CanUse(RG_IRON_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_WATER_BK_JUMP_DIVE)) || logic->IsChild || logic->HasItem(RG_SILVER_SCALE)) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_SOUTH_LOWER] = Region("Water Temple South Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_GS_BEHIND_GATE, logic->CanJumpslash() && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_LOWER] = Region("Water Temple West Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,       {[]{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_GORONS_BRACELET);}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return logic->CanJumpslashExceptHammer() || logic->CanUseProjectile();}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_ROOM] = Region("Water Temple Dragon Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_DRAGON_CHEST, (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS)) || (((logic->IsAdult && ctx->GetTrickOption(RT_WATER_ADULT_DRAGON) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOMBCHU_5))) || (logic->IsChild && ctx->GetTrickOption(RT_WATER_CHILD_DRAGON) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_BOMBCHU_5)))) && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))) ||
                                                                       Here(RR_WATER_TEMPLE_RIVER, []{return logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && ((ctx->GetTrickOption(RT_WATER_ADULT_DRAGON) && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))) || ctx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE));})),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER] = Region("Water Temple Central Pillar Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                   {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,    {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return logic->CanWaterTempleMiddle && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Region("Water Temple Central Pillar Upper", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CanWaterTempleMiddle, {[]{return logic->CanWaterTempleMiddle || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, logic->CanUse(RG_LONGSHOT) || (((ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->SmallKeys(RR_WATER_TEMPLE, 5))) || (ctx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) && logic->CanUse(RG_IRON_BOOTS) && ((logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FAIRY_BOW)) || (logic->CanUse(RG_DINS_FIRE))))) && logic->CanWaterTempleHigh && logic->HookshotOrBoomerang())),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT] = Region("Water Temple Central Pillar Basement", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_MIDDLE] = Region("Water Temple East Middle", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_COMPASS_CHEST, logic->CanUseProjectile()),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_MIDDLE] = Region("Water Temple West Middle", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,      {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER, {[]{return logic->CanUseProjectile();}}),
  });

  areaTable[RR_WATER_TEMPLE_HIGH_WATER] = Region("Water Temple High Water", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CanWaterTempleHigh, {[]{return logic->CanWaterTempleHigh || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_CORRIDOR] = Region("Water Temple Block Corridor", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, logic->HasItem(RG_GORONS_BRACELET) && (logic->CanWaterTempleLowFromHigh || logic->CanWaterTempleMiddle)),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM] = Region("Water Temple Falling Platform Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_RANG_FALLING_PLATFORM_GS) && logic->IsChild && logic->CanUse(RG_BOOMERANG)) || (ctx->GetTrickOption(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT))),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,               {[]{return logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM] = Region("Water Temple Dragon Pillars Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return logic->CanUseProjectile();}}),
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM,        {[]{return logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_DARK_LINK_ROOM] = Region("Water Temple Dark Link Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
                  Entrance(RR_WATER_TEMPLE_LONGSHOT_ROOM,       {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
  });

  areaTable[RR_WATER_TEMPLE_LONGSHOT_ROOM] = Region("Water Temple Longshot Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_LONGSHOT_CHEST, true),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_RIVER,          {[]{return logic->IsChild || logic->CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_WATER_TEMPLE_RIVER] = Region("Water Temple River", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_RIVER_CHEST, (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))),
                  LOCATION(RC_WATER_TEMPLE_GS_RIVER,    (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)) || (ctx->GetTrickOption(RT_WATER_RIVER_GS) && logic->CanUse(RG_LONGSHOT))),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_WATER_TEMPLE_PRE_BOSS_ROOM] = Region("Water Temple Pre Boss Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->HasItem(RG_WATER_TEMPLE_BOSS_KEY);}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(WATER_TEMPLE)->IsMQ()) {
  areaTable[RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE] = Region("Water Temple MQ 3F South Ledge", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,      {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,       {[]{return true;}}),
                  //If we are not on WL_HIGH, we reach RR_WATER_TEMPLE_MQ_3F_MAIN with hookshot via 2F, otherwise we can reach the platform
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL, {[]{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL, {[]{return logic->MQWaterLevel(WL_LOW_OR_MID);}}),
  });

//This region covers simply existing in the area around the central pillar without being on a specific platform, either swimming or walking on the lakebed
//Entry should only include being in the correct area, taking any possible fall damage, and floating up to the surface of WL_HIGH if coming from below
//This area then leads to others based on level and worst-case water timers for follow-up exits from the water's surface
//remember that any solution that works for any level doesn't need to be given a level, even if that solution is overkill for a lower level
  areaTable[RR_WATER_TEMPLE_MQ_MAIN] = Region("Water Temple MQ Main", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE,         {[]{return logic->HasItem(RG_BRONZE_SCALE) && logic->MQWaterLevel(WL_HIGH);}}),
                  //Jumping across is possible but a trick due to the janky ledge
                  Entrance(RR_WATER_TEMPLE_MQ_EAST_TOWER,             {[]{return (logic->WaterTimer() >= 24 && logic->CanUse(RG_IRON_BOOTS)) ||
                                                                                 (logic->MQWaterLevel(WL_MID) && logic->HasItem(RG_GOLDEN_SCALE) && logic->WaterTimer() >= 16) ||
                                                                                 logic->MQWaterLevel(WL_LOW);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL,             {[]{return logic->MQWaterLevel(WL_HIGH) && logic->HasItem(RG_BRONZE_SCALE);}}),
                  //First water timer uses the hook to go from the top of center to storage room/central pillar as coming from the bottom
                  //Second water timer is simply diving down and entering the door as fast as possible from the surface
                  Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL,             {[]{return ((logic->MQWaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && (logic->MQWaterLevel(WL_MID) || logic->WaterTimer() >= 16))) && logic->CanUse(RG_LONGSHOT)) ||
                                                                                 ((logic->MQWaterLevel(WL_MID) || (logic->MQWaterLevel(WL_HIGH_OR_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8)) && logic->HasItem(RG_BRONZE_SCALE));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_1F,      {[]{return logic->MQWaterLevel(WL_LOW);}}),
                  //A special entry as we can't set it to high after entering at a lower height
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH,    {[]{return logic->MQWaterLevel(WL_HIGH) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_2F_SOUTH,               {[]{return (logic->MQWaterLevel(WL_MID) || (logic->MQWaterLevel(WL_HIGH_OR_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16)) && logic->HasItem(RG_BRONZE_SCALE);}}),        
                  Entrance(RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH,         {[]{return logic->MQWaterB1Switch && (logic->MQWaterLevel(WL_LOW) || ((logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24) && logic->HasItem(RG_BRONZE_SCALE)));}}),  
                  Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM,    {[]{return logic->MQWaterB1Switch &&
                                                                                 ((logic->MQWaterLevel(WL_LOW) && logic->HasItem(RG_SILVER_SCALE)) || 
                                                                                  (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 &&  (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT))));}}),  
                  //Adult needs to jump in instead of dive for swim access, but you just hold forward. RT_WATER_BK_REGION Isn't relevant unless the Dark Link loop can be done without longshot with other tricks
                  Entrance(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM, {[]{return logic->MQWaterB1Switch && 
                                                                                 ((logic->MQWaterLevel(WL_LOW) && logic->HasItem(RG_BRONZE_SCALE)) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT))) &&
                                                                                 (logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS)));}}),    
  });

//This region specifically covers the topmost platform around central pillar
  areaTable[RR_WATER_TEMPLE_MQ_3F_CENTRAL] = Region("Water Temple MQ 3F Central", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE,      {[]{return logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_2F_CENTRAL,          {[]{return (logic->MQWaterLevel(WL_LOW_OR_MID) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16)) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH, {[]{return logic->MQWaterLevel(WL_HIGH) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE,      {[]{return logic->MQWaterLevel(WL_HIGH) && logic->CanUse(RG_LONGSHOT);}}),
                  //Jumping across is possible but a trick due to the janky ledge
                  Entrance(RR_WATER_TEMPLE_MQ_HIGH_EMBLEM,         {[]{return logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}}),
                  //room access is (logic->IsAdult || (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)))
                  Entrance(RR_WATER_TEMPLE_MQ_WATERFALL,           {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 1) && logic->MQWaterLevel(WL_HIGH) && logic->CanUse(RG_LONGSHOT);}}),
                  //this swimless jump with irons may be a trick as you have to put irons on quite late.
                  Entrance(RR_WATER_TEMPLE_MQ_2F_SOUTH,            {[]{return (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16) || logic->MQWaterLevel(WL_LOW_OR_MID);}}),
  });

//This region specifically covers walking on the lower platform around central pillar. This is underwater when WL_HIGH 
//RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH should be accessed directly to use the central pillar door while at WL_HIGH
  areaTable[RR_WATER_TEMPLE_MQ_2F_CENTRAL] = Region("Water Temple MQ 2F Central", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                  {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL,            {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F,     {[]{return logic->MQWaterLevel(WL_LOW_OR_MID);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STORAGE_ROOM,          {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F, {[]{return logic->MQWaterLevel(WL_LOW_OR_MID) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_2F_SOUTH,              {[]{return logic->MQWaterLevel(WL_LOW_OR_MID) && logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_HIGH_EMBLEM] = Region("Water Temple MQ High Emblem", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ReachedWaterHighEmblem, {[]{return true;}}),
                  EventAccess(&logic->CanWaterTempleHigh,     {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,       {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE] = Region("Water Temple MQ 3F North Ledge", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //what we need if WL_LOW, we can't guarantee repeated access otherwise.
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,       {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->TakeDamage();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL, {[]{return logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR,  {[]{return logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_ICE_ARROWS) || logic->CanUse(RG_NAYRUS_LOVE);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BOSS_DOOR] = Region("Water Temple MQ Main", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE, {[]{return logic->CanUse(RG_ICE_ARROWS) || logic->TakeDamage();}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,     {[]{return logic->HasItem(RG_WATER_TEMPLE_BOSS_KEY);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_EAST_TOWER] = Region("Water Temple MQ East Tower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //if we can't reach these, we can't move the water at all, so no need to specify level or account for WL_LOW access here
                  //review is some way to play ocarina underwater exists
                  EventAccess(&logic->CouldWaterTempleLow,       {[]{return true;}}),
                  EventAccess(&logic->CanWaterTempleLowFromHigh, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  //Reserved for glitches/tricks that could do this
                  //EventAccess(&logic->CanWaterTempleLowFromMid,  {[]{return false;}}),
  }, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_MAP_CHEST,      logic->MQWaterLevel(WL_HIGH) && logic->HasFireSource() && logic->CanUse(RG_HOOKSHOT)),
                  //easy to get at WL_HIGH with the hook-the-underwater-chest glitch
                  LOCATION(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, logic->MQWaterLevel(WL_MID) && logic->CanUse(RG_HOOKSHOT)),
  }, {
                  Entrance(RR_WATER_TEMPLE_MQ_EAST_TOWER_1F_ROOM, {[]{return logic->MQWaterLevel(WL_LOW) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_STICKS));}}),
  });

  //Raising the targets by clearing this room achieves nothing logically because it requires WL_LOW to do and hookshot to use, which implies access to WL_MID and WL_HIGH already
  areaTable[RR_WATER_TEMPLE_MQ_EAST_TOWER_1F_ROOM] = Region("Water Temple MQ East Tower 1F Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_COMPASS_CHEST,  logic->CanKillEnemy(RE_LIZALFOS) && logic->CanKillEnemy(RE_SPIKE)),
  }, {
                  Entrance(RR_WATER_TEMPLE_MQ_EAST_TOWER, {[]{return true;}}),
  });

//This area assumes we entered through the lower door, so water is low and cannot be changed without leaving.
  areaTable[RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_1F] = Region("Water Temple MQ Central Pillar 1F", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //This is harder than the other possibilities as you have to move between shots on top of the extra range, but there's basically no universe this should matter.
                  EventAccess(&logic->MQWaterB1Switch, {[]{return ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH, {[]{return logic->MQWaterLevel(WL_HIGH) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->HasItem(RG_BRONZE_SCALE);}}),
                  //I don't know if this FW trick can ever matter but maybe it's needed to get child to CENTRAL_2F or something
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F,   {[]{return logic->CanUse(RG_HOOKSHOT) || 
                                                                              (logic->MQWaterLevel(WL_MID) && ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && logic->HasItem(RG_BRONZE_SCALE));}}),
                  //if the gate is open, you sink straight in, so you can't climb up this way in logic without swimming
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1,   {[]{return logic->MQWaterOpenedPillarB1 && logic->MQWaterLevel(WL_HIGH_OR_MID) &&
                                                                              ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) &&
                                                                              logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}}),
  });

//If we enter here in WL_HIGH, go to RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH instead, Assumes WL_MID_OR_LOW
  areaTable[RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F] = Region("Water Temple MQ Central Pillar 2F", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CouldWaterTempleMiddle, {[]{return true;}}),
                  EventAccess(&logic->CanWaterTempleMiddle,   {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  //It's possible to do this even on low water, but more awkward. I'm not sure if it's even possible for it to be relevant though.
                  EventAccess(&logic->MQWaterOpenedPillarB1,  {[]{return ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS);}}),
                  //this could theoretically matter once OI and equip swap is in logic, as one age may be able to get here dry and not wet, and the other may not be able to OI, but as you can OI with hookshot it probably never happens
                  //EventAccess(&logic->MQWaterPillarSoTBlock,  {[]{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SONG_OF_TIME);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH, {[]{return logic->MQWaterLevel(WL_HIGH) && logic->CanUse(RG_FARORES_WIND) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1,   {[]{return logic->MQWaterOpenedPillarB1 && logic->MQWaterLevel(WL_MID) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH] = Region("Water Temple MQ Central Pillar High", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQWaterOpenedPillarB1, {[]{return ((logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_DINS_FIRE)) || (ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS))) &&
                                                                        (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslash()));}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1, {[]{return logic->MQWaterB1Switch && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_ZORA_TUNIC);}}),
  });

//Assuming tunic and irons was checked on entry
  areaTable[RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1] = Region("Water Temple MQ Central Pillar B1", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //Can't know water level, so we'll just assume any possibility and skip to MAIN
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                    {[]{return logic->MQWaterOpenedPillarB1 && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_BRONZE_SCALE);}}),
                  //Child needs to release irons for height to push down the larger "peg", however they can push the lower one down by climbing and then hit the switch through the larger peg, but it's a trick
                  Entrance(RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1_FINAL, {[]{return ((logic->IsAdult && logic->CanUse(RG_LONGSHOT)) || (logic->CanUse(RG_HOOKSHOT) && logic->HasItem(RG_BRONZE_SCALE)));}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1_FINAL] = Region("Water Temple MQ Central Pillar B1 Final", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, logic->CanUse(RG_HOOKSHOT)),
  }, {});

//Region exists to add crate/pot/box locations
  areaTable[RR_WATER_TEMPLE_MQ_STORAGE_ROOM] = Region("Water Temple MQ Storage Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN, {[]{return logic->MQWaterLevel(WL_LOW_OR_MID) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F] = Region("Water Temple MQ Behind Blue Switch 2F", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                  {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_3F, {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_3F] = Region("Water Temple MQ Behind Blue Switch 2F", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_HIGH_EMBLEM,           {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_2F_SOUTH] = Region("Water Temple MQ Lowered Water Levels", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_2F_SOUTH_CAGE, {[]{return logic->MQWaterLevel(WL_LOW_OR_MID) && logic->CanUse(RG_DINS_FIRE);}}),
                  //this technically exists, but only complicates things, uncomment if some edge case/glitch can use RR_WATER_TEMPLE_MQ_2F_SOUTH to reach RR_WATER_TEMPLE_MQ_3F_CENTRAL, or if a void warp goes here
                  /*Entrance(RR_WATER_TEMPLE_MQ_3F_EAST_LEDGE, {[]{return (logic->CanUse(RG_HOOKSHOT) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS))) ||
                                                                        (logic->MQWaterLevel(WL_LOW_OR_MID) && logic->CanUse(RG_HOOKSHOT)) ||
                                                                        logic->MQWaterLevel(WL_HIGH) && (logic->HasItem(RG_BRONZE_SCALE));}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_3F_EAST_LEDGE] = Region("Water Temple MQ 3F East Ledge", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                  {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL,            {[]{return logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}),*/
  });
                                                          
  areaTable[RR_WATER_TEMPLE_MQ_2F_SOUTH_CAGE] = Region("Water Temple MQ Lowered Water Levels", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,  logic->CanKillEnemy(RE_GOLD_SKULLTULA)),
  }, {});

//This room exists to hold the wonderitems that drop from the emblems here. Specifically this assumes you are standing on the final ledge
  areaTable[RR_WATER_TEMPLE_MQ_WATERFALL] = Region("Water Temple Waterfall", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_3F_CENTRAL,        {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 1) && logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT,       {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS,  {[]{return (logic->MQWaterStalfosPit && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, {[]{return logic->MQWaterStalfosPit && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT] = Region("Water Temple MQ Stalfos Pit", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQWaterStalfosPit, {[]{return ((logic->IsAdult && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 3, false, true)) || 
                                                                    (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT) && logic->CanKillEnemy(RE_STALFOS, ED_BOMB_THROW, true, 3, false, true)));}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_WATERFALL,         {[]{return logic->MQWaterStalfosPit && logic->CanUse(RG_HOOKSHOT) && (logic->IsAdult || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS,  {[]{return (logic->IsAdult && logic->CanUse(RG_HOOKSHOT)) || 
                                                                            (logic->CanUse(RG_HOOKSHOT) && (logic->IsAdult || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) && (logic->CanUse(RG_HOVER_BOOTS) || logic->MQWaterStalfosPit));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, {[]{return logic->MQWaterStalfosPit && (logic->IsAdult || logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) && logic->CanUse(RG_HOOKSHOT);}}),
  });

  //also includes the suns fairy in the middle
  areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS] = Region("Water Temple MQ Stalfos Pit Pots", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_WATERFALL,         {[]{return logic->MQWaterStalfosPit && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT,       {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, {[]{return logic->MQWaterStalfosPit && logic->CanUse(RG_HOOKSHOT);}}),
  });

//specifically the area past the spikes
  areaTable[RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER] = Region("Water Temple MQ Stalfos Pit Upper", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT,      {[]{return logic->IsAdult || logic->TakeDamage();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS, {[]{return logic->IsAdult || logic->TakeDamage();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_AFTER_DARK_LINK,  {[]{return logic->CanKillEnemy(RE_DARK_LINK);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_AFTER_DARK_LINK] = Region("Water Temple MQ After Dark Link", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER,  {[]{return logic->CanKillEnemy(RE_DARK_LINK);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_RIVER_SKULL,        {[]{return logic->CanUse(RG_HOOKSHOT) && (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8) || logic->CanUse(RG_LONGSHOT));}}),
  });

//if we can use hookshot, we are standing on the targets, otherwise assume we're in the water
  areaTable[RR_WATER_TEMPLE_MQ_RIVER_SKULL] = Region("Water Temple MQ River Skull", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_RIVER, logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS, {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_RIVER_POTS] = Region("Water Temple MQ River Pots", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_RIVER_SKULL,        {[]{return logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT));}}),
                  //You don't need to swim for this if you put irons on in midair and hold forward while aiming for the tunnel with a tight angle, but if you miss you have to void unless you have a hook. It's only relevant with glitches anyway
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL, {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE, {[]{return logic->HasItem(RG_SILVER_SCALE) || (logic->IsAdult && logic->HasItem(RG_BRONZE_SCALE) && ctx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanJumpslash());}}),
  });

//This region assumes Iron boots to access
  areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL] = Region("Water Temple MQ Dragon Room Tunnel", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS,         {[]{return logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE, {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   {[]{return logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE] = Region("Water Temple MQ Dragon Room Alcove", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQWaterDragonTorches, {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL, {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,   {[]{return logic->HasItem(RG_SILVER_SCALE);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR] = Region("Water Temple MQ Dragon Room Door", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_RIVER_POTS,            {[]{return logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL,    {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16 && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE,    {[]{return logic->HasItem(RG_SILVER_SCALE);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH,  {[]{return logic->MQWaterDragonTorches;}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH] = Region("Water Temple MQ Boss Key Room Switch", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,    {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT,   {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST, {[]{return logic->CanHitSwitch() && Here(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH, []{return logic->CanUse(RG_DINS_FIRE);});}}),
  });

//this exists for the crates in preparation for clips through the grate
  areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT] = Region("Water Temple MQ Boss Key Room Pit", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH,  {[]{return logic->CanHitSwitch(ED_BOOMERANG);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST] = Region("Water Temple MQ Boss Key Room Chest", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH, {[]{return logic->CanHitSwitch(ED_BOMB_THROW) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT,    {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH,       {[]{return logic->HasItem(RG_SILVER_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && (logic->HasItem(RG_BRONZE_SCALE) || (logic->WaterTimer() >= 24 && logic->CanUse(RG_LONGSHOT))));}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH] = Region("Water Temple MQ B1 Gate Switch", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //If the water is low, the switch is underwater and needs irons to press, otherwise, the water is too low to climb up and you need irons to hookshot a target
                  //If a glitch clips through the gate on low, have it logically press the switch and let entrance logic enter 
                  EventAccess(&logic->MQWaterB1Switch, {[]{return logic->CanUse(RG_IRON_BOOTS);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                {[]{return logic->MQWaterB1Switch && (logic->MQWaterLevel(WL_LOW) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST, {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->HasItem(RG_BRONZE_SCALE) && (logic->MQWaterLevel(WL_LOW) || logic->WaterTimer() >= 24);}})
  });

  areaTable[RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM] = Region("Water Temple MQ Triangle Torch Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                {[]{return logic->MQWaterB1Switch && 
                                                                              ((logic->MQWaterLevel(WL_LOW) && logic->HasItem(RG_GOLDEN_SCALE)) || 
                                                                              (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40 && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_LONGSHOT))));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE, {[]{return logic->CanUse(RG_FIRE_ARROWS) && 
                                                                              ((logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)) || (logic->CanUse(RG_LONGSHOT) && Here(RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE, []{return logic->ScarecrowsSong();})));}})
  });

  areaTable[RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE] = Region("Water Temple MQ Triangle Torch Cage", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
  }, {});

  areaTable[RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM] = Region("Water Temple MQ Crates Whirlpools Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //we can backflip over the spikes, but land in water.
                  Entrance(RR_WATER_TEMPLE_MQ_MAIN,                   {[]{return logic->MQWaterB1Switch && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24 && (logic->CanUse(RG_LONGSHOT) || logic->HasItem(RG_BRONZE_SCALE));}}),
                  //Child can use the crate to get the height to make it with hovers, but it's annoyingly tight so would be a trick
                  Entrance(RR_WATER_TEMPLE_MQ_SINGLE_STALFOS_ROOM,    {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 &&
                                                                              //We're putting the requirement to get out of the water here as the scarecrow method in includes hook which satisfies it
                                                                              ((logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP)) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BRONZE_SCALE))) ||
                                                                               (Here(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM, []{return logic->ScarecrowsSong();}) && logic->CanUse(RG_HOOKSHOT)));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_4_TORCH_ROOM,           {[]{return logic->IsAdult && 
                                                                              (logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP) ||
                                                                               (Here(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM, []{return logic->ScarecrowsSong();}) && logic->CanUse(RG_HOOKSHOT)));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE, {[]{return ctx->GetTrickOption(RT_WATER_MQ_LOCKED_GS) && (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT));}}),
 });

  areaTable[RR_WATER_TEMPLE_MQ_SINGLE_STALFOS_ROOM] = Region("Water Temple MQ Single Stalfos Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY, true),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM, {[]{return logic->HasItem(RG_SILVER_SCALE) || (logic->IsChild && logic->HasItem(RG_BRONZE_SCALE)) ||
                                                                                 (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOOKSHOT)));}})
  });

  areaTable[RR_WATER_TEMPLE_MQ_4_TORCH_ROOM] = Region("Water Temple MQ 4 Torch Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM, {[]{return (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanJumpslash())) || 
                                                                                 (logic->HasItem(RG_BRONZE_SCALE) || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 8 && logic->CanUse(RG_HOOKSHOT) ));}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DODONGO_ROOM,           {[]{return logic->CanHitSwitch() && logic->HasFireSource();}})
  });

  areaTable[RR_WATER_TEMPLE_MQ_DODONGO_ROOM] = Region("Water Temple MQ Dodongo Room", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_4_TORCH_ROOM,           {[]{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && 
                                                                                  Here(RR_WATER_TEMPLE_MQ_DODONGO_ROOM, []{return logic->CanKillEnemy(RE_DODONGO, ED_CLOSE, true, 5);});}}),
                  Entrance(RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE, {[]{return (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS)) && 
                                                                                  Here(RR_WATER_TEMPLE_MQ_DODONGO_ROOM, []{return logic->CanKillEnemy(RE_DODONGO, ED_CLOSE, true, 5);});}})
  });

  areaTable[RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE] = Region("Water Temple MQ Basement Gated Areas", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                 LOCATION(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  Entrance(RR_WATER_TEMPLE_MQ_DODONGO_ROOM, {[]{return true;}})
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_WATER_TEMPLE_BOSS_ENTRYWAY] =
        Region("Water Temple Boss Entryway", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM, {[]{return ctx->GetDungeon(WATER_TEMPLE)->IsVanilla() && false;}}),
                 Entrance(RR_WATER_TEMPLE_MQ_BOSS_DOOR,  {[]{return ctx->GetDungeon(WATER_TEMPLE)->IsMQ() && false;}}),
                 Entrance(RR_WATER_TEMPLE_BOSS_ROOM,     {[]{return true;}}),
             });

    areaTable[RR_WATER_TEMPLE_BOSS_ROOM] = Region("Water Temple Boss Room", "Water Temple", {}, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&logic->WaterTempleClear, { [] {
                return logic->WaterTempleClear || (logic->HasBossSoul(RG_MORPHA_SOUL) &&
                       (logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))));
            } }),
        },
        {
            // Locations
            LOCATION(RC_WATER_TEMPLE_MORPHA_HEART, logic->WaterTempleClear),
            LOCATION(RC_MORPHA, logic->WaterTempleClear),
        },
        {
            // Exits
            Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_LAKE_HYLIA, { [] { return logic->WaterTempleClear; } }, false),
        });
}
