#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"
#include "../dungeon.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_DodongosCavern() {
  /*--------------------------
  |    VANILLA/MQ DECIDER    |
  ---------------------------*/
  areaTable[DODONGOS_CAVERN_ENTRYWAY] = Area("Dodongos Cavern Entryway", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BEGINNING,    {[]{return Dungeon::DodongosCavern.IsVanilla();}}),
                  Entrance(DODONGOS_CAVERN_MQ_BEGINNING, {[]{return Dungeon::DodongosCavern.IsMQ();}}),
                  Entrance(DEATH_MOUNTAIN_TRAIL,         {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::DodongosCavern.IsVanilla()) {
  areaTable[DODONGOS_CAVERN_BEGINNING] = Area("Dodongos Cavern Beginning", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOBBY,    {[]{return Here(DODONGOS_CAVERN_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[DODONGOS_CAVERN_LOBBY] = Area("Dodongos Cavern Lobby", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (CanSummonGossipFairy && Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;}));}}),
                }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MAP_CHEST,        {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, {[]{return CanStunDeku || GoronBracelet;}}),
                  LocationAccess(DODONGOS_CAVERN_GOSSIP_STONE,     {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BEGINNING,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOBBY_SWITCH, {[]{return IsAdult;}}),
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR,  {[]{return Here(DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return HasAccessTo(DODONGOS_CAVERN_LOBBY_SWITCH);}}),
                  Entrance(DODONGOS_CAVERN_FAR_BRIDGE,   {[]{return HasAccessTo(DODONGOS_CAVERN_FAR_BRIDGE);}}),
                  Entrance(DODONGOS_CAVERN_BOSS_AREA,    {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return HasExplosives;});}}),
                  Entrance(DODONGOS_CAVERN_BOSS_ENTRYWAY,{[]{return false;}}),
  });

  areaTable[DODONGOS_CAVERN_LOBBY_SWITCH] = Area("Dodongos Cavern Lobby Switch", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_SE_CORRIDOR] = Area("Dodongos Cavern SE Corridor", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_SCARECROW, {[]{return CanUse(SCARECROW) || (IsAdult && CanUse(LONGSHOT)) || (LogicDCScarecrowGS && (CanAdultAttack || CanChildAttack));}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,               {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_SE_ROOM,             {[]{return Here(DODONGOS_CAVERN_SE_CORRIDOR, []{return CanBlastOrSmash || CanAdultAttack || CanChildAttack || (CanTakeDamage && CanShield);});}}),
                  Entrance(DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_SE_ROOM] = Area("Dodongos Cavern SE Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Area("Dodongos Cavern Near Lower Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_SE_CORRIDOR,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_LOWER_LIZALFOS] = Area("Dodongos Cavern Lower Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(STICKS) || CanUse(KOKIRI_SWORD) ||
                    CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || HasExplosives;});}}),
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM,        {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(STICKS) || CanUse(KOKIRI_SWORD) || 
                    CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || HasExplosives;});}}),
  });

  areaTable[DODONGOS_CAVERN_DODONGO_ROOM] = Area("Dodongos Cavern Dodongo Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY_SWITCH,      {[]{return HasFireSourceWithTorch;}}),
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_NEAR_DODONGO_ROOM, {[]{return Here(DODONGOS_CAVERN_DODONGO_ROOM, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Area("Dodongos Cavern Near Dodongo Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_STAIRS_LOWER] = Area("Dodongos Cavern Stairs Lower", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_STAIRS_UPPER, {[]{return HasExplosives || GoronBracelet || CanUse(DINS_FIRE) || (LogicDCStaircase && CanUse(BOW));}}),
                  Entrance(DODONGOS_CAVERN_COMPASS_ROOM, {[]{return Here(DODONGOS_CAVERN_STAIRS_LOWER, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[DODONGOS_CAVERN_STAIRS_UPPER] = Area("Dodongos Cavern Stairs Upper", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return HookshotOrBoomerang;}) || CanUse(LONGSHOT);}}),
                  LocationAccess(DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  {[]{return IsAdult || CanChildAttack || (HasAccessTo(DODONGOS_CAVERN_STAIRS_LOWER) && CanUse(LONGSHOT) && LogicDCVinesGS);}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_ARMOS_ROOM,   {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_COMPASS_ROOM] = Area("Dodongos Cavern Compass Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_LOWER, {[]{return CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || HasExplosives || GoronBracelet;}}),
  });

  areaTable[DODONGOS_CAVERN_ARMOS_ROOM] = Area("Dodongos Cavern Armos Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_STAIRS_UPPER,    {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Area("Dodongos Cavern Bomb Room Lower", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_2F_SIDE_ROOM,         {[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash || (LogicDCScrubRoom && GoronBracelet);});}}),
                  Entrance(DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, {[]{return Here(DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER,      {[]{return (IsAdult && LogicDCJump) || CanUse(HOVER_BOOTS) || (IsAdult && CanUse(LONGSHOT));}}),
  });

  areaTable[DODONGOS_CAVERN_2F_SIDE_ROOM] = Area("Dodongos Cavern 2F Side Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Area("Dodongos Cavern First Slingshot Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return CanUse(SLINGSHOT) || CanUse(BOW) || LogicDCSlingshotSkip;}}),
  });

  areaTable[DODONGOS_CAVERN_UPPER_LIZALFOS] = Area("Dodongos Cavern Upper Lizalfos", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOWER_LIZALFOS,        {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(STICKS) || CanUse(KOKIRI_SWORD) || 
                    CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || HasExplosives;});}}),
                  Entrance(DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return Here(DODONGOS_CAVERN_LOWER_LIZALFOS, []{return CanUse(BOW) || CanUse(SLINGSHOT) || CanUse(STICKS) || CanUse(KOKIRI_SWORD) || 
                    CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(MEGATON_HAMMER) || HasExplosives;});}}),
  });

  areaTable[DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Area("Dodongos Cavern Second Slingshot Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return CanUse(SLINGSHOT) || CanUse(BOW) || LogicDCSlingshotSkip;}}),
  });

  areaTable[DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Area("Dodongos Cavern Bomb Room Upper", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_BOMB_BAG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_LOWER,       {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_FAR_BRIDGE,            {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_FAR_BRIDGE] = Area("Dodongos Cavern Far Bridge", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, {[]{return Here(DODONGOS_CAVERN_FAR_BRIDGE, []{return CanBlastOrSmash;});}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY,           {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BOSS_AREA] = Area("Dodongos Cavern Boss Area", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_LOBBY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_BACK_ROOM, {[]{return Here(DODONGOS_CAVERN_BOSS_AREA, []{return CanBlastOrSmash;});}}),
                  Entrance(DODONGOS_CAVERN_BOSS_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_BACK_ROOM] = Area("Dodongos Cavern Back Room", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_GS_BACK_ROOM, {[]{return CanAdultAttack || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOSS_AREA, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::DodongosCavern.IsMQ()) {
  areaTable[DODONGOS_CAVERN_MQ_BEGINNING] = Area("Dodongos Cavern MQ Beginning", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(DODONGOS_CAVERN_MQ_LOBBY, {[]{return Here(DODONGOS_CAVERN_MQ_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_LOBBY] = Area("Dodongos Cavern MQ Lobby", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,   {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_MAP_CHEST,                  {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_COMPASS_CHEST,              {[]{return CanAdultAttack || CanChildAttack || Nuts;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,          {[]{return (IsChild && CanUse(STICKS)) || HasFireSource;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,    {[]{return CanBlastOrSmash || (IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) || (IsAdult && (LogicDCJump || HoverBoots || Hookshot));}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, {[]{return CanPlay(SongOfTime) && (CanChildAttack || CanAdultAttack);}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,             {[]{return (IsChild && CanUse(STICKS)) || HasFireSource;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,           {[]{return CanBlastOrSmash;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,      {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,     {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,       {[]{return CanStunDeku;}}),
                  LocationAccess(DODONGOS_CAVERN_GOSSIP_STONE,                  {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  {[]{return Here(DODONGOS_CAVERN_MQ_LOBBY, []{return CanBlastOrSmash || (((IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE)) && CanTakeDamage);});}}),
                  Entrance(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA,     {[]{return IsAdult || (Here(DODONGOS_CAVERN_MQ_LOBBY, []{return IsAdult;}) && HasExplosives) || (LogicDCMQChildBombs && CanJumpslash && CanTakeDamage);}}),
                    //Trick: IsAdult || HasExplosives || (LogicDCMQChildBombs && (KokiriSword || Sticks) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  Entrance(DODONGOS_CAVERN_MQ_BOSS_AREA,         {[]{return HasExplosives || (GoronBracelet && ((IsAdult && LogicDCMQEyesAdult) || (IsChild && LogicDCMQEyesChild)) && ((IsChild && (CanUse(STICKS))) || CanUse(DINS_FIRE) || (IsAdult && (LogicDCJump || Hammer || HoverBoots || Hookshot))));}}), 
                    //Trick: HasExplosives || (LogicDCMQEyes && GoronBracelet && (IsAdult || LogicDCMQChildBack) && ((IsChild && CanUse(STICKS)) || CanUse(DINS_FIRE) || (IsAdult && (LogicDCJump || Hammer || HoverBoots || Hookshot))))
  });

  areaTable[DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Area("Dodongos Cavern MQ Lower Right Side", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return CanStunDeku;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, {[]{return (Here(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, []{return IsAdult && CanUse(BOW);}) || GoronBracelet ||
                                                                                CanUse(DINS_FIRE) || HasExplosives) &&
                                                                                IsChild && CanUse(SLINGSHOT);}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_BOMB_BAG_AREA] = Area("Dodongos Cavern MQ Bomb Bag Area", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  {[]{return (Here(DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, []{return IsAdult && CanUse(BOW);}) ||  GoronBracelet || CanUse(DINS_FIRE) || HasExplosives) && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, {[]{return true;}}),
  });

  areaTable[DODONGOS_CAVERN_MQ_BOSS_AREA] = Area("Dodongos Cavern MQ BossArea", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot,            {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, {[]{return true;}}),
                  LocationAccess(DODONGOS_CAVERN_MQ_GS_BACK_AREA,      {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(DODONGOS_CAVERN_BOSS_ENTRYWAY, {[]{return true;}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[DODONGOS_CAVERN_BOSS_ENTRYWAY] =
        Area("Dodongos Cavern Boss Entryway", "Dodongos Cavern", DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(DODONGOS_CAVERN_BOSS_AREA, { [] { return Dungeon::DodongosCavern.IsVanilla(); } }),
                 Entrance(DODONGOS_CAVERN_MQ_BOSS_AREA, { [] { return Dungeon::DodongosCavern.IsMQ(); } }),
                 Entrance(DODONGOS_CAVERN_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[DODONGOS_CAVERN_BOSS_ROOM] =
        Area("Dodongos Cavern Boss Room", "Dodongos Cavern", NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&DodongosCavernClear,
                             { [] {
                                  return DodongosCavernClear ||
                                         (Here(DODONGOS_CAVERN_BOSS_ROOM,
                                               [] { return HasExplosives || (CanUse(MEGATON_HAMMER) && LogicDCHammerFloor); }) &&
                                          (Bombs || GoronBracelet) && CanJumpslash); /*todo add chu kill to tricks*/
                              }}),
             },
             {
                 // Locations
                 LocationAccess(DODONGOS_CAVERN_BOSS_ROOM_CHEST, { [] { return true; } }),
                 LocationAccess(DODONGOS_CAVERN_KING_DODONGO_HEART, { [] { return DodongosCavernClear; } }),
                 LocationAccess(KING_DODONGO, { [] { return DodongosCavernClear; } }),
             },
             {
                 // Exits
                 Entrance(DODONGOS_CAVERN_BOSS_ENTRYWAY, { [] { return true; } }),
                 Entrance(DEATH_MOUNTAIN_TRAIL, { [] { return DodongosCavernClear; } }),
             });
}
