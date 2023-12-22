#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_GerudoValley() {
  areaTable[RR_GERUDO_VALLEY] = Area("Gerudo Valley", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BugRock, {[]{return BugRock || IsChild;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GV_GS_SMALL_BRIDGE, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,      {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return IsChild || CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_GV_GROTTO_LEDGE,   {[]{return true;}}),
                  Entrance(RR_GV_FORTRESS_SIDE,  {[]{return (IsAdult && (CanRideEpona || CanUse(RG_LONGSHOT) || randoCtx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN) || CarpenterRescue)) || (IsChild && CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GV_UPPER_STREAM] = Area("GV Upper Stream", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(RR_GV_UPPER_STREAM) && CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GV_WATERFALL_FREESTANDING_POH, {[]{return true;}}),
                  LocationAccess(RC_GV_GS_BEAN_PATCH,              {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(RC_GV_COW,                        {[]{return IsChild && CanUse(RG_EPONAS_SONG);}}),
                  LocationAccess(RC_GV_GOSSIP_STONE,               {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_LOWER_STREAM] = Area("GV Lower Stream", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_GV_GROTTO_LEDGE] = Area("GV Grotto Ledge", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_OCTOROK_GROTTO, {[]{return CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GV_CRATE_LEDGE] = Area("GV Crate Ledge", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GV_CRATE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_FORTRESS_SIDE] = Area("GV Fortress Side", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BrokenSwordAccess, {[]{return IsAdult && (PoachersSawAccess || PoachersSaw);}}),
                }, {
                  //Locations                                       
                  LocationAccess(RC_GV_CHEST,          {[]{return IsAdult && CanUse(RG_MEGATON_HAMMER);}}),
                  LocationAccess(RC_GV_TRADE_SAW,      {[]{return IsAdult && PoachersSaw;}}),
                  LocationAccess(RC_GV_GS_BEHIND_TENT, {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_GV_GS_PILLAR,      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,   {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,     {[]{return IsChild || CanRideEpona || CanUse(RG_LONGSHOT) || randoCtx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN) || CarpenterRescue;}}),
                  Entrance(RR_GV_CARPENTER_TENT, {[]{return IsAdult;}}),
                  Entrance(RR_GV_STORMS_GROTTO,  {[]{return IsAdult && CanOpenStormGrotto;}}),
                  Entrance(RR_GV_CRATE_LEDGE, {[]{return false;}}),
  });

  areaTable[RR_GV_CARPENTER_TENT] = Area("GV Carpenter Tent", "GV Carpenter Tent", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[RR_GV_OCTOROK_GROTTO] = Area("GV Octorok Grotto", "GV Octorok Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_GROTTO_LEDGE, {[]{return true;}}),
  });

  areaTable[RR_GV_STORMS_GROTTO] = Area("GV Storms Grotto", "GV Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GV_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_GV_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_FORTRESS] = Area("Gerudo Fortress", "Gerudo Fortress", RA_GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CarpenterRescue, {[]{return CanFinishGerudoFortress;}}),
                  EventAccess(&GF_GateOpen,     {[]{return IsAdult && GerudoToken;}}),
                  EventAccess(&GtG_GateOpen,    {[]{return GtG_GateOpen || (IsAdult && GerudoToken);}}),
                }, {
                  //Locations
                  LocationAccess(RC_GF_CHEST,              {[]{return CanUse(RG_HOVER_BOOTS) || (IsAdult && CanUse(RG_SCARECROW)) || CanUse(RG_LONGSHOT);}}),
                  LocationAccess(RC_GF_HBA_1000_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(RC_GF_HBA_1500_POINTS,    {[]{return GerudoToken && CanRideEpona && Bow && AtDay;}}),
                  LocationAccess(RC_GF_NORTH_F1_CARPENTER, {[]{return  CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GF_NORTH_F2_CARPENTER, {[]{return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (GerudoToken || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS) || randoCtx->GetTrickOption(RT_GF_KITCHEN));}}),
                  LocationAccess(RC_GF_SOUTH_F1_CARPENTER, {[]{return  CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GF_SOUTH_F2_CARPENTER, {[]{return  CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                  LocationAccess(RC_GF_GERUDO_MEMBERSHIP_CARD,       {[]{return CanFinishGerudoFortress;}}),
                  LocationAccess(RC_GF_GS_ARCHERY_RANGE,   {[]{return IsAdult && HookshotOrBoomerang && GerudoToken && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_GF_GS_TOP_FLOOR,       {[]{return IsAdult && AtNight && (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG) || HasExplosives || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_DINS_FIRE)) && (GerudoToken || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS) || randoCtx->GetTrickOption(RT_GF_KITCHEN) || randoCtx->GetTrickOption(RT_GF_JUMP)) && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE,                 {[]{return true;}}),
                  Entrance(RR_GF_OUTSIDE_GATE,                  {[]{return GF_GateOpen;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY, {[]{return GtG_GateOpen && (IsAdult || randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES));}}),
                  Entrance(RR_GF_STORMS_GROTTO,                 {[]{return IsAdult && CanOpenStormGrotto;}}),
  });

  areaTable[RR_GF_OUTSIDE_GATE] = Area("GF Outside Gate", "Gerudo Fortress", RA_GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GF_GateOpen, {[]{return IsAdult && GerudoToken && (randoCtx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) || randoCtx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) /*|| ShuffleSpecialIndoorEntrances*/);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,         {[]{return (IsAdult && (Hookshot || !randoCtx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES))) || GF_GateOpen;}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_GF_STORMS_GROTTO] = Area("GF Storms Grotto", "GF Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_WASTELAND_NEAR_FORTRESS] = Area("Wasteland Near Fortress", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GF_OUTSIDE_GATE,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return CanUse(RG_HOVER_BOOTS) || CanUse(RG_LONGSHOT) || randoCtx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_HAUNTED_WASTELAND] = Area("Haunted Wasteland", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPot, {[]{return true;}}),
                  EventAccess(&NutPot,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_WASTELAND_CHEST,            {[]{return HasFireSource;}}),
                  LocationAccess(RC_WASTELAND_BOMBCHU_SALESMAN, {[]{return AdultsWallet && (CanJumpslash || CanUse(RG_HOVER_BOOTS)) ;}}),
                  LocationAccess(RC_WASTELAND_GS,               {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS, {[]{return randoCtx->GetTrickOption(RT_LENS_HW) || CanUse(RG_LENS_OF_TRUTH);}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return CanUse(RG_HOVER_BOOTS) || CanUse(RG_LONGSHOT) || randoCtx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_WASTELAND_NEAR_COLOSSUS] = Area("Wasteland Near Colossus", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return randoCtx->GetTrickOption(RT_HW_REVERSE) || false;}}),
  });

  areaTable[RR_DESERT_COLOSSUS] = Area("Desert Colossus", "Desert Colossus", RA_DESERT_COLOSSUS, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FairyPond, {[]{return FairyPond || CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&BugRock,   {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_COLOSSUS_FREESTANDING_POH, {[]{return IsAdult && CanPlantBean(RR_DESERT_COLOSSUS);}}),
                  LocationAccess(RC_COLOSSUS_GS_BEAN_PATCH,    {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(RC_COLOSSUS_GS_TREE,          {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_COLOSSUS_GS_HILL,          {[]{return IsAdult && AtNight && ((CanPlantBean(RR_DESERT_COLOSSUS) && CanAdultAttack) || CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_COLOSSUS_GS) && CanUse(RG_HOOKSHOT))) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_COLOSSUS_GOSSIP_STONE,     {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, {[]{return HasExplosives;}}),
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS,       {[]{return true;}}),
                  Entrance(RR_COLOSSUS_GROTTO,               {[]{return CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY] = Area("Desert Colossus From Spirit Entryway", "Desert Colossus", RA_DESERT_COLOSSUS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SHEIK_AT_COLOSSUS, {[]{return true;}}),
                }, {
                  //Exist
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = Area("Colossus Great Fairy Fountain", "Colossus Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_COLOSSUS_GREAT_FAIRY_REWARD, {[]{return CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GROTTO] = Area("Colossus Grotto", "Colossus Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });
}
