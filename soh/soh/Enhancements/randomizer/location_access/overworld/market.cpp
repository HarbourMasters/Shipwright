#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/entrance.h"

using namespace Rando;

void RegionTable_Init_Market() {
    areaTable[RR_MARKET_ENTRANCE] = Region("Market Entrance", "Market Entrance", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_HYRULE_FIELD,       []{return logic->IsAdult || logic->AtDay;}),
        Entrance(RR_THE_MARKET,         []{return true;}),
        Entrance(RR_MARKET_GUARD_HOUSE, []{return true;}),
    });

    areaTable[RR_THE_MARKET] = Region("Market", "Market", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_MARKET_ENTRANCE,            []{return true;}),
        Entrance(RR_TOT_ENTRANCE,               []{return true;}),
        Entrance(RR_CASTLE_GROUNDS,             []{return true;}),
        Entrance(RR_MARKET_BAZAAR,              []{return logic->IsChild && logic->AtDay;}),
        Entrance(RR_MARKET_MASK_SHOP,           []{return logic->IsChild && logic->AtDay;}),
        Entrance(RR_MARKET_SHOOTING_GALLERY,    []{return logic->IsChild && logic->AtDay;}),
        Entrance(RR_MARKET_BOMBCHU_BOWLING,     []{return logic->IsChild;}),
        Entrance(RR_MARKET_TREASURE_CHEST_GAME, []{return logic->IsChild && logic->AtNight;}),
        Entrance(RR_MARKET_POTION_SHOP,         []{return logic->IsChild && logic->AtDay;}),
        Entrance(RR_MARKET_BACK_ALLEY,          []{return logic->IsChild;}),
    });

    areaTable[RR_MARKET_BACK_ALLEY] = Region("Market Back Alley", "Market", {RA_THE_MARKET}, NO_DAY_NIGHT_CYCLE, {}, {}, {
        //Exits
        Entrance(RR_THE_MARKET,                []{return true;}),
        Entrance(RR_MARKET_BOMBCHU_SHOP,       []{return logic->AtNight;}),
        Entrance(RR_MARKET_DOG_LADY_HOUSE,     []{return true;}),
        Entrance(RR_MARKET_MAN_IN_GREEN_HOUSE, []{return logic->AtNight;}),
    });

    areaTable[RR_MARKET_GUARD_HOUSE] = Region("Market Guard House", "Market Guard House", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CouldEmptyBigPoes, []{return logic->IsAdult;}),
        EventAccess(&logic->CanEmptyBigPoes,   []{return logic->IsAdult;}),
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
        Entrance(RR_MARKET_ENTRANCE, []{return true;}),
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
        Entrance(RR_THE_MARKET, []{return true;}),
    });

    areaTable[RR_MARKET_MASK_SHOP] = Region("Market Mask Shop", "Market Mask Shop", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->SkullMask,   []{return logic->SkullMask   || (logic->HasItem(RG_ZELDAS_LETTER) && (ctx->GetOption(RSK_COMPLETE_MASK_QUEST) ||  ChildCanAccess(RR_KAKARIKO_VILLAGE)));}), //RANDOTODO Complete mask quest does not need this location, so should be tied to link's pocket
        EventAccess(&logic->MaskOfTruth, []{return logic->MaskOfTruth || (logic->SkullMask && (ctx->GetOption(RSK_COMPLETE_MASK_QUEST) || (ChildCanAccess(RR_THE_LOST_WOODS) && logic->CanUse(RG_SARIAS_SONG) && RegionTable(RR_THE_GRAVEYARD)->childDay && ChildCanAccess(RR_HYRULE_FIELD) && logic->StoneCount() == 3)));}),
    }, {
        //Locations
        LOCATION(RC_MASK_SHOP_HINT, true),
    }, {
        //Exits
        Entrance(RR_THE_MARKET, []{return true;}),
    });

    areaTable[RR_MARKET_SHOOTING_GALLERY] = Region("Market Shooting Gallery", "Market Shooting Gallery", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_MARKET_SHOOTING_GALLERY_REWARD, logic->IsChild && logic->HasItem(RG_CHILD_WALLET)),
    }, {
        //Exits
        Entrance(RR_THE_MARKET, []{return true;}),
    });

    areaTable[RR_MARKET_BOMBCHU_BOWLING] = Region("Market Bombchu Bowling", "Market Bombchu Bowling", {}, NO_DAY_NIGHT_CYCLE, {
        //Events
        EventAccess(&logic->CouldPlayBowling, []{return (logic->HasItem(RG_CHILD_WALLET));}),
    }, {
        //Locations
        LOCATION(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,  logic->CouldPlayBowling && logic->BombchusEnabled()),
        LOCATION(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, logic->CouldPlayBowling && logic->BombchusEnabled()),
    }, {
        //Exits
        Entrance(RR_THE_MARKET, []{return true;}),
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
        Entrance(RR_THE_MARKET, []{return true;}),
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
        Entrance(RR_THE_MARKET, []{return true;}),
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
        Entrance(RR_MARKET_BACK_ALLEY, []{return true;}),
    });

    areaTable[RR_MARKET_DOG_LADY_HOUSE] = Region("Market Dog Lady House", "Market Dog Lady House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        //Locations
        LOCATION(RC_MARKET_LOST_DOG, logic->IsChild && logic->AtNight),
    }, {
        //Exits
        Entrance(RR_MARKET_BACK_ALLEY, []{return true;}),
    });

    areaTable[RR_MARKET_MAN_IN_GREEN_HOUSE] = Region("Market Man in Green House", "Market Man in Green House", {}, NO_DAY_NIGHT_CYCLE, {}, {
        // Locations
        LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_1, logic->CanBreakPots()),
        LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_2, logic->CanBreakPots()),
        LOCATION(RC_MK_BACK_ALLEY_HOUSE_POT_3, logic->CanBreakPots()),
    }, {
        //Exits
        Entrance(RR_MARKET_BACK_ALLEY, []{return true;}),
    });
}