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
  areaTable[RR_GANONS_CASTLE_MQ_LOBBY] = Region("Ganon's Castle MQ Lobby", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_ENTRYWAY, {[]{return logic->CanPassEnemy(RE_GREEN_BUBBLE) || Here(RR_GANONS_CASTLE_MQ_LOBBY, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);});}}),
                  //Implies CanKillEnemy(RE_GREEN_BUBBLE)
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,  {[]{return Here(RR_GANONS_CASTLE_MQ_LOBBY, []{return logic->CanKillEnemy(RE_IRON_KNUCKLE) && logic->CanKillEnemy(RE_ARMOS);});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_MAIN] = Region("Ganon's Castle MQ Main", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHEIK_HINT_MQ_GC,  true),
                }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_LOBBY,                       {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM,        {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,     {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,    {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,   {[]{return Here(RR_GANONS_CASTLE_MQ_MAIN, []{return logic->CanUse(RG_GOLDEN_GAUNTLETS);});}}),
                  //RANDOTODO could we just set these events automatically based on the setting?
                  Entrance(RR_GANONS_CASTLE_TOWER,                          {[]{return (logic->ForestTrialClear || ctx->GetTrial(TK_FOREST_TRIAL)->IsSkipped()) &&
                                                                                       (logic->FireTrialClear   || ctx->GetTrial(TK_FIRE_TRIAL)->IsSkipped())   &&
                                                                                       (logic->WaterTrialClear  || ctx->GetTrial(TK_WATER_TRIAL)->IsSkipped())  &&
                                                                                       (logic->ShadowTrialClear || ctx->GetTrial(TK_SHADOW_TRIAL)->IsSkipped()) &&
                                                                                       (logic->SpiritTrialClear || ctx->GetTrial(TK_SPIRIT_TRIAL)->IsSkipped()) &&
                                                                                       (logic->LightTrialClear  || ctx->GetTrial(TK_LIGHT_TRIAL)->IsSkipped());}}),
                  Entrance(RR_GANONS_CASTLE_MQ_DEKU_SCRUBS,                 {[]{return ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
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
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN, {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Stalfos Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, logic->HookshotOrBoomerang()),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                     {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, {[]{return Here(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);});}}),
  });

//If it is ever possible to warp into the RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM, this needs splitting up as the requirements to reach things from the other side are more complex
  areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM] = Region("Ganon's Castle MQ Forest Trial Beamos Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,        logic->CanHitEyeTargets()),
                  LOCATION(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, logic->HasFireSource()),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM, {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM,   {[]{return logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME);}}),

  });
  
  areaTable[RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Forest Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ForestTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM] = Region("Ganon's Castle MQ Fire Trial Main Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                  {[]{return true;}}),
                                                                                //2 checks, 1 for the rupees, 1 for actually making it, as the rupees are permanent but throwing a pillar is not
                  Entrance(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM, {[]{return Here(RR_GANONS_CASTLE_MQ_FIRE_TRIAL_MAIN_ROOM, []{return logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_GOLDEN_GAUNTLETS);}) &&
                                                                                logic->CanUse(RG_GORON_TUNIC) && (logic->CanUse(RG_LONGSHOT) || (logic->CanUse(RG_GOLDEN_GAUNTLETS) && (logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_GANON_MQ_FIRE_TRIAL) && logic->IsAdult && logic->CanUse(RG_HOOKSHOT)))));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Fire Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FireTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
                  //There's no way back across the lava without glitches
  }, {}, {});

  areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM] = Region("Ganon's Castle MQ Water Trial Geyser Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BlueFireAccess,  {[]{return logic->CanJumpslash() || logic->HasExplosives();}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, logic->BlueFire()),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                   {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, {[]{return logic->SmallKeys(RR_GANONS_CASTLE, 3) && Here(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM, []{return logic->BlueFire();});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM] = Region("Ganon's Castle MQ Water Trial Block Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, { 
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM, {[]{return logic->SmallKeys(RR_GANONS_CASTLE, 3);}}),
                  //This assumes there's no way for child to have blue fire and not adult.
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM,  {[]{return logic->IsAdult && logic->BlueFire();}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Water Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WaterTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, logic->BlueFire()),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, {[]{return Here(RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM, []{return logic->BlueFire();});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE] = Region("Ganon's Castle MQ Shadow Trial Starting Ledge", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowTrialFirstChest, {[]{return logic->CanUse(RG_FAIRY_BOW);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                        {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, {[]{return (logic->ShadowTrialFirstChest && logic->CanUse(RG_HOOKSHOT)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Chest Platform", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowTrialFirstChest, {[]{return logic->CanUse(RG_FAIRY_BOW);}}),
  }, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, logic->ShadowTrialFirstChest),
  }, {
                  //Exits
                  //Hookshot here is possible but very tight, but it's basically never relevant
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE,  {[]{return logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, {[]{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && (logic->IsAdult || logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM] = Region("Ganon's Castle MQ Shadow Trial Moving Platform", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but tight, so would be a trick
                  EventAccess(&logic->ShadowTrialFirstChest, {[]{return logic->CanDetonateUprightBombFlower();}}),
  }, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_CHEST_PLATFORM, {[]{return logic->IsAdult || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH,   {[]{return true;}}),
  });


  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH] = Region("Ganon's Castle MQ Shadow Trial Beamos Torch", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_MOVING_PLATFORM, {[]{return ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
                  //A torch run from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE is possible but very tight, so would be a trick
                  //The bow trick assumes RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE access, if you can somehow void warp directly here it will need handling properly
                  //Hovers is possible as child but a bit tight, requires good rolls
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE,        {[]{return logic->HasFireSource() || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_GANON_MQ_SHADOW_TRIAL) && logic->CanUse(RG_FAIRY_BOW));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE] = Region("Ganon's Castle MQ Shadow Trial Far Side", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,  logic->CanHitEyeTargets()),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BEAMOS_TORCH, {[]{return logic->CanUse(RG_FIRE_ARROWS) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  //Modelling the silver rupees properly will require a way to check temp flags in different regions. 
                  //It may be tempting to use a Here-like command for this but it could cause sphere skipping in playthroughs
                  //So a system like event access which sets based on TimeAge would be preferable, as the application of these can be tracked and accounted for, unlike Here-like commands
                  //For Now I am assuming the player has made it all the way from RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, which logically means every rupee is available
                  //with no extra requirements except the lens logic needed to reach the door, which also enables the beamos-platform rupee
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM,   {[]{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Shadow Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FAR_SIDE, {[]{return (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM] = Region("Ganon's Castle MQ Spirit Trial Chairs Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                       {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, {[]{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM, []{return (logic->CanHitEyeTargets() || ctx->GetTrickOption(RT_RUSTED_SWITCHES)) && logic->CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH] = Region("Ganon's Castle MQ Spirit Trial Before Switch", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, logic->CanPassEnemy(RE_GREEN_BUBBLE)),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,  {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, {[]{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, []{return logic->CanUse(RG_BOMBCHU_5);});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH] = Region("Ganon's Castle MQ Spirit Trial After Switch", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,         ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
                  //better names for these would be nice.
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,    (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,     (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,  (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
                  LOCATION(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,    (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD)) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS))),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, {[]{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return logic->CanUse(RG_BOMBCHU_5);});}}),
                  //Sunlight arrows are bugged, should set a perm flag like mirror shield
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM,    {[]{return Here(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, []{return (logic->CanUse(RG_FIRE_ARROWS) && logic->CanUse(RG_MIRROR_SHIELD));}) || (ctx->GetOption(RSK_SUNLIGHT_ARROWS) && logic->CanUse(RG_LIGHT_ARROWS));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Spirit Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->SpiritTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
                  EventAccess(&logic->NutPot,           {[]{return true;}}),
  }, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM] = Region("Ganon's Castle MQ Light Trial Dinolfos Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_MAIN,                      {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM, {[]{return Here(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM, []{return logic->CanKillEnemy(RE_DINOLFOS) && logic->CanKillEnemy(RE_TORCH_SLUG);});}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM] = Region("Ganon's Castle MQ Light Trial Triforce Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, logic->CanUse(RG_ZELDAS_LULLABY)),
  }, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,      {[]{return true;}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, {[]{return logic->SmallKeys(RR_GANONS_CASTLE, 2);}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT] = Region("Ganon's Castle MQ Light Trial Boulder Room Front", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM,     {[]{return logic->SmallKeys(RR_GANONS_CASTLE, 2);}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, {[]{return logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL);}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK] = Region("Ganon's Castle MQ Light Trial Boulder Room Back", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  //I got the trick going backwards, but only while taking damage. this isn't relevant anyway though
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, {[]{return logic->CanUse(RG_HOOKSHOT) || ctx->GetTrickOption(RT_GANON_MQ_LIGHT_TRIAL);}}),
                  Entrance(RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,         {[]{return logic->SmallKeys(RR_GANONS_CASTLE, 3) && (ctx->GetTrickOption(RT_LENS_GANON_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && 
                                                                                          (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanJumpslash() || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_BOOMERANG));}}),
  });

  areaTable[RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM] = Region("Ganon's Castle MQ Light Trial Final Room", "Ganons Castle", {RA_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->LightTrialClear, {[]{return logic->CanUse(RG_LIGHT_ARROWS);}}),
  }, {}, {});
  }
}
