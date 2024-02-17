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
  areaTable[WATER_TEMPLE_ENTRYWAY] = Area("Water Temple Entryway", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,    {[]{return Dungeon::WaterTemple.IsVanilla();}}),
                  Entrance(WATER_TEMPLE_MQ_LOBBY, {[]{return Dungeon::WaterTemple.IsMQ();}}),
                  Entrance(LAKE_HYLIA,            {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::WaterTemple.IsVanilla()) {
  //Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
  areaTable[WATER_TEMPLE_LOBBY] = Area("Water Temple Lobby", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_ENTRYWAY,              {[]{return true;}}),
                  Entrance(WATER_TEMPLE_EAST_LOWER,            {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && (CanUse(IRON_BOOTS) || (CanUse(LONGSHOT) && LogicWaterTempleTorchLongshot)));}}),
                  Entrance(WATER_TEMPLE_NORTH_LOWER,           {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && CanUse(IRON_BOOTS));}}),
                  Entrance(WATER_TEMPLE_SOUTH_LOWER,           {[]{return WaterTempleLow && HasExplosives && (CanDive || CanUse(IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC));}}),
                  Entrance(WATER_TEMPLE_WEST_LOWER,            {[]{return WaterTempleLow && GoronBracelet && (IsChild || CanDive || CanUse(IRON_BOOTS)) && (LogicFewerTunicRequirements || CanUse(ZORA_TUNIC));}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER,  {[]{return WaterTempleLow && SmallKeys(WATER_TEMPLE, 5);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_UPPER,  {[]{return (WaterTempleLow || WaterTempleMiddle) && (HasFireSourceWithTorch || CanUse(BOW));}}),
                  Entrance(WATER_TEMPLE_EAST_MIDDLE,           {[]{return (WaterTempleLow || WaterTempleMiddle || (CanUse(IRON_BOOTS) && WaterTimer >= 16)) && CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_WEST_MIDDLE,           {[]{return WaterTempleMiddle;}}),
                  Entrance(WATER_TEMPLE_HIGH_WATER,            {[]{return IsAdult && (CanUse(HOVER_BOOTS) || (LogicDamageBoost && Bombs && CanTakeDamage));}}),
                  Entrance(WATER_TEMPLE_BLOCK_CORRIDOR,        {[]{return (WaterTempleLow || WaterTempleMiddle) && (CanUse(SLINGSHOT) || CanUse(BOW)) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS) || (LogicWaterCentralBow && (IsAdult || WaterTempleMiddle)));}}),
                  Entrance(WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return WaterTempleHigh && SmallKeys(WATER_TEMPLE, 4);}}),
                  Entrance(WATER_TEMPLE_PRE_BOSS_ROOM,         {[]{return WaterTempleHigh && CanUse(LONGSHOT);}}),
  });

  areaTable[WATER_TEMPLE_EAST_LOWER] = Area("Water Temple East Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleLow, {[]{return WaterTempleLow || CanPlay(ZeldasLullaby);}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,        {[]{return WaterTempleLow || ((LogicFewerTunicRequirements || CanUse(ZORA_TUNIC)) && CanUse(IRON_BOOTS));}}),
                  Entrance(WATER_TEMPLE_MAP_ROOM,     {[]{return WaterTempleHigh;}}),
                  Entrance(WATER_TEMPLE_CRACKED_WALL, {[]{return WaterTempleMiddle || (WaterTempleHigh && WaterTempleLow && ((CanUse(HOVER_BOOTS) && LogicWaterCrackedWallHovers) || LogicWaterCrackedWallNothing));}}),
                  Entrance(WATER_TEMPLE_TORCH_ROOM,   {[]{return WaterTempleLow && (HasFireSourceWithTorch || CanUse(BOW));}}),
  });

  areaTable[WATER_TEMPLE_MAP_ROOM] = Area("Water Temple Map Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MAP_CHEST, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_CRACKED_WALL] = Area("Water Temple Cracked Wall", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CRACKED_WALL_CHEST, {[]{return HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_TORCH_ROOM] = Area("Water Temple Torch Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_TORCHES_CHEST, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_EAST_LOWER, {[]{return (MagicMeter && CanUse(KOKIRI_SWORD)) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_NORTH_LOWER] = Area("Water Temple North Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,          {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return (CanUse(LONGSHOT) || (LogicWaterBossKeyRegion && CanUse(HOVER_BOOTS))) && SmallKeys(WATER_TEMPLE, 4);}}),
  });

  areaTable[WATER_TEMPLE_BOULDERS_LOWER] = Area("Water Temple Boulders Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, {[]{return CanUse(LONGSHOT) || Here(WATER_TEMPLE_BOULDERS_UPPER, []{return (IsAdult && HookshotOrBoomerang) || (CanUse(IRON_BOOTS) && CanUse(HOOKSHOT));});}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_NORTH_LOWER,    {[]{return SmallKeys(WATER_TEMPLE, 4);}}),
                  Entrance(WATER_TEMPLE_BLOCK_ROOM,     {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return (IsAdult && (CanUse(HOVER_BOOTS) || LogicWaterNorthBasementLedgeJump)) || (CanUse(HOVER_BOOTS) && CanUse(IRON_BOOTS));}}),
  });

  areaTable[WATER_TEMPLE_BLOCK_ROOM] = Area("Water Temple Block Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return (GoronBracelet && HasExplosives) || CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_JETS_ROOM,      {[]{return (GoronBracelet && HasExplosives) || (CanUse(HOOKSHOT) && CanUse(HOVER_BOOTS));}}),
  });

  areaTable[WATER_TEMPLE_JETS_ROOM] = Area("Water Temple Jets Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BLOCK_ROOM,     {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_BOULDERS_UPPER] = Area("Water Temple Boulders Upper", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_LOWER, {[]{return true;}}),
                  Entrance(WATER_TEMPLE_JETS_ROOM,      {[]{return IsAdult;}}),
                  Entrance(WATER_TEMPLE_BOSS_KEY_ROOM,  {[]{return (CanUse(IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive)) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_BOSS_KEY_ROOM] = Area("Water Temple Boss Key Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_BOSS_KEY_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_BOULDERS_UPPER, {[]{return (CanUse(IRON_BOOTS) || (IsAdult && LogicWaterBKJumpDive) || IsChild || CanDive) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_SOUTH_LOWER] = Area("Water Temple South Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_BEHIND_GATE, {[]{return (CanJumpslash || CanUse(MEGATON_HAMMER)) && (CanUse(HOOKSHOT) || (IsAdult && CanUse(HOVER_BOOTS)));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_WEST_LOWER] = Area("Water Temple West Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,       {[]{return CanUse(HOOKSHOT) && CanUse(IRON_BOOTS) && GoronBracelet;}}),
                  Entrance(WATER_TEMPLE_DRAGON_ROOM, {[]{return CanJumpslash || CanUseProjectile;}}),
  });

  areaTable[WATER_TEMPLE_DRAGON_ROOM] = Area("Water Temple Dragon Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_DRAGON_CHEST, {[]{return (CanUse(HOOKSHOT) && CanUse(IRON_BOOTS)) || (((IsAdult && LogicWaterDragonAdult && (CanUse(HOOKSHOT) || CanUse(BOW) || HasBombchus)) || (IsChild && LogicWaterDragonChild && (CanUse(SLINGSHOT) || CanUse(BOOMERANG) || HasBombchus))) && (CanDive || CanUse(IRON_BOOTS))) ||
                                                                       Here(WATER_TEMPLE_RIVER, []{return IsAdult && CanUse(BOW) && ((LogicWaterDragonAdult && (CanDive || CanUse(IRON_BOOTS))) || LogicWaterDragonJumpDive);});}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_WEST_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_LOWER] = Area("Water Temple Central Pillar Lower", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,                   {[]{return SmallKeys(WATER_TEMPLE, 5);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_UPPER,    {[]{return CanUse(HOOKSHOT);}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT, {[]{return WaterTempleMiddle && CanUse(IRON_BOOTS) && WaterTimer >= 40;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_UPPER] = Area("Water Temple Central Pillar Upper", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleMiddle, {[]{return WaterTempleMiddle || CanPlay(ZeldasLullaby);}}),
                }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_CENTRAL_PILLAR, {[]{return CanUse(LONGSHOT) || (((LogicWaterCentralGSFW && CanUse(FARORES_WIND) && (CanUse(BOW) || CanUse(DINS_FIRE) || SmallKeys(WATER_TEMPLE, 5))) || (LogicWaterCentralGSIrons && CanUse(IRON_BOOTS) && ((CanUse(HOOKSHOT) && CanUse(BOW)) || (CanUse(DINS_FIRE))))) && WaterTempleHigh && HookshotOrBoomerang);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,                {[]{return true;}}),
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT] = Area("Water Temple Central Pillar Basement", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CENTRAL_PILLAR_CHEST, {[]{return CanUse(HOOKSHOT) && CanUse(IRON_BOOTS) && WaterTimer >= 40;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_CENTRAL_PILLAR_LOWER, {[]{return CanUse(IRON_BOOTS) && WaterTimer >= 16;}}),
  });

  areaTable[WATER_TEMPLE_EAST_MIDDLE] = Area("Water Temple East Middle", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_COMPASS_CHEST, {[]{return CanUseProjectile;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_WEST_MIDDLE] = Area("Water Temple West Middle", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,      {[]{return true;}}),
                  Entrance(WATER_TEMPLE_HIGH_WATER, {[]{return CanUseProjectile;}}),
  });

  areaTable[WATER_TEMPLE_HIGH_WATER] = Area("Water Temple High Water", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTempleHigh, {[]{return WaterTempleHigh || CanPlay(ZeldasLullaby);}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return true;}}),
  });

  areaTable[WATER_TEMPLE_BLOCK_CORRIDOR] = Area("Water Temple Block Corridor", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, {[]{return GoronBracelet && (WaterTempleLow || WaterTempleMiddle);}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY, {[]{return CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_FALLING_PLATFORM_ROOM] = Area("Water Temple Falling Platform Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, {[]{return CanUse(LONGSHOT) || (LogicWaterFallingPlatformGSBoomerang && IsChild && CanUse(BOOMERANG)) || (LogicWaterFallingPlatformGSHookshot && IsAdult && CanUse(HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,               {[]{return CanUse(HOOKSHOT) && SmallKeys(WATER_TEMPLE, 4);}}),
                  Entrance(WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(HOOKSHOT) && SmallKeys(WATER_TEMPLE, 5);}}),
  });

  areaTable[WATER_TEMPLE_DRAGON_PILLARS_ROOM] = Area("Water Temple Dragon Pillars Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_FALLING_PLATFORM_ROOM, {[]{return CanUseProjectile;}}),
                  Entrance(WATER_TEMPLE_DARK_LINK_ROOM,        {[]{return CanUse(HOOKSHOT);}}),
  });

  areaTable[WATER_TEMPLE_DARK_LINK_ROOM] = Area("Water Temple Dark Link Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_DRAGON_PILLARS_ROOM, {[]{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  Entrance(WATER_TEMPLE_LONGSHOT_ROOM,       {[]{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
  });

  areaTable[WATER_TEMPLE_LONGSHOT_ROOM] = Area("Water Temple Longshot Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_LONGSHOT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_DARK_LINK_ROOM, {[]{return true;}}),
                  Entrance(WATER_TEMPLE_RIVER,          {[]{return IsChild || CanPlay(SongOfTime);}}),
  });

  areaTable[WATER_TEMPLE_RIVER] = Area("Water Temple River", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_RIVER_CHEST, {[]{return (CanUse(SLINGSHOT) || CanUse(BOW)) && (IsAdult || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
                  LocationAccess(WATER_TEMPLE_GS_RIVER,    {[]{return (CanUse(IRON_BOOTS) && CanUse(HOOKSHOT)) || (LogicWaterRiverGS && CanUse(LONGSHOT));}}),
                }, {
                  //Exits
                  Entrance(WATER_TEMPLE_DRAGON_ROOM, {[]{return (CanUse(SLINGSHOT) || CanUse(BOW)) && (IsAdult || CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));}}),
  });

  areaTable[WATER_TEMPLE_PRE_BOSS_ROOM] = Area("Water Temple Pre Boss Room", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_LOBBY,     {[]{return true;}}),
                  Entrance(WATER_TEMPLE_BOSS_ENTRYWAY, {[]{return BossKeyWaterTemple;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::WaterTemple.IsMQ()) {
  areaTable[WATER_TEMPLE_MQ_LOBBY] = Area("Water Temple MQ Lobby", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(WATER_TEMPLE_ENTRYWAY,            {[]{return true;}}),
                  Entrance(WATER_TEMPLE_MQ_DIVE,             {[]{return IsAdult && WaterTimer >= 24 && CanUse(IRON_BOOTS);}}),
                  Entrance(WATER_TEMPLE_MQ_DARK_LINK_REGION, {[]{return SmallKeys(WATER_TEMPLE, 1) && IsAdult && CanUse(LONGSHOT) && CanJumpslash && Hearts > 0;}}),
                  Entrance(WATER_TEMPLE_BOSS_ENTRYWAY,       {[]{return BossKeyWaterTemple && IsAdult && CanJumpslash && CanUse(LONGSHOT);}}),
  });

  areaTable[WATER_TEMPLE_MQ_DIVE] = Area("Water Temple MQ Dive", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_MAP_CHEST,            {[]{return HasFireSource && IsAdult && CanUse(HOOKSHOT);}}),
                  LocationAccess(WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, {[]{return IsAdult && CanUse(ZORA_TUNIC) && CanUse(HOOKSHOT) && ((LogicWaterMQCentralPillar && CanUse(FIRE_ARROWS)) || (CanUse(DINS_FIRE) && CanPlay(SongOfTime)));}}),
                    //Trick: IsAdult && CanUse(ZORA_TUNIC) && CanUse(HOOKSHOT) && ((LogicWaterMQCentralPillar && CanUse(FIRE_ARROWS)) || (CanUse(DINS_FIRE) && CanPlay(SongOfTime)))
  }, {
                  //Exits
                  Entrance(WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS, {[]{return CanPlay(ZeldasLullaby);}}),
  });

  areaTable[WATER_TEMPLE_MQ_LOWERED_WATER_LEVELS] = Area("Water Temple MQ Lowered Water Levels", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_COMPASS_CHEST,                {[]{return ((IsAdult && CanUse(BOW)) || CanUse(DINS_FIRE) || Here(WATER_TEMPLE_MQ_LOBBY, []{return IsChild && CanUse(STICKS) && HasExplosives;})) &&
                    (CanJumpslash || CanUseProjectile);}}),
                  LocationAccess(WATER_TEMPLE_MQ_LONGSHOT_CHEST,               {[]{return IsAdult && CanUse(HOOKSHOT);}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,          {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, {[]{return IsAdult && CanUse(LONGSHOT);}}),
  }, {});

  areaTable[WATER_TEMPLE_MQ_DARK_LINK_REGION] = Area("Water Temple MQ Dark Link Region", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_BOSS_KEY_CHEST, {[]{return IsAdult && WaterTimer >= 24 && CanUse(DINS_FIRE) && (LogicWaterDragonJumpDive || CanDive || CanUse(IRON_BOOTS));}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_RIVER,       {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS, {[]{return IsAdult && WaterTimer >= 24 && CanUse(DINS_FIRE) && CanUse(IRON_BOOTS);}}),
  });

  areaTable[WATER_TEMPLE_MQ_BASEMENT_GATED_AREAS] = Area("Water Temple MQ Basement Gated Areas", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(WATER_TEMPLE_MQ_FREESTANDING_KEY,         {[]{return HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump;}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,     {[]{return CanUse(FIRE_ARROWS) && (HoverBoots || CanUse(SCARECROW));}}),
                  LocationAccess(WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, {[]{return LogicWaterMQLockedGS || (SmallKeys(WATER_TEMPLE, 2) && (HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump) && CanJumpslash);}}),
                    //Trick: LogicWaterMQLockedGS || (SmallKeys(WATER_TEMPLE, 2) && (HoverBoots || CanUse(SCARECROW) || LogicWaterNorthBasementLedgeJump))
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[WATER_TEMPLE_BOSS_ENTRYWAY] =
        Area("Water Temple Boss Entryway", "Water Temple", WATER_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(WATER_TEMPLE_PRE_BOSS_ROOM, { [] { return Dungeon::WaterTemple.IsVanilla() && false; } }),
                 Entrance(WATER_TEMPLE_MQ_LOBBY, { [] { return Dungeon::WaterTemple.IsMQ() && false; } }),
                 Entrance(WATER_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[WATER_TEMPLE_BOSS_ROOM] = Area(
        "Water Temple Boss Room", "Water Temple", NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&WaterTempleClear, { [] {
                return WaterTempleClear ||
                       (CanUse(HOOKSHOT) && (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)));
            } }),
        },
        {
            // Locations
            LocationAccess(WATER_TEMPLE_MORPHA_HEART, { [] { return WaterTempleClear; } }),
            LocationAccess(MORPHA, { [] { return WaterTempleClear; } }),
        },
        {
            // Exits
            Entrance(WATER_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(LAKE_HYLIA, { [] { return WaterTempleClear; } }),
        });
}
