#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"
#include "../../dungeon.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_DekuTree() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[RR_DEKU_TREE_ENTRYWAY] = Area("Deku Tree Entryway", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,      {[]{return randoCtx->GetDungeon(DEKU_TREE)->IsVanilla();}}),
                  Entrance(RR_DEKU_TREE_MQ_LOBBY,   {[]{return randoCtx->GetDungeon(DEKU_TREE)->IsMQ();}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE, {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (randoCtx->GetDungeon(DEKU_TREE)->IsVanilla()) {
  areaTable[RR_DEKU_TREE_LOBBY] = Area("Deku Tree Lobby", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MAP_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,          {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM,    {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_COMPASS_ROOM,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,    {[]{return Here(RR_DEKU_TREE_LOBBY, []{return CanAdultAttack || CanChildAttack || Nuts;});}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, {[]{return false;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,     {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_2F_MIDDLE_ROOM] = Area("Deku Tree 2F Middle Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,         {[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(RG_MEGATON_HAMMER);});}}),
                  Entrance(RR_DEKU_TREE_SLINGSHOT_ROOM,{[]{return Here(RR_DEKU_TREE_2F_MIDDLE_ROOM, []{return HasShield || CanUse(RG_MEGATON_HAMMER);});}}),
  });

  areaTable[RR_DEKU_TREE_SLINGSHOT_ROOM] = Area("Deku Tree Slingshot Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_SLINGSHOT_CHEST,           {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_2F_MIDDLE_ROOM, {[]{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_HOVER_BOOTS);}}),
  });

  areaTable[RR_DEKU_TREE_COMPASS_ROOM] = Area("Deku Tree Compass Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_COMPASS_CHEST,           {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_GS_COMPASS_ROOM,         {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,     {[]{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY, {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_LOWER] = Area("Deku Tree Basement Lower", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));},
                                    /*Glitched*/[]{return CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_BASEMENT_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_GATE,  {[]{return CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG) || HasExplosives || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_DINS_FIRE);}}),
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_VINES, {[]{return CanUseProjectile || CanUse(RG_DINS_FIRE) || (randoCtx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS) && CanJumpslash);}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_LOBBY,               {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return Here(RR_DEKU_TREE_BASEMENT_LOWER, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return IsAdult || randoCtx->GetTrickOption(RT_DEKU_B1_SKIP) || HasAccessTo(RR_DEKU_TREE_BASEMENT_UPPER);}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return false;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_SCRUB_ROOM] = Area("Deku Tree Basement Scrub Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return Here(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, []{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_WATER_ROOM] = Area("Deku Tree Basement Water Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_TORCH_ROOM] = Area("Deku Tree Basement Torch Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_WATER_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return Here(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);});}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_LOBBY] = Area("Deku Tree Basement Back Lobby", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) &&
                                                                            (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE)));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_TORCH_ROOM, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_ROOM,  {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) &&
                                                                     Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER,      {[]{return Here(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, []{return HasFireSourceWithTorch || CanUse(RG_FAIRY_BOW);}) && IsChild;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_BACK_ROOM] = Area("Deku Tree Basement Back Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_BASEMENT_UPPER] = Area("Deku Tree Basement Upper", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_LOWER,      {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BASEMENT_BACK_LOBBY, {[]{return IsChild;}}),
                  Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM,   {[]{return Here(RR_DEKU_TREE_BASEMENT_UPPER, []{return HasFireSourceWithTorch || (randoCtx->GetTrickOption(RT_DEKU_B1_BOW_WEBS) && IsAdult && CanUse(RG_FAIRY_BOW));});}}),
  });

  areaTable[RR_DEKU_TREE_OUTSIDE_BOSS_ROOM] = Area("Deku Tree Outside Boss Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DEKU_TREE_BASEMENT_UPPER, {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_BOSS_ENTRYWAY,  {[]{return Here(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, []{return HasShield;});}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (randoCtx->GetDungeon(DEKU_TREE)->IsMQ()) {
  areaTable[RR_DEKU_TREE_MQ_LOBBY] = Area("Deku Tree MQ Lobby", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks, {[]{return DekuBabaSticks || (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));}}),
                  EventAccess(&DekuBabaNuts,   {[]{return DekuBabaNuts   || (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_MEGATON_HAMMER) || HasExplosives || CanUse(RG_DINS_FIRE));}}),
  }, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_MAP_CHEST,                 {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,           {[]{return CanAdultAttack || CanChildAttack;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
                  LocationAccess(RC_DEKU_TREE_MQ_BASEMENT_CHEST,            {[]{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_LOBBY,                  {[]{return CanAdultAttack || CanChildAttack;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY,                     {[]{return true;}}),
                  Entrance(RR_DEKU_TREE_MQ_COMPASS_ROOM,              {[]{return Here(RR_DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(RG_FAIRY_SLINGSHOT)) || (IsAdult && CanUse(RG_FAIRY_BOW));}) &&
                                                                               Here(RR_DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch || (IsAdult && CanUse(RG_FAIRY_BOW));});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return Here(RR_DEKU_TREE_MQ_LOBBY, []{return (IsChild && CanUse(RG_FAIRY_SLINGSHOT)) || (IsAdult && CanUse(RG_FAIRY_BOW));}) &&
                                                                               Here(RR_DEKU_TREE_MQ_LOBBY, []{return HasFireSourceWithTorch;});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,            {[]{return randoCtx->GetTrickOption(RT_DEKU_B1_SKIP) || Here(RR_DEKU_TREE_MQ_LOBBY, []{return IsAdult;});}}),
  });

  areaTable[RR_DEKU_TREE_MQ_COMPASS_ROOM] = Area("Deku Tree MQ Compass Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_COMPASS_CHEST,   {[]{return true;}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_COMPASS_ROOM, {[]{return HookshotOrBoomerang &&
                                                                               Here(RR_DEKU_TREE_MQ_COMPASS_ROOM, []{return HasBombchus ||
                                                                               (Bombs && (CanUse(RG_SONG_OF_TIME) || IsAdult)) ||
                                                                               (IsAdult && CanUse(RG_MEGATON_HAMMER) && (CanUse(RG_SONG_OF_TIME) || randoCtx->GetTrickOption(RT_DEKU_MQ_COMPASS_GS)));});}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_LOBBY, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return randoCtx->GetTrickOption(RT_DEKU_MQ_LOG) ||  (IsChild && (DekuShield || HylianShield)) ||
                                                                             (IsAdult && (CanUse(RG_LONGSHOT) || (CanUse(RG_HOOKSHOT) && CanUse(RG_IRON_BOOTS))));}}),
                  Entrance(RR_DEKU_TREE_MQ_LOBBY,                    {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK] = Area("Deku Tree MQ Basement Water Room Front", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, {[]{return CanUse(RG_SONG_OF_TIME);}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,        {[]{return Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return (IsChild && CanUse(RG_STICKS)) || CanUse(RG_DINS_FIRE) ||
                                                                               Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, []{return IsAdult && CanUse(RG_FIRE_ARROWS);});}) &&
                                                                                 Here(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, []{return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) ||
                                                                                  CanUseProjectile || (Nuts && CanUse(RG_STICKS));});}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, {[]{return true;}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM] = Area("Deku Tree MQ Basement Back Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, {[]{return (IsAdult && CanUse(RG_LONGSHOT)) || (CanUse(RG_SONG_OF_TIME) && HookshotOrBoomerang);}}),
                  LocationAccess(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,   {[]{return HasFireSourceWithTorch && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_LEDGE,           {[]{return IsChild;}}),
                  Entrance(RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, {[]{return (IsChild && CanUse(RG_KOKIRI_SWORD)) || CanUseProjectile || (Nuts && (IsChild && CanUse(RG_STICKS)));}}),
  });

  areaTable[RR_DEKU_TREE_MQ_BASEMENT_LEDGE] = Area("Deku Tree MQ Basement Ledge", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {
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
            Area("Deku Tree MQ Outside Boss Room", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
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
        Area("Deku Tree Boss Entryway", "Deku Tree", RA_DEKU_TREE, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_DEKU_TREE_OUTSIDE_BOSS_ROOM, { [] { return randoCtx->GetDungeon(DEKU_TREE)->IsVanilla(); } }),
                 Entrance(RR_DEKU_TREE_MQ_OUTSIDE_BOSS_ROOM, { [] { return randoCtx->GetDungeon(DEKU_TREE)->IsMQ(); } }),
                 Entrance(RR_DEKU_TREE_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_DEKU_TREE_BOSS_ROOM] =
        Area("Deku Tree Boss Room", "Deku Tree", RA_NONE, NO_DAY_NIGHT_CYCLE,
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
