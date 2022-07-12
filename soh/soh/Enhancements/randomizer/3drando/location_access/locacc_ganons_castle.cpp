#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"
#include "../trial.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_GanonsCastle() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[GANONS_CASTLE_ENTRYWAY] = Area("Ganon's Castle Entryway", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_LOBBY,    {[]{return Dungeon::GanonsCastle.IsVanilla();}}),
                  Entrance(GANONS_CASTLE_MQ_LOBBY, {[]{return Dungeon::GanonsCastle.IsMQ();}}),
                  Entrance(GANONS_CASTLE_GROUNDS,  {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::GanonsCastle.IsVanilla()) {
  areaTable[GANONS_CASTLE_LOBBY] = Area("Ganon's Castle Lobby", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(GANONS_CASTLE_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(GANONS_CASTLE_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(GANONS_CASTLE_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_LIGHT_TRIAL,  {[]{return CanUse(GOLDEN_GAUNTLETS);}}),
                  Entrance(GANONS_CASTLE_TOWER,        {[]{return (ForestTrialClear || Trial::ForestTrial.IsSkipped()) &&
                                                                         (FireTrialClear   || Trial::FireTrial.IsSkipped())   &&
                                                                         (WaterTrialClear  || Trial::WaterTrial.IsSkipped())  &&
                                                                         (ShadowTrialClear || Trial::ShadowTrial.IsSkipped()) &&
                                                                         (SpiritTrialClear || Trial::SpiritTrial.IsSkipped()) &&
                                                                         (LightTrialClear  || Trial::LightTrial.IsSkipped());}}),
                  Entrance(GANONS_CASTLE_DEKU_SCRUBS,  {[]{return LogicLensCastle || CanUse(LENS_OF_TRUTH);}}),
  });

  areaTable[GANONS_CASTLE_DEKU_SCRUBS] = Area("Ganon's Castle Deku Scrubs", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
  }, {});

  areaTable[GANONS_CASTLE_FOREST_TRIAL] = Area("Ganon's Castle Forest Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return CanUse(LIGHT_ARROWS) && (FireArrows || DinsFire);}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_FOREST_TRIAL_CHEST, {[]{return IsAdult || CanChildDamage;}}),
  }, {});

  areaTable[GANONS_CASTLE_FIRE_TRIAL] = Area("Ganon's Castle Fire Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && CanUse(LONGSHOT);}}),
  }, {}, {});

  areaTable[GANONS_CASTLE_WATER_TRIAL] = Area("Ganon's Castle Water Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
                  EventAccess(&FairyPot,        {[]{return FairyPot || BlueFire;}}),
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && Hammer && CanUse(LIGHT_ARROWS);}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, {[]{return true;}}),
  }, {});

  areaTable[GANONS_CASTLE_SHADOW_TRIAL] = Area("Ganon's Castle Shadow Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return CanUse(LIGHT_ARROWS) && Hammer && ((FireArrows && (LogicLensCastle || CanUse(LENS_OF_TRUTH))) || (CanUse(LONGSHOT) && (CanUse(HOVER_BOOTS) || (DinsFire && (LogicLensCastle || CanUse(LENS_OF_TRUTH))))));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,            {[]{return CanUse(FIRE_ARROWS) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || CanPlay(SongOfTime) || IsChild;}}),
                  LocationAccess(GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, {[]{return CanUse(FIRE_ARROWS) || (CanUse(LONGSHOT) && (CanUse(HOVER_BOOTS) || CanUse(DINS_FIRE)));}}),
  }, {});

  areaTable[GANONS_CASTLE_SPIRIT_TRIAL] = Area("Ganon's Castle Spirit Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,           {[]{return NutPot || ((LogicSpiritTrialHookshot || CanUse(HOOKSHOT)) && HasBombchus && Bow && MirrorShield && IsAdult);}}),
                  EventAccess(&SpiritTrialClear, {[]{return CanUse(LIGHT_ARROWS)  && MirrorShield && HasBombchus && (LogicSpiritTrialHookshot || CanUse(HOOKSHOT));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, {[]{return LogicSpiritTrialHookshot || CanUse(HOOKSHOT);}}),
                  LocationAccess(GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      {[]{return (LogicSpiritTrialHookshot || CanUse(HOOKSHOT)) && HasBombchus && (LogicLensCastle || CanUse(LENS_OF_TRUTH));}}),
  }, {});

  areaTable[GANONS_CASTLE_LIGHT_TRIAL] = Area("Ganon's Castle Light Trial", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return CanUse(LIGHT_ARROWS) && CanUse(HOOKSHOT) && SmallKeys(GANONS_CASTLE, 2) && (LogicLensCastle || CanUse(LENS_OF_TRUTH));}}),
                }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       {[]{return true;}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, {[]{return LogicLensCastle || CanUse(LENS_OF_TRUTH);}}),
                  LocationAccess(GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,           {[]{return CanPlay(ZeldasLullaby) && SmallKeys(GANONS_CASTLE, 1);}}),
  }, {});
  }

  areaTable[GANONS_CASTLE_TOWER] = Area("Ganon's Castle Tower", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GANONS_TOWER_BOSS_KEY_CHEST, {[]{return true;}}),
                  LocationAccess(GANONDORF_HINT,              {[]{return BossKeyGanonsCastle;}}),
                  LocationAccess(GANON,                       {[]{return BossKeyGanonsCastle && CanUse(LIGHT_ARROWS);}}),
  }, {});

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::GanonsCastle.IsMQ()) {
  areaTable[GANONS_CASTLE_MQ_LOBBY] = Area("Ganon's Castle MQ Lobby", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GANONS_CASTLE_ENTRYWAY,        {[]{return (IsAdult || (HasExplosives || ((Nuts || Boomerang) && (Sticks || KokiriSword))));}}),
                  Entrance(GANONS_CASTLE_MQ_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(GANONS_CASTLE_MQ_LIGHT_TRIAL,  {[]{return CanUse(GOLDEN_GAUNTLETS);}}),
                  Entrance(GANONS_CASTLE_TOWER,           {[]{return (ForestTrialClear || Trial::ForestTrial.IsSkipped()) &&
                                                                            (FireTrialClear   || Trial::FireTrial.IsSkipped())   &&
                                                                            (WaterTrialClear  || Trial::WaterTrial.IsSkipped())  &&
                                                                            (ShadowTrialClear || Trial::ShadowTrial.IsSkipped()) &&
                                                                            (SpiritTrialClear || Trial::SpiritTrial.IsSkipped()) &&
                                                                            (LightTrialClear  || Trial::LightTrial.IsSkipped());}}),
                  Entrance(GANONS_CASTLE_MQ_DEKU_SCRUBS,  {[]{return LogicLensCastleMQ || CanUse(LENS_OF_TRUTH);}}),
  });

  areaTable[GANONS_CASTLE_MQ_DEKU_SCRUBS] = Area("Ganon's Castle MQ Deku Scrubs", "Ganon's Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         {[]{return CanStunDeku;}}),
                  LocationAccess(GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        {[]{return CanStunDeku;}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_FOREST_TRIAL] = Area("Ganon's Castle MQ Forest Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ForestTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && CanPlay(SongOfTime);}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,        {[]{return (IsAdult && CanUse(BOW)) || (IsChild && CanUse(SLINGSHOT));}}),
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, {[]{return HasFireSource;}}),
                  LocationAccess(GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,        {[]{return HookshotOrBoomerang;}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_FIRE_TRIAL] = Area("Ganon's Castle MQ Fire Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FireTrialClear, {[]{return CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && (CanUse(LONGSHOT) || CanUse(HOVER_BOOTS));}}),
                    //Trick: CanUse(GORON_TUNIC) && CanUse(GOLDEN_GAUNTLETS) && CanUse(LIGHT_ARROWS) && (CanUse(LONGSHOT) || HoverBoots || (LogicFireTrialMQ && CanUse(HOOKSHOT)))
  }, {}, {});

  areaTable[GANONS_CASTLE_MQ_WATER_TRIAL] = Area("Ganon's Castle MQ Water Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WaterTrialClear, {[]{return BlueFire && IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3);}}),
                  EventAccess(&BlueFireAccess,  {[]{return BlueFireAccess || HasBottle;}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, {[]{return BlueFire && (IsAdult || CanUse(STICKS) || KokiriSword || CanUseProjectile);}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_SHADOW_TRIAL] = Area("Ganon's Castle MQ Shadow Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ShadowTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || (CanUse(HOOKSHOT) && HasFireSource));}}),
                    //Trick: IsAdult && CanUse(LIGHT_ARROWS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, {[]{return IsAdult && ((Bow && (CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS))) || (CanUse(HOVER_BOOTS) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HasExplosives || GoronBracelet || CanUse(DINS_FIRE))));}}),
                  LocationAccess(GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  {[]{return IsAdult && Bow && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (CanUse(HOVER_BOOTS) || (CanUse(HOOKSHOT) && HasFireSource));}}),
                    //Trick: IsAdult && Bow && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (HoverBoots || (Hookshot && (HasFireSource || LogicShadowTrialMQ)))
  }, {});

  areaTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL] = Area("Ganon's Castle MQ Spirit Castle", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SpiritTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && Hammer && HasBombchus && FireArrows && MirrorShield;}}),
                  EventAccess(&NutPot,           {[]{return NutPot || (Hammer && HasBombchus && IsAdult && CanUse(FIRE_ARROWS) && MirrorShield);}}),
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,             {[]{return IsAdult && Bow && Hammer;}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         {[]{return IsAdult && Bow && Hammer && HasBombchus && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH));}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
                  LocationAccess(GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    {[]{return IsAdult && Hammer && HasBombchus && CanUse(FIRE_ARROWS) && CanUse(MIRROR_SHIELD);}}),
  }, {});

  areaTable[GANONS_CASTLE_MQ_LIGHT_TRIAL] = Area("Ganon's Castle MQ Light Trial", "Ganons Castle", GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&LightTrialClear, {[]{return IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && CanUse(HOOKSHOT);}}),
                    //Trick: IsAdult && CanUse(LIGHT_ARROWS) && SmallKeys(GANONS_CASTLE, 3) && (LogicLensCastleMQ || CanUse(LENS_OF_TRUTH)) && (Hookshot || LogicLightTrialMQ)
  }, {
                  //Locations
                  LocationAccess(GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, {[]{return CanPlay(ZeldasLullaby);}}),
  }, {});
  }
}
