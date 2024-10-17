#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void RegionTable_Init_ZorasDomain() {
  areaTable[RR_ZR_FRONT] = Region("ZR Front", "Zora River", {RA_ZORAS_RIVER}, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ZR_GS_TREE, logic->IsChild && logic->CanAttack()),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER,  {[]{return logic->IsAdult || logic->BlastOrSmash();}}),
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_RIVER] = Region("Zora River", "Zora River", {RA_ZORAS_RIVER}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairy();}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_ZORAS_RIVER) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugShrub,         {[]{return logic->BugShrub         || logic->CanCutShrubs();}}),
                }, {
                  //Locations
                  LOCATION(RC_ZR_MAGIC_BEAN_SALESMAN,               logic->HasItem(RG_CHILD_WALLET) && logic->IsChild),
                  LOCATION(RC_ZR_FROGS_OCARINA_GAME,                logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanUse(RG_SARIAS_SONG) && logic->CanUse(RG_SUNS_SONG) && logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_ZR_FROGS_IN_THE_RAIN,                 logic->IsChild && logic->CanUse(RG_SONG_OF_STORMS)),
                  LOCATION(RC_ZR_FROGS_ZELDAS_LULLABY,              logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY)),
                  LOCATION(RC_ZR_FROGS_EPONAS_SONG,                 logic->IsChild && logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_ZR_FROGS_SARIAS_SONG,                 logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
                  LOCATION(RC_ZR_FROGS_SUNS_SONG,                   logic->IsChild && logic->CanUse(RG_SUNS_SONG)),
                  LOCATION(RC_ZR_FROGS_SONG_OF_TIME,                logic->IsChild && logic->CanUse(RG_SONG_OF_TIME)),
                  LOCATION(RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, logic->IsChild || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_ZR_LOWER))),
                  LOCATION(RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,      logic->IsChild || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && ctx->GetTrickOption(RT_ZR_UPPER))),
                  LOCATION(RC_ZR_GS_LADDER,                         logic->IsChild && logic->AtNight && logic->CanAttack() && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZR_GS_NEAR_RAISED_GROTTOS,            logic->IsAdult && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZR_GS_ABOVE_BRIDGE,                   logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,         true),
                  LOCATION(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,          true),
                }, {
                  //Exits
                  Entrance(RR_ZR_FRONT,            {[]{return true;}}),
                  Entrance(RR_ZR_OPEN_GROTTO,      {[]{return true;}}),
                  Entrance(RR_ZR_FAIRY_GROTTO,     {[]{return Here(RR_ZORAS_RIVER, []{return logic->BlastOrSmash();});}}),
                  Entrance(RR_THE_LOST_WOODS,      {[]{return logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_ZR_STORMS_GROTTO,    {[]{return logic->CanOpenStormsGrotto();}}),
                  Entrance(RR_ZR_BEHIND_WATERFALL, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) || (logic->IsChild && ctx->GetTrickOption(RT_ZR_CUCCO)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && ctx->GetTrickOption(RT_ZR_HOVERS));}}),
  });

  areaTable[RR_ZR_BEHIND_WATERFALL] = Region("ZR Behind Waterfall", "Zora River", {RA_ZORAS_RIVER}, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER,  {[]{return true;}}),
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZR_OPEN_GROTTO] = Region("ZR Open Grotto", "ZR Open Grotto", {}, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_ZR_OPEN_GROTTO_CHEST,         true),
                  LOCATION(RC_ZR_OPEN_GROTTO_FISH,          logic->HasBottle()),
                  LOCATION(RC_ZR_OPEN_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_ZR_OPEN_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives()),
                  LOCATION(RC_ZR_OPEN_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives()),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZR_FAIRY_GROTTO] = Region("ZR Fairy Grotto", "ZR Fairy Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Event
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZR_STORMS_GROTTO] = Region("ZR Storms Grotto", "ZR Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ZR_DEKU_SCRUB_GROTTO_REAR,  logic->CanStunDeku()),
                  LOCATION(RC_ZR_DEKU_SCRUB_GROTTO_FRONT, logic->CanStunDeku()),
                  LOCATION(RC_ZR_STORMS_GROTTO_BEEHIVE,   logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_DOMAIN] = Region("Zoras Domain", "Zoras Domain", {RA_ZORAS_DOMAIN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy,  {[]{return logic->CallGossipFairyExceptSuns();}}),
                  EventAccess(&logic->NutPot,            {[]{return true;}}),
                  EventAccess(&logic->StickPot,          {[]{return logic->StickPot          || logic->IsChild;}}),
                  EventAccess(&logic->FishGroup,         {[]{return logic->FishGroup         || logic->IsChild;}}),
                  EventAccess(&logic->KingZoraThawed,    {[]{return logic->KingZoraThawed    || (logic->IsAdult     && logic->BlueFire());}}),
                  EventAccess(&logic->DeliverLetter,     {[]{return logic->DeliverLetter     || (logic->CanUse(RG_RUTOS_LETTER) && logic->IsChild && ctx->GetOption(RSK_ZORAS_FOUNTAIN).IsNot(RO_ZF_OPEN));}}),
                }, {
                  //Locations
                  LOCATION(RC_ZD_DIVING_MINIGAME,                     logic->HasItem(RG_BRONZE_SCALE) && logic->HasItem(RG_CHILD_WALLET) && logic->IsChild),
                  LOCATION(RC_ZD_CHEST,                               logic->IsChild && logic->CanUse(RG_STICKS)),
                  LOCATION(RC_ZD_KING_ZORA_THAWED,                    logic->KingZoraThawed),
                  LOCATION(RC_ZD_TRADE_PRESCRIPTION,                  logic->KingZoraThawed && logic->CanUse(RG_PRESCRIPTION)),
                  LOCATION(RC_ZD_GS_FROZEN_WATERFALL,                 logic->IsAdult && logic->AtNight && (logic->HookshotOrBoomerang() || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || (logic->CanUse(RG_MAGIC_SINGLE) && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))) || (ctx->GetTrickOption(RT_ZD_GS) && logic->CanJumpslashExceptHammer())) && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZD_FISH_1,                              logic->IsChild && logic->HasBottle()),
                  LOCATION(RC_ZD_FISH_2,                              logic->IsChild && logic->HasBottle()),
                  LOCATION(RC_ZD_FISH_3,                              logic->IsChild && logic->HasBottle()),
                  LOCATION(RC_ZD_FISH_4,                              logic->IsChild && logic->HasBottle()),
                  LOCATION(RC_ZD_FISH_5,                              logic->IsChild && logic->HasBottle()),
                  LOCATION(RC_ZD_GOSSIP_STONE,                        true),
                  LOCATION(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_LEFT,  logic->CanBreakUpperBeehives()),
                  LOCATION(RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_RIGHT, logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_ZR_BEHIND_WATERFALL, {[]{return true;}}),
                  Entrance(RR_LAKE_HYLIA,          {[]{return logic->IsChild && (logic->HasItem(RG_SILVER_SCALE) || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_ZD_BEHIND_KING_ZORA, {[]{return logic->DeliverLetter || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult) || (ctx->GetTrickOption(RT_ZD_KING_ZORA_SKIP) && logic->IsAdult);}}),
                  Entrance(RR_ZD_SHOP,             {[]{return logic->IsChild || logic->BlueFire();}}),
                  Entrance(RR_ZD_STORMS_GROTTO,    {[]{return logic->CanOpenStormsGrotto();}}),
  });

  areaTable[RR_ZD_BEHIND_KING_ZORA] = Region("ZD Behind King Zora", "Zoras Domain", {RA_ZORAS_DOMAIN}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ZD_BEHIND_KING_ZORA_BEEHIVE, logic->CanBreakUpperBeehives()),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_DOMAIN,   {[]{return logic->DeliverLetter || ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (ctx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult);}}),
                  Entrance(RR_ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[RR_ZD_SHOP] = Region("ZD Shop", "ZD Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ZD_SHOP_ITEM_1, true),
                  LOCATION(RC_ZD_SHOP_ITEM_2, true),
                  LOCATION(RC_ZD_SHOP_ITEM_3, true),
                  LOCATION(RC_ZD_SHOP_ITEM_4, true),
                  LOCATION(RC_ZD_SHOP_ITEM_5, true),
                  LOCATION(RC_ZD_SHOP_ITEM_6, true),
                  LOCATION(RC_ZD_SHOP_ITEM_7, true),
                  LOCATION(RC_ZD_SHOP_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZD_STORMS_GROTTO] = Region("ZD Storms Grotto", "ZD Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_FOUNTAIN] = Region("Zoras Fountain", "Zoras Fountain", {RA_ZORAS_FOUNTAIN}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairyExceptSuns();}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || (logic->CanUse(RG_STICKS) && logic->AtDay);}}),
                }, {
                  //Locations
                  LOCATION(RC_ZF_ICEBERC_FREESTANDING_POH, logic->IsAdult),
                  LOCATION(RC_ZF_BOTTOM_FREESTANDING_POH,  logic->IsAdult && logic->CanUse(RG_IRON_BOOTS) && logic->WaterTimer() >= 24),
                  LOCATION(RC_ZF_GS_TREE,                  logic->IsChild),
                  LOCATION(RC_ZF_GS_ABOVE_THE_LOG,         logic->IsChild && logic->HookshotOrBoomerang() && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZF_GS_HIDDEN_CAVE,           logic->CanUse(RG_SILVER_GAUNTLETS) && logic->BlastOrSmash() && logic->HookshotOrBoomerang() && logic->IsAdult && logic->AtNight && logic->CanGetNightTimeGS()),
                  LOCATION(RC_ZF_FAIRY_GOSSIP_STONE,       true),
                  LOCATION(RC_ZF_JABU_GOSSIP_STONE,        true),
                }, {
                  //Exits
                  Entrance(RR_ZD_BEHIND_KING_ZORA,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, {[]{return (logic->IsChild && logic->CanUse(RG_BOTTLE_WITH_FISH));}}),
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,       {[]{return logic->IsAdult;}}),
                  Entrance(RR_ZF_GREAT_FAIRY_FOUNTAIN,   {[]{return logic->HasExplosives();}}),
  });

  areaTable[RR_ZF_GREAT_FAIRY_FOUNTAIN] = Region("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_ZF_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_FOUNTAIN, {[]{return true;}}),
  });
}
