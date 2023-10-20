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
  areaTable[RR_DEKU_TREE_ENTRYWAY] = Area("Deku Tree Entryway", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,      {[]{return Dungeon::DekuTree.IsVanilla();}}),
                  Entrance(RR_DEKU_TREE_MQ_LOBBY,   {[]{return Dungeon::DekuTree.IsMQ();}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::DekuTree.IsVanilla()) {
  areaTable[RR_DEKU_TREE_LOBBY] = Area("Deku Tree Lobby", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM,    {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_COMPASS_ROOM,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,    {[]{return Here(RR_DEKU_TREE_LOBBY, []{return IsAdult || CanChildAttack || Nuts;});}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return false;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_2F_MIDDLE_ROOM] = Area("Deku Tree 2F Middle Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,         {[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(RG_MEGATON_HAMMER);});}}),
                  Entrance(RR_DEKU_TREE_SLINGSHOT_ROOM,{[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_DEKU_TREE_SLINGSHOT_ROOM] = Area("Deku Tree Slingshot Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_SLINGSHOT_CHEST,           {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM, {[]{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_DEKU_TREE_COMPASS_ROOM] = Area("Deku Tree Compass Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_COMPASS_CHEST,           {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_GS_COMPASS_ROOM,         {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,     {[]{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_LOWER] = Area("Deku Tree Basement Lower", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_BASEMENT_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_GATE,  {[]{return IsAdult || CanChildAttack;}}),
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_VINES, {[]{return CanUseProjectile || CanUse(RG_DINS_FIRE) || (LogicDekuBasementGS && (IsAdult || Sticks || KokiriSword));}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,               {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return Here(RR_DEKU_TREE_BASEMENT_LOWER, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return IsAdult || LogicDekuB1Skip || HasAccessTo(RR_DEKU_TREE_BASEMENT_UPPER);}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_SCRUB_ROOM] = Area("Deku Tree Basement Scrub Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return Here(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM] = Area("Deku Tree Basement Water Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_TORCH_ROOM] = Area("Deku Tree Basement Torch Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return Here(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_LOBBY] = Area("Deku Tree Basement Back Lobby", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) &&
                                                                            (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE)));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_ROOM,  {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) &&
                                                                     Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) && IsChild;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_ROOM] = Area("Deku Tree Basement Back Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_UPPER] = Area("Deku Tree Basement Upper", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return IsChild;}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return Here(RR_DEKU_TREE_BASEMENT_UPPER, []{return HasFireSourceWithTorch || (LogicDekuB1WebsWithBow && IsAdult && CanUse(RG_FAIRY_BOW));});}}),
  });

  areaTable[RR_DEKU_TREE_OUTSIDE_BOSS_ROOM] = Area("Deku Tree Outside Boss Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,  {[]{return Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return HasShield;});}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::DekuTree.IsMQ()) {
  areaTable[RR_DEKU_TREE_MQ_LOBBY] = Area("Deku Tree MQ Lobby", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (IsAdult || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(RG_DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,           {[]{return IsAdult || CanChildAttack;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
                  LocationAccess(RC_DEKU_TREE_MQ_BASEMENT_CHEST,            {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_LOBBY,                  {[]{return IsAdult || CanChildAttack;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,                     {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM,              {[]{return Here(RR_DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(RG_FAIRY_SLINGSHOT)) || (IsAdult && CanUse(RG_FAIRY_BOW));}) &&
                                                                               Here(RR_DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(RR_DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(RG_FAIRY_SLINGSHOT)) || (IsAdult && CanUse(RG_FAIRY_BOW));}) &&
                                                                               Here(RR_DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch;});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,            {[]{return LogicDekuB1Skip || Here(RR_DEKU_TREE_MQ_LOBBY, []{return IsAdult;});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_COMPASS_ROOM] = Area("Deku Tree MQ Compass Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_COMPASS_CHEST,   {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_COMPASS_ROOM, {[]{return HookshotOrBoomerang &&
                                                                               Here(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return HasBombchus ||
                                                                               (Bombs && (CanPlay(SongOfTime) || IsAdult)) ||
                                                                               (IsAdult && CanUse(RG_MEGATON_HAMMER) && (CanPlay(SongOfTime) || LogicDekuMQCompassGS));});}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return LogicDekuMQLog ||  (IsChild && (DekuShield || HylianShield)) ||
                                                                             (IsAdult && (CanUse(RG_LONGSHOT) || (CanUse(RG_HOOKSHOT) && CanUse(RG_IRON_BOOTS))));}}),
                  Entrance(RR_DEKU_TREE_MQ_LOBBY,                    {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, {[]{return CanPlay(SongOfTime);}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,        {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return (IsChild && CanUse(RG_STICKS)) || CanUse(RG_DINS_FIRE) ||
                                                                               Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return IsAdult && CanUse(RG_FIRE_ARROWS);});}) &&
                                                                                 Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return IsAdult || KokiriSword || CanUseProjectile || (Nuts && Sticks);});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Area("Deku Tree MQ Basement Back Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, {[]{return (IsAdult && CanUse(RG_LONGSHOT)) || (CanPlay(SongOfTime) && HookshotOrBoomerang);}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,   {[]{return HasFireSourceWithTorch && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,           {[]{return IsChild;}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return (IsChild && CanUse(RG_KOKIRI_SWORD)) || CanUseProjectile || (Nuts && (IsChild && CanUse(RG_STICKS)));}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_LEDGE] = Area("Deku Tree MQ Basement Ledge", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_DEKU_SCRUB, {[]{return CanStunDeku;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM, {[]{return IsChild;}}),
                  Entrance(RR_DEKU_TREE_MQ_LOBBY,              {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM,
                         { [] { return Here(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, [] { return HasFireSourceWithTorch; }); } }),
  });

    areaTable[RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM] =
            Area("Deku Tree MQ Outside Boss Room", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
                 {
                     // Exits
                     Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, { [] { return true; } }),
                     Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,
                              { [] { return Here(RR_DEKU_TREE_MQ_BASEMENT_LEDGE, [] { return HasShield; }); } }),
                 });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_DEKU_TREE_BOSS_ENTRYWAY] =
        Area("Deku Tree Boss Entryway", "Deku Tree", RHT_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, { [] { return Dungeon::DekuTree.IsVanilla(); } }),
                 Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, { [] { return Dungeon::DekuTree.IsMQ(); } }),
                 Entrance(RR_DEKU_TREE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_DEKU_TREE_BOSS_ROOM] =
        Area("Deku Tree Boss Room", "Deku Tree", RHT_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&DekuTreeClear, { [] {
                                                  return DekuTreeClear || (HasBossSoul(RG_GOHMA_SOUL) && 
                                                         (CanJumpslash && (Nuts || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) ||
                                                                           HookshotOrBoomerang)));
                                              }}),
             },
             {
                 // Locations
                 LocationAccess(RC_QUEEN_GOHMA, { [] { return DekuTreeClear; } }),
                 LocationAccess(RC_DEKU_TREE_QUEEN_GOHMA_HEART, { [] { return DekuTreeClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, { [] { return true; } }),
             });
}
