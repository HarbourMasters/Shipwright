#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_Kakariko() {
  areaTable[KAKARIKO_VILLAGE] = Area("Kakariko Village", "Kakariko Village", RHT_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CojiroAccess,            {[]{return CojiroAccess || (IsAdult && WakeUpAdultTalon);}}),
                  EventAccess(&BugRock,                 {[]{return true;}}),
                  EventAccess(&KakarikoVillageGateOpen, {[]{return KakarikoVillageGateOpen || (IsChild && (ZeldasLetter || OpenKakariko.Is(OPENKAKARIKO_OPEN)));}}),
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
                  LocationAccess(RC_KAK_GS_WATCHTOWER,               {[]{return IsChild && (Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT) || (LogicKakarikoTowerGS && CanJumpslash)) && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,                {[]{return true;}}),
                  Entrance(KAK_CARPENTER_BOSS_HOUSE,    {[]{return true;}}),
                  Entrance(KAK_HOUSE_OF_SKULLTULA,      {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE,             {[]{return true;}}),
                  Entrance(KAK_WINDMILL,                {[]{return true;}}),
                  Entrance(KAK_BAZAAR,                  {[]{return IsAdult && AtDay;}}),
                  Entrance(KAK_SHOOTING_GALLERY,        {[]{return IsAdult && AtDay;}}),
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return DrainWell && (IsChild || ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF));}}),
                  Entrance(KAK_POTION_SHOP_FRONT,       {[]{return AtDay || IsChild;}}),
                  Entrance(KAK_REDEAD_GROTTO,           {[]{return CanOpenBombGrotto;}}),
                  Entrance(KAK_IMPAS_LEDGE,             {[]{return (IsChild && AtDay) || CanUse(HOOKSHOT) || (IsAdult && LogicVisibleCollision);}}),
                  Entrance(KAK_ROOFTOP,                 {[]{return CanUse(HOOKSHOT) || (LogicManOnRoof && (IsAdult || AtDay || Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT)));}}),
                  Entrance(KAK_IMPAS_ROOFTOP,           {[]{return CanUse(HOOKSHOT) || (LogicKakarikoRooftopGS && CanUse(HOVER_BOOTS));}}),
                  Entrance(THE_GRAVEYARD,               {[]{return true;}}),
                  Entrance(KAK_BEHIND_GATE,             {[]{return IsAdult || (KakarikoVillageGateOpen);}}),
  });

  areaTable[KAK_IMPAS_LEDGE] = Area("Kak Impas Ledge", "Kakariko Village", RHT_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_BACK, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_ROOFTOP] = Area("Kak Impas Rooftop", "Kakariko Village", RHT_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_GS_ABOVE_IMPAS_HOUSE, {[]{return IsAdult && AtNight && CanGetNightTimeGS && (CanJumpslash || CanUseProjectile);}}),
                }, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE,  {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_ROOFTOP] = Area("Kak Rooftop", "Kakariko Village", RHT_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_MAN_ON_ROOF, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[KAK_BACKYARD] = Area("Kak Backyard", "Kakariko Village", RHT_KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,          {[]{return true;}}),
                  Entrance(KAK_OPEN_GROTTO,           {[]{return true;}}),
                  Entrance(KAK_ODD_POTION_BUILDING, {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_BACK,      {[]{return IsAdult && AtDay;}}),
  });

  areaTable[KAK_CARPENTER_BOSS_HOUSE] = Area("Kak Carpenter Boss House", "Kak Carpenter Boss House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WakeUpAdultTalon, {[]{return WakeUpAdultTalon || (IsAdult && PocketEgg);}}),
                }, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_HOUSE_OF_SKULLTULA] = Area("Kak House of Skulltula", "Kak House of Skulltula", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_10_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 10;}}),
                  LocationAccess(RC_KAK_20_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 20;}}),
                  LocationAccess(RC_KAK_30_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 30;}}),
                  LocationAccess(RC_KAK_40_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 40;}}),
                  LocationAccess(RC_KAK_50_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 50;}}),
                  LocationAccess(RC_KAK_100_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 100;}})
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE] = Area("Kak Impas House", "Kak Impas House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,         {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_BACK] = Area("Kak Impas House Back", "Kak Impas House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE,          {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_NEAR_COW] = Area("Kak Impas House Near Cow", "Kak Impas House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_IMPAS_HOUSE_COW, {[]{return CanPlay(EponasSong);}}),
  }, {});

  areaTable[KAK_WINDMILL] = Area("Kak Windmill", "Windmill and Dampes Grave", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DrainWell, {[]{return DrainWell || (IsChild && CanPlay(SongOfStorms));}}),
                }, {
                  //Locations
                  LocationAccess(RC_KAK_WINDMILL_FREESTANDING_POH, {[]{return CanUse(BOOMERANG) || DampesWindmillAccess || (IsAdult && LogicAdultWindmillPoH) || (IsChild && CanJumpslash && LogicChildWindmillPoH);}}),
                    //PoH as child not added to trick options yet (needs uncommenting in randomizer_tricks.cpp)
                  LocationAccess(RC_SONG_FROM_WINDMILL,            {[]{return IsAdult && Ocarina;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_BAZAAR] = Area("Kak Bazaar", "Kak Bazaar", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_SHOOTING_GALLERY] = Area("Kak Shooting Gallery", "Kak Shooting Gallery", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_SHOOTING_GALLERY_REWARD, {[]{return IsAdult && Bow;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_POTION_SHOP_FRONT] = Area("Kak Potion Shop Front", "Kak Potion Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
                  Entrance(KAK_POTION_SHOP_BACK, {[]{return IsAdult;}}),
  });

  areaTable[KAK_POTION_SHOP_BACK] = Area("Kak Potion Shop Back", "Kak Potion Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_BACKYARD,          {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_FRONT, {[]{return true;}}),
  });

  areaTable[KAK_ODD_POTION_BUILDING] =
      Area("Kak Granny's Potion Shop", "Kak Granny's Potion Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
          
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
               Entrance(KAK_BACKYARD, { [] { return true; } }),
           });

  areaTable[KAK_REDEAD_GROTTO] = Area("Kak Redead Grotto", "Kak Redead Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KAK_REDEAD_GROTTO_CHEST, {[]{return IsAdult || (Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_OPEN_GROTTO] = Area("Kak Open Grotto", "Kak Open Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_KAK_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_KAK_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[THE_GRAVEYARD] = Area("The Graveyard", "The Graveyard", RHT_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || (CanUse(STICKS) && AtDay);}}),
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(THE_GRAVEYARD) && CanPlay(SongOfStorms));}}),
                  EventAccess(&BugRock,        {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_FREESTANDING_POH,        {[]{return (IsAdult && CanPlantBean(THE_GRAVEYARD)) || CanUse(LONGSHOT) || (LogicGraveyardPoH && CanUse(BOOMERANG));}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, {[]{return IsChild && AtNight;}}), //TODO: This needs to change
                  LocationAccess(RC_GRAVEYARD_GS_WALL,                 {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_GRAVEYARD_GS_BEAN_PATCH,           {[]{return CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(GRAVEYARD_SHIELD_GRAVE,       {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_COMPOSERS_GRAVE,    {[]{return CanPlay(ZeldasLullaby);}}),
                  Entrance(GRAVEYARD_HEART_PIECE_GRAVE,  {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_DAMPES_GRAVE,       {[]{return IsAdult;}}),
                  Entrance(GRAVEYARD_DAMPES_HOUSE,       {[]{return IsAdult || AtDampeTime;}}), //TODO: This needs to be handled
                  Entrance(KAKARIKO_VILLAGE,             {[]{return true;}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION,    {[]{return false;}}),
  });

  areaTable[GRAVEYARD_SHIELD_GRAVE] = Area("Graveyard Shield Grave", "Graveyard Shield Grave", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_SHIELD_GRAVE_CHEST, {[]{return true;}}),
                  //Free Fairies
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_HEART_PIECE_GRAVE] = Area("Graveyard Heart Piece Grave", "Graveyard Heart Piece Grave", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, {[]{return CanPlay(SunsSong);}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_COMPOSERS_GRAVE] = Area("Graveyard Composers Grave", "Graveyard Composers Grave", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, {[]{return HasFireSource;}}),
                  LocationAccess(RC_SONG_FROM_ROYAL_FAMILYS_TOMB,       {[]{return IsAdult || (Slingshot || Boomerang || Sticks || HasExplosives || KokiriSword || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(BOW) || CanUse(HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_DAMPES_GRAVE] = Area("Graveyard Dampes Grave", "Windmill and Dampes Grave", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,               {[]{return true;}}),
                  EventAccess(&DampesWindmillAccess, {[]{return DampesWindmillAccess || (IsAdult && CanPlay(SongOfTime));}}),
                }, {
                  //Locations
                  LocationAccess(RC_GRAVEYARD_HOOKSHOT_CHEST,              {[]{return true;}}),
                  LocationAccess(RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, {[]{return IsAdult || LogicChildDampeRacePoH;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
                  Entrance(KAK_WINDMILL,  {[]{return IsAdult && CanPlay(SongOfTime);}}),
  });

  areaTable[GRAVEYARD_DAMPES_HOUSE] = Area("Graveyard Dampes House", "Graveyard Dampes House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_WARP_PAD_REGION] = Area("Graveyard Warp Pad Region", "Graveyard", RHT_THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_GY_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD,             {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,    {[]{return CanUse(DINS_FIRE) || (LogicShadowFireArrowEntry && IsAdult && CanUse(FIRE_ARROWS));}}),
  });

  areaTable[KAK_BEHIND_GATE] = Area("Kak Behind Gate", "Kakariko Village", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,     {[]{return IsAdult || LogicVisibleCollision || KakarikoVillageGateOpen || OpenKakariko.Is(OPENKAKARIKO_OPEN);}}),
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });
}
