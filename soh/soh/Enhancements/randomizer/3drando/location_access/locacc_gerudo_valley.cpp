#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_GerudoValley() {
  areaTable[RR_GERUDO_VALLEY] = Area("Gerudo Valley", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BugRock, {[]{return logic->BugRock || logic->IsChild;}}),
                }, {
                  //Locations
                  LOCATION(RC_GV_GS_SMALL_BRIDGE, logic->IsChild && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,      {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return logic->IsChild || logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_GV_GROTTO_LEDGE,   {[]{return true;}}),
                  Entrance(RR_GV_FORTRESS_SIDE,  {[]{return (logic->IsAdult && (logic->CanRideEpona || logic->CanUse(RG_LONGSHOT) || randoCtx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN) || logic->CarpenterRescue)) || (logic->IsChild && logic->CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GV_UPPER_STREAM] = Area("GV Upper Stream", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairy;}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_GV_UPPER_STREAM) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LOCATION(RC_GV_WATERFALL_FREESTANDING_POH, logic->IsChild || logic->Swim),//can use cucco as child
                  LOCATION(RC_GV_GS_BEAN_PATCH,              logic->CanPlantBugs && logic->CanChildAttack),
                  LOCATION(RC_GV_COW,                        logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_GV_GOSSIP_STONE,               true),
                }, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_LOWER_STREAM] = Area("GV Lower Stream", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return logic->IsChild || logic->Swim;}}),//can use cucco as child
  });

  areaTable[RR_GV_GROTTO_LEDGE] = Area("GV Grotto Ledge", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_OCTOROK_GROTTO, {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GV_CRATE_LEDGE] = Area("GV Crate Ledge", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GV_CRATE_FREESTANDING_POH, true),
                }, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_FORTRESS_SIDE] = Area("GV Fortress Side", "Gerudo Valley", RA_GERUDO_VALLEY, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BrokenSwordAccess, {[]{return logic->IsAdult && (logic->PoachersSawAccess || logic->PoachersSaw);}}),
                }, {
                  //Locations                                       
                  LOCATION(RC_GV_CHEST,          logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
                  LOCATION(RC_GV_TRADE_SAW,      logic->IsAdult && logic->PoachersSaw),
                  LOCATION(RC_GV_GS_BEHIND_TENT, logic->IsAdult && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_GV_GS_PILLAR,      logic->IsAdult && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,   {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,     {[]{return logic->IsChild || logic->CanRideEpona || logic->CanUse(RG_LONGSHOT) || randoCtx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN) || logic->CarpenterRescue;}}),
                  Entrance(RR_GV_CARPENTER_TENT, {[]{return logic->IsAdult;}}),
                  Entrance(RR_GV_STORMS_GROTTO,  {[]{return logic->IsAdult && logic->CanOpenStormGrotto;}}),
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
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_REAR,    logic->CanStunDeku),
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_FRONT,   logic->CanStunDeku),
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE, logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_FORTRESS] = Area("Gerudo Fortress", "Gerudo Fortress", RA_GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CarpenterRescue, {[]{return logic->CanFinishGerudoFortress;}}),
                  EventAccess(&logic->GF_GateOpen,     {[]{return logic->IsAdult && logic->GerudoToken;}}),
                  EventAccess(&logic->GtG_GateOpen,    {[]{return logic->GtG_GateOpen || (logic->IsAdult && logic->GerudoToken && logic->ChildsWallet);}}),
                }, {
                  //Locations
                  LOCATION(RC_GF_CHEST,              logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_SCARECROW)) || logic->CanUse(RG_LONGSHOT)),
                  LOCATION(RC_GF_HBA_1000_POINTS,    logic->ChildsWallet && logic->GerudoToken && logic->CanRideEpona && logic->Bow && logic->AtDay),
                  LOCATION(RC_GF_HBA_1500_POINTS,    logic->ChildsWallet && logic->GerudoToken && logic->CanRideEpona && logic->Bow && logic->AtDay),
                  LOCATION(RC_GF_NORTH_F1_CARPENTER,  logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GF_NORTH_F2_CARPENTER, (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)) && (logic->GerudoToken || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || randoCtx->GetTrickOption(RT_GF_KITCHEN))),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER,  logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GF_SOUTH_F2_CARPENTER,  logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD)),
                  LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD,       logic->CanFinishGerudoFortress),
                  LOCATION(RC_GF_GS_ARCHERY_RANGE,   logic->IsAdult && logic->HookshotOrBoomerang && logic->GerudoToken && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_GF_GS_TOP_FLOOR,       logic->IsAdult && logic->AtNight && (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasExplosives || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE)) && (logic->GerudoToken || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || randoCtx->GetTrickOption(RT_GF_KITCHEN) || randoCtx->GetTrickOption(RT_GF_JUMP)) && logic->CanGetNightTimeGS),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE,                 {[]{return true;}}),
                  Entrance(RR_GF_OUTSIDE_GATE,                  {[]{return logic->GF_GateOpen;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUNDS_ENTRYWAY, {[]{return logic->GtG_GateOpen && (logic->IsAdult || randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES));}}),
                  Entrance(RR_GF_STORMS_GROTTO,                 {[]{return logic->IsAdult && logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_GF_OUTSIDE_GATE] = Area("GF Outside Gate", "Gerudo Fortress", RA_GERUDO_FORTRESS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GF_GateOpen, {[]{return logic->IsAdult && logic->GerudoToken && (randoCtx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) || randoCtx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) /*|| ShuffleSpecialIndoorEntrances*/);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,         {[]{return (logic->IsAdult && (logic->Hookshot || !randoCtx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES))) || logic->GF_GateOpen;}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_GF_STORMS_GROTTO] = Area("GF Storms Grotto", "GF Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_WASTELAND_NEAR_FORTRESS] = Area("Wasteland Near Fortress", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GF_OUTSIDE_GATE,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || randoCtx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_HAUNTED_WASTELAND] = Area("Haunted Wasteland", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot,       {[]{return true;}}),
                  EventAccess(&logic->NutPot,         {[]{return true;}}),
                  EventAccess(&logic->CarpetMerchant, {[]{return logic->AdultsWallet && (logic->CanJumpslash || logic->CanUse(RG_HOVER_BOOTS));}}),
                }, {
                  //Locations
                  LOCATION(RC_WASTELAND_CHEST,            logic->HasFireSource),
                  LOCATION(RC_WASTELAND_BOMBCHU_SALESMAN, logic->CarpetMerchant),
                  LOCATION(RC_WASTELAND_GS,               logic->HookshotOrBoomerang),
                }, {
                  //Exits
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS, {[]{return randoCtx->GetTrickOption(RT_LENS_HW) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || randoCtx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_WASTELAND_NEAR_COLOSSUS] = Area("Wasteland Near Colossus", "Haunted Wasteland", RA_HAUNTED_WASTELAND, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return randoCtx->GetTrickOption(RT_HW_REVERSE) || false;}}),
  });

  areaTable[RR_DESERT_COLOSSUS] = Area("Desert Colossus", "Desert Colossus", RA_DESERT_COLOSSUS, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPond, {[]{return logic->FairyPond || logic->CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&logic->BugRock,   {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_COLOSSUS_FREESTANDING_POH, logic->IsAdult && CanPlantBean(RR_DESERT_COLOSSUS)),
                  LOCATION(RC_COLOSSUS_GS_BEAN_PATCH,    logic->CanPlantBugs && logic->CanChildAttack),
                  LOCATION(RC_COLOSSUS_GS_TREE,          logic->IsAdult && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_COLOSSUS_GS_HILL,          logic->IsAdult && logic->AtNight && ((CanPlantBean(RR_DESERT_COLOSSUS) && logic->CanAdultAttack) || logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_COLOSSUS_GS) && logic->CanUse(RG_HOOKSHOT))) && logic->CanGetNightTimeGS),
                  LOCATION(RC_COLOSSUS_GOSSIP_STONE,     true),
                }, {
                  //Exits
                  Entrance(RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, {[]{return logic->HasExplosives;}}),
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS,       {[]{return true;}}),
                  Entrance(RR_COLOSSUS_GROTTO,               {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

  areaTable[RR_DESERT_COLOSSUS_FROM_SPIRIT_ENTRYWAY] = Area("Desert Colossus From Spirit Entryway", "Desert Colossus", RA_DESERT_COLOSSUS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHEIK_AT_COLOSSUS, true),
                }, {
                  //Exist
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = Area("Colossus Great Fairy Fountain", "Colossus Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_COLOSSUS_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GROTTO] = Area("Colossus Grotto", "Colossus Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku),
                  LOCATION(RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku),
                  LOCATION(RC_COLOSSUS_GROTTO_BEEHIVE,          logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });
}
