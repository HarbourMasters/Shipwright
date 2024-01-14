#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_LostWoods() {
  areaTable[RR_KOKIRI_FOREST] = Area("Kokiri Forest", "Kokiri Forest", RA_KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->BeanPlantFairy,           {[]{return logic->BeanPlantFairy   || (CanPlantBean(RR_KOKIRI_FOREST) && logic->CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&logic->GossipStoneFairy,         {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&logic->ShowedMidoSwordAndShield, {[]{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->KokiriSword && logic->DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(RC_KF_KOKIRI_SWORD_CHEST,   {[]{return logic->IsChild;}}),
                  LocationAccess(RC_KF_GS_KNOW_IT_ALL_HOUSE, {[]{return logic->IsChild && logic->CanChildAttack && logic->AtNight && (/*TODO: HasNightStart ||*/ logic->CanLeaveForest || logic->CanUse(RG_SUNS_SONG)) && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KF_GS_BEAN_PATCH,        {[]{return logic->CanPlantBugs && logic->CanChildAttack;}}),
                  LocationAccess(RC_KF_GS_HOUSE_OF_TWINS,    {[]{return logic->IsAdult && logic->AtNight && (logic->HookshotOrBoomerang || (randoCtx->GetTrickOption(RT_KF_ADULT_GS) && logic->CanUse(RG_HOVER_BOOTS))) && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_KF_GOSSIP_STONE,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_MIDOS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_SARIAS_HOUSE,       {[]{return true;}}),
                  Entrance(RR_KF_HOUSE_OF_TWINS,     {[]{return true;}}),
                  Entrance(RR_KF_KNOW_IT_ALL_HOUSE,  {[]{return true;}}),
                  Entrance(RR_KF_KOKIRI_SHOP,        {[]{return true;}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE,  {[]{return logic->IsAdult || randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || logic->ShowedMidoSwordAndShield;}}),
                  Entrance(RR_THE_LOST_WOODS,        {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE_FROM_FOREST, {[]{return logic->IsAdult || randoCtx->GetOption(RSK_FOREST).IsNot(RO_FOREST_CLOSED) || logic->DekuTreeClear;}}),
                  Entrance(RR_KF_STORMS_GROTTO,      {[]{return logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_KF_OUTSIDE_DEKU_TREE] = Area("KF Outside Deku Tree", "Kokiri Forest", RA_KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->DekuBabaSticks,           {[]{return logic->DekuBabaSticks || ((logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_KOKIRI_SWORD)) && !randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES)) || (logic->IsChild && (logic->CanUse(RG_KOKIRI_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BOOMERANG))));}}),
                  EventAccess(&logic->DekuBabaNuts,             {[]{return logic->DekuBabaNuts   || ((logic->IsAdult && (logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD) || logic->CanUse(RG_KOKIRI_SWORD)) && !randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES)) || (logic->IsChild && (logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->HasExplosives || logic->CanUse(RG_DINS_FIRE))));}}),
                  EventAccess(&logic->ShowedMidoSwordAndShield, {[]{return logic->ShowedMidoSwordAndShield || (logic->IsChild && logic->KokiriSword && logic->DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,  {[]{return true;}}),
                  LocationAccess(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY, {[]{return logic->IsChild || (randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && (randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || logic->ShowedMidoSwordAndShield));}}),
                  Entrance(RR_KOKIRI_FOREST,      {[]{return logic->IsAdult || randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || logic->ShowedMidoSwordAndShield;}}),
  });

  areaTable[RR_KF_LINKS_HOUSE] = Area("KF Link's House", "KF Link's House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KF_LINKS_HOUSE_COW, {[]{return logic->IsAdult && logic->CanUse(RG_EPONAS_SONG) && logic->LinksCow;}}),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_KF_MIDOS_HOUSE] = Area("KF Mido's House", "KF Mido's House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KF_MIDOS_TOP_LEFT_CHEST,     {[]{return true;}}),
                  LocationAccess(RC_KF_MIDOS_TOP_RIGHT_CHEST,    {[]{return true;}}),
                  LocationAccess(RC_KF_MIDOS_BOTTOM_LEFT_CHEST,  {[]{return true;}}),
                  LocationAccess(RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_SARIAS_HOUSE] = Area("KF Saria's House", "KF Saria's House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_HOUSE_OF_TWINS] = Area("KF House of Twins", "KF House of Twins", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_KNOW_IT_ALL_HOUSE] = Area("KF Know It All House", "KF Know It All House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_KOKIRI_SHOP] = Area("KF Kokiri Shop", "KF Kokiri Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KF_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_KF_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_STORMS_GROTTO] = Area("KF Storms Grotto", "KF Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_KF_STORMS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_KF_STORMS_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_KF_STORMS_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_LW_FOREST_EXIT] = Area("LW Forest Exit", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}})
  });

  areaTable[RR_THE_LOST_WOODS] = Area("Lost Woods", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->OddMushroomAccess, {[]{return logic->OddMushroomAccess || (logic->IsAdult && (logic->CojiroAccess || logic->Cojiro));}}),
                  EventAccess(&logic->PoachersSawAccess, {[]{return logic->PoachersSawAccess || (logic->IsAdult && logic->OddPoulticeAccess);}}),
                  EventAccess(&logic->GossipStoneFairy,  {[]{return logic->GossipStoneFairy  || logic->CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&logic->BeanPlantFairy,    {[]{return logic->BeanPlantFairy    || logic->CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&logic->BugShrub,          {[]{return logic->IsChild && logic->CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(RC_LW_SKULL_KID,                 {[]{return logic->IsChild && logic->CanUse(RG_SARIAS_SONG);}}),
                  LocationAccess(RC_LW_TRADE_COJIRO,              {[]{return logic->IsAdult && logic->Cojiro;}}),
                  LocationAccess(RC_LW_TRADE_ODD_POTION,        {[]{return logic->IsAdult && logic->OddPoultice && logic->Cojiro;}}),
                                                                                                //all 5 buttons are logically required for memory game
                                                                                                //because the chances of being able to beat it
                                                                                                //every time you attempt it are as follows:
                                                                                                //0 or 1 button(s) => 0%
                                                                                                //2 buttons        => 0.15625%
                                                                                                //3 buttons        => 3.75%
                                                                                                //4 buttons        => 25.3125%
                                                                                                //5 buttons        => 100%
                  LocationAccess(RC_LW_OCARINA_MEMORY_GAME,       {[]{return logic->IsChild && logic->Ocarina && logic->OcarinaButtons >= 5;}}),
                  LocationAccess(RC_LW_TARGET_IN_WOODS,           {[]{return logic->IsChild && logic->CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_BRIDGE,    {[]{return logic->IsChild && logic->CanStunDeku;}}),
                  LocationAccess(RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE, {[]{return logic->CanPlantBugs && logic->CanChildAttack;}}),
                  LocationAccess(RC_LW_GOSSIP_STONE,              {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,           {[]{return true;}}),
                  Entrance(RR_GC_WOODS_WARP,            {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE,                {[]{return logic->CanLeaveForest && ((logic->IsAdult && (CanPlantBean(RR_THE_LOST_WOODS) || randoCtx->GetTrickOption(RT_LW_BRIDGE))) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_LONGSHOT));}}),
                  Entrance(RR_ZORAS_RIVER,              {[]{return logic->CanLeaveForest && (logic->CanDive || logic->CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_LW_BEYOND_MIDO,           {[]{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG) || randoCtx->GetTrickOption(RT_LW_MIDO_BACKFLIP);}}),
                  Entrance(RR_LW_NEAR_SHORTCUTS_GROTTO, {[]{return Here(RR_THE_LOST_WOODS, []{return logic->CanBlastOrSmash;});}}),
  });

  areaTable[RR_LW_BEYOND_MIDO] = Area("LW Beyond Mido", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->ButterflyFairy, {[]{return logic->ButterflyFairy || logic->CanUse(RG_STICKS);}}),
                }, {
                  //Locations
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, {[]{return logic->IsChild && logic->CanStunDeku;}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  {[]{return logic->IsChild && logic->CanStunDeku;}}),
                  LocationAccess(RC_LW_GS_ABOVE_THEATER,                   {[]{return logic->IsAdult && logic->AtNight && ((CanPlantBean(RR_LW_BEYOND_MIDO) && logic->CanAdultAttack) || (randoCtx->GetTrickOption(RT_LW_GS_BEAN) && logic->CanUse(RG_HOOKSHOT) && (logic->CanUse(RG_LONGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->HasBombchus || logic->CanUse(RG_DINS_FIRE)))) && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_LW_GS_BEAN_PATCH_NEAR_THEATER,         {[]{return logic->CanPlantBugs && (logic->CanChildAttack || (randoCtx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_OFF) && logic->DekuShield));}}),
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,   {[]{return true;}}),
                  Entrance(RR_THE_LOST_WOODS,   {[]{return logic->IsChild || logic->CanUse(RG_SARIAS_SONG);}}),
                  Entrance(RR_SFM_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_DEKU_THEATER,     {[]{return true;}}),
                  Entrance(RR_LW_SCRUBS_GROTTO, {[]{return Here(RR_LW_BEYOND_MIDO, []{return logic->CanBlastOrSmash;});}}),
  });

  areaTable[RR_LW_NEAR_SHORTCUTS_GROTTO] = Area("LW Near Shortcuts Grotto", "LW Near Shortcuts Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_LW_NEAR_SHORTCUTS_GROTTO_FISH,         {[]{return logic->HasBottle;}}),
                  LocationAccess(RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[RR_DEKU_THEATER] = Area("Deku Theater", "Deku Theater", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_THEATER_SKULL_MASK,    {[]{return logic->IsChild && logic->SkullMask;}}),
                  LocationAccess(RC_DEKU_THEATER_MASK_OF_TRUTH, {[]{return logic->IsChild && logic->MaskOfTruth;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_LW_SCRUBS_GROTTO] = Area("LW Scrubs Grotto", "LW Scrubs Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LW_DEKU_SCRUB_GROTTO_REAR,  {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_GROTTO_FRONT, {[]{return logic->CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_SFM_ENTRYWAY] = Area("SFM Entryway", "Sacred Forest Meadow", RA_SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO,       {[]{return true;}}),
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return logic->CanJumpslash || logic->CanUse(RG_FAIRY_SLINGSHOT) || logic->CanUse(RG_FAIRY_BOW) || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_SFM_WOLFOS_GROTTO,    {[]{return logic->CanOpenBombGrotto;}}),
  });

  areaTable[RR_SACRED_FOREST_MEADOW] = Area("Sacred Forest Meadow", "Sacred Forest Meadow", RA_SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SONG_FROM_SARIA,             {[]{return logic->IsChild && logic->ZeldasLetter;}}),
                  LocationAccess(RC_SHEIK_IN_FOREST,             {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_SFM_GS,                      {[]{return logic->IsAdult && logic->HookshotOrBoomerang && logic->AtNight && logic->CanGetNightTimeGS;}}),
                  LocationAccess(RC_SFM_MAZE_LOWER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_SFM_MAZE_UPPER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_SFM_SARIA_GOSSIP_STONE,      {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY,           {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY, {[]{return logic->CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SFM_FAIRY_GROTTO,       {[]{return true;}}),
                  Entrance(RR_SFM_STORMS_GROTTO,      {[]{return logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_SFM_FAIRY_GROTTO] = Area("SFM Fairy Grotto", "SFM Fairy Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_SFM_WOLFOS_GROTTO] = Area("SFM Wolfos Grotto", "SFM Wolfos Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SFM_WOLFOS_GROTTO_CHEST, {[]{return logic->IsAdult || logic->Slingshot || logic->Sticks || logic->KokiriSword || logic->CanUse(RG_DINS_FIRE) || logic->CanUse(RG_MEGATON_HAMMER) || logic->CanUse(RG_MASTER_SWORD) || logic->CanUse(RG_BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[RR_SFM_STORMS_GROTTO] = Area("SFM Storms Grotto", "SFM Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SFM_DEKU_SCRUB_GROTTO_REAR,  {[]{return logic->CanStunDeku;}}),
                  LocationAccess(RC_SFM_DEKU_SCRUB_GROTTO_FRONT, {[]{return logic->CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_LW_BRIDGE_FROM_FOREST] = Area("LW Bridge From Forest", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LW_GIFT_FROM_SARIA, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BRIDGE, {[]{return true;}}),
  });

  areaTable[RR_LW_BRIDGE] = Area("LW Bridge", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST,  {[]{return true;}}),
                  Entrance(RR_HYRULE_FIELD,   {[]{return true;}}),
                  Entrance(RR_THE_LOST_WOODS, {[]{return logic->CanUse(RG_LONGSHOT);}}),
  });
}
