#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_Kakariko() {
  areaTable[RR_KAKARIKO_VILLAGE] = Area("Kakariko Village", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CojiroAccess,            {[]{return CojiroAccess || (IsAdult && WakeUpAdultTalon);}}),
                  EventAccess(&BugRock,                 {[]{return true;}}),
                  EventAccess(&KakarikoVillageGateOpen, {[]{return KakarikoVillageGateOpen || (IsChild && (ZeldasLetter || randoCtx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN)));}}),
                }, {
                  //Locations
                  LocationAccess(RC_SHEIK_IN_KAKARIKO,               {[]{return IsAdult && ForestMedallion && FireMedallion && WaterMedallion;}}),
                  LocationAccess(RC_KAK_ANJU_AS_CHILD,               {[]{return IsChild && AtDay;}}),
                  LocationAccess(RC_KAK_ANJU_AS_ADULT,               {[]{return IsAdult && AtDay;}}),
                  LocationAccess(RC_KAK_TRADE_POCKET_CUCCO,          {[]{return IsAdult && AtDay && PocketEgg && WakeUpAdultTalon;}}),
                  LocationAccess(RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION, {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_SKULLTULA_HOUSE,          {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_GUARDS_HOUSE,             {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_TREE,                     {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KAK_GS_WATCHTOWER,               {[]{return IsChild && (Slingshot || HasBombchus || CanUse(RG_FAIRY_BOW) || CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_KAK_TOWER_GS) && CanJumpslash)) && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,                {[]{return true;}}),
                  Entrance(RR_KAK_CARPENTER_BOSS_HOUSE,    {[]{return true;}}),
                  Entrance(RR_KAK_HOUSE_OF_SKULLTULA,      {[]{return true;}}),
                  Entrance(RR_KAK_IMPAS_HOUSE,             {[]{return true;}}),
                  Entrance(RR_KAK_WINDMILL,                {[]{return true;}}),
                  Entrance(RR_KAK_BAZAAR,                  {[]{return IsAdult && AtDay;}}),
                  Entrance(RR_KAK_SHOOTING_GALLERY,        {[]{return IsAdult && AtDay;}}),
                  Entrance(RR_BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return DrainWell && (IsChild || randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF));}}),
                  Entrance(RR_KAK_POTION_SHOP_FRONT,       {[]{return AtDay || IsChild;}}),
                  Entrance(RR_KAK_REDEAD_GROTTO,           {[]{return CanOpenBombGrotto;}}),
                  Entrance(RR_KAK_IMPAS_LEDGE,             {[]{return (IsChild && AtDay) || CanUse(RG_HOOKSHOT) || (IsAdult && randoCtx->GetTrickOption(RT_VISIBLE_COLLISION));}}),
                  Entrance(RR_KAK_ROOFTOP,                 {[]{return CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_KAK_MAN_ON_ROOF) && (IsAdult || AtDay || Slingshot || HasBombchus || CanUse(RG_FAIRY_BOW) || CanUse(RG_LONGSHOT)));}}),
                  Entrance(RR_KAK_IMPAS_ROOFTOP,           {[]{return CanUse(RG_HOOKSHOT) || (randoCtx->GetTrickOption(RT_KAK_ROOFTOP_GS) && CanUse(RG_HOVER_BOOTS));}}),
                  Entrance(RR_THE_GRAVEYARD,               {[]{return true;}}),
                  Entrance(RR_KAK_BEHIND_GATE,             {[]{return IsAdult || (KakarikoVillageGateOpen);}}),
  });

  areaTable[RR_KAK_IMPAS_LEDGE] = Area("Kak Impas Ledge", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_IMPAS_HOUSE_BACK, {[]{return true;}}),
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return true;}}),
  });

  areaTable[RR_KAK_IMPAS_ROOFTOP] = Area("Kak Impas Rooftop", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_GS_ABOVE_IMPAS_HOUSE, {[]{return IsAdult && AtNight && CanGetNightTimeGS && (CanJumpslash || CanUseProjectile);}}),
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
                  Entrance(RR_KAK_ODD_POTION_BUILDING, {[]{return IsAdult;}}),
                  Entrance(RR_KAK_POTION_SHOP_BACK,      {[]{return IsAdult && AtDay;}}),
  });

  areaTable[RR_KAK_CARPENTER_BOSS_HOUSE] = Area("Kak Carpenter Boss House", "Kak Carpenter Boss House", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WakeUpAdultTalon, {[]{return WakeUpAdultTalon || (IsAdult && PocketEgg);}}),
                }, {}, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_HOUSE_OF_SKULLTULA] = Area("Kak House of Skulltula", "Kak House of Skulltula", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_10_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 10;}}),
                  LocationAccess(RC_KAK_20_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 20;}}),
                  LocationAccess(RC_KAK_30_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 30;}}),
                  LocationAccess(RC_KAK_40_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 40;}}),
                  LocationAccess(RC_KAK_50_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 50;}}),
                  LocationAccess(RC_KAK_100_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 100;}})
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
                  LocationAccess(RC_KAK_IMPAS_HOUSE_COW, {[]{return CanUse(RG_EPONAS_SONG);}}),
  }, {});

  areaTable[RR_KAK_WINDMILL] = Area("Kak Windmill", "Windmill and Dampes Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DrainWell, {[]{return DrainWell || (IsChild && CanUse(RG_SONG_OF_STORMS));}}),
                }, {
                  //Locations
                  LocationAccess(RC_KAK_WINDMILL_FREESTANDING_POH, {[]{return CanUse(RG_BOOMERANG) || DampesWindmillAccess || (IsAdult && randoCtx->GetTrickOption(RT_KAK_ADULT_WINDMILL_POH)) || (IsChild && CanJumpslash && randoCtx->GetTrickOption(RT_KAK_CHILD_WINDMILL_POH));}}),
                    //PoH as child not added to trick options yet (needs uncommenting in randomizer_tricks.cpp)
                  LocationAccess(RC_SONG_FROM_WINDMILL,            {[]{return IsAdult && Ocarina;}}),
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
                  LocationAccess(RC_KAK_SHOOTING_GALLERY_REWARD, {[]{return IsAdult && Bow;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_POTION_SHOP_FRONT] = Area("Kak Potion Shop Front", "Kak Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_1, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_2, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_3, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_4, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_5, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_6, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_7, {[]{return IsAdult;}}),
                  LocationAccess(RC_KAK_POTION_SHOP_ITEM_8, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return true;}}),
                  Entrance(RR_KAK_POTION_SHOP_BACK, {[]{return IsAdult;}}),
  });

  areaTable[RR_KAK_POTION_SHOP_BACK] = Area("Kak Potion Shop Back", "Kak Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAK_BACKYARD,          {[]{return IsAdult;}}),
                  Entrance(RR_KAK_POTION_SHOP_FRONT, {[]{return true;}}),
  });

  areaTable[RR_KAK_ODD_POTION_BUILDING] =
      Area("Kak Granny's Potion Shop", "Kak Granny's Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {
          
               // Events
               EventAccess(&OddPoulticeAccess, { [] {
                   return OddPoulticeAccess || (IsAdult && (OddMushroomAccess || (OddMushroom && DisableTradeRevert)));
               } }),
           },
           {
               LocationAccess(RC_KAK_TRADE_ODD_MUSHROOM, { [] { return IsAdult && OddMushroom; } }),
               LocationAccess(RC_KAK_GRANNYS_SHOP, { [] { return IsAdult && OddMushroom && AdultsWallet; } }),
           },
           {
               // Exits
               Entrance(RR_KAK_BACKYARD, { [] { return true; } }),
           });

  areaTable[RR_KAK_REDEAD_GROTTO] = Area("Kak Redead Grotto", "Kak Redead Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_REDEAD_GROTTO_CHEST, {[]{return CanUse(RG_STICKS) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_DINS_FIRE) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[RR_KAK_OPEN_GROTTO] = Area("Kak Open Grotto", "Kak Open Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_KAK_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_KAK_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[RR_THE_GRAVEYARD] = Area("The Graveyard", "The Graveyard", RA_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || (CanUse(RG_STICKS) && AtDay);}}),
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(RR_THE_GRAVEYARD) && CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&BugRock,        {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_FREESTANDING_POH,        {[]{return (IsAdult && CanPlantBean(RR_THE_GRAVEYARD)) || CanUse(RG_LONGSHOT) || (randoCtx->GetTrickOption(RT_GY_POH) && CanUse(RG_BOOMERANG));}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, {[]{return IsChild && AtNight;}}), //TODO: This needs to change
                  LocationAccess(RC_GRAVEYARD_GS_WALL,                 {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_GRAVEYARD_GS_BEAN_PATCH,           {[]{return CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_GRAVEYARD_SHIELD_GRAVE,       {[]{return IsAdult || AtNight;}}),
                  Entrance(RR_GRAVEYARD_COMPOSERS_GRAVE,    {[]{return CanUse(RG_ZELDAS_LULLABY);}}),
                  Entrance(RR_GRAVEYARD_HEART_PIECE_GRAVE,  {[]{return IsAdult || AtNight;}}),
                  Entrance(RR_GRAVEYARD_DAMPES_GRAVE,       {[]{return IsAdult;}}),
                  Entrance(RR_GRAVEYARD_DAMPES_HOUSE,       {[]{return IsAdult || AtDampeTime;}}), //TODO: This needs to be handled
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
                  LocationAccess(RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, {[]{return CanUse(RG_SUNS_SONG);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_COMPOSERS_GRAVE] = Area("Graveyard Composers Grave", "Graveyard Composers Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, {[]{return HasFireSource;}}),
                  LocationAccess(RC_SONG_FROM_ROYAL_FAMILYS_TOMB,       {[]{return CanUseProjectile || CanJumpslash || CanUse(RG_MEGATON_HAMMER);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_DAMPES_GRAVE] = Area("Graveyard Dampes Grave", "Windmill and Dampes Grave", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,               {[]{return true;}}),
                  EventAccess(&DampesWindmillAccess, {[]{return DampesWindmillAccess || (IsAdult && CanUse(RG_SONG_OF_TIME));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_HOOKSHOT_CHEST,              {[]{return true;}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, {[]{return IsAdult || randoCtx->GetTrickOption(RT_GY_CHILD_DAMPE_RACE_POH);}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
                  Entrance(RR_KAK_WINDMILL,  {[]{return IsAdult && CanUse(RG_SONG_OF_TIME);}}),
  });

  areaTable[RR_GRAVEYARD_DAMPES_HOUSE] = Area("Graveyard Dampes House", "Graveyard Dampes House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DAMPE_HINT, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[RR_GRAVEYARD_WARP_PAD_REGION] = Area("Graveyard Warp Pad Region", "Graveyard", RA_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GY_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_GRAVEYARD,             {[]{return true;}}),
                  Entrance(RR_SHADOW_TEMPLE_ENTRYWAY,    {[]{return CanUse(RG_DINS_FIRE) || (randoCtx->GetTrickOption(RT_GY_SHADOW_FIRE_ARROWS) && IsAdult && CanUse(RG_FIRE_ARROWS));}}),
  });

  areaTable[RR_KAK_BEHIND_GATE] = Area("Kak Behind Gate", "Kakariko Village", RA_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KAKARIKO_VILLAGE,     {[]{return IsAdult || randoCtx->GetTrickOption(RT_VISIBLE_COLLISION) || KakarikoVillageGateOpen || randoCtx->GetOption(RSK_KAK_GATE).Is(RO_KAK_GATE_OPEN);}}),
                  Entrance(RR_DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });
}
