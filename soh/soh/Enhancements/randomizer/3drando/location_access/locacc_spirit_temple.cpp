#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_SpiritTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_SPIRIT_TEMPLE_ENTRYWAY] = Region("Spirit Temple Entryway", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_LOBBY,                  {[]{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,               {[]{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ();}}),
                  Entrance(RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla()) {
  areaTable[RR_SPIRIT_TEMPLE_LOBBY] = Region("Spirit Temple Lobby", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,    {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_CHILD,       {[]{return logic->IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_EARLY_ADULT, {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD] = Region("Child Spirit Temple", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutCrate, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,        (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives() || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_STICKS)  || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT))))),
                  LOCATION(RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives() || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_STICKS)  || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT)))) && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_DINS_FIRE))),
                  LOCATION(RC_SPIRIT_TEMPLE_GS_METAL_FENCE,            (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_FAIRY_SLINGSHOT) || (logic->CanUse(RG_BOMBCHU_5) && ctx->GetTrickOption(RT_SPIRIT_CHILD_CHU))) && (logic->HasExplosives() || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_STICKS)  || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_FAIRY_SLINGSHOT))))),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CHILD_CLIMB] = Region("Child Spirit Temple Climb", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, logic->HasProjectile(HasProjectileAge::Both) || ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasProjectile(HasProjectileAge::Adult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->IsChild && logic->HasProjectile(HasProjectileAge::Child))),
                  LOCATION(RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,  logic->HasProjectile(HasProjectileAge::Both) || ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasProjectile(HasProjectileAge::Adult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->IsChild && logic->HasProjectile(HasProjectileAge::Child))),
                  LOCATION(RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,    logic->HasProjectile(HasProjectileAge::Both) || logic->CanUse(RG_DINS_FIRE) ||
                                                                                      (logic->TakeDamage() && (logic->CanJumpslashExceptHammer() || logic->HasProjectile(HasProjectileAge::Child))) ||
                                                                                        (logic->IsChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasProjectile(HasProjectileAge::Child)) ||
                                                                                          ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) && logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->HasProjectile(HasProjectileAge::Adult) || (logic->TakeDamage() && logic->CanJumpslashExceptHammer())))),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return logic->HasExplosives() || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_EARLY_ADULT] = Region("Early Adult Spirit Temple", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_COMPASS_CHEST,            logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,  (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_BOMB_BAG) && logic->IsAdult && ctx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH))) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanJumpslashExceptHammer())),
                  LOCATION(RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,  logic->SmallKeys(RR_SPIRIT_TEMPLE, 3)),
                  LOCATION(RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, logic->SmallKeys(RR_SPIRIT_TEMPLE, 3)),
                  LOCATION(RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,          logic->CanUse(RG_SONG_OF_TIME) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_BOMBCHU_5) || (logic->CanUse(RG_BOMB_BAG) && ctx->GetTrickOption(RT_SPIRIT_LOWER_ADULT_SWITCH)))),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER] = Region("Spirit Temple Central Chamber", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MAP_CHEST,                    ((logic->HasExplosives() || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                            (logic->CanUse(RG_DINS_FIRE) || ((logic->CanUse(RG_FIRE_ARROWS) || ctx->GetTrickOption(RT_SPIRIT_MAP_CHEST)) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_STICKS) ))) ||
                                                                          (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives() && logic->CanUse(RG_STICKS)) ||
                                                                          (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && (logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_MAP_CHEST) && logic->CanUse(RG_FAIRY_BOW))) && logic->CanUse(RG_SILVER_GAUNTLETS))),
                  
                  LOCATION(RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,         ((logic->HasExplosives() || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                           (logic->CanUse(RG_DINS_FIRE) || ((logic->CanUse(RG_FIRE_ARROWS) || ctx->GetTrickOption(RT_SPIRIT_SUN_CHEST)) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_STICKS) ))) ||
                                                                          (logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives() && logic->CanUse(RG_STICKS)) ||
                                                                          (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && (logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_SUN_CHEST) && logic->CanUse(RG_FAIRY_BOW))) && logic->CanUse(RG_SILVER_GAUNTLETS))),

                  LOCATION(RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,       logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,  logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanUse(RG_ZELDAS_LULLABY) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP))),
                  LOCATION(RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, (logic->HasExplosives() && logic->CanUse(RG_BOOMERANG) && logic->CanUse(RG_HOOKSHOT)) ||
                                                                          (logic->CanUse(RG_BOOMERANG) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives()) ||
                                                                          (logic->CanUse(RG_HOOKSHOT) && logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && 
                                                                           logic->CanUse(RG_BOOMERANG) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))))),

                  LOCATION(RC_SPIRIT_TEMPLE_GS_LOBBY,                     ((logic->HasExplosives() || logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 2) && ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && logic->BombchuRefill() && ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) &&
                                                                           ctx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && logic->CanUse(RG_BOOMERANG) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP))) ||
                                                                          (ctx->GetTrickOption(RT_SPIRIT_LOBBY_GS) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->HasExplosives() && logic->CanUse(RG_BOOMERANG)) ||
                                                                          (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_SILVER_GAUNTLETS) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_SPIRIT_LOBBY_JUMP)))),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_OUTDOOR_HANDS,              {[]{return logic->CanJumpslashExceptHammer() || logic->HasExplosives();}}),
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_SPIRIT_TEMPLE_CHILD_CLIMB,                {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_OUTDOOR_HANDS] = Region("Spirit Temple Outdoor Hands", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, (logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->CanUse(RG_LONGSHOT) && logic->HasExplosives()) || logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)),
                  LOCATION(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,    logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SILVER_GAUNTLETS) && logic->HasExplosives()),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return (logic->IsChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)) || (logic->CanUse(RG_SILVER_GAUNTLETS) && ((logic->SmallKeys(RR_SPIRIT_TEMPLE, 3) && logic->HasExplosives()) || logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_CENTRAL_LOCKED_DOOR] = Region("Spirit Temple Beyond Central Locked Door", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,         (logic->CanUse(RG_MIRROR_SHIELD) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))) && logic->HasExplosives()),
                  LOCATION(RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,  (ctx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives()),
                  LOCATION(RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, (ctx->GetTrickOption(RT_LENS_SPIRIT) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->HasExplosives()),
                }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && (ctx->GetTrickOption(RT_SPIRIT_WALL) || logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_BOMBCHU_5) || ((logic->CanUse(RG_BOMB_BAG) || logic->CanUse(RG_NUTS) || logic->CanUse(RG_DINS_FIRE)) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_MEGATON_HAMMER))));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_BEYOND_FINAL_LOCKED_DOOR] = Region("Spirit Temple Beyond Final Locked Door", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY) && ((logic->TakeDamage() && ctx->GetTrickOption(RT_FLAMING_CHESTS)) || (logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_HOOKSHOT)))),
                  LOCATION(RC_SPIRIT_TEMPLE_TOPMOST_CHEST,  (logic->CanUse(RG_MIRROR_SHIELD) && logic->CanAttack()) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD, {[]{return logic->CanUse(RG_MIRROR_SHIELD) && logic->HasExplosives() && logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD] =
            Region("Spirit Temple Inside Statue Head", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_CENTRAL_CHAMBER, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return logic->HasItem(RG_SPIRIT_TEMPLE_BOSS_KEY); } }),
                 });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ()) {
  areaTable[RR_SPIRIT_TEMPLE_MQ_LOBBY] = Region("Spirit Temple MQ Lobby", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, true),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,  Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return logic->BlastOrSmash();}) && ((logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)) || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, logic->CanUse(RG_BOMBCHU_5) || (logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT))) || (logic->IsChild && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG)))),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_CHILD, {[]{return logic->IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_ADULT, {[]{return logic->CanUse(RG_BOMBCHU_5) && logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_CHILD] = Region("Spirit Temple MQ Child", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_FAIRY_SLINGSHOT));}}),
  }, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,  Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanUse(RG_MEGATON_HAMMER);})),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,       logic->CanUse(RG_KOKIRI_SWORD) && logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST,                  logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BOMB_BAG)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, logic->CanUse(RG_BOMBCHU_5) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanUse(RG_FAIRY_SLINGSHOT) && (logic->CanUse(RG_DINS_FIRE) || (Here(RR_SPIRIT_TEMPLE_MQ_ADULT, []{return logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_MQ_FROZEN_EYE) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME)));})))),
                    //Trick: logic->CanUse(RG_BOMBCHU_5) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanUse(RG_FAIRY_SLINGSHOT) && (logic->CanUse(RG_DINS_FIRE) || (SPIRIT_TEMPLE_MQ_ADULT.Adult() && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQFrozenEye && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME)))))
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, {[]{return logic->CanUse(RG_BOMBCHU_5) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_ADULT] = Region("Spirit Temple MQ Adult", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,     logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   logic->CanUse(RG_ZELDAS_LULLABY) && (logic->CanJumpslashExceptHammer() || logic->CanUse(RG_HOVER_BOOTS))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,           logic->SmallKeys(RR_SPIRIT_TEMPLE, 5)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,          logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,              logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME) && (logic->CanUse(RG_MIRROR_SHIELD) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT,        {[]{return logic->CanUse(RG_MIRROR_SHIELD) && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_MQ_LOWER_ADULT) && logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_FAIRY_BOW)));}}),
                    //Trick: logic->CanUse(RG_MIRROR_SHIELD) && logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || (LogicSpiritMQLowerAdult && logic->CanUse(RG_DINS_FIRE) && logic->CanUse(RG_FAIRY_BOW)))
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED,             {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BOSS_AREA,          {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6) && logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslashExceptHammer() && (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SHARED] = Region("Spirit Temple MQ Shared", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, logic->SmallKeys(RR_SPIRIT_TEMPLE, 6)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,           (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7)) || (logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)) || (logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_FAIRY_SLINGSHOT))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,    logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,       (ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_GS) && logic->CanUse(RG_BOOMERANG) && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT))) || logic->IsAdult),
                    //Trick: (LogicSpiritMQSunBlockGS && logic->CanUse(RG_BOOMERANG) && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT)) || logic->IsAdult
   }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslashExceptHammer() && (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)));}}),
                    //Trick: (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || logic->CanUse(RG_LENS_OF_TRUTH)))
                  Entrance(RR_DESERT_COLOSSUS,                        {[]{return (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanJumpslashExceptHammer() && (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->IsAdult);}}),
                    //Trick: (logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && (logic->CanUse(RG_SONG_OF_TIME) || LogicSpiritMQSunBlockSoT || logic->IsAdult)) || (logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) && logic->CanUse(RG_SONG_OF_TIME) && (LogicLensSpiritMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->IsAdult)
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_LOWER_ADULT] = Region("Spirit Temple MQ Lower Adult", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,          true),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,        logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_SUNS_SONG)
                                                                           && logic->CanUse(RG_SONG_OF_STORMS) && logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, logic->CanUse(RG_MEGATON_HAMMER)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,             true),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,           logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_SUNS_SONG)
                                                                           && logic->CanUse(RG_SONG_OF_STORMS) && logic->CanUse(RG_ZELDAS_LULLABY)),
  }, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_BOSS_AREA] = Region("Spirit Temple MQ Boss Region", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, {[]{return logic->CanUse(RG_MIRROR_SHIELD) && logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD] =
            Region("Spirit Temple MQ Inside Statue Head", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_MQ_SHARED, { [] { return true; } }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return logic->HasItem(RG_SPIRIT_TEMPLE_BOSS_KEY); } }),
                 });

  areaTable[RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND] = Region("Spirit Temple MQ Mirror Shield Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, true),
  }, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND] = Region("Spirit Temple MQ Silver Gauntlets Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, true),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY] = Region(
        "Spirit Temple Boss Entryway", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
        {
            // Exits
            Entrance(RR_SPIRIT_TEMPLE_INSIDE_STATUE_HEAD, { [] { return ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla() && false; } }),
            Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, { [] { return ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ() && false; } }),
            Entrance(RR_SPIRIT_TEMPLE_BOSS_ROOM, { [] { return true; } }),
        });

    areaTable[RR_SPIRIT_TEMPLE_BOSS_ROOM] = Region("Spirit Temple Boss Room", "Spirit Temple", {}, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&logic->SpiritTempleClear, { [] {
                return logic->SpiritTempleClear || (logic->HasBossSoul(RG_TWINROVA_SOUL) && (logic->CanUse(RG_MIRROR_SHIELD) &&
                                             (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))));
            } }),
        },
        {
            // Locations
            LOCATION(RC_SPIRIT_TEMPLE_TWINROVA_HEART, logic->SpiritTempleClear),
            LOCATION(RC_TWINROVA, logic->SpiritTempleClear),
        },
        {
            // Exits
            Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
            Entrance(RR_DESERT_COLOSSUS, { [] { return logic->SpiritTempleClear; } }, false),
        });
}
