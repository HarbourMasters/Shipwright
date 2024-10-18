#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"
#include "../../trial.h"

using namespace Rando;

void RegionTable_Init_GanonsCastle() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_GANONS_CASTLE_ENTRYWAY] = Region("Ganon's Castle Entryway", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_LOBBY,    {[]{return ctx->GetDungeon(GANONS_CASTLE)->IsVanilla();}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LOBBY, {[]{return ctx->GetDungeon(GANONS_CASTLE)->IsMQ();}}),
                  Entrance(RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE,    {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (ctx->GetDungeon(GANONS_CASTLE)->IsVanilla()) {
  areaTable[RR_GANONS_CASTLE_LOBBY] = Region("Ganon's Castle Lobby", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHEIK_HINT_GC,  true), 
                }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_LIGHT_TRIAL,  {[]{return logic->CanUse(RG_GOLDEN_GAUNTLETS);}}),
                  Entrance(RR_GANONS_CASTLE_TOWER,        {[]{return (logic->ForestTrialClear || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                         (logic->FireTrialClear   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                         (logic->WaterTrialClear  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                         (logic->ShadowTrialClear || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                         (logic->SpiritTrialClear || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                         (logic->LightTrialClear  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped());}}),
                  Entrance(RR_GANONS_CASTLE_DEKU_SCRUBS,  {[]{return ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_GANONS_CASTLE_DEKU_SCRUBS] = Region("Ganon's Castle Deku Scrubs", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,        logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,         logic->CanStunDeku()),
  }, {});

  areaTable[RR_GANONS_CASTLE_FOREST_TRIAL] = Region("Ganon's Castle Forest Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS) && (logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, logic->CanDamage()),
  }, {});

  areaTable[RR_GANONS_CASTLE_FIRE_TRIAL] = Region("Ganon's Castle Fire Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FireTrialClear, {[]{return logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->CanUse(RG_LIGHT_ARROWS) && logic->CanUse(RG_LONGSHOT);}}),
  }, {}, {});

  areaTable[RR_GANONS_CASTLE_WATER_TRIAL] = Region("Ganon's Castle Water Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->BlueFireAccess || logic->HasBottle();}}),
                  EventAccess(&logic->FairyPot,        {[]{return logic->FairyPot || logic->BlueFire();}}),
                  EventAccess(&logic->WaterTrialClear, {[]{return logic->BlueFire() && logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_LIGHT_ARROWS);}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,  true),
                  LOCATION(RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, true),
  }, {});

  areaTable[RR_GANONS_CASTLE_SHADOW_TRIAL] = Region("Ganon's Castle Shadow Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS) && logic->CanUse(RG_MEGATON_HAMMER) && ((logic->CanUse(RG_FIRE_ARROWS) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_DINS_FIRE) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))))));}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,            logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_SONG_OF_TIME) || logic->IsChild),
                  LOCATION(RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, logic->CanUse(RG_FIRE_ARROWS) || (logic->CanUse(RG_LONGSHOT) && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_DINS_FIRE)))),
  }, {});

  areaTable[RR_GANONS_CASTLE_SPIRIT_TRIAL] = Region("Ganon's Castle Spirit Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot,           {[]{return logic->NutPot || (((ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) && logic->CanJumpslashExceptHammer()) || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_BOMBCHU_5) && logic->CanUse(RG_FAIRY_BOW) && (logic->CanUse(RG_MIRROR_SHIELD) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))));}}),
                  EventAccess(&logic->SpiritTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS) && (logic->CanUse(RG_MIRROR_SHIELD) || ctx->GetOption(RSK_SUNLIGHT_ARROWS)) && logic->CanUse(RG_BOMBCHU_5) && ((ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) && logic->CanJumpslashExceptHammer()) || logic->CanUse(RG_HOOKSHOT));}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, (ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || logic->CanUse(RG_HOOKSHOT)) && logic->CanJumpslashExceptHammer()),
                  LOCATION(RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,      (ctx->GetTrickOption(RT_GANON_SPIRIT_TRIAL_HOOKSHOT) || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_BOMBCHU_5) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH))),
  }, {});

  areaTable[RR_GANONS_CASTLE_LIGHT_TRIAL] = Region("Ganon's Castle Light Trial", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->LightTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS) && logic->CanUse(RG_HOOKSHOT) && logic->SmallKeys(RR_GANONS_CASTLE, 2) && (ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,        true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,       true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,        true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,       true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,      true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,       true),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, ctx->GetTrickOption(RT_LENS_GANON) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  LOCATION(RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,           logic->CanUse(RG_ZELDAS_LULLABY) && logic->SmallKeys(RR_GANONS_CASTLE, 1)),
  }, {});
  }

  areaTable[RR_GANONS_CASTLE_TOWER] = Region("Ganon's Castle Tower", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_TOWER_BOSS_KEY_CHEST, logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GANONDORF_HINT,              logic->HasItem(RG_GANONS_CASTLE_BOSS_KEY) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))),
                  LOCATION(RC_GANON,                       logic->HasBossSoul(RG_GANON_SOUL) && logic->HasItem(RG_GANONS_CASTLE_BOSS_KEY) && logic->CanUse(RG_LIGHT_ARROWS) && logic->CanUse(RG_MASTER_SWORD)),
  }, {});

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (ctx->GetDungeon(GANONS_CASTLE)->IsMQ()) {
  areaTable[RR_GANONS_CASTLE_MQ_LOBBY] = Region("Ganon's Castle MQ Lobby", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHEIK_HINT_MQ_GC,  true),
                }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_ENTRYWAY,        {[]{return (logic->CanUse(RG_MASTER_SWORD) || (logic->HasExplosives() || ((logic->CanUse(RG_NUTS) || logic->CanUse(RG_BOOMERANG)) && (logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD)))));}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL,   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL,  {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL,  {[]{return logic->CanUse(RG_GOLDEN_GAUNTLETS);}}),
                  Entrance(RR_GANONS_CASTLE_TOWER,           {[]{return (logic->ForestTrialClear || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                            (logic->FireTrialClear   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                            (logic->WaterTrialClear  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                            (logic->ShadowTrialClear || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                            (logic->SpiritTrialClear || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                            (logic->LightTrialClear  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped());}}),
                  Entrance(RR_GANONS_CASTLE_MQ_DEKU_SCRUBS,  {[]{return ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_DEKU_SCRUBS] = Region("Ganon's Castle MQ Deku Scrubs", "Ganon's Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,  logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,       logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,         logic->CanStunDeku()),
                  LOCATION(RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,        logic->CanStunDeku()),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL] = Region("Ganon's Castle MQ Forest Trial", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTrialClear, {[]{return logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->CanUse(RG_SONG_OF_TIME);}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,        (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && (logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT))),
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->HasFireSource()),
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,        (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->HookshotOrBoomerang()),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL] = Region("Ganon's Castle MQ Fire Trial", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FireTrialClear, {[]{return logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->CanUse(RG_LIGHT_ARROWS) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_GANON_MQ_FIRE_TRIAL) && logic->CanUse(RG_HOOKSHOT)));}}),
                    //Trick: logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->CanUse(RG_LIGHT_ARROWS) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_HOVER_BOOTS) || (LogicFireTrialMQ && logic->CanUse(RG_HOOKSHOT)))
  }, {}, {});

  areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL] = Region("Ganon's Castle MQ Water Trial", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTrialClear, {[]{return logic->BlueFire() && logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && logic->SmallKeys(RR_GANONS_CASTLE, 3);}}),
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->BlueFireAccess || (logic->HasBottle() && logic->CanJumpslashExceptHammer());}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, logic->BlueFire()),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL] = Region("Ganon's Castle MQ Shadow Trial", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowTrialClear, {[]{return logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && (logic->HasFireSource() || ctx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL))));}}),
                    //Trick: logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && (LogicLensCastleMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && (logic->HasFireSource() || LogicShadowTrialMQ)))
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, logic->IsAdult && ((logic->CanUse(RG_FAIRY_BOW) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS))) || (logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->HasExplosives() || logic->HasItem(RG_GORONS_BRACELET) || logic->CanUse(RG_DINS_FIRE))))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && (logic->HasFireSource() || ctx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL))))),
                    //Trick: logic->IsAdult && logic->CanUse(RG_FAIRY_BOW) && (LogicLensCastleMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOVER_BOOTS) || (logic->CanUse(RG_HOOKSHOT) && (logic->HasFireSource() || LogicShadowTrialMQ)))
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL] = Region("Ganon's Castle MQ Spirit Castle", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->SpiritTrialClear, {[]{return logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || ctx->GetOption(RSK_SUNLIGHT_ARROWS));}}),
                  EventAccess(&logic->NutPot,           {[]{return logic->NutPot || (logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && logic->IsAdult && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))));}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,             logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER)),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         logic->IsAdult && (logic->CanUse(RG_FAIRY_BOW) || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER) && logic->CanUse(RG_BOMBCHU_5) && ((logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS)))),
  }, {});

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL] = Region("Ganon's Castle MQ Light Trial", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->LightTrialClear, {[]{return logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->CanUse(RG_LIGHT_ARROWS) && logic->SmallKeys(RR_GANONS_CASTLE, 3) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL));}}),
                    //Trick: logic->IsAdult && logic->CanUse(RG_LIGHT_ARROWS) && logic->SmallKeys(RR_GANONS_CASTLE, 3) && (LogicLensCastleMQ || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->CanUse(RG_HOOKSHOT) || LogicLightTrialMQ)
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && logic->CanUse(RG_ZELDAS_LULLABY)),
  }, {});
  }
}
