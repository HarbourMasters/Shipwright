#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_LostWoods() {
  areaTable[KOKIRI_FOREST] = Area("Kokiri Forest", "Kokiri Forest", KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy,           {[]{return BeanPlantFairy   || (CanPlantBean(KOKIRI_FOREST) && CanPlay(SongOfStorms));}}),
                  EventAccess(&GossipStoneFairy,         {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(KF_KOKIRI_SWORD_CHEST,   {[]{return IsChild;}}),
                  LocationAccess(KF_GS_KNOW_IT_ALL_HOUSE, {[]{return IsChild && CanChildAttack && AtNight && (HasNightStart || CanLeaveForest || CanPlay(SunsSong)) && CanGetNightTimeGS;}}),
                  LocationAccess(KF_GS_BEAN_PATCH,        {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(KF_GS_HOUSE_OF_TWINS,    {[]{return IsAdult && AtNight && HookshotOrBoomerang && CanGetNightTimeGS;},
                                               /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && (CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE));}}),
                  LocationAccess(KF_GOSSIP_STONE,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KF_LINKS_HOUSE,        {[]{return true;}}),
                  Entrance(KF_MIDOS_HOUSE,        {[]{return true;}}),
                  Entrance(KF_SARIAS_HOUSE,       {[]{return true;}}),
                  Entrance(KF_HOUSE_OF_TWINS,     {[]{return true;}}),
                  Entrance(KF_KNOW_IT_ALL_HOUSE,  {[]{return true;}}),
                  Entrance(KF_KOKIRI_SHOP,        {[]{return true;}}),
                  Entrance(KF_OUTSIDE_DEKU_TREE,  {[]{return IsAdult || OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield;},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(THE_LOST_WOODS,        {[]{return true;}}),
                  Entrance(LW_BRIDGE_FROM_FOREST, {[]{return IsAdult || OpenForest.IsNot(OPENFOREST_CLOSED) || DekuTreeClear;},
                                       /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || GlitchWWTEscape);}}),
                  Entrance(KF_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;},
                                       /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[KF_OUTSIDE_DEKU_TREE] = Area("KF Outside Deku Tree", "Kokiri Forest", KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,           {[]{return DekuBabaSticks || ((IsAdult && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF)) || KokiriSword || Boomerang);}}),
                  EventAccess(&DekuBabaNuts,             {[]{return DekuBabaNuts   || ((IsAdult && ShuffleDungeonEntrances.Is(SHUFFLEDUNGEONS_OFF)) || KokiriSword || Slingshot || Sticks || HasExplosives || CanUse(DINS_FIRE));}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(KF_DEKU_TREE_GOSSIP_STONE_LEFT,  {[]{return true;}}),
                  LocationAccess(KF_DEKU_TREE_GOSSIP_STONE_RIGHT, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(DEKU_TREE_ENTRYWAY, {[]{return IsChild || (ShuffleDungeonEntrances.IsNot(SHUFFLEDUNGEONS_OFF) && (OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield));},
                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::HammerSlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HoverBoost, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(KOKIRI_FOREST,      {[]{return IsAdult || OpenForest.Is(OPENFOREST_OPEN) || ShowedMidoSwordAndShield;},
                                    /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
  });

  areaTable[KF_LINKS_HOUSE] = Area("KF Link's House", "KF Link's House", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_LINKS_HOUSE_COW, {[]{return IsAdult && CanPlay(EponasSong) && LinksCow;},
                                          /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT))) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && Bombs && IsAdult && EponasSong && LinksCow;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[KF_MIDOS_HOUSE] = Area("KF Mido's House", "KF Mido's House", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_MIDOS_TOP_LEFT_CHEST,     {[]{return true;}}),
                  LocationAccess(KF_MIDOS_TOP_RIGHT_CHEST,    {[]{return true;}}),
                  LocationAccess(KF_MIDOS_BOTTOM_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(KF_MIDOS_BOTTOM_RIGHT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_SARIAS_HOUSE] = Area("KF Saria's House", "KF Saria's House", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_HOUSE_OF_TWINS] = Area("KF House of Twins", "KF House of Twins", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_KNOW_IT_ALL_HOUSE] = Area("KF Know It All House", "KF Know It All House", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_KOKIRI_SHOP] = Area("KF Kokiri Shop", "KF Kokiri Shop", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(KF_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(KF_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[KF_STORMS_GROTTO] = Area("KF Storms Grotto", "KF Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(KF_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(KF_STORMS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[LW_FOREST_EXIT] = Area("LW Forest Exit", "Lost Woods", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[THE_LOST_WOODS] = Area("Lost Woods", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&OddMushroomAccess, {[]{return OddMushroomAccess || (IsAdult && (CojiroAccess || Cojiro));}}),
                  EventAccess(&PoachersSawAccess, {[]{return PoachersSawAccess || (IsAdult && OddPoulticeAccess);}}),
                  EventAccess(&GossipStoneFairy,  {[]{return GossipStoneFairy  || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&BeanPlantFairy,    {[]{return BeanPlantFairy    || CanPlay(SongOfStorms);}}),
                  EventAccess(&BugShrub,          {[]{return IsChild && CanCutShrubs;},
                                       /*Glitched*/[]{return IsChild && Sticks && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                }, {
                  //Locations
                  LocationAccess(LW_SKULL_KID,                 {[]{return IsChild && CanPlay(SariasSong);},
                                                    /*Glitched*/[]{return IsChild && (Fish || Bugs) && SariasSong && CanShield && (CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) || (HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                  LocationAccess(LW_TRADE_COJIRO,              {[]{return IsAdult && Cojiro;}}),
                  LocationAccess(LW_TRADE_ODD_POTION,        {[]{return IsAdult && OddPoultice && Cojiro;}}),
                  LocationAccess(LW_OCARINA_MEMORY_GAME,       {[]{return IsChild && Ocarina;},
                                                    /*Glitched*/[]{return IsChild && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Fish || Bugs) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)));}}),
                  LocationAccess(LW_TARGET_IN_WOODS,           {[]{return IsChild && CanUse(SLINGSHOT);}}),
                  LocationAccess(LW_DEKU_SCRUB_NEAR_BRIDGE,    {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_GS_BEAN_PATCH_NEAR_BRIDGE, {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(LW_GOSSIP_STONE,              {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(LW_FOREST_EXIT,           {[]{return true;}}),
                  Entrance(GC_WOODS_WARP,            {[]{return true;}}),
                  Entrance(LW_BRIDGE,                {[]{return CanLeaveForest && ((IsAdult && CanPlantBean(THE_LOST_WOODS)) || CanUse(HOVER_BOOTS) || CanUse(LONGSHOT));},
                                          /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE) && HasBombchus) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(ZORAS_RIVER,              {[]{return CanLeaveForest && (CanDive || CanUse(IRON_BOOTS));},
                                          /*Glitched*/[]{return CanLeaveForest && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::ADVANCED) ||
                                                                ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                                ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))));}}),
                  Entrance(LW_BEYOND_MIDO,           {[]{return IsChild || CanPlay(SariasSong);},
                                          /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) ||
                                                                CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::ADVANCED) && (CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::INTERMEDIATE) || HoverBoots)) ||
                                                                ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SariasSong);}}),
                  Entrance(LW_NEAR_SHORTCUTS_GROTTO, {[]{return Here(THE_LOST_WOODS, []{return CanBlastOrSmash;});},
                                          /*Glitched*/[]{return Here(THE_LOST_WOODS, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
  });

  areaTable[LW_BEYOND_MIDO] = Area("LW Beyond Mido", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || CanUse(STICKS);}}),
                }, {
                  //Locations
                  LocationAccess(LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(LW_GS_ABOVE_THEATER,                   {[]{return IsAdult && AtNight && (CanPlantBean(LW_BEYOND_MIDO) || (LogicLostWoodsGSBean && CanUse(HOOKSHOT) && (CanUse(LONGSHOT) || CanUse(BOW) || CanUse(SLINGSHOT) || HasBombchus || CanUse(DINS_FIRE)))) && CanGetNightTimeGS;},
                                                             /*Glitched*/[]{return IsAdult && AtNight && CanGetNightTimeGS && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(LW_GS_BEAN_PATCH_NEAR_THEATER,         {[]{return CanPlantBugs && (CanChildAttack || (Scrubsanity.Is(SCRUBSANITY_OFF) && DekuShield));}}),
                }, {
                  //Exits
                  Entrance(LW_FOREST_EXIT,   {[]{return true;}}),
                  Entrance(THE_LOST_WOODS,   {[]{return IsChild || CanPlay(SariasSong);},
                                  /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::Megaflip, GlitchDifficulty::NOVICE) || ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                        ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SariasSong);}}),
                  Entrance(SFM_ENTRYWAY,     {[]{return true;}}),
                  Entrance(DEKU_THEATER,     {[]{return true;}}),
                  Entrance(LW_SCRUBS_GROTTO, {[]{return Here(LW_BEYOND_MIDO, []{return CanBlastOrSmash;});},
                                  /*Glitched*/[]{return Here(LW_BEYOND_MIDO, []{return IsChild && CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
  });

  areaTable[LW_NEAR_SHORTCUTS_GROTTO] = Area("LW Near Shortcuts Grotto", "LW Near Shortcuts Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(LW_NEAR_SHORTCUTS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[DEKU_THEATER] = Area("Deku Theater", "Deku Theater", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(DEKU_THEATER_SKULL_MASK,    {[]{return IsChild && SkullMask;}}),
                  LocationAccess(DEKU_THEATER_MASK_OF_TRUTH, {[]{return IsChild && MaskOfTruth;}}),
                }, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[LW_SCRUBS_GROTTO] = Area("LW Scrubs Grotto", "LW Scrubs Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LW_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(LW_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[SFM_ENTRYWAY] = Area("SFM Entryway", "Sacred Forest Meadow", SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LW_BEYOND_MIDO,       {[]{return true;}}),
                  Entrance(SACRED_FOREST_MEADOW, {[]{return IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                  Entrance(SFM_WOLFOS_GROTTO,    {[]{return CanOpenBombGrotto;}}),
  });

  areaTable[SACRED_FOREST_MEADOW] = Area("Sacred Forest Meadow", "Sacred Forest Meadow", SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(SONG_FROM_SARIA,             {[]{return IsChild && ZeldasLetter;}}),
                  LocationAccess(SHEIK_IN_FOREST,             {[]{return IsAdult;}}),
                  LocationAccess(SFM_GS,                      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(SFM_MAZE_GOSSIP_STONE_LOWER, {[]{return true;}}),
                  LocationAccess(SFM_MAZE_GOSSIP_STONE_UPPER, {[]{return true;}}),
                  LocationAccess(SFM_SARIA_GOSSIP_STONE,      {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(SFM_ENTRYWAY,           {[]{return true;}}),
                  Entrance(FOREST_TEMPLE_ENTRYWAY, {[]{return CanUse(HOOKSHOT);},
                                        /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(SFM_FAIRY_GROTTO,       {[]{return true;}}),
                  Entrance(SFM_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;},
                                        /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[SFM_FAIRY_GROTTO] = Area("SFM Fairy Grotto", "SFM Fairy Grotto", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[SFM_WOLFOS_GROTTO] = Area("SFM Wolfos Grotto", "SFM Wolfos Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SFM_WOLFOS_GROTTO_CHEST, {[]{return IsAdult || Slingshot || Sticks || KokiriSword || CanUse(DINS_FIRE) || CanUse(MEGATON_HAMMER) || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(SFM_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[SFM_STORMS_GROTTO] = Area("SFM Storms Grotto", "SFM Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(SFM_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(SFM_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[LW_BRIDGE_FROM_FOREST] = Area("LW Bridge From Forest", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LW_GIFT_FROM_SARIA, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(LW_BRIDGE, {[]{return true;}}),
  });

  areaTable[LW_BRIDGE] = Area("LW Bridge", "Lost Woods", THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(KOKIRI_FOREST,  {[]{return true;}}),
                  Entrance(HYRULE_FIELD,   {[]{return true;}}),
                  Entrance(THE_LOST_WOODS, {[]{return CanUse(LONGSHOT);},
                                /*Glitched*/[]{return (CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && Bombs && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE)) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::NOVICE);}}),
  });
}
