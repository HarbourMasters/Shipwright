#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_HyruleField() {
  areaTable[HYRULE_FIELD] = Area("Hyrule Field", "Hyrule Field", HYRULE_FIELD, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BigPoeKill, {[]{return CanUse(BOW) && CanRideEpona && HasBottle;}}),
                }, {
                  //Locations
                  LocationAccess(HF_OCARINA_OF_TIME_ITEM,   {[]{return IsChild && HasAllStones;}}),
                  LocationAccess(SONG_FROM_OCARINA_OF_TIME, {[]{return IsChild && HasAllStones;}}),
                }, {
                  //Exits
                  Entrance(LW_BRIDGE,              {[]{return true;}}),
                  Entrance(LAKE_HYLIA,             {[]{return true;}}),
                  Entrance(GERUDO_VALLEY,          {[]{return true;}}),
                  Entrance(MARKET_ENTRANCE,        {[]{return true;}}),
                  Entrance(KAKARIKO_VILLAGE,       {[]{return true;}}),
                  Entrance(ZR_FRONT,               {[]{return true;}}),
                  Entrance(LON_LON_RANCH,          {[]{return true;}}),
                  Entrance(HF_SOUTHEAST_GROTTO,    {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(HF_OPEN_GROTTO,         {[]{return true;}}),
                  Entrance(HF_INSIDE_FENCE_GROTTO, {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HF_COW_GROTTO,          {[]{return (CanUse(MEGATON_HAMMER) || IsChild) && CanOpenBombGrotto;},
                                        /*Glitched*/[]{return (CanUse(STICKS) && (ShardOfAgony || LogicGrottosWithoutAgony) && (IsChild || (CanOpenBombGrotto && (CanTakeDamageTwice || CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)))) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE));}}),
                  Entrance(HF_NEAR_MARKET_GROTTO,  {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(HF_FAIRY_GROTTO,        {[]{return Here(HYRULE_FIELD, []{return CanBlastOrSmash;});},
                                        /*Glitched*/[]{return Here(HYRULE_FIELD, []{return CanUse(STICKS) && ((IsChild && CanTakeDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::EXPERT)) || (IsAdult && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)));});}}),
                  Entrance(HF_NEAR_KAK_GROTTO,     {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HF_TEKTITE_GROTTO,      {[]{return CanOpenBombGrotto;},
                                        /*Glitched*/[]{return Sticks && IsChild && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
  });

  areaTable[HF_SOUTHEAST_GROTTO] = Area("HF Southeast Grotto", "HF Southeast Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_SOUTHEAST_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_SOUTHEAST_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_OPEN_GROTTO] = Area("HF Open Grotto", "HF Open Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_INSIDE_FENCE_GROTTO] = Area("HF Inside Fence Grotto", "HF Inside Fence Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_DEKU_SCRUB_GROTTO, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_COW_GROTTO] = Area("HF Cow Grotto", "HF Cow Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_GS_COW_GROTTO,           {[]{return HasFireSource && HookshotOrBoomerang;},
                                                  /*Glitched*/[]{return (CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) && HookshotOrBoomerang;}}),
                  LocationAccess(HF_COW_GROTTO_COW,          {[]{return HasFireSource && CanPlay(EponasSong);},
                                                  /*Glitched*/[]{return ((CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE)) || HasFireSource) &&
                                                                        (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && Bombs && (CanSurviveDamage || (NumBottles >= 2 && Fairy)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && HasBombchus && CanShield && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED)) || Ocarina) && EponasSong;}}),
                  LocationAccess(HF_COW_GROTTO_GOSSIP_STONE, {[]{return HasFireSource;},
                                                  /*Glitched*/[]{return CanUse(STICKS) && Bombs && CanSurviveDamage && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::NOVICE);}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_NEAR_MARKET_GROTTO] = Area("HF Near Market Grotto", "HF Near Market Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(HF_NEAR_MARKET_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(HF_NEAR_MARKET_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_FAIRY_GROTTO] = Area("HF Fairy Grotto", "HF Fairy Grotto", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_NEAR_KAK_GROTTO] = Area("HF Near Kak Grotto", "HF Near Kak Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_GS_NEAR_KAK_GROTTO, {[]{return HookshotOrBoomerang;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[HF_TEKTITE_GROTTO] = Area("HF Tektite Grotto", "HF Tektite Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(HF_TEKTITE_GROTTO_FREESTANDING_POH, {[]{return ProgressiveScale >= 2 || CanUse(IRON_BOOTS);}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[LAKE_HYLIA] = Area("Lake Hylia", "Lake Hylia", LAKE_HYLIA, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(LAKE_HYLIA) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || (IsChild && CanCutShrubs);}}),
                  EventAccess(&ChildScarecrow,   {[]{return ChildScarecrow   || (IsChild && Ocarina);},
                                      /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && (KokiriSword || Sticks || Bombs || HasBombchus || Boomerang || Slingshot || CanUse(MEGATON_HAMMER)) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE))) && IsChild;}}),
                  EventAccess(&AdultScarecrow,   {[]{return AdultScarecrow   || (IsAdult && Ocarina);},
                                      /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE))) && IsAdult;}}),
                }, {
                  //Locations
                  LocationAccess(LH_UNDERWATER_ITEM,        {[]{return IsChild && CanDive;}}),
                  LocationAccess(LH_SUN,                    {[]{return IsAdult && WaterTempleClear && CanUse(BOW);}}),
                  LocationAccess(LH_FREESTANDING_POH,       {[]{return IsAdult && (CanUse(SCARECROW) || CanPlantBean(LAKE_HYLIA));},
                                                 /*Glitched*/[]{return (IsAdult && CanUse(HOOKSHOT) && ScarecrowSong && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  LocationAccess(LH_GS_BEAN_PATCH,          {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(LH_GS_LAB_WALL,            {[]{return IsChild && (HookshotOrBoomerang || (LogicLabWallGS && (Sticks || KokiriSword || CanUse(MASTER_SWORD) || CanUse(BIGGORON_SWORD)))) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_GS_SMALL_ISLAND,        {[]{return IsChild && CanChildAttack && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_GS_TREE,                {[]{return IsAdult && CanUse(LONGSHOT) && AtNight && CanGetNightTimeGS;},
                                                 /*Glitched*/[]{return (CanDoGlitch(GlitchType::HookshotJump_Bonk, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::NOVICE)) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LH_LAB_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(LH_GOSSIP_STONE_SOUTHEAST, {[]{return true;}}),
                  LocationAccess(LH_GOSSIP_STONE_SOUTHWEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,          {[]{return true;}}),
                  Entrance(ZORAS_DOMAIN,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));},
                                       /*Glitched*/[]{return IsChild && (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::NaviDive_Stick, GlitchDifficulty::INTERMEDIATE) ||
                                                             (Bugs && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                             ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED)))));}}),
                  Entrance(LH_OWL_FLIGHT,         {[]{return IsChild;}}),
                  Entrance(LH_FISHING_ISLAND,     {[]{return IsChild || CanUse(SCARECROW) || CanPlantBean(LAKE_HYLIA) || WaterTempleClear;},
                                       /*Glitched*/[]{return (CanUse(HOOKSHOT) && ScarecrowSong && (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::NOVICE)))) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  Entrance(LH_LAB,                {[]{return true;}}),
                  Entrance(WATER_TEMPLE_ENTRYWAY, {[]{return CanUse(HOOKSHOT) && (CanUse(IRON_BOOTS) || (IsAdult && CanUse(LONGSHOT) && ProgressiveScale >= 2));},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::ADVANCED);}}),
                  Entrance(LH_GROTTO,             {[]{return true;}}),
  });

  areaTable[LH_FISHING_ISLAND] = Area("LH Fishing Island", "Lake Hylia", LAKE_HYLIA, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(LAKE_HYLIA,      {[]{return true;}}),
                  Entrance(LH_FISHING_HOLE, {[]{return true;}}),
  });

  areaTable[LH_OWL_FLIGHT] = Area("LH Owl Flight", "Lake Hylia", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[LH_LAB] = Area("LH Lab", "LH Lab", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&EyedropsAccess, {[]{return EyedropsAccess || (IsAdult && (EyeballFrogAccess || (EyeballFrog && DisableTradeRevert)));}}),
                }, {
                  //Locations
                  LocationAccess(LH_LAB_DIVE,     {[]{return ProgressiveScale >= 2 || (LogicLabDiving && CanUse(IRON_BOOTS) && CanUse(HOOKSHOT));},
                                       /*Glitched*/[]{return CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                             (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) && CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE));}}),
                  LocationAccess(LH_TRADE_FROG,   {[]{return IsAdult && EyeballFrog;}}),
                  LocationAccess(LH_GS_LAB_CRATE, {[]{return CanUse(IRON_BOOTS) && CanUse(HOOKSHOT);},
                                       /*Glitched*/[]{return (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && HasBombchus && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)) &&
                                                             (CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanUse(HOVER_BOOTS) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::INTERMEDIATE)) ||
                                                             (CanUse(FARORES_WIND) && CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || ProgressiveScale >= 2 || CanUse(IRON_BOOTS))) ||
                                                             (CanUse(IRON_BOOTS) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE));}}),
                }, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[LH_FISHING_HOLE] = Area("LH Fishing Hole", "LH Fishing Hole", NONE, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LH_CHILD_FISHING, {[]{return IsChild;}}),
                  LocationAccess(LH_ADULT_FISHING, {[]{return IsAdult;}}),
                }, {
                  //Exits
                  Entrance(LH_FISHING_ISLAND, {[]{return true;}}),
  });

  areaTable[LH_GROTTO] = Area("LH Grotto", "LH Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(LH_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LAKE_HYLIA, {[]{return true;}}),
  });

  areaTable[LON_LON_RANCH] = Area("Lon Lon Ranch", "Lon Lon Ranch", LON_LON_RANCH, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&Epona,    {[]{return Epona    || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                  EventAccess(&LinksCow, {[]{return LinksCow || (CanPlay(EponasSong) && IsAdult && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(SONG_FROM_MALON,     {[]{return IsChild && ZeldasLetter && Ocarina && AtDay;},
                                           /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                 ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLetter && AtDay;}}),
                  LocationAccess(LLR_GS_TREE,         {[]{return IsChild;}}),
                  LocationAccess(LLR_GS_RAIN_SHED,    {[]{return IsChild && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LLR_GS_HOUSE_WINDOW, {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(LLR_GS_BACK_WALL,    {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && IsChild && AtNight && CanGetNightTimeGS;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_FIELD,     {[]{return true;}}),
                  Entrance(LLR_TALONS_HOUSE, {[]{return true;}}),
                  Entrance(LLR_STABLES,      {[]{return true;}}),
                  Entrance(LLR_TOWER,        {[]{return true;}}),
                  Entrance(LLR_GROTTO,       {[]{return IsChild;}}),
  });

  areaTable[LLR_TALONS_HOUSE] = Area("LLR Talons House", "LLR Talons House", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_TALONS_CHICKENS, {[]{return IsChild && AtDay && ZeldasLetter;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_STABLES] = Area("LLR Stables", "LLR Stables", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_STABLES_LEFT_COW,  {[]{return CanPlay(EponasSong);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                  LocationAccess(LLR_STABLES_RIGHT_COW, {[]{return CanPlay(EponasSong);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_TOWER] = Area("LLR Tower", "LLR Tower", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_FREESTANDING_POH, {[]{return IsChild;}}),
                  LocationAccess(LLR_TOWER_LEFT_COW,   {[]{return CanPlay(EponasSong);},
                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                  LocationAccess(LLR_TOWER_RIGHT_COW,  {[]{return CanPlay(EponasSong);},
                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::IndoorBombOI, GlitchDifficulty::EXPERT) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED) && CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE))) && EponasSong;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });

  areaTable[LLR_GROTTO] = Area("LLR Grotto", "LLR Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_LEFT,   {[]{return CanStunDeku;}}),
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_RIGHT,  {[]{return CanStunDeku;}}),
                  LocationAccess(LLR_DEKU_SCRUB_GROTTO_CENTER, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(LON_LON_RANCH, {[]{return true;}}),
  });
}
