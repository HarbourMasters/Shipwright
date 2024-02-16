#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_JabuJabusBelly() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[JABU_JABUS_BELLY_ENTRYWAY] = Area("Jabu Jabus Belly Entryway", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_BEGINNING,    {[]{return Dungeon::JabuJabusBelly.IsVanilla();}}),
                  Entrance(JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return Dungeon::JabuJabusBelly.IsMQ();}}),
                  Entrance(ZORAS_FOUNTAIN,                {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::JabuJabusBelly.IsVanilla()) {
  areaTable[JABU_JABUS_BELLY_BEGINNING] = Area("Jabu Jabus Belly Beginning", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY,    {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return CanUseProjectile;}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_MIDDLE] = Area("Jabu Jabus Belly Lift Middle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_BEGINNING,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_NEAR_BOSS_ROOM, {[]{return HasAccessTo(JABU_JABUS_BELLY_LIFT_UPPER) || (LogicJabuBossHover && IsAdult && CanUse(HOVER_BOOTS));}}),
  });

  areaTable[JABU_JABUS_BELLY_MAIN_UPPER] = Area("Jabu Jabus Belly Main Upper", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BIGOCTO_ROOM,    {[]{return Here(JABU_JABUS_BELLY_GREEN_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_MAIN_LOWER] = Area("Jabu Jabus Belly Main Lower", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, {[]{return HookshotOrBoomerang;}}),
                  LocationAccess(JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LOWER_SIDE_ROOM,  {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_SHABOMB_CORRIDOR] = Area("Jabu Jabus Belly Shabomb Corridor", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER, {[]{return CanUseProjectile;}}),
  });

  areaTable[JABU_JABUS_BELLY_LOWER_SIDE_ROOM] = Area("Jabu Jabus Belly Lower Side Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return FairyPot || (CanUse(BOOMERANG) || CanUse(HOVER_BOOTS));}}),
                }, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_LOWER] = Area("Jabu Jabus Belly Lift Lower", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_DEKU_SCRUB, {[]{return (IsChild || CanDive || LogicJabuAlcoveJumpDive || CanUse(IRON_BOOTS)) && CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_SHABOMB_CORRIDOR, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE,      {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_FORKED_CORRIDOR] = Area("Jabu Jabus Belly Forked Corridor", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_UPPER,     {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BOOMERANG_ROOM, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MAP_ROOM,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_COMPASS_ROOM,   {[]{return Here(JABU_JABUS_BELLY_MAP_ROOM,      []{return CanUse(BOOMERANG);});}}),
                  Entrance(JABU_JABUS_BELLY_BLUE_TENTACLE,  {[]{return Here(JABU_JABUS_BELLY_MAP_ROOM,      []{return CanUse(BOOMERANG);});}}),
                  Entrance(JABU_JABUS_BELLY_GREEN_TENTACLE, {[]{return Here(JABU_JABUS_BELLY_BLUE_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_BOOMERANG_ROOM] = Area("Jabu Jabus Belly Boomerang Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_BOOMERANG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_MAP_ROOM] = Area("Jabu Jabus Belly Map Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MAP_CHEST, {[]{return CanUse(BOOMERANG);}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_COMPASS_ROOM] = Area("Jabu Jabus Belly Compass Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_COMPASS_CHEST, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_BLUE_TENTACLE] = Area("Jabu Jabus Belly Blue Tentacle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(JABU_JABUS_BELLY_BLUE_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_GREEN_TENTACLE] = Area("Jabu Jabus Belly Green Tentacle", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_FORKED_CORRIDOR, {[]{return Here(JABU_JABUS_BELLY_GREEN_TENTACLE, []{return CanUse(BOOMERANG);});}}),
  });

  areaTable[JABU_JABUS_BELLY_BIGOCTO_ROOM] = Area("Jabu Jabus Belly Bigocto Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MAIN_LOWER,    {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_ABOVE_BIGOCTO, {[]{return Here(JABU_JABUS_BELLY_BIGOCTO_ROOM, []{return (CanUse(BOOMERANG) || Nuts) && (CanUse(KOKIRI_SWORD) || CanUse(STICKS));});}}),
  });

  areaTable[JABU_JABUS_BELLY_ABOVE_BIGOCTO] = Area("Jabu Jabus Belly Above Bigocto", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_UPPER, {[]{return CanUse(BOOMERANG);}}),
  });

  areaTable[JABU_JABUS_BELLY_LIFT_UPPER] = Area("Jabu Jabus Belly Lift Upper", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_LIFT_LOWER,  {[]{return true;}}),
  });

  areaTable[JABU_JABUS_BELLY_NEAR_BOSS_ROOM] = Area("Jabu Jabus Belly Near Boss Room", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_GS_NEAR_BOSS, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_LIFT_MIDDLE, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BOSS_ENTRYWAY, {[]{return CanUse(BOOMERANG) || (LogicJabuNearBossRanged && ((IsAdult && (CanUse(HOOKSHOT) || CanUse(BOW))) || (IsChild && CanUse(SLINGSHOT)))) || (LogicJabuNearBossExplosives && (HasBombchus || (IsAdult && CanUse(HOVER_BOOTS) && Bombs)));}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::JabuJabusBelly.IsMQ()) {
  areaTable[JABU_JABUS_BELLY_MQ_BEGINNING] = Area("Jabu Jabus Belly MQ Beginning", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_MAP_CHEST,             {[]{return CanBlastOrSmash;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, {[]{return IsChild && CanUse(SLINGSHOT);}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN,  {[]{return Here(JABU_JABUS_BELLY_MQ_BEGINNING, []{return IsChild && CanUse(SLINGSHOT);});}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_MAIN] = Area("Jabu Jabus Belly MQ Main", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,      {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,      {[]{return (IsAdult && (CanUse(HOVER_BOOTS) || CanUse(HOOKSHOT))) || ChildCanAccess(JABU_JABUS_BELLY_MQ_BOSS_AREA);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_COMPASS_CHEST,                {[]{return (IsChild || CanDive || CanUse(IRON_BOOTS) || LogicJabuAlcoveJumpDive) && (CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(HOOKSHOT) || HasBombchus || (LogicJabuMQRangJump && CanUse(BOOMERANG)));}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,    {[]{return CanUse(SLINGSHOT);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, {[]{return CanUse(SLINGSHOT);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,   {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,              {[]{return CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || CanUse(SLINGSHOT) || CanUse(BOW) || CanUse(STICKS) || Bombs;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,      {[]{return CanPlay(SongOfTime) || (LogicJabuMQSoTGS && IsChild && CanUse(BOOMERANG));}}),
                    //Trick: CanPlay(SongOfTime) || (LogicJabuMQSoTGS && IsChild && CanUse(BOOMERANG))
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_BEGINNING, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_DEPTHS,    {[]{return HasExplosives && CanUse(SLINGSHOT) && CanUse(BOOMERANG);}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_DEPTHS] = Area("Jabu Jabus Belly MQ Depths", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, {[]{return true;}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,          {[]{return Sticks || CanUse(DINS_FIRE);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,    {[]{return (LogicLensJabuMQ || CanUse(LENS_OF_TRUTH)) || Here(JABU_JABUS_BELLY_MQ_MAIN, []{return IsAdult && CanUse(HOVER_BOOTS) && CanUse(HOOKSHOT);});}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN,      {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_MQ_BOSS_AREA, {[]{return Sticks || (CanUse(DINS_FIRE) && KokiriSword);}}),
  });

  areaTable[JABU_JABUS_BELLY_MQ_BOSS_AREA] = Area("Jabu Jabus Belly MQ Boss Area", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(JABU_JABUS_BELLY_MQ_COW,             {[]{return CanPlay(EponasSong);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, {[]{return CanUse(SLINGSHOT);}}),
                  LocationAccess(JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,    {[]{return CanUse(BOOMERANG) || (LogicJabuNearBossRanged && CanUse(HOOKSHOT));}}),
  }, {
                  //Exits
                  Entrance(JABU_JABUS_BELLY_MQ_MAIN, {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_BOSS_ENTRYWAY, {[]{return CanUse(SLINGSHOT);}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[JABU_JABUS_BELLY_BOSS_ENTRYWAY] =
        Area("Jabu Jabus Belly Boss Entryway", "Jabu Jabus Belly", JABU_JABUS_BELLY, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(JABU_JABUS_BELLY_NEAR_BOSS_ROOM, { [] { return Dungeon::JabuJabusBelly.IsVanilla(); } }),
                 Entrance(JABU_JABUS_BELLY_MQ_BOSS_AREA, { [] { return Dungeon::JabuJabusBelly.IsMQ(); } }),
                 Entrance(JABU_JABUS_BELLY_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[JABU_JABUS_BELLY_BOSS_ROOM] =
        Area("Jabu Jabus Belly Boss Room", "Jabu Jabus Belly", NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events //todo: add pot kill trick
                 EventAccess(&JabuJabusBellyClear,
                             { [] { return JabuJabusBellyClear || (CanUse(BOOMERANG) && CanJumpslash); } }),
             },
             {
                 // Locations
                 LocationAccess(JABU_JABUS_BELLY_BARINADE_HEART, { [] { return JabuJabusBellyClear; } }),
                 LocationAccess(BARINADE, { [] { return JabuJabusBellyClear; } }),
             },
             {
                 // Exits
                 Entrance(JABU_JABUS_BELLY_BOSS_ENTRYWAY, { [] { return false; } }),
                 Entrance(ZORAS_FOUNTAIN, { [] { return JabuJabusBellyClear; } }),
             });
}
