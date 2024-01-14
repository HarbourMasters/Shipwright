#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_ZorasDomain() {
  areaTable[RR_ZR_FRONT] = Area("ZR Front", "Zora River", RA_ZORAS_RIVER, DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZR_GS_TREE, {[]{return logic->IsChild && logic->CanChildAttack;}}),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER,  {[]{return logic->IsAdult || logic->CanBlastOrSmash;}}),
                  Entrance(RR_HYRULE_FIELD, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_RIVER] = Area("Zora River", "Zora River", RA_ZORAS_RIVER, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairy;}}),
                  EventAccess(&logic->BeanPlantFairy,   {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_ZORAS_RIVER) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugShrub,         {[]{return logic->BugShrub         || logic->CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(RC_ZR_MAGIC_BEAN_SALESMAN,               {[]{return logic->IsChild;}}),
                  LocationAccess(RC_ZR_FROGS_OCARINA_GAME,                {[]{return logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY) && logic->CanUse(RG_SARIAS_SONG) && logic->CanUse(RG_SUNS_SONG) && logic->CanUse(RG_EPONAS_SONG) && logic->CanUse(RG_SONG_OF_TIME) && logic->CanUse(RG_SONG_OF_STORMS);}}),
                  LocationAccess(RC_ZR_FROGS_IN_THE_RAIN,                 {[]{return logic->IsChild && logic->CanUse(RG_SONG_OF_STORMS);}}),
                  LocationAccess(RC_ZR_FROGS_ZELDAS_LULLABY,              {[]{return logic->IsChild && logic->CanUse(RG_ZELDAS_LULLABY);}}),
                  LocationAccess(RC_ZR_FROGS_EPONAS_SONG,                 {[]{return logic->IsChild && logic->CanUse(RG_EPONAS_SONG);}}),
                  LocationAccess(RC_ZR_FROGS_SARIAS_SONG,                 {[]{return logic->IsChild && logic->CanUse(RG_SARIAS_SONG);}}),
                  LocationAccess(RC_ZR_FROGS_SUNS_SONG,                   {[]{return logic->IsChild && logic->CanUse(RG_SUNS_SONG);}}),
                  LocationAccess(RC_ZR_FROGS_SONG_OF_TIME,                {[]{return logic->IsChild && logic->CanUse(RG_SONG_OF_TIME);}}),
                  LocationAccess(RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, {[]{return logic->IsChild || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && randoCtx->GetTrickOption(RT_ZR_LOWER));}}),
                  LocationAccess(RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,      {[]{return logic->IsChild || logic->CanUse(RG_HOVER_BOOTS) || (logic->IsAdult && randoCtx->GetTrickOption(RT_ZR_UPPER));}}),
                  LocationAccess(RC_ZR_GS_LADDER,                         {[]{return logic->IsChild && logic->AtNight && logic->CanChildAttack && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_GS_NEAR_RAISED_GROTTOS,            {[]{return logic->IsAdult && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_GS_ABOVE_BRIDGE,                   {[]{return logic->IsAdult && logic->CanUse(RG_HOOKSHOT) && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,          {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_ZR_FRONT,            {[]{return true;}}),
                  Entrance(RR_ZR_OPEN_GROTTO,      {[]{return true;}}),
                  Entrance(RR_ZR_FAIRY_GROTTO,     {[]{return Here(RR_ZORAS_RIVER, []{return logic->CanBlastOrSmash;});}}),
                  Entrance(RR_THE_LOST_WOODS,      {[]{return logic->CanDive || logic->CanUse(RG_IRON_BOOTS);}}),
                  Entrance(RR_ZR_STORMS_GROTTO,    {[]{return logic->CanOpenStormGrotto;}}),
                  Entrance(RR_ZR_BEHIND_WATERFALL, {[]{return logic->CanUse(RG_ZELDAS_LULLABY) || (logic->IsChild && randoCtx->GetTrickOption(RT_ZR_CUCCO)) || (logic->IsAdult && logic->CanUse(RG_HOVER_BOOTS) && randoCtx->GetTrickOption(RT_ZR_HOVERS));}}),
  });

  areaTable[RR_ZR_BEHIND_WATERFALL] = Area("ZR Behind Waterfall", "Zora River", RA_ZORAS_RIVER, DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER,  {[]{return true;}}),
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZR_OPEN_GROTTO] = Area("ZR Open Grotto", "ZR Open Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_ZR_OPEN_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_ZR_OPEN_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_ZR_OPEN_GROTTO_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZR_FAIRY_GROTTO] = Area("ZR Fairy Grotto", "ZR Fairy Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Event
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZR_STORMS_GROTTO] = Area("ZR Storms Grotto", "ZR Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZR_DEKU_SCRUB_GROTTO_REAR,  {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_ZR_DEKU_SCRUB_GROTTO_FRONT, {[]{return logic->CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_RIVER, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_DOMAIN] = Area("Zoras Domain", "Zoras Domain", RA_ZORAS_DOMAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->EyeballFrogAccess, {[]{return logic->EyeballFrogAccess || (logic->IsAdult && logic->KingZoraThawed && (logic->Eyedrops || logic->EyeballFrog || logic->Prescription || logic->PrescriptionAccess));}}),
                  EventAccess(&logic->GossipStoneFairy,  {[]{return logic->GossipStoneFairy  || logic->CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&logic->NutPot,            {[]{return true;}}),
                  EventAccess(&logic->StickPot,          {[]{return logic->StickPot          || logic->IsChild;}}),
                  EventAccess(&logic->FishGroup,         {[]{return logic->FishGroup         || logic->IsChild;}}),
                  EventAccess(&logic->KingZoraThawed,    {[]{return logic->KingZoraThawed    || (logic->IsAdult     && logic->BlueFire);}}),
                  EventAccess(&logic->DeliverLetter,     {[]{return logic->DeliverLetter     || (logic->RutosLetter && logic->IsChild && randoCtx->GetOption(RSK_ZORAS_FOUNTAIN).IsNot(RO_ZF_OPEN));}}),
                }, {
                  //Locations
                  LocationAccess(RC_ZD_DIVING_MINIGAME,     {[]{return logic->IsChild;}}),
                  LocationAccess(RC_ZD_CHEST,               {[]{return logic->IsChild && logic->CanUse(RG_STICKS);}}),
                  LocationAccess(RC_ZD_KING_ZORA_THAWED,    {[]{return logic->KingZoraThawed;}}),
                  LocationAccess(RC_ZD_TRADE_PRESCRIPTION,  {[]{return logic->KingZoraThawed && logic->Prescription;}}),
                  LocationAccess(RC_ZD_GS_FROZEN_WATERFALL, {[]{return logic->IsAdult && logic->AtNight && (logic->HookshotOrBoomerang || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->Bow || (logic->MagicMeter && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD))) || (randoCtx->GetTrickOption(RT_ZD_GS) && logic->CanJumpslash)) && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZD_FISH_1,              {[]{return logic->IsChild && logic->HasBottle;}}),
                  LocationAccess(RC_ZD_FISH_2,              {[]{return logic->IsChild && logic->HasBottle;}}),
                  LocationAccess(RC_ZD_FISH_3,              {[]{return logic->IsChild && logic->HasBottle;}}),
                  LocationAccess(RC_ZD_FISH_4,              {[]{return logic->IsChild && logic->HasBottle;}}),
                  LocationAccess(RC_ZD_FISH_5,              {[]{return logic->IsChild && logic->HasBottle;}}),
                  LocationAccess(RC_ZD_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_ZR_BEHIND_WATERFALL, {[]{return true;}}),
                  Entrance(RR_LAKE_HYLIA,          {[]{return logic->IsChild && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_ZD_BEHIND_KING_ZORA, {[]{return logic->DeliverLetter || randoCtx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (randoCtx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult) || (randoCtx->GetTrickOption(RT_ZD_KING_ZORA_SKIP) && logic->IsAdult);}}),
                  Entrance(RR_ZD_SHOP,             {[]{return logic->IsChild || logic->BlueFire;}}),
                  Entrance(RR_ZD_STORMS_GROTTO,    {[]{return logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_ZD_BEHIND_KING_ZORA] = Area("ZD Behind King Zora", "Zoras Domain", RA_ZORAS_DOMAIN, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_ZORAS_DOMAIN,   {[]{return logic->DeliverLetter || randoCtx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_OPEN) || (randoCtx->GetOption(RSK_ZORAS_FOUNTAIN).Is(RO_ZF_CLOSED_CHILD) && logic->IsAdult);}}),
                  Entrance(RR_ZORAS_FOUNTAIN, {[]{return true;}}),
  });

  areaTable[RR_ZD_SHOP] = Area("ZD Shop", "ZD Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZD_STORMS_GROTTO] = Area("ZD Storms Grotto", "ZD Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_ZORAS_DOMAIN, {[]{return true;}}),
  });

  areaTable[RR_ZORAS_FOUNTAIN] = Area("Zoras Fountain", "Zoras Fountain", RA_ZORAS_FOUNTAIN, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || (logic->CanUse(RG_STICKS) && logic->AtDay);}}),
                }, {
                  //Locations
                  LocationAccess(RC_ZF_ICEBERC_FREESTANDING_POH, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_ZF_BOTTOM_FREESTANDING_POH,  {[]{return logic->IsAdult && logic->IronBoots && logic->WaterTimer >= 24;}}),
                  LocationAccess(RC_ZF_GS_TREE,                  {[]{return logic->IsChild;}}),
                  LocationAccess(RC_ZF_GS_ABOVE_THE_LOG,         {[]{return logic->IsChild && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_ZF_GS_HIDDEN_CAVE,           {[]{return logic->CanUse(RG_SILVER_GAUNTLETS) && logic->CanBlastOrSmash && logic->HookshotOrBoomerang && logic->IsAdult && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_FAIRY_GOSSIP_STONE,       {[]{return true;}}),
                  LocationAccess(RC_JABU_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_ZD_BEHIND_KING_ZORA,       {[]{return true;}}),
                  Entrance(RR_JABU_JABUS_BELLY_ENTRYWAY, {[]{return (logic->IsChild && logic->Fish);}}),
                  Entrance(RR_ICE_CAVERN_ENTRYWAY,       {[]{return logic->IsAdult;}}),
                  Entrance(RR_ZF_GREAT_FAIRY_FOUNTAIN,   {[]{return logic->HasExplosives;}}),
  });

  areaTable[RR_ZF_GREAT_FAIRY_FOUNTAIN] = Area("ZF Great Fairy Fountain", "ZF Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_ZF_GREAT_FAIRY_REWARD, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Exits
                  Entrance(RR_ZORAS_FOUNTAIN, {[]{return true;}}),
  });
}
