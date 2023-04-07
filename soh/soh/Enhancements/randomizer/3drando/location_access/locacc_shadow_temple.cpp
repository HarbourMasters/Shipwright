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
  areaTable[SHADOW_TEMPLE_ENTRYWAY] = Area("Shadow Temple Entryway", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_BEGINNING,    {[]{return Dungeon::ShadowTemple.IsVanilla() && (LogicLensShadow || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));},
                                            /*Glitched*/[]{return Dungeon::ShadowTemple.IsVanilla() && (LogicLensShadow || CanUse(LENS_OF_TRUTH)) && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(SHADOW_TEMPLE_MQ_BEGINNING, {[]{return Dungeon::ShadowTemple.IsMQ()    && (LogicLensShadowMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT));},
                                            /*Glitched*/[]{return Dungeon::ShadowTemple.IsMQ()    && (LogicLensShadowMQ || CanUse(LENS_OF_TRUTH)) && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION,  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::ShadowTemple.IsVanilla()) {
  areaTable[SHADOW_TEMPLE_BEGINNING] = Area("Shadow Temple Beginning", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MAP_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_HOVER_BOOTS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_FIRST_BEAMOS, {[]{return HoverBoots;},
                                            /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[SHADOW_TEMPLE_FIRST_BEAMOS] = Area("Shadow Temple First Beamos", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}), //This fairy pot is only on 3DS
                }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_COMPASS_CHEST,            {[]{return IsAdult || KokiriSword || Sticks;}}),
                  LocationAccess(SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, {[]{return CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT);}}),
                  LocationAccess(SHADOW_TEMPLE_GS_NEAR_SHIP,             {[]{return false;},
                                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && Longshot;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_HUGE_PIT,    {[]{return HasExplosives && IsAdult && SmallKeys(SHADOW_TEMPLE, 1, 2);}}),
                  Entrance(SHADOW_TEMPLE_BEYOND_BOAT, {[]{return false;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE) && Longshot && CanPlay(ZeldasLullaby);}}),
  });

  areaTable[SHADOW_TEMPLE_HUGE_PIT] = Area("Shadow Temple Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,       {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,       {[]{return LogicShadowUmbrella || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,      {[]{return LogicShadowUmbrella || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,           {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_FREESTANDING_KEY,                 {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot && (Bombs || GoronBracelet || (LogicShadowFreestandingKey && HasBombchus));}}),
                  LocationAccess(SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,           {[]{return Hookshot;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,              {[]{return SmallKeys(SHADOW_TEMPLE, 2, 3) && (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot;}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_WIND_TUNNEL, {[]{return (LogicLensShadowBack || CanUse(LENS_OF_TRUTH)) && Hookshot && SmallKeys(SHADOW_TEMPLE, 3, 4);}}),
  });

  areaTable[SHADOW_TEMPLE_WIND_TUNNEL] = Area("Shadow Temple Wind Tunnel", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_NEAR_SHIP,            {[]{return CanUse(LONGSHOT) && SmallKeys(SHADOW_TEMPLE, 4, 5);}}),
                }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_BEYOND_BOAT, {[]{return CanPlay(ZeldasLullaby) && SmallKeys(SHADOW_TEMPLE, 4, 5);}}),
  });

  areaTable[SHADOW_TEMPLE_BEYOND_BOAT] = Area("Shadow Temple Beyond Boat", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,      {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(SHADOW_TEMPLE_BOSS_KEY_CHEST,              {[]{return CanUse(DINS_FIRE);}}),
                  LocationAccess(SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,         {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_BOSS_ENTRYWAY, {[]{return (CanUse(BOW) || CanUse(DISTANT_SCARECROW) || (LogicShadowStatue && HasBombchus)) && SmallKeys(SHADOW_TEMPLE, 5) && CanUse(HOVER_BOOTS) && BossKeyShadowTemple;}})
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::ShadowTemple.IsMQ()) {
  areaTable[SHADOW_TEMPLE_MQ_BEGINNING] = Area("Shadow Temple MQ Beginning", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return IsAdult && (CanUse(FIRE_ARROWS) || CanUse(HOVER_BOOTS));}}),
                    //Trick: IsAdult && (CanUse(FIRE_ARROWS) || HoverBoots || (LogicShadowMQGap && CanUse(LONGSHOT)))
                  Entrance(SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return HasExplosives && SmallKeys(SHADOW_TEMPLE, 6);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Area("Shadow Temple MQ Dead Hand Area", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_COMPASS_CHEST,     {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, {[]{return CanPlay(SongOfTime) && IsAdult && CanUse(BOW);}}),
  }, {});

  areaTable[SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Area("Shadow Temple MQ First Beamos", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,        {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return HasExplosives && SmallKeys(SHADOW_TEMPLE, 2);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Area("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return CanPlay(SongOfTime);}}),
                    //Trick: CanPlay(SongOfTime) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return HasFireSource;}}),
                    //Trick: HasFireSource || LogicShadowMQHugePit
  });

  areaTable[SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Area("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,  {[]{return IsAdult && CanUse(LONGSHOT);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, {[]{return (LogicShadowUmbrella && HoverBoots) || GoronBracelet;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,      {[]{return HoverBoots && SmallKeys(SHADOW_TEMPLE, 3) && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,          {[]{return HoverBoots && SmallKeys(SHADOW_TEMPLE, 3) && Hookshot && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      {[]{return Hookshot;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return HoverBoots && (LogicLensShadowMQBack || CanUse(LENS_OF_TRUTH)) && Hookshot && SmallKeys(SHADOW_TEMPLE, 4);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Area("Shadow Temple MQ Wind Tunnel", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,       {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_BEYOND_BOAT, {[]{return CanPlay(ZeldasLullaby) && SmallKeys(SHADOW_TEMPLE, 5);}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Area("Shadow Temple MQ Beyond Boat", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,  {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,   {[]{return Bow || (LogicShadowStatue && HasBombchus);}}),
  }, {
                  //Exits
                  Entrance(SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return Bow && CanPlay(SongOfTime) && IsAdult && CanUse(LONGSHOT);}}),
                  Entrance(SHADOW_TEMPLE_BOSS_ENTRYWAY,     {[]{return (CanUse(BOW) || (LogicShadowStatue && HasBombchus)) && CanUse(HOVER_BOOTS) && BossKeyShadowTemple;}}),
  });

  areaTable[SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Area("Shadow Temple MQ Invisible Maze", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, {[]{return CanUse(DINS_FIRE) && SmallKeys(SHADOW_TEMPLE, 6);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         {[]{return CanUse(DINS_FIRE) && SmallKeys(SHADOW_TEMPLE, 6);}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,      {[]{return true;}}),
                  LocationAccess(SHADOW_TEMPLE_MQ_FREESTANDING_KEY,       {[]{return true;}}),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[SHADOW_TEMPLE_BOSS_ENTRYWAY] =
        Area("Shadow Temple Boss Entryway", "Shadow Temple", SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(SHADOW_TEMPLE_BEYOND_BOAT, { [] { return Dungeon::ShadowTemple.IsVanilla() && false; } }),
                 Entrance(SHADOW_TEMPLE_MQ_BEYOND_BOAT, { [] { return Dungeon::ShadowTemple.IsMQ() && false; } }),
                 Entrance(SHADOW_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[SHADOW_TEMPLE_BOSS_ROOM] = Area(
        "Shadow Temple Boss Room", "Shadow Temple", NONE, NO_DAY_NIGHT_CYCLE,
        {
            // Events
            EventAccess(&ShadowTempleClear, { [] {
                return ShadowTempleClear ||
                       ((CanUse(LENS_OF_TRUTH) || ((Dungeon::ShadowTemple.IsVanilla() && LogicLensShadowBack) ||
                                                   (Dungeon::ShadowTemple.IsMQ() && LogicLensShadowMQBack))) &&
                        (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || LogicShadowBongo) &&
                        (CanUse(HOOKSHOT) || CanUse(BOW) || CanUse(SLINGSHOT)));
            } }),
        },
        {
            // Locations
            LocationAccess(SHADOW_TEMPLE_BONGO_BONGO_HEART, { [] { return ShadowTempleClear; } }),
            LocationAccess(BONGO_BONGO, { [] { return ShadowTempleClear; } }),
        },
        {
            // Exits
            Entrance(SHADOW_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
        });
}
