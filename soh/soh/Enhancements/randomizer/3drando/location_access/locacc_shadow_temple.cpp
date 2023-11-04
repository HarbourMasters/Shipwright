#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_ShadowTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_SHADOW_TEMPLE_ENTRYWAY] = Area("Shadow Temple Entryway", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEGINNING,    {[]{return Dungeon::ShadowTemple.IsVanilla() && (LogicLensShadow || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEGINNING, {[]{return Dungeon::ShadowTemple.IsMQ()    && (LogicLensShadowMQ || CanUse(RG_LENS_OF_TRUTH)) && (CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION,  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::ShadowTemple.IsVanilla()) {
  areaTable[RR_SHADOW_TEMPLE_BEGINNING] = Area("Shadow Temple Beginning", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MAP_CHEST,         {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_FIRST_BEAMOS, {[]{return CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_FIRST_BEAMOS] = Area("Shadow Temple First Beamos", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}), //This fairy pot is only on 3DS
                }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_COMPASS_CHEST,            {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, {[]{return CanUse(RG_HOVER_BOOTS) || CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,             {[]{return false;}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_HUGE_PIT,    {[]{return HasExplosives && IsAdult && SmallKeys(RR_SHADOW_TEMPLE, 1, 2);}}),
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return false;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_HUGE_PIT] = Area("Shadow Temple Huge Pit", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,       {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,      {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,           {[]{return SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((LogicLensShadowPlatform && LogicLensShadow) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FREESTANDING_KEY,                 {[]{return SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((LogicLensShadowPlatform && LogicLensShadow) || CanUse(RG_LENS_OF_TRUTH)) && Hookshot && (Bombs || GoronBracelet || (LogicShadowFreestandingKey && HasBombchus));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,           {[]{return Hookshot || (LogicShadowUmbrellaGS && HoverBoots);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,              {[]{return SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((LogicLensShadowPlatform && LogicLensShadow) || CanUse(RG_LENS_OF_TRUTH)) && Hookshot;}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_WIND_TUNNEL, {[]{return ((LogicLensShadowPlatform && LogicLensShadow) || CanUse(RG_LENS_OF_TRUTH)) && Hookshot && SmallKeys(RR_SHADOW_TEMPLE, 3, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_WIND_TUNNEL] = Area("Shadow Temple Wind Tunnel", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,            {[]{return CanUse(RG_LONGSHOT) && SmallKeys(RR_SHADOW_TEMPLE, 4, 5);}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return CanJumpslash && CanPlay(ZeldasLullaby) && SmallKeys(RR_SHADOW_TEMPLE, 4, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_BEYOND_BOAT] = Area("Shadow Temple Beyond Boat", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,      {[]{return CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,              {[]{return CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,         {[]{return CanAdultAttack;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, {[]{return (CanUse(RG_FAIRY_BOW) || CanUse(RG_DISTANT_SCARECROW) || (LogicShadowStatue && HasBombchus)) && SmallKeys(RR_SHADOW_TEMPLE, 5) && CanUse(RG_HOVER_BOOTS) && BossKeyShadowTemple;}})
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::ShadowTemple.IsMQ()) {
  areaTable[RR_SHADOW_TEMPLE_MQ_BEGINNING] = Area("Shadow Temple MQ Beginning", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return IsAdult && (CanUse(RG_FIRE_ARROWS) || HoverBoots || (LogicShadowMQGap && CanUse(RG_LONGSHOT)));}}),
                    //Trick: IsAdult && (CanUse(RG_FIRE_ARROWS) || HoverBoots || (LogicShadowMQGap && CanUse(RG_LONGSHOT)))
                  Entrance(RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return HasExplosives && SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Area("Shadow Temple MQ Dead Hand Area", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,     {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, {[]{return CanJumpslash && CanPlay(SongOfTime) && IsAdult && CanUse(RG_FAIRY_BOW);}}),
  }, {});

  areaTable[RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Area("Shadow Temple MQ First Beamos", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_MAP_CHEST,                 {[]{return CanAdultAttack || Nuts;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,        {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, {[]{return CanAdultAttack || Nuts;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return HasExplosives && SmallKeys(RR_SHADOW_TEMPLE, 2);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Area("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO));}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO));}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return HasFireSource || LogicShadowMQHugePit;}}),
                    //Trick: HasFireSource || LogicShadowMQHugePit
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Area("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,  {[]{return IsAdult && CanUse(RG_LONGSHOT);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,      {[]{return CanJumpslash && HoverBoots && SmallKeys(RR_SHADOW_TEMPLE, 3) && ((LogicLensShadowMQ && LogicLensShadowMQPlatform) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,          {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && HoverBoots && SmallKeys(RR_SHADOW_TEMPLE, 3) && Hookshot && ((LogicLensShadowMQ &&
                   LogicLensShadowMQInvisibleBlades && LogicLensShadowMQPlatform) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      {[]{return Hookshot || (LogicShadowUmbrellaGS && HoverBoots);}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return HoverBoots && ((LogicLensShadowMQ && LogicLensShadowMQPlatform) || CanUse(RG_LENS_OF_TRUTH)) && Hookshot && SmallKeys(RR_SHADOW_TEMPLE, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Area("Shadow Temple MQ Wind Tunnel", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  {[]{return CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,       {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, {[]{return CanPlay(ZeldasLullaby) && SmallKeys(RR_SHADOW_TEMPLE, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Area("Shadow Temple MQ Beyond Boat", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,  {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,   {[]{return Bow || (LogicShadowStatue && HasBombchus);}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return Bow && CanPlay(SongOfTime) && IsAdult && CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,     {[]{return (CanUse(RG_FAIRY_BOW) || (LogicShadowStatue && HasBombchus)) && CanUse(RG_HOVER_BOOTS) && BossKeyShadowTemple;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Area("Shadow Temple MQ Invisible Maze", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, {[]{return CanUse(RG_DINS_FIRE) && SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         {[]{return CanUse(RG_DINS_FIRE) && SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
                  //below previously returned true
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,      {[]{return CanUse(RG_LENS_OF_TRUTH) || LogicLensShadowMQDeadHand;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,       {[]{return true;}}),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_SHADOW_TEMPLE_BOSS_ENTRYWAY] =
        Area("Shadow Temple Boss Entryway", "Shadow Temple", RHT_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, { [] { return Dungeon::ShadowTemple.IsVanilla() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, { [] { return Dungeon::ShadowTemple.IsMQ() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_SHADOW_TEMPLE_BOSS_ROOM] =
        Area("Shadow Temple Boss Room", "Shadow Temple", RHT_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&ShadowTempleClear, { [] {
                     return ShadowTempleClear ||
                            ((CanUse(RG_LENS_OF_TRUTH) || LogicLensBongo) &&
                             (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) &&
                             (CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || LogicShadowBongo));
                 } }),
             },
             {
                 // Locations
                 LocationAccess(RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, { [] { return ShadowTempleClear; } }),
                 LocationAccess(RC_BONGO_BONGO, { [] { return ShadowTempleClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
             });
}
