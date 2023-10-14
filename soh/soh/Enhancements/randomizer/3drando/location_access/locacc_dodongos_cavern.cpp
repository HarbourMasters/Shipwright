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
  areaTable[RR_DODONGOS_CAVERN_ENTRYWAY] = Area("Dodongos Cavern Entryway", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BEGINNING,    {[]{return Dungeon::DodongosCavern.IsVanilla();}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_BEGINNING, {[]{return Dungeon::DodongosCavern.IsMQ();}}),
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL,         {[]{return true;}}),
  });

  /*--------------------------
  |     VANILLA DUNGEON      |
  ---------------------------*/
  if (Dungeon::DodongosCavern.IsVanilla()) {
  areaTable[RR_DODONGOS_CAVERN_BEGINNING] = Area("Dodongos Cavern Beginning", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,    {[]{return Here(RR_DODONGOS_CAVERN_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOBBY] = Area("Dodongos Cavern Lobby", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (CanSummonGossipFairy && Here(RR_DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;}));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_MAP_CHEST,        {[]{return Here(RR_DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  LocationAccess(RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, {[]{return CanStunDeku || GoronBracelet;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_GOSSIP_STONE,     {[]{return Here(RR_DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BEGINNING,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOBBY_SWITCH, {[]{return IsAdult;}}),
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR,  {[]{return Here(RR_DODONGOS_CAVERN_LOBBY, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return HasAccessTo(RR_DODONGOS_CAVERN_LOBBY_SWITCH);}}),
                  Entrance(RR_DODONGOS_CAVERN_FAR_BRIDGE,   {[]{return HasAccessTo(RR_DODONGOS_CAVERN_FAR_BRIDGE);}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_AREA,    {[]{return Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return HasExplosives;});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,{[]{return false;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOBBY_SWITCH] = Area("Dodongos Cavern Lobby Switch", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SE_CORRIDOR] = Area("Dodongos Cavern SE Corridor", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_GS_SCARECROW, {[]{return CanUse(RG_SCARECROW) || (IsAdult && CanUse(RG_LONGSHOT)) || (LogicDCScarecrowGS && (IsAdult || CanChildAttack));}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,               {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_SE_ROOM,             {[]{return Here(RR_DODONGOS_CAVERN_SE_CORRIDOR, []{return CanBlastOrSmash || IsAdult || CanChildAttack || (CanTakeDamage && CanShield);});}}),
                  Entrance(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SE_ROOM] = Area("Dodongos Cavern SE Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS] = Area("Dodongos Cavern Near Lower Lizalfos", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_SE_CORRIDOR,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_LOWER_LIZALFOS] = Area("Dodongos Cavern Lower Lizalfos", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});}}),
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM,        {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_DODONGO_ROOM] = Area("Dodongos Cavern Dodongo Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY_SWITCH,      {[]{return HasFireSourceWithTorch;}}),
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_DODONGO_ROOM, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM] = Area("Dodongos Cavern Near Dodongo Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_DODONGO_ROOM, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_STAIRS_LOWER] = Area("Dodongos Cavern Stairs Lower", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_UPPER, {[]{return HasExplosives || GoronBracelet || CanUse(RG_DINS_FIRE) || (LogicDCStaircase && CanUse(RG_FAIRY_BOW));}}),
                  Entrance(RR_DODONGOS_CAVERN_COMPASS_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_STAIRS_LOWER, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_STAIRS_UPPER] = Area("Dodongos Cavern Stairs Upper", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, {[]{return Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return HookshotOrBoomerang;}) || CanUse(RG_LONGSHOT);}}),
                  LocationAccess(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,  {[]{return IsAdult || CanChildAttack || (HasAccessTo(RR_DODONGOS_CAVERN_STAIRS_LOWER) && CanUse(RG_LONGSHOT) && LogicDCVinesGS);}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_ARMOS_ROOM,   {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_COMPASS_ROOM] = Area("Dodongos Cavern Compass Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_COMPASS_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_LOWER, {[]{return IsAdult || HasExplosives || GoronBracelet;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_ARMOS_ROOM] = Area("Dodongos Cavern Armos Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_STAIRS_UPPER,    {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER] = Area("Dodongos Cavern Bomb Room Lower", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_2F_SIDE_ROOM,         {[]{return Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash || (LogicDCScrubRoom && GoronBracelet);});}}),
                  Entrance(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, []{return CanBlastOrSmash || GoronBracelet;});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER,      {[]{return (IsAdult && LogicDCJump) || CanUse(RG_HOVER_BOOTS) || (IsAdult && CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_DODONGOS_CAVERN_2F_SIDE_ROOM] = Area("Dodongos Cavern 2F Side Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM] = Area("Dodongos Cavern First Slingshot Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || LogicDCSlingshotSkip;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_UPPER_LIZALFOS] = Area("Dodongos Cavern Upper Lizalfos", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOWER_LIZALFOS,        {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,  {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});}}),
                  Entrance(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_LOWER_LIZALFOS, []{return IsAdult || Slingshot || Sticks || KokiriSword || HasExplosives;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM] = Area("Dodongos Cavern Second Slingshot Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_UPPER_LIZALFOS,  {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || LogicDCSlingshotSkip;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER] = Area("Dodongos Cavern Bomb Room Upper", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,       {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_FAR_BRIDGE,            {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_FAR_BRIDGE] = Area("Dodongos Cavern Far Bridge", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, {[]{return Here(RR_DODONGOS_CAVERN_FAR_BRIDGE, []{return CanBlastOrSmash;});}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY,           {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BOMB_ROOM_UPPER, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BOSS_AREA] = Area("Dodongos Cavern Boss Area", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_LOBBY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_BACK_ROOM, {[]{return Here(RR_DODONGOS_CAVERN_BOSS_AREA, []{return CanBlastOrSmash;});}}),
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_BACK_ROOM] = Area("Dodongos Cavern Back Room", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_GS_BACK_ROOM, {[]{return IsAdult || CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOSS_AREA, {[]{return true;}}),
  });
  }

  /*---------------------------
  |   MASTER QUEST DUNGEON    |
  ---------------------------*/
  if (Dungeon::DodongosCavern.IsMQ()) {
  areaTable[RR_DODONGOS_CAVERN_MQ_BEGINNING] = Area("Dodongos Cavern MQ Beginning", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return true;}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOBBY, {[]{return Here(RR_DODONGOS_CAVERN_MQ_BEGINNING, []{return CanBlastOrSmash || GoronBracelet;});}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LOBBY] = Area("Dodongos Cavern MQ Lobby", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,   {[]{return DekuBabaSticks || (IsAdult || KokiriSword || Boomerang);}}),
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
  }, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_MAP_CHEST,                  {[]{return true;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST,              {[]{return IsAdult || CanChildAttack || Nuts;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,          {[]{return (IsChild && CanUse(RG_STICKS)) || HasFireSource;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,    {[]{return CanBlastOrSmash || (IsChild && CanUse(RG_STICKS)) || CanUse(RG_DINS_FIRE) || (IsAdult && (LogicDCJump || HoverBoots || Hookshot));}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, {[]{return CanPlay(SongOfTime) && (CanChildAttack || IsAdult);}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,             {[]{return (IsChild && CanUse(RG_STICKS)) || HasFireSource;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,           {[]{return CanBlastOrSmash;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,      {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,     {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,       {[]{return CanStunDeku;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_GOSSIP_STONE,                  {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,  {[]{return Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return CanBlastOrSmash || (((IsChild && CanUse(RG_STICKS)) || CanUse(RG_DINS_FIRE)) && CanTakeDamage);});}}),
                  Entrance(RR_DODONGOS_CAVERN_MQ_BOMB_BAG_AREA,     {[]{return IsAdult || (Here(RR_DODONGOS_CAVERN_MQ_LOBBY, []{return IsAdult;}) && HasExplosives) || (LogicDCMQChildBombs && CanJumpslash && CanTakeDamage);}}),
                    //Trick: IsAdult || HasExplosives || (LogicDCMQChildBombs && (KokiriSword || Sticks) && DamageMultiplier.IsNot(DAMAGEMULTIPLIER_OHKO))
                  Entrance(RR_DODONGOS_CAVERN_MQ_BOSS_AREA,         {[]{return HasExplosives || (GoronBracelet && ((IsAdult && LogicDCMQEyesAdult) || (IsChild && LogicDCMQEyesChild)) && ((IsChild && (CanUse(RG_STICKS))) || CanUse(RG_DINS_FIRE) || (IsAdult && (LogicDCJump || Hammer || HoverBoots || Hookshot))));}}), 
                    //Trick: HasExplosives || (LogicDCMQEyes && GoronBracelet && (IsAdult || LogicDCMQChildBack) && ((IsChild && CanUse(RG_STICKS)) || CanUse(RG_DINS_FIRE) || (IsAdult && (LogicDCJump || Hammer || HoverBoots || Hookshot))))
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE] = Area("Dodongos Cavern MQ Lower Right Side", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, {[]{return CanStunDeku;}}),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, {[]{return (Here(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, []{return IsAdult && CanUse(RG_FAIRY_BOW);}) || GoronBracelet ||
                                                                                CanUse(RG_DINS_FIRE) || HasExplosives) &&
                                                                                IsChild && CanUse(RG_FAIRY_SLINGSHOT);}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_BOMB_BAG_AREA] = Area("Dodongos Cavern MQ Bomb Bag Area", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, {[]{return true;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,  {[]{return (Here(RR_DODONGOS_CAVERN_MQ_BOMB_BAG_AREA, []{return IsAdult && CanUse(RG_FAIRY_BOW);}) ||  GoronBracelet || CanUse(RG_DINS_FIRE) || HasExplosives) && HookshotOrBoomerang;}}),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, {[]{return true;}}),
  });

  areaTable[RR_DODONGOS_CAVERN_MQ_BOSS_AREA] = Area("Dodongos Cavern MQ BossArea", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot,            {[]{return true;}}),
  }, {
                  //Locations
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, {[]{return true;}}),
                  LocationAccess(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,      {[]{return true;}}),
  }, {
                  //Exits
                  Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, {[]{return true;}}),
  });
  }

    /*---------------------------
    |         BOSS ROOM         |
    ---------------------------*/
    areaTable[RR_DODONGOS_CAVERN_BOSS_ENTRYWAY] =
        Area("Dodongos Cavern Boss Entryway", "Dodongos Cavern", RHT_DODONGOS_CAVERN, NO_DAY_NIGHT_CYCLE, {}, {},
             {
                 // Exits
                 Entrance(RR_DODONGOS_CAVERN_BOSS_AREA, { [] { return Dungeon::DodongosCavern.IsVanilla(); } }),
                 Entrance(RR_DODONGOS_CAVERN_MQ_BOSS_AREA, { [] { return Dungeon::DodongosCavern.IsMQ(); } }),
                 Entrance(RR_DODONGOS_CAVERN_BOSS_ROOM, { [] { return true; } }),
             });

    areaTable[RR_DODONGOS_CAVERN_BOSS_ROOM] =
        Area("Dodongos Cavern Boss Room", "Dodongos Cavern", RHT_NONE, NO_DAY_NIGHT_CYCLE,
             {
                 // Events
                 EventAccess(&DodongosCavernClear,
                             { [] {
                                  return DodongosCavernClear ||
                                         (Here(RR_DODONGOS_CAVERN_BOSS_ROOM,
                                               [] { return HasExplosives || (CanUse(RG_MEGATON_HAMMER) && LogicDCHammerFloor); }) &&
                                          (Bombs || GoronBracelet) && CanJumpslash); /*todo add chu kill to tricks*/
                              }}),
             },
             {
                 // Locations
                 LocationAccess(RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, { [] { return true; } }),
                 LocationAccess(RC_DODONGOS_CAVERN_KING_DODONGO_HEART, { [] { return DodongosCavernClear; } }),
                 LocationAccess(RC_KING_DODONGO, { [] { return DodongosCavernClear; } }),
             },
             {
                 // Exits
                 Entrance(RR_DODONGOS_CAVERN_BOSS_ENTRYWAY, { [] { return true; } }),
             });
}
