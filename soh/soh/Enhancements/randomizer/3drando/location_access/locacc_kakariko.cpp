#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_Kakariko() {
  areaTable[RR_KAKARIKO_VILLAGE] = Area("Kakariko Village", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CojiroAccess,            {[]{return logic->CojiroAccess || (logic->IsAdult && logic->WakeUpAdultTalon);}}),
                  EventAccess(&logic->BugRock,                 {[]{return true;}}),
                  EventAccess(&logic->KakarikoVillageGateOpen, {[]{return logic->KakarikoVillageGateOpen || (logic->IsChild && (logic->ZeldasLetter || randoCtx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN)));}}),
                }, {
                  //Locations
                  LocationAccess(RC_SHEIK_IN_KAKARIKO,               {[]{return logic->IsAdult && logic->ForestMedallion && logic->FireMedallion && logic->WaterMedallion;}}),
                  LocationAccess(RC_KAK_ANJU_AS_CHILD,               {[]{return logic->IsChild && logic->AtDay;}}),
                  LocationAccess(RC_KAK_ANJU_AS_ADULT,               {[]{return logic->IsAdult && logic->AtDay;}}),
                  LocationAccess(RC_KAK_TRADE_POCKET_CUCCO,          {[]{return logic->IsAdult && logic->AtDay && logic->PocketEgg && logic->WakeUpAdultTalon;}}),
                  LocationAccess(RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION, {[]{return logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_SKULLTULA_HOUSE,          {[]{return logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_GUARDS_HOUSE,             {[]{return logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_TREE,                     {[]{return logic->IsChild && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_WATCHTOWER,               {[]{return logic->IsChild && (logic->Slingshot || logic->HasBombchus || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_KAK_TOWER_GS) && logic->CanJumpslash)) && logic->AtNight && logic->CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,                {[]{return true;}}),
                  Entrance(RR_KAK_CARPENTER_BOSS_HOUSE,    {[]{return true;}}),
                  Entrance(RR_KAK_HOUSE_OF_SKULLTULA,      {[]{return true;}}),
                  Entrance(RR_KAK_IMPAS_HOUSE,             {[]{return true;}}),
                  Entrance(RR_KAK_WINDMILL,                {[]{return true;}}),
                  Entrance(RR_KAK_BAZAAR,                  {[]{return logic->IsAdult && logic->AtDay;}}),
                  Entrance(RR_KAK_SHOOTING_GALLERY,        {[]{return logic->IsAdult && logic->AtDay;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return logic->DrainWell && (logic->IsChild || randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF));}}),
                  Entrance(RR_KAK_POTION_SHOP_FRONT,       {[]{return logic->AtDay || logic->IsChild;}}),
                  Entrance(RR_KAK_REDEAD_GROTTO,           {[]{return logic->CanOpenBombGrotto;}}),
                  Entrance(RR_KAK_IMPAS_LEDGE,             {[]{return (logic->IsChild && logic->AtDay) || logic->CanUse(RG_HOOKSHOT) || (logic->IsAdult && randoCtx->GetTrickOption(RT_VISIBLE_COLLISION));}}),
                  Entrance(RR_KAK_ROOFTOP,                 {[]{return logic->CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_KAK_MAN_ON_ROOF) && (logic->IsAdult || logic->AtDay || logic->Slingshot || logic->HasBombchus || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_LONGSHOT)));}}),
                  Entrance(RR_KAK_IMPAS_ROOFTOP,           {[]{return logic->CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_KAK_ROOFTOP_GS) && logic->CanUse(RG_HOVER_BOOTS));}}),
                  Entrance(RR_THE_GRAVEYARD,               {[]{return true;}}),
                  Entrance(RR_KAK_BEHIND_GATE,             {[]{return logic->IsAdult || (logic->KakarikoVillageGateOpen);}}),
  });

  areaTable[RR_KAK_IMPAS_LEDGE] = Area("Kak Impas Ledge", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_HOUSE_BACK, {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return true;}}),
  });

  areaTable[RR_KAK_IMPAS_ROOFTOP] = Area("Kak Impas Rooftop", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_GS_ABOVE_IMPAS_HOUSE, {[]{return logic->IsAdult && logic->AtNight && logic->CanGetNightTimeGS && (logic->CanJumpslash || logic->CanUseProjectile);}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_LEDGE,  {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_ROOFTOP] = Area("Kak Rooftop", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_MAN_ON_ROOF, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[RR_KAK_BACKYARD] = Area("Kak Backyard", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE,          {[]{return true;}}),
                  Entrance(RR_KAK_OPEN_GROTTO,           {[]{return true;}}),
                  Entrance(RR_KAK_ODD_POTION_BUILDING, {[]{return logic->IsAdult;}}),
                  Entrance(RR_KAK_POTION_SHOP_BACK,      {[]{return logic->IsAdult && logic->AtDay;}}),
  });

  areaTable[RR_KAK_CARPENTER_BOSS_HOUSE] = Area("Kak Carpenter Boss House", "Kak Carpenter Boss House", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->WakeUpAdultTalon, {[]{return logic->WakeUpAdultTalon || (logic->IsAdult && logic->PocketEgg);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_HOUSE_OF_SKULLTULA] = Area("Kak House of Skulltula", "Kak House of Skulltula", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_10_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 10;}}),
                  LocationAccess(RC_KAK_20_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 20;}}),
                  LocationAccess(RC_KAK_30_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 30;}}),
                  LocationAccess(RC_KAK_40_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 40;}}),
                  LocationAccess(RC_KAK_50_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 50;}}),
                  LocationAccess(RC_KAK_100_GOLD_SKULLTULA_REWARD, {[]{return logic->GoldSkulltulaTokens >= 100;}})
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_IMPAS_HOUSE] = Area("Kak Impas House", "Kak Impas House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,         {[]{return true;}}),
  });

  areaTable[RR_KAK_IMPAS_HOUSE_BACK] = Area("Kak Impas House Back", "Kak Impas House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_LEDGE,          {[]{return true;}}),
                  Entrance(RR_KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
  });

  areaTable[RR_KAK_IMPAS_HOUSE_NEAR_COW] = Area("Kak Impas House Near Cow", "Kak Impas House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_IMPAS_HOUSE_COW, {[]{return logic->CanUse(RG_EPONAS_SONG);}}),
  }, {});

  areaTable[RR_KAK_WINDMILL] = Area("Kak Windmill", "Windmill and Dampes Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DrainWell, {[]{return logic->DrainWell || (logic->IsChild && logic->CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LocationAccess(RC_KAK_WINDMILL_FREESTANDING_POH, {[]{return logic->CanUse(RG_BOOMERANG) || logic->DampesWindmillAccess || (logic->IsAdult && randoCtx->GetTrickOption(RT_KAK_ADULT_WINDMILL_POH)) || (logic->IsChild && logic->CanJumpslash && randoCtx->GetTrickOption(RT_KAK_CHILD_WINDMILL_POH));}}),
                    //PoH as child not added to trick options yet (needs uncommenting in randomizer_tricks.cpp)
                  LocationAccess(RC_SONG_FROM_WINDMILL,            {[]{return logic->IsAdult && logic->Ocarina;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_BAZAAR] = Area("Kak Bazaar", "Kak Bazaar", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_BAZAAR_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_KAK_BAZAAR_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_SHOOTING_GALLERY] = Area("Kak Shooting Gallery", "Kak Shooting Gallery", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_SHOOTING_GALLERY_REWARD, {[]{return logic->IsAdult && logic->Bow;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_POTION_SHOP_FRONT] = Area("Kak Potion Shop Front", "Kak Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_1, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_2, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_3, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_4, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_5, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_6, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_7, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_8, {[]{return logic->IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return true;}}),
                  Entrance(RR_KAK_POTION_SHOP_BACK, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_KAK_POTION_SHOP_BACK] = Area("Kak Potion Shop Back", "Kak Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_BACKYARD,          {[]{return logic->IsAdult;}}),
                  Entrance(RR_KAK_POTION_SHOP_FRONT, {[]{return true;}}),
  });

  areaTable[RR_KAK_ODD_POTION_BUILDING] =
      Area("Kak Granny's Potion Shop", "Kak Granny's Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {
          
               // Events
               EventAccess(&logic->OddPoulticeAccess, { [] {
                   return logic->OddPoulticeAccess || (logic->IsAdult && (logic->OddMushroomAccess || (logic->OddMushroom && logic->DisableTradeRevert)));
               } }),
           },
           {
               LocationAccess(RC_KAK_TRADE_ODD_MUSHROOM, { [] { return logic->IsAdult && logic->OddMushroom; } }),
               LocationAccess(RC_KAK_GRANNYS_SHOP, { [] { return logic->IsAdult && logic->OddMushroom && logic->AdultsWallet; } }),
           },
           {
               // Exits
               Entrance(RR_KAK_BACKYARD, { [] { return true; } }),
           });

  areaTable[RR_KAK_REDEAD_GROTTO] = Area("Kak Redead Grotto", "Kak Redead Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_REDEAD_GROTTO_CHEST, {[]{return logic->CanUse(RG_STICKS) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_OPEN_GROTTO] = Area("Kak Open Grotto", "Kak Open Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_KAK_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_KAK_OPEN_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_KAK_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[RR_THE_GRAVEYARD] = Area("The Graveyard", "The Graveyard", RA_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ButterflyFairy, {[]{return logic->ButterflyFairy || (logic->CanUse(RG_STICKS) && logic->AtDay);}}),
                  EventAccess(&logic->BeanPlantFairy, {[]{return logic->BeanPlantFairy || (CanPlantBean(RR_THE_GRAVEYARD) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->BugRock,        {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_FREESTANDING_POH,        {[]{return (logic->IsAdult && CanPlantBean(RR_THE_GRAVEYARD)) || logic->CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_GY_POH) && logic->CanUse(RG_BOOMERANG));}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, {[]{return logic->IsChild && logic->AtNight;}}), //TODO: This needs to change
                  LocationAccess(RC_GRAVEYARD_GS_WALL,                 {[]{return logic->IsChild && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_GRAVEYARD_GS_BEAN_PATCH,           {[]{return logic->CanPlantBugs && logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_GRAVEYARD_SHIELD_GRAVE,       {[]{return logic->IsAdult || logic->AtNight;}}),
                  Entrance(RR_GRAVEYARD_COMPOSERS_GRAVE,    {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  Entrance(RR_GRAVEYARD_HEART_PIECE_GRAVE,  {[]{return logic->IsAdult || logic->AtNight;}}),
                  Entrance(RR_GRAVEYARD_DAMPES_GRAVE,       {[]{return logic->IsAdult;}}),
                  Entrance(RR_GRAVEYARD_DAMPES_HOUSE,       {[]{return logic->IsAdult || logic->AtDampeTime;}}), //TODO: This needs to be handled
                  Entrance(RR_KAKARIKO_VILLAGE,             {[]{return true;}}),
                  Entrance(RR_GRAVEYARD_WARP_PAD_REGION,    {[]{return false;}}),
  });

  areaTable[RR_GRAVEYARD_SHIELD_GRAVE] = Area("Graveyard Shield Grave", "Graveyard Shield Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_SHIELD_GRAVE_CHEST, {[]{return true;}}),
                  //Free Fairies
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_HEART_PIECE_GRAVE] = Area("Graveyard Heart Piece Grave", "Graveyard Heart Piece Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, {[]{return logic->CanUse(RG_SUNS_SONG);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_COMPOSERS_GRAVE] = Area("Graveyard Composers Grave", "Graveyard Composers Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, {[]{return logic->HasFireSource;}}),
                  LocationAccess(RC_SONG_FROM_ROYAL_FAMILYS_TOMB,       {[]{return logic->CanUseProjectile || logic->CanJumpslash || logic->CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_DAMPES_GRAVE] = Area("Graveyard Dampes Grave", "Windmill and Dampes Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot,               {[]{return true;}}),
                  EventAccess(&logic->DampesWindmillAccess, {[]{return logic->DampesWindmillAccess || (logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_HOOKSHOT_CHEST,              {[]{return true;}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, {[]{return logic->IsAdult || randoCtx->GetTrickOption(RT_GY_CHILD_DAMPE_RACE_POH);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
                  Entrance(RR_KAK_WINDMILL,  {[]{return logic->IsAdult && logic->CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_GRAVEYARD_DAMPES_HOUSE] = Area("Graveyard Dampes House", "Graveyard Dampes House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DAMPE_HINT, {[]{return logic->IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_WARP_PAD_REGION] = Area("Graveyard Warp Pad Region", "Graveyard", RA_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GY_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD,             {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,    {[]{return logic->CanUse(RG_DINS_FIRE) || (randoCtx->GetTrickOption(RT_GY_SHADOW_FIRE_ARROWS) && logic->IsAdult && logic->CanUse(RG_FIRE_ARROWS));}}),
  });

  areaTable[RR_KAK_BEHIND_GATE] = Area("Kak Behind Gate", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return logic->IsAdult || randoCtx->GetTrickOption(RT_VISIBLE_COLLISION) || logic->KakarikoVillageGateOpen || randoCtx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN);}}),
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });
}
