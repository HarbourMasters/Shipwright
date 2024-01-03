#include "../location_access.hpp"
#include "../logic.hpp"
#include "../../entrance.h"

using namespace Logic;
using namespace Rando;

void AreaTable_Init_LostWoods() {
  areaTable[RR_KOKIRI_FOREST] = Area("Kokiri Forest", "Kokiri Forest", RA_KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&BeanPlantFairy,           {[]{return BeanPlantFairy   || (CanPlantBean(RR_KOKIRI_FOREST) && CanUse(RG_SONG_OF_STORMS));}}),
                  EventAccess(&GossipStoneFairy,         {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(RC_KF_KOKIRI_SWORD_CHEST,   {[]{return IsChild;}}),
                  LocationAccess(RC_KF_GS_KNOW_IT_ALL_HOUSE, {[]{return IsChild && CanChildAttack && AtNight && (/*TODO: HasNightStart ||*/ CanLeaveForest || CanUse(RG_SUNS_SONG)) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KF_GS_BEAN_PATCH,        {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(RC_KF_GS_HOUSE_OF_TWINS,    {[]{return IsAdult && AtNight && (HookshotOrBoomerang || (randoCtx->GetTrickOption(RT_KF_ADULT_GS) && CanUse(RG_HOVER_BOOTS))) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_KF_GOSSIP_STONE,         {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KF_LINKS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_MIDOS_HOUSE,        {[]{return true;}}),
                  Entrance(RR_KF_SARIAS_HOUSE,       {[]{return true;}}),
                  Entrance(RR_KF_HOUSE_OF_TWINS,     {[]{return true;}}),
                  Entrance(RR_KF_KNOW_IT_ALL_HOUSE,  {[]{return true;}}),
                  Entrance(RR_KF_KOKIRI_SHOP,        {[]{return true;}}),
                  Entrance(RR_KF_OUTSIDE_DEKU_TREE,  {[]{return IsAdult || randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || ShowedMidoSwordAndShield;}}),
                  Entrance(RR_THE_LOST_WOODS,        {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE_FROM_FOREST, {[]{return IsAdult || randoCtx->GetOption(RSK_FOREST).IsNot(RO_FOREST_CLOSED) || DekuTreeClear;}}),
                  Entrance(RR_KF_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[RR_KF_OUTSIDE_DEKU_TREE] = Area("KF Outside Deku Tree", "Kokiri Forest", RA_KOKIRI_FOREST, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&DekuBabaSticks,           {[]{return DekuBabaSticks || ((IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_KOKIRI_SWORD)) && !randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES)) || (IsChild && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BOOMERANG))));}}),
                  EventAccess(&DekuBabaNuts,             {[]{return DekuBabaNuts   || ((IsAdult && (CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_KOKIRI_SWORD)) && !randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES)) || (IsChild && (CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || HasExplosives || CanUse(RG_DINS_FIRE))));}}),
                  EventAccess(&ShowedMidoSwordAndShield, {[]{return ShowedMidoSwordAndShield || (IsChild && KokiriSword && DekuShield);}}),
                }, {
                  //Locations
                  LocationAccess(RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,  {[]{return true;}}),
                  LocationAccess(RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_DEKU_TREE_ENTRYWAY, {[]{return IsChild || (randoCtx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF) && (randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || ShowedMidoSwordAndShield));}}),
                  Entrance(RR_KOKIRI_FOREST,      {[]{return IsAdult || randoCtx->GetOption(RSK_FOREST).Is(RO_FOREST_OPEN) || ShowedMidoSwordAndShield;}}),
  });

  areaTable[RR_KF_LINKS_HOUSE] = Area("KF Link's House", "KF Link's House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KF_LINKS_HOUSE_COW, {[]{return IsAdult && CanUse(RG_EPONAS_SONG) && LinksCow;}}),
                  LocationAccess(RC_KF_LINKS_HOUSE_POT, {[]{return true;}}),
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

  areaTable[RR_KF_HOUSE_OF_TWINS] = Area("KF House of Twins", "KF House of Twins", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_KF_TWINS_HOUSE_POT_1, {[]{return true;}}),
                  LocationAccess(RC_KF_TWINS_HOUSE_POT_2, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_KOKIRI_FOREST, {[]{return true;}}),
  });

  areaTable[RR_KF_KNOW_IT_ALL_HOUSE] = Area("KF Know It All House", "KF Know It All House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                 // Locations
                 LocationAccess(RC_KF_BROTHERS_HOUSE_POT_1, {[]{return true;}}),
                 LocationAccess(RC_KF_BROTHERS_HOUSE_POT_2, {[]{return true;}}),
                }, {
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
                  EventAccess(&OddMushroomAccess, {[]{return OddMushroomAccess || (IsAdult && (CojiroAccess || Cojiro));}}),
                  EventAccess(&PoachersSawAccess, {[]{return PoachersSawAccess || (IsAdult && OddPoulticeAccess);}}),
                  EventAccess(&GossipStoneFairy,  {[]{return GossipStoneFairy  || CanSummonGossipFairyWithoutSuns;}}),
                  EventAccess(&BeanPlantFairy,    {[]{return BeanPlantFairy    || CanUse(RG_SONG_OF_STORMS);}}),
                  EventAccess(&BugShrub,          {[]{return IsChild && CanCutShrubs;}}),
                }, {
                  //Locations
                  LocationAccess(RC_LW_SKULL_KID,                 {[]{return IsChild && CanUse(RG_SARIAS_SONG);}}),
                  LocationAccess(RC_LW_TRADE_COJIRO,              {[]{return IsAdult && Cojiro;}}),
                  LocationAccess(RC_LW_TRADE_ODD_POTION,        {[]{return IsAdult && OddPoultice && Cojiro;}}),
                                                                                                //all 5 buttons are logically required for memory game
                                                                                                //because the chances of being able to beat it
                                                                                                //every time you attempt it are as follows:
                                                                                                //0 or 1 button(s) => 0%
                                                                                                //2 buttons        => 0.15625%
                                                                                                //3 buttons        => 3.75%
                                                                                                //4 buttons        => 25.3125%
                                                                                                //5 buttons        => 100%
                  LocationAccess(RC_LW_OCARINA_MEMORY_GAME,       {[]{return IsChild && Ocarina && OcarinaButtons >= 5;}}),
                  LocationAccess(RC_LW_TARGET_IN_WOODS,           {[]{return IsChild && CanUse(RG_FAIRY_SLINGSHOT);}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_BRIDGE,    {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE, {[]{return CanPlantBugs && CanChildAttack;}}),
                  LocationAccess(RC_LW_GOSSIP_STONE,              {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,           {[]{return true;}}),
                  Entrance(RR_GC_WOODS_WARP,            {[]{return true;}}),
                  Entrance(RR_LW_BRIDGE,                {[]{return CanLeaveForest && ((IsAdult && (CanPlantBean(RR_THE_LOST_WOODS) || randoCtx->GetTrickOption(RT_LW_BRIDGE))) || CanUse(RG_HOVER_BOOTS) || CanUse(RG_LONGSHOT));}}),
                  Entrance(RR_ZORAS_RIVER,              {[]{return CanLeaveForest && (CanDive || CanUse(RG_IRON_BOOTS));}}),
                  Entrance(RR_LW_BEYOND_MIDO,           {[]{return IsChild || CanUse(RG_SARIAS_SONG) || randoCtx->GetTrickOption(RT_LW_MIDO_BACKFLIP);}}),
                  Entrance(RR_LW_NEAR_SHORTCUTS_GROTTO, {[]{return Here(RR_THE_LOST_WOODS, []{return CanBlastOrSmash;});}}),
  });

  areaTable[RR_LW_BEYOND_MIDO] = Area("LW Beyond Mido", "Lost Woods", RA_THE_LOST_WOODS, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&ButterflyFairy, {[]{return ButterflyFairy || CanUse(RG_STICKS);}}),
                }, {
                  //Locations
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,  {[]{return IsChild && CanStunDeku;}}),
                  LocationAccess(RC_LW_GS_ABOVE_THEATER,                   {[]{return IsAdult && AtNight && ((CanPlantBean(RR_LW_BEYOND_MIDO) && CanAdultAttack) || (randoCtx->GetTrickOption(RT_LW_GS_BEAN) && CanUse(RG_HOOKSHOT) && (CanUse(RG_LONGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || HasBombchus || CanUse(RG_DINS_FIRE)))) && CanGetNightTimeGS;}}),
                  LocationAccess(RC_LW_GS_BEAN_PATCH_NEAR_THEATER,         {[]{return CanPlantBugs && (CanChildAttack || (randoCtx->GetOption(RSK_SHUFFLE_SCRUBS).Is(RO_SCRUBS_OFF) && DekuShield));}}),
                }, {
                  //Exits
                  Entrance(RR_LW_FOREST_EXIT,   {[]{return true;}}),
                  Entrance(RR_THE_LOST_WOODS,   {[]{return IsChild || CanUse(RG_SARIAS_SONG);}}),
                  Entrance(RR_SFM_ENTRYWAY,     {[]{return true;}}),
                  Entrance(RR_DEKU_THEATER,     {[]{return true;}}),
                  Entrance(RR_LW_SCRUBS_GROTTO, {[]{return Here(RR_LW_BEYOND_MIDO, []{return CanBlastOrSmash;});}}),
  });

  areaTable[RR_LW_NEAR_SHORTCUTS_GROTTO] = Area("LW Near Shortcuts Grotto", "LW Near Shortcuts Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, grottoEvents, {
                  //Locations
                  LocationAccess(RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,        {[]{return true;}}),
                  LocationAccess(RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_LOST_WOODS, {[]{return true;}}),
  });

  areaTable[RR_DEKU_THEATER] = Area("Deku Theater", "Deku Theater", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_DEKU_THEATER_SKULL_MASK,    {[]{return IsChild && SkullMask;}}),
                  LocationAccess(RC_DEKU_THEATER_MASK_OF_TRUTH, {[]{return IsChild && MaskOfTruth;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_LW_SCRUBS_GROTTO] = Area("LW Scrubs Grotto", "LW Scrubs Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_LW_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_LW_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
                }, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO, {[]{return true;}}),
  });

  areaTable[RR_SFM_ENTRYWAY] = Area("SFM Entryway", "Sacred Forest Meadow", RA_SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_LW_BEYOND_MIDO,       {[]{return true;}}),
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return CanJumpslash || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_DINS_FIRE) || CanUse(RG_MEGATON_HAMMER);}}),
                  Entrance(RR_SFM_WOLFOS_GROTTO,    {[]{return CanOpenBombGrotto;}}),
  });

  areaTable[RR_SACRED_FOREST_MEADOW] = Area("Sacred Forest Meadow", "Sacred Forest Meadow", RA_SACRED_FOREST_MEADOW, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SONG_FROM_SARIA,             {[]{return IsChild && ZeldasLetter;}}),
                  LocationAccess(RC_SHEIK_IN_FOREST,             {[]{return IsAdult;}}),
                  LocationAccess(RC_SFM_GS,                      {[]{return IsAdult && HookshotOrBoomerang && AtNight && CanGetNightTimeGS;}}),
                  LocationAccess(RC_SFM_MAZE_LOWER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_SFM_MAZE_UPPER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_SFM_SARIA_GOSSIP_STONE,      {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY,           {[]{return true;}}),
                  Entrance(RR_FOREST_TEMPLE_ENTRYWAY, {[]{return CanUse(RG_HOOKSHOT);}}),
                  Entrance(RR_SFM_FAIRY_GROTTO,       {[]{return true;}}),
                  Entrance(RR_SFM_STORMS_GROTTO,      {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[RR_SFM_FAIRY_GROTTO] = Area("SFM Fairy Grotto", "SFM Fairy Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&FreeFairies, {[]{return true;}}),
                }, {}, {
                  //Exits
                  Entrance(RR_SACRED_FOREST_MEADOW, {[]{return true;}}),
  });

  areaTable[RR_SFM_WOLFOS_GROTTO] = Area("SFM Wolfos Grotto", "SFM Wolfos Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SFM_WOLFOS_GROTTO_CHEST, {[]{return IsAdult || Slingshot || Sticks || KokiriSword || CanUse(RG_DINS_FIRE) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);}}),
                }, {
                  //Exits
                  Entrance(RR_SFM_ENTRYWAY, {[]{return true;}}),
  });

  areaTable[RR_SFM_STORMS_GROTTO] = Area("SFM Storms Grotto", "SFM Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_SFM_DEKU_SCRUB_GROTTO_REAR,  {[]{return CanStunDeku;}}),
                  LocationAccess(RC_SFM_DEKU_SCRUB_GROTTO_FRONT, {[]{return CanStunDeku;}}),
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
                  Entrance(RR_THE_LOST_WOODS, {[]{return CanUse(RG_LONGSHOT);}}),
  });
}
