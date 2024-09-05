#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_DeathMountain() {
  auto ctx = Rando::Context::GetInstance();
  areaTable[RR_DEATH_MOUNTAIN_TRAIL] = Area("Death Mountain", "Death Mountain", RA_DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BeanPlantFairy, {[]{return logic->BeanPlantFairy || (CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && logic->CanUse(RG_SONG_OF_STORMS) && (logic->HasExplosives || logic->GoronBracelet));}}),
                }, {
                  //Locations
                  LOCATION(RC_DMT_CHEST,                    logic->CanBlastOrSmash || (randoCtx->GetTrickOption(RT_DMT_BOMBABLE) && logic->IsChild && logic->GoronBracelet)),
                  LOCATION(RC_DMT_FREESTANDING_POH,         logic->CanTakeDamage || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && (logic->HasExplosives || logic->GoronBracelet))),
                  LOCATION(RC_DMT_GS_BEAN_PATCH,            logic->CanPlantBugs && (logic->HasExplosives || logic->GoronBracelet || (randoCtx->GetTrickOption(RT_DMT_SOIL_GS) && (logic->CanTakeDamage || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_BOOMERANG)))),
                  LOCATION(RC_DMT_GS_NEAR_KAK,              logic->CanBlastOrSmash),
                  LOCATION(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || (randoCtx->GetTrickOption(RT_DMT_HOOKSHOT_LOWER_GS) && logic->CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_DMT_BEAN_LOWER_GS) && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL)) || (randoCtx->GetTrickOption(RT_DMT_HOVERS_LOWER_GS) && logic->CanUse(RG_HOVER_BOOTS)) || randoCtx->GetTrickOption(RT_DMT_JS_LOWER_GS)) && logic->CanGetNightTimeGS),
                }, {
                  //Exits
                  Entrance(RR_KAK_BEHIND_GATE,          {[]{return true;}}),
                  Entrance(RR_GORON_CITY,               {[]{return true;}}),
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT,    {[]{return Here(RR_DEATH_MOUNTAIN_TRAIL, []{return logic->CanBlastOrSmash;}) || (logic->IsAdult && ((CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && logic->GoronBracelet) || (logic->HoverBoots && randoCtx->GetTrickOption(RT_DMT_CLIMB_HOVERS))));}}),
                  Entrance(RR_DODONGOS_CAVERN_ENTRYWAY, {[]{return logic->HasExplosives || logic->GoronBracelet || logic->IsAdult;}}),
                  Entrance(RR_DMT_STORMS_GROTTO,        {[]{return logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_DEATH_MOUNTAIN_SUMMIT] = Area("Death Mountain Summit", "Death Mountain", RA_DEATH_MOUNTAIN_TRAIL, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->PrescriptionAccess, {[]{return logic->PrescriptionAccess || (logic->IsAdult && (logic->BrokenSwordAccess || logic->BrokenSword));}}),
                  EventAccess(&logic->GossipStoneFairy,   {[]{return logic->GossipStoneFairy   || logic->CanSummonGossipFairy;}}),
                  EventAccess(&logic->BugRock,            {[]{return logic->BugRock            || logic->IsChild;}}),
                }, {
                  //Locations
                  LOCATION(RC_DMT_TRADE_BROKEN_SWORD,    logic->IsAdult && logic->BrokenSword),
                  LOCATION(RC_DMT_TRADE_EYEDROPS,        logic->IsAdult && logic->Eyedrops),
                  LOCATION(RC_DMT_TRADE_CLAIM_CHECK,     logic->IsAdult && logic->ClaimCheck),
                  LOCATION(RC_DMT_GS_FALLING_ROCKS_PATH, logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || randoCtx->GetTrickOption(RT_DMT_UPPER_GS)) && logic->CanGetNightTimeGS),
                  LOCATION(RC_DMT_GOSSIP_STONE,          true),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL,     {[]{return true;}}),
                  Entrance(RR_DMC_UPPER_LOCAL,          {[]{return true;}}),
                  Entrance(RR_DMT_OWL_FLIGHT,           {[]{return logic->IsChild;}}),
                  Entrance(RR_DMT_COW_GROTTO,           {[]{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return logic->CanBlastOrSmash;});}}),
                  Entrance(RR_DMT_GREAT_FAIRY_FOUNTAIN, {[]{return Here(RR_DEATH_MOUNTAIN_SUMMIT, []{return logic->CanBlastOrSmash;});}}),
  });

  areaTable[RR_DMT_OWL_FLIGHT] = Area("DMT Owl Flight", "Death Mountain", RA_DEATH_MOUNTAIN_TRAIL, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_ROOFTOP, {[]{return true;}}),
  });

  areaTable[RR_DMT_COW_GROTTO] = Area("DMT Cow Grotto", "DMT Cow Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMT_COW_GROTTO_COW,     logic->CanUse(RG_EPONAS_SONG)),
                  LOCATION(RC_DMT_COW_GROTTO_BEEHIVE, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),

  });

  areaTable[RR_DMT_STORMS_GROTTO] = Area("DMT Storms Grotto", "DMT Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_DMT_STORMS_GROTTO_CHEST,         true),
                  LOCATION(RC_DMT_STORMS_GROTTO_FISH,          logic->HasBottle),
                  LOCATION(RC_DMT_STORMS_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives),
                  LOCATION(RC_DMT_STORMS_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[RR_DMT_GREAT_FAIRY_FOUNTAIN] = Area("DMT Great Fairy Fountain", "DMT Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMT_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
  });

  areaTable[RR_GORON_CITY] = Area("Goron City", "Goron City", RA_GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy,          {[]{return logic->GossipStoneFairy          || logic->CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&logic->StickPot,                  {[]{return logic->StickPot                  || logic->IsChild;}}),
                  EventAccess(&logic->BugRock,                   {[]{return logic->BugRock                   || (logic->CanBlastOrSmash || logic->CanUse(RG_SILVER_GAUNTLETS));}}),
                  EventAccess(&logic->GoronCityChildFire,        {[]{return logic->GoronCityChildFire        || (logic->IsChild && logic->CanUse(RG_DINS_FIRE));}}),
                  EventAccess(&logic->GCWoodsWarpOpen,           {[]{return logic->GCWoodsWarpOpen           || (logic->CanBlastOrSmash || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_FAIRY_BOW) || logic->GoronBracelet || logic->GoronCityChildFire);}}),
                  EventAccess(&logic->GCDaruniasDoorOpenChild,   {[]{return logic->GCDaruniasDoorOpenChild   || (logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY));}}),
                  EventAccess(&logic->StopGCRollingGoronAsAdult, {[]{return logic->StopGCRollingGoronAsAdult || (logic->IsAdult && (logic->GoronBracelet || logic->HasExplosives || logic->Bow || (randoCtx->GetTrickOption(RT_GC_LINK_GORON_DINS) && logic->CanUse(RG_DINS_FIRE))));}}),
                }, {
                  //Locations
                  LOCATION(RC_GC_MAZE_LEFT_CHEST,        logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_SILVER_GAUNTLETS) || (randoCtx->GetTrickOption(RT_GC_LEFTMOST) && logic->HasExplosives && logic->CanUse(RG_HOVER_BOOTS))),
                  LOCATION(RC_GC_MAZE_CENTER_CHEST,      logic->CanBlastOrSmash  || logic->CanUse(RG_SILVER_GAUNTLETS)),
                  LOCATION(RC_GC_MAZE_RIGHT_CHEST,       logic->CanBlastOrSmash  || logic->CanUse(RG_SILVER_GAUNTLETS)),
                  LOCATION(RC_GC_POT_FREESTANDING_POH,   logic->IsChild && logic->GoronCityChildFire && (logic->Bombs || (logic->GoronBracelet && randoCtx->GetTrickOption(RT_GC_POT_STRENGTH)) || (logic->CanUse(RG_BOMBCHU_5) && randoCtx->GetTrickOption(RT_GC_POT)))),
                  LOCATION(RC_GC_ROLLING_GORON_AS_CHILD, logic->IsChild && (logic->HasExplosives || (logic->GoronBracelet && randoCtx->GetTrickOption(RT_GC_ROLLING_STRENGTH)))),
                  LOCATION(RC_GC_ROLLING_GORON_AS_ADULT, logic->StopGCRollingGoronAsAdult),
                  LOCATION(RC_GC_GS_BOULDER_MAZE,        logic->IsChild && logic->CanBlastOrSmash),
                  LOCATION(RC_GC_GS_CENTER_PLATFORM,     logic->CanAdultAttack),
                  LOCATION(RC_GC_MEDIGORON,              logic->IsAdult && logic->AdultsWallet && (logic->CanBlastOrSmash || logic->GoronBracelet)),
                  LOCATION(RC_GC_MAZE_GOSSIP_STONE,      logic->CanBlastOrSmash || logic->CanUse(RG_SILVER_GAUNTLETS)),
                  LOCATION(RC_GC_MEDIGORON_GOSSIP_STONE, logic->CanBlastOrSmash || logic->GoronBracelet),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
                  Entrance(RR_GC_WOODS_WARP,        {[]{return logic->GCWoodsWarpOpen;}}),
                  Entrance(RR_GC_SHOP,              {[]{return (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && (logic->CanBlastOrSmash || logic->GoronBracelet || logic->GoronCityChildFire || logic->CanUse(RG_FAIRY_BOW)));}}),
                  Entrance(RR_GC_DARUNIAS_CHAMBER,  {[]{return (logic->IsAdult && logic->StopGCRollingGoronAsAdult) || (logic->IsChild && logic->GCDaruniasDoorOpenChild);}}),
                  Entrance(RR_GC_GROTTO_PLATFORM,   {[]{return logic->IsAdult && ((logic->CanUse(RG_SONG_OF_TIME) && ((logic->EffectiveHealth > 2) || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_NAYRUS_LOVE))) || (logic->EffectiveHealth > 1 && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_HOOKSHOT)) || (logic->CanUse(RG_NAYRUS_LOVE) && logic->CanUse(RG_HOOKSHOT)) || (logic->EffectiveHealth > 2 && logic->CanUse(RG_HOOKSHOT) && randoCtx->GetTrickOption(RT_GC_GROTTO)));}}),
  });

  areaTable[RR_GC_WOODS_WARP] = Area("GC Woods Warp", "Goron City", RA_GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GCWoodsWarpOpen, {[]{return logic->GCWoodsWarpOpen || (logic->CanBlastOrSmash || logic->CanUse(RG_DINS_FIRE));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_GORON_CITY,     {[]{return logic->CanLeaveForest && logic->GCWoodsWarpOpen;}}),
                  Entrance(RR_THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[RR_GC_DARUNIAS_CHAMBER] = Area("GC Darunias Chamber", "Goron City", RA_GORON_CITY, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GoronCityChildFire, {[]{return logic->GoronCityChildFire || (logic->IsChild && logic->CanUse(RG_STICKS));}}),
                }, {
                  //Locations
                  LOCATION(RC_GC_DARUNIAS_JOY, logic->IsChild && logic->CanUse(RG_SARIAS_SONG)),
                }, {
                  //Exits
                  Entrance(RR_GORON_CITY,      {[]{return true;}}),
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_GC_GROTTO_PLATFORM] = Area("GC Grotto Platform", "Goron City", RA_GORON_CITY, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_GC_GROTTO,  {[]{return true;}}),
                  Entrance(RR_GORON_CITY, {[]{return logic->EffectiveHealth > 2 || logic->CanUse(RG_GORON_TUNIC) || logic->CanUse(RG_NAYRUS_LOVE) || ((logic->IsChild || logic->CanUse(RG_SONG_OF_TIME)) && logic->CanUse(RG_LONGSHOT));}}),
  });

  areaTable[RR_GC_SHOP] = Area("GC Shop", "GC Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GC_SHOP_ITEM_1, true),
                  LOCATION(RC_GC_SHOP_ITEM_2, true),
                  LOCATION(RC_GC_SHOP_ITEM_3, true),
                  LOCATION(RC_GC_SHOP_ITEM_4, true),
                  LOCATION(RC_GC_SHOP_ITEM_5, true),
                  LOCATION(RC_GC_SHOP_ITEM_6, true),
                  LOCATION(RC_GC_SHOP_ITEM_7, true),
                  LOCATION(RC_GC_SHOP_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_GORON_CITY, {[]{return true;}}),
  });

  areaTable[RR_GC_GROTTO] = Area("GC Grotto", "GC Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku),
                  LOCATION(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku),
                  LOCATION(RC_GC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku),
                  LOCATION(RC_GC_GROTTO_BEEHIVE,           logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_GC_GROTTO_PLATFORM, {[]{return true;}}),
  });

  areaTable[RR_DMC_UPPER_NEARBY] = Area("DMC Upper Nearby", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_UPPER_LOCAL,       {[]{return logic->FireTimer >= 48;}}),
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),
                  Entrance(RR_DMC_UPPER_GROTTO,      {[]{return Here(RR_DMC_UPPER_NEARBY, []{return logic->CanBlastOrSmash && (logic->FireTimer >= 8 || logic->Hearts >= 3);});}})
  });

  areaTable[RR_DMC_UPPER_LOCAL] = Area("DMC Upper Local", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || (logic->HasExplosives && logic->CanSummonGossipFairyWithoutSuns && (logic->FireTimer >= 16 || logic->Hearts >= 3));}}),
                }, {
                  //Locations
                  LOCATION(RC_DMC_WALL_FREESTANDING_POH, logic->FireTimer >= 16 || logic->Hearts >= 3),
                  LOCATION(RC_DMC_GS_CRATE,              (logic->FireTimer >= 8 || logic->Hearts >= 3) && logic->IsChild && logic->CanChildAttack),
                  LOCATION(RC_DMC_GOSSIP_STONE,          logic->HasExplosives && (logic->FireTimer >= 16 || logic->Hearts >= 3)),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_NEARBY,         {[]{return true;}}),
                  Entrance(RR_DMC_LADDER_AREA_NEARBY,   {[]{return logic->FireTimer >= 16 || logic->Hearts >= 3;}}),
                  Entrance(RR_DMC_CENTRAL_NEARBY,       {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_DISTANT_SCARECROW) && ((logic->EffectiveHealth > 2) || (logic->CanUse(RG_BOTTLE_WITH_FAIRY) && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || logic->CanUse(RG_NAYRUS_LOVE));}}),
                  Entrance(RR_DMC_LOWER_NEARBY,         {[]{return false;}}),
  });

  areaTable[RR_DMC_LADDER_AREA_NEARBY] = Area("DMC Ladder Area Nearby", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMC_DEKU_SCRUB, logic->IsChild && logic->CanStunDeku),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_NEARBY, {[]{return logic->Hearts >= 3;}}),
                  Entrance(RR_DMC_LOWER_NEARBY, {[]{return logic->Hearts >= 3 && (logic->CanUse(RG_HOVER_BOOTS) || (randoCtx->GetTrickOption(RT_DMC_BOULDER_JS) && logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER)) || (randoCtx->GetTrickOption(RT_DMC_BOULDER_SKIP) && logic->IsAdult));}}),
  });

  areaTable[RR_DMC_LOWER_NEARBY] = Area("DMC Lower Nearby", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL,          {[]{return logic->FireTimer >= 48;}}),
                  Entrance(RR_GC_DARUNIAS_CHAMBER,      {[]{return true;}}),
                  Entrance(RR_DMC_GREAT_FAIRY_FOUNTAIN, {[]{return logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_DMC_HAMMER_GROTTO,        {[]{return logic->IsAdult && logic->CanUse(RG_MEGATON_HAMMER);}}),
  });

  areaTable[RR_DMC_LOWER_LOCAL] = Area("DMC Lower Local", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_DMC_LOWER_NEARBY,       {[]{return true;}}),
                  Entrance(RR_DMC_LADDER_AREA_NEARBY, {[]{return logic->FireTimer >= 8 || logic->Hearts >= 3;}}),
                  Entrance(RR_DMC_CENTRAL_NEARBY,     {[]{return (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)) && (logic->FireTimer >= 8 || logic->Hearts >= 3);}}),
                  Entrance(RR_DMC_CENTRAL_LOCAL,      {[]{return (logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && logic->CanShield && randoCtx->GetTrickOption(RT_DMC_BOLERO_JUMP))) && logic->FireTimer >= 24;}}),
  });

  areaTable[RR_DMC_CENTRAL_NEARBY] = Area("DMC Central Nearby", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMC_VOLCANO_FREESTANDING_POH, logic->IsAdult && logic->Hearts >= 3 && (CanPlantBean(RR_DMC_CENTRAL_LOCAL) || (randoCtx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->HoverBoots))),
                  LOCATION(RC_SHEIK_IN_CRATER,              logic->IsAdult && (logic->FireTimer >= 8 || logic->Hearts >= 3)),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return logic->FireTimer >= 48;}}),
  });

  areaTable[RR_DMC_CENTRAL_LOCAL] = Area("DMC Central Local", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BeanPlantFairy, {[]{return logic->BeanPlantFairy || (CanPlantBean(RR_DMC_CENTRAL_LOCAL) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LOCATION(RC_DMC_GS_BEAN_PATCH, (logic->FireTimer >= 8 || logic->Hearts >= 3) && logic->CanPlantBugs && logic->CanChildAttack),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_NEARBY,   {[]{return true;}}),
                  Entrance(RR_DMC_LOWER_NEARBY,     {[]{return (logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_DMC_UPPER_NEARBY,     {[]{return logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL);}}),
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY, {[]{return (logic->IsChild && logic->Hearts >= 3 && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || (logic->IsAdult && logic->FireTimer >= 24);}}),
  });

  areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Area("DMC Great Fairy Fountain", "DMC Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_UPPER_GROTTO] = Area("DMC Upper Grotto", "DMC Upper Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LOCATION(RC_DMC_UPPER_GROTTO_CHEST,         true),
                  LOCATION(RC_DMC_UPPER_GROTTO_FISH,          logic->HasBottle),
                  LOCATION(RC_DMC_UPPER_GROTTO_GOSSIP_STONE,  true),
                  LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT,  logic->CanBreakLowerBeehives),
                  LOCATION(RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT, logic->CanBreakLowerBeehives),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_HAMMER_GROTTO] = Area("DMC Hammer Grotto", "DMC Hammer Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   logic->CanStunDeku),
                  LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  logic->CanStunDeku),
                  LOCATION(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, logic->CanStunDeku),
                  LOCATION(RC_DMC_HAMMER_GROTTO_BEEHIVE,    logic->CanBreakUpperBeehives),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });
}
