#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_GerudoValley() {
  areaTable[GERUDO_VALLEY] = Area("Gerudo Valley", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BugRock, {[]{return BugRock || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(GV_GS_SMALL_BRIDGE, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,      {[]{return true;}}),
                  Entrance(GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(GV_CRATE_LEDGE,    {[]{return IsChild || CanUse(LONGSHOT);}}),
                  Entrance(GV_GROTTO_LEDGE,   {[]{return true;}}),
                  Entrance(GV_FORTRESS_SIDE,  {[]{return (IsAdult && (CanRideEpona || CanUse(LONGSHOT) || GerudoFortress.Is(GERUDOFORTRESS_OPEN) || CarpenterRescue)) || (IsChild && CanUse(HOOKSHOT));}}),
  });

  areaTable[GV_UPPER_STREAM] = Area("GV Upper Stream", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(GV_UPPER_STREAM) && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(GV_WATERFALL_FREESTANDING_POH, {[]{return true;}}),
                  LocationAccess(GV_GS_BEAN_PATCH,              {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(GV_COW,                        {[]{return IsChild && CanPlay(EponasSong);}}),
                  LocationAccess(GV_GOSSIP_STONE,               {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[GV_LOWER_STREAM] = Area("GV Lower Stream", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[GV_GROTTO_LEDGE] = Area("GV Grotto Ledge", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_LOWER_STREAM,   {[]{return true;}}),
                  Entrance(GV_OCTOROK_GROTTO, {[]{return CanUse(SILVER_GAUNTLETS);}}),
                  Entrance(GV_CRATE_LEDGE,    {[]{return CanUse(LONGSHOT);}}),
  });

  areaTable[GV_CRATE_LEDGE] = Area("GV Crate Ledge", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GV_CRATE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[GV_FORTRESS_SIDE] = Area("GV Fortress Side", "Gerudo Valley", GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BrokenSwordAccess, {[]{return IsAdult && (PoachersSawAccess || PoachersSaw);}}),
                }, {
                  //Locations                                       
                  LocationAccess(GV_CHEST,          {[]{return IsAdult && CanUse(MEGATON_HAMMER);}}),
                  LocationAccess(GV_TRADE_SAW,      {[]{return IsAdult && PoachersSaw;}}),
                  LocationAccess(GV_GS_BEHIND_TENT, {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(GV_GS_PILLAR,      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(GERUDO_FORTRESS,   {[]{return true;}}),
                  Entrance(GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(GERUDO_VALLEY,     {[]{return IsChild || CanRideEpona || CanUse(LONGSHOT) || GerudoFortress.Is(GERUDOFORTRESS_OPEN) || CarpenterRescue;}}),
                  Entrance(GV_CARPENTER_TENT, {[]{return IsAdult;}}),
                  Entrance(GV_STORMS_GROTTO,  {[]{return IsAdult && CanOpenStormGrotto;}}),
                  Entrance(GV_CRATE_LEDGE, {[]{return false;}}),
  });

  areaTable[GV_CARPENTER_TENT] = Area("GV Carpenter Tent", "GV Carpenter Tent", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[GV_OCTOROK_GROTTO] = Area("GV Octorok Grotto", "GV Octorok Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GV_GROTTO_LEDGE, {[]{return true;}}),
  });

  areaTable[GV_STORMS_GROTTO] = Area("GV Storms Grotto", "GV Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GV_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(GV_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[GERUDO_FORTRESS] = Area("Gerudo Fortress", "Gerudo Fortress", GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CarpenterRescue, {[]{return CanFinishGerudoFortress;}}),
                  EventAccess(&GF_GateOpen,     {[]{return IsAdult && GerudoToken;}}),
                  EventAccess(&GtG_GateOpen,    {[]{return GtG_GateOpen || (IsAdult && GerudoToken);}}),
                }, {
                  //Locations
                  LocationAccess(GF_CHEST,              {[]{return CanUse(HOVER_BOOTS) || (IsAdult && CanUse(SCARECROW)) || CanUse(LONGSHOT);}}),
                  LocationAccess(GF_HBA_1000_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(GF_HBA_1500_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(GF_NORTH_F1_CARPENTER, {[]{return  CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  LocationAccess(GF_NORTH_F2_CARPENTER, {[]{return (CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen);}}),
                  LocationAccess(GF_SOUTH_F1_CARPENTER, {[]{return  CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  LocationAccess(GF_SOUTH_F2_CARPENTER, {[]{return  CanUse(KOKIRI_SWORD) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  LocationAccess(GF_GERUDO_MEMBERSHIP_CARD,       {[]{return CanFinishGerudoFortress;}}),
                  LocationAccess(GF_GS_ARCHERY_RANGE,   {[]{return IsAdult && HookshotOrBoomerang && GerudoToken && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(GF_GS_TOP_FLOOR,       {[]{return IsAdult && AtNight && (CanJumpslash || CanUse(SLINGSHOT) || CanUse(BOOMERANG) || HasExplosives || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(DINS_FIRE)) && (GerudoToken || CanUse(BOW) || CanUse(HOOKSHOT) || CanUse(HOVER_BOOTS) || LogicGerudoKitchen || LogicGFJump) && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(GV_FORTRESS_SIDE,                 {[]{return true;}}),
                  Entrance(GF_OUTSIDE_GATE,                  {[]{return GF_GateOpen;}}),
                  Entrance(GERUDO_TRAINING_GROUNDS_ENTRYWAY, {[]{return GtG_GateOpen && (IsAdult || ShuffleDungeonEntrances);}}),
                  Entrance(GF_STORMS_GROTTO,                 {[]{return IsAdult && CanOpenStormGrotto;}}),
  });

  areaTable[GF_OUTSIDE_GATE] = Area("GF Outside Gate", "Gerudo Fortress", GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GF_GateOpen, {[]{return IsAdult && GerudoToken && (ShuffleGerudoToken || ShuffleOverworldEntrances /*|| ShuffleSpecialIndoorEntrances*/);}}),
                }, {}, {
                  //Exits
                  Entrance(GERUDO_FORTRESS,         {[]{return (IsAdult && (Hookshot || !ShuffleOverworldEntrances)) || GF_GateOpen;}}),
                  Entrance(WASTELAND_NEAR_FORTRESS, {[]{return true;}}),
  });

  areaTable[GF_STORMS_GROTTO] = Area("GF Storms Grotto", "GF Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(GERUDO_FORTRESS, {[]{return true;}}),
  });

  areaTable[WASTELAND_NEAR_FORTRESS] = Area("Wasteland Near Fortress", "Haunted Wasteland", HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(GF_OUTSIDE_GATE,   {[]{return true;}}),
                  Entrance(HAUNTED_WASTELAND, {[]{return CanUse(HOVER_BOOTS) || CanUse(LONGSHOT) || LogicWastelandCrossing;}}),
  });

  areaTable[HAUNTED_WASTELAND] = Area("Haunted Wasteland", "Haunted Wasteland", HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(WASTELAND_CHEST,            {[]{return HasFireSource;}}),
                  LocationAccess(WASTELAND_BOMBCHU_SALESMAN, {[]{return AdultsWallet && (CanJumpslash || CanUse(HOVER_BOOTS)) ;}}),
                  LocationAccess(WASTELAND_GS,               {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(WASTELAND_NEAR_COLOSSUS, {[]{return LogicLensWasteland || CanUse(LENS_OF_TRUTH);}}),
                  Entrance(WASTELAND_NEAR_FORTRESS, {[]{return CanUse(HOVER_BOOTS) || CanUse(LONGSHOT) || LogicWastelandCrossing;}}),
  });

  areaTable[WASTELAND_NEAR_COLOSSUS] = Area("Wasteland Near Colossus", "Haunted Wasteland", HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(DESERT_COLOSSUS,   {[]{return true;}}),
                  Entrance(HAUNTED_WASTELAND, {[]{return LogicReverseWasteland || false;}}),
  });

  areaTable[DESERT_COLOSSUS] = Area("Desert Colossus", "Desert Colossus", DESERT_COLOSSUS, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPond, {[]{return FairyPond || CanPlay(SongOfStorms);}}),
                  EventAccess(&BugRock,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(COLOSSUS_FREESTANDING_POH, {[]{return IsAdult && CanPlantBean(DESERT_COLOSSUS);}}),
                  LocationAccess(COLOSSUS_GS_BEAN_PATCH,    {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(COLOSSUS_GS_TREE,          {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(COLOSSUS_GS_HILL,          {[]{return IsAdult && AtNight && ((CanPlantBean(DESERT_COLOSSUS) && CanAdultAttack) || CanUse(LONGSHOT) || (LogicColossusGS && CanUse(HOOKSHOT))) && CanGetNightTimeGS;}}),
                  LocationAccess(COLOSSUS_GOSSIP_STONE,     {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(COLOSSUS_GREAT_FAIRY_FOUNTAIN, {[]{return HasExplosives;}}),
                  Entrance(SPIRIT_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(WASTELAND_NEAR_COLOSSUS,       {[]{return true;}}),
                  Entrance(COLOSSUS_GROTTO,               {[]{return CanUse(SILVER_GAUNTLETS);}}),
  });

  areaTable[DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY] = Area("Desert Colossus From Spirit Entryway", "Desert Colossus", DESERT_COLOSSUS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SHEIK_AT_COLOSSUS, {[]{return true;}}),
                }, {
                  //Exist
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[COLOSSUS_GREAT_FAIRY_FOUNTAIN] = Area("Colossus Great Fairy Fountain", "Colossus Great Fairy Fountain", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(COLOSSUS_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[COLOSSUS_GROTTO] = Area("Colossus Grotto", "Colossus Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(COLOSSUS_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(DESERT_COLOSSUS, {[]{return true;}}),
  });
}
