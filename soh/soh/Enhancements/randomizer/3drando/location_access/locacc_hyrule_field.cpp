#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_HyruleField() {
  areaTable[RR_HYRULE_FIELD] = Area("Hyrule Field", "Hyrule Field", RA_HYRULE_FIELD, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BigPoeKill, {[]{return logic->CanUse(RG_FAIRY_BOW) && logic->CanRideEpona && logic->HasBottle;}}),
                }, {
                  //Locations
                  LOCATION(RC_HF_OCARINA_OF_TIME_ITEM,   logic->IsChild && logic->HasAllStones),
                  LOCATION(RC_SONG_FROM_OCARINA_OF_TIME, logic->IsChild && logic->HasAllStones),
                }, {
                  //Exits
                  Entrance(RR_LW_BRIDGE,              {[]{return true;}}),
                  Entrance(RR_LAKE_HYLIA,             {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,          {[]{return true;}}),
                  Entrance(RR_MARKET_ENTRANCE,        {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,       {[]{return true;}}),
                  Entrance(RR_ZR_FRONT,               {[]{return true;}}),
                  Entrance(RR_LON_LON_RANCH,          {[]{return true;}}),
                  Entrance(RR_HF_SOUTHEAST_GROTTO,    {[]{return Here(RR_HYRULE_FIELD, []{return logic->CanBlastOrSmash;});}}),
                  Entrance(RR_HF_OPEN_GROTTO,         {[]{return true;}}),
                  Entrance(RR_HF_INSIDE_FENCE_GROTTO, {[]{return logic->CanOpenBombGrotto;}}),
                  Entrance(RR_HF_COW_GROTTO,          {[]{return (logic->CanUse(RG_MEGATON_HAMMER) || logic->IsChild) && logic->CanOpenBombGrotto;}}),
                  Entrance(RR_HF_NEAR_MARKET_GROTTO,  {[]{return Here(RR_HYRULE_FIELD, []{return logic->CanBlastOrSmash;});}}),
                  Entrance(RR_HF_FAIRY_GROTTO,        {[]{return Here(RR_HYRULE_FIELD, []{return logic->CanBlastOrSmash;});}}),
                  Entrance(RR_HF_NEAR_KAK_GROTTO,     {[]{return logic->CanOpenBombGrotto;}}),
                  Entrance(RR_HF_TEKTITE_GROTTO,      {[]{return logic->CanOpenBombGrotto;}}),
  });

  areaTable[RR_HF_SOUTHEAST_GROTTO] = Area("HF Southeast Grotto", "HF Southeast Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_FISH,          logic->HasBottle),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_OPEN_GROTTO] = Area("HF Open Grotto", "HF Open Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_OPEN_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_OPEN_GROTTO_FISH,          logic->HasBottle),
                  LOCATION(RC_HF_OPEN_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives),
                  LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_INSIDE_FENCE_GROTTO] = Area("HF Inside Fence Grotto", "HF Inside Fence Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_DEKU_SCRUB_GROTTO,           logic->CanStunDeku),
                  LOCATION(RC_HF_INSIDE_FENCE_GROTTO_BEEHIVE, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_COW_GROTTO] = Area("HF Cow Grotto", "HF Cow Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_GS_COW_GROTTO,           logic->HasFireSource && logic->HookshotOrBoomerang),
                  LOCATION(RC_HF_COW_GROTTO_COW,          logic->HasFireSource && logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_HF_COW_GROTTO_GOSSIP_STONE, logic->HasFireSource),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_MARKET_GROTTO] = Area("HF Near Market Grotto", "HF Near Market Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_FISH,          logic->HasBottle),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_FAIRY_GROTTO] = Area("HF Fairy Grotto", "HF Fairy Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_KAK_GROTTO] = Area("HF Near Kak Grotto", "HF Near Kak Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_GS_NEAR_KAK_GROTTO, logic->HookshotOrBoomerang),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_TEKTITE_GROTTO] = Area("HF Tektite Grotto", "HF Tektite Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, logic->ProgressiveScale >= 2 || logic->CanUse(RG_IRON_BOOTS)),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_LAKE_HYLIA] = Area("Lake Hylia", "Lake Hylia", RA_LAKE_HYLIA, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairy;}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_LAKE_HYLIA) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugShrub,         {[]{return logic->BugShrub         || (logic->IsChild && logic->CanCutShrubs);}}),
                  EventAccess(&logic->ChildScarecrow,   {[]{return logic->ChildScarecrow   || (logic->IsChild && logic->Ocarina && logic->OcarinaButtons >= 2);}}),
                  EventAccess(&logic->AdultScarecrow,   {[]{return logic->AdultScarecrow   || (logic->IsAdult && logic->Ocarina && logic->OcarinaButtons >= 2);}}),
                }, {
                  //Locations
                  LOCATION(RC_LH_UNDERWATER_ITEM,        logic->IsChild && logic->CanDive),
                  LOCATION(RC_LH_SUN,                    logic->IsAdult && logic->WaterTempleClear && logic->CanUse(RG_FAIRY_BOW)),
                  LOCATION(RC_LH_FREESTANDING_POH,       logic->IsAdult && (logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA))),
                  LOCATION(RC_LH_GS_BEAN_PATCH,          logic->CanPlantBugs && logic->CanChildAttack),
                  LOCATION(RC_LH_GS_LAB_WALL,            logic->IsChild && (logic->HookshotOrBoomerang || (randoCtx->GetTrickOption(RT_LH_LAB_WALL_GS) && logic->CanJumpslash)) && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_LH_GS_SMALL_ISLAND,        logic->IsChild && logic->CanChildAttack && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_LH_GS_TREE,                logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_LH_LAB_GOSSIP_STONE,       true),
                  LOCATION(RC_LH_SOUTHEAST_GOSSIP_STONE, true),
                  LOCATION(RC_LH_SOUTHWEST_GOSSIP_STONE, true),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,          {[]{return true;}}),
                  Entrance(RR_ZORAS_DOMAIN,          {[]{return logic->IsChild && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_LH_OWL_FLIGHT,         {[]{return logic->IsChild;}}),
                  Entrance(RR_LH_FISHING_ISLAND,     {[]{return logic->IsChild || logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA) || logic->WaterTempleClear;}}),
                  Entrance(RR_LH_LAB,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY, {[]{return logic->CanUse(RG_HOOKSHOT) && ((logic->CanUse(RG_IRON_BOOTS) || (randoCtx->GetTrickOption(RT_LH_WATER_HOOKSHOT) && logic->ProgressiveScale >= 2)) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->ProgressiveScale >= 2));}}),
                  Entrance(RR_LH_GROTTO,             {[]{return true;}}),
  });

  areaTable[RR_LH_FISHING_ISLAND] = Area("LH Fishing Island", "Lake Hylia", RA_LAKE_HYLIA, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA,      {[]{return true;}}),
                  Entrance(RR_LH_FISHING_HOLE, {[]{return true;}}),
  });

  areaTable[RR_LH_OWL_FLIGHT] = Area("LH Owl Flight", "Lake Hylia", RA_LAKE_HYLIA, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_LH_LAB] = Area("LH Lab", "LH Lab", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->EyedropsAccess, {[]{return logic->EyedropsAccess || (logic->IsAdult && (logic->EyeballFrogAccess || (logic->EyeballFrog && logic->DisableTradeRevert)));}}),
                }, {
                  //Locations
                  LOCATION(RC_LH_LAB_DIVE,     logic->ProgressiveScale >= 2 || (randoCtx->GetTrickOption(RT_LH_LAB_DIVING) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
                  LOCATION(RC_LH_TRADE_FROG,   logic->IsAdult && logic->EyeballFrog),
                  LOCATION(RC_LH_GS_LAB_CRATE, logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  // TODO: should some of these helpers be done via events instead?
  areaTable[RR_LH_FISHING_HOLE] = Area("LH Fishing Hole", "LH Fishing Hole", RA_NONE, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LH_CHILD_FISHING,  logic->CanFish && logic->IsChild),
                  LOCATION(RC_LH_CHILD_FISH_1,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_2,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_3,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_4,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_5,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_6,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_7,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_8,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_9,   logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_10,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_11,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_12,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_13,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_14,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_FISH_15,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_LOACH_1,  logic->CanGetChildFish),
                  LOCATION(RC_LH_CHILD_LOACH_2,  logic->CanGetChildFish),
                  LOCATION(RC_LH_ADULT_FISHING,  logic->CanFish && logic->IsAdult),
                  LOCATION(RC_LH_ADULT_FISH_1,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_2,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_3,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_4,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_5,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_6,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_7,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_8,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_9,   logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_10,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_11,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_12,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_13,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_14,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_FISH_15,  logic->CanGetAdultFish),
                  LOCATION(RC_LH_ADULT_LOACH,    logic->CanGetAdultFish),
                  LOCATION(RC_LH_HYRULE_LOACH,   logic->CanFish),
                  LOCATION(RC_FISHING_POLE_HINT, true),
                }, {
                  //Exits
                  Entrance(RR_LH_FISHING_ISLAND, {[]{return true;}}),
  });

  areaTable[RR_LH_GROTTO] = Area("LH Grotto", "LH Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku),
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku),
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku),
                  LOCATION(RC_LH_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_LON_LON_RANCH] = Area("Lon Lon Ranch", "Lon Lon Ranch", RA_LON_LON_RANCH, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->Epona,    {[]{return logic->Epona    || ((logic->ChildsWallet || randoCtx->GetOption(RSK_SKIP_EPONA_RACE)) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay);}}),
                  EventAccess(&logic->LinksCow, {[]{return logic->LinksCow || (logic->ChildsWallet && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay);}}),
                }, {
                  //Locations
                  LOCATION(RC_SONG_FROM_MALON,     logic->IsChild && logic->ZeldasLetter && logic->Ocarina && logic->AtDay),
                  LOCATION(RC_LLR_GS_TREE,         logic->IsChild),
                  LOCATION(RC_LLR_GS_RAIN_SHED,    logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_LLR_GS_HOUSE_WINDOW, logic->IsChild && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                  LOCATION(RC_LLR_GS_BACK_WALL,    logic->IsChild && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,     {[]{return true;}}),
                  Entrance(RR_LLR_TALONS_HOUSE, {[]{return true;}}),
                  Entrance(RR_LLR_STABLES,      {[]{return true;}}),
                  Entrance(RR_LLR_TOWER,        {[]{return true;}}),
                  Entrance(RR_LLR_GROTTO,       {[]{return logic->IsChild;}}),
  });

  areaTable[RR_LLR_TALONS_HOUSE] = Area("LLR Talons House", "LLR Talons House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_TALONS_CHICKENS, logic->ChildsWallet && logic->IsChild && logic->AtDay && logic->ZeldasLetter),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_STABLES] = Area("LLR Stables", "LLR Stables", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_STABLES_LEFT_COW,  logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_LLR_STABLES_RIGHT_COW, logic->CanUse(RG_EPONAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_TOWER] = Area("LLR Tower", "LLR Tower", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_FREESTANDING_POH, logic->IsChild),
                  LOCATION(RC_LLR_TOWER_LEFT_COW,   logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_LLR_TOWER_RIGHT_COW,  logic->CanUse(RG_EPONAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_GROTTO] = Area("LLR Grotto", "LLR Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku),
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku),
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku),
                  LOCATION(RC_LLR_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });
}
