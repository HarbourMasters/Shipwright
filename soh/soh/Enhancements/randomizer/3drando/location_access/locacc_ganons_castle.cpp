#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"
#include "../../trial.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_GanonsCastle() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_GANONS_CASTLE_ENTRYWAY] = Area("Ganon's Castle Entryway", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_LOBBY,    {[]{return randoCtx->GetDungeon(GANONS_CASTLE)->IsVanilla();}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LOBBY, {[]{return randoCtx->GetDungeon(GANONS_CASTLE)->IsMQ();}}),
                  Entrance(RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE,    {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(GANONS_CASTLE)->IsVanilla()) {
  areaTable[RR_GANONS_CASTLE_LOBBY] = Area("Ganon's Castle Lobby", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHEIK_HINT_GC,  {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MAIN,     {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_MAIN] = Area("Ganon's Castle Main", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_LOBBY,        {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL,  {[]{return CanUse(RG_GOLDEN_GAUNTLETS);}}),
                  Entrance(RR_GANONS_CASTLE_TOWER,        {[]{return (ForestTrialClear || randoCtx->GetTrial(Rando::FOREST_TRIAL)->IsSkipped()) &&
                                                                         (FireTrialClear   || randoCtx->GetTrial(Rando::FIRE_TRIAL)->IsSkipped())   &&
                                                                         (WaterTrialClear  || randoCtx->GetTrial(Rando::WATER_TRIAL)->IsSkipped())  &&
                                                                         (ShadowTrialClear || randoCtx->GetTrial(Rando::SHADOW_TRIAL)->IsSkipped()) &&
                                                                         (SpiritTrialClear || randoCtx->GetTrial(Rando::SPIRIT_TRIAL)->IsSkipped()) &&
                                                                         (LightTrialClear  || randoCtx->GetTrial(Rando::LIGHT_TRIAL)->IsSkipped());}}),
                  Entrance(RR_GANONS_CASTLE_DEKU_SCRUBS,  {[]{return randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_GANONS_CASTLE_DEKU_SCRUBS] = Area("Ganon's Castle Deku Scrubs", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_FOREST_TRIAL] = Area("Ganon's Castle Forest Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return CanUse(RG_LIGHT_ARROWS) && (FireArrows || DinsFire);}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, {[]{return CanAdultDamage || CanChildDamage;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_FIRE_TRIAL] = Area("Ganon's Castle Fire Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(RG_GORON_TUNIC) && CanUse(RG_GOLDEN_GAUNTLETS) && CanUse(RG_LIGHT_ARROWS) && CanUse(RG_LONGSHOT);}}),
  }, {}, {});

  areaTable[RR_GANONS_CASTLE_WATER_TRIAL] = Area("Ganon's Castle Water Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
                  EventAccess(&FairyPot,        {[]{return FairyPot || BlueFire;}}),
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && IsAdult && CanUse(RG_MEGATON_HAMMER) && CanUse(RG_LIGHT_ARROWS);}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, {[]{return true;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL] = Area("Ganon's Castle Shadow Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return CanUse(RG_LIGHT_ARROWS) && CanUse(RG_MEGATON_HAMMER) && ((FireArrows && (randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH))) || (CanUse(RG_LONGSHOT) && (CanUse(RG_HOVER_BOOTS) || (DinsFire && (randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH))))));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,            {[]{return CanUse(RG_FIRE_ARROWS) || CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS) || CanUse(RG_SONG_OF_TIME) || IsChild;}}),
                  LocationAccess(RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, {[]{return CanUse(RG_FIRE_ARROWS) || (CanUse(RG_LONGSHOT) && (CanUse(RG_HOVER_BOOTS) || CanUse(RG_DINS_FIRE)));}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL] = Area("Ganon's Castle Spirit Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,           {[]{return NutPot || (((randoCtx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) && CanJumpslash) || CanUse(RG_HOOKSHOT)) && HasBombchus && CanUse(RG_FAIRY_BOW) && (CanUse(RG_MIRROR_SHIELD) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS))));}}),
                  EventAccess(&SpiritTrialClear, {[]{return CanUse(RG_LIGHT_ARROWS) && (CanUse(RG_MIRROR_SHIELD) || randoCtx->GetOption(RSK_SUNLIGHT_ARROWS)) && HasBombchus && ((randoCtx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) && CanJumpslash) || CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, {[]{return (randoCtx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || CanUse(RG_HOOKSHOT)) && CanJumpslash;}}),
                  LocationAccess(RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      {[]{return (randoCtx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || CanUse(RG_HOOKSHOT)) && HasBombchus && (randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH));}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL] = Area("Ganon's Castle Light Trial", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return CanUse(RG_LIGHT_ARROWS) && CanUse(RG_HOOKSHOT) && SmallKeys(RR_GANONS_CASTLE, 2) && (randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, {[]{return randoCtx->GetTrickOption(RT_LENS_GANON) || CanUse(RG_LENS_OF_TRUTH);}}),
                  LocationAccess(RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,           {[]{return CanUse(RG_ZELDAS_LULLABY) && SmallKeys(RR_GANONS_CASTLE, 1);}}),
  }, {});
  }

  areaTable[RR_GANONS_CASTLE_TOWER] = Area("Ganon's Castle Tower", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GANONS_TOWER_BOSS_KEY_CHEST, {[]{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GANONDORF_HINT,              {[]{return BossKeyGanonsCastle && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD));}}),
                  LocationAccess(RC_GANON,                       {[]{return HasBossSoul(RG_GANON_SOUL) && BossKeyGanonsCastle && CanUse(RG_LIGHT_ARROWS) && CanUse(RG_MASTER_SWORD);}}),
  }, {});

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(GANONS_CASTLE)->IsMQ()) {
  areaTable[RR_GANONS_CASTLE_MQ_LOBBY] = Area("Ganon's Castle MQ Lobby", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHEIK_HINT_MQ_GC,  {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,  {[]{return (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER)) || ((HasExplosives || Nuts || CanUse(RG_BOOMERANG)) && CanJumpslash);}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_MAIN] = Area("Ganon's Castle MQ Main", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_LOBBY,        {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL,  {[]{return CanUse(RG_GOLDEN_GAUNTLETS);}}),
                  Entrance(RR_GANONS_CASTLE_TOWER,           {[]{return (ForestTrialClear || randoCtx->GetTrial(Rando::FOREST_TRIAL)->IsSkipped()) &&
                                                                            (FireTrialClear   || randoCtx->GetTrial(Rando::FIRE_TRIAL)->IsSkipped())   &&
                                                                            (WaterTrialClear  || randoCtx->GetTrial(Rando::WATER_TRIAL)->IsSkipped())  &&
                                                                            (ShadowTrialClear || randoCtx->GetTrial(Rando::SHADOW_TRIAL)->IsSkipped()) &&
                                                                            (SpiritTrialClear || randoCtx->GetTrial(Rando::SPIRIT_TRIAL)->IsSkipped()) &&
                                                                            (LightTrialClear  || randoCtx->GetTrial(Rando::LIGHT_TRIAL)->IsSkipped());}}),
                  Entrance(RR_GANONS_CASTLE_MQ_DEKU_SCRUBS,  {[]{return randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_DEKU_SCRUBS] = Area("Ganon's Castle MQ Deku Scrubs", "Ganon's Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL] = Area("Ganon's Castle MQ Forest Trial", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return IsAdult && CanUse(RG_LIGHT_ARROWS) && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && CanUse(RG_SONG_OF_TIME);}}),
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,        {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && HasFireSource;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,        {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && HookshotOrBoomerang;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL] = Area("Ganon's Castle MQ Fire Trial", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(RG_GORON_TUNIC) && CanUse(RG_GOLDEN_GAUNTLETS) && CanUse(RG_LIGHT_ARROWS) && (CanUse(RG_LONGSHOT) || HoverBoots || (randoCtx->GetTrickOption(RT_GANON_MQ_FIRE_TRIAL) && CanUse(RG_HOOKSHOT)));}}),
                    //Trick: CanUse(RG_GORON_TUNIC) && CanUse(RG_GOLDEN_GAUNTLETS) && CanUse(RG_LIGHT_ARROWS) && (CanUse(RG_LONGSHOT) || HoverBoots || (LogicFireTrialMQ && CanUse(RG_HOOKSHOT)))
  }, {}, {});

  areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL] = Area("Ganon's Castle MQ Water Trial", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && IsAdult && CanUse(RG_LIGHT_ARROWS) && SmallKeys(RR_GANONS_CASTLE, 3);}}),
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || (HasBottle && CanJumpslash);}}),
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, {[]{return BlueFire;}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL] = Area("Ganon's Castle MQ Shadow Trial", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return IsAdult && CanUse(RG_LIGHT_ARROWS) && (randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || randoCtx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL))));}}),
                    //Trick: IsAdult && CanUse(RG_LIGHT_ARROWS) && (LogicLensCastleMQ || CanUse(RG_LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, {[]{return IsAdult && ((Bow && (CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS))) || (CanUse(RG_HOVER_BOOTS) && (randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH)) && (HasExplosives || GoronBracelet || CanUse(RG_DINS_FIRE))));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  {[]{return IsAdult && Bow && (randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || randoCtx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL))));}}),
                    //Trick: IsAdult && Bow && (LogicLensCastleMQ || CanUse(RG_LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL] = Area("Ganon's Castle MQ Spirit Castle", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTrialClear, {[]{return IsAdult && CanUse(RG_LIGHT_ARROWS) && Hammer && HasBombchus && ((FireArrows && MirrorShield) || randoCtx->GetOption(RSK_SUNLIGHT_ARROWS));}}),
                  EventAccess(&NutPot,           {[]{return NutPot || (Hammer && HasBombchus && IsAdult && ((CanUse(RG_FIRE_ARROWS) && MirrorShield) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS))));}}),
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,             {[]{return IsAdult && (Bow || randoCtx->GetTrickOption(RT_RUSTED_SWITCHES)) && Hammer;}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         {[]{return IsAdult && (Bow || randoCtx->GetTrickOption(RT_RUSTED_SWITCHES)) && Hammer && HasBombchus && (randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && ((CanUse(RG_FIRE_ARROWS) && CanUse(RG_MIRROR_SHIELD)) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     {[]{return IsAdult && Hammer && HasBombchus && ((CanUse(RG_FIRE_ARROWS) && CanUse(RG_MIRROR_SHIELD)) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  {[]{return IsAdult && Hammer && HasBombchus && ((CanUse(RG_FIRE_ARROWS) && CanUse(RG_MIRROR_SHIELD)) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)));}}),
                  LocationAccess(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && ((CanUse(RG_FIRE_ARROWS) && CanUse(RG_MIRROR_SHIELD)) || (randoCtx->GetOption(RSK_SUNLIGHT_ARROWS) && CanUse(RG_LIGHT_ARROWS)));}}),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL] = Area("Ganon's Castle MQ Light Trial", "Ganons Castle", RA_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD)) && CanUse(RG_LIGHT_ARROWS) && SmallKeys(RR_GANONS_CASTLE, 3) && (randoCtx->GetTrickOption(RT_LENS_GANON_MQ) || CanUse(RG_LENS_OF_TRUTH)) && (Hookshot || randoCtx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL));}}),
                    //Trick: IsAdult && CanUse(RG_LIGHT_ARROWS) && SmallKeys(RR_GANONS_CASTLE, 3) && (LogicLensCastleMQ || CanUse(RG_LENS_OF_TRUTH)) && (Hookshot || LogicLightTrialMQ)
  }, {
                  //Locations
                  LocationAccess(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, {[]{return (CanUse(RG_MASTER_SWORD) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD)) && CanUse(RG_ZELDAS_LULLABY);}}),
  }, {});
  }
}
