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
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEGINNING, {[]{return ctx->GetDungeon(SHADOW_TEMPLE)->IsMQ();}}),
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
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,         {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM,  {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM] = Region("Shadow Temple MQ Spinner Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return Here(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM, []{return logic->CanUse(RG_HOVER_BOOTS) || (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}) &&
                                                                          (logic->CanUse(RG_HOVER_BOOTS) || Here(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM, []{return logic->CanUse(RG_FIRE_ARROWS);}) || (ctx->GetTrickOption(RT_SHADOW_MQ_GAP) && logic->CanUse(RG_LONGSHOT) && logic->CanJumpslashExceptHammer()));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, {[]{return Here(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM, []{return logic->HasExplosives();}) && logic->SmallKeys(RR_SHADOW_TEMPLE, 6) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

//Assumes we're in the "main" area and needed lens to enter. logic will need changes if a void warp puts us somewhere weird
  areaTable[RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA] = Region("Shadow Temple MQ Dead Hand Region", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,     logic->CanKillEnemy(RE_REDEAD)),
                  //There's a shared flag tied to some glass here. eye target here and killing an enemy group later in the dungeon toggles. I'm building the logic as "intended", assuming the switch needs flipping
                  LOCATION(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, logic->CanKillEnemy(RE_DEAD_HAND) && (logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanHitEyeTargets()),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM, {[]{return true;}}),
  });

//also includes the B2 gibdo room
  areaTable[RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS] = Region("Shadow Temple MQ First Beamos", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //Doing this sets the shared flag for the glass in RR_SHADOW_TEMPLE_MQ_DEAD_HAND_AREA, but doesn't seem to affect the chest
                  LOCATION(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,  logic->CanKillEnemy(RE_GIBDO) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,         {[]{return logic->HasExplosives() && logic->SmallKeys(RR_SHADOW_TEMPLE, 2);}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, {[]{return ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM] = Region("Shadow Temple MQ B2 Spinning Blade Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_MAP_CHEST, logic->CanPassEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,  {[]{return Here(RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, []{return logic->CanKillEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS)));});}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH, {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH] = Region("Shadow Temple MQ Shortcut Path", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA);}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_DOCK,                   {[]{return logic->ShadowShortcutBlock;}}),
                  //WARNING if there's any way past here to ship without already reaching the other side the key logic in this dungeon becomes Quantum
  });

//Room exists for if it's ever possible to go backwards or void warp into the middle of shadow
  areaTable[RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR] = Region("Shadow Temple MQ B2 to B3 Corridor", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,   {[]{return logic->HasExplosives() && logic->SmallKeys(RR_SHADOW_TEMPLE, 2);}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return true;}}),
                  //bunnyhovers + lens lets you go from the very top of upper pit to the stationary invisible platform below quite easily
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT] = Region("Shadow Temple MQ Upper Huge Pit", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT,        {[]{return (logic->HasFireSource() && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))) || ctx->GetTrickOption(RT_SHADOW_MQ_HUGE_PIT);}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM, {[]{return ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM] = Region("Shadow Temple MQ Invisible Blades Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //RT_SHADOW_MQ_INVISIBLE_BLADES does not work with NL as like-likes will not swallow you, likewise like-likes will not spit you with a fairy revive
                  //you take half a heart base from a spit out, double check EffectiveHealth when damage logic gets reworked
                  //Child is too small to get hit by the blades doesn't need the trick or lens for dodging them
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,   (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1)) &&
                                                                                 (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) || logic->IsChild || logic->CanUse(RG_NAYRUS_LOVE) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, (logic->CanUse(RG_SONG_OF_TIME) || (ctx->GetTrickOption(RT_SHADOW_MQ_INVISIBLE_BLADES) && logic->EffectiveHealth() > 1)) && 
                                                                                 ((ctx->GetTrickOption(RT_LENS_SHADOW_MQ) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES) || logic->IsChild || logic->CanUse(RG_NAYRUS_LOVE))) || logic->CanUse(RG_LENS_OF_TRUTH))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, {[]{return true;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT] = Region("Shadow Temple MQ Lower Huge Pit", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, logic->CanUse(RG_LONGSHOT)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FALLING_SPIKES_ROOM,   {[]{return Here(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, []{return logic->CanJumpslash() || logic->HasExplosives();});}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, {[]{return logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ_PLATFORM) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->SmallKeys(RR_SHADOW_TEMPLE, 3);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_FALLING_SPIKES_ROOM] = Region("Shadow Temple MQ Falling Spikes Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,  true),
                  //Assuming the known setup for RT_SHADOW_UMBRELLA and RT_SHADOW_UMBRELLA_GS, probably possible without sword + shield.
                  //Handling the trick here instead of upper as using the block to climb is not a valid method for getting this skull without other tricks to use the block before it is intended
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,      logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG) ||
                                                                            (ctx->GetTrickOption(RT_SHADOW_UMBRELLA_GS) && ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanStandingShield() && logic->CanUse(RG_MASTER_SWORD))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT, {[]{return Here(RR_SHADOW_TEMPLE_MQ_FALLING_SPIKES_ROOM, []{return ctx->GetTrickOption(RT_VISIBLE_COLLISION) || logic->CanHitSwitch();});}}),
                  //Assuming the known setup for RT_SHADOW_UMBRELLA, probably possible without sword + shield
                  Entrance(RR_SHADOW_TEMPLE_MQ_UPPER_FALLING_SPIKES_ROOM, {[]{return logic->IsAdult && (logic->HasItem(RG_GORONS_BRACELET) || (ctx->GetTrickOption(RT_SHADOW_UMBRELLA) && logic->CanUse(RG_HOVER_BOOTS) && logic->CanStandingShield() && logic->CanUse(RG_MASTER_SWORD)));}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_UPPER_FALLING_SPIKES_ROOM] = Region("Shadow Temple MQ Upper Falling Spikes Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,  true),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, true),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FALLING_SPIKES_ROOM, {[]{return true;}}),
  });

//while the spikes here are annoying, they don't really stop you doing anything, so I'll assume either lens trick, lens to see them, or taking damage from them. Not hovers though as a new player won't see the threat without lens to react properly            
  areaTable[RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM] = Region("Shadow Temple MQ Floor Spikes Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events                                        //lens or trick is always required for hookshot targets. We handle it here to not complicate the RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_UPPER_DOOR logic
                  EventAccess(&logic->MQShadowFloorSpikeRupees, {[]{return (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) &&
                                                                  //Upper door side high rupee needs (hookshot and redead kill(as either age) for chest and adult) or longshot. hovers can cross from the left side with a backflip but that would be a trick
                                                                  //East midair rupee needs (hookshot and(hover boots or jumpslash from the upper door platform)) or longshot.
                                                                  //Combined these are longshot or (IsAdult && hookshot && (CanJumpslash || (Hover Boots && Here(CanKillRedeads))))
                                                                  (logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->CanJumpslash() || (logic->CanUse(RG_HOVER_BOOTS) && Here(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, []{return logic->CanKillEnemy(RE_REDEAD);}))))) &&
                                                                  //1 rupee is in spikes, needs hovers or damage
                                                                  (logic->TakeDamage() || logic->CanUse(RG_HOVER_BOOTS));}}),
  }, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, logic->CanKillEnemy(RE_REDEAD) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->TakeDamage() || logic->CanUse(RG_LENS_OF_TRUTH))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM, {[]{return logic->MQShadowFloorSpikeRupees;}}),
                                                             //We need to assume we can get here with or without the glass platforms
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL,  {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 4) &&
                                                                       (logic->CanUse(RG_LONGSHOT) || (logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && (logic->MQShadowFloorSpikeRupees || Here(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, []{return logic->CanKillEnemy(RE_REDEAD);})))) && 
                                                                       (logic->CanJumpslash() || logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM] = Region("Shadow Temple MQ Stalfos Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, {[]{return Here(RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM, []{return logic->CanKillEnemy(RE_STALFOS, ED_CLOSE, true, 2);});}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL] = Region("Shadow Temple MQ Wind Tunnel", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 4) && logic->CanPassEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM,    {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM,     {[]{return logic->CanPassEnemy(RE_BIG_SKULLTULA) && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM] = Region("Shadow Temple MQ Wind Hint Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,   (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) && logic->CanPassEnemy(RE_REDEAD)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return true;}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM] = Region("Shadow Temple MQ B4 Gibdo Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,  logic->CanKillEnemy(RE_GIBDO)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, logic->HasExplosives() && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,           logic->HasExplosives()),
  }, {
                  //Exits
                  //child can make it using the wind strat
                  Entrance(RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL, {[]{return (ctx->GetTrickOption(RT_SHADOW_MQ_WINDY_WALKWAY)) || logic->CanUse(RG_HOVER_BOOTS);}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_DOCK,        {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 5);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_DOCK] = Region("Shadow Temple MQ Dock", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ShadowShortcutBlock, {[]{return logic->HasItem(RG_GORONS_BRACELET);}}),
  }, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_SHORTCUT_PATH, {[]{return logic->ShadowShortcutBlock;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM, {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 5);}}),
                  //funnily enough, the wheel jump seems to be in logic as there's no strength requirement in N64
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,   {[]{return (logic->IsAdult || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_ZELDAS_LULLABY);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT] = Region("Shadow Temple MQ Beyond Boat", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //It's a trick on N64 to kill this and drop down to collect this with normal weapons, as doing so without the statue dropped doing voids you to before the boat
                  //hilariously, you can also hit this with a pot before you bring down the statue, but there's no great way to reset it without crossing. the statues collision is very inconvenient afterwards
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP, logic->CanGetEnemyDrop(RE_GOLD_SKULLTULA, ED_BOOMERANG)),
                  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM,   {[]{return Here(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, []{return logic->CanUse(RG_FAIRY_BOW) || (ctx->GetTrickOption(RT_SHADOW_STATUE) && logic->CanUse(RG_BOMBCHU_5));});}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM] = Region("Shadow Temple MQ Across Chasm", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,    {[]{return Here(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM, []{return logic->CanDetonateUprightBombFlower();}) && logic->IsAdult;}}),
                  //assumes RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT by previous access. If backwards shadow ever exists remember that child cannot jump onto the statue from this side and make an event for the switch
                  //Lens isn't needed to reach it but is needed to navigate the next room
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, {[]{return Here(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM, []{return logic->CanHitEyeTargets() && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_LONGSHOT);});}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_BOSS_DOOR,      {[]{return logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_BOSS_DOOR] = Region("Shadow Temple MQ Boss Door", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //you can drop onto this and the respawn is reasonable
                  LOCATION(RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,  (logic->CanKillEnemy(RE_GOLD_SKULLTULA, ED_BOOMERANG) || logic->CanUse(RG_MEGATON_HAMMER)) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH))),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM, {[]{return logic->CanUse(RG_HOVER_BOOTS) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),
                  Entrance(RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,   {[]{return logic->HasItem(RG_SHADOW_TEMPLE_BOSS_KEY);}}),
  });

  //Assumes lens is checked on entry
  areaTable[RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE] = Region("Shadow Temple MQ Invisible Maze", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, (logic->CanUse(RG_LENS_OF_TRUTH) || ctx->GetTrickOption(RT_LENS_SHADOW_MQ_DEADHAND)) && logic->CanKillEnemy(RE_DEAD_HAND) && logic->CanDetonateUprightBombFlower()),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,  true),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT,        {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM,   {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 6);}}),
  });

  areaTable[RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM] = Region("Shadow Temple MQ Spike Walls Room", "Shadow Temple", {RA_SHADOW_TEMPLE}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, logic->CanUse(RG_DINS_FIRE)),
                  LOCATION(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,         logic->CanUse(RG_DINS_FIRE)),
  }, {
                  //Exits
                  Entrance(RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,  {[]{return logic->SmallKeys(RR_SHADOW_TEMPLE, 6) && (ctx->GetTrickOption(RT_LENS_SHADOW_MQ) || logic->CanUse(RG_LENS_OF_TRUTH));}}),});
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
