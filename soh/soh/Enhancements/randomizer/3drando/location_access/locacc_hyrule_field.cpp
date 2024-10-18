#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void RegionTable_Init_HyruleField() {
  areaTable[RR_HYRULE_FIELD] = Region("Hyrule Field", "Hyrule Field", {RA_HYRULE_FIELD}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BigPoeKill, {[]{return logic->CanUse(RG_FAIRY_BOW) && logic->CanUse(RG_EPONA) && logic->HasBottle();}}),
                }, {
                  //Locations
                  LOCATION(RC_HF_OCARINA_OF_TIME_ITEM,   logic->IsChild && logic->StoneCount() == 3),
                  LOCATION(RC_SONG_FROM_OCARINA_OF_TIME, logic->IsChild && logic->StoneCount() == 3),
                }, {
                  //Exits
                  Entrance(RR_LW_BRIDGE,              {[]{return true;}}),
                  Entrance(RR_LAKE_HYLIA,             {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,          {[]{return true;}}),
                  Entrance(RR_MARKET_ENTRANCE,        {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,       {[]{return true;}}),
                  Entrance(RR_ZR_FRONT,               {[]{return true;}}),
                  Entrance(RR_LON_LON_RANCH,          {[]{return true;}}),
                  Entrance(RR_HF_SOUTHEAST_GROTTO,    {[]{return Here(RR_HYRULE_FIELD, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_HF_OPEN_GROTTO,         {[]{return true;}}),
                  Entrance(RR_HF_INSIDE_FENCE_GROTTO, {[]{return logic->CanOpenBombGrotto();}}),
                  Entrance(RR_HF_COW_GROTTO,          {[]{return (logic->CanUse(RG_MEGATON_HAMMER) || logic->IsChild) && logic->CanOpenBombGrotto();}}),
                  Entrance(RR_HF_NEAR_MARKET_GROTTO,  {[]{return Here(RR_HYRULE_FIELD, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_HF_FAIRY_GROTTO,        {[]{return Here(RR_HYRULE_FIELD, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_HF_NEAR_KAK_GROTTO,     {[]{return logic->CanOpenBombGrotto();}}),
                  Entrance(RR_HF_TEKTITE_GROTTO,      {[]{return logic->CanOpenBombGrotto();}}),
  });

  areaTable[RR_HF_SOUTHEAST_GROTTO] = Region("HF Southeast Grotto", "HF Southeast Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_FISH,          logic->HasBottle()),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives()),
                  LOCATION(RC_HF_SOUTHEAST_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_OPEN_GROTTO] = Region("HF Open Grotto", "HF Open Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_OPEN_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_OPEN_GROTTO_FISH,          logic->HasBottle()),
                  LOCATION(RC_HF_OPEN_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives()),
                  LOCATION(RC_HF_OPEN_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_INSIDE_FENCE_GROTTO] = Region("HF Inside Fence Grotto", "HF Inside Fence Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_DEKU_SCRUB_GROTTO,           logic->CanStunDeku()),
                  LOCATION(RC_HF_INSIDE_FENCE_GROTTO_BEEHIVE, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_COW_GROTTO] = Region("HF Cow Grotto", "HF Cow Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_GS_COW_GROTTO,           logic->HasFireSource() && logic->HookshotOrBoomerang()),
                  LOCATION(RC_HF_COW_GROTTO_COW,          logic->HasFireSource() && logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_HF_COW_GROTTO_GOSSIP_STONE, logic->HasFireSource()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_MARKET_GROTTO] = Region("HF Near Market Grotto", "HF Near Market Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_CHEST,         true),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_FISH,          logic->HasBottle()),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives()),
                  LOCATION(RC_HF_NEAR_MARKET_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_FAIRY_GROTTO] = Region("HF Fairy Grotto", "HF Fairy Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_NEAR_KAK_GROTTO] = Region("HF Near Kak Grotto", "HF Near Kak Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_GS_NEAR_KAK_GROTTO, logic->HookshotOrBoomerang()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_HF_TEKTITE_GROTTO] = Region("HF Tektite Grotto", "HF Tektite Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, logic->HasItem(RG_GOLDEN_SCALE) || logic->CanUse(RG_IRON_BOOTS)),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_LAKE_HYLIA] = Region("Lake Hylia", "Lake Hylia", {RA_LAKE_HYLIA}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairy();}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_LAKE_HYLIA) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugShrub,         {[]{return logic->BugShrub         || (logic->IsChild && logic->CanCutShrubs());}}),
                  EventAccess(&logic->ChildScarecrow,   {[]{return logic->ChildScarecrow   || (logic->IsChild && logic->HasItem(RG_FAIRY_OCARINA) && logic->OcarinaButtons() >= 2);}}),
                  EventAccess(&logic->AdultScarecrow,   {[]{return logic->AdultScarecrow   || (logic->IsAdult && logic->HasItem(RG_FAIRY_OCARINA) && logic->OcarinaButtons() >= 2);}}),
                }, {
                  //Locations
                  LOCATION(RC_LH_UNDERWATER_ITEM,        logic->IsChild && logic->HasItem(RG_SILVER_SCALE)),
                  LOCATION(RC_LH_SUN,                    logic->IsAdult && logic->WaterTempleClear && logic->CanUse(RG_FAIRY_BOW)),
                  LOCATION(RC_LH_FREESTANDING_POH,       logic->IsAdult && (logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA))),
                  LOCATION(RC_LH_GS_BEAN_PATCH,          logic->CanSpawnSoilSkull() && logic->CanAttack()),
                  LOCATION(RC_LH_GS_LAB_WALL,            logic->IsChild && (logic->HookshotOrBoomerang() || (ctx->GetTrickOption(RT_LH_LAB_WALL_GS) && logic->CanJumpslashExceptHammer())) && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_LH_GS_SMALL_ISLAND,        logic->IsChild && logic->CanAttack() && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_LH_GS_TREE,                logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_LH_LAB_GOSSIP_STONE,       true),
                  LOCATION(RC_LH_SOUTHEAST_GOSSIP_STONE, true),
                  LOCATION(RC_LH_SOUTHWEST_GOSSIP_STONE, true),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,          {[]{return true;}}),
                  Entrance(RR_ZORAS_DOMAIN,          {[]{return logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_LH_OWL_FLIGHT,         {[]{return logic->IsChild;}}),
                  Entrance(RR_LH_FISHING_ISLAND,     {[]{return logic->IsChild || logic->CanUse(RG_SCARECROW) || CanPlantBean(RR_LAKE_HYLIA) || logic->WaterTempleClear;}}),
                  Entrance(RR_LH_LAB,                {[]{return true;}}),
                  Entrance(RR_WATER_TEMPLE_ENTRYWAY, {[]{return logic->CanUse(RG_HOOKSHOT) && ((logic->CanUse(RG_IRON_BOOTS) || (ctx->GetTrickOption(RT_LH_WATER_HOOKSHOT) && logic->HasItem(RG_GOLDEN_SCALE))) || (logic->IsAdult && logic->CanUse(RG_LONGSHOT) && logic->HasItem(RG_GOLDEN_SCALE)));}}),
                  Entrance(RR_LH_GROTTO,             {[]{return true;}}),
  });

  areaTable[RR_LH_FISHING_ISLAND] = Region("LH Fishing Island", "Lake Hylia", {RA_LAKE_HYLIA}, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA,      {[]{return true;}}),
                  Entrance(RR_LH_FISHING_HOLE, {[]{return true;}}),
  });

  areaTable[RR_LH_OWL_FLIGHT] = Region("LH Owl Flight", "Lake Hylia", {RA_LAKE_HYLIA}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}, false),
  });

  areaTable[RR_LH_LAB] = Region("LH Lab", "LH Lab", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LH_LAB_DIVE,     logic->HasItem(RG_GOLDEN_SCALE) || (ctx->GetTrickOption(RT_LH_LAB_DIVING) && logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT))),
                  LOCATION(RC_LH_TRADE_FROG,   logic->IsAdult && logic->CanUse(RG_EYEBALL_FROG)),
                  LOCATION(RC_LH_GS_LAB_CRATE, logic->CanUse(RG_IRON_BOOTS) && logic->CanUse(RG_HOOKSHOT)),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  // TODO: should some of these helpers be done via events instead?
  areaTable[RR_LH_FISHING_HOLE] = Region("LH Fishing Hole", "LH Fishing Hole", {}, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LH_CHILD_FISHING,  logic->CanUse(RG_FISHING_POLE) && logic->IsChild),
                  LOCATION(RC_LH_CHILD_FISH_1,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_2,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_3,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_4,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_5,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_6,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_7,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_8,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_9,   logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_10,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_11,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_12,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_13,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_14,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_FISH_15,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_LOACH_1,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_CHILD_LOACH_2,  logic->CanUse(RG_FISHING_POLE) && (logic->IsChild || ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT))),
                  LOCATION(RC_LH_ADULT_FISHING,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult),
                  LOCATION(RC_LH_ADULT_FISH_1,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_2,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_3,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_4,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_5,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_6,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_7,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_8,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_9,   logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_10,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_11,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_12,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_13,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_14,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_FISH_15,  logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_ADULT_LOACH,    logic->CanUse(RG_FISHING_POLE) && logic->IsAdult && ctx->GetOption(RSK_FISHSANITY_AGE_SPLIT)),
                  LOCATION(RC_LH_HYRULE_LOACH,   logic->CanUse(RG_FISHING_POLE)),
                  LOCATION(RC_FISHING_POLE_HINT, true),
                }, {
                  //Exits
                  Entrance(RR_LH_FISHING_ISLAND, {[]{return true;}}),
  });

  areaTable[RR_LH_GROTTO] = Region("LH Grotto", "LH Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
                  LOCATION(RC_LH_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
                  LOCATION(RC_LH_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[RR_LON_LON_RANCH] = Region("Lon Lon Ranch", "Lon Lon Ranch", {RA_LON_LON_RANCH}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreedEpona,    {[]{return logic->FreedEpona    || ((logic->HasItem(RG_CHILD_WALLET) || ctx->GetOption(RSK_SKIP_EPONA_RACE)) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay);}}),
                  EventAccess(&logic->LinksCow, {[]{return logic->LinksCow || (logic->HasItem(RG_CHILD_WALLET) && logic->CanUse(RG_EPONAS_SONG) && logic->IsAdult && logic->AtDay);}}),
                }, {
                  //Locations
                  LOCATION(RC_SONG_FROM_MALON,     logic->IsChild && logic->HasItem(RG_ZELDAS_LETTER) && logic->HasItem(RG_FAIRY_OCARINA) && logic->AtDay),
                  LOCATION(RC_LLR_GS_TREE,         logic->IsChild),
                  LOCATION(RC_LLR_GS_RAIN_SHED,    logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_LLR_GS_HOUSE_WINDOW, logic->IsChild && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_LLR_GS_BACK_WALL,    logic->IsChild && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,     {[]{return true;}}),
                  Entrance(RR_LLR_TALONS_HOUSE, {[]{return true;}}),
                  Entrance(RR_LLR_STABLES,      {[]{return true;}}),
                  Entrance(RR_LLR_TOWER,        {[]{return true;}}),
                  Entrance(RR_LLR_GROTTO,       {[]{return logic->IsChild;}}),
  });

  areaTable[RR_LLR_TALONS_HOUSE] = Region("LLR Talons House", "LLR Talons House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_TALONS_CHICKENS, logic->HasItem(RG_CHILD_WALLET) && logic->IsChild && logic->AtDay && logic->HasItem(RG_ZELDAS_LETTER)),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_STABLES] = Region("LLR Stables", "LLR Stables", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_STABLES_LEFT_COW,  logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_LLR_STABLES_RIGHT_COW, logic->CanUse(RG_EPONAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_TOWER] = Region("LLR Tower", "LLR Tower", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_FREESTANDING_POH, logic->IsChild),
                  LOCATION(RC_LLR_TOWER_LEFT_COW,   logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_LLR_TOWER_RIGHT_COW,  logic->CanUse(RG_EPONAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[RR_LLR_GROTTO] = Region("LLR Grotto", "LLR Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku()),
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku()),
                  LOCATION(RC_LLR_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku()),
                  LOCATION(RC_LLR_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_LON_LON_RANCH, {[]{return true;}}),
  });
}
