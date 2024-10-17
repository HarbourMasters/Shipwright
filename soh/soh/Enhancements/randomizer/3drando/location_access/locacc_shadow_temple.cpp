#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void RegionTable_Init_ShadowTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_SHADOW_TEMPLE_ENTRYWAY] = Region("Shadow Temple Entryway", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEGINNING,    {[]{return ctx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && (ctx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEGINNING, {[]{return ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ()    && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION,  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(SHADOW_TEMPLE)->IsVanilla()) {
  areaTable[RR_SHADOW_TEMPLE_BEGINNING] = Region("Shadow Temple Beginning", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MAP_CHEST,         logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_FIRST_BEAMOS, {[]{return logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_FIRST_BEAMOS] = Region("Shadow Temple First Beamos", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}), //This fairy pot is only on 3DS
                }, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_COMPASS_CHEST,            logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)),
                  LOCATION(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,             false),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_HUGE_PIT,    {[]{return logic->HasExplosives() && logic->IsAdult && logic->SmallKeys(RR_SHADOW_TEMPLE, 1, 2);}}),
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return false;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_HUGE_PIT] = Region("Shadow Temple Huge Pit", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,       true),
                  LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,       (ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS)) || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,      (ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS)) || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,           logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SHADOW_TEMPLE_FREESTANDING_KEY,                 logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_BOMB_BAG) || logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_SHADOW_FREESTANDING_KEY) && logic->CanUse(RG_BOMBCHU_5)))),
                  LOCATION(RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,           logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->CanUse(RG_HOVER_BOOTS))),
                  LOCATION(RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,              logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOOKSHOT)),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_WIND_TUNNEL, {[]{return ((ctx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && ctx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_SHADOW_TEMPLE, 3, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_WIND_TUNNEL] = Region("Shadow Temple Wind Tunnel", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_WIND_HINT_CHEST,         true),
                  LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, true),
                  LOCATION(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,            logic->CanUse(RG_LONGSHOT) && logic->SmallKeys(RR_SHADOW_TEMPLE, 4, 5)),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return logic->CanJumpslashExceptHammer() && logic->CanUse(RG_ZELDAS_LULLABY) && logic->SmallKeys(RR_SHADOW_TEMPLE, 4, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_BEYOND_BOAT] = Region("Shadow Temple Beyond Boat", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,      logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,              logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, logic->CanJumpslashExceptHammer()),
                  //RANDOTODO check if child can reach the token
                  LOCATION(RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,         logic->IsAdult && logic->CanAttack()),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, {[]{return (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DISTANT_SCARECROW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5))) && logic->SmallKeys(RR_SHADOW_TEMPLE, 5) && logic->CanUse(RG_HOVER_BOOTS) && logic->HasItem(RG_SHADOW_TEMPLE_BOSS_KEY);}})
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ()) {
    //RANDOTODO doublecheck CanAttack when rewriting, as I assumed it only checked adult due to the entrance
  areaTable[RR_SHADOW_TEMPLE_MQ_BEGINNING] = Region("Shadow Temple MQ Beginning", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_SHADOW_MQ_GAP) && logic->CanUse(RG_LONGSHOT)));}}),
                    //Trick: logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS) || (LogicShadowMQGap && logic->CanUse(RG_LONGSHOT)))
                  Entrance(RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return logic->HasExplosives() && logic->SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Region("Shadow Temple MQ Dead Hand Region", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,     logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, logic->CanJumpslashExceptHammer() && logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult && logic->CanUse(RG_FAIRY_BOW)),
  }, {});

  areaTable[RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Region("Shadow Temple MQ First Beamos", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_MAP_CHEST,                 logic->CanAttack() || logic->CanUse(RG_NUTS)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,        logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, logic->CanAttack() || logic->CanUse(RG_NUTS)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return logic->HasExplosives() && logic->SmallKeys(RR_SHADOW_TEMPLE, 2);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Region("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO))),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && ctx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO))),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return logic->HasFireSource() || ctx->GetTrickOption(RT_SHADOW_MQ_HUGE_PIT);}}),
                    //Trick: logic->HasFireSource() || LogicShadowMQHugePit
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Region("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,  logic->IsAdult && logic->CanUse(RG_LONGSHOT)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  (ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS)) || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, (ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS)) || logic->HasItem(RG_GORONS_BRACELET)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,      logic->CanJumpslashExceptHammer() && logic->CanUse(RG_HOVER_BOOTS) && logic->SmallKeys(RR_SHADOW_TEMPLE, 3) && ((ctx->GetTrickOption(RT_LENS_SHADOW_MQ) && ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,          (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->CanUse(RG_HOVER_BOOTS) && logic->SmallKeys(RR_SHADOW_TEMPLE, 3) && logic->CanUse(RG_HOOKSHOT) && ((ctx->GetTrickOption(RT_LENS_SHADOW_MQ) &&
                   ctx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) && ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      logic->CanUse(RG_HOOKSHOT) || (ctx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->CanUse(RG_HOVER_BOOTS))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return logic->CanUse(RG_HOVER_BOOTS) && ((ctx->GetTrickOption(RT_LENS_SHADOW_MQ) && ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_SHADOW_TEMPLE, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Region("Shadow Temple MQ Wind Tunnel", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,         true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,       true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           true),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) && logic->SmallKeys(RR_SHADOW_TEMPLE, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Region("Shadow Temple MQ Beyond Boat", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,  true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,   logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,     {[]{return (logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5))) && logic->CanUse(RG_HOVER_BOOTS) && logic->HasItem(RG_SHADOW_TEMPLE_BOSS_KEY);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Region("Shadow Temple MQ Invisible Maze", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, logic->CanUse(RG_DINS_FIRE) && logic->SmallKeys(RR_SHADOW_TEMPLE, 6)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         logic->CanUse(RG_DINS_FIRE) && logic->SmallKeys(RR_SHADOW_TEMPLE, 6)),
                  //below previously returned true
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,      logic->CanUse(RG_LENS_OF_TRUTH) || ctx->GetTrickOption(RT_LENS_SHADOW_MQ_DEADHAND)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,       true),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_SHADOW_TEMPLE_BOSS_ENTRYWAY] =
        Region("Shadow Temple Boss Entryway", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, { [] { return ctx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, { [] { return ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_SHADOW_TEMPLE_BOSS_ROOM] =
        Region("Shadow Temple Boss Room", "Shadow Temple", {}, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&logic->ShadowTempleClear, { [] {
                     return logic->ShadowTempleClear || (logic->HasBossSoul(RG_BONGO_BONGO_SOUL) &&
                            ((logic->CanUse(RG_LENS_OF_TRUTH) || ctx->GetTrickOption(RT_LENS_BONGO)) &&
                             (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) &&
                             (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || ctx->GetTrickOption(RT_SHADOW_BONGO))));
                 } }),
             },
             {
                 // Locations
                 LOCATION(RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, logic->ShadowTempleClear),
                 LOCATION(RC_BONGO_BONGO, logic->ShadowTempleClear),
             },
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_GRAVEYARD_WARP_PAD_REGION, { [] { return logic->ShadowTempleClear; } }, false),
             });
}
