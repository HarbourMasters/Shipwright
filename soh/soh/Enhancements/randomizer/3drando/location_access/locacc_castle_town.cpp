#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void AreaTable_Init_CastleTown() {
  areaTable[RR_MARKET_ENTRANCE] = Area("Market Entrance", "Market Entrance", RA_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,       {[]{return logic->IsAdult || logic->AtDay;}}),
                  Entrance(RR_THE_MARKET,         {[]{return true;}}),
                  Entrance(RR_MARKET_GUARD_HOUSE, {[]{return true;}}),
  });

  areaTable[RR_THE_MARKET] = Area("Market", "Market", RA_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_MARKET_ENTRANCE,            {[]{return true;}}),
                  Entrance(RR_TOT_ENTRANCE,               {[]{return true;}}),
                  Entrance(RR_CASTLE_GROUNDS,             {[]{return true;}}),
                  Entrance(RR_MARKET_BAZAAR,              {[]{return logic->IsChild && logic->AtDay;}}),
                  Entrance(RR_MARKET_MASK_SHOP,           {[]{return logic->IsChild && logic->AtDay;}}),
                  Entrance(RR_MARKET_SHOOTING_GALLERY,    {[]{return logic->IsChild && logic->AtDay;}}),
                  Entrance(RR_MARKET_BOMBCHU_BOWLING,     {[]{return logic->IsChild;}}),
                  Entrance(RR_MARKET_TREASURE_CHEST_GAME, {[]{return logic->IsChild && logic->AtNight;}}),
                  Entrance(RR_MARKET_POTION_SHOP,         {[]{return logic->IsChild && logic->AtDay;}}),
                  Entrance(RR_MARKET_BACK_ALLEY,          {[]{return logic->IsChild;}}),
  });

  areaTable[RR_MARKET_BACK_ALLEY] = Area("Market Back Alley", "Market", RA_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_THE_MARKET,                 {[]{return true;}}),
                  Entrance(RR_MARKET_BOMBCHU_SHOP,        {[]{return logic->AtNight;}}),
                  Entrance(RR_MARKET_DOG_LADY_HOUSE,      {[]{return true;}}),
                  Entrance(RR_MARKET_MAN_IN_GREEN_HOUSE,  {[]{return logic->AtNight;}}),
  });

  areaTable[RR_TOT_ENTRANCE] = Area("ToT Entrance", "ToT Entrance", RA_THE_MARKET, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_TOT_LEFT_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(RC_TOT_LEFT_CENTER_GOSSIP_STONE,  {[]{return true;}}),
                  LocationAccess(RC_TOT_RIGHT_CENTER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_TOT_RIGHT_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET,  {[]{return true;}}),
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_TEMPLE_OF_TIME] = Area("Temple of Time", "Temple of Time", RA_TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_TOT_LIGHT_ARROWS_CUTSCENE, {[]{return logic->IsAdult && logic->CanTriggerLACS;}}),
                  LocationAccess(RC_ALTAR_HINT_CHILD, {[]{return logic->IsChild;}}),
                  LocationAccess(RC_ALTAR_HINT_ADULT, {[]{return logic->IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_TOT_ENTRANCE,            {[]{return true;}}),
                  Entrance(RR_TOT_BEYOND_DOOR_OF_TIME, {[]{return randoCtx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN) || (logic->CanUse(RG_SONG_OF_TIME) && (randoCtx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY) || (logic->HasAllStones && logic->OcarinaOfTime)));}}),
  });

  areaTable[RR_TOT_BEYOND_DOOR_OF_TIME] = Area("Beyond Door of Time", "Beyond Door of Time", RA_TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&logic->TimeTravel, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_TOT_MASTER_SWORD, {[]{return logic->IsAdult;}}),
                  LocationAccess(RC_SHEIK_AT_TEMPLE, {[]{return logic->ForestMedallion && logic->IsAdult;}}),
                }, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_CASTLE_GROUNDS] = Area("Castle Grounds", "Castle Grounds", RA_CASTLE_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_THE_MARKET,            {[]{return true;}}),
                  Entrance(RR_HYRULE_CASTLE_GROUNDS, {[]{return logic->IsChild;}}),
                  Entrance(RR_GANONS_CASTLE_GROUNDS, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_HYRULE_CASTLE_GROUNDS] = Area("Hyrule Castle Grounds", "Castle Grounds", RA_HYRULE_CASTLE, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || logic->CanSummonGossipFairy;}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugRock,          {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_HC_MALON_EGG,              {[]{return true;}}),
                  LocationAccess(RC_HC_GS_TREE,                {[]{return logic->CanChildAttack;}}),
                  LocationAccess(RC_HC_MALON_GOSSIP_STONE,     {[]{return true;}}),
                  LocationAccess(RC_HC_ROCK_WALL_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS,          {[]{return true;}}),
                  Entrance(RR_HC_GARDEN,               {[]{return logic->WeirdEgg || !randoCtx->GetOption(RSK_SHUFFLE_WEIRD_EGG);}}),
                  Entrance(RR_HC_GREAT_FAIRY_FOUNTAIN, {[]{return logic->CanBlastOrSmash;}}),
                  Entrance(RR_HC_STORMS_GROTTO,        {[]{return logic->CanOpenStormGrotto;}}),
  });

  areaTable[RR_HC_GARDEN] = Area("HC Garden", "Castle Grounds", RA_HYRULE_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                }, {
                  //Locations
                  LocationAccess(RC_HC_ZELDAS_LETTER, {[]{return true;}}),
                  LocationAccess(RC_SONG_FROM_IMPA,   {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_HC_GREAT_FAIRY_FOUNTAIN] = Area("HC Great Fairy Fountain", "HC Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_HC_GREAT_FAIRY_REWARD, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_HC_STORMS_GROTTO] = Area("HC Storms Grotto", "HC Storms Grotto", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot,           {[]{return logic->NutPot           || logic->CanBlastOrSmash;}}),
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->GossipStoneFairy || (logic->CanBlastOrSmash && logic->CanSummonGossipFairy);}}),
                  EventAccess(&logic->WanderingBugs,    {[]{return logic->WanderingBugs    || logic->CanBlastOrSmash;}}),
                }, {
                  //Locations
                  LocationAccess(RC_HC_GS_STORMS_GROTTO,           {[]{return (logic->CanBlastOrSmash && logic->HookshotOrBoomerang) || (logic->Boomerang && randoCtx->GetTrickOption(RT_HC_STORMS_GS));}}),
                  LocationAccess(RC_HC_STORMS_GROTTO_GOSSIP_STONE, {[]{return logic->CanBlastOrSmash;}}),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_GROUNDS] = Area("Ganon's Castle Grounds", "Castle Grounds", RA_OUTSIDE_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  EventAccess(&logic->BuiltRainbowBridge, {[]{return logic->CanBuildRainbowBridge;}}),
                }, {
                  //Locations                                   //the terrain was lowered such that you can't get this GS with a simple sword slash
                  LocationAccess(RC_OGC_GS, {[]{return logic->CanJumpslash || logic->CanUseProjectile
                  || (logic->CanShield && logic->CanUse(RG_MEGATON_HAMMER)) || logic->CanUse(RG_DINS_FIRE);}}),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS,           {[]{return logic->AtNight;}}),
                  Entrance(RR_OGC_GREAT_FAIRY_FOUNTAIN, {[]{return logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->AtNight;}}),
                  Entrance(RR_GANONS_CASTLE_LEDGE,      {[]{return logic->BuiltRainbowBridge;}}),
  });

  areaTable[RR_OGC_GREAT_FAIRY_FOUNTAIN] = Area("OGC Great Fairy Fountain", "OGC Great Fairy Fountain", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_OGC_GREAT_FAIRY_REWARD, {[]{return logic->CanUse(RG_ZELDAS_LULLABY);}}),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE] = Area("Castle Grounds From Ganon's Castle", "Castle Grounds From Ganon's Castle", RA_CASTLE_GROUNDS, NO_DAY_NIGHT_CYCLE, {}, {}, {
    // Exits
    Entrance(RR_HYRULE_CASTLE_GROUNDS, { [] { return logic->IsChild; }}),
    Entrance(RR_GANONS_CASTLE_LEDGE, { [] { return logic->IsAdult; }}),
  });

  areaTable[RR_GANONS_CASTLE_LEDGE] = Area("Ganon's Castle Ledge", "OGC Ganon's Castle Ledge", RA_OUTSIDE_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE,
  {}, {}, {
    // Exits
    Entrance(RR_GANONS_CASTLE_GROUNDS, {[]{return logic->BuiltRainbowBridge;}}),
    Entrance(RR_GANONS_CASTLE_ENTRYWAY, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_MARKET_GUARD_HOUSE] = Area("Market Guard House", "Market Guard House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  //LocationAccess(RC_MARKET_GUARD_HOUSE_CHILD_POT_1, {[]{return CanBreakPots();}}),
                  //LocationAccess(RC_MARKET_GUARD_HOUSE_CHILD_POT_2, {[]{return CanBreakPots();}}),
                  LocationAccess(RC_MARKET_10_BIG_POES,    {[]{return logic->IsAdult && logic->BigPoeKill;}}),
                  LocationAccess(RC_MARKET_GS_GUARD_HOUSE, {[]{return logic->IsChild;}}),
                }, {
                  //Exits
                  Entrance(RR_MARKET_ENTRANCE, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BAZAAR] = Area("Market Bazaar", "Market Bazaar", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BAZAAR_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_MASK_SHOP] = Area("Market Mask Shop", "Market Mask Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->SkullMask,   {[]{return logic->SkullMask   || (logic->ZeldasLetter && (randoCtx->GetOption(RSK_COMPLETE_MASK_QUEST) ||  ChildCanAccess(RR_KAKARIKO_VILLAGE)));}}),
                  EventAccess(&logic->MaskOfTruth, {[]{return logic->MaskOfTruth || (logic->SkullMask && (randoCtx->GetOption(RSK_COMPLETE_MASK_QUEST) || (ChildCanAccess(RR_THE_LOST_WOODS) && logic->CanUse(RG_SARIAS_SONG) && AreaTable(RR_THE_GRAVEYARD)->childDay && ChildCanAccess(RR_HYRULE_FIELD) && logic->HasAllStones)));}}),
                }, {}, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_SHOOTING_GALLERY] = Area("Market Shooting Gallery", "Market Shooting Gallery", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_SHOOTING_GALLERY_REWARD, {[]{return logic->IsChild && logic->ChildsWallet;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BOMBCHU_BOWLING] = Area("Market Bombchu Bowling", "Market Bombchu Bowling", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,  {[]{return logic->CanPlayBowling;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, {[]{return logic->CanPlayBowling;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS,     {[]{return logic->CanPlayBowling;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_POTION_SHOP] = Area("Market Potion Shop", "Market Potion Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_MARKET_POTION_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_TREASURE_CHEST_GAME] = Area("Market Treasure Chest Game", "Market Treasure Chest Game", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_GREG_HINT, {[]{return true;}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_REWARD, {[]{return logic->ChildsWallet && ((logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 6)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, {[]{return logic->ChildsWallet && ((randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, {[]{return logic->ChildsWallet && ((randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 2)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, {[]{return logic->ChildsWallet && ((randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 3)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, {[]{return logic->ChildsWallet && ((randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 4)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, {[]{return logic->ChildsWallet && ((randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 5)) || (randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !randoCtx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)));}}),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BOMBCHU_SHOP] = Area("Market Bombchu Shop", "Market Bombchu Shop", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_1, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_2, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_3, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_4, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_5, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_6, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_7, {[]{return true;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_SHOP_ITEM_8, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[RR_MARKET_DOG_LADY_HOUSE] = Area("Market Dog Lady House", "Market Dog Lady House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_LOST_DOG, {[]{return logic->IsChild && logic->AtNight;}}),
                }, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[RR_MARKET_MAN_IN_GREEN_HOUSE] = Area("Market Man in Green House", "Market Man in Green House", RA_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });
}
