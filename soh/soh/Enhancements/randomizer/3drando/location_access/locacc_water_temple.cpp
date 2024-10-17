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
                  Entrance(RR_WATER_TEMPLE_LOBBY,    {[]{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_LOBBY, {[]{return logic->HasItem(RG_BRONZE_SCALE) && ctx->GetDungeon(WATER_TEMPLE)->IsMQ();}}),
                  Entrance(RR_LAKE_HYLIA,            {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(WATER_TEMPLE)->IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[RR_WATER_TEMPLE_LOBBY] = Region("Water Temple Lobby", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER,            {[]{return logic->WaterTempleLow || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && (logic->CanUse(RG_IRON_BOOTS) || (logic->CanUse(RG_LONGSHOT) && ctx->GetTrickOption(RT_WATER_LONGSHOT_TORCH))));}}),
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,           {[]{return logic->WaterTempleLow || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_SOUTH_LOWER,           {[]{return logic->WaterTempleLow && logic->HasExplosives() && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)) && (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER,            {[]{return logic->WaterTempleLow && logic->HasItem(RG_GORONS_BRACELET) && (logic->IsChild || logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS)) && (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return logic->WaterTempleLow && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle) && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_WATER_TEMPLE_EAST_MIDDLE,           {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 16)) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_WEST_MIDDLE,           {[]{return logic->WaterTempleMiddle;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER,            {[]{return logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_DAMAGE_BOOST) && logic->CanUse(RG_BOMB_BAG) && logic->TakeDamage()));}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_WATER_CENTRAL_BOW) && (logic->IsAdult || logic->WaterTempleMiddle)));}}),
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return logic->WaterTempleHigh && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return logic->WaterTempleHigh && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_LOWER] = Region("Water Temple East Lower", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTempleLow, {[]{return logic->WaterTempleLow || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,        {[]{return logic->WaterTempleLow || ((ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_MAP_ROOM,     {[]{return logic->WaterTempleHigh;}}),
                  Entrance(RR_WATER_TEMPLE_CRACKED_WALL, {[]{return logic->WaterTempleMiddle || (logic->WaterTempleHigh && logic->WaterTempleLow && ((logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_WATER_CRACKED_WALL_HOVERS)) || ctx->GetTrickOption(RT_WATER_CRACKED_WALL)));}}),
                  Entrance(RR_WATER_TEMPLE_TORCH_ROOM,   {[]{return logic->WaterTempleLow && (logic->HasFireSourceWithTorch() || logic->CanUse(RG_FAIRY_BOW));}}),
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
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return logic->WaterTempleMiddle && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 40;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Region("Water Temple Central Pillar Upper", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTempleMiddle, {[]{return logic->WaterTempleMiddle || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, logic->CanUse(RG_LONGSHOT) || (((ctx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->SmallKeys(RR_WATER_TEMPLE, 5))) || (ctx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) && logic->CanUse(RG_IRON_BOOTS) && ((logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FAIRY_BOW)) || (logic->CanUse(RG_DINS_FIRE))))) && logic->WaterTempleHigh && logic->HookshotOrBoomerang())),
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
                  EventAccess(&logic->WaterTempleHigh, {[]{return logic->WaterTempleHigh || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_CORRIDOR] = Region("Water Temple Block Corridor", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, logic->HasItem(RG_GORONS_BRACELET) && (logic->WaterTempleLow || logic->WaterTempleMiddle)),
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
  areaTable[RR_WATER_TEMPLE_MQ_LOBBY] = Region("Water Temple MQ Lobby", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,            {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DIVE,             {[]{return logic->IsAdult && logic->WaterTimer() >= 24 && logic->CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DARK_LINK_REGION, {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 1) && logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslashExceptHammer();}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,       {[]{return logic->HasItem(RG_WATER_TEMPLE_BOSS_KEY) && logic->IsAdult && logic->CanJumpslashExceptHammer() && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_DIVE] = Region("Water Temple MQ Dive", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_MAP_CHEST,            logic->HasFireSource() && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)),
                  LOCATION(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, logic->IsAdult && logic->CanUse(RG_ZORA_TUNIC) && logic->CanUse(RG_HOOKSHOT) && ((ctx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS)) || (logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_SONG_OF_TIME)))),
                    //Trick: logic->IsAdult && logic->CanUse(RG_ZORA_TUNIC) && logic->CanUse(RG_HOOKSHOT) && ((LogicWaterMQCentralPillar && logic->CanUse(RG_FIRE_ARROWS)) || (logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_SONG_OF_TIME)))
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS] = Region("Water Temple MQ Lowered Water Levels", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_COMPASS_CHEST,                ((logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || logic->CanUse(RG_DINS_FIRE) || Here(RR_WATER_TEMPLE_MQ_LOBBY, []{return logic->IsChild && logic->CanUse(RG_STICKS) && logic->HasExplosives();})) &&
                    (logic->CanJumpslashExceptHammer() || logic->CanUseProjectile())),
                  LOCATION(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST,               logic->IsAdult && logic->CanUse(RG_HOOKSHOT)),
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,          logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, logic->IsAdult && logic->CanUse(RG_LONGSHOT)),
  }, {});

  areaTable[RR_WATER_TEMPLE_MQ_DARK_LINK_REGION] = Region("Water Temple MQ Dark Link Region", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, logic->IsAdult && logic->WaterTimer() >= 24 && logic->CanUse(RG_DINS_FIRE) && (ctx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE) || logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS))),
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_RIVER,       true),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS, {[]{return logic->IsAdult && logic->WaterTimer() >= 24 && logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS] = Region("Water Temple MQ Basement Gated Areas", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,         logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SCARECROW) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP)),
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,     logic->CanUse(RG_FIRE_ARROWS) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SCARECROW))),
                  LOCATION(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, ctx->GetTrickOption(RT_WATER_MQ_LOCKED_GS) || (logic->SmallKeys(RR_WATER_TEMPLE, 2) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SCARECROW) || ctx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP)) && logic->CanJumpslashExceptHammer())),
                    //Trick: LogicWaterMQLockedGS || (logic->SmallKeys(RR_WATER_TEMPLE, 2) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SCARECROW) || LogicWaterNorthBasementLedgeJump))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_WATER_TEMPLE_BOSS_ENTRYWAY] =
        Region("Water Temple Boss Entryway", "Water Temple", {RA_WATER_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM, { [] { return ctx->GetDungeon(WATER_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_WATER_TEMPLE_MQ_LOBBY, { [] { return ctx->GetDungeon(WATER_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_WATER_TEMPLE_BOSS_ROOM, { [] { return true; } }),
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
