#include "../location_access.hpp"
#include "../logic.hpp"
#include "../entrance.hpp"

using namespace Logic;
using namespace Settings;

void AreaTable_Init_CastleTown() {
  areaTable[MARKET_ENTRANCE] = Area("Market Entrance", "Market Entrance", RHT_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(HYRULE_FIELD,       {[]{return IsAdult || AtDay;}}),
                  Entrance(THE_MARKET,         {[]{return true;}}),
                  Entrance(MARKET_GUARD_HOUSE, {[]{return true;}}),
  });

  areaTable[THE_MARKET] = Area("Market", "Market", RHT_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(MARKET_ENTRANCE,            {[]{return true;}}),
                  Entrance(TOT_ENTRANCE,               {[]{return true;}}),
                  Entrance(CASTLE_GROUNDS,             {[]{return true;}}),
                  Entrance(MARKET_BAZAAR,              {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_MASK_SHOP,           {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_SHOOTING_GALLERY,    {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_BOMBCHU_BOWLING,     {[]{return IsChild;}}),
                  Entrance(MARKET_TREASURE_CHEST_GAME, {[]{return IsChild && AtNight;}}),
                  Entrance(MARKET_POTION_SHOP,         {[]{return IsChild && AtDay;}}),
                  Entrance(MARKET_BACK_ALLEY,          {[]{return IsChild;}}),
  });

  areaTable[MARKET_BACK_ALLEY] = Area("Market Back Alley", "Market", RHT_THE_MARKET, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_MARKET,                 {[]{return true;}}),
                  Entrance(MARKET_BOMBCHU_SHOP,        {[]{return AtNight;}}),
                  Entrance(MARKET_DOG_LADY_HOUSE,      {[]{return true;}}),
                  Entrance(MARKET_MAN_IN_GREEN_HOUSE,  {[]{return AtNight;}}),
  });

  areaTable[TOT_ENTRANCE] = Area("ToT Entrance", "ToT Entrance", RHT_THE_MARKET, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairyWithoutSuns;}}),
                }, {
                  //Locations
                  LocationAccess(RC_TOT_LEFT_GOSSIP_STONE,         {[]{return true;}}),
                  LocationAccess(RC_TOT_LEFT_CENTER_GOSSIP_STONE,  {[]{return true;}}),
                  LocationAccess(RC_TOT_RIGHT_CENTER_GOSSIP_STONE, {[]{return true;}}),
                  LocationAccess(RC_TOT_RIGHT_GOSSIP_STONE,        {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET,  {[]{return true;}}),
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[TEMPLE_OF_TIME] = Area("Temple of Time", "Temple of Time", RHT_TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_TOT_LIGHT_ARROWS_CUTSCENE, {[]{return IsAdult && CanTriggerLACS;}}),
                }, {
                  //Exits
                  Entrance(TOT_ENTRANCE,            {[]{return true;}}),
                  Entrance(TOT_BEYOND_DOOR_OF_TIME, {[]{return OpenDoorOfTime.Is(OPENDOOROFTIME_OPEN) || (CanPlay(SongOfTime) && (OpenDoorOfTime.Is(OPENDOOROFTIME_SONGONLY) || (HasAllStones && OcarinaOfTime)));}}),
  });

  areaTable[TOT_BEYOND_DOOR_OF_TIME] = Area("Beyond Door of Time", "Beyond Door of Time", RHT_TEMPLE_OF_TIME, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  //EventAccess(&TimeTravel, {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_SHEIK_AT_TEMPLE, {[]{return ForestMedallion && IsAdult;}}),
                }, {
                  //Exits
                  Entrance(TEMPLE_OF_TIME, {[]{return true;}}),
  });

  areaTable[CASTLE_GROUNDS] = Area("Castle Grounds", "Castle Grounds", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(THE_MARKET,            {[]{return true;}}),
                  Entrance(HYRULE_CASTLE_GROUNDS, {[]{return IsChild;}}),
                  Entrance(GANONS_CASTLE_GROUNDS, {[]{return IsAdult;}}),
  });

  areaTable[HYRULE_CASTLE_GROUNDS] = Area("Hyrule Castle Grounds", "Castle Grounds", RHT_HYRULE_CASTLE, DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || CanSummonGossipFairy;}}),
                  EventAccess(&ButterflyFairy,   {[]{return ButterflyFairy   || CanUse(STICKS);}}),
                  EventAccess(&BugRock,          {[]{return true;}}),
                }, {
                  //Locations
                  LocationAccess(RC_HC_MALON_EGG,              {[]{return true;}}),
                  LocationAccess(RC_HC_GS_TREE,                {[]{return CanChildAttack;}}),
                  LocationAccess(RC_HC_MALON_GOSSIP_STONE,     {[]{return true;}}),
                  LocationAccess(RC_HC_ROCK_WALL_GOSSIP_STONE, {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS,          {[]{return true;}}),
                  Entrance(HC_GARDEN,               {[]{return WeirdEgg || !ShuffleWeirdEgg;}}),
                  Entrance(HC_GREAT_FAIRY_FOUNTAIN, {[]{return CanBlastOrSmash;}}),
                  Entrance(HC_STORMS_GROTTO,        {[]{return CanOpenStormGrotto;}}),
  });

  areaTable[HC_GARDEN] = Area("HC Garden", "Castle Grounds", RHT_HYRULE_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                }, {
                  //Locations
                  LocationAccess(RC_HC_ZELDAS_LETTER, {[]{return true;}}),
                  LocationAccess(RC_SONG_FROM_IMPA,   {[]{return true;}}),
                }, {
                  //Exits
                  Entrance(HYRULE_CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[HC_GREAT_FAIRY_FOUNTAIN] = Area("HC Great Fairy Fountain", "HC Great Fairy Fountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_HC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[HC_STORMS_GROTTO] = Area("HC Storms Grotto", "HC Storms Grotto", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&NutPot,           {[]{return NutPot           || CanBlastOrSmash;}}),
                  EventAccess(&GossipStoneFairy, {[]{return GossipStoneFairy || (CanBlastOrSmash && CanSummonGossipFairy);}}),
                  EventAccess(&WanderingBugs,    {[]{return WanderingBugs    || CanBlastOrSmash;}}),
                }, {
                  //Locations
                  LocationAccess(RC_HC_GS_STORMS_GROTTO,           {[]{return (CanBlastOrSmash && HookshotOrBoomerang) || (Boomerang && LogicCastleStormsGS);}}),
                  LocationAccess(RC_HC_STORMS_GROTTO_GOSSIP_STONE, {[]{return CanBlastOrSmash;}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[GANONS_CASTLE_GROUNDS] = Area("Ganon's Castle Grounds", "Castle Grounds", RHT_OUTSIDE_GANONS_CASTLE, NO_DAY_NIGHT_CYCLE, {
                  EventAccess(&BuiltRainbowBridge, {[]{return CanBuildRainbowBridge;}}),
                }, {
                  //Locations                                   //the terrain was lowered such that you can't get this GS with a simple sword slash
                  LocationAccess(RC_OGC_GS, {[]{return CanJumpslash || CanUseProjectile
                  || (CanShield && CanUse(MEGATON_HAMMER)) || CanUse(DINS_FIRE);}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS,           {[]{return AtNight;}}),
                  Entrance(OGC_GREAT_FAIRY_FOUNTAIN, {[]{return CanUse(GOLDEN_GAUNTLETS) && AtNight;}}),
                  Entrance(GANONS_CASTLE_LEDGE,      {[]{return BuiltRainbowBridge;}}),
  });

  areaTable[OGC_GREAT_FAIRY_FOUNTAIN] = Area("OGC Great Fairy Fountain", "OGC Great Fairy Fountain", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_OGC_GREAT_FAIRY_REWARD, {[]{return CanPlay(ZeldasLullaby);}}),
                }, {
                  //Exits
                  Entrance(CASTLE_GROUNDS, {[]{return true;}}),
  });

  areaTable[CASTLE_GROUNDS_FROM_GANONS_CASTLE] = Area("Castle Grounds From Ganon's Castle", "Castle Grounds From Ganon's Castle", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
    // Exits
    Entrance(HYRULE_CASTLE_GROUNDS, { [] { return IsChild; }}),
    Entrance(GANONS_CASTLE_LEDGE, { [] { return IsAdult; }}),
  });

  areaTable[GANONS_CASTLE_LEDGE] = Area("Ganon's Castle Ledge", "OGC Ganon's Castle Ledge", RHT_NONE, NO_DAY_NIGHT_CYCLE,
  {}, {}, {
    // Exits
    Entrance(GANONS_CASTLE_GROUNDS, {[]{return BuiltRainbowBridge;}}),
    Entrance(GANONS_CASTLE_ENTRYWAY, {[]{return IsAdult;}}),
  });

  areaTable[MARKET_GUARD_HOUSE] = Area("Market Guard House", "Market Guard House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_10_BIG_POES,    {[]{return IsAdult && BigPoeKill;}}),
                  LocationAccess(RC_MARKET_GS_GUARD_HOUSE, {[]{return IsChild;}}),
                }, {
                  //Exits
                  Entrance(MARKET_ENTRANCE, {[]{return true;}}),
  });

  areaTable[MARKET_BAZAAR] = Area("Market Bazaar", "Market Bazaar", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_MASK_SHOP] = Area("Market Mask Shop", "Market Mask Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {
                  //Events
                  EventAccess(&SkullMask,   {[]{return SkullMask   || (ZeldasLetter && (CompleteMaskQuest ||  ChildCanAccess(KAKARIKO_VILLAGE)));}}),
                  EventAccess(&MaskOfTruth, {[]{return MaskOfTruth || (SkullMask && (CompleteMaskQuest || (ChildCanAccess(THE_LOST_WOODS) && CanPlay(SariasSong) && AreaTable(THE_GRAVEYARD)->childDay && ChildCanAccess(HYRULE_FIELD) && HasAllStones)));}}),
                }, {}, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_SHOOTING_GALLERY] = Area("Market Shooting Gallery", "Market Shooting Gallery", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_SHOOTING_GALLERY_REWARD, {[]{return IsChild;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_BOMBCHU_BOWLING] = Area("Market Bombchu Bowling", "Market Bombchu Bowling", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,  {[]{return CanPlayBowling;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, {[]{return CanPlayBowling;}}),
                  LocationAccess(RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS,     {[]{return CanPlayBowling;}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_POTION_SHOP] = Area("Market Potion Shop", "Market Potion Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_TREASURE_CHEST_GAME] = Area("Market Treasure Chest Game", "Market Treasure Chest Game", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_REWARD, {[]{return (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 6)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1));}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 2)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 3)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 4)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                  LocationAccess(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, {[]{return (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 5)) || (ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK) && SmallKeys(MARKET_TREASURE_CHEST_GAME, 1)) || (CanUse(LENS_OF_TRUTH) && !ShuffleChestMinigame);}}),
                }, {
                  //Exits
                  Entrance(THE_MARKET, {[]{return true;}}),
  });

  areaTable[MARKET_BOMBCHU_SHOP] = Area("Market Bombchu Shop", "Market Bombchu Shop", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
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
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[MARKET_DOG_LADY_HOUSE] = Area("Market Dog Lady House", "Market Dog Lady House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {
                  //Locations
                  LocationAccess(RC_MARKET_LOST_DOG, {[]{return IsChild && AtNight;}}),
                }, {
                  //Exits
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });

  areaTable[MARKET_MAN_IN_GREEN_HOUSE] = Area("Market Man in Green House", "Market Man in Green House", RHT_NONE, NO_DAY_NIGHT_CYCLE, {}, {}, {
                  //Exits
                  Entrance(MARKET_BACK_ALLEY, {[]{return true;}}),
  });
}
