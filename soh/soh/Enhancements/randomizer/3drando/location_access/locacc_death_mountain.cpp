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
                  LocationAccess(RC_DMT_CHEST,                    {[]{return logic->CanBlastOrSmash || (randoCtx->GetTrickOption(RT_DMT_BOMBABLE) && logic->IsChild && logic->GoronBracelet);}}),
                  LocationAccess(RC_DMT_FREESTANDING_POH,         {[]{return logic->CanTakeDamage || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL) && (logic->HasExplosives || logic->GoronBracelet));}}),
                  LocationAccess(RC_DMT_GS_BEAN_PATCH,            {[]{return logic->CanPlantBugs && (logic->HasExplosives || logic->GoronBracelet || (randoCtx->GetTrickOption(RT_DMT_SOIL_GS) && (logic->CanTakeDamage || logic->CanUse(RG_HOVER_BOOTS)) && logic->CanUse(RG_BOOMERANG)));}}),
                  LocationAccess(RC_DMT_GS_NEAR_KAK,              {[]{return logic->CanBlastOrSmash;}}),
                  LocationAccess(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, {[]{return logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || (randoCtx->GetTrickOption(RT_DMT_HOOKSHOT_LOWER_GS) && logic->CanUse(RG_HOOKSHOT)) || (randoCtx->GetTrickOption(RT_DMT_BEAN_LOWER_GS) && CanPlantBean(RR_DEATH_MOUNTAIN_TRAIL)) || (randoCtx->GetTrickOption(RT_DMT_HOVERS_LOWER_GS) && logic->CanUse(RG_HOVER_BOOTS)) || randoCtx->GetTrickOption(RT_DMT_JS_LOWER_GS)) && logic->CanGetNightTimeGS;}}),
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
                  LocationAccess(RC_DMT_TRADE_BROKEN_SWORD,    {[]{return logic->IsAdult && logic->BrokenSword;}}),
                  LocationAccess(RC_DMT_TRADE_EYEDROPS,        {[]{return logic->IsAdult && logic->Eyedrops;}}),
                  LocationAccess(RC_DMT_TRADE_CLAIM_CHECK,     {[]{return logic->IsAdult && logic->ClaimCheck;}}),
                  LocationAccess(RC_DMT_GS_FALLING_ROCKS_PATH, {[]{return logic->IsAdult && logic->AtNight && (logic->CanUse(RG_MEGATON_HAMMER) || randoCtx->GetTrickOption(RT_DMT_UPPER_GS)) && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_DMT_GOSSIP_STONE,          {[]{return true;}}),
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
                  LocationAccess(RC_DMT_COW_GROTTO_COW, {[]{return logic->CanUse(RG_EPONAS_SONG);}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_SUMMIT, {[]{return true;}}),

  });

  areaTable[RR_DMT_STORMS_GROTTO] = Area("DMT Storms Grotto", "DMT Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_DMT_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_DMT_STORMS_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_DMT_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });

  areaTable[RR_DMT_GREAT_FAIRY_FOUNTAIN] = Area("DMT Great Fairy Fountain", "DMT Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMT_GREAT_FAIRY_REWARD, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
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
                  LocationAccess(RC_GC_MAZE_LEFT_CHEST,        {[]{return logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_SILVER_GAUNTLETS) || (randoCtx->GetTrickOption(RT_GC_LEFTMOST) && logic->HasExplosives && logic->CanUse(RG_HOVER_BOOTS));}}),
                  LocationAccess(RC_GC_MAZE_CENTER_CHEST,      {[]{return logic->CanBlastOrSmash  || logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_MAZE_RIGHT_CHEST,       {[]{return logic->CanBlastOrSmash  || logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_POT_FREESTANDING_POH,   {[]{return logic->IsChild && logic->GoronCityChildFire && (logic->Bombs || (logic->GoronBracelet && randoCtx->GetTrickOption(RT_GC_POT_STRENGTH)) || (logic->HasBombchus && randoCtx->GetTrickOption(RT_GC_POT)));}}),
                  LocationAccess(RC_GC_ROLLING_GORON_AS_CHILD, {[]{return logic->IsChild && (logic->HasExplosives || (logic->GoronBracelet && randoCtx->GetTrickOption(RT_GC_ROLLING_STRENGTH)));}}),
                  LocationAccess(RC_GC_ROLLING_GORON_AS_ADULT, {[]{return logic->StopGCRollingGoronAsAdult;}}),
                  LocationAccess(RC_GC_GS_BOULDER_MAZE,        {[]{return logic->IsChild && logic->CanBlastOrSmash;}}),
                  LocationAccess(RC_GC_GS_CENTER_PLATFORM,     {[]{return logic->CanAdultAttack;}}),
                  LocationAccess(RC_GC_MEDIGORON,              {[]{return logic->IsAdult && logic->AdultsWallet && (logic->CanBlastOrSmash || logic->GoronBracelet);}}),
                  LocationAccess(RC_GC_MAZE_GOSSIP_STONE,      {[]{return logic->CanBlastOrSmash || logic->CanUse(RG_SILVER_GAUNTLETS);}}),
                  LocationAccess(RC_GC_MEDIGORON_GOSSIP_STONE, {[]{return logic->CanBlastOrSmash || logic->GoronBracelet;}}),
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
                  LocationAccess(RC_GC_DARUNIAS_JOY, {[]{return logic->IsChild && logic->CanUse(RG_SARIAS_SONG);}}),
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
                  LocationAccess(RC_GC_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_GC_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_GORON_CITY, {[]{return true;}}),
  });

  areaTable[RR_GC_GROTTO] = Area("GC Grotto", "GC Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_GC_DEKU_SCRUB_GROTTO_CENTER, {[]{return logic->CanStunDeku;}}),
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
                  LocationAccess(RC_DMC_WALL_FREESTANDING_POH, {[]{return logic->FireTimer >= 16 || logic->Hearts >= 3;}}),
                  LocationAccess(RC_DMC_GS_CRATE,              {[]{return (logic->FireTimer >= 8 || logic->Hearts >= 3) && logic->IsChild && logic->CanChildAttack;}}),
                  LocationAccess(RC_DMC_GOSSIP_STONE,          {[]{return logic->HasExplosives && (logic->FireTimer >= 16 || logic->Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_NEARBY,         {[]{return true;}}),
                  Entrance(RR_DMC_LADDER_AREA_NEARBY,   {[]{return logic->FireTimer >= 16 || logic->Hearts >= 3;}}),
                  Entrance(RR_DMC_CENTRAL_NEARBY,       {[]{return logic->IsAdult && logic->CanUse(RG_GORON_TUNIC) && logic->CanUse(RG_DISTANT_SCARECROW) && ((logic->EffectiveHealth > 2) || (logic->Fairy && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || logic->CanUse(RG_NAYRUS_LOVE));}}),
                  Entrance(RR_DMC_LOWER_NEARBY,         {[]{return false;}}),
  });

  areaTable[RR_DMC_LADDER_AREA_NEARBY] = Area("DMC Ladder Area Nearby", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_DEKU_SCRUB, {[]{return logic->IsChild && logic->CanStunDeku;}}),
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
                  LocationAccess(RC_DMC_VOLCANO_FREESTANDING_POH, {[]{return logic->IsAdult && logic->Hearts >= 3 && (CanPlantBean(RR_DMC_CENTRAL_LOCAL) || (randoCtx->GetTrickOption(RT_DMC_HOVER_BEAN_POH) && logic->HoverBoots));}}),
                  LocationAccess(RC_SHEIK_IN_CRATER,              {[]{return logic->IsAdult && (logic->FireTimer >= 8 || logic->Hearts >= 3);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_LOCAL, {[]{return logic->FireTimer >= 48;}}),
  });

  areaTable[RR_DMC_CENTRAL_LOCAL] = Area("DMC Central Local", "Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BeanPlantFairy, {[]{return logic->BeanPlantFairy || (CanPlantBean(RR_DMC_CENTRAL_LOCAL) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LocationAccess(RC_DMC_GS_BEAN_PATCH, {[]{return (logic->FireTimer >= 8 || logic->Hearts >= 3) && logic->CanPlantBugs && logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_CENTRAL_NEARBY,   {[]{return true;}}),
                  Entrance(RR_DMC_LOWER_NEARBY,     {[]{return (logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL)) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_DMC_UPPER_NEARBY,     {[]{return logic->IsAdult && CanPlantBean(RR_DMC_CENTRAL_LOCAL);}}),
                  Entrance(RR_FIRE_TEMPLE_ENTRYWAY, {[]{return (logic->IsChild && logic->Hearts >= 3 && randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) || (logic->IsAdult && logic->FireTimer >= 24);}}),
  });

  areaTable[RR_DMC_GREAT_FAIRY_FOUNTAIN] = Area("DMC Great Fairy Fountain", "DMC Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_GREAT_FAIRY_REWARD, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_UPPER_GROTTO] = Area("DMC Upper Grotto", "DMC Upper Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_DMC_UPPER_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_DMC_UPPER_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_DMC_UPPER_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_UPPER_LOCAL, {[]{return true;}}),
  });

  areaTable[RR_DMC_HAMMER_GROTTO] = Area("DMC Hammer Grotto", "DMC Hammer Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_LEFT,   {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_DMC_DEKU_SCRUB_GROTTO_CENTER, {[]{return logic->CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_DMC_LOWER_LOCAL, {[]{return true;}}),
  });
}
