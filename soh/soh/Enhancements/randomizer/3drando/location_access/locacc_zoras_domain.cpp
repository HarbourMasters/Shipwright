#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_ZorasDomain() {
  areaTable[ZR_FRONT] = Area("ZR Front", "Zora River", RHT_ZORAS_RIVER, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZR_GS_TREE, {[]{return IsChild && CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return IsAdult || CanBlastOrSmash;}}),
                  Entrance(HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[ZORAS_RIVER] = Area("Zora River", "Zora River", RHT_ZORAS_RIVER, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&BeanPlantFairy,   {[]{return BeanPlantFairy   || (CanPlantBean(ZORAS_RIVER) && CanPlay(SongOfStorms));}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugShrub,         {[]{return BugShrub         || CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(RC_ZR_MAGIC_BEAN_SALESMAN,               {[]{return IsChild;}}),
                  LocationAccess(RC_ZR_FROGS_OCARINA_GAME,                {[]{return IsChild && CanPlay(ZeldasLullaby) && CanPlay(SariasSong) && CanPlay(SunsSong) && CanPlay(EponasSong) && CanPlay(SongOfTime) && CanPlay(SongOfStorms);}}),
                  LocationAccess(RC_ZR_FROGS_IN_THE_RAIN,                 {[]{return IsChild && CanPlay(SongOfStorms);}}),
                  LocationAccess(RC_ZR_FROGS_ZELDAS_LULLABY,              {[]{return IsChild && CanPlay(ZeldasLullaby);}}),
                  LocationAccess(RC_ZR_FROGS_EPONAS_SONG,                 {[]{return IsChild && CanPlay(EponasSong);}}),
                  LocationAccess(RC_ZR_FROGS_SARIAS_SONG,                 {[]{return IsChild && CanPlay(SariasSong);}}),
                  LocationAccess(RC_ZR_FROGS_SUNS_SONG,                   {[]{return IsChild && CanPlay(SunsSong);}}),
                  LocationAccess(RC_ZR_FROGS_SONG_OF_TIME,                {[]{return IsChild && CanPlay(SongOfTime);}}),
                  LocationAccess(RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverLower);}}),
                  LocationAccess(RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,      {[]{return IsChild || CanUse(HOVER_BOOTS) || (IsAdult && LogicZoraRiverUpper);}}),
                  LocationAccess(RC_ZR_GS_LADDER,                         {[]{return IsChild && AtNight && CanChildAttack && CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_GS_NEAR_RAISED_GROTTOS,            {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_GS_ABOVE_BRIDGE,                   {[]{return IsAdult && CanUse(HOOKSHOT) && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_FRONT,            {[]{return true;}}),
                  Entrance(ZR_OPEN_GROTTO,      {[]{return true;}}),
                  Entrance(ZR_FAIRY_GROTTO,     {[]{return Here(ZORAS_RIVER, []{return CanBlastOrSmash;});}}),
                  Entrance(THE_LOST_WOODS,      {[]{return CanDive || CanUse(IRON_BOOTS);}}),
                  Entrance(ZR_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;}}),
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return CanPlay(ZeldasLullaby) || (IsChild && LogicZoraWithCucco) || (IsAdult && CanUse(HOVER_BOOTS) && LogicZoraWithHovers);}}),
  });

  areaTable[ZR_BEHIND_WATERFALL] = Area("ZR Behind Waterfall", "Zora River", RHT_NONE, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER,  {[]{return true;}}),
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZR_OPEN_GROTTO] = Area("ZR Open Grotto", "ZR Open Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_ZR_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_ZR_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_FAIRY_GROTTO] = Area("ZR Fairy Grotto", "ZR Fairy Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Event
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZR_STORMS_GROTTO] = Area("ZR Storms Grotto", "ZR Storms Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZR_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_ZR_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[ZORAS_DOMAIN] = Area("Zoras Domain", "Zoras Domain", RHT_ZORAS_DOMAIN, NO_DAY_NIGHT_CYCLE, {
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
                  LocationAccess(RC_ZD_DIVING_MINIGAME,     {[]{return IsChild;}}),
                  LocationAccess(RC_ZD_CHEST,               {[]{return IsChild && CanUse(STICKS);}}),
                  LocationAccess(RC_ZD_KING_ZORA_THAWED,    {[]{return KingZoraThawed;}}),
                  LocationAccess(RC_ZD_TRADE_PRESCRIPTION,  {[]{return KingZoraThawed && Prescription;}}),
                  LocationAccess(RC_ZD_GS_FROZEN_WATERFALL, {[]{return IsAdult && AtNight && (HookshotOrBoomerang || CanUse(SLINGSHOT) || Bow || MagicMeter || LogicDomainGS) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZD_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZR_BEHIND_WATERFALL, {[]{return true;}}),
                  Entrance(LAKE_HYLIA,          {[]{return IsChild && (CanDive || CanUse(IRON_BOOTS));}}),
                  Entrance(ZD_BEHIND_KING_ZORA, {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult) || (LogicKingZoraSkip && IsAdult);}}),
                  Entrance(ZD_SHOP,             {[]{return IsChild || BlueFire;}}),
                  Entrance(ZD_STORMS_GROTTO,    {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[ZD_BEHIND_KING_ZORA] = Area("ZD Behind King Zora", "Zoras Domain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN,   {[]{return DeliverLetter || ZorasFountain.Is(ZORASFOUNTAIN_OPEN) || (ZorasFountain.Is(ZORASFOUNTAIN_ADULT) && IsAdult);}}),
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[ZD_SHOP] = Area("ZD Shop", "ZD Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZD_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_ZD_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZD_STORMS_GROTTO] = Area("ZD Storms Grotto", "ZD Storms Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[ZORAS_FOUNTAIN] = Area("Zoras Fountain", "Zoras Fountain", RHT_ZORAS_FOUNTAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || (CanUse(STICKS) && AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(RC_ZF_ICEBERC_FREESTANDING_POH, {[]{return IsAdult;}}),
                  LocationAccess(RC_ZF_BOTTOM_FREESTANDING_POH,  {[]{return IsAdult && IronBoots && WaterTimer >= 24;}}),
                  LocationAccess(RC_ZF_GS_TREE,                  {[]{return IsChild;}}),
                  LocationAccess(RC_ZF_GS_ABOVE_THE_LOG,         {[]{return IsChild && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZF_GS_HIDDEN_CAVE,           {[]{return CanUse(SILVER_GAUNTLETS) && CanBlastOrSmash && HookshotOrBoomerang && IsAdult && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_FAIRY_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(RC_JABU_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(ZD_BEHIND_KING_ZORA,       {[]{return true;}}),
                  Entrance(JABU_JABUS_BELLY_ENTRYWAY, {[]{return (IsChild && Fish);}}),
                  Entrance(ICE_CAVERN_ENTRYWAY,       {[]{return IsAdult;}}),
                  Entrance(ZF_GREAT_FAIRY_FOUNTAIN,   {[]{return HasExplosives;}}),
  });

  areaTable[ZF_GREAT_FAIRY_FOUNTAIN] = Area("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZF_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(ZORAS_FOUNTAIN, {[]{return true;}}),
  });
}
