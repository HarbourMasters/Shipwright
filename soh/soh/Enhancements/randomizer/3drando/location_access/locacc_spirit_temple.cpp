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
                  Entrance(RR_SPIRIT_TEMPLE_LOBBY,            {[]{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsVanilla();}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,         {[]{return ctx->GetDungeon(SPIRIT_TEMPLE)->IsMQ();}}),
                  Entrance(RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE, {[]{return true;}}),
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
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST,  true),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,   Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return logic->BlastOrSmash();}) && logic->CanHitEyeTargets()),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST,  logic->CanHitSwitch(ED_BOOMERANG)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, logic->Spirit1FSilverRupees),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,                {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,              {[]{return logic->IsChild;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH, {[]{return logic->CanUse(RG_LONGSHOT) && logic->CanUse(RG_BOMBCHU_5);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_1F_WEST] = Region("Spirit Temple MQ 1F West", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //not technically a rusted switch, but a boulder through a wall, but is part of the same trick on N64
                  EventAccess(&logic->MQSpiritCrawlBoulder, {[]{return logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && logic->CanUse(RG_MEGATON_HAMMER));}}),
  }, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, logic->MQSpiritTimeTravelChest),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_SOUTH,   {[]{return Here(RR_SPIRIT_TEMPLE_MQ_1F_WEST, []{return logic->CanKillEnemy(RE_TORCH_SLUG);});}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH,        {[]{return Here(RR_SPIRIT_TEMPLE_MQ_1F_WEST, []{return logic->CanKillEnemy(RE_TORCH_SLUG);});}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH, {[]{return logic->IsChild && logic->MQSpiritCrawlBoulder;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_SOUTH] = Region("Spirit Temple MQ 1F Gibdo Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,             {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH, {[]{return logic->CanUse(RG_BOMBCHU_5) && logic->CanHitEyeTargets();}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM,      {[]{return logic->CanUse(RG_BOMBCHU_5) && logic->CanHitEyeTargets() && logic->CanKillEnemy(RE_GIBDO);}}),
  });

// Room to store the 2 pots in to handle glitch logic going backwards around the loop later
  areaTable[RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH] = Region("Spirit Temple MQ Gibdo Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {});

  areaTable[RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM] = Region("Spirit Temple Turntable Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //For non-fairy pot items, you can also get them with rang without killing the stalfos
                  EventAccess(&logic->FairyPot, {[]{return Here(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}}),
  }, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_GIBDO_ROOM_NORTH, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH,      {[]{return Here(RR_SPIRIT_TEMPLE_MQ_TURNTABLE_ROOM, []{return logic->CanKillEnemy(RE_STALFOS);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH] = Region("Spirit Temple MQ Map Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQSpiritMapRoomEnemies, {[]{return logic->CanKillEnemy(RE_ANUBIS) && logic->CanKillEnemy(RE_KEESE);}}),
  }, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,  logic->MQSpiritMapRoomEnemies),
  }, {
                  //Exits
                  //Stalfos room blocks you in with fire until you kill the stalfos, which won't spawn from behind the fire
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH, {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_SOUTH] = Region("Spirit Temple MQ Map Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //You can lure the keese over by aggroing them with dins if you use it as close to the torch keese as possible, but it's a trick as it's not intuitive and basically never comes up
                  EventAccess(&logic->MQSpiritMapRoomEnemies, {[]{return logic->CanKillEnemy(RE_ANUBIS) && logic->CanKillEnemy(RE_KEESE, ED_BOOMERANG);}}),
  }, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, true),
  }, {
                  //Exits
                  //The bridge is a temp flag, so not a way to cross south to north in logic
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MAP_ROOM_NORTH,  {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST, {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH] = Region("Spirit Temple MQ West 1F Rusted Switch", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->MQSpiritTimeTravelChest, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  EventAccess(&logic->MQSpiritCrawlBoulder,    {[]{return logic->CanUse(RG_BOMBCHU_5) || (ctx->GetTrickOption(RT_RUSTED_SWITCHES) && logic->CanUse(RG_MEGATON_HAMMER));}}),
  }, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_WEST,         {[]{return logic->IsChild && logic->MQSpiritCrawlBoulder;}}),
                  //This tracks possible child access, if adult has not entered STATUE_ROOM. Certain Child Access is checked for separately as 7 Keys
                  Entrance(RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 1);}}),
  });

//THIS REGION NEEDS MQSpiritSharedBrokenWallRoom WHEN ADDING THE POT
  areaTable[RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE] = Region("Spirit Temple MQ Under Like Like", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //This covers adult access only, as child arrives here from the other side of this door
                  Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_1F_RUSTED_SWITCH, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM,      {[]{return logic->CanHitSwitch();}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM] = Region("Spirit Temple MQ Broken Wall Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //Implies CanKillEnemy(RE_LIKE_LIKE)
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, MQSpiritSharedBrokenWallRoom(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM, []{return logic->CanKillEnemy(RE_BEAMOS);})),
                  //Sunlights only temp spawn this chest, which is unintuitive/a bug.
                  //chest is only reachable as adult glitchlessly, so we can skip the shared in favour of IsAdult as adult access is always Certain
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST, logic->IsAdult && logic->HasExplosives() && (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)) && logic->CanUse(RG_HOOKSHOT)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, {[]{return logic->CanHitSwitch();}}),
                  //This exit only governs child possible access, adult access starts on the other side so never checks this
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,     {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 2);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM] = Region("Spirit Temple MQ Statue Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST, MQSpiritSharedStatueRoom(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->CanHitEyeTargets();})),
  }, {
                  //Exits
                  //we check possible adult access directly in MQSpiritSharedBrokenWallRoom, so this exit only covers Certain Access
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BROKEN_WALL_ROOM,     {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  //We can use Here instead of Shared here because adult will never need to rely on child access to reach this room, and adult access is Certain
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH, {[]{return Here(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, []{return logic->HasFireSource() || (ctx->GetTrickOption(RT_SPIRIT_MQ_FROZEN_EYE) && logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME));});}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM,       {[]{return logic->IsAdult || ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_SOT) || logic->CanUse(RG_SONG_OF_TIME);}}),
                  //explicit adult check here is a precaution against possible child logic leaking, child with a hookshot can do this
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,     {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM] = Region("Spirit Temple MQ Sun Block Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //We don't need Shared here because If we are checking as child, universe 2 adult access needs nothing so it always passes, and if we are checking as adult, it is Certain Access
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, true),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,    MQSpiritSharedStatueRoom(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, []{return logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_SPIRIT_MQ_SUN_BLOCK_GS) && logic->CanUse(RG_BOOMERANG));})),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,       {[]{return true;}}),
                  //This door causes the Universes to merge as it requires 7 keys for both ages
                  Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_IRON_KNUCKLE, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_WEST_IRON_KNUCKLE] = Region("Spirit Temple MQ East Iron Knuckle", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM,        {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return logic->CanKillEnemy(RE_IRON_KNUCKLE);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND] = Region("Spirit Temple MQ Silver Gauntlets Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, true),
  }, {
                  //Exits
                  //If it is ever relevent for 1 age to spawn the mirror shield chest for the other can longshot across, it needs an eventAccess
                  Entrance(RR_SPIRIT_TEMPLE_MQ_WEST_IRON_KNUCKLE, {[]{return true;}}),
                  Entrance(RR_DESERT_COLOSSUS,                    {[]{return true;}}),
  });
  
  areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH] = Region("Spirit Temple MQ Block Room South", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY, {[]{return true;}}),
                  //The block here is unusual in that it is a permanent flag, but reset anyway as child. This is because there's a check that would be blocked off by pushing them otherwise
                  //It may be worth considering making this always temp in future so adult doesn't have the same issue
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH, {[]{return logic->IsChild ? logic->CanUse(RG_SILVER_GAUNTLETS) : Here(RR_SPIRIT_TEMPLE_MQ_LOBBY, []{return logic->CanUse(RG_SILVER_GAUNTLETS);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_NORTH] = Region("Spirit Temple MQ Block Room North", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //Does not need to be shared as it's hard child locked, because adult pushing the block is a permanent flag that blocks the eye target and cannot be undone
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, logic->IsChild && logic->SmallKeys(RR_SPIRIT_TEMPLE, 7) && logic->CanHitEyeTargets()),
  }, {
                  //Exits
                  //if going to RR_SPIRIT_TEMPLE_MQ_BIG_BLOCK_ROOM_SOUTH from here is ever relevant, there needs to be an event to handle the block
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST] = Region("Spirit Temple MQ Statue Room East", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,   logic->CanUse(RG_HOOKSHOT) & logic->CanUse(RG_ZELDAS_LULLABY) && (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS))),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS))),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,           {[]{return true;}}),
                  //!QUANTUM LOGIC!
                  //We only need 4 keys, access to Shield hand and longshot to reach Gauntlets hand, as if we waste the 5th key we have given ourselves Gauntlets hand access through child climb
                  //This exit handles that possibility as cleanly as possible without quantum logic, but will not survive glitch logic
                  //logic->CanKillEnemy(RE_FLOORMASTER) is implied
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 4) &&
                                                                                 logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME) &&
                                                                                 logic->CanJumpslash() && 
                                                                                 (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                                 logic->CanKillEnemy(RE_IRON_KNUCKLE) && 
                                                                                 logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_FOUR_BEAMOS_ROOM,      {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F,    {[]{return logic->CanUse(RG_FIRE_ARROWS) || (ctx->GetTrickOption(RT_SPIRIT_MQ_LOWER_ADULT) && logic->CanUse(RG_DINS_FIRE));}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F] = Region("Spirit Temple MQ Three Suns Room 2F", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //implies logic->CanKillEnemy(RE_WALLMASTER). If we have lights, we can kill stalfos and wallmasters with bow
                  EventAccess(&logic->MQSpirit3SunsEnemies, {[]{return (logic->CanUse(RG_MIRROR_SHIELD) && logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)) || 
                                                                       (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}}),
  }, {}, {
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,   {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F, {[]{return logic->MQSpirit3SunsEnemies;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F] = Region("Spirit Temple MQ Three Suns Room 1F", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_2F, {[]{return logic->MQSpirit3SunsEnemies;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST,            {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_1F_EAST] = Region("Spirit Temple MQ 1F East", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //Assumes RR_SPIRIT_TEMPLE_MQ_LOBBY access
                  EventAccess(&logic->Spirit1FSilverRupees, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,              {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_THREE_SUNS_ROOM_1F, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_LEEVER_ROOM,        {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM,      {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_LEEVER_ROOM] = Region("Spirit Temple MQ Leever Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, logic->CanKillEnemy(RE_PURPLE_LEEVER) && logic->CanUse(RG_HOOKSHOT)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,    logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
  }, {
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM] = Region("Spirit Temple MQ Symphony Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_1F_EAST,             {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  //Implies CanPassEnemy(RE_MOBLIN_CHIEF)
                  Entrance(RR_SPIRIT_TEMPLE_MQ_AFTER_SYMPHONY_ROOM, {[]{return logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_EPONAS_SONG) &&
                                                                               logic->CanUse(RG_SUNS_SONG) && logic->CanUse(RG_SONG_OF_STORMS) && logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_AFTER_SYMPHONY_ROOM] = Region("Spirit Temple MQ After Symphony Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,    logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM, {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_FOUR_BEAMOS_ROOM] = Region("Spirit Temple MQ Four Beamos Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,  logic->CanKillEnemy(RE_BEAMOS)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_EAST,   {[]{return logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME) && logic->SmallKeys(RR_SPIRIT_TEMPLE, 5) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,       {[]{return logic->CanAvoidEnemy(RE_BEAMOS, true, 4) && logic->CanUse(RG_SONG_OF_TIME);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL,           {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 6);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM] = Region("Spirit Temple MQ SoT Sun Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_FOUR_BEAMOS_ROOM,    {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_STAIRS_TO_HAND, {[]{return logic->CanJumpslash();}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM,       {[]{return Here(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, []{return (logic->IsAdult || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanUse(RG_MIRROR_SHIELD);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_EAST_STAIRS_TO_HAND] = Region("Spirit Temple MQ East Stairs to Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,      {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_IRON_KNUCKLE, {[]{return (ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && 
                                                                             Here(RR_SPIRIT_TEMPLE_MQ_EAST_STAIRS_TO_HAND, []{return logic->CanKillEnemy(RE_FLOORMASTER);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_EAST_IRON_KNUCKLE] = Region("Spirit Temple MQ East Iron Knuckle", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_STAIRS_TO_HAND, {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND,  {[]{return Here(RR_SPIRIT_TEMPLE_MQ_EAST_STAIRS_TO_HAND, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_MIRROR_SHIELD_HAND] = Region("Spirit Temple MQ Mirror Shield Hand", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, true),
  }, { 
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SILVER_GAUNTLETS_HAND, {[]{return logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_EAST_IRON_KNUCKLE,     {[]{return true;}}),
                  Entrance(RR_DESERT_COLOSSUS,                        {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM] = Region("Spirit Temple MQ 3F Gibdo Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,    {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_WALL] = Region("Spirit Temple MQ Big Wall", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_FOUR_BEAMOS_ROOM, {[]{return true;}}),
                  //technically we only need to avoid them, but the sheer height and the moving walls makes getting to the top after only stunning them very difficult/impossible
                  //The silver rupees are irrelevant without silver shuffle
                  Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,       {[]{return logic->CanKillEnemy(RE_KEESE);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL] = Region("Spirit Temple MQ 4F Central", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_WALL,         {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_NINE_CHAIRS_ROOM, {[]{return logic->SmallKeys(RR_SPIRIT_TEMPLE, 7);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM,  {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_NINE_CHAIRS_ROOM] = Region("Spirit Temple MQ Nine Chairs Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //These skulls rely on the iron knuckle existing without a trick to shoot through the chairs
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,   logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,  logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA)),
  }, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL, {[]{return true;}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM] = Region("Spirit Temple MQ Big Mirror Room", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,      {[]{return true;}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE, {[]{return Here(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM, []{return logic->CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE] = Region("Spirit Temple MQ Big Mirror Cave", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SPIRIT_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
  }, {
                  //Exits
                  //If it's ever relevant to longshot into head from lobby, this needs to be an event access
                  Entrance(RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD, {[]{return Here(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE, []{return logic->CanUse(RG_MIRROR_SHIELD);}) && logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,        {[]{return Here(RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE, []{return logic->CanUse(RG_MIRROR_SHIELD);});}}),
  });

  areaTable[RR_SPIRIT_TEMPLE_MQ_INSIDE_STATUE_HEAD] = Region("Spirit Temple MQ Inside Statue Head", "Spirit Temple", {RA_SPIRIT_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                     // Exits
                     Entrance(RR_SPIRIT_TEMPLE_MQ_LOBBY,      {[]{return true;} }),
                     Entrance(RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, {[]{return logic->HasItem(RG_SPIRIT_TEMPLE_BOSS_KEY);}}),
                 });
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
