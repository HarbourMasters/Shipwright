#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_DekuTree() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[DEKU_TREE_ENTRYWAY] = Area("Deku Tree Entryway", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,      {[]{return Dungeon::DekuTree.IsVanilla();}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,   {[]{return Dungeon::DekuTree.IsMQ();}}),
                  Entrance(KF_OUTSIDE_DEKU_TREE, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::DekuTree.IsVanilla()) {
  areaTable[DEKU_TREE_LOBBY] = Area("Deku Tree Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(DEKU_TREE_2F_MIDDLE_ROOM,    {[]{return true;}}),
                  Entrance(DEKU_TREE_COMPASS_ROOM,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_LOWER,    {[]{return Here(DEKU_TREE_LOBBY, []{return IsAdult || CanChildAttack || Nuts;});},
                                             /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return false;},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE);}}),
                  Entrance(DEKU_TREE_BOSS_ENTRYWAY,     {[]{return false;},
                                             /*Glitched*/[]{return IsChild && CanUse(KOKIRI_SWORD) && Nuts && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::EXPERT);}}),
  });

  areaTable[DEKU_TREE_2F_MIDDLE_ROOM] = Area("Deku Tree 2F Middle Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,         {[]{return Here(DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(MEGATON_HAMMER);});}}),
                  Entrance(DEKU_TREE_SLINGSHOT_ROOM,{[]{return Here(DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(MEGATON_HAMMER);});}}),
  });

  areaTable[DEKU_TREE_SLINGSHOT_ROOM] = Area("Deku Tree Slingshot Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_SLINGSHOT_CHEST,           {[]{return true;}}),
                  LocationAccess(DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_2F_MIDDLE_ROOM, {[]{return CanUse(SLINGSHOT) || CanUse(HOVER_BOOTS);},
                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[DEKU_TREE_COMPASS_ROOM] = Area("Deku Tree Compass Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_COMPASS_CHEST,           {[]{return true;}}),
                  LocationAccess(DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, {[]{return true;}}),
                  LocationAccess(DEKU_TREE_GS_COMPASS_ROOM,         {[]{return IsAdult || CanChildAttack;},
                                                         /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,     {[]{return HasFireSourceWithTorch || CanUse(BOW);}}),
                  Entrance(DEKU_TREE_BOSS_ENTRYWAY, {[]{return false;},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[DEKU_TREE_BASEMENT_LOWER] = Area("Deku Tree Basement Lower", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(DEKU_TREE_BASEMENT_CHEST,    {[]{return true;}}),
                  LocationAccess(DEKU_TREE_GS_BASEMENT_GATE,  {[]{return IsAdult || CanChildAttack;},
                                                   /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                  LocationAccess(DEKU_TREE_GS_BASEMENT_VINES, {[]{return CanUseProjectile || CanUse(DINS_FIRE) || (LogicDekuBasementGS && (IsAdult || Sticks || KokiriSword));},
                                                   /*Glitched*/[]{return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_LOBBY,               {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return Here(DEKU_TREE_BASEMENT_LOWER, []{return HasFireSourceWithTorch || CanUse(BOW);});}}),
                  Entrance(DEKU_TREE_BASEMENT_UPPER,      {[]{return IsAdult || LogicDekuB1Skip || HasAccessTo(DEKU_TREE_BASEMENT_UPPER);},
                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return false;},
                                               /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[DEKU_TREE_BASEMENT_SCRUB_ROOM] = Area("Deku Tree Basement Scrub Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return Here(DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return CanUse(SLINGSHOT) || CanUse(BOW);});}}),
  });

  areaTable[DEKU_TREE_BASEMENT_WATER_ROOM] = Area("Deku Tree Basement Water Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_TORCH_ROOM] = Area("Deku Tree Basement Torch Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return Here(DEKU_TREE_BASEMENT_TORCH_ROOM, []{return HasFireSourceWithTorch || CanUse(BOW);});}}),
  });

  areaTable[DEKU_TREE_BASEMENT_BACK_LOBBY] = Area("Deku Tree Basement Back Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(BOW);}) &&
                                                                            (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE)));},
                                    /*Glitched*/[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(BOW);}) && CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_ROOM,  {[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) &&
                                                                     Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return CanBlastOrSmash;});},
                                               /*Glitched*/[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) &&
                                                                     Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return (GlitchBlueFireWall && BlueFire) || (CanUse(STICKS) && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT));});}}),
                  Entrance(DEKU_TREE_BASEMENT_UPPER,      {[]{return Here(DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(Bow);}) && IsChild;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_BACK_ROOM] = Area("Deku Tree Basement Back Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_GS_BASEMENT_BACK_ROOM, {[]{return HookshotOrBoomerang;},
                                                       /*Glitched*/[]{return Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE);}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_BASEMENT_UPPER] = Area("Deku Tree Basement Upper", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(MEGATON_HAMMER);}}),
                }, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return Here(DEKU_TREE_BASEMENT_UPPER, []{return HasFireSourceWithTorch || (LogicDekuB1WebsWithBow && IsAdult && CanUse(BOW));});}}),
  });

  areaTable[DEKU_TREE_OUTSIDE_BOSS_ROOM] = Area("Deku Tree Outside Boss Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DEKU_TREE_BASEMENT_UPPER, {[]{return true;}}),
                  Entrance(DEKU_TREE_BOSS_ENTRYWAY,  {[]{return Here(DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return HasShield;});}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::DekuTree.IsMQ()) {
  areaTable[DEKU_TREE_MQ_LOBBY] = Area("Deku Tree MQ Lobby", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(DEKU_TREE_MQ_SLINGSHOT_CHEST,           {[]{return IsAdult || CanChildAttack;}}),
                  LocationAccess(DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));}}),
                  LocationAccess(DEKU_TREE_MQ_BASEMENT_CHEST,            {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));}}),
                  LocationAccess(DEKU_TREE_MQ_GS_LOBBY,                  {[]{return IsAdult || CanChildAttack;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY,                     {[]{return true;}}),
                  Entrance(DEKU_TREE_MQ_COMPASS_ROOM,              {[]{return Here(DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW));}) &&
                                                                               Here(DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch || (IsAdult && CanUse(BOW));});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(SLINGSHOT)) || (IsAdult && CanUse(BOW));}) &&
                                                                               Here(DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch;});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_LEDGE,            {[]{return LogicDekuB1Skip || Here(DEKU_TREE_MQ_LOBBY, []{return IsAdult;});}}),
  });

  areaTable[DEKU_TREE_MQ_COMPASS_ROOM] = Area("Deku Tree MQ Compass Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_COMPASS_CHEST,   {[]{return true;}}),
                  LocationAccess(DEKU_TREE_MQ_GS_COMPASS_ROOM, {[]{return HookshotOrBoomerang &&
                                                                               Here(DEKU_TREE_MQ_COMPASS_ROOM, []{return HasBombchus ||
                                                                               (Bombs && (CanPlay(SongOfTime) || IsAdult)) ||
                                                                               (IsAdult && CanUse(MEGATON_HAMMER) && (CanPlay(SongOfTime) /*|| LogicDekuMQCompassGS*/));});}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_LOBBY, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return /*LogicDekuMQLog || */ (IsChild && (DekuShield || HylianShield)) ||
                                                                             (IsAdult && (CanUse(LONGSHOT) || (CanUse(HOOKSHOT) && CanUse(IRON_BOOTS))));}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,                    {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, {[]{return CanPlay(SongOfTime);}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_BACK_ROOM,        {[]{return Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return (IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) ||
                                                                               Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return IsAdult && CanUse(FIRE_ARROWS);});}) &&
                                                                                 Here(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return IsAdult || KokiriSword || CanUseProjectile || (Nuts && Sticks);});}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return true;}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Area("Deku Tree MQ Basement Back Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, {[]{return (IsAdult && CanUse(LONGSHOT)) || (CanPlay(SongOfTime) && HookshotOrBoomerang);}}),
                  LocationAccess(DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,   {[]{return HasFireSourceWithTorch && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_LEDGE,           {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return (IsChild && CanUse(KOKIRI_SWORD)) || CanUseProjectile || (Nuts && (IsChild && CanUse(STICKS)));}}),
  });

  areaTable[DEKU_TREE_MQ_BASEMENT_LEDGE] = Area("Deku Tree MQ Basement Ledge", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_TREE_MQ_DEKU_SCRUB, {[]{return CanStunDeku;}}),
  }, {
                  //Exits
                  Entrance(DEKU_TREE_MQ_BASEMENT_BACK_ROOM, {[]{return IsChild;}}),
                  Entrance(DEKU_TREE_MQ_LOBBY,              {[]{return true;}}),
                  Entrance(DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM,
                         { [] { return Here(DEKU_TREE_MQ_BASEMENT_LEDGE, [] { return HasFireSourceWithTorch; }); } }),
  });

    areaTable[DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM] =
            Area("Deku Tree MQ Outside Boss Room", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(DEKU_TREE_MQ_BASEMENT_LEDGE, { [] { return true; } }),
                     Entrance(DEKU_TREE_BOSS_ENTRYWAY,
                              { [] { return Here(DEKU_TREE_MQ_BASEMENT_LEDGE, [] { return HasShield; }); } }),
                 });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[DEKU_TREE_BOSS_ENTRYWAY] =
        Area("Deku Tree Boss Entryway", "Deku Tree", DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(DEKU_TREE_OUTSIDE_BOSS_ROOM, { [] { return Dungeon::DekuTree.IsVanilla(); } }),
                 Entrance(DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, { [] { return Dungeon::DekuTree.IsMQ(); } }),
                 Entrance(DEKU_TREE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[DEKU_TREE_BOSS_ROOM] =
        Area("Deku Tree Boss Room", "Deku Tree", NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&DekuTreeClear, { [] {
                                                  return DekuTreeClear ||
                                                         (CanJumpslash && (Nuts || CanUse(SLINGSHOT) || CanUse(BOW) ||
                                                                           HookshotOrBoomerang));
                                              },
                                               /*Glitched*/
                                               [] { return CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE); } }),
             },
             {
                 // Locations
                 LocationAccess(QUEEN_GOHMA, { [] { return DekuTreeClear; } }),
                 LocationAccess(DEKU_TREE_QUEEN_GOHMA_HEART, { [] { return DekuTreeClear; } }),
             },
             {
                 // Exits
                 Entrance(DEKU_TREE_BOSS_ENTRYWAY, { [] { return true; } }),
             });
}
