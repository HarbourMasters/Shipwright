#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

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
                  Entrance(RR_SPIRIT_TEMPLE_CHILD,       {[]{return logic->IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_EARLY_ADULT, {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD] = Area("Child Spirit Temple", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutCrate, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,        {[]{return (logic->Boomerang || logic->Slingshot || (logic->HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives || ((logic->Nuts || logic->Boomerang) && (logic->Sticks || logic->KokiriSword || logic->Slingshot)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, {[]{return (logic->Boomerang || logic->Slingshot || (logic->HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives || ((logic->Nuts || logic->Boomerang) && (logic->Sticks || logic->KokiriSword || logic->Slingshot))) && (logic->Sticks || logic->CanUse(RG_DINS_FIRE));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_METAL_FENCE,            {[]{return (logic->Boomerang || logic->Slingshot || (logic->HasBombchus && randoCtx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives || ((logic->Nuts || logic->Boomerang) && (logic->Sticks || logic->KokiriSword || logic->Slingshot)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD_CLIMB] = Area("Child Spirit Temple Climb", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, {[]{return logic->HasProjectile(HasProjectileAge::Both) || ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasProjectile(HasProjectileAge::Adult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->IsChild && logic->HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  {[]{return logic->HasProjectile(HasProjectileAge::Both) || ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasProjectile(HasProjectileAge::Adult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->IsChild && logic->HasProjectile(HasProjectileAge::Child));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    {[]{return logic->HasProjectile(HasProjectileAge::Both) || logic->CanUse(RG_DINS_FIRE) ||
                                                                                      (logic->CanTakeDamage && (logic->CanJumpslash || logic->HasProjectile(HasProjectileAge::Child))) ||
                                                                                        (logic->IsChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasProjectile(HasProjectileAge::Child)) ||
                                                                                          ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->HasProjectile(HasProjectileAge::Adult) || (logic->CanTakeDamage && logic->CanJumpslash)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return logic->HasExplosives || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_EARLY_ADULT] = Area("Early Adult Spirit Temple", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_COMPASS_CHEST,            {[]{return logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,  {[]{return (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasBombchus || (logic->Bombs && logic->IsAdult && randoCtx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH))) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanJumpslash);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,          {[]{return logic->CanUse(RG_SONG_OF_TIME) && (logic->Bow || logic->Hookshot || logic->HasBombchus || (logic->Bombs && randoCtx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER] = Area("Spirit Temple Central Chamber", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MAP_CHEST,                    {[]{return ((logic->HasExplosives || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&

                                                                                                (logic->CanUse(RG_DINS_FIRE) ||
                                                                                                    (((logic->MagicMeter && logic->FireArrows) || randoCtx->GetTrickOption(RT_SPIRIT_MAP_CHEST)) && logic->Bow && logic->Sticks))) ||
                                                                                            (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives &&
                                                                                                logic->CanUse(RG_STICKS)) ||
                                                                                            (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) &&
                                                                                                (logic->CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MAP_CHEST) && logic->Bow)) &&
                                                                                                logic->CanUse(RG_SILVER_GAUNTLETS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,         {[]{return ((logic->HasExplosives || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                                                (logic->CanUse(RG_DINS_FIRE) ||
                                                                                                    (((logic->MagicMeter && logic->FireArrows) || randoCtx->GetTrickOption(RT_SPIRIT_SUN_CHEST)) && logic->Bow && logic->Sticks))) ||
                                                                                            (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives &&
                                                                                                logic->CanUse(RG_STICKS)) ||
                                                                                            (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) &&
                                                                                                (logic->CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_SUN_CHEST) && logic->Bow)) &&
                                                                                                logic->CanUse(RG_SILVER_GAUNTLETS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,       {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,  {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanUse(RG_ZELDAS_LULLABY) && (logic->Hookshot || logic->HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, {[]{return (logic->HasExplosives && logic->Boomerang && logic->Hookshot) ||
                                                                                            (logic->CanUse(RG_BOOMERANG) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives) ||
                                                                                            (logic->Hookshot && logic->CanUse(RG_SILVER_GAUNTLETS) &&
                                                                                                (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) ||
                                                                                                    (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && logic->Boomerang && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_GS_LOBBY,                     {[]{return ((logic->HasExplosives || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && randoCtx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                                                randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && logic->Boomerang && (logic->Hookshot || logic->HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP))) ||
                                                                                            (randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives && logic->CanUse(RG_BOOMERANG)) ||
                                                                                            (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->Hookshot || logic->HoverBoots || randoCtx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP)));}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_OUTDOOR_HANDS,              {[]{return logic->CanJumpslash || logic->HasExplosives;}}),
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB,                {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_OUTDOOR_HANDS] = Area("Spirit Temple Outdoor Hands", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, {[]{return (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->Longshot && logic->HasExplosives) || logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,    {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return (logic->IsChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)) || (logic->CanUse(RG_SILVER_GAUNTLETS) && ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->HasExplosives) || logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Central Locked Door", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,         {[]{return (logic->MirrorShield || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasExplosives;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives;}}),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && (randoCtx->GetTrickOption(RT_SPIRIT_WALL) || logic->CanUse(RG_LONGSHOT) || logic->HasBombchus || ((logic->Bombs || logic->Nuts || logic->CanUse(RG_DINS_FIRE)) && (logic->Bow || logic->CanUse(RG_HOOKSHOT) || logic->Hammer)));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR] = Area("Spirit Temple Beyond Final Locked Door", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) && ((logic->CanTakeDamage && randoCtx->GetTrickOption(RT_FLAMING_CHESTS)) || (logic->Bow && logic->Hookshot));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_TOPMOST_CHEST,  {[]{return (logic->MirrorShield && logic->CanAdultAttack) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD, {[]{return logic->MirrorShield && logic->HasExplosives && logic->Hookshot;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD] =
            Area("Spirit Temple Inside Statue Head", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return logic->BossKeySpiritTemple; } }),
                 });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(SPIRIT_TEMPLE)->IsMQ()) {
  areaTable[RR_SPIRIT_TEMPLE_MQ_LOBBY] = Area("Spirit Temple MQ Lobby", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,  {[]{return Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return logic->CanBlastOrSmash;}) && ((logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)) || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, {[]{return logic->HasBombchus || (logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT))) || (logic->IsChild && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)));}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_CHILD, {[]{return logic->IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_ADULT, {[]{return logic->HasBombchus && logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_CHILD] = Area("Spirit Temple MQ Child", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->KokiriSword && logic->HasBombchus && logic->Slingshot);}}),
  }, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,  {[]{return Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->Hammer;});}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,       {[]{return logic->KokiriSword && logic->HasBombchus && logic->Slingshot && logic->CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST,                  {[]{return logic->KokiriSword || logic->Bombs;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, {[]{return logic->HasBombchus && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->Slingshot && (logic->CanUse(RG_DINS_FIRE) || (Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MQ_FROZEN_EYE) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME)));})));}}),
                    //Trick: logic->HasBombchus && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->Slingshot && (logic->CanUse(RG_DINS_FIRE) || (SPIRIT_TEMPLE_MQ_ADULT.Adult() && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQFrozenEye && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME)))))
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, {[]{return logic->HasBombchus && logic->SmallKeys(RR_SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_ADULT] = Area("Spirit Temple MQ Adult", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,     {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   {[]{return logic->CanUse(RG_ZELDAS_LULLABY) && (logic->CanJumpslash || logic->CanUse(RG_HOVER_BOOTS));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, {[]{return (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,           {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,          {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,              {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME) && (logic->MirrorShield || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)));}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT,        {[]{return logic->MirrorShield && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (randoCtx->GetTrickOption(RT_SPIRIT_MQ_LOWER_ADULT) && logic->CanUse(RG_DINS_FIRE) && logic->Bow));}}),
                    //Trick: logic->MirrorShield && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQLowerAdult && logic->CanUse(RG_DINS_FIRE) && logic->Bow))
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED,             {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BOSS_AREA,          {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6) && logic->CanUse(RG_ZELDAS_LULLABY) && logic->Hammer;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SHARED] = Area("Spirit Temple MQ Shared", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,           {[]{return (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)) || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || (logic->Bow && logic->Slingshot);}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,    {[]{return logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult;}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,       {[]{return (randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_GS) && logic->Boomerang && (logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT))) || logic->IsAdult;}}),
                    //Trick: (LogicSpiritMQSunBlockGS && logic->Boomerang && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT)) || logic->IsAdult
   }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)));}}),
                    //Trick: (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || logic->CanUse(RG_LENS_OF_TRUTH)))
                  Entrance(RR_DESERT_COLOSSUS,                        {[]{return (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslash && (randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->IsAdult);}}),
                    //Trick: (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->IsAdult)
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT] = Area("Spirit Temple MQ Lower Adult", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,          {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,        {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->Hammer && logic->Ocarina && logic->SongOfTime && logic->EponasSong && logic->SunsSong && logic->SongOfStorms && logic->ZeldasLullaby;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, {[]{return logic->Hammer;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,             {[]{return true;}}),
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,           {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->Hammer && logic->Ocarina && logic->SongOfTime && logic->EponasSong && logic->SunsSong && logic->SongOfStorms && logic->ZeldasLullaby;}}),
  }, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_BOSS_AREA] = Area("Spirit Temple MQ Boss Area", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, {[]{return randoCtx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, {[]{return logic->MirrorShield && logic->Hookshot;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD] =
            Area("Spirit Temple MQ Inside Statue Head", "Spirit Temple", RA_SPIRIT_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return logic->BossKeySpiritTemple; } }),
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
            EventAccess(&logic->SpiritTempleClear, { [] {
                return logic->SpiritTempleClear || (logic->HasBossSoul(RG_TWINROVA_SOUL) && (logic->CanUse(RG_MIRROR_SHIELD) &&
                                             (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))));
            } }),
        },
        {
            // Locations
            LocationAccess(RC_SPIRIT_TEMPLE_TWINROVA_HEART, { [] { return logic->SpiritTempleClear; } }),
            LocationAccess(RC_TWINROVA, { [] { return logic->SpiritTempleClear; } }),
        },
        {
            // Exits
            Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_DESERT_COLOSSUS, { [] { return logic->SpiritTempleClear; } }),
        });
}
