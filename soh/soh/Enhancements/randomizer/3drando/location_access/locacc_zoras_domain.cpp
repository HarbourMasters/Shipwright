#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_ZorasDomain() {
  areaTable[ZR_FRONT] = Area("ZR Front", "Zora River", ZORAS_RIVER, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZR_GS_TREE, {[]{return IsChild && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return IsAdult || CanBlastOrSmash;},
                              /*Glitched*/[]{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);}}),
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[ZORAS_RIVER] = Area("Zora River", "Zora River", ZORAS_RIVER, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(ZORAS_RIVER) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(ZR_MAGIC_BEAN_SALESMAN,               {[]{return IsChild;}}),
                  LocationAccess(ZR_FROGS_OCARINA_GAME,                {[]{return IsChild && CanPlay(ZeldasLullaby) && CanPlay(SariasSong) && CanPlay(SunsSong) && CanPlay(EponasSong) && CanPlay(SongOfTime) && CanPlay(SongOfStorms);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLullaby && SariasSong && SunsSong && EponasSong && SongOfTime && SongOfStorms;}}),
                  LocationAccess(ZR_FROGS_IN_THE_RAIN,                 {[]{return IsChild && CanPlay(SongOfStorms);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SongOfStorms;}}),
                  LocationAccess(ZR_FROGS_ZELDAS_LULLABY,              {[]{return IsChild && CanPlay(ZeldasLullaby);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && ZeldasLullaby;}}),
                  LocationAccess(ZR_FROGS_EPONAS_SONG,                 {[]{return IsChild && CanPlay(EponasSong);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && EponasSong;}}),
                  LocationAccess(ZR_FROGS_SARIAS_SONG,                 {[]{return IsChild && CanPlay(SariasSong);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SariasSong;}}),
                  LocationAccess(ZR_FROGS_SUNS_SONG,                   {[]{return IsChild && CanPlay(SunsSong);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SunsSong;}}),
                  LocationAccess(ZR_FROGS_SONG_OF_TIME,                {[]{return IsChild && CanPlay(SongOfTime);},
                                                            /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::ADVANCED) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                                  ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && IsChild && SongOfTime;}}),
                  LocationAccess(ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverLower);}}),
                  LocationAccess(ZR_NEAR_DOMAIN_FREESTANDING_POH,      {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverUpper);}}),
                  LocationAccess(ZR_GS_LADDER,                         {[]{return IsChild && AtNight && CanChildAttack && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_GS_NEAR_RAISED_GROTTOS,            {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_GS_ABOVE_BRIDGE,                   {[]{return IsAdult && CanUse(HOOKSHOT) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZR_NEAR_GROTTOS_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(ZR_NEAR_DOMAIN_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_FRONT,            {[]{return true;}}),
                  Entrance(ZR_OPEN_GROTTO,      {[]{return true;}}),
                  Entrance(ZR_FAIRY_GROTTO,     {[]{return Here(ZORAS_RIVER, []{return CanBlastOrSmash;});},
                                     /*Glitched*/[]{return Here(ZORAS_RIVER, []{return CanUse(STICKS) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED);});}}),
                  Entrance(THE_LOST_WOODS,      {[]{return CanDive || CanUse(IRON_BOOTS);},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::INTERMEDIATE) || (CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || (IsAdult && (HasBoots || ClaimCheck)) || (IsChild && WeirdEgg)))) &&
                                                           ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(ZR_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;},
                                     /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                           ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return CanPlay(ZeldasLullaby);},
                                     /*Glitched*/[]{return CanDoGlitch(GlitchType::HookshotJump_Boots, GlitchDifficulty::ADVANCED) || ((CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) ||
                                                           ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) || ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby);}}),
  });

  areaTable[ZR_BEHIND_WATERFALL] = Area("ZR Behind Waterfall", "Zora River", NONE, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return true;}}),
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZR_OPEN_GROTTO] = Area("ZR Open Grotto", "ZR Open Grotto", NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(ZR_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(ZR_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_FAIRY_GROTTO] = Area("ZR Fairy Grotto", "ZR Fairy Grotto", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Event
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_STORMS_GROTTO] = Area("ZR Storms Grotto", "ZR Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZR_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(ZR_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZORAS_DOMAIN] = Area("Zoras Domain", "Zoras Domain", ZORAS_DOMAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&EyeballFrogAccess, {[]{return EyeballFrogAccess || (IsAdult && KingZoraThawed && (Eyedrops || EyeballFrog || Prescription || PrescriptionAccess));}}),
                  EventAccess(&GossipStoneFairy,  {[]{return GossipStoneFairy  || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&NutPot,            {[]{return true;}}),
                  EventAccess(&StickPot,          {[]{return StickPot          || IsChild;}}),
                  EventAccess(&FishGroup,         {[]{return FishGroup         || IsChild;}}),
                  EventAccess(&KingZoraThawed,    {[]{return KingZoraThawed    || (IsAdult     && BlueFire);}}),
                  EventAccess(&DeliverLetter,     {[]{return DeliverLetter     || (RutosLetter && IsChild && ZorasFountain.IsNot(ZORASFOUNTAIN_OPEN));}}),
                }, {
                  //Locations
                  LocationAccess(ZD_DIVING_MINIGAME,     {[]{return IsChild;}}),
                  LocationAccess(ZD_CHEST,               {[]{return IsChild && CanUse(STICKS);}}),
                  LocationAccess(ZD_KING_ZORA_THAWED,    {[]{return KingZoraThawed;}}),
                  LocationAccess(ZD_TRADE_PRESCRIPTION,  {[]{return KingZoraThawed && Prescription;}}),
                  LocationAccess(ZD_GS_FROZEN_WATERFALL, {[]{return IsAdult && AtNight && (HookshotOrBoomerang || CanUse(SLINGSHOT) || Bow || MagicMeter) && CanGetNightTimeGS;}}),
                  LocationAccess(ZD_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return true;}}),
                  Entrance(LAKE_HYLIA,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));},
                                     /*Glitched*/[]{return (IsAdult && GlitchZDOoBJumpSlash) || (IsChild && CanUse(FARORES_WIND) && (FaroresWindAnywhere || (CanDoGlitch(GlitchType::RestrictedItems, GlitchDifficulty::NOVICE) && (HasBottle || WeirdEgg))) &&
                                                           ((CanUse(NAYRUS_LOVE) && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::NOVICE)) || (CanUseMagicArrow && CanDoGlitch(GlitchType::CutsceneDive, GlitchDifficulty::ADVANCED))));}}),
                  Entrance(ZD_BEHIND_KING_ZORA, {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult);},
                                     /*Glitched*/[]{return ((IsChild || CanUse(IRON_BOOTS)) && CanDoGlitch(GlitchType::TripleSlashClip, GlitchDifficulty::NOVICE)) || CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::NOVICE) ||
                                                           CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::NOVICE) || CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) || CanDoGlitch(GlitchType::LedgeClip, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(ZD_SHOP,             {[]{return IsChild || BlueFire;},
                                     /*Glitched*/[]{return GlitchZDOoBJumpSlash.Value<bool>();}}),
                  Entrance(ZD_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;},
                                     /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                           ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && SongOfStorms && (ShardOfAgony || LogicGrottosWithoutAgony);}}),
  });

  areaTable[ZD_BEHIND_KING_ZORA] = Area("ZD Behind King Zora", "Zoras Domain", NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN,   {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult);},
                                /*Glitched*/[]{return CanDoGlitch(GlitchType::ASlide, GlitchDifficulty::NOVICE) || (CanUse(LONGSHOT) && CanDoGlitch(GlitchType::HookshotClip, GlitchDifficulty::NOVICE)) || (Bombs && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE) && CanDoGlitch(GlitchType::ISG, GlitchDifficulty::INTERMEDIATE));}}),
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[ZD_SHOP] = Area("ZD Shop", "ZD Shop", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZD_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(ZD_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZD_STORMS_GROTTO] = Area("ZD Storms Grotto", "ZD Storms Grotto", NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZORAS_FOUNTAIN] = Area("Zoras Fountain", "Zoras Fountain", ZORAS_FOUNTAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(STICKS) && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(ZF_ICEBERG_FREESTANDING_POH, {[]{return IsAdult;}}),
                  LocationAccess(ZF_BOTTOM_FREESTANDING_POH,  {[]{return IsAdult && IronBoots && WaterTimer >= 24;}}),
                  LocationAccess(ZF_GS_TREE,                  {[]{return IsChild;}}),
                  LocationAccess(ZF_GS_ABOVE_THE_LOG,         {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;},
                                                   /*Glitched*/[]{return IsChild && AtNight && CanGetNightTimeGS && HasBombchus && CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::NOVICE);}}),
                  LocationAccess(ZF_GS_HIDDEN_CAVE,           {[]{return CanUse(SILVER_GAUNTLETS) && CanBlastOrSmash && HookshotOrBoomerang && IsAdult && AtNight && CanGetNightTimeGS;},
                                                   /*Glitched*/[]{return CanDoGlitch(GlitchType::LedgeCancel, GlitchDifficulty::INTERMEDIATE) && IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(ZF_FAIRY_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(ZF_JABU_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZD_BEHIND_KING_ZORA,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY, {[]{return (IsChild && Fish);},
                                           /*Glitched*/[]{return (IsChild && CanUse(STICKS) && GlitchJabuStickRecoil) || (IsAdult && GlitchJabuAdult);}}),
                  Entrance(ICE_CAVERN_ENTRYWAY,       {[]{return IsAdult;},
                                           /*Glitched*/[]{return CanDoGlitch(GlitchType::BombHover, GlitchDifficulty::INTERMEDIATE);}}),
                  Entrance(ZF_GREAT_FAIRY_FOUNTAIN,   {[]{return HasExplosives || (CanUse(SILVER_GAUNTLETS) && Hammer && LogicZFGreatFairy);},
                                           /*Glitched*/[]{return IsChild && (KokiriSword || Sticks) && CanShield && (CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::ADVANCED) || (CanDoGlitch(GlitchType::ISG, GlitchDifficulty::NOVICE) && CanDoGlitch(GlitchType::SeamWalk, GlitchDifficulty::INTERMEDIATE)));}}),
  });

  areaTable[ZF_GREAT_FAIRY_FOUNTAIN] = Area("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(ZF_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);},
                                             /*Glitched*/[]{return (CanDoGlitch(GlitchType::OutdoorBombOI, GlitchDifficulty::INTERMEDIATE) || ((Bugs || Fish) && CanShield && (CanSurviveDamage || (Fairy && NumBottles >= 2)) && CanDoGlitch(GlitchType::QPA, GlitchDifficulty::ADVANCED)) ||
                                                                   ((Bugs || Fish) && CanShield && HasBombchus && CanDoGlitch(GlitchType::ActionSwap, GlitchDifficulty::ADVANCED))) && ZeldasLullaby;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });
}
