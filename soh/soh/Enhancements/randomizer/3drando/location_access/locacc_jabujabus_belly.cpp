#include "../location_access.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Rando;

void AreaTable_Init_JabuJabusBelly() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_JABU_JABUS_BELLY_ENTRYWAY] = Area("Jabu Jabus Belly Entryway", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_BEGINNING,    {[]{return randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla();}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsMQ();}}),
                  Entrance(RR_ZORAS_FOUNTAIN,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla()) {
  areaTable[RR_JABU_JABUS_BELLY_BEGINNING] = Area("Jabu Jabus Belly Beginning", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY,    {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return logic->CanUseProjectile;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_MIDDLE] = Area("Jabu Jabus Belly Lift Middle", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_BEGINNING,      {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM, {[]{return HasAccessTo(RR_JABU_JABUS_BELLY_LIFT_UPPER) || (randoCtx->GetTrickOption(RT_JABU_BOSS_HOVER) && logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS));}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAIN_UPPER] = Area("Jabu Jabus Belly Main Upper", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER,      {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BIGOCTO_ROOM,    {[]{return Here(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAIN_LOWER] = Area("Jabu Jabus Belly Main Lower", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, {[]{return logic->HookshotOrBoomerang;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, {[]{return logic->HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LOWER_SIDE_ROOM,  {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR] = Area("Jabu Jabus Belly Shabomb Corridor", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER, {[]{return logic->Swim;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER, {[]{return logic->Swim && logic->CanUseProjectile;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LOWER_SIDE_ROOM] = Area("Jabu Jabus Belly Lower Side Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return logic->FairyPot || (logic->CanUse(RG_BOOMERANG) || logic->CanUse(RG_HOVER_BOOTS));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_LOWER] = Area("Jabu Jabus Belly Lift Lower", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_DEKU_SCRUB, {[]{return logic->Swim && (logic->IsChild || logic->CanDive || randoCtx->GetTrickOption(RT_JABU_ALCOVE_JUMP_DIVE) || logic->CanUse(RG_IRON_BOOTS)) && logic->CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE,      {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_FORKED_CORRIDOR] = Area("Jabu Jabus Belly Forked Corridor", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOOMERANG_ROOM, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MAP_ROOM,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_COMPASS_ROOM,   {[]{return Here(RR_JABU_JABUS_BELLY_MAP_ROOM,      []{return logic->CanUse(RG_BOOMERANG);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_BLUE_TENTACLE,  {[]{return Here(RR_JABU_JABUS_BELLY_MAP_ROOM,      []{return logic->CanUse(RG_BOOMERANG);});}}),
                  Entrance(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, {[]{return Here(RR_JABU_JABUS_BELLY_BLUE_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BOOMERANG_ROOM] = Area("Jabu Jabus Belly Boomerang Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MAP_ROOM] = Area("Jabu Jabus Belly Map Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_MAP_CHEST, {[]{return logic->CanUse(RG_BOOMERANG);}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_COMPASS_ROOM] = Area("Jabu Jabus Belly Compass Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_COMPASS_CHEST, {[]{return logic->CanAdultAttack || logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BLUE_TENTACLE] = Area("Jabu Jabus Belly Blue Tentacle", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(RR_JABU_JABUS_BELLY_BLUE_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_GREEN_TENTACLE] = Area("Jabu Jabus Belly Green Tentacle", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(RR_JABU_JABUS_BELLY_GREEN_TENTACLE, []{return logic->CanUse(RG_BOOMERANG);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_BIGOCTO_ROOM] = Area("Jabu Jabus Belly Bigocto Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MAIN_LOWER,    {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, {[]{return Here(RR_JABU_JABUS_BELLY_BIGOCTO_ROOM, []{return (logic->CanUse(RG_BOOMERANG) || logic->Nuts) && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_STICKS));});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Area("Jabu Jabus Belly Above Bigocto", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
                  EventAccess(&logic->NutPot,   {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_UPPER, {[]{return logic->CanUse(RG_BOOMERANG);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_LIFT_UPPER] = Area("Jabu Jabus Belly Lift Upper", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_LOWER,  {[]{return true;}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Area("Jabu Jabus Belly Near Boss Room", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, {[]{return logic->CanAdultAttack || logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, {[]{return logic->CanUse(RG_BOOMERANG) || (randoCtx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && ((logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_FAIRY_BOW))) || (logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT)))) || (randoCtx->GetTrickOption(RT_JABU_NEAR_BOSS_EXPLOSIVES) && (logic->HasBombchus || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->Bombs)));}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsMQ()) {
  areaTable[RR_JABU_JABUS_BELLY_MQ_BEGINNING] = Area("Jabu Jabus Belly MQ Beginning", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,             {[]{return logic->CanBlastOrSmash;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, {[]{return logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_MAIN,  {[]{return Here(RR_JABU_JABUS_BELLY_MQ_BEGINNING, []{return logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT);});}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_MAIN] = Area("Jabu Jabus Belly MQ Main", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,      {[]{return true;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,      {[]{return (logic->IsAdult && (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT))) || ChildCanAccess(RR_JABU_JABUS_BELLY_MQ_BOSS_AREA);}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST,                {[]{return (logic->IsChild || logic->CanDive || logic->CanUse(RG_IRON_BOOTS) || randoCtx->GetTrickOption(RT_JABU_ALCOVE_JUMP_DIVE)) && (logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->HasBombchus || (randoCtx->GetTrickOption(RT_JABU_MQ_RANG_JUMP) && logic->CanUse(RG_BOOMERANG)));}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,   {[]{return true;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,              {[]{return logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_STICKS) || logic->Bombs;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,      {[]{return logic->CanUse(RG_SONG_OF_TIME) || (randoCtx->GetTrickOption(RT_JABU_MQ_SOT_GS) && logic->IsChild && logic->CanUse(RG_BOOMERANG));}}),
                    //Trick: logic->CanUse(RG_SONG_OF_TIME) || (LogicJabuMQSoTGS && logic->IsChild && logic->CanUse(RG_BOOMERANG))
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_DEPTHS,    {[]{return logic->HasExplosives && logic->CanUse(RG_FAIRY_SLINGSHOT) && logic->CanUse(RG_BOOMERANG);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_DEPTHS] = Area("Jabu Jabus Belly MQ Depths", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,          {[]{return logic->Swim && (logic->Sticks || logic->CanUse(RG_DINS_FIRE));}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,    {[]{return (randoCtx->GetTrickOption(RT_LENS_JABU_MQ) || logic->CanUse(RG_LENS_OF_TRUTH)) || Here(RR_JABU_JABUS_BELLY_MQ_MAIN, []{return logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && logic->CanUse(RG_HOOKSHOT);});}}),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_MAIN,      {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_MQ_BOSS_AREA, {[]{return logic->Sticks || (logic->CanUse(RG_DINS_FIRE) && logic->KokiriSword);}}),
  });

  areaTable[RR_JABU_JABUS_BELLY_MQ_BOSS_AREA] = Area("Jabu Jabus Belly MQ Boss Area", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_COW,             {[]{return logic->CanUse(RG_EPONAS_SONG);}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,    {[]{return logic->CanUse(RG_BOOMERANG) || (randoCtx->GetTrickOption(RT_JABU_NEAR_BOSS_RANGED) && logic->CanUse(RG_HOOKSHOT));}}),
  }, {
                  //Exits
                  Entrance(RR_JABU_JABUS_BELLY_MQ_MAIN, {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, {[]{return logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY] =
        Area("Jabu Jabus Belly Boss Entryway", "Jabu Jabus Belly", RA_JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM, { [] { return randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsVanilla(); } }),
                 Entrance(RR_JABU_JABUS_BELLY_MQ_BOSS_AREA, { [] { return randoCtx->GetDungeon(JABU_JABUS_BELLY)->IsMQ(); } }),
                 Entrance(RR_JABU_JABUS_BELLY_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_JABU_JABUS_BELLY_BOSS_ROOM] =
        Area("Jabu Jabus Belly Boss Room", "Jabu Jabus Belly", RA_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events //todo: add pot kill trick
                 EventAccess(&logic->JabuJabusBellyClear,
                             { [] { return logic->JabuJabusBellyClear || (logic->HasBossSoul(RG_BARINADE_SOUL) && (logic->CanUse(RG_BOOMERANG) && logic->CanJumpslash)); } }),
             },
             {
                 // Locations
                 LocationAccess(RC_JABU_JABUS_BELLY_BARINADE_HEART, { [] { return logic->JabuJabusBellyClear; } }),
                 LocationAccess(RC_BARINADE, { [] { return logic->JabuJabusBellyClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(RR_ZORAS_FOUNTAIN, { [] { return logic->JabuJabusBellyClear; } }),
             });
}
