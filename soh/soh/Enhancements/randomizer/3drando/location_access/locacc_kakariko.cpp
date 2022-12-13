#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_Kakariko() {
  areaTable[KAKARIKO_VILLAGE] = Area("Kakariko Village", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&CojiroAccess,            {[]{return CojiroAccess || (IsAdult && WakeUpAdultTalon);}}),
                  EventAccess(&BugRock,                 {[]{return true;}}),
                  EventAccess(&KakarikoVillageGateOpen, {[]{return KakarikoVillageGateOpen || (IsChild && (ZeldasLetter || OpenKakariko.Is(OPENKAKARIKO_OPEN)));}}),
                }, {
                  //Locations
                  LocationAccess(SHEIK_IN_KAKARIKO,               {[]{return IsAdult && ForestMedallion && FireMedallion && WaterMedallion;}}),
                  LocationAccess(KAK_ANJU_AS_CHILD,               {[]{return IsChild && AtDay;}}),
                  LocationAccess(KAK_ANJU_AS_ADULT,               {[]{return IsAdult && AtDay;}}),
                  LocationAccess(KAK_TRADE_POCKET_CUCCO,          {[]{return IsAdult && AtDay && PocketEgg && WakeUpAdultTalon;}}),
                  LocationAccess(KAK_GS_HOUSE_UNDER_CONSTRUCTION, {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_SKULLTULA_HOUSE,          {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_GUARDS_HOUSE,             {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_TREE,                     {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(KAK_GS_WATCHTOWER,               {[]{return IsChild && (Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT)) && AtNight && CanGetNightTimeGS;},
                                                       /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && (CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::SuperStab, GlitchDifficulty::NOVICE) || (Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE)));}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,                {[]{return true;}}),
                  Entrance(KAK_CARPENTER_BOSS_HOUSE,    {[]{return true;}}),
                  Entrance(KAK_HOUSE_OF_SKULLTULA,      {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE,             {[]{return true;}}),
                  Entrance(KAK_WINDMILL,                {[]{return true;}}),
                  Entrance(KAK_BAZAAR,                  {[]{return IsAdult && AtDay;},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE);}}),
                  Entrance(KAK_SHOOTING_GALLERY,        {[]{return IsAdult && AtDay;}}),
                  Entrance(BOTTOM_OF_THE_WELL_ENTRYWAY, {[]{return DrainWell && (IsChild || ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF));},
                                             /*Glitched*/[]{return (IsChild && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || (AtNight && (CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                                   (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || WeirdEgg))) && ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) ||
                                                                   (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))))))) || (IsAdult && CanUse(LONGSHOT) && AtDay && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE));}}),
                  Entrance(KAK_POTION_SHOP_FRONT,       {[]{return AtDay || IsChild;}}),
                  Entrance(KAK_REDEAD_GROTTO,           {[]{return CanOpenBombGrotto;}}),
                  Entrance(KAK_IMPAS_LEDGE,             {[]{return (IsChild && AtDay) || CanUse(HOOKSHOT);},
                                             /*Glitched*/[]{return IsAdult && ((HoverBoots && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(KAK_ROOFTOP,                 {[]{return CanUse(HOOKSHOT) || (LogicManOnRoof && (IsAdult || AtDay || Slingshot || HasBombchus || CanUse(BOW) || CanUse(LONGSHOT)));},
                                             /*Glitched*/[]{return LogicManOnRoof && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE);}}),
                  Entrance(KAK_IMPAS_ROOFTOP,           {[]{return CanUse(HOOKSHOT);},
                                             /*Glitched*/[]{return (HoverBoots && CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE)) || (Bombs && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(THE_GRAVEYARD,               {[]{return true;}}),
                  Entrance(KAK_BEHIND_GATE,             {[]{return IsAdult || (KakarikoVillageGateOpen);},
                                             /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[KAK_IMPAS_LEDGE] = Area("Kak Impas Ledge", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_BACK, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_ROOFTOP] = Area("Kak Impas Rooftop", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_GS_ABOVE_IMPAS_HOUSE, {[]{return IsAdult && AtNight && CanGetNightTimeGS && (CanJumpslash || CanUseProjectile);}}),
                }, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE,  {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_ROOFTOP] = Area("Kak Rooftop", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_MAN_ON_ROOF, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[KAK_BACKYARD] = Area("Kak Backyard", "Kakariko Village", KAKARIKO_VILLAGE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,          {[]{return true;}}),
                  Entrance(KAK_OPEN_GROTTO,           {[]{return true;}}),
                  Entrance(KAK_ODD_POTION_BUILDING, {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_BACK,      {[]{return IsAdult && AtDay;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[KAK_CARPENTER_BOSS_HOUSE] = Area("Kak Carpenter Boss House", "Kak Carpenter Boss House", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&WakeUpAdultTalon, {[]{return WakeUpAdultTalon || (IsAdult && PocketEgg);}}),
                }, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_HOUSE_OF_SKULLTULA] = Area("Kak House of Skulltula", "Kak House of Skulltula", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_10_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 10;}}),
                  LocationAccess(KAK_20_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 20;}}),
                  LocationAccess(KAK_30_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 30;}}),
                  LocationAccess(KAK_40_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 40;}}),
                  LocationAccess(KAK_50_GOLD_SKULLTULA_REWARD, {[]{return GoldSkulltulaTokens >= 50;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE] = Area("Kak Impas House", "Kak Impas House", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,         {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_BACK] = Area("Kak Impas House Back", "Kak Impas House", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_IMPAS_HOUSE_FREESTANDING_POH, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_IMPAS_LEDGE,          {[]{return true;}}),
                  Entrance(KAK_IMPAS_HOUSE_NEAR_COW, {[]{return true;}}),
  });

  areaTable[KAK_IMPAS_HOUSE_NEAR_COW] = Area("Kak Impas House Near Cow", "Kak Impas House", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_IMPAS_HOUSE_COW, {[]{return CanPlay(EponasSong);},
                                           /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED))) && EponasSong;}}),
  }, {});

  areaTable[KAK_WINDMILL] = Area("Kak Windmill", "Windmill and Dampes Grave", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DrainWell, {[]{return DrainWell || (IsChild && CanPlay(SongOfStorms));},
                               /*Glitched*/[]{return IsChild && SongOfStorms && (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::ADVANCED) || ((Fish || Bugs) && CanShield && ((Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE)) &&
                                                     CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Fish || Bugs) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                }, {
                  //Locations
                  LocationAccess(KAK_WINDMILL_FREESTANDING_POH, {[]{return CanUse(BOOMERANG) || DampesWindmillAccess || (IsAdult && CanUse(HOOKSHOT) && LogicWindmillPoHHookshot);},
                                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(SONG_FROM_WINDMILL,            {[]{return IsAdult && Ocarina;},
                                                     /*Glitched*/[]{return IsAdult && (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::EXPERT) || ((Fish || Bugs) && CanShield && ((Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) || (DampesWindmillAccess || (IsAdult && CanUse(HOOKSHOT) && LogicWindmillPoHHookshot) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE))) &&
                                                                           CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Fish || Bugs) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_BAZAAR] = Area("Kak Bazaar", "Kak Bazaar", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_BAZAAR_ITEM_1, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_2, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_3, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_4, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_5, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_6, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_7, {[]{return true;}}),
                  LocationAccess(KAK_BAZAAR_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_SHOOTING_GALLERY] = Area("Kak Shooting Gallery", "Kak Shooting Gallery", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_SHOOTING_GALLERY_REWARD, {[]{return IsAdult && Bow;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_POTION_SHOP_FRONT] = Area("Kak Potion Shop Front", "Kak Potion Shop", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_POTION_SHOP_ITEM_1, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_2, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_3, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_4, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_5, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_6, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_7, {[]{return IsAdult;}}),
                  LocationAccess(KAK_POTION_SHOP_ITEM_8, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,     {[]{return true;}}),
                  Entrance(KAK_POTION_SHOP_BACK, {[]{return IsAdult;}}),
  });

  areaTable[KAK_POTION_SHOP_BACK] = Area("Kak Potion Shop Back", "Kak Potion Shop", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAK_BACKYARD,          {[]{return IsAdult;}}),
                  Entrance(KAK_POTION_SHOP_FRONT, {[]{return true;}}),
  });

  areaTable[KAK_ODD_POTION_BUILDING] = Area("Kak Granny's Potion Shop", "Kak Granny's Potion Shop", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&OddPoulticeAccess, {[]{return OddPoulticeAccess || (IsAdult && (OddMushroomAccess || (OddMushroom && DisableTradeRevert)));}}),
                }, {
                  LocationAccess(KAK_TRADE_ODD_MUSHROOM, {[]{return IsAdult && OddMushroom;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[KAK_REDEAD_GROTTO] = Area("Kak Redead Grotto", "Kak Redead Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KAK_REDEAD_GROTTO_CHEST, {[]{return IsAdult || (Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD));}}),
                }, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE, {[]{return true;}}),
  });

  areaTable[KAK_OPEN_GROTTO] = Area("Kak Open Grotto", "Kak Open Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(KAK_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(KAK_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KAK_BACKYARD, {[]{return true;}}),
  });

  areaTable[THE_GRAVEYARD] = Area("The Graveyard", "The Graveyard", THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || (CanUse(STICKS) && AtDay);}}),
                  EventAccess(&BeanPlantFairy, {[]{return BeanPlantFairy || (CanPlantBean(THE_GRAVEYARD) && CanPlay(SongOfStorms));}}),
                  EventAccess(&BugRock,        {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_FREESTANDING_POH,        {[]{return (IsAdult && CanPlantBean(THE_GRAVEYARD)) || CanUse(LONGSHOT) || (LogicGraveyardPoH && CanUse(BOOMERANG));},
                                                         /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, {[]{return IsChild && AtNight;}}), //TODO: This needs to change
                  LocationAccess(GRAVEYARD_GS_WALL,                 {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                                         /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(GRAVEYARD_GS_BEAN_PATCH,           {[]{return CanPlantBugs && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(GRAVEYARD_SHIELD_GRAVE,       {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_COMPOSERS_GRAVE,    {[]{return CanPlay(ZeldasLullaby);},
                                              /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanShield && (IsChild || AdultCanAccess(GRAVEYARD_WARP_PAD_REGION) || (IsAdult && (CanPlantBean(THE_GRAVEYARD) || CanUse(LONGSHOT))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || (Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2)))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                    ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                  Entrance(GRAVEYARD_HEART_PIECE_GRAVE,  {[]{return IsAdult || AtNight;}}),
                  Entrance(GRAVEYARD_DAMPES_GRAVE,       {[]{return IsAdult;}}),
                  Entrance(GRAVEYARD_DAMPES_HOUSE,       {[]{return IsAdult || AtDampeTime;}}), //TODO: This needs to be handled
                  Entrance(KAKARIKO_VILLAGE,             {[]{return true;}}),
                  Entrance(GRAVEYARD_WARP_PAD_REGION,    {[]{return false;},
                                              /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE);}}),
  });

  areaTable[GRAVEYARD_SHIELD_GRAVE] = Area("Graveyard Shield Grave", "Graveyard Shield Grave", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_SHIELD_GRAVE_CHEST, {[]{return true;}}),
                  //Free Fairies
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_HEART_PIECE_GRAVE] = Area("Graveyard Heart Piece Grave", "Graveyard Heart Piece Grave", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_HEART_PIECE_GRAVE_CHEST, {[]{return CanPlay(SunsSong);},
                                                         /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::NOVICE) || ((Bugs || Fish) && CanShield && (Bombs && (CanSurviveDamage || (Fairy && NumBottles >= 2))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SunsSong;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_COMPOSERS_GRAVE] = Area("Graveyard Composers Grave", "Graveyard Composers Grave", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(GRAVEYARD_COMPOSERS_GRAVE_CHEST, {[]{return HasFireSource;},
                                                       /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(SONG_FROM_COMPOSERS_GRAVE,       {[]{return IsAdult || (Slingshot || Boomerang || Sticks || HasExplosives || KokiriSword || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD) || CanUse(BOW) || CanUse(HOOKSHOT));}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_DAMPES_GRAVE] = Area("Graveyard Dampes Grave", "Windmill and Dampes Grave", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,               {[]{return true;}}),
                  EventAccess(&DampesWindmillAccess, {[]{return DampesWindmillAccess || (IsAdult && CanPlay(SongOfTime));},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfTime;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_HOOKSHOT_CHEST,              {[]{return true;}}),
                  LocationAccess(GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, {[]{return IsAdult || LogicChildDampeRacePoH;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
                  Entrance(KAK_WINDMILL,  {[]{return IsAdult && CanPlay(SongOfTime);},
                               /*Glitched*/[]{return (CanDoGlitch(GlitchType::WindmillBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                     ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsAdult && SongOfTime;}}),
  });

  areaTable[GRAVEYARD_DAMPES_HOUSE] = Area("Graveyard Dampes House", "Graveyard Dampes House", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_GRAVEYARD, {[]{return true;}}),
  });

  areaTable[GRAVEYARD_WARP_PAD_REGION] = Area("Graveyard Warp Pad Region", "Graveyard", THE_GRAVEYARD, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(GRAVEYARD_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_GRAVEYARD,             {[]{return true;}}),
                  Entrance(SHADOW_TEMPLE_ENTRYWAY,    {[]{return CanUse(DINS_FIRE) || (LogicShadowFireArrowEntry && IsAdult && CanUse(FIRE_ARROWS));},
                                           /*Glitched*/[]{return (CanUse(STICKS) && (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::INTERMEDIATE) || CanUse(FIRE_ARROWS))) || (CanTakeDamage && (
                                                                 CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE)));}}),
  });

  areaTable[KAK_BEHIND_GATE] = Area("Kak Behind Gate", "Kakariko Village", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KAKARIKO_VILLAGE,     {[]{return IsAdult || LogicVisibleCollision || KakarikoVillageGateOpen || OpenKakariko.Is(OPENKAKARIKO_OPEN);}}),
                  Entrance(DEATH_MOUNTAIN_TRAIL, {[]{return true;}}),
  });
}
