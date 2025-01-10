#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void RegionTable_Init_GerudoValley() {
  areaTable[RR_GERUDO_VALLEY] = Region("Gerudo Valley", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BugRock, {[]{return logic->BugRock || logic->IsChild;}}),
                }, {
                  //Locations
                  LOCATION(RC_GV_GS_SMALL_BRIDGE, logic->IsChild && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,      {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return logic->IsChild || logic->CanUse(RG_LONGSHOT);}}),
                  Entrance(RR_GV_GROTTO_LEDGE,   {[]{return true;}}),
                  Entrance(RR_GV_FORTRESS_SIDE,  {[]{return (logic->IsAdult && (logic->CanUse(RG_EPONA) || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->CarpenterRescue)) || (logic->IsChild && logic->CanUse(RG_HOOKSHOT));}}),
  });

  areaTable[RR_GV_UPPER_STREAM] = Region("GV Upper Stream", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairy();}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS);}}),
                }, {
                  //Locations
                  LOCATION(RC_GV_WATERFALL_FREESTANDING_POH, logic->IsChild || logic->HasItem(RG_BRONZE_SCALE)),//can use cucco as child
                  LOCATION(RC_GV_GS_BEAN_PATCH,              logic->CanSpawnSoilSkull() && logic->CanAttack()),
                  LOCATION(RC_GV_COW,                        logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_GV_BEAN_SPROUT_FAIRY_1,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_GV_BEAN_SPROUT_FAIRY_2,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_GV_BEAN_SPROUT_FAIRY_3,        logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_GV_GOSSIP_STONE_FAIRY,         logic->CallGossipFairy()),
                  LOCATION(RC_GV_GOSSIP_STONE_FAIRY_BIG,     logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_GV_GOSSIP_STONE,               true),
                }, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_LOWER_STREAM] = Region("GV Lower Stream", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LAKE_HYLIA, {[]{return logic->IsChild || logic->HasItem(RG_BRONZE_SCALE);}}),//can use cucco as child
  });

  areaTable[RR_GV_GROTTO_LEDGE] = Region("GV Grotto Ledge", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_LOWER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GV_OCTOROK_GROTTO, {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });

  areaTable[RR_GV_CRATE_LEDGE] = Region("GV Crate Ledge", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GV_CRATE_FREESTANDING_POH, true),
                }, {
                  //Exits
                  Entrance(RR_GV_UPPER_STREAM, {[]{return ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives();}}),
                  Entrance(RR_GV_LOWER_STREAM, {[]{return true;}}),
  });

  areaTable[RR_GV_FORTRESS_SIDE] = Region("GV Fortress Side", "Gerudo Valley", {RA_GERUDO_VALLEY}, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GV_CHEST,          logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)),
                  LOCATION(RC_GV_TRADE_SAW,      logic->IsAdult && logic->CanUse(RG_POACHERS_SAW)),
                  LOCATION(RC_GV_GS_BEHIND_TENT, logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
                  LOCATION(RC_GV_GS_PILLAR,      logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,   {[]{return true;}}),
                  Entrance(RR_GV_UPPER_STREAM,   {[]{return true;}}),
                  Entrance(RR_GERUDO_VALLEY,     {[]{return logic->IsChild || logic->CanUse(RG_EPONA) || logic->CanUse(RG_LONGSHOT) || ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) || logic->CarpenterRescue;}}),
                  Entrance(RR_GV_CARPENTER_TENT, {[]{return logic->IsAdult;}}),
                  Entrance(RR_GV_STORMS_GROTTO,  {[]{return logic->IsAdult && logic->CanOpenStormsGrotto();}}),
                  Entrance(RR_GV_CRATE_LEDGE,    {[]{return ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives();}}),
  });

  areaTable[RR_GV_CARPENTER_TENT] = Region("GV Carpenter Tent", "GV Carpenter Tent", {}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[RR_GV_OCTOROK_GROTTO] = Region("GV Octorok Grotto", "GV Octorok Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_BLUE_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_BLUE_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_BACK_BLUE_RUPEE,         logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_LEFT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_FRONT_RIGHT_GREEN_RUPEE, logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_BACK_LEFT_GREEN_RUPEE,   logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_BACK_RIGHT_GREEN_RUPEE,  logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                  LOCATION(RC_GV_OCTOROK_GROTTO_RED_RUPEE,               logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS) || logic->CanUse(RG_BOOMERANG)),
                }, {
                  //Exits
                  Entrance(RR_GV_GROTTO_LEDGE, {[]{return true;}}),
  });

  areaTable[RR_GV_STORMS_GROTTO] = Region("GV Storms Grotto", "GV Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_REAR,    logic->CanStunDeku()),
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_FRONT,   logic->CanStunDeku()),
                  LOCATION(RC_GV_DEKU_SCRUB_GROTTO_BEEHIVE, logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE, {[]{return true;}}),
  });

  areaTable[RR_GERUDO_FORTRESS] = Region("Gerudo Fortress", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CarpenterRescue, {[]{return logic->CanFinishGerudoFortress();}}),
                  EventAccess(&logic->GF_GateOpen,     {[]{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD);}}),
                  EventAccess(&logic->GtG_GateOpen,    {[]{return logic->GtG_GateOpen || (logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->HasItem(RG_CHILD_WALLET));}}),
                }, {
                  //Locations
                  LOCATION(RC_GF_CHEST,                          logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && logic->CanUse(RG_SCARECROW)) || logic->CanUse(RG_LONGSHOT)),
                  LOCATION(RC_GF_HBA_1000_POINTS,                logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
                  LOCATION(RC_GF_HBA_1500_POINTS,                logic->HasItem(RG_CHILD_WALLET) && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanUse(RG_EPONA) && logic->CanUse(RG_FAIRY_BOW) && logic->AtDay),
                  LOCATION(RC_GF_NORTH_F1_CARPENTER,             logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
                  LOCATION(RC_GF_NORTH_F2_CARPENTER,             logic->CanKillEnemy(RE_GERUDO_WARRIOR) && (logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN))),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER,             logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
                  LOCATION(RC_GF_SOUTH_F2_CARPENTER,             logic->CanKillEnemy(RE_GERUDO_WARRIOR)),
                  LOCATION(RC_GF_GERUDO_MEMBERSHIP_CARD,         logic->CanFinishGerudoFortress()),
                  LOCATION(RC_GF_GS_ARCHERY_RANGE,               logic->IsAdult && logic->HookshotOrBoomerang() && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_GF_GS_TOP_FLOOR,                   logic->IsAdult && (logic->CanJumpslashExceptHammer() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_BOOMERANG) || logic->HasExplosives() || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_DINS_FIRE)) && (logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT) || logic->CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN) || ctx->GetTrickOption(RT_GF_JUMP)) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_GF_BREAK_ROOM_POT_1,               logic->CanBreakPots()),
                  LOCATION(RC_GF_BREAK_ROOM_POT_2,               logic->CanBreakPots()),
                  LOCATION(RC_GF_KITCHEN_POT_1,                  (logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT)) && logic->CanBreakPots()),
                  LOCATION(RC_GF_KITCHEN_POT_2,                  (logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT)) && logic->CanBreakPots()),
                  LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_1,       logic->CanBreakPots()),
                  LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_2,       logic->CanBreakPots()),
                  LOCATION(RC_GF_NORTH_F1_CARPENTER_POT_3,       logic->CanBreakPots()),
                  LOCATION(RC_GF_NORTH_F2_CARPENTER_POT_1,       logic->CanBreakPots()),
                  LOCATION(RC_GF_NORTH_F2_CARPENTER_POT_2,       logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_1,       logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_2,       logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_POT_3,       logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_1, logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_2, logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_3, logic->CanBreakPots()),
                  LOCATION(RC_GF_SOUTH_F1_CARPENTER_CELL_POT_4, logic->CanBreakPots()),
                  //RANDOTODO doublecheck when GF isn't a blob
                  LOCATION(RC_GF_KITCHEN_SUN_FAIRY,             (logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_HOOKSHOT)) && logic->CanUse(RG_SUNS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_GV_FORTRESS_SIDE,                 {[]{return true;}}),
                  Entrance(RR_GF_OUTSIDE_GATE,                  {[]{return logic->GF_GateOpen;}}),
                  Entrance(RR_GERUDO_TRAINING_GROUND_ENTRYWAY, {[]{return logic->GtG_GateOpen && (logic->IsAdult || ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES));}}),
                  Entrance(RR_GF_STORMS_GROTTO,                 {[]{return logic->IsAdult && logic->CanOpenStormsGrotto();}}),
  });

  areaTable[RR_GF_OUTSIDE_GATE] = Region("GF Outside Gate", "Gerudo Fortress", {RA_GERUDO_FORTRESS}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GF_GateOpen, {[]{return logic->IsAdult && logic->HasItem(RG_GERUDO_MEMBERSHIP_CARD) && (ctx->GetOption(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) /*|| ShuffleSpecialIndoorEntrances*/);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS,         {[]{return (logic->IsAdult && (logic->CanUse(RG_HOOKSHOT) || !ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES))) || logic->GF_GateOpen;}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_GF_STORMS_GROTTO] = Region("GF Storms Grotto", "GF Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_1,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_2,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_3,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_4,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_5,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_6,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_7,  true),
                  LOCATION(RC_GF_FAIRY_GROTTO_FAIRY_8,  true),
                }, {
                  //Exits
                  Entrance(RR_GERUDO_FORTRESS, {[]{return true;}}),
  });

  areaTable[RR_WASTELAND_NEAR_FORTRESS] = Region("Wasteland Near Fortress", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GF_OUTSIDE_GATE,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_HAUNTED_WASTELAND] = Region("Haunted Wasteland", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPot,       {[]{return true;}}),
                  EventAccess(&logic->NutPot,         {[]{return true;}}),
                  EventAccess(&logic->CarpetMerchant, {[]{return logic->HasItem(RG_ADULT_WALLET) && CanBuyAnother(RC_WASTELAND_BOMBCHU_SALESMAN) && (logic->CanJumpslashExceptHammer() || logic->CanUse(RG_HOVER_BOOTS));}}),
                }, {
                  //Locations
                  LOCATION(RC_WASTELAND_CHEST,            logic->HasFireSource()),
                  LOCATION(RC_WASTELAND_BOMBCHU_SALESMAN, logic->CanJumpslashExceptHammer() || logic->CanUse(RG_HOVER_BOOTS)),
                  LOCATION(RC_WASTELAND_GS,               logic->HookshotOrBoomerang()),
                  LOCATION(RC_WASTELAND_NEAR_GS_POT_1,    logic->CanBreakPots()),
                  LOCATION(RC_WASTELAND_NEAR_GS_POT_2,    logic->CanBreakPots()),
                  LOCATION(RC_WASTELAND_NEAR_GS_POT_3,    logic->CanBreakPots()),
                  LOCATION(RC_WASTELAND_NEAR_GS_POT_4,    logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS, {[]{return ctx->GetTrickOption(RT_LENS_HW) || logic->CanUse(RG_LENS_OF_TRUTH);}}),
                  Entrance(RR_WASTELAND_NEAR_FORTRESS, {[]{return logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT) || ctx->GetTrickOption(RT_HW_CROSSING);}}),
  });

  areaTable[RR_WASTELAND_NEAR_COLOSSUS] = Region("Wasteland Near Colossus", "Haunted Wasteland", {RA_HAUNTED_WASTELAND}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS,   {[]{return true;}}),
                  Entrance(RR_HAUNTED_WASTELAND, {[]{return ctx->GetTrickOption(RT_HW_REVERSE) || false;}}),
  });

  areaTable[RR_DESERT_COLOSSUS] = Region("Desert Colossus", "Desert Colossus", {RA_DESERT_COLOSSUS}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPond, {[]{return logic->FairyPond || logic->CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&logic->BugRock,   {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_COLOSSUS_FREESTANDING_POH,       logic->IsAdult && CanPlantBean(RR_DESERT_COLOSSUS)),
                  LOCATION(RC_COLOSSUS_GS_BEAN_PATCH,          logic->CanSpawnSoilSkull() && logic->CanAttack()),
                  LOCATION(RC_COLOSSUS_GS_TREE,                logic->IsAdult && logic->HookshotOrBoomerang() && logic->CanGetNightTimeGS()),
                  LOCATION(RC_COLOSSUS_GS_HILL,                logic->IsAdult && ((CanPlantBean(RR_DESERT_COLOSSUS) && logic->CanAttack()) || logic->CanUse(RG_LONGSHOT) || (ctx->GetTrickOption(RT_COLOSSUS_GS) && logic->CanUse(RG_HOOKSHOT))) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_COLOSSUS_BEAN_SPROUT_FAIRY_1,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_COLOSSUS_BEAN_SPROUT_FAIRY_2,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_COLOSSUS_BEAN_SPROUT_FAIRY_3,    logic->IsChild && logic->CanUse(RG_MAGIC_BEAN) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_COLOSSUS_GOSSIP_STONE_FAIRY,     logic->CallGossipFairy()),
                  LOCATION(RC_COLOSSUS_GOSSIP_STONE_FAIRY_BIG, logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_COLOSSUS_GOSSIP_STONE,           true),
                }, {
                  //Exits
                  //You can kinda get the fairies without entering the water, but it relies on them cooperating and leevers are jerks. should be a trick
                  Entrance(RR_DESERT_COLOSSUS_OASIS,         {[]{return logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, {[]{return logic->HasExplosives();}}),
                  Entrance(RR_SPIRIT_TEMPLE_ENTRYWAY,        {[]{return true;}}),
                  Entrance(RR_WASTELAND_NEAR_COLOSSUS,       {[]{return true;}}),
                  Entrance(RR_COLOSSUS_GROTTO,               {[]{return logic->CanUse(RG_SILVER_GAUNTLETS);}}),
  });

//specifically the full oasis, after the fairies have spawned
  areaTable[RR_DESERT_COLOSSUS_OASIS] = Region("Desert Colossus Oasis", "Desert Colossus", {RA_DESERT_COLOSSUS}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FairyPond, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_1, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_2, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_3, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_4, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_5, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_6, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_7, true),
                  LOCATION(RC_COLOSSUS_OASIS_FAIRY_8, true),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE] = Region("Desert Colossus From Spirit Entryway", "Desert Colossus", {RA_DESERT_COLOSSUS}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_SHEIK_AT_COLOSSUS, true),
                }, {
                  //Exist
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = Region("Colossus Great Fairy Fountain", "Colossus Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_COLOSSUS_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });

  areaTable[RR_COLOSSUS_GROTTO] = Region("Colossus Grotto", "Colossus Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku()),
                  LOCATION(RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku()),
                  LOCATION(RC_COLOSSUS_GROTTO_BEEHIVE,          logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_DESERT_COLOSSUS, {[]{return true;}}),
  });
}
