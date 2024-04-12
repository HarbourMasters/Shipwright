#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_ShadowTemple() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_SHADOW_TEMPLE_ENTRYWAY] = Area("Shadow Temple Entryway", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEGINNING,    {[]{return randoCtx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && (randoCtx->GetTrickOption(RT_LENS_SHADOW) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEGINNING, {[]{return randoCtx->GetDungeon(SHADOW_TEMPLE)->IsMQ()    && (randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION,  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(SHADOW_TEMPLE)->IsVanilla()) {
  areaTable[RR_SHADOW_TEMPLE_BEGINNING] = Area("Shadow Temple Beginning", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MAP_CHEST,         {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, {[]{return (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_FIRST_BEAMOS, {[]{return logic->CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_FIRST_BEAMOS] = Area("Shadow Temple First Beamos", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}), //This fairy pot is only on 3DS
                }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_COMPASS_CHEST,            {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,             {[]{return false;}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_HUGE_PIT,    {[]{return logic->HasExplosives && logic->IsAdult && logic->SmallKeys(RR_SHADOW_TEMPLE, 1, 2);}}),
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return false;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_HUGE_PIT] = Area("Shadow Temple Huge Pit", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,       {[]{return (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->HoverBoots) || logic->GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,      {[]{return (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->HoverBoots) || logic->GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,           {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && randoCtx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_FREESTANDING_KEY,                 {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && randoCtx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->Hookshot && (logic->Bombs || logic->GoronBracelet || (randoCtx->GetTrickOption(RT_SHADOW_FREESTANDING_KEY) && logic->HasBombchus));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,                {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,           {[]{return logic->Hookshot || (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->HoverBoots);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,              {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 2, 3) && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && randoCtx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->Hookshot;}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_WIND_TUNNEL, {[]{return ((randoCtx->GetTrickOption(RT_LENS_SHADOW_PLATFORM) && randoCtx->GetTrickOption(RT_LENS_SHADOW)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->Hookshot && logic->SmallKeys(RR_SHADOW_TEMPLE, 3, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_WIND_TUNNEL] = Area("Shadow Temple Wind Tunnel", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,  {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_NEAR_SHIP,            {[]{return logic->CanUse(RG_LONGSHOT) && logic->SmallKeys(RR_SHADOW_TEMPLE, 4, 5);}}),
                }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, {[]{return logic->CanJumpslash && logic->CanUse(RG_ZELDAS_LULLABY) && logic->SmallKeys(RR_SHADOW_TEMPLE, 4, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_BEYOND_BOAT] = Area("Shadow Temple Beyond Boat", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,      {[]{return logic->CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,              {[]{return logic->CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,         {[]{return logic->CanAdultAttack;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, {[]{return (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DISTANT_SCARECROW) || (randoCtx->GetTrickOption(RT_SHADOW_STATUE) && logic->HasBombchus)) && logic->SmallKeys(RR_SHADOW_TEMPLE, 5) && logic->CanUse(RG_HOVER_BOOTS) && logic->BossKeyShadowTemple;}})
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(SHADOW_TEMPLE)->IsMQ()) {
  areaTable[RR_SHADOW_TEMPLE_MQ_BEGINNING] = Area("Shadow Temple MQ Beginning", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || logic->HoverBoots || (randoCtx->GetTrickOption(RT_SHADOW_MQ_GAP) && logic->CanUse(RG_LONGSHOT)));}}),
                    //Trick: logic->IsAdult && (logic->CanUse(RG_FIRE_ARROWS) || logic->HoverBoots || (LogicShadowMQGap && logic->CanUse(RG_LONGSHOT)))
                  Entrance(RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return logic->HasExplosives && logic->SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Area("Shadow Temple MQ Dead Hand Area", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,     {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, {[]{return logic->CanJumpslash && logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult && logic->CanUse(RG_FAIRY_BOW);}}),
  }, {});

  areaTable[RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Area("Shadow Temple MQ First Beamos", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_MAP_CHEST,                 {[]{return logic->CanAdultAttack || logic->Nuts;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,        {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, {[]{return logic->CanAdultAttack || logic->Nuts;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return logic->HasExplosives && logic->SmallKeys(RR_SHADOW_TEMPLE, 2);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Area("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   {[]{return logic->CanUse(RG_SONG_OF_TIME) || (randoCtx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO));}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, {[]{return logic->CanUse(RG_SONG_OF_TIME) || (randoCtx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && randoCtx->GetOption(RSK_DAMAGE_MULTIPLIER).IsNot(RO_DAMAGE_MULTIPLIER_OHKO));}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || (LogicShadowMQInvisibleBlades && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return logic->HasFireSource || randoCtx->GetTrickOption(RT_SHADOW_MQ_HUGE_PIT);}}),
                    //Trick: logic->HasFireSource || LogicShadowMQHugePit
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Area("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,  {[]{return logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  {[]{return (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->HoverBoots) || logic->GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, {[]{return (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->HoverBoots) || logic->GoronBracelet;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,      {[]{return logic->CanJumpslash && logic->HoverBoots && logic->SmallKeys(RR_SHADOW_TEMPLE, 3) && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ) && randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,          {[]{return (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->HoverBoots && logic->SmallKeys(RR_SHADOW_TEMPLE, 3) && logic->Hookshot && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ) &&
                   randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) && randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      {[]{return logic->Hookshot || (randoCtx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && logic->HoverBoots);}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return logic->HoverBoots && ((randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ) && randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM)) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->Hookshot && logic->SmallKeys(RR_SHADOW_TEMPLE, 4);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Area("Shadow Temple MQ Wind Tunnel", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,         {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  {[]{return logic->CanJumpslash;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,       {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) && logic->SmallKeys(RR_SHADOW_TEMPLE, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Area("Shadow Temple MQ Beyond Boat", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,  {[]{return true;}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,   {[]{return logic->Bow || (randoCtx->GetTrickOption(RT_SHADOW_STATUE) && logic->HasBombchus);}}),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return logic->Bow && logic->CanUse(RG_SONG_OF_TIME) && logic->IsAdult && logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,     {[]{return (logic->CanUse(RG_FAIRY_BOW) || (randoCtx->GetTrickOption(RT_SHADOW_STATUE) && logic->HasBombchus)) && logic->CanUse(RG_HOVER_BOOTS) && logic->BossKeyShadowTemple;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Area("Shadow Temple MQ Invisible Maze", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, {[]{return logic->CanUse(RG_DINS_FIRE) && logic->SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         {[]{return logic->CanUse(RG_DINS_FIRE) && logic->SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
                  //below previously returned true
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,      {[]{return logic->CanUse(RG_LENS_OF_TRUTH) || randoCtx->GetTrickOption(RT_LENS_SHADOW_MQ_DEADHAND);}}),
                  LocationAccess(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,       {[]{return true;}}),
  }, {});
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_SHADOW_TEMPLE_BOSS_ENTRYWAY] =
        Area("Shadow Temple Boss Entryway", "Shadow Temple", RA_SHADOW_TEMPLE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BEYOND_BOAT, { [] { return randoCtx->GetDungeon(SHADOW_TEMPLE)->IsVanilla() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, { [] { return randoCtx->GetDungeon(SHADOW_TEMPLE)->IsMQ() && false; } }),
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_SHADOW_TEMPLE_BOSS_ROOM] =
        Area("Shadow Temple Boss Room", "Shadow Temple", RA_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&logic->ShadowTempleClear, { [] {
                     return logic->ShadowTempleClear || (logic->HasBossSoul(RG_BONGO_BONGO_SOUL) &&
                            ((logic->CanUse(RG_LENS_OF_TRUTH) || randoCtx->GetTrickOption(RT_LENS_BONGO)) &&
                             (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) &&
                             (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || randoCtx->GetTrickOption(RT_SHADOW_BONGO))));
                 } }),
             },
             {
                 // Locations
                 LocationAccess(RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, { [] { return logic->ShadowTempleClear; } }),
                 LocationAccess(RC_BONGO_BONGO, { [] { return logic->ShadowTempleClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_GRAVEYARD_WARP_PAD_REGION, { [] { return logic->ShadowTempleClear; } }),
             });
}
