#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_WaterTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_WATER_TEMPLE_ENTRYWAY] = Area("Water Temple Entryway", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,    {[]{return logic->Swim && randoCtx->GetDungeon(WATER_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_LOBBY, {[]{return logic->Swim && randoCtx->GetDungeon(WATER_TEMPLE)->IsMQ();}}),
                  Entrance(RR_LAKE_HYLIA,            {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(WATER_TEMPLE)->IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[RR_WATER_TEMPLE_LOBBY] = Area("Water Temple Lobby", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER,            {[]{return logic->WaterTempleLow || ((randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && (logic->CanUse(RG_IRON_BOOTS) || (logic->CanUse(RG_LONGSHOT) && randoCtx->GetTrickOption(RT_WATER_LONGSHOT_TORCH))));}}),
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,           {[]{return logic->WaterTempleLow || ((randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_SOUTH_LOWER,           {[]{return logic->WaterTempleLow && logic->HasExplosives && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS)) && (randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER,            {[]{return logic->WaterTempleLow && logic->GoronBracelet && (logic->IsChild || logic->CanDive || logic->CanUse(RG_IRON_BOOTS)) && (randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return logic->WaterTempleLow && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle) && (logic->HasFireSourceWithTorch || logic->CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_WATER_TEMPLE_EAST_MIDDLE,           {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle || (logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer >= 16)) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_WEST_MIDDLE,           {[]{return logic->WaterTempleMiddle;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER,            {[]{return logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_DAMAGE_BOOST) && logic->Bombs && logic->CanTakeDamage));}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (logic->WaterTempleLow || logic->WaterTempleMiddle) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_WATER_CENTRAL_BOW) && (logic->IsAdult || logic->WaterTempleMiddle)));}}),
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return logic->WaterTempleHigh && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return logic->WaterTempleHigh && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_LOWER] = Area("Water Temple East Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTempleLow, {[]{return logic->WaterTempleLow || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,        {[]{return logic->WaterTempleLow || ((randoCtx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS) || logic->CanUse(RG_ZORA_TUNIC)) && logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_MAP_ROOM,     {[]{return logic->WaterTempleHigh;}}),
                  Entrance(RR_WATER_TEMPLE_CRACKED_WALL, {[]{return logic->WaterTempleMiddle || (logic->WaterTempleHigh && logic->WaterTempleLow && ((logic->CanUse(RG_HOVER_BOOTS) && randoCtx->GetTrickOption(RT_WATER_CRACKED_WALL_HOVERS)) || randoCtx->GetTrickOption(RT_WATER_CRACKED_WALL)));}}),
                  Entrance(RR_WATER_TEMPLE_TORCH_ROOM,   {[]{return logic->WaterTempleLow && (logic->HasFireSourceWithTorch || logic->CanUse(RG_FAIRY_BOW));}}),
  });

  areaTable[RR_WATER_TEMPLE_MAP_ROOM] = Area("Water Temple Map Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MAP_CHEST, {[]{return (logic->MagicMeter && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (logic->MagicMeter && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_CRACKED_WALL] = Area("Water Temple Cracked Wall", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CRACKED_WALL_CHEST, {[]{return logic->HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_TORCH_ROOM] = Area("Water Temple Torch Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_TORCHES_CHEST, {[]{return (logic->MagicMeter && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (logic->MagicMeter && logic->CanUse(RG_KOKIRI_SWORD)) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_NORTH_LOWER] = Area("Water Temple North Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,          {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_WATER_BK_REGION) && logic->CanUse(RG_HOVER_BOOTS))) && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_LOWER] = Area("Water Temple Boulders Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, {[]{return logic->CanUse(RG_LONGSHOT) || Here(RR_WATER_TEMPLE_BOULDERS_UPPER, []{return (logic->IsAdult && logic->HookshotOrBoomerang) || (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT));});}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,    {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || randoCtx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP))) || (logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_IRON_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_ROOM] = Area("Water Temple Block Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (logic->GoronBracelet && logic->HasExplosives) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return (logic->GoronBracelet && logic->HasExplosives) || (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_JETS_ROOM] = Area("Water Temple Jets Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_UPPER] = Area("Water Temple Boulders Upper", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return logic->IsAdult;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_KEY_ROOM,  {[]{return (logic->CanUse(RG_IRON_BOOTS) || (logic->IsAdult && randoCtx->GetTrickOption(RT_WATER_BK_JUMP_DIVE))) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOSS_KEY_ROOM] = Area("Water Temple Boss Key Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (logic->CanUse(RG_IRON_BOOTS) || (logic->IsAdult && randoCtx->GetTrickOption(RT_WATER_BK_JUMP_DIVE)) || logic->IsChild || logic->CanDive) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_SOUTH_LOWER] = Area("Water Temple South Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_BEHIND_GATE, {[]{return (logic->CanJumpslash || logic->CanUse(RG_MEGATON_HAMMER)) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_LOWER] = Area("Water Temple West Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,       {[]{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->GoronBracelet;}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return logic->CanJumpslash || logic->CanUseProjectile;}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_ROOM] = Area("Water Temple Dragon Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_DRAGON_CHEST, {[]{return (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS)) || (((logic->IsAdult && randoCtx->GetTrickOption(RT_WATER_ADULT_DRAGON) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->HasBombchus)) || (logic->IsChild && randoCtx->GetTrickOption(RT_WATER_CHILD_DRAGON) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasBombchus))) && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS))) ||
                                                                       Here(RR_WATER_TEMPLE_RIVER, []{return logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && ((randoCtx->GetTrickOption(RT_WATER_ADULT_DRAGON) && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS))) || randoCtx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE));});}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER] = Area("Water Temple Central Pillar Lower", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                   {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,    {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return logic->WaterTempleMiddle && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer >= 40;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Area("Water Temple Central Pillar Upper", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTempleMiddle, {[]{return logic->WaterTempleMiddle || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, {[]{return logic->CanUse(RG_LONGSHOT) || (((randoCtx->GetTrickOption(RT_WATER_FW_CENTRAL_GS) && logic->CanUse(RG_FARORES_WIND) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->SmallKeys(RR_WATER_TEMPLE, 5))) || (randoCtx->GetTrickOption(RT_WATER_IRONS_CENTRAL_GS) && logic->CanUse(RG_IRON_BOOTS) && ((logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_FAIRY_BOW)) || (logic->CanUse(RG_DINS_FIRE))))) && logic->WaterTempleHigh && logic->HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT] = Area("Water Temple Central Pillar Basement", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, {[]{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer >= 40;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer >= 16;}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_MIDDLE] = Area("Water Temple East Middle", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_COMPASS_CHEST, {[]{return logic->CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_MIDDLE] = Area("Water Temple West Middle", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,      {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER, {[]{return logic->CanUseProjectile;}}),
  });

  areaTable[RR_WATER_TEMPLE_HIGH_WATER] = Area("Water Temple High Water", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTempleHigh, {[]{return logic->WaterTempleHigh || logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_CORRIDOR] = Area("Water Temple Block Corridor", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, {[]{return logic->GoronBracelet && (logic->WaterTempleLow || logic->WaterTempleMiddle);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM] = Area("Water Temple Falling Platform Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, {[]{return logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_WATER_RANG_FALLING_PLATFORM_GS) && logic->IsChild && logic->CanUse(RG_BOOMERANG)) || (randoCtx->GetTrickOption(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,               {[]{return logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM] = Area("Water Temple Dragon Pillars Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return logic->CanUseProjectile;}}),
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM,        {[]{return logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_DARK_LINK_ROOM] = Area("Water Temple Dark Link Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
                  Entrance(RR_WATER_TEMPLE_LONGSHOT_ROOM,       {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
  });

  areaTable[RR_WATER_TEMPLE_LONGSHOT_ROOM] = Area("Water Temple Longshot Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_LONGSHOT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_RIVER,          {[]{return logic->IsChild || logic->CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_WATER_TEMPLE_RIVER] = Area("Water Temple River", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_RIVER_CHEST, {[]{return (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                  LocationAccess(RC_WATER_TEMPLE_GS_RIVER,    {[]{return (logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_WATER_RIVER_GS) && logic->CanUse(RG_LONGSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_WATER_TEMPLE_PRE_BOSS_ROOM] = Area("Water Temple Pre Boss Room", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->BossKeyWaterTemple;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(WATER_TEMPLE)->IsMQ()) {
  areaTable[RR_WATER_TEMPLE_MQ_LOBBY] = Area("Water Temple MQ Lobby", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,            {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DIVE,             {[]{return logic->IsAdult && logic->WaterTimer >= 24 && logic->CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DARK_LINK_REGION, {[]{return logic->SmallKeys(RR_WATER_TEMPLE, 1) && logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslash && logic->Hearts > 0;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,       {[]{return logic->BossKeyWaterTemple && logic->IsAdult && logic->CanJumpslash && logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_DIVE] = Area("Water Temple MQ Dive", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_MAP_CHEST,            {[]{return logic->HasFireSource && logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, {[]{return logic->IsAdult && logic->CanUse(RG_ZORA_TUNIC) && logic->CanUse(RG_HOOKSHOT) && ((randoCtx->GetTrickOption(RT_WATER_MQ_CENTRAL_PILLAR) && logic->CanUse(RG_FIRE_ARROWS)) || (logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_SONG_OF_TIME)));}}),
                    //Trick: logic->IsAdult && logic->CanUse(RG_ZORA_TUNIC) && logic->CanUse(RG_HOOKSHOT) && ((LogicWaterMQCentralPillar && logic->CanUse(RG_FIRE_ARROWS)) || (logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_SONG_OF_TIME)))
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS] = Area("Water Temple MQ Lowered Water Levels", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_COMPASS_CHEST,                {[]{return ((logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || logic->CanUse(RG_DINS_FIRE) || Here(RR_WATER_TEMPLE_MQ_LOBBY, []{return logic->IsChild && logic->CanUse(RG_STICKS) && logic->HasExplosives;})) &&
                    (logic->CanJumpslash || logic->CanUseProjectile);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST,               {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,          {[]{return logic->CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, {[]{return logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
  }, {});

  areaTable[RR_WATER_TEMPLE_MQ_DARK_LINK_REGION] = Area("Water Temple MQ Dark Link Region", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return logic->IsAdult && logic->WaterTimer >= 24 && logic->CanUse(RG_DINS_FIRE) && (randoCtx->GetTrickOption(RT_WATER_DRAGON_JUMP_DIVE) || logic->CanDive || logic->CanUse(RG_IRON_BOOTS));}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_RIVER,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS, {[]{return logic->IsAdult && logic->WaterTimer >= 24 && logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS] = Area("Water Temple MQ Basement Gated Areas", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,         {[]{return logic->HoverBoots || logic->CanUse(RG_SCARECROW) || randoCtx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,     {[]{return logic->CanUse(RG_FIRE_ARROWS) && (logic->HoverBoots || logic->CanUse(RG_SCARECROW));}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, {[]{return randoCtx->GetTrickOption(RT_WATER_MQ_LOCKED_GS) || (logic->SmallKeys(RR_WATER_TEMPLE, 2) && (logic->HoverBoots || logic->CanUse(RG_SCARECROW) || randoCtx->GetTrickOption(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP)) && logic->CanJumpslash);}}),
                    //Trick: LogicWaterMQLockedGS || (logic->SmallKeys(RR_WATER_TEMPLE, 2) && (logic->HoverBoots || logic->CanUse(RG_SCARECROW) || LogicWaterNorthBasementLedgeJump))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_WATER_TEMPLE_BOSS_ENTRYWAY] =
        Area("Water Temple Boss Entryway", "Water Temple", RA_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM, { [] { return randoCtx->GetDungeon(WATER_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_WATER_TEMPLE_MQ_LOBBY, { [] { return randoCtx->GetDungeon(WATER_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_WATER_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_WATER_TEMPLE_BOSS_ROOM] = Area(
        "Water Temple Boss Room", "Water Temple", RA_NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&logic->WaterTempleClear, { [] {
                return logic->WaterTempleClear || (logic->HasBossSoul(RG_MORPHA_SOUL) &&
                       (logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))));
            } }),
        },
        {
            // Locations
            LocationAccess(RC_WATER_TEMPLE_MORPHA_HEART, { [] { return logic->WaterTempleClear; } }),
            LocationAccess(RC_MORPHA, { [] { return logic->WaterTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_LAKE_HYLIA, { [] { return logic->WaterTempleClear; } }),
        });
}
