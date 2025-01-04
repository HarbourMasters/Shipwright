#include "../location_access.hpp"
#include "../../entrance.h"

using namespace Rando;

void RegionTable_Init_CastleTown() {
  areaTable[RR_MARKET_ENTRANCE] = Region("Market Entrance", "Market Entrance", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_HYRULE_FIELD,       {[]{return logic->IsAdult || logic->AtDay;}}),
                  Entrance(RR_THE_MARKET,         {[]{return true;}}),
                  Entrance(RR_MARKET_GUARD_HOUSE, {[]{return true;}}),
  });

  areaTable[RR_THE_MARKET] = Region("Market", "Market", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
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

  areaTable[RR_MARKET_BACK_ALLEY] = Region("Market Back Alley", "Market", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_THE_MARKET,                 {[]{return true;}}),
                  Entrance(RR_MARKET_BOMBCHU_SHOP,        {[]{return logic->AtNight;}}),
                  Entrance(RR_MARKET_DOG_LADY_HOUSE,      {[]{return true;}}),
                  Entrance(RR_MARKET_MAN_IN_GREEN_HOUSE,  {[]{return logic->AtNight;}}),
  });

  areaTable[RR_TOT_ENTRANCE] = Region("ToT Entrance", "ToT Entrance", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairyExceptSuns();}}),
                }, {
                  //Locations
                  LOCATION(RC_TOT_LEFTMOST_GOSSIP_STONE,     true),
                  LOCATION(RC_TOT_LEFT_CENTER_GOSSIP_STONE,  true),
                  LOCATION(RC_TOT_RIGHT_CENTER_GOSSIP_STONE, true),
                  LOCATION(RC_TOT_RIGHTMOST_GOSSIP_STONE,    true),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET,     {[]{return true;}}),
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[RR_TEMPLE_OF_TIME] = Region("Temple of Time", "Temple of Time", {RA_TEMPLE_OF_TIME}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_TOT_LIGHT_ARROWS_CUTSCENE, logic->IsAdult && logic->CanTriggerLACS()),
                  LOCATION(RC_ALTAR_HINT_CHILD,          logic->IsChild),
                  LOCATION(RC_ALTAR_HINT_ADULT,          logic->IsAdult),
                  LOCATION(RC_TOT_SHEIK_HINT,            logic->IsAdult),
                }, {
                  //Exits
                  Entrance(RR_TOT_ENTRANCE,            {[]{return true;}}),
                  Entrance(RR_TOT_BEYOND_DOOR_OF_TIME, {[]{return ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_OPEN) || (logic->CanUse(RG_SONG_OF_TIME) && (ctx->GetOption(RSK_DOOR_OF_TIME).Is(RO_DOOROFTIME_SONGONLY) || (logic->StoneCount() == 3 && logic->HasItem(RG_OCARINA_OF_TIME))));}}),
  });

  areaTable[RR_TOT_BEYOND_DOOR_OF_TIME] = Region("Beyond Door of Time", "Beyond Door of Time", {RA_TEMPLE_OF_TIME}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&logic->TimeTravel, {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_TOT_MASTER_SWORD, logic->IsAdult),
                  LOCATION(RC_GIFT_FROM_RAURU,  logic->IsAdult),
                  LOCATION(RC_SHEIK_AT_TEMPLE,  logic->HasItem(RG_FOREST_MEDALLION) && logic->IsAdult),
                }, {
                  //Exits
                  Entrance(RR_TEMPLE_OF_TIME, {[]{return true;}}),
  });

//With multi-area support {RA_CASTLE_GROUNDS} is not strictly required anymore, as any interior here could inherit both 
//{RA_HYRULE_CASTLE} and {RA_OUTSIDE_GANONS_CASTLE}, but an setting to merge the latter 2 into the former may be preffered
  areaTable[RR_CASTLE_GROUNDS] = Region("Castle Grounds", "Castle Grounds", {RA_CASTLE_GROUNDS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(RR_THE_MARKET,            {[]{return true;}}),
                  Entrance(RR_HYRULE_CASTLE_GROUNDS, {[]{return logic->IsChild;}}),
                  Entrance(RR_GANONS_CASTLE_GROUNDS, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_HYRULE_CASTLE_GROUNDS] = Region("Hyrule Castle Grounds", "Castle Grounds", {RA_HYRULE_CASTLE}, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CallGossipFairy();}}),
                  EventAccess(&logic->ButterflyFairy,   {[]{return logic->ButterflyFairy   || logic->CanUse(RG_STICKS);}}),
                  EventAccess(&logic->BugRock,          {[]{return true;}}),
                }, {
                  //Locations
                  LOCATION(RC_HC_MALON_EGG,              true),
                  LOCATION(RC_HC_GS_TREE,                logic->IsChild && logic->CanAttack()),
                  LOCATION(RC_HC_MALON_GOSSIP_STONE,     true),
                  LOCATION(RC_HC_ROCK_WALL_GOSSIP_STONE, true),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS,          {[]{return true;}}),
                  Entrance(RR_HC_GARDEN,               {[]{return logic->CanUse(RG_WEIRD_EGG) || !ctx->GetOption(RSK_SHUFFLE_WEIRD_EGG) || (ctx->GetTrickOption(RT_DAMAGE_BOOST_SIMPLE) && logic->HasExplosives() && logic->CanJumpslash());}}),
                  Entrance(RR_HC_GREAT_FAIRY_FOUNTAIN, {[]{return logic->BlastOrSmash();}}),
                  Entrance(RR_HC_STORMS_GROTTO,        {[]{return logic->CanOpenStormsGrotto();}}),
  });

  areaTable[RR_HC_GARDEN] = Region("HC Garden", "Castle Grounds", {RA_HYRULE_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                }, {
                  //Locations
                  LOCATION(RC_HC_ZELDAS_LETTER, true),
                  LOCATION(RC_SONG_FROM_IMPA,   true),
                }, {
                  //Exits
                  Entrance(RR_HYRULE_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_HC_GREAT_FAIRY_FOUNTAIN] = Region("HC Great Fairy Fountain", "HC Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_HC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_HC_STORMS_GROTTO] = Region("HC Storms Grotto", "HC Storms Grotto", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->NutPot,           {[]{return logic->NutPot           || logic->BlastOrSmash();}}),
                  EventAccess(&logic->GossipStoneFairy, {[]{return logic->CanBreakMudWalls() && logic->CallGossipFairy();}}),
                  EventAccess(&logic->WanderingBugs,    {[]{return logic->WanderingBugs    || logic->BlastOrSmash();}}),
                }, {
                  //Locations
                  LOCATION(RC_HC_GS_STORMS_GROTTO,           (logic->BlastOrSmash() && logic->HookshotOrBoomerang()) || (logic->CanUse(RG_BOOMERANG) && ctx->GetTrickOption(RT_HC_STORMS_GS))),
                  LOCATION(RC_HC_STORMS_GROTTO_GOSSIP_STONE, logic->BlastOrSmash()),
                  LOCATION(RC_HC_STORMS_GROTTO_POT_1,        logic->BlastOrSmash() && logic->CanBreakPots()),
                  LOCATION(RC_HC_STORMS_GROTTO_POT_2,        logic->BlastOrSmash() && logic->CanBreakPots()),
                  LOCATION(RC_HC_STORMS_GROTTO_POT_3,        logic->BlastOrSmash() && logic->CanBreakPots()),
                  LOCATION(RC_HC_STORMS_GROTTO_POT_4,        logic->BlastOrSmash() && logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_GANONS_CASTLE_GROUNDS] = Region("Ganon's Castle Grounds", "Castle Grounds", {RA_OUTSIDE_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE, {
                  EventAccess(&logic->BuiltRainbowBridge, {[]{return logic->CanBuildRainbowBridge();}}),
                }, {
                  //Locations                                   
                  LOCATION(RC_OGC_GS, logic->CanJumpslashExceptHammer() || logic->CanUseProjectile() || (logic->CanShield() && logic->CanUse(RG_MEGATON_HAMMER)) || logic->CanUse(RG_DINS_FIRE)),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS,           {[]{return logic->AtNight;}}),
                  Entrance(RR_OGC_GREAT_FAIRY_FOUNTAIN, {[]{return logic->CanUse(RG_GOLDEN_GAUNTLETS) && logic->AtNight;}}),
                  Entrance(RR_GANONS_CASTLE_LEDGE,      {[]{return logic->BuiltRainbowBridge;}}),
  });

  areaTable[RR_OGC_GREAT_FAIRY_FOUNTAIN] = Region("OGC Great Fairy Fountain", "OGC Great Fairy Fountain", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_OGC_GREAT_FAIRY_REWARD, logic->CanUse(RG_ZELDAS_LULLABY)),
                }, {
                  //Exits
                  Entrance(RR_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE] = Region("Castle Grounds From Ganon's Castle", "Castle Grounds From Ganon's Castle", {RA_CASTLE_GROUNDS}, NO_DAY_NIGHT_CYCLE, {}, {}, {
    // Exits
    Entrance(RR_HYRULE_CASTLE_GROUNDS, { [] { return logic->IsChild; }}),
    Entrance(RR_GANONS_CASTLE_LEDGE, { [] { return logic->IsAdult; }}),
  });

  areaTable[RR_GANONS_CASTLE_LEDGE] = Region("Ganon's Castle Ledge", "OGC Ganon's Castle Ledge", {RA_OUTSIDE_GANONS_CASTLE}, NO_DAY_NIGHT_CYCLE,
  {}, {}, {
    // Exits
    Entrance(RR_GANONS_CASTLE_GROUNDS, {[]{return logic->BuiltRainbowBridge;}}),
    Entrance(RR_GANONS_CASTLE_ENTRYWAY, {[]{return logic->IsAdult;}}),
  });

  areaTable[RR_MARKET_GUARD_HOUSE] = Region("Market Guard House", "Market Guard House", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CanEmptyBigPoes,   {[]{return logic->IsAdult;}}),
                }, {
                  //Locations
                  LOCATION(RC_MARKET_10_BIG_POES,          logic->IsAdult && logic->BigPoeKill),
                  LOCATION(RC_MARKET_GS_GUARD_HOUSE,       logic->IsChild),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_1,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_2,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_3,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_4,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_5,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_6,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_7,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_8,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_9,  logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_10, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_11, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_12, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_13, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_14, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_15, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_16, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_17, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_18, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_19, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_20, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_21, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_22, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_23, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_24, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_25, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_26, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_27, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_28, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_29, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_30, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_31, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_32, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_33, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_34, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_35, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_36, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_37, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_38, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_39, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_40, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_41, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_42, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_43, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_CHILD_POT_44, logic->IsChild && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_1,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_2,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_3,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_4,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_5,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_6,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_7,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_8,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_9,  logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_10, logic->IsAdult && logic->CanBreakPots()),
                  LOCATION(RC_MK_GUARD_HOUSE_ADULT_POT_11, logic->IsAdult && logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_MARKET_ENTRANCE, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BAZAAR] = Region("Market Bazaar", "Market Bazaar", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_MARKET_BAZAAR_ITEM_1, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_2, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_3, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_4, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_5, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_6, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_7, true),
                  LOCATION(RC_MARKET_BAZAAR_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_MASK_SHOP] = Region("Market Mask Shop", "Market Mask Shop", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->SkullMask,   {[]{return logic->SkullMask   || (logic->HasItem(RG_ZELDAS_LETTER) && (ctx->GetOption(RSK_COMPLETE_MASK_QUEST) ||  ChildCanAccess(RR_KAKARIKO_VILLAGE)));}}), //RANDOTODO Complete mask quest does not need this location, so should be tied to link's pocket
                  EventAccess(&logic->MaskOfTruth, {[]{return logic->MaskOfTruth || (logic->SkullMask && (ctx->GetOption(RSK_COMPLETE_MASK_QUEST) || (ChildCanAccess(RR_THE_LOST_WOODS) && logic->CanUse(RG_SARIAS_SONG) && RegionTable(RR_THE_GRAVEYARD)->childDay && ChildCanAccess(RR_HYRULE_FIELD) && logic->StoneCount() == 3)));}}),
                }, {
                  LOCATION(RC_MASK_SHOP_HINT, true),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_SHOOTING_GALLERY] = Region("Market Shooting Gallery", "Market Shooting Gallery", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_MARKET_SHOOTING_GALLERY_REWARD, logic->IsChild && logic->HasItem(RG_CHILD_WALLET)),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BOMBCHU_BOWLING] = Region("Market Bombchu Bowling", "Market Bombchu Bowling", {}, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&logic->CouldPlayBowling, {[]{return (logic->HasItem(RG_CHILD_WALLET));}}),
                }, {
                  //Locations
                  LOCATION(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,  logic->CouldPlayBowling && logic->BombchusEnabled()),
                  LOCATION(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, logic->CouldPlayBowling && logic->BombchusEnabled()),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_POTION_SHOP] = Region("Market Potion Shop", "Market Potion Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_1, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_2, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_3, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_4, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_5, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_6, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_7, true),
                  LOCATION(RC_MARKET_POTION_SHOP_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_TREASURE_CHEST_GAME] = Region("Market Treasure Chest Game", "Market Treasure Chest Game", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_GREG_HINT,                         logic->HasItem(RG_CHILD_WALLET)),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_REWARD, logic->HasItem(RG_CHILD_WALLET) && ((logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 6)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_KEY_1,  logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_KEY_2,  logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 2)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 2)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_KEY_3,  logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 3)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 3)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_KEY_4,  logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 4)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 4)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_KEY_5,  logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 5)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                  LOCATION(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, logic->HasItem(RG_CHILD_WALLET) && ((ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_SINGLE_KEYS) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 5)) || (ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME).Is(RO_CHEST_GAME_PACK) && logic->SmallKeys(RR_MARKET_TREASURE_CHEST_GAME, 1)) || (logic->CanUse(RG_LENS_OF_TRUTH) && !ctx->GetOption(RSK_SHUFFLE_CHEST_MINIGAME)))),
                }, {
                  //Exits
                  Entrance(RR_THE_MARKET, {[]{return true;}}),
  });

  areaTable[RR_MARKET_BOMBCHU_SHOP] = Region("Market Bombchu Shop", "Market Bombchu Shop", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_1, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_2, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_3, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_4, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_5, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_6, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_7, true),
                  LOCATION(RC_MARKET_BOMBCHU_SHOP_ITEM_8, true),
                }, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[RR_MARKET_DOG_LADY_HOUSE] = Region("Market Dog Lady House", "Market Dog Lady House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LOCATION(RC_MARKET_LOST_DOG, logic->IsChild && logic->AtNight),
                }, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[RR_MARKET_MAN_IN_GREEN_HOUSE] = Region("Market Man in Green House", "Market Man in Green House", {}, NO_DAY_NIGHT_CYCLE, {}, {
                  // Locations
                  LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_1, logic->CanBreakPots()),
                  LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_2, logic->CanBreakPots()),
                  LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_3, logic->CanBreakPots()),
                }, {
                  //Exits
                  Entrance(RR_MARKET_BACK_ALLEY, {[]{return true;}}),
  });
}
