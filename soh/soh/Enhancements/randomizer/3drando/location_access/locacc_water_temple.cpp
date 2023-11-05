#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_WaterTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_WATER_TEMPLE_ENTRYWAY] = Area("Water Temple Entryway", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,    {[]{return Dungeon::WaterTemple.IsVanilla();}}),
                  Entrance(RR_WATER_TEMPLE_MQ_LOBBY, {[]{return Dungeon::WaterTemple.IsMQ();}}),
                  Entrance(RR_LAKE_HYLIA,            {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::WaterTemple.IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[RR_WATER_TEMPLE_LOBBY] = Area("Water Temple Lobby", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER,            {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(RG_ZORA_TUNIC)) && (CanUse(RG_IRON_BOOTS) || (CanUse(RG_LONGSHOT) && LogicWaterTempleTorchLongshot)));}}),
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,           {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(RG_ZORA_TUNIC)) && CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_SOUTH_LOWER,           {[]{return WaterTempleLow && HasExplosives && (CanDive || CanUse(RG_IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER,            {[]{return WaterTempleLow && GoronBracelet && (IsChild || CanDive || CanUse(RG_IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(RG_ZORA_TUNIC));}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return WaterTempleLow && SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (WaterTempleLow || WaterTempleMiddle) && (HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_WATER_TEMPLE_EAST_MIDDLE,           {[]{return (WaterTempleLow || WaterTempleMiddle || (CanUse(RG_IRON_BOOTS) && WaterTimer >= 16)) && CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_WEST_MIDDLE,           {[]{return WaterTempleMiddle;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER,            {[]{return IsAdult && (CanUse(RG_HOVER_BOOTS) || (LogicDamageBoost && Bombs && CanTakeDamage));}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (WaterTempleLow || WaterTempleMiddle) && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW)) && (CanUse(RG_LONGSHOT) || CanUse(RG_HOVER_BOOTS) || (LogicWaterCentralBow && (IsAdult || WaterTempleMiddle)));}}),
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return WaterTempleHigh && SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return WaterTempleHigh && CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_LOWER] = Area("Water Temple East Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleLow, {[]{return WaterTempleLow || CanPlay(ZeldasLullaby);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,        {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(RG_ZORA_TUNIC)) && CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_WATER_TEMPLE_MAP_ROOM,     {[]{return WaterTempleHigh;}}),
                  Entrance(RR_WATER_TEMPLE_CRACKED_WALL, {[]{return WaterTempleMiddle || (WaterTempleHigh && WaterTempleLow && ((CanUse(RG_HOVER_BOOTS) && LogicWaterCrackedWallHovers) || LogicWaterCrackedWallNothing));}}),
                  Entrance(RR_WATER_TEMPLE_TORCH_ROOM,   {[]{return WaterTempleLow && (HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW));}}),
  });

  areaTable[RR_WATER_TEMPLE_MAP_ROOM] = Area("Water Temple Map Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MAP_CHEST, {[]{return (MagicMeter && CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_CRACKED_WALL] = Area("Water Temple Cracked Wall", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CRACKED_WALL_CHEST, {[]{return HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_TORCH_ROOM] = Area("Water Temple Torch Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_TORCHES_CHEST, {[]{return (MagicMeter && CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(RG_KOKIRI_SWORD)) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_NORTH_LOWER] = Area("Water Temple North Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,          {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (CanUse(RG_LONGSHOT) || (LogicWaterBossKeyRegion && CanUse(RG_HOVER_BOOTS))) && SmallKeys(RR_WATER_TEMPLE, 4);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_LOWER] = Area("Water Temple Boulders Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, {[]{return CanUse(RG_LONGSHOT) || Here(RR_WATER_TEMPLE_BOULDERS_UPPER, []{return (IsAdult && HookshotOrBoomerang) || (CanUse(RG_IRON_BOOTS) && CanUse(RG_HOOKSHOT));});}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_NORTH_LOWER,    {[]{return SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (IsAdult && (CanUse(RG_HOVER_BOOTS) || LogicWaterNorthBasementLedgeJump)) || (CanUse(RG_HOVER_BOOTS) && CanUse(RG_IRON_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_ROOM] = Area("Water Temple Block Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return (GoronBracelet && HasExplosives) || CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return (GoronBracelet && HasExplosives) || (CanUse(RG_HOOKSHOT) && CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_WATER_TEMPLE_JETS_ROOM] = Area("Water Temple Jets Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BLOCK_ROOM,     {[]{return CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BOULDERS_UPPER] = Area("Water Temple Boulders Upper", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_LOWER, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_JETS_ROOM,      {[]{return IsAdult;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_KEY_ROOM,  {[]{return (CanUse(RG_IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive)) && SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_BOSS_KEY_ROOM] = Area("Water Temple Boss Key Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_BOULDERS_UPPER, {[]{return (CanUse(RG_IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive) || IsChild || CanDive) && SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_SOUTH_LOWER] = Area("Water Temple South Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_BEHIND_GATE, {[]{return (CanJumpslash || CanUse(RG_MEGATON_HAMMER)) && (CanUse(RG_HOOKSHOT) || (IsAdult && CanUse(RG_HOVER_BOOTS)));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_LOWER] = Area("Water Temple West Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,       {[]{return CanUse(RG_HOOKSHOT) && CanUse(RG_IRON_BOOTS) && GoronBracelet;}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return CanJumpslash || CanUseProjectile;}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_ROOM] = Area("Water Temple Dragon Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_DRAGON_CHEST, {[]{return (CanUse(RG_HOOKSHOT) && CanUse(RG_IRON_BOOTS)) || (((IsAdult && LogicWaterDragonAdult && (CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW) || HasBombchus)) || (IsChild && LogicWaterDragonChild && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG) || HasBombchus))) && (CanDive || CanUse(RG_IRON_BOOTS))) ||
                                                                       Here(RR_WATER_TEMPLE_RIVER, []{return IsAdult && CanUse(RG_FAIRY_BOW) && ((LogicWaterDragonAdult && (CanDive || CanUse(RG_IRON_BOOTS))) || LogicWaterDragonJumpDive);});}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_WEST_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER] = Area("Water Temple Central Pillar Lower", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                   {[]{return SmallKeys(RR_WATER_TEMPLE, 5);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER,    {[]{return CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return WaterTempleMiddle && CanUse(RG_IRON_BOOTS) && WaterTimer >= 40;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Area("Water Temple Central Pillar Upper", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleMiddle, {[]{return WaterTempleMiddle || CanPlay(ZeldasLullaby);}}),
                }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, {[]{return CanUse(RG_LONGSHOT) || (((LogicWaterCentralGSFW && CanUse(RG_FARORES_WIND) && (CanUse(RG_FAIRY_BOW) || CanUse(RG_DINS_FIRE) || SmallKeys(RR_WATER_TEMPLE, 5))) || (LogicWaterCentralGSIrons && CanUse(RG_IRON_BOOTS) && ((CanUse(RG_HOOKSHOT) && CanUse(RG_FAIRY_BOW)) || (CanUse(RG_DINS_FIRE))))) && WaterTempleHigh && HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT] = Area("Water Temple Central Pillar Basement", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, {[]{return CanUse(RG_HOOKSHOT) && CanUse(RG_IRON_BOOTS) && WaterTimer >= 40;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return CanUse(RG_IRON_BOOTS) && WaterTimer >= 16;}}),
  });

  areaTable[RR_WATER_TEMPLE_EAST_MIDDLE] = Area("Water Temple East Middle", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_COMPASS_CHEST, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_WEST_MIDDLE] = Area("Water Temple West Middle", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,      {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_HIGH_WATER, {[]{return CanUseProjectile;}}),
  });

  areaTable[RR_WATER_TEMPLE_HIGH_WATER] = Area("Water Temple High Water", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleHigh, {[]{return WaterTempleHigh || CanPlay(ZeldasLullaby);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_WATER_TEMPLE_BLOCK_CORRIDOR] = Area("Water Temple Block Corridor", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, {[]{return GoronBracelet && (WaterTempleLow || WaterTempleMiddle);}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY, {[]{return CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM] = Area("Water Temple Falling Platform Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, {[]{return CanUse(RG_LONGSHOT) || (LogicWaterFallingPlatformGSBoomerang && IsChild && CanUse(RG_BOOMERANG)) || (LogicWaterFallingPlatformGSHookshot && IsAdult && CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,               {[]{return CanUse(RG_HOOKSHOT) && SmallKeys(RR_WATER_TEMPLE, 4);}}),
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(RG_HOOKSHOT) && SmallKeys(RR_WATER_TEMPLE, 5);}}),
  });

  areaTable[RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM] = Area("Water Temple Dragon Pillars Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return CanUseProjectile;}}),
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM,        {[]{return CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_DARK_LINK_ROOM] = Area("Water Temple Dark Link Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  Entrance(RR_WATER_TEMPLE_LONGSHOT_ROOM,       {[]{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
  });

  areaTable[RR_WATER_TEMPLE_LONGSHOT_ROOM] = Area("Water Temple Longshot Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_LONGSHOT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DARK_LINK_ROOM, {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_RIVER,          {[]{return IsChild || CanPlay(SongOfTime);}}),
  });

  areaTable[RR_WATER_TEMPLE_RIVER] = Area("Water Temple River", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_RIVER_CHEST, {[]{return (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW)) && (IsAdult || CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT));}}),
                  LocationAccess(RC_WATER_TEMPLE_GS_RIVER,    {[]{return (CanUse(RG_IRON_BOOTS) && CanUse(RG_HOOKSHOT)) || (LogicWaterRiverGS && CanUse(RG_LONGSHOT));}}),
                }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_DRAGON_ROOM, {[]{return (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW)) && (IsAdult || CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_WATER_TEMPLE_PRE_BOSS_ROOM] = Area("Water Temple Pre Boss Room", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyWaterTemple;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::WaterTemple.IsMQ()) {
  areaTable[RR_WATER_TEMPLE_MQ_LOBBY] = Area("Water Temple MQ Lobby", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY,            {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DIVE,             {[]{return IsAdult && WaterTimer >= 24 && CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_WATER_TEMPLE_MQ_DARK_LINK_REGION, {[]{return SmallKeys(RR_WATER_TEMPLE, 1) && IsAdult && CanUse(RG_LONGSHOT) && CanJumpslash && Hearts > 0;}}),
                  Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY,       {[]{return BossKeyWaterTemple && IsAdult && CanJumpslash && CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_DIVE] = Area("Water Temple MQ Dive", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_MAP_CHEST,            {[]{return HasFireSource && IsAdult && CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, {[]{return IsAdult && CanUse(RG_ZORA_TUNIC) && CanUse(RG_HOOKSHOT) && ((LogicWaterMQCentralPillar && CanUse(RG_FIRE_ARROWS)) || (CanUse(RG_DINS_FIRE) && CanPlay(SongOfTime)));}}),
                    //Trick: IsAdult && CanUse(RG_ZORA_TUNIC) && CanUse(RG_HOOKSHOT) && ((LogicWaterMQCentralPillar && CanUse(RG_FIRE_ARROWS)) || (CanUse(RG_DINS_FIRE) && CanPlay(SongOfTime)))
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS, {[]{return CanPlay(ZeldasLullaby);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS] = Area("Water Temple MQ Lowered Water Levels", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_COMPASS_CHEST,                {[]{return ((IsAdult && CanUse(RG_FAIRY_BOW)) || CanUse(RG_DINS_FIRE) || Here(RR_WATER_TEMPLE_MQ_LOBBY, []{return IsChild && CanUse(RG_STICKS) && HasExplosives;})) &&
                    (CanJumpslash || CanUseProjectile);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST,               {[]{return IsAdult && CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,          {[]{return CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, {[]{return IsAdult && CanUse(RG_LONGSHOT);}}),
  }, {});

  areaTable[RR_WATER_TEMPLE_MQ_DARK_LINK_REGION] = Area("Water Temple MQ Dark Link Region", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return IsAdult && WaterTimer >= 24 && CanUse(RG_DINS_FIRE) && (LogicWaterDragonJumpDive || CanDive || CanUse(RG_IRON_BOOTS));}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_RIVER,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS, {[]{return IsAdult && WaterTimer >= 24 && CanUse(RG_DINS_FIRE) && CanUse(RG_IRON_BOOTS);}}),
  });

  areaTable[RR_WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS] = Area("Water Temple MQ Basement Gated Areas", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,         {[]{return HoverBoots || CanUse(RG_SCARECROW) || LogicWaterNorthBasementLedgeJump;}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,     {[]{return CanUse(RG_FIRE_ARROWS) && (HoverBoots || CanUse(RG_SCARECROW));}}),
                  LocationAccess(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, {[]{return LogicWaterMQLockedGS || (SmallKeys(RR_WATER_TEMPLE, 2) && (HoverBoots || CanUse(RG_SCARECROW) || LogicWaterNorthBasementLedgeJump) && CanJumpslash);}}),
                    //Trick: LogicWaterMQLockedGS || (SmallKeys(RR_WATER_TEMPLE, 2) && (HoverBoots || CanUse(RG_SCARECROW) || LogicWaterNorthBasementLedgeJump))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_WATER_TEMPLE_BOSS_ENTRYWAY] =
        Area("Water Temple Boss Entryway", "Water Temple", RHT_WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_WATER_TEMPLE_PRE_BOSS_ROOM, { [] { return Dungeon::WaterTemple.IsVanilla() && false; } }),
                 Entrance(RR_WATER_TEMPLE_MQ_LOBBY, { [] { return Dungeon::WaterTemple.IsMQ() && false; } }),
                 Entrance(RR_WATER_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_WATER_TEMPLE_BOSS_ROOM] = Area(
        "Water Temple Boss Room", "Water Temple", RHT_NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&WaterTempleClear, { [] {
                return WaterTempleClear ||
                       (CanUse(RG_HOOKSHOT) && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)));
            } }),
        },
        {
            // Locations
            LocationAccess(RC_WATER_TEMPLE_MORPHA_HEART, { [] { return WaterTempleClear; } }),
            LocationAccess(RC_MORPHA, { [] { return WaterTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_WATER_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
        });
}
