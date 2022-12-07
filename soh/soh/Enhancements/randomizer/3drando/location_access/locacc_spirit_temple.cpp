#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_SpiritTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[SPIRIT_TEMPLE_ENTRYWAY] = Area("Spirit Temple Entryway", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_LOBBY,                  {[]{return Dungeon::SpiritTemple.IsVanilla();}}),
                  Entrance(SPIRIT_TEMPLE_MQ_LOBBY,               {[]{return Dungeon::SpiritTemple.IsMQ();}}),
                  Entrance(DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::SpiritTemple.IsVanilla()) {
  areaTable[SPIRIT_TEMPLE_LOBBY] = Area("Spirit Temple Lobby", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY,    {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_CHILD,       {[]{return IsChild;}}),
                  Entrance(SPIRIT_TEMPLE_EARLY_ADULT, {[]{return CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[SPIRIT_TEMPLE_CHILD] = Area("Child Spirit Temple", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutCrate, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,        {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                  LocationAccess(SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot))) && (Sticks || CanUse(DINS_FIRE));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_METAL_FENCE,            {[]{return (Boomerang || Slingshot || (HasExplosives && LogicSpiritChildBombchu)) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CHILD_CLIMB, {[]{return SmallKeys(SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[SPIRIT_TEMPLE_CHILD_CLIMB] = Area("Child Spirit Temple Climb", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    {[]{return HasProjectile(HasProjectileAge::Both) || CanUse(DINS_FIRE) ||
                                                                                      (CanTakeDamage && (Sticks || KokiriSword || HasProjectile(HasProjectileAge::Child))) ||
                                                                                        (IsChild && SmallKeys(SPIRIT_TEMPLE, 5) && HasProjectile(HasProjectileAge::Child)) ||
                                                                                          ((SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) && CanUse(SILVER_GAUNTLETS) && (HasProjectile(HasProjectileAge::Adult) || CanTakeDamage));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return HasExplosives || (SunlightArrows && CanUse(LIGHT_ARROWS));}}),
  });

  areaTable[SPIRIT_TEMPLE_EARLY_ADULT] = Area("Early Adult Spirit Temple", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_COMPASS_CHEST,            {[]{return CanUse(HOOKSHOT) && CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,  {[]{return Bow || Hookshot || HasBombchus || (Bombs && LogicSpiritLowerAdultSwitch);}}),
                  LocationAccess(SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  {[]{return SmallKeys(SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, {[]{return SmallKeys(SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_BOULDER_ROOM,          {[]{return CanPlay(SongOfTime) && (Bow || Hookshot || HasBombchus || (Bombs && LogicSpiritLowerAdultSwitch));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return SmallKeys(SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[SPIRIT_TEMPLE_CENTRAL_CHAMBER] = Area("Spirit Temple Central Chamber", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MAP_CHEST,                    {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&

                                                                                                (CanUse(DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || LogicSpiritMapChest) && Bow && Sticks))) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(STICKS)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(FIRE_ARROWS) || (LogicSpiritMapChest && Bow)) &&
                                                                                                CanUse(SILVER_GAUNTLETS));}}),
                  LocationAccess(SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,         {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&
                                                                                                (CanUse(DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || LogicSpiritSunChest) && Bow && Sticks))) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(STICKS)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(FIRE_ARROWS) || (LogicSpiritSunChest && Bow)) &&
                                                                                                CanUse(SILVER_GAUNTLETS));}}),
                  LocationAccess(SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,       {[]{return SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,  {[]{return SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && CanPlay(ZeldasLullaby) && (Hookshot || HoverBoots);}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, {[]{return (HasExplosives && Boomerang && Hookshot) ||
                                                                                            (CanUse(BOOMERANG) && SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives) ||
                                                                                            (Hookshot && CanUse(SILVER_GAUNTLETS) &&
                                                                                                (SmallKeys(SPIRIT_TEMPLE, 3) ||
                                                                                                    (SmallKeys(SPIRIT_TEMPLE, 2) && Boomerang && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))));}}),
                  LocationAccess(SPIRIT_TEMPLE_GS_LOBBY,                     {[]{return ((HasExplosives || SmallKeys(SPIRIT_TEMPLE, 3) || (SmallKeys(SPIRIT_TEMPLE, 2) && BombchusInLogic && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF))) &&
                                                                                                LogicSpiritLobbyGS && Boomerang && (Hookshot || HoverBoots)) ||
                                                                                            (LogicSpiritLobbyGS && SmallKeys(SPIRIT_TEMPLE, 5) && HasExplosives && CanUse(BOOMERANG)) ||
                                                                                            (SmallKeys(SPIRIT_TEMPLE, 3) && CanUse(SILVER_GAUNTLETS) && (Hookshot || HoverBoots));}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_OUTDOOR_HANDS,              {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR, {[]{return SmallKeys(SPIRIT_TEMPLE, 4) && CanUse(SILVER_GAUNTLETS);}}),
                  Entrance(SPIRIT_TEMPLE_CHILD_CLIMB,                {[]{return true;}}),
  });

  areaTable[SPIRIT_TEMPLE_OUTDOOR_HANDS] = Area("Spirit Temple Outdoor Hands", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return (SmallKeys(SPIRIT_TEMPLE, 3) && Longshot && HasExplosives) || SmallKeys(SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,    {[]{return SmallKeys(SPIRIT_TEMPLE, 4) && CanUse(SILVER_GAUNTLETS) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return (IsChild && SmallKeys(SPIRIT_TEMPLE, 5)) || (CanUse(SILVER_GAUNTLETS) && ((SmallKeys(SPIRIT_TEMPLE, 3) && HasExplosives) || SmallKeys(SPIRIT_TEMPLE, 5)));}}),
  });

  areaTable[SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Central Locked Door", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,         {[]{return (MirrorShield || (SunlightArrows && CanUse(LIGHT_ARROWS))) && HasExplosives;}}),
                  LocationAccess(SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  {[]{return (LogicLensSpirit || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, {[]{return (LogicLensSpirit || CanUse(LENS_OF_TRUTH)) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR, {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && (LogicSpiritWall || CanUse(LONGSHOT) || HasBombchus || ((Bombs || Nuts || CanUse(DINS_FIRE)) && (Bow || CanUse(HOOKSHOT) || Hammer)));}}),
  });

  areaTable[SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Final Locked Door", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTempleClear, {[]{return SpiritTempleClear || (MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_BOSS_KEY_CHEST, {[]{return CanPlay(ZeldasLullaby) && ((CanTakeDamage && LogicFlamingChests) || (Bow && Hookshot));}}),
                  LocationAccess(SPIRIT_TEMPLE_TOPMOST_CHEST,  {[]{return MirrorShield || (SunlightArrows && CanUse(LIGHT_ARROWS));}}),
                  LocationAccess(SPIRIT_TEMPLE_TWINROVA_HEART, {[]{return MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple;}}),
                  LocationAccess(TWINROVA,                     {[]{return MirrorShield && HasExplosives && Hookshot && BossKeySpiritTemple;}}),
  }, {});
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::SpiritTemple.IsMQ()) {
  areaTable[SPIRIT_TEMPLE_MQ_LOBBY] = Area("Spirit Temple MQ Lobby", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,  {[]{return Here(SPIRIT_TEMPLE_MQ_LOBBY, []{return CanBlastOrSmash;}) && ((IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW)));}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, {[]{return HasBombchus || (IsAdult && (CanUse(BOW) || CanUse(HOOKSHOT))) || (IsChild && (CanUse(SLINGSHOT) || CanUse(BOOMERANG)));}}),
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_CHILD, {[]{return IsChild;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_ADULT, {[]{return HasBombchus && IsAdult && CanUse(LONGSHOT) && CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_CHILD] = Area("Spirit Temple MQ Child", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (KokiriSword && HasBombchus && Slingshot);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,  {[]{return Here(SPIRIT_TEMPLE_MQ_ADULT, []{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer;});}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,       {[]{return KokiriSword && HasBombchus && Slingshot && CanUse(DINS_FIRE);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_MAP_CHEST,                  {[]{return KokiriSword || Bombs;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, {[]{return HasBombchus && SmallKeys(SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(DINS_FIRE) || (Here(SPIRIT_TEMPLE_MQ_ADULT, []{return IsAdult && CanUse(FIRE_ARROWS);})));}}),
                    //Trick: HasBombchus && SmallKeys(SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(DINS_FIRE) || (SPIRIT_TEMPLE_MQ_ADULT.Adult() && IsAdult && (CanUse(FIRE_ARROWS) || (LogicSpiritMQFrozenEye && CanUse(BOW) && CanPlay(SongOfTime)))))
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_SHARED, {[]{return HasBombchus && SmallKeys(SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_ADULT] = Area("Spirit Temple MQ Adult", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,     {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   {[]{return CanPlay(ZeldasLullaby);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, {[]{return (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,           {[]{return SmallKeys(SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,          {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,              {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime) && (MirrorShield || (SunlightArrows && CanUse(LIGHT_ARROWS)));}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  {[]{return SmallKeys(SPIRIT_TEMPLE, 7);}}),
  }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_LOWER_ADULT,        {[]{return (MirrorShield || (SunlightArrows && CanUse(LIGHT_ARROWS))) && IsAdult && CanUse(FIRE_ARROWS);}}),
                    //Trick: MirrorShield && IsAdult && (CanUse(FIRE_ARROWS) || (LogicSpiritMQLowerAdult && CanUse(DINS_FIRE) && Bow))
                  Entrance(SPIRIT_TEMPLE_MQ_SHARED,             {[]{return true;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_BOSS_AREA,          {[]{return SmallKeys(SPIRIT_TEMPLE, 6) && CanPlay(ZeldasLullaby) && Hammer;}}),
                  Entrance(SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND, {[]{return SmallKeys(SPIRIT_TEMPLE, 5) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH));}}),
  });

  areaTable[SPIRIT_TEMPLE_MQ_SHARED] = Area("Spirit Temple MQ Shared", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, {[]{return SmallKeys(SPIRIT_TEMPLE, 6);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_COMPASS_CHEST,           {[]{return (IsChild && CanUse(SLINGSHOT) && SmallKeys(SPIRIT_TEMPLE, 7)) || (IsAdult && CanUse(BOW)) || (Bow && Slingshot);}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,    {[]{return CanPlay(SongOfTime) || IsAdult;}}),
                    //Trick: CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,       {[]{return IsAdult;}}),
                    //Trick: (LogicSpiritMQSunBlockGS && Boomerange && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT)) || IsAdult
   }, {
                  //Exits
                  Entrance(SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)));}}),
                    //Trick: (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)))
                  Entrance(DESERT_COLOSSUS,                        {[]{return (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)) && IsAdult);}}),
                    //Trick: (SmallKeys(SPIRIT_TEMPLE, 7) && (CanPlay(SongOfTime) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(SPIRIT_TEMPLE, 4) && CanPlay(SongOfTime) && (LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH)) && IsAdult)
  });

  areaTable[SPIRIT_TEMPLE_MQ_LOWER_ADULT] = Area("Spirit Temple MQ Lower Adult", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,          {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,        {[]{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, {[]{return Hammer;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,             {[]{return true;}}),
                  LocationAccess(SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,           {[]{return SmallKeys(SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_BOSS_AREA] = Area("Spirit Temple MQ Boss Area", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTempleClear, {[]{return SpiritTempleClear || (MirrorShield && BossKeySpiritTemple);}}),
  }, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, {[]{return LogicLensSpiritMQ || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(SPIRIT_TEMPLE_TWINROVA_HEART,                   {[]{return MirrorShield && BossKeySpiritTemple;}}),
                  LocationAccess(TWINROVA,                                       {[]{return MirrorShield && BossKeySpiritTemple;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND] = Area("Spirit Temple MQ Mirror Shield Hand", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, {[]{return true;}}),
  }, {});

  areaTable[SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND] = Area("Spirit Temple MQ Silver Gauntlets Hand", "Spirit Temple", SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return true;}}),
  }, {});
  }
}
