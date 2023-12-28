#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_SpiritTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_SPIRIT_TEMPLE_ENTRYWAY] = Area("Spirit Temple Entryway", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_LOBBY,                  {[]{return randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,               {[]{return randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsMQ();}}),
                  Entrance(RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla()) {
  areaTable[RR_SPIRIT_TEMPLE_LOBBY] = Area("Spirit Temple Lobby", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,    {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_CHILD,       {[]{return IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_EARLY_ADULT, {[]{return CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD] = Area("Child Spirit Temple", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutCrate, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,        {[]{return (Boomerang || Slingshot || (HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, {[]{return (Boomerang || Slingshot || (HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot))) && (Sticks || CanUse(RG_DINS_FIRE));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_METAL_FENCE,            {[]{return (Boomerang || Slingshot || (HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword || Slingshot)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD_CLIMB] = Area("Child Spirit Temple Climb", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && CanUse(RG_SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  {[]{return HasProjectile(HasProjectileAge::Both) || ((SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && CanUse(RG_SILVER_GAUNTLETS) && HasProjectile(HasProjectileAge::Adult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 5) && IsChild && HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    {[]{return HasProjectile(HasProjectileAge::Both) || CanUse(RG_DINS_FIRE) ||
                                                                                      (CanTakeDamage && (CanJumpslash || HasProjectile(HasProjectileAge::Child))) ||
                                                                                        (IsChild && SmallKeys(RR_SPIRIT_TEMPLE, 5) && HasProjectile(HasProjectileAge::Child)) ||
                                                                                          ((SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && CanUse(RG_SILVER_GAUNTLETS) && (HasProjectile(HasProjectileAge::Adult) || (CanTakeDamage && CanJumpslash)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return HasExplosives || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_EARLY_ADULT] = Area("Early Adult Spirit Temple", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_COMPASS_CHEST,            {[]{return CanUse(RG_HOOKSHOT) && CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,  {[]{return (CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG) || HasBombchus || (Bombs && IsAdult && randoCtx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH))) && (CanUse(RG_HOVER_BOOTS) || CanJumpslash);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,          {[]{return CanUse(RG_SONG_OF_TIME) && (Bow || Hookshot || HasBombchus || (Bombs && randoCtx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER] = Area("Spirit Temple Central Chamber", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MAP_CHEST,                    {[]{return ((HasExplosives || SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&

                                                                                                (CanUse(RG_DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || randoCtx->GetTrickOption(RT_SPIRIT_MAP_CHEST)) && Bow && Sticks))) ||
                                                                                            (SmallKeys(RR_SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(RG_STICKS)) ||
                                                                                            (SmallKeys(RR_SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MAP_CHEST) && Bow)) &&
                                                                                                CanUse(RG_SILVER_GAUNTLETS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,         {[]{return ((HasExplosives || SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                                                (CanUse(RG_DINS_FIRE) ||
                                                                                                    (((MagicMeter && FireArrows) || randoCtx->GetTrickOption(RT_SPIRIT_SUN_CHEST)) && Bow && Sticks))) ||
                                                                                            (SmallKeys(RR_SPIRIT_TEMPLE, 5) && HasExplosives &&
                                                                                                CanUse(RG_STICKS)) ||
                                                                                            (SmallKeys(RR_SPIRIT_TEMPLE, 3) &&
                                                                                                (CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_SUN_CHEST) && Bow)) &&
                                                                                                CanUse(RG_SILVER_GAUNTLETS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,       {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 3) && CanUse(RG_SILVER_GAUNTLETS) && CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,  {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 3) && CanUse(RG_SILVER_GAUNTLETS) && CanUse(RG_ZELDAS_LULLABY) && (Hookshot || HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, {[]{return (HasExplosives && Boomerang && Hookshot) ||
                                                                                            (CanUse(RG_BOOMERANG) && SmallKeys(RR_SPIRIT_TEMPLE, 5) && HasExplosives) ||
                                                                                            (Hookshot && CanUse(RG_SILVER_GAUNTLETS) &&
                                                                                                (SmallKeys(RR_SPIRIT_TEMPLE, 3) ||
                                                                                                    (SmallKeys(RR_SPIRIT_TEMPLE, 2) && Boomerang && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_LOBBY,                     {[]{return ((HasExplosives || SmallKeys(RR_SPIRIT_TEMPLE, 3) || (SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                                                randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && Boomerang && (Hookshot || HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP))) ||
                                                                                            (randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && SmallKeys(RR_SPIRIT_TEMPLE, 5) && HasExplosives && CanUse(RG_BOOMERANG)) ||
                                                                                            (SmallKeys(RR_SPIRIT_TEMPLE, 3) && CanUse(RG_SILVER_GAUNTLETS) && (Hookshot || HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_OUTDOOR_HANDS,              {[]{return CanJumpslash || HasExplosives;}}),
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB,                {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_OUTDOOR_HANDS] = Area("Spirit Temple Outdoor Hands", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return (SmallKeys(RR_SPIRIT_TEMPLE, 3) && Longshot && HasExplosives) || SmallKeys(RR_SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,    {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SILVER_GAUNTLETS) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return (IsChild && SmallKeys(RR_SPIRIT_TEMPLE, 5)) || (CanUse(RG_SILVER_GAUNTLETS) && ((SmallKeys(RR_SPIRIT_TEMPLE, 3) && HasExplosives) || SmallKeys(RR_SPIRIT_TEMPLE, 5)));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Central Locked Door", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,         {[]{return (MirrorShield || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS))) && HasExplosives;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT) || CanUse(RG_LENS_OF_TRUTH)) && HasExplosives;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT) || CanUse(RG_LENS_OF_TRUTH)) && HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 5) && (randoCtx->GetTrickOption(RT_SPIRIT_WALL) || CanUse(RG_LONGSHOT) || HasBombchus || ((Bombs || Nuts || CanUse(RG_DINS_FIRE)) && (Bow || CanUse(RG_HOOKSHOT) || Hammer)));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Final Locked Door", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, {[]{return CanUse(RG_ZELDAS_LULLABY) && ((CanTakeDamage && randoCtx->GetTrickOption(RT_FLAMING_CHESTS)) || (Bow && Hookshot));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_TOPMOST_CHEST,  {[]{return (MirrorShield && CanAdultAttack) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS));}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD, {[]{return MirrorShield && HasExplosives && Hookshot;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD] =
            Area("Spirit Temple Inside Statue Head", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return BossKeySpiritTemple; } }),
                 });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsMQ()) {
  areaTable[RR_SPIRIT_TEMPLE_MQ_LOBBY] = Area("Spirit Temple MQ Lobby", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,  {[]{return Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return CanBlastOrSmash;}) && ((IsChild && CanUse(RG_FAIRY_SLINGSHOT)) || (IsAdult && CanUse(RG_FAIRY_BOW)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, {[]{return HasBombchus || (IsAdult && (CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT))) || (IsChild && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG)));}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_CHILD, {[]{return IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_ADULT, {[]{return HasBombchus && IsAdult && CanUse(RG_LONGSHOT) && CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_CHILD] = Area("Spirit Temple MQ Child", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (KokiriSword && HasBombchus && Slingshot);}}),
  }, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,  {[]{return Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return SmallKeys(RR_SPIRIT_TEMPLE, 7) && Hammer;});}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,       {[]{return KokiriSword && HasBombchus && Slingshot && CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST,                  {[]{return KokiriSword || Bombs;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, {[]{return HasBombchus && SmallKeys(RR_SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(RG_DINS_FIRE) || (Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return IsAdult && (CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MQ_FROZEN_EYE) && CanUse(RG_FAIRY_BOW) && CanUse(RG_SONG_OF_TIME)));})));}}),
                    //Trick: HasBombchus && SmallKeys(RR_SPIRIT_TEMPLE, 7) && Slingshot && (CanUse(RG_DINS_FIRE) || (SPIRIT_TEMPLE_MQ_ADULT.Adult() && IsAdult && (CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQFrozenEye && CanUse(RG_FAIRY_BOW) && CanUse(RG_SONG_OF_TIME)))))
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, {[]{return HasBombchus && SmallKeys(RR_SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_ADULT] = Area("Spirit Temple MQ Adult", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,     {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   {[]{return CanUse(RG_ZELDAS_LULLABY) && (CanJumpslash || CanUse(RG_HOVER_BOOTS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,           {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,          {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 5) && CanUse(RG_SONG_OF_TIME);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,              {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 5) && CanUse(RG_SONG_OF_TIME) && (MirrorShield || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT,        {[]{return MirrorShield && IsAdult && (CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MQ_LOWER_ADULT) && CanUse(RG_DINS_FIRE) && Bow));}}),
                    //Trick: MirrorShield && IsAdult && (CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQLowerAdult && CanUse(RG_DINS_FIRE) && Bow))
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED,             {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BOSS_AREA,          {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 6) && CanUse(RG_ZELDAS_LULLABY) && Hammer;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 5) && CanUse(RG_SONG_OF_TIME) && CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SHARED] = Area("Spirit Temple MQ Shared", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 6);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,           {[]{return (IsChild && CanUse(RG_FAIRY_SLINGSHOT) && SmallKeys(RR_SPIRIT_TEMPLE, 7)) || (IsAdult && CanUse(RG_FAIRY_BOW)) || (Bow && Slingshot);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,    {[]{return CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || IsAdult;}}),
                    //Trick: CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || IsAdult
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,       {[]{return (randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_GS) && Boomerang && (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT))) || IsAdult;}}),
                    //Trick: (LogicSpiritMQSunBlockGS && Boomerang && (CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT)) || IsAdult
   }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return (SmallKeys(RR_SPIRIT_TEMPLE, 7) && (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || IsAdult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SONG_OF_TIME) && CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || CanUse(RG_LENS_OF_TRUTH)));}}),
                    //Trick: (SmallKeys(RR_SPIRIT_TEMPLE, 7) && (CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || CanUse(RG_LENS_OF_TRUTH)))
                  Entrance(RR_DESERT_COLOSSUS,                        {[]{return (SmallKeys(RR_SPIRIT_TEMPLE, 7) && (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || IsAdult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SONG_OF_TIME) && CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || CanUse(RG_LENS_OF_TRUTH)) && IsAdult);}}),
                    //Trick: (SmallKeys(RR_SPIRIT_TEMPLE, 7) && (CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || IsAdult)) || (SmallKeys(RR_SPIRIT_TEMPLE, 4) && CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || CanUse(RG_LENS_OF_TRUTH)) && IsAdult)
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT] = Area("Spirit Temple MQ Lower Adult", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,          {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,        {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, {[]{return Hammer;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,             {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,           {[]{return SmallKeys(RR_SPIRIT_TEMPLE, 7) && Hammer && Ocarina && SongOfTime && EponasSong && SunsSong && SongOfStorms && ZeldasLullaby;}}),
  }, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_BOSS_AREA] = Area("Spirit Temple MQ Boss Area", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, {[]{return randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || CanUse(RG_LENS_OF_TRUTH);}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, {[]{return MirrorShield && Hookshot;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD] =
            Area("Spirit Temple MQ Inside Statue Head", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return BossKeySpiritTemple; } }),
                 });

  areaTable[RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND] = Area("Spirit Temple MQ Mirror Shield Hand", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, {[]{return true;}}),
  }, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND] = Area("Spirit Temple MQ Silver Gauntlets Hand", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return true;}}),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY] = Area(
        "Spirit Temple Boss Entryway", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
        {
            // Exits
            Entrance(RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD, { [] { return randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla() && false; } }),
            Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, { [] { return randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsMQ() && false; } }),
            Entrance(RR_SPIRIT_TEMPLE_BOSS_ROOM, { [] { return true; } }),
        });

    areaTable[RR_SPIRIT_TEMPLE_BOSS_ROOM] = Area(
        "Spirit Temple Boss Room", "Spirit Temple", RA_NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&SpiritTempleClear, { [] {
                return SpiritTempleClear || (HasBossSoul(RG_TWINROVA_SOUL) && (CanUse(RG_MIRROR_SHIELD) &&
                                             (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD))));
            } }),
        },
        {
            // Locations
            LocationAccess(RC_SPIRIT_TEMPLE_TWINROVA_HEART, { [] { return SpiritTempleClear; } }),
            LocationAccess(RC_TWINROVA, { [] { return SpiritTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
        });
}
